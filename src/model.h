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
#ifndef MODEL_H_
#define MODEL_H_

#include "topaz.h"
#include <map>
#include <utility>
#include <vector>
#include <string>
#include "vertex.h"
#include <iostream>
#include "light.h"
#include "joint.h"
#include "animation.h"
#include "gl_program.h"
#include <tuple>

using std::map;
using std::make_pair;
using std::vector;
using std::string;
using std::tuple;

namespace topaz
{
    class animation;
    class model
    {
      public:
        model();
        ~model();

        void set_num_verticies(size_t num);
        void set_num_indicies(GLuint num);

        void move_to_gpu();

        vertex* verticies;
        size_t num_verticies;
        GLuint* indicies;
        GLuint num_indicies;
        GLuint texture;

        gl_program* model_program;

        struct {
            bool uses_color : 1;
            bool has_texture : 1;
            bool has_joints : 1;
            bool has_normals : 1;
        };

        joint* root_joint;

        map<string, animation*> animations;

        void print(std::ostream & out = std::cout);
        void prep_for_draw(const matrix & M, const matrix & V, const matrix & P, camera* C, gl_program* program, light* light_source = NULL);
        void draw();

        union
        {
            struct {
                GLuint vao;
                GLuint vbo;
                GLuint ibo;
            };
            GLuint buffers[3];
        };

        void set_num_joints(int _num_joints);
        void set_num_joints_per_vertex(int _num_joints_per_vertex);
        int get_num_joints() {return num_joints;}
        tuple<unsigned int, char*> arrange_vertex_memory() const;
        void write_to_gnu_plot(ostream & out);

      private:
        int num_joints;
        int num_joints_per_vertex;

    };

}

#endif
