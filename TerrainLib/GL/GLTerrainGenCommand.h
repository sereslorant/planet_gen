#ifndef TERRAIN_GEN_COMMAND_H
#define TERRAIN_GEN_COMMAND_H

#include "GLUtils.h"



class GLTerrainGenCommand
{
protected:
	GLTextureRect &texture;
	
public:
	
	virtual void Generate() = 0;
	
	GLTerrainGenCommand(GLTextureRect &p_texture)
		:texture(p_texture)
	{}
	
	virtual ~GLTerrainGenCommand()
	{}
};



class GaussianCommand : public GLTerrainGenCommand
{
public:
	unsigned int resolution = 128;
	unsigned int seed = std::mt19937::default_seed;
	
	virtual void Generate() override
	{
		std::vector<float> noise_data;
		NoiseUtils::GenerateGaussianNoise(noise_data,resolution);
		
		texture.UploadData(GL_R32F,resolution,noise_data.data());
	}
	
	GaussianCommand(GLTextureRect &p_texture)
		:GLTerrainGenCommand(p_texture)
	{}
	
	virtual ~GaussianCommand() override
	{}
};



class StationaryGaussianCommand : public GLTerrainGenCommand
{
private:
	GLFramebuffer output_fb;
	GLStationaryProcessInstance proc_instance;
	
public:
	
	void SetCovariance(unsigned int resolution,std::function<float(const vec4&,const vec4&)> covariance_function)
	{
		proc_instance.SetCovariance(resolution,covariance_function);
	}
	
	void SetNoiseReal(GLTextureRect &real)
	{
		proc_instance.SetNoiseReal(real);
	}
	
	void SetNoiseIm(GLTextureRect &im)
	{
		proc_instance.SetNoiseIm(im);
	}
	
	virtual void Generate() override
	{
		proc_instance.Run(output_fb,texture.GetSize());
	}
	
	struct Programs
	{
		GLShaderProgram &dft;
		GLShaderProgram &dft2;
		GLShaderProgram &diagonal;
		
		Programs(GLShaderProgram &p_dft,GLShaderProgram &p_dft2,GLShaderProgram &p_diagonal)
			:dft(p_dft),dft2(p_dft2),diagonal(p_diagonal)
		{}
	};
	
	StationaryGaussianCommand(GLTextureRect &p_texture,const Programs &programs,GLQuad &screen_size_quad)
		:GLTerrainGenCommand(p_texture),proc_instance(programs.dft,programs.dft2,programs.diagonal,screen_size_quad)
	{
		output_fb.SetColorAttachment(texture.GetTexture());
	}
	
	virtual ~StationaryGaussianCommand() override
	{}
};



/*
 * TerrainPipeline
 */



#include <TerrainLib/GL/GLLineStrip.h>

class GLRenderProcess
{
private:
	GLShaderProgram &shader_program;
	
	GLTextureRect *noise = nullptr;
	
	vec4 curve_begin;
	vec4 curve_end;
	
	GLLineStripList *line_strip_list;
	
	GLenum blend_equation = GL_FUNC_ADD;
	
public:
	
	void SetNoise(GLTextureRect *p_noise)
	{
		noise = p_noise;
	}
	
	void SetLineStripList(GLLineStripList *new_line_strip_list)
	{
		line_strip_list = new_line_strip_list;
	}
	
	void SetInterpolationCurveBegin(const vec4 &p_curve_begin)
	{
		curve_begin = p_curve_begin;
	}
	
	void SetInterpolationCurveEnd(const vec4 &p_curve_end)
	{
		curve_end = p_curve_end;
	}
	
	void SetBlendFunc(GLenum new_blend_func)
	{
		blend_equation = new_blend_func;
	}
	
	void Execute()
	{
		glBlendEquation(blend_equation);
		glBlendFunc(GL_ONE,GL_ONE);
		
		shader_program.UseProgram();
		
		shader_program.UploadUniformVector("height_curve_v0",curve_begin);
		shader_program.UploadUniformVector("height_curve_v1",curve_end);
		
		glActiveTexture(GL_TEXTURE0);
		if(noise != nullptr)
		{
			glBindTexture(GL_TEXTURE_2D,noise->GetTexture());
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D,0);
		}
		
