#ifndef GL_SHADER_PROGRAM_H
#define GL_SHADER_PROGRAM_H

#include "GLExt.h"

#include <iostream>

class GLShader
{
private:
	GLuint shader;
	
	static void PrintShaderCompileError(GLuint shader)
	{
		GLint compiled;
		
		glGetObjectParameterivARB(shader,GL_COMPILE_STATUS,&compiled);
		if(!compiled)
		{
			GLint ret_size;
			char log[256];
			glGetInfoLogARB(shader,sizeof(log),&ret_size,log);
			
			std::cerr << log << std::endl;
		}
	}

	static GLuint CompileShader(const char *shader_source,GLenum shader_type)
	{
		GLuint shader = glCreateShader(shader_type);
		glShaderSource(shader,1,&shader_source,nullptr);
		glCompileShader(shader);
		
		PrintShaderCompileError(shader);
		
		return shader;
	}
	
public:
	
	GLuint GetShader()
	{
		return shader;
	}
	
	GLShader(const char *shader_source,GLenum shader_type)
	{
		shader = CompileShader(shader_source,shader_type);
	}
	
	~GLShader()
	{
		glDeleteShader(shader);
	}
};

class GLShaderProgram
{
private:
	GLuint shader_program = 0;
	
public:
	
	void UploadUniformMatrix(const std::string &uniform_name,const mat4 &matrix)
	{
		GLint uniform_location = glGetUniformLocation(shader_program,uniform_name.c_str());
		
		glUniformMatrix4fv(uniform_location,1,GL_FALSE,matrix.m[0]);
	}
	
	void UploadUniformVector(const std::string &uniform_name,const vec4 &vec)
	{
		GLint uniform_location = glGetUniformLocation(shader_program,uniform_name.c_str());
		
		glUniform4fv(uniform_location,1,vec.v);
	}
	
	void UploadUniformFloat(const std::string &uniform_name,float f)
	{
		GLint uniform_location = glGetUniformLocation(shader_program,uniform_name.c_str());
		
		glUniform1f(uniform_location,f);
	}
	
	void AssignTexture(const std::string &uniform_name,GLint texture_id)
	{
		GLint uniform_location = glGetUniformLocation(shader_program,uniform_name.c_str());
		
		glUniform1i(uniform_location,texture_id);
	}
	
	GLint GetAttribLocation(const std::string &attrib_name)
	{
		return glGetAttribLocation(shader_program,attrib_name.c_str());
	}
	
	void UseProgram()
	{
		glUseProgram(shader_program);
	}
	
	void Initialize(GLuint shaders[],unsigned int size)
	{
		shader_program = glCreateProgram();
		
		for(int i=0;i < size;i++)
			{glAttachShader(shader_program,shaders[i]);}
		
		glLinkProgram(shader_program);
		
		for(int i=0;i < size;i++)
			{glDetachShader(shader_program,shaders[i]);}
		
		GLint success;
		glGetProgramiv(shader_program,GL_LINK_STATUS,&success);
		if (success == 0) {
			char log[256];
			glGetProgramInfoLog(shader_program, sizeof(log), nullptr, log);
			std::cerr << log << std::endl;
		}
	}
	
	GLShaderProgram()
	{}
	
	~GLShaderProgram()
	{
		glDeleteProgram(shader_program);
	}
};


#endif // GL_SHADER_PROGRAM_H
