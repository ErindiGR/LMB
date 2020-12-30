#pragma once

#include "lmb/calculator.h"
#include "lmb/base_type.h"


namespace LMB
{



class DenoiseCalculator : public Calculator
{

public:

    DenoiseCalculator(const uint8_t num_iterations,const real_t contributing_amount)
    {
        m_iteration = num_iterations;
        m_amount = contributing_amount;
    }

    inline const vec4 GetPixelAvrg(const bitmap_size_t x,const bitmap_size_t y)
    {
        int n=0;

        vec4 col(0);

        int ix = x;
        int iy = y;

        const int num_samples = 2;

        for(bitmap_size_t sy= iy-num_samples;sy<=(iy+num_samples);sy++)
        {
            for(bitmap_size_t sx= ix-num_samples;sx<=(ix+num_samples);sx++)
            {
                if(sx==x && sy==y)
                    continue;

                if(GetRealFlags(sx,sy) == Lightmap::EFlags::UnUsed)
                    continue;

                ++n;
                
                col += GetColor(sx,sy);
                
            }
        }

        if(n <= 0)
            return vec4(0,0,0,0);

        col /=n;
        return col;
    }

    void PixelSimilarityBlur(const bitmap_size_t x,const bitmap_size_t y)
    {
        if(GetRealFlags(x,y) == Lightmap::EFlags::UnUsed)
            return;
        
        const vec3 a = GetPixelAvrg(x,y);

        vec3 c = GetColor(x,y);
        
        const vec3 luminance = vec3(0.2125, 0.7154, 0.0721);

        const real_t c_lum = glm::dot(luminance,c);
        const real_t a_lum = glm::dot(luminance,a);

        const real_t sim = to_real(1.0) - glm::min(a_lum-c_lum,to_real(1.0));

        const real_t amount = glm::min(m_amount * sim,to_real(1.0));

        c = glm::lerp(c,a,amount);

        SetColor(x,y,vec4(c,to_real(1.0)));
    }

    void SimiarityBlur()
    {
        for(bitmap_size_t y=0;y<GetLightmapHeight();y++)
        {
            for(bitmap_size_t x=0;x<GetLightmapWidth();x++)
            {
                PixelSimilarityBlur(x,y);
            }   
        }
    }

    void StartCalc(const size_t lightmap)
    {
        Calculator::StartCalc(lightmap);

        CopyRealColorToTemp();

        for(size_t i=0;i<m_iteration;i++)
        {
            SimiarityBlur();
        }
    }

    void EndCalc()
    {

    }

    const Progress<0,100> GetProgress() const
    {
        return Progress<0,100>(100);
    }

protected:

    uint8_t m_iteration;
    real_t   m_amount;

};


}