#pragma once

namespace LMB
{

/**
* @brief CalcBelnd are used by the calculator to blend 
* the resulting pixel with the existing pixel color
* CalcBlendSet is the default blender
* CalcBlendSet sets the current pixel color to the result
*/
class CalcBlendSet
{

public:

    virtual const vec4 Blend(const vec4 &rh,const vec4 &lh) const
    {
        return lh;
    }

};


class CalcBlendMul : public CalcBlendSet
{

public:

    const vec4 Blend(const vec4 &rh,const vec4 &lh) const
    {
        return rh * lh;
    };

};


class CalcBlendAdd : public CalcBlendSet
{

public:

    const vec4 Blend(const vec4 &rh,const vec4 &lh) const
    {
        return rh + lh;
    };

};

class CalcBlendMax : public CalcBlendSet
{

public:

    const vec4 Blend(const vec4 &rh,const vec4 &lh) const
    {
        return glm::max(rh,lh);
    };

};

class CalcBlendAvrg : public CalcBlendSet
{

public:

    const vec4 Blend(const vec4 &rh,const vec4 &lh) const
    {
        return rh / to_real(2.0) + lh / to_real(2.0);
    };

};

}