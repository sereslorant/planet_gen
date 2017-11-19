#ifndef LOD_NEGATION_OP_H
#define LOD_NEGATION_OP_H

#include <PlanetLib/LevelOfDetail/ILodStrategy.h>

class LodNegationOp : public ILodStrategy
{
private:
	const ILodStrategy &operand;
	
public:
	
	virtual bool Subdivide(const PatchTerrain &terrain) const override
	{
		return !operand.Subdivide(terrain);
	}
	
	LodNegationOp(const ILodStrategy &p_operand)
		:operand(p_operand)
	{}
	
	virtual ~LodNegationOp() override
	{}
};

#endif // LOD_NEGATION_OP_H
