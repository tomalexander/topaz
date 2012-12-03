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
#include "topaz.h"
#include "model.h"
#include "panda_node.h"
#include "egg_parser.h"
#include <stdio.h>
#include <unordered_map>
#include "lookat_camera.h"
#include "free_view_camera.h"
#include "uberlight.h"
#include "unit.h"
#include "animation.h"
#include "sphere_primitive.h"
#include "rigidbody.h"
#include <fstream>
#include "overlay.h"
#include "terrain.h"
#include <glm/gtc/matrix_transform.hpp>
#include "print.h"

using std::unordered_map;

void game_loop();
bool handle_keypress(const sf::Event & event);
bool handle_resize(const sf::Event & event);
void handle_keyboard(int milliseconds);
void handle_mouse_move();
void print_fps(int milliseconds);

glm::mat4 P;
topaz::lookat_camera camera;
topaz::uberlight main_light;
topaz::model* panda_model;
topaz::unit* panda_unit;
topaz::unit* other_panda_unit;
topaz::unit* pipe_unit;
int time_elapsed;

namespace
{
    bool similar(const glm::mat4 & a, const glm::mat4 & b, float ok_range = 1.0f)
    {
        for (u32 i = 0; i < 16; ++i)
        {
            float lower = a[i%4][i/4] - ok_range;
            float upper = a[i%4][i/4] + ok_range;
            float test = b[i%4][i/4];
            if (test < lower || test > upper)
                return false;
        }
        return true;
    }

    template<class T>
    void rotate_values(T* begin, T* end)
    {
        T first = *begin;
        for (T* current = begin; current != end; current = &current[1])
        {
            *current = current[1];
        }
        *end = first;
    }

    template<class T>
    vector<vector<T> > every_possible_order(const vector<T> & values)
    {
        vector<vector<T> > ret;
        if (values.size() == 1)
        {
            ret.push_back(values);
            return ret;
        }
        vector<T> current_iteration(values);
        for (size_t i = 0; i < values.size(); ++i)
        {
            T front = current_iteration[0];
            vector<T> remaining(++current_iteration.begin(), current_iteration.end());
            for (vector<T> remaining_orders : every_possible_order(remaining))
            {
                vector<T> add;
                add.push_back(front);
                for (T cur : remaining_orders)
                {
                    add.push_back(cur);
                }
                ret.push_back(add);
            }
            rotate_values(&current_iteration[0], &current_iteration[current_iteration.size()-1]);
            
        }
        return ret;
    }

    template<class T>
    vector<vector<T> > every_possible_combination(size_t num, std::initializer_list<T> possible_values)
    {
        vector<vector<T> > ret;
        if (num == 0)
            return ret;
        vector<vector<T> > all_remaining = every_possible_combination<T>(num-1, possible_values);
        for (T current : possible_values)        
        {
            vector<T> new_link;
            new_link.push_back(current);
            if (all_remaining.empty())
                ret.push_back(new_link);
                
            for (vector<T> remaining : all_remaining)
            {
                vector<T> sub_new_link(new_link);
                for (T entry : remaining)
                {
                    sub_new_link.push_back(entry);
                }
                ret.push_back(sub_new_link);
            }
        }
        return ret;
    }

