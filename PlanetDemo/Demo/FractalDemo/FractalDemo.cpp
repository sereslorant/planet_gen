
#include "FractalDemo.h"

#include <SDL2/SDL_opengl.h>

void FractalDemo::Logic()
{}

float param_x = 0.0;
float param_y = 0.0;

unsigned int num_frames = 0;
constexpr unsigned int NUM_FRAMES = 20;
bool wireframe_planet = false;



void DrawSurface(PatchTerrain &terrain,const IHeightmap &heightmap,const vec4 &position,float orientation)
{
	FlatSurface heightmap_surface(heightmap);
	NeighborTrackingRenderer neighbor_tracking_renderer(heightmap_surface);
	std::function<void(PatchTerrain &)> draw_function = [&neighbor_tracking_renderer](PatchTerrain &terrain)
															{neighbor_tracking_renderer.Draw(terrain);};
	
	glPushMatrix();
		glTranslatef(position[0],position[1],position[2]);
		glTranslatef( 450.0,0.0, 450.0);
		glRotatef(orientation,0.0,1.0,0.0);
		glTranslatef(-450.0,0.0,-450.0);
		ForeachLeaf(terrain,draw_function);
	glPopMatrix();
}

void FractalDemo::DrawStitchedHeightmaps()
{
	FractalHeightmap test_fractal1(terrain_heightmap,{0.0f + param_x,0.0f + param_y},std::array<float,4>{{1.25,0.05,3.7,0.4,}},std::array<float,4>{{60,30,15,170,}});
	FractalHeightmap test_fractal2(terrain_heightmap,{4.0f + param_x,6.0f + param_y},std::array<float,4>{{1.25,0.05,3.7,0.4,}},std::array<float,4>{{60,30,15,170,}});
	FractalHeightmap test_fractal3(terrain_heightmap,{-5.0f + param_x,-6.0f + param_y},std::array<float,4>{{1.25,0.05,3.7,0.4,}},std::array<float,4>{{60,30,15,170,}});
	
	ParamTransformedHeightmap test_heightmap1(test_fractal1);
	ParamTransformedHeightmap test_heightmap2(test_fractal2);
	ParamTransformedHeightmap test_heightmap3(test_fractal3);
	
	StitchedHeightmap test_segment1(test_heightmap1);
	StitchedHeightmap test_segment2(test_heightmap2);
	
	test_segment1.SetNeighbor(StitchedHeightmap::EAST,test_heightmap2,StitchedHeightmap::DEG_0);
	test_segment2.SetNeighbor(StitchedHeightmap::WEST,test_heightmap1,StitchedHeightmap::DEG_0);
	
	glPushMatrix();
	{
		FlatSurface test_surface1(test_heightmap1);
		NeighborTrackingRenderer test_renderer_1(test_surface1);
		std::function<void(PatchTerrain &)> draw_function_test1 = [&test_renderer_1](PatchTerrain &terrain)
												{test_renderer_1.Draw(terrain);};
		
		glTranslatef(1000.0,0.0,600.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test1);
		
		FlatSurface test_surface2(test_heightmap2);
		NeighborTrackingRenderer test_renderer_2(test_surface2);
		std::function<void(PatchTerrain &)> draw_function_test2 = [&test_renderer_2](PatchTerrain &terrain)
												{test_renderer_2.Draw(terrain);};
		
		glTranslatef(900.0,0.0,0.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test2);
	}
	glPopMatrix();
	
	glPushMatrix();
	{
		FlatSurface test_surface1(test_segment1);
		NeighborTrackingRenderer test_renderer_1(test_surface1);
		std::function<void(PatchTerrain &)> draw_function_test1 = [&test_renderer_1](PatchTerrain &terrain)
												{test_renderer_1.Draw(terrain);};
		
		glTranslatef(1000.0,0.0,1800.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test1);
		
		FlatSurface test_surface2(test_segment2);
		NeighborTrackingRenderer test_renderer_2(test_surface2);
		std::function<void(PatchTerrain &)> draw_function_test2 = [&test_renderer_2](PatchTerrain &terrain)
												{test_renderer_2.Draw(terrain);};
		
		glTranslatef(900.0,0.0,0.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test2);
	}
	glPopMatrix();
	
	StitchedHeightmap test_segment3(test_heightmap1);
	StitchedHeightmap test_segment4(test_heightmap2);
	
	test_segment3.SetNeighbor(StitchedHeightmap::SOUTH,test_heightmap2,StitchedHeightmap::DEG_270);
	test_segment4.SetNeighbor(StitchedHeightmap::EAST,test_heightmap1,StitchedHeightmap::DEG_90);
	
	glPushMatrix();
	{
		FlatSurface test_surface1(test_heightmap1);
		NeighborTrackingRenderer test_renderer_1(test_surface1);
		std::function<void(PatchTerrain &)> draw_function_test1 = [&test_renderer_1](PatchTerrain &terrain)
												{test_renderer_1.Draw(terrain);};
		
		glTranslatef(3600.0,0.0,600.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test1);
		
		FlatSurface test_surface2(test_heightmap2);
		NeighborTrackingRenderer test_renderer_2(test_surface2);
		std::function<void(PatchTerrain &)> draw_function_test2 = [&test_renderer_2](PatchTerrain &terrain)
												{test_renderer_2.Draw(terrain);};
		
		glTranslatef(0.0,0.0,1800.0);
		glRotatef(90.0,0.0,1.0,0.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test2);
	}
	glPopMatrix();
	
	glPushMatrix();
	{
		FlatSurface test_surface1(test_segment3);
		NeighborTrackingRenderer test_renderer_1(test_surface1);
		std::function<void(PatchTerrain &)> draw_function_test1 = [&test_renderer_1](PatchTerrain &terrain)
												{test_renderer_1.Draw(terrain);};
		
		glTranslatef(3600.0,0.0,2700.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test1);
		
		FlatSurface test_surface2(test_segment4);
		NeighborTrackingRenderer test_renderer_2(test_surface2);
		std::function<void(PatchTerrain &)> draw_function_test2 = [&test_renderer_2](PatchTerrain &terrain)
												{test_renderer_2.Draw(terrain);};
		
		glTranslatef(0.0,0.0,1800.0);
		glRotatef(90.0,0.0,1.0,0.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test2);
	}
	glPopMatrix();
	
	StitchedHeightmap test_segment5(test_heightmap1);
	StitchedHeightmap test_segment6(test_heightmap2);
	
	test_segment5.SetNeighbor(StitchedHeightmap::NORTH,test_heightmap2,StitchedHeightmap::DEG_90);
	test_segment6.SetNeighbor(StitchedHeightmap::EAST,test_heightmap1,StitchedHeightmap::DEG_270);
	
	glPushMatrix();
	{
		FlatSurface test_surface1(test_heightmap1);
		NeighborTrackingRenderer test_renderer_1(test_surface1);
		std::function<void(PatchTerrain &)> draw_function_test1 = [&test_renderer_1](PatchTerrain &terrain)
												{test_renderer_1.Draw(terrain);};
		
		glTranslatef(5400.0,0.0,600.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test1);
		
		FlatSurface test_surface2(test_heightmap2);
		NeighborTrackingRenderer test_renderer_2(test_surface2);
		std::function<void(PatchTerrain &)> draw_function_test2 = [&test_renderer_2](PatchTerrain &terrain)
												{test_renderer_2.Draw(terrain);};
		
		glTranslatef(900.0,0.0,-900.0);
		glRotatef(-90.0,0.0,1.0,0.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test2);
	}
	glPopMatrix();
	
	glPushMatrix();
	{
		FlatSurface test_surface1(test_segment5);
		NeighborTrackingRenderer test_renderer_1(test_surface1);
		std::function<void(PatchTerrain &)> draw_function_test1 = [&test_renderer_1](PatchTerrain &terrain)
												{test_renderer_1.Draw(terrain);};
		
		glTranslatef(5400.0,0.0,2700.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test1);
		
		FlatSurface test_surface2(test_segment6);
		NeighborTrackingRenderer test_renderer_2(test_surface2);
		std::function<void(PatchTerrain &)> draw_function_test2 = [&test_renderer_2](PatchTerrain &terrain)
												{test_renderer_2.Draw(terrain);};
		
		glTranslatef(900.0,0.0,-900.0);
		glRotatef(-90.0,0.0,1.0,0.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test2);
	}
	glPopMatrix();
	
	StitchedHeightmap test_segment7(test_heightmap1);
	StitchedHeightmap test_segment8(test_heightmap2);
	StitchedHeightmap test_segment9(test_heightmap3);
	
	test_segment7.SetNeighbor(StitchedHeightmap::NORTH,test_heightmap2,StitchedHeightmap::DEG_90);
	test_segment8.SetNeighbor(StitchedHeightmap::EAST,test_heightmap1,StitchedHeightmap::DEG_270);
	
	test_segment7.SetNeighbor(StitchedHeightmap::EAST,test_heightmap3,StitchedHeightmap::DEG_0);
	test_segment9.SetNeighbor(StitchedHeightmap::WEST,test_heightmap1,StitchedHeightmap::DEG_0);
	
	test_segment8.SetNeighbor(StitchedHeightmap::NORTH,test_heightmap3,StitchedHeightmap::DEG_180);
	test_segment9.SetNeighbor(StitchedHeightmap::NORTH,test_heightmap2,StitchedHeightmap::DEG_180);
	
	glPushMatrix();
	{
		FlatSurface test_surface1(test_heightmap1);
		NeighborTrackingRenderer test_renderer_1(test_surface1);
		std::function<void(PatchTerrain &)> draw_function_test1 = [&test_renderer_1](PatchTerrain &terrain)
												{test_renderer_1.Draw(terrain);};
		
		glTranslatef(7250.0,0.0,600.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test1);
		
		FlatSurface test_surface3(test_heightmap3);
		NeighborTrackingRenderer test_renderer_3(test_surface3);
		std::function<void(PatchTerrain &)> draw_function_test3 = [&test_renderer_3](PatchTerrain &terrain)
												{test_renderer_3.Draw(terrain);};
		
		glTranslatef(900.0,0.0,0.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test3);
		
		FlatSurface test_surface2(test_heightmap2);
		NeighborTrackingRenderer test_renderer_2(test_surface2);
		std::function<void(PatchTerrain &)> draw_function_test2 = [&test_renderer_2](PatchTerrain &terrain)
												{test_renderer_2.Draw(terrain);};
		
		glTranslatef(0.0,0.0,-900.0);
		glRotatef(-90.0,0.0,1.0,0.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test2);
	}
	glPopMatrix();
	
	glPushMatrix();
	{
		FlatSurface test_surface1(test_segment7);
		NeighborTrackingRenderer test_renderer_1(test_surface1);
		std::function<void(PatchTerrain &)> draw_function_test1 = [&test_renderer_1](PatchTerrain &terrain)
												{test_renderer_1.Draw(terrain);};
		
		glTranslatef(7250.0,0.0,2700.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test1);
		
		FlatSurface test_surface3(test_segment9);
		NeighborTrackingRenderer test_renderer_3(test_surface3);
		std::function<void(PatchTerrain &)> draw_function_test3 = [&test_renderer_3](PatchTerrain &terrain)
												{test_renderer_3.Draw(terrain);};
		
		glTranslatef(900.0,0.0,0.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test3);
		
		FlatSurface test_surface2(test_segment8);
		NeighborTrackingRenderer test_renderer_2(test_surface2);
		std::function<void(PatchTerrain &)> draw_function_test2 = [&test_renderer_2](PatchTerrain &terrain)
												{test_renderer_2.Draw(terrain);};
		
		glTranslatef(0.0,0.0,-900.0);
		glRotatef(-90.0,0.0,1.0,0.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test2);
		
		glTranslatef(900.0,0.0,-900.0);
		glRotatef(-90.0,0.0,1.0,0.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test2);
	}
	glPopMatrix();
	
	StitchedHeightmap test_segment10(test_fractal1);
	StitchedHeightmap test_segment11(test_fractal2);
	StitchedHeightmap test_segment12(test_fractal3);
	
	test_segment10.SetNeighbor(StitchedHeightmap::NORTH,test_fractal2,StitchedHeightmap::DEG_90);
	test_segment11.SetNeighbor(StitchedHeightmap::EAST,test_fractal1,StitchedHeightmap::DEG_270);
	
	test_segment10.SetNeighbor(StitchedHeightmap::EAST,test_fractal3,StitchedHeightmap::DEG_0);
	test_segment12.SetNeighbor(StitchedHeightmap::WEST,test_fractal1,StitchedHeightmap::DEG_0);
	
	test_segment11.SetNeighbor(StitchedHeightmap::NORTH,test_fractal3,StitchedHeightmap::DEG_180);
	test_segment12.SetNeighbor(StitchedHeightmap::NORTH,test_fractal2,StitchedHeightmap::DEG_180);
	
	glPushMatrix();
	{
		FlatSurface test_surface1(test_fractal1);
		NeighborTrackingRenderer test_renderer_1(test_surface1);
		std::function<void(PatchTerrain &)> draw_function_test1 = [&test_renderer_1](PatchTerrain &terrain)
												{test_renderer_1.Draw(terrain);};
		
		glTranslatef(9100.0,0.0,600.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test1);
		
		FlatSurface test_surface3(test_fractal3);
		NeighborTrackingRenderer test_renderer_3(test_surface3);
		std::function<void(PatchTerrain &)> draw_function_test3 = [&test_renderer_3](PatchTerrain &terrain)
												{test_renderer_3.Draw(terrain);};
		
		glTranslatef(900.0,0.0,0.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test3);
		
		FlatSurface test_surface2(test_fractal2);
		NeighborTrackingRenderer test_renderer_2(test_surface2);
		std::function<void(PatchTerrain &)> draw_function_test2 = [&test_renderer_2](PatchTerrain &terrain)
												{test_renderer_2.Draw(terrain);};
		
		glTranslatef(0.0,0.0,-900.0);
		glRotatef(-90.0,0.0,1.0,0.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test2);
	}
	glPopMatrix();
	
	glPushMatrix();
	{
		FlatSurface test_surface1(test_segment10);
		NeighborTrackingRenderer test_renderer_1(test_surface1);
		std::function<void(PatchTerrain &)> draw_function_test1 = [&test_renderer_1](PatchTerrain &terrain)
												{test_renderer_1.Draw(terrain);};
		
		glTranslatef(9100.0,0.0,2700.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test1);
		
		FlatSurface test_surface3(test_segment12);
		NeighborTrackingRenderer test_renderer_3(test_surface3);
		std::function<void(PatchTerrain &)> draw_function_test3 = [&test_renderer_3](PatchTerrain &terrain)
												{test_renderer_3.Draw(terrain);};
		
		glTranslatef(900.0,0.0,0.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test3);
		
		FlatSurface test_surface2(test_segment11);
		NeighborTrackingRenderer test_renderer_2(test_surface2);
		std::function<void(PatchTerrain &)> draw_function_test2 = [&test_renderer_2](PatchTerrain &terrain)
												{test_renderer_2.Draw(terrain);};
		
		glTranslatef(0.0,0.0,-900.0);
		glRotatef(-90.0,0.0,1.0,0.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test2);
		
		glTranslatef(900.0,0.0,-900.0);
		glRotatef(-90.0,0.0,1.0,0.0);
		ForeachLeaf(test_planet.GetTerrain(PlanetTerrain::TOP),draw_function_test2);
	}
	glPopMatrix();
}

