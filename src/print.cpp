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
#include "print.h"

namespace topaz
{

    void print(const glm::vec3 & data, std::ostream & out, int indentation)
    {
        out << std::string(indentation*4, ' ') << "VEC3:" << std::endl;
        for (int y = 0; y < 3; ++y)
        {
            if (y != 0)
                out << "\t";
            out << std::string(indentation*4+2, ' ') << std::setw(8) << data[y];
        }
        out << std::endl;
    }

    void print(const glm::vec4 & data, std::ostream & out, int indentation)
    {
        out << std::string(indentation*4, ' ') << "VEC4:" << std::endl;
        for (int y = 0; y < 4; ++y)
        {
            if (y != 0)
                out << "\t";
            out << std::string(indentation*4+2, ' ') << std::setw(8) << data[y];
        }
        out << std::endl;
    }

    void print(const glm::mat4 & data, std::ostream & out, int indentation)
    {
        out << std::string(indentation*4, ' ') << "MATRIX:" << std::endl;
        for (int y = 0; y < 4; ++y)
        {
            for (int x = 0; x < 4; ++x)
            {
                if (x != 0)
                    out << "\t";
                out << std::string(indentation*4+2, ' ') << std::setw(8) << data[x][y];
            }
            out << std::endl;
        }
    }

    void print_matrix(const float* data, std::ostream & out, int indentation)
    {
        out << std::string(indentation*4, ' ') << "MATRIX:" << std::endl;
        for (int y = 0; y < 4; ++y)
        {
            for (int x = 0; x < 4; ++x)
            {
                if (x != 0)
                    out << "\t";
                out << std::string(indentation*4+2, ' ') << std::setw(8) << data[x*4+y];
            }
            out << std::endl;
        }
    }

    void print(const glm::quat & data, std::ostream & out, int indentation)
    {
        out << std::string(indentation*4, ' ') << "Quaternion:\n";
        out << std::string(indentation*4 + 2, ' ') << data.w << " " << data.x << "i " << data.y << "j " << data.z << "k\n";
    }

    void print_floats(const float* data, size_t size, std::ostream & out, int indentation)
    {
        out << std::string(indentation*4 + 2, ' ');
        for (size_t i = 0; i < size; ++i)
        {
            if (i != 0)
                out << "  ";
            out << data[i];
        }
        out << "\n";
    }
    
    void print_ints(const int* data, size_t size, std::ostream & out, int indentation)
    {
        out << std::string(indentation*4 + 2, ' ');
        for (size_t i = 0; i < size; ++i)
        {
            if (i != 0)
                out << "  ";
            out << data[i];
        }
        out << "\n";
    }
}
