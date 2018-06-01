
#include <DemoFramework/SDL2DemoFramework.h>
#include <DemoFramework/Camera3D.h>

#include <DemoFramework/GL4/GL4DemoBase.h>

#include <TerrainLib/GL/GLUtils.h>

#include <TerrainLib/GL/Shaders/ImgProcShaderSrc.h>
#include <TerrainLib/GL/Shaders/DftShaderSrc.h>

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
		fragment_color = vec4(value.r,value.g,value.b,1.0);
	}
)";

const char *abs_value_fragment_shader_source = R"(
	#version 400
	
	uniform sampler2D gauss;
	uniform int gauss_size;
	
	in vec2 tex_coord;
	
	layout(location = 0) out vec4 fragment_color;
	
	void main()
	{
		vec2 texel_coord = tex_coord*gauss_size;
		vec2 value = texelFetch(gauss,ivec2(texel_coord.x,texel_coord.y),0).rg;
		
		//vec2 value = texture(gauss,tex_coord).rg;
		
		//fragment_color = vec4(abs(value),0.0,1.0);
		//fragment_color = vec4(value*value,0.0,1.0);
		fragment_color = vec4(sqrt(value.r*value.r + value.g*value.g),0.0,0.0,1.0);
	}
)";

#include <vector>
#include <cmath>

class GL4DftDemo : public GL4DemoBase
{
private:
	GLShaderProgram passthrough_program;
	GLShaderProgram dct_program;
	GLShaderProgram dft_program;
	GLShaderProgram dft2_program;
	GLShaderProgram abs_value_program;
	
	GLQuad screen_size_quad;
	
	
	
	GLTextureRect gauss_matrix;
	GLTextureRect gauss_dct;
	GLTextureRect gauss_dft;
	GLTextureRect gauss_dft_abs;
	
	
	
	GLTextureRect sine_matrix;
	GLTextureRect sine_dct;
	GLTextureRect sine_dft;
	GLTextureRect sine_dft_abs;
	
	
	
	GLTextureRect covariance_matrix;
	GLTextureRect eigenvalue_dct_matrix;
	GLTextureRect eigenvalue_dft_matrix;
	GLTextureRect eigenvalue_dft_matrix_abs;
	
	
	
	GLTextureRect rect_matrix;
	GLTextureRect rect_dct;
	GLTextureRect rect_dft;
	GLTextureRect rect_dft_abs;
	
	
	
	GLTextureRect sinc_matrix;
	GLTextureRect sinc_dct;
	GLTextureRect sinc_dft;
	GLTextureRect sinc_dft_abs;
	
	
	GLQuad quad11;
	GLQuad quad21;
	GLQuad quad31;
	GLQuad quad41;
	
	GLQuad quad12;
	GLQuad quad22;
	GLQuad quad32;
	GLQuad quad42;
	
	GLQuad quad13;
	GLQuad quad23;
	GLQuad quad33;
	GLQuad quad43;
	
	GLQuad quad14;
	GLQuad quad24;
	GLQuad quad34;
	GLQuad quad44;
	
	GLQuad quad15;
	GLQuad quad25;
	GLQuad quad35;
	GLQuad quad45;
	
