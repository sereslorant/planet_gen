
#include <DemoFramework/SDL2DemoFramework.h>
#include <DemoFramework/Camera3D.h>

#include <DemoFramework/GL4/GL4DemoBase.h>

#include <TerrainLib/GL/GLTerrainGenCommand.h>

#include <TerrainLib/GL/GLLineStrip.h>
#include <TerrainLib/LineStrip.h>

#include <vector>
#include <array>

struct NeighborData
{
	unsigned int up;
	unsigned int left;
	unsigned int down;
	unsigned int right;
};

struct PatchMesh
{
	unsigned int num_patches = 0;
	
	std::vector<vec4> vertex_array;
	std::vector<NeighborData> neighbor_array;
	
	void AddPatch(const std::array<vec4,4> &vertices,const NeighborData &neighbor_data)
	{
		for(int i=0;i < 4;i++)
			{vertex_array.push_back(vertices[i]);}
		
		for(int i=0;i < 4;i++)
			{neighbor_array.push_back(neighbor_data);}
		
		num_patches++;
	}
};

#include <PlanetLib/MeshGenerator/LeafGenerator.h>

class PatchSampler
{
protected:
	PatchMesh &patch_mesh;
	
public:
	
	virtual void Sample(PatchTerrain &patch_terrain)
	{
		NeighborInformation neighbor_info(patch_terrain);
		
		NeighborData neighbor_data;
		
		neighbor_data.up	= neighbor_info.ScaleDown(PatchTerrain::UP)		? 0 : 1;
		neighbor_data.down	= neighbor_info.ScaleDown(PatchTerrain::DOWN)	? 0 : 1;
		neighbor_data.left	= neighbor_info.ScaleDown(PatchTerrain::LEFT)	? 0 : 1;
		neighbor_data.right	= neighbor_info.ScaleDown(PatchTerrain::RIGHT)	? 0 : 1;
		
		patch_mesh.AddPatch(patch_terrain.GetVertices(),neighbor_data);
	}
	
	PatchSampler(PatchMesh &p_patch_mesh)
		:patch_mesh(p_patch_mesh)
	{}
	
	virtual ~PatchSampler()
	{}
};

class GLPatchMesh
{
private:
	GLuint vertex_array_object;
	
	unsigned int num_patches;
	GLuint vertex_buffer;
	GLuint neighbor_buffer;
	
	static constexpr unsigned int CP_COUNT = 4;
	
public:
	
	void Initialize(GLShaderProgram &shader_program)
	{
		glGenVertexArrays(1,&vertex_array_object);
		glBindVertexArray(vertex_array_object);
		
		glGenBuffers(1,&vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
		
		glEnableVertexAttribArray(shader_program.GetAttribLocation("position"));
		glVertexAttribPointer(shader_program.GetAttribLocation("position"),4,GL_FLOAT,GL_FALSE,0,0);
		
		glGenBuffers(1,&neighbor_buffer);
		glBindBuffer(GL_ARRAY_BUFFER,neighbor_buffer);
		
		glEnableVertexAttribArray(shader_program.GetAttribLocation("neighbor"));
		glVertexAttribIPointer(shader_program.GetAttribLocation("neighbor"),4,GL_UNSIGNED_INT,0,0);
		
		glBindVertexArray(0);
	}
	
	void Fill(const PatchMesh &patch_mesh_data)
	{
		glBindVertexArray(vertex_array_object);
		
		glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER,patch_mesh_data.vertex_array.size()*sizeof(vec4),patch_mesh_data.vertex_array.data(),GL_STATIC_DRAW);
		
		glBindBuffer(GL_ARRAY_BUFFER,neighbor_buffer);
		glBufferData(GL_ARRAY_BUFFER,patch_mesh_data.neighbor_array.size()*sizeof(NeighborData),patch_mesh_data.neighbor_array.data(),GL_STATIC_DRAW);
		
		num_patches = patch_mesh_data.num_patches;
		
		glBindVertexArray(0);
	}
	
