#ifndef GL4DEMO_H
#define GL4DEMO_H

#include <PlanetDemo/Demo/IDemo.h>

#include <PlanetLib/PlanetTerrain.h>
#include <PlanetLib/Heightmap/SpherePatch.h>
#include <PlanetLib/Heightmap/Decorators/FractalHeightmap.h>
#include <PlanetLib/Heightmap/Composites/StitchedHeightmap/StitchedHeightmap.h>
#include <PlanetLib/LevelOfDetail/LodManager.h>

#include "GLShaderProgram.h"
#include <PlanetDemo/Demo/DemoUtils.h>


class GL4DemoBase : public IDemo
{
protected:
	Sphere planet_sphere = {{0.0,0.0,0.0},1000000};
	//Sphere planet_sphere = {{0.0,0.0,0.0},22000.0};
	PlanetTerrain test_planet;
	
	SpherePatch sphere_patches[6] =
	{
		{{{{-1.0, 1.0,-1.0},{ 1.0, 1.0,-1.0},{ 1.0, 1.0, 1.0},{-1.0, 1.0, 1.0}}}},
		{{{{-1.0,-1.0, 1.0},{ 1.0,-1.0, 1.0},{ 1.0,-1.0,-1.0},{-1.0,-1.0,-1.0}}}},
		{{{{-1.0, 1.0,-1.0},{-1.0, 1.0, 1.0},{-1.0,-1.0, 1.0},{-1.0,-1.0,-1.0}}}},
		{{{{ 1.0, 1.0, 1.0},{ 1.0, 1.0,-1.0},{ 1.0,-1.0,-1.0},{ 1.0,-1.0, 1.0}}}},
		{{{{-1.0,-1.0,-1.0},{ 1.0,-1.0,-1.0},{ 1.0, 1.0,-1.0},{-1.0, 1.0,-1.0}}}},
		{{{{-1.0, 1.0, 1.0},{ 1.0, 1.0, 1.0},{ 1.0,-1.0, 1.0},{-1.0,-1.0, 1.0}}}},
	};
	
	GrayscaleImage &noise_image;
	ImageHeightmap terrain_heightmap;
	
	FractalHeightmap fractals[6] =
	{
		{terrain_heightmap,{+0.0f,+1.0f},std::array<float,4>{{0.25,1.35,1.58,0.84,}},std::array<float,4>{{1.0*34000,1.0*8000,1.0*15000,1.0*7000,}}},
		{terrain_heightmap,{+0.0f,-1.0f},std::array<float,4>{{0.25,1.35,1.58,0.84,}},std::array<float,4>{{1.0*34000,1.0*8000,1.0*15000,1.0*7000,}}},
		{terrain_heightmap,{-1.0f,+0.0f},std::array<float,4>{{0.25,1.35,1.58,8.84,}},std::array<float,4>{{1.0*76000,1.0*8000,1.0*15000,1.0*7000,}}},
		{terrain_heightmap,{+1.0f,+0.0f},std::array<float,4>{{0.25,1.35,1.58,3.84,}},std::array<float,4>{{1.0*76000,1.0*8000,1.0*15000,1.0*7000,}}},
		{terrain_heightmap,{+0.0f,-2.0f},std::array<float,4>{{0.25,1.35,1.58,0.84,}},std::array<float,4>{{1.0*76000,1.0*8000,1.0*15000,1.0*7000,}}},
		{terrain_heightmap,{+0.0f,+0.0f},std::array<float,4>{{0.25,1.35,1.58,0.84,}},std::array<float,4>{{1.0*76000,1.0*8000,1.0*15000,1.0*7000,}}},
	};
	
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
	std::tuple<FractalHeightmap*,StitchedHeightmap::HeightmapRotation>neighbors[6][4] =
	{
		{
			// p_top,{{&p_far,&p_near,&p_left,&p_right}},
			// {{StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_90,StitchedHeightmap::DEG_270}})
			{&fractals[PlanetTerrain::FAR],StitchedHeightmap::DEG_0},
			{&fractals[PlanetTerrain::NEAR],StitchedHeightmap::DEG_0},
			{&fractals[PlanetTerrain::LEFT],StitchedHeightmap::DEG_90},
			{&fractals[PlanetTerrain::RIGHT],StitchedHeightmap::DEG_270}
		},
		{
			// bottom(p_bottom,{{&p_near,&p_far,&p_left,&p_right}},
			// {{StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_270,StitchedHeightmap::DEG_90}})
			{&fractals[PlanetTerrain::NEAR],StitchedHeightmap::DEG_0},
			{&fractals[PlanetTerrain::FAR],StitchedHeightmap::DEG_0},
			{&fractals[PlanetTerrain::LEFT],StitchedHeightmap::DEG_270},
			{&fractals[PlanetTerrain::RIGHT],StitchedHeightmap::DEG_90}
		},
		{
			// left(p_left,{{&p_top,&p_bottom,&p_far,&p_near}},
			// {{StitchedHeightmap::DEG_270,StitchedHeightmap::DEG_90,StitchedHeightmap::DEG_180,StitchedHeightmap::DEG_0}})
			{&fractals[PlanetTerrain::TOP],StitchedHeightmap::DEG_270},
			{&fractals[PlanetTerrain::BOTTOM],StitchedHeightmap::DEG_90},
			{&fractals[PlanetTerrain::FAR],StitchedHeightmap::DEG_180},
			{&fractals[PlanetTerrain::NEAR],StitchedHeightmap::DEG_0}
		},
		{
			// right(p_right,{{&p_top,&p_bottom,&p_near,&p_far}},
			// {{StitchedHeightmap::DEG_90,StitchedHeightmap::DEG_270,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_180}})
			{&fractals[PlanetTerrain::TOP],StitchedHeightmap::DEG_90},
			{&fractals[PlanetTerrain::BOTTOM],StitchedHeightmap::DEG_270},
			{&fractals[PlanetTerrain::NEAR],StitchedHeightmap::DEG_0},
			{&fractals[PlanetTerrain::FAR],StitchedHeightmap::DEG_180}
		},
		{
			// far(p_far,{{&p_bottom,&p_top,&p_left,&p_right}},
			// {{StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_180,StitchedHeightmap::DEG_180}})
			{&fractals[PlanetTerrain::BOTTOM],StitchedHeightmap::DEG_0},
			{&fractals[PlanetTerrain::TOP],StitchedHeightmap::DEG_0},
			{&fractals[PlanetTerrain::LEFT],StitchedHeightmap::DEG_180},
			{&fractals[PlanetTerrain::RIGHT],StitchedHeightmap::DEG_180}
		},
		{
			// near(p_near,{{&p_top,&p_bottom,&p_left,&p_right}},
			// {{StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0}})
			{&fractals[PlanetTerrain::TOP],StitchedHeightmap::DEG_0},
			{&fractals[PlanetTerrain::BOTTOM],StitchedHeightmap::DEG_0},
			{&fractals[PlanetTerrain::LEFT],StitchedHeightmap::DEG_0},
			{&fractals[PlanetTerrain::RIGHT],StitchedHeightmap::DEG_0}
		},
	};
	
