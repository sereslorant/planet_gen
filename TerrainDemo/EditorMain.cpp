
#include <DemoFramework/SDL2DemoFramework.h>
#include <DemoFramework/Camera3D.h>

#include <DemoFramework/GL4/GL4DemoBase.h>

#include <TerrainLib/GL/GLUtils.h>

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

#include <TerrainLib/GL/GLTerrainGenCommand.h>

#include <TerrainLib/GL/Shaders/ImgProcShaderSrc.h>
#include <TerrainLib/GL/Shaders/DftShaderSrc.h>
#include <TerrainLib/GL/Shaders/NoiseShaderSrc.h>
#include <TerrainLib/GL/Shaders/CurveShaders.h>

class TestShaderLibrary
{
private:
	GLShaderProgram dft_program;
	GLShaderProgram dft2_program;
	GLShaderProgram diagonal_program;
	
public:
	
	StationaryGaussianCommand::Programs GetStationaryGaussianPrograms()
	{
		return {dft_program,dft2_program,diagonal_program};
	}
	
	void Initialize()
	{
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
	}
};

class GL4ImageDemo : public GL4DemoBase
{
private:
	GLShaderProgram passthrough_program;
	
	GLShaderProgram shader_program_fb;
	
	std::vector<LineStrip> test_line_strip_list1;
	std::vector<LineStrip> test_line_strip_list2;
	std::vector<LineStrip> test_line_strip_list3;
	
	GLQuad screen_size_quad;
	
	TestShaderLibrary test_shader_library;
	
	std::unique_ptr<GLTerrainGenerator> terrain_generator;
	
	static constexpr unsigned int NUM_TEXTURES = 4;
	
	GLTextureRect *textures[NUM_TEXTURES];
	GLQuad quads[NUM_TEXTURES];
	
	enum EditState
	{
		ADDING_ELEMENT,
		MODIFYING_PROCESS,
		ADDING_CONTROL_POINT,
		MODIFYING_CONTROL_POINT,
		REMOVING_CONTROL_POINT,
		//ADDING_POLYGON,
		//REMOVING_POLYGON,
		REMOVING_ELEMENT,
	};
	
	EditState edit_state = ADDING_ELEMENT;
	
	unsigned int timer = 0;
	const unsigned int TIMER_MAX = 100;
	
	void InitializeQuad()
	{
		vec4 position = {-1.0,-1.0};
		float screen_width = 2.0;
		float screen_height = 2.0;
		screen_size_quad.Initialize(position,screen_width,screen_height,passthrough_program);
		
		unsigned int num_transforms = 2;
		unsigned int num_test_images = 2;
		
		float width = screen_width/num_transforms;
		float height = screen_height/num_test_images;
		
		vec4 position11 = {-1.0f + 0*width,1.0f - 1*height};
		quads[0].Initialize(position11,width,height,passthrough_program);
		
		vec4 position21 = {-1.0f + 1*width,1.0f - 1*height};
		quads[1].Initialize(position21,width,height,passthrough_program);
		
		vec4 position12 = {-1.0f + 0*width,1.0f - 2*height};
		quads[2].Initialize(position12,width,height,passthrough_program);
		
		vec4 position22 = {-1.0f + 1*width,1.0f - 2*height};
		quads[3].Initialize(position22,width,height,passthrough_program);
	}
	
public:
	
