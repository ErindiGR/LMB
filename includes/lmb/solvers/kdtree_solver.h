#pragma once

#include "aabb_base_solver.h"
#include "lmb/aabbox.h"

#include <array>
#include <chrono>

#define KDTREE_AXIS 3


namespace LMB
{

class KDCell
{

public:

    AABB3D m_bbox;
    std::array<size_t,2> m_branch;
    std::vector<size_t> m_triangle_indexes;
    uint16_t m_depth;
    uint16_t m_axis;
};


/**
 * @brief !!!WORK IN PROGRESS!!!
 * 
 */
class KDTreeSolver : public AABBBaseSolver
{

public:
    void Gen();

    /**
     * @brief Spilits a Cell
     * 
     * @param cell 
     * @return true if the cell was split
     * @return false if not
     */
    bool SplitCell(const size_t cell);

    const bool Intersect(const Ray &ray,SHitInfo &out_hit_info) const;

    const void IntersectCell(Ray &ray,const size_t cell,SHitInfo &hit_info,bool &hit) const;

protected:

    //size_t m_axis;
    std::vector<KDCell> m_cells;
};

struct SAABB3DAndDepth
{
    AABB3D bbox;
    size_t depth;
};

}