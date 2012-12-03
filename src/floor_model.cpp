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
#include "floor_model.h"
#include "nolight.h"
#include "util.h"

namespace topaz
{

    floor_model::floor_model(const string & texture_name)
    {
        verticies = NULL;
        indicies = NULL;
        
        set_num_verticies(4);
        struct vertex v1 = {0.5,0,-0.5, 0,1,0, 1,1};
        verticies[0] = v1;
        struct vertex v2 = {0.5,0,0.5, 0,1,0, 1,0};
        verticies[1] = v2;
        struct vertex v3 = {-0.5,0,0.5, 0,1,0, 0,0};
        verticies[2] = v3;
        struct vertex v4 = {-0.5,0,-0.5, 0,1,0, 0,1};
        verticies[3] = v4;
        
        set_num_indicies(6);
        indicies[0] = 0;
        indicies[1] = 1;
        indicies[2] = 2;
        indicies[3] = 0;
        indicies[4] = 2;
        indicies[5] = 3;

        texture = load_texture(texture_name);

        move_to_gpu();
    }

    floor_model::~floor_model()
    {
        if (verticies != NULL)
            delete [] verticies;
        if (indicies != NULL)
            delete [] indicies;

        glDeleteBuffers(2, &buffers[1]);
        glDeleteVertexArrays(1, &buffers[0]);
    }
}
