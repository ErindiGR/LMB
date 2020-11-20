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
    IndirectLightCalculator* aocalc)
    : LightmapChunkJob(x_start,y_start,x_end,y_end,aocalc->GetLightmap())
    , m_aocalc(aocalc)
{
}

void ILJob::CalculatePixel(const bitmap_size_t x,const bitmap_size_t y)
{
    auto &lightmap = LightmapChunkJob::m_lightmap;

    if(lightmap->GetFlags().GetPixel(x,y) == Lightmap::EFlags::UnUsed)
        return;

    real_t xf = ((real_t)x+ to_real(0.5))/(real_t)lightmap->GetColor().GetWidth();
    real_t yf = ((real_t)y+ to_real(0.5))/(real_t)lightmap->GetColor().GetHeight();

    const vec4 gi = m_aocalc->CalcPixel(x,y,
        lightmap->GetPos().GetPixel(xf,yf),lightmap->GetNorm().GetPixel(xf,yf));

    const vec4 color = lightmap->GetColor().GetPixel(x,y) * to_real(0.5) + gi * to_real(0.5);

    lightmap->GetColor().SetPixel(x,y,color);
}

void IndirectLightCalculator::StartCalc()
{
    DEBUG_LOG("Started Calculating AO.\n");


    size_t chunk_size = std::max((int)(GetLightmap()->GetColor().GetWidth()/(JobManager::Get()->GetNumThreads()*2)),1);

    for(int i=0;i<m_config.num_bounces;i++)
    {
        for(size_t x=0;x<GetLightmap()->GetColor().GetWidth()/chunk_size;x++)
        {
            for(size_t y=0;y<GetLightmap()->GetColor().GetHeight()/chunk_size;y++)
            {
                std::shared_ptr<Job> job = std::make_shared<ILJob>(
                    x*chunk_size,y*chunk_size,(x+1)*chunk_size,(y+1)*chunk_size,this);

                m_jobs.push_back(job);
                JobManager::Get()->Push(std::static_pointer_cast<Job>(job));
            }
        }
    }

}





vec4 IndirectLightCalculator::CalcPixel(
    const int x,
    const int y,
    const vec3 &world_pos,
    const vec3 &world_norm)
{

    const std::vector<Ray> rays = std::move(GenRays(world_pos,world_norm));

    auto solver = m_lmb->GetSolver();

    vec3 ret_color(0);

    const real_t ray_contribution = to_real(1.0) / (real_t)rays.size();
    

    for(size_t ray_i=0;ray_i<rays.size();ray_i++)
    {    
        Solver::SHitInfo hit;

        if(solver->Intersect(rays[ray_i],hit))
        {
            assert(m_lmb->GetLightmaps().size());

            auto &hit_triangle = m_lmb->GetTriangles()[hit.triangle_index];

            assert(hit_triangle.GetLightmap() <= m_lmb->GetLightmaps().size());

            auto &hit_lightmap = m_lmb->GetLightmaps()[hit_triangle.GetLightmap()];
            
            assert(hit_lightmap);
            
            const vec2 hit_uv = hit_triangle.BarycentricToLightmapUV(hit.uvw);
            
            const vec3 hit_light = hit_lightmap->GetColor().GetPixel(hit_uv.x,hit_uv.y);

            const real_t n_dot_l = glm::max(glm::dot(world_norm,rays[ray_i].GetDir()),to_real(0.0));
            
            const vec3 recived_light = hit_light * n_dot_l;

            ret_color += recived_light /* * texture color */;
        }
        else
        {
            const real_t n_dot_l = glm::max(glm::dot(world_norm,rays[ray_i].GetDir()),to_real(0.0));
            
            const vec3 recived_light = m_config.ambient_color * n_dot_l;

            ret_color += recived_light;
        }
        
    }

    ret_color *= ray_contribution;

    return vec4(ret_color,1);

}





const std::vector<Ray> IndirectLightCalculator::GenRays(
    const vec3 &pos,
    const vec3 &norm) const
{

    std::vector<Ray> rays(m_config.num_rays);
    
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

        const real_t anglex = glm::radians(m_config.max_angle /** (to_real(1.0) - dir.y)*/ * randx );
        const real_t angley = glm::radians(m_config.max_angle /** (to_real(1.0) - dir.x)*/ * randy );
        const real_t anglez = glm::radians(m_config.max_angle /** (to_real(1.0) - dir.z)*/ * randz );

        dir = glm::rotate(dir,anglex,vec3(1,0,0));
        dir = glm::rotate(dir,angley,vec3(0,1,0));
        dir = glm::rotate(dir,anglez,vec3(0,0,1));

        dir = glm::normalize(dir);

        ray.SetEnd(ray.GetStart() + dir * m_config.ray_distance);
    }

    return std::move(rays);
}



}