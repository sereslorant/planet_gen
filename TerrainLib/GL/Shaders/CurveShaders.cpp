
#include "CurveShaders.h"

const char *curve_vertex_shader_source = R"(
	#version 400
	
	in vec4 position;
	in vec4 velocity;
	
	out vec4 tc_position;
	out vec4 tc_velocity;
	
	void main()
	{
		tc_position = position; 
		tc_velocity = velocity;
	}
)";

const char *curve_tc_shader_source = R"(
	#version 400
	
	layout(vertices = 2) out;
	
	in vec4 tc_position[];
	in vec4 tc_velocity[];
	
	out vec4 te_position[];
	out vec4 te_velocity[];
	
	void main(void)
	{
		te_position[gl_InvocationID] = tc_position[gl_InvocationID];
		te_velocity[gl_InvocationID] = tc_velocity[gl_InvocationID];
		
		gl_TessLevelOuter[0] = 8.0;
		gl_TessLevelOuter[1] = 8.0;
		gl_TessLevelOuter[2] = 8.0;
		gl_TessLevelOuter[3] = 8.0;
		
		gl_TessLevelInner[0] = 8.0;
		gl_TessLevelInner[1] = 8.0;
	}
)";

const char *curve_te_shader_source = R"(
	#version 400
	
	layout(quads, equal_spacing, ccw) in;
	
	in vec4 te_position[];
	in vec4 te_velocity[];
	
	out vec4 position;
	out vec4 max_height;
	out vec2 tex_coord;
	
	vec4 Hermite(vec4 p0,vec4 p1,vec4 v0,vec4 v1,float t0,float t1,float t)
	{
		vec4 a0 = p0;
		vec4 a1 = v0;
		vec4 a2 = (3.0f*(p1-p0)/pow(t1-t0,2)) - ((v1+2.0f*v0)/(t1-t0));
		vec4 a3 = (2.0f*(p0-p1)/pow(t1-t0,3)) + ((v1+v0)/pow(t1-t0,2));

		return a0 + a1*(t-t0) + a2*pow(t-t0,2) + a3*pow(t-t0,3);
	}
	
	void main()
	{
		tex_coord = gl_TessCoord.xy;
		tex_coord.x += gl_PrimitiveID;
		
		vec4 curve_position = Hermite(te_position[0],te_position[1],te_velocity[0],te_velocity[1],0.0,1.0,gl_TessCoord.x);
		
		vec4 begin_side = normalize(vec4(-te_velocity[0].y,te_velocity[0].x,0.0,0.0));
		vec4 end_side = normalize(vec4(-te_velocity[1].y,te_velocity[1].x,0.0,0.0));
		
		vec4 side = normalize(mix(begin_side,end_side,gl_TessCoord.x));
		
		vec4 surface_position = curve_position + 2.0*(gl_TessCoord.y - 0.5)*curve_position.w*side;
		surface_position.z *= 1.0 - 2.0 * abs(gl_TessCoord.y - 0.5);
		
		position = surface_position;
		max_height = vec4(curve_position.z,0.0,0.0,0.0);
		
		gl_Position = vec4(2.0 * surface_position.xy - 1.0,0.0,1.0);
	}
)";

const char *curve_fragment_shader_source = R"(
	#version 400
	
	uniform vec4 height_curve_v0;
	uniform vec4 height_curve_v1;
	
	uniform sampler2D noise;
	
	in vec4 position;
	in vec4 max_height;
	in vec2 tex_coord;
	
	layout(location = 0) out vec4 fragment_color;
	
	vec4 Hermite(vec4 p0,vec4 p1,vec4 v0,vec4 v1,float t0,float t1,float t)
	{
		vec4 a0 = p0;
		vec4 a1 = v0;
		vec4 a2 = (3.0f*(p1-p0)/pow(t1-t0,2)) - ((v1+2.0f*v0)/(t1-t0));
		vec4 a3 = (2.0f*(p0-p1)/pow(t1-t0,3)) + ((v1+v0)/pow(t1-t0,2));

		return a0 + a1*(t-t0) + a2*pow(t-t0,2) + a3*pow(t-t0,3);
	}
	
	void main()
	{
		float noise_amp = 0.75*position.z;
		//vec2 tex_coord_scale = vec2(0.25,0.25);
		//vec2 tex_coord_scale = vec2(0.5,0.5);
		//vec2 tex_coord_scale = vec2(0.75,0.75);
		//vec2 tex_coord_scale = vec2(1.0,1.0);
		vec2 tex_coord_scale = vec2(1.25,1.25);
		//vec2 tex_coord_scale = vec2(2.0,2.0);
		
		vec4 height = Hermite(vec4(0.0),max_height,height_curve_v0,height_curve_v1,0.0,1.0,position.z);
		
		//float result = height.x + noise_amp*texture(noise,tex_coord*tex_coord_scale).r;
		float result = height.x + noise_amp*texture(noise,position.xy*tex_coord_scale).r;
		
		fragment_color = vec4(result,result,result,1.0);
	}
)";
