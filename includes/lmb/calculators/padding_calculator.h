#pragma once

#include "lmb/calculator.h"
#include <algorithm>


namespace LMB
{

    

class PaddingCalculator : public Calculator
{

public:

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
                if(m_lightmap->GetColor().GetPixel(sx,sy).w != to_real(0.0))
                {
                    ++n;
                    col += m_lightmap->GetColor().GetPixel(sx,sy);
                }
            }
        }

        if(n <=0)
            return vec4(0,0,0,0);

        col /= n;
        col.w = 1;
        return col;
    }

    void StartCalc()
    {
        size_t num_iter=16;
        for(size_t i=0;i<num_iter;i++)
        {
            for(bitmap_size_t x=0;x<m_lightmap->GetColor().GetWidth();x++)
            {
                for(bitmap_size_t y=0;y<m_lightmap->GetColor().GetHeight();y++)
                {
                    if(m_lightmap->GetColor().GetPixel(x,y).w <= to_real(0.0))
                    {
                        vec4 color = GetPixelAvrg(x,y);
                        m_lightmap->GetColor().SetPixel(x,y,color);
                    }
                }   
            }
        }
    }

    void EndCalc()
    {

    }

    const size_t GetProgress()
    {
        return 100;
    }



};



}