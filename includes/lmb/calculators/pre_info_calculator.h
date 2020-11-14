#pragma once

#include "lmb/calculators/job_base_calculator.h"
#include "lmb/calculators/lightmap_chunks_job.h"
#include "lmb/thread_job.h"



namespace LMB
{

class PreInfoCalculator;

class PreInfoJob : public LightmapChunkJob
{

    PreInfoCalculator* m_calc;

public:

    PreInfoJob( 
        const bitmap_size_t x_start,
        const bitmap_size_t y_start,
        const bitmap_size_t x_end,
        const bitmap_size_t y_end,
        PreInfoCalculator* calc
    );

    void CalculatePixel(const bitmap_size_t x,const bitmap_size_t y);

};


class PreInfoCalculator : public JobBaseCalculator
{

public:

    void StartCalc();

protected:

    friend PreInfoJob;

};


}