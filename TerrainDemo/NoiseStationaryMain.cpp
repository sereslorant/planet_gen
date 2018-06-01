
#include <DemoFramework/SDL2DemoFramework.h>
#include <DemoFramework/Camera3D.h>

#include <DemoFramework/GL4/GL4DemoBase.h>

#include <TerrainLib/GL/GLUtils.h>

#include <TerrainLib/GL/Shaders/ImgProcShaderSrc.h>
#include <TerrainLib/GL/Shaders/DftShaderSrc.h>
#include <TerrainLib/GL/Shaders/NoiseShaderSrc.h>

const char *passthrough_fragment_shader_source = R"(
	#version 400
	
	uniform sampler2D gauss;
	uniform int gauss_size;
	
	in vec2 tex_coord;
	
	out vec4 fragment_color;
	
	void main()
	{
		vec3 value = texture(gauss,tex_coord).rgb;
		//value = 0.5*value + 0.5;
		//value = 0.85*value + 0.5;
		
		//fragment_color = vec4(value.r,value.r,value.r,1.0);
		//fragment_color = vec4(value.g,value.g,value.g,1.0);
		//fragment_color = vec4(value*value,1.0);
		//fragment_color = vec4(sqrt(dot(value,value)),0.0,0.0,1.0);
		fragment_color = vec4(value.r,value.g,value.b,1.0);
	}
)";

#include <cmath>

class GL4NoiseDemo : public GL4DemoBase
{
private:
	GLShaderProgram passthrough_program;
	
	GLShaderProgram dft_program;
	GLShaderProgram dft2_program;
	
	GLShaderProgram diagonal_program;
	
	GLStationaryProcessInstance proc1_instance;
	GLStationaryProcessInstance proc2_instance;
	
	
	GLQuad screen_size_quad;
	
	
	
	GLTextureRect noise1;
	GLTextureRect noise2;
	
	GLTextureRect covariance_matrix1;
	GLTextureRect eigenvalue_dft_matrix1;
	GLTextureRect complex_probability1;
	GLTextureRect complex_probability_final1;
	
	GLTextureRect covariance_matrix2;
	GLTextureRect eigenvalue_dft_matrix2;
	GLTextureRect complex_probability2;
	GLTextureRect complex_probability_final2;
	
	
	GLQuad quad11;
	GLQuad quad12;
	GLQuad quad21;
	GLQuad quad22;
	GLQuad quad31;
	GLQuad quad32;
	GLQuad quad41;
	GLQuad quad42;
	GLQuad quad51;
	GLQuad quad52;
	
	void InitializeQuad()
	{
		vec4 position11 = {-1.0,0.5};
		float width11 = 0.5;
		float height11 = 0.5;
		quad11.Initialize(position11,width11,height11,passthrough_program);
		vec4 position12 = {-1.0,0.0};
		float width12 = 0.5;
		float height12 = 0.5;
		quad12.Initialize(position12,width12,height12,passthrough_program);
		vec4 position21 = {-0.5,0.5};
		float width21 = 0.5;
		float height21 = 0.5;
		quad21.Initialize(position21,width21,height21,passthrough_program);
		vec4 position22 = {-0.5,0.0};
		float width22 = 0.5;
		float height22 = 0.5;
		quad22.Initialize(position22,width22,height22,passthrough_program);
		vec4 position31 = {0.0,0.5};
		float width31 = 0.5;
		float height31 = 0.5;
		quad31.Initialize(position31,width31,height31,passthrough_program);
		vec4 position32 = {0.0,0.0};
		float width32 = 0.5;
		float height32 = 0.5;
		quad32.Initialize(position32,width32,height32,passthrough_program);
		vec4 position41 = {0.5,0.5};
		float width41 = 0.5;
		float height41 = 0.5;
		quad41.Initialize(position41,width41,height41,passthrough_program);
		vec4 position42 = {0.5,0.0};
		float width42 = 0.5;
		float height42 = 0.5;
		quad42.Initialize(position42,width42,height42,passthrough_program);
		
		vec4 position51 = {-1.0,-1.0};
		float width51 = 1.0;
		float height51 = 1.0;
		quad51.Initialize(position51,width51,height51,passthrough_program);
		
		vec4 position52 = {0.0,-1.0};
		float width52 = 1.0;
		float height52 = 1.0;
		quad52.Initialize(position52,width52,height52,passthrough_program);
		
		vec4 position = {-1.0,-1.0};
		float width = 2.0;
		float height = 2.0;
		screen_size_quad.Initialize(position,width,height,passthrough_program);
	}
	
