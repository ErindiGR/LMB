#include "lmb/solvers/default_solver.h"
#include "lmb/lmb.h"
#include "lmb/geometry.h"



namespace LMB
{



const bool DefaultSolver::Intersect(const Ray &ray,SHitInfo &out_hit_info) const
{
    Ray r = ray;
    
    real_t  t = 0;
    size_t  index = 0;
    vec3    uvw(0);
    
    bool hit = false;

    auto &triangles = m_lmb->GetTriangles();

    for(size_t i=0;i<triangles.size();i++)
    {
        if(IntersectTriangle(r,triangles[i],uvw,t))
        {
            r.SetEnd(r.GetStart()+r.GetDir()*t);
            index = i;
            hit = true;
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





const bool DefaultSolver::IntersectTriangle(const Ray &ray,const Triangle &tri,vec3 &out_uvw,real_t &outt) const
{

    
    real_t t = 0;
    vec3 uvw(0);
    auto &v = tri.GetPos();
    bool res = rayTriangleIntersect(ray.GetStart(),
                                    ray.GetDir(),
                                    v[0],v[1],v[2],
                                    t,uvw.x,uvw.y
                                    );


    if(res && t>to_real(0.0) && t<ray.GetLength())
    {
        outt = t;
        uvw.z = 1-uvw.x-uvw.y;
        out_uvw = uvw;
        return true;
    }

    return false;
}

void DefaultSolver::Gen()
{
    
}

}