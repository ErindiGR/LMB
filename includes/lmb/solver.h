
#pragma once

#include "ray.h"
#include "triangle.h"

#include "lmb_obj.h"


namespace LMB
{

class Solver : public LMBObject
{

public:

   struct SHitInfo
   {
      vec3     uvw;
      real_t   t;
      size_t   triangle_index;
   };

public:

	/**
	* @brief generates the structure needed for the solver
	*/
   virtual void Gen() = 0;

   /**
   * @brief intersects the ray with the world
   */
   virtual const bool Intersect(const Ray &ray,SHitInfo &out_hit_info) const = 0;
};

}