    void brute_force(const glm::mat4 & goal, const glm::mat4 & origin, vector<float> values)
    {
        vector<u8> axis = {1, 2, 3};
        vector<vector<float> > all_value_orders = every_possible_order(values);
        vector<vector<u8> > all_axis_orders = every_possible_order(axis);
        vector<vector<bool> > all_negations = every_possible_combination(values.size(), {true, false});

        for (vector<float> values : all_value_orders)
        {
            for (vector<u8> axis_orders : all_axis_orders)
            {
                for (vector<bool> current_negation : all_negations)
                {
                    glm::mat4 current(origin);
                    //Grab first 3 values, match with first 3 axes and first 3 negations
                    for (u8 i = 0; i < 3; ++i)
                    {
                        switch (axis_orders[i])
                        {
                          case 1:
                            current = glm::rotate(current, (current_negation[i] ? -values[i] : values[i]), glm::vec3(1, 0, 0)); 
                            break;
                          case 2:
                            current = glm::rotate(current, (current_negation[i] ? -values[i] : values[i]), glm::vec3(0, 1, 0));
                            break;
                          case 3:
                            current = glm::rotate(current, (current_negation[i] ? -values[i] : values[i]), glm::vec3(0, 0, 1));
                            break;
                        }
                    }
                    if (similar(goal, current, 0.025f))
                    {
                        std::cout << "FOUND!\n";
                        for (u8 i = 0; i < 3; ++i)
                        {
                            switch (axis_orders[i])
                            {
                              case 1:
                                std::cout << "  Pitch: " << (current_negation[i] ? -values[i] : values[i]) << "\n";
                                break;
                              case 2:
                                std::cout << "  Heading: " << (current_negation[i] ? -values[i] : values[i]) << "\n";
                                break;
                              case 3:
                                std::cout << "  Roll: " << (current_negation[i] ? -values[i] : values[i]) << "\n";
                                break;
                            }
                        }
                        topaz::print(current, std::cout, 2);
                    }
                }
            }
        }
    }

    void brute_force(const glm::mat4 & goal, const glm::mat4 & origin, const std::initializer_list<float> & _values)
    {
        brute_force(goal, origin, vector<float>(_values));
    }

    void brute_force(const glm::mat4 & goal, const glm::mat4 & origin, const std::initializer_list<std::initializer_list<float> > & values)
    {
        vector<std::initializer_list<float>::iterator> current_position;
        vector<std::initializer_list<float>::iterator> ends;
        for (const std::initializer_list<float> & cur : values)
        {
            current_position.push_back(cur.begin());
            ends.push_back(cur.end());
        }
        while (true)
        {
            vector<float> current_round;
            for (size_t i = 0; i < current_position.size(); ++i)
            {
                if (current_position[i] == ends[i])
                    return;
                current_round.push_back(*current_position[i]);
                current_position[i]++;
            }
            brute_force(goal, origin, current_round);
        }
    }
}

