#pragma once


#include "bitmap.h"
#include "triangle.h"

#include <vector>
#include <glm/glm.hpp>


namespace LMB
{

class Lightmap
{

public:

	enum class EFlags : unsigned char
	{
		UnUsed,
		Used
	};

public:

	Lightmap(const size_t width, const size_t height,const size_t index)
	: m_colors(width,height,vec4(0,0,0,1))
	, m_flags(width,height,EFlags::UnUsed)
	, m_positions(width,height,vec3(0))
	, m_normals(width,height,vec3(0))
	, m_index(index)
	{
	}

	inline Bitmap<vec3>& GetPos()
	{
		return m_positions;
	}

	inline Bitmap<vec3>& GetNorm()
	{
		return m_normals;
	}

	inline Bitmap<vec4>& GetColor()
	{
		return m_colors;
	}

	inline Bitmap<EFlags>& GetFlags()
	{
		return m_flags;
	}

	inline size_t GetIndex()
	{
		return m_index;
	}

	void GenPosAndNorm(const std::vector<Triangle>& triangles);

	void GenPixelPositions(const size_t x, const size_t y,const std::vector<Triangle>& triangles);
	
	void GenPixelNormal(const size_t x, const size_t y,const std::vector<Triangle>& triangles);


protected:

	size_t 		 	m_index;
	Bitmap<vec4> 	m_colors;
	Bitmap<EFlags>  m_flags;
	Bitmap<vec3> 	m_positions;
	Bitmap<vec3> 	m_normals;

};

}