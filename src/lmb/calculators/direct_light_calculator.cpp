#include "lmb/calculators/direct_light_calculator.h"
#include "lmb/lmb.h"
#include "lmb/base_type.h"
#include "lmb/ray_spread.h"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>


namespace LMB
{



DLJob::DLJob( 
    const bitmap_size_t x_start,
    const bitmap_size_t y_start,
    const bitmap_size_t x_end,
    const bitmap_size_t y_end,
    const size_t lightmap,
    DirectLightCalculator* calc)
    : LightmapChunkJob(x_start,y_start,x_end,y_end,lightmap,calc)
{
}

void DLJob::CalculatePixel(const bitmap_size_t x,const bitmap_size_t y)
{
    auto &lightmap = m_calc->GetRealLightmap(m_lightmap);
    auto &temp_lightmap = m_calc->GetTempLightmapColor(m_lightmap);

    if(lightmap->GetFlags().GetPixel(x,y) == Lightmap::EFlags::UnUsed)
        return;

    const vec2 pixel_pos = GetPixelCenter(x,y);

    const vec4 direct = ((DirectLightCalculator*) m_calc)->CalcPixel(
        lightmap->GetPos().GetPixel(pixel_pos.x,pixel_pos.y),
        lightmap->GetNorm().GetPixel(pixel_pos.x,pixel_pos.y));
    
    temp_lightmap->SetPixel(x,y,direct);
}

void DirectLightCalculator::StartCalc(const size_t lightmap)
{
    Calculator::StartCalc(lightmap);

    bitmap_size_t chunk_size = std::max(
        (int)(GetLightmapWidth()/(JobManager::GetNumThreads()*2)),1);

    for(bitmap_size_t x=0;x<GetLightmapWidth()/chunk_size;x++)
    {
        for(bitmap_size_t y=0;y<GetLightmapHeight()/chunk_size;y++)
        {
            std::shared_ptr<DLJob> job = std::make_shared<DLJob>(
                x*chunk_size,
                y*chunk_size,
                (x+1)*chunk_size,
                (y+1)*chunk_size,
                m_current_lightmap_handle,
                this);

            m_jobs.push_back(job);
            JobManager::Push(std::static_pointer_cast<Job>(job));
        }
    }
}

vec4 DirectLightCalculator::CalcPixel(
    const vec3 &world_pos,
    const vec3 &world_norm)
{
    
    vec3 ret_color(m_config.ambient_color);

    for(size_t i=0;i<m_lights.size();i++)
    {
        if(m_lights[i].GetType() == Light::EType::Directional)
        {
            ret_color += CalcDirectionalLight(world_pos,world_norm,i);
        }
        else if(m_lights[i].GetType() == Light::EType::Point)
        {
            ret_color += CalcPointLight(world_pos,world_norm,i);
        }

    }

    if(m_config.use_emissive)
        ret_color += CalcEmission(world_pos,world_norm);

    return vec4(ret_color,1);
}

std::vector<Ray> DirectLightCalculator::GenRays(
    const real_t softness,
    const real_t length,
    const vec3 &pos,
    const vec3 &norm,
    const Light::EType light_type)
{
    real_t angle = 0;

    if(light_type == Light::EType::Directional)
    {
        angle = glm::lerp(to_real(0),to_real(45),softness);
    }
    else
    {
        angle = glm::lerp(to_real(0),to_real(89),softness);
    }

    return std::move(RaySpread::GenHemisphere(
        pos,
        norm,
        angle,
        length,
        m_config.bias,
        m_config.num_rays));

}

const std::vector<Ray> DirectLightCalculator::GenRays(
    const vec3 &pos,
    const vec3 &norm) const
{
    return std::move(RaySpread::GenHemisphere(
        pos,
        norm,
        to_real(89.9),
        m_config.max_ray_distance,
        m_config.bias,
        m_config.mun_emissive_rays));
}

const vec3 DirectLightCalculator::CalcDirectionalLight(
    const vec3 &world_pos,
    const vec3 &world_norm,
    const size_t light_index)
{
    const vec3 &light_dir = m_lights[light_index].GetDir();
    const vec3 &light_color = m_lights[light_index].GetColor();
    const real_t light_softness = m_lights[light_index].GetSoftness();

    const std::vector<Ray> rays = std::move(
        GenRays(light_softness,m_config.max_ray_distance,world_pos,light_dir, Light::EType::Directional));

    const real_t ray_contribute = to_real(1.0)/ to_real(rays.size());

    auto solver = GetLMB()->GetSolver();

    vec3 ret(0);

    for(size_t ray_i=0;ray_i<rays.size();ray_i++)
    {
        auto &ray = rays[ray_i];
        Solver::SHitInfo hit;

        if(!solver->Intersect(ray,hit))
        {
            const real_t n_dot_dir = glm::max(glm::dot(world_norm,light_dir),to_real(0.0));
            
            ret += light_color * n_dot_dir;
        }
        else
        {
            ret += CalcTransparentLight(ray,hit,light_color);
        }
    }

    ret *= ray_contribute;

    return vec3(ret);
}

const vec3 DirectLightCalculator::CalcPointLight(
    const vec3 &world_pos,
    const vec3 &world_norm,
    const size_t light_index)
{
    const vec3 &light_pos = m_lights[light_index].GetPos();
    const vec3 &light_color = m_lights[light_index].GetColor();
    const real_t light_softness = m_lights[light_index].GetSoftness();
    const vec3 light_dir = light_pos - world_pos;

    const std::vector<Ray> rays = std::move(
        GenRays(light_softness,glm::length(light_dir),world_pos,glm::normalize(light_dir),Light::EType::Point));

    const real_t ray_contribute = to_real(1.0) / (real_t)rays.size();

    const real_t attenuation = glm::min(to_real(1.0) / (m_config.max_ray_distance * m_config.max_ray_distance),to_real(1.0));

    auto solver = GetLMB()->GetSolver();

    vec3 ret(0);

    for(size_t ray_i=0;ray_i<rays.size();ray_i++)
    {
        auto &ray = rays[ray_i];
        Solver::SHitInfo hit;

        if(!solver->Intersect(ray,hit))
        {
            const vec3 line = light_pos-world_pos;
            const vec3 dir = glm::normalize(line);
            const real_t dist = glm::length(line);
            
            const real_t n_dot_dir = glm::max(glm::dot(world_norm,dir),to_real(0.0));
            
            const real_t color_multiplier = n_dot_dir * ray_contribute;
            
            ret += light_color * color_multiplier;
        }
        else
        {
            ret += CalcTransparentLight(ray,hit,light_color);
        }
    }

    ret *= attenuation;

    return ret;
}

const vec3 DirectLightCalculator::CalcEmission(
    const vec3 &world_pos,
    const vec3 &world_norm)
{
    const std::vector<Ray> rays = std::move(GenRays(world_pos,world_norm));

    const real_t ray_contribute = to_real(1.0) / (real_t)rays.size();

    auto solver = GetLMB()->GetSolver();

    vec3 ret(0);

    for(size_t ray_i=0;ray_i<rays.size();ray_i++)
    {
        auto &ray = rays[ray_i];

        Solver::SHitInfo hit;

        if(solver->Intersect(ray,hit))
        {
            auto &hit_triangle = GetLMB()->GetTriangles()[hit.triangle_index];

            auto &triangle_info = GetLMB()->GetTrianglesInfo()[hit_triangle.GetInfo()];

            const vec2 hit_uv = hit_triangle.BarycentricToLightmapUV(hit.uvw);

            vec3 hit_light(0);

            if(triangle_info.HasEmissive())
            {
                hit_light += vec3(triangle_info.GetEmissive().Get()->GetPixel(hit_uv.x,hit_uv.y));
            }

            hit_light += CalcTransparentEmission(ray,hit);

            const real_t n_dot_l = glm::min(glm::max(glm::dot(world_norm,ray.GetDir()),to_real(0.0)),to_real(1.0));
            
            const vec3 received_light = hit_light * n_dot_l;

            ret += received_light;
        }
    }

    ret *= ray_contribute;

    return ret;
}

const vec3 DirectLightCalculator::CalcTransparentEmission(const Ray &ray,const Solver::SHitInfo &hit)
{
    auto &hit_triangle = GetLMB()->GetTriangles()[hit.triangle_index];

    auto &triangle_info = GetLMB()->GetTrianglesInfo()[hit_triangle.GetInfo()];

    if(!triangle_info.HasAlbedo())
        return vec3(0);

    const vec2 hit_uv = hit_triangle.BarycentricToLightmapUV(hit.uvw);

    if(triangle_info.GetAlbedo().Get()->GetPixel(hit_uv.x,hit_uv.y).w >= to_real(0.999))
        return vec3(0);

    auto solver = GetLMB()->GetSolver();

    vec4 hit_color = triangle_info.GetAlbedo().Get()->GetPixel(hit_uv.x,hit_uv.y);

    Ray transparency_ray(ray.GetStart()+ray.GetDir()*(hit.t+m_config.bias),ray.GetEnd());

    Solver::SHitInfo transparency_hit;

    if(solver->Intersect(transparency_ray,transparency_hit))
    {
        auto &transparency_hit_triangle = GetLMB()->GetTriangles()[transparency_hit.triangle_index];
        auto &transparency_hit_triangle_info = GetLMB()->GetTrianglesInfo()[transparency_hit_triangle.GetInfo()];
        
        if(transparency_hit_triangle_info.HasEmissive())
        {
            const vec2 transparency_hit_uv = transparency_hit_triangle.BarycentricToLightmapUV(transparency_hit.uvw);

            vec3 transparency_hit_color(transparency_hit_triangle_info.GetEmissive().Get()->GetPixel(transparency_hit_uv.x,transparency_hit_uv.y));

            return GetTransparentContribution(hit_color,transparency_hit_color);
        }

        return GetTransparentContribution(hit_color,CalcTransparentEmission(transparency_ray,transparency_hit));
    }
    else
    {
        return GetTransparentContribution(hit_color,m_config.ambient_color);
    }
}


const vec3 DirectLightCalculator::CalcTransparentLight(const Ray &ray,const Solver::SHitInfo &hit,const vec3 &light_color)
{
    auto &hit_triangle = GetLMB()->GetTriangles()[hit.triangle_index];

    auto &triangle_info = GetLMB()->GetTrianglesInfo()[hit_triangle.GetInfo()];

    if(!triangle_info.HasAlbedo())
        return vec3(0);

    const vec2 hit_uv = hit_triangle.BarycentricToLightmapUV(hit.uvw);

    if(triangle_info.GetAlbedo().Get()->GetPixel(hit_uv.x,hit_uv.y).w >= to_real(0.999))
        return vec3(0);

    auto solver = GetLMB()->GetSolver();

    const vec4 hit_color = triangle_info.GetAlbedo().Get()->GetPixel(hit_uv.x,hit_uv.y);

    const Ray transparency_ray(ray.GetStart()+ray.GetDir()*(hit.t+m_config.bias),ray.GetEnd());

    Solver::SHitInfo transparency_hit;

    if(!solver->Intersect(transparency_ray,transparency_hit))
    {   
        return GetTransparentContribution(hit_color,light_color);
    }
    else
    {
        return GetTransparentContribution(hit_color,CalcTransparentLight(transparency_ray,transparency_hit,light_color));
    }
}

const vec3 DirectLightCalculator::GetTransparentContribution(const vec4 color,const vec3 light_color)
{
    return glm::lerp(light_color,vec3(color) * light_color,vec3(color.w)) * (1-color.w);
}




}