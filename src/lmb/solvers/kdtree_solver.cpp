#include "lmb/solvers/kdtree_solver.h"
#include "lmb/lmb.h"
#include "lmb/dumper.h"
#include "lmb/solvers/grid_solver.h"
#include "lmb/debug.h"


namespace LMB
{

void KDTreeSolver::Gen()
{
    m_cells.push_back(KDCell());
    
    m_cells[0].m_bbox = GenWorldAABB();
    
    for(size_t i = 0;i < m_lmb->GetTriangles().size();i++)
        m_cells[0].m_triangle_indexes.push_back(i);

    size_t depth_count = 0;
    while(1)
    {
        ++depth_count;
        
        size_t count = 0;
        const size_t cell_count = m_cells.size();
        for(size_t c = 0;c < cell_count;c++)
        {
            if(!m_cells[c].m_branch[0] && m_cells[c].m_triangle_indexes.size() > 2)
            {
                SplitCell(c);
                ++count;
            }
        }

        if(!count || depth_count > 8)
            break;        
    }

    DEBUG_LOG("kdtree cell count %d \n",m_cells.size());
}

void KDTreeSolver::SplitCell(const size_t cell)
{
    ++m_axis;
    m_axis %= KDTREE_AXIS;

    real_t mid = 0;
    for(size_t i = 0;i < m_cells[cell].m_triangle_indexes.size();i++)
    {
        const size_t ti = m_cells[cell].m_triangle_indexes[i];

        mid += m_lmb->GetTriangles()[ti].GetAABB().GetMax()[m_axis];
    }

    mid /= m_cells[cell].m_triangle_indexes.size();

    AABB3D a;
    AABB3D b;

    vec3 min = m_cells[cell].m_bbox.GetMin();
    vec3 max = m_cells[cell].m_bbox.GetMax();

    max[m_axis] = mid;

    a.SetMin(min);
    a.SetMax(max);

    min = m_cells[cell].m_bbox.GetMin();
    max = m_cells[cell].m_bbox.GetMax();

    min[m_axis] = mid;

    b.SetMin(min);
    b.SetMax(max);

    size_t a_index = 0;
    size_t b_index = 0;

    a_index = m_cells.size();
    m_cells.push_back(KDCell());
    m_cells[a_index].m_bbox = a;

    b_index = m_cells.size();
    m_cells.push_back(KDCell());
    m_cells[b_index].m_bbox = b;

    m_cells[cell].m_branch[0] = a_index;
    m_cells[cell].m_branch[1] = b_index;

    GenCellTriangles(cell,a_index);
    GenCellTriangles(cell,b_index);

    //clear the s_data vector
    std::vector<size_t>().swap(m_cells[cell].m_triangle_indexes);
    
}

void KDTreeSolver::GenCellTriangles(const size_t parent,const size_t cell)
{
    auto &triangles = m_lmb->GetTriangles();

    size_t count=0;

    for(size_t i=0;i<m_cells[parent].m_triangle_indexes.size();i++)
    {
        const size_t ti = m_cells[parent].m_triangle_indexes[i];

        if(Intersect2AABB3D(m_cells[cell].m_bbox,triangles[ti].GetAABB()))
        {
            m_cells[cell].m_triangle_indexes.push_back(ti);
            ++count;
        }
        else
        {
            bool inside = false;

            for(int v=0;v<triangles[i].GetPos().size();v++)
            {
                if(PointInsideBox(
                    triangles[i].GetPos()[v],
                    m_cells[cell].m_bbox.GetMin(),
                    m_cells[cell].m_bbox.GetMax()))
                    inside=true;
            }

            if(inside)
            {
                m_cells[cell].m_triangle_indexes.push_back(ti);
                ++count;
            }

        }
    }

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
    auto &triangles = m_lmb->GetTriangles();

    real_t t = 0;

    auto &cell_ref = m_cells[cell];

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