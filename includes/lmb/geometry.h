#pragma once 


#include "base_type.h"
#include "aabbox.h"
#include "ray.h"

#include <glm/gtx/component_wise.hpp>

#include <array>

namespace LMB
{


inline const bool PointInsideBox(const vec3 &point,const vec3 &min,const vec3 &max)
{
    return  (point.x >= min.x && point.x <= max.x) &&
            (point.y >= min.y && point.y <= max.y) &&
            (point.z >= min.z && point.z <= max.z);
}

inline const bool PointInsideBox(const vec3 &point,const AABB3D &bbox)
{
    const vec3 min = bbox.GetMin();
    const vec3 max = bbox.GetMax();
    
    return  (point.x >= min.x && point.x <= max.x) &&
            (point.y >= min.y && point.y <= max.y) &&
            (point.z >= min.z && point.z <= max.z);
}

inline const bool PointInsideBox(const vec2 &point,const AABB2D &bbox)
{
    const vec2 min = bbox.GetMin();
    const vec2 max = bbox.GetMax();
    
    return  (point.x >= min.x && point.x <= max.x) &&
            (point.y >= min.y && point.y <= max.y);
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
    const vec3 &rinvdir = ray.GetInvDir();
    const vec3 &vmin = aabb.GetMin();
    const vec3 &vmax = aabb.GetMax();

    const real_t t1 = (vmin.x - rpos.x) * rinvdir.x;
    const real_t t2 = (vmax.x - rpos.x) * rinvdir.x;
    const real_t t3 = (vmin.y - rpos.y) * rinvdir.y;
    const real_t t4 = (vmax.y - rpos.y) * rinvdir.y;
    const real_t t5 = (vmin.z - rpos.z) * rinvdir.z;
    const real_t t6 = (vmax.z - rpos.z) * rinvdir.z;
 
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
    const vec3 &rinvdir = ray.GetInvDir();
    const vec3 &vmin = aabb.GetMin();
    const vec3 &vmax = aabb.GetMax();

    const vec3 a = (vmin - rpos) * rinvdir;
    const vec3 b = (vmax - rpos) * rinvdir;

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



//source: https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
inline const bool AABBRayIntersection(const Ray &r, const AABB3D &bb,real_t &out_t) 
{ 
    real_t tmin = (bb.GetMax().x - r.GetStart().x) / r.GetDir().x; 
    real_t tmax = (bb.GetMax().x - r.GetStart().x) / r.GetDir().x; 
 
    if (tmin > tmax) std::swap(tmin, tmax); 
 
    real_t tymin = (bb.GetMin().y - r.GetStart().y) / r.GetDir().y; 
    real_t tymax = (bb.GetMax().y - r.GetStart().y) / r.GetDir().y; 
 
    if (tymin > tymax) std::swap(tymin, tymax); 
 
    if ((tmin > tymax) || (tymin > tmax)) 
        return false; 
 
    if (tymin > tmin) 
        tmin = tymin; 
 
    if (tymax < tmax) 
        tmax = tymax; 
 
    real_t tzmin = (bb.GetMin().z - r.GetStart().z) / r.GetDir().z; 
    real_t tzmax = (bb.GetMax().z - r.GetStart().z) / r.GetDir().z; 
 
    if (tzmin > tzmax) std::swap(tzmin, tzmax); 
 
    if ((tmin > tzmax) || (tzmin > tmax)) 
        return false; 
 
    if (tzmin > tmin) 
        tmin = tzmin; 
 
    if (tzmax < tmax) 
        tmax = tzmax; 

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
    const vec3 v0v1 = v1 - v0; 
    const vec3 v0v2 = v2 - v0; 
    const vec3 pvec = glm::cross(dir,v0v2); 
    const real_t det = glm::dot(v0v1,pvec); 
#ifdef CULLING 
    // if the determinant is negative the triangle is backfacing
    // if the determinant is close to 0, the ray misses the triangle
    if (det < kEpsilon) return false; 
#else 
    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < kEpsilon) return false; 
#endif 
    const real_t invDet = to_real(1) / det; 
 
    const vec3 tvec = orig - v0; 
    u = glm::dot(tvec,pvec) * invDet; 
    if (u < to_real(0) || u > to_real(1))
        return false; 
 
    const vec3 qvec = glm::cross(tvec,v0v1); 
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
    const std::array<vec2,3> &triangle,
    vec3 &out_uvw)
{
    const vec2 v0 = triangle[1] - triangle[0];
    const vec2 v1 = triangle[2] - triangle[0];
    const vec2 v2 = p - triangle[0];

    const real_t d00 = glm::dot(v0, v0);
    const real_t d01 = glm::dot(v0, v1);
    const real_t d11 = glm::dot(v1, v1);
    const real_t d20 = glm::dot(v2, v0);
    const real_t d21 = glm::dot(v2, v1);
    const real_t denom = d00 * d11 - d01 * d01;
    const real_t inv_denom = to_real(1.0) / denom;
    out_uvw.y = (d11 * d20 - d01 * d21) * inv_denom;
    out_uvw.z = (d00 * d21 - d01 * d20) * inv_denom;
    out_uvw.x = to_real(1.0) - out_uvw.y - out_uvw.z;
}





//source: https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
inline const bool IntersectAABBPoint(const AABB2D &a,const vec2 &p)
{
  return 	(p.x >= a.GetMin().x && p.x <= a.GetMax().x)&&
  			(p.y >= a.GetMin().y && p.y <= a.GetMax().y);
}



}