#pragma once

#include "aabb_base_solver.h"
#include "lmb/base_type.h"

#include <vector>



namespace LMB
{



class GridCell
{

public:

    AABB3D m_bbox;
    std::vector<size_t> m_triangle_indexes;
};


/**
* @brief This solver uses a 3 dimensional grid of axis aligned
* bounding boxes in a grid to speed up the ray-world intersection
*/
class GridSolver : public AABBBaseSolver
{

public:
    
    /**
     * @brief Construct a new Grid Solver object
     * 
     * @param num_grid_cells the number of grid cells the solver is goind to divide
     * the scene on each dimension
     */
    explicit GridSolver(const uint16_t num_grid_cells)
    : m_num_cells(num_grid_cells)
    {
    }

    //Solver
    /**
    * @brief creates the grid and assigns the triangles to each grid cell
    */
    void Gen() override;
    //!Solver

    const bool Intersect(const Ray &ray,SHitInfo &out_hit_info) const override;

protected:

    std::vector<GridCell> m_grid;
    size_t m_num_cells;
};


}