int main(int argc, char** argv)
{
    topaz::init(argv[0]);
    P = glm::perspective(60.0f, 800.0f/600.0f, 0.1f, 100.f);
    
    topaz::model* pipe_model = topaz::load_from_egg("bar", {"bar-bend"});
    panda_model = topaz::load_from_egg("panda-model", {"panda-walk"});
    topaz::unit u(panda_model);
    u.set_scale(0.005);
    u.transform->rotateP(-90);
    u.transform->rotateR(180);
    u.set_animation("panda-walk");
    // topaz::unit u(pipe_model);
    // u.set_animation("bar-bend");
    
    

    // topaz::overlay o(1, 1, topaz::load_texture("green-panda-model.png"));
    // o.scale(0.25);
    // o.translateXY(0.5, 0.5);
    
    // panda_unit = new topaz::unit(panda_model);
    // panda_unit->set_scale(0.005);
    
    topaz::add_event_handler(&handle_keypress);
    topaz::add_event_handler(&handle_resize);
    topaz::add_pre_draw_function(&handle_keyboard);
    topaz::add_pre_draw_function(&print_fps);

    // float grid[] = {1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1};
    // float grid[] = {0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0};
    // topaz::terrain t(4,4,grid,0.5);
    // t.set_scale(4);
    // t.paint(0,0,3,3,topaz::load_texture("green-panda-model.png"));
    // t.paint(1,1,2,2,1.0f,0.0f,0.0f);
    // t.finalize();

    //Binding matricies
    glm::mat4 dummy_shoulders(0.686981, 0, 0.0244345, 0, 0, 0.687415, 0, 0, -0.0244345, 0, 0.686981, 0, -5.0411, -175.499, 484.442, 1);
    glm::mat4 bone_rf_clavicle(-1.09141, 0.201582, -0.940435, 0, 0.0727515, -1.40104, -0.384742, 0, 0.959041, 0.335685, -1.04105, 0, -67.2191, -4.56166, 13.7786, 1);
    glm::mat4 bone_rf_leg_upper(0.891446, -0.0679001, 0.44801, 0, 0.217837, 0.931179, -0.292322, 0, -0.397329, 0.358183, 0.844887, 0, 180.408, -0.000120127, 1.06114e-005, 1);
    glm::mat4 bone_rf_leg_lower(0.227785, 0.603119, 0.764435, 0, -0.972624, 0.17803, 0.14936, 0, -0.0460101, -0.77753, 0.62716, 0, 187.977, 4.73459e-005, 1.99121e-005, 1);
    glm::mat4 bone_rf_foot(0.670927, 0.459455, -0.58203, 0, -0.599002, 0.798484, -0.0601673, 0, 0.437097, 0.389005, 0.810938, 0, 198.933, -0.000236638, 3.29361e-005, 1);
    glm::mat4 bone_rf_foot_nub(0.977258, 0.070261, -0.200076, 0, -0.201948, 0.596166, -0.777047, 0, 0.0646822, 0.799781, 0.596798, 0, 160.363, 6.92648e-006, 2.72178e-006, 1);
    
    //World Matricies
    glm::mat4 shoulders(1.0f);
    shoulders = glm::translate(shoulders, glm::vec3(-5.0411, -175.499, 484.442));
    shoulders = glm::rotate(shoulders, 2.03704f, glm::vec3(0,-1,0));
    shoulders = glm::scale(shoulders, glm::vec3(0.687415));

    glm::mat4 leg_upper(1.0f);
    /*
     * leg upper
     * x 180.408
     * y -0.000120127
     * roll 35.4189
     * heading 10.7426
     * pitch -20.2138
     */
    // leg_upper = glm::translate(leg_upper, glm::vec3(180.408, -0.000120127, 0.0f));
    // leg_upper = glm::rotate(leg_upper, 35.4189f, glm::vec3(0, -1, 0));
    // leg_upper = glm::rotate(leg_upper, 10.7426f, glm::vec3(0, 0, -1));
    // leg_upper = glm::rotate(leg_upper, -20.2138f, glm::vec3(1, 0, 0));
    
    // std::cout << "Shoulders:\n";
    // topaz::print(shoulders);
    // topaz::print(dummy_shoulders);
    // std::cout << "Clavicle:\n";
    // topaz::print(bone_rf_clavicle);
    // std::cout << "leg upper:\n";
    // topaz::print(bone_rf_leg_upper);
    // topaz::print(leg_upper);
    
    // brute_force(bone_rf_leg_upper, leg_upper, {10.7426f, 35.4189f, -20.2138f});
    // brute_force(bone_rf_leg_upper, leg_upper, {{10.7426, 8.03755, 5.44799, 2.9877, 0.657159, -1.53634, -3.57758, -5.46565, -7.18709, -8.71322, -10.0303, -11.123, -11.9972, -12.6895, -13.1957, -13.5297, -13.384, -13.1207, -12.7569, -12.3275, -11.8357, -11.3112, -10.7834, -10.2892, -9.87165, -9.59884, -9.46301, -9.39658, -9.40617, -9.46901, -9.584, -12.0343, -13.7137, -14.9422, -15.8509, -16.5072, -16.9512, -17.2089, -17.2986, -17.2343, -17.0191, -16.6807, -16.2379, -15.6903, -15.0599, -14.3459, -12.7126, -11.0475, -9.36302, -7.65946, -5.93785, -4.20003, -2.44875, -0.682107, 1.07695, 2.83841, 4.54787, 6.17309, 7.73626, 9.24625, 10.7426}, {-20.2138, -20.0738, -19.8733, -19.5669, -19.1785, -18.7156, -18.1699, -17.5875, -16.9829, -16.3373, -15.6875, -15.0545, -14.4383, -13.9083, -13.4194, -12.9776, -12.8004, -12.6424, -12.4998, -12.4256, -12.3572, -12.3087, -12.2748, -12.2501, -12.2095, -12.1861, -12.1786, -12.1329, -12.1309, -12.0965, -12.112, -11.5773, -11.1116, -10.7099, -10.3743, -10.1101, -9.92446, -9.8257, -9.82267, -9.9243, -10.1209, -10.4558, -10.9267, -11.4938, -12.1705, -12.9278, -13.692, -14.399, -15.07, -15.7024, -16.2939, -16.8426, -17.3468, -17.7882, -18.2166, -18.5806, -18.9176, -19.25, -19.5613, -19.9011, -20.2138}, {35.4189, 33.8328, 32.3493, 30.9538, 29.663, 28.4902, 27.4441, 26.562, 25.869, 25.3757, 25.1226, 25.1361, 25.3988, 25.919, 26.6524, 27.5807, 28.6751, 29.8943, 31.2151, 32.6337, 34.1022, 35.5986, 37.09, 38.5384, 39.8928, 41.1078, 42.2297, 43.3309, 44.4222, 45.4843, 46.5312, 44.1691, 42.3824, 40.912, 39.6484, 38.5324, 37.5266, 36.6053, 35.7492, 34.9426, 34.1625, 33.4166, 32.7016, 31.9962, 31.3105, 30.6304, 31.1302, 31.5699, 31.9739, 32.3512, 32.7106, 33.0611, 33.4112, 33.761, 34.144, 34.5415, 34.8903, 35.1283, 35.2715, 35.3658, 35.4189}});
    // std::cout << "Goal:\n";
    // topaz::print(bone_rf_leg_upper);
    // return 0;
    // topaz::print(leg_upper);
    // std::cout << "Goal:\n";
    // topaz::print(bone_rf_leg_upper);

    //topaz::print(bone_rf_foot);

    game_loop(camera, P);
  
    topaz::cleanup();
   
    return 0;
}

