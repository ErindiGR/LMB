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
		UnUsed = 0,
		Used = 1
	};

public:

	Lightmap(const size_t size,const size_t index)
	: m_colors(4,4)
	, m_flags(4,4)
	, m_positions(4,4)
	, m_normals(4,4)
	, m_index(index)
	{

		const int min_size = 5;
		const int max_size = 12;

		m_size = pow(2,min_size);

		for(int i=min_size;i<max_size;i++)
		{
			if(size < pow(2,i))
			{
				m_size = pow(2,i);
				break;
			}
		}

		m_colors.Copy(Bitmap(m_size,m_size,vec4(0,0,0,1)));
		m_flags.Copy(Bitmap(m_size,m_size,EFlags::UnUsed));
		m_positions.Copy(Bitmap(m_size,m_size,vec3(0)));
		m_normals.Copy(Bitmap(m_size,m_size,vec3(0)));

		m_colors.SetInterpolate(true);
		m_positions.SetInterpolate(true);
		m_normals.SetInterpolate(true);
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

	inline const size_t GetIndex()
	{
		return m_index;
	}

	inline const bitmap_size_t GetSize()
	{
		return m_size;
	}


protected:

	bitmap_size_t 	m_size;
	size_t 		 	m_index;
	Bitmap<vec4> 	m_colors;
	Bitmap<vec3> 	m_positions;
	Bitmap<vec3> 	m_normals;
	Bitmap<EFlags>  m_flags;

};

}