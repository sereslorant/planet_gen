
#include "PatchTerrainEdge.h"

#include "PatchTerrain.h"


bool PatchTerrainEdge::IsLeft() const
{
	return terrain->parent->GetChild(left_id) == terrain;
}

bool PatchTerrainEdge::IsRight() const
{
	return terrain->parent->GetChild(right_id) == terrain;
}

bool PatchTerrainEdge::TerrainSplit() const
{
	return terrain->IsSplit();
}

PatchTerrainEdge PatchTerrainEdge::GetParent() const
{
	return {terrain->parent,left_id,right_id};
}

PatchTerrainEdge PatchTerrainEdge::GetLeft() const
{
	return {terrain->GetChild(left_id),left_id,right_id};
}

PatchTerrainEdge PatchTerrainEdge::GetRight() const
{
	return {terrain->GetChild(right_id),left_id,right_id};
}

