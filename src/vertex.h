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
#ifndef VERTEX_H_
#define VERTEX_H_

#include <iostream>
#include <string>
#include <vector>
#include <SFML/OpenGL.hpp>

using std::cout;
using std::string;
using std::ostream;
using std::vector;

namespace topaz
{
    class point;

    struct texture_data
    {
        float u, v;
        float opacity;
        GLuint texture_id;
    };

    struct vertex
    {
        union {
            struct {
                float x, y, z; //Position
            };
            float position[3];
        };
        union {
            struct {
                float nx, ny, nz; //Normal
            };
            float normal[3];
        };
        union{
            union {
                struct {
                    float u,v; //Texture Coordinates
                };
                float tex[2];
            };
            union {
                struct {
                    float r,g,b,a; // Color values
                };
                float rgb[4];
            };
        };
        vector<texture_data> multitex;
        vector<int> joint_indicies;
        vector<float> joint_membership;
    };

    void print_vertex(vertex & vert, ostream & out = cout);
    vertex generate_vertex(const point & location);
    vertex generate_vertex(float x, float y, float z);
}

#endif
