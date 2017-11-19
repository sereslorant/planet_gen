#ifndef TERRAIN_DEMO_H
#define TERRAIN_DEMO_H

#include <iostream>

#include <PlanetDemo/Demo/FFGLDemoBase.h>
#include <PlanetDemo/Demo/DemoUtils.h>

#include <PlanetLib/Heightmap/IHeightmap.h>
#include <PlanetLib/PatchTerrain/PatchTerrain.h>

class TerrainDemo : public FFGLDemoBase
{
private:
	ConstHeightmap test_heightmap1;
	WaveHeightmap test_heightmap2;
	
	PatchTerrain test_terrain1;
	PatchTerrain test_terrain2;
	PatchTerrain test_terrain3;
	PatchTerrain test_terrain4;
	PatchTerrain test_terrain5;
	
public:
	
	virtual void Logic() override;
	
	virtual void Draw() override;
	
	TerrainDemo()
		:test_heightmap2(vec4(1000,0)),
		test_terrain1(std::array<vec4,4>{{vec4(-400.0,-400.0),vec4(-400.0,400.0),vec4(400.0,400.0),vec4(400.0,-400.0)}}),
		test_terrain2(std::array<vec4,4>{{vec4(600.0,-400.0),vec4(1400.0,-400.0),vec4(1400.0,400.0),vec4(600.0,400.0)}}),
		test_terrain3(std::array<vec4,4>{{vec4(600.0,-400.0),vec4(1400.0,-400.0),vec4(1400.0,400.0),vec4(600.0,400.0)}}),
		test_terrain4(std::array<vec4,4>{{vec4(600.0,-400.0),vec4(1400.0,-400.0),vec4(1400.0,400.0),vec4(600.0,400.0)}}),
		test_terrain5(std::array<vec4,4>{{vec4(-400.0,-400.0),vec4(400.0,-400.0),vec4(400.0,400.0),vec4(-400.0,400.0)}})
	{
		test_terrain1.Split();
		
		test_terrain1.GetChildren()[0]->Split();
		//testTerrain1.GetChildren()[0]->Merge();
		
		test_terrain2.Split();
		for(PatchTerrain *child1 : test_terrain2.GetChildren())
		{
			child1->Split();
			
			for(PatchTerrain *child2 : child1->GetChildren())
			{
				child2->Split();
			
				for(PatchTerrain *child3 : child2->GetChildren())
				{
					child3->Split();
					
					for(PatchTerrain *child4 : child3->GetChildren())
					{
						child4->Split();
						
						for(PatchTerrain *child5 : child4->GetChildren())
						{
							child5->Split();
						}
					}
				}
			}
		}
		
		test_terrain3.Split();
		
		for(PatchTerrain *child1 : test_terrain3.GetChildren())
		{
			child1->Split();
			
			/*child1->GetChildren()[3]->Split();
			
			child1->GetChildren()[3]->GetChildren()[1]->Split();
			
			for(PatchTerrain *child2 : child1->GetChildren()[3]->GetChildren()[1]->GetChildren())
			{
				child2->Split();
				
				child2->GetChildren()[0]->Split();
				
				for(PatchTerrain *child3 : child2->GetChildren()[0]->GetChildren())
				{
					child3->Split();
				}
				
				for(PatchTerrain *child3 : child2->GetChildren())
				{
					child3->Split();
				}
			}*/
			
		}
		
		test_terrain3.GetChild(PatchTerrain::TOP_LEFT)->GetChild(PatchTerrain::BOTTOM_RIGHT)->Split();
		test_terrain3.GetChild(PatchTerrain::TOP_RIGHT)->GetChild(PatchTerrain::BOTTOM_LEFT)->Split();
		test_terrain3.GetChild(PatchTerrain::BOTTOM_RIGHT)->GetChild(PatchTerrain::TOP_LEFT)->Split();
		test_terrain3.GetChild(PatchTerrain::BOTTOM_LEFT)->GetChild(PatchTerrain::TOP_RIGHT)->Split();
		
		test_terrain3.GetChild(PatchTerrain::TOP_LEFT)->GetChild(PatchTerrain::BOTTOM_RIGHT)->GetChild(PatchTerrain::BOTTOM_RIGHT)->Split();
		for(PatchTerrain *child3 : test_terrain3.GetChild(PatchTerrain::TOP_LEFT)->GetChild(PatchTerrain::BOTTOM_RIGHT)->GetChild(PatchTerrain::BOTTOM_RIGHT)->GetChildren())
		{
			child3->Split();
			
			for(PatchTerrain *child4 : child3->GetChildren())
			{child4->Split();}
		}
		test_terrain3.GetChild(PatchTerrain::TOP_RIGHT)->GetChild(PatchTerrain::BOTTOM_LEFT)->GetChild(PatchTerrain::BOTTOM_LEFT)->Split();
		for(PatchTerrain *child3 : test_terrain3.GetChild(PatchTerrain::TOP_RIGHT)->GetChild(PatchTerrain::BOTTOM_LEFT)->GetChild(PatchTerrain::BOTTOM_LEFT)->GetChildren())
		{
			child3->Split();
			
			for(PatchTerrain *child4 : child3->GetChildren())
			{child4->Split();}
		}
		test_terrain3.GetChild(PatchTerrain::BOTTOM_RIGHT)->GetChild(PatchTerrain::TOP_LEFT)->GetChild(PatchTerrain::TOP_LEFT)->Split();
		for(PatchTerrain *child3 : test_terrain3.GetChild(PatchTerrain::BOTTOM_RIGHT)->GetChild(PatchTerrain::TOP_LEFT)->GetChild(PatchTerrain::TOP_LEFT)->GetChildren())
		{
			child3->Split();
			
			for(PatchTerrain *child4 : child3->GetChildren())
			{child4->Split();}
		}
		test_terrain3.GetChild(PatchTerrain::BOTTOM_LEFT)->GetChild(PatchTerrain::TOP_RIGHT)->GetChild(PatchTerrain::TOP_RIGHT)->Split();
		for(PatchTerrain *child3 : test_terrain3.GetChild(PatchTerrain::BOTTOM_LEFT)->GetChild(PatchTerrain::TOP_RIGHT)->GetChild(PatchTerrain::TOP_RIGHT)->GetChildren())
		{
			child3->Split();
			
			for(PatchTerrain *child4 : child3->GetChildren())
			{child4->Split();}
		}
		
		
		test_terrain4.Split();
		
		//testTerrain4.GetChild(PatchTerrain::TOP_LEFT)->Disable();
		
		test_terrain4.GetChild(PatchTerrain::BOTTOM_RIGHT)->Split();
		for(PatchTerrain *child : test_terrain4.GetChild(PatchTerrain::BOTTOM_RIGHT)->GetChildren())
			{child->Split();}
		
		test_terrain4.GetChild(PatchTerrain::BOTTOM_LEFT)->Split();
		for(PatchTerrain *child : test_terrain4.GetChild(PatchTerrain::BOTTOM_LEFT)->GetChildren())
			{child->Split();}
		
		test_terrain5.Split();
		test_terrain5.GetChild(PatchTerrain::TOP_LEFT)->Split();
		for(PatchTerrain *child : test_terrain5.GetChild(PatchTerrain::TOP_LEFT)->GetChildren())
		{
			child->Split();
		}
		
		test_terrain5.GetChild(PatchTerrain::TOP_LEFT)->GetChild(PatchTerrain::BOTTOM_RIGHT)->GetChild(PatchTerrain::BOTTOM_RIGHT)->Split();
		test_terrain5.GetChild(PatchTerrain::TOP_LEFT)->GetChild(PatchTerrain::BOTTOM_RIGHT)->GetChild(PatchTerrain::BOTTOM_RIGHT)->GetChild(PatchTerrain::BOTTOM_RIGHT)->Split();
		
		test_terrain5.GetChild(PatchTerrain::BOTTOM_LEFT)->GetChild(PatchTerrain::TOP_RIGHT)->GetChild(PatchTerrain::TOP_RIGHT)->Split();
		test_terrain5.GetChild(PatchTerrain::BOTTOM_LEFT)->GetChild(PatchTerrain::TOP_RIGHT)->GetChild(PatchTerrain::TOP_RIGHT)->GetChild(PatchTerrain::TOP_RIGHT)->Split();
		
		test_terrain5.GetChild(PatchTerrain::BOTTOM_RIGHT)->GetChildren()[PatchTerrain::TOP_LEFT]->GetChildren()[PatchTerrain::TOP_LEFT]->GetChildren()[PatchTerrain::TOP_LEFT]->Split();
		
		test_terrain5.GetChild(PatchTerrain::TOP_RIGHT)->GetChild(PatchTerrain::TOP_LEFT)->Split();
		test_terrain5.GetChild(PatchTerrain::TOP_RIGHT)->GetChild(PatchTerrain::TOP_LEFT)->GetChild(PatchTerrain::BOTTOM_RIGHT)->Split();
		
		test_terrain5.GetChild(PatchTerrain::TOP_RIGHT)->GetChild(PatchTerrain::BOTTOM_RIGHT)->Split();
		test_terrain5.GetChild(PatchTerrain::TOP_RIGHT)->GetChild(PatchTerrain::BOTTOM_RIGHT)->GetChild(PatchTerrain::BOTTOM_LEFT)->Split();
		
		//testTerrain5.GetChild(PatchTerrain::TOP_LEFT)->GetChild(PatchTerrain::BOTTOM_RIGHT)->GetChild(PatchTerrain::BOTTOM_LEFT)->Split();
		//testTerrain5.GetChild(PatchTerrain::TOP_LEFT)->GetChild(PatchTerrain::BOTTOM_RIGHT)->GetChild(PatchTerrain::TOP_LEFT)->Split();
		
		
		test_terrain5.GetChild(PatchTerrain::TOP_RIGHT)->GetChild(PatchTerrain::BOTTOM_LEFT)->GetChild(PatchTerrain::BOTTOM_LEFT)->Split();
		for(PatchTerrain *child : test_terrain5.GetChild(PatchTerrain::TOP_RIGHT)->GetChild(PatchTerrain::BOTTOM_LEFT)->GetChild(PatchTerrain::BOTTOM_LEFT)->GetChildren())
		{
			child->Split();
			for(PatchTerrain *child2 : child->GetChildren())
			{
				child2->Split();
				
				for(PatchTerrain *child3 : child2->GetChildren())
				{
					child3->Split();
				}
			}
		}
		
		
		test_terrain5.GetChild(PatchTerrain::TOP_RIGHT)->GetChild(PatchTerrain::BOTTOM_LEFT)->Merge();
		//testTerrain5.GetChild(PatchTerrain::BOTTOM_RIGHT)->GetChild(PatchTerrain::TOP_LEFT)->Merge();
		
		//testTerrain5.GetChild(PatchTerrain::TOP_RIGHT)->GetChild(PatchTerrain::BOTTOM_RIGHT)->GetChild(PatchTerrain::BOTTOM_LEFT)->Split();
	}
	
	virtual ~TerrainDemo() override
	{}
};

#endif // TERRAIN_DEMO_H
