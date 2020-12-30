#pragma once

//#define LMB_DOUBLE

#include <glm/glm.hpp>
#include <memory>
#include <vector>



namespace LMB
{

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;



#ifdef LMB_DOUBLE

    typedef double real_t;
    typedef glm::dvec2 vec2;
    typedef glm::dvec3 vec3;
    typedef glm::dvec4 vec4;

#else

    typedef float real_t;
    typedef glm::vec2 vec2;
    typedef glm::vec3 vec3;
    typedef glm::vec4 vec4;

#endif


constexpr real_t to_real(const real_t r)
{
    return r;
}





template<typename T>
constexpr T NegModul(const T &a,const T &b)
{
    if(a<0)
    {
        return b + ( a % b );
    }

    return ( a % b );
}



}//namespace LMB