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
#include "animation.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include "print.h"

namespace topaz
{
    extern glm::mat4 fix_z_up_matrix;

    panda_node* load_animation(const string & file_path)
    {
        long file_data_size;
        char* file_data = read_fully(file_path.c_str(), file_data_size);
        if (file_data == NULL)
            return NULL;
        
        string contents(file_data);

        panda_node* top_node = new panda_node("","",contents);
        fix_coordinate_system(top_node);
        top_node->parse();
        
        delete [] file_data;
        return top_node;
    }

    animation::animation()
    {
        
    }

    animation::~animation()
    {
        map<string, animation_joint*> joints;
        for (pair<string, animation_joint*> cur : joints)
        {
            delete cur.second;
        }
    }

    void animation::apply(const unsigned int & animation_progress, joint* target_joint)
    {
        string joint_name = target_joint->name;
        animation_joint* an_joint = NULL;
        for (pair<string, animation_joint*> cur : joints)
        {
            if (cur.first != joint_name)
                continue;

            an_joint = cur.second;
        }
            
        float seconds = ((float) animation_progress)/1000.0f;
        // seconds = 0;
        // target_joint->local = glm::translate(target_joint->local, glm::vec3(2.0f * seconds,0,0));
        // return;

        glm::mat4 tmp_transform;

        for (const char & order_char : an_joint->order)
        {
            for (const pair<char, vector<float> > & pos : an_joint->positions)
            {
                
                if (pos.first != order_char && order_char != 't' && order_char != 's')
                    continue;
                float time_for_frame = 1.0f / ((float) pos.second.size());
                float frame_number_float = seconds / time_for_frame;
                int pre_frame_ind = floor(frame_number_float);
                int post_frame_ind = pre_frame_ind + 1;
                //int post_frame_ind = ceil(frame_number_float);
                float percent_to_post = frame_number_float - ((float)pre_frame_ind);
                if (order_char == 't')
                {
                    switch (pos.first)
                    {
                      case 'x':
                        if (pre_frame_ind == 0)
                        {
                            tmp_transform = glm::translate(tmp_transform, glm::vec3(pos.second[post_frame_ind-1], 0, 0));
                        } else {
                            tmp_transform = glm::translate(tmp_transform, glm::vec3(glm::lerp(pos.second[pre_frame_ind-1], pos.second[post_frame_ind-1], percent_to_post), 0, 0));
                        }
                        break;
                      case 'y':
                        if (pre_frame_ind == 0)
                        {
                            tmp_transform = glm::translate(tmp_transform, glm::vec3(0, pos.second[post_frame_ind-1], 0));
                        } else {
                            tmp_transform = glm::translate(tmp_transform, glm::vec3(0, glm::lerp(pos.second[pre_frame_ind-1], pos.second[post_frame_ind-1], percent_to_post), 0));
                        }
                        break;
                      case 'z':
                        if (pre_frame_ind == 0)
                        {
                            tmp_transform = glm::translate(tmp_transform, glm::vec3(0, 0, pos.second[post_frame_ind-1]));
                        } else {
                            tmp_transform = glm::translate(tmp_transform, glm::vec3(0, 0, glm::lerp(pos.second[pre_frame_ind-1], pos.second[post_frame_ind-1], percent_to_post)));
                        }
                        break;
                      default:
                        break;
                    }
                } else if (order_char == 's') {
                    //Scale and Shear
                    switch (pos.first)
                    {
                      case 'i':
                        if (pre_frame_ind == 0)
                        {
                            tmp_transform = glm::scale(tmp_transform, glm::vec3(pos.second[post_frame_ind-1], 1.0f, 1.0f));
                        } else {
                            tmp_transform = glm::scale(tmp_transform, glm::vec3(glm::lerp(pos.second[pre_frame_ind-1], pos.second[post_frame_ind-1], percent_to_post), 1, 1));
                        }
                        break;
                      case 'j':
                        if (pre_frame_ind == 0)
                        {
                            tmp_transform = glm::scale(tmp_transform, glm::vec3(1, pos.second[post_frame_ind-1], 1));
                        } else {
                            tmp_transform = glm::scale(tmp_transform, glm::vec3(1, glm::lerp(pos.second[pre_frame_ind-1], pos.second[post_frame_ind-1], percent_to_post), 1));
                        }
                        break;
                      case 'k':
                        if (pre_frame_ind == 0)
                        {
                            tmp_transform = glm::scale(tmp_transform, glm::vec3(1, 1, pos.second[post_frame_ind-1]));
                        } else {
                            tmp_transform = glm::scale(tmp_transform, glm::vec3(1, 1, glm::lerp(pos.second[pre_frame_ind-1], pos.second[post_frame_ind-1], percent_to_post)));
                        }
                        break;
                      default:
                        break;
                    }
                } else {
                    switch(pos.first)
                    {
                      case 'h':
                        if (pre_frame_ind == 0)
                        {
                            tmp_transform = glm::rotate(tmp_transform, pos.second[post_frame_ind-1], glm::vec3(0, 1, 0));
                        } else {
                            tmp_transform = glm::rotate(tmp_transform, glm::lerp(pos.second[pre_frame_ind-1], pos.second[post_frame_ind-1], percent_to_post), glm::vec3(0, 1, 0));
                        }
                        break;
                      case 'p':
                        if (pre_frame_ind == 0)
                        {
                            tmp_transform = glm::rotate(tmp_transform, pos.second[post_frame_ind-1], glm::vec3(1, 0, 0));
                        } else {
                            tmp_transform = glm::rotate(tmp_transform, glm::lerp(pos.second[pre_frame_ind-1], pos.second[post_frame_ind-1], percent_to_post), glm::vec3(1, 0, 0));
                        }
                        break;
                      case 'r':
                        if (pre_frame_ind == 0)
                        {
                            tmp_transform = glm::rotate(tmp_transform, pos.second[post_frame_ind-1], glm::vec3(0, 0, 1));
                        } else {
                            tmp_transform = glm::rotate(tmp_transform, glm::lerp(pos.second[pre_frame_ind-1], pos.second[post_frame_ind-1], percent_to_post), glm::vec3(0, 0, 1));
                        }
                        break;
                      default:
                        std::cerr << "Unrecognized degree of freedom " << pos.first << "\n";
                    }
                }
            }
        }

        // target_joint->local = tmp_transform * target_joint->local;
        target_joint->local *= tmp_transform;
        // target_joint->local *= tmp_transform;
    }
}
