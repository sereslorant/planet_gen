#ifndef CUBE_HEIGHTMAP_H
#define CUBE_HEIGHTMAP_H

#include <PlanetLib/Heightmap/Composites/StitchedHeightmap/StitchedHeightmap.h>

class CubeHeightmap
{
private:
	StitchedHeightmap top;
	StitchedHeightmap near;
	StitchedHeightmap bottom;
	StitchedHeightmap far;
	StitchedHeightmap left;
	StitchedHeightmap right;
	
public:
	
	const StitchedHeightmap &GetTop() const
	{
		return top;
	}
	
	const StitchedHeightmap &GetNear() const
	{
		return near;
	}
	
	const StitchedHeightmap &GetBottom() const
	{
		return bottom;
	}
	
	const StitchedHeightmap &GetFar() const
	{
		return far;
	}
	
	const StitchedHeightmap &GetLeft() const
	{
		return left;
	}
	
	const StitchedHeightmap &GetRight() const
	{
		return right;
	}
	
	CubeHeightmap(const IHeightmap &p_top,const IHeightmap &p_near,const IHeightmap &p_bottom,const IHeightmap &p_far,const IHeightmap &p_left,const IHeightmap &p_right)
		:top(p_top,{{&p_far,&p_near,&p_left,&p_right}},{{StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_90,StitchedHeightmap::DEG_270}}),
		 near(p_near,{{&p_top,&p_bottom,&p_left,&p_right}},{{StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0}}),
		 bottom(p_bottom,{{&p_near,&p_far,&p_left,&p_right}},{{StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_270,StitchedHeightmap::DEG_90}}),
		 far(p_far,{{&p_bottom,&p_top,&p_left,&p_right}},{{StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_180,StitchedHeightmap::DEG_180}}),
		 left(p_left,{{&p_top,&p_bottom,&p_far,&p_near}},{{StitchedHeightmap::DEG_270,StitchedHeightmap::DEG_90,StitchedHeightmap::DEG_180,StitchedHeightmap::DEG_0}}),
		 right(p_right,{{&p_top,&p_bottom,&p_near,&p_far}},{{StitchedHeightmap::DEG_90,StitchedHeightmap::DEG_270,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_180}})
	{}
	
	~CubeHeightmap()
	{}
};

#endif // CUBE_HEIGHTMAP_H
