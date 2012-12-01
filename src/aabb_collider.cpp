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
#include "aabb_collider.h"
#if DRAW_COLLISION_SOLIDS == 1
#include "box_primitive.h"
#endif

namespace topaz
{
    aabb_collider::aabb_collider(sqt* _parent_transform, const glm::vec3 & _lesser_corner, const glm::vec3 & _greater_corner)
    {
        type = AABB;
        transform = new sqt(_parent_transform);
        lesser_corner = _lesser_corner;
        greater_corner = _greater_corner;
        #if DRAW_COLLISION_SOLIDS == 1
        display_box = new box_primitive(transform, glm::vec4(1,1,1,0.5), lesser_corner, greater_corner);
        #endif
        #if USE_OPENCL == 1
        collider_id = global_accelerator->num_of_colliders;
        global_accelerator->increase_collider_size_to(global_accelerator->num_of_colliders+1);
        global_accelerator->colliders.push_back(this);
        push_to_cl();
        #endif
    }

    aabb_collider::~aabb_collider()
    {
        delete transform;
        #if DRAW_COLLISION_SOLIDS == 1
        delete display_box;
        #endif
    }

    glm::vec3* aabb_collider::is_colliding_with(collider* other)
    {
        switch (other->type)
        {
          default:
            break;
          case AABB:
            return AABB_collision((aabb_collider*) other);
            break;
        }
        return NULL;
    }

    glm::vec3* aabb_collider::AABB_collision(aabb_collider* other)
    {
        glm::mat4 this_to_world = transform->to_matrix();
        glm::mat4 world_to_this = glm::inverse(this_to_world);
        glm::mat4 other_to_world = other->transform->to_matrix();
        glm::mat4 world_to_other = glm::inverse(other_to_world);
        
        glm::vec3 other_diag = other->greater_corner - other->lesser_corner;
        glm::vec3 other_diag_world = glm::vec3(other_to_world * glm::vec4(other_diag,1.0f));
        glm::vec3 other_diag_this = glm::vec3(world_to_this * glm::vec4(other_diag_world, 1.0f));
        
        glm::vec3 other_lesser_corner_world = glm::vec3(other_to_world * glm::vec4(other->lesser_corner, 1.0f));
        glm::vec3 other_lesser_corner_this = glm::vec3(world_to_this * glm::vec4(other_lesser_corner_world, 1.0f));

        glm::vec3* ret = new glm::vec3();

        if ((other_lesser_corner_this.x+other_diag_this.x > lesser_corner.x && other_lesser_corner_this.x+other_diag_this.x < greater_corner.x) && (other_lesser_corner_this.x > lesser_corner.x && other_lesser_corner_this.x < greater_corner.x))
        {
            //other fully contained
            ret->x = other_lesser_corner_this.x+other_diag_this.x/2.0f;
        } else if (other_lesser_corner_this.x > lesser_corner.x && other_lesser_corner_this.x < greater_corner.x)
        {
            //lesser contained
            ret->x = other_lesser_corner_this.x + (greater_corner.x-other_lesser_corner_this.x)/2.0f;
        } else if (other_lesser_corner_this.x+other_diag_this.x > lesser_corner.x && other_lesser_corner_this.x+other_diag_this.x < greater_corner.x)
        {
            //greater contained
            ret->x = lesser_corner.x + (other_lesser_corner_this.x+other_diag_this.x-lesser_corner.x)/2.0f;
        } else {
            delete ret;
            return NULL;
        }

        if ((other_lesser_corner_this.y+other_diag_this.y > lesser_corner.y && other_lesser_corner_this.y+other_diag_this.y < greater_corner.y) && (other_lesser_corner_this.y > lesser_corner.y && other_lesser_corner_this.y < greater_corner.y))
        {
            //other fully contained
            ret->y = other_lesser_corner_this.y+other_diag_this.y/2.0f;
        } else if (other_lesser_corner_this.y > lesser_corner.y && other_lesser_corner_this.y < greater_corner.y)
        {
            //lesser contained
            ret->y = other_lesser_corner_this.y + (greater_corner.y-other_lesser_corner_this.y)/2.0f;
        } else if (other_lesser_corner_this.y+other_diag_this.y > lesser_corner.y && other_lesser_corner_this.y+other_diag_this.y < greater_corner.y)
        {
            //greater contained
            ret->y = lesser_corner.y + (other_lesser_corner_this.y+other_diag_this.y-lesser_corner.y)/2.0f;
        } else {
            delete ret;
            return NULL;
        }

        if ((other_lesser_corner_this.z+other_diag_this.z > lesser_corner.z && other_lesser_corner_this.z+other_diag_this.z < greater_corner.z) && (other_lesser_corner_this.z > lesser_corner.z && other_lesser_corner_this.z < greater_corner.z))
        {
            //other fully contained
            ret->z = other_lesser_corner_this.z+other_diag_this.z/2.0f;
        } else if (other_lesser_corner_this.z > lesser_corner.z && other_lesser_corner_this.z < greater_corner.z)
        {
            //lesser contained
            ret->z = other_lesser_corner_this.z + (greater_corner.z-other_lesser_corner_this.z)/2.0f;
        } else if (other_lesser_corner_this.z+other_diag_this.z > lesser_corner.z && other_lesser_corner_this.z+other_diag_this.z < greater_corner.z)
        {
            //greater contained
            ret->z = lesser_corner.z + (other_lesser_corner_this.z+other_diag_this.z-lesser_corner.z)/2.0f;
        } else {
            delete ret;
            return NULL;
        }


        *ret = glm::vec3(this_to_world * glm::vec4(*ret, 1.0f));
        
        return ret;
    }

    glm::vec3 aabb_collider::get_world_position()
    {
        glm::vec4 origin(0,0,0,1);
        return glm::vec3(transform->to_matrix() * origin);
    }

    #if DRAW_COLLISION_SOLIDS == 1
    void aabb_collider::display_as_colliding(bool colliding)
    {
        if (colliding)
        {
            display_box->color = glm::vec4(1,0,0,0.5);
        } else {
            display_box->color = glm::vec4(1,1,1,0.5);
        }
    }
    #endif
}
