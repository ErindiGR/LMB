#pragma once

#include "solver.h"
#include "lightmap.h"
#include "icalculable.h"
#include "calc_blends.h"


namespace LMB
{

class Calculator : public LMBObject , public ICalculable
{

public:

    Calculator()
    {
        m_lightmap = nullptr;
        m_blend = std::make_shared<CalcBlendSet>();
    }

    //return value 0 - 100
    virtual const size_t GetProgress()=0;

    void SetLightmap(std::shared_ptr<Lightmap> &lightmap)
    {
        m_lightmap = lightmap;
    }

    void SetBlend(const std::shared_ptr<CalcBlendSet> &blend)
    {
        m_blend = blend;
    }

    const std::shared_ptr<CalcBlendSet> &GetBlend()
    {
        return m_blend;
    }

protected:

    std::shared_ptr<Lightmap> m_lightmap;

private:
    std::shared_ptr<CalcBlendSet> m_blend;

};

}