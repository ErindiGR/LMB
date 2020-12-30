
#pragma once

#include "base_type.h"
#include "aabbox.h"


namespace LMB
{


class Ray
{


public:

    Ray(const vec3& start,const vec3& end)
    : m_start(start)
    , m_end(end)
    {
        GenDirAndLength();
    }

    Ray()
    : m_start(0)
    , m_end(0)
    , m_cache_dir(0)
    , m_cache_length(0)
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
        GenDirAndLength();
    }

    inline void SetEnd(const vec3& end)
    {
        m_end = end;
        GenDirAndLength();
    }

    inline const vec3 GetDir() const
    {
        return m_cache_dir;
    }

    inline const vec3 GetInvDir() const
    {
        return m_cache_invdir;
    }

    inline const vec3 GetVec() const
    {
        return m_end - m_start;
    }

    inline const real_t GetLength() const
    {
        return m_cache_length;
    }

    inline const AABB3D GetAABB() const
    {
        AABB3D ret;
        ret.SetMin(glm::min(m_start,m_end));
        ret.SetMax(glm::max(m_start,m_end));
        return ret;
    }

    inline void GenDir()
    {
        m_cache_dir = glm::normalize(m_end-m_start);
    }

    inline void GenLength()
    {
        m_cache_length = glm::length(m_end-m_start);
    }

    inline void GenDirAndLength()
    {
        const vec3 v = GetVec();
        m_cache_dir = glm::normalize(v);
        m_cache_invdir = to_real(1.0) / m_cache_dir;
        m_cache_length = glm::length(v);
    }

protected:

    vec3 m_start;
    vec3 m_end;
    
    vec3 m_cache_dir;
    vec3 m_cache_invdir;
    real_t m_cache_length;

};


}