void print_fps(int milliseconds)
{
    static fu32 frames = 0;
    static fs32 time_till_print = 1000;
    time_till_print -= milliseconds;
    frames++;
    if (time_till_print <= 0)
    {
        std::cout << "FPS: " << frames << "\n";
        time_till_print += 1000;
        frames = 0;
    }
}

bool handle_keypress(const sf::Event & event)
{
    if (event.type != sf::Event::KeyPressed)
        return false;
    switch (event.key.code)
    {
      case sf::Keyboard::Escape:
        topaz::cleanup();
        return true;
        break;
      default:
        return false;
        break;
    }
}

void handle_keyboard(int time_elapsed)
{
    float seconds = ((float)time_elapsed) / 1000.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        camera.slide(glm::vec3(10.0f*seconds, 0.0f, 0.0f));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        camera.slide(glm::vec3(-10.0f*seconds, 0.0f, 0.0f));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        camera.slide(glm::vec3(0.0f, 0.0f, 10.0f*seconds));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        camera.slide(glm::vec3(0.0f, 0.0f, -10.0f*seconds));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        camera.slide(glm::vec3(0.0f, 10.0f*seconds, 0.0f));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        camera.slide(glm::vec3(0.0f, -10.0f*seconds, 0.0f));
    }
}

bool handle_resize(const sf::Event & event)
{
    if (event.type != sf::Event::Resized)
        return false;
    topaz::resize_window(event.size.width, event.size.height);
    glm::perspective(60.0f, ((float)event.size.width)/((float)event.size.height), 0.1f, 100.f);
    return true;
}

void handle_mouse_move()
{
    sf::Vector2i mouse_position = sf::Mouse::getPosition(*topaz::window);

    sf::Vector2u size = topaz::window->getSize();

    int center_window_x = size.x / 2;
    int center_window_y = size.y / 2;

    int diff_x = mouse_position.x - center_window_x;
    int diff_y = mouse_position.y - center_window_y;

    float rot_yaw = ((float)diff_x) * 0.001;
    float rot_pitch = ((float)diff_y) * 0.001;

    //c2.yaw(rot_yaw);
    //c2.pitch(rot_pitch);

    sf::Vector2i window_center(center_window_x, center_window_y);

    sf::Mouse::setPosition(window_center, *topaz::window);
}

