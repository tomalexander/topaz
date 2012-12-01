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
#include "coordinate_system.h"
#include "panda_node.h"
#include <sstream>

using std::stringstream;

namespace topaz
{
    extern glm::mat4 fix_z_up_matrix;

    void assign_coordinate_system(panda_node* node, coordinate_system system)
    {
        node->coordinate_system = system;
        for (panda_node* child : node->children)
            assign_coordinate_system(child, system);
    }
    
    coordinate_system detect_coordinate_system(panda_node* node)
    {
        if (node->coordinate_system != UNASSIGNED)
            return node->coordinate_system;
        std::list<panda_node*> queue;
        queue.push_back(node);

        while (!queue.empty())
        {
            panda_node* current = queue.front();
            queue.pop_front();
            for (panda_node* child : current->children)
                queue.push_back(child);

            if (current->tag == "CoordinateSystem")
            {
                std::cout << "CONTENT:" << current->content << "\n";
                if (current->content == "Z-Up")
                {
                    assign_coordinate_system(node, ZUP);
                    return ZUP;
                } else if (current->content == "Y-Up") {
                    assign_coordinate_system(node, YUP);
                    return YUP;
                }
            }
        }
        assign_coordinate_system(node, ZUP);
        return ZUP;
    }

    void fix_coordinate_system(panda_node* top)
    {
        std::list<panda_node*> queue;
        queue.push_back(top);

        while (!queue.empty())
        {
            panda_node* node = queue.front();
            queue.pop_front();
            for (panda_node* child : node->children)
                queue.push_back(child);
        
            coordinate_system system = detect_coordinate_system(node);
            if (system == ZUP)
                std::cout << "ZUP DETECTED\n";
            else
                std::cout << "YUP DETECTED\n";
            if (system == ZUP && (node->tag == "Vertex" || node->tag == "Normal"))
            {
                float x,y,z;
                stringstream tmp(node->content);
                tmp >> x >> y >> z;
            
                stringstream out;
                out << x << " " << z << " " << -y;
            
                node->content = out.str();
            } else if (node->tag == "VertexRef") {
                //Reverse Order
                // vector<u32> indicies;
                // u32 tmp;
                // stringstream content(node->content);
                // while (content >> tmp)
                // {
                //     indicies.push_back(tmp);
                // }
                // stringstream out;
                // while (!indicies.empty())
                // {
                //     out << indicies.back() << " ";
                //     indicies.pop_back();
                // }
                // node->content = out.str();
            } else if (node->tag == "Matrix4") {
                stringstream tmp(node->content);
                glm::mat4 mat(1.0f);
                for (int i = 0; i < 16; ++i)
                {
                    float x;
                    tmp >> x;
                    mat[i%4][i/4] = x;
                }
                mat = glm::transpose(mat);
                if (system == ZUP)
                    mat = fix_z_up_matrix * mat;
                stringstream out;
                for (int i = 0; i < 16; ++i)
                {
                    out << mat[i%4][i/4] << " ";
                }
                node->content = out.str();
            }
        }
    }
}
