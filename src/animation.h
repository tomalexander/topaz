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
#ifndef ANIMATION_H_
#define ANIMATION_H_

#include "panda_node.h"
#include <map>
#include <utility>
#include <string>
#include <unordered_map>

using std::map;
using std::make_pair;
using std::string;
using std::pair;
using std::unordered_map;

namespace topaz
{
    panda_node* load_animation(const string & file_path);

    typedef struct animation_joint
    {
        string name;
        float fps;
        string order;
        map<char, vector<float> > positions; /**< char = degree of freedom (x,y,z,h,p,r) */
    } animation_joint;

    class animation
    {
      public:
        animation();
        ~animation();

        glm::mat4 apply(const unsigned int & animation_progress, joint* target_joint, bool progress_is_frame_number = false);

        unordered_map<string, animation_joint*> joints;
      private:
    };
}
#endif
