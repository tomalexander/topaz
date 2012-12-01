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
#ifndef LOOKAT_CAMERA_H_
#define LOOKAT_CAMERA_H_

#include "camera.h"

namespace topaz
{
    class lookat_camera : public camera
    {
      public:
        lookat_camera();
        ~lookat_camera();

        virtual glm::mat4 to_matrix();
        virtual glm::vec3 get_position();

        void set_location(glm::vec3 new_location) {location = new_location;}
        void add_location(const glm::vec3 & other) {location = location + other;}
        void slide(const glm::vec3 & other) {location = location + other; target = target + other;}
        const glm::vec3& get_target();

      private:
        glm::vec3 location;
        glm::vec3 target;
        glm::vec3 up;
    };
}

#endif
