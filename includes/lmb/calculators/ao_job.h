#pragma once

#include "lmb/calculators/job_base_calculator.h"
#include "lmb/triangle.h"
#include "lmb/lightmap.h"


namespace LMB
{



class AOJob : public Job
{
    size_t m_x_start,m_y_start;
    size_t m_x_end,m_y_end;
    class AOCalculator* m_aocalc;
    std::shared_ptr<Lightmap> m_lightmap;

public:

    AOJob( 
        const size_t x_start,
        const size_t y_start,
        const size_t x_end,
        const size_t y_end,
        class AOCalculator* aocalc
    );

    void Execute();
};



}