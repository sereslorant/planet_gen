#ifndef SPHERE_PATCH_H
#define SPHERE_PATCH_H

#include "IParametricSurface.h"

#include <array>

class SpherePatch : public IParametricSurface
{
private:
	std::array<vec4,4> corners;
	
public:
	
	const std::array<vec4,4> &GetCorners()
	{
		return corners;
	}
	
	virtual vec4 GetNormal(const vec4 &vec) const override
	{
		vec4 pointX1 = slerp(corners[0],corners[1],vec[0]);
		vec4 pointX2 = slerp(corners[3],corners[2],vec[0]);
		
		return slerp(pointX1,pointX2,vec[1]);
	}
	
	virtual vec4 GetValue(const vec4 &vec) const override
	{
		vec4 pointX1 = slerp(corners[0],corners[1],vec[0]);
		vec4 pointX2 = slerp(corners[3],corners[2],vec[0]);
		
		return slerp(pointX1,pointX2,vec[1]);
	}
	
	SpherePatch(const std::array<vec4,4> &p_corners)
		:corners(p_corners)
	{
		for(vec4 &corner : corners)
		{
			corner = normalize(corner);
		}
	}
	
	virtual ~SpherePatch()
	{}
};

#endif // SPHERE_PATCH_H
