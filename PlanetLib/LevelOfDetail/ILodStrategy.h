#ifndef ILOD_STRATEGY_H
#define ILOD_STRATEGY_H

#include <PlanetLib/PatchTerrain/PatchTerrain.h>

class ILodStrategy
{
public:
	virtual bool Subdivide(const PatchTerrain &terrain) const = 0;
	
	ILodStrategy()
	{}
	
	virtual ~ILodStrategy()
	{}
};

#endif // ILOD_STRATEGY_H
