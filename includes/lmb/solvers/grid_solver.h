#pragma once

#include "lmb/solvers/default_solver.h"
#include "lmb/base_type.h"

#include <vector>



namespace LMB
{



class GridCell
{

public:

    GridCell(const AABB3D &bbox)
    : m_bbox(bbox)
    , m_triangle_indexes()
    {
    }

    GridCell()
    : m_bbox()
    , m_triangle_indexes()
    {
    }

    inline void AddTriangle(const size_t index)
    {
        m_triangle_indexes.push_back(index);
    }

    inline const AABB3D& GetAABB() const
    {
        return m_bbox;
    }

    inline const std::vector<size_t> &GetTriangles() const
    {
        return m_triangle_indexes;
    }


protected:

    AABB3D m_bbox;
    std::vector<size_t> m_triangle_indexes;
};


/**
* @brief This solver uses a 3 dimensional grid of axis aligned
* bounding boxes in a grid to speed up the ray-world intersection
*/
class GridSolver : public DefaultSolver
{

public:
    
    /**
     * @brief Construct a new Grid Solver object
     * 
     * @param num_grid_cells the number of grid cells the solver is goind to divide
     * the scene on each dimension
     */
    GridSolver(const uint16_t num_grid_cells)
    : m_num_cells(num_grid_cells)
    {
    }

    /**
    * @brief creates the grid and assigns the triangles to each grid cell
    */
    void Gen();

    /**
    * @brief checks which triangles are inside the grid cell and adds them
    * to it.
    */
    void GenCellTriangles(const size_t index);

    /**
    * @brief find the bounds of the world
    */
    const AABB3D GenWorldAABB() const;

    const bool Intersect(const Ray &ray,vec3 &out_uvw,real_t &outt)const;

    const bool IntersectAABB3D(const Ray &ray,const AABB3D &bbox,real_t &outt)const;


protected:

    std::vector<GridCell> m_grid;
    size_t m_num_cells;
};


}