#ifndef GL_LINE_STRIP_H
#define GL_LINE_STRIP_H

#include <DemoFramework/GL4/GLShaderProgram.h>

#include <vector>

class GLLineStrip
{
private:
	GLuint vertex_array_object = 0;
	
	GLuint vertex_buffer	= 0;
	GLuint velocity_buffer	= 0;
	GLuint index_buffer		= 0;
	
	GLsizei num_indices	= 0;
	
public:
	
	void Draw()
	{
		glBindVertexArray(vertex_array_object);
		
		glPatchParameteri(GL_PATCH_VERTICES,2);
		glDrawElements(GL_PATCHES,num_indices,GL_UNSIGNED_INT,0);
	}
	
	void Initialize(GLShaderProgram &shader_program)
	{
		glGenVertexArrays(1,&vertex_array_object);
		glBindVertexArray(vertex_array_object);
		
		glGenBuffers(1,&vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
		
		glEnableVertexAttribArray(shader_program.GetAttribLocation("position"));
		glVertexAttribPointer(shader_program.GetAttribLocation("position"),4,GL_FLOAT,GL_FALSE,0,0);
		
		glGenBuffers(1,&velocity_buffer);
		glBindBuffer(GL_ARRAY_BUFFER,velocity_buffer);
		
		glEnableVertexAttribArray(shader_program.GetAttribLocation("velocity"));
		glVertexAttribPointer(shader_program.GetAttribLocation("velocity"),4,GL_FLOAT,GL_FALSE,0,0);
		
		glGenBuffers(1,&index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,index_buffer);
		
		glBindVertexArray(0);
	}
	
	void UploadData(const std::vector<vec4> &vertices,const std::vector<vec4> &velocities,const std::vector<unsigned int> &indices)
	{
		glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(vec4),vertices.data(),GL_STATIC_DRAW);
		
		glBindBuffer(GL_ARRAY_BUFFER,velocity_buffer);
		glBufferData(GL_ARRAY_BUFFER,velocities.size() * sizeof(vec4),velocities.data(),GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size() * sizeof(unsigned int),indices.data(),GL_STATIC_DRAW);
		
		num_indices = indices.size();
	}
	
	GLLineStrip()
	{}
	
	~GLLineStrip()
	{
		glDeleteVertexArrays(1,&vertex_array_object);
		
		glDeleteBuffers(1,&vertex_buffer);
		glDeleteBuffers(1,&velocity_buffer);
		glDeleteBuffers(1,&index_buffer);
	}
};

#include <TerrainLib/LineStrip.h>

class GLLineStripList
{
private:
	std::vector<GLLineStrip> line_strip_list;
	
public:
	
	void Draw()
	{
		for(auto &line_strip : line_strip_list)
		{
			line_strip.Draw();
		}
	}
	
	void Upload(unsigned int id,LineStrip &src)
	{
		std::vector<vec4> vertices;
		std::vector<vec4> velocities;
		std::vector<unsigned int> indices;
		
		src.ExtractCtrlPoints(vertices,velocities,indices);
		
		line_strip_list[id].UploadData(vertices,velocities,indices);
	}
	
	void Upload(std::vector<LineStrip> &line_strip_list)
	{
		std::size_t i=0;
		for(LineStrip &line_strip : line_strip_list)
		{
			Upload(i,line_strip);
			i++;
		}
	}
	
	void UploadData(unsigned int id,const std::vector<vec4> &vertices,const std::vector<vec4> &velocities,const std::vector<unsigned int> &indices)
	{
		line_strip_list[id].UploadData(vertices,velocities,indices);
	}
	
	void Initialize(GLShaderProgram &shader_program,unsigned int size)
	{
		line_strip_list.resize(size);
		
		for(auto &line_strip : line_strip_list)
		{
			line_strip.Initialize(shader_program);
		}
	}
	
	GLLineStripList()
	{}
};

#endif // GL_LINE_STRIP_H
