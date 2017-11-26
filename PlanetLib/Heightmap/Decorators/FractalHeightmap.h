#ifndef FRACTAL_HEIGHTMAP_H
#define FRACTAL_HEIGHTMAP_H

#include <PlanetLib/Heightmap/IHeightmap.h>

#include <array>

class FractalHeightmap : public IHeightmap
{
private:
	const IHeightmap &heightmap;
	
	vec4 param_offset;
	
	std::array<float,4> frequency = 
	{{
		1.25,
		0.05,
		3.7,
		0.4,//0.3,
	}};
	
	std::array<float,4> amplitude =
	{{
		60,//120,
		30,
		15,
		170,//470,//170,
	}};
	
public:
	
	const vec4 &GetParamOffset()
	{
		return param_offset;
	}
	
	const std::array<float,4> &GetFrequency() const
	{
		return frequency;
	}
	
	const std::array<float,4> &GetAmplitude() const
	{
		return amplitude;
	}
	
	virtual float GetValue(const vec4 &vec) const override
	{
		float result = 0.0;
		
		vec4 new_vec = vec + param_offset;
		
		for(int i=0;i < 4;i++)
		{
			result += heightmap.GetValue({new_vec[0]*frequency[i],new_vec[1]*frequency[i]}) * amplitude[i];
		}
		
		return result;
	}
	
	FractalHeightmap(const IHeightmap &p_heightmap,const vec4 &p_param_offset,
					 const std::array<float,4> &p_frequency,
					 const std::array<float,4> &p_amplitude
					)
		:heightmap(p_heightmap),param_offset(p_param_offset),frequency(p_frequency),amplitude(p_amplitude)
	{}
	
	virtual ~FractalHeightmap() override
	{}
};

#endif // FRACTAL_HEIGHTMAP_H
