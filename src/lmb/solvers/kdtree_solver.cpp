#include "lmb/solvers/kdtree_solver.h"
#include "lmb/lmb.h"
#include "lmb/dumper.h"
#include "lmb/solvers/grid_solver.h"
#include "lmb/debug.h"

#include <chrono>

namespace LMB
{

real_t g_log_64[64];

void KDTreeSolver::Gen()
{
    m_cells.push_back(KDCell());
    
    m_cells[0].m_bbox = GenWorldAABB();
    m_cells[0].m_depth = 1;
    
    for(size_t i = 0;i < GetLMB()->GetTriangles().size();i++)
        m_cells[0].m_triangle_indexes.push_back(i);

    for(size_t i = 0;i < 64;i++)
        g_log_64[i] = glm::log(i);

    m_cells.reserve(100000);

    for(size_t i =0;i<24;i++)
    {
        size_t count_splits=0;
        const size_t cell_count = m_cells.size();
        for(size_t c = 0;c < cell_count;c++)
        {
            if(SplitCell(c))
            {
                ++count_splits;
            }
        }

        if(count_splits == 0)
            break;
    }

    Dump_Term(SAABB3DAndDepth);

    DEBUG_LOG("kdtree cell count %d \n",m_cells.size());
}

bool KDTreeSolver::SplitCell(const size_t cell)
{
    if(m_cells[cell].m_triangle_indexes.size() <= 1 || m_cells[cell].m_branch[0])
        return false;

    int depth = m_cells[cell].m_depth+1;

    int low_cost = 0;
    AABB3D a_box;
    AABB3D b_box;
    std::vector<size_t> a_tri;
    std::vector<size_t> b_tri;
    
    const int num_splits = 4;
    const real_t split_size = to_real(1.0) / num_splits;

    for(int i=0;i<3;i++)
    {
        for(int k=0;k<num_splits-1;k++)
        {
            real_t l =  split_size * (k+1);
            const real_t temp_mid = m_cells[cell].m_bbox.GetMin()[i]+
                (m_cells[cell].m_bbox.GetMax()[i]-m_cells[cell].m_bbox.GetMin()[i])*l;


            AABB3D a;
            AABB3D b;
            

            vec3 min = m_cells[cell].m_bbox.GetMin();
            vec3 max = m_cells[cell].m_bbox.GetMax();

            max[i] = temp_mid;

            a.SetMin(min);
            a.SetMax(max);

            min = m_cells[cell].m_bbox.GetMin();
            max = m_cells[cell].m_bbox.GetMax();

            min[i] = temp_mid;

            b.SetMin(min);
            b.SetMax(max);

            auto a_vec = std::move(IntersectTriangleAABB(a,m_cells[cell].m_triangle_indexes));
            auto b_vec = std::move(IntersectTriangleAABB(b,m_cells[cell].m_triangle_indexes));

            const int cost = (depth)*g_log_64[depth] + (a_vec.size()*l + b_vec.size()*(1.0-l));

            if(cost < low_cost || low_cost == 0)
            {
                low_cost = cost;
                a_box = a;
                b_box = b;
                a_tri = std::move(a_vec);
                b_tri = std::move(b_vec);
            }
        }
    }

    if(low_cost >= (depth-1)*g_log_64[glm::max(depth-1,0)] + m_cells[cell].m_triangle_indexes.size())
        return false;



    const size_t a_index = m_cells.size();
    m_cells.push_back(KDCell());
    const size_t b_index = m_cells.size();
    m_cells.push_back(KDCell());

    m_cells[a_index].m_bbox = a_box;
    m_cells[b_index].m_bbox = b_box;

    m_cells[a_index].m_depth = depth;
    m_cells[b_index].m_depth = depth;

    m_cells[cell].m_branch[0] = a_index;
    m_cells[cell].m_branch[1] = b_index;

    m_cells[a_index].m_triangle_indexes = std::move(a_tri);
    m_cells[b_index].m_triangle_indexes = std::move(b_tri);

    std::vector<size_t>().swap(m_cells[cell].m_triangle_indexes);

#define CREATE_SAABB3DAndDepth(x)  (SAABB3DAndDepth){m_cells[x].m_bbox,m_cells[x].m_depth}

    Dump_Push(SAABB3DAndDepth,CREATE_SAABB3DAndDepth(a_index));
    Dump_Push(SAABB3DAndDepth,CREATE_SAABB3DAndDepth(b_index));

    return true;
}

const bool KDTreeSolver::Intersect(const Ray &ray,SHitInfo &out_hit_info) const
{   
    Ray r = ray;

    SHitInfo hit_info;
    bool hit = false;

    IntersectCell(r,0,hit_info,hit);

    if(hit)
    {
        out_hit_info.t = hit_info.t;
        out_hit_info.uvw = hit_info.uvw;
        out_hit_info.triangle_index = hit_info.triangle_index;
    }


    return hit;
}


const void KDTreeSolver::IntersectCell(Ray &ray,const size_t cell,SHitInfo &hit_info,bool &hit) const
{


    auto &cell_ref = m_cells[cell];

    if(cell_ref.m_triangle_indexes.size() <= 0 && !cell_ref.m_branch[0])
        return;

    auto &triangles = GetLMB()->GetTriangles();

    if(cell_ref.m_triangle_indexes.size() == 1)
    {
        if(IntersectTriangle(ray,triangles[cell_ref.m_triangle_indexes[0]],hit_info.uvw,hit_info.t))
        {
            ray.SetEnd(ray.GetStart() + ray.GetDir() * hit_info.t);
            hit_info.triangle_index = cell_ref.m_triangle_indexes[0];
            hit = true;
        }
    }

    

    real_t t = 0;

    if(!IntersectAABB3D(ray,cell_ref.m_bbox,t))
        return;



    const bool it_branches = cell_ref.m_branch[0];
    if(it_branches)
    {
        for(size_t i =0;i < cell_ref.m_branch.size();i++)
        {
            IntersectCell(ray,cell_ref.m_branch[i],hit_info,hit);
        }
    }
    else
    {
        auto &triangle_indexes = cell_ref.m_triangle_indexes;
        for(size_t i = 0;i < triangle_indexes.size();i++)
        {
            const size_t ti = triangle_indexes[i];

            if(IntersectTriangle(ray,triangles[ti],hit_info.uvw,hit_info.t))
            {
                ray.SetEnd(ray.GetStart() + ray.GetDir() * hit_info.t);
                hit_info.triangle_index = ti;
                hit = true;
            }
        }

    }

}



}