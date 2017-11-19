#ifndef STITCHED_HEIGHTMAP_H
#define STITCHED_HEIGHTMAP_H

#include <PlanetLib/Heightmap/IHeightmap.h>

class ParamTransformation
{
private:
	float epsilon;
	
public:
	
	vec4 GetValue(const vec4 &param)
	{
		vec4 new_param = param;
		vec4 result = param;
		
		if(new_param[1] < 0.0 + epsilon)
			{new_param[1] = 0.0;}
		
		if(new_param[1] > 1.0 - epsilon)
			{new_param[1] = 1.0;}
		
		if(new_param[0] < 0.0 + epsilon)
			{new_param[0] = 0.0;}
		
		if(new_param[0] > 1.0 - epsilon)
			{new_param[0] = 1.0;}
		
		if(new_param[0] < 0.0 + epsilon && new_param[1] < 0.0 + epsilon)
		{
			vec4 direction = normalize((1.0 - new_param[1]) * normalize(vec4{-1.0,-1.0}) + /*(1.0-*/new_param[1]/*)*/ * normalize(vec4{+1.0,-1.0}));
			result = vec4{0.0,new_param[1]} + std::min(std::abs(param[0]),std::abs(param[1])) * direction;
		}
		else if(new_param[0] < 0.0 + epsilon && new_param[1] > 1.0 - epsilon)
		{
			vec4 direction = normalize((1.0-new_param[0]) * normalize(vec4{-1.0,-1.0}) + /*(1.0-*/new_param[0]/*)*/ * normalize(vec4{+1.0,-1.0}));
			result = vec4{new_param[0],0.0} + std::min(std::abs(param[0]),std::abs(param[1] - 1.0f)) * direction;
		}
		else if (new_param[0] > 1.0 - epsilon  && new_param[1] < 0.0 + epsilon)
		{
			vec4 direction = (1.0 - new_param[1]) * normalize(vec4{+1.0,-1.0}) + /*(1.0-*/new_param[1]/*)*/ * normalize(vec4{+1.0,+1.0});
			result = vec4{1.0,new_param[1]} + std::min(std::abs(param[0] - 1.0f),std::abs(param[1])) * direction;
		}
		else if(new_param[0] > 1.0 - epsilon  && new_param[1] > 1.0 - epsilon)
		{
			vec4 direction = (1.0-new_param[0]) * normalize(vec4{-1.0,+1.0}) + /*(1.0-*/new_param[0]/*)*/ * normalize(vec4{+1.0,+1.0});
			result = vec4{new_param[0],1.0} + std::min(std::abs(param[0] - 1.0),std::abs(param[1] - 1.0)) * direction;
		}
		
		return result;
	}
	
	ParamTransformation(float p_epsilon = 1e-2)
		:epsilon(p_epsilon)
	{}
	
	~ParamTransformation()
	{}
};

class ParamTransformedHeightmap : public IHeightmap
{
private:
	const IHeightmap &wrapped_heightmap;
	
	static constexpr float EPSILON = 1e-2;
	
public:
	
	virtual float GetValue(const vec4 &param) const override
	{
		ParamTransformation param_transform(EPSILON);
		vec4 result = param_transform.GetValue(param);
		
		return wrapped_heightmap.GetValue(result);
	}
	
	ParamTransformedHeightmap(const IHeightmap &p_wrapped_heightmap)
		:wrapped_heightmap(p_wrapped_heightmap)
	{}
	
	virtual ~ParamTransformedHeightmap() override
	{}
};

class QuadrantWeight
{
protected:
	virtual float GetFirstLinearInCorner(const vec4 &param) const = 0;
	virtual float GetFirstLinearInMiddle(const vec4 &param) const = 0;
	
	virtual float GetSecondLinearInCorner(const vec4 &param) const = 0;
	virtual float GetSecondLinearInMiddle(const vec4 &param) const = 0;
	
	virtual float GetFirstWeight(const vec4 &param) const = 0;
	virtual float GetSecondWeight(const vec4 &param) const = 0;
	
public:
	
	vec4 GetWeights(const std::array<bool,2> &has_neighbors,const vec4 &param) const
	{
		vec4 result = {0.0,0.0,0.0};
		
		if(has_neighbors[0] && has_neighbors[1])
		{
			result[0] = GetFirstWeight(param) * 4.0 * ((1.0/3.0) * GetFirstLinearInCorner(param) + 0.5 * GetFirstLinearInMiddle(param));
			result[1] = GetSecondWeight(param) * 4.0 * ((1.0/3.0) * GetSecondLinearInCorner(param) + 0.5 * GetSecondLinearInMiddle(param));
		}
		else if(has_neighbors[0])
		{
			result[0] = GetFirstWeight(param);
		}
		else if(has_neighbors[1])
		{
			result[1] = GetSecondWeight(param);
		}
		
		result[2]	= 1.0 - result[0] - result[1];
		
		return result;
	}
	
