#ifndef GL_TRI_DEMO_H
#define GL_TRI_DEMO_H

#include "GL4DemoBase.h"

#include <vector>
#include <array>

/*
 * 1) Nem gömb terrain (GPU-n)
 * 2) Dinamikus LOD (próbálgatás)
 * 3) PBR beemelése
 * 4) Valami material paraméter gyártás
 * 
 */

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

class GL4TessDemo : public GL4DemoBase
{
private:
	GLShaderProgram shader_program;
	
	GLPatchMesh patch_mesh[6];
	
	GLuint heightmap_texture;
	
public:
	
	virtual void Logic() override
	{
		ApplyLod();
		
		for(int i=0;i < 6;i++)
		{
			PatchMesh patch_mesh_data;
			PatchSampler sampler(patch_mesh_data);
			
			std::function<void(PatchTerrain &)> sample = [&sampler](PatchTerrain &terrain)
																			{sampler.Sample(terrain);};
			
			ForeachLeaf(test_planet.GetTerrain((PlanetTerrain::TerrainPosition)i),sample);
			
			patch_mesh[i].Fill(patch_mesh_data);
		}
	}
	
	virtual void Draw() override
	{
		glEnable(GL_DEPTH_TEST);
		
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		
		//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		
		shader_program.UseProgram();
		
		//mat4 mvp_matrix = projection_matrix * view_matrix;
		mat4 mvp_matrix = view_matrix * projection_matrix;
		
		shader_program.UploadUniformMatrix("model_view_proj_matrix",mvp_matrix);
		
		shader_program.UploadUniformVector("CameraPosition",camera_pos);
		
		for(int i=0;i < 6;i++)
		{
			const std::array<vec4,4> &corners = sphere_patches[i].GetCorners();
			
			shader_program.UploadUniformVector("corners[0]",corners[0]);
			shader_program.UploadUniformVector("corners[1]",corners[1]);
			shader_program.UploadUniformVector("corners[2]",corners[2]);
			shader_program.UploadUniformVector("corners[3]",corners[3]);
			
			shader_program.UploadUniformFloat("radius",planet_sphere.GetRadius());
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,heightmap_texture);
			
			shader_program.AssignTexture("noise_map",0);
			
			shader_program.UploadUniformVector("param_offset[0]",fractals[i].GetParamOffset());
			
			const std::array<float,4> &frequency = fractals[i].GetFrequency();
			shader_program.UploadUniformFloat("frequency[0][0]",frequency[0]);
			shader_program.UploadUniformFloat("frequency[0][1]",frequency[1]);
			shader_program.UploadUniformFloat("frequency[0][2]",frequency[2]);
			shader_program.UploadUniformFloat("frequency[0][3]",frequency[3]);
			
			const std::array<float,4> &amplitude = fractals[i].GetAmplitude();
			shader_program.UploadUniformFloat("amplitude[0][0]",amplitude[0]);
			shader_program.UploadUniformFloat("amplitude[0][1]",amplitude[1]);
			shader_program.UploadUniformFloat("amplitude[0][2]",amplitude[2]);
			shader_program.UploadUniformFloat("amplitude[0][3]",amplitude[3]);
			
			for(int j = 0;j < 4;j++)
			{
				std::string str_id = std::to_string(j + 1);
				shader_program.UploadUniformVector("param_offset[" + str_id + "]",std::get<0>(neighbors[i][j])->GetParamOffset());
				
				const std::array<float,4> &frequency = std::get<0>(neighbors[i][j])->GetFrequency();
				shader_program.UploadUniformFloat("frequency[" + str_id + "][0]",frequency[0]);
				shader_program.UploadUniformFloat("frequency[" + str_id + "][1]",frequency[1]);
				shader_program.UploadUniformFloat("frequency[" + str_id + "][2]",frequency[2]);
				shader_program.UploadUniformFloat("frequency[" + str_id + "][3]",frequency[3]);
				
				const std::array<float,4> &amplitude = std::get<0>(neighbors[i][j])->GetAmplitude();
				shader_program.UploadUniformFloat("amplitude[" + str_id + "][0]",amplitude[0]);
				shader_program.UploadUniformFloat("amplitude[" + str_id + "][1]",amplitude[1]);
				shader_program.UploadUniformFloat("amplitude[" + str_id + "][2]",amplitude[2]);
				shader_program.UploadUniformFloat("amplitude[" + str_id + "][3]",amplitude[3]);
				
				std::string str_rot_id = std::to_string(j);
				shader_program.UploadUniformInt("neighbor_rotation[" + str_rot_id + "]",std::get<1>(neighbors[i][j]));
			}
			
			patch_mesh[i].Draw();
		}
	}
	
	GL4TessDemo(GrayscaleImage &p_noise_image,Camera3D &p_camera)
		:GL4DemoBase(p_noise_image,p_camera)
	{
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
		for(int i=0;i < 6;i++)
			{patch_mesh[i].Initialize(shader_program);}
		
		glGenTextures(1,&heightmap_texture);
		
		glBindTexture(GL_TEXTURE_2D, heightmap_texture);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glTexImage2D(GL_TEXTURE_2D,0,GL_R32F,noise_image.GetWidth(),noise_image.GetHeight(),0,GL_RED,GL_FLOAT,noise_image.GetData());
	}
	
	virtual ~GL4TessDemo() override
	{
		glDeleteTextures(1,&heightmap_texture);
	}
};

#endif // GL_TRI_DEMO_H
