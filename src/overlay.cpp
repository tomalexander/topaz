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
#include "overlay.h"
#include <cstring>
#include <glm/gtc/type_ptr.hpp>
namespace topaz
{
    overlay::overlay(float width, float height, GLuint texture) :
        width(width),
        height(height),
        texture(texture)
    {
        verticies[0].x = 0;
        verticies[0].y = 0;
        verticies[0].z = 0;
        verticies[0].u = 0;
        verticies[0].v = 0;

        verticies[1].x = width;
        verticies[1].y = 0;
        verticies[1].z = 0;
        verticies[1].u = 1;
        verticies[1].v = 0;

        verticies[2].x = width;
        verticies[2].y = height;
        verticies[2].z = 0;
        verticies[2].u = 1;
        verticies[2].v = 1;

        verticies[3].x = 0;
        verticies[3].y = height;
        verticies[3].z = 0;
        verticies[3].u = 0;
        verticies[3].v = 1;

        indicies[0] = 0;
        indicies[1] = 1;
        indicies[2] = 2;
        indicies[3] = 0;
        indicies[4] = 2;
        indicies[5] = 3;

        position.translateXYZ(-1,-1,0);
        position.scale(2);
        move_to_gpu();
        add_draw_function(id, std::bind(&topaz::overlay::draw, this));
    }

    overlay::~overlay()
    {
        glDeleteBuffers(2, &buffers[1]);
        glDeleteVertexArrays(1, &buffers[0]);
    }

    void overlay::move_to_gpu()
    {
        std::cout << "Making Overlay Shader\n";
        program = get_program(gl_program_id(0, 0, false, true, false, true, false));

        glGenVertexArrays(1, &vao);
        CHECK_GL_ERROR("Gen Vertex Array");
        glBindVertexArray(vao);
        CHECK_GL_ERROR("Bind Vertex Array");

        glGenBuffers(2, &vbo);
        CHECK_GL_ERROR("Generating Buffers");

        unsigned int size_per_vertex;
        char* vert_data;
        std::tie(size_per_vertex, vert_data) = arrange_vertex_memory();

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        CHECK_GL_ERROR("Bind Buffer");
        glBufferData(GL_ARRAY_BUFFER, size_per_vertex*4, vert_data, GL_STATIC_DRAW);
        CHECK_GL_ERROR("Buffer Data");
        

        unsigned int offset = 0;
        //Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, size_per_vertex, (GLvoid*)offset);
        glEnableVertexAttribArray(0);
        offset += 3*sizeof(float);
        CHECK_GL_ERROR("Attrib Pointer");
        //UV
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, size_per_vertex, (GLvoid*)(offset));
        glEnableVertexAttribArray(1);
        offset += 2*sizeof(float);
        CHECK_GL_ERROR("Texture");

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        CHECK_GL_ERROR("Bind Buffer");
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies[0])*6, indicies, GL_STATIC_DRAW);

        glBindVertexArray(0);
        free(vert_data);
    }

    tuple<unsigned int, char*> overlay::arrange_vertex_memory()
    {
        unsigned int size_per_vertex = 3*sizeof(float); /*position*/
        size_per_vertex += 2*sizeof(float); /*uv*/
        char* data = (char*)malloc(size_per_vertex * 4);
        
        for (int x = 0; x < 4; ++x)
        {
            vertex* cur = &(verticies[x]);
            char* dest = data + size_per_vertex*x;
            //Copy Position
            memcpy(dest, &(cur->position[0]), 3*sizeof(float));
            dest += 3*sizeof(float);
            //Copy UV
            memcpy(dest, &(cur->tex[0]), 2*sizeof(float));
            dest += 2*sizeof(float);
            
        }
        return tuple<unsigned int, char*>(size_per_vertex, data);
    }

    void overlay::draw()
    {
        glUseProgram(program->program_id);
        glUniform1i(program->uniform_locations["s_tex"], 0);
        glUniformMatrix4fv(program->uniform_locations["ModelMatrix"], 1, GL_FALSE, glm::value_ptr(position.to_matrix()));
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)0);
    }
}
