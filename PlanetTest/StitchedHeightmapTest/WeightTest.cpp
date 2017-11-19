
/*
 * Manuális teszt
 */

#include <iostream>

#include <PlanetDemo/Demo/FractalDemo/FractalDemo.h>

void PrintWeights(const vec4 &param,const vec4 &weights)
{
	std::cout << "[" << param[0] << "," << param[1] << "] [" << weights[0] << "," << weights[1] << "," << weights[2] << "]" << std::endl;
}

int main(int argc,char *argv[])
{
	NorthEastQuadrantWeight ne;
	std::array<vec4,5> ne_params =
	{{
		{0.5,0.0},
		{0.8,0.0},
		{1.0,0.0},
		{1.0,0.2},
		{1.0,0.49999},
	}};
	
	std::cout << "NE:" << std::endl;
	for(const vec4 &param : ne_params)
	{
		PrintWeights(param,ne.GetWeights({{true,true}},param));
	}
	
	NorthWestQuadrantWeight nw;
	std::array<vec4,5> nw_params =
	{{
		{0.49999,0.0},
		{0.2,0.0},
		{0.0,0.0},
		{0.0,0.2},
		{0.0,0.49999},
	}};
	
	std::cout << "NW:" << std::endl;
	for(const vec4 &param : nw_params)
	{
		PrintWeights(param,nw.GetWeights({{true,true}},param));
	}
	
	SouthEastQuadrantWeight se;
	std::array<vec4,5> se_params =
	{{
		{1.0,0.5},
		{1.0,0.8},
		{1.0,1.0},
		{0.8,1.0},
		{0.5,1.0},
	}};
	
	std::cout << "SE:" << std::endl;
	for(const vec4 &param : se_params)
	{
		PrintWeights(param,se.GetWeights({{true,true}},param));
	}
	
	SouthWestQuadrantWeight sw;
	std::array<vec4,5> sw_params =
	{{
		{0.49999,1.0},
		{0.2,1.0},
		{0.0,1.0},
		{0.0,0.8},
		{0.0,0.5},
	}};
	
	std::cout << "SW:" << std::endl;
	for(const vec4 &param : sw_params)
	{
		PrintWeights(param,sw.GetWeights({{true,true}},param));
	}
	
	vec4 test0_param_0 = vec4{0.25,0.0};
	vec4 test0_param_1 = vec4{0.25,1.0};
	
	std::cout << "Neighbor test 0:" << std::endl;
	PrintWeights(test0_param_0,nw.GetWeights({{true,true}},test0_param_0));
	PrintWeights(test0_param_1,sw.GetWeights({{true,true}},test0_param_1));
	
	vec4 test1_param_0 = vec4{0.35,0.0};
	vec4 test1_param_1 = vec4{0.35,1.0};
	
	std::cout << "Neighbor test 1:" << std::endl;
	PrintWeights(test1_param_0,nw.GetWeights({{true,true}},test1_param_0));
	PrintWeights(test1_param_1,sw.GetWeights({{true,true}},test1_param_1));
	
	vec4 test2_param_0 = vec4{0.85,0.0};
	vec4 test2_param_1 = vec4{0.0,0.85};
	
	std::cout << "Neighbor test 2:" << std::endl;
	PrintWeights(test2_param_0,ne.GetWeights({{true,true}},test2_param_0));
	PrintWeights(test2_param_1,sw.GetWeights({{true,true}},test2_param_1));
	
	return 0;
}
