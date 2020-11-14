#pragma once

#include "memory"

namespace LMB
{

template<class T>
class Singleton
{

public:
    
    static inline T* Get()
    {
        if(!s_instance)
            s_instance = new T();

        return s_instance;
    }

private:

    static inline T* s_instance = nullptr;

};

}