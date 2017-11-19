
#include "PlanetTerrain.h"

const std::array<vec4,4> PlanetTerrain::terrainParameters = {{{0.0,0.0},{1.0,0.0},{1.0,1.0},{0.0,1.0}}};

PlanetTerrain::PlanetTerrain()
{
	terrains[NEAR].SetNeighbor(PatchTerrain::UP,	terrains[TOP].GetEdge(PatchTerrain::DOWN));
	terrains[NEAR].SetNeighbor(PatchTerrain::DOWN,	terrains[BOTTOM].GetEdge(PatchTerrain::UP));
	terrains[NEAR].SetNeighbor(PatchTerrain::LEFT,	terrains[LEFT].GetEdge(PatchTerrain::RIGHT));
	terrains[NEAR].SetNeighbor(PatchTerrain::RIGHT,	terrains[RIGHT].GetEdge(PatchTerrain::LEFT));
	
	terrains[TOP].SetNeighbor(PatchTerrain::UP,		terrains[FAR].GetEdge(PatchTerrain::DOWN));
	terrains[TOP].SetNeighbor(PatchTerrain::DOWN,	terrains[NEAR].GetEdge(PatchTerrain::UP));
	terrains[TOP].SetNeighbor(PatchTerrain::LEFT,	terrains[LEFT].GetEdge(PatchTerrain::UP));
	terrains[TOP].SetNeighbor(PatchTerrain::RIGHT,	terrains[RIGHT].GetEdge(PatchTerrain::UP));
	
	terrains[FAR].SetNeighbor(PatchTerrain::UP,		terrains[BOTTOM].GetEdge(PatchTerrain::DOWN));
	terrains[FAR].SetNeighbor(PatchTerrain::DOWN,	terrains[TOP].GetEdge(PatchTerrain::UP));
	terrains[FAR].SetNeighbor(PatchTerrain::LEFT,	terrains[LEFT].GetEdge(PatchTerrain::LEFT));
	terrains[FAR].SetNeighbor(PatchTerrain::RIGHT,	terrains[RIGHT].GetEdge(PatchTerrain::RIGHT));
	
	terrains[LEFT].SetNeighbor(PatchTerrain::UP,	terrains[TOP].GetEdge(PatchTerrain::LEFT));
	terrains[LEFT].SetNeighbor(PatchTerrain::DOWN,	terrains[BOTTOM].GetEdge(PatchTerrain::LEFT));
	terrains[LEFT].SetNeighbor(PatchTerrain::LEFT,	terrains[FAR].GetEdge(PatchTerrain::LEFT));
	terrains[LEFT].SetNeighbor(PatchTerrain::RIGHT,	terrains[NEAR].GetEdge(PatchTerrain::LEFT));
	
	terrains[RIGHT].SetNeighbor(PatchTerrain::UP,	terrains[TOP].GetEdge(PatchTerrain::RIGHT));
	terrains[RIGHT].SetNeighbor(PatchTerrain::DOWN,	terrains[BOTTOM].GetEdge(PatchTerrain::RIGHT));
	terrains[RIGHT].SetNeighbor(PatchTerrain::LEFT,	terrains[NEAR].GetEdge(PatchTerrain::RIGHT));
	terrains[RIGHT].SetNeighbor(PatchTerrain::RIGHT,terrains[FAR].GetEdge(PatchTerrain::RIGHT));
	
	terrains[BOTTOM].SetNeighbor(PatchTerrain::UP,		terrains[NEAR].GetEdge(PatchTerrain::DOWN));
	terrains[BOTTOM].SetNeighbor(PatchTerrain::DOWN,	terrains[FAR].GetEdge(PatchTerrain::UP));
	terrains[BOTTOM].SetNeighbor(PatchTerrain::LEFT,	terrains[LEFT].GetEdge(PatchTerrain::DOWN));
	terrains[BOTTOM].SetNeighbor(PatchTerrain::RIGHT,	terrains[RIGHT].GetEdge(PatchTerrain::DOWN));
}
