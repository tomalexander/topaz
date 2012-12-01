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
#ifndef FREE_VIEW_CAMERA_H_
#define FREE_VIEW_CAMERA_H_

#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace topaz
{
    class free_view_camera : public camera
    {
      public:
        free_view_camera();
        ~free_view_camera();

        virtual glm::mat4 to_matrix();
        virtual glm::vec3 get_position();

        void move_forward(float magnitude);
        void strafe(float magnitude);
        void yaw(float magnitude);
        void pitch(float magnitude);

      private:
        glm::quat rotation;
        glm::vec3 camera_position;
    };
}
#endif
