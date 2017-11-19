#ifndef LEAF_GENERATOR_H
#define LEAF_GENERATOR_H

#include <array>

#include <PlanetLib/PatchTerrain/PatchTerrain.h>

class NeighborInformation
{
private:
	bool scale_down[4];
	
public:
	
	bool ScaleDown(unsigned int i) const
	{
		return scale_down[i];
	}
	
	NeighborInformation(PatchTerrain &terrain)
	{
		for(int i=0;i < 4;i++)
		{
			if(terrain.GetNeighbor(i) != nullptr)
				{scale_down[i] = terrain.GetNeighbor(i)->GetDepth() < terrain.GetDepth();}
		}
	}
};

class LeafSize
{
private:
	unsigned int vertex_array_size = 9;
	unsigned int index_array_size = 0;
	
public:
	
	unsigned int GetVertexArraySize() const
	{
		return vertex_array_size;
	}
	
	unsigned int GetIndexArraySize() const
	{
		return index_array_size;
	}
	
	LeafSize(const NeighborInformation &neighbor_information)
	{
		for(int i=0;i < 4;i++)
		{
			if(neighbor_information.ScaleDown(i))
			{
				index_array_size += 3;
			}
			else
			{
				index_array_size += 6;
			}
		}
	}
};

class LeafGenerator
{
private:
	std::array<vec4,9> vertex_array;
	
	unsigned int index_array_size = 0;
	std::array<unsigned int,24> index_array;
	
public:
	
	unsigned int GetVertexArraySize() const
	{
		return 9;
	}
	
	const std::array<vec4,9> &GetVertexArray() const
	{
		return vertex_array;
	}
	
	unsigned int GetIndexArraySize() const
	{
		return index_array_size;
	}
	
	const std::array<unsigned int,24> &GetIndexArray() const
	{
		return index_array;
	}
	
	void CopyContent(vec4 *p_vertex_array,unsigned int *p_index_array) const;
	
	LeafGenerator(PatchTerrain &patchTerrain);
};

#endif // LEAF_GENERATOR_H
