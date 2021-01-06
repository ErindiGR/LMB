#pragma once

#include "lmb/thread_job.h"
#include "lmb/lightmap.h"
#include "lmb/calculator.h"
#include "lmb/calculators/job_base_calculator.h"

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
        Calculator* calc);

    void Execute() override;

    const vec2 GetPixelCenter(const bitmap_size_t x,const bitmap_size_t y);

    const vec2 GetPixelPoint(const bitmap_size_t x,const bitmap_size_t y,const real_t r,const real_t t);

    virtual void CalculatePixel(const bitmap_size_t x,const bitmap_size_t y);

protected:

    bitmap_size_t m_x_start;
    bitmap_size_t m_y_start;
    bitmap_size_t m_x_end;
    bitmap_size_t m_y_end;
    size_t m_lightmap;
    Calculator* m_calc;
    bool m_interpolate;
};


class LightmapChunkJobBaseCalculator : public JobBaseCalculator
{

public:

    void StartCalc(const size_t lightmap) override;

    virtual vec4 CalcPixel(
        const vec3 &world_pos,
        const vec3 &world_norm)=0;


};


}