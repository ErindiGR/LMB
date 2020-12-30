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
    const AABB3D GenWorldAABB(const std::vector<Triangle> &triangles) const;
    const AABB3D GenWorldAABB(const std::vector<size_t> &triangles_indices) const;

    const bool IntersectAABB3D(const Ray &ray,const AABB3D &bbox,real_t &outt)const;

    std::vector<size_t> IntersectTriangleAABB(const AABB3D &bbox)const;
    std::vector<size_t> IntersectTriangleAABB(const AABB3D &bbox, const std::vector<size_t> &triangle_list)const;

};

}