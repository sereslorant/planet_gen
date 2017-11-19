
#include <iostream>

#include "LodTestUtils.h"

#include <PlanetLib/LevelOfDetail/LodOperations/LodNegationOp.h>

bool NegationSuccessful(bool param)
{
	LodLeafVariable leaf = {param};
	PatchTerrain dummy_terrain({{{},{},{},{}}});
	
	LodNegationOp negation(leaf);
	
	bool SubdivideResult = negation.Subdivide(dummy_terrain);
	bool ExpectedResult = !param;
	
	std::cout << "Parameters: " << param << std::endl;
	std::cout << "Expected: " << ExpectedResult << std::endl;
	std::cout << "Result: " << SubdivideResult << std::endl;
	
	return SubdivideResult == ExpectedResult;
}

int main(int argc,char *argv[])
{
	if(NegationSuccessful(true) && NegationSuccessful(false))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
