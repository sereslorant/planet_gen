#ifndef LOD_TEST_UTILS_H
#define LOD_TEST_UTILS_H

#include <PlanetLib/LevelOfDetail/ILodStrategy.h>

struct LodLeafVariable : public ILodStrategy
{
	bool value;
	
	virtual bool Subdivide(const PatchTerrain &terrain) const override
	{
		return value;
	}
	
	LodLeafVariable(bool p_value)
		:value(p_value)
	{}
	
	virtual ~LodLeafVariable() override
	{}
};

#endif // LOD_TEST_UTILS_H
