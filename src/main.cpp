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
        vector<vector<bool> > all_inverses = every_possible_combination(values.size(), {true, false});

        for (vector<float> values : all_value_orders)
        {
            for (vector<u8> axis_orders : all_axis_orders)
            {
                for (vector<bool> current_negation : all_negations)
                {
                    for (vector<bool> current_inverses : all_inverses)
                    {
                        glm::mat4 current(origin);
                        //Grab first 3 values, match with first 3 axes and first 3 negations
                        for (u8 i = 0; i < 3; ++i)
                        {
                            float rotation_amount = (current_negation[i] ? -values[i] : values[i]);
                            if (current_inverses[i])
                                rotation_amount = 1.0f/rotation_amount;
                            switch (axis_orders[i])
                            {
                              case 1:
                                current = glm::rotate(current, rotation_amount, glm::vec3(1, 0, 0)); 
                                break;
                              case 2:
                                current = glm::rotate(current, rotation_amount, glm::vec3(0, 1, 0));
                                break;
                              case 3:
                                current = glm::rotate(current, rotation_amount, glm::vec3(0, 0, 1));
                                break;
                            }
                        }
                        if (similar(goal, current, 0.025f))
                        {
                            std::cout << "FOUND!\n";
                            for (u8 i = 0; i < 3; ++i)
                            {
                                float rotation_amount = (current_negation[i] ? -values[i] : values[i]);
                                if (current_inverses[i])
                                    rotation_amount = 1.0f/rotation_amount;
                                switch (axis_orders[i])
                                {
                                  case 1:
                                    std::cout << "  Pitch: " << rotation_amount << "\n";
                                    break;
                                  case 2:
                                    std::cout << "  Heading: " << rotation_amount << "\n";
                                    break;
                                  case 3:
                                    std::cout << "  Roll: " << rotation_amount << "\n";
                                    break;
                                }
                            }
                            topaz::print(current, std::cout, 2);
                        }
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
    leg_upper = glm::translate(leg_upper, glm::vec3(180.408, -0.000120127, 0.0f));
    leg_upper = glm::rotate(leg_upper, 35.4189f, glm::vec3(0, 0, 1));
    leg_upper = glm::rotate(leg_upper, 10.7426f, glm::vec3(0, 1, 0));
    leg_upper = glm::rotate(leg_upper, -20.2138f, glm::vec3(1, 0, 0));
    
    // std::cout << "Shoulders:\n";
    // topaz::print(shoulders);
    // topaz::print(dummy_shoulders);
    // std::cout << "Clavicle:\n";
    // topaz::print(bone_rf_clavicle);
    // std::cout << "leg upper:\n";
    // topaz::print(bone_rf_leg_upper);
    // topaz::print(leg_upper);
    
    // topaz::print(bone_rf_foot_nub);

    // int frame_number = 0;
    // topaz::print(u.current_animation->apply(frame_number, u.root_joint->find_joint_name("Bone_rf_foot"), true));
    // vector<string> joint_name_list = {"Dummy_hips", "Bone_lr_leg_hip", "Bone_lr_leg_upper", "Bone_lr_leg_lower", "Bone_lr_foot", "Bone_lr_foot_nub", "Bone_rr_leg_hip", "Bone_rr_leg_upper", "Bone_rr_leg_lower", "Bone_rr_foot", "Bone_rr_foot_nub", "Bone_spine01", "Bone_spine02", "Bone_spine03", "Bone_spine_nub", "Dummy_lf_foot_heel", "Dummy_lf_foot_toe", "Dummy_lr_foot_heel", "Dummy_lr_foot_toe", "Dummy_rf_foot_heel", "Dummy_rf_foot_toe", "Dummy_rr_foot_heel", "Dummy_rr_foot_toe", "Dummy_shoulders", "Bone_lf_clavicle", "Bone_lf_leg_upper", "Bone_lf_leg_lower", "Bone_lf_foot", "Bone_lf_foot_nub", "Bone_neck", "Bone_jaw01", "Bone_jaw02", "Bone_jaw03", "Bone_jaw_nub", "Bone_skull", "Bone_skull_nub", "Dummy_jaw", "Bone_rf_clavicle", "Bone_rf_leg_upper", "Bone_rf_leg_lower", "Bone_rf_foot", "Bone_rf_foot_nub"};
    // for (int frame_number = 0; frame_number < 62; ++frame_number)
    // {
    //     for (string jname : joint_name_list)
    //     {
    //         glm::mat4 world(1.0f);
    //         vector<topaz::joint*> jlist;
    //         for (topaz::joint* current = u.root_joint->find_joint_name(jname); current->name != "_ROOT"; current = current->parent)
    //         {
    //             jlist.push_back(current);
    //         }
    //         for (auto it = jlist.rbegin(); it != jlist.rend(); ++it)
    //         {
    //             topaz::joint* current = *it;
    //             world *= u.current_animation->apply(frame_number, current, true);
    //         }
    //         std::cout << jname << "\n";
    //         for (u8 y = 0; y < 4; ++y)
    //         {
    //             for (u8 x = 0; x < 4; ++x)
    //             {
    //                 if (x != 0)
    //                     std::cout << " ";
    //                 std::cout << world[x][y];
    //             }
    //             std::cout << "\n";
    //         }
    //         std::cout << "\n";
    //     }
    // }
    // return 0;

    // vector<string> joint_name_list = {"Dummy_hips", "Bone_lr_leg_hip", "Bone_lr_leg_upper", "Bone_lr_leg_lower", "Bone_lr_foot", "Bone_lr_foot_nub", "Bone_rr_leg_hip", "Bone_rr_leg_upper", "Bone_rr_leg_lower", "Bone_rr_foot", "Bone_rr_foot_nub", "Bone_spine01", "Bone_spine02", "Bone_spine03", "Bone_spine_nub", "Dummy_lf_foot_heel", "Dummy_lf_foot_toe", "Dummy_lr_foot_heel", "Dummy_lr_foot_toe", "Dummy_rf_foot_heel", "Dummy_rf_foot_toe", "Dummy_rr_foot_heel", "Dummy_rr_foot_toe", "Dummy_shoulders", "Bone_lf_clavicle", "Bone_lf_leg_upper", "Bone_lf_leg_lower", "Bone_lf_foot", "Bone_lf_foot_nub", "Bone_neck", "Bone_jaw01", "Bone_jaw02", "Bone_jaw03", "Bone_jaw_nub", "Bone_skull", "Bone_skull_nub", "Dummy_jaw", "Bone_rf_clavicle", "Bone_rf_leg_upper", "Bone_rf_leg_lower", "Bone_rf_foot", "Bone_rf_foot_nub"};
    // for (string jname : joint_name_list)
    // {
    //     glm::mat4 world(1.0f);
    //     vector<topaz::joint*> jlist;
    //     topaz::joint* current = u.root_joint->find_joint_name(jname);
    //     world = current->local_binding;
    //     std::cout << jname << "\n";
    //     for (u8 y = 0; y < 4; ++y)
    //     {
    //         for (u8 x = 0; x < 4; ++x)
    //         {
    //             if (x != 0)
    //                 std::cout << " ";
    //             std::cout << world[x][y];
    //         }
    //         std::cout << "\n";
    //     }
    //     std::cout << "\n";
    // }
    // return 0;

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

