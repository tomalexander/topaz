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
#ifndef GL_PROGRAM_H_
#define GL_PROGRAM_H_

#include <map>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <iostream>
#include "types.h"

using std::map;
using std::vector;
using std::string;

namespace topaz
{
    class gl_program
    {
      public:
        gl_program();
        ~gl_program();
        
        GLuint program_id;
        vector<GLuint> shader_ids;
        map<string, GLuint> uniform_locations;

        struct {
            bool uses_color : 1;
            bool uses_texture : 1;
            bool uses_joints : 1;
            bool is_2d : 1;
            bool multitex : 1;
        };

        int num_joints;
        int num_joints_per_vertex;
        u8 num_textures;

        void set_num_joints(int _num_joints);
        string generate_vertex_shader(vector<string> & uniforms);
        string generate_fragment_shader(vector<string> & uniforms);
        void create_program();
        int get_num_joint_vec4();
        string get_joint_index(int index);
        string get_joint_membership(int index);
    };

    void check_shader(GLuint shader);
}
#endif
