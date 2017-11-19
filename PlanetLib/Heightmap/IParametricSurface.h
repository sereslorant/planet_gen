#ifndef IPARAMETRIC_SURFACE_H
#define IPARAMETRIC_SURFACE_H

#include <glmath.hpp>

class IParametricSurface
{
public:
	
	virtual vec4 GetNormal(const vec4 &vec) const = 0;
	virtual vec4 GetValue(const vec4 &vec) const = 0;
	
	IParametricSurface()
	{}
	
	virtual ~IParametricSurface()
	{}
};

#include "IHeightmap.h"

class HeightmapSurface : public IParametricSurface
{
private:
	const IParametricSurface &base_surface;
	const IParametricSurface &up_surface;
	
	const IHeightmap &heightmap;
	
public:
	
	virtual vec4 GetNormal(const vec4 &vec) const override
	{
		/*
		 * TODO: Meg kéne csinálni!
		 */
		return up_surface.GetNormal(vec);
	}
	
	virtual vec4 GetValue(const vec4 &vec) const override
	{
		return base_surface.GetValue(vec) + heightmap.GetValue(vec)*normalize(up_surface.GetValue(vec));
	}
	
	HeightmapSurface(const IParametricSurface &p_base_surface,const IParametricSurface &p_up_surface,const IHeightmap &p_heightmap)
		:base_surface(p_base_surface),up_surface(p_up_surface),heightmap(p_heightmap)
	{}
	
	virtual ~HeightmapSurface() override
	{}
};

class TranslatedSurface : public IParametricSurface
{
private:
	const IParametricSurface &surface;
	vec4 displacement;
	
public:
	
	virtual vec4 GetNormal(const vec4 &vec) const override
	{
		return surface.GetNormal(vec);
	}
	
	virtual vec4 GetValue(const vec4 &vec) const override
	{
		return displacement + surface.GetValue(vec);
	}
	
	TranslatedSurface(const IParametricSurface &p_heightmap,const vec4 &p_displacement)
		:surface(p_heightmap),displacement(p_displacement)
	{}
	
	virtual ~TranslatedSurface() override
	{}
};

class ScaledSurface : public IParametricSurface
{
private:
	const IParametricSurface &surface;
	float scale;
	
public:
	
	virtual vec4 GetNormal(const vec4 &vec) const override
	{
		return surface.GetNormal(vec);
	}
	
	virtual vec4 GetValue(const vec4 &vec) const override
	{
		return surface.GetValue(vec) * scale;
	}
	
	ScaledSurface(const IParametricSurface &p_heightmap,float p_scale)
		:surface(p_heightmap),scale(p_scale)
	{}
	
	virtual ~ScaledSurface() override
	{}
};

#endif // IPARAMETRIC_SURFACE_H
