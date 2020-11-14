#pragma once


namespace LMB
{

class ICalculable
{
public:

    virtual void StartCalc()=0;

    virtual void EndCalc()=0;
};

}