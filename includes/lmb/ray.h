
#pragma once

#include "base_type.h"


namespace LMB
{


class Ray
{


public:

    Ray(const vec3& start,const vec3& end)
    : m_start(start)
    , m_end(end)
    {
    }

    Ray()
    : m_start(0)
    , m_end(0)
    {
    }

    inline const vec3& GetStart() const
    {
        return m_start;
    }

    inline const vec3& GetEnd() const
    {
        return m_end;
    }

    inline void SetStart(const vec3& start)
    {
        m_start = start;
    }

    inline void SetEnd(const vec3& end)
    {
        m_end = end;
    }

    inline const vec3 GetDir() const
    {
        return glm::normalize(m_end-m_start);
    }

    inline const vec3 GetVec() const
    {
        return m_end - m_start;
    }

    inline const real_t GetLength() const
    {
        return glm::length(m_end - m_start);
    }

protected:

    vec3 m_start;
    vec3 m_end;

};


}