	QuadrantWeight()
	{}
	
	virtual ~QuadrantWeight()
	{}
};

class NorthWestQuadrantWeight : public QuadrantWeight
{
protected:
	
	virtual float GetFirstLinearInCorner(const vec4 &param) const override
	{
		return (0.5 - param[1]);
	}
	
	virtual float GetFirstLinearInMiddle(const vec4 &param) const override
	{
		return (param[1]);
	}
	
	virtual float GetSecondLinearInCorner(const vec4 &param) const override
	{
		return (0.5 - param[0]);
	}
	
	virtual float GetSecondLinearInMiddle(const vec4 &param) const override
	{
		return (param[0]);
	}
	
	virtual float GetFirstWeight(const vec4 &param) const override
	{
		return (0.5 - param[0]);
	}
	
	virtual float GetSecondWeight(const vec4 &param) const override
	{
		return (0.5 - param[1]);
	}
		
public:
	
	NorthWestQuadrantWeight()
	{}
	
	virtual ~NorthWestQuadrantWeight() override
	{}
};

class NorthEastQuadrantWeight : public QuadrantWeight
{
protected:
	
	virtual float GetFirstLinearInCorner(const vec4 &param) const override
	{
		return (0.5 - param[1]);
	}
	
	virtual float GetFirstLinearInMiddle(const vec4 &param) const override
	{
		return (param[1]);
	}
	
	virtual float GetSecondLinearInCorner(const vec4 &param) const override
	{
		return (param[0] - 0.5);
	}
	
	virtual float GetSecondLinearInMiddle(const vec4 &param) const override
	{
		return (1.0 - param[0]);
	}
	
	virtual float GetFirstWeight(const vec4 &param) const override
	{
		return (param[0] - 0.5);
	}
	
	virtual float GetSecondWeight(const vec4 &param) const override
	{
		return (0.5 - param[1]);
	}
	
public:
	
	NorthEastQuadrantWeight()
	{}
	
	virtual ~NorthEastQuadrantWeight() override
	{}
};

class SouthWestQuadrantWeight : public QuadrantWeight
{
protected:
	
	virtual float GetFirstLinearInCorner(const vec4 &param) const override
	{
		return (param[1] - 0.5);
	}
	
	virtual float GetFirstLinearInMiddle(const vec4 &param) const override
	{
		return (1.0 - param[1]);
	}
	
	virtual float GetSecondLinearInCorner(const vec4 &param) const override
	{
		return (0.5 - param[0]);
	}
	
	virtual float GetSecondLinearInMiddle(const vec4 &param) const override
	{
		return (param[0]);
	}
	
	virtual float GetFirstWeight(const vec4 &param) const override
	{
		return (0.5 - param[0]);
	}
	
	virtual float GetSecondWeight(const vec4 &param) const override
	{
		return (param[1] - 0.5);
	}
		
public:
	
	SouthWestQuadrantWeight()
	{}
	
	virtual ~SouthWestQuadrantWeight() override
	{}
};

class SouthEastQuadrantWeight : public QuadrantWeight
{
protected:
	
	virtual float GetFirstLinearInCorner(const vec4 &param) const override
	{
		return (param[1] - 0.5);
	}
	
	virtual float GetFirstLinearInMiddle(const vec4 &param) const override
	{
		return (1.0 - param[1]);
	}
	
	virtual float GetSecondLinearInCorner(const vec4 &param) const override
	{
		return (param[0] - 0.5);
	}
	
	virtual float GetSecondLinearInMiddle(const vec4 &param) const override
	{
		return (1.0 - param[0]);
	}
	
	virtual float GetFirstWeight(const vec4 &param) const override
	{
		return (param[0] - 0.5);
	}
	
	virtual float GetSecondWeight(const vec4 &param) const override
	{
		return (param[1] - 0.5);
	}
			
public:
	
	SouthEastQuadrantWeight()
	{}
	
	virtual ~SouthEastQuadrantWeight() override
	{}
};

class StitchedHeightmap : public IHeightmap
{
public:
	
	enum HeightmapRotation
	{
		DEG_0,
		DEG_90,
		DEG_180,
		DEG_270,
	};
	
	enum NeighborDirection
	{
		NORTH	= 0,
		SOUTH	= 1,
		WEST	= 2,
		EAST	= 3,
	};
	
private:
	
	class Neighbor : public IHeightmap
	{
	private:
		const IHeightmap *heightmap = nullptr;
		HeightmapRotation rotation;
		
	public:
		
		bool HasNeighbor() const
		{
			return heightmap != nullptr;
		}
		
