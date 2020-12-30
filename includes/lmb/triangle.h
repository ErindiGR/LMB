#pragma once

#include "aabbox.h"
#include "base_type.h"
#include "geometry.h"
#include "handles.h"

#include <array>
#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>

namespace LMB
{

class Lightmap;



class Triangle
{

public:

	Triangle()
	{
		m_has_normals = false;
		m_has_uv2 = false;
	}

	inline void SetPos(const std::array<vec3,3>& positions)
	{
		m_positions = positions;
		GenAABB();
	}

	inline const std::array<vec3,3>& GetNormal() const
	{
		return m_positions;
	}

	inline void SetNormal(const std::array<vec3,3>& normals)
	{
		m_normals = normals;
		m_has_normals = true;
		GenAABB();
	}

	inline const std::array<vec3,3>& GetPos() const
	{
		return m_positions;
	}

	inline const size_t GetInfo() const
	{
		return m_info;
	}

	inline const std::array<vec2,3>& GetUV() const
	{
		return m_uv;
	}

	inline const std::array<vec2,3>& GetUV2() const
	{
		return m_uv2;
	}

	inline const std::array<vec2,3>& GetLightmapUV() const
	{
		if(m_has_uv2)
			return m_uv2;
		else
			return m_uv;
	}

	inline const bool HasUV2() const
	{
		return m_has_uv2;
	}

	inline void SetInfo(const size_t info)
	{
		m_info = info;
	}

	inline void SetUV(const std::array<vec2,3>& uv)
	{
		m_uv = uv;
		GenAABBUV1();
	}

	inline void SetUV2(const std::array<vec2,3>& uv2)
	{
		m_uv2 = uv2;
		m_has_uv2 = true;
		GenAABBUV2();
	}

	const bool PointInsideUV1(const vec2& p,vec3& out_uvw) const
	{
		if(!IntersectAABBPoint(m_uv_bb,p))
			return false;

		vec3 uvw(0);

		Barycentric(p,m_uv,uvw);

		if(	uvw.x >= to_real(1.0) || uvw.x <= to_real(0.0) ||
			uvw.y >= to_real(1.0) || uvw.y <= to_real(0.0) ||
			uvw.z >= to_real(1.0) || uvw.z <= to_real(0.0))
			return false;

		out_uvw = uvw;

		return true;
	}

	const bool PointInsideUV2(const vec2& p,vec3& out_uvw) const
	{
		if(!IntersectAABBPoint(m_uv2_bb,p))
			return false;

		vec3 uvw(0);

		Barycentric(p,m_uv2,uvw);

		if(	uvw.x >= to_real(1.0) || uvw.x <= to_real(0.0) ||
			uvw.y >= to_real(1.0) || uvw.y <= to_real(0.0) ||
			uvw.z >= to_real(1.0) || uvw.z <= to_real(0.0))
			return false;

		out_uvw = uvw;

		return true;
	}

	const bool PointInsideLightmapUV(const vec2& p,vec3& out_uvw) const
	{
		if(m_has_uv2)
		{
			return PointInsideUV2(p,out_uvw);
		}
		else
		{
			return PointInsideUV1(p,out_uvw);
		}
	}

	const vec3 BarycentricToPos(const vec3& uvw) const
	{
		return 	m_positions[0] * uvw[0] + m_positions[1] * uvw[1] + m_positions[2] * uvw[2];
	}

	const vec3 BarycentricToNormal(const vec3& uvw) const
	{
		if(m_has_normals)
		{
			return m_normals[0] * uvw[0] + m_normals[1] * uvw[1] + m_normals[2] * uvw[2];
		}
		else
		{
			return glm::triangleNormal(m_positions[0],m_positions[1],m_positions[2]);
		}
	}

	const vec2 BarycentricToUV1(const vec3& uvw) const
	{
		return 	m_uv[0] * uvw[0] + m_uv[1] * uvw[1] + m_uv[2] * uvw[2];
	}

	const vec2 BarycentricToUV2(const vec3& uvw) const
	{
		return 	m_uv2[0] * uvw[0] + m_uv2[1] * uvw[1] + m_uv2[2] * uvw[2];
	}

	const vec2 BarycentricToLightmapUV(const vec3& uvw) const
	{
		if(m_has_uv2)
		{
			return BarycentricToUV2(uvw);
		}
		else
		{
			return BarycentricToUV1(uvw);
		}
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

		m_bbox.SetMax(max);
		m_bbox.SetMin(min);

	}

	inline void GenAABBUV1()
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
	}

	inline void GenAABBUV2()
	{
		vec2 min = vec2(1000000);
		vec2 max = vec2(-1000000);

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

	inline const AABB2D& GetLightmapAABBUV() const
	{
		if(m_has_uv2)
			return m_uv2_bb;
		
		return m_uv_bb;
	}


protected:

	size_t m_info;

	AABB3D m_bbox;
	AABB2D m_uv_bb;
	AABB2D m_uv2_bb;

    std::array<vec3,3> m_positions;
    std::array<vec3,3> m_normals;
    std::array<vec2,3> m_uv;
    std::array<vec2,3> m_uv2;

	bool m_has_uv2;
	bool m_has_normals;

};


class TriangleInfo
{

public:

	TriangleInfo()
	{
	}

	void SetLightmap(const LightmapHandle &lightmap)
	{
		m_lightmap = lightmap;
	}

	void SetEmissive(const BitmapHandle &emissive)
	{
		m_emissive = emissive;
	}

	void SetAlbedo(const BitmapHandle &albedo)
	{
		m_albedo = albedo;
	}

	const LightmapHandle GetLightmap()const
	{
		return m_lightmap;
	}

	const BitmapHandle GetEmissive()const
	{
		return m_emissive;
	}

	const BitmapHandle GetAlbedo()const
	{
		return m_albedo;
	}

	const bool HasLightmap()const
	{
		return m_lightmap.IsValid();
	}

	const bool HasEmissive()const
	{
		return m_emissive.IsValid();
	}

	const bool HasAlbedo()const
	{
		return m_albedo.IsValid();
	}

protected:

	LightmapHandle m_lightmap;
	BitmapHandle m_emissive;
	BitmapHandle m_albedo;
};

}