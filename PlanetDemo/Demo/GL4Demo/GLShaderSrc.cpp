
#include "GLShaderSrc.h"

const char *vertex_shader_source = R"(
	#version 400
	
	uniform mat4 model_view_proj_matrix;
	
	in vec4 position;
	in vec4 normal;
	
	out vec4 fs_normal;
	
	void main()
	{
		fs_normal = normal;
		
		gl_Position = model_view_proj_matrix * vec4(position.x,position.y,position.z,1.0); 
	}
)";

const char *tess_vertex_shader_source = R"(
	#version 400
	
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
	#version 400
	
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
	#version 400
	
	uniform float radius;
	uniform vec4 corners[4];
	
	uniform sampler2D noise_map;
	
	//float terrain_height = 100000.0;
	float terrain_height = 30000.0;
	
	vec3 slerp(const vec3 begin,const vec3 end,float param)
	{
		float angle = acos(dot(normalize(begin),normalize(end)));
		
		return (sin((1.0 - param)*angle)*begin + sin(param*angle)*end)/sin(angle);
	}
	
	vec3 slerp_derivative(vec3 begin,vec3 end,float param)
	{
		float angle = acos(dot(normalize(begin),normalize(end)));
		
		return (-angle*cos((1.0 - param)*angle)*begin + angle*cos(param*angle)*end)/sin(angle);
	}
	
	vec4 GetNormal(vec4 param)
	{
		vec3 v0 = corners[0].xyz;
		vec3 v1 = corners[1].xyz;
		vec3 v2 = corners[2].xyz;
		vec3 v3 = corners[3].xyz;
		
		vec3 pointX1 = slerp(v0,v1,param.x);
		vec3 pointX2 = slerp(v3,v2,param.x);
		
		vec3 pointY1 = slerp(v0,v3,param.y);
		vec3 pointY2 = slerp(v1,v2,param.y);
		
		vec3 sphere_point = slerp(pointX1,pointX2,param.y);
		
		vec3 sphere_derivative_x = normalize(slerp_derivative(pointY1,pointY2,param.x));
		vec3 sphere_derivative_y = normalize(slerp_derivative(pointX1,pointX2,param.y));
		
		float value  =  texture2D(noise_map, param.xy).r;
		float diff_x = (texture2D(noise_map, param.xy + vec2(0.01,0.0 )).r - texture2D(noise_map, param.xy - vec2(0.01,0.0 )).r)/2.0;
		float diff_y = (texture2D(noise_map, param.xy + vec2(0.0 ,0.01)).r - texture2D(noise_map, param.xy - vec2(0.0 ,0.01)).r)/2.0;
		
		//vec3 dnoise_dx = vec3(1.0,0.0,diff_x);
		//vec3 dnoise_dy = vec3(0.0,1.0,diff_y);
		
		vec3 normal = vec3(-diff_x,-diff_y,1.0);
		
		mat3 sphere_jacobian = mat3(
			sphere_derivative_x[0],sphere_derivative_x[1],sphere_derivative_x[2],
			sphere_derivative_y[0],sphere_derivative_y[1],sphere_derivative_y[2],
			sphere_point[0],       sphere_point[1],       sphere_point[2]
			);
		
		//vec3 dtransf_dx = sphere_jacobian * dnoise_dx;
		//vec3 dtransf_dy = sphere_jacobian * dnoise_dy;
		
		return vec4(normalize(sphere_jacobian*normal),1.0);
	}
	
	vec4 GetPosition(vec4 param)
	{
		vec3 v0 = corners[0].xyz;
		vec3 v1 = corners[1].xyz;
		vec3 v2 = corners[2].xyz;
		vec3 v3 = corners[3].xyz;
		
		vec3 pointX1 = slerp(v0,v1,param.x);
		vec3 pointX2 = slerp(v3,v2,param.x);
		
		vec3 sphere_point = slerp(pointX1,pointX2,param.y);
		
		float height = terrain_height * texture2D(noise_map, param.xy).r;
		
		return vec4(radius * sphere_point + height * sphere_point,1.0);
	}
)";

const char *tess_eval_shader_source = R"(
	#version 400
	
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
	
	uniform sampler2D noise_map;
	
	void main()
	{
		vec4 one_x = vec4(1.0 - gl_TessCoord.x) * es_position[0] + vec4(gl_TessCoord.x) * es_position[3];
		vec4 other_x = vec4(1.0 - gl_TessCoord.x) * es_position[1] + vec4(gl_TessCoord.x) * es_position[2];
		
		vec4 final_param = vec4(1.0 - gl_TessCoord.y) * one_x + vec4(gl_TessCoord.y) * other_x;
		
		fs_normal = GetNormal(final_param);
		vec4 position = GetPosition(final_param);
		fs_position = position;
		
		float h = texture2D(noise_map, final_param.xy).r;
		
		fs_albedo = h * vec4(0.25,0.25,0.25,1.0) + (1.0 - h) * vec4(0.5,1.0,0.125,1.0);
		fs_fresnel = 0.5;
		fs_roughness = 0.75;
		fs_reflectiveness = 0.5;
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
		fragment_color = vec4(((vec4(1.0,1.0,1.0,0.0) + fs_normal)/2.0).xyz,1.0);
	}
)";
