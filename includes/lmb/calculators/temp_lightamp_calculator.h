#pragma once

#include "job_base_calculator.h"
#include "lmb/lightmap.h"

namespace LMB
{

class TempLigthmapCalculator : public JobBaseCalculator
{

public:

    TempLigthmapCalculator(const bool use_temp)
    {
        m_use_temp = use_temp;
    }

    void SetLightmap(std::shared_ptr<Lightmap> &lightmap)
    {
        Calculator::SetLightmap(lightmap);

        if(m_use_temp && m_lightmap)
        {
            m_temp_lightmap = std::make_shared<Lightmap>(m_lightmap->GetColor().GetWidth(),
                m_lightmap->GetColor().GetHeight(),0);
            
            m_temp_lightmap->GetPos().Copy(m_lightmap->GetPos());
            m_temp_lightmap->GetNorm().Copy(m_lightmap->GetNorm());
            m_temp_lightmap->GetFlags().Copy(m_lightmap->GetFlags());
        }
    }

    void EndCalc()
    {
        JobBaseCalculator::EndCalc();

        if(!m_temp_lightmap)
            return;

        for(bitmap_size_t x = 0; x < m_temp_lightmap->GetColor().GetWidth();x++)
        {
            for(bitmap_size_t y = 0; y < m_temp_lightmap->GetColor().GetHeight();y++)
            {
                const vec4 color = GetBlend()->Blend(
                    m_lightmap->GetColor().GetPixel(x,y),m_temp_lightmap->GetColor().GetPixel(x,y));
                
                m_lightmap->GetColor().SetPixel(x,y,color);
            }
        }
    }

    inline std::shared_ptr<Lightmap> GetLightmap()
    {
        if(m_temp_lightmap)
        {       
            return m_temp_lightmap;
        }
        else
        {
            if(m_lightmap)
            {
                return m_lightmap;
            }
            else
            {   
                return m_temp_lightmap;
            }
        }
    }

protected:

    std::shared_ptr<Lightmap> m_temp_lightmap;
    bool m_use_temp;

};

}