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
