#ifndef TERRAIN_JOB_H
#define TERRAIN_JOB_H

#include <PlanetLib/PatchTerrain/PatchTerrain.h>
#include <PlanetLib/MeshGenerator/HeightmapSampler.h>

#include <vector>

class TerrainJob
{
private:
	unsigned int next_index = 0;
	std::vector<PatchTerrain *> terrains;
	
	//DefaultHeightmapSampler sampler;
	HeightmapSampler sampler;
	
public:
	
	bool IsFull() const
	{
		return next_index == terrains.size();
	}
	
	void AddTerrain(PatchTerrain *new_terrain)
	{
		terrains[next_index] = new_terrain;
		next_index++;
	}
	
	void PerformOperation()
	{
		if(terrains.size() > next_index)
			{terrains.resize(next_index);}
		
		for(PatchTerrain *terrain : terrains)
		{
			sampler.Sample(*terrain);
		}
	}
	
	TerrainJob(unsigned int p_job_size,const IParametricSurface &heightmap,StaticMesh &result)
		:terrains(p_job_size),sampler(heightmap,result)
	{}
};

#include <list>

class TerrainJobPartitioner
{
private:
	unsigned int job_size = 5000;
	
	std::list<StaticMesh> meshes;
	std::list<TerrainJob> terrain_jobs;
	
	void PushLeaf(PatchTerrain *leaf,const IParametricSurface &heightmap)
	{
		if(terrain_jobs.back().IsFull())
		{
			meshes.emplace_back();
			terrain_jobs.emplace_back(job_size,heightmap,meshes.back());
		}
		
		terrain_jobs.back().AddTerrain(leaf);
	}
	
public:
	
	std::list<TerrainJob> &GetTerrainJobs()
	{
		return terrain_jobs;
	}
	
	const std::list<StaticMesh> &GetMeshes()
	{
		return meshes;
	}
	
	void PartitionLeaves(PatchTerrain &root,const IParametricSurface &heightmap)
	{
		std::function<void(PatchTerrain &)> sample = [this,&heightmap](PatchTerrain &terrain)
														{PushLeaf(&terrain,heightmap);};
		
		meshes.emplace_back();
		terrain_jobs.emplace_back(job_size,heightmap,meshes.back());
		
		ForeachLeaf(root,sample);
	}
	
	void Clear()
	{
		terrain_jobs.clear();
		meshes.clear();
	}
};

#endif // TERRAIN_JOB_H
