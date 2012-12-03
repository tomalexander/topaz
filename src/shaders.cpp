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
#include "shaders.h"
#include "physfs.h"
#include "topaz.h"
#include <vector>
#include "util.h"
#include <iostream>
#include <map>

using std::vector;


namespace topaz
{
    map<string, gl_program*> shader_ids;

    /** 
     * Create a shader program from a fragment and vertex shader
     *
     * @param program_name name of the program used as a key for the map
     * @param fragment_shader name of fragment shader file
     * @param vertex_shader name of vertex shader file
     * 
     * @return pointer to generated gl_program
     */
    gl_program* create_program(const string & program_name, const string & fragment_shader, const string & vertex_shader)
    {
        vector<string> tmp;
        return create_program(program_name, fragment_shader, vertex_shader, tmp);
    }

    /** 
     * Create a shader program from a fragment and vertex shader
     *
     * @param program_name name of the program used as a key for the map
     * @param fragment_shader name of fragment shader file
     * @param vertex_shader name of vertex shader file
     * @param uniform_locations vector of names of variables to get uniform locations for
     *
     * @return pointer to generated gl_program
     */
    gl_program* create_program(const string & program_name, const string & fragment_shader, const string & vertex_shader, const vector<string> & uniform_locations)
    {
        GLenum error_check_value = glGetError();

        GLuint program_id = glCreateProgram();
        CHECK_GL_ERROR("Create Program");
        GLuint fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
        GLuint vertex_id = glCreateShader(GL_VERTEX_SHADER);
        CHECK_GL_ERROR("Create Shader");
        
        //Read fragment shader
        long fragment_shader_length;
        const GLchar* fragment_shader_content = (GLchar*) read_fully_string(("shaders/" + fragment_shader).c_str(), fragment_shader_length);

        //Read vertex shader
        long vertex_shader_length;
        const GLchar* vertex_shader_content = (GLchar*) read_fully_string(("shaders/" + vertex_shader).c_str(), vertex_shader_length);

        //Compile Shaders
        glShaderSource(fragment_id, 1, &(fragment_shader_content), NULL);
        glCompileShader(fragment_id);

        glShaderSource(vertex_id, 1, &(vertex_shader_content), NULL);
        glCompileShader(vertex_id);

        check_shader(fragment_id);
        check_shader(vertex_id);

        //Link Everything!
        glAttachShader(program_id, fragment_id);
        glAttachShader(program_id, vertex_id);
        glLinkProgram(program_id);
        CHECK_GL_ERROR("Linking Program");

        //Use the program
        glUseProgram(program_id);
        CHECK_GL_ERROR("Use Program");
        
        //Error Checking
        error_check_value = glGetError();
        if (error_check_value != GL_NO_ERROR)
        {
            std::cerr << "Error: Could not create the shaders" << std::endl;
        }

        //Add the IDs to a gl_program so that they get deleted in cleanup
        gl_program* new_program = new gl_program();
        new_program->program_id = program_id;
        new_program->shader_ids.push_back(fragment_id);
        new_program->shader_ids.push_back(vertex_id);
        shader_ids.insert(make_pair(program_name, new_program));

        //Get uniform locations
        for (const string & cur : uniform_locations)
        {
            GLuint uniform_loc = glGetUniformLocation(program_id, cur.c_str());
            new_program->uniform_locations.insert(make_pair(cur, uniform_loc));
            CHECK_GL_ERROR("Getting Uniform Location for " + cur);
        }


        //Clean up memory
        delete [] fragment_shader_content;
        delete [] vertex_shader_content;

        return new_program;
    }

    gl_program* get_program(const string & program_name)
    {
        auto ret_iterator = shader_ids.find(program_name);
        if (ret_iterator == shader_ids.end())
            return NULL;

        return ret_iterator->second;
    }
}
