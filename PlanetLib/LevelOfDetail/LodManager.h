#ifndef LOD_MANAGER_H
#define LOD_MANAGER_H

//TMP!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include <DemoFramework/Camera3D.h>
//TMP!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#include "ILodStrategy.h"

#include <PlanetLib/Heightmap/IParametricSurface.h>

class Sphere
{
private:
	vec4 position;
	float radius;
	
public:
	
	const vec4 &GetPosition() const
	{
		return position;
	}
	
	float GetRadius() const
	{
		return radius;
	}
	
	Sphere(const vec4 &p_position,float p_radius)
		:position(p_position),radius(p_radius)
	{}
	
	~Sphere()
	{}
};

class DepthConstrainedLod : public ILodStrategy
{
private:
	unsigned int max_depth;
	
public:
	
	virtual bool Subdivide(const PatchTerrain &terrain) const override
	{
		return terrain.GetDepth() < max_depth;
	}
	
	DepthConstrainedLod(unsigned int p_max_depth)
		:max_depth(p_max_depth)
	{}
	
	virtual ~DepthConstrainedLod() override
	{}
};

#include <functional>

class ConditionalStrategy : public ILodStrategy
{
private:
	const ILodStrategy &true_strategy;
	const ILodStrategy &false_strategy;
	
	std::function<bool()> predicate;
	
public:
	
	virtual bool Subdivide(const PatchTerrain &terrain) const override
	{
		if(predicate())
		{
			return true_strategy.Subdivide(terrain);
		}
		else
		{
			return false_strategy.Subdivide(terrain);
		}
	}
	
	ConditionalStrategy(const ILodStrategy &p_true,const ILodStrategy &p_false,std::function<bool()> p_predicate)
		:true_strategy(p_true),false_strategy(p_false),predicate(p_predicate)
	{}
	
	virtual ~ConditionalStrategy() override
	{}
};

class IncreaseDetailLod : public ILodStrategy
{
private:
	unsigned int max_depth;
	
	float power_of_camera_pos;
	
	const Camera3D &camera;
	const IParametricSurface &heightmap;
	
public:
	
	virtual bool Subdivide(const PatchTerrain &terrain) const override
	{
		vec4 dir = heightmap.GetValue(terrain.GetCenter()) - camera.GetPosition();
		float patch_center_dist_sqr = dot(dir,dir);
		
		return patch_center_dist_sqr < ((power_of_camera_pos) * ((max_depth - terrain.GetDepth()) / (float)max_depth));
	}
	
	IncreaseDetailLod(unsigned int p_max_depth,float p_power_of_camera_pos,const Camera3D &p_camera,const IParametricSurface &p_heightmap)
		:max_depth(p_max_depth),power_of_camera_pos(p_power_of_camera_pos),camera(p_camera),heightmap(p_heightmap)
	{}
	
	virtual ~IncreaseDetailLod() override
	{}
};

class WithinRadiusLod : public ILodStrategy
{
private:
	const Sphere &sphere;
	
	const Camera3D &camera;
	const IParametricSurface &heightmap;
	
	float FindClosestDist(const PatchTerrain &terrain) const
	{
		float closest_dist;
		{
			vec4 dir = heightmap.GetValue(terrain.GetCenter()) - camera.GetPosition();
			closest_dist = dot(dir,dir);
		}
		
		for(const vec4 &vertex : terrain.GetVertices())
		{
			vec4 dir = heightmap.GetValue(vertex) - camera.GetPosition();
			float current_length = dot(dir,dir);
			
			if(current_length < closest_dist)
				{closest_dist = current_length;}
		}
		
		return closest_dist;
	}
	
public:
	
	virtual bool Subdivide(const PatchTerrain &terrain) const override
	{
		vec4 dir = heightmap.GetValue(terrain.GetCenter()) - camera.GetPosition();
		float patch_center_dist_sqr = dot(dir,dir);
		
		return patch_center_dist_sqr < sphere.GetRadius() * sphere.GetRadius() * 1.75 * 1.75;
	}
	
	WithinRadiusLod(const Sphere &p_sphere,const Camera3D &p_camera,const IParametricSurface &p_heightmap)
		:sphere(p_sphere),camera(p_camera),heightmap(p_heightmap)
	{}
	
	virtual ~WithinRadiusLod() override
	{}
};

#include <PlanetLib/Heightmap/SpherePatch.h>
#include <tuple>

#include <PlanetLib/PlanetTerrain.h>

