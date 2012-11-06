#include <iomanip>
#include "types.h"
#include "matrix.h"
#if defined GCC
#include "gcc_shuffle.h"
#endif

namespace topaz
{
    matrix::matrix()
    {
        set_identity();
    }

    void matrix::set_identity()
    {
        for (size_t x = 0; x < 16; ++x)
        {
            (*this)[x] = 0.0f;
        }
        for (size_t x = 0; x < 4; ++x)
        {
            (*this)(x,x) = 1.0f;
        }
    }

    void matrix::fill_float_array(float* destination)
    {
        memcpy(destination, &(data.matrix_floats[0]), sizeof(float)*16);
    }

    void matrix::print(ostream & out)
    {
        out << "MATRIX:" << std::endl;
        for (int y = 0; y < 4; ++y)
        {
            for (int x = 0; x < 4; ++x)
            {
                if (x != 0)
                    out << "\t";
                out << std::setw(8) << (*this)(x,y);
            }
            out << std::endl;
        }
    }

    matrix matrix::slow_multiply(const matrix& other)
    {
        matrix ret;
        for (int y_this = 0; y_this < 4; ++y_this)
        {//row of first matrix
            for (int x_other = 0; x_other < 4; ++x_other)
            {//column of second matrix
                float dot_product = 0.0f;
                for (int sub_index = 0; sub_index < 4; ++sub_index)
                {//column of first matrix and row of second matrix
                    dot_product += (*this)(sub_index,y_this) * other(x_other,sub_index);
                }
                ret(x_other,y_this) = dot_product;
            }
        }
        return ret;
    }

    point matrix::operator*(const point& other) const
    {
        point ret;

        #if defined GCC
        v16sf other_column_1 = {other[0], other[0], other[0], other[0], other[1], other[1], other[1], other[1], other[2], other[2], other[2], other[2], other[3], other[3], other[3], other[3]};
        #endif
        #if defined CLANG
        v16sf other_column_1 = __builtin_shufflevector(other.data.full_vector, other.data.full_vector, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3);
        #endif

        matrix_data multiplied_column_1;

        multiplied_column_1.full_matrix = data.full_matrix * other_column_1;

        ret.data.full_vector = multiplied_column_1.matrix_vectors[0] + multiplied_column_1.matrix_vectors[1] + multiplied_column_1.matrix_vectors[2] + multiplied_column_1.matrix_vectors[3];

        return ret;
    }

    vertex matrix::operator*(const vertex& other) const
    {
        vertex ret;
        memcpy(&ret, &other, sizeof(vertex));
        point tmp(other.x, other.y, other.z);
        tmp = *this * tmp;
        ret.x = tmp.x();
        ret.y = tmp.y();
        ret.z = tmp.z();
        return ret;
    }

    matrix matrix::operator*(const float other) const
    {
        v16sf scalar = {other, other, other, other, other, other, other, other, other, other, other, other, other, other, other, other};
        return matrix(data.full_matrix * scalar);
    }

    matrix& matrix::operator*=(const float other)
    {
        v16sf scalar = {other, other, other, other, other, other, other, other, other, other, other, other, other, other, other, other};
        data.full_matrix *= scalar;
        return (*this);
    }

