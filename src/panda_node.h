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
#ifndef PANDA_NODE_H_
#define PANDA_NODE_H_

#include <vector>
#include <string>
#include "model.h"
#include "vertex.h"
#include "util.h"
#include "animation.h"
#include "coordinate_system.h"

using std::vector;
using std::string;

namespace topaz
{
    class animation;
    class model;
    string filter_out_tags(const string & inp);

    class panda_node
    {
      public:
        panda_node(const string & tag, const string & name, const string & content, panda_node* parent = nullptr):
            tag(tag),
            name(name),
            content(content),
            parent(parent)
        {coordinate_system = UNASSIGNED; uses_color = false; if (parent != nullptr) parent->add_child(this); generate_tag_tree(content, this); this->content = trim_whitespace(filter_out_tags(this->content));}
        ~panda_node()
        {
            for (panda_node* & cur : children)
                delete cur;
        }

        void add_child(panda_node* child)
        {
            children.push_back(child);
            child->parent = this;
        }

        static int get_next_vertex_id() {static int next_id = 1; return next_id++;}

        void parse();
        static bool get_tag(const string & input, string & tag, string & name, string & content, size_t & start_index);
static void generate_tag_tree(const string & content, panda_node* parent);

        /* Stuff related to the tree structure */
        vector<panda_node*> children;
        string tag, name, content;
        panda_node* parent;

        /* Stuff parsed out of content to make transition to model class easier */
        coordinate_system coordinate_system;
        string texture_file_name;

        vertex vert;
        int vertex_number;

        float polygon_rgba[4];
        string polygon_texture_ref;
        vector<int> vertex_refs;
        string vertex_ref_pool;

        /* Stuff parsed out of content to make animation easier */
        float fps;
        glm::mat4 mat;
        float membership;
        //vertex refs from above is re-used
        vector<float> V_values;
        string dof;
        string order;

        bool uses_color;

        long count_verticies();
        long count_indicies();
        static model* to_model(panda_node* root);
        static animation* to_animation(panda_node* root);
      private:
    };

    
}

#endif
