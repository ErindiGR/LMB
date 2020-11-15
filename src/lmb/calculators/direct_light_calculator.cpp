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
{
    m_x_start = x_start;
    m_y_start = y_start;
    m_x_end = x_end;
    m_y_end = y_end;
    m_calc = calc;
    m_lightmap = calc->m_lightmap;
}

void DLJob::Execute()
{

    for(bitmap_size_t x=m_x_start;x<m_x_end;x++)
        for(bitmap_size_t y=m_y_start;y<m_y_end;y++)
        {
            real_t xf = ((real_t)x + to_real(0.5))/(real_t)m_lightmap->GetColor().GetWidth();
            real_t yf = ((real_t)y + to_real(0.5))/(real_t)m_lightmap->GetColor().GetHeight();

            const vec4 direct = m_calc->CalcPixel(x,y,
                m_lightmap->GetPos().GetPixel(xf,yf),m_lightmap->GetNorm().GetPixel(xf,yf));
            
            const vec4 color = m_lightmap->GetColor().GetPixel(x,y);
            m_lightmap->GetColor().SetPixel(x,y,m_calc->GetBlend()->Blend(color,direct));
        }
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
    if(glm::length(world_pos)<=to_real(0.0))
        return vec4(0);
    
    vec3 ret_color(m_ambient_color);

    for(size_t i=0;i<m_lights.size();i++)
    {
        if(m_lights[i].GetType() == Light::Directional)
        {
            ret_color += CalcDirectionalLight(world_pos,world_norm,i);
        }
        else if(m_lights[i].GetType() == Light::Point)
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
    const vec3 &norm)
{
    std::vector<Ray> rays(m_num_rays,ray);

    const real_t s = softness * ray.GetLength();

    for(size_t i=0;i<rays.size();i++)
    {
        Ray &r = rays[i];

        const vec3 pos = r.GetEnd();

        const vec3 rand_pos(
            s * (to_real(1.0)/(rand()%32-16.5f)),
            s * (to_real(1.0)/(rand()%32-16.5f)),
            s * (to_real(1.0)/(rand()%32-16.5f))
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

    const Ray ray(world_pos + world_norm * m_bias,
        world_pos + light_dir * m_max_ray_distance);

    const std::vector<Ray> rays = std::move(
        GenRays(ray,light_softness,world_pos,world_norm));

    const real_t ray_contribute = to_real(1.0)/(real_t)rays.size();

    auto solver = m_lmb->GetSolver();

    vec3 ret(0);

    for(size_t ray_i=0;ray_i<rays.size();ray_i++)
    {
        vec3 uvw(0);
        real_t t;

        if(!solver->Intersect(rays[ray_i],uvw,t))
        {
            const real_t n_dot_dir = glm::max(glm::dot(world_norm,light_dir),to_real(0.0));
            const real_t color_multiplier = n_dot_dir * ray_contribute;
            
            ret += light_color * color_multiplier;
        }
    }

    return ret;
}

const vec3 DirectLightCalculator::CalcPointLight(
    const vec3 &world_pos,
    const vec3 &world_norm,
    const size_t light_index)
{
    const vec3 &light_pos = m_lights[light_index].GetPos();
    const vec3 &light_color = m_lights[light_index].GetColor();
    const real_t light_softness = m_lights[light_index].GetSoftness();


    const Ray ray(
        world_pos + world_norm * m_bias,
        light_pos
    );

    const std::vector<Ray> rays = std::move(
        GenRays(ray,light_softness,world_pos,world_norm)
    );

    const real_t ray_contribute = to_real(1.0)/(real_t)rays.size();

    auto solver = m_lmb->GetSolver();

    vec3 ret(0);

    for(size_t ray_i=0;ray_i<rays.size();ray_i++)
    {
        vec3 uvw(0);
        real_t t;

        if(!solver->Intersect(rays[ray_i],uvw,t))
        {
            const vec3 line = light_pos-world_pos;
            const vec3 dir = glm::normalize(line);
            const real_t dist = glm::length(line);

            const real_t inverse_dist = to_real(1.0) / dist;
            
            const real_t n_dot_dir = glm::max(glm::dot(world_norm,dir),to_real(0.0));
            
            const real_t color_multiplier = inverse_dist * n_dot_dir * ray_contribute;
            
            ret += light_color * color_multiplier;
        }
    }

    return ret;
}




}