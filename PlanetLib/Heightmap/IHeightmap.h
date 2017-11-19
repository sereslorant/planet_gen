#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <glmath.hpp>

class IHeightmap
{
public:
	virtual float GetValue(const vec4 &vec) const = 0;
	
	IHeightmap()
	{}
	
	virtual ~IHeightmap()
	{}
};

#endif // HEIGHTMAP_H
