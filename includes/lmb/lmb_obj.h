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

    bool HasLMB()
    {
        return (m_lmb);
    }

private:

   LMBSession* m_lmb;
   
};

}