    matrix matrix::operator*(const matrix& other) const
    {
        matrix ret;

        #if defined GCC
        v16sf other_column_1 = {other.data.matrix_floats[0], other.data.matrix_floats[0], other.data.matrix_floats[0], other.data.matrix_floats[0], other.data.matrix_floats[1], other.data.matrix_floats[1], other.data.matrix_floats[1], other.data.matrix_floats[1], other.data.matrix_floats[2], other.data.matrix_floats[2], other.data.matrix_floats[2], other.data.matrix_floats[2], other.data.matrix_floats[3], other.data.matrix_floats[3], other.data.matrix_floats[3], other.data.matrix_floats[3]};
        v16sf other_column_2 = {other.data.matrix_floats[4], other.data.matrix_floats[4], other.data.matrix_floats[4], other.data.matrix_floats[4], other.data.matrix_floats[5], other.data.matrix_floats[5], other.data.matrix_floats[5], other.data.matrix_floats[5], other.data.matrix_floats[6], other.data.matrix_floats[6], other.data.matrix_floats[6], other.data.matrix_floats[6], other.data.matrix_floats[7], other.data.matrix_floats[7], other.data.matrix_floats[7], other.data.matrix_floats[7]};
        v16sf other_column_3 = {other.data.matrix_floats[8], other.data.matrix_floats[8], other.data.matrix_floats[8], other.data.matrix_floats[8], other.data.matrix_floats[9], other.data.matrix_floats[9], other.data.matrix_floats[9], other.data.matrix_floats[9], other.data.matrix_floats[10], other.data.matrix_floats[10], other.data.matrix_floats[10], other.data.matrix_floats[10], other.data.matrix_floats[11], other.data.matrix_floats[11], other.data.matrix_floats[11], other.data.matrix_floats[11]};
        v16sf other_column_4 = {other.data.matrix_floats[12], other.data.matrix_floats[12], other.data.matrix_floats[12], other.data.matrix_floats[12], other.data.matrix_floats[13], other.data.matrix_floats[13], other.data.matrix_floats[13], other.data.matrix_floats[13], other.data.matrix_floats[14], other.data.matrix_floats[14], other.data.matrix_floats[14], other.data.matrix_floats[14], other.data.matrix_floats[15], other.data.matrix_floats[15], other.data.matrix_floats[15], other.data.matrix_floats[15]};
        #endif
        #if defined CLANG
        v16sf other_column_1 = __builtin_shufflevector(other.data.full_matrix, other.data.full_matrix, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3);
        v16sf other_column_2 = __builtin_shufflevector(other.data.full_matrix, other.data.full_matrix, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7);
        v16sf other_column_3 = __builtin_shufflevector(other.data.full_matrix, other.data.full_matrix, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11);
        v16sf other_column_4 = __builtin_shufflevector(other.data.full_matrix, other.data.full_matrix, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15);
        #endif

        matrix_data multiplied_column_1;
        matrix_data multiplied_column_2;
        matrix_data multiplied_column_3;
        matrix_data multiplied_column_4;

        multiplied_column_1.full_matrix = data.full_matrix * other_column_1;
        multiplied_column_2.full_matrix = data.full_matrix * other_column_2;
        multiplied_column_3.full_matrix = data.full_matrix * other_column_3;
        multiplied_column_4.full_matrix = data.full_matrix * other_column_4;

        ret.data.matrix_vectors[0] = multiplied_column_1.matrix_vectors[0] + multiplied_column_1.matrix_vectors[1] + multiplied_column_1.matrix_vectors[2] + multiplied_column_1.matrix_vectors[3];
        ret.data.matrix_vectors[1] = multiplied_column_2.matrix_vectors[0] + multiplied_column_2.matrix_vectors[1] + multiplied_column_2.matrix_vectors[2] + multiplied_column_2.matrix_vectors[3];
        ret.data.matrix_vectors[2] = multiplied_column_3.matrix_vectors[0] + multiplied_column_3.matrix_vectors[1] + multiplied_column_3.matrix_vectors[2] + multiplied_column_3.matrix_vectors[3];
        ret.data.matrix_vectors[3] = multiplied_column_4.matrix_vectors[0] + multiplied_column_4.matrix_vectors[1] + multiplied_column_4.matrix_vectors[2] + multiplied_column_4.matrix_vectors[3];


        return ret;
    }

