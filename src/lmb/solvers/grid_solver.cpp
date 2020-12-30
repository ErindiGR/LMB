#include "lmb/solvers/grid_solver.h"
#include "lmb/lmb.h"
#include "lmb/dumper.h"
#include "lmb/debug.h"
#include "lmb/geometry.h"



namespace LMB
{

const bool GridSolver::Intersect(const Ray &ray,SHitInfo &out_hit_info) const
{
    Ray r = ray;
    
    real_t  t = 0;
    size_t  index = 0;
    vec3    uvw(0);
    
    bool hit = false;

    auto &triangles = GetLMB()->GetTriangles();

    for(size_t i=0;i<m_grid.size();i++)
    {
        auto &cell_triangles = m_grid[i].m_triangle_indexes;


        real_t bboxt = 0;
        if(IntersectAABB3D(r,m_grid[i].m_bbox,bboxt))
        {
            for(size_t j=0;j<cell_triangles.size();j++)
            {
                if(IntersectTriangle(r,triangles[cell_triangles[j]],uvw,t))
                {
                    r.SetEnd(r.GetStart()+r.GetDir()* t);
                    index = cell_triangles[j];
                    hit = true;
                }
            }
        }
    }


    if(hit)
    {
        out_hit_info.t = t;
        out_hit_info.uvw = uvw;
        out_hit_info.triangle_index = index;
    }

    return hit;
}

#define INCREMENT3D(x,y,z,count)\
{\
    ++x;\
    if(x>=count)\
    {\
        x=0;\
        ++y;\
    }\
    if(y>=count)\
    {\
        y=0;\
        ++z;\
    }\
}

void GridSolver::Gen()
{
    DEBUG_LOG("Generating solvers grid.\n");

    const AABB3D world_aabb = GenWorldAABB();

    const vec3 min = world_aabb.GetMin();
    const vec3 max = world_aabb.GetMax();

    const vec3 cell_size = (max-min) / (real_t)m_num_cells;

    const size_t num_cells3 = m_num_cells * m_num_cells * m_num_cells;

    m_grid.reserve(num_cells3);

    
    size_t x=0;
    size_t y=0;
    size_t z=0;
    for(size_t j=0;j<num_cells3;j++)
    {
        AABB3D bbox;

        const vec3 pos((real_t)x,(real_t)y,(real_t)z);

        vec3 b_min = min;
        b_min += cell_size * pos;

        vec3 b_max = min;
        b_max += cell_size * (pos + vec3(to_real(1.0)));

        bbox.SetMax(b_max);
        bbox.SetMin(b_min);


        auto tri = std::move(IntersectTriangleAABB(bbox));

        if(tri.size() > 0)
        {
            Dump_Push(AABB3D,bbox);
            m_grid.push_back(GridCell());
            m_grid[m_grid.size()-1].m_bbox = bbox;
            m_grid[m_grid.size()-1].m_triangle_indexes = std::move(tri);
        }

        INCREMENT3D(x,y,z,m_num_cells)
    }

    Dump_Term(AABB3D);


    for(size_t c = 0;c < m_grid.size();c++)
    {
        printf("grid cell%d = %d \n",c,m_grid[c].m_triangle_indexes.size());
    }

}

}