	void InitializeQuad()
	{
		vec4 position = {-1.0,-1.0};
		float screen_width = 2.0;
		float screen_height = 2.0;
		screen_size_quad.Initialize(position,screen_width,screen_height,passthrough_program);
		
		unsigned int num_transforms = 4;
		unsigned int num_test_images = 5;
		
		float width = screen_width/num_transforms;
		float height = screen_height/num_test_images;
		
		vec4 position11 = {-1.0f + 0*width,1.0f - 1*height};
		quad11.Initialize(position11,width,height,passthrough_program);
		
		vec4 position21 = {-1.0f + 1*width,1.0f - 1*height};
		quad21.Initialize(position21,width,height,passthrough_program);
		
		vec4 position31 = {-1.0f + 2*width,1.0f - 1*height};
		quad31.Initialize(position31,width,height,passthrough_program);
		
		vec4 position41 = {-1.0f + 3*width,1.0f - 1*height};
		quad41.Initialize(position41,width,height,passthrough_program);
		
		vec4 position12 = {-1.0f + 0*width,1.0f - 2*height};
		quad12.Initialize(position12,width,height,passthrough_program);
		
		vec4 position22 = {-1.0f + 1*width,1.0f - 2*height};
		quad22.Initialize(position22,width,height,passthrough_program);
		
		vec4 position32 = {-1.0f + 2*width,1.0f - 2*height};
		quad32.Initialize(position32,width,height,passthrough_program);
		
		vec4 position42 = {-1.0f + 3*width,1.0f - 2*height};
		quad42.Initialize(position42,width,height,passthrough_program);
		
		vec4 position13 = {-1.0f + 0*width,1.0f - 3*height};
		quad13.Initialize(position13,width,height,passthrough_program);
		
		vec4 position23 = {-1.0f + 1*width,1.0f - 3*height};
		quad23.Initialize(position23,width,height,passthrough_program);
		
		vec4 position33 = {-1.0f + 2*width,1.0f - 3*height};
		quad33.Initialize(position33,width,height,passthrough_program);
		
		vec4 position43 = {-1.0f + 3*width,1.0f - 3*height};
		quad43.Initialize(position43,width,height,passthrough_program);
		
		vec4 position14 = {-1.0f + 0*width,1.0f - 4*height};
		quad14.Initialize(position14,width,height,passthrough_program);
		
		vec4 position24 = {-1.0f + 1*width,1.0f - 4*height};
		quad24.Initialize(position24,width,height,passthrough_program);
		
		vec4 position34 = {-1.0f + 2*width,1.0f - 4*height};
		quad34.Initialize(position34,width,height,passthrough_program);
		
		vec4 position44 = {-1.0f + 3*width,1.0f - 4*height};
		quad44.Initialize(position44,width,height,passthrough_program);
		
		vec4 position15 = {-1.0f + 0*width,1.0f - 5*height};
		quad15.Initialize(position15,width,height,passthrough_program);
		
		vec4 position25 = {-1.0f + 1*width,1.0f - 5*height};
		quad25.Initialize(position25,width,height,passthrough_program);
		
		vec4 position35 = {-1.0f + 2*width,1.0f - 5*height};
		quad35.Initialize(position35,width,height,passthrough_program);
		
		vec4 position45 = {-1.0f + 3*width,1.0f - 5*height};
		quad45.Initialize(position45,width,height,passthrough_program);
	}
	
	void GenerateTextures(GLTextureRect &input,GLTextureRect &dct,GLTextureRect &dft,GLTextureRect &abs_val)
	{
		dct.AllocateStorage(GL_R32F,input.GetSize());
		
		GL4ProcessInstance dct_process(dct_program,screen_size_quad);
		
		dct_process.SetInput("gauss",input);
		//dct_process.SetOutput(dct);
		
		GLFramebuffer dct_famebuf;
		dct_famebuf.SetColorAttachment(dct.GetTexture());
		
		dct_process.Run(dct_famebuf,dct.GetSize());
		
		dft.AllocateStorage(GL_RG32F,input.GetSize());
		
		GL4ProcessInstance dft_process(dft_program,screen_size_quad);
		
		dft_process.SetInput("gauss",input);
		//dft_process.SetOutput(dft);
		
		GLFramebuffer dft_famebuf;
		dft_famebuf.SetColorAttachment(dft.GetTexture());
		
		dft_process.Run(dft_famebuf,dft.GetSize());
		
		abs_val.AllocateStorage(GL_RG32F,input.GetSize());
		
		GL4ProcessInstance dft_abs_process(abs_value_program,screen_size_quad);
		
		dft_abs_process.SetInput("gauss",dft);
		//dft_abs_process.SetOutput(abs_val);
		
		GLFramebuffer abs_val_famebuf;
		abs_val_famebuf.SetColorAttachment(abs_val.GetTexture());
		
		dft_abs_process.Run(abs_val_famebuf,abs_val.GetSize());
		
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}
	
	void InitializeGauss()
	{
		unsigned int resolution = 128;
		//unsigned int resolution = 256;
		//unsigned int resolution = 512;
		
		std::vector<float> gauss_matrix_data;
		{
			gauss_matrix_data.resize(resolution * resolution);
			
			vec4 center = {0.5,0.5};
			float width = 0.001;
			
			for(int i=0;i < resolution;i++)
			{
				for(int j=0;j < resolution;j++)
				{
					vec4 current_vec = {(i+1.0f)/resolution,(j+1.0f)/resolution};
					
					gauss_matrix_data[j*resolution + i] = std::exp(-dot(current_vec - center,current_vec - center)/width);
				}
			}
		}
		
		gauss_matrix.UploadData(GL_R32F,resolution,gauss_matrix_data.data());
		
		GenerateTextures(gauss_matrix,gauss_dct,gauss_dft,gauss_dft_abs);
		
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}
	
