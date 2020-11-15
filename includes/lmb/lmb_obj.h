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

    void SetLmb(LMBSession* lmb);

protected:

   LMBSession* m_lmb;
   
};

}