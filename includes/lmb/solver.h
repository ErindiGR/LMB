
#pragma once

#include "ray.h"
#include "triangle.h"

#include "lmb_obj.h"


namespace LMB
{

class Solver : public LMBObject
{

public:

   virtual void Gen()=0;

   virtual const bool Intersect(const Ray &ray,vec3 &out_uvw,real_t &out_t) const =0;
};

}