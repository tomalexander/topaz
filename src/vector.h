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
#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iomanip>
#include <iostream>
#include "types.h"

namespace topaz
{
    class point;

    class vec
    {
      public:
        vec() {data.vector_floats[0] = 0.0f;
            data.vector_floats[1] = 0.0f;
            data.vector_floats[2] = 0.0f;
            sanitize();}
        vec(const vec & other): data(other.data) {}
        vec(const v4sf & other) {data.full_vector = other;}
        vec(float x, float y, float z) {data.vector_floats[0] = x;
            data.vector_floats[1] = y;
            data.vector_floats[2] = z;
            sanitize();}
        vec(float x, float y, float z, float w) {data.vector_floats[0] = x;
            data.vector_floats[1] = y;
            data.vector_floats[2] = z;
            data.vector_floats[3] = w;}
        vec(const point & other);

        inline vec& sanitize() {data.vector_floats[3] = 0.0f; return *this;}

        float& operator[](size_t i) {return data.vector_floats[i];}
        const float& operator[](size_t i) const {return data.vector_floats[i];}
        float& x() {return data.vector_floats[0];}
        float& y() {return data.vector_floats[1];}
        float& z() {return data.vector_floats[2];}
        float& w() {return data.vector_floats[3];}
        const float& x() const {return data.vector_floats[0];}
        const float& y() const {return data.vector_floats[1];}
        const float& z() const {return data.vector_floats[2];}
        const float& w() const {return data.vector_floats[3];}

        float get_length() const {vector_data mult;
            mult.full_vector = data.full_vector * data.full_vector;
            float sum = mult.vector_floats[0] + mult.vector_floats[1] + mult.vector_floats[2];
            return sqrt(sum);}
            

        vec& normalize();
        vec normalized() const;
        
        /** 
         * Get the dot product
         *
         * @param other vector to dot
         *
         * @return dot product of this and other
         */
        float dot(const vec & other) const
        {
            vector_data mult;
            mult.full_vector = data.full_vector * other.data.full_vector;
            return mult.vector_floats[0] + mult.vector_floats[1] + mult.vector_floats[2] + mult.vector_floats[3];
        }

        /** 
         * Get the cross product
         *
         * @param other vector to cross
         *
         * @return cross product of this and other
         */
        vec cross(const vec & other)
        {
            vec ret;
            ret.x() = y()*other.z() - z()*other.y();
            ret.y() = z()*other.x() - x()*other.z();
            ret.z() = x()*other.y() - y()*other.x();
            ret.w() = 0.0f;
            return ret;
        }

        vec operator/(const vec& other) const {return vec(data.full_vector/other.data.full_vector);}
        vec operator/(const float other) const {v4sf div = {other, other, other, other};
            return vec(data.full_vector / div);}
        vec& operator/=(const float other) {v4sf div = {other, other, other, other};
            data.full_vector /= div;
            return *this;}
        vec operator-(const vec& other) const {return vec(data.full_vector-other.data.full_vector);}
        vec operator+(const vec& other) const {return vec(data.full_vector+other.data.full_vector);}
        vec& operator+=(const vec& other) {data.full_vector += other.data.full_vector;
            return *this;}
        vec operator*(const float other) const {v4sf mult = {other, other, other, other};
            return vec(data.full_vector * mult);}
        vec operator*(const vec& other) const {return vec(data.full_vector * other.data.full_vector);}
        vec& operator*=(const float other) {v4sf mult = {other, other, other, other};
            data.full_vector *= mult;
            return *this;}
        
        vector_data data;

        void print(std::ostream & out = std::cout, int indentation = 0) const
        {
            out << std::string(indentation*4, ' ') << "VECTOR:" << std::endl;
            for (int y = 0; y < 4; ++y)
            {
                if (y == 0)
                    out << std::string(indentation*4, ' ');
                else
                    out << "\t";
                out << std::setw(8) << (*this)[y];
            }
            out << std::endl;
        }
        float get_angle_between(const vec & other) const;

      private:
        
    };

    vec normalize(const vec & other);
    vec lerp(const vec & a, const vec & b, const float percent);
    float lerp(const float & a, const float & b, const float percent);
}

#endif
