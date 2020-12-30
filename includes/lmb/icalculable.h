#pragma once


namespace LMB
{

class ICalculable
{
public:

    virtual void StartCalc(const size_t lightmap)=0;

    virtual void EndCalc()=0;
};

}