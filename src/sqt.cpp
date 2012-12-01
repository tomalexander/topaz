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
