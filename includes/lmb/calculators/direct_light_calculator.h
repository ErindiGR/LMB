#pragma once

#include "lmb/calculators/job_base_calculator.h"
#include "lmb/thread_job.h"
#include "lmb/base_type.h"



namespace LMB
{





class Light
{

public:

    enum Type
    {
        Directional,
        Point
    };

    Light(const Type &type,const vec3 &color = vec3(1))
    {
        m_type=type;
        m_position= vec3(0);
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

    const Type GetType()
    {
        return m_type;
    }

    const real_t GetSoftness()
    {
        return m_softness;
    }

    void SetDir(const vec3 &dir)
    {
        m_direction=glm::normalize(dir);
    }

    void SetPos(const vec3 &pos)
    {
        m_position=pos;
    }

    void SetColor(const vec3 &color)
    {
        m_color=color;
    }

    void SetSoftness(const real_t softness)
    {
        m_softness=softness;
    }


protected:

    Type      m_type;
    real_t    m_softness;
    vec3 m_position;
    vec3 m_direction;
    vec3 m_color;

};

class DLJob : public Job
{

public:

    DLJob(
        const bitmap_size_t x_start,
        const bitmap_size_t y_start,
        const bitmap_size_t x_end,
        const bitmap_size_t y_end,
        class DirectLightCalculator* calc
    );

    void Execute();

protected:

    bitmap_size_t m_x_start;
    bitmap_size_t m_y_start;
    bitmap_size_t m_x_end;
    bitmap_size_t m_y_end;
    class DirectLightCalculator* m_calc;
    std::shared_ptr<Lightmap> m_lightmap;
};


struct dl_config_s
{
    uint16_t    num_rays;
    real_t      max_ray_distance;
    real_t      bias;
    vec3   ambient_color;
};

dl_config_s default_dl_config
{
.num_rays = 64,
.max_ray_distance = to_real(1000000.0),
.bias = to_real(1.0)/to_real(1024.0),
.ambient_color = vec3(to_real(0.2),to_real(0.2),to_real(0.3))
};

class DirectLightCalculator : public JobBaseCalculator
{


public:

    DirectLightCalculator(const dl_config_s &config)
    {
        m_num_rays = config.num_rays;
        m_max_ray_distance = config.max_ray_distance;
        m_bias = config.bias;
        m_ambient_color = config.ambient_color;
    }

    void StartCalc();

    vec4 CalcPixel(
        const bitmap_size_t x,
        const bitmap_size_t y,
        const vec3 &world_pos,
        const vec3 &world_norm
    );

    const vec3 CalcDirectionalLight(
        const vec3 &world_pos,
        const vec3 &world_norm,
        const size_t light_index
    );
    const vec3 CalcPointLight(
        const vec3 &world_pos,
        const vec3 &world_norm,
        const size_t light_index
    );

    std::vector<Ray> GenRays(
        const Ray &ray,
        const real_t softness,
        const vec3 &pos,
        const vec3 &norm
    );

    void AddLight(const Light &l)
    {
        m_lights.push_back(l);
    }


protected:

    friend DLJob;

    uint16_t m_num_rays;
    real_t m_max_ray_distance;
    real_t m_bias;
    vec3 m_ambient_color;
    
    std::vector<Light> m_lights;

};



}