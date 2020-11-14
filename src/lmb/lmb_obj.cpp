#include "lmb/lmb_obj.h"
#include "lmb/lmb.h"

namespace LMB
{

void LMBObject::SetLmb(LMBSession* lmb)
{
    m_lmb = lmb;
}

}