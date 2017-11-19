#ifndef PATCH_TERRAIN_EDGE_H
#define PATCH_TERRAIN_EDGE_H

class PatchTerrain;

class PatchTerrainEdge
{
private:
	PatchTerrain *terrain = nullptr;
	unsigned int left_id;
	unsigned int right_id;
	
public:
	
	bool IsEmpty() const
	{
		return terrain == nullptr;
	}
	
	bool IsLeft() const;
	
	bool IsRight() const;
	
	bool TerrainSplit() const;
	
	PatchTerrain *GetTerrain() const
	{
		return terrain;
	}
	
	PatchTerrainEdge GetParent() const;
	
	PatchTerrainEdge GetLeft() const;
	
	PatchTerrainEdge GetRight() const;
	
	PatchTerrainEdge() = default;
	
	PatchTerrainEdge(PatchTerrain *p_terrain,unsigned int p_left_id,unsigned int p_right_id)
		:terrain(p_terrain),left_id(p_left_id),right_id(p_right_id)
	{}
};

#endif // PATCH_TERRAIN_EDGE_H