    matrix& matrix::operator*=(const matrix& other)
    {
        #if defined GCC
        v16sf other_column_1 = {other.data.matrix_floats[0], other.data.matrix_floats[0], other.data.matrix_floats[0], other.data.matrix_floats[0], other.data.matrix_floats[1], other.data.matrix_floats[1], other.data.matrix_floats[1], other.data.matrix_floats[1], other.data.matrix_floats[2], other.data.matrix_floats[2], other.data.matrix_floats[2], other.data.matrix_floats[2], other.data.matrix_floats[3], other.data.matrix_floats[3], other.data.matrix_floats[3], other.data.matrix_floats[3]};
        v16sf other_column_2 = {other.data.matrix_floats[4], other.data.matrix_floats[4], other.data.matrix_floats[4], other.data.matrix_floats[4], other.data.matrix_floats[5], other.data.matrix_floats[5], other.data.matrix_floats[5], other.data.matrix_floats[5], other.data.matrix_floats[6], other.data.matrix_floats[6], other.data.matrix_floats[6], other.data.matrix_floats[6], other.data.matrix_floats[7], other.data.matrix_floats[7], other.data.matrix_floats[7], other.data.matrix_floats[7]};
        v16sf other_column_3 = {other.data.matrix_floats[8], other.data.matrix_floats[8], other.data.matrix_floats[8], other.data.matrix_floats[8], other.data.matrix_floats[9], other.data.matrix_floats[9], other.data.matrix_floats[9], other.data.matrix_floats[9], other.data.matrix_floats[10], other.data.matrix_floats[10], other.data.matrix_floats[10], other.data.matrix_floats[10], other.data.matrix_floats[11], other.data.matrix_floats[11], other.data.matrix_floats[11], other.data.matrix_floats[11]};
        v16sf other_column_4 = {other.data.matrix_floats[12], other.data.matrix_floats[12], other.data.matrix_floats[12], other.data.matrix_floats[12], other.data.matrix_floats[13], other.data.matrix_floats[13], other.data.matrix_floats[13], other.data.matrix_floats[13], other.data.matrix_floats[14], other.data.matrix_floats[14], other.data.matrix_floats[14], other.data.matrix_floats[14], other.data.matrix_floats[15], other.data.matrix_floats[15], other.data.matrix_floats[15], other.data.matrix_floats[15]};
        #endif
        #if defined CLANG
        v16sf other_column_1 = __builtin_shufflevector(other.data.full_matrix, other.data.full_matrix, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3);
        v16sf other_column_2 = __builtin_shufflevector(other.data.full_matrix, other.data.full_matrix, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7);
        v16sf other_column_3 = __builtin_shufflevector(other.data.full_matrix, other.data.full_matrix, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11);
        v16sf other_column_4 = __builtin_shufflevector(other.data.full_matrix, other.data.full_matrix, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15);
        #endif

        matrix_data multiplied_column_1;
        matrix_data multiplied_column_2;
        matrix_data multiplied_column_3;
        matrix_data multiplied_column_4;

        multiplied_column_1.full_matrix = data.full_matrix * other_column_1;
        multiplied_column_2.full_matrix = data.full_matrix * other_column_2;
        multiplied_column_3.full_matrix = data.full_matrix * other_column_3;
        multiplied_column_4.full_matrix = data.full_matrix * other_column_4;

        data.matrix_vectors[0] = multiplied_column_1.matrix_vectors[0] + multiplied_column_1.matrix_vectors[1] + multiplied_column_1.matrix_vectors[2] + multiplied_column_1.matrix_vectors[3];
        data.matrix_vectors[1] = multiplied_column_2.matrix_vectors[0] + multiplied_column_2.matrix_vectors[1] + multiplied_column_2.matrix_vectors[2] + multiplied_column_2.matrix_vectors[3];
        data.matrix_vectors[2] = multiplied_column_3.matrix_vectors[0] + multiplied_column_3.matrix_vectors[1] + multiplied_column_3.matrix_vectors[2] + multiplied_column_3.matrix_vectors[3];
        data.matrix_vectors[3] = multiplied_column_4.matrix_vectors[0] + multiplied_column_4.matrix_vectors[1] + multiplied_column_4.matrix_vectors[2] + multiplied_column_4.matrix_vectors[3];

        return *this;
    }

    v4sf matrix::get_row(const int y)
    {
#if defined CLANG
//indicies for builtin shufflevector must be constant
        if (y == 0)
            return __builtin_shufflevector(data.full_matrix, data.full_matrix, 0+0, 4+0, 8+0, 12+0);
        else if (y == 1)
            return __builtin_shufflevector(data.full_matrix, data.full_matrix, 0+1, 4+1, 8+1, 12+1);
        else if (y == 2)
            return __builtin_shufflevector(data.full_matrix, data.full_matrix, 0+2, 4+2, 8+2, 12+2);
        else if (y == 3)
            return __builtin_shufflevector(data.full_matrix, data.full_matrix, 0+3, 4+3, 8+3, 12+3);
#endif
#if defined GCC
        v4sf mask = {(float)y, (float)y+4, (float)y+8, (float)y+12};
        //return manual_swizzle<v16sf, v4sf, float>(data.full_matrix, mask);
        return manual_swizzle<float>(data.full_matrix, mask);
#endif
        v4sf ret = {-1.0f,-1.0f,-1.0f,-1.0f};
        return ret;
    }

    /** 
     * translate the matrix on the x axis
     *
     * @param x number of units to move along the x axis
     *
     * a reference to this matrix with the operation applied
     */ 
    matrix& matrix::translateX(const float x)
    {
        matrix translate_matrix;
        translate_matrix(3,0) = x;
        *this *= translate_matrix;
        return *this;
    }