#include <PlanetLib/LevelOfDetail/LodOperations/LodNegationOp.h>
#include <PlanetLib/LevelOfDetail/LodOperations/LodConjunctionOp.h>
#include <PlanetLib/LevelOfDetail/LodOperations/LodDisjunctionOp.h>

#include <list>

class LodManager
{
private:
	
	const Camera3D		&camera;
	const Sphere		&planet_sphere;
	const SpherePatch	*sphere_patches;
	
	PlanetTerrain &terrain;
	
	unsigned int max_depth;
	unsigned int switch_depth = 3;
	
	float dist_from_center = 0.0;
	float power_of_camera_pos = 0.0;
	
	void CalculateLodForTerrain(PatchTerrain &terrain,const IParametricSurface &heightmap)
	{
		DepthConstrainedLod depth_constrain(max_depth);
		
		IncreaseDetailLod increase_detail(max_depth,power_of_camera_pos,camera,heightmap);
		WithinRadiusLod within_radius(planet_sphere,camera,heightmap);
		
		DepthConstrainedLod below_switch_depth(switch_depth);
		LodNegationOp greater_or_equal_switch(below_switch_depth);
		
		LodConjunctionOp deep_strategy(greater_or_equal_switch,increase_detail);
		LodConjunctionOp shallow_strategy(below_switch_depth,within_radius);
		
		LodDisjunctionOp close_strategy(deep_strategy,shallow_strategy);
		
		ConditionalStrategy too_far_strategy(close_strategy,increase_detail,std::function<bool()> (
					[this]
				{
					return dist_from_center < planet_sphere.GetRadius() * 1.75;
				}
			)
		);
		
		LodConjunctionOp depth_constrained_lod(depth_constrain,too_far_strategy);
		
		if(depth_constrained_lod.Subdivide(terrain))
		{
			if(!terrain.IsSplit())
			{
				terrain.Split();
			}
		}
		else
		{
			if(terrain.IsSplit())
			{
				terrain.Merge();
			}
		}
	}
	
public:
	
	void CalculateLod()
	{
		dist_from_center	= length(camera.GetPosition() - planet_sphere.GetPosition());
		power_of_camera_pos	= dist_from_center * dist_from_center - planet_sphere.GetRadius() * planet_sphere.GetRadius();
		
		ScaledSurface scales[6] =
		{
			ScaledSurface(sphere_patches[0],planet_sphere.GetRadius()),
			ScaledSurface(sphere_patches[1],planet_sphere.GetRadius()),
			ScaledSurface(sphere_patches[2],planet_sphere.GetRadius()),
			ScaledSurface(sphere_patches[3],planet_sphere.GetRadius()),
			ScaledSurface(sphere_patches[4],planet_sphere.GetRadius()),
			ScaledSurface(sphere_patches[5],planet_sphere.GetRadius()),
		};
		
		TranslatedSurface translate[6] =
		{
			TranslatedSurface(scales[0],planet_sphere.GetPosition()),
			TranslatedSurface(scales[1],planet_sphere.GetPosition()),
			TranslatedSurface(scales[2],planet_sphere.GetPosition()),
			TranslatedSurface(scales[3],planet_sphere.GetPosition()),
			TranslatedSurface(scales[4],planet_sphere.GetPosition()),
			TranslatedSurface(scales[5],planet_sphere.GetPosition()),
		};
		
		std::list<std::tuple<PatchTerrain &,const IParametricSurface &> > terrains_left;
		
		int i=0;
		for(PatchTerrain *terrain : terrain.GetTerrains())
		{
			terrains_left.push_back(std::tie(*terrain,translate[i]));
			i++;
		}
		
		while(terrains_left.size() > 0)
		{
			std::tuple<PatchTerrain &,const IParametricSurface &> next = terrains_left.front();
			terrains_left.pop_front();
			
			CalculateLodForTerrain(std::get<0>(next),std::get<1>(next));
			
			if(std::get<0>(next).IsSplit())
			{
				for(PatchTerrain *child : std::get<0>(next).GetChildren())
				{
					terrains_left.push_back(std::tie(*child,std::get<1>(next)));
				}
			}
		}
	}
	
	LodManager(const Camera3D &p_camera,const Sphere &p_planetSphere,const SpherePatch *p_sphere_patches,PlanetTerrain &p_terrain,unsigned int p_max_depth)
		:camera(p_camera),planet_sphere(p_planetSphere),sphere_patches(p_sphere_patches),terrain(p_terrain),max_depth(p_max_depth)
	{}
};

#endif // LOD_MANAGER_H
