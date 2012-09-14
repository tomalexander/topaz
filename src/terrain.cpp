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
#include "terrain.h"

namespace topaz
{
    terrain::terrain(u64 width, u64 height, float* data) :
        light_source(nullptr),
        light_program(nullptr),
        model_ptr(nullptr),
        width(width),
        height(height)
    {
        transform = new sqt();
        fill_verticies(data);
        add_draw_function(id, std::bind(&topaz::terrain::draw, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    terrain::~terrain()
    {
        delete transform;
        delete model_ptr;
    }

    void terrain::draw(const matrix & V, const matrix & P, camera* C)
    {
        gl_program* program = light_program;
        if (light_program == NULL)
            program = model_ptr->model_program;
        model_ptr->prep_for_draw(transform->to_matrix(), V, P, C, program, light_source);
        
        model_ptr->draw();
    }

    void terrain::fill_verticies(float* data)
    {
        if (model_ptr != nullptr)
            delete model_ptr;
        model_ptr = new model();

        //Create verticies
        model_ptr->set_num_verticies(width*height);
        for (u64 x = 0; x < width; ++x)
        {
            for (u64 y = 0; y < height; ++y)
            {
                model_ptr->verticies[x*height+y].x = ((float)x) / ((float)(width-1));
                model_ptr->verticies[x*height+y].z = ((float)y) / ((float)(height-1));
                model_ptr->verticies[x*height+y].y = data[x*height+y];

                model_ptr->verticies[x*height+y].u = 0;
                model_ptr->verticies[x*height+y].v = 0;
            }
        }

        //Create indicies
        model_ptr->set_num_indicies(6 * (width-1) * (height-1));
        for (u64 x = 0; x < width-1; ++x)
        {
            for (u64 y = 0; y < height-1; ++y)
            {
                model_ptr->indicies[6*(x*(height-1)+y) + 0] = x*height+y;
                model_ptr->indicies[6*(x*(height-1)+y) + 1] = x*height+y+1;
                model_ptr->indicies[6*(x*(height-1)+y) + 2] = (x+1)*height+y;
                model_ptr->indicies[6*(x*(height-1)+y) + 3] = (x+1)*height+y;
                model_ptr->indicies[6*(x*(height-1)+y) + 5] = (x+1)*height+y+1;
                model_ptr->indicies[6*(x*(height-1)+y) + 4] = x*height+y+1;
            }
        }
    }

    void terrain::finalize()
    {
        //Move to GPU
        model_ptr->move_to_gpu();
    }

    /** 
     * Paint the terrain with a texture
     *
     * @param x1 An x-coordinate corresponding to the float grid coordinates
     * @param y1 A y-coordinate corresponding to the float grid coordinates
     * @param x2 An x-coordinate corresponding to the float grid coordinates
     * @param y2 A y-coordinate corresponding to the float grid coordinates
     * @param texture The texture to paint on the terrain
     * @param u1 A texture x-coordinate
     * @param v1 A texture y-coordinate
     * @param u2 A texture x-coordinate
     * @param v2 A texture y-coordinate
     */
    void terrain::paint(u64 x1, u64 y1, u64 x2, u64 y2, GLuint texture, float u1, float v1, float u2, float v2)
    {
        u64 min_x = (x1 < x2 ? x1 : x2);
        u64 min_y = (y1 < y2 ? y1 : y2);
        u64 max_x = (x1 > x2 ? x1 : x2);
        u64 max_y = (y1 > y2 ? y1 : y2);
        
        float min_u = (u1 < u2 ? u1 : u2);
        float min_v = (v1 < v2 ? v1 : v2);
        float max_u = (u1 > u2 ? u1 : u2);
        float max_v = (v1 > v2 ? v1 : v2);
        
        for (u64 x = min_x; x < max_x; ++x)
        {
            for (u64 y = min_y; y < max_y; ++y)
            {
                float x_percent = ((float)(x-min_x)) / ((float)(max_x-min_x));
                float y_percent = ((float)(y-min_y)) / ((float)(max_y-min_y));
                model_ptr->verticies[x*height+y].u = x_percent*(max_u-min_u) + min_u;
                model_ptr->verticies[x*height+y].v = y_percent*(max_v-min_v) + min_v;
            }
        }
        model_ptr->texture = texture;
    }

    void terrain::set_scale(float new_scale)
    {
        transform->scale(new_scale);
    }
}
