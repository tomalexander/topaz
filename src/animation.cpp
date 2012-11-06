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

namespace topaz
{
    panda_node* load_animation(const string & file_path)
    {
        long file_data_size;
        char* file_data = read_fully(file_path.c_str(), file_data_size);
        if (file_data == NULL)
            return NULL;
        
        string contents(file_data);

        panda_node* top_node = new panda_node("","",contents);
        
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

    void animation::apply(unsigned int & animation_progress, joint* target_joint)
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
        //std::cout << seconds << std::endl;

        vec translate;
        //quaternion rotate;
        matrix tmp_transform;

        vec translate_origin;
        quaternion rotate_origin;

        for (const char & order_char : an_joint->order)
        {
            for (const pair<char, vector<float> > & pos : an_joint->positions)
            {
                
                if (pos.first != order_char && order_char != 't')
                    continue;
                // if (pos.second.size() == 1)
                //     continue;
                float time_for_frame = 1.0f / ((float) pos.second.size());
                float frame_number_float = seconds / time_for_frame;
                int pre_frame_ind = floor(frame_number_float);
                int post_frame_ind = ceil(frame_number_float);
                float percent_to_post = frame_number_float - ((float)pre_frame_ind);
                if (order_char == 't')
                {
                    switch (pos.first)
                    {
                      case 'x':
                        if (pre_frame_ind == 0)
                        {
                            translate.x() = pos.second[post_frame_ind-1];
                        } else {
                            translate.x() += lerp(pos.second[pre_frame_ind-1], pos.second[post_frame_ind-1], percent_to_post);
                        }
                        break;
                      case 'y':
                        if (pre_frame_ind == 0)
                        {
                            translate.y() = pos.second[post_frame_ind-1];
                        } else {
                            translate.y() += lerp(pos.second[pre_frame_ind-1], pos.second[post_frame_ind-1], percent_to_post);
                        }
                        break;
                      case 'z':
                        if (pre_frame_ind == 0)
                        {
                            translate.z() = pos.second[post_frame_ind-1];
                        } else {
                            translate.z() += lerp(pos.second[pre_frame_ind-1], pos.second[post_frame_ind-1], percent_to_post);
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
                            tmp_transform.rotateH(TO_RADIANS(pos.second[post_frame_ind-1]));
                        } else {
                            tmp_transform.rotateH(TO_RADIANS(lerp(pos.second[pre_frame_ind-1], pos.second[post_frame_ind-1], percent_to_post)));
                        }
                        break;
                      case 'p':
                        if (pre_frame_ind == 0)
                        {
                            tmp_transform.rotateP(TO_RADIANS(pos.second[post_frame_ind-1]));
                        } else {
                            tmp_transform.rotateP(TO_RADIANS(lerp(pos.second[pre_frame_ind-1], pos.second[post_frame_ind-1], percent_to_post)));
                        }
                        break;
                      case 'r':
                        if (pre_frame_ind == 0)
                        {
                            tmp_transform.rotateR(TO_RADIANS(pos.second[post_frame_ind-1]));
                        } else {
                            tmp_transform.rotateR(TO_RADIANS(lerp(pos.second[pre_frame_ind-1], pos.second[post_frame_ind-1], percent_to_post)));
                        }
                        break;
                      default:
                        std::cerr << "Unrecognized degree of freedom " << pos.first << "\n";
                    }
                }
            }
            if (order_char == 't')
            {
                tmp_transform.translateXYZ(translate.x(), translate.y(), translate.z());
            }
        }

        // target_joint->local = tmp_transform * target_joint->local;
        target_joint->local = tmp_transform;
        // target_joint->local *= tmp_transform;
    }
}
