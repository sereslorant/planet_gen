
#include "ImgProcShaderSrc.h"

const char *ssquad_vertex_shader_source = R"(
	#version 400
	
	in vec4 data;
	
	out vec2 tex_coord;
	
	void main()
	{
		vec2 final_pos = data.xy;
		
		tex_coord = data.zw;
		
		gl_Position = vec4(final_pos,0.0,1.0); 
	}
)";