    /** 
     * translate the matrix on the y axis
     *
     * @param y number of units to move along the y axis
     *
     * a reference to this matrix with the operation applied
     */ 
    matrix& matrix::translateY(const float y)
    {
        matrix translate_matrix;
        translate_matrix(3,1) = y;
        *this *= translate_matrix;
        return *this;
    }

    /** 
     * translate the matrix on the z axis
     *
     * @param z number of units to move along the z axis
     *
     * a reference to this matrix with the operation applied
     */ 
    matrix& matrix::translateZ(const float z)
    {
        matrix translate_matrix;
        translate_matrix(3,2) = z;
        *this *= translate_matrix;
        return *this;
    }

    /** 
     * rotate the matrix on the x axis
     *
     * @param h number of units to rotate along the x axis
     *
     * a reference to this matrix with the operation applied
     */ 
    matrix& matrix::rotateH(const float h)
    {
        matrix translate_matrix;
        //translate_matrix.get_index(0,0)
        float cos_angle = cos(h);
        float sin_angle = sin(h);
        translate_matrix(1,1) = cos_angle;
        translate_matrix(2,2) = cos_angle;
        translate_matrix(2,1) = -sin_angle;
        translate_matrix(1,2) = sin_angle;
        *this *= translate_matrix;
        return *this;
    }

    /** 
     * rotate the matrix on the y axis
     *
     * @param p number of units to rotate along the y axis
     *
     * a reference to this matrix with the operation applied
     */ 
    matrix& matrix::rotateP(const float p)
    {
        matrix translate_matrix;
        float cos_angle = cos(p);
        float sin_angle = sin(p);
        translate_matrix(0,0) = cos_angle;
        translate_matrix(2,2) = cos_angle;
        translate_matrix(2,0) = sin_angle;
        translate_matrix(0,2) = -sin_angle;
        *this *= translate_matrix;
        return *this;
    }

    /** 
     * rotate the matrix on the z axis
     *
     * @param r number of units to rotate along the z axis
     *
     * @return a pointer to the new matrix, to to which this matrix now points
     */ 
    matrix& matrix::rotateR(const float r)
    {
        matrix translate_matrix;
        float cos_angle = cos(r);
        float sin_angle = sin(r);
        translate_matrix(0,0) = cos_angle;
        translate_matrix(1,1) = cos_angle;
        translate_matrix(1,0) = -sin_angle;
        translate_matrix(0,1) = sin_angle;
        *this *= translate_matrix;
        return *this;
    }

    /** 
     * scale the matrix along every axis
     *
     * @param s magnitude of the scale
     *
     * a reference to this matrix with the operation applied
     */
    matrix& matrix::scale(const float s)
    {
        matrix translate_matrix;
        translate_matrix(0,0) = s;
        translate_matrix(1,1) = s;
        translate_matrix(2,2) = s;
        *this *= translate_matrix;
        return *this;
    }

    /** 
     * translate the matrix along the x, y, and z axes
     *
     * @param x number of units to translate along the x axis
     * @param y number of units to translate along the y axis
     * @param z number of units to translate along the z axis
     *
     * a reference to this matrix with the operation applied
     */
    matrix& matrix::translateXYZ(const float x, const float y, const float z)
    {
        matrix translate_matrix;
        translate_matrix(3,0) = x;
        translate_matrix(3,1) = y;
        translate_matrix(3,2) = z;
        *this *= translate_matrix;
        return *this;
    }

    /** 
     * translate the matrix along the x, y, and z axes
     *
     * @param vec a vector containing the x, y, and z magnitudes
     *
     * a reference to this matrix with the operation applied
     */
    matrix& matrix::translateXYZ(const vec & vec)
    {
        matrix translate_matrix;
        translate_matrix(3,0) = vec[0];
        translate_matrix(3,1) = vec[1];
        translate_matrix(3,2) = vec[2];
        *this *= translate_matrix;
        return *this;
    }

