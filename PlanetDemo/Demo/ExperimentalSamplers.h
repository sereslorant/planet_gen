#ifndef EXPERIMENTAL_SAMPLERS_H
#define EXPERIMENTAL_SAMPLERS_H

#include <PlanetLib/MeshGenerator/HeightmapSampler.h>

class InPlaceLeafGenerator
{
private:
	unsigned int vertex_array_size = 9;
	vec4 *vertex_array;
	
	unsigned int index_array_size = 0;
	unsigned int *index_array;
	
public:
	
	unsigned int GetVertexArraySize() const
	{
		return vertex_array_size;
	}
	
	const vec4 *GetVertexArray() const
	{
		return vertex_array;
	}
	
	unsigned int GetIndexArraySize() const
	{
		return index_array_size;
	}
	
	const unsigned int *GetIndexArray() const
	{
		return index_array;
	}
	
	InPlaceLeafGenerator(vec4 *p_vertex_array,unsigned int *p_index_array,PatchTerrain &patchTerrain)
		:vertex_array(p_vertex_array),index_array(p_index_array)
	{
		const std::array<vec4,4> &vertices = patchTerrain.GetVertices();
		
		vertex_array[0] = vertices[0];
		vertex_array[1] = vertices[1];
		vertex_array[2] = vertices[2];
		vertex_array[3] = vertices[3];
		vertex_array[4] = (vertices[0] + vertices[1] + vertices[2] + vertices[3]) / 4.0;
		vertex_array[5] = (vertices[0] + vertices[1]) / 2.0;
		vertex_array[6] = (vertices[1] + vertices[2]) / 2.0;
		vertex_array[7] = (vertices[2] + vertices[3]) / 2.0;
		vertex_array[8] = (vertices[3] + vertices[0]) / 2.0;
		
		unsigned int blocks[4][4]
		{
			{0,5,4,1},
			{2,7,4,3},
			{3,8,4,0},
			{1,6,4,2},
		};
		
		NeighborInformation neighbor_information(patchTerrain);
		for(int i=0;i < 4;i++)
		{
			if(neighbor_information.ScaleDown(i))
			{
				index_array[index_array_size] = blocks[i][0];
				index_array_size++;
				index_array[index_array_size] = blocks[i][2];
				index_array_size++;
				index_array[index_array_size] = blocks[i][3];
				index_array_size++;
			}
			else
			{
				index_array[index_array_size] = blocks[i][0];
				index_array_size++;
				index_array[index_array_size] = blocks[i][2];
				index_array_size++;
				index_array[index_array_size] = blocks[i][1];
				index_array_size++;
				
				index_array[index_array_size] = blocks[i][2];
				index_array_size++;
				index_array[index_array_size] = blocks[i][3];
				index_array_size++;
				index_array[index_array_size] = blocks[i][1];
				index_array_size++;
			}
		}
	}
};

class ArrayHeightmapSampler : public HeightmapSampler
{
private:
	unsigned int last_vert_index = 0;
	unsigned int last_index = 0;
	
public:
	
	virtual void Sample(PatchTerrain &patch_terrain) override
	{
		LeafGenerator leaf_vertices(patch_terrain);
		leaf_vertices.CopyContent(&static_mesh.vertex_array[last_vert_index],&static_mesh.index_array[last_index]);
		
		SampleHeightmap(&static_mesh.vertex_array[last_vert_index],&static_mesh.normal_array[last_vert_index],leaf_vertices.GetVertexArraySize());
		OffsetVertices(&static_mesh.index_array[last_index],leaf_vertices.GetIndexArraySize(),last_vert_index);
		
		last_vert_index	+= leaf_vertices.GetVertexArraySize();
		last_index		+= leaf_vertices.GetIndexArraySize();
	}
	
	ArrayHeightmapSampler(const IParametricSurface &p_heightmap,StaticMesh &p_static_mesh)
		:HeightmapSampler(p_heightmap,p_static_mesh)
	{}
};

class InPlaceHeightmapSampler : public HeightmapSampler
{
private:
	unsigned int last_vert_index = 0;
	unsigned int last_index = 0;
	
public:
	
	virtual void Sample(PatchTerrain &patch_terrain) override
	{
		InPlaceLeafGenerator leaf_vertices(&static_mesh.vertex_array[last_vert_index],&static_mesh.index_array[last_index],patch_terrain);
		
		SampleHeightmap(&static_mesh.vertex_array[last_vert_index],&static_mesh.normal_array[last_vert_index],leaf_vertices.GetVertexArraySize());
		OffsetVertices(&static_mesh.index_array[last_index],leaf_vertices.GetIndexArraySize(),last_vert_index);
		
		last_vert_index	+= leaf_vertices.GetVertexArraySize();
		last_index		+= leaf_vertices.GetIndexArraySize();
	}
	
	InPlaceHeightmapSampler(const IParametricSurface &p_heightmap,StaticMesh &p_static_mesh)
		:HeightmapSampler(p_heightmap,p_static_mesh)
	{}
};

class InPlaceAllocatingHeightmapSampler : public HeightmapSampler
{
public:
	
	virtual void Sample(PatchTerrain &patch_terrain) override
	{
		unsigned int last_vert_index = static_mesh.vertex_array.size();
		unsigned int last_index = static_mesh.index_array.size();
		
		LeafSize leaf_size(patch_terrain);
		
		static_mesh.vertex_array.resize(static_mesh.vertex_array.size() + leaf_size.GetVertexArraySize());
		static_mesh.normal_array.resize(static_mesh.normal_array.size() + leaf_size.GetVertexArraySize());
		static_mesh.index_array.resize(static_mesh.index_array.size() + leaf_size.GetIndexArraySize());
		
		InPlaceLeafGenerator leaf_vertices(&static_mesh.vertex_array[last_vert_index],&static_mesh.index_array[last_index],patch_terrain);
		
		SampleHeightmap(&static_mesh.vertex_array[last_vert_index],&static_mesh.normal_array[last_vert_index],leaf_vertices.GetVertexArraySize());
		OffsetVertices(&static_mesh.index_array[last_index],leaf_vertices.GetIndexArraySize(),last_vert_index);
	}
	
	InPlaceAllocatingHeightmapSampler(const IParametricSurface &p_heightmap,StaticMesh &p_static_mesh)
		:HeightmapSampler(p_heightmap,p_static_mesh)
	{}
};

#endif // EXPERIMENTAL_SAMPLERS_H
