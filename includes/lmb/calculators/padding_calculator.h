#pragma once

#include "lmb/calculator.h"
#include <algorithm>


namespace LMB
{

    

class PaddingCalculator : public Calculator
{

public:

    inline bool GetPixelAvrg(const bitmap_size_t x,const bitmap_size_t y,vec4 &out_color)
    {
        int n=0;

        vec4 col(0);

        int ix = x;
        int iy = y;

        for(bitmap_size_t sx= ix-1;sx<(ix+2);sx++)
        {
            for(bitmap_size_t sy= iy-1;sy<(iy+2);sy++)
            {
                if(m_lightmap->GetFlags().GetPixel(sx,sy) == Lightmap::EFlags::Used)
                {
                    ++n;
                    col += m_lightmap->GetColor().GetPixel(sx,sy);
                }
            }
        }

        if(n <=0)
            return false;

        col /= n;
        col.w = 1;
        out_color = col;

        return true;
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
                    if(m_lightmap->GetFlags().GetPixel(x,y) == Lightmap::EFlags::UnUsed)
                    {
                        vec4 color(0);
                        if(GetPixelAvrg(x,y,color))
                        {
                            m_lightmap->GetColor().SetPixel(x,y,color);
                            m_lightmap->GetFlags().SetPixel(x,y,Lightmap::EFlags::Used);
                        }
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