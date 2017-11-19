#ifndef PLANET_TERRAIN_H
#define PLANET_TERRAIN_H

#include "PatchTerrain/PatchTerrain.h"

class PlanetTerrain
{
public:
	
	enum TerrainPosition
	{
		TOP,
		BOTTOM,
		LEFT,
		RIGHT,
		FAR,
		NEAR,
	};
	
private:
	
	const static std::array<vec4,4> terrainParameters;
	
	PatchTerrain terrains[6] =
	{
		{{terrainParameters}},
		{{terrainParameters}},
		{{terrainParameters}},
		{{terrainParameters}},
		{{terrainParameters}},
		{{terrainParameters}},
	};
	
public:
	
	PatchTerrain &GetTerrain(TerrainPosition position)
	{
		return terrains[position];
	}
	
	std::array<PatchTerrain *,6>GetTerrains()
	{
		return {{&terrains[0],&terrains[1],&terrains[2],&terrains[3],&terrains[4],&terrains[5]}};
	}
	
	PlanetTerrain();
};

#endif // PLANET_TERRAIN_H
