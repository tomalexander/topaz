/*
 * Copyright (c) 2012 Tom Alexander
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 */
#include "rigidbody.h"
#include "unit.h"
#include "collider.h"
#include "aabb_collider.h"

namespace topaz {

    list<rigidbody*> rigidbodies;

    rigidbody::rigidbody(sqt* _transform, unit* parent_unit, TENSOR tensor, const string & type)
    {
        transform = _transform;
        use_gravity = true;
        #if USE_OPENCL == 0
        add_pre_draw_function(id, std::bind(&topaz::rigidbody::update, this, std::placeholders::_1));
        add_begin_update_function(id, std::bind(&topaz::rigidbody::handle_collisions, this, std::placeholders::_1));
        #endif

        add_post_draw_function(id, [&](int milliseconds){handled_collisions.clear();});
        mass = 1;
        elasticity = 1.0f;
        rigidbodies.push_back(this);
        calculate_center_of_mass(parent_unit);
        calculate_farthest_dimension(parent_unit);
        if (type == "BOX")
        {
            calculate_axis_aligned_bounding_box(parent_unit);
        } else if (type == "SPHERE") {
            colliders.push_back(new sphere_collider(transform, center_of_mass, farthest_dimension * transform->get_world_s()));
        }
        last_position = transform->get_t();
        calculate_tensor(tensor);

        #if USE_OPENCL == 1
        rigidbody_id = global_accelerator->num_of_rigidbodies;
        global_accelerator->increase_rigidbody_size_to(global_accelerator->num_of_rigidbodies+1);
        global_accelerator->rigidbodies.push_back(this);
        push_to_cl();
        #endif
    }

    rigidbody::~rigidbody()
    {
        rigidbodies.remove(this);
        for (collider* cur : colliders)
            delete cur;
    }

    void rigidbody::push_to_cl()
    {
        #if USE_OPENCL == 0
        return;
        #else
        if (use_gravity)
            global_accelerator->get_use_gravity(rigidbody_id) = 1.0f;
        else
            global_accelerator->get_use_gravity(rigidbody_id) = 0.0f;
        global_accelerator->get_center_of_mass(rigidbody_id)[0] = center_of_mass.x;
        global_accelerator->get_center_of_mass(rigidbody_id)[1] = center_of_mass.y;
        global_accelerator->get_center_of_mass(rigidbody_id)[2] = center_of_mass.z;
        global_accelerator->get_velocity(rigidbody_id)[0] = velocity.x;
        global_accelerator->get_velocity(rigidbody_id)[1] = velocity.y;
        global_accelerator->get_velocity(rigidbody_id)[2] = velocity.z;
        global_accelerator->get_mass(rigidbody_id) = mass;
        global_accelerator->get_elasticity(rigidbody_id) = elasticity;
        memcpy(global_accelerator->get_I(rigidbody_id), &(I[0]), 16*sizeof(float));
        global_accelerator->get_angular_momentum(rigidbody_id)[0] = angular_momentum.x;
        global_accelerator->get_angular_momentum(rigidbody_id)[1] = angular_momentum.y;
        global_accelerator->get_angular_momentum(rigidbody_id)[2] = angular_momentum.z;
        global_accelerator->get_rigidbody_parent(rigidbody_id) = transform->sqt_id;
        for (collider* cur : colliders)
        {
        global_accelerator->collider_rigidbodies[cur->collider_id] = rigidbody_id;
    }
        #endif
    }

