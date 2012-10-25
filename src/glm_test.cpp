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
#include "topaz.h"
#include "point.h"
#include "matrix.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <tuple>

using namespace std;

int main(int argc, char** argv)
{
    {
        topaz::quaternion p(topaz::vec(1,2,3), TO_RADIANS(90));
        topaz::quaternion q(topaz::vec(0,0,1), TO_RADIANS(180));
        p.print();
        q.print();
        topaz::point a(1,0,0);
        a.print();
        (p*a).print();
        topaz::quaternion pq = p*q;
        pq.print();
        topaz::vec axis;
        float angle;
        tie(axis, angle) = pq.get_axis_angle();
        cout << "angle: " << TO_DEGREES(angle) << "\n";
        axis.print();
    }
    return 0;
}
