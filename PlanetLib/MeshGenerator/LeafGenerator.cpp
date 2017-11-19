
#include "LeafGenerator.h"


void LeafGenerator::CopyContent(vec4 *p_vertex_array,unsigned int *p_index_array) const
{
	for(int i=0;i < GetVertexArraySize();i++)
	{
		p_vertex_array[i] = vertex_array[i];
	}
	
	for(int i=0;i < GetIndexArraySize();i++)
	{
		p_index_array[i] = index_array[i];
	}
}

LeafGenerator::LeafGenerator(PatchTerrain &patchTerrain)
{
	const std::array<vec4,4> &vertices = patchTerrain.GetVertices();
	
	vertex_array =
		{{
			vertices[0],
			vertices[1],
			vertices[2],
			vertices[3],
			(vertices[0] + vertices[1] + vertices[2] + vertices[3]) / 4.0,
			(vertices[0] + vertices[1]) / 2.0,
			(vertices[1] + vertices[2]) / 2.0,
			(vertices[2] + vertices[3]) / 2.0,
			(vertices[3] + vertices[0]) / 2.0,
		}};
	
	unsigned int blocks[4][4]
	{
		{0,5,4,1},
		{2,7,4,3},
		{3,8,4,0},
		{1,6,4,2},
	};
	
	NeighborInformation neighborInformation(patchTerrain);
	for(int i=0;i < 4;i++)
	{
		if(neighborInformation.ScaleDown(i))
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
