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
#include "panda_node.h"
#include "egg_parser.h"
#include <sstream>
#include <iostream>
#include <map>
#include <utility>
#include <list>
#include <functional>
#include "joint.h"
#include "coordinate_system.h"

using std::stringstream;
using std::map;
using std::make_pair;
using std::list;
using std::function;

namespace topaz
{
    extern glm::mat4 fix_z_up_matrix;
    void panda_node::parse()
    {
        //std::cout << tag << std::endl;
        for (panda_node* child : children)
            child->parse();

        if (tag == "Group")
        {
        }
        else if (tag == "Vertex")
        {
            float x,y,z,u,v,r,g,b,a;
            {
                stringstream tmp(content);
                tmp >> x >> y >> z;
            }
            {
                stringstream tmp(name);
                int tmp_next_id = panda_node::get_next_vertex_id();
                if (!(tmp >> vertex_number))
                    vertex_number = tmp_next_id;
            }
            vert.x = x;
            vert.y = y;
            vert.z = z;
            for (panda_node* child : children)
            {
                if (child->tag == "Normal")
                {
                    stringstream tmp(child->content);
                    tmp >> x >> y >> z;
                    vert.nx = x;
                    vert.ny = y;
                    vert.nz = z;
                }
                else if (child->tag == "UV")
                {
                    stringstream tmp(child->content);
                    tmp >> u >> v;
                    vert.u = u;
                    vert.v = v;
                }
                else if (child->tag == "RGBA")
                {
                    stringstream tmp(child->content);
                    tmp >> r >> g >> b >> a;
                    vert.r = r;
                    vert.g = g;
                    vert.b = b;
                    vert.a = a;
                    uses_color = true;
                }
                delete child;
            }
            children.clear();
            content.clear(); // No longer need string representation of data
        }
        else if (tag == "Texture")
        {
            size_t start_index = content.find("\"")+1;
            if (start_index == string::npos+1)
            {
                std::cerr << "ERROR: Texture without opening quote\n";
                return;
            }

            size_t end_index = content.find("\"", start_index);
            if (end_index == string::npos)
            {
                std::cerr << "ERROR: Texture without closing quote\n";
                return;
            }
            texture_file_name = string(content, start_index, end_index-start_index);
            
        }
        else if (tag == "Polygon")
        {
            for (panda_node* child : children)
            {
                if (child->tag == "RGBA")
                {
                    stringstream tmp(child->content);
                    tmp >> polygon_rgba[0] >> polygon_rgba[1] >> polygon_rgba[2] >> polygon_rgba[3];
                }
                else if (child->tag == "TRef")
                {
                    polygon_texture_ref = child->content;
                }
                else if (child->tag == "VertexRef")
                {
                    stringstream tmp(child->content);
                    int current;
                    int index = 0;
                    while (tmp >> current)
                    {
                        if (index == 3)
                        {
                            //push back 2 from end again
                            vertex_refs.push_back(vertex_refs[vertex_refs.size()-3]);
                            //push back end which is now 1 from end
                            vertex_refs.push_back(vertex_refs[vertex_refs.size()-2]);
                            //grab 1 more
                            index = 2;
                        }
                        vertex_refs.push_back(current);
                        ++index;
                    }
                    //Copy data up from child <Ref>
                    for (panda_node* subchild : child->children)
                    {
                        if (subchild->tag == "Ref")
                            vertex_ref_pool = subchild->vertex_ref_pool;
                    }
                }
            }
        }
        else if (tag == "Ref")
        {
            vertex_ref_pool = content;
        }
        else if (tag == "Xfm$Anim_S$")
        {

        }
        else if (tag == "Scalar")
        {
            if (name == "membership")
            {
                stringstream tmp(content);
                tmp >> membership;
            }
            if (name == "fps")
            {
                stringstream tmp(content);
                tmp >> fps;
            }
        }
        else if (tag == "Char*")
        {
            if (name == "order")
            {
                order = content;
            }
        }
        else if (tag == "S$Anim")
        {
            dof = name;
            if (children.size() > 1)
            {
                std::cerr << "More than one child to S$Anim\n";
            }
            for (panda_node* child : children)
            {
                if (child->tag == "V")
                {
                    V_values = child->V_values;
                }
            }
        }
        else if (tag == "V")
        {
            stringstream tmp(content);
            float tmp_float;
            while (tmp >> tmp_float)
            {
                V_values.push_back(tmp_float);
            }
        }
        else if (tag == "Joint")
        {
            panda_node* transform = NULL;
            for (panda_node* child : children)
            {
                if (child->tag == "Transform")
                {
                    transform = child;
                    break;
                }
            }
            if (transform == NULL) return;

            panda_node* matrix4 = NULL;
            for (panda_node* child : transform->children)
            {
                if (child->tag == "Matrix4")
                {
                    matrix4 = child;
                    mat = matrix4->mat;
                }
            }
        }
        else if (tag == "Matrix4")
        {
            stringstream tmp(content);
            for (int i = 0; i < 16; ++i)
            {
                float x;
                tmp >> x;
                mat[i%4][i/4] = x;
            }
        }
        else if (tag == "VertexRef")
        {
            stringstream tmp(content);
            float flt;
            while (tmp >> flt)
            {
                vertex_refs.push_back(flt);
            }
        }
    }

