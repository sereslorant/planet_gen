

#include <iostream>


#include "PlanetDemo.h"

#include <PlanetDemo/Demo/DemoUtils.h>

#include <SDL2/SDL_opengl.h>

#include <chrono>



class BackfacePredicate
{
private:
	const IParametricSurface &heightmap;
	vec4 dir;
	
public:
	
	bool IsBackface (PatchTerrain &terrain)
	{
		float epsilon = -0.5;
		if(terrain.GetDepth() > 0)
		{
			epsilon = epsilon/terrain.GetDepth();
		}
		/*
		vec4 verts[3] =
			{
				heightmap.GetValue(terrain.GetVertices()[0]),
				heightmap.GetValue(terrain.GetVertices()[1]),
				heightmap.GetValue(terrain.GetVertices()[2]),
			};
		
		return dot(dir,cross(verts[2] - verts[0],verts[1] - verts[0])) < -1.0e-4;
		*/
		return dot(dir,heightmap.GetValue(terrain.GetCenter())) < epsilon;
	}
	
	BackfacePredicate(const IParametricSurface &p_heightmap,const vec4 &p_dir)
		:heightmap(p_heightmap),dir(normalize(p_dir))
	{}
};

bool eliminate_backface = false;

unsigned int sample_method = 0;
double sample_time_acc[4] = {0.0,0.0,0.0,0.0};
double sample_time[4] = {0.0,0.0,0.0,0.0};

unsigned int timer = 0;

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

void PlanetDemo::SingleThreadedTriangulation()
{
	for(int i=0;i < 6;i++)
	{
		ScaledSurface scale(sphere_patches[i],test_planet.GetRadius());
		TranslatedSurface translate(scale,test_planet.GetPosition());
		
		RippleHeightmap ripple_heightmap(20,{0.5,0.5});
		
		HeightmapSurface heightmap(translate,sphere_patches[i],ripple_heightmap);
		//IHeightmap &heightmap = translate;
		
		BackfacePredicate cull_predicate(sphere_patches[i],camera.GetPosition() - test_planet.GetPosition());
		
		std::function<bool(PatchTerrain&)> predicate = [&cull_predicate](PatchTerrain &terrain)
									{return !cull_predicate.IsBackface(terrain);};
		
		if(sample_method == 0)
		{
			InPlaceHeightmapSampler sampler(heightmap,planet_mesh[i]);
			
			unsigned int vertex_array_size = 0;
			unsigned int index_array_size = 0;
			
			std::function<void(PatchTerrain&)> counter = [&vertex_array_size,&index_array_size](PatchTerrain &terrain)
																		{
																			LeafSize size((NeighborInformation(terrain)));
																			
																			vertex_array_size += size.GetVertexArraySize();
																			index_array_size += size.GetIndexArraySize();
																		};
			
			if(eliminate_backface)
				{ForeachLeaf(terrain.GetTerrain((PlanetTerrain::TerrainPosition)i),predicate,counter);}
			else
				{ForeachLeaf(terrain.GetTerrain((PlanetTerrain::TerrainPosition)i),counter);}
			
			//std::cout << "Num vertices: " << vertex_array_size << std::endl;
			//std::cout << "Num indices: " << index_array_size << std::endl;
			
			planet_mesh[i].vertex_array.resize(vertex_array_size);
			planet_mesh[i].normal_array.resize(vertex_array_size);
			planet_mesh[i].index_array.resize(index_array_size);
			
			std::function<void(PatchTerrain &)> sample = [&sampler](PatchTerrain &terrain)
																		{sampler.Sample(terrain);};
			
			if(eliminate_backface)
				{ForeachLeaf(terrain.GetTerrain((PlanetTerrain::TerrainPosition)i),predicate,sample);}
			else
				{ForeachLeaf(terrain.GetTerrain((PlanetTerrain::TerrainPosition)i),sample);}
		}
		else if(sample_method == 1)
		{
			ArrayHeightmapSampler sampler(heightmap,planet_mesh[i]);
			unsigned int vertex_array_size = 0;
			unsigned int index_array_size = 0;
			
			std::function<void(PatchTerrain&)> counter = [&vertex_array_size,&index_array_size](PatchTerrain &terrain)
																		{
																			LeafSize size((NeighborInformation(terrain)));
																			
																			vertex_array_size += size.GetVertexArraySize();
																			index_array_size += size.GetIndexArraySize();
																		};
			
			if(eliminate_backface)
				{ForeachLeaf(terrain.GetTerrain((PlanetTerrain::TerrainPosition)i),predicate,counter);}
			else
				{ForeachLeaf(terrain.GetTerrain((PlanetTerrain::TerrainPosition)i),counter);}
			
			//std::cout << "Num vertices: " << vertexArraySize << std::endl;
			//std::cout << "Num indices: " << indexArraySize << std::endl;
			
			planet_mesh[i].vertex_array.resize(vertex_array_size);
			planet_mesh[i].normal_array.resize(vertex_array_size);
			planet_mesh[i].index_array.resize(index_array_size);
			
			std::function<void(PatchTerrain &)> sample = [&sampler](PatchTerrain &terrain)
																		{sampler.Sample(terrain);};
			
			if(eliminate_backface)
				{ForeachLeaf(terrain.GetTerrain((PlanetTerrain::TerrainPosition)i),predicate,sample);}
			else
				{ForeachLeaf(terrain.GetTerrain((PlanetTerrain::TerrainPosition)i),sample);}
		}
		else if(sample_method == 2)
		{
			InPlaceAllocatingHeightmapSampler sampler(heightmap,planet_mesh[i]);
			
			planet_mesh[i].vertex_array.clear();
			planet_mesh[i].normal_array.clear();
			planet_mesh[i].index_array.clear();
			
			std::function<void(PatchTerrain &)> sample = [&sampler](PatchTerrain &terrain)
																		{sampler.Sample(terrain);};
			
			if(eliminate_backface)
				{ForeachLeaf(terrain.GetTerrain((PlanetTerrain::TerrainPosition)i),predicate,sample);}
			else
				{ForeachLeaf(terrain.GetTerrain((PlanetTerrain::TerrainPosition)i),sample);}
		}
		else
		{
			//DefaultHeightmapSampler sampler(heightmap,planet_mesh[i]);
			HeightmapSampler sampler(heightmap,planet_mesh[i]);
			
			planet_mesh[i].vertex_array.clear();
			planet_mesh[i].normal_array.clear();
			planet_mesh[i].index_array.clear();
			
			std::function<void(PatchTerrain &)> sample = [&sampler](PatchTerrain &terrain)
																		{sampler.Sample(terrain);};
			
			if(eliminate_backface)
				{ForeachLeaf(terrain.GetTerrain((PlanetTerrain::TerrainPosition)i),predicate,sample);}
			else
				{ForeachLeaf(terrain.GetTerrain((PlanetTerrain::TerrainPosition)i),sample);}
		}
	}
}

