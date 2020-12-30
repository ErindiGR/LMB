#pragma once

#include "lmb/calculators/job_base_calculator.h"
#include "lmb/calculators/lightmap_chunks_job.h"
#include "lmb/thread_job.h"



namespace LMB
{

class PreInfoCalculator;

class PreInfoJob final : public LightmapChunkJob
{
public:

    PreInfoJob( 
        const bitmap_size_t x_start,
        const bitmap_size_t y_start,
        const bitmap_size_t x_end,
        const bitmap_size_t y_end,
        const size_t lightmap,
        PreInfoCalculator* calc);

    void CalculatePixel(const bitmap_size_t x,const bitmap_size_t y);

};


class PIGridCell
{
public:
    AABB2D bbox;
    std::vector<size_t> triangle_indexes;
};

class PreInfoCalculator : public JobBaseCalculator
{

public:

    void StartCalc(const size_t lightmap);

    void GenerateGrid();

    const bool GetCell(const vec2 &point,size_t &out_cell_index);

protected:

    friend PreInfoJob;

    std::vector<PIGridCell> m_grid;

};


}