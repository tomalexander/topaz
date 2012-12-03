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
#ifndef SQT_H_
#define SQT_H_

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace topaz {

    class sqt {
      public:
        sqt();
        sqt(sqt* _parent);
        ~sqt();

        glm::mat4 to_matrix();
        sqt& translateX(const float x);
        sqt& translateY(const float y);
        sqt& translateZ(const float z);
        sqt& rotateH(const float h);
        sqt& rotateP(const float p);
        sqt& rotateR(const float r);
        sqt& rotateHPR(const float h, const float p, const float r);
        sqt& scale(const float sc);
        sqt& translateXYZ(const float x, const float y, const float z);
        sqt& translateXYZ(const glm::vec3 & vec);
        sqt& transformXYZHPRS(const float x, const float y, const float z, const float h, const float p, const float r, const float sc);
        sqt& reset();
        float get_world_s();
        const glm::vec3& get_t() {return t;}
        sqt& set_t(const glm::vec3 & pos) { t = pos; return *this;}
        const glm::quat& get_q() {return q;}
        sqt& set_q(const glm::quat & new_q) { q = new_q; return *this;}

        float& get_x() {return t.x;}
        float& get_y() {return t.y;}
        float& get_z() {return t.z;}
        float& get_s() {return s;}
        float& get_qx() {return q.x;}
        float& get_qy() {return q.y;}
        float& get_qz() {return q.z;}
        float& get_qw() {return q.w;}


        float s;                /**< Scale Magnitude */
        glm::quat q;           /**< Rotation Quaternion */
        glm::vec3 t;        /**< Translation Vector */
        sqt* parent;            /**< coordinate space this sqt is in */
    };

}

#endif
