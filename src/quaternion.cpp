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
#include "quaternion.h"
#include <string.h>
#include <math.h>
#include "matrix.h"
#include "vector.h"

namespace topaz
{
    quaternion::quaternion()
    {
        q.vector_floats[1] = 0.0f;
        q.vector_floats[2] = 0.0f;
        q.vector_floats[3] = 0.0f;
        q.vector_floats[0] = 1;
    }

    quaternion::quaternion(const quaternion & other)
    {
        q = other.q;
    }

    quaternion::quaternion(const vec & axis, float theta)
    {
        theta /= 2;
        w() = cos(theta);
        float sint = sin(theta);
        x() = axis.x() * sint;
        y() = axis.y() * sint;
        z() = axis.z() * sint;
    }

    quaternion::~quaternion()
    {

    }

    void quaternion::normalize()
    {
        vector_data square;
        square.full_vector = q.full_vector * q.full_vector;
        float length = square.vector_floats[0] + square.vector_floats[1] + square.vector_floats[2] + square.vector_floats[3];
        if (fabs(length - 1.0f) > FLOAT_TOLERANCE )
        {
            length = sqrt(length);
            v4sf length_vector = {length, length, length, length};
            q.full_vector = q.full_vector / length_vector;
        }
    }

    quaternion quaternion::normalized()
    {
        vector_data square;
        square.full_vector = q.full_vector * q.full_vector;
        float length = square.vector_floats[0] + square.vector_floats[1] + square.vector_floats[2] + square.vector_floats[3];
        if (fabs(length - 1.0f) > FLOAT_TOLERANCE )
        {
            length = sqrt(length);
            v4sf length_vector = {length, length, length, length};
            quaternion ret;
            ret.q.full_vector = q.full_vector / length_vector;
            return ret;
        } else {
            return quaternion(*this);
        }
    }

    quaternion quaternion::get_conjugate() const
    {
        quaternion ret(*this);
        ret.normalize();
        ret = ret * -1;
        ret.w() *= -1;
        return ret;
    }

    // quaternion quaternion::get_inverse() const
    // {
    //     quaternion conjugate = get_conjugate();
    //     return conjugate / (*this * conjugate);
    // }

    quaternion& quaternion::operator=(const quaternion & other)
    {
        q = other.q;
        return *this;
    }

    quaternion quaternion::operator*(const quaternion & other) const
    {
        quaternion ret;
        ret.x() = x()*other.w() + w()*other.x() + y()*other.z() - z()*other.y();
        ret.y() = y()*other.w() + w()*other.y() + z()*other.x() - x()*other.z();
        ret.z() = z()*other.w() + w()*other.z() + x()*other.y() - y()*other.x();
        ret.w() = w()*other.w() - x()*other.x() - y()*other.y() - z()*other.z();

        return ret;
    }

    quaternion& quaternion::operator*=(const quaternion & other)
    {
        *this = *this * other;
        return *this;
    }

    quaternion quaternion::operator*(float b) const
    {
        quaternion ret;
        v4sf tmp = {b,b,b,b};
        ret.q.full_vector = q.full_vector * tmp;
        return ret;
    }

    vec quaternion::operator*(const vec & other) const
    {
        vec normalized(other);
        normalized.normalize();

        quaternion vector_quaternion;
        vector_quaternion.x() = normalized.x();
        vector_quaternion.y() = normalized.y();
        vector_quaternion.z() = normalized.z();
        vector_quaternion.w() = 0.0f;

        quaternion return_quaternion = vector_quaternion * get_conjugate();
        return_quaternion = *this * return_quaternion;

        return vec(return_quaternion.x(), return_quaternion.y(), return_quaternion.z());
    }

    quaternion quaternion::operator+(const quaternion & other) const
    {
        return quaternion(q.full_vector+other.q.full_vector);
    }

    matrix quaternion::to_matrix()
    {
        quaternion norm(*this);
        norm.normalize();
        matrix ret;
        ret(0,0) = 1 - 2*norm.y()*norm.y() - 2*norm.z()*norm.z();
        ret(1,0) = 2*norm.x()*norm.y() - 2*norm.w()*norm.z();
        ret(2,0) = 2*norm.x()*norm.z() + 2*norm.w()*norm.y();
        ret(0,1) = 2*norm.x()*norm.y() + 2*norm.w()*norm.z();
        ret(1,1) = 1 - 2*norm.x()*norm.x() - 2*norm.z()*norm.z();
        ret(2,1) = 2*norm.y()*norm.z() - 2*norm.w()*norm.x();
        ret(0,2) = 2*norm.x()*norm.z() - 2*norm.w()*norm.y();
        ret(1,2) = 2*norm.y()*norm.z() + 2*norm.w()*norm.x();
        ret(2,2) = 1 - 2*norm.x()*norm.x() - 2*norm.y()*norm.y();
        return ret;
    }

    float quaternion::dot(const quaternion & other) const
    {
        vector_data mult;
        mult.full_vector = q.full_vector * other.q.full_vector;
        return mult.vector_floats[0] + mult.vector_floats[1] + mult.vector_floats[2] + mult.vector_floats[3];
    }
    
    void quaternion::print(std::ostream & out, int indentation)
    {
        out << std::string(indentation*4, ' ') << "Quaternion:\n";
        out << std::string(indentation*4 + 2, ' ') << x() << " " << y() << " " << z() << " " << w() << "\n";
    }

    quaternion& quaternion::rotateH(const float deg)
    {
        *this = *this * quaternion(vec(1.0f, 0.0f, 0.0f), deg);
        return *this;
    }

    quaternion& quaternion::rotateP(const float deg)
    {
        *this = *this * quaternion(vec(1.0f, 0.0f, 0.0f), deg);
        return *this;
    }

    quaternion& quaternion::rotateR(const float deg)
    {
        *this = *this * quaternion(vec(1.0f, 0.0f, 0.0f), deg);
        return *this;
    }
}
