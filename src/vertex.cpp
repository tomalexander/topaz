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
#include "vertex.h"
#include <glm/glm.hpp>

namespace topaz
{

    void print_vertex(vertex & vert, ostream & out)
    {
        out << "vertex\n";
        out << "  position: " << vert.x << " " << vert.y << " " << vert.z << "\n";
        out << "  normal: " << vert.nx << " " << vert.ny << " " << vert.nz << "\n";
        out << "  uv: " << vert.u << " " << vert.v << "\n";
        out << "  rgba: " << vert.r << " " << vert.g << " " << vert.b << " " << vert.a << "\n";
        out << "  joint indicies:";
        for (const int & cur : vert.joint_indicies)
            out << " " << cur;
        out << "\n";
        out << "  joint indicies:";
        for (const float & cur : vert.joint_membership)
            out << " " << cur;
        out << "\n";
        out << "\n";
    }

    vertex generate_vertex(const glm::vec3 & location)
    {
        vertex ret;
        ret.x = location.x;
        ret.y = location.y;
        ret.z = location.z;
        return ret;
    }
    vertex generate_vertex(float x, float y, float z)
    {
        vertex ret;
        ret.x = x;
        ret.y = y;
        ret.z = z;
        return ret;
    }
}
