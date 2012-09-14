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
            for (u64 y = 0; y < height; ++x)
            {
                model_ptr->verticies[x*height+y].x = ((float)x) / ((float)width);
                model_ptr->verticies[x*height+y].z = ((float)y) / ((float)height);
                model_ptr->verticies[x*height+y].y = data[x*height+y];

                model_ptr->verticies[x*height+y].u = 0;
                model_ptr->verticies[x*height+y].v = 0;
            }
        }

        //Create indicies
        model_ptr->set_num_indicies(6 * (width-1) * (height-1));
        for (u64 x = 0; x < width-1; ++x)
        {
            for (u64 y = 0; y < height-1; ++x)
            {
                model_ptr->indicies[6*(x*(height-1)+y) + 0] = x*height+y;
                model_ptr->indicies[6*(x*(height-1)+y) + 1] = x*height+y+1;
                model_ptr->indicies[6*(x*(height-1)+y) + 2] = (x+1)*height+y;
                model_ptr->indicies[6*(x*(height-1)+y) + 3] = (x+1)*height+y;
                model_ptr->indicies[6*(x*(height-1)+y) + 4] = (x+1)*height+y+1;
                model_ptr->indicies[6*(x*(height-1)+y) + 5] = x*height+y+1;
            }
        }

        //Move to GPU
        model_ptr->move_to_gpu();
    }
}
