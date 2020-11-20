#pragma once

#include "aabb_base_solver.h"
#include "lmb/aabbox.h"

#include <array>

#define KDTREE_AXIS 3


namespace LMB
{

class KDCell
{

public:

    AABB3D m_bbox;
    std::array<size_t,2> m_branch;
    std::vector<size_t> m_triangle_indexes;
};


class KDTreeSolver : public AABBBaseSolver
{


    void Gen();

    void SplitCell(const size_t cell);

    void GenCellTriangles(const size_t parent,const size_t cell);

    const bool Intersect(const Ray &ray,SHitInfo &out_hit_info) const;

    const void IntersectCell(Ray &ray,const size_t cell,SHitInfo &hit_info,bool &hit) const;

protected:

    size_t m_axis;
    std::vector<KDCell> m_cells;

};

}