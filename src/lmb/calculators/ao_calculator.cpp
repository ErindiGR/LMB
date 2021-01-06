#include "lmb/calculators/ao_calculator.h"
#include "lmb/lmb.h"
#include "lmb/debug.h"
#include "lmb/ray_spread.h"

#include <algorithm>

namespace LMB
{

vec4 AOCalculator::CalcPixel(
    const vec3 &world_pos,
    const vec3 &world_norm)
{
    
    const std::vector<Ray> rays = std::move(GenRays(world_pos,world_norm));

    auto solver = GetLMB()->GetSolver();

    real_t ret_color = 1;

    const real_t inverse_dist = to_real(1.0) / m_config.ray_distance;
    const real_t ray_contribution = to_real(1.0) / (real_t)rays.size();
    
    for(size_t ray_i=0;ray_i<rays.size();ray_i++)
    {    
        auto &ray = rays[ray_i];
        Solver::SHitInfo hit;

        if(solver->Intersect(ray,hit))
        {
            ret_color -= CalcOcclusion(ray,hit) * inverse_dist * ray_contribution;
        }
    }

    return vec4(ret_color,ret_color,ret_color,1);

}

const std::vector<Ray> AOCalculator::GenRays(
    const vec3 &pos,
    const vec3 &norm) const
{
    return std::move(RaySpread::GenHemisphere(
        pos,
        norm,
        m_config.max_angle,
        m_config.ray_distance,
        m_config.bias,
        m_config.num_rays));
}


const real_t AOCalculator::CalcOcclusion(const Ray &ray,const Solver::SHitInfo &hit)
{
    auto &hit_triangle = GetLMB()->GetTriangles()[hit.triangle_index];

    auto &triangle_info = GetLMB()->GetTrianglesInfo()[hit_triangle.GetInfo()];

    if(!triangle_info.HasAlbedo())
        return 1 - hit.t;

    const vec2 hit_uv = hit_triangle.BarycentricToLightmapUV(hit.uvw);

    if(triangle_info.GetAlbedo().Get()->GetPixel(hit_uv.x,hit_uv.y).w >= to_real(0.999))
        return 1 - hit.t;

    auto solver = GetLMB()->GetSolver();

    const real_t hit_alpha = triangle_info.GetAlbedo().Get()->GetPixel(hit_uv.x,hit_uv.y).w;

    Ray transparency_ray(ray.GetStart()+ray.GetDir()*(hit.t+m_config.bias),ray.GetEnd());

    Solver::SHitInfo transparency_hit;

    if(solver->Intersect(transparency_ray,transparency_hit))
    {
        return glm::min(hit_alpha + CalcOcclusion(transparency_ray,transparency_hit),to_real(1));
    }
    else
    {
        return hit_alpha;
    }
}

}