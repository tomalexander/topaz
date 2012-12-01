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
#ifndef UBERLIGHT_H_
#define UBERLIGHT_H_

#include "light.h"

namespace topaz
{
    class uberlight : public light
    {
      public:
        uberlight();
        ~uberlight();

        virtual void populate_uniforms(const glm::mat4 & M, const glm::mat4 & V, const glm::mat4 & P, camera* C, gl_program* program);

        glm::mat4 to_matrix();

        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 up;
        float se_width,
            se_height,
            se_width_edge,
            se_height_edge,
            se_roundness,
            surface_roughness,
            ds_near,
            ds_far,
            ds_near_edge,
            ds_far_edge;

        bool barn_shaping,
            ambient_clamping;
        
        union
        {
            struct
            {
                float color_r;
                float color_g;
                float color_b;
            };
            float colors[3];
        };

        union
        {
            struct
            {
                float weight_r;
                float weight_g;
                float weight_b;
            };
            float weights[3];
        };

        union
        {
            struct
            {
                float surface_weight_r;
                float surface_weight_g;
                float surface_weight_b;
            };
            float surface_weights[3];
        };


            
      private:
    };
}

#endif
