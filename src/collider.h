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
#ifndef COLLIDER_H_
#define COLLIDER_H_

#include "def.h"
#include "topaz.h"
#include "sqt.h"

namespace topaz
{
    enum collider_type {SPHERE, AABB};

    class collider
    {
      public:
        collider();
        collider(const collider_type & _type);
        ~collider();

        virtual glm::vec3* is_colliding_with(collider* other) = 0;
        #if DRAW_COLLISION_SOLIDS == 1
        virtual void display_as_colliding(bool colliding) = 0;
        #endif

        collider_type type;
        glm::vec3 position;

        sqt* transform;
        int collider_id;
      private:
    };
}

#endif
