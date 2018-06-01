
#include <DemoFramework/SDL2DemoFramework.h>
#include <DemoFramework/Camera3D.h>

#include <DemoFramework/GL4/GL4DemoBase.h>

#include <TerrainDemo/GLUtils.h>

#include <TerrainLib/GL/Shaders/ImgProcShaderSrc.h>
#include <TerrainLib/GL/Shaders/DftShaderSrc.h>
#include <TerrainLib/GL/Shaders/NoiseShaderSrc.h>

const char *poly_vertex_shader_source = R"(
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
	
	uniform sampler2D noise;
	
	in float height;
	in vec2 tex_coord;
	
	out vec4 fragment_color;
	
	void main()
	{
		float noise_amp = 2.75*height;
		float tex_coord_scale = 7.5;
		float result = height + noise_amp*texture(noise,tex_coord*tex_coord_scale).r;
		
		fragment_color = vec4(result,result,result,1.0);
	}
)";

#include <TerrainLib/LineStrip.h>



class GL4FlatNoiseTerrainDemo : public GL4DemoBase
{
private:
	GLShaderProgram shader_program;
	
	GLShaderProgram dft_program;
	GLShaderProgram dft2_program;
	
	GLShaderProgram diagonal_program;
	
	GLTextureRect noise1;
	GLTextureRect noise2;
	
	GLTextureRect final_noise;
	
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
					float c = -8.0;
					//float c = -10.0;
					//float c = -16.0;
					//float c = -32.0;
					float norm =	std::pow(std::min(std::abs(s[0] - t[0]),1.0f - std::abs(s[0] - t[0])),2.0) +
									std::pow(std::min(std::abs(s[1] - t[1]),1.0f - std::abs(s[1] - t[1])),2.0);
					
					return std::exp(c*std::sqrt(norm));
				}
			);
			
			proc_instance.SetNoiseReal(noise1);
			proc_instance.SetNoiseIm(noise2);
			
			final_noise.AllocateStorage(GL_RG32F,resolution);
			
			proc_instance.SetOutput(final_noise);
			
			proc_instance.Run();
			
			glBindFramebuffer(GL_FRAMEBUFFER,0);
			
			std::vector<vec4> vertices;
			std::vector<vec4> properties;
			std::vector<unsigned int> indices;
			
			test_line_strip.PushBackPoint({0.5,0.8},0.5,0.05);
			test_line_strip.PushBackPoint({0.2,0.2},0.75,0.125);
			test_line_strip.PushBackPoint({0.8,0.2},0.7,0.15);
			test_line_strip.PushBackPoint({0.8,0.5},0.6,0.05);
			
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
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,final_noise.GetTexture());
		
		shader_program.AssignTexture("noise",0);
		
		glBindVertexArray(vertex_array_object);
		
		//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glDrawElements(GL_TRIANGLE_STRIP,num_indices/2,GL_UNSIGNED_INT,0);
		glDrawElements(GL_TRIANGLE_STRIP,num_indices/2,GL_UNSIGNED_INT,(void *)((num_indices/2) * sizeof(unsigned int)));
		
	}
	
	GL4FlatNoiseTerrainDemo()
	{
		GLShader poly_vertex_shader(poly_vertex_shader_source,GL_VERTEX_SHADER);
		//GLShader fragment_shader(linear_fragment_shader_source,GL_FRAGMENT_SHADER);
		GLShader fragment_shader(linear_fragment_shader_source,GL_FRAGMENT_SHADER);
		
		GLuint shaders[2] =
		{
			poly_vertex_shader.GetShader(),
			fragment_shader.GetShader()
		};
		
		shader_program.Initialize(shaders,2);
		
		
		GLShader vertex_shader(ssquad_vertex_shader_source,GL_VERTEX_SHADER);
		GLShader dft_shader(real_dft_fragment_shader_source,GL_FRAGMENT_SHADER);
		GLShader dft2_shader(complex_dft_fragment_shader_source,GL_FRAGMENT_SHADER);
		GLShader diagonal_shader(diagonal_fragment_shader_source,GL_FRAGMENT_SHADER);
		
		GLuint dft_shaders[2] =
		{
			vertex_shader.GetShader(),
			dft_shader.GetShader()
		};
		
		dft_program.Initialize(dft_shaders,2);
		
		GLuint dft2_shaders[2] =
		{
			vertex_shader.GetShader(),
			dft2_shader.GetShader()
		};
		
		dft2_program.Initialize(dft2_shaders,2);
		
		GLuint diagonal_shaders[2] =
		{
			vertex_shader.GetShader(),
			diagonal_shader.GetShader()
		};
		
		diagonal_program.Initialize(diagonal_shaders,2);
		
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
	
	virtual ~GL4FlatNoiseTerrainDemo() override
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
	
	DemoFramework.SetDemo(new GL4FlatNoiseTerrainDemo);
	
	DemoFramework.Loop();
	return 0;
}