	/*
	 :top(p_top,{{&p_far,&p_near,&p_left,&p_right}},{{StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_90,StitchedHeightmap::DEG_270}}),
		 near(p_near,{{&p_top,&p_bottom,&p_left,&p_right}},{{StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0}}),
		 bottom(p_bottom,{{&p_near,&p_far,&p_left,&p_right}},{{StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_270,StitchedHeightmap::DEG_90}}),
		 far(p_far,{{&p_bottom,&p_top,&p_left,&p_right}},{{StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_180,StitchedHeightmap::DEG_180}}),
		 left(p_left,{{&p_top,&p_bottom,&p_far,&p_near}},{{StitchedHeightmap::DEG_270,StitchedHeightmap::DEG_90,StitchedHeightmap::DEG_180,StitchedHeightmap::DEG_0}}),
		 right(p_right,{{&p_top,&p_bottom,&p_near,&p_far}},{{StitchedHeightmap::DEG_90,StitchedHeightmap::DEG_270,StitchedHeightmap::DEG_0,StitchedHeightmap::DEG_180}})
	 */
	
	LodManager dynamic_lod;
	
	mat4 projection_matrix;
	mat4 view_matrix;
	
	vec4 camera_pos;
	
	void ApplyLod()
	{
		dynamic_lod.CalculateLod();
	}
	
public:
	
	virtual void ApplyProjectionTransform() override
	{
		projection_matrix = perspective<float>(PI/2.0,1.0,1.0,-200.0);
	}
	
	virtual void ApplyViewTransform(const vec4 &position,float yaw,float pitch) override
	{
		mat4 rot_x = rotate_x<float>(pitch);
		mat4 rot_y = rotate_y<float>(yaw);
		mat4 move = translate<float>(-1.0 * position);
		
		//view_matrix = rot_x * rot_y * move;
		view_matrix = move * rot_y * rot_x;
		
		camera_pos = position;
	}
	
	GL4DemoBase(GrayscaleImage &p_noise_image,Camera3D &p_camera)
		:noise_image(p_noise_image),terrain_heightmap(p_noise_image),dynamic_lod(p_camera,planet_sphere,sphere_patches,test_planet,8)
	{
		GLLoadExtensions();
		
		glEnable(GL_DEPTH_TEST);
	}
	
	virtual ~GL4DemoBase() override
	{}
};

#endif // GL4DEMO_H
