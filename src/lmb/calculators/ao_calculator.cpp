#include "lmb/calculators/ao_calculator.h"
#include "lmb/calculators/ao_job.h"
#include "lmb/lmb.h"
#include "lmb/debug.h"

#include <algorithm>

namespace LMB
{





void AOCalculator::StartCalc()
{
    DEBUG_LOG("Started Calculating AO.\n");

    size_t chunk_size = std::max((int)(m_lightmap->GetColor().GetWidth()/(JobManager::Get()->GetNumThreads()*2)),1);

    for(size_t x=0;x<m_lightmap->GetColor().GetWidth()/chunk_size;x++)
    {
        for(size_t y=0;y<m_lightmap->GetColor().GetHeight()/chunk_size;y++)
        {
            std::shared_ptr<AOJob> job = std::make_shared<AOJob>(
                x*chunk_size,
                y*chunk_size,
                (x+1)*chunk_size,
                (y+1)*chunk_size,
                this
            );

            m_jobs.push_back(job);
            JobManager::Get()->Push(std::static_pointer_cast<Job>(job));
        }
    }


}





vec4 AOCalculator::CalcPixel(
    const int x,
    const int y,
    const vec3 &world_pos,
    const vec3 &world_norm
)
{
    if(glm::length(world_pos)<=to_real(0.0))
        return vec4(0);
    
    const std::vector<Ray> rays = std::move(GenRays(world_pos,world_norm));

    auto solver = m_lmb->GetSolver();

    vec3 ret_color(1);

    const real_t inverse_dist = to_real(1.0) / m_ray_distance;
    const real_t ray_contribution = to_real(1.0) / (real_t)rays.size();
    
    for(size_t ray_i=0;ray_i<rays.size();ray_i++)
    {    
        vec3 uvw(0);
        real_t t=0;

        if(solver->Intersect(rays[ray_i],uvw,t))
        {
            ret_color -= (to_real(1.0) - t * inverse_dist) * ray_contribution;
        }
    }

    return vec4(ret_color,1);

}





const std::vector<Ray> AOCalculator::GenRays(
    const vec3 &pos,
    const vec3 &norm
)const
{

    std::vector<Ray> rays(m_num_rays);
    
    for(size_t i=0;i<rays.size();i++)
    {
        rays[i].SetStart(pos + norm * m_bias);
        rays[i].SetEnd(pos + norm * m_ray_distance);
    }



    for(size_t i=0;i<rays.size();i++)
    {
        Ray &ray = rays[i];


        vec3 dir = ray.GetDir();

        const real_t angley = glm::radians(m_max_angle * (to_real(1.0)-dir.y) * (to_real(1.0)/(rand()%32-16.5f)) );
        const real_t anglex = glm::radians(m_max_angle * (to_real(1.0)-dir.x) * (to_real(1.0)/(rand()%32-16.5f)) );
        const real_t anglez = glm::radians(m_max_angle * (to_real(1.0)-dir.z) * (to_real(1.0)/(rand()%32-16.5f)) );

        dir = glm::rotate(dir,angley,vec3(0,1,0));
        dir = glm::rotate(dir,anglex,vec3(1,0,0));
        dir = glm::rotate(dir,anglez,vec3(0,0,1));

        dir = glm::normalize(dir);

        ray.SetEnd(ray.GetStart() + dir * m_ray_distance);
    }

    return std::move(rays);
}



}