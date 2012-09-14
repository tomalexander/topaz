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
        model_ptr(nullptr)
    {
        transform = new sqt();
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

    
}
