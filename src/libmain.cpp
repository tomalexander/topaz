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
#define LIBMAIN_CPP_
#include "physfs.h"
#include "topaz.h"
#include <stdio.h>
#include "util.h"
#include "egg_parser.h"
#include "matrix.h"
#include "shaders.h"
#include <unordered_map>
#include <sstream>
#include "gl_program.h"
#include <oolua/oolua.h>
#include "lua_demo.h"

using std::unordered_map;
using std::stringstream;

namespace topaz
{
    sf::Window* window = NULL;
    int window_width;
    int window_height;
    extern vector<gl_program*> shader_ids;

    unordered_map<string, GLuint> textures;
    void init_glew();

    list<pair<unsigned long, function< bool(const sf::Event&)> > > event_handlers;
    list<pair<unsigned long, function< void(int)> > > begin_update_functions;
    list<pair<unsigned long, function< void(int)> > > pre_draw_functions;
    list<pair<unsigned long, function< void(int)> > > post_draw_functions;
    list<pair<unsigned long, function< void(matrix&, matrix&, camera*)> > > draw_functions;
    list<pair<unsigned long, function< void()> > > cleanup_functions;
    vector<gameobject*> grim_reaper_list;

    model* sphere_model = NULL;

    void init(char* argv0, int width, int height, const string & title)
    {
        

        //Initialize PhysFS
        PHYSFS_init(argv0);

        PHYSFS_setSaneConfig("topaz","topaz","tpz",0,0);

        string up_one_dir(get_up_one_dir(PHYSFS_getBaseDir()));
        PHYSFS_mount(up_one_dir.c_str(), "", 1);
        string sep(PHYSFS_getDirSeparator());
        string share_dir = up_one_dir + "share" + sep + "topaz" + sep;
        if (path_exists(share_dir.c_str()))
            PHYSFS_mount(share_dir.c_str(), "", 1);

        //Create the window
        create_window(width, height, title);

        //Init Lua
        lua_init();

        sphere_model = load_from_egg("sphere");

        //Load Shaders
        // vector<string> uberlight_shader_uniforms;
        // uberlight_shader_uniforms.push_back("LightColor");
        // uberlight_shader_uniforms.push_back("LightWeights");
        // uberlight_shader_uniforms.push_back("SurfaceWeights");
        // uberlight_shader_uniforms.push_back("SurfaceRoughness");
        // uberlight_shader_uniforms.push_back("AmbientClamping");
        // uberlight_shader_uniforms.push_back("BarnShaping");
        // uberlight_shader_uniforms.push_back("SeWidth");
        // uberlight_shader_uniforms.push_back("SeHeight");
        // uberlight_shader_uniforms.push_back("SeWidthEdge");
        // uberlight_shader_uniforms.push_back("SeHeightEdge");
        // uberlight_shader_uniforms.push_back("SeRoundness");
        // uberlight_shader_uniforms.push_back("DsNear");
        // uberlight_shader_uniforms.push_back("DsFar");
        // uberlight_shader_uniforms.push_back("DsNearEdge");
        // uberlight_shader_uniforms.push_back("DsFarEdge");
        // uberlight_shader_uniforms.push_back("WCLightPos");
        // uberlight_shader_uniforms.push_back("ViewPosition");
        // uberlight_shader_uniforms.push_back("WCtoLC");
        // uberlight_shader_uniforms.push_back("WCtoLCit");
        // uberlight_shader_uniforms.push_back("MCtoWC");
        // uberlight_shader_uniforms.push_back("MCtoWCit");
        // uberlight_shader_uniforms.push_back("s_tex");
        // uberlight_shader_uniforms.push_back("ModelMatrix");
        // uberlight_shader_uniforms.push_back("ViewMatrix");
        // uberlight_shader_uniforms.push_back("ProjectionMatrix");
        // create_program("uberlight", "uberlight.fs", "uberlight.vs", uberlight_shader_uniforms);
        // vector<string> nolight_shader_uniforms;
        // nolight_shader_uniforms.push_back("s_tex");
        // nolight_shader_uniforms.push_back("ModelMatrix");
        // nolight_shader_uniforms.push_back("ViewMatrix");
        // nolight_shader_uniforms.push_back("ProjectionMatrix");
        // create_program("nolight", "nolight.fs", "nolight.vs", nolight_shader_uniforms);
        // nolight_shader_uniforms.push_back("joints");
        // for (int x = 0; x < 50; ++x)
        // {
        //     stringstream tmp;
        //     tmp << "joints[" << x << "]";
        //     nolight_shader_uniforms.push_back(tmp.str());
        // }
        // create_program("color", "color.fs", "color.vs", nolight_shader_uniforms);
    }

