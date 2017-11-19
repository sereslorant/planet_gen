
#include <iostream>

#include <PlanetDemo/Demo/DemoUtils.h>
#include <PlanetDemo/Demo/FractalDemo/FractalDemo.h>

#include "StitchedHeightmapTestUtils.h"


bool TestTopLeft()
{
	RippleHeightmap test_noisemap{1.0,{0.5,0.5}};
	FractalHeightmap heightmap(test_noisemap,{0.0,0.0},std::array<float,4>{{1.25,0.05,3.7,0.4,}},std::array<float,4>{{60,30,15,170,}});
	
	ParamTransformedHeightmap test_weird_heightmap(heightmap);
	
	vec4 test_param1{-1.0/2.0,-1.0};
	vec4 test_param2{-1.0,-1.0/2.0};
	
	std::cout << "Test 1: Top left" << std::endl;
	std::cout << "Test param 1: " << std::endl;
	PrintVector(test_param1);
	std::cout << "Transformed param 1: " << std::endl;
	float result1 = test_weird_heightmap.GetValue(test_param1);
	std::cout << "Value: " << result1 << std::endl;
	std::cout << "Test param 2: " << std::endl;
	PrintVector(test_param2);
	std::cout << "Transformed param 2: " << std::endl;
	float result2 = test_weird_heightmap.GetValue(test_param2);
	std::cout << "Value: " << result2 << std::endl;
	
	return std::abs(result1 - result2) < 1e-2;
}

bool TestTopRight()
{
	RippleHeightmap test_noisemap{1.0,{0.5,0.5}};
	FractalHeightmap heightmap(test_noisemap,{0.0,0.0},std::array<float,4>{{1.25,0.05,3.7,0.4,}},std::array<float,4>{{60,30,15,170,}});
	
	ParamTransformedHeightmap test_weird_heightmap(heightmap);
	
	vec4 test_param1{1.0 + 1.0/2.0,-1.0};
	vec4 test_param2{1.0 + 1.0,-1.0/2.0};
	
	std::cout << "Test 2: Top right" << std::endl;
	std::cout << "Test param 1: " << std::endl;
	PrintVector(test_param1);
	std::cout << "Transformed param 1: " << std::endl;
	float result1 = test_weird_heightmap.GetValue(test_param1);
	std::cout << "Value: " << result1 << std::endl;
	std::cout << "Test param 2: " << std::endl;
	PrintVector(test_param2);
	std::cout << "Transformed param 2: " << std::endl;
	float result2 = test_weird_heightmap.GetValue(test_param2);
	std::cout << "Value: " << result2 << std::endl;
	
	return std::abs(result1 - result2) < 1e-2;
}

bool TestBottomLeft()
{
	RippleHeightmap test_noisemap{1.0,{0.5,0.5}};
	FractalHeightmap heightmap(test_noisemap,{0.0,0.0},std::array<float,4>{{1.25,0.05,3.7,0.4,}},std::array<float,4>{{60,30,15,170,}});
	
	ParamTransformedHeightmap test_weird_heightmap(heightmap);
	
	vec4 test_param1{-1.0,1.0 + 1.0/2.0};
	vec4 test_param2{-1.0/2.0,1.0 + 1.0};
	
	std::cout << "Test 3: Bottom left" << std::endl;
	std::cout << "Test param 1: " << std::endl;
	PrintVector(test_param1);
	std::cout << "Transformed param 1: " << std::endl;
	float result1 = test_weird_heightmap.GetValue(test_param1);
	std::cout << "Value: " << result1 << std::endl;
	std::cout << "Test param 2: " << std::endl;
	PrintVector(test_param2);
	std::cout << "Transformed param 2: " << std::endl;
	float result2 = test_weird_heightmap.GetValue(test_param2);
	std::cout << "Value: " << result2 << std::endl;
	
	return std::abs(result1 - result2) < 1e-2;
}

bool TestBottomRight()
{
	RippleHeightmap test_noisemap{1.0,{0.5,0.5}};
	FractalHeightmap heightmap(test_noisemap,{0.0,0.0},std::array<float,4>{{1.25,0.05,3.7,0.4,}},std::array<float,4>{{60,30,15,170,}});
	
	ParamTransformedHeightmap test_weird_heightmap(heightmap);
	
	vec4 test_param1{1.0 + 1.0,1.0 + 1.0/2.0};
	vec4 test_param2{1.0 + 1.0/2.0,1.0 + 1.0};
	
	std::cout << "Test 4: Bottom right" << std::endl;
	std::cout << "Test param 1: " << std::endl;
	PrintVector(test_param1);
	std::cout << "Transformed param 1: " << std::endl;
	float result1 = test_weird_heightmap.GetValue(test_param1);
	std::cout << "Value: " << result1 << std::endl;
	std::cout << "Test param 2: " << std::endl;
	PrintVector(test_param2);
	std::cout << "Transformed param 2: " << std::endl;
	float result2 = test_weird_heightmap.GetValue(test_param2);
	std::cout << "Value: " << result2 << std::endl;
	
	return std::abs(result1 - result2) < 1e-2;
}

int main(int argc,char *argv[])
{
	if(	TestTopLeft() &&
		TestTopRight() &&
		TestBottomLeft() &&
		TestBottomRight()
		)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
