
#include "TerrainDemo.h"

#include <SDL2/SDL_opengl.h>

void TerrainDemo::Logic()
{}

void TerrainDemo::Draw()
{
	glColor3f(0.5,0.5,0.5);
	
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	
	SimpleRenderer simple_renderer1(test_heightmap1);
	
	simple_renderer1.Draw(test_terrain1);
	
	glPushMatrix();
	for(PatchTerrain *child : test_terrain1.GetChildren())
	{
		glTranslatef(0.0,30.0,0.0);
		
		simple_renderer1.Draw(*child);
	}
	
	for(PatchTerrain *child : test_terrain1.GetChildren()[0]->GetChildren())
	{
		glTranslatef(0.0,30.0,0.0);
		
		simple_renderer1.Draw(*child);
	}
	glPopMatrix();
	
	SimpleRenderer simple_renderer2(test_heightmap2);
	std::function<void(PatchTerrain &)> draw2 = [&simple_renderer2](PatchTerrain &terrain){simple_renderer2.Draw(terrain);};
	
	glPushMatrix();
	
	glTranslatef(0.0,0.0,-900.0);
	ForeachLeaf(test_terrain5,draw2);
	
	glPopMatrix();
	
	ForeachLeaf(test_terrain2,draw2);
	
	glPushMatrix();
	
	glTranslatef(0.0,0.0,-900.0);
	ForeachLeaf(test_terrain3,draw2);
	
	NeighborTrackingRenderer neighbor_tracking_renderer(test_heightmap2);
	std::function<void(PatchTerrain &)> draw_with_neighbors = [&neighbor_tracking_renderer](PatchTerrain &terrain)
											{neighbor_tracking_renderer.Draw(terrain);};
	
	glTranslatef(0.0,0.0,-900.0);
	ForeachLeaf(test_terrain3,draw_with_neighbors);
	
	glTranslatef(0.0,0.0,-900.0);
	ForeachLeaf(test_terrain4,draw_with_neighbors);
	
	
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	
	glTranslatef(900.0,0.0,0.0);
	ForeachLeaf(test_terrain4,draw_with_neighbors);
	
	glTranslatef(0.0,0.0,900.0);
	ForeachLeaf(test_terrain3,draw_with_neighbors);
	
	glTranslatef(0.0,0.0,900.0);
	ForeachLeaf(test_terrain3,draw2);
	
	glPopMatrix();
}
