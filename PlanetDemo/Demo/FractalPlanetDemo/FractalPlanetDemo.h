#ifndef FRACTAL_PLANET_DEMO_H
#define FRACTAL_PLANET_DEMO_H

#include <iostream>

#include <PlanetDemo/Demo/FFGLDemoBase.h>
#include <PlanetDemo/Demo/DemoUtils.h>

#include <PlanetLib/Heightmap/IHeightmap.h>
#include <PlanetLib/Heightmap/SpherePatch.h>

#include <PlanetLib/PlanetTerrain.h>
#include <PlanetLib/LevelOfDetail/LodManager.h>
#include <PlanetLib/MeshGenerator/TerrainJob.h>

#include <PlanetLib/Heightmap/Composites/CubeHeightmap.h>

#include <PlanetLib/Heightmap/Decorators/FractalHeightmap.h>

class FlatSurface : public IParametricSurface
{
private:
	const IHeightmap &heightmap;
	
public:
	
	virtual vec4 GetValue(const vec4 &vec) const override
	{
		return {vec[0]* 900,heightmap.GetValue(vec) * 4.0f,vec[1] * 900};
	}
	
	FlatSurface(const IHeightmap &p_fractal_heightmap)
		:heightmap(p_fractal_heightmap)
	{}
	
	virtual ~FlatSurface() override
	{}
};

class FractalPlanetDemo : public FFGLDemoBase
{
private:
	ImageHeightmap terrain_heightmap;
	//CubeHeightmap planet_heightmap;
	/*
	FractalHeightmap fractals[6] =
	{
		{terrain_heightmap,{+0.0f,+1.0f},std::array<float,4>{{0.5,3.5,5.8,2.4,}},std::array<float,4>{{460,80,150,70,}}},
		{terrain_heightmap,{+0.0f,-1.0f},std::array<float,4>{{0.5,3.5,5.8,2.4,}},std::array<float,4>{{460,80,150,70,}}},
		{terrain_heightmap,{-1.0f,+0.0f},std::array<float,4>{{0.5,3.5,5.8,2.4,}},std::array<float,4>{{460,80,150,70,}}},
		{terrain_heightmap,{+1.0f,+0.0f},std::array<float,4>{{0.5,3.5,5.8,2.4,}},std::array<float,4>{{460,80,150,70,}}},
		{terrain_heightmap,{+0.0f,-2.0f},std::array<float,4>{{0.5,3.5,5.8,2.4,}},std::array<float,4>{{460,80,150,70,}}},
		{terrain_heightmap,{+0.0f,+0.0f},std::array<float,4>{{0.5,3.5,5.8,2.4,}},std::array<float,4>{{460,80,150,70,}}},
	};
	*/
	
	FractalHeightmap fractals[6] =
	{
		{terrain_heightmap,{+0.0f,+1.0f},std::array<float,4>{{0.25,1.35,1.58,0.84,}},std::array<float,4>{{1.0*340,1.0*80,1.0*150,1.0*70,}}},
		{terrain_heightmap,{+0.0f,-1.0f},std::array<float,4>{{0.25,1.35,1.58,0.84,}},std::array<float,4>{{1.0*340,1.0*80,1.0*150,1.0*70,}}},
		{terrain_heightmap,{-1.0f,+0.0f},std::array<float,4>{{0.25,1.35,1.58,8.84,}},std::array<float,4>{{1.0*760,1.0*80,1.0*150,1.0*70,}}},
		{terrain_heightmap,{+1.0f,+0.0f},std::array<float,4>{{0.25,1.35,1.58,3.84,}},std::array<float,4>{{1.0*760,1.0*80,1.0*150,1.0*70,}}},
		{terrain_heightmap,{+0.0f,-2.0f},std::array<float,4>{{0.25,1.35,1.58,0.84,}},std::array<float,4>{{1.0*760,1.0*80,1.0*150,1.0*70,}}},
		{terrain_heightmap,{+0.0f,+0.0f},std::array<float,4>{{0.25,1.35,1.58,0.84,}},std::array<float,4>{{1.0*760,1.0*80,1.0*150,1.0*70,}}},
	};
	
	ParamTransformedHeightmap weird_fractals[6] =
	{
		{fractals[0]},
		{fractals[1]},
		{fractals[2]},
		{fractals[3]},
		{fractals[4]},
		{fractals[5]},
	};
	
	CubeHeightmap planet_heightmap =
	{
		weird_fractals[PlanetTerrain::TOP],
		weird_fractals[PlanetTerrain::NEAR],
		weird_fractals[PlanetTerrain::BOTTOM],
		weird_fractals[PlanetTerrain::FAR],
		weird_fractals[PlanetTerrain::LEFT],
		weird_fractals[PlanetTerrain::RIGHT]
	};
	
	//PatchTerrain test_terrain;
	
	SpherePatch sphere_patches[6] =
	{
		{{{{-1.0, 1.0,-1.0},{ 1.0, 1.0,-1.0},{ 1.0, 1.0, 1.0},{-1.0, 1.0, 1.0}}}},
		{{{{-1.0,-1.0, 1.0},{ 1.0,-1.0, 1.0},{ 1.0,-1.0,-1.0},{-1.0,-1.0,-1.0}}}},
		{{{{-1.0, 1.0,-1.0},{-1.0, 1.0, 1.0},{-1.0,-1.0, 1.0},{-1.0,-1.0,-1.0}}}},
		{{{{ 1.0, 1.0, 1.0},{ 1.0, 1.0,-1.0},{ 1.0,-1.0,-1.0},{ 1.0,-1.0, 1.0}}}},
		{{{{-1.0,-1.0,-1.0},{ 1.0,-1.0,-1.0},{ 1.0, 1.0,-1.0},{-1.0, 1.0,-1.0}}}},
		{{{{-1.0, 1.0, 1.0},{ 1.0, 1.0, 1.0},{ 1.0,-1.0, 1.0},{-1.0,-1.0, 1.0}}}},
	};
	
	PlanetTerrain test_planet_terrain;
	
	//Sphere test_planet = {{0.0,-14000.0,-28000.0},22000.0};
	Sphere test_planet = {{0.0,-14000.0,-28000.0},10000.0};
	
	LodManager dynamic_lod;
	
	TerrainJobPartitioner job_partitioner;
	
public:
	
	virtual void Logic() override;
	
	virtual void Draw() override;
	
	FractalPlanetDemo(GrayscaleImage &p_terrain,Camera3D &p_camera)
		:terrain_heightmap(p_terrain),
		 //planet_heightmap(terrain_heightmap,terrain_heightmap,terrain_heightmap,terrain_heightmap,terrain_heightmap,terrain_heightmap),
		 //test_terrain(std::array<vec4,4>{{vec4(0.0,0.0),vec4(0.0,1.0),vec4(1.0,1.0),vec4(1.0,0.0)}}),
		 dynamic_lod(p_camera,test_planet,sphere_patches,test_planet_terrain,10)
	{
		//
	}
	
	virtual ~FractalPlanetDemo() override
	{}
};

#endif // FRACTAL_PLANET_DEMO_H
