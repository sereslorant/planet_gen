
#include <DemoFramework/SDL2DemoFramework.h>
#include <DemoFramework/Camera3D.h>

#include <DemoFramework/GL4/GL4DemoBase.h>

#include <TerrainLib/GL/GLTerrainGenCommand.h>

#include <TerrainLib/GL/Shaders/CurveShaders.h>


class GL4ParamSurfDemo : public GL4DemoBase
{
private:
	GLShaderProgram shader_program;
	GLLineStripList line_strip_list;
	
public:
	
	virtual void Logic() override
	{
		
	}
	
	virtual void Draw() override
	{
		GLRenderProcess render_process(shader_program);
		
		render_process.SetLineStripList(&line_strip_list);
		
		render_process.SetInterpolationCurveBegin({0.0,0.0,0.0,0.0});
		render_process.SetInterpolationCurveEnd({1.3,0.0,0.0,0.0});
		
		render_process.Execute();
	}
	
	GL4ParamSurfDemo()
	{
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
		
		std::vector<vec4> vertices =
		{
			{0.5,0.8,0.5,0.13},
			{0.2,0.2,1.0,0.25},
			{0.8,0.2,0.75,0.14},
		};
		
		std::vector<vec4> velocities =
		{
			(vertices[1] - vertices[0]),
			(vertices[2] - vertices[0]) /2.0,
			(vertices[2] - vertices[1]),
		};
		
		std::vector<unsigned int> indices = {0,1,1,2};
		
		line_strip_list.Initialize(shader_program,2);
		line_strip_list.UploadData(0,vertices,velocities,indices);
	}
	
	virtual ~GL4ParamSurfDemo() override
	{
		
	}
};

int main(int argc,char *argv[])
{
	Camera3D *camera = new Camera3D;
	SDL2DemoFramework DemoFramework(800,600,nullptr,camera,1,10000);
	
	DemoFramework.SetDemo(new GL4ParamSurfDemo);
	
	DemoFramework.Loop();
	return 0;
}