    /** 
     * rotate the matrix by the x, y, and z axes
     *
     * @param h number of radians to rotate along the x axis
     * @param p number of radians to rotate along the y axis
     * @param r number of radians to rotate along the z axis
     *
     * a reference to this matrix with the operation applied
     */
    matrix& matrix::rotateHPR(const float h, const float p, const float r)
    {
        matrix translate_matrix;
        float cos_h = cos(h);
        float sin_h = sin(h);
        float cos_p = cos(p);
        float sin_p = sin(p);
        float cos_r = cos(r);
        float sin_r = sin(r);
        //phi x h
        //theta y p
        //psi z r
        translate_matrix(0,0) = cos_p * cos_r;
        translate_matrix(1,0) = (-1*cos_h)*sin_r + sin_h*sin_p*cos_r;
        translate_matrix(2,0) = sin_h*sin_r + cos_h*sin_p*cos_r;
        translate_matrix(0,1) = cos_p*sin_r;
        translate_matrix(1,1) = cos_h*cos_r + sin_h*sin_p*sin_r;
        translate_matrix(2,1) = (-1*sin_h)*cos_r + cos_h*sin_p*sin_r;
        translate_matrix(0,2) = -1*sin_p;
        translate_matrix(1,2) = sin_h*cos_p;
        translate_matrix(2,2) = cos_h*cos_p;
        *this *= translate_matrix;
        return *this;
    }

    /** 
     * scale the matrix, rotate it on the x, y, and z axes, and then translate it along the x, y, and z axes
     *
     * @param x number of units to translate along the x axis
     * @param y number of units to translate along the y axis
     * @param z number of units to translate along the z axis
     * @param h number of radians to rotate along the x axis
     * @param p number of radians to rotate along the y axis
     * @param r number of radians to rotate along the z axis
     * @param s magnitude to scale the matrix
     *
     * a reference to this matrix with the operation applied
     */
    matrix& matrix::transformXYZHPRS(const float x, const float y, const float z, const float h, const float p, const float r, const float s)
    {
        scale(s);
        rotateHPR(h,p,r);
        return translateXYZ(x,y,z);
    }

    /** 
     * Transpose the current matrix in place
     *
     *
     * @return a reference to the current matrix
     */
    matrix& matrix::transpose()
    {
        #if defined GCC
        v16sf tmp = {data.matrix_floats[0], data.matrix_floats[4], data.matrix_floats[8], data.matrix_floats[12], data.matrix_floats[1], data.matrix_floats[5], data.matrix_floats[9], data.matrix_floats[13], data.matrix_floats[2], data.matrix_floats[6], data.matrix_floats[10], data.matrix_floats[14], data.matrix_floats[3], data.matrix_floats[7], data.matrix_floats[11], data.matrix_floats[15]};
        data.full_matrix = tmp;
        #endif
        #if defined CLANG
        data.full_matrix = __builtin_shufflevector(data.full_matrix, data.full_matrix, 0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15);
        #endif
        return *this;
    }

    quaternion matrix::to_quaternion()
    {
        quaternion ret;
        float q[4] = {0,0,0,0};
        int nxt[3] = {1,2,0};
        float trace = (*this)(0,0) + (*this)(1,1) + (*this)(2,2);
        if (trace > 0)
        {
            float s = sqrt(trace+1);
            ret.w() = s/2;
            s = .5/s;
            ret.x() = ((*this)(1,2) - (*this)(2,1)) * s;
            ret.y() = ((*this)(2,0) - (*this)(0,2)) * s;
            ret.z() = ((*this)(0,1) - (*this)(1,0)) * s;
        } else {
            int i = 0;
            if ((*this)(1,1) > (*this)(0,0))
                i = 1;
            if ((*this)(2,2) > (*this)(i,i))
                i = 2;
            int j = nxt[i];
            int k = nxt[j];
            float s = sqrt(((*this)(i,i) - ((*this)(j,j) + (*this)(k,k))) + 1);
            q[i] = s * .5;
            if (s != 0)
                s = .5 / s;
            ret.w() = ((*this)(k,j) - (*this)(j,k)) * s;
            q[j] = ((*this)(j,i) + (*this)(i,j)) * s;
            q[k] = ((*this)(k,i) + (*this)(i,k)) * s;
            ret.x() = q[0];
            ret.y() = q[1];
            ret.z() = q[2];
        }
        return ret;
    }