    void rigidbody::pull_from_cl()
    {
        #if USE_OPENCL == 0
        return;
        #else
        if (global_accelerator->get_use_gravity(rigidbody_id) > 0.5)
            use_gravity = true;
        else
            use_gravity = false;
        center_of_mass.x = global_accelerator->get_center_of_mass(rigidbody_id)[0];
        center_of_mass.y = global_accelerator->get_center_of_mass(rigidbody_id)[1];
        center_of_mass.z = global_accelerator->get_center_of_mass(rigidbody_id)[2];
        velocity.x = global_accelerator->get_velocity(rigidbody_id)[0];
        velocity.y = global_accelerator->get_velocity(rigidbody_id)[1];
        velocity.z = global_accelerator->get_velocity(rigidbody_id)[2];
        mass = global_accelerator->get_mass(rigidbody_id);
        elasticity = global_accelerator->get_elasticity(rigidbody_id);
        memcpy(&(I[0]), global_accelerator->get_I(rigidbody_id), 16*sizeof(float));
        angular_momentum.x = global_accelerator->get_angular_momentum(rigidbody_id)[0];
        angular_momentum.y = global_accelerator->get_angular_momentum(rigidbody_id)[1];
        angular_momentum.z = global_accelerator->get_angular_momentum(rigidbody_id)[2];
        #endif
    }

    void rigidbody::calculate_tensor(TENSOR tensor)
    {
        if (tensor == SPHERE_TENSOR)
        {
            float value = 2.0f/5.0f * mass * farthest_dimension  * farthest_dimension * transform->get_world_s() * transform->get_world_s();
            I[0][0] = value;
            I[1][1] = value;
            I[2][2] = value;
        }
    }

    vector<collision> rigidbody::check_for_collision()
    {
        vector<collision> ret;
        for (rigidbody* cur : rigidbodies)
        {
            if (cur == this)
                continue;

            pair<bool, collision> contact = is_colliding_with(cur);
            if (contact.first)
            {
                ret.push_back(contact.second);
            }
        }
        return ret;
    }

    pair<bool, collision> rigidbody::is_colliding_with(rigidbody* other)
    {
        for (collider* my_collider : colliders)
        {
            for (collider* their_collider : other->colliders)
            {
                glm::vec3* contact = my_collider->is_colliding_with(their_collider);
                if (contact != NULL)
                {
                    #if DRAW_COLLISION_SOLIDS == 1
                    my_collider->display_as_colliding(true);
                    their_collider->display_as_colliding(true);
                    #endif
                    return make_pair(true, collision(this, other, contact));
                }
            }
            #if DRAW_COLLISION_SOLIDS == 1
            my_collider->display_as_colliding(false);
            #endif
        }
        return make_pair(false, collision(NULL,NULL,NULL));
    }

    void rigidbody::handle_collisions(int milliseconds)
    {
        double seconds = ((double)milliseconds) / ((double)1000.0);
        vector<collision> collisions = check_for_collision();
        for (const collision & cur : collisions)
        {
            //Don't collide objects who have already handled their collision
            bool skip = false;
            for (rigidbody* possibly_this : cur.into->handled_collisions)
            {
                if (possibly_this == this)
                {
                    skip = true;
                    break;
                }
            }
            if (skip)
                continue;

            //Handle math
            glm::vec3 collision_force = handle_linear_collision(cur);
            handle_angular_collision(cur, collision_force);
            handled_collisions.push_back(cur.into);
        }

        for (const collision & cur : collisions)
            delete cur.contact;
    }

    glm::vec3 rigidbody::handle_linear_collision(const collision & other)
    {
        // transform->set_t(last_position);
        // other.into->transform->set_t(other.into->last_position);
        /*
         * Calculate vectors
         */
        glm::vec3 this_com = get_world_center_of_mass();
        glm::vec3 other_com = other.into->get_world_center_of_mass();
        glm::vec3 norm = glm::normalize((this_com - *(other.contact)));

        /*
         * Calculate p hat
         */
        float bottom = 1.0f/mass + 1.0f/other.into->mass;
        float top = (elasticity + 1.0f) * (glm::dot(other.into->velocity, norm) - glm::dot(velocity, norm));
        float italic_p_hat = top/bottom;

        /*
         * Calculate new velocities
         */
        velocity = velocity + norm*(italic_p_hat/mass);
        other.into->velocity = other.into->velocity - norm*(italic_p_hat/other.into->mass);
        return norm*italic_p_hat;
    }

