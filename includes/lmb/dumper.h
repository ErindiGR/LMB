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

#ifdef DUMP
#define Dump_Push(type,val) Dumper<type>::Push("dump_" #type ".bin",val)
#define Dump_Term(type) Dumper<type>::Term();
#else
#define Dump_Push(type,val) /*nothing*/
#define Dump_Term(type) /*nothing*/
#endif


#define DUMPER_CHUNK_SIZE  1024

namespace LMB
{


template<typename T>
class Dumper
{

public:

    static void Push(const char *filename,const T &val)
    {
        std::scoped_lock<std::mutex> lock(s_push_mutex);

        s_data.push_back(val);

        if(!s_file)
        {
            s_file = fopen(filename,"wb");
        }

        if(s_data.size() - s_writen > DUMPER_CHUNK_SIZE)
        {
            fwrite(s_data.data() + s_writen,sizeof(T),DUMPER_CHUNK_SIZE,s_file);
            s_writen += DUMPER_CHUNK_SIZE;
        }
    }

    static void Term()
    {
        if(s_data.size() - s_writen > 0)
        {
            fwrite(s_data.data() + s_writen,sizeof(T),s_data.size() - s_writen,s_file);
            s_writen += s_data.size() - s_writen;
        }
        

        fclose(s_file);
    }



protected:

    inline static std::vector<T> s_data;
    inline static size_t s_writen;
    inline static std::mutex s_push_mutex;
    inline static FILE* s_file = nullptr;

};


}