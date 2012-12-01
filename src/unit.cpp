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
#include "def.h"
#include "unit.h"
#include "types.h"
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include "rigidbody.h"
#include "sqt.h"
#include <glm/gtc/matrix_transform.hpp>
#include "print.h"

using std::min;
using std::max;
using std::stringstream;

namespace topaz
{
    unit::unit()
    {
        model_ptr = NULL;
        light_program = NULL;
        light_source = NULL;
        root_joint = NULL;
        current_animation = NULL;
        animation_progress = 0;
        rigid_body = NULL;
        transform = new sqt();
        add_pre_draw_function(id, std::bind(&topaz::unit::update, this, std::placeholders::_1));
        add_draw_function(id, std::bind(&topaz::unit::draw, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    unit::unit(model* _model_ptr)
    {
        light_program = NULL;
        light_source = NULL;
        root_joint = NULL;
        model_ptr = NULL;
        set_model(_model_ptr);
        current_animation = NULL;
        animation_progress = 0;
        rigid_body = NULL;
        transform = new sqt();
        add_pre_draw_function(id, std::bind(&topaz::unit::update, this, std::placeholders::_1));
        add_draw_function(id, std::bind(&topaz::unit::draw, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    unit::~unit()
    {
        delete root_joint;
        delete rigid_body;
        delete transform;
    }

    void unit::draw(const glm::mat4 & V, const glm::mat4 & P, camera* C)
    {
        topaz::gl_program* program = light_program;
        if (light_program == NULL)
            program = model_ptr->model_program;
        model_ptr->prep_for_draw(transform->to_matrix(), V, P, C, program, light_source);
        

        if (model_ptr->has_joints)
        {
            root_joint->update(animation_progress, current_animation);
            
            float joint_matricies[model_ptr->get_num_joints()*16];
            root_joint->populate_float_array(joint_matricies);

            glUniformMatrix4fv(program->uniform_locations["joints"], model_ptr->get_num_joints(), GL_FALSE, &(joint_matricies[0]));
            CHECK_GL_ERROR("filling joint matricies");
        }

        model_ptr->draw();
        if (model_ptr->has_joints)
            draw_joints(V, P, C);
    }

    void unit::draw_joints(const glm::mat4 & V, const glm::mat4 & P, camera* C)
    {
        list<joint*> to_draw;
        to_draw.push_back(root_joint);
        while (!to_draw.empty())
        {
            joint* current = to_draw.front();
            to_draw.pop_front();
            for (const pair<string, joint*> & child : current->joints)
                to_draw.push_back(child.second);
            if (current == root_joint)
                continue;

            draw_sphere_single_frame(current->world, V, P, C, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        }
    }

    void unit::set_scale(float new_scale)
    {
        transform->scale(new_scale);
    }

    void unit::update(int milliseconds)
    {
        if (current_animation != NULL)
        {
            animation_progress += milliseconds;
            while (animation_progress > 1000)
                animation_progress -= 1000;
        }
    }

    float unit::get_distance_to(unit* other)
    {
        glm::vec3 difference = transform->get_t() - other->transform->get_t();
        glm::vec3 square = difference * difference;
        float sum = square.x + square.y + square.z;
        return sqrt(sum);
    }

    float unit::get_distance_to(const glm::vec3 & other)
    {
        glm::vec3 difference = -other + transform->get_t();
        glm::vec3 square = difference * difference;
        float sum = square.x + square.y + square.z;
        return sqrt(sum);
    }

    void unit::set_model(model* _model_ptr)
    {
        if (model_ptr != NULL)
            delete model_ptr;

        model_ptr = _model_ptr;
        
        if (root_joint != NULL)
            delete root_joint;

        root_joint = duplicate_joint_tree(model_ptr->root_joint);
    }

    bool unit::set_animation(const string & animation_name)
    {
        if (model_ptr == NULL)
            return false;

        auto it = model_ptr->animations.find(animation_name);
        
        if (it == model_ptr->animations.end())
            return false;
        
        current_animation = it->second;
        return true;
    }

    void unit::set_rigidbody(const string & type)
    {
        if (type == "SPHERE")
            rigid_body = new rigidbody(transform, this, SPHERE_TENSOR, type);
        else if (type == "BOX")
            rigid_body = new rigidbody(transform, this, SPHERE_TENSOR, type);
    }
    
    void unit::add_location(const glm::vec3 & diff)
    {
        transform->translateXYZ(diff);
        //transform->t += diff;
    }
}
