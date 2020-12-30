#include "lmb/solvers/aabb_base_solver.h"
#include "lmb/lmb.h"

namespace LMB
{

const AABB3D AABBBaseSolver::GenWorldAABB() const
{
    vec3 min(FLT_MAX);
    vec3 max(-FLT_MAX);

    auto &triangles = GetLMB()->GetTriangles();


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

const AABB3D AABBBaseSolver::GenWorldAABB(const std::vector<size_t> &triangles_indices) const
{
    AABB3D ret;
    ret.SetMax(vec3(0));
    ret.SetMin(vec3(0));

    if(triangles_indices.size() == 0)
        return ret;

    vec3 min(FLT_MAX);
    vec3 max(-FLT_MAX);

    auto &triangles = GetLMB()->GetTriangles();

    for(size_t i = 0;i < triangles_indices.size();i++)
    {
        const size_t ti = triangles_indices[i];
        for(size_t j = 0;j < 3;j++)
		{
			if(min[j]>triangles[ti].GetAABB().GetMin()[j])
				min[j] = triangles[ti].GetAABB().GetMin()[j];
			
			if(max[j]<triangles[ti].GetAABB().GetMax()[j])
				max[j] = triangles[ti].GetAABB().GetMax()[j];

		}
    }
    
    ret.SetMax(max);
    ret.SetMin(min);

    return ret;
}

const AABB3D AABBBaseSolver::GenWorldAABB(const std::vector<Triangle> &triangles) const
{
    vec3 min(FLT_MAX);
    vec3 max(-FLT_MAX);


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

    if(PointInsideBox(ray.GetStart(),bbox))
    {
        outt=t;
        return true;
    }

    if(PointInsideBox(ray.GetEnd(),bbox))
    {
        outt=ray.GetLength();
        return true;
    }

#if 0
    if(!Intersect2AABB3D(ray.GetAABB(),bbox))
    {
        return false;
    }
#endif

    t = RayBoxIntersectFast2(ray,bbox);
    if(t>0 && t<= ray.GetLength())
    {
        outt=t;
        return true;
    }

    return false;
}

std::vector<size_t> AABBBaseSolver::IntersectTriangleAABB(const AABB3D &bbox)const
{
    auto &triangles = GetLMB()->GetTriangles();

    std::vector<size_t> tri_indexes;

    size_t count=0;

    for(size_t i=0;i<triangles.size();i++)
    {
        if(Intersect2AABB3D(bbox,triangles[i].GetAABB()))
        {
            tri_indexes.push_back(i);
            ++count;
        }
        else
        {
            bool inside = false;

            for(int v=0;v<triangles[i].GetPos().size();v++)
            {
                if(PointInsideBox(triangles[i].GetPos()[v], bbox))
                    inside=true;
            }

            if(inside)
            {
                tri_indexes.push_back(i);
                ++count;
            }

        }
    }

    return std::move(tri_indexes);

}

std::vector<size_t> AABBBaseSolver::IntersectTriangleAABB(const AABB3D &bbox, const std::vector<size_t> &triangle_list)const
{
    auto &triangles = GetLMB()->GetTriangles();

    std::vector<size_t> tri_indexes;
    tri_indexes.reserve(2000);

    for(size_t i=0;i < triangle_list.size();i++)
    {
        const size_t ti = triangle_list[i];

        if(Intersect2AABB3D(bbox,triangles[ti].GetAABB()))
        {
            tri_indexes.push_back(ti);
        }
        else
        {
            bool inside = false;

            for(int v=0;v<triangles[i].GetPos().size();v++)
            {
                if(PointInsideBox(triangles[i].GetPos()[v], bbox))
                {
                    inside = true;
                    break;
                }
            }

            if(inside)
            {
                tri_indexes.push_back(ti);
            }

        }
    }

    return std::move(tri_indexes);

}


}