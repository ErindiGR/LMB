
#pragma once

#include "lmb/calculators/job_base_calculator.h"
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
        const size_t lightmap,
        class IndirectLightCalculator* calc,
        const uint16_t bounce);

    //LightmapChunkJob
    void CalculatePixel(const bitmap_size_t x,const bitmap_size_t y);
    //!LightmapChunkJob

protected:

    uint16_t m_bounce;
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

    /** number of rays per pixel*/
    uint16_t num_final_rays;

    uint16_t num_bounces;
};

inline const SGICalcConfig default_il_config =
{
.ambient_color =  vec3(0),//vec3(to_real(0.1),to_real(0.1),to_real(0.15)),
.max_angle = to_real(89.0),
.ray_distance = to_real(1000.0),
.bias = to_real(1.0)/to_real(1024.0),
.num_rays = 64,
.num_final_rays = 246,
.num_bounces = 4
};

/**
* @brief Calculates Indirect illumination
*/
class IndirectLightCalculator : public JobBaseCalculator
{

public:

    IndirectLightCalculator(const SGICalcConfig &config)
    : m_config(config)
    {
    }

    void StartCalc(const size_t lightmap);

    vec4 CalcPixel(
        const vec3 &world_pos,
        const vec3 &world_norm,
        const uint16_t bounce);

    const std::vector<Ray> GenRays(
        const vec3 &pos,
        const vec3 &norm,
        const uint16_t bounce) const;

    const vec3 CalcIndirectLighting(
        const vec3 norm,
        const Ray &ray,
        const Solver::SHitInfo &hit);

    const vec3 CalcReceivedLight(
        const vec3 norm,
        const Ray &ray,
        const Solver::SHitInfo &hit);

protected:

    friend ILJob;

    SGICalcConfig m_config;
};



}