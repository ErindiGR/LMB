#pragma once

#include "lmb/solver.h"
#include "lmb/aabbox.h"

#include <vector>



namespace LMB
{



/**
* @brief This solver tests each triangle if it is intersecting
* with the ray
*/
class DefaultSolver : public Solver
{

public:

    //Solver
    void Gen() override;
    const bool Intersect(const Ray &ray,SHitInfo &out_hit_info) const override;
    //!Solver

    const bool IntersectTriangle(
        const Ray &ray,
        const Triangle &tri,
        vec3 &out_uvw,
        real_t &outt) const;

    /**
     * @brief find the bounds of the world
     */
    const AABB3D GenWorldAABB() const;

protected:

};


}