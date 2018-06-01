#ifndef GL_UTILS_H
#define GL_UTILS_H

//GL_QUAD_H

#include <glmath.hpp>
#include <DemoFramework/GL4/GLExt.h>
#include <DemoFramework/GL4/GLShaderProgram.h>

#include <array>

class GLQuad
{
private:
	GLuint vertex_array_object = 0;
	GLuint vertex_buffer = 0;
	
public:
	
	void Draw()
	{
		glBindVertexArray(vertex_array_object);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	}
	
	void Initialize(const vec4 &pos,float width,float height,GLShaderProgram &shader_program)
	{
		if(vertex_array_object == 0)
			{glGenVertexArrays(1,&vertex_array_object);}
		glBindVertexArray(vertex_array_object);
		
		if(vertex_buffer == 0)
			{glGenBuffers(1,&vertex_buffer);}
		glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
		
		std::array<vec4,4> vertices =
		{{
			{pos[0],pos[1],0.0,0.0},
			{pos[0],pos[1] + height,0.0,1.0},
			{pos[0] + width,pos[1],1.0,0.0},
			{pos[0] + width,pos[1] + height,1.0,1.0}
		}};
		
		glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(vec4),vertices.data(),GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(shader_program.GetAttribLocation("data"));
		glVertexAttribPointer(shader_program.GetAttribLocation("data"),4,GL_FLOAT,GL_FALSE,0,0);
		
		glBindVertexArray(0);
	}
	
	GLQuad()
	{}
	
	~GLQuad()
	{
		glDeleteVertexArrays(1,&vertex_array_object);
		glDeleteBuffers(1,&vertex_buffer);
	}
};

//GL_TEXTURE_RECT_H

#include <DemoFramework/GL4/GLExt.h>

class GLTextureRect
{
protected:
	unsigned int size;
	GLuint texture = 0;
	
public:
	
	unsigned int GetSize()
	{
		return size;
	}
	
	GLuint GetTexture()
	{
		return texture;
	}
	
	void UploadData(GLint internal_format,unsigned int new_size,void *data)
	{
		size = new_size;
		
		if(texture == 0)
			{glGenTextures(1,&texture);}
		glBindTexture(GL_TEXTURE_2D, texture);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glTexImage2D(GL_TEXTURE_2D,0,internal_format,size,size,0,GL_RED,GL_FLOAT,data);
	}
	
	void AllocateStorage(GLint internal_format,unsigned int new_size)
	{
		UploadData(internal_format,new_size,nullptr);
	}
	
	GLTextureRect()
	{}
	
	GLTextureRect(GLTextureRect &&other)
	{
		size	= other.size;
		texture	= other.texture;
		
		other.texture = 0;
	}
	
	~GLTextureRect()
	{
		glDeleteTextures(1,&texture);
	}
};

//GL_FRAMEBUFFER_H

#include <DemoFramework/GL4/GLExt.h>

class GLFramebuffer
{
private:
	GLuint framebuffer = 0;
	unsigned int size = 0;
	
public:
	
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
	}
	
	void SetColorAttachment(GLuint texture)
	{
		if(framebuffer == 0)
			{glGenFramebuffers(1,&framebuffer);}
		
		glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,texture,0);
		
		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers);
		
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Something happened" << std::endl;
		}
		
		//else
		//{
		//	std::cout << "Success!" << std::endl;
		//}
	}
	
	GLFramebuffer()
	{}
	
	~GLFramebuffer()
	{
		glDeleteFramebuffers(1,&framebuffer);
	}
};

//GL_PROCESS_INSTANCE_H

#include <DemoFramework/GL4/GLExt.h>
#include <DemoFramework/GL4/GLShaderProgram.h>

#include <map>
#include <string>

class GL4ProcessInstance
{
private:
	GLShaderProgram &program;
	GLQuad &screen_size_quad;
	
	std::map<std::string,GLTextureRect*> input_map;
	
	//GLTextureRect *output_tex = nullptr;
	//GLFramebuffer output_buffer;
	
public:
	
