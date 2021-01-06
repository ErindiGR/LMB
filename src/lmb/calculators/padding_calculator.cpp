#include "lmb/calculators/padding_calculator.h"


namespace LMB
{

bool PaddingCalculator::GetPixelAvrg(const bitmap_size_t x,const bitmap_size_t y,vec4 &out_color)
{

    CalcLightmapHelper calc_helper(m_current_lightmap,this);

    int n=0;

    vec4 col(0);

    int ix = x;
    int iy = y;

    for(bitmap_size_t sy= iy-1;sy<=(iy+1);sy++)
    {
        for(bitmap_size_t sx= ix-1;sx<=(ix+1);sx++)
        {
            if(calc_helper.GetRealFlags(sx,sy) == Lightmap::EFlags::Used)
            {
                ++n;
                col += calc_helper.GetColor(sx,sy);
            }
        }
    }

    if(n <= 0)
        return false;

    col /= n;
    col.w = 1;
    out_color = col;

    return true;
}

void PaddingCalculator::StartCalc(const size_t lightmap)
{
    Calculator::StartCalc(lightmap);

    m_current_lightmap = lightmap;

    CalcLightmapHelper calc_helper(lightmap,this);

    calc_helper.CopyRealColorToTemp();

    struct SPaddingHelper
    {
        vec4 color;
        ivec2 pos;
    };

    std::vector<SPaddingHelper> padding_pixels;

    padding_pixels.reserve(2000);
    
    int num_iter=16;
    for(int i=0;i<num_iter;i++)
    {
        
        for(bitmap_size_t y=0;y<calc_helper.GetLightmapHeight();y++)
        {
            for(bitmap_size_t x=0;x<calc_helper.GetLightmapWidth();x++)
            {
                if(calc_helper.GetRealFlags(x,y) == Lightmap::EFlags::UnUsed)
                {
                    vec4 color(0);
                    if(GetPixelAvrg(x,y,color))
                    {
                        padding_pixels.push_back({color,ivec2(x,y)});
                    }
                }
            }   
        }

        for(size_t j=0;j<padding_pixels.size();j++)
        {
            calc_helper.SetColor(padding_pixels[j].pos.x,padding_pixels[j].pos.y,padding_pixels[j].color);
            calc_helper.SetRealFlags(padding_pixels[j].pos.x,padding_pixels[j].pos.y,Lightmap::EFlags::Used);
        }

        padding_pixels.clear();
    }
}

}