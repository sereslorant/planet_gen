#ifndef HEIGHTMAP_SAMPLER_H
#define HEIGHTMAP_SAMPLER_H

#include <PlanetLib/MeshGenerator/LeafGenerator.h>

#include <PlanetLib/Heightmap/IParametricSurface.h>

#include <vector>

struct StaticMesh
{
	std::vector<vec4> vertex_array;
	std::vector<vec4> normal_array;
	std::vector<unsigned int> index_array;
};

class HeightmapSampler
{
protected:
	const IParametricSurface &surface;
	StaticMesh &static_mesh;
	
	void SampleHeightmap(vec4 *vertices,vec4 *normals,unsigned int array_size)
	{
		for(int i=0;i < array_size;i++)
		{
			normals[i] = surface.GetNormal(vertices[i]);
			vertices[i] = surface.GetValue(vertices[i]);
		}
	}
	
	void OffsetVertices(unsigned int *array,unsigned int array_size,unsigned int offset)
	{
		for(int i=0;i < array_size;i++)
			{array[i] += offset;}
	}
	
public:
	
	virtual void Sample(PatchTerrain &patch_terrain)
	{
		LeafGenerator leaf_vertices(patch_terrain);
		
		unsigned int last_vert_index = static_mesh.vertex_array.size();
		unsigned int last_index = static_mesh.index_array.size();
		
		static_mesh.vertex_array.resize(static_mesh.vertex_array.size() + leaf_vertices.GetVertexArraySize());
		static_mesh.normal_array.resize(static_mesh.normal_array.size() + leaf_vertices.GetVertexArraySize());
		static_mesh.index_array.resize(static_mesh.index_array.size() + leaf_vertices.GetIndexArraySize());
		
		leaf_vertices.CopyContent(&static_mesh.vertex_array[last_vert_index],&static_mesh.index_array[last_index]);
		
		SampleHeightmap(&static_mesh.vertex_array[last_vert_index],&static_mesh.normal_array[last_vert_index],leaf_vertices.GetVertexArraySize());
		OffsetVertices(&static_mesh.index_array[last_index],leaf_vertices.GetIndexArraySize(),last_vert_index);
	}
	
	HeightmapSampler(const IParametricSurface &p_heightmap,StaticMesh &p_static_mesh)
		:surface(p_heightmap),static_mesh(p_static_mesh)
	{}
	
	virtual ~HeightmapSampler()
	{}
};
/*
class DefaultHeightmapSampler : public HeightmapSampler
{
public:
	
	virtual void Sample(PatchTerrain &patch_terrain) override
	{
		LeafGenerator leaf_vertices(patch_terrain);
		
		unsigned int last_vert_index = static_mesh.vertex_array.size();
		unsigned int last_index = static_mesh.index_array.size();
		
		static_mesh.vertex_array.resize(static_mesh.vertex_array.size() + leaf_vertices.GetVertexArraySize());
		static_mesh.index_array.resize(static_mesh.index_array.size() + leaf_vertices.GetIndexArraySize());
		
		leaf_vertices.CopyContent(&static_mesh.vertex_array[last_vert_index],&static_mesh.index_array[last_index]);
		
		SampleHeightmap(&static_mesh.vertex_array[last_vert_index],leaf_vertices.GetVertexArraySize());
		OffsetVertices(&static_mesh.index_array[last_index],leaf_vertices.GetIndexArraySize(),last_vert_index);
	}
	
	DefaultHeightmapSampler(const IParametricSurface &p_heightmap,StaticMesh &p_static_mesh)
		:HeightmapSampler(p_heightmap,p_static_mesh)
	{}
	
	virtual ~DefaultHeightmapSampler() override
	{}
};
*/
#endif // HEIGHTMAP_SAMPLER_H
