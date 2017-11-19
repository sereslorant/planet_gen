
#include <iostream>

#include "LodTestUtils.h"

#include <PlanetLib/LevelOfDetail/LodOperations/LodDisjunctionOp.h>

bool DisjunctionSuccessful(bool param1,bool param2)
{
	LodLeafVariable leaf1 = {param1};
	LodLeafVariable leaf2 = {param2};
	PatchTerrain dummy_terrain({{{},{},{},{}}});
	
	LodDisjunctionOp disjunction(leaf1,leaf2);
	
	bool SubdivideResult = disjunction.Subdivide(dummy_terrain);
	bool ExpectedResult = param1 || param2;
	
	std::cout << "Parameters: " << param1 << "," << param2 << std::endl;
	std::cout << "Expected: " << ExpectedResult << std::endl;
	std::cout << "Result: " << SubdivideResult << std::endl;
	
	return SubdivideResult == ExpectedResult;
}

int main(int argc,char *argv[])
{
	if(	DisjunctionSuccessful(true,true) && DisjunctionSuccessful(true,false) &&
		DisjunctionSuccessful(false,true) && DisjunctionSuccessful(false,false)
	  )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
