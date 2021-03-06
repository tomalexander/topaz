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
#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <iomanip>
#include "types.h"

namespace topaz
{
    void print(const glm::vec3 & data, std::ostream & out = std::cout, int indentation = 0);
    void print(const glm::vec4 & data, std::ostream & out = std::cout, int indentation = 0);
    void print(const glm::mat4 & data, std::ostream & out = std::cout, int indentation = 0);
    void print_matrix(const float* data, std::ostream & out = std::cout, int indentation = 0);
    void print(const glm::quat & data, std::ostream & out = std::cout, int indentation = 0);
    void print_floats(const float* data, size_t size, std::ostream & out = std::cout, int indentation = 0);
    void print_ints(const int* data, size_t size, std::ostream & out = std::cout, int indentation = 0);
}
