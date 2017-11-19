#ifndef PLANET_DEMO_H
#define PLANET_DEMO_H

#include <PlanetDemo/Demo/FFGLDemoBase.h>

#include <glmath.hpp>

/*
 * Planet renderer tutorial:
 * https://acko.net/blog/making-worlds-1-of-spheres-and-cubes/
 * 
 * Non uniform patch terrain
 * http://victorbush.com/2015/01/tessellated-terrain/
 */

#include <PlanetDemo/Camera3D.h>

#include <list>

#include <PlanetLib/PlanetTerrain.h>
#include <PlanetLib/LevelOfDetail/LodManager.h>
#include <PlanetLib/MeshGenerator/TerrainJob.h>

#include <PlanetDemo/Demo/ExperimentalSamplers.h>

class PlanetDemo : public FFGLDemoBase
{
private:
	Sphere test_planet;
	
	SpherePatch sphere_patches[6] =
	{
		{{{{-1.0, 1.0,-1.0},{ 1.0, 1.0,-1.0},{ 1.0, 1.0, 1.0},{-1.0, 1.0, 1.0}}}},
		{{{{-1.0,-1.0, 1.0},{ 1.0,-1.0, 1.0},{ 1.0,-1.0,-1.0},{-1.0,-1.0,-1.0}}}},
		{{{{-1.0, 1.0,-1.0},{-1.0, 1.0, 1.0},{-1.0,-1.0, 1.0},{-1.0,-1.0,-1.0}}}},
		{{{{ 1.0, 1.0, 1.0},{ 1.0, 1.0,-1.0},{ 1.0,-1.0,-1.0},{ 1.0,-1.0, 1.0}}}},
		{{{{-1.0,-1.0,-1.0},{ 1.0,-1.0,-1.0},{ 1.0, 1.0,-1.0},{-1.0, 1.0,-1.0}}}},
		{{{{-1.0, 1.0, 1.0},{ 1.0, 1.0, 1.0},{ 1.0,-1.0, 1.0},{-1.0,-1.0, 1.0}}}},
	};
	
	PlanetTerrain terrain;
	
	LodManager dynamic_lod;
	
	StaticMesh planet_mesh[6];
	
	TerrainJobPartitioner job_partitioner;
	
	const Camera3D &camera;
	
	bool first_iteration = true;
	
	void SetupTerrain1()
	{
		terrain.GetTerrain(PlanetTerrain::TOP).Split();
		terrain.GetTerrain(PlanetTerrain::TOP).GetChild(PatchTerrain::BOTTOM_LEFT)->Split();
		terrain.GetTerrain(PlanetTerrain::TOP).GetChild(PatchTerrain::BOTTOM_LEFT)->GetChild(PatchTerrain::BOTTOM_LEFT)->Split();
		
		for(PatchTerrain *child : terrain.GetTerrain(PlanetTerrain::TOP).GetChild(PatchTerrain::BOTTOM_LEFT)->GetChild(PatchTerrain::BOTTOM_LEFT)->GetChildren())
		{
			child->Split();
			
			for(PatchTerrain *child2 : child->GetChildren())
			{
				child2->Split();
				
				for(PatchTerrain *child3 : child2->GetChildren())
				{
					child3->Split();
				}
			}
		}
	}
	
	void SetupTerrain2()
	{
		terrain.GetTerrain(PlanetTerrain::NEAR).Split();
		terrain.GetTerrain(PlanetTerrain::NEAR).GetChild(PatchTerrain::TOP_LEFT)->Split();
		terrain.GetTerrain(PlanetTerrain::NEAR).GetChild(PatchTerrain::TOP_LEFT)->GetChild(PatchTerrain::TOP_LEFT)->Split();
		
		for(PatchTerrain *child : terrain.GetTerrain(PlanetTerrain::NEAR).GetChild(PatchTerrain::TOP_LEFT)->GetChild(PatchTerrain::TOP_LEFT)->GetChildren())
		{
			child->Split();
			
			for(PatchTerrain *child2 : child->GetChildren())
			{
				child2->Split();
				
				for(PatchTerrain *child3 : child2->GetChildren())
				{
					child3->Split();
				}
			}
		}
		
		terrain.GetTerrain(PlanetTerrain::NEAR).Merge();
	}
	
	void SingleThreadedTriangulation();
	void MultiThreadedTriangulation();
	
public:
	
	virtual void Logic() override;
	
	virtual void Draw() override;
	
	PlanetDemo(Camera3D &p_camera)
		:test_planet({0.0,-2000.0,-6000.0},3000.0),dynamic_lod(p_camera,test_planet,sphere_patches,terrain,10),camera(p_camera)
	{
		p_camera.Move(0.0,-600,-1000);
		//SetupTerrain2();
	}
	
	virtual ~PlanetDemo() override
	{}
};

#endif // PLANET_DEMO_H