	void InitializeSine()
	{
		unsigned int resolution = 128;
		//unsigned int resolution = 256;
		//unsigned int resolution = 512;
		
		std::vector<float> covariance_matrix_data;
		{
			covariance_matrix_data.resize(resolution * resolution);
			
			for(int i=0;i < resolution;i++)
			{
				for(int j=0;j < resolution;j++)
				{
					float frequency = PI*32;
					vec4 current_vec = {(i+1.0f)/resolution,(j+1.0f)/resolution};
					
					float Amp = 1.0;//0.025;
					
					covariance_matrix_data[j*resolution + i] = Amp*std::cos(frequency * current_vec[0]);
				}
			}
		}
		
		sine_matrix.UploadData(GL_R32F,resolution,covariance_matrix_data.data());
		
		GenerateTextures(sine_matrix,sine_dct,sine_dft,sine_dft_abs);
		
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}
	
	void InitializeSine2()
	{
		unsigned int resolution = 128;
		//unsigned int resolution = 256;
		//unsigned int resolution = 512;
		
		std::vector<float> covariance_matrix_data;
		{
			covariance_matrix_data.resize(resolution * resolution);
			
			for(int i=0;i < resolution;i++)
			{
				for(int j=0;j < resolution;j++)
				{
					float frequency = PI*32;
					vec4 current_vec = {(i+1.0f)/resolution,(j+1.0f)/resolution};
					
					float Amp = 1.0;//0.025;
					
					covariance_matrix_data[j*resolution + i] = Amp*std::cos(frequency * dot(current_vec,{1.0,1.0}));
				}
			}
		}
		
		covariance_matrix.UploadData(GL_R32F,resolution,covariance_matrix_data.data());
		
		GenerateTextures(covariance_matrix,eigenvalue_dct_matrix,eigenvalue_dft_matrix,eigenvalue_dft_matrix_abs);
		
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}
	
	void InitializeRect()
	{
		unsigned int resolution = 128;
		//unsigned int resolution = 256;
		//unsigned int resolution = 512;
		
		std::vector<float> rect_data;
		{
			rect_data.resize(resolution * resolution);
			
			
			float width = 0.125;
			float height = 0.125;
			vec4 center = {0.25,0.75};
			
			for(int i=0;i < resolution;i++)
			{
				for(int j=0;j < resolution;j++)
				{
					vec4 current_vec = {(i+1.0f)/resolution,(j+1.0f)/resolution};
					
					if(	current_vec[0] > (center[0]-width) &&
						current_vec[0] < (center[0]+width) &&
						current_vec[1] > (center[1]-height) &&
						current_vec[1] < (center[1]+height))
					{
						rect_data[j*resolution + i] = 1.0;
					}
					else
					{
						rect_data[j*resolution + i] = 0.0;
					}
				}
			}
		}
		
		rect_matrix.UploadData(GL_R32F,resolution,rect_data.data());
		
		GenerateTextures(rect_matrix,rect_dct,rect_dft,rect_dft_abs);
		
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}
	
