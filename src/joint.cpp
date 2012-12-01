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
#include "joint.h"
#include "panda_node.h"
#include <list>
#include <utility>
#include <glm/gtc/type_ptr.hpp>
#include "print.h"

using std::list;
using std::make_pair;
using std::pair;

namespace topaz
{
    joint::joint()
    {
        parent = NULL;
    }

    joint::joint(const string & name, unsigned int & available_index_in_shader) : name(name)
    {
        parent = NULL;
        if (name != "_ROOT")
        {
            index_in_shader = available_index_in_shader++;
        }
    }

    joint::~joint()
    {
        for (pair<string, joint*> child : joints)
        {
            delete child.second;
        }
    }

    void joint::push_binding_down()
    {
        if (name != "_ROOT")
        {
            binding = parent->binding * binding;
        } else {
            binding = glm::mat4(1.0f);
        }
        for (pair<string, joint*> element : joints)
        {
            element.second->push_binding_down();
        }
        inverse_binding = glm::inverse(binding);
    }

    void joint::add_child(joint* child)
    {
        joints.insert(make_pair(child->name, child));
        child->parent = this;
        child->transform.parent = &transform;
    }

    void joint::update(unsigned int & animation_progress, animation* current_animation)
    {
        if (name != "_ROOT")
        {
            //Compute local matrix
            // local = rotation.to_matrix();
            // local(3,0) = translation.x();
            // local(3,1) = translation.y();
            // local(3,2) = translation.z();
            local = glm::mat4(1.0f);
            if (current_animation != NULL)
            {
                // local = binding * local;
                current_animation->apply(animation_progress, this);
            } else {
                local = binding * local;
            }

            
            //Compute world matrix
            world = parent->world * local;
            world = world * inverse_binding;
        }

        //Recursively call update on all children
        for (pair<string, joint*> element : joints)
        {
            element.second->update(animation_progress, current_animation);
        }

        //factor in B^-1
        if (name != "_ROOT")
        {
            //world = world * inverse_binding;
        }
        //world.print();
    }

    void joint::populate_float_array(float* joint_matricies)
    {
        if (name != "_ROOT")
        {
            size_t start_index = index_in_shader*16;
            memcpy(&(joint_matricies[start_index]), glm::value_ptr(world), sizeof(float)*16);
        }

        for (pair<string, joint*> child : joints)
        {
            child.second->populate_float_array(joint_matricies);
        }
    }

    joint* joint::find_joint_indexed_in_shader(int index)
    {
        if (name != "_ROOT" && index_in_shader == index)
        {
            return this;
        }

        for (pair<string, joint*> child : joints)
        {
            joint* ret = child.second->find_joint_indexed_in_shader(index);
            if (ret != NULL)
                return ret;
        }
        return NULL;
    }

    joint* joint::find_joint_name(const string & _name)
    {
        joint* search_down = _find_joint_name(_name);
        if (search_down != NULL)
            return search_down;

        joint* root = find_root();
        return root->_find_joint_name(_name);
    }

    joint* joint::_find_joint_name(const string & _name)
    {
        if (_name == name)
            return this;

        for (pair<string, joint*> child : joints)
        {
            joint* ret = child.second->_find_joint_name(_name);
            if (ret != NULL)
                return ret;
        }
        return NULL;
    }

    joint* joint::find_root()
    {
        if (parent == NULL)
            return this;
        return parent->find_root();
    }

    

    void joint::print(int indentation, ostream & out)
    {
        out << string(indentation*4, ' ') << "Joint " << name << "\n";
        out << string(indentation*4, ' ') << "Index in shader " << index_in_shader << "\n";
        out << string(indentation*4, ' ') << "Rotation:\n";
        topaz::print(transform.q, out, indentation+1);
        out << string(indentation*4, ' ') << "Translation:\n";
        topaz::print(transform.t, out, indentation+1);
        
        for (pair<const int, float> & entry : membership)
        {
            out << string(indentation*4, ' ') << "  " << entry.first << " " << entry.second << "\n";
        }
        out << "\n";
    }

    void joint::recursive_print(int indentation, ostream & out)
    {
        print(indentation, out);
        for (pair<string, joint*> entry : joints)
        {
            entry.second->recursive_print(indentation+1, out);
        }
    }

    void joint::check_if_associated(const int vert_index, map<int, float> & joint_association)
    {
        map<int, float>::iterator pos = membership.find(vert_index);
        if (pos != membership.end())
        {
            joint_association.insert(make_pair(index_in_shader, pos->second));
        }
        for (pair<string, joint*> child : joints)
        {
            child.second->check_if_associated(vert_index, joint_association);
        }
    }

    joint* generate_joint(panda_node* inp, const map<int,int> & vertex_id_to_array_index, unsigned int & available_index_in_shader)
    {
        joint* ret = new joint(inp->name, available_index_in_shader);
        
        
        ret->binding = inp->mat/*.inverse()*/;

        // ret->rotation = ret->transform.to_quaternion();
        // ret->translation.x() = ret->transform(3,0);
        // ret->translation.y() = ret->transform(3,1);
        // ret->translation.z() = ret->transform(3,2);
        // ret->local = inp->mat;
        
        list<panda_node*> to_visit;
        to_visit.push_back(inp);
        while (!to_visit.empty())
        {
            panda_node* current = to_visit.front();
            to_visit.pop_front();
            if (current != inp && current->tag == "Joint")
            {
                //child joint
                joint* child_joint = generate_joint(current, vertex_id_to_array_index, available_index_in_shader);
                ret->add_child(child_joint);
            }
            else if (current->tag == "VertexRef")
            {
                //membership group

                float member = 0.0f;
                //get membership float
                for (panda_node* child : current->children)
                {
                    if (child->name == "membership")
                    {
                        member = child->membership;
                        break;
                    }
                }

                for (int ref : current->vertex_refs)
                {
                    int adjusted = vertex_id_to_array_index.find(ref)->second;
                    //std::cout << "Adding " << ref << " (" << adjusted << ") at membership " << member << "\n";
                    ret->membership.insert(make_pair(adjusted, member));
                }
            }
            else {
                for (panda_node* child : current->children)
                {
                    to_visit.push_back(child);
                }
            }
        }

        return ret;
    }

    joint* duplicate_joint_tree(joint* current)
    {
        joint* new_joint = new joint();
        new_joint->name = current->name;
        new_joint->binding = current->binding;
        new_joint->inverse_binding = current->inverse_binding;
        new_joint->local = current->local;
        new_joint->world = current->world;
        new_joint->membership = current->membership;
        new_joint->index_in_shader = current->index_in_shader;

        for (pair<string, joint*> cur : current->joints)
        {
            joint* child = duplicate_joint_tree(cur.second);
            new_joint->add_child(child);
        }
        return new_joint;
    }

    unsigned int joint::size() const
    {
        unsigned int ret = 1;
        for (const pair<string, joint*> & child : joints)
        {
            ret += child.second->size();
        }
        return ret;
    }
}
