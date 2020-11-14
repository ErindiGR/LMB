#pragma once

#include "lmb/solver.h"

#include <vector>



namespace LMB
{




class DefaultSolver : public Solver
{

public:

    void Gen();

    const bool Intersect(const Ray &ray,vec3 &out_uvw,real_t &outt)const;

    const bool IntersectTriangle(
        const Ray &ray,
        const Triangle &tri,
        vec3 &out_uvw,
        real_t &outt
    )const;

protected:

};


}