		shader_program.AssignTexture("noise",0);
		
		line_strip_list->Draw();
	}
	
	GLRenderProcess(GLShaderProgram &p_shader_program)
		:shader_program(p_shader_program)
	{}
	
	~GLRenderProcess()
	{
		std::cout << "Render process deleted" << std::endl;
	}
};

class GLTerrainPipeline
{
private:
	std::vector<std::unique_ptr<GLRenderProcess> > pipeline_elements;
	
	std::map<std::vector<LineStrip> *,GLLineStripList> line_strip_lists;
	
	GLShaderProgram &shader_program;
	
public:
	
	GLLineStripList &GetLineStripList(std::vector<LineStrip> *line_strip_list)
	{
		auto I = line_strip_lists.find(line_strip_list);
		
		if(I == line_strip_lists.end())
		{
			GLLineStripList &result = line_strip_lists[line_strip_list];
			
			result.Initialize(shader_program,line_strip_list->size());
			result.Upload(*line_strip_list);
			
			return result;
		}
		else
		{
			return I->second;
		}
	}
	
	void Execute()
	{
		for(auto &pipeline_element : pipeline_elements)
		{
			pipeline_element->Execute();
		}
	}
	
	std::size_t GetSize()
	{
		return pipeline_elements.size();
	}
	
	void InsertElement(unsigned int id,GLRenderProcess *pipeline_element)
	{
		pipeline_elements.insert(pipeline_elements.begin() + id,std::unique_ptr<GLRenderProcess>(pipeline_element));
	}
	
	GLRenderProcess &GetElement(unsigned int id)
	{
		return *pipeline_elements[id];
	}
	
	void RemoveElement(unsigned int id)
	{
		pipeline_elements.erase(pipeline_elements.begin() + id);
	}
	
	GLTerrainPipeline(GLShaderProgram &p_shader_program)
		:shader_program(p_shader_program)
	{}
};

class GLTerrainRenderCommand : public GLTerrainGenCommand
{
private:
	GLTerrainPipeline &pipeline;
	
	GLFramebuffer output_buffer;
	
public:
	
	void Generate()
	{
		output_buffer.Bind();
		
		glViewport(0,0,texture.GetSize(),texture.GetSize());
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glEnable(GL_BLEND);
		
		pipeline.Execute();
		
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}
	
	GLTerrainRenderCommand(GLTextureRect &p_texture,GLTerrainPipeline &p_pipeline)
		:GLTerrainGenCommand(p_texture),pipeline(p_pipeline)
	{
		output_buffer.SetColorAttachment(texture.GetTexture());
	}
};

/*
 * TODO: Függőségek DAG-gal.
 */

struct GLTerrainGenerator
{
	GLShaderProgram &shader_program;
	
	std::map<std::size_t,std::unique_ptr<GLTerrainPipeline> > terrain_pipelines;
	
	std::map<std::size_t,GLTextureRect> textures;
	
	std::vector<std::unique_ptr<GLTerrainGenCommand> > commands;
	
	GLTextureRect &GetTexture(std::size_t texture_id)
	{
		auto I = textures.find(texture_id);
		
		if(I == textures.end())
		{
			return textures[texture_id];
		}
		else
		{
			return I->second;
		}
	}
	
	GLTerrainPipeline &GetTerrainPipeline(std::size_t terrain_pipeline_id)
	{
		auto I = terrain_pipelines.find(terrain_pipeline_id);
		
		if(I == terrain_pipelines.end())
		{
			GLTerrainPipeline *result = new GLTerrainPipeline(shader_program);
			terrain_pipelines[terrain_pipeline_id] = std::unique_ptr<GLTerrainPipeline>(result);
			
			return *result;
		}
		else
		{
			return *I->second;
		}
	}
	
	void Execute()
	{
		for(auto &command : commands)
		{
			command->Generate();
		}
	}
	
	GLTerrainGenerator(GLShaderProgram &p_shader_program)
		:shader_program(p_shader_program)
	{}
};

#endif // TERRAIN_GEN_COMMAND_H