	void SetInput(const std::string &name,GLTextureRect &texture)
	{
		input_map[name] = &texture;
	}
	/*
	void SetOutput(GLTextureRect &output_texture)
	{
		output_tex = &output_texture;
		output_buffer.SetColorAttachment(output_texture.GetTexture());
	}
	*/
	//void Run()
	void Run(GLFramebuffer &output_buffer,unsigned int output_size)
	{
		//if(output_tex != nullptr)
		//{
			output_buffer.Bind();
			
			//unsigned int output_size = output_tex->GetSize();
			glViewport(0,0,output_size,output_size);
			
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			
			program.UseProgram();
			
			unsigned int i = 0;
			for(auto &input : input_map)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D,input.second->GetTexture());
				
				program.UploadUniformInt(input.first + "_size",input.second->GetSize());
				program.AssignTexture(input.first,i);
				i++;
			}
			
			screen_size_quad.Draw();
		//}
	}
	
	GL4ProcessInstance(GLShaderProgram &p_program,GLQuad &p_screen_size_quad)
		:program(p_program),screen_size_quad(p_screen_size_quad)
	{}
	~GL4ProcessInstance()
	{}
};

#include <TerrainLib/NoiseUtils.h>

class GLStationaryProcessInstance
{
private:
	GL4ProcessInstance covariance_dft_process;
	GL4ProcessInstance complex_process;
	GL4ProcessInstance final_dft_process;
	
	GLTextureRect covariance_matrix;
	GLTextureRect eigenvalue_matrix;
	GLTextureRect complex_probability;
	
	GLFramebuffer eigenvalue_matrix_fb;
	GLFramebuffer complex_probability_fb;
	
public:
	
	GLTextureRect &GetCovarianceMatrix()
	{
		return covariance_matrix;
	}
	
	GLTextureRect &GetEigenvalueMatrix()
	{
		return eigenvalue_matrix;
	}
	
	GLTextureRect &GetComplexProbability()
	{
		return complex_probability;
	}
	
	void SetCovariance(unsigned int resolution,std::function<float(const vec4&,const vec4&)> covariance_function)
	{
		std::vector<float> covariance_matrix_data;
		
		NoiseUtils::GenerateCovarianceMatrix(covariance_matrix_data,resolution,covariance_function);
		
		covariance_matrix.UploadData(GL_R32F,resolution,covariance_matrix_data.data());
		
		eigenvalue_matrix.AllocateStorage(GL_RG32F,resolution);
		complex_probability.AllocateStorage(GL_RG32F,resolution);
		
		eigenvalue_matrix_fb.SetColorAttachment(eigenvalue_matrix.GetTexture());
		complex_probability_fb.SetColorAttachment(complex_probability.GetTexture());
		
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}
	
	void SetNoiseReal(GLTextureRect &real)
	{
		complex_process.SetInput("real",real);
	}
	
	void SetNoiseIm(GLTextureRect &im)
	{
		complex_process.SetInput("imaginary",im);
	}
	/*
	void SetOutput(GLTextureRect &output)
	{
		final_dft_process.SetOutput(output);
	}
	*/
	void Run(GLFramebuffer &output_buffer,unsigned int output_size)
	{
		covariance_dft_process.SetInput("gauss",covariance_matrix);
		//covariance_dft_process.SetOutput(eigenvalue_matrix);
		
		complex_process.SetInput("eigenvalue_matrix",eigenvalue_matrix);
		//complex_process.SetOutput(complex_probability);
		
		final_dft_process.SetInput("gauss",complex_probability);
		
		covariance_dft_process.Run(eigenvalue_matrix_fb,output_size);
		complex_process.Run(complex_probability_fb,output_size);
		final_dft_process.Run(output_buffer,output_size);
	}
	
	GLStationaryProcessInstance(GLShaderProgram &dft_program,GLShaderProgram &dft2_program,GLShaderProgram &complex_program,GLQuad &screen_size_quad)
		:covariance_dft_process(dft_program,screen_size_quad),complex_process(complex_program,screen_size_quad),final_dft_process(dft2_program,screen_size_quad)
	{}
	
	~GLStationaryProcessInstance()
	{}
};

#endif // GL_UTILS_H
