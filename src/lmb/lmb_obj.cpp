#include "lmb/lmb_obj.h"
#include "lmb/lmb.h"

namespace LMB
{

void LMBObject::SetLMB(LMBSession* lmb)
{
    m_lmb = lmb;
}

LMBSession* LMBObject::GetLMB()const
{
    return m_lmb;
}

}