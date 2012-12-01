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
#ifndef UNIT_H_
#define UNIT_H_

#include "model.h"
#include "gameobject.h"
#include "sqt.h"

namespace topaz
{
    class joint;
    class rigidbody;

    class unit : public gameobject
    {
      public:

        unit();
        unit(model* _model_ptr);
        ~unit();

        void update_matrix();
        void draw(const glm::mat4 & V, const glm::mat4 & P, camera* C);
        void draw_joints(const glm::mat4 & V, const glm::mat4 & P, camera* C);

        void set_scale(float new_scale = 1.0f);

        void update(int milliseconds);
        float get_distance_to(unit* other);
        float get_distance_to(const glm::vec3 & other);
        void set_model(model* _model_ptr);
        void set_rigidbody(const string & type);
        void sync_from_rigidbody(int milliseconds);
        void add_location(const glm::vec3 & diff);

        light* light_source;
        gl_program* light_program;

        joint* root_joint;
        animation* current_animation;
        unsigned int animation_progress; /**< in milliseconds */

        rigidbody* rigid_body;

        const glm::vec3 get_location() {return transform->get_t();}
        model* get_model_ptr() {return model_ptr;}
        bool set_animation(const string & animation_name);

      private:
        sqt* transform;
        model* model_ptr;
    };

    

}
#endif
