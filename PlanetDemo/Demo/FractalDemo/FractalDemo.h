#ifndef FRACTAL_DEMO_H
#define FRACTAL_DEMO_H

#include <iostream>

#include <PlanetDemo/Demo/FFGLDemoBase.h>
#include <PlanetDemo/Demo/DemoUtils.h>

#include <PlanetLib/Heightmap/IHeightmap.h>
#include <PlanetLib/Heightmap/SpherePatch.h>

#include <PlanetLib/PatchTerrain/PatchTerrain.h>
#include <PlanetLib/PlanetTerrain.h>

#include <PlanetLib/Heightmap/Composites/CubeHeightmap.h>

#include <PlanetLib/Heightmap/Decorators/FractalHeightmap.h>

class FlatSurface : public IParametricSurface
{
private:
	const IHeightmap &heightmap;
	
	const vec4 normal = {0.0,1.0f,0.0f};
	
public:
	
	virtual vec4 GetNormal(const vec4 &vec) const override
	{
		/*
		 * TODO: Meg kéne csinálni!
		 */
		return normal;
	}
	
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

class FractalDemo : public FFGLDemoBase
{
private:
	ImageHeightmap terrain_heightmap;
	//CubeHeightmap planet_heightmap;
	
	PatchTerrain test_terrain;
	
	FractalHeightmap image_fractal;
	FlatSurface image_heightmap;
	
	SpherePatch sphere_patches[6] =
	{
		{{{{-1.0, 1.0,-1.0},{ 1.0, 1.0,-1.0},{ 1.0, 1.0, 1.0},{-1.0, 1.0, 1.0}}}},
		{{{{-1.0,-1.0, 1.0},{ 1.0,-1.0, 1.0},{ 1.0,-1.0,-1.0},{-1.0,-1.0,-1.0}}}},
		{{{{-1.0, 1.0,-1.0},{-1.0, 1.0, 1.0},{-1.0,-1.0, 1.0},{-1.0,-1.0,-1.0}}}},
		{{{{ 1.0, 1.0, 1.0},{ 1.0, 1.0,-1.0},{ 1.0,-1.0,-1.0},{ 1.0,-1.0, 1.0}}}},
		{{{{-1.0,-1.0,-1.0},{ 1.0,-1.0,-1.0},{ 1.0, 1.0,-1.0},{-1.0, 1.0,-1.0}}}},
		{{{{-1.0, 1.0, 1.0},{ 1.0, 1.0, 1.0},{ 1.0,-1.0, 1.0},{-1.0,-1.0, 1.0}}}},
	};
	
	PlanetTerrain test_planet;
	
	void DrawStitchedHeightmaps();
	
public:
	
	virtual void Logic() override;
	
	virtual void Draw() override;
	
	FractalDemo(GrayscaleImage &p_terrain)
		:terrain_heightmap(p_terrain),
		 //planet_heightmap(terrain_heightmap,terrain_heightmap,terrain_heightmap,terrain_heightmap,terrain_heightmap,terrain_heightmap),
		 test_terrain(std::array<vec4,4>{{vec4(0.0,0.0),vec4(0.0,1.0),vec4(1.0,1.0),vec4(1.0,0.0)}}),
		 image_fractal(terrain_heightmap,{0.0,0.0},std::array<float,4>{{1.25,0.05,3.7,0.4,}},std::array<float,4>{{60,30,15,170,}}),
		 image_heightmap(image_fractal)
	{
		StitchedHeightmap test_segment10(image_fractal);
		StitchedHeightmap test_segment11(image_fractal);
		StitchedHeightmap test_segment12(image_fractal);
		
		test_segment10.SetNeighbor(StitchedHeightmap::NORTH,image_fractal,StitchedHeightmap::DEG_90);
		test_segment11.SetNeighbor(StitchedHeightmap::EAST,image_fractal,StitchedHeightmap::DEG_270);
		
		test_segment10.SetNeighbor(StitchedHeightmap::EAST,image_fractal,StitchedHeightmap::DEG_0);
		test_segment12.SetNeighbor(StitchedHeightmap::WEST,image_fractal,StitchedHeightmap::DEG_0);
		
		test_segment11.SetNeighbor(StitchedHeightmap::NORTH,image_fractal,StitchedHeightmap::DEG_180);
		test_segment12.SetNeighbor(StitchedHeightmap::NORTH,image_fractal,StitchedHeightmap::DEG_180);
		
		test_segment10.GetValue({0.5,0.0});
		test_segment10.GetValue({0.8,0.0});
		test_segment10.GetValue({1.0,0.0});
		test_segment10.GetValue({1.0,0.2});
		test_segment10.GetValue({1.0,0.49999});
		
		test_segment12.GetValue({0.49999,0.0});
		test_segment12.GetValue({0.2,0.0});
		test_segment12.GetValue({0.0,0.0});
		test_segment12.GetValue({0.0,0.2});
		test_segment12.GetValue({0.0,0.49999});
		
		std::cout << "8==================D" << std::endl;
		
		test_terrain.Split();
		for(PatchTerrain *child1 : test_terrain.GetChildren())
		{
			child1->Split();
			
			for(PatchTerrain *child2 : child1->GetChildren())
			{
				child2->Split();
				
				for(PatchTerrain *child3 : child2->GetChildren())
				{
					child3->Split();
				}
			}
		}
		
		for(PatchTerrain *terrain : test_planet.GetTerrains())
		{
			terrain->Split();
			
			for(PatchTerrain *child1 : terrain->GetChildren())
			{
				child1->Split();
				
				for(PatchTerrain *child2 : child1->GetChildren())
				{
					child2->Split();
					
					for(PatchTerrain *child3 : child2->GetChildren())
					{
						child3->Split();
					}
				}
			}
		}
	}
	
	virtual ~FractalDemo() override
	{}
};

#endif // FRACTAL_DEMO_H
