#pragma once

#include "default_solver.h"

namespace LMB
{

class AABBBaseSolver : public DefaultSolver
{

public:

    /**
     * @brief find the bounds of the world
     */
    const AABB3D GenWorldAABB() const;

    const bool IntersectAABB3D(const Ray &ray,const AABB3D &bbox,real_t &outt)const;

};

}