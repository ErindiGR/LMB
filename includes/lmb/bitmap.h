
#pragma once

#include "lmb/base_type.h"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>


namespace LMB
{

struct RGBA8
{

public:

	vec4 ToVec() const
	{
		return vec4(m_r/(real_t)0xff,m_g/(real_t)0xff,m_b/(real_t)0xff,m_a/(real_t)0xff);
	}

	static RGBA8 FromVec(const vec4& v)
	{
		RGBA8 ret;
		ret.m_r = glm::clamp(v.x,to_real(0.0),to_real(1.0)) * 0xff;
		ret.m_g = glm::clamp(v.y,to_real(0.0),to_real(1.0)) * 0xff;
		ret.m_b = glm::clamp(v.z,to_real(0.0),to_real(1.0)) * 0xff;
		ret.m_a = glm::clamp(v.w,to_real(0.0),to_real(1.0)) * 0xff;

		return ret;
	}

public:

	uint8_t m_r,m_g,m_b,m_a;

};

struct RGBA16
{

public:

	vec4 ToVec() const
	{
		return vec4(m_r/(real_t)0xffff,m_g/(real_t)0xffff,m_b/(real_t)0xffff,m_a/(real_t)0xffff);
	}

	static RGBA16 FromVec(const vec4& v)
	{
		const real_t max = 0xffff/0xff;
		RGBA16 ret;
		ret.m_r = glm::clamp(v.x,to_real(0.0),max) * 0xffff;
		ret.m_g = glm::clamp(v.y,to_real(0.0),max) * 0xffff;
		ret.m_b = glm::clamp(v.z,to_real(0.0),max) * 0xffff;
		ret.m_a = glm::clamp(v.w,to_real(0.0),max) * 0xffff;

		return ret;
	}

public:

	uint16_t m_r,m_g,m_b,m_a;

};


template<typename T>
inline const T Lerp(const T &lh,const T &rh,const real_t t);

inline const RGBA8 Lerp(const RGBA8 &lh,const RGBA8 &rh,const real_t t)
{
	uint8_t r = lh.m_r + (rh.m_r-lh.m_r)*t;
	uint8_t g = lh.m_g + (rh.m_g-lh.m_g)*t;
	uint8_t b = lh.m_b + (rh.m_b-lh.m_b)*t;
	uint8_t a = lh.m_a + (rh.m_a-lh.m_a)*t;

	return RGBA8{r,g,b,a};
}

inline const vec3 Lerp(const vec3 &lh,const vec3 &rh,const real_t t)
{
	return glm::lerp(rh,lh,t);
}

inline const vec4 Lerp(const vec4 &lh,const vec4 &rh,const real_t t)
{
	return glm::lerp(rh,lh,t);
}

typedef int bitmap_size_t;

template<typename T>
class Bitmap
{

public:

	struct Flags
	{
		bool interpolate :1;
	};

public:

	Bitmap(const bitmap_size_t width,const bitmap_size_t height)
	{
		m_width = width;
		m_height = height;

		m_pixels.resize(m_width * m_height);

		m_flags.interpolate = false;
	}

	Bitmap(const bitmap_size_t width,const bitmap_size_t height,const T& val)
	{
		m_width = width;
		m_height = height;

		m_pixels.resize(m_width * m_height,val);

		m_flags.interpolate = false;
	}


	const T& GetPixel(const bitmap_size_t x,const bitmap_size_t y) const
	{
		const size_t rx = NegModule(x,m_width);
		const size_t ry = NegModule(y,m_height);

		return m_pixels[ry * m_width + rx];
	}

	const T GetPixel(const real_t x,const real_t y) const
	{
		const real_t xf = x*m_width;
		const real_t yf = y*m_height;

		const bitmap_size_t x_min = std::floorf(xf);
		const bitmap_size_t y_min = std::floorf(yf);

		if(!m_flags.interpolate)
			return GetPixel(x_min,y_min);

		const bitmap_size_t x_max = std::ceilf(xf);
		const bitmap_size_t y_max = std::ceilf(yf);

		const real_t tx = (xf)-x_min;
		const real_t ty = (yf)-y_min;

		const T tl = GetPixel(x_max,y_min);
		const T tr = GetPixel(x_max,y_max);
		const T bl = GetPixel(x_min,y_min);
		const T br = GetPixel(x_min,y_max);

		const T t = Lerp(tl,tr,tx);
		const T b = Lerp(bl,br,tx);

		return Lerp(t,b,ty);
	}