	void InitializeInput()
	{
		//unsigned int resolution = 128;
		unsigned int resolution = 256;
		//unsigned int resolution = 512;
		
		
		
		std::vector<float> noise1_data;
		NoiseUtils::GenerateGaussianNoise(noise1_data,resolution);
		
		noise1.UploadData(GL_R32F,resolution,noise1_data.data());
		
		std::vector<float> noise2_data;
		NoiseUtils::GenerateGaussianNoise(noise2_data,resolution,10000);
		
		noise2.UploadData(GL_R32F,resolution,noise2_data.data());
	}
	
	void InitializeProc1()
	{
		//unsigned int resolution = 128;
		unsigned int resolution = 256;
		//unsigned int resolution = 512;
		
		proc1_instance.SetCovariance(resolution,
			[resolution](const vec4&s,const vec4&t)
			{
				vec4 h = (s - t)*resolution;
				
				/*
				vec4 h =
				{
					std::min(std::abs(s[0] - t[0]),1.0f - std::abs(s[0] - t[0])),
					std::min(std::abs(s[1] - t[1]),1.0f - std::abs(s[1] - t[1])),
				};
				h *= resolution;
				 */
				
				float x_divisor = 50.0;
				float y_divisor = 15.0;
				
				float A = 1.0 - h[0]*h[0]/(x_divisor*x_divisor) - h[0]*h[1]/(x_divisor*y_divisor) - h[1]*h[1]/(y_divisor*y_divisor);
				return A * std::exp(-h[0]*h[0]/(x_divisor*x_divisor) - h[1]*h[1]/(y_divisor*y_divisor));
			}
		);
		
		proc1_instance.SetNoiseReal(noise1);
		proc1_instance.SetNoiseIm(noise2);
		
		complex_probability_final1.AllocateStorage(GL_RG32F,resolution);
		
		//proc1_instance.SetOutput(complex_probability_final1);
		
		GLFramebuffer complex_prob_final1_fb;
		complex_prob_final1_fb.SetColorAttachment(complex_probability_final1.GetTexture());
		
		proc1_instance.Run(complex_prob_final1_fb,complex_probability_final1.GetSize());
		
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}
	
