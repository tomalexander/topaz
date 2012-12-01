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
#ifndef RIGIDBODY_H_
#define RIGIDBODY_H_
#include "def.h"
#include "topaz.h"
#include "gameobject.h"
#include <utility>
#include "sphere_collider.h"
#include "collision.h"
#include "sqt.h"

using std::pair;
using std::make_pair;

/*
 * Meters, Kilograms, Seconds... use it
 */

namespace topaz
{
    class unit;
    class collider;

    enum TENSOR {SPHERE_TENSOR};

    class rigidbody : gameobject
    {
      public:
        rigidbody(sqt* _transform, unit* parent_unit, TENSOR tensor = SPHERE_TENSOR, const string & type = "SPHERE");
        ~rigidbody();

        void update(int milliseconds);
        void calculate_center_of_mass(unit* parent);
        float calculate_farthest_dimension(unit* parent);
        void calculate_axis_aligned_bounding_box(unit* parent);
        void handle_collisions(int milliseconds);
        glm::vec3 handle_linear_collision(const collision & other);
        void handle_angular_collision(const collision & other, const glm::vec3 & collision_force);
        void immovable_collision(const glm::vec3 & norm);
        void calculate_tensor(TENSOR tensor);

        glm::vec3 velocity;
        glm::vec3 center_of_mass;
        sqt* transform;
        bool use_gravity;
        float mass;
        float elasticity;
        vector<collider*> colliders;
        list<pair<int, glm::vec3> > forces;
        list<pair<int, glm::vec3> > torques;
        list<glm::vec3> constant_forces;
        list<glm::vec3> constant_torques;
        float farthest_dimension;
        vector<rigidbody*> handled_collisions;
        glm::vec3 last_position;
        glm::mat4 I;
        glm::vec3 angular_momentum;   /**< L */
        int rigidbody_id;

        glm::vec3 get_world_center_of_mass();
        vector<collision> check_for_collision();
        pair<bool, collision> is_colliding_with(rigidbody* other);
        void push_to_cl();
        void pull_from_cl();
        void populate_acceleration(float* accelerations, int milliseconds);
        void check_bounds();
        
      private:
        void add_force_to_acceleration(glm::vec3 & acceleration, const glm::vec3 & force);
        void add_torque_to_angular_momentum(const glm::vec3 & torque);
        void explicit_euler_numerical_integration(const glm::vec3 & acceleration, const double & seconds);
        void maths(const glm::vec3 & acceleration, const double & seconds);
    };
}
#endif
