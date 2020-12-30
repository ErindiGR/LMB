#pragma once

#include "ray.h"
#include <glm/gtx/rotate_vector.hpp>
#include <vector>

namespace LMB
{

class RaySpread
{

public:

    static const std::vector<Ray> GenHemisphere(
        const vec3 &pos,
        const vec3 &norm,
        const real_t angle,
        const real_t length,
        const real_t bias,
        const uint16_t num_rays)
    {

        std::vector<Ray> rays(num_rays);
        
        for(size_t i=0;i<rays.size();i++)
        {
            rays[i].SetStart(pos + norm * bias);
            rays[i].SetEnd(pos + norm);
        }



        for(size_t i=0;i<rays.size();i++)
        {
            Ray &ray = rays[i];


            vec3 dir = ray.GetDir();

            const real_t randx = ((rand() % 1024) - to_real(512.5)) / 512.5;
            const real_t randy = ((rand() % 1024) - to_real(512.5)) / 512.5;
            const real_t randz = ((rand() % 1024) - to_real(512.5)) / 512.5;

            const real_t angley = glm::radians(angle * randx );
            const real_t anglex = glm::radians(angle * randy );
            const real_t anglez = glm::radians(angle * randz );

            dir = glm::rotate(dir,angley,vec3(0,1,0));
            dir = glm::rotate(dir,anglex,vec3(1,0,0));
            dir = glm::rotate(dir,anglez,vec3(0,0,1));

            dir = glm::normalize(dir);

            ray.SetEnd(ray.GetStart() + dir * length);
        }

        return std::move(rays);
    }

};

}