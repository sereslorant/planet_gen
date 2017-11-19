
#include <PlanetLib/PatchTerrain/PatchTerrain.h>

struct TerrainMergeTest
{
	PatchTerrain test_terrain;
	
	bool PreconditionsApply()
	{
		bool is_split_true = test_terrain.IsSplit() == true;
		
		bool children_not_null = true;
		for(PatchTerrain *child : test_terrain.GetChildren())
		{
			if(child == nullptr)
			{children_not_null = false;}
		}
		
		return is_split_true && children_not_null;
	}
	
	bool PostconditionsApply()
	{
		bool is_split_false = test_terrain.IsSplit() == false;
		
		bool children_null = true;
		for(PatchTerrain *child : test_terrain.GetChildren())
		{
			if(child != nullptr)
			{children_null = false;}
		}
		
		return is_split_false && children_null;
	}
	
	TerrainMergeTest()
		:test_terrain(std::array<vec4,4>{{{},{},{},{}}})
	{
		test_terrain.Split();
	}
};

int main(int argc,char *argv[])
{
	TerrainMergeTest merge_test;
	
	bool preconditions_apply = merge_test.PreconditionsApply();
	
	merge_test.test_terrain.Merge();
	
	bool postconditions_apply = merge_test.PostconditionsApply();
	
	if(preconditions_apply && postconditions_apply)
	{
		return 0;
	}
	
	return 1;
}