	void InitializeSinc()
	{
		unsigned int resolution = 128;
		//unsigned int resolution = 256;
		//unsigned int resolution = 512;
		
		vec4 center = {0.5,0.5};
		
		std::vector<float> covariance_matrix_data;
		{
			covariance_matrix_data.resize(resolution * resolution);
			
			for(int i=0;i < resolution;i++)
			{
				for(int j=0;j < resolution;j++)
				{
					float frequency = PI*32;
					vec4 current_vec = {(i+1.0f)/resolution,(j+1.0f)/resolution};
					
					current_vec -= center;
					
					float sincx = 1.0;
					if(std::abs(current_vec[0]) > 1e-4)
					{
						sincx = std::sin(frequency * current_vec[0])/(frequency * current_vec[0]);
					}
					
					float sincy = 1.0;
					if(std::abs(current_vec[1]) > 1e-4)
					{
						sincy = std::sin(frequency * current_vec[1])/(frequency * current_vec[1]);
					}
					
					float Amp = 25.0;
					
					//covariance_matrix_data[j*resolution + i] = Amp*sincx*sincy;
					covariance_matrix_data[j*resolution + i] = Amp*sincx*sincy*sincx*sincy;
				}
			}
		}
		
		sinc_matrix.UploadData(GL_R32F,resolution,covariance_matrix_data.data());
		
		GenerateTextures(sinc_matrix,sinc_dct,sinc_dft,sinc_dft_abs);
		
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
		glBindTexture(GL_TEXTURE_2D,gauss_matrix.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",gauss_matrix.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad11.Draw();
		
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,gauss_dct.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",gauss_dct.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad21.Draw();
		
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,gauss_dft.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",gauss_dft.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad31.Draw();
		
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,gauss_dft_abs.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",gauss_dft_abs.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad41.Draw();
		
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,sine_matrix.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",sine_matrix.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad12.Draw();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,sine_dct.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",sine_dct.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad22.Draw();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,sine_dft.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",sine_dft.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad32.Draw();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,sine_dft_abs.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",sine_dft_abs.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad42.Draw();
		
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,covariance_matrix.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",covariance_matrix.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad13.Draw();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,eigenvalue_dct_matrix.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",eigenvalue_dct_matrix.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad23.Draw();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,eigenvalue_dft_matrix.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",eigenvalue_dft_matrix.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad33.Draw();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,eigenvalue_dft_matrix_abs.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",eigenvalue_dft_matrix_abs.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad43.Draw();
		
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,rect_matrix.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",rect_matrix.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad14.Draw();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,rect_dct.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",rect_dct.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad24.Draw();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,rect_dft.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",rect_dft.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad34.Draw();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,rect_dft_abs.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",rect_dft_abs.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad44.Draw();
		
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,sinc_matrix.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",sinc_matrix.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad15.Draw();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,sinc_dct.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",sinc_dct.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad25.Draw();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,sinc_dft.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",sinc_dft.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad35.Draw();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,sinc_dft_abs.GetTexture());
		
		passthrough_program.UploadUniformInt("gauss_size",sinc_dft_abs.GetSize());
		passthrough_program.AssignTexture("gauss_map",0);
		
		quad45.Draw();
	}
	
	GL4DftDemo()
	{
		GLShader vertex_shader(ssquad_vertex_shader_source,GL_VERTEX_SHADER);
		GLShader passthrough_fragment_shader(passthrough_fragment_shader_source,GL_FRAGMENT_SHADER);
		GLShader dct_shader(dct_fragment_shader_source,GL_FRAGMENT_SHADER);
		GLShader dft_shader(real_dft_fragment_shader_source,GL_FRAGMENT_SHADER);
		GLShader dft2_shader(complex_dft_fragment_shader_source,GL_FRAGMENT_SHADER);
		GLShader abs_value_fragment_shader(abs_value_fragment_shader_source,GL_FRAGMENT_SHADER);
		
		GLuint passthrough_shaders[2] =
		{
			vertex_shader.GetShader(),
			passthrough_fragment_shader.GetShader()
		};
		passthrough_program.Initialize(passthrough_shaders,2);
		
		GLuint dct_shaders[2] =
		{
			vertex_shader.GetShader(),
			dct_shader.GetShader()
		};
		dct_program.Initialize(dct_shaders,2);
		
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
		
		GLuint abs_value_shaders[2] =
		{
			vertex_shader.GetShader(),
			abs_value_fragment_shader.GetShader()
		};
		abs_value_program.Initialize(abs_value_shaders,2);
		
		InitializeQuad();
		
		InitializeGauss();
		InitializeSine();
		InitializeSine2();
		InitializeRect();
		InitializeSinc();
	}
	
	virtual ~GL4DftDemo() override
	{}
};

int main(int argc,char *argv[])
{
	Camera3D *camera = new Camera3D;
	SDL2DemoFramework DemoFramework(800,600,nullptr,camera,1,10000);
	
	DemoFramework.SetDemo(new GL4DftDemo);
	
	DemoFramework.Loop();
	return 0;
}

