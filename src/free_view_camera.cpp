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
#include "free_view_camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace topaz
{
    free_view_camera::free_view_camera()
    {

    }

    free_view_camera::~free_view_camera()
    {

    }

    glm::mat4 free_view_camera::to_matrix()
    {
        glm::mat4 ret = glm::mat4_cast(rotation);
        ret = glm::transpose(ret);
        ret[3][0] = -camera_position.x;
        ret[3][1] = -camera_position.y;
        ret[3][2] = -camera_position.z;
        return ret;
    }

    glm::vec3 free_view_camera::get_position()
    {
        return camera_position;
    }

    void free_view_camera::move_forward(float magnitude)
    {
        glm::vec4 offset = rotation * glm::vec4(0.0f, 0.0f, -magnitude, 1);
        camera_position.x += offset.x;
        camera_position.y += offset.y;
        camera_position.z += offset.z;
    }

    void free_view_camera::strafe(float magnitude)
    {
        glm::vec4 offset = rotation * glm::vec4(magnitude, 0.0f, 0.0f, 1);
        camera_position.x += offset.x;
        camera_position.y += offset.y;
        camera_position.z += offset.z;
    }

    void free_view_camera::yaw(float magnitude)
    {
        glm::quat new_rotation = glm::angleAxis(magnitude, 0.0f, 1.0f, 0.0f);
        rotation = new_rotation * rotation;
    }

    void free_view_camera::pitch(float magnitude)
    {
        glm::quat new_rotation = glm::angleAxis(magnitude, 1.0f, 0.0f, 0.0f);
        rotation = rotation * new_rotation;
    }
}
