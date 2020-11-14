#pragma once

namespace LMB
{

class CalcBlendSet
{

public:

    virtual vec4 Blend(const vec4 &rh,const vec4 &lh) const
    {
        return lh;
    }

};


class CalcBlendMul : public CalcBlendSet
{

public:

    virtual vec4 Blend(const vec4 &rh,const vec4 &lh) const
    {
        return rh * lh;
    };

};


class CalcBlendAdd : public CalcBlendSet
{

public:

    virtual vec4 Blend(const vec4 &rh,const vec4 &lh) const
    {
        return rh + lh;
    };

};

}