
#pragma once


#include "lmb/calculators/temp_lightamp_calculator.h"
#include "lmb/calculators/lightmap_chunks_job.h"
#include "lmb/lightmap.h"
#include "lmb/base_type.h"

#include <glm/vec4.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>



namespace LMB
{


class ILJob final : public LightmapChunkJob
{

public:

    ILJob( 
        const size_t x_start,
        const size_t y_start,
        const size_t x_end,
        const size_t y_end,
        class IndirectLightCalculator* aocalc);

    //LightmapChunkJob
    void CalculatePixel(const bitmap_size_t x,const bitmap_size_t y);
    //!LightmapChunkJob

protected:
    
    class IndirectLightCalculator* m_aocalc;
};


/**
 * @brief the configuration setting for AOCalculator
 */
struct SGICalcConfig
{
    vec3 ambient_color;

    real_t   max_angle;
    
    /** the max distance the pixel can be occluded from*/
    real_t   ray_distance;
    
    /** 
     * the distance the ray origin will get moved 
     * towards the normal direction
     */
    real_t   bias;
    
    /** number of rays per pixel*/
    uint16_t num_rays;

    uint16_t num_bounces;
};

inline const SGICalcConfig default_il_config =
{
.ambient_color = vec3(0),
.max_angle = to_real(90.0),
.ray_distance = to_real(10000.0),
.bias = to_real(1.0)/to_real(1024.0),
.num_rays = 128,
.num_bounces = 8
};

/**
* @brief Calculates Inditect illumination
*/
class IndirectLightCalculator : public TempLigthmapCalculator
{

public:

    IndirectLightCalculator(const SGICalcConfig &config)
    : TempLigthmapCalculator(true)
    , m_config(config)
    {
    }

    void StartCalc();

    vec4 CalcPixel(
        const bitmap_size_t x,
        const bitmap_size_t y,
        const vec3 &world_pos,
        const vec3 &world_norm);

    const std::vector<Ray> GenRays(
        const vec3 &pos,
        const vec3 &norm) const;

protected:

    friend ILJob;

    SGICalcConfig m_config;
};



}