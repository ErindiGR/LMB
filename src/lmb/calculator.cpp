#include "lmb/calculator.h"

#include "lmb/lmb.h"

namespace LMB
{

const vec4 Calculator::GetRealColor(const bitmap_size_t x,const bitmap_size_t y)
{
    assert(GetLMB());
    
    return GetLMB()->GetLightmaps()[m_current_lightmap_handle]->GetColor().GetPixel(x,y);
}

void Calculator::SetRealColor(const bitmap_size_t x,const bitmap_size_t y,const vec4 val)
{
    assert(GetLMB());
    
    GetLMB()->GetLightmaps()[m_current_lightmap_handle]->GetColor().SetPixel(x,y,val);
}

void Calculator::CopyRealColorToTemp()
{
    assert(GetLMB());
    
    auto &a = m_working_bitmap_color[m_current_lightmap_handle];
    a->Copy(GetLMB()->GetLightmaps()[m_current_lightmap_handle]->GetColor());
}

const Lightmap::EFlags Calculator::GetRealFlags(const bitmap_size_t x,const bitmap_size_t y)
{
    assert(GetLMB());
    
    return GetLMB()->GetLightmaps()[m_current_lightmap_handle]->GetFlags().GetPixel(x,y);
}

void Calculator::SetRealFlags(const bitmap_size_t x,const bitmap_size_t y,const Lightmap::EFlags val)
{
    assert(GetLMB());
    
    GetLMB()->GetLightmaps()[m_current_lightmap_handle]->GetFlags().SetPixel(x,y,val);
}

const std::shared_ptr<Lightmap>& Calculator::GetRealLightmap(const size_t lightmap)
{
    assert(GetLMB());

    return GetLMB()->GetLightmaps()[lightmap];
}

}