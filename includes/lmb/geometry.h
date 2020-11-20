#pragma once 


#include "base_type.h"
#include "aabbox.h"
#include "ray.h"

#define GLM_FORCE_INLINE 
#include <glm/gtx/component_wise.hpp>
#undef GLM_FORCE_INLINE 

#include <array>


namespace LMB
{


inline const bool PointInsideBox(const vec3 &point,const vec3 &min,const vec3 &max)
{
    return  (point.x >= min.x && point.x <= max.x) &&
            (point.y >= min.y && point.y <= max.y) &&
            (point.z >= min.z && point.z <= max.z);
}





inline const real_t RayBoxIntersect(
    const vec3 &rpos,
    const vec3 &rdir,
    const vec3 &vmin,
    const vec3 &vmax)
{
    const real_t t1 = (vmin.x - rpos.x) / rdir.x;
    const real_t t2 = (vmax.x - rpos.x) / rdir.x;
    const real_t t3 = (vmin.y - rpos.y) / rdir.y;
    const real_t t4 = (vmax.y - rpos.y) / rdir.y;
    const real_t t5 = (vmin.z - rpos.z) / rdir.z;
    const real_t t6 = (vmax.z - rpos.z) / rdir.z;
 
    const real_t aMin = t1 < t2 ? t1 : t2;
    const real_t bMin = t3 < t4 ? t3 : t4;
    const real_t cMin = t5 < t6 ? t5 : t6;
 
    const real_t aMax = t1 > t2 ? t1 : t2;
    const real_t bMax = t3 > t4 ? t3 : t4;
    const real_t cMax = t5 > t6 ? t5 : t6;
 
    const real_t fMax = aMin > bMin ? aMin : bMin;
    const real_t fMin = aMax < bMax ? aMax : bMax;

    const real_t t7 = fMax > cMin ? fMax : cMin;
    const real_t t8 = fMin < cMax ? fMin : cMax;

    const real_t t9 = (t8 < 0 || t7 > t8) ? -1 : t7;

    return t9;
}

inline const real_t RayBoxIntersectFast(
    const Ray &ray,
    const AABB3D &aabb)
{

    const vec3 &rpos = ray.GetStart();
    const vec3 &rdir = ray.GetDir();
    const vec3 &vmin = aabb.GetMin();
    const vec3 &vmax = aabb.GetMax();

    const real_t t1 = (vmin.x - rpos.x) / rdir.x;
    const real_t t2 = (vmax.x - rpos.x) / rdir.x;
    const real_t t3 = (vmin.y - rpos.y) / rdir.y;
    const real_t t4 = (vmax.y - rpos.y) / rdir.y;
    const real_t t5 = (vmin.z - rpos.z) / rdir.z;
    const real_t t6 = (vmax.z - rpos.z) / rdir.z;
 
    const real_t aMin = t1 < t2 ? t1 : t2;
    const real_t bMin = t3 < t4 ? t3 : t4;
    const real_t cMin = t5 < t6 ? t5 : t6;
 
    const real_t aMax = t1 > t2 ? t1 : t2;
    const real_t bMax = t3 > t4 ? t3 : t4;
    const real_t cMax = t5 > t6 ? t5 : t6;
 
    const real_t fMax = aMin > bMin ? aMin : bMin;
    const real_t fMin = aMax < bMax ? aMax : bMax;

    const real_t t7 = fMax > cMin ? fMax : cMin;
    const real_t t8 = fMin < cMax ? fMin : cMax;

    const real_t t9 = (t8 < 0 || t7 > t8) ? -1 : t7;

    return t9;
}


inline const real_t RayBoxIntersectFast2(
    const Ray &ray,
    const AABB3D &aabb)
{

    const vec3 &rpos = ray.GetStart();
    const vec3 &rdir = ray.GetDir();
    const vec3 &vmin = aabb.GetMin();
    const vec3 &vmax = aabb.GetMax();

    const vec3 a = (vmin - rpos) / rdir;
    const vec3 b = (vmax - rpos) / rdir;

    const vec3 min(
        glm::min(a[0],b[0]),
        glm::min(a[1],b[1]),
        glm::min(a[2],b[2]));

    const vec3 max(
        glm::max(a[0],b[0]),
        glm::max(a[1],b[1]),
        glm::max(a[2],b[2]));

    const real_t t7 = glm::compMax(min);
    const real_t t8 = glm::compMin(max);

    const real_t t9 = (t8 < 0 || t7 > t8) ? -1 : t7;

    return t9;
}

inline const bool RayBoxIntersecFastBool(
    const vec3 &rpos,
    const vec3 &rdir,
    const vec3 &vmin,
    const vec3 &vmax,
    real_t &out_t)
{
    vec3 dirfrac;
    // r.dir is unit direction vector of ray
    dirfrac.x = to_real(1.0) / rdir.x;
    dirfrac.y = to_real(1.0) / rdir.y;
    dirfrac.z = to_real(1.0) / rdir.z;
    // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
    // r.org is origin of ray
    real_t t1 = (vmin.x - rpos.x)*dirfrac.x;
    real_t t2 = (vmax.x - rpos.x)*dirfrac.x;
    real_t t3 = (vmin.y - rpos.y)*dirfrac.y;
    real_t t4 = (vmax.y - rpos.y)*dirfrac.y;
    real_t t5 = (vmin.z - rpos.z)*dirfrac.z;
    real_t t6 = (vmax.z - rpos.z)*dirfrac.z;

    real_t tmin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));
    real_t tmax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
    if (tmax < 0)
    {
        out_t = tmax;
        return false;
    }

    // if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax)
    {
        out_t = tmax;
        return false;
    }

    out_t = tmin;
    return true;
}

