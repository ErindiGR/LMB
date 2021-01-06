#pragma once

#include "lmb/calculator.h"
#include "lmb/base_type.h"
#include <algorithm>


namespace LMB
{

    

class PaddingCalculator : public Calculator
{

public:

    bool GetPixelAvrg(const bitmap_size_t x,const bitmap_size_t y,vec4 &out_color);

    void StartCalc(const size_t lightmap) override;

    void EndCalc() override{}

    const Progress<0,100> GetProgress() const override
    {
        return Progress<0,100>(100);
    }

protected:

    size_t m_current_lightmap;

};



}