void PlanetDemo::MultiThreadedTriangulation()
{
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
	
	RippleHeightmap ripple_heightmap(20,{0.5,0.5});
	
	HeightmapSurface heightmap[6] =
	{
		HeightmapSurface(translate[0],sphere_patches[0],ripple_heightmap),
		HeightmapSurface(translate[1],sphere_patches[1],ripple_heightmap),
		HeightmapSurface(translate[2],sphere_patches[2],ripple_heightmap),
		HeightmapSurface(translate[3],sphere_patches[3],ripple_heightmap),
		HeightmapSurface(translate[4],sphere_patches[4],ripple_heightmap),
		HeightmapSurface(translate[5],sphere_patches[5],ripple_heightmap),
	};
	
	auto Before = std::chrono::steady_clock::now();
	
	for(int i=0;i < 6;i++)
	{
		job_partitioner.PartitionLeaves(terrain.GetTerrain((PlanetTerrain::TerrainPosition)i),heightmap[i]);
	}
	
	auto After = std::chrono::steady_clock::now();
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
	
	std::thread threads[4];
	threads[0] = std::thread(Worker::RunWorker,&firstWorker);
	threads[1] = std::thread(Worker::RunWorker,&secondWorker);
	threads[2] = std::thread(Worker::RunWorker,&thirdWorker);
	threads[3] = std::thread(Worker::RunWorker,&fourthWorker);
	
	for(int i=0;i < 4;i++)
	{
		threads[i].join();
	}
}

#define SINGLE_THREADED