    void close_window() { if (window != NULL) {window->close(); window = NULL;} }

    void cleanup()
    {
        //Clean up shaders
        glUseProgram(0);
        for (gl_program* cur : shader_ids)
            delete cur;
        shader_ids.clear();

        for (const pair<unsigned long, function< void()> > & func : cleanup_functions)
        {
            func.second();
        }

        //Clean up virtual filesystem
        PHYSFS_deinit();

        //Clean up SFML's window
        close_window();
        exit(EXIT_SUCCESS);
    }

    sf::Window* create_window(int width, int height, const string & title)
    {
        close_window();
        window_width = width;
        window_height = height;
        window = new sf::Window(sf::VideoMode(width, height, 32), title);
        window->setActive();
        window->display();
#if LIMIT_FRAMES > -1
        window->setFramerateLimit(LIMIT_FRAMES);
#endif
        CHECK_GL_ERROR("Init Window");
        init_glew();
        CHECK_GL_ERROR("Init Glew");
        return window;
    }

    void resize_window(int new_width, int new_height)
    {
        window_width = new_width;
        window_height = new_height;
        glViewport(0, 0, window_width, window_height);
    }

    void init_glew()
    {
        GLenum GlewInitResult;
        glewExperimental = GL_TRUE;
        GlewInitResult = glewInit();

        if (GLEW_OK != GlewInitResult) {
            fprintf(
                stderr,
                "ERROR: %s\n",
                glewGetErrorString(GlewInitResult)
                );
            exit(EXIT_FAILURE);
        }
    
        fprintf(
            stdout,
            "INFO: OpenGL Version: %s\n",
            glGetString(GL_VERSION)
            );

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        CHECK_GL_ERROR("Clear Color");
        glEnable(GL_DEPTH_TEST);
        CHECK_GL_ERROR("Depth Test");
        glDepthFunc(GL_LESS);
        CHECK_GL_ERROR("GL Less");
        glEnable(GL_CULL_FACE);
        CHECK_GL_ERROR("Cull Face");
        glCullFace(GL_BACK);
        CHECK_GL_ERROR("GL Back");
        glFrontFace(GL_CCW);
        CHECK_GL_ERROR("GL CCW");
        glEnable(GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        CHECK_GL_ERROR("GL Blend");

    }

    void check_gl_error(const string & error_message, long line_number, const string & file_name)
    {
        const GLenum error_value = glGetError();
        if (error_value == GL_NO_ERROR)
            return;

        std::cerr << "\nGL Error on line " << line_number << " of file " << file_name << ":\n";
        switch (error_value)
        {
          case GL_INVALID_VALUE:
            std::cerr << "GL_INVALID_VALUE\n";
            break;
          case GL_INVALID_OPERATION:
            std::cerr << "GL_INVALID_OPERATION\n";
            break;
          default:
            std::cerr << "Error Value: " << error_value << '\n';
        }
        std::cerr << error_message << std::endl << std::endl;
        exit(EXIT_FAILURE);
    }

    void game_loop(camera& C, matrix& P)
    {
        CHECK_GL_ERROR("Entering Game Loop");
        if (window == NULL)
            create_window();

        static sf::Clock clock;
        double average_time_elapsed = 0;
        int time_to_next_fps = 1000;
        int time_elapsed;
        int last_tick = clock.getElapsedTime().asMilliseconds();
        while (window->isOpen())
        {
            int new_tick = clock.getElapsedTime().asMilliseconds();
            time_elapsed = new_tick - last_tick;
            average_time_elapsed += time_elapsed / 1000.0f;
            average_time_elapsed /= 2;
            time_to_next_fps -= time_elapsed;
            if (time_to_next_fps < 0)
            {
                time_to_next_fps += 1000;
                std::cout << "FPS: " << 1/average_time_elapsed << "\n";
            }
            // time_elapsed /= 10;
            for (pair<unsigned long, function< void(int)> > cur : begin_update_functions)
                cur.second(time_elapsed);

            //Process Events
            sf::Event event;
            while (window->pollEvent(event))
                for (pair<unsigned long, function< bool(sf::Event&)> > cur : event_handlers)
                    if (cur.second(event))
                        break;

            for (pair<unsigned long, function< void(int)> > cur : pre_draw_functions)
                cur.second(time_elapsed);

            matrix V = C.to_matrix();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            for (pair<unsigned long, function< void(matrix&, matrix&, camera*)> > cur : draw_functions)
            {
                cur.second(V, P, &C);
                
            }

            for (pair<unsigned long, function< void(int)> > cur : post_draw_functions)
                cur.second(time_elapsed);

            glBindVertexArray(0);
            glUseProgram(0);

            window->display();
            last_tick = new_tick;

            for (gameobject* cur : grim_reaper_list)
            {
                delete cur;
            }
            grim_reaper_list.clear();
        }
    }

    void lua_init(const string & script_name)
    {
        // OOLUA::Script lua_context;
        // lua_context.register_class<car>();
        // long file_size;
        // string main_lua_script = string(read_fully_string(("scripts/" + script_name).c_str(), file_size));
        // int err = lua_context.run_chunk(main_lua_script);
        // if (err == false)
        //     std::cerr << "Lua Error: " << OOLUA::get_last_error(lua_context) << std::endl;
    }

    void add_event_handler(const function< bool(const sf::Event&)> & func)
    {
        add_event_handler(-1, func);
    }

    void add_event_handler(unsigned long owner, const function< bool(const sf::Event&)> & func)
    {
        event_handlers.push_back(make_pair(owner, func));
    }

    void add_begin_update_function(const function< void(int)> & func)
    {
        add_begin_update_function(-1, func);
    }

    void add_begin_update_function(unsigned long owner, const function< void(int)> & func)
    {
        begin_update_functions.push_back(make_pair(owner, func));
    }

    void add_pre_draw_function(const function< void(int)> & func)
    {
        add_pre_draw_function(-1, func);
    }

    void add_pre_draw_function(unsigned long owner, const function< void(int)> & func)
    {
        pre_draw_functions.push_back(make_pair(owner, func));
    }

    void add_post_draw_function(const function< void(int)> & func)
    {
        add_post_draw_function(-1, func);
    }

    void add_post_draw_function(unsigned long owner, const function< void(int)> & func)
    {
        post_draw_functions.push_back(make_pair(owner, func));
    }

    void add_draw_function(const function< void(matrix&, matrix&, camera*)> & func)
    {
        add_draw_function(-1, func);
    }

    void add_draw_function(unsigned long owner, const function< void(matrix&, matrix&, camera*)> & func)
    {
        draw_functions.push_back(make_pair(owner, func));
    }

    void add_cleanup_function(const function< void()> & func)
    {
        add_cleanup_function(-1, func);
    }

    void add_cleanup_function(unsigned long owner, const function< void()> & func)
    {
        cleanup_functions.push_back(make_pair(owner, func));
    }

    void remove_handles(unsigned long owner)
    {
        event_handlers.remove_if([owner](const pair<unsigned long, function< bool(const sf::Event&)> > & func) { return func.first == owner; });
        begin_update_functions.remove_if([owner](const pair<unsigned long, function< void(int)> > & func) { return func.first == owner; });
        pre_draw_functions.remove_if([owner](const pair<unsigned long, function< void(int)> > & func) { return func.first == owner; });
        post_draw_functions.remove_if([owner](const pair<unsigned long, function< void(int)> > & func) { return func.first == owner; });
        draw_functions.remove_if([owner](const pair<unsigned long, function< void(matrix&, matrix&, camera*)> > & func) { return func.first == owner; });
        cleanup_functions.remove_if([owner](const pair<unsigned long, function< void()> > & func) { return func.first == owner; });
    }

    void add_to_grim_reaper(gameobject* ob)
    {
        grim_reaper_list.push_back(ob);
    }
}



