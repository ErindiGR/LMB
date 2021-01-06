#pragma once

#include "lmb/calculators/lightmap_chunks_job.h"
#include "lmb/thread_job.h"
#include "lmb/base_type.h"
#include "lmb/solver.h"
#include "lmb/ray.h"

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

    explicit Light(const EType &type)
    :m_type(type)
    ,m_position(0)
    ,m_color(1)
    ,m_direction(1)
    ,m_softness(0.2)
    {
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

struct SDLCalcConfig
{
    bool        use_emissive;
    uint16_t    mun_emissive_rays;
    uint16_t    num_rays;
    real_t      max_ray_distance;
    real_t      bias;
    vec3        ambient_color;
};

inline SDLCalcConfig default_dl_config
{
.use_emissive = true,
.mun_emissive_rays = 256,
.num_rays = 128,
.max_ray_distance = to_real(10000.0),
.bias = to_real(1.0)/to_real(1024.0),
.ambient_color = vec3(0)//vec3(to_real(0.1),to_real(0.1),to_real(0.15))
};

/**
 * @brief Calculates direct lighting from a light source
 * like a point light or a directional light
 */
class DirectLightCalculator : public LightmapChunkJobBaseCalculator
{


public:

    explicit DirectLightCalculator(const SDLCalcConfig &config)
    : m_config(config)
    {
    }
    

    vec4 CalcPixel(
        const vec3 &world_pos,
        const vec3 &world_norm) override;

    const vec3 CalcDirectionalLight(
        const vec3 &world_pos,
        const vec3 &world_norm,
        const size_t light_index);

    const vec3 CalcPointLight(
        const vec3 &world_pos,
        const vec3 &world_norm,
        const size_t light_index);

    const vec3 CalcEmission(
        const vec3 &world_pos,
        const vec3 &world_norm);

    const vec3 CalcTransparentEmission(const Ray &ray,const Solver::SHitInfo &hit);
    const vec3 CalcTransparentLight(const Ray &ray,const Solver::SHitInfo &hit,const vec3 &light_color);
    const vec3 GetTransparentContribution(const vec4 color,const vec3 light_color);

    std::vector<Ray> GenRays(
        const real_t softness,
        const real_t length,
        const vec3 &pos,
        const vec3 &norm,
        const Light::EType light_type);

    const std::vector<Ray> GenRays(
        const vec3 &pos,
        const vec3 &norm) const;

    void AddLight(const Light &l)
    {
        m_lights.push_back(l);
    }


protected:
    
    std::vector<Light>  m_lights;
    SDLCalcConfig m_config;

};



}