#include <lmb/calculators/ao_job.h>
#include <lmb/calculators/ao_calculator.h>
#include <lmb/lmb.h>


namespace LMB
{



AOJob::AOJob(
    const size_t x_start,
    const size_t y_start,
    const size_t x_end,
    const size_t y_end,
    AOCalculator* aocalc
)
{
    m_x_start = x_start;
    m_y_start = y_start;
    m_x_end = x_end;
    m_y_end = y_end;
    m_aocalc = aocalc;
    m_lightmap = aocalc->m_lightmap;
}

void AOJob::Execute()
{

    for(bitmap_size_t x=m_x_start;x<m_x_end;x++)
    for(bitmap_size_t y=m_y_start;y<m_y_end;y++)
    {
        real_t xf = ((real_t)x+ to_real(0.5))/(real_t)m_lightmap->GetColor().GetWidth();
        real_t yf = ((real_t)y+ to_real(0.5))/(real_t)m_lightmap->GetColor().GetHeight();

        const vec4 ao = m_aocalc->CalcPixel(
            x,y,
            m_lightmap->GetPos().GetPixel(xf,yf),
            m_lightmap->GetNorm().GetPixel(xf,yf)
        );
        

        const vec4 color = m_lightmap->GetColor().GetPixel(x,y);
        m_lightmap->GetColor().SetPixel(x,y,m_aocalc->GetBlend()->Blend(color,ao));
    }
}


}