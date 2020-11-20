#pragma once

#include "lmb/calculators/job_base_calculator.h"
#include "lmb/calculators/lightmap_chunks_job.h"
#include "lmb/thread_job.h"
#include "lmb/base_type.h"



namespace LMB
{





class Light
{

public:

    enum class EType
    {
        Directional,
        Point
    };

    Light(const EType &type,const vec3 &color = vec3(1))
    {
        m_type = type;
        m_position = vec3(0);
        m_color = color;
        m_direction = vec3(1);
        m_softness = to_real(0.2);
    }

    const vec3 &GetPos()
    {
        return m_position;
    }

    const vec3 &GetColor()
    {
        return m_color;
    }

    const vec3 &GetDir()
    {
        return m_direction;
    }

    const EType GetType()
    {
        return m_type;
    }

    const real_t GetSoftness()
    {
        return m_softness;
    }

    void SetDir(const vec3 &dir)
    {
        m_direction = glm::normalize(dir);
    }

    void SetPos(const vec3 &pos)
    {
        m_position = pos;
    }

    void SetColor(const vec3 &color)
    {
        m_color = color;
    }

    void SetSoftness(const real_t softness)
    {
        m_softness = softness;
    }


protected:

    vec3    m_position;
    vec3    m_direction;
    vec3    m_color;
    real_t  m_softness;
    EType   m_type;
};

class DLJob final : public LightmapChunkJob
{

public:

    DLJob(
        const bitmap_size_t x_start,
        const bitmap_size_t y_start,
        const bitmap_size_t x_end,
        const bitmap_size_t y_end,
        class DirectLightCalculator* calc);

    void CalculatePixel(const bitmap_size_t x,const bitmap_size_t y);

protected:

    class DirectLightCalculator* m_calc;
};


struct SDLCalcConfig
{
    uint16_t    num_rays;
    real_t      max_ray_distance;
    real_t      bias;
    vec3        ambient_color;
};

inline SDLCalcConfig default_dl_config
{
.num_rays = 128,
.max_ray_distance = to_real(1000000.0),
.bias = to_real(1.0)/to_real(1024.0),
.ambient_color = vec3(0)//vec3(to_real(0.1),to_real(0.1),to_real(0.15))
};

/**
 * @brief Calculates direct lighting from a light source
 * like a point light or a directional light
 */
class DirectLightCalculator : public JobBaseCalculator
{


public:

    DirectLightCalculator(const SDLCalcConfig &config)
    : m_config(config)
    {
    }
    
    //ICalculable
    void StartCalc();
    //!ICalculable

    vec4 CalcPixel(
        const bitmap_size_t x,
        const bitmap_size_t y,
        const vec3 &world_pos,
        const vec3 &world_norm);

    const vec3 CalcDirectionalLight(
        const vec3 &world_pos,
        const vec3 &world_norm,
        const size_t light_index);

    const vec3 CalcPointLight(
        const vec3 &world_pos,
        const vec3 &world_norm,
        const size_t light_index);

    std::vector<Ray> GenRays(
        const Ray &ray,
        const real_t softness,
        const vec3 &pos,
        const vec3 &norm,
        const Light::EType light_type);

    void AddLight(const Light &l)
    {
        m_lights.push_back(l);
    }


protected:

    friend DLJob;
    
    std::vector<Light>  m_lights;
    SDLCalcConfig m_config;

};



}