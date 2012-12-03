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
#ifndef AABB_COLLIDER_H_
#define AABB_COLLIDER_H_

#include "def.h"
#include "topaz.h"
#include "collider.h"

namespace topaz
{
    #if DRAW_COLLISION_SOLIDS == 1
    class box_primitive;
    #endif
    class aabb_collider : public collider
    {
      public:
        aabb_collider(sqt* _parent_transform, const glm::vec3 & _lesser_corner, const glm::vec3 & _greater_corner);
        ~aabb_collider();

        virtual glm::vec3* is_colliding_with(collider* other);
        glm::vec3 get_world_position();
        glm::vec3* AABB_collision(aabb_collider* other);

        #if DRAW_COLLISION_SOLIDS == 1
        virtual void display_as_colliding(bool colliding);
        box_primitive* display_box;
        #endif

        sqt* transform;
        glm::vec3 lesser_corner;
        glm::vec3 greater_corner;
      private:
    };
}

#endif