    float matrix::determinate()
    {
         // 0*5*10*15 + 0*9*14*7 + 0*13*6*11
         // +4*1*14*11 + 4*9*2*15 + 4*13*10*3
         // +8*1*6*15 + 8*5*14*3 + 8*13*2*7
         // +12*1*10*7 + 12*5*2*11 + 12*9*6*3
         // -0*5*14*11 - 0*9*6*15 - 0*13*10*7
         // -4*1*10*15 - 4*9*14*3 - 4*13*2*11
         // -8*1*14*7 - 8*5*2*15 - 8*13*6*3
         // -12*1*6*11 - 12*5*10*3 - 12*9*2*7
        float ret;
        ret = (*this)[0]*(*this)[5]*(*this)[10]*(*this)[15] + (*this)[0]*(*this)[9]*(*this)[14]*(*this)[7] + (*this)[0]*(*this)[13]*(*this)[6]*(*this)[11]+(*this)[4]*(*this)[1]*(*this)[14]*(*this)[11] + (*this)[4]*(*this)[9]*(*this)[2]*(*this)[15] + (*this)[4]*(*this)[13]*(*this)[10]*(*this)[3]+(*this)[8]*(*this)[1]*(*this)[6]*(*this)[15] + (*this)[8]*(*this)[5]*(*this)[14]*(*this)[3] + (*this)[8]*(*this)[13]*(*this)[2]*(*this)[7]+(*this)[12]*(*this)[1]*(*this)[10]*(*this)[7] + (*this)[12]*(*this)[5]*(*this)[2]*(*this)[11] + (*this)[12]*(*this)[9]*(*this)[6]*(*this)[3]-(*this)[0]*(*this)[5]*(*this)[14]*(*this)[11] - (*this)[0]*(*this)[9]*(*this)[6]*(*this)[15] - (*this)[0]*(*this)[13]*(*this)[10]*(*this)[7]-(*this)[4]*(*this)[1]*(*this)[10]*(*this)[15] - (*this)[4]*(*this)[9]*(*this)[14]*(*this)[3] - (*this)[4]*(*this)[13]*(*this)[2]*(*this)[11]-(*this)[8]*(*this)[1]*(*this)[14]*(*this)[7] - (*this)[8]*(*this)[5]*(*this)[2]*(*this)[15] - (*this)[8]*(*this)[13]*(*this)[6]*(*this)[3] -(*this)[12]*(*this)[1]*(*this)[6]*(*this)[11] - (*this)[12]*(*this)[5]*(*this)[10]*(*this)[3] - (*this)[12]*(*this)[9]*(*this)[2]*(*this)[7];

        return ret;
    }