void PlanetDemo::Logic()
{
	for(int i=0;i < 4;i++)
	{
		std::cout << "Sample method: " << i << " " << sample_time[i] << std::endl;
	}
	
	auto Before = std::chrono::steady_clock::now();
	dynamic_lod.CalculateLod();
	auto After = std::chrono::steady_clock::now();
	
	std::cout << "Lod time: " << std::chrono::duration<float,std::milli>(After - Before).count() << std::endl;
	
	Before = std::chrono::steady_clock::now();
	
#ifdef SINGLE_THREADED
	
	SingleThreadedTriangulation();
	
#else
	
	MultiThreadedTriangulation();
	
#endif
	
	After = std::chrono::steady_clock::now();
	
	std::cout << "Triangulate time: " << std::chrono::duration<float,std::milli>(After - Before).count() << std::endl;
	
	sample_time_acc[sample_method] += std::chrono::duration<float,std::milli>(After - Before).count();
	
	
	timer++;
	
	if(timer == 30)
	{
		if(sample_method == 3)
		{eliminate_backface = !eliminate_backface;}
		
		sample_time[sample_method] = sample_time_acc[sample_method] / 30.0;
		sample_time_acc[sample_method] = 0.0;
		sample_method = (sample_method + 1) % 4;
		timer = 0;
	}
}

void DrawSpherePatch(const std::array<vec4,4> &corners,float radius)
{
	glColor3f(0.0,0.0,0.0);
	
	for(int i=0;i < 4;i++)
	{
		glBegin(GL_LINE_STRIP);
		for(int j=0;j < 8;j++)
		{
			float param = j/7.0;
			vec4 result = slerp(corners[i],corners[(i+1) % 4],param);
			result = normalize(result) * radius;
			glVertex3f(result[0],result[1],result[2]);
		}
		glEnd();
	}
	
	glColor3f(1.0,0.0,0.0);
	
	glBegin(GL_POINTS);
	for(float x=0;x < 1.0;x += 1.0/10.0)
	{
		vec4 pointX1 = slerp(corners[0],corners[1],x);
		vec4 pointX2 = slerp(corners[3],corners[2],x);
		for(float y=0;y < 1.0;y += 1.0/10.0)
		{
			vec4 pointY = normalize(slerp(pointX1,pointX2,y)) * radius;
			glVertex3f(pointY[0],pointY[1],pointY[2]);
		}
	}
	glEnd();
}

void DrawSphereTest()
{
	std::array<vec4,4> vertices = {{{-1.0,1.0,-1.0},{1.0,1.0,-1.0},{1.0,1.0,1.0},{-1.0,1.0,1.0},}};
	DrawSpherePatch(vertices,100);
	
	vertices = {{{-1.0,1.0,-1.0},{1.0,1.0,-1.0},{1.0,-1.0,-1.0},{-1.0,-1.0,-1.0},}};
	DrawSpherePatch(vertices,100);
	
	vertices = {{{-1.0,-1.0,1.0},{1.0,-1.0,1.0},{1.0,-1.0,-1.0},{-1.0,-1.0,-1.0},}};
	DrawSpherePatch(vertices,100);
	
	vertices = {{{-1.0,1.0,1.0},{1.0,1.0,1.0},{1.0,-1.0,1.0},{-1.0,-1.0,1.0},}};
	DrawSpherePatch(vertices,100);
	
	vertices = {{{1.0,1.0,1.0},{1.0,-1.0,1.0},{1.0,-1.0,-1.0},{1.0,1.0,-1.0},}};
	DrawSpherePatch(vertices,100);
	
	vertices = {{{-1.0,1.0,1.0},{-1.0,-1.0,1.0},{-1.0,-1.0,-1.0},{-1.0,1.0,-1.0},}};
	DrawSpherePatch(vertices,100);
}

void PlanetDemo::Draw()
{
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	
	//DrawSphereTest();
	
	glColor3f(0.5,0.5,0.5);
	
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	
	/*
	for(int i=0;i < 6;i++)
	{
		ScaleHeightmap scale(spherePatches[i],testPlanet.GetRadius());
		TranslateHeightmap translate(scale,testPlanet.GetPosition());
		
		RippleHeightmap heightmap(translate,spherePatches[i],20,{0.5,0.5});
		NeighborTrackingRenderer sideDiscretizer(heightmap);
		std::function<void(PatchTerrain &)> drawWithNeighbors = [&sideDiscretizer](PatchTerrain &terrain)
																	{sideDiscretizer.Draw(terrain);};
		
		ForeachLeaf(terrain.GetTerrain((PlanetTerrain::TerrainPosition)i),drawWithNeighbors);
	}
	*/
	
#ifdef SINGLE_THREADED

	glBegin(GL_TRIANGLES);
	for(int i=0;i < 6;i++)
	{
		for(int j=0;j < planet_mesh[i].index_array.size();j++)
		{
			const vec4 &vertex = planet_mesh[i].vertex_array[planet_mesh[i].index_array[j]];
			glVertex3f(vertex[0],vertex[1],vertex[2]);
		}
	}
	glEnd();
	
#else
	
	DrawMeshes(job_partitioner.GetMeshes());
	
#endif
}
