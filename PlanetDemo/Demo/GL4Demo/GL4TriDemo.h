#ifndef GL_TRI_DEMO_H
#define GL_TRI_DEMO_H

#include "GL4PlanetDemoBase.h"

#include "GLTriangleMesh.h"

#include "GLShaderSrc.h"

#include <PlanetDemo/Demo/DemoUtils.h>

class GL4TriDemo : public GL4PlanetDemoBase
{
private:
	GLShaderProgram shader_program;
	
	GLTriangleMesh triangle_mesh[6];
	
public:
	
	virtual void Logic() override
	{
		ApplyLod();
		
		for(int i=0;i<6;i++)
		{
			StaticMesh test_mesh;
			
			ScaledHeightmap noise_heightmap(noise_image);
			
			ScaledSurface scale(sphere_patches[(PlanetTerrain::TerrainPosition)i],planet_sphere.GetRadius());
			
			//HeightmapSurface heightmap_surface(scale,sphere_patches[(PlanetTerrain::TerrainPosition)i],noise_heightmap);
			
			
			HeightmapSampler sampler(scale,test_mesh);
			//HeightmapSampler sampler(heightmap_surface,test_mesh);
			
			std::function<void(PatchTerrain &)> sample = [&sampler](PatchTerrain &terrain)
																		{sampler.Sample(terrain);};
			
			ForeachLeaf(test_planet.GetTerrain((PlanetTerrain::TerrainPosition)i),sample);
			
			triangle_mesh[i].Fill(test_mesh);
		}
	}
	
	virtual void Draw() override
	{
		glEnable(GL_DEPTH_TEST);
		
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		
		//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		
		shader_program.UseProgram();
		
		mat4 mvp_matrix = view_matrix * projection_matrix;
		
		shader_program.UploadUniformMatrix("model_view_proj_matrix",mvp_matrix);
		
		for(int i=0;i<6;i++)
			{triangle_mesh[i].Draw();}
	}
	
	GL4TriDemo(GrayscaleImage &p_noise_image,Camera3D &p_camera)
		:GL4PlanetDemoBase(p_noise_image,p_camera)
	{
		GLShader vertex_shader(vertex_shader_source,GL_VERTEX_SHADER);
		GLShader fragment_shader(fragment_shader_source,GL_FRAGMENT_SHADER);
		
		GLuint shaders[2] =
		{
			vertex_shader.GetShader(),
			fragment_shader.GetShader()
		};
		
		shader_program.Initialize(shaders,2);
		
		for(int i=0;i<6;i++)
			{triangle_mesh[i].Initialize(shader_program);}
	}
	
	virtual ~GL4TriDemo() override
	{}
};

#endif // GL_TRI_DEMO_H
