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


namespace LMB
{

class DumpInfo
{

public:

    std::vector<Triangle> m_triangles;
    std::vector<Ray> m_rays;
    std::vector<GridCell> m_grid;

};


class Dumper
{

protected:

    static void Save();

    template<typename T>
    static void __Push(const T &obj);


public:


    static void Init(const std::string &filename);

    static void Term();

    static std::shared_ptr<DumpInfo> Load(const std::string &filename);


    template<typename T>
    static void Push(const T &obj)
    {
#if 1
        std::lock_guard<std::mutex> lock(s_push_mutex);

        __Push(obj);

        Save();
#endif
    }
    
    template<typename T>
    static void Pop(T &obj);



protected:

    static std::vector<uint8_t> s_data;
    static size_t s_pop_index;
    static size_t s_writen;
    static std::mutex s_push_mutex;
    static FILE* s_file;

};


}