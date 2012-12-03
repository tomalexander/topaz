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
#ifndef LUA_DEMO_H_
#define LUA_DEMO_H_
#include <oolua/oolua.h>
#include <string>
#include <iostream>

using std::string;
using std::cout;
class car
{
  public:
    car(string new_state) {state = new_state; cout << "Car Created! " << state << "\n";}
    ~car() {}
    
    void go() {state = "MOVING";}
    void stop() {state = "STOPPED"; cout << "CAR: STOPPING\n";}
    void print() {cout << "CAR: " << state << "\n";}

    string state;
  private:
};

OOLUA_CLASS_NO_BASES(car)
OOLUA_TYPEDEFS No_default_constructor OOLUA_END_TYPES
OOLUA_CONSTRUCTORS_BEGIN
OOLUA_CONSTRUCTOR(std::string)
OOLUA_CONSTRUCTORS_END

OOLUA_MEM_FUNC_0(void, go)
OOLUA_MEM_FUNC_0(void, stop)
OOLUA_MEM_FUNC_0(void, print)

OOLUA_CLASS_END

EXPORT_OOLUA_FUNCTIONS_0_CONST(car)
EXPORT_OOLUA_FUNCTIONS_NON_CONST(car, go, stop, print)
#endif
