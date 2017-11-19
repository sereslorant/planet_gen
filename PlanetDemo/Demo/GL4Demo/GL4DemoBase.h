#ifndef GL4DEMO_H
#define GL4DEMO_H

#include <PlanetDemo/Demo/IDemo.h>

#include <PlanetLib/PlanetTerrain.h>
#include <PlanetLib/Heightmap/SpherePatch.h>
#include <PlanetLib/LevelOfDetail/LodManager.h>

#include "GLShaderProgram.h"


class GL4DemoBase : public IDemo
{
protected:
	Sphere planet_sphere = {{0.0,0.0,0.0},1000000};
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
		:noise_image(p_noise_image),dynamic_lod(p_camera,planet_sphere,sphere_patches,test_planet,8)
	{
		GLLoadExtensions();
		
		glEnable(GL_DEPTH_TEST);
		
		/*
		test_planet.GetTerrain(PlanetTerrain::NEAR).Split();
		test_planet.GetTerrain(PlanetTerrain::NEAR).GetChild(PatchTerrain::TOP_RIGHT)->Split();
		test_planet.GetTerrain(PlanetTerrain::NEAR).GetChild(PatchTerrain::TOP_RIGHT)->GetChild(PatchTerrain::BOTTOM_LEFT)->Split();
		*/
	}
	
	virtual ~GL4DemoBase() override
	{}
};

#endif // GL4DEMO_H