	void Draw()
	{
		glBindVertexArray(vertex_array_object);
		glPatchParameteri(GL_PATCH_VERTICES,CP_COUNT);
		
		glDrawArrays(GL_PATCHES,0,num_patches * CP_COUNT);
	}
	
	GLPatchMesh()
	{}
	
	~GLPatchMesh()
	{
		glDeleteVertexArrays(1,&vertex_array_object);
		
		glDeleteBuffers(1,&vertex_buffer);
	}
};

#include "GLShaderSrc.h"
#include <TerrainLib/GL/Shaders/ImgProcShaderSrc.h>
#include <TerrainLib/GL/Shaders/DftShaderSrc.h>
#include <TerrainLib/GL/Shaders/NoiseShaderSrc.h>
#include <TerrainLib/GL/Shaders/CurveShaders.h>

class GL4FlatTerrain3DDemo : public GL4DemoBase
{
private:
	GLShaderProgram shader_program;
	
	GLTextureRect heightmap;
	
	PatchTerrain test_terrain = {{{{0.0,0.0},{1.0,0.0},{1.0,1.0},{0.0,1.0}}}};
	GLPatchMesh test_mesh;
	
	vec4 camera_pos;
	
public:
	
	virtual void Logic() override
	{
		
	}
	
	virtual void ApplyViewTransform(const vec4 &position,float yaw,float pitch) override
	{
		GL4DemoBase::ApplyViewTransform(position,yaw,pitch);
		camera_pos = position;
	}
	
	virtual void Draw() override
	{
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		
		//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		
		shader_program.UseProgram();
		
		mat4 mvp_matrix = view_matrix * projection_matrix;
		
		shader_program.UploadUniformMatrix("model_view_proj_matrix",mvp_matrix);
		shader_program.UploadUniformVector("CameraPosition",camera_pos);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,heightmap.GetTexture());
		
		shader_program.AssignTexture("noise_map",0);
		