    /** 
     * Get the highest level tag that begins at start_index or later
     *
     * @param input the panda egg text
     * @param tag the name that occurs between <>
     * @param name the name that occurs after the tag and before the braces
     * @param content the text inside the braces
     * @param start_index the index to start searching
     *
     * @return true if tag is found, false if not
     */
    bool panda_node::get_tag(const string & input, string & tag, string & name, string & content, size_t & start_index)
    {
        /****************************************
         *             Get Tag                  *
         ****************************************/
        size_t tag_open = input.find('<', start_index);
        if (tag_open == string::npos)
            return false;

        size_t tag_close = input.find('>', tag_open);
        if (tag_close == string::npos)
            return false;

        tag = trim_whitespace(string(input, tag_open+1, tag_close-tag_open-1));
        
        /****************************************
         *             Get Name                 *
         ****************************************/
        size_t name_open = tag_close+1;
        while (input[name_open] == ' ') ++name_open;
        size_t name_close = name_open;
        while (input[name_close] != ' ' && input[name_close] != '{') ++name_close;
        name = trim_whitespace(string(input, name_open, name_close-name_open));

        /****************************************
         *             Get Content              *
         ****************************************/
        size_t content_start = input.find('{', name_close) + 1;
        int indentation = 1;
        size_t content_end = content_start;
        while (indentation > 0)
        {
            if (input[content_end] == '{')
                ++indentation;
            else if (input[content_end] == '}')
                --indentation;
            ++content_end;
            if (content_end == input.size()+1)
                return false;
        }
        content = trim_whitespace(string(input, content_start, content_end-content_start-1));
        to_unix_line_endings_ip(content);
        start_index = content_end+1;
        return true;
    }

    /** 
     * Parse through the egg text to generate a tree of tags
     *
     * @param content the panda egg text
     * @param parent pointer to the panda_node to which we will parent the next level of nodes
     */
    void panda_node::generate_tag_tree(const string & content, panda_node* parent)
    {
        string tag = "", name = "", subcontent = "";
        size_t index = 0;
        while (get_tag(content, tag, name, subcontent, index))
        {
            panda_node* child_node = new panda_node(tag,name,subcontent, parent);
        }
    }

    /** 
     * Count the number of verticies that are children of this node
     *
     *
     * @return the number of verticies that are children of this node
     */
    long panda_node::count_verticies()
    {
        long ret = (tag == "Vertex") ? 1 : 0;
        for (panda_node* child : children)
        {
            ret += child->count_verticies();
        }
        return ret;
    }

    /** 
     * Count the number of indicies that are children of this node
     *
     *
     * @return the number of indicies that are children of this node
     */
    long panda_node::count_indicies()
    {
        long ret = (tag == "Polygon") ? vertex_refs.size() : 0;
        for (panda_node* child : children)
            ret += child->count_indicies();
        return ret;
    }

