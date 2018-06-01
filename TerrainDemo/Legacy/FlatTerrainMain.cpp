
#include <DemoFramework/SDL2DemoFramework.h>
#include <DemoFramework/Camera3D.h>

#include <DemoFramework/GL4/GL4DemoBase.h>
#include <DemoFramework/GL4/GLShaderProgram.h>

const char *vertex_shader_source = R"(
	#version 400
	
	in vec4 position;
	in vec4 properties;
	
	out float height;
	out vec2 tex_coord;
	
	void main()
	{
		vec4 final_pos = 2.0 * position - 1.0;
		
		height = position.z;
		tex_coord = properties.xy;
		
		gl_Position = vec4(final_pos.xy,0.0,1.0); 
	}
)";

const char *linear_fragment_shader_source = R"(
	#version 400
	
	in float height;
	in vec2 tex_coord;
	
	out vec4 fragment_color;
	
	void main()
	{
		//fragment_color = vec4(height,height,height,1.0);
		fragment_color = vec4(tex_coord.x,tex_coord.y/20.0,0.0,1.0);
	}
)";

#include <TerrainLib/LineStrip.h>



class GL4FlatTerrainDemo : public GL4DemoBase
{
private:
	GLShaderProgram shader_program;
	
	LineStrip test_line_strip;
	
	GLuint vertex_array_object;
	GLuint vertex_buffer;
	GLuint property_buffer;
	GLuint index_buffer;
	
	unsigned int num_points;
	std::size_t num_indices;
	
	bool changed = true;
	
public:
	
	virtual void Logic() override
	{
		if(changed)
		{
			std::vector<vec4> vertices;
			std::vector<vec4> properties;
			std::vector<unsigned int> indices;
			
			test_line_strip.PushBackPoint({0.5,0.8},0.5,0.05);
			test_line_strip.PushBackPoint({0.2,0.2},0.75,0.125);
			test_line_strip.PushBackPoint({0.8,0.2},0.6,0.05);
			
			test_line_strip.ExtractMesh(vertices,properties,indices);
			
			glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
			glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(vec4),vertices.data(),GL_STATIC_DRAW);
			
			glBindBuffer(GL_ARRAY_BUFFER,property_buffer);
			glBufferData(GL_ARRAY_BUFFER,properties.size() * sizeof(vec4),properties.data(),GL_STATIC_DRAW);
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,index_buffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size() * sizeof(unsigned int),indices.data(),GL_STATIC_DRAW);
			
			num_points = vertices.size();
			num_indices = indices.size();
			
			changed = false;
		}
	}
	
	virtual void Draw() override
	{
		shader_program.UseProgram();
		
		glPointSize(3.0);
		
		glBindVertexArray(vertex_array_object);
		glDrawArrays(GL_POINTS,0,num_points);
		
		//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glDrawElements(GL_TRIANGLE_STRIP,num_indices/2,GL_UNSIGNED_INT,0);
		glDrawElements(GL_TRIANGLE_STRIP,num_indices/2,GL_UNSIGNED_INT,(void *)((num_indices/2) * sizeof(unsigned int)));
		
	}
	
	GL4FlatTerrainDemo()
	{
		GLShader vertex_shader(vertex_shader_source,GL_VERTEX_SHADER);
		//GLShader fragment_shader(linear_fragment_shader_source,GL_FRAGMENT_SHADER);
		GLShader fragment_shader(linear_fragment_shader_source,GL_FRAGMENT_SHADER);
		
		GLuint shaders[2] =
		{
			vertex_shader.GetShader(),
			fragment_shader.GetShader()
		};
		
		shader_program.Initialize(shaders,2);
		
		glGenVertexArrays(1,&vertex_array_object);
		glBindVertexArray(vertex_array_object);
		
		glGenBuffers(1,&vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
		
		glEnableVertexAttribArray(shader_program.GetAttribLocation("position"));
		glVertexAttribPointer(shader_program.GetAttribLocation("position"),4,GL_FLOAT,GL_FALSE,0,0);
		
		glGenBuffers(1,&property_buffer);
		glBindBuffer(GL_ARRAY_BUFFER,property_buffer);
		
		glEnableVertexAttribArray(shader_program.GetAttribLocation("properties"));
		glVertexAttribPointer(shader_program.GetAttribLocation("properties"),4,GL_FLOAT,GL_FALSE,0,0);
		
		glGenBuffers(1,&index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,index_buffer);
		
		glBindVertexArray(0);
	}
	
	virtual ~GL4FlatTerrainDemo() override
	{
		glDeleteVertexArrays(1,&vertex_array_object);
		glDeleteBuffers(1,&vertex_buffer);
		glDeleteBuffers(1,&property_buffer);
		glDeleteBuffers(1,&index_buffer);
	}
};

int main(int argc,char *argv[])
{
	Camera3D *camera = new Camera3D;
	SDL2DemoFramework DemoFramework(800,600,nullptr,camera,1,10000);
	
	DemoFramework.SetDemo(new GL4FlatTerrainDemo);
	
	DemoFramework.Loop();
	return 0;
}
