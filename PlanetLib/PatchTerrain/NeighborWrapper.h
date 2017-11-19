#ifndef NEIGHBOR_WRAPPER_H
#define NEIGHBOR_WRAPPER_H

#include "PatchTerrainEdge.h"

class PatchTerrain;

class NeighborWrapper
{
private:
	const PatchTerrainEdge &owner_edge;
	PatchTerrainEdge neighboring_edge;
	
	PatchTerrainEdge FindNeighbor(const PatchTerrainEdge &parent_edge);
	
public:
	
	bool HasNeighbor() const
	{
		return !neighboring_edge.IsEmpty();
	}
	
	const PatchTerrain *GetNeighbor() const
	{
		return neighboring_edge.GetTerrain();
	}
	
	PatchTerrain *GetNeighbor();
	
	const PatchTerrainEdge &GetOwnerEdge()
	{
		return owner_edge;
	}
	
	const PatchTerrainEdge &GetNeighboringEdge()
	{
		return neighboring_edge;
	}
	
	void SetNeighbor(const PatchTerrainEdge &new_neighbor_edge)
	{
		neighboring_edge = new_neighbor_edge;
	}
	
	NeighborWrapper(const PatchTerrainEdge &p_owner_edge)
		:owner_edge(p_owner_edge)
	{}
};

#endif // NEIGHBOR_WRAPPER_H
