#include "lmb/calculators/lightmap_chunks_job.h"

namespace LMB
{

LightmapChunkJob::LightmapChunkJob(
    const bitmap_size_t x_start,
    const bitmap_size_t y_start,
    const bitmap_size_t x_end,
    const bitmap_size_t y_end,
    const size_t lightmap,
    Calculator* calc)
{
    m_x_start  = x_start;
    m_y_start  = y_start;
    m_x_end    = x_end;
    m_y_end    = y_end;
    m_lightmap = lightmap;
    m_calc = calc;
    m_interpolate = false;
}

void LightmapChunkJob::Execute()
{


    if(!m_interpolate)
    {
        bitmap_size_t x = m_x_start;
        bitmap_size_t y = m_y_start;
        
        size_t num_loops = (m_x_end - m_x_start) * (m_y_end - m_y_start);

        for(size_t i = 0;i < num_loops;i++)
        {
            CalculatePixel(x,y);

            ++x;
            if(x >= m_x_end)
            {
                x = m_x_start;
                y++;
            }
        }
    }
    else
    {
        for(bitmap_size_t y=m_y_start;y<m_y_end;y+=2)
        {
            for(bitmap_size_t x=m_x_start;x<m_x_end;x+=2)
            {
                CalculatePixel(x,y);
            }
        }

        for(bitmap_size_t y=m_y_start;y<m_y_end;y++)
        {
            CalculatePixel(m_x_end-1,y);
        }

        for(bitmap_size_t x=m_x_start;x<m_x_end;x++)
        {
            CalculatePixel(x,m_y_end-1);
        }

#define LIGHTMAP_CHUNK_JOB_INTERPOLATION_DEBUG 1

        CalcLightmapHelper calc_helper(m_lightmap,m_calc);

        real_t threshold = 0.06;

        for(bitmap_size_t y=m_y_start+1;y<m_y_end-2;y+=2)
        {
            for(bitmap_size_t x=m_x_start;x<m_x_end;x+=2)
            {
                real_t dif = glm::distance(calc_helper.GetColor(x,y-1),calc_helper.GetColor(x,y+1));

                if(dif > threshold)
                {
                    CalculatePixel(x,y);
                }
                else
                {

#if LIGHTMAP_CHUNK_JOB_INTERPOLATION_DEBUG
                    calc_helper.SetColor(x,y,glm::lerp(
                        calc_helper.GetColor(x,y-1),
                        calc_helper.GetColor(x,y+1),
                        to_real(0.5)));
#else
                    calc_helper.SetColor(x,y,vec4(1,0,0,1));
#endif
                }
            }
        }

        for(bitmap_size_t y=m_y_start;y<m_y_end;y+=2)
        {
            for(bitmap_size_t x=m_x_start+1;x<m_x_end-2;x+=2)
            {
                real_t dif = glm::distance(calc_helper.GetColor(x-1,y),calc_helper.GetColor(x+1,y));

                if(dif > threshold)
                {
                    CalculatePixel(x,y);
                }
                else
                {

#if LIGHTMAP_CHUNK_JOB_INTERPOLATION_DEBUG
                    calc_helper.SetColor(x,y,glm::lerp(
                        calc_helper.GetColor(x-1,y),
                        calc_helper.GetColor(x+1,y),
                        to_real(0.5)));
#else
                    calc_helper.SetColor(x,y,vec4(0,1,0,1));
#endif
                }
            }
        }

        for(bitmap_size_t y=m_y_start+1;y<m_y_end-2;y+=2)
        {
            for(bitmap_size_t x=m_x_start+1;x<m_x_end-2;x+=2)
            {
                real_t dif = glm::max(glm::distance(calc_helper.GetColor(x-1,y),calc_helper.GetColor(x+1,y)),
                glm::distance(calc_helper.GetColor(x-1,y),calc_helper.GetColor(x+1,y)));

                if(dif > threshold)
                {
                    CalculatePixel(x,y);
                }
                else
                {

#if LIGHTMAP_CHUNK_JOB_INTERPOLATION_DEBUG
                    vec4 t = glm::lerp(
                        calc_helper.GetColor(x-1,y),
                        calc_helper.GetColor(x+1,y),
                        to_real(0.5));

                    vec4 b = glm::lerp(
                        calc_helper.GetColor(x,y-1),
                        calc_helper.GetColor(x,y+1),
                        to_real(0.5));

                    vec4 color = glm::lerp(t,b,to_real(0.5));

                    calc_helper.SetColor(x,y,color);
#else
                    calc_helper.SetColor(x,y,vec4(0,0,1,1));
#endif
                }
            }
        }
        
    }
}

void LightmapChunkJob::CalculatePixel(const bitmap_size_t x,const bitmap_size_t y)
{
    CalcLightmapHelper calc_helper(m_lightmap,m_calc);

    auto &lightmap = m_calc->GetRealLightmap(m_lightmap);

    if(calc_helper.GetRealFlags(x,y) == Lightmap::EFlags::UnUsed)
        return;

    const vec2 pixel_pos = GetPixelCenter(x,y);

    LightmapChunkJobBaseCalculator* calc = (LightmapChunkJobBaseCalculator*)m_calc;

    const vec4 res = calc->CalcPixel(
        calc_helper.GetRealPos(pixel_pos.x,pixel_pos.y),
        calc_helper.GetRealNorm(pixel_pos.x,pixel_pos.y));

    calc_helper.SetColor(x,y,res);
}

const vec2 LightmapChunkJob::GetPixelCenter(const bitmap_size_t x,const bitmap_size_t y)
{
    auto &lightmap = m_calc->GetTempLightmapColor(m_lightmap);
    const real_t xf = (x + to_real(0.5)) / to_real(lightmap->GetWidth());
    const real_t yf = (y + to_real(0.5)) / to_real(lightmap->GetHeight());

    return vec2(xf,yf);
}

const vec2 LightmapChunkJob::GetPixelPoint(const bitmap_size_t x,const bitmap_size_t y,const real_t r,const real_t t)
{
    auto &lightmap = m_calc->GetTempLightmapColor(m_lightmap);
    const real_t xf = (x + r) / to_real(lightmap->GetWidth());
    const real_t yf = (y + t) / to_real(lightmap->GetHeight());

    return vec2(xf,yf);
}

void LightmapChunkJobBaseCalculator::StartCalc(const size_t lightmap)
{
    Calculator::StartCalc(lightmap);

    CalcLightmapHelper calc_helper(lightmap,this);

    bitmap_size_t chunk_size = 32;

    for(bitmap_size_t x=0;x<calc_helper.GetLightmapWidth()/chunk_size;x++)
    {
        for(bitmap_size_t y=0;y<calc_helper.GetLightmapHeight()/chunk_size;y++)
        {
            std::shared_ptr<LightmapChunkJob> job = std::make_shared<LightmapChunkJob>(
                x*chunk_size,
                y*chunk_size,
                (x+1)*chunk_size,
                (y+1)*chunk_size,
                lightmap,
                this
            );

            m_jobs.push_back(std::static_pointer_cast<Job>(job));
            JobManager::Push(std::static_pointer_cast<Job>(job));
        }
    }
}

}