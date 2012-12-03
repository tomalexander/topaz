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
#include "gameobject.h"
#include <iostream>
#include "topaz.h"

namespace
{
    unsigned long next_unique_id = 0;
}

namespace topaz
{
    unsigned long get_next_unique_id()
    {
        return next_unique_id++;
    }

    gameobject::gameobject()
    {
        get_id();
    }

    gameobject::~gameobject()
    {
        #if PRINT_ID_ALLOCATION == 1
        std::cout << "Deleted ID " << id << "\n";
        #endif
        remove_handles(id);
    }

    void gameobject::get_id()
    {
        id = get_next_unique_id();
        #if PRINT_ID_ALLOCATION == 1
        std::cout << "Allocated ID " << id << "\n";
        #endif
    }
}
