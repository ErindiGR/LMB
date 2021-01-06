#include "lmb/calculator.h"

#include "lmb/lmb.h"

namespace LMB
{


void Calculator::ApplyResultTempColor(size_t lightmap_handle)
{

    CalcLightmapHelper helper(lightmap_handle,this);

    for(bitmap_size_t x=0;x< helper.GetLightmapWidth();x++)
    {
        for(bitmap_size_t y=0;y< helper.GetLightmapHeight();y++)
        {
            const vec4 temp = helper.GetColor(x,y);
            const vec4 real = helper.GetRealColor(x,y);

            helper.SetRealColor(x,y,GetBlend()->Blend(real,temp));
        }
    }
}

const std::shared_ptr<Lightmap>& Calculator::GetRealLightmap(const size_t lightmap)
{
    assert(GetLMB());

    return GetLMB()->GetLightmaps()[lightmap];
}


//CalcLightmapHelper

const vec4 CalcLightmapHelper::GetRealColor(const bitmap_size_t x,const bitmap_size_t y)
{
    assert(m_calc->GetLMB());
    
    return m_calc->GetLMB()->GetLightmaps()[m_current_lightmap_handle]->GetColor().GetPixel(x,y);
}

void CalcLightmapHelper::SetRealColor(const bitmap_size_t x,const bitmap_size_t y,const vec4 val)
{
    assert(m_calc->GetLMB());
    
    m_calc->GetLMB()->GetLightmaps()[m_current_lightmap_handle]->GetColor().SetPixel(x,y,val);
}

void CalcLightmapHelper::CopyRealColorToTemp()
{
    assert(m_calc->GetLMB());
    
    auto &a = m_calc->m_working_bitmap_color[m_current_lightmap_handle];
    a->Copy(m_calc->GetLMB()->GetLightmaps()[m_current_lightmap_handle]->GetColor());
}

const Lightmap::EFlags CalcLightmapHelper::GetRealFlags(const bitmap_size_t x,const bitmap_size_t y)
{
    assert(m_calc->GetLMB());
    
    return m_calc->GetLMB()->GetLightmaps()[m_current_lightmap_handle]->GetFlags().GetPixel(x,y);
}

void CalcLightmapHelper::SetRealFlags(const bitmap_size_t x,const bitmap_size_t y,const Lightmap::EFlags val)
{
    assert(m_calc->GetLMB());
    
    m_calc->GetLMB()->GetLightmaps()[m_current_lightmap_handle]->GetFlags().SetPixel(x,y,val);
}

const vec3 CalcLightmapHelper::GetRealPos(const bitmap_size_t x,const bitmap_size_t y)
{
    assert(m_calc->GetLMB());
    
    return m_calc->GetLMB()->GetLightmaps()[m_current_lightmap_handle]->GetPos().GetPixel(x,y);
}
    
const vec3 CalcLightmapHelper::GetRealNorm(const bitmap_size_t x,const bitmap_size_t y)
{
    assert(m_calc->GetLMB());
    
    return m_calc->GetLMB()->GetLightmaps()[m_current_lightmap_handle]->GetNorm().GetPixel(x,y);
}

const vec3 CalcLightmapHelper::GetRealPos(const real_t x,const real_t y)
{
    assert(m_calc->GetLMB());
    
    return m_calc->GetLMB()->GetLightmaps()[m_current_lightmap_handle]->GetPos().GetPixel(x,y);
}
    
const vec3 CalcLightmapHelper::GetRealNorm(const real_t x,const real_t y)
{
    assert(m_calc->GetLMB());
    
    return m_calc->GetLMB()->GetLightmaps()[m_current_lightmap_handle]->GetNorm().GetPixel(x,y);
}

}