void FractalDemo::Draw()
{
	glColor3f(0.5,0.5,0.5);
	
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	
	glDisable(GL_CULL_FACE);
	
	glPushMatrix();
		NeighborTrackingRenderer neighbor_tracking_renderer(image_heightmap);
		std::function<void(PatchTerrain &)> draw_function = [&neighbor_tracking_renderer](PatchTerrain &terrain)
												{neighbor_tracking_renderer.Draw(terrain);};
		
		glTranslatef(0.0,0.0,600.0);
		ForeachLeaf(test_terrain,draw_function);
	glPopMatrix();
	
	DrawStitchedHeightmaps();
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	constexpr float TRANSLATE = 900.0;
	
	//CubeHeightmap planet_heightmap(terrain_heightmap,terrain_heightmap,terrain_heightmap,terrain_heightmap,terrain_heightmap,terrain_heightmap);
	
	//glTranslatef(0.0,0.0,-200.0);
	/*
	FractalHeightmap fractals[6] =
	{
		{terrain_heightmap,{+0.0f + param_x,+0.0f + param_y}},
		{terrain_heightmap,{+0.0f + param_x,+2.0f + param_y}},
		{terrain_heightmap,{-1.0f + param_x,+1.0f + param_y}},
		{terrain_heightmap,{+1.0f + param_x,+1.0f + param_y}},
		{terrain_heightmap,{+0.0f + param_x,+3.0f + param_y}},
		{terrain_heightmap,{+0.0f + param_x,+1.0f + param_y}},
	};
	*/
	/*
	FractalHeightmap fractals[6] =
	{
		{terrain_heightmap,{+0.0f + param_x,+0.0f + param_y}},
		{terrain_heightmap,{+0.0f + param_x,-2.0f + param_y}},
		{terrain_heightmap,{+1.0f + param_x,-1.0f + param_y}},
		{terrain_heightmap,{-1.0f + param_x,-1.0f + param_y}},
		{terrain_heightmap,{+0.0f + param_x,-3.0f + param_y}},
		{terrain_heightmap,{+0.0f + param_x,-1.0f + param_y}},
	};
	*/
	/*
	FractalHeightmap fractals[6] =
	{
		{terrain_heightmap,{+0.0f + param_x,+1.0f + param_y}},
		{terrain_heightmap,{+0.0f + param_x,-1.0f + param_y}},
		{terrain_heightmap,{+1.0f + param_x,+0.0f + param_y}},
		{terrain_heightmap,{-1.0f + param_x,+0.0f + param_y}},
		{terrain_heightmap,{+0.0f + param_x,-2.0f + param_y}},
		{terrain_heightmap,{+0.0f + param_x,+0.0f + param_y}},
	};
	*/
	FractalHeightmap fractals[6] =
	{
		{terrain_heightmap,{+0.0f + param_x,+1.0f + param_y},std::array<float,4>{{1.25,0.05,3.7,0.4,}},std::array<float,4>{{60,30,15,170,}}},
		{terrain_heightmap,{+0.0f + param_x,-1.0f + param_y},std::array<float,4>{{1.25,0.05,3.7,0.4,}},std::array<float,4>{{60,30,15,170,}}},
		{terrain_heightmap,{-1.0f + param_x,+0.0f + param_y},std::array<float,4>{{1.25,0.05,3.7,0.4,}},std::array<float,4>{{60,30,15,170,}}},
		{terrain_heightmap,{+1.0f + param_x,+0.0f + param_y},std::array<float,4>{{1.25,0.05,3.7,0.4,}},std::array<float,4>{{60,30,15,170,}}},
		{terrain_heightmap,{+0.0f + param_x,-2.0f + param_y},std::array<float,4>{{1.25,0.05,3.7,0.4,}},std::array<float,4>{{60,30,15,170,}}},
		{terrain_heightmap,{+0.0f + param_x,+0.0f + param_y},std::array<float,4>{{1.25,0.05,3.7,0.4,}},std::array<float,4>{{60,30,15,170,}}},
	};
	
	ParamTransformedHeightmap weird_fractals[6] =
	{
		{fractals[0]},
		{fractals[1]},
		{fractals[2]},
		{fractals[3]},
		{fractals[4]},
		{fractals[5]},
	};
	//param_x += 0.005;
	//param_y += 0.005;
	/*
	CubeHeightmap planet_heightmap(	fractals[PlanetTerrain::TOP],
									fractals[PlanetTerrain::NEAR],
									fractals[PlanetTerrain::BOTTOM],
									fractals[PlanetTerrain::FAR],
									fractals[PlanetTerrain::LEFT],
									fractals[PlanetTerrain::RIGHT]
								  );
	*/
	CubeHeightmap planet_heightmap(	weird_fractals[PlanetTerrain::TOP],
									weird_fractals[PlanetTerrain::NEAR],
									weird_fractals[PlanetTerrain::BOTTOM],
									weird_fractals[PlanetTerrain::FAR],
									weird_fractals[PlanetTerrain::LEFT],
									weird_fractals[PlanetTerrain::RIGHT]
								  );
	
	constexpr float OFFSET_X = 0.0;
	constexpr float OFFSET_Z = - 6.0 * TRANSLATE;
	
	//Top
	DrawSurface(test_planet.GetTerrain(PlanetTerrain::TOP),planet_heightmap.GetTop(),{0.0 + OFFSET_X,0.0,TRANSLATE + OFFSET_Z},0.0);
	
	//Near
	DrawSurface(test_planet.GetTerrain(PlanetTerrain::NEAR),planet_heightmap.GetNear(),{0.0 + OFFSET_X,0.0,2.0*TRANSLATE + OFFSET_Z},0.0);
	
	//Left
	DrawSurface(test_planet.GetTerrain(PlanetTerrain::LEFT),planet_heightmap.GetLeft(),{-1.0*TRANSLATE + OFFSET_X,0.0,2.0*TRANSLATE + OFFSET_Z},0.0);
	
	//A lefttel szomszédosak
	DrawSurface(test_planet.GetTerrain(PlanetTerrain::TOP),planet_heightmap.GetTop(),{-1.0*TRANSLATE + OFFSET_X,0.0,1.0*TRANSLATE + OFFSET_Z}, 90.0);
	DrawSurface(test_planet.GetTerrain(PlanetTerrain::FAR),planet_heightmap.GetFar(),{-2.0*TRANSLATE + OFFSET_X,0.0,2.0*TRANSLATE + OFFSET_Z}, 180.0);
	
	//Right
	DrawSurface(test_planet.GetTerrain(PlanetTerrain::RIGHT),planet_heightmap.GetRight(),{1.0*TRANSLATE + OFFSET_X,0.0,2.0*TRANSLATE + OFFSET_Z},0.0);
	
	//A righttal szomszédosak
	DrawSurface(test_planet.GetTerrain(PlanetTerrain::TOP),planet_heightmap.GetTop(),{1.0*TRANSLATE + OFFSET_X,0.0,1.0*TRANSLATE + OFFSET_Z},-90.0);
	DrawSurface(test_planet.GetTerrain(PlanetTerrain::FAR),planet_heightmap.GetFar(),{2.0*TRANSLATE + OFFSET_X,0.0,2.0*TRANSLATE + OFFSET_Z},-180.0);
	
	//Bottom
	DrawSurface(test_planet.GetTerrain(PlanetTerrain::BOTTOM),planet_heightmap.GetBottom(),{0.0 + OFFSET_X,0.0,3.0*TRANSLATE + OFFSET_Z},0.0);
	
	//A bottommal szomszédosak
	DrawSurface(test_planet.GetTerrain(PlanetTerrain::LEFT),planet_heightmap.GetLeft(),{-1.0*TRANSLATE + OFFSET_X,0.0,3.0*TRANSLATE + OFFSET_Z}, 90.0);
	DrawSurface(test_planet.GetTerrain(PlanetTerrain::RIGHT),planet_heightmap.GetRight(),{1.0*TRANSLATE + OFFSET_X,0.0,3.0*TRANSLATE + OFFSET_Z},-90.0);
	
	//Far
	DrawSurface(test_planet.GetTerrain(PlanetTerrain::FAR),planet_heightmap.GetFar(),{0.0 + OFFSET_X,0.0,4.0*TRANSLATE + OFFSET_Z},0.0);
	
	//A farral szomszédosak
	DrawSurface(test_planet.GetTerrain(PlanetTerrain::LEFT),planet_heightmap.GetLeft(),{-1.0*TRANSLATE + OFFSET_X,0.0,4.0*TRANSLATE + OFFSET_Z}, 180.0);
	DrawSurface(test_planet.GetTerrain(PlanetTerrain::RIGHT),planet_heightmap.GetRight(),{1.0*TRANSLATE + OFFSET_X,0.0,4.0*TRANSLATE + OFFSET_Z},-180.0);
	DrawSurface(test_planet.GetTerrain(PlanetTerrain::TOP),planet_heightmap.GetTop(),{0.0 + OFFSET_X,0.0,5.0*TRANSLATE + OFFSET_Z},0.0);
	
	num_frames++;
	if(num_frames > NUM_FRAMES)
	{
		wireframe_planet = !wireframe_planet;
		num_frames = 0;
	}
	
	if(!wireframe_planet)
		{glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);}
	
	const IHeightmap *sphere_heightmaps[6] = 
	{
		&planet_heightmap.GetTop(),
		&planet_heightmap.GetBottom(),
		&planet_heightmap.GetLeft(),
		&planet_heightmap.GetRight(),
		&planet_heightmap.GetFar(),
		&planet_heightmap.GetNear(),
	};
	
	glTranslatef(0,600.0,5.0 * -1250.0);
	for(int i=0;i < 6;i++)
	{
		ScaledSurface scaled_surface(sphere_patches[i],500.0);
		HeightmapSurface sphere_surface(scaled_surface,sphere_patches[i],*sphere_heightmaps[i]);
		NeighborTrackingRenderer planet_renderer(sphere_surface);
		std::function<void(PatchTerrain &)> drawWithNeighbors = [&planet_renderer](PatchTerrain &terrain)
																	{planet_renderer.Draw(terrain);};
		
		ForeachLeaf(test_planet.GetTerrain((PlanetTerrain::TerrainPosition)i),drawWithNeighbors);
	}
}