	void InitializeProc2()
	{
		//unsigned int resolution = 128;
		unsigned int resolution = 256;
		//unsigned int resolution = 512;
		
		proc2_instance.SetCovariance(resolution,
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
		
		proc2_instance.SetNoiseReal(noise1);
		proc2_instance.SetNoiseIm(noise2);
		
		complex_probability_final2.AllocateStorage(GL_RG32F,resolution);
		
		//proc2_instance.SetOutput(complex_probability_final2);
		
		GLFramebuffer complex_prob_final2_fb;
		complex_prob_final2_fb.SetColorAttachment(complex_probability_final2.GetTexture());
		
		proc2_instance.Run(complex_prob_final2_fb,complex_probability_final2.GetSize());
		
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}
	
public:
	
	virtual void Logic() override
	{}
	
	virtual void Draw() override
	{
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport(0,0,800,600);
		
		passthrough_program.UseProgram();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,noise1.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",noise1.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad11.Draw();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,noise2.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",noise2.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad12.Draw();
		
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D,covariance_matrix1.GetTexture());
		glBindTexture(GL_TEXTURE_2D,proc2_instance.GetCovarianceMatrix().GetTexture());
		
		//passthrough_program.UploadUniformInt("gauss_size",covariance_matrix1.GetSize());
		passthrough_program.UploadUniformInt("gauss_size",proc2_instance.GetCovarianceMatrix().GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad21.Draw();
		
		
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D,eigenvalue_dft_matrix1.GetTexture());
		glBindTexture(GL_TEXTURE_2D,proc2_instance.GetEigenvalueMatrix().GetTexture());
		
		//passthrough_program.UploadUniformInt("gauss_size",eigenvalue_dft_matrix1.GetSize());
		passthrough_program.UploadUniformInt("gauss_size",proc2_instance.GetEigenvalueMatrix().GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad31.Draw();
		
		
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D,complex_probability1.GetTexture());
		glBindTexture(GL_TEXTURE_2D,proc2_instance.GetComplexProbability().GetTexture());
		
		//passthrough_program.UploadUniformInt("gauss_size",complex_probability1.GetSize());
		passthrough_program.UploadUniformInt("gauss_size",proc2_instance.GetComplexProbability().GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad41.Draw();
		
		
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D,covariance_matrix2.GetTexture());
		glBindTexture(GL_TEXTURE_2D,proc1_instance.GetCovarianceMatrix().GetTexture());
		
		//passthrough_program.UploadUniformInt("gauss_size",covariance_matrix2.GetSize());
		passthrough_program.UploadUniformInt("gauss_size",proc1_instance.GetCovarianceMatrix().GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad22.Draw();
		
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D,eigenvalue_dft_matrix2.GetTexture());
		glBindTexture(GL_TEXTURE_2D,proc1_instance.GetEigenvalueMatrix().GetTexture());
		
		//passthrough_program.UploadUniformInt("gauss_size",eigenvalue_dft_matrix2.GetSize());
		passthrough_program.UploadUniformInt("gauss_size",proc1_instance.GetEigenvalueMatrix().GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad32.Draw();
		
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D,complex_probability2.GetTexture());
		glBindTexture(GL_TEXTURE_2D,proc1_instance.GetComplexProbability().GetTexture());
		
		//passthrough_program.UploadUniformInt("gauss_size",complex_probability2.GetSize());
		passthrough_program.UploadUniformInt("gauss_size",proc1_instance.GetComplexProbability().GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad42.Draw();
		
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,complex_probability_final1.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",complex_probability_final1.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad51.Draw();
		
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,complex_probability_final2.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",complex_probability_final2.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad52.Draw();
	}
	
	GL4NoiseDemo()
		:proc1_instance(dft_program,dft2_program,diagonal_program,screen_size_quad),
		 proc2_instance(dft_program,dft2_program,diagonal_program,screen_size_quad)
	{
		GLShader vertex_shader(ssquad_vertex_shader_source,GL_VERTEX_SHADER);
		GLShader passthrough_fragment_shader(passthrough_fragment_shader_source,GL_FRAGMENT_SHADER);
		GLShader dft_shader(real_dft_fragment_shader_source,GL_FRAGMENT_SHADER);
		GLShader dft2_shader(complex_dft_fragment_shader_source,GL_FRAGMENT_SHADER);
		GLShader diagonal_shader(diagonal_fragment_shader_source,GL_FRAGMENT_SHADER);
		
		GLuint passthrough_shaders[2] =
		{
			vertex_shader.GetShader(),
			passthrough_fragment_shader.GetShader()
		};
		
		passthrough_program.Initialize(passthrough_shaders,2);
		
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
		
		InitializeQuad();
		InitializeInput();
		InitializeProc1();
		InitializeProc2();
	}
	
	virtual ~GL4NoiseDemo() override
	{}
};

int main(int argc,char *argv[])
{
	Camera3D *camera = new Camera3D;
	SDL2DemoFramework DemoFramework(800,600,nullptr,camera,1,10000);
	
	DemoFramework.SetDemo(new GL4NoiseDemo);
	
	DemoFramework.Loop();
	return 0;
}
