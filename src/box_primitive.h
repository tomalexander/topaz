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
#ifndef BOX_PRIMITIVE_H_
#define BOX_PRIMITIVE_H_

#include "def.h"
#include "gameobject.h"
#include "sqt.h"

namespace topaz
{
    class camera;
    class model;
    
    class box_primitive : public gameobject
    {
      public:
        box_primitive(sqt* _parent_transform, const glm::vec4 & _color, const glm::vec3 & _lesser_corner, const glm::vec3 & _greater_corner);
        ~box_primitive();
        void draw(const glm::mat4 & V, const glm::mat4 & P, camera* C);
        void generate_model(const glm::vec3 & _lesser_corner, const glm::vec3 & _greater_corner);

        glm::vec4 color;
        glm::vec3 lesser_corner;
        glm::vec3 greater_corner;

      private:
        sqt* transform;
        model* mod;
    };
}

#endif
