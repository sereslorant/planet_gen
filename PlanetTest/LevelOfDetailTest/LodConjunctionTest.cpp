
#include <iostream>

#include "LodTestUtils.h"

#include <PlanetLib/LevelOfDetail/LodOperations/LodConjunctionOp.h>

bool ConjunctionSuccessful(bool param1,bool param2)
{
	LodLeafVariable leaf1 = {param1};
	LodLeafVariable leaf2 = {param2};
	PatchTerrain dummy_terrain({{{},{},{},{}}});
	
	LodConjunctionOp conjunction(leaf1,leaf2);
	
	bool SubdivideResult = conjunction.Subdivide(dummy_terrain);
	bool ExpectedResult = param1 && param2;
	
	std::cout << "Parameters: " << param1 << "," << param2 << std::endl;
	std::cout << "Expected: " << ExpectedResult << std::endl;
	std::cout << "Result: " << SubdivideResult << std::endl;
	
	return SubdivideResult == ExpectedResult;
}

int main(int argc,char *argv[])
{
	if(	ConjunctionSuccessful(true,true) && ConjunctionSuccessful(true,false) &&
		ConjunctionSuccessful(false,true) && ConjunctionSuccessful(false,false)
	  )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
