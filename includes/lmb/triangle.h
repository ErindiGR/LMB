#pragma once

#include "aabbox.h"
#include "base_type.h"
#include "geometry.h"

#include <array>
#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>

namespace LMB
{

class Lightmap;



class Triangle
{

public:

	inline void SetPos(const std::array<vec3,3>& positions)
	{
		m_positions = positions;
		GenAABB();
	}

	inline const std::array<vec3,3>& GetPos() const
	{
		return m_positions;
	}

	inline const size_t GetLightmap() const
	{
		return m_lightmap;
	}

	inline const std::array<vec2,3>& GetUV() const
	{
		return m_uv;
	}

	inline const std::array<vec2,3>& GetUV2() const
	{
		return m_uv2;
	}

	inline const bool HasUV2() const
	{
		return m_has_uv2;
	}

	inline void SetLightmap(const size_t lightmap)
	{
		m_lightmap = lightmap;
	}

	inline void SetUV(const std::array<vec2,3>& uv)
	{
		m_uv = uv;
		GenAABBUV();
	}

	inline void SetUV2(const std::array<vec2,3>& uv2)
	{
		m_uv2 = uv2;
		m_has_uv2 = true;
		GenAABBUV();
	}

	const bool PointInsideUV1(const vec2& p,vec3& out_uvw) const
	{
		if(!IntersectAABBPoint(m_uv_bb,p))
			return false;

		real_t u=0,v=0,w=0;
		Barycentric(p,m_uv[0],m_uv[1],m_uv[2],u,v,w);

		if(	u >= to_real(1.0) || u <= to_real(0.0) ||
			v >= to_real(1.0) || v <= to_real(0.0) ||
			w >= to_real(1.0) || w <= to_real(0.0))
			return false;


		out_uvw = vec3(u,v,w);
		return true;
	}

	const bool PointInsideUV2(const vec2& p,vec3& out_uvw) const
	{
		if(!IntersectAABBPoint(m_uv2_bb,p))
			return false;

		real_t u=0,v=0,w=0;
		Barycentric(p,m_uv2[0],m_uv2[1],m_uv2[2],u,v,w);

		if(	u >= to_real(1.01) || u <= to_real(-0.01) ||
			v >= to_real(1.01) || v <= to_real(-0.01) ||
			w >= to_real(1.01) || w <= to_real(-0.01))
			return false;


		out_uvw = vec3(u,v,w);
		return true;
	}


	vec3 BarycentricToPos(const vec3& uvw) const
	{
		return 	m_positions[0] * uvw[0] +
				m_positions[1] * uvw[1] +
				m_positions[2] * uvw[2];
	}

	vec3 BarycentricToNormal(const vec3& uvw) const
	{

		//return m_normals[0]*u +
		//		 m_normals[1]*v +
		//		 m_normals[2]*w;

		return glm::triangleNormal(
			m_positions[0],
			m_positions[1],
			m_positions[2]
		);
	}


	inline void GenAABB()
	{
		vec3 min(1000000);
		vec3 max(-1000000);

		for(int i=0;i<m_positions.size();i++)
		{
			for(int j=0;j<3;j++)
			{
				if(min[j]>m_positions[i][j])
					min[j]=m_positions[i][j];
				
				if(max[j]<m_positions[i][j])
					max[j]=m_positions[i][j];

			}
		}

		for(int j=0;j<3;j++)
		{
			if(glm::abs(min[j]-max[j]) < to_real(1.0)/to_real(512.0))
			{
				max[j]+= to_real(1.0)/to_real(512.0);
			}
		}

		m_bbox.SetMax(max);
		m_bbox.SetMin(min);

	}

	inline void GenAABBUV()
	{
		vec2 min(1000000);
		vec2 max(-1000000);

		for(int i=0;i<m_uv.size();i++)
		{
			for(int j=0;j<2;j++)
			{
				if(min[j]>m_uv[i][j])
					min[j]=m_uv[i][j];
				
				if(max[j]<m_uv[i][j])
					max[j]=m_uv[i][j];

			}
		}

		m_uv_bb.SetMax(max);
		m_uv_bb.SetMin(min);


		min = vec2(1000000);
		max = vec2(-1000000);

		for(int i=0;i<m_uv2.size();i++)
		{
			for(int j=0;j<2;j++)
			{
				if(min[j]>m_uv2[i][j])
					min[j]=m_uv2[i][j];
				
				if(max[j]<m_uv2[i][j])
					max[j]=m_uv2[i][j];

			}
		}

		m_uv2_bb.SetMax(max);
		m_uv2_bb.SetMin(min);

	}

	inline const AABB3D& GetAABB() const
	{
		return m_bbox;
	}

	inline const AABB2D& GetAABBUV() const
	{
		return m_uv_bb;
	}

	inline const AABB2D& GetAABBUV2() const
	{
		return m_uv2_bb;
	}


protected:

	size_t m_lightmap;

	AABB3D m_bbox;
	AABB2D m_uv_bb;
	AABB2D m_uv2_bb;

    std::array<vec3,3> m_positions;
    std::array<vec3,3> m_normals;
    std::array<vec2,3> m_uv;
    std::array<vec2,3> m_uv2;

	bool m_has_uv2;

};

}