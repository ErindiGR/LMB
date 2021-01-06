#pragma once

#include "lmb/calculator.h"
#include "lmb/base_type.h"


namespace LMB
{



class DenoiseCalculator : public Calculator
{

public:

    DenoiseCalculator(const uint8_t num_iterations,const real_t contributing_amount);

    const vec4 GetPixelAvrg(const bitmap_size_t x,const bitmap_size_t y);

    void PixelSimilarityBlur(const bitmap_size_t x,const bitmap_size_t y);

    void SimilarityBlur();

    void StartCalc(const size_t lightmap) override;

    void EndCalc() override {}

    const Progress<0,100> GetProgress() const override
    {
        return Progress<0,100>(100);
    }

protected:

    uint8_t m_iteration;
    real_t   m_amount;
    size_t m_current_lightmap;

};


}