	void SetPixel(const bitmap_size_t x,const bitmap_size_t y,const T& val)
	{
		const size_t rx = NegModule(x,m_width);
		const size_t ry = NegModule(y,m_height);
		m_pixels[ry * m_width + rx]  = val;
	}

	void Copy(const Bitmap<T> &bitmap)
	{
		m_pixels = bitmap.m_pixels;
		m_width = bitmap.m_width;
		m_height = bitmap.m_height;
		m_flags = bitmap.m_flags;
	}

	void Move(Bitmap<T> &bitmap)
	{
		m_pixels = std::move(bitmap.m_pixels);
		m_width = bitmap.m_width;
		m_height = bitmap.m_height;
		m_flags = bitmap.m_flags;

		bitmap.m_width = 0;
		bitmap.m_height = 0;
		bitmap.m_flags.interpolate = false;
	}

	T* GetData()
	{
		return m_pixels.data();
	}


	const bitmap_size_t GetWidth() const
	{
		return m_width;
	}

	const bitmap_size_t GetHeight() const
	{
		return m_height;
	}

	void SetInterpolate(bool interpolate)
	{
		m_flags.interpolate = interpolate;
	}

protected:

	std::vector<T> 	m_pixels;  
	bitmap_size_t 	m_width;
	bitmap_size_t 	m_height;
	Flags 			m_flags;
	
};


class BitmapUtils
{

public:

	static Bitmap<RGBA8> ToRGBA8(const Bitmap<vec4> &bitmap)
	{
		Bitmap<RGBA8> ret(bitmap.GetWidth(),bitmap.GetHeight());

		for(bitmap_size_t x=0;x<ret.GetWidth();x++)
		{
			for(bitmap_size_t y=0;y<ret.GetHeight();y++)
			{
				ret.SetPixel(x,y,RGBA8::FromVec(bitmap.GetPixel(x,y)));
			}	
		}

		return ret;
	}

	static Bitmap<RGBA16> ToRGBA16(const Bitmap<vec4> &bitmap)
	{
		Bitmap<RGBA16> ret(bitmap.GetWidth(),bitmap.GetHeight());

		for(bitmap_size_t x=0;x<ret.GetWidth();x++)
		{
			for(bitmap_size_t y=0;y<ret.GetHeight();y++)
			{
				ret.SetPixel(x,y,RGBA16::FromVec(bitmap.GetPixel(x,y)));
			}	
		}

		return ret;
	}

	static Bitmap<glm::vec4> ToRGBAFloat(const Bitmap<vec4> &bitmap)
	{
		Bitmap<glm::vec4> ret(bitmap.GetWidth(),bitmap.GetHeight());

		for(bitmap_size_t x=0;x<ret.GetWidth();x++)
		{
			for(bitmap_size_t y=0;y<ret.GetHeight();y++)
			{
				ret.SetPixel(x,y,bitmap.GetPixel(x,y));
			}	
		}

		return ret;
	}

	static void Multiply(Bitmap<vec4> &out_bitmap,real_t val)
	{
		for(bitmap_size_t x=0;x<out_bitmap.GetWidth();x++)
		{
			for(bitmap_size_t y=0;y<out_bitmap.GetHeight();y++)
			{
				out_bitmap.SetPixel(x,y,out_bitmap.GetPixel(x,y) * val);
			}	
		}
	}

	static void FlipVertically(Bitmap<vec4> &out_bitmap)
	{
		Bitmap<glm::vec4> ret(out_bitmap.GetWidth(),out_bitmap.GetHeight());

		for(bitmap_size_t x=0;x<ret.GetWidth();x++)
		{
			for(bitmap_size_t y=0;y<ret.GetHeight();y++)
			{
				ret.SetPixel(x,ret.GetHeight()-y,out_bitmap.GetPixel(x,y));
			}	
		}

		for(bitmap_size_t x=0;x<ret.GetWidth();x++)
		{
			for(bitmap_size_t y=0;y<ret.GetHeight();y++)
			{
				out_bitmap.SetPixel(x,y,ret.GetPixel(x,y));
			}	
		}
	}

	static Bitmap<vec4> ToVec4(const Bitmap<RGBA8> &bitmap)
	{
		Bitmap<vec4> ret(bitmap.GetWidth(),bitmap.GetHeight());

		for(bitmap_size_t x=0;x<ret.GetWidth();x++)
		{
			for(bitmap_size_t y=0;y<ret.GetHeight();y++)
			{
				ret.SetPixel(x,y,bitmap.GetPixel(x,y).ToVec());
			}	
		}

		return ret;
	}

};

}