    /** 
     * Generate a new topaz model from the root panda_node
     *
     * @param root pointer to root panda_node
     *
     * @return pointer to new model
     */
    model* panda_node::to_model(panda_node* root)
    {
        model* ret = new model();
        list<panda_node*> to_visit;
        
        //Check for normals
        to_visit.push_back(root);
        while (to_visit.size() > 0)
        {
            panda_node* current = to_visit.front();
            to_visit.pop_front();

            for (panda_node* child : current->children)
            {
                to_visit.push_back(child);
            }

            if (current->tag == "Normal")
            {
                to_visit.clear();
                ret->has_normals = true;
            }
        }

        //Check for color
        to_visit.push_back(root);
        while (to_visit.size() > 0)
        {
            panda_node* current = to_visit.front();
            to_visit.pop_front();

            for (panda_node* child : current->children)
            {
                to_visit.push_back(child);
            }

            if (current->uses_color)
            {
                to_visit.clear();
                ret->uses_color = true;
            }
        }

        //Load textures
        to_visit.push_back(root);
        bool found_texture = false;
        while (to_visit.size() > 0)
        {
            panda_node* current = to_visit.front();
            to_visit.pop_front();

            for (panda_node* child : current->children)
            {
                to_visit.push_back(child);
            }

            if (current->tag == "Texture")
            {
                to_visit.clear();
                ret->texture = load_texture(current->texture_file_name);
                found_texture = true;
            }
        }
        ret->has_texture = found_texture;

        //Count the verticies
        size_t num_of_verticies = root->count_verticies();
        ret->set_num_verticies(num_of_verticies);

        size_t current_index = 0;
        
        //BFS to collect all verticies
        to_visit.push_back(root);
        map<int,int> vertex_id_to_array_index;
        while (to_visit.size() > 0)
        {
            panda_node* current = to_visit.front();
            to_visit.pop_front();

            if (current->tag == "Vertex")
            {
                ret->verticies[current_index] = current->vert;
                vertex_id_to_array_index.insert(make_pair(current->vertex_number, current_index));
                ++current_index;
            }

            for (panda_node* child : current->children)
            {
                to_visit.push_back(child);
            }
        }

        //Count the indicies in polygons
        size_t num_of_indicies = root->count_indicies();
        ret->set_num_indicies(num_of_indicies);

        current_index = 0;

        //BFS to collect all polygons
        to_visit.push_back(root);
        while (to_visit.size() > 0)
        {
            panda_node* current = to_visit.front();
            to_visit.pop_front();

            if (current->tag == "Polygon")
            {
                int count = 0;
                for (int cur : current->vertex_refs)
                {
                    ret->indicies[current_index] = vertex_id_to_array_index[cur];
                    ++current_index;
                    ++count;
                    if (count == 3)
                    {
                        count = 0;
                    }
                }
                
            }

            for (panda_node* child : current->children)
            {
                to_visit.push_back(child);
            }
        }

        //Find all the joints
        unsigned int next_available = 0;
        ret->root_joint = new joint("_ROOT", next_available);
        to_visit.push_back(root);
        while (to_visit.size() > 0)
        {
            panda_node* current = to_visit.front();
            to_visit.pop_front();

            if (current->tag == "Joint")
            {
                joint* cur_joint = generate_joint(current, vertex_id_to_array_index, next_available);
                ret->root_joint->add_child(cur_joint);
            } else {
                for (panda_node* child : current->children)
                    to_visit.push_back(child);
            }
        }
        ret->set_num_joints(ret->root_joint->size()-1);

        //Populate joint info in verticies
        unsigned int num_joints_per_vertex = 0;
        for (int x = 0; x < ret->num_verticies; ++x)
        {
            map<int, float> joint_association; //int = joint index in shader, float = weight
            ret->root_joint->check_if_associated(x, joint_association);
            if (joint_association.size() != 3)
            {
                //std::cerr << "ERROR: Different than 3 joints on a vertex\n";
                //exit(1);
            }
            for (std::pair<int, float> cur : joint_association)
            {
                ret->verticies[x].joint_indicies.push_back(cur.first);
                ret->verticies[x].joint_membership.push_back(cur.second);
            }
            if (ret->verticies[x].joint_indicies.size() > num_joints_per_vertex)
            {
                num_joints_per_vertex = ret->verticies[x].joint_indicies.size();
            }
        }

        if (num_joints_per_vertex > 0)
        {
            ret->set_num_joints_per_vertex(num_joints_per_vertex);
        }

        ret->root_joint->push_binding_down();
        ret->move_to_gpu();
        return ret;
    }

    animation* panda_node::to_animation(panda_node* root)
    {
        animation* ret = new animation();

        list<panda_node*> to_visit;
        to_visit.push_back(root);
        while (!to_visit.empty())
        {
            panda_node* current = to_visit.front();
            panda_node* parent = NULL;
            to_visit.pop_front();
            
            for (panda_node* child : current->children)
            {
                to_visit.push_back(child);
            }

            if (current->tag == "Xfm$Anim_S$")
            {
                parent = current->parent;
                animation_joint* new_joint = new animation_joint();
                new_joint->name = parent->name;
                ret->joints.insert(make_pair(parent->name, new_joint));
                for (panda_node* child : current->children)
                {
                    if (child->tag == "Scalar" && child->name == "fps")
                    {
                        new_joint->fps = child->fps;
                    }
                    else if (child->tag == "S$Anim")
                    {
                        char dof = child->name[0];
                        new_joint->positions.insert(make_pair(dof, child->V_values));
                    }
                    else if (child->tag == "Char*" && child->name == "order")
                    {
                        new_joint->order = child->order;
                    }
                }
            }

        }
        
        return ret;
    }

    string filter_out_tags(const string & inp)
    {
        string ret = "";
        size_t tag_open = inp.find('<', 0);
        if (tag_open == string::npos)
            return inp;

        size_t content_start = inp.find('{', tag_open+1) + 1;
        size_t content_end = content_start;
        int indentation = 1;
        while (indentation > 0)
        {
            if (inp[content_end] == '{')
                ++indentation;
            else if (inp[content_end] == '}')
                --indentation;
            ++content_end;
            if (content_end == inp.size()+1)
                return inp;
        }

        ret = string(inp, 0, tag_open) + string(inp, content_end);
        return filter_out_tags(ret);
    }
}