	virtual void Logic() override
	{
		if(edit_state == MODIFYING_CONTROL_POINT)
		{
			GLLineStripList *line_strip_list = &terrain_generator->GetTerrainPipeline(0).GetLineStripList(&test_line_strip_list3);
			
			test_line_strip_list3[0].GetPoint(2).position[0] = 1.0 - float(timer)/TIMER_MAX;
			
			line_strip_list->Upload(0,test_line_strip_list3[0]);
			
			terrain_generator->commands[3]->Generate();
			
			glBindFramebuffer(GL_FRAMEBUFFER,0);
			
			if(timer == TIMER_MAX)
			{
				terrain_generator->GetTerrainPipeline(0).GetElement(2).SetBlendFunc(GL_FUNC_REVERSE_SUBTRACT);
				edit_state = REMOVING_CONTROL_POINT;
				timer = 0;
			}
		}
		else if(timer == TIMER_MAX)
		{
			GLRenderProcess *render_process = nullptr;
			GLLineStripList *line_strip_list = nullptr;
			
			switch(edit_state)
			{
			case ADDING_ELEMENT:
				render_process = new GLRenderProcess(shader_program_fb);
				
				render_process->SetNoise(&terrain_generator->GetTexture(2));
				
				render_process->SetLineStripList(&terrain_generator->GetTerrainPipeline(0).GetLineStripList(&test_line_strip_list3));
				
				render_process->SetInterpolationCurveBegin({0.0,0.0,0.0,0.0});
				render_process->SetInterpolationCurveEnd({1.0,0.0,0.0,0.0});
				
				terrain_generator->GetTerrainPipeline(0).InsertElement(2,render_process);
				
				edit_state = MODIFYING_PROCESS;
				break;
			case MODIFYING_PROCESS:
				render_process = &terrain_generator->GetTerrainPipeline(0).GetElement(2);
				
				render_process->SetInterpolationCurveBegin({0.0,0.0,0.0,0.0});
				render_process->SetInterpolationCurveEnd({0.0,0.0,0.0,0.0});
				
				edit_state = ADDING_CONTROL_POINT;
				break;
			case ADDING_CONTROL_POINT:
				line_strip_list = &terrain_generator->GetTerrainPipeline(0).GetLineStripList(&test_line_strip_list3);
				
				test_line_strip_list3[0].PushBackPoint({1.0,0.0},0.75,0.25);
				
				line_strip_list->Upload(0,test_line_strip_list3[0]);
				
				edit_state = MODIFYING_CONTROL_POINT;
				break;
			/*case MODIFYING_CONTROL_POINT:
				line_strip_list = &terrain_generator->GetTerrainPipeline(0).GetLineStripList(&test_line_strip_list3);
				
				test_line_strip_list3[0].GetPoint(2).position[0] = 0.0;
				
				line_strip_list->Upload(0,test_line_strip_list3[0]);
				
				edit_state = REMOVING_CONTROL_POINT;*/
				break;
			case REMOVING_CONTROL_POINT:
				line_strip_list = &terrain_generator->GetTerrainPipeline(0).GetLineStripList(&test_line_strip_list3);
				
				test_line_strip_list3[0].RemovePoint(2);
				
				line_strip_list->Upload(0,test_line_strip_list3[0]);
				
				edit_state = REMOVING_ELEMENT;
				break;
			/*
			case ADDING_POLYGON:
				line_strip_list = &terrain_generator->GetTerrainPipeline(0).GetLineStripList(&test_line_strip_list3);
				
				test_line_strip_list3[0].GetPoint(2).position[0] = 0.0;
				
				line_strip_list->Upload(0,test_line_strip_list3[0]);
				
				edit_state = REMOVING_CONTROL_POINT;
				break;
			case REMOVING_POLYGON:
				line_strip_list = &terrain_generator->GetTerrainPipeline(0).GetLineStripList(&test_line_strip_list3);
				
				test_line_strip_list3[0].RemovePoint(2);
				
				line_strip_list->Upload(0,test_line_strip_list3[0]);
				
				edit_state = REMOVING_ELEMENT;
				break;
			*/
			case REMOVING_ELEMENT:
				terrain_generator->GetTerrainPipeline(0).RemoveElement(2);
				edit_state = ADDING_ELEMENT;
			}
			
			terrain_generator->Execute();
			
			glBindFramebuffer(GL_FRAMEBUFFER,0);
			
			timer = 0;
		}
		
		timer++;
	}
	
	virtual void Draw() override
	{
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport(0,0,800,600);
		
		glDisable(GL_BLEND);
		
		passthrough_program.UseProgram();
		
		glActiveTexture(GL_TEXTURE0);
		
		for(unsigned int i=0;i < NUM_TEXTURES;i++)
		{
			glBindTexture(GL_TEXTURE_2D,textures[i]->GetTexture());
			
			passthrough_program.UploadUniformInt("gauss_size",textures[i]->GetSize());
			passthrough_program.AssignTexture("gauss_map",0);
			
			quads[i].Draw();
		}
	}
	