		test_mesh.Draw();
	}
	
	GL4FlatTerrain3DDemo()
		:GL4DemoBase()
	{
		/*
		 * Setting up 3D shaders and patch terrain
		 */
		
		GLShader vertex_shader(tess_vertex_shader_source,GL_VERTEX_SHADER);
		GLShader ctrl_shader(tess_ctrl_shader_source,GL_TESS_CONTROL_SHADER);
		GLShader eval_surface_shader(tess_eval_sphere_shader_source,GL_TESS_EVALUATION_SHADER);
		GLShader eval_shader(tess_eval_shader_source,GL_TESS_EVALUATION_SHADER);
		GLShader fragment_shader(fragment_shader_source,GL_FRAGMENT_SHADER);
		
		GLShader pbr_eq_fragment_shader(PbEquationsSource,GL_FRAGMENT_SHADER);
		GLShader pbr_final_fragment_shader(FragmentShaderSource,GL_FRAGMENT_SHADER);
		 /*
		GLuint shaders[5] =
		{
			vertex_shader.GetShader(),
			ctrl_shader.GetShader(),
			eval_surface_shader.GetShader(),
			eval_shader.GetShader(),
			fragment_shader.GetShader()
		};
		
		shader_program.Initialize(shaders,5);
		// */
		// /*
		GLuint shaders[6] =
		{
			vertex_shader.GetShader(),
			ctrl_shader.GetShader(),
			eval_surface_shader.GetShader(),
			eval_shader.GetShader(),
			pbr_eq_fragment_shader.GetShader(),
			pbr_final_fragment_shader.GetShader()
		};
		
		shader_program.Initialize(shaders,6);
		// */
		
		test_terrain.Split();
		
		for(auto child1 : test_terrain.GetChildren())
		{
			child1->Split();
			
			for(auto child2 : child1->GetChildren())
			{
				child2->Split();
				
				for(auto child3 : child2->GetChildren())
				{
					child3->Split();
					
					for(auto child4 : child3->GetChildren())
					{
						child4->Split();
						
						for(auto child5 : child4->GetChildren())
						{
							child5->Split();
							/*
							for(auto child6 : child5->GetChildren())
							{
								child6->Split();
								
								for(auto child7 : child6->GetChildren())
								{
									child7->Split();
								}
							}*/
						}
					}
				}
			}
		}
		
		test_mesh.Initialize(shader_program);
		
		PatchMesh patch_mesh_data;
		PatchSampler sampler(patch_mesh_data);
		
		std::function<void(PatchTerrain &)> sample = [&sampler](PatchTerrain &terrain)
																		{sampler.Sample(terrain);};
		
		ForeachLeaf(test_terrain,sample);
		
		test_mesh.Fill(patch_mesh_data);
		
		/*
		 * Generating noise
		 */
		
		GLShaderProgram dft_program;
		GLShaderProgram dft2_program;
		
		GLShaderProgram diagonal_program;
		
		GLShader ssquad_vertex_shader(ssquad_vertex_shader_source,GL_VERTEX_SHADER);
		GLShader dft_shader(real_dft_fragment_shader_source,GL_FRAGMENT_SHADER);
		GLShader dft2_shader(complex_dft_fragment_shader_source,GL_FRAGMENT_SHADER);
		GLShader diagonal_shader(diagonal_fragment_shader_source,GL_FRAGMENT_SHADER);
		
		GLuint dft_shaders[2] =
		{
			ssquad_vertex_shader.GetShader(),
			dft_shader.GetShader()
		};
		
		dft_program.Initialize(dft_shaders,2);
		
		
		GLuint dft2_shaders[2] =
		{
			ssquad_vertex_shader.GetShader(),
			dft2_shader.GetShader()
		};
		
		dft2_program.Initialize(dft2_shaders,2);
		
		
		GLuint diagonal_shaders[2] =
		{
			ssquad_vertex_shader.GetShader(),
			diagonal_shader.GetShader()
		};
		
		diagonal_program.Initialize(diagonal_shaders,2);
		
		
		GLTextureRect noise1;
		GLTextureRect noise2;
		unsigned int resolution = 128;
		
		std::vector<float> noise1_data;
		NoiseUtils::GenerateGaussianNoise(noise1_data,resolution);
		
		noise1.UploadData(GL_R32F,resolution,noise1_data.data());
		
		std::vector<float> noise2_data;
		NoiseUtils::GenerateGaussianNoise(noise2_data,resolution,10000);
		
		noise2.UploadData(GL_R32F,resolution,noise2_data.data());
		
		GLQuad screen_size_quad;
		
		vec4 position = {-1.0,-1.0};
		float width = 2.0;
		float height = 2.0;
		screen_size_quad.Initialize(position,width,height,dft_program);
		
		GLStationaryProcessInstance proc_instance(dft_program,dft2_program,diagonal_program,screen_size_quad);
		
		proc_instance.SetCovariance(resolution,
			[](const vec4&s,const vec4&t)
			{
				//float c = -4.0;
				//float c = -8.0;
				//float c = -10.0;
				float c = -16.0;
				//float c = -32.0;
				float norm =	std::pow(std::min(std::abs(s[0] - t[0]),1.0f - std::abs(s[0] - t[0])),2.0) +
								std::pow(std::min(std::abs(s[1] - t[1]),1.0f - std::abs(s[1] - t[1])),2.0);
				
				return std::exp(c*std::sqrt(norm));
			}
		);
		
		proc_instance.SetNoiseReal(noise1);
		proc_instance.SetNoiseIm(noise2);
		
		GLTextureRect final_noise;
		final_noise.AllocateStorage(GL_RG32F,resolution);
		
		//proc_instance.SetOutput(final_noise);
		
		GLFramebuffer final_noise_fb;
		final_noise_fb.SetColorAttachment(final_noise.GetTexture());
		
		proc_instance.Run(final_noise_fb,final_noise.GetSize());
		
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		
		/*
		 * Generating terrain
		 */
		
		GLShader curve_vertex_shader(curve_vertex_shader_source,GL_VERTEX_SHADER);
		GLShader tc_shader(curve_tc_shader_source,GL_TESS_CONTROL_SHADER);
		GLShader te_shader(curve_te_shader_source,GL_TESS_EVALUATION_SHADER);
		GLShader curve_fragment_shader(curve_fragment_shader_source,GL_FRAGMENT_SHADER);
		
		GLuint curve_shaders[4] =
		{
			curve_vertex_shader.GetShader(),
			tc_shader.GetShader(),
			te_shader.GetShader(),
			curve_fragment_shader.GetShader()
		};
		
		GLShaderProgram shader_program_fb;
		shader_program_fb.Initialize(curve_shaders,4);
		
		LineStrip test_line_strip1;
		test_line_strip1.PushBackPoint({0.5,0.8},0.5,0.1);
		test_line_strip1.PushBackPoint({0.2,0.2},0.75,0.25);
		test_line_strip1.PushBackPoint({0.8,0.2},0.7,0.15);
		test_line_strip1.PushBackPoint({0.8,0.5},0.6,0.1);
		
		LineStrip test_line_strip2;
		test_line_strip2.PushBackPoint({0.8,0.2},0.7,0.15);
		test_line_strip2.PushBackPoint({0.8,0.72},0.5,0.1);
		test_line_strip2.PushBackPoint({0.38,0.62},0.75,0.25);
		
		LineStrip test_line_strip3;
		test_line_strip3.PushBackPoint({-0.8,0.9},0.7,0.15);
		test_line_strip3.PushBackPoint({0.0,0.8},0.5,0.1);
		test_line_strip3.PushBackPoint({1.0,1.0},0.75,0.25);
		
		std::vector<LineStrip> test_line_strip_list1;
		
		test_line_strip_list1.push_back(test_line_strip1);
		test_line_strip_list1.push_back(test_line_strip2);
		
		std::vector<LineStrip> test_line_strip_list2;
		
		test_line_strip_list2.push_back(test_line_strip3);
		
		GLTerrainGenerator terrain_generator(shader_program_fb);
		
		
		heightmap.AllocateStorage(GL_R32F,512);
		
		GLTerrainPipeline &pipeline = terrain_generator.GetTerrainPipeline(0);
		
		
		GLRenderProcess *render_process1 = new GLRenderProcess(shader_program_fb);
		
		render_process1->SetNoise(&final_noise);
		
		render_process1->SetLineStripList(&pipeline.GetLineStripList(&test_line_strip_list1));
		
		render_process1->SetInterpolationCurveBegin({0.0,0.0,0.0,0.0});
		render_process1->SetInterpolationCurveEnd({1.0,0.0,0.0,0.0});
		
		GLRenderProcess *render_process2 = new GLRenderProcess(shader_program_fb);
		
		//render_process2->SetNoise(&final_noise);
		
		render_process2->SetLineStripList(&pipeline.GetLineStripList(&test_line_strip_list2));
		
		render_process2->SetInterpolationCurveBegin({1.0,0.0,0.0,0.0});
		render_process2->SetInterpolationCurveEnd({0.0,0.0,0.0,0.0});
		
		pipeline.InsertElement(0,render_process1);
		pipeline.InsertElement(1,render_process2);
		
		GLTerrainRenderCommand terrain_render_command(heightmap,pipeline);
		terrain_render_command.Generate();
	}
	
	virtual ~GL4FlatTerrain3DDemo() override
	{}
};

int main(int argc,char *argv[])
{
	Camera3D *camera = new Camera3D;
	SDL2DemoFramework DemoFramework(800,600,nullptr,camera,2);
	
	DemoFramework.SetDemo(new GL4FlatTerrain3DDemo);
	
	DemoFramework.Loop();
	return 0;
}