    void rigidbody::handle_angular_collision(const collision & other, const glm::vec3 & collision_force)
    {
        glm::vec3 this_com = get_world_center_of_mass();
        glm::vec3 other_com = other.into->get_world_center_of_mass();
        glm::vec3 this_r = *(other.contact) - this_com;
        glm::vec3 other_r = *(other.contact) - other_com;

        torques.push_back(make_pair(0, glm::cross(this_r, collision_force)));
        other.into->torques.push_back(make_pair(0, glm::cross(other_r,-collision_force)));
    }

    void rigidbody::immovable_collision(const glm::vec3 & norm)
    {
        glm::vec3 new_velocity = velocity - norm*(glm::dot(velocity, norm) * (elasticity + 1.0f));
        velocity = new_velocity;
    }

    void rigidbody::populate_acceleration(float* accelerations, int milliseconds)
    {
        glm::vec3 acceleration;
        for (pair<int, glm::vec3> & force : forces)
        {
            add_force_to_acceleration(acceleration, force.second);
            force.first -= milliseconds;
        }
        for (const glm::vec3 & force : constant_forces)
        {
            add_force_to_acceleration(acceleration, force);
        }
        if (use_gravity)
            add_force_to_acceleration(acceleration, glm::vec3(0, -9.8, 0));
        forces.remove_if([](const pair<int, glm::vec3> & force){return force.first <= 0;});

        accelerations[rigidbody_id*3+0] = acceleration.x;
        accelerations[rigidbody_id*3+1] = acceleration.y;
        accelerations[rigidbody_id*3+2] = acceleration.z;
    }

    void rigidbody::update(int milliseconds)
    {
        last_position = transform->get_t();
        float seconds = ((double)milliseconds) / ((double)1000.0);
        
        glm::vec3 acceleration;
        for (pair<int, glm::vec3> & force : forces)
        {
            add_force_to_acceleration(acceleration, force.second);
            force.first -= milliseconds;
        }
        for (const glm::vec3 & force : constant_forces)
        {
            add_force_to_acceleration(acceleration, force);
        }
        if (use_gravity)
            add_force_to_acceleration(acceleration, glm::vec3(0, -9.8, 0));
        forces.remove_if([](const pair<int, glm::vec3> & force){return force.first <= 0;});

        for (pair<int, glm::vec3> & torque : torques)
        {
            add_torque_to_angular_momentum(torque.second * seconds);
            torque.first -= milliseconds;
        }
        for (const glm::vec3 & torque : constant_torques)
        {
            add_torque_to_angular_momentum(torque * seconds);
        }
        torques.remove_if([](const pair<int, glm::vec3> & torque){return torque.first <= 0;});
        glm::vec3 angular_velocity = glm::vec3(I * glm::vec4(angular_momentum, 1.0f));
        glm::quat angular_velocity_q(angular_velocity);
        angular_velocity_q.w = 0;
        transform->set_q(transform->get_q() + (angular_velocity_q * transform->get_q()) * 0.5 * seconds);

        #if PHYSICS_METHOD == 0
        explicit_euler_numerical_integration(acceleration, seconds);
        #endif
        #if PHYSICS_METHOD == 1
        maths(acceleration, seconds);
        #endif


        #if PRINT_VELOCITY == 1
        static double global_time;
        global_time += seconds;
        std::cout << "Velocity for object " << id << " at time " << global_time << "\n";
        velocity.print();
        #endif

        check_bounds();
    }

    void rigidbody::check_bounds()
    {
        #if CLIP_Y_TO_0
        if (transform->get_y() < 0)
        {
            transform->get_y() = 0;
            immovable_collision(glm::vec3(0,1,0));
        }
        #endif
        #if CLIP_BOX == 1
        if (transform->get_x() < -CLIP_BOX_SIZE)
        {
            transform->get_x() = -CLIP_BOX_SIZE;
            immovable_collision(glm::vec3(1,0,0));
        }
        if (transform->get_x() > CLIP_BOX_SIZE)
        {
            transform->get_x() = CLIP_BOX_SIZE;
            immovable_collision(glm::vec3(-1,0,0));
        }
        if (transform->get_z() < -CLIP_BOX_SIZE)
        {
            transform->get_z() = -CLIP_BOX_SIZE;
            immovable_collision(glm::vec3(0,0,1));
        }
        if (transform->get_z() > CLIP_BOX_SIZE)
        {
            transform->get_z() = CLIP_BOX_SIZE;
            immovable_collision(glm::vec3(0,0,-1));
        }
        #endif
    }

