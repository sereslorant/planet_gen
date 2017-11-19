
#include "NeighborWrapper.h"

#include "PatchTerrain.h"

PatchTerrainEdge NeighborWrapper::FindNeighbor(const PatchTerrainEdge &parent_edge)
{
	PatchTerrain *current_neighbor = neighboring_edge.GetTerrain();
	
	if(current_neighbor->GetDepth() + 1 < parent_edge.GetTerrain()->GetDepth())
	{
		PatchTerrainEdge parentNeighborEdge = FindNeighbor(parent_edge.GetParent());
		
		if(parentNeighborEdge.TerrainSplit())
		{
			if(parent_edge.IsLeft())
			{
				return parentNeighborEdge.GetRight();
			}
			else
			{
				return parentNeighborEdge.GetLeft();
			}
		}
		
		return parentNeighborEdge;
	}
	else
	{
		if(current_neighbor->IsSplit())
		{
			if(parent_edge.IsLeft())
			{
				return neighboring_edge.GetRight();
			}
			else
			{
				return neighboring_edge.GetLeft();
			}
		}
		
		return neighboring_edge;
	}
}

PatchTerrain *NeighborWrapper::GetNeighbor()
{
	if(neighboring_edge.GetTerrain()->GetDepth() < owner_edge.GetTerrain()->GetDepth())
	{
		neighboring_edge = FindNeighbor(owner_edge);
	}
	
	return neighboring_edge.GetTerrain();
}
