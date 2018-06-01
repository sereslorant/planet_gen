

const char *dct_fragment_shader_source = R"(
	#version 400
	
	float PI = 3.14159265359;
	
	uniform sampler2D gauss;
	uniform int gauss_size;
	
	in vec2 tex_coord;
	
	layout(location = 0) out vec4 fragment_color;
	
	float DiscreteCosineTransform(vec2 pos)
	{
		float accumulator = 0.0;
		
		//for(int i=(gauss_size/2 - 1);i < gauss_size;i++)
		for(int i=0;i < gauss_size;i++)
		{
			//float outer_cos = cos(2.0*PI * ((i-(gauss_size/2 - 1)) + 0.5) * (pos.x) / gauss_size);
			//float outer_cos = cos(2.0*PI * (i + 0.5) * (pos.x) / gauss_size);
			//float outer_cos = cos(PI * (i) * (pos.x) / gauss_size);
			float outer_cos = cos(PI * (i + 0.5) * (pos.x) / gauss_size);
			
			//for(int j=(gauss_size/2 - 1);j < gauss_size;j++)
			for(int j=0;j < gauss_size;j++)
			{
				//float inner_cos = cos(2.0*PI * ((j-(gauss_size/2 - 1)) + 0.5) * (pos.y) / gauss_size);
				//float inner_cos = cos(2.0*PI * (j + 0.5) * (pos.y) / gauss_size);
				//float inner_cos = cos(PI * (j) * (pos.y) / gauss_size);
				float inner_cos = cos(PI * (j + 0.5) * (pos.y) / gauss_size);
				
				accumulator += texelFetch(gauss,ivec2(i,j),0).r*outer_cos*inner_cos;
			}
		}
		
		accumulator /= gauss_size;
		
		return accumulator;
		
		//return abs(accumulator);
		//return accumulator*accumulator;
	}
	
	void main()
	{
		float x = tex_coord.x * gauss_size;
		float y = tex_coord.y * gauss_size;
		
		//fragment_color = vec4(DiscreteCosineTransform(vec2(x-(gauss_size/2 - 1),y-(gauss_size/2 - 1))),0.0,0.0,1.0);
		fragment_color = vec4(DiscreteCosineTransform(vec2(x,y)),0.0,0.0,1.0);
	}
)";

const char *real_dft_fragment_shader_source = R"(
	#version 400
	
	float PI = 3.14159265359;
	
	uniform sampler2D gauss;
	uniform int gauss_size;
	
	in vec2 tex_coord;
	
	layout(location = 0) out vec4 fragment_color;
	
	vec2 UnitRoot(float exponent)
	{
		return vec2(cos(-2.0*PI*exponent/gauss_size),sin(-2.0*PI*exponent/gauss_size));
	}
	
	vec2 DiscreteFourierTransform(vec2 pos)
	{
		vec2 accumulator = vec2(0.0,0.0);
		for(int i=0;i < gauss_size;i++)
		{
			for(int j=0;j < gauss_size;j++)
			{
				//vec2 e = UnitRoot((i-(gauss_size/2 - 1))*(pos.x) + (j-(gauss_size/2 - 1))*(pos.y));
				
				vec2 e = UnitRoot((i)*(pos.x) + (j)*(pos.y));
				accumulator += texelFetch(gauss,ivec2(i,j),0).r*e;
				/*
				vec2 e = UnitRoot((j)*(pos.x) + (i)*(pos.y));
				accumulator += texelFetch(gauss,ivec2(j,i),0).r*e;
				*/
			}
		}
		
		accumulator /= gauss_size;
		return accumulator;
		
		//return abs(accumulator);
		return accumulator*accumulator;
		//return vec2(accumulator.r*accumulator.r + accumulator.g*accumulator.g,0.0);
		//return vec2(accumulator.r,0.0);
	}
	
	void main()
	{
		float x = tex_coord.x * gauss_size;
		float y = tex_coord.y * gauss_size;
		
		//fragment_color = vec4(DiscreteFourierTransform(vec2(x-(gauss_size/2 - 1),y-(gauss_size/2 - 1))),0.0,1.0);
		fragment_color = vec4(DiscreteFourierTransform(vec2(x,y)),0.0,1.0);
	}
)";

const char *complex_dft_fragment_shader_source = R"(
	#version 400
	
	float PI = 3.14159265359;
	
	uniform sampler2D gauss;
	uniform int gauss_size;
	
	in vec2 tex_coord;
	
	layout(location = 0) out vec4 fragment_color;
	
	vec2 UnitRoot(float exponent)
	{
		return vec2(cos(-2.0*PI*exponent/gauss_size),sin(-2.0*PI*exponent/gauss_size));
	}
	
	vec2 DiscreteFourierTransform(vec2 pos)
	{
		vec2 accumulator = vec2(0.0,0.0);
		for(int i=0;i < gauss_size;i++)
		{
			for(int j=0;j < gauss_size;j++)
			{
				//vec2 e = UnitRoot((i-(gauss_size/2 - 1))*(pos.x) + (j-(gauss_size/2 - 1))*(pos.y));
				
				vec2 e = UnitRoot((i)*(pos.x) + (j)*(pos.y));
				
				vec2 value = texelFetch(gauss,ivec2(i,j),0).rg;
				accumulator += vec2(value.x*e.x - value.y*e.y,value.x*e.y + value.y*e.x);
				/*
				vec2 e = UnitRoot((j)*(pos.x) + (i)*(pos.y));
				
				vec2 value = texelFetch(gauss,ivec2(j,i),0).rg;
				accumulator += vec2(value.x*e.x - value.y*e.y,value.x*e.y + value.y*e.x);
				*/
			}
		}
		
		accumulator /= gauss_size;
		return accumulator;
		
		//return abs(accumulator);
		//return accumulator*accumulator;
		//return vec2(accumulator.r*accumulator.r + accumulator.g*accumulator.g,0.0);
		//return vec2(accumulator.r,0.0);
	}
	
	void main()
	{
		float x = tex_coord.x * gauss_size;
		float y = tex_coord.y * gauss_size;
		
		//fragment_color = vec4(DiscreteFourierTransform(vec2(x-(gauss_size/2 - 1),y-(gauss_size/2 - 1))),0.0,1.0);
		fragment_color = vec4(DiscreteFourierTransform(vec2(x,y)),0.0,1.0);
	}
)";
