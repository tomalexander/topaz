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
#ifndef QUATERNION_H_
#define QUATERNION_H_

#include "types.h"
#include <iostream>
#include <tuple>

namespace topaz
{
    class matrix;
    class vec;
    class quaternion
    {
      public:
        quaternion();
        quaternion(const quaternion & other);
        quaternion(const v4sf & other) {q.full_vector = other;}
        quaternion(const vec & axis, float theta);
        ~quaternion();

        void normalize();
        quaternion normalized();
        quaternion get_conjugate() const;
        std::tuple<vec, float> get_axis_angle() const;
        // quaternion get_inverse() const;

        float& x() { return q.vector_floats[1]; }
        float& y() { return q.vector_floats[2]; }
        float& z() { return q.vector_floats[3]; }
        float& w() { return q.vector_floats[0]; }
        const float& x() const { return q.vector_floats[1]; }
        const float& y() const { return q.vector_floats[2]; }
        const float& z() const { return q.vector_floats[3]; }
        const float& w() const { return q.vector_floats[0]; }
        vector_data q;

        quaternion operator*(const quaternion & other) const;
        quaternion& operator*=(const quaternion & other);
        quaternion operator*(float b) const;
        vec operator*(const vec & other) const;
        quaternion operator+(const quaternion & other) const;
        quaternion& operator=(const quaternion & other);
        float dot(const quaternion & other) const;

        quaternion& rotateH(const float deg);
        quaternion& rotateP(const float deg);
        quaternion& rotateR(const float deg);


        matrix to_matrix();

        void print(std::ostream & out = std::cout, int indentation = 0);
    };
}

#endif
