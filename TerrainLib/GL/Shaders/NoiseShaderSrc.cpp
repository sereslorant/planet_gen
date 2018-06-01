
#include "NoiseShaderSrc.h"

const char *diagonal_fragment_shader_source = R"(
	#version 400
	
	uniform sampler2D eigenvalue_matrix;
	
	uniform sampler2D real;
	uniform int real_size;
	
	uniform sampler2D imaginary;
	uniform int imaginary_size;
	
	//uniform int size;
	
	in vec2 tex_coord;
	
	layout(location = 0) out vec4 fragment_color;
	
	void main()
	{
		//float x = tex_coord.x * size;
		//float y = tex_coord.y * size;
		float x = tex_coord.x * real_size;
		float y = tex_coord.y * real_size;
		
		vec2 complex_probability;
		
		//complex_probability.x = texelFetch(real,ivec2(x,y),0).r;
		//complex_probability.y = texelFetch(imaginary,ivec2(x,y),0).r;
		
		complex_probability.x = texture(real,tex_coord).r;
		complex_probability.y = texture(imaginary,tex_coord).r;
		
		//float eigenvalue = sqrt(abs(texelFetch(eigenvalue_matrix,ivec2(x,y),0).r));
		//float eigenvalue = sqrt(max(0.0,texelFetch(eigenvalue_matrix,ivec2(x,y),0).r));
		
		//vec2 cmplx = texelFetch(eigenvalue_matrix,ivec2(x,y),0).rg;
		//float eigenvalue = sqrt(dot(cmplx,cmplx));
		
		float eigenvalue = abs(texelFetch(eigenvalue_matrix,ivec2(x,y),0).r);
		
		
		
		//fragment_color = vec4(eigenvalue * complex_probability,0.0,1.0);
		//fragment_color = vec4(eigenvalue * complex_probability,0.0,1.0);
		fragment_color = vec4(sqrt(eigenvalue) * complex_probability,0.0,1.0);
	}
)";
