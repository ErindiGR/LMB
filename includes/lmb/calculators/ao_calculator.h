
#pragma once


#include "lmb/calculators/lightmap_chunks_job.h"
#include "lmb/lightmap.h"
#include "lmb/thread_job.h"
#include "lmb/base_type.h"

#include <glm/vec4.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>



namespace LMB
{

/**
 * @brief the configuration setting for AOCalculator
 */
struct SAOCalcConfig
{
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
};

inline const SAOCalcConfig default_ao_config =
{
.max_angle = to_real(89.9),
.ray_distance = to_real(1.0),
.bias = to_real(1.0)/to_real(1024.0),
.num_rays = 128
};

/**
* @brief Calculates Ambient occlusion
*/
class AOCalculator : public LightmapChunkJobBaseCalculator
{

public:

    explicit AOCalculator(const SAOCalcConfig &config)
    : m_config(config)
    {
    }

    vec4 CalcPixel(
        const vec3 &world_pos,
        const vec3 &world_norm) override;

    const std::vector<Ray> GenRays(
        const vec3 &pos,
        const vec3 &norm) const;

    const real_t CalcOcclusion(
        const Ray &ray,
        const Solver::SHitInfo &hit);

protected:

    SAOCalcConfig m_config;
};



}