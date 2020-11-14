#include "lmb/solvers/grid_solver.h"
#include "lmb/lmb.h"
#include "lmb/dumper.h"
#include "lmb/debug.h"
#include "lmb/geometry.h"



namespace LMB
{




const bool GridSolver::Intersect(const Ray &ray,vec3 &out_uvw,real_t &outt) const
{
    Ray r = ray;
    real_t t = 0;
    vec3 uvw(0);
    bool hit = false;

    auto &triangles = m_lmb->GetTriangles();

    for(size_t i=0;i<m_grid.size();i++)
    {
        auto &cell_triangles = m_grid[i].GetTriangles();


        real_t bboxt = 0;
        if(IntersectAABB3D(r,m_grid[i].GetAABB(),bboxt))
        {
            for(size_t j=0;j<cell_triangles.size();j++)
            {
                if(IntersectTriangle(r,triangles[cell_triangles[j]],uvw,t))
                {
                    r.SetEnd(r.GetStart()+r.GetDir()* t);
                    hit = true;
                }
            }
        }
    }


    if(hit)
    {
        outt = t;
        out_uvw = uvw;
    }

#if 1
        Dumper::Push(r);
#endif

    return hit;
}






const bool GridSolver::IntersectAABB3D(const Ray &ray,const AABB3D &bbox,real_t &outt)const
{
    real_t t=0;

    if(PointInsideBox(ray.GetStart(),bbox.GetMin(),bbox.GetMax()))
    {
        outt=t;
        return true;
    }

    t = RayBoxIntersect(ray.GetStart(),ray.GetDir(),bbox.GetMin(),bbox.GetMax());

    if(t>0)
    {
        outt=t;
        return true;
    }

    return false;
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

        m_grid.push_back(bbox);
        GenCellTriangles(m_grid.size()-1);

        if(m_grid[m_grid.size()-1].GetTriangles().size() <= 0)
            m_grid.pop_back();

#if 1
        Dumper::Push(m_grid[m_grid.size()-1]);
#endif

        INCREMENT3D(x,y,z,m_num_cells)
    }

}





void GridSolver::GenCellTriangles(const size_t index)
{
    auto &triangles = m_lmb->GetTriangles();

    size_t count=0;

    for(size_t i=0;i<triangles.size();i++)
    {
        if(Intersect2AABB3D(m_grid[index].GetAABB(),triangles[i].GetAABB()))
        {
            m_grid[index].AddTriangle(i);
            ++count;
        }
        else
        {
            bool inside = false;

            for(int v=0;v<triangles[i].GetPos().size();v++)
            {
                if(PointInsideBox(
                    triangles[i].GetPos()[v],
                    m_grid[index].GetAABB().GetMin(),
                    m_grid[index].GetAABB().GetMax()
                )) inside=true;
            }

            if(inside)
            {
                m_grid[index].AddTriangle(i);
                ++count;
            }

        }
    }

    //printf("count %d.\n",count);

}





const AABB3D GridSolver::GenWorldAABB() const
{
    vec3 min(FLT_MAX);
    vec3 max(-FLT_MAX);

    auto &triangles = m_lmb->GetTriangles();


    for(size_t i = 0;i < triangles.size();i++)
    {
        for(size_t j = 0;j < 3;j++)
		{
			if(min[j]>triangles[i].GetAABB().GetMin()[j])
				min[j] = triangles[i].GetAABB().GetMin()[j];
			
			if(max[j]<triangles[i].GetAABB().GetMax()[j])
				max[j] = triangles[i].GetAABB().GetMax()[j];

		}
    }

    AABB3D ret;
    
    ret.SetMax(max);
    ret.SetMin(min);

    return ret;
}



}