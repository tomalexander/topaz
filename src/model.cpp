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
#include <glm/gtc/type_ptr.hpp>
#include "model.h"
#include "nolight.h"
#include "egg_parser.h"
#include "animation.h"
#include "vertex.h"
#include <unordered_map>
#include <cstring>
#include "print.h"

namespace std
{
   template <>
   struct hash<tuple<string, vector<string> >> : public unary_function<tuple<string, vector<string> >, size_t>
   {
       size_t operator()(const tuple<string, vector<string> >& v) const
       {
           hash<string> hash_fn;
           size_t ret = hash_fn(get<0>(v));
           for (const string & cur : get<1>(v))
               ret ^= hash_fn(cur);
           return ret;
       }
   };
}

namespace
{
    std::unordered_map<std::tuple<string, std::vector<string> >, topaz::model*> loaded_models;

    topaz::model* load_from_egg(const string & model_name)
    {
        #if PRINT_SHADERS == 1
        std::cout << "LOADING: " << model_name << "\n";
        #endif
        topaz::panda_node* model_egg = topaz::load_model("models/" + model_name + ".egg.txt");
        topaz::model* ret = topaz::panda_node::to_model(model_egg);
        delete model_egg;
        
        return ret;
    }

    topaz::model* load_from_egg(const string & model_name, const std::initializer_list<string> & animation_names)
    {
        #if PRINT_SHADERS == 1
        std::cout << "LOADING: " << model_name << "\n";
        #endif
        topaz::panda_node* model_egg = topaz::load_model("models/" + model_name + ".egg.txt");
        topaz::model* ret = topaz::panda_node::to_model(model_egg);
        delete model_egg;
        
        for (const string & cur : animation_names)
        {
            topaz::panda_node* animation_egg = topaz::load_animation("animations/" + cur + ".egg.txt");
            topaz::animation* ani = topaz::panda_node::to_animation(animation_egg);
            ret->animations.insert(make_pair(cur, ani));
        }

        return ret;
    }
}

namespace topaz
{
    topaz::gl_program* color_program = NULL;

    model::model()
    {
        verticies = NULL;
        indicies = NULL;
        has_texture = true;
        has_joints = false;
        has_normals = false;
        uses_color = false;
        model_program = NULL;
        root_joint = NULL;
        multitex = false;
        if (color_program == NULL)
            color_program = get_program(gl_program_id(0, 0, true, false, false, false, false));;
    }

    model::~model()
    {
        if (verticies != NULL)
            delete [] verticies;
        if (indicies != NULL)
            delete [] indicies;

        glDeleteBuffers(2, &buffers[1]);
        glDeleteVertexArrays(1, &buffers[0]);
        delete root_joint;
    }

    void model::set_num_verticies(size_t num)
    {
        verticies = new vertex[num];
        num_verticies = num;
    }

    void model::set_num_indicies(GLuint num)
    {
        indicies = new GLuint[num];
        num_indicies = num;
    }

    void model::move_to_gpu()
    {
        model_program = get_program(gl_program_id(num_joints, num_joints_per_vertex, uses_color, has_texture, has_joints, false, multitex));


        glGenVertexArrays(1, &vao);
        CHECK_GL_ERROR("Gen Vertex Array");
        glBindVertexArray(vao);
        CHECK_GL_ERROR("Bind Vertex Array");

        glGenBuffers(2, &vbo);
        CHECK_GL_ERROR("Generating Buffers");

        // tuple<unsigned int, char*> formatted_verticies = arrange_vertex_memory();
        unsigned int size_per_vertex;// = std::get<0>(formatted_verticies);
        char* vert_data;// = std::get<1>(formatted_verticies);
        std::tie(size_per_vertex, vert_data) = arrange_vertex_memory();

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        CHECK_GL_ERROR("Bind Buffer");
        glBufferData(GL_ARRAY_BUFFER, size_per_vertex*num_verticies, vert_data, GL_STATIC_DRAW);
        CHECK_GL_ERROR("Buffer Data");
        
        // GLint tmp = 0;
        // glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &tmp);
        // std::cout << "GL_MAX_VERTEX_ATTRIBS: " << tmp << "\n";

        unsigned int offset = 0;
        //Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, size_per_vertex, (GLvoid*)offset);
        glEnableVertexAttribArray(0);
        offset += 3*sizeof(float);
        CHECK_GL_ERROR("Attrib Pointer");
        if (has_normals)
        {
            //Normals
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, size_per_vertex, (GLvoid*)(offset));
            glEnableVertexAttribArray(2);
            offset += 3*sizeof(float);
            CHECK_GL_ERROR("Attrib Pointer");
        }

        if (uses_color) //RGBA
        {
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, size_per_vertex, (GLvoid*)(offset));
            glEnableVertexAttribArray(3);
            offset += 4*sizeof(float);
            CHECK_GL_ERROR("Color");
        }

        if (has_texture) //UV
        {
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, size_per_vertex, (GLvoid*)(offset));
            glEnableVertexAttribArray(1);
            offset += 2*sizeof(float);
            CHECK_GL_ERROR("Texture");
        }

        u8 current_index = 4;
        if (has_joints)
        {
            for (int x = 0; x < num_joints_per_vertex; x+=4)
            {
                if (x+4 > num_joints_per_vertex) //less than 4 remaining
                {
                    glVertexAttribIPointer(current_index, num_joints_per_vertex - x, GL_INT, size_per_vertex, (GLvoid*)(offset));
                    offset += (num_joints_per_vertex - x)*sizeof(int);
                } else {
                    glVertexAttribIPointer(current_index, 4, GL_INT, size_per_vertex, (GLvoid*)(offset));
                    offset += 4*sizeof(int);
                }
                glEnableVertexAttribArray(current_index++);
            }
            for (int x = 0; x < num_joints_per_vertex; x+=4)
            {
                if (x+4 > num_joints_per_vertex) //less than 4 remaining
                {
                    glVertexAttribPointer(current_index, num_joints_per_vertex - x, GL_FLOAT, GL_FALSE, size_per_vertex, (GLvoid*)(offset));
                    offset += (num_joints_per_vertex - x)*sizeof(float);
                } else {
                    glVertexAttribPointer(current_index, 4, GL_FLOAT, GL_FALSE, size_per_vertex, (GLvoid*)(offset));
                    offset += 4*sizeof(float);
                }
                
                glEnableVertexAttribArray(current_index++);
            }
        }
        CHECK_GL_ERROR("Joints");

        if (multitex)
        {
            size_t num_textures = num_textures = count_num_textures(verticies, num_verticies);
            glVertexAttribPointer(current_index++, 3, GL_FLOAT, GL_FALSE, size_per_vertex, (GLvoid*)(offset));
            offset += 3*sizeof(float);
            glVertexAttribIPointer(current_index++, 1, GL_INT, size_per_vertex, (GLvoid*)(offset));
            offset += 1*sizeof(GLuint);
        }
        CHECK_GL_ERROR("multitex");

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        CHECK_GL_ERROR("Bind Buffer");
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies[0])*num_indicies, indicies, GL_STATIC_DRAW);

        glBindVertexArray(0);
        free(vert_data);

    }

    tuple<unsigned int, char*> model::arrange_vertex_memory() const
    {
        size_t num_textures = 0;
        unsigned int size_per_vertex = 3*sizeof(float); /*position*/
        if (has_normals)
            size_per_vertex += 3*sizeof(float); /*normal*/
        if (uses_color)
            size_per_vertex += 4*sizeof(float); /*rgba*/
        if (has_texture)
            size_per_vertex += 2*sizeof(float); /*uv*/
        if (has_joints)
        {
            size_per_vertex += num_joints_per_vertex * (sizeof(int) + sizeof(float)); /*joint index and membership*/
        }
        if (multitex)
        {    
            num_textures = count_num_textures(verticies, num_verticies);
            size_per_vertex += num_textures * sizeof(texture_data);
        }

        char* data = (char*)malloc(size_per_vertex * num_verticies);
        
        for (int x = 0; x < num_verticies; ++x)
        {
            vertex* cur = &(verticies[x]);
            char* dest = data + size_per_vertex*x;
            char* original_dest = dest;
            //Copy Position
            memcpy(dest, &(cur->position[0]), 3*sizeof(float));
            dest += 3*sizeof(float);
            if (has_normals)
            {
                //Copy Normal
                memcpy(dest, &(cur->normal[0]), 3*sizeof(float));
                dest += 3*sizeof(float);
            }
            if (uses_color) //Copy RGBA
            {
                memcpy(dest, &(cur->rgb[0]), 4*sizeof(float));
                dest += 4*sizeof(float);
            }
            if (has_texture) //Copy UV
            {
                memcpy(dest, &(cur->tex[0]), 2*sizeof(float));
                dest += 2*sizeof(float);
            }
            if (has_joints)
            {
                char* dest_membership = dest + num_joints_per_vertex * sizeof(int);
                char* dest_indicies = dest;
                char* original_dest_membership = dest_membership;
                char* original_dest_indicies = dest_indicies;

                for (int y = 0; y < num_joints_per_vertex; ++y)
                {
                    if (y < cur->joint_indicies.size())
                    {
                        memcpy(dest_membership, &(cur->joint_membership[y]), sizeof(float));
                        dest_membership += sizeof(float);
                        memcpy(dest_indicies, &(cur->joint_indicies[y]), sizeof(int));
                        dest_indicies += sizeof(int);
                    } else {
                        float membership = 0.0f;
                        int index = 0;
                        memcpy(dest_membership, &(membership), sizeof(float));
                        dest_membership += sizeof(float);
                        memcpy(dest_indicies, &(index), sizeof(int));
                        dest_indicies += sizeof(int);
                    }
                    dest += sizeof(int) + sizeof(float);
                }
            }
            if (multitex)
            {
                memcpy(dest, &(cur->multitex[0]), num_textures * sizeof(texture_data));
                dest += num_textures * sizeof(texture_data);
            }
            if ((int)(dest - original_dest) != size_per_vertex)
                std::cout << "Actual size: " << (int)(dest - original_dest) << "\n";
        }
        return tuple<unsigned int, char*>(size_per_vertex, data);
    }

    void model::print(std::ostream & out)
    {
        out << "MODEL:" << '\n';
        out << "vao: " << vao << " " << buffers[0] << '\n';
        out << "vbo: " << vbo << " " << buffers[1] << '\n';
        out << "ibo: " << ibo << " " << buffers[2] << '\n';
        out << '\n';
        out << "Verticies:\n";
        for (size_t x = 0; x < num_verticies; ++x)
            out << "  " << verticies[x].x << "  " << verticies[x].y << "  " << verticies[x].z << "  " << verticies[x].nx << "  " << verticies[x].ny << "  " << verticies[x].nz << "  " << verticies[x].u << "  " << verticies[x].v << "  " << verticies[x].r << "  " << verticies[x].g << "  " << verticies[x].b << "  " << verticies[x].a << '\n';
        out << "\nIndicies:\n";
        for (GLuint x = 0; x < num_indicies; ++x)
            out << "  " << indicies[x] << ' ' << verticies[indicies[x]].x << "  " << verticies[indicies[x]].y << "  " << verticies[indicies[x]].z << "  " << verticies[indicies[x]].nx << "  " << verticies[indicies[x]].ny << "  " << verticies[indicies[x]].nz << "  " << verticies[indicies[x]].u << "  " << verticies[indicies[x]].v << '\n';
        out << '\n';
    }

    void model::prep_for_draw(const glm::mat4 & M, const glm::mat4 & V, const glm::mat4 & P, camera* C, gl_program* program, light* light_source)
    {
        static nolight nolight_tmp;
        glUseProgram(program->program_id);
        if (has_texture)
        {
            glUniform1i(program->uniform_locations["s_tex"], 0);
            if (light_source == NULL)
            {//Use nolight
                nolight_tmp.populate_uniforms(M, V, P, C, program);
            } else {
                light_source->populate_uniforms(M, V, P, C, program);
            }
        } else { //No texture
            glUniformMatrix4fv(program->uniform_locations["ModelMatrix"], 1, GL_FALSE, glm::value_ptr(M));
            CHECK_GL_ERROR("Filling model matrix");
            glUniformMatrix4fv(program->uniform_locations["ViewMatrix"], 1, GL_FALSE, glm::value_ptr(V));
            CHECK_GL_ERROR("Filling view matrix");
            glUniformMatrix4fv(program->uniform_locations["ProjectionMatrix"], 1, GL_FALSE, glm::value_ptr(P));
            CHECK_GL_ERROR("filling projection matrix");
            //nolight_tmp.populate_uniforms(M, V, P, C, color_program);
        }
        

    }

    void model::draw()
    {   
        if (has_texture)
            glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, num_indicies, GL_UNSIGNED_INT, (GLvoid*)0);
    }

    void model::set_num_joints(int _num_joints)
    {
        num_joints = _num_joints;
        has_joints = (num_joints != 0);
    }

    void model::set_num_joints_per_vertex(int _num_joints_per_vertex)
    {
        num_joints_per_vertex = _num_joints_per_vertex;
        has_joints = true;
    }

    /** 
     * Load a model from the model name, if it was already loaded return that
     *
     * @param model_name The model name without the file extension or path
     *
     * @return The model
     */
    model* get_model(const string & model_name)
    {
        static vector<string> no_animations;
        auto it = loaded_models.find(make_tuple(model_name, no_animations));
        if (it == loaded_models.end())
        {
            model* ret = load_from_egg(model_name);
            loaded_models.insert(make_pair(make_tuple(model_name, no_animations), ret));
            add_cleanup_function([ret]() {delete ret;});
            return ret;
        } else {
            return it->second;
        }
    }

    /** 
     * Load a model from the model name and list of animations. If it was already loaded then return that
     *
     * @param model_name The model name without the file extension or path
     * @param animation_names The animation names without file extension or path
     *
     * @return The model
     */
    model* get_model(const string & model_name, const std::initializer_list<string> & animation_names)
    {
        vector<string> animations(animation_names);
        std::sort(animations.begin(), animations.end());
        auto it = loaded_models.find(make_tuple(model_name, animations));
        if (it == loaded_models.end())
        {
            model* ret = load_from_egg(model_name, animation_names);
            loaded_models.insert(make_pair(make_tuple(model_name, animations), ret));
            add_cleanup_function([ret]() {delete ret;});
            return ret;
        } else {
            return it->second;
        }
    }

    void model::write_to_gnu_plot(std::ostream & out)
    {
        for (GLuint i = 0; i < num_indicies; i += 3)
        {
            for (GLuint index : {indicies[i], indicies[i+1], indicies[i+2], indicies[i]})
            {
                out << verticies[index].x << " " << verticies[index].y << " " << verticies[index].z << "\n";
            }
            out << "\n";
        }
    }
}



