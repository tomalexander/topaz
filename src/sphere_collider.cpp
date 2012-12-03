/*
 * Copyright (c) 2012 Tom Alexander, Tate Larsen
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
#include "sphere_collider.h"
#include "aabb_collider.h"

namespace topaz
{
    sphere_collider::sphere_collider(sqt* _parent_transform, const glm::vec3 & _offset, float _radius)
    {
        type = SPHERE;
        radius = _radius;
        visible = false;
        draw_sphere = NULL;
        transform = new sqt(_parent_transform);
        transform->translateXYZ(_offset);
        #if DRAW_COLLISION_SOLIDS == 1
        set_visible(true);
        #endif
        #if USE_OPENCL == 1
        collider_id = global_accelerator->num_of_colliders;
        global_accelerator->increase_collider_size_to(global_accelerator->num_of_colliders+1);
        global_accelerator->colliders.push_back(this);
        push_to_cl();
        #endif
    }

    sphere_collider::~sphere_collider()
    {
        delete transform;
    }

    glm::vec3* sphere_collider::is_colliding_with(collider* other)
    {
        switch (other->type)
        {
          case SPHERE:
            return sphere_collision((sphere_collider*) other);
            break;
          case AABB:
            return AABB_collision((aabb_collider*) other);
            break;
          default:
            break;
        }
        return NULL;
    }

    glm::vec3* sphere_collider::AABB_collision(aabb_collider* other)
    {
        glm::vec3 world_sphere_location = get_world_position();
        float world_radius = radius/* * transform->get_world_s()*/;
        glm::mat4 box_to_world = other->transform->to_matrix();
        glm::mat4 world_to_box = glm::inverse(box_to_world);
        glm::vec3 box_sphere_location = glm::vec3(world_to_box * glm::vec4(world_sphere_location,1.0f));
        glm::vec3* closest = new glm::vec3();
        if (box_sphere_location.x <  other->lesser_corner.x)
            closest->x = other->lesser_corner.x;
        else if (box_sphere_location.x >  other->greater_corner.x)
            closest->x = other->greater_corner.x;
        else
            closest->x = box_sphere_location.x;

        if (box_sphere_location.y <  other->lesser_corner.y)
            closest->y = other->lesser_corner.y;
        else if (box_sphere_location.y >  other->greater_corner.y)
            closest->y = other->greater_corner.y;
        else
            closest->y = box_sphere_location.y;

        if (box_sphere_location.z <  other->lesser_corner.z)
            closest->z = other->lesser_corner.z;
        else if (box_sphere_location.z >  other->greater_corner.z)
            closest->z = other->greater_corner.z;
        else
            closest->z = box_sphere_location.z;

        (*closest) = glm::vec3(box_to_world * glm::vec4((*closest), 1.0f));
        if (glm::length(world_sphere_location - (*closest)) <= world_radius)
        {
            return closest;
        } else {
            delete closest;
        }

        return NULL;
    }

    glm::vec3* sphere_collider::sphere_collision(sphere_collider* other)
    {
        glm::vec3 difference = get_world_position() - other->get_world_position();
        difference = difference * difference;
        float distance = difference.x + difference.y + difference.z;
        float min_distance = radius + other->radius;
        if (distance <= (min_distance * min_distance))
        {
            glm::vec3 this_position = get_world_position();
            glm::vec3 this_to_other = other->get_world_position() - this_position;
            this_to_other /= 2.0f;
            return new glm::vec3(this_position.x+this_to_other.x, this_position.y+this_to_other.y, this_position.z+this_to_other.z);
        } else {
            return NULL;
        }
    }

    void sphere_collider::set_visible(bool _visible)
    {
        if (_visible == visible)
            return;

        visible = _visible;
        if (visible)
        {
            draw_sphere = new sphere_primitive(transform, radius / transform->get_world_s(), glm::vec4(1,1,1,0.5));
        } else {
            delete draw_sphere;
            draw_sphere = NULL;
        }
    }

    #if DRAW_COLLISION_SOLIDS == 1
    void sphere_collider::display_as_colliding(bool colliding)
    {
        if (colliding)
        {
            draw_sphere->color = glm::vec4(1,0,0,0.5);
        } else {
            draw_sphere->color = glm::vec4(1,1,1,0.5);
        }
    }
    #endif

    glm::vec3 sphere_collider::get_world_position()
    {
        glm::vec3 origin(0,0,0);
        return glm::vec3(transform->to_matrix() * glm::vec4(origin, 1.0f));
    }
}
