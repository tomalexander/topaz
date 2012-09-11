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
#pragma once

#include "topaz.h"
#include "vertex.h"
#include "sqt.h"
#include <tuple>

using std::tuple;

namespace topaz
{
    class overlay : gameobject
    {
      public:
        overlay(float width, float height, GLuint texture);
        ~overlay();

        void move_to_gpu();
        void draw();
        void translateXY(float x, float y) {position.translateXYZ(2.0f*x, 2.0f*y, 0);}
        void scale(float s) {position.scale(s);}

        union
        {
            struct {
                GLuint vao;
                GLuint vbo;
                GLuint ibo;
            };
            GLuint buffers[3];
        };

      private:
        tuple<unsigned int, char*> arrange_vertex_memory();

        float width;            /**< Percent of screen width (0,1] */
        float height;           /**< percent of screen height (0,1] */
        GLuint texture;
        sqt position;

        vertex verticies[4];
        GLuint indicies[6];
        gl_program* program;
    };
}
