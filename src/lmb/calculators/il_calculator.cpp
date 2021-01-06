#include "lmb/calculators/il_calculator.h"
#include "lmb/lmb.h"
#include "lmb/debug.h"

#include <algorithm>

namespace LMB
{



ILJob::ILJob(
    const size_t x_start,
    const size_t y_start,
    const size_t x_end,
    const size_t y_end,
    const size_t lightmap,
    IndirectLightCalculator* calc,
    const uint16_t bounce)
    : LightmapChunkJob(x_start,y_start,x_end,y_end,lightmap,calc)
    , m_bounce(bounce)
{
}

void ILJob::CalculatePixel(const bitmap_size_t x,const bitmap_size_t y)
{
    CalcLightmapHelper calc_helper(m_lightmap,m_calc);

    if(calc_helper.GetRealFlags(x,y) == Lightmap::EFlags::UnUsed)
        return;

    const vec2 pixel_pos = GetPixelCenter(x,y);

    const vec4 gi = ((IndirectLightCalculator*) m_calc)->CalcPixel(
        calc_helper.GetRealPos(pixel_pos.x,pixel_pos.y),
        calc_helper.GetRealNorm(pixel_pos.x,pixel_pos.y),
        m_bounce);

    calc_helper.SetColor(x,y,gi);
}

void IndirectLightCalculator::StartCalc(const size_t lightmap)
{
    Calculator::StartCalc(lightmap);

    DEBUG_LOG("Started Calculating AO.\n");

    CalcLightmapHelper calc_helper(lightmap,this);

    size_t chunk_size = 32;

    for(int i=0;i<m_config.num_bounces;i++)
    {
        for(size_t x=0;x<calc_helper.GetLightmapWidth()/chunk_size;x++)
        {
            for(size_t y=0;y<calc_helper.GetLightmapHeight()/chunk_size;y++)
            {
                std::shared_ptr<Job> job = std::make_shared<ILJob>(
                    x*chunk_size,
                    y*chunk_size,
                    (x+1)*chunk_size,
                    (y+1)*chunk_size,
                    lightmap,
                    this,
                    i);

                m_jobs.push_back(job);
                JobManager::Push(std::static_pointer_cast<Job>(job));
            }
        }
    }

}





vec4 IndirectLightCalculator::CalcPixel(
    const vec3 &world_pos,
    const vec3 &world_norm,
    const uint16_t bounce)
{

    const std::vector<Ray> rays = std::move(GenRays(world_pos,world_norm,bounce));

    auto solver = GetLMB()->GetSolver();

    vec3 ret_color(0);

    const real_t ray_contribution = to_real(1.0) / to_real(rays.size());
    

    for(size_t ray_i=0;ray_i<rays.size();ray_i++)
    {    
        auto &ray = rays[ray_i];
        Solver::SHitInfo hit;

        if(solver->Intersect(ray,hit))
        {
            ret_color += CalcIndirectLighting(world_norm,ray,hit);
        }
        else
        {
            const real_t n_dot_l = glm::clamp(glm::dot(world_norm,ray.GetDir()),to_real(0.0),to_real(1.0));
            const vec3 received_light = m_config.ambient_color * n_dot_l;
            ret_color += received_light;
        }
        
    }

    ret_color *= ray_contribution;

    return vec4(ret_color,1);

}





const std::vector<Ray> IndirectLightCalculator::GenRays(
    const vec3 &pos,
    const vec3 &norm,
    const uint16_t bounce) const
{

    size_t num_rays = m_config.num_rays;

    if(bounce == (m_config.num_bounces - 1))
        num_rays = m_config.num_final_rays;


    std::vector<Ray> rays(num_rays);
    
    for(size_t i=0;i<rays.size();i++)
    {
        rays[i].SetStart(pos + norm * m_config.bias);
        rays[i].SetEnd(pos + norm * m_config.ray_distance);
    }



    for(size_t i=0;i<rays.size();i++)
    {
        Ray &ray = rays[i];


        vec3 dir = ray.GetDir();

        const real_t randx = ((rand() % 1024) - to_real(512.5)) / 512.5;
        const real_t randy = ((rand() % 1024) - to_real(512.5)) / 512.5;
        const real_t randz = ((rand() % 1024) - to_real(512.5)) / 512.5;

        const real_t anglex = glm::radians(m_config.max_angle * randx );
        const real_t angley = glm::radians(m_config.max_angle * randy );
        const real_t anglez = glm::radians(m_config.max_angle * randz );

        dir = glm::rotate(dir,anglex,vec3(1,0,0));
        dir = glm::rotate(dir,angley,vec3(0,1,0));
        dir = glm::rotate(dir,anglez,vec3(0,0,1));

        dir = glm::normalize(dir);

        ray.SetEnd(ray.GetStart() + dir * m_config.ray_distance);
    }

    return std::move(rays);
}


const vec3 IndirectLightCalculator::CalcIndirectLighting(const vec3 norm,const Ray &ray,const Solver::SHitInfo &hit)
{
    real_t alpha(1);

    auto &hit_triangle = GetLMB()->GetTriangles()[hit.triangle_index];

    auto &triangle_info = GetLMB()->GetTrianglesInfo()[hit_triangle.GetInfo()];

    const vec2 hit_uv = hit_triangle.BarycentricToLightmapUV(hit.uvw);

    if(triangle_info.HasAlbedo())
    {
        alpha = triangle_info.GetAlbedo().Get()->GetPixel(hit_uv.x,hit_uv.y).w;
    }

    if(alpha > to_real(0.999))
    {
        return CalcReceivedLight(norm,ray,hit);
    }
    else
    {
        auto solver = GetLMB()->GetSolver();

        Ray transparency_ray(ray.GetStart()+ray.GetDir()*(hit.t+m_config.bias),ray.GetEnd());
        Solver::SHitInfo transparency_hit;

        if(solver->Intersect(transparency_ray,transparency_hit))
        {
            return glm::lerp(
                CalcIndirectLighting(norm,transparency_ray,transparency_hit),
                CalcReceivedLight(norm,transparency_ray,transparency_hit),
                alpha);
        }
        else
        {   
            const real_t n_dot_l = glm::clamp(glm::dot(norm,ray.GetDir()),to_real(0.0),to_real(1.0));
            const vec3 received_ambientlight = m_config.ambient_color * n_dot_l;

            return  glm::lerp(received_ambientlight,CalcReceivedLight(norm,ray,hit),alpha);
        }
    }
}

const vec3 IndirectLightCalculator::CalcReceivedLight(const vec3 norm,const Ray &ray,const Solver::SHitInfo &hit)
{
    auto &hit_triangle = GetLMB()->GetTriangles()[hit.triangle_index];

    auto &triangle_info = GetLMB()->GetTrianglesInfo()[hit_triangle.GetInfo()];

    const vec2 hit_uv = hit_triangle.BarycentricToLightmapUV(hit.uvw);

    vec3 hit_light(0);
    vec3 albedo(1);

    if(triangle_info.HasLightmap())
    {
        auto hit_lightmap = triangle_info.GetLightmap().Get();

        auto &hit_temp_lightmap = GetTempLightmapColor(triangle_info.GetLightmap().GetIndex());
        
        hit_light = hit_lightmap->GetColor().GetPixel(hit_uv.x,hit_uv.y) +
            hit_temp_lightmap->GetPixel(hit_uv.x,hit_uv.y);
    }

    if(triangle_info.HasAlbedo())
    {
        albedo = triangle_info.GetAlbedo().Get()->GetPixel(hit_uv.x,hit_uv.y);
    }

    const real_t n_dot_l = glm::clamp(glm::dot(norm,ray.GetDir()),to_real(0.0),to_real(1.0));
    const vec3 received_light = hit_light * albedo * n_dot_l;

    return received_light;
}


}