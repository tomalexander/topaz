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

#include "vector.h"
#include "point.h"

namespace topaz
{
    vec::vec(const point & other)
    {
        x() = other.x();
        y() = other.y();
        z() = other.z();
        sanitize();
    }

    /** 
     * Normalize the vector, reducing its length to one
     *
     *
     * @return a reference to this vector
     */
    vec& vec::normalize()
    {
        //square the data
        vector_data squared;
        squared.full_vector = data.full_vector * data.full_vector;
        float length = sqrt(squared.vector_floats[0] + squared.vector_floats[1] + squared.vector_floats[2]);
        if (length == 0.0f)
            return *this;
        v4sf length_vector = {length, length, length, length};
        data.full_vector /= length_vector;
            
        //Just to be safe, make sure the last element is 0
        sanitize();
            
        return *this;
    }

    /** 
     * Normalize the vector, reducing its length to one
     *
     *
     * @return a reference to this vector
     */
    vec normalize(const vec & other)
    {
        //square the data
        vector_data squared;
        squared.full_vector = other.data.full_vector * other.data.full_vector;
        float length = sqrt(squared.vector_floats[0] + squared.vector_floats[1] + squared.vector_floats[2]);
        if (length == 0.0f)
            return vec();
        v4sf length_vector = {length, length, length, length};
        vec ret(other.data.full_vector / length_vector);
        ret.sanitize();
        return ret;
    }

    vec vec::normalized() const
    {
        //square the data
        vector_data squared;
        squared.full_vector = data.full_vector * data.full_vector;
        float length = sqrt(squared.vector_floats[0] + squared.vector_floats[1] + squared.vector_floats[2]);
        if (length == 0.0f)
            return vec();
        v4sf length_vector = {length, length, length, length};
        vec ret(data.full_vector / length_vector);
        ret.sanitize();
        return ret;
    }

    float vec::get_angle_between(const vec & other) const
    {
        float top = dot(other);
        float bottom = get_length() * other.get_length();
        return acos(top/bottom);
    }

    vec lerp(const vec & a, const vec & b, const float percent)
    {
        return (a*(1.0f-percent)) + (b*percent);
    }

    float lerp(const float & a, const float & b, const float percent)
    {
        return (a*(1.0f-percent)) + (b*percent);
    }


}
