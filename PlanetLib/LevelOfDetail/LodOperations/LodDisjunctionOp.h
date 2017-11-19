#ifndef LOD_DISJUNCTION_OP_H
#define LOD_DISJUNCTION_OP_H

#include <PlanetLib/LevelOfDetail/ILodStrategy.h>

class LodDisjunctionOp : public ILodStrategy
{
private:
	const ILodStrategy &operand1;
	const ILodStrategy &operand2;
	
public:
	
	virtual bool Subdivide(const PatchTerrain &terrain) const override
	{
		return operand1.Subdivide(terrain) || operand2.Subdivide((terrain));
	}
	
	LodDisjunctionOp(const ILodStrategy &p_operand1,const ILodStrategy &p_operand2)
		:operand1(p_operand1),operand2(p_operand2)
	{}
	
	virtual ~LodDisjunctionOp() override
	{}
};

#endif // LOD_DISJUNCTION_OP_H
