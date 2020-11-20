#include "lmb/solvers/aabb_base_solver.h"
#include "lmb/lmb.h"

namespace LMB
{

const AABB3D AABBBaseSolver::GenWorldAABB() const
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

const bool AABBBaseSolver::IntersectAABB3D(const Ray &ray,const AABB3D &bbox,real_t &outt)const
{
    real_t t=0;
    real_t far_t=0;

    if(PointInsideBox(ray.GetStart(),bbox.GetMin(),bbox.GetMax()))
    {
        outt=t;
        return true;
    }

    t = RayBoxIntersectFast(ray,bbox);
    if(t>0)
    {
        outt=t;
        return true;
    }

    return false;
}

}