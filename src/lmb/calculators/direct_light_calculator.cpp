#include "lmb/calculators/direct_light_calculator.h"
#include "lmb/lmb.h"
#include "lmb/base_type.h"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>


namespace LMB
{



DLJob::DLJob( 
    const bitmap_size_t x_start,
    const bitmap_size_t y_start,
    const bitmap_size_t x_end,
    const bitmap_size_t y_end,
    DirectLightCalculator* calc)
    : LightmapChunkJob(x_start,y_start,x_end,y_end,calc->m_lightmap)
    , m_calc(calc)
{
}

void DLJob::CalculatePixel(const bitmap_size_t x,const bitmap_size_t y)
{
    auto &lightmap = LightmapChunkJob::m_lightmap;

    if(lightmap->GetFlags().GetPixel(x,y) == Lightmap::EFlags::UnUsed)
        return;

    const real_t xf = (x + to_real(0.5))/ to_real(lightmap->GetColor().GetWidth());
    const real_t yf = (y + to_real(0.5))/ to_real(lightmap->GetColor().GetHeight());

    const vec4 direct = m_calc->CalcPixel(x,y,
        lightmap->GetPos().GetPixel(xf,yf),lightmap->GetNorm().GetPixel(xf,yf));
    
    const vec4 color = lightmap->GetColor().GetPixel(x,y);
    lightmap->GetColor().SetPixel(x,y,m_calc->GetBlend()->Blend(color,direct));
}

void DirectLightCalculator::StartCalc()
{

    bitmap_size_t chunk_size = std::max(
        (int)(m_lightmap->GetColor().GetWidth()/(JobManager::Get()->GetNumThreads()*2)),1);

    for(bitmap_size_t x=0;x<m_lightmap->GetColor().GetWidth()/chunk_size;x++)
    {
        for(bitmap_size_t y=0;y<m_lightmap->GetColor().GetHeight()/chunk_size;y++)
        {
            std::shared_ptr<DLJob> job = std::make_shared<DLJob>(
                x*chunk_size,y*chunk_size,(x+1)*chunk_size,(y+1)*chunk_size,this);

            m_jobs.push_back(job);
            JobManager::Get()->Push(std::static_pointer_cast<Job>(job));
        }
    }
}

vec4 DirectLightCalculator::CalcPixel(
    const int x,
    const int y,
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

    return vec4(ret_color,1);
}

std::vector<Ray> DirectLightCalculator::GenRays(
    const Ray &ray,
    const real_t softness,
    const vec3 &pos,
    const vec3 &norm,
    const Light::EType light_type)
{
    std::vector<Ray> rays(m_config.num_rays,ray);

    real_t s = 0;
    
    if(light_type == Light::EType::Directional)
    {
       s = softness * ray.GetLength();
    }
    else if(light_type == Light::EType::Point)
    {
        s = softness * 20;
    }


    for(size_t i=0;i<rays.size();i++)
    {
        Ray &r = rays[i];

        const vec3 pos = r.GetEnd();
        
        const real_t randx = ((rand() % 1024) - to_real(512.5)) / 512.5;
        const real_t randy = ((rand() % 1024) - to_real(512.5)) / 512.5;
        const real_t randz = ((rand() % 1024) - to_real(512.5)) / 512.5;

        const vec3 rand_pos(
            s * randx,
            s * randy,
            s * randz
        );

        r.SetEnd(pos + rand_pos);
    }

    return std::move(rays);
}


const vec3 DirectLightCalculator::CalcDirectionalLight(
    const vec3 &world_pos,
    const vec3 &world_norm,
    const size_t light_index)
{
    const vec3 &light_dir = m_lights[light_index].GetDir();
    const vec3 &light_color = m_lights[light_index].GetColor();
    const real_t light_softness = m_lights[light_index].GetSoftness();

    const Ray ray(world_pos + world_norm * m_config.bias,
        world_pos + light_dir * m_config.max_ray_distance);

    const std::vector<Ray> rays = std::move(
        GenRays(ray,light_softness,world_pos,world_norm, Light::EType::Directional));

    const real_t ray_contribute = to_real(1.0)/(real_t)rays.size();

    auto solver = m_lmb->GetSolver();

    vec3 ret(0);

    for(size_t ray_i=0;ray_i<rays.size();ray_i++)
    {
        Solver::SHitInfo hit;

        if(!solver->Intersect(rays[ray_i],hit))
        {
            const real_t n_dot_dir = glm::max(glm::dot(world_norm,light_dir),to_real(0.0));
            
            ret += light_color * n_dot_dir;
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


    const Ray ray( world_pos + world_norm * m_config.bias,light_pos);

    const std::vector<Ray> rays = std::move(
        GenRays(ray,light_softness,world_pos,world_norm,Light::EType::Point));

    const real_t ray_contribute = to_real(1.0) / (real_t)rays.size();

    const real_t attenuation = glm::min(to_real(1.0) / (ray.GetLength() * ray.GetLength()),to_real(1.0));

    auto solver = m_lmb->GetSolver();

    vec3 ret(0);

    for(size_t ray_i=0;ray_i<rays.size();ray_i++)
    {
        Solver::SHitInfo hit;

        if(!solver->Intersect(rays[ray_i],hit))
        {
            const vec3 line = light_pos-world_pos;
            const vec3 dir = glm::normalize(line);
            const real_t dist = glm::length(line);
            
            const real_t n_dot_dir = glm::max(glm::dot(world_norm,dir),to_real(0.0));
            
            const real_t color_multiplier = n_dot_dir * ray_contribute;
            
            ret += light_color * color_multiplier;
        }
    }

    ret *= attenuation;

    return ret;
}




}