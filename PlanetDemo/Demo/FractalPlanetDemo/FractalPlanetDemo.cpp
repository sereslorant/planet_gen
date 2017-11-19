
#include "FractalPlanetDemo.h"


#include <SDL2/SDL_opengl.h>

#include <Multithreading/JobQueue.h>

class TerrainJobWrapper : public Job
{
private:
	TerrainJob &wrapped_job;
	bool finished = false;
	
public:
	
	virtual bool IsFinished() override
	{
		return finished;
	}
	
	virtual void Execute() override
	{
		wrapped_job.PerformOperation();
		finished = true;
	}
	
	TerrainJobWrapper(TerrainJob *p_wrapped_job)
		:Job(0,nullptr),wrapped_job(*p_wrapped_job)
	{}
	
	virtual ~TerrainJobWrapper()
	{}
	/*
	 * End of class
	 */
};

void DrawMeshes(const std::list<StaticMesh> &meshes)
{
	glBegin(GL_TRIANGLES);
	for(const StaticMesh &mesh : meshes)
	{
		for(int j=0;j < mesh.index_array.size();j++)
		{
			const vec4 &vertex = mesh.vertex_array[mesh.index_array[j]];
			glVertex3f(vertex[0],vertex[1],vertex[2]);
		}
	}
	glEnd();
}

#include <Multithreading/Worker.h>

#include <thread>

void FractalPlanetDemo::Logic()
{
	auto Before = std::chrono::steady_clock::now();
	dynamic_lod.CalculateLod();
	auto After = std::chrono::steady_clock::now();
	
	std::cout << "Lod time: " << std::chrono::duration<float,std::milli>(After - Before).count() << std::endl;
	
	job_partitioner.Clear();
	
	ScaledSurface scales[6] =
	{
		ScaledSurface(sphere_patches[0],test_planet.GetRadius()),
		ScaledSurface(sphere_patches[1],test_planet.GetRadius()),
		ScaledSurface(sphere_patches[2],test_planet.GetRadius()),
		ScaledSurface(sphere_patches[3],test_planet.GetRadius()),
		ScaledSurface(sphere_patches[4],test_planet.GetRadius()),
		ScaledSurface(sphere_patches[5],test_planet.GetRadius()),
	};
	
	TranslatedSurface translate[6] =
	{
		TranslatedSurface(scales[0],test_planet.GetPosition()),
		TranslatedSurface(scales[1],test_planet.GetPosition()),
		TranslatedSurface(scales[2],test_planet.GetPosition()),
		TranslatedSurface(scales[3],test_planet.GetPosition()),
		TranslatedSurface(scales[4],test_planet.GetPosition()),
		TranslatedSurface(scales[5],test_planet.GetPosition()),
	};
	
	const IHeightmap *sphere_heightmaps[6] = 
	{
		&planet_heightmap.GetTop(),
		&planet_heightmap.GetBottom(),
		&planet_heightmap.GetLeft(),
		&planet_heightmap.GetRight(),
		&planet_heightmap.GetFar(),
		&planet_heightmap.GetNear(),
	};
	
	HeightmapSurface heightmap[6] =
	{
		HeightmapSurface(translate[0],sphere_patches[0],*sphere_heightmaps[0]),
		HeightmapSurface(translate[1],sphere_patches[1],*sphere_heightmaps[1]),
		HeightmapSurface(translate[2],sphere_patches[2],*sphere_heightmaps[2]),
		HeightmapSurface(translate[3],sphere_patches[3],*sphere_heightmaps[3]),
		HeightmapSurface(translate[4],sphere_patches[4],*sphere_heightmaps[4]),
		HeightmapSurface(translate[5],sphere_patches[5],*sphere_heightmaps[5]),
	};
	
	Before = std::chrono::steady_clock::now();
	
	for(int i=0;i < 6;i++)
	{
		job_partitioner.PartitionLeaves(test_planet_terrain.GetTerrain((PlanetTerrain::TerrainPosition)i),heightmap[i]);
	}
	
	After = std::chrono::steady_clock::now();
	std::cout << "Partitioning: " << std::chrono::duration<float,std::milli>(After - Before).count() << std::endl;
	
	JobQueue queue;
	for(TerrainJob &terrain_job : job_partitioner.GetTerrainJobs())
	{
		queue.CreateJob<TerrainJobWrapper>(&terrain_job);
	}
	
	Worker firstWorker(queue);
	Worker secondWorker(queue);
	Worker thirdWorker(queue);
	Worker fourthWorker(queue);
	
	firstWorker.AllowToStop();
	secondWorker.AllowToStop();
	thirdWorker.AllowToStop();
	fourthWorker.AllowToStop();
	
	Before = std::chrono::steady_clock::now();
	
	std::thread threads[4];
	threads[0] = std::thread(Worker::RunWorker,&firstWorker);
	threads[1] = std::thread(Worker::RunWorker,&secondWorker);
	threads[2] = std::thread(Worker::RunWorker,&thirdWorker);
	threads[3] = std::thread(Worker::RunWorker,&fourthWorker);
	
	for(int i=0;i < 4;i++)
	{
		threads[i].join();
	}
	
	After = std::chrono::steady_clock::now();
	std::cout << "Mesh generation: " << std::chrono::duration<float,std::milli>(After - Before).count() << std::endl;
}

void FractalPlanetDemo::Draw()
{
	glColor3f(0.5,0.5,0.5);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	
	DrawMeshes(job_partitioner.GetMeshes());
	/*
	glTranslatef(0,600.0,-1250.0);
	for(int i=0;i < 6;i++)
	{
		ScaledSurface scaled_surface(sphere_patches[i],500.0);
		HeightmapSurface sphere_surface(scaled_surface,sphere_patches[i],*sphere_heightmaps[i]);
		NeighborTrackingRenderer planet_renderer(sphere_surface);
		std::function<void(PatchTerrain &)> drawWithNeighbors = [&planet_renderer](PatchTerrain &terrain)
																	{planet_renderer.Draw(terrain);};
		
		ForeachLeaf(test_planet_terrain.GetTerrain((PlanetTerrain::TerrainPosition)i),drawWithNeighbors);
	}*/
}
