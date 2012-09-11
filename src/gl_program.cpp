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
#include "def.h"
#include "gl_program.h"
#include <sstream>
#include "shaders.h"

using std::stringstream;

namespace topaz
{
    gl_program::gl_program()
    {
        uses_color = false;
        uses_texture = false;
        uses_joints = false;
        is_2d = false;
    }

    gl_program::~gl_program()
    {
        GLenum error_check_value = glGetError();
        for (GLuint cur : shader_ids)
        {
            glDetachShader(program_id, cur);
            glDeleteShader(cur);
        }
        glDeleteProgram(program_id);
        error_check_value = glGetError();
        if (error_check_value != GL_NO_ERROR)
        {
            std::cerr << "Could not destroy the shaders!\n";
        }
    }

    void gl_program::set_num_joints(int _num_joints)
    {
        num_joints = _num_joints;
        uses_joints = (num_joints != 0);
    }

    inline int gl_program::get_num_joint_vec4()
    {
        return ceil(((float)num_joints_per_vertex)/4.0f);
    }

    string gl_program::get_joint_index(int index)
    {
        stringstream ret;
        ret << "in_joint_indicies";
        ret << floor(((float)index)/4.0f);
        ret << "[" << index%4 << "]";
        return ret.str();
    }

    string gl_program::get_joint_membership(int index)
    {
        stringstream ret;
        ret << "in_joint_membership";
        ret << floor(((float)index)/4.0f);
        ret << "[" << index%4 << "]";
        return ret.str();
    }

    string gl_program::generate_vertex_shader(vector<string> & uniforms)
    {
        stringstream ret;
        ret << "#version 150\n#extension GL_ARB_explicit_attrib_location : enable\n\nlayout(location=0) in vec3 in_Position;\n";
        
        if (uses_color)
            ret << "layout(location=3) in vec4 in_Color;\n";
        if (uses_texture)
            ret << "layout(location=1) in vec2 in_Tex;\n";

        if (uses_joints)
        {
            int layout_loc = 4;
            int num_joint_vec4 = get_num_joint_vec4();
            for (int x = 0; x < num_joint_vec4; ++x)
            {
                ret << "layout(location=" << layout_loc + x << ") in ivec4 in_joint_indicies" << x << ";\n";
            }
            layout_loc += num_joint_vec4;
            for (int x = 0; x < num_joint_vec4; ++x)
            {
                ret << "layout(location=" << layout_loc + x << ") in vec4 in_joint_membership" << x << ";\n";
            }
        }

        ret << "uniform mat4 ModelMatrix;\n";
        uniforms.push_back("ModelMatrix");
        if (!is_2d)
        {
            ret << "uniform mat4 ViewMatrix;\nuniform mat4 ProjectionMatrix;\n";
            uniforms.push_back("ViewMatrix");
            uniforms.push_back("ProjectionMatrix");
        }

        if (uses_joints)
        {
            ret << "uniform mat4 joints[" << num_joints << "];\n";
            uniforms.push_back("joints");
        }

        if (uses_color)
            ret << "out vec4 v_Color;\n";
        if (uses_texture)
            ret << "out vec2 v_tex;\n";
        ret << "void main( void )\n{\n    mat4 mvp_mat = ";
        if (!is_2d)
            ret << "ProjectionMatrix * ViewMatrix * ";
        ret << "ModelMatrix;\n";

        if (uses_joints)
        {
            for (int x = 0; x < num_joints_per_vertex; ++x)
            {
                ret << "    vec4 joint" << x << " = (joints[" << get_joint_index(x) << "] * vec4(in_Position, 1.0)) * " << get_joint_membership(x) << ";\n";
            }
            ret << "    vec4 v_prime = ";
            for (int x = 0; x < num_joints_per_vertex; ++x)
            {
                ret << (x == 0 ? "" : " + ") << "joint" << x;
            }
            ret << ";\n    v_prime.w = 1.0;\n";
        }

        if (uses_color)
            ret << "    v_Color = in_Color;\n";
        if (uses_texture)
            ret << "    v_tex = in_Tex;\n";

        ret << "    gl_Position = mvp_mat";
        if (uses_joints)
            ret << " * v_prime";
        else
            ret << " * vec4(in_Position, 1.0)";
        ret << ";\n}\n";

        return ret.str();
    }

    string gl_program::generate_fragment_shader(vector<string> & uniforms)
    {
        stringstream ret;
        ret << "#version 150\n";
        
        if (uses_color)
            ret << "in vec4 v_Color;\n";
        if (uses_texture)
            ret << "in vec2 v_tex;\n";

        if (uses_texture)
        {
            ret << "uniform sampler2D s_tex;\n";
            uniforms.push_back("s_tex");
        }
        if (!uses_color && !uses_texture)
        {
            ret << "uniform vec4 RGBA;\n";
            uniforms.push_back("RGBA");
        }

        ret << "out vec4 FragColor;\nvoid main(void)\n{\n";

        if (uses_color)
            ret << "	FragColor = v_Color;\n";
        if (uses_texture)
            ret << "    FragColor = texture(s_tex, v_tex);\n";
        if (!uses_color && !uses_texture)
            ret << "	FragColor = RGBA;\n";
        
        ret << "}\n";
        return ret.str();
    }

    void gl_program::create_program()
    {
        vector<string> uniforms;
        program_id = glCreateProgram();
        GLuint fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
        GLuint vertex_id = glCreateShader(GL_VERTEX_SHADER);
        
        string vertex_shader = generate_vertex_shader(uniforms);
        string fragment_shader = generate_fragment_shader(uniforms);
        #if PRINT_SHADERS == 1
        std::cout << "---------- Vertex Shader ----------\n" << vertex_shader << "\n";
        std::cout << "---------- Fragment Shader ----------\n" << fragment_shader << "\n";
        #endif
        const char* _vertex_shader = vertex_shader.c_str();
        const char* _fragment_shader = fragment_shader.c_str();

        glShaderSource(fragment_id, 1, &(_fragment_shader), NULL);
        glCompileShader(fragment_id);

        glShaderSource(vertex_id, 1, &(_vertex_shader), NULL);
        glCompileShader(vertex_id);

        check_shader(fragment_id);
        check_shader(vertex_id);

        //Link all the things
        glAttachShader(program_id, fragment_id);
        glAttachShader(program_id, vertex_id);
        glLinkProgram(program_id);
        CHECK_GL_ERROR("Linking Program");

        //Use the program
        glUseProgram(program_id);
        CHECK_GL_ERROR("Use Program");

        //Add the IDs to array so they get deleted in cleanup
        shader_ids.push_back(fragment_id);
        shader_ids.push_back(vertex_id);

        for (const string & cur : uniforms)
        {
            GLuint uniform_loc = glGetUniformLocation(program_id, cur.c_str());
            uniform_locations.insert(make_pair(cur, uniform_loc));
            CHECK_GL_ERROR("Getting Uniform Location for " + cur);
        }
    }

    void check_shader(GLuint shader)
    {
        GLint status;
        static int log_length;
        static char buffer[1000];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        
        if (status != GL_FALSE)
            return;
        
        glGetShaderInfoLog(shader, sizeof(buffer), &log_length, buffer);
        std::cerr << "Shader Failed Compilation!\n" << buffer;
        exit(1);
    }
}







