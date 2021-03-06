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
#pragma once
#include "topaz.h"
#include "model.h"
#include "sqt.h"

namespace topaz
{
    class terrain : gameobject
    {
      public:
        terrain(u64 width, u64 height, float* data, float height_multiplier = 1.0f);
        ~terrain();

        void draw(const glm::mat4 & V, const glm::mat4 & P, camera* C);
        void paint(u64 x1, u64 y1, u64 x2, u64 y2, GLuint texture, float u1 = 0.0f, float v1 = 0.0f, float u2 = 1.0f, float v2 = 1.0f);
        void paint(u64 x1, u64 y1, u64 x2, u64 y2, float r, float g, float b, float a = 1.0f);
        void finalize();
        void set_scale(float new_scale);
      private:
        void fill_verticies(float* data);
        
        model* model_ptr;
        light* light_source;
        gl_program* light_program;
        sqt* transform;
        u64 width;
        u64 height;
        float height_multiplier;
    };
}
