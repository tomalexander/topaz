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
#include "animation.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include "print.h"

namespace
{
    bool similar(const glm::mat4 & a, const glm::mat4 & b, float ok_range = 0.1f)
    {
        for (u32 i = 0; i < 16; ++i)
        {
            float lower = a[i%4][i/4] - ok_range;
            float upper = a[i%4][i/4] + ok_range;
            float test = b[i%4][i/4];
            if (test < lower || test > upper)
                return false;
        }
        return true;
    }

    template<class T, class V>
    bool contains(const V & needle, const T & haystack)
    {
        for (const V & entry : haystack)
        {
            if (entry == needle)
                return true;
        }
        return false;
    }

    bool has_negative(const vector<float> & values)
    {
        for (const float & cur : values)
            if (cur < 0.0f)
                return true;
        return false;
    }
}

namespace topaz
{
    extern glm::mat4 fix_z_up_matrix;

    panda_node* load_animation(const string & file_path)
    {
        long file_data_size;
        char* file_data = read_fully_string(file_path.c_str(), file_data_size);
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

    glm::mat4 animation::apply(const unsigned int & animation_progress, joint* target_joint, bool progress_is_frame_number)
    {
        string joint_name = target_joint->name;
        animation_joint* an_joint = NULL;
        glm::mat4 tmp_transform;
        auto it = joints.find(joint_name);
        if (it == joints.end())
        {
            std::cerr << "Attempted to apply animation to joint that does not exist: " << joint_name << "\n";
            return tmp_transform;
        }
        an_joint = it->second;
            
        float seconds = ((float) animation_progress)/1000.0f;
        

        for (const char & order_char : an_joint->order)
        {
            for (const pair<char, vector<float> > & pos : an_joint->positions)
            {
                if (pos.first != order_char && order_char != 't' && order_char != 's')
                    continue;
                //Calculate the frame
                float time_for_frame = 1.0f / ((float) pos.second.size());
                float frame_number_float = seconds / time_for_frame;
                int pre_frame_ind = floor(frame_number_float);
                int post_frame_ind = pre_frame_ind + 1;
                if (pre_frame_ind == pos.second.size())
                {
                    pre_frame_ind = 0;
                    post_frame_ind = 1;
                }
                if (pre_frame_ind != 0 && post_frame_ind == pos.second.size())
                    post_frame_ind = 0;
                float percent_to_post = frame_number_float - ((float)pre_frame_ind);
                
                if (progress_is_frame_number)
                {
                    pre_frame_ind = (pos.second.size() == 1 ? 0 : animation_progress);
                    post_frame_ind = pre_frame_ind + 1;
                    if (pre_frame_ind != 0 && post_frame_ind == pos.second.size())
                        post_frame_ind = 0;
                    percent_to_post = 0.0f;
                }

                float value_for_frame;
                if (pos.second.size() == 1)
                {
                    value_for_frame = pos.second[pre_frame_ind];
                } else {
                    value_for_frame = glm::lerp(pos.second[pre_frame_ind], pos.second[post_frame_ind], percent_to_post);
                }


                if (order_char == 't')
                {
                    switch (pos.first)
                    {
                      case 'x':
                        tmp_transform = glm::translate(tmp_transform, glm::vec3(value_for_frame, 0, 0));
                        break;
                      case 'y':
                        tmp_transform = glm::translate(tmp_transform, glm::vec3(0, value_for_frame, 0));
                        break;
                      case 'z':
                        tmp_transform = glm::translate(tmp_transform, glm::vec3(0, 0, value_for_frame));
                        break;
                      default:
                        break;
                    }
                } else if (order_char == 's') {
                    //Scale and Shear
                    switch (pos.first)
                    {
                      case 'i':
                        tmp_transform = glm::scale(tmp_transform, glm::vec3(value_for_frame, 1.0f, 1.0f));
                        break;
                      case 'j':
                        tmp_transform = glm::scale(tmp_transform, glm::vec3(1, value_for_frame, 1));
                        break;
                      case 'k':
                        tmp_transform = glm::scale(tmp_transform, glm::vec3(1, 1, value_for_frame));
                        break;
                      default:
                        break;
                    }
                } else {
                    switch(pos.first)
                    {
                      case 'h':
                        tmp_transform = glm::rotate(tmp_transform, value_for_frame, glm::vec3(0, 1, 0));
                        break;
                      case 'p':
                        tmp_transform = glm::rotate(tmp_transform, value_for_frame, glm::vec3(1, 0, 0));
                        break;
                      case 'r':
                        tmp_transform = glm::rotate(tmp_transform, value_for_frame, glm::vec3(0, 0, 1));
                        break;
                      default:
                        std::cerr << "Unrecognized degree of freedom " << pos.first << "\n";
                    }
                }
            }
        }
        return tmp_transform;
    }
}