    matrix matrix::inverse()
    {
        // b0 = 5*10*15 + 9*14*7 + 13*6*11 - 5*14*11 - 9*6*15 - 13*10*7
        // b4 = 4*14*11 + 8*6*15 + 12*10*7 - 4*10*15 - 8*14*7 - 12*6*11
        // b8 = 4*9*15 + 8*13*7 + 12*5*11 - 4*13*11 - 8*5*15 - 12*9*7
        // b12 = 4*13*10 + 8*5*14 + 12*9*6 - 4*9*14 - 8*13*6 - 12*5*10
        // b1 = 1*14*11 + 9*2*15 + 13*10*3 - 1*10*15 - 9*14*3 - 13*2*11
        // b5 = 0*10*15 + 8*14*3 + 12*2*11 - 0*14*11 - 8*2*15 - 12*10*3
        // b9 = 0*13*11 + 8*1*15 + 12*9*3 - 0*9*15 - 8*13*3 - 12*1*11
        // b13 = 0*9*14 + 8*13*2 + 12*1*10 - 0*13*10 - 8*1*14 - 12*9*2
        // b2 = 1*6*15 + 5*14*3 + 13*2*7 - 1*14*7 - 5*2*15 - 13*6*3
        // b6 = 0*14*7 + 4*2*15 + 12*6*3 - 0*6*15 - 4*14*3 - 12*2*7
        // b10 = 0*5*15 + 4*13*3 + 12*1*7 - 0*13*7 - 4*1*15 - 12*5*3
        // b14 = 0*13*6 + 4*1*14 + 12*5*2 - 0*5*14 - 4*13*2 - 12*1*6
        // b3 = 1*10*7 + 5*2*11 + 9*6*3 - 1*6*11 - 5*10*3 - 9*2*7
        // b7 = 0*6*11 + 4*10*3 + 8*2*7 - 0*10*7 - 4*2*11 - 8*6*3
        // b11 = 0*9*7 + 4*1*11 + 8*5*3 - 0*5*11 - 4*9*3 - 8*1*7
        // b15 = 0*5*10 + 4*9*2 + 8*1*6 - 0*9*6 - 4*1*10 - 8*5*2

        float scalar = 1.0f / determinate();

        matrix ret;

        ret[0] = (*this)[5]*(*this)[10]*(*this)[15] + (*this)[9]*(*this)[14]*(*this)[7] + (*this)[13]*(*this)[6]*(*this)[11] - (*this)[5]*(*this)[14]*(*this)[11] - (*this)[9]*(*this)[6]*(*this)[15] - (*this)[13]*(*this)[10]*(*this)[7];
        ret[4] = (*this)[4]*(*this)[14]*(*this)[11] + (*this)[8]*(*this)[6]*(*this)[15] + (*this)[12]*(*this)[10]*(*this)[7] - (*this)[4]*(*this)[10]*(*this)[15] - (*this)[8]*(*this)[14]*(*this)[7] - (*this)[12]*(*this)[6]*(*this)[11];
        ret[8] = (*this)[4]*(*this)[9]*(*this)[15] + (*this)[8]*(*this)[13]*(*this)[7] + (*this)[12]*(*this)[5]*(*this)[11] - (*this)[4]*(*this)[13]*(*this)[11] - (*this)[8]*(*this)[5]*(*this)[15] - (*this)[12]*(*this)[9]*(*this)[7];
        ret[12] = (*this)[4]*(*this)[13]*(*this)[10] + (*this)[8]*(*this)[5]*(*this)[14] + (*this)[12]*(*this)[9]*(*this)[6] - (*this)[4]*(*this)[9]*(*this)[14] - (*this)[8]*(*this)[13]*(*this)[6] - (*this)[12]*(*this)[5]*(*this)[10];
        ret[1] = (*this)[1]*(*this)[14]*(*this)[11] + (*this)[9]*(*this)[2]*(*this)[15] + (*this)[13]*(*this)[10]*(*this)[3] - (*this)[1]*(*this)[10]*(*this)[15] - (*this)[9]*(*this)[14]*(*this)[3] - (*this)[13]*(*this)[2]*(*this)[11];
        ret[5] = (*this)[0]*(*this)[10]*(*this)[15] + (*this)[8]*(*this)[14]*(*this)[3] + (*this)[12]*(*this)[2]*(*this)[11] - (*this)[0]*(*this)[14]*(*this)[11] - (*this)[8]*(*this)[2]*(*this)[15] - (*this)[12]*(*this)[10]*(*this)[3];
        ret[9] = (*this)[0]*(*this)[13]*(*this)[11] + (*this)[8]*(*this)[1]*(*this)[15] + (*this)[12]*(*this)[9]*(*this)[3] - (*this)[0]*(*this)[9]*(*this)[15] - (*this)[8]*(*this)[13]*(*this)[3] - (*this)[12]*(*this)[1]*(*this)[11];
        ret[13] = (*this)[0]*(*this)[9]*(*this)[14] + (*this)[8]*(*this)[13]*(*this)[2] + (*this)[12]*(*this)[1]*(*this)[10] - (*this)[0]*(*this)[13]*(*this)[10] - (*this)[8]*(*this)[1]*(*this)[14] - (*this)[12]*(*this)[9]*(*this)[2];
        ret[2] = (*this)[1]*(*this)[6]*(*this)[15] + (*this)[5]*(*this)[14]*(*this)[3] + (*this)[13]*(*this)[2]*(*this)[7] - (*this)[1]*(*this)[14]*(*this)[7] - (*this)[5]*(*this)[2]*(*this)[15] - (*this)[13]*(*this)[6]*(*this)[3];
        ret[6] = (*this)[0]*(*this)[14]*(*this)[7] + (*this)[4]*(*this)[2]*(*this)[15] + (*this)[12]*(*this)[6]*(*this)[3] - (*this)[0]*(*this)[6]*(*this)[15] - (*this)[4]*(*this)[14]*(*this)[3] - (*this)[12]*(*this)[2]*(*this)[7];
        ret[10] = (*this)[0]*(*this)[5]*(*this)[15] + (*this)[4]*(*this)[13]*(*this)[3] + (*this)[12]*(*this)[1]*(*this)[7] - (*this)[0]*(*this)[13]*(*this)[7] - (*this)[4]*(*this)[1]*(*this)[15] - (*this)[12]*(*this)[5]*(*this)[3];
        ret[14] = (*this)[0]*(*this)[13]*(*this)[6] + (*this)[4]*(*this)[1]*(*this)[14] + (*this)[12]*(*this)[5]*(*this)[2] - (*this)[0]*(*this)[5]*(*this)[14] - (*this)[4]*(*this)[13]*(*this)[2] - (*this)[12]*(*this)[1]*(*this)[6];
        ret[3] = (*this)[1]*(*this)[10]*(*this)[7] + (*this)[5]*(*this)[2]*(*this)[11] + (*this)[9]*(*this)[6]*(*this)[3] - (*this)[1]*(*this)[6]*(*this)[11] - (*this)[5]*(*this)[10]*(*this)[3] - (*this)[9]*(*this)[2]*(*this)[7];
        ret[7] = (*this)[0]*(*this)[6]*(*this)[11] + (*this)[4]*(*this)[10]*(*this)[3] + (*this)[8]*(*this)[2]*(*this)[7] - (*this)[0]*(*this)[10]*(*this)[7] - (*this)[4]*(*this)[2]*(*this)[11] - (*this)[8]*(*this)[6]*(*this)[3];
        ret[11] = (*this)[0]*(*this)[9]*(*this)[7] + (*this)[4]*(*this)[1]*(*this)[11] + (*this)[8]*(*this)[5]*(*this)[3] - (*this)[0]*(*this)[5]*(*this)[11] - (*this)[4]*(*this)[9]*(*this)[3] - (*this)[8]*(*this)[1]*(*this)[7];
        ret[15] = (*this)[0]*(*this)[5]*(*this)[10] + (*this)[4]*(*this)[9]*(*this)[2] + (*this)[8]*(*this)[1]*(*this)[6] - (*this)[0]*(*this)[9]*(*this)[6] - (*this)[4]*(*this)[1]*(*this)[10] - (*this)[8]*(*this)[5]*(*this)[2];

        ret *= scalar;

        return ret;
    }