//source: https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
inline const bool Intersect2AABB3D(const AABB3D &a,const AABB3D &b)
{
  return (a.GetMin().x <= b.GetMax().x && a.GetMax().x >= b.GetMin().x) &&
         (a.GetMin().y <= b.GetMax().y && a.GetMax().y >= b.GetMin().y) &&
         (a.GetMin().z <= b.GetMax().z && a.GetMax().z >= b.GetMin().z);
}





//source: https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
inline const bool Intersect2AABB2D(const AABB2D &a,const AABB2D &b)
{
  return (a.GetMin().x <= b.GetMax().x && a.GetMax().x >= b.GetMin().x) &&
         (a.GetMin().y <= b.GetMax().y && a.GetMax().y >= b.GetMin().y);
}





#define MOLLER_TRUMBORE
#define CULLING
const real_t kEpsilon = to_real(1.0) / to_real(1024*16);

inline bool rayTriangleIntersect( 
    const vec3 &orig,
    const vec3 &dir, 
    const vec3 &v0,
    const vec3 &v1,
    const vec3 &v2, 
    real_t &t,
    real_t &u,
    real_t &v) 
{ 
#ifdef MOLLER_TRUMBORE 
    vec3 v0v1 = v1 - v0; 
    vec3 v0v2 = v2 - v0; 
    vec3 pvec = glm::cross(dir,v0v2); 
    real_t det = glm::dot(v0v1,pvec); 
#ifdef CULLING 
    // if the determinant is negative the triangle is backfacing
    // if the determinant is close to 0, the ray misses the triangle
    if (det < kEpsilon) return false; 
#else 
    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < kEpsilon) return false; 
#endif 
    real_t invDet = 1 / det; 
 
    vec3 tvec = orig - v0; 
    u = glm::dot(tvec,pvec) * invDet; 
    if (u < to_real(0) || u > to_real(1))
        return false; 
 
    vec3 qvec = glm::cross(tvec,v0v1); 
    v = glm::dot(dir,qvec) * invDet; 
    if (v < to_real(0) || u + v > to_real(1))
        return false; 
 
    t = glm::dot(v0v2,qvec) * invDet; 
 
    return true; 
#else  
#endif 
} 





/*source:
https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
*/
static inline void Barycentric(
    const vec2 &p, 
    const vec2 &a,
    const vec2 &b,
    const vec2 &c,
    real_t &out_u,
    real_t &out_v,
    real_t &out_w)
{
    vec2 v0 = b - a, v1 = c - a, v2 = p - a;
    real_t d00 = glm::dot(v0, v0);
    real_t d01 = glm::dot(v0, v1);
    real_t d11 = glm::dot(v1, v1);
    real_t d20 = glm::dot(v2, v0);
    real_t d21 = glm::dot(v2, v1);
    real_t denom = d00 * d11 - d01 * d01;
    out_v = (d11 * d20 - d01 * d21) / denom;
    out_w = (d00 * d21 - d01 * d20) / denom;
    out_u = to_real(1.0) - out_v - out_w;
}

/*source:
https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
*/
static inline void Barycentric(
    const vec2 &p, 
    const std::array<vec2,3> &triangle,
    vec3 &out_uvw)
{
    vec2 v0 = triangle[1] - triangle[0];
    vec2 v1 = triangle[2] - triangle[0];
    vec2 v2 = p - triangle[0];

    real_t d00 = glm::dot(v0, v0);
    real_t d01 = glm::dot(v0, v1);
    real_t d11 = glm::dot(v1, v1);
    real_t d20 = glm::dot(v2, v0);
    real_t d21 = glm::dot(v2, v1);
    real_t denom = d00 * d11 - d01 * d01;
    out_uvw.y = (d11 * d20 - d01 * d21) / denom;
    out_uvw.z = (d00 * d21 - d01 * d20) / denom;
    out_uvw.x = to_real(1.0) - out_uvw.y - out_uvw.z;
}





//source: https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
inline const bool IntersectAABBPoint(const AABB2D &a,const vec2 &p)
{
  return 	(p.x >= a.GetMin().x && p.x <= a.GetMax().x)&&
  			(p.y >= a.GetMin().y && p.y <= a.GetMax().y);
}



}