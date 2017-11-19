#ifndef STITCHED_HEIGHTMAP_TEST_UTILS_H
#define STITCHED_HEIGHTMAP_TEST_UTILS_H

#include <iostream>
#include <glmath.hpp>

void PrintVector(const vec4 &v)
{
	std::cout << "["  << v[0] << "," << v[1] << "," << v[2] << "]" << std::endl;
}

bool VectorsEqual(const vec4 &v1,const vec4 &v2,float epsilon)
{
	vec4 tmp = v1 - v2;
	
	return	std::abs(tmp[0]) < epsilon &&
			std::abs(tmp[1]) < epsilon;
}

#endif // STITCHED_HEIGHTMAP_TEST_UTILS_H
