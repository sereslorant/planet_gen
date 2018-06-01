
#include "GLShaderSrc.h"

const char *tess_vertex_shader_source = R"(
	#version 430
	
	in vec4 position;
	in ivec4 neighbor;
	
	out vec4 cs_position;
	out ivec4 cs_neighbor;
	
	void main()
	{
		cs_position = position; 
		cs_neighbor = neighbor;
	}
)";

const char *tess_ctrl_shader_source = R"(
	#version 430
	
	layout (vertices = 4) out;
	
	uniform int tess_resolution = 4;
	
	in vec4 cs_position[];
	in ivec4 cs_neighbor[];
	
	out vec4 es_position[];
	
	int GetTessellationLevel(int neighbor_data)
	{
		if(neighbor_data == 1)
		{
			return 2;
		}
		
		return 1;
	}
	
	void main()
	{
		es_position[gl_InvocationID] = cs_position[gl_InvocationID];
		
		gl_TessLevelOuter[0] = GetTessellationLevel(cs_neighbor[gl_InvocationID][0]) * tess_resolution;
		gl_TessLevelOuter[1] = GetTessellationLevel(cs_neighbor[gl_InvocationID][1]) * tess_resolution;
		gl_TessLevelOuter[2] = GetTessellationLevel(cs_neighbor[gl_InvocationID][2]) * tess_resolution;
		gl_TessLevelOuter[3] = GetTessellationLevel(cs_neighbor[gl_InvocationID][3]) * tess_resolution;
		gl_TessLevelInner[0] = tess_resolution * 2;
		gl_TessLevelInner[1] = tess_resolution * 2;
	}
)";

const char *tess_eval_sphere_shader_source = R"(
	#version 430
	
	float noise_map_size = 256.0;
	uniform sampler2D noise_map;
	
	float FiniteDiff(vec4 param,vec4 delta)
	{
		return (texture2D(noise_map, param.xy + delta.xy).r - texture2D(noise_map, param.xy - delta.xy).r)/2.0;
	}
	
	float NoiseValue(vec4 param)
	{
		return texture2D(noise_map,param.xy).r;
	}
	
	vec4 GetNormal(vec4 param)
	{
		float EPSILON = 1.0/noise_map_size;
		float diff_x = 10.0*FiniteDiff(param,vec4(EPSILON,0.0,    0.0,0.0));
		float diff_y = 10.0*FiniteDiff(param,vec4(0.0,    EPSILON,0.0,0.0));
		
		vec3 dnoise_dx = vec3(1.0,0.0,diff_x);
		vec3 dnoise_dy = vec3(0.0,1.0,diff_y);
		
		mat3 sphere_jacobian = mat3(
			1.0,0.0,0.0,
			0.0,0.0,1.0,
			0.0,1.0,0.0
		);
		
		vec3 dtransf_dx = sphere_jacobian * dnoise_dx;
		vec3 dtransf_dy = sphere_jacobian * dnoise_dy;
		
		return vec4(normalize(cross(dtransf_dy,dtransf_dx)),0.0);
		//return vec3(-diff_x,-diff_y,1.0);
		//return vec4(dtransf_dy,0.0);
		//return vec4(dtransf_dx,0.0);
	}
	
	vec4 GetPosition(vec4 param)
	{
		return vec4(1000.0*vec3(param.x,0.0,param.y) - vec3(500,0.0,500) + NoiseValue(param)*vec3(0.0,300.0,0.0),1.0);
	}
)";

const char *tess_eval_shader_source = R"(
	#version 430
	
	layout(quads,equal_spacing,ccw) in;
	
	uniform mat4 model_view_proj_matrix;
	
	in vec4 es_position[];
	
	out vec4 fs_normal;
	out vec4 fs_position;
	
	out vec4 fs_albedo;
	out float fs_fresnel;
	out float fs_roughness;
	out float fs_reflectiveness;
	out float fs_metallic;
	
	vec4 GetNormal(vec4 param);
	vec4 GetPosition(vec4 param);
	
	void main()
	{
		vec4 one_x = vec4(1.0 - gl_TessCoord.x) * es_position[0] + vec4(gl_TessCoord.x) * es_position[3];
		vec4 other_x = vec4(1.0 - gl_TessCoord.x) * es_position[1] + vec4(gl_TessCoord.x) * es_position[2];
		
		vec4 final_param = vec4(1.0 - gl_TessCoord.y) * one_x + vec4(gl_TessCoord.y) * other_x;
		
		fs_normal = GetNormal(final_param);
		vec4 position = GetPosition(final_param);
		fs_position = position;
		
		fs_albedo = vec4(0.25,0.25,0.25,1.0);
		fs_fresnel = 0.5;
		fs_roughness = 0.5;
		fs_reflectiveness = 0.75;
		fs_metallic = 0.0;
		
		gl_Position = model_view_proj_matrix * position;
	}
)";


const char *fragment_shader_source = R"(
	#version 400
	
	in vec4 fs_normal;
	
	out vec4 fragment_color;
	
	void main()
	{
		fragment_color = vec4(0.5 * fs_normal.xyz + 0.5,1.0);
	}
)";
