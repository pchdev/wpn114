/*
 * =====================================================================================
 *
 *       Filename:  trigonometry.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12.11.2017 17:36:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>

namespace wpn114 {
namespace audio {
namespace utilities {

    inline void atoc2(float r, float th, float& res[2])
    {
        res[0] = r*cos(th);
        res[1] = r*sin(th);
    }

    inline void ctoa2(float x, float y, float& res[2])
    {
        res[0] = sqrt(x*x + y*y);
        res[1] = atan(y/x);
    }

}
}
}
