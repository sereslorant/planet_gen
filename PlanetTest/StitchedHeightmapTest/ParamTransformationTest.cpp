
#include <iostream>

#include <PlanetDemo/Demo/DemoUtils.h>
#include <PlanetDemo/Demo/FractalDemo/FractalDemo.h>

#include "StitchedHeightmapTestUtils.h"

bool TestTopLeft()
{
	ParamTransformation param_transformation;
	
	vec4 test_param1{-1.0/2.0,-1.0};
	vec4 test_param2{-1.0,-1.0/2.0};
	
	std::cout << "Test 1: Top left" << std::endl;
	std::cout << "Test param 1: " << std::endl;
	PrintVector(test_param1);
	std::cout << "Transformed param 1: " << std::endl;
	vec4 result1 = param_transformation.GetValue(test_param1);
	PrintVector(result1);
	std::cout << "Test param 2: " << std::endl;
	PrintVector(test_param2);
	std::cout << "Transformed param 2: " << std::endl;
	vec4 result2 = param_transformation.GetValue(test_param2);
	PrintVector(result2);
	
	return VectorsEqual(result1,result2,1e-2);
}

bool TestTopRight()
{
	ParamTransformation param_transformation;
	
	vec4 test_param1{1.0 + 1.0/2.0,-1.0};
	vec4 test_param2{1.0 + 1.0,-1.0/2.0};
	
	std::cout << "Test 2: Top right" << std::endl;
	std::cout << "Test param 1: " << std::endl;
	PrintVector(test_param1);
	std::cout << "Transformed param 1: " << std::endl;
	vec4 result1 = param_transformation.GetValue(test_param1);
	PrintVector(result1);
	std::cout << "Test param 2: " << std::endl;
	PrintVector(test_param2);
	std::cout << "Transformed param 2: " << std::endl;
	vec4 result2 = param_transformation.GetValue(test_param2);
	PrintVector(result2);
	
	return VectorsEqual(result1,result2,1e-2);
}

bool TestBottomLeft()
{
	ParamTransformation param_transformation;
	
	vec4 test_param1{-1.0,1.0 + 1.0/2.0};
	vec4 test_param2{-1.0/2.0,1.0 + 1.0};
	
	std::cout << "Test 3: Bottom left" << std::endl;
	std::cout << "Test param 1: " << std::endl;
	PrintVector(test_param1);
	std::cout << "Transformed param 1: " << std::endl;
	vec4 result1 = param_transformation.GetValue(test_param1);
	PrintVector(result1);
	std::cout << "Test param 2: " << std::endl;
	PrintVector(test_param2);
	std::cout << "Transformed param 2: " << std::endl;
	vec4 result2 = param_transformation.GetValue(test_param2);
	PrintVector(result2);
	
	return VectorsEqual(result1,result2,1e-2);
}

bool TestBottomRight()
{
	ParamTransformation param_transformation;
	
	vec4 test_param1{1.0 + 1.0,1.0 + 1.0/2.0};
	vec4 test_param2{1.0 + 1.0/2.0,1.0 + 1.0};
	
	std::cout << "Test 4: Bottom right" << std::endl;
	std::cout << "Test param 1: " << std::endl;
	PrintVector(test_param1);
	std::cout << "Transformed param 1: " << std::endl;
	vec4 result1 = param_transformation.GetValue(test_param1);
	PrintVector(result1);
	std::cout << "Test param 2: " << std::endl;
	PrintVector(test_param2);
	std::cout << "Transformed param 2: " << std::endl;
	vec4 result2 = param_transformation.GetValue(test_param2);
	PrintVector(result2);
	
	return VectorsEqual(result1,result2,1e-2);
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
