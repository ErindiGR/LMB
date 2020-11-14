#pragma once

#include "base_type.h"

namespace LMB
{

template<class T>
class AABBox
{

public:


    inline const T& GetMin() const
    {
        return m_min;
    }

    inline const T& GetMax() const
    {
        return m_max;
    }

    inline void SetMin(const T &min)
    {
        m_min=min;
    }

    inline void SetMax(const T &max)
    {
        m_max=max;
    }

protected:

    T m_min;
    T m_max;

};


using AABB3D = AABBox<vec3>;
using AABB2D = AABBox<vec2>;

}