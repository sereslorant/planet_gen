
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
	
	uniform float radius;
	uniform vec4 corners[4];
	
	uniform sampler2D noise_map;
	
	float FiniteDiffX(vec4 param)
	{
		return (texture2D(noise_map, param.xy + vec2(0.01,0.0 )).r - texture2D(noise_map, param.xy - vec2(0.01,0.0 )).r)/2.0;
	}
	
	float FiniteDiffY(vec4 param)
	{
		return (texture2D(noise_map, param.xy + vec2(0.0 ,0.01)).r - texture2D(noise_map, param.xy - vec2(0.0 ,0.01)).r)/2.0;
	}
	
	float NoiseValue(vec4 param)
	{
		return texture2D(noise_map,param.xy).r;
	}
	
	const int CURRENT	= 0;
	const int NORTH		= 1;
	const int SOUTH		= 2;
	const int WEST		= 3;
	const int EAST		= 4;
	
	uniform vec4 param_offset[5];
	uniform float frequency[5][4];
	uniform float amplitude[5][4];
	
	uniform int neighbor_rotation[4];
	
	float GetFractalDiffX(int fractal_id,vec4 param)
	{
		float sum_amplitude = 0.0;
		for(int i=0;i < 4;i++)
		{
			sum_amplitude += amplitude[fractal_id][i] * frequency[fractal_id][i];
		}
		
		float diff_x = 0.0;
		for(int i=0;i < 4;i++)
		{
			float current_amp = amplitude[fractal_id][i];
			float current_freq = frequency[fractal_id][i];
			vec4 current_param_offset = param_offset[fractal_id];
			diff_x += ((current_amp*current_freq)/sum_amplitude)*FiniteDiffX(current_freq*(param + current_param_offset));
		}
		
		return diff_x;
	}
	
	float GetFractalDiffY(int fractal_id,vec4 param)
	{
		float sum_amplitude = 0.0;
		for(int i=0;i < 4;i++)
		{
			sum_amplitude += amplitude[fractal_id][i] * frequency[fractal_id][i];
		}
		
		float diff_y = 0.0;
		for(int i=0;i < 4;i++)
		{
			float current_amp = amplitude[fractal_id][i];
			float current_freq = frequency[fractal_id][i];
			vec4 current_param_offset = param_offset[fractal_id];
			diff_y += ((current_amp*current_freq)/sum_amplitude)*FiniteDiffY(current_freq*(param + current_param_offset));
		}
		
		return diff_y;
	}
	
	float GetFractalValue(int fractal_id,vec4 param)
	{
		float accumulator = 0.0;
		
		for(int i=0;i < 4;i++)
		{
			float current_amp = amplitude[fractal_id][i];
			float current_freq = frequency[fractal_id][i];
			vec4 current_param_offset = param_offset[fractal_id];
			accumulator += current_amp*NoiseValue(current_freq*(param + current_param_offset));
		}
		
		return accumulator;
	}
	
	const int NORTH_WEST = 0;
	const int NORTH_EAST = 1;
	const int SOUTH_WEST = 2;
	const int SOUTH_EAST = 3;
	
	
	float GetFirstLinearInCorner(int quadrant,vec4 param)
	{
		if(quadrant == NORTH_WEST)
		{
			return 0.5 - param.y;
		}
		if(quadrant == NORTH_EAST)
		{
			return 0.5 - param.y;
		}
		if(quadrant == SOUTH_WEST)
		{
			return param.y - 0.5;
		}
		if(quadrant == SOUTH_EAST)
		{
			return param.y - 0.5;
		}
	}
	
	float GetFirstLinearInMiddle(int quadrant,vec4 param)
	{
		if(quadrant == NORTH_WEST)
		{
			return param.y;
		}
		if(quadrant == NORTH_EAST)
		{
			return param.y;
		}
		if(quadrant == SOUTH_WEST)
		{
			return 1.0 - param.y;
		}
		if(quadrant == SOUTH_EAST)
		{
			return 1.0 - param.y;
		}
	}
	
	float GetSecondLinearInCorner(int quadrant,vec4 param)
	{
		if(quadrant == NORTH_WEST)
		{
			return 0.5 - param.x;
		}
		if(quadrant == NORTH_EAST)
		{
			return param.x - 0.5;
		}
		if(quadrant == SOUTH_WEST)
		{
			return 0.5 - param.x;
		}
		if(quadrant == SOUTH_EAST)
		{
			return param.x - 0.5;
		}
	}
	
	float GetSecondLinearInMiddle(int quadrant,vec4 param)
	{
		if(quadrant == NORTH_WEST)
		{
			return param.x;
		}
		if(quadrant == NORTH_EAST)
		{
			return 1.0 - param.x;
		}
		if(quadrant == SOUTH_WEST)
		{
			return param.x;
		}
		if(quadrant == SOUTH_EAST)
		{
			return 1.0 - param.x;
		}
	}
	
	float GetFirstWeight(int quadrant,vec4 param)
	{
		if(quadrant == NORTH_WEST)
		{
			return 0.5 - param.x;
		}
		if(quadrant == NORTH_EAST)
		{
			return param.x - 0.5;
		}
		if(quadrant == SOUTH_WEST)
		{
			return 0.5 - param.x;
		}
		if(quadrant == SOUTH_EAST)
		{
			return param.x - 0.5;
		}
	}
	
	float GetSecondWeight(int quadrant,vec4 param)
	{
		if(quadrant == NORTH_WEST)
		{
			return 0.5 - param.y;
		}
		if(quadrant == NORTH_EAST)
		{
			return 0.5 - param.y;
		}
		if(quadrant == SOUTH_WEST)
		{
			return param.y - 0.5;
		}
		if(quadrant == SOUTH_EAST)
		{
			return param.y - 0.5;
		}
	}
	
	vec4 GetWeights(int quadrant,vec4 param)
	{
		vec4 result = {0.0,0.0,0.0,0.0};
		
		result.x = GetFirstWeight(quadrant,param) * 4.0 * ((1.0/3.0) * GetFirstLinearInCorner(quadrant,param) + 0.5 * GetFirstLinearInMiddle(quadrant,param));
		result.y = GetSecondWeight(quadrant,param) * 4.0 * ((1.0/3.0) * GetSecondLinearInCorner(quadrant,param) + 0.5 * GetSecondLinearInMiddle(quadrant,param));
		result.z = 1.0 - result.x - result.y;
		
		return result;
	}
	
	const int DEG_0		= 0;
	const int DEG_90	= 1;
	const int DEG_180	= 2;
	const int DEG_270	= 3;
	
	vec4 ParamRotate(const int rotation,vec4 param)
	{
		float x = 0;
		float y = 0;
		switch(rotation)
		{
		case DEG_0:
			x = param.x;
			y = param.y;
			break;
			
		case DEG_90:
			x = param.y;
			y = -1.0 * param.x + 1.0;
			break;
			
		case DEG_180:
			x = -1.0 * param.x + 1.0;
			y = -1.0 * param.y + 1.0;
			break;
			
		case DEG_270:
			x = -1.0 * param.y + 1.0;
			y = param.x;
			break;
		}
		
		return vec4(x,y,0.0,0.0);
	}
	
	vec4 ParamTransform(vec4 param)
	{
		float epsilon = 1e-4;
		
		vec2 new_param = param.xy;
		vec2 result = param.xy;
		
		if(new_param.y < 0.0 + epsilon)
			{new_param.y = 0.0;}
		
		if(new_param.y > 1.0 - epsilon)
			{new_param.y = 1.0;}
		
		if(new_param.x < 0.0 + epsilon)
			{new_param.x = 0.0;}
		
		if(new_param.x > 1.0 - epsilon)
			{new_param.x = 1.0;}
		
		if(new_param.x < 0.0 + epsilon && new_param.y < 0.0 + epsilon)
		{
			vec2 direction = normalize((1.0 - new_param.y) * normalize(vec2(-1.0,-1.0)) + new_param.y * normalize(vec2(+1.0,-1.0)));
			result = vec2(0.0,new_param.y) + min(abs(param.x),abs(param.y)) * direction;
		}
		else if(new_param.x < 0.0 + epsilon && new_param.y > 1.0 - epsilon)
		{
			vec2 direction = normalize((1.0-new_param.x) * normalize(vec2(-1.0,-1.0)) + new_param.x * normalize(vec2(+1.0,-1.0)));
			result = vec2(new_param.x,0.0) + min(abs(param.x),abs(param.y - 1.0f)) * direction;
		}
		else if (new_param.x > 1.0 - epsilon  && new_param.y < 0.0 + epsilon)
		{
			vec2 direction = (1.0 - new_param.y) * normalize(vec2(+1.0,-1.0)) + new_param.y * normalize(vec2(+1.0,+1.0));
			result = vec2(1.0,new_param.y) + min(abs(param.x - 1.0f),abs(param.y)) * direction;
		}
		else if(new_param.x > 1.0 - epsilon  && new_param.y > 1.0 - epsilon)
		{
			vec2 direction = (1.0-new_param.x) * normalize(vec2(-1.0,+1.0)) + new_param.x * normalize(vec2(+1.0,+1.0));
			result = vec2(new_param.x,1.0) + min(abs(param.x - 1.0),abs(param.y - 1.0)) * direction;
		}
		
		return vec4(result.xy,0.0,0.0);
	}
	
	float GetTerrainDiffX(vec4 param)
	{
		int neighbor_1_id = NORTH;
		vec4 neighbor_1_offset = vec4(0.0,+1.0,0.0,0.0);
		if(param.y > 0.5)
		{
			neighbor_1_id = SOUTH;
			neighbor_1_offset = vec4(0.0,-1.0,0.0,0.0);
		}
		
		int neighbor_2_id = WEST;
		vec4 neighbor_2_offset = vec4(+1.0,0.0,0.0,0.0);
		if(param.x > 0.5)
		{
			neighbor_2_id = EAST;
			neighbor_2_offset = vec4(-1.0,0.0,0.0,0.0);
		}
		
		int quadrant = NORTH_WEST;
		if(neighbor_1_id == NORTH)
		{
			if(neighbor_2_id == WEST)
			{quadrant = NORTH_WEST;}
			else
			{quadrant = NORTH_EAST;}
		}
		else
		{
			if(neighbor_2_id == WEST)
			{quadrant = SOUTH_WEST;}
			else
			{quadrant = SOUTH_EAST;}
		}
		
		vec4 weights = GetWeights(quadrant,param);
		
		float diff_x = GetFractalDiffX(CURRENT,param);
		float neighbor_1_diff_x = GetFractalDiffX(neighbor_1_id,ParamTransform(ParamRotate(neighbor_rotation[neighbor_1_id - 1],param + neighbor_1_offset)));
		float neighbor_2_diff_x = GetFractalDiffX(neighbor_2_id,ParamTransform(ParamRotate(neighbor_rotation[neighbor_2_id - 1],param + neighbor_2_offset)));
		
		//return diff_x;
		return weights.z*diff_x + weights.y*neighbor_1_diff_x + weights.x*neighbor_2_diff_x;
	}
	
	float GetTerrainDiffY(vec4 param)
	{
		int neighbor_1_id = NORTH;
		vec4 neighbor_1_offset = vec4(0.0,+1.0,0.0,0.0);
		if(param.y > 0.5)
		{
			neighbor_1_id = SOUTH;
			neighbor_1_offset = vec4(0.0,-1.0,0.0,0.0);
		}
		
		int neighbor_2_id = WEST;
		vec4 neighbor_2_offset = vec4(+1.0,0.0,0.0,0.0);
		if(param.x > 0.5)
		{
			neighbor_2_id = EAST;
			neighbor_2_offset = vec4(-1.0,0.0,0.0,0.0);
		}
		
		int quadrant = NORTH_WEST;
		if(neighbor_1_id == NORTH)
		{
			if(neighbor_2_id == WEST)
			{quadrant = NORTH_WEST;}
			else
			{quadrant = NORTH_EAST;}
		}
		else
		{
			if(neighbor_2_id == WEST)
			{quadrant = SOUTH_WEST;}
			else
			{quadrant = SOUTH_EAST;}
		}
		
		vec4 weights = GetWeights(quadrant,param);
		
		float diff_y = GetFractalDiffY(CURRENT,param);
		float neighbor_1_diff_y = GetFractalDiffY(neighbor_1_id,ParamTransform(ParamRotate(neighbor_rotation[neighbor_1_id - 1],param + neighbor_1_offset)));
		float neighbor_2_diff_y = GetFractalDiffY(neighbor_2_id,ParamTransform(ParamRotate(neighbor_rotation[neighbor_2_id - 1],param + neighbor_2_offset)));
		
		//return diff_y;
		return weights.z*diff_y + weights.y*neighbor_1_diff_y + weights.x*neighbor_2_diff_y;
	}
	
	float GetTerrainValue(const vec4 param)
	{
		int neighbor_1_id = NORTH;
		vec4 neighbor_1_offset = vec4(0.0,+1.0,0.0,0.0);
		if(param.y > 0.5)
		{
			neighbor_1_id = SOUTH;
			neighbor_1_offset = vec4(0.0,-1.0,0.0,0.0);
		}
		
		int neighbor_2_id = WEST;
		vec4 neighbor_2_offset = vec4(+1.0,0.0,0.0,0.0);
		if(param.x > 0.5)
		{
			neighbor_2_id = EAST;
			neighbor_2_offset = vec4(-1.0,0.0,0.0,0.0);
		}
		
		int quadrant = NORTH_WEST;
		if(neighbor_1_id == NORTH)
		{
			if(neighbor_2_id == WEST)
			{quadrant = NORTH_WEST;}
			else
			{quadrant = NORTH_EAST;}
		}
		else
		{
			if(neighbor_2_id == WEST)
			{quadrant = SOUTH_WEST;}
			else
			{quadrant = SOUTH_EAST;}
		}
		
		vec4 weights = GetWeights(quadrant,param);
		
		float current_height = GetFractalValue(CURRENT,param);
		float neighbor_1_height = GetFractalValue(neighbor_1_id,ParamTransform(ParamRotate(neighbor_rotation[neighbor_1_id - 1],param + neighbor_1_offset)));
		float neighbor_2_height = GetFractalValue(neighbor_2_id,ParamTransform(ParamRotate(neighbor_rotation[neighbor_2_id - 1],param + neighbor_2_offset)));
		
		return weights.z*current_height + weights.y*neighbor_1_height + weights.x*neighbor_2_height;
	}
	
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
		
		float diff_x = GetTerrainDiffX(param);//(texture2D(noise_map, param.xy + vec2(0.01,0.0 )).r - texture2D(noise_map, param.xy - vec2(0.01,0.0 )).r)/2.0;
		float diff_y = GetTerrainDiffY(param);//(texture2D(noise_map, param.xy + vec2(0.0 ,0.01)).r - texture2D(noise_map, param.xy - vec2(0.0 ,0.01)).r)/2.0;
		
		vec3 dnoise_dx = vec3(1.0,0.0,diff_x);
		vec3 dnoise_dy = vec3(0.0,1.0,diff_y);
		
		vec3 normal = vec3(-diff_x,-diff_y,1.0);
		
		mat3 sphere_jacobian = mat3(
			sphere_derivative_x[0],sphere_derivative_x[1],sphere_derivative_x[2],
			sphere_derivative_y[0],sphere_derivative_y[1],sphere_derivative_y[2],
			sphere_point[0],       sphere_point[1],       sphere_point[2]
			);
		
		vec3 dtransf_dx = sphere_jacobian * dnoise_dx;
		vec3 dtransf_dy = sphere_jacobian * dnoise_dy;
		
		//return vec4(normalize(sphere_jacobian*normalize(normal)),1.0);
		return vec4(normalize(cross(dtransf_dy,dtransf_dx)),1.0);
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
		
		return vec4(radius * sphere_point + GetTerrainValue(param) * sphere_point,1.0);
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
	
	float GetTerrainValue(const vec4 param);
	
	void main()
	{
		vec4 one_x = vec4(1.0 - gl_TessCoord.x) * es_position[0] + vec4(gl_TessCoord.x) * es_position[3];
		vec4 other_x = vec4(1.0 - gl_TessCoord.x) * es_position[1] + vec4(gl_TessCoord.x) * es_position[2];
		
		vec4 final_param = vec4(1.0 - gl_TessCoord.y) * one_x + vec4(gl_TessCoord.y) * other_x;
		
		fs_normal = GetNormal(final_param);
		vec4 position = GetPosition(final_param);
		fs_position = position;
		
		float max_height = 76000 + 8000 + 15000 + 7000;
		float h = GetTerrainValue(final_param)/max_height;
		
		fs_albedo = h * vec4(0.25,0.25,0.25,1.0) + (1.0 - h) * vec4(0.75,1.0,0.125,1.0);
		//fs_albedo = vec4(0.25,0.25,0.25,1.0);
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
