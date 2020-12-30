#pragma once

#include <memory>

namespace LMB
{

class LMBSession;

class LMBObject
{

public:

    LMBObject()
    {
        m_lmb = nullptr;
    }

    void SetLMB(LMBSession* lmb);

    LMBSession* GetLMB()const;

private:

   LMBSession* m_lmb;
   
};

}