    matrix matrix::get_upper_three()
    {
        matrix ret(*this);
        ret(3,0) = 0.0f;
        ret(3,1) = 0.0f;
        ret(3,2) = 0.0f;
        ret(3,3) = 1.0f;
        ret(0,3) = 0.0f;
        ret(1,3) = 0.0f;
        ret(2,3) = 0.0f;
        return ret;
    }

    matrix slerp(matrix & a, matrix & b, float scale)
    {
        return slerp(a.to_quaternion(), b.to_quaternion(), scale);
    }
    matrix slerp(const quaternion & a, const quaternion & b, float scale)
    {
        float theta = a.dot(b);
        theta = acos(theta);
        float wp = sin((1-scale)*theta)/sin(theta);
        float wq = sin(scale*theta)/sin(theta);
        quaternion new_quat = a*wp + b*wq;
        return new_quat.to_matrix();
    }

    quaternion quat_slerp(const quaternion & a, const quaternion & b, float scale)
    {
        float theta = a.dot(b);
        theta = acos(theta);
        float wp = sin((1-scale)*theta)/sin(theta);
        float wq = sin(scale*theta)/sin(theta);
        quaternion new_quat = a*wp + b*wq;
        return new_quat;
    }

    matrix perspective(float angle_of_view, float aspect_ratio, float near, float far)
    {
        matrix ret;
        angle_of_view = (angle_of_view/180.f) * PI;
        float d = 1/tan(angle_of_view/2);
        ret(0,0) = d/aspect_ratio;
        ret(1,1) = d;
        ret(2,2) = (near+far)/(near-far);
        ret(3,2) = (2*near*far)/(near-far);
        ret(2,3) = -1.0f;
        ret(3,3) = 0.0f;
        return ret;
    }

    matrix look_at(const point & position, const point & target, const vec & up_direction)
    {
        matrix E;
        E(3,0) = -position.x();
        E(3,1) = -position.y();
        E(3,2) = -position.z();

        vec view_direction = normalize(target-position);
        vec view_right = normalize(view_direction.cross(normalize(up_direction)));
        vec view_up = normalize(view_right.cross(view_direction));
        
        matrix mat;
        mat.data.matrix_vectors[0] = (view_right.sanitize()).data.full_vector;
        mat.data.matrix_vectors[1] = (view_up.sanitize()).data.full_vector;
        mat.data.matrix_vectors[2] = -((view_direction.sanitize()).data.full_vector);
        
        return (mat.transpose() * E);
    }
}
