#pragma once

#include "lmb/thread_job.h"
#include "lmb/lightmap.h"

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
        const std::shared_ptr<Lightmap> &lightmap)
    {
        m_x_start  = x_start;
        m_y_start  = y_start;
        m_x_end    = x_end;
        m_y_end    = y_end;
        m_lightmap = lightmap;
    }

    /*void Execute()
    {

        for(bitmap_size_t x=m_x_start;x<m_x_end;x++)
        {
            for(bitmap_size_t y=m_y_start;y<m_y_end;y++)
            {
                CalculatePixel(x,y);
            }
        }
    }*/

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

    virtual void CalculatePixel(const bitmap_size_t x,const bitmap_size_t y) =0;

protected:

    bitmap_size_t m_x_start;
    bitmap_size_t m_y_start;
    bitmap_size_t m_x_end;
    bitmap_size_t m_y_end;
    std::shared_ptr<Lightmap> m_lightmap;

};


}