	GL4ImageDemo()
	{
		test_shader_library.Initialize();
		
		GLShader vertex_shader(ssquad_vertex_shader_source,GL_VERTEX_SHADER);
		GLShader passthrough_fragment_shader(passthrough_fragment_shader_source,GL_FRAGMENT_SHADER);
		
		GLuint passthrough_shaders[2] =
		{
			vertex_shader.GetShader(),
			passthrough_fragment_shader.GetShader()
		};
		passthrough_program.Initialize(passthrough_shaders,2);
		
		
		InitializeQuad();
		
		
		
		GLShader curve_vertex_shader(curve_vertex_shader_source,GL_VERTEX_SHADER);
		GLShader tc_shader(curve_tc_shader_source,GL_TESS_CONTROL_SHADER);
		GLShader te_shader(curve_te_shader_source,GL_TESS_EVALUATION_SHADER);
		GLShader curve_fragment_shader(curve_fragment_shader_source,GL_FRAGMENT_SHADER);
		
		GLuint curve_shaders[4] =
		{
			curve_vertex_shader.GetShader(),
			tc_shader.GetShader(),
			te_shader.GetShader(),
			curve_fragment_shader.GetShader()
		};
		
		shader_program_fb.Initialize(curve_shaders,4);
		
		terrain_generator = std::unique_ptr<GLTerrainGenerator>(new GLTerrainGenerator(shader_program_fb));
		
		GaussianCommand *command1 = new GaussianCommand(terrain_generator->GetTexture(0));
		
		//command1.Generate();
		terrain_generator->commands.push_back(std::unique_ptr<GLTerrainGenCommand>{command1});
		
		GaussianCommand *command2 = new GaussianCommand(terrain_generator->GetTexture(1));
		command2->seed = 1000;
		
		//command2.Generate();
		terrain_generator->commands.push_back(std::unique_ptr<GLTerrainGenCommand>{command2});
		
		unsigned int resolution = 128;
		//unsigned int resolution = 256;
		//unsigned int resolution = 512;
		
		terrain_generator->GetTexture(2).AllocateStorage(GL_RG32F,resolution);
		
		StationaryGaussianCommand *stationary_gaussian_command = new StationaryGaussianCommand(terrain_generator->GetTexture(2),test_shader_library.GetStationaryGaussianPrograms(),screen_size_quad);
		
		stationary_gaussian_command->SetCovariance(resolution,
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
		
		stationary_gaussian_command->SetNoiseReal(terrain_generator->GetTexture(0));
		stationary_gaussian_command->SetNoiseIm(terrain_generator->GetTexture(1));
		
		//stationary_gaussian_command.Generate();
		terrain_generator->commands.push_back(std::unique_ptr<GLTerrainGenCommand>{stationary_gaussian_command});
		
		
		/*
		 * Generating terrain
		 */
		
		LineStrip test_line_strip1;
		test_line_strip1.PushBackPoint({0.5,0.8},0.5,0.1);
		test_line_strip1.PushBackPoint({0.2,0.2},0.75,0.25);
		test_line_strip1.PushBackPoint({0.8,0.2},0.7,0.15);
		test_line_strip1.PushBackPoint({0.8,0.5},0.6,0.1);
		
		LineStrip test_line_strip2;
		test_line_strip2.PushBackPoint({0.8,0.2},0.7,0.15);
		test_line_strip2.PushBackPoint({0.8,0.72},0.5,0.1);
		test_line_strip2.PushBackPoint({0.38,0.62},0.75,0.25);
		
		LineStrip test_line_strip3;
		test_line_strip3.PushBackPoint({-0.8,0.9},0.7,0.15);
		test_line_strip3.PushBackPoint({0.0,0.8},0.5,0.1);
		test_line_strip3.PushBackPoint({1.0,1.0},0.75,0.25);
		
		LineStrip test_line_strip4;
		test_line_strip4.PushBackPoint({-0.8,0.9},0.7,0.15);
		test_line_strip4.PushBackPoint({0.6,0.8},0.5,0.1);
		
		
		
		test_line_strip_list1.push_back(test_line_strip1);
		test_line_strip_list1.push_back(test_line_strip2);
		
		test_line_strip_list2.push_back(test_line_strip3);
		
		test_line_strip_list3.push_back(test_line_strip4);
		
		GLTerrainPipeline &pipeline = terrain_generator->GetTerrainPipeline(0);
		
		GLRenderProcess *render_process1 = new GLRenderProcess(shader_program_fb);
		
		render_process1->SetNoise(&terrain_generator->GetTexture(2));
		
		render_process1->SetLineStripList(&pipeline.GetLineStripList(&test_line_strip_list1));
		
		render_process1->SetInterpolationCurveBegin({0.0,0.0,0.0,0.0});
		render_process1->SetInterpolationCurveEnd({1.0,0.0,0.0,0.0});
		
		GLRenderProcess *render_process2 = new GLRenderProcess(shader_program_fb);
		
		render_process2->SetLineStripList(&pipeline.GetLineStripList(&test_line_strip_list2));
		
		render_process2->SetInterpolationCurveBegin({1.0,0.0,0.0,0.0});
		render_process2->SetInterpolationCurveEnd({0.0,0.0,0.0,0.0});
		
		pipeline.InsertElement(0,render_process1);
		pipeline.InsertElement(1,render_process2);
		
		
		terrain_generator->GetTexture(3).AllocateStorage(GL_R32F,512);
		
		GLTerrainRenderCommand *terrain_render_command = new GLTerrainRenderCommand(terrain_generator->GetTexture(3),pipeline);
		//terrain_render_command.Generate();
		terrain_generator->commands.push_back(std::unique_ptr<GLTerrainGenCommand>{terrain_render_command});
		
		terrain_generator->Execute();
		
		for(int i=0;i< 4;i++)
		{
			textures[i] = &terrain_generator->GetTexture(i);
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}
	
	virtual ~GL4ImageDemo() override
	{}
};

int main(int argc,char *argv[])
{
	Camera3D *camera = new Camera3D;
	SDL2DemoFramework DemoFramework(800,600,nullptr,camera,1,10000);
	
	DemoFramework.SetDemo(new GL4ImageDemo);
	
	DemoFramework.Loop();
	return 0;
}

