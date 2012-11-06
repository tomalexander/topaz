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
#ifndef MATRIX_H_
#define MATRIX_H_

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "types.h"
#include "vector.h"
#include "point.h"
#include "quaternion.h"
#include "vertex.h"

using std::ostream;

namespace topaz
{
    class matrix
    {
      public:
        matrix();
        matrix(const matrix_data & other): data(other) {}
        matrix(const v16sf & other) {data.full_matrix = other;}

        float& operator()(size_t x, size_t y)
        {
            return data.matrix_floats[x*4+y];
        }
        const float& operator()(size_t x, size_t y) const
        {
            return data.matrix_floats[x*4+y];
        }
        float& operator[](size_t i)
        {
            return data.matrix_floats[i];
        }
        matrix& operator=(const matrix& other) {
            data = other.data;
            return *this;
        }
        matrix operator*(const matrix& other) const;
        matrix operator*(const float other) const;
        point operator*(const point& other) const;
        vertex operator*(const vertex& other) const;
        matrix& operator*=(const matrix& other);
        matrix& operator*=(const float other);
        matrix operator+(const matrix& other) const
        {
            return matrix(data.full_matrix+other.data.full_matrix);
        }
        matrix operator-(const matrix& other)
        {
            return matrix(data.full_matrix-other.data.full_matrix);
        }

        matrix slow_multiply(const matrix& other);

        v4sf get_row(const int y);

        void set_identity();

        void fill_float_array(float* destination);

        void print(ostream & out = std::cout);

        matrix& translateX(const float x);
        matrix& translateY(const float y);
        matrix& translateZ(const float z);

        matrix& rotateH(const float h);
        matrix& rotateP(const float p);
        matrix& rotateR(const float r);

        matrix& scale(const float s);

        matrix& translateXYZ(const float x, const float y, const float z);
        matrix& translateXYZ(const vec & vec);
        matrix& rotateHPR(const float h, const float p, const float r);

        matrix& transformXYZHPRS(const float x, const float y, const float z, const float h, const float p, const float r, const float s);

        matrix& transpose();

        quaternion to_quaternion();
        
        float determinate();
        matrix inverse();
        matrix get_upper_three();

        matrix_data data;
      private:
    };

    matrix slerp(matrix & a, matrix & b, float scale);
    matrix slerp(const quaternion & a, const quaternion & b, float scale);
    quaternion quat_slerp(const quaternion & a, const quaternion & b, float scale);

    matrix perspective(float angle_of_view, float aspect_ratio, float near, float far);
    matrix look_at(const point & position, const point & target, const vec & up_direction);
}
#endif