    void rigidbody::explicit_euler_numerical_integration(const glm::vec3 & acceleration, const double & seconds)
    {
        velocity += acceleration * (float)seconds;
        transform->translateXYZ(velocity * (float)seconds);
    }

    void rigidbody::maths(const glm::vec3 & acceleration, const double & seconds)
    {
        //Update position based on 1/2at^2 + vt + x
        transform->translateXYZ(acceleration * 0.5f * (float)seconds * (float)seconds + velocity*(float)seconds);
        //Update velocity so that v_0 is correct for the next calculation
        velocity += acceleration * (float)seconds;
    }

    void rigidbody::add_force_to_acceleration(glm::vec3 & acceleration, const glm::vec3 & force)
    {
        acceleration += force / mass;
    }

    void rigidbody::add_torque_to_angular_momentum(const glm::vec3 & torque)
    {
        angular_momentum += torque;
    }

    void rigidbody::calculate_center_of_mass(unit* parent)
    {
        glm::vec3 com;
        model* mod = parent->get_model_ptr();
        //float scale = parent->get_scale();
        for (size_t x = 0; x < mod->num_verticies; ++x)
        {
            com.x += mod->verticies[x].x;
            com.y += mod->verticies[x].y;
            com.z += mod->verticies[x].z;
        }
        center_of_mass = com / (float)mod->num_verticies;
    }

    glm::vec3 rigidbody::get_world_center_of_mass()
    {
        glm::vec4 transformed = transform->to_matrix() * glm::vec4(center_of_mass, 1.0f);
        return glm::vec3(transformed.x, transformed.y, transformed.z);
    }

    float rigidbody::calculate_farthest_dimension(unit* parent)
    {
        float farthest = 0.0;
        model* mod = parent->get_model_ptr();
        for (size_t x = 0; x < mod->num_verticies; ++x)
        {
            if (fabs(mod->verticies[x].x - center_of_mass.x) > farthest)
                farthest = fabs(mod->verticies[x].x - center_of_mass.x);
            if (fabs(mod->verticies[x].y - center_of_mass.y) > farthest)
                farthest = fabs(mod->verticies[x].y - center_of_mass.y);
            if (fabs(mod->verticies[x].z - center_of_mass.z) > farthest)
                farthest = fabs(mod->verticies[x].z - center_of_mass.z);
        }
        farthest_dimension = farthest;
        return farthest;
    }

    void rigidbody::calculate_axis_aligned_bounding_box(unit* parent)
    {
        model* mod = parent->get_model_ptr();
        float min_x = mod->verticies[0].x;
        float max_x = mod->verticies[0].x;
        float min_y = mod->verticies[0].y;
        float max_y = mod->verticies[0].y;
        float min_z = mod->verticies[0].z;
        float max_z = mod->verticies[0].z;
        for (size_t x = 0; x < mod->num_verticies; ++x)
        {
            if (mod->verticies[x].x < min_x)
                min_x = mod->verticies[x].x;
            if (mod->verticies[x].x > max_x)
                max_x = mod->verticies[x].x;
            if (mod->verticies[x].y < min_y)
                min_y = mod->verticies[x].y;
            if (mod->verticies[x].y > max_y)
                max_y = mod->verticies[x].y;
            if (mod->verticies[x].z < min_z)
                min_z = mod->verticies[x].z;
            if (mod->verticies[x].z > max_z)
                max_z = mod->verticies[x].z;
        }

        colliders.push_back(new aabb_collider(transform, glm::vec3(min_x, min_y, min_z), glm::vec3(max_x, max_y, max_z)));
    }
}
