#pragma once

#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <typeinfo>
#include <mutex>

#include "lmb/base_type.h"

#include "lmb/triangle.h"
#include "lmb/ray.h"
#include "lmb/solvers/grid_solver.h"
#include "lmb/aabbox.h"

#define Dump_Push(type,val) Dumper<type,"dump_" ##type ".bin">::Push(val)

#define DUMPER_CHUNK_SIZE  1024

namespace LMB
{


template<typename T,const char* Filename>
class Dumper
{

public:

    void Push()
    {
        std::scoped_lock<std::mutex> lock(s_push_mutex);

        if(!s_file)
        {
            s_file = fopen(Filename,"wb");
        }

        if(s_data.size() - s_writen > DUMPER_CHUNK_SIZE)
        {
            fwrite(s_data.data() + s_writen,sizeof(T),DUMPER_CHUNK_SIZE,s_file);
            s_writen += DUMPER_CHUNK_SIZE;
        }
    }



protected:

    static std::vector<uint8_t> s_data;
    static size_t s_writen;
    static std::mutex s_push_mutex;
    inline static FILE* s_file = nullptr;

};


}