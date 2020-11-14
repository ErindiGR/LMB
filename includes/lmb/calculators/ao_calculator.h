
#pragma once


#include "lmb/calculators/job_base_calculator.h"
#include "lmb/lightmap.h"
#include "lmb/thread_job.h"
#include "ao_job.h"
#include "lmb/base_type.h"

#include <glm/vec4.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>



namespace LMB
{




struct ao_calc_config_s
{
    real_t   max_angle;
    real_t   ray_distance;
    real_t   bias;
    uint16_t num_rays;
};

inline ao_calc_config_s default_ao_config =
{
.max_angle = to_real(80.0),
.ray_distance = to_real(3.0),
.bias = to_real(1.0)/to_real(1024.0),
.num_rays = 64
};

class AOCalculator : public JobBaseCalculator
{

public:
    /**
     * @brief Construct a new AOCalculator object
     * 
     * @param num_rays the number of rays emitted per pixel
     * 
     * @param bias the distance from the ray origin to the surface 
     * that emitted the ray
     */
    AOCalculator(const ao_calc_config_s &info)
    {
        m_num_rays = info.num_rays;
        m_ray_distance = info.ray_distance;
        m_max_angle = info.max_angle;
        m_bias = info.bias;
    }

    void StartCalc();


    vec4 CalcPixel(
        const int x,
        const int y,
        const vec3 &world_pos,
        const vec3 &world_norm
    );


    const std::vector<Ray> GenRays(
        const vec3 &pos,
        const vec3 &norm
    )const;


protected:

    friend AOJob;

    real_t      m_max_angle;
    real_t      m_bias;
    real_t      m_ray_distance;
    uint16_t    m_num_rays;
};



}