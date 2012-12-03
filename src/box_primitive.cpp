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
#include "topaz.h"
#include "box_primitive.h"
#include "topaz.h"
#include "camera.h"
#include "model.h"

namespace topaz
{
    box_primitive::box_primitive(sqt* _parent_transform, const glm::vec4 & _color, const glm::vec3 & _lesser_corner, const glm::vec3 & _greater_corner)
    {
        transform = new sqt(_parent_transform);
        color = _color;
        lesser_corner = _lesser_corner;
        greater_corner = _greater_corner;
        generate_model(_lesser_corner, _greater_corner);
        add_draw_function(id, std::bind(&topaz::box_primitive::draw, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    box_primitive::~box_primitive()
    {
        delete transform;
        delete mod;
    }

    void box_primitive::draw(const glm::mat4 & V, const glm::mat4 & P, camera* C)
    {
        glm::mat4 M = transform->to_matrix();
        mod->prep_for_draw(M, V, P, C, mod->model_program, NULL);
        glUniform4fv(mod->model_program->uniform_locations["RGBA"], 1, &(color[0]));
        mod->draw();
    }

    void box_primitive::generate_model(const glm::vec3 & _lesser_corner, const glm::vec3 & _greater_corner)
    {
        mod = new model();
        
        mod->set_num_verticies(8);
        mod->set_num_indicies(36);
        mod->uses_color = false;
        mod->has_texture = false;
        mod->has_joints = false;
        mod->has_normals = false;
        
        mod->verticies[0] = generate_vertex(lesser_corner);
        mod->verticies[1] = generate_vertex(greater_corner.x, lesser_corner.y, lesser_corner.z);
        mod->verticies[2] = generate_vertex(lesser_corner.x, greater_corner.y, lesser_corner.z);
        mod->verticies[3] = generate_vertex(greater_corner.x, greater_corner.y, lesser_corner.z);
        mod->verticies[4] = generate_vertex(lesser_corner.x, lesser_corner.y, greater_corner.z);
        mod->verticies[5] = generate_vertex(greater_corner.x, lesser_corner.y, greater_corner.z);
        mod->verticies[6] = generate_vertex(lesser_corner.x, greater_corner.y, greater_corner.z);
        mod->verticies[7] = generate_vertex(greater_corner);

        mod->indicies[0] = 0;
        mod->indicies[1] = 2;
        mod->indicies[2] = 1;

        mod->indicies[3] = 1;
        mod->indicies[4] = 2;
        mod->indicies[5] = 3;

        mod->indicies[6] = 0;
        mod->indicies[7] = 6;
        mod->indicies[8] = 2;

        mod->indicies[9] = 0;
        mod->indicies[10] = 4;
        mod->indicies[11] = 6;

        mod->indicies[12] = 1;
        mod->indicies[13] = 3;
        mod->indicies[14] = 7;

        mod->indicies[15] = 7;
        mod->indicies[16] = 5;
        mod->indicies[17] = 1;

        mod->indicies[18] = 0;
        mod->indicies[19] = 1;
        mod->indicies[20] = 5;

        mod->indicies[21] = 0;
        mod->indicies[22] = 5;
        mod->indicies[23] = 4;

        mod->indicies[24] = 2;
        mod->indicies[25] = 7;
        mod->indicies[26] = 3;

        mod->indicies[27] = 2;
        mod->indicies[28] = 6;
        mod->indicies[29] = 7;

        mod->indicies[30] = 4;
        mod->indicies[31] = 5;
        mod->indicies[32] = 7;

        mod->indicies[33] = 4;
        mod->indicies[34] = 7;
        mod->indicies[35] = 6;

        mod->move_to_gpu();
    }

    

}
