#pragma once

#include "lmb/calculator.h"
#include "lmb/base_type.h"


namespace LMB
{



class DenoisCalculator : public Calculator
{

public:

    DenoisCalculator(const uint8_t num_iterations,const real_t contributing_amount)
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

        for(bitmap_size_t sx= ix-1;sx<(ix+2);sx++)
        {
            for(bitmap_size_t sy= iy-1;sy<(iy+2);sy++)
            {
                if(sx==x && sy==y)
                    continue;

                if(sx == sy)
                    continue;

                if(m_lightmap->GetColor().GetPixel(sx,sy).w != 0)
                {
                    ++n;
                    col +=m_lightmap->GetColor().GetPixel(sx,sy);
                }
            }
        }

        if(n <=0)
            return vec4(0,0,0,0);

        col /=n;
        return col;
    }

    void PixelSimilarityBlur(const bitmap_size_t x,const bitmap_size_t y)
    {
        if(m_lightmap->GetColor().GetPixel(x,y).w == 0)
            return;
        
        const vec4 a = GetPixelAvrg(x,y);

        vec4 c = m_lightmap->GetColor().GetPixel(x,y);

        const real_t vec3_len = glm::length(vec3(1));

        const real_t sim = to_real(1.0) - (glm::length(vec3(a)-vec3(c)) / vec3_len);

        const real_t amount = glm::min(m_amount * sim,to_real(1.0));

        c = glm::lerp(c,a,amount);

        m_lightmap->GetColor().SetPixel(x,y,c);
    }

    void SimiarityBlur()
    {
        for(bitmap_size_t x=0;x<m_lightmap->GetColor().GetWidth();x++)
        {
            for(bitmap_size_t y=0;y<m_lightmap->GetColor().GetHeight();y++)
            {
                PixelSimilarityBlur(x,y);
            }   
        }
    }

    void StartCalc()
    {
        for(size_t i=0;i<m_iteration;i++)
        {
            SimiarityBlur();
        }
    }

    void EndCalc()
    {

    }

    const size_t GetProgress()
    {
        return 100;
    }

protected:

    uint8_t m_iteration;
    real_t   m_amount;

};


}