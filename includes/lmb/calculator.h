#pragma once

#include "solver.h"
#include "lightmap.h"
#include "icalculable.h"
#include "calc_blends.h"
#include "bitmap.h"

#include <memory>


namespace LMB
{

template<int min,int max>
class Progress
{

static_assert(min < max , "min has to be smaller than max");

public:

    explicit Progress(const int val)
    {
        value = val;

        if(value < min)
            value = min;
        
        if(value > max)
            value = max;
    }

    const int GetVal()const
    {
        return value;
    }


protected:

    int value;

};

class Calculator : public LMBObject , public ICalculable
{

public:

    Calculator()
    :m_blend(std::make_shared<CalcBlendSet>())
    {
    }

    
    void StartCalc(const size_t lightmap) override
    {
    }
    

    /**
     * @brief Get the Progress 
     * 
     * @return the progress in percentage from 0 to 100
     */
    virtual const Progress<0,100> GetProgress()const = 0;

    virtual void SetLightmaps(std::vector<std::shared_ptr<Lightmap>> &lightmaps)
    {
        if(m_working_bitmap_color.size() == lightmaps.size())
            return;

        for(size_t i = m_working_bitmap_color.size();i < lightmaps.size();i++)
        {
            std::shared_ptr<Bitmap<vec4>> b = std::make_shared<Bitmap<vec4>>(lightmaps[i]->GetSize(),lightmaps[i]->GetSize());
            
            m_working_bitmap_color.push_back(b);
        }
    }

    const std::vector<std::shared_ptr<Bitmap<vec4>>>& GetAllTempLightmapColor() const
    {
        return m_working_bitmap_color;
    }

    const std::shared_ptr<Bitmap<vec4>>& GetTempLightmapColor(const size_t lightmap) const
    {
        return m_working_bitmap_color[lightmap];
    }

    void SetBlend(const std::shared_ptr<CalcBlendSet> &blend)
    {
        m_blend = blend;
    }

    const std::shared_ptr<CalcBlendSet> &GetBlend()
    {
        return m_blend;
    }

    const std::shared_ptr<Lightmap>& GetRealLightmap(const size_t lightmap);

    void ApplyResultTempColor(size_t lightmap_handle);

protected:

    friend class CalcLightmapHelper;
    std::vector<std::shared_ptr<Bitmap<vec4>>> m_working_bitmap_color;

private:

    std::shared_ptr<CalcBlendSet> m_blend;

};

class CalcLightmapHelper
{

public:

    CalcLightmapHelper(size_t lightmap, Calculator* calc)
    :m_current_lightmap_handle(lightmap)
    ,m_calc(calc)
    {
    }

    void SetColor(const bitmap_size_t x,const bitmap_size_t y,const vec4 color)
    {
        m_calc->m_working_bitmap_color[m_current_lightmap_handle]->SetPixel(x,y,color);
    }

    const vec4 GetColor(const bitmap_size_t x,const bitmap_size_t y)
    {
        return m_calc->m_working_bitmap_color[m_current_lightmap_handle]->GetPixel(x,y);
    }

    const bitmap_size_t GetLightmapWidth() const
    {
        return m_calc->m_working_bitmap_color[m_current_lightmap_handle]->GetWidth();
    }

    const bitmap_size_t GetLightmapHeight() const
    {
        return m_calc->m_working_bitmap_color[m_current_lightmap_handle]->GetHeight();
    }

    const std::shared_ptr<Bitmap<vec4>>& GetTempLightmapColor() const
    {
        return m_calc->m_working_bitmap_color[m_current_lightmap_handle];
    }

    const vec4 GetRealColor(const bitmap_size_t x,const bitmap_size_t y);

    void SetRealColor(const bitmap_size_t x,const bitmap_size_t y,const vec4 val);

    void CopyRealColorToTemp();

    const Lightmap::EFlags GetRealFlags(const bitmap_size_t x,const bitmap_size_t y);

    void SetRealFlags(const bitmap_size_t x,const bitmap_size_t y,const Lightmap::EFlags val);

    const vec3 GetRealPos(const bitmap_size_t x,const bitmap_size_t y);
    
    const vec3 GetRealNorm(const bitmap_size_t x,const bitmap_size_t y);

    const vec3 GetRealPos(const real_t x,const real_t y);
    
    const vec3 GetRealNorm(const real_t x,const real_t y);

protected:

    size_t m_current_lightmap_handle;
    Calculator* m_calc;

};

}