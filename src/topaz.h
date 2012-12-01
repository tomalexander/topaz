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
#ifndef TOPAZ_H_
#define TOPAZ_H_

#include <GL/glew.h>
#include "def.h"
#include "types.h"
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <string>
#include <vector>
#include <cstdarg>
#include <map>
#include <utility>
#include <iostream>
#include <list>
#include <functional>
#include "camera.h"
#include "gl_program.h"
#include "gameobject.h"
#include <glm/glm.hpp>

using std::map;
using std::string;
using std::vector;
using std::list;
using std::function;

namespace topaz
{
    class model;

    typedef void(*cleanup_func_ptr)(void);
    extern sf::Window* window;
    extern int window_width;
    extern int window_height;
    #ifndef LIBMAIN_CPP_
    extern model* sphere_model;
    #endif

    void init(char* argv0, int width = 800, int height = 600, const string & title = "Topaz Window");
    void cleanup();

    sf::Window* create_window(int width = 800, int height = 600, const string & title = "Topaz Window");
    void resize_window(int new_width = window_width, int new_height = window_height);

    gl_program* create_program(const string & program_name, const string & fragment_shader, const string & vertex_shader);
    gl_program* create_program(const string & program_name, const string & fragment_shader, const string & vertex_shader, const vector<string> & uniform_locations);
    gl_program* get_program(const string & program_name);

    void check_gl_error(const string & error_message, long line_number, const string & file_name);
    void game_loop(camera& C, glm::mat4& P);
    model* load_from_egg(const string & model_name, const std::initializer_list<string> & animation_names);
    model* load_from_egg(const string & model_name);
    void lua_init(const string & script_name = "main.lua");

    void add_event_handler(const function< bool(const sf::Event&)> & func, u8 priority = 128);
    void add_event_handler(unsigned long owner, const function< bool(const sf::Event&)> & func, u8 priority = 128);
    void add_begin_update_function(const function< void(int)> & func);
    void add_begin_update_function(unsigned long owner, const function< void(int)> & func);
    void add_pre_draw_function(const function< void(int)> & func);
    void add_pre_draw_function(unsigned long owner, const function< void(int)> & func);
    void add_post_draw_function(const function< void(int)> & func);
    void add_post_draw_function(unsigned long owner, const function< void(int)> & func);
    void add_draw_function(const function< void(glm::mat4&, glm::mat4&, camera*)> & func);
    void add_draw_function(unsigned long owner, const function< void(glm::mat4&, glm::mat4&, camera*)> & func);
    void add_cleanup_function(const function< void()> & func);
    void add_cleanup_function(unsigned long owner, const function< void()> & func);
    inline void remove_begin_update_handle(u64 owner);
    inline void remove_pre_draw_handle(u64 owner);
    inline void remove_post_draw_handle(u64 owner);
    inline void remove_draw_handle(u64 owner);
    inline void remove_cleanup_handle(u64 owner);
    inline void remove_event_handler(u64 owner);
    void remove_handles(unsigned long owner);
    void add_to_grim_reaper(gameobject* ob);
}
#define CHECK_GL_ERROR(X) topaz::check_gl_error(X, __LINE__, __FILE__)
#endif
