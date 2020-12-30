#pragma once

#include "lmb/thread_job.h"
#include "lmb/lightmap.h"
#include "lmb/calculator.h"

namespace LMB
{

/**
* @brief Calculates each pixel of a chunk of the lightmap
* this way you dont need a mutex to lock the pixels
*/
class LightmapChunkJob : public Job
{

public:

    LightmapChunkJob(
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
    }

    void Execute()
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
                ++y;
            }
        }
    }

    const vec2 GetPixelCenter(const bitmap_size_t x,const bitmap_size_t y)
    {
        auto &lightmap = m_calc->GetTempLightmapColor(m_lightmap);
        const real_t xf = (x + to_real(0.5)) / to_real(lightmap->GetWidth());
        const real_t yf = (y + to_real(0.5)) / to_real(lightmap->GetHeight());

        return vec2(xf,yf);
    }

    const vec2 GetPixelPoint(const bitmap_size_t x,const bitmap_size_t y,const real_t r,const real_t t)
    {
        auto &lightmap = m_calc->GetTempLightmapColor(m_lightmap);
        const real_t xf = (x + r) / to_real(lightmap->GetWidth());
        const real_t yf = (y + t) / to_real(lightmap->GetHeight());

        return vec2(xf,yf);
    }

    virtual void CalculatePixel(const bitmap_size_t x,const bitmap_size_t y) =0;

protected:

    bitmap_size_t m_x_start;
    bitmap_size_t m_y_start;
    bitmap_size_t m_x_end;
    bitmap_size_t m_y_end;
    size_t m_lightmap;
    Calculator* m_calc;
};


}