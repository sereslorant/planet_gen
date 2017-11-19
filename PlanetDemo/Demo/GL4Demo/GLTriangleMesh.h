#ifndef GL_TRIANGLE_MESH_H
#define GL_TRIANGLE_MESH_H

#include "GLShaderProgram.h"

#include <PlanetLib/MeshGenerator/HeightmapSampler.h>

class GLTriangleMesh
{
private:
	GLuint vertex_array_object;
	
	GLuint vertex_buffer;
	GLuint normal_buffer;
	
	unsigned int num_indices;
	GLuint index_buffer;
	
public:
	
	void Initialize(GLShaderProgram &shader_program)
	{
		glGenVertexArrays(1,&vertex_array_object);
		glBindVertexArray(vertex_array_object);
		
		glGenBuffers(1,&vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
		
		glEnableVertexAttribArray(shader_program.GetAttribLocation("position"));
		glVertexAttribPointer(shader_program.GetAttribLocation("position"),4,GL_FLOAT,GL_FALSE,0,0);
		
		glGenBuffers(1,&normal_buffer);
		glBindBuffer(GL_ARRAY_BUFFER,normal_buffer);
		
		glEnableVertexAttribArray(shader_program.GetAttribLocation("normal"));
		glVertexAttribPointer(shader_program.GetAttribLocation("normal"),4,GL_FLOAT,GL_FALSE,0,0);
		
		glGenBuffers(1,&index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,index_buffer);
		
		glBindVertexArray(0);
	}
	
	void Fill(const StaticMesh &static_mesh)
	{
		glBindVertexArray(vertex_array_object);
		
		glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER,static_mesh.vertex_array.size() * sizeof(vec4),static_mesh.vertex_array.data(),GL_STATIC_DRAW);
		
		glBindBuffer(GL_ARRAY_BUFFER,normal_buffer);
		glBufferData(GL_ARRAY_BUFFER,static_mesh.normal_array.size() * sizeof(vec4),static_mesh.normal_array.data(),GL_STATIC_DRAW);
		
		num_indices = static_mesh.index_array.size();
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,static_mesh.index_array.size()*sizeof(unsigned int),static_mesh.index_array.data(),GL_STATIC_DRAW);
		
		glBindVertexArray(0);
	}
	
	void Draw()
	{
		glBindVertexArray(vertex_array_object);
		glDrawElements(GL_TRIANGLES,num_indices,GL_UNSIGNED_INT,0);
	}
	
	GLTriangleMesh()
	{}
	
	~GLTriangleMesh()
	{
		glDeleteVertexArrays(1,&vertex_array_object);
		
		glDeleteBuffers(1,&vertex_buffer);
		glDeleteBuffers(1,&normal_buffer);
		
		glDeleteBuffers(1,&index_buffer);
	}
};

#endif // GL_TRIANGLE_MESH_H
