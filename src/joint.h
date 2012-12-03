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
#ifndef JOINT_H_
#define JOINT_H_

#include "topaz.h"
#include <map>
#include <string>
#include "sqt.h"

using std::map;
using std::string;

namespace topaz
{
    class panda_node;
    class animation;
    class joint
    {
      public:
        joint();
        joint(const string & name, unsigned int & available_index_in_shader);
        ~joint();

        void update(const unsigned int & animation_progress, animation* current_animation);
        void print(int indentation = 0, std::ostream & out = std::cout);
        void recursive_print(int indentation = 0, std::ostream & out = std::cout);
        void check_if_associated(const int vert_index, map<int, float> & joint_association);
        void populate_float_array(float* joint_matricies);
        joint* find_joint_indexed_in_shader(int index);
        joint* find_joint_name(const string & _name);
        joint* find_root();
        void push_binding_down();
        void update_binding_matrix(animation* current_animation);

        sqt transform;

        //matrix transform;       /**< From egg file, B-1 */
        glm::mat4 binding;
        glm::mat4 inverse_binding;
        glm::mat4 local;
        glm::mat4 world;
        glm::mat4 local_binding;
        glm::mat4 anim_binding;
        glm::mat4 anim_inverse_binding;

        map<int, float> membership; /**< key is index or vertex, float is percentage of membership */
        map<string, joint*> joints; /**< contains child joints */
        string name;
        joint* parent;
        unsigned int index_in_shader;
        void add_child(joint* child);
        unsigned int size() const;
        

      private:
        joint* _find_joint_name(const string & _name);
    };

    joint* generate_joint(panda_node* inp, const map<int,int> & vertex_id_to_array_index, unsigned int & available_index_in_shader);
    joint* duplicate_joint_tree(joint* root);
}
#endif
