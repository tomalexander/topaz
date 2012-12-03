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
#include "sqt.h"
#include <math.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

using std::cout;
using std::endl;

namespace topaz
{
    sqt::sqt()
    {
        parent = NULL;
        reset();
    }

    sqt::sqt(sqt* _parent)
    {
        parent = _parent;
        reset();
    }

    sqt::~sqt()
    {

    }

    glm::mat4 sqt::to_matrix()
    {
        glm::mat4 ret;
        glm::mat4 rotate_matrix = glm::mat4_cast(q);
        ret = glm::translate(ret, glm::vec3(t[0],t[1],t[2]));
        ret = ret * rotate_matrix;
        ret = glm::scale(ret, glm::vec3(s));
        if (parent != NULL)
            ret = parent->to_matrix() * ret;
        return ret;
    }

    sqt& sqt::translateX(const float x)
    {
        t.x += x;
        return *this;
    }

    sqt& sqt::translateY(const float y)
    {
        t.y += y;
        return *this;
    }

    sqt& sqt::translateZ(const float z)
    {
        t.z += z;
        return *this;
    }

    sqt& sqt::rotateH(const float h)
    {
        q = glm::rotate(q, h, glm::vec3(0, 1, 0));
        return *this;
    }

    sqt& sqt::rotateP(const float p)
    {
        q = glm::rotate(q, p, glm::vec3(1, 0, 0));
        return *this;
    }

    sqt& sqt::rotateR(const float r)
    {
        q = glm::rotate(q, r, glm::vec3(0, 0, 1));
        return *this;
    }

    sqt& sqt::rotateHPR(const float h, const float p, const float r)
    {
        rotateH(h);
        rotateP(p);
        rotateR(r);
        return *this;
    }

    sqt& sqt::scale(const float sc)
    {
        s *= sc;
        return *this;
    }

    sqt& sqt::translateXYZ(const float x, const float y, const float z)
    {
        t.x += x;
        t.y += y;
        t.z += z;
        return *this;
    }

    sqt& sqt::translateXYZ(const glm::vec3 & vec)
    {
        t = t + vec;
        return *this;
    }

    sqt& sqt::transformXYZHPRS(const float x, const float y, const float z, const float h, const float p, const float r, const float sc)
    {
        rotateH(h);
        rotateP(p);
        rotateR(r);
        scale(sc);
        t.x += x;
        t.y += y;
        t.z += z;
        return *this;
    }

    sqt& sqt::reset()
    {
        s = 1.0f;
        q = glm::quat();
        t = glm::vec3();
        return *this;
    }

    float sqt::get_world_s()
    {
        if (parent == NULL)
            return s;
        else
            return s * parent->get_world_s();
    }
}