		virtual float GetValue(const vec4 &param) const override
		{
			if(!HasNeighbor())
			{
				return 0.0;
			}
			
			float x = 0;
			float y = 0;
			switch(rotation)
			{
			case DEG_0:
				x = param[0];
				y = param[1];
				break;
				
			case DEG_90:
				x = param[1];
				y = -1.0 * param[0] + 1.0;
				break;
				
			case DEG_180:
				x = -1.0 * param[0] + 1.0;
				y = -1.0 * param[1] + 1.0;
				break;
				
			case DEG_270:
				x = -1.0 * param[1] + 1.0;
				y = param[0];
				break;
			}
			
			return heightmap->GetValue({x,y,0.0,0.0});
		}
		
		Neighbor()
		{}
		
		Neighbor(const IHeightmap *p_heightmap,HeightmapRotation p_rotation)
			:heightmap(p_heightmap),rotation(p_rotation)
		{}
		
		virtual ~Neighbor() override
		{}
	};
	
	const IHeightmap &heightmap;
	std::array<Neighbor,4> neighbors;
	
	float GetWeightedSum(const std::array<const IHeightmap*,3> &heightmaps,const std::array<vec4,3> offsets,const vec4 &weights,const vec4 &param) const
	{
		float result = 0.0;
		
		for(int i=0;i < 3;i++)
		{
			result += weights[i] * heightmaps[i]->GetValue(param + offsets[i]);
		}
		
		return result;
	}
	
	float GetNorthWestQuadrant(const vec4 &param) const
	{
		NorthWestQuadrantWeight weight;
		vec4 weights = weight.GetWeights({{neighbors[WEST].HasNeighbor(),neighbors[NORTH].HasNeighbor()}},param);
		
		return GetWeightedSum({{&neighbors[WEST],&neighbors[NORTH],&heightmap}},
							  {{{+1.0,+0.0},{+0.0,+1.0},{+0.0,+0.0}}},
							  //{west_weight,north_weight,current_weight},
							  weights,
							  param
							 );
	}
	
	float GetSouthWestQuadrant(const vec4 &param) const
	{
		SouthWestQuadrantWeight weight;
		vec4 weights = weight.GetWeights({{neighbors[WEST].HasNeighbor(),neighbors[SOUTH].HasNeighbor()}},param);
		
		return GetWeightedSum({{&neighbors[WEST],&neighbors[SOUTH],&heightmap}},
							  {{{+1.0,+0.0},{+0.0,-1.0},{+0.0,+0.0}}},
							  //{west_weight,south_weight,current_weight},
							  weights,
							  param
							 );
	}
	
	float GetNorthEastQuadrant(const vec4 &param) const
	{
		NorthEastQuadrantWeight weight;
		vec4 weights = weight.GetWeights({{neighbors[EAST].HasNeighbor(),neighbors[NORTH].HasNeighbor()}},param);
		
		return GetWeightedSum({{&neighbors[EAST],&neighbors[NORTH],&heightmap}},
							  {{{-1.0,+0.0},{+0.0,+1.0},{+0.0,+0.0}}},
							  //{east_weight,north_weight,current_weight},
							  weights,
							  param
							 );
	}
	
	float GetSouthEastQuadrant(const vec4 &param) const
	{
		SouthEastQuadrantWeight weight;
		vec4 weights = weight.GetWeights({{neighbors[EAST].HasNeighbor(),neighbors[SOUTH].HasNeighbor()}},param);
		
		return GetWeightedSum({{&neighbors[EAST],&neighbors[SOUTH],&heightmap}},
							  {{{-1.0,+0.0},{+0.0,-1.0},{+0.0,+0.0}}},
							  //{east_weight,south_weight,current_weight},
							  weights,
							  param
							 );
	}
	
public:
	
	void SetNeighbor(NeighborDirection p_direction,const IHeightmap &p_heightmap,HeightmapRotation p_rotation)
	{
		neighbors[p_direction] = {&p_heightmap,p_rotation};
	};
	
	virtual float GetValue(const vec4 &param) const override
	{
		if(param[0] < 0.5)
		{
			if(param[1] < 0.5)
			{
				return GetNorthWestQuadrant(param);
			}
			else
			{
				return GetSouthWestQuadrant(param);
			}
		}
		else
		{
			if(param[1] < 0.5)
			{
				return GetNorthEastQuadrant(param);
			}
			else
			{
				return GetSouthEastQuadrant(param);
			}
		}
		
		return heightmap.GetValue(param);
	}
	
	StitchedHeightmap(const IHeightmap &p_heightmap)
		:heightmap(p_heightmap)
	{}
	
	StitchedHeightmap(const IHeightmap &p_heightmap,std::array<const IHeightmap *,4> p_neighbors,std::array<HeightmapRotation,4> p_neighbor_orientations)
		:heightmap(p_heightmap),
		neighbors({{
			{p_neighbors[0],p_neighbor_orientations[0]},
			{p_neighbors[1],p_neighbor_orientations[1]},
			{p_neighbors[2],p_neighbor_orientations[2]},
			{p_neighbors[3],p_neighbor_orientations[3]},
		}})
	{}
	
	virtual ~StitchedHeightmap() override
	{}
};
#endif // STITCHED_HEIGHTMAP_H
