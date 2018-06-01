
#include <DemoFramework/SDL2DemoFramework.h>
#include <DemoFramework/Camera3D.h>

#include <DemoFramework/GL4/GL4DemoBase.h>

#include <TerrainLib/GL/GLLineStrip.h>

#include <TerrainLib/GL/GLTerrainGenCommand.h>

#include <TerrainLib/GL/Shaders/ImgProcShaderSrc.h>
#include <TerrainLib/GL/Shaders/DftShaderSrc.h>
#include <TerrainLib/GL/Shaders/NoiseShaderSrc.h>
#include <TerrainLib/GL/Shaders/CurveShaders.h>

#include <TerrainLib/LineStrip.h>

class GL4ParamSurfDemo : public GL4DemoBase
{
private:
	GLTextureRect final_noise;
	
	GLShaderProgram shader_program;
	GLLineStripList line_strip_list;
	
public:
	
	virtual void Logic() override
	{
		
	}
	
	virtual void Draw() override
	{
		GLRenderProcess render_process(shader_program);
		
		render_process.SetNoise(&final_noise);
		
		render_process.SetLineStripList(&line_strip_list);
		
		render_process.SetInterpolationCurveBegin({0.0,0.0,0.0,0.0});
		render_process.SetInterpolationCurveEnd({1.0,0.0,0.0,0.0});
		
		render_process.Execute();
	}
	
	GL4ParamSurfDemo()
	{
		/*
		 * Generating noise map
		 */
		
		GLShader ssquad_vertex_shader(ssquad_vertex_shader_source,GL_VERTEX_SHADER);
		GLShader dft_shader(real_dft_fragment_shader_source,GL_FRAGMENT_SHADER);
		GLShader dft2_shader(complex_dft_fragment_shader_source,GL_FRAGMENT_SHADER);
		GLShader diagonal_shader(diagonal_fragment_shader_source,GL_FRAGMENT_SHADER);
		
		GLuint dft_shaders[2] =
		{
			ssquad_vertex_shader.GetShader(),
			dft_shader.GetShader()
		};
		
		GLShaderProgram dft_program;
		dft_program.Initialize(dft_shaders,2);
		
		GLuint dft2_shaders[2] =
		{
			ssquad_vertex_shader.GetShader(),
			dft2_shader.GetShader()
		};
		
		GLShaderProgram dft2_program;
		dft2_program.Initialize(dft2_shaders,2);
		
		GLuint diagonal_shaders[2] =
		{
			ssquad_vertex_shader.GetShader(),
			diagonal_shader.GetShader()
		};
		
		GLShaderProgram diagonal_program;
		diagonal_program.Initialize(diagonal_shaders,2);
		
		{
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
			
			final_noise.AllocateStorage(GL_RG32F,resolution);
			
			//proc_instance.SetOutput(final_noise);
			
			GLFramebuffer final_noise_fb;
			final_noise_fb.SetColorAttachment(final_noise.GetTexture());
			
			proc_instance.Run(final_noise_fb,final_noise.GetSize());
			
			glBindFramebuffer(GL_FRAMEBUFFER,0);
		}
		
		/*
		 * Initializing line strip and shaders
		 */
		
		GLShader vertex_shader(curve_vertex_shader_source,GL_VERTEX_SHADER);
		GLShader tc_shader(curve_tc_shader_source,GL_TESS_CONTROL_SHADER);
		GLShader te_shader(curve_te_shader_source,GL_TESS_EVALUATION_SHADER);
		GLShader fragment_shader(curve_fragment_shader_source,GL_FRAGMENT_SHADER);
		
		GLuint shaders[4] =
		{
			vertex_shader.GetShader(),
			tc_shader.GetShader(),
			te_shader.GetShader(),
			fragment_shader.GetShader()
		};
		
		shader_program.Initialize(shaders,4);
		
		LineStrip test_line_strip;
		
		test_line_strip.PushBackPoint({0.5,0.8},0.5,0.1);
		test_line_strip.PushBackPoint({0.2,0.2},0.75,0.25);
		test_line_strip.PushBackPoint({0.8,0.2},0.7,0.15);
		test_line_strip.PushBackPoint({0.8,0.5},0.6,0.1);
		
		line_strip_list.Initialize(shader_program,1);
		line_strip_list.Upload(0,test_line_strip);
	}
	
	virtual ~GL4ParamSurfDemo() override
	{}
};

int main(int argc,char *argv[])
{
	Camera3D *camera = new Camera3D;
	SDL2DemoFramework DemoFramework(800,600,nullptr,camera,1,10000);
	
	DemoFramework.SetDemo(new GL4ParamSurfDemo);
	
	DemoFramework.Loop();
	return 0;
}
