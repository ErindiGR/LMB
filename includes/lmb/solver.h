
#pragma once

#include "ray.h"
#include "triangle.h"

#include "lmb_obj.h"


namespace LMB
{

class Solver : public LMBObject
{

public:

	/**
	* @brief generates the structure needed for the solver
	*/
   virtual void Gen() = 0;

   /**
   * @brief intersects the ray with the world
   */
   virtual const bool Intersect(const Ray &ray,vec3 &out_uvw,real_t &out_t) const = 0;
};

}