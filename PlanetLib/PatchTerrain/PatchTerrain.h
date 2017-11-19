#ifndef PATCH_TERRAIN_H
#define PATCH_TERRAIN_H

#include <glmath.hpp>

#include <memory>

#include <array>

/*
 * Non uniform patch terrain
 * http://victorbush.com/2015/01/tessellated-terrain/
 */

#include "PatchTerrainEdge.h"

#include "NeighborWrapper.h"



/*
 * TODO: Lehet, hogy ki kéne emelni a Quadtree funkcionalitást külön osztályba.
 * 
 * TODO: Lehet, hogy a gyerekek indexelését ki kéne emelni Strategybe.
 */
class PatchTerrain
{
public:
	
	enum NeighborDirection
	{
		UP		= 0, //Edge between vertex[0] and vertex[1]
		DOWN	= 1, //Edge between vertex[1] and vertex[2]
		LEFT	= 2, //Edge between vertex[2] and vertex[3]
		RIGHT	= 3, //Edge between vertex[3] and vertex[0]
	};
	
	enum ChildPosition
	{
		NONE			= -1,
		TOP_LEFT		=  0,
		TOP_RIGHT		=  1,
		BOTTOM_RIGHT	=  2,
		BOTTOM_LEFT		=  3,
	};
	
private:
	
	friend class PatchTerrainEdge;
	friend class NeighborWrapper;
	
	/*
	 * Quadtree funkciók
	 */
	
	unsigned int depth = 0;
	bool split = false;
	
	PatchTerrain *parent = nullptr;
	std::unique_ptr<PatchTerrain> children[4]	= {nullptr,nullptr,nullptr,nullptr};
	std::array<const PatchTerrainEdge,4> edges		= 
	{{
		{this,TOP_LEFT,TOP_RIGHT},			// UP
		{this,BOTTOM_RIGHT,BOTTOM_LEFT},	// DOWN
		{this,BOTTOM_LEFT,TOP_LEFT},		// LEFT
		{this,TOP_RIGHT,BOTTOM_RIGHT},		// RIGHT
	}};
	
	std::array<NeighborWrapper,4> neighbors		= {{{edges[UP]},{edges[DOWN]},{edges[LEFT]},{edges[RIGHT]}}};
	
	/*
	 * Heightmap funkciók
	 */
	
	std::array<vec4,4> vertices;
	
	void SetupTopLeftNeighbors()
	{
		if(neighbors[UP].HasNeighbor())
			//{children[TOP_LEFT].get()->neighbors[UP].SetNeighbor(neighbors[UP].GetNeighbor(),neighbors[UP].GetNeighborSide());}
			{children[TOP_LEFT].get()->neighbors[UP].SetNeighbor(neighbors[UP].GetNeighboringEdge());}
		
		children[TOP_LEFT].get()->neighbors[DOWN].SetNeighbor(children[BOTTOM_LEFT].get()->GetEdge(UP));
		
		if(neighbors[LEFT].HasNeighbor())
			//{children[TOP_LEFT].get()->neighbors[LEFT].SetNeighbor(neighbors[LEFT].GetNeighbor(),neighbors[LEFT].GetNeighborSide());}
			{children[TOP_LEFT].get()->neighbors[LEFT].SetNeighbor(neighbors[LEFT].GetNeighboringEdge());}
		
		children[TOP_LEFT].get()->neighbors[RIGHT].SetNeighbor(children[TOP_RIGHT].get()->GetEdge(LEFT));
	}
	
	void SetupTopRightNeighbors()
	{
		if(neighbors[UP].HasNeighbor())
			//{children[TOP_RIGHT].get()->neighbors[UP].SetNeighbor(neighbors[UP].GetNeighbor(),neighbors[UP].GetNeighborSide());}
			{children[TOP_RIGHT].get()->neighbors[UP].SetNeighbor(neighbors[UP].GetNeighboringEdge());}
		
		children[TOP_RIGHT].get()->neighbors[DOWN].SetNeighbor(children[BOTTOM_RIGHT].get()->GetEdge(UP));
		
		children[TOP_RIGHT].get()->neighbors[LEFT].SetNeighbor(children[TOP_LEFT].get()->GetEdge(RIGHT));
		
		if(neighbors[RIGHT].HasNeighbor())
			//{children[TOP_RIGHT].get()->neighbors[RIGHT].SetNeighbor(neighbors[RIGHT].GetNeighbor(),neighbors[RIGHT].GetNeighborSide());}
			{children[TOP_RIGHT].get()->neighbors[RIGHT].SetNeighbor(neighbors[RIGHT].GetNeighboringEdge());}
	}
	
	void SetupBottomRightNeighbors()
	{
		children[BOTTOM_RIGHT].get()->neighbors[UP].SetNeighbor(children[TOP_RIGHT].get()->GetEdge(DOWN));
		
		if(neighbors[DOWN].HasNeighbor())
			//{children[BOTTOM_RIGHT].get()->neighbors[DOWN].SetNeighbor(neighbors[DOWN].GetNeighbor(),neighbors[DOWN].GetNeighborSide());}
			{children[BOTTOM_RIGHT].get()->neighbors[DOWN].SetNeighbor(neighbors[DOWN].GetNeighboringEdge());}
		
		children[BOTTOM_RIGHT].get()->neighbors[LEFT].SetNeighbor(children[BOTTOM_LEFT].get()->GetEdge(RIGHT));
		
		if(neighbors[RIGHT].HasNeighbor())
			//{children[BOTTOM_RIGHT].get()->neighbors[RIGHT].SetNeighbor(neighbors[RIGHT].GetNeighbor(),neighbors[RIGHT].GetNeighborSide());}
			{children[BOTTOM_RIGHT].get()->neighbors[RIGHT].SetNeighbor(neighbors[RIGHT].GetNeighboringEdge());}
	}
	
	void SetupBottomLeftNeighbors()
	{
		children[BOTTOM_LEFT].get()->neighbors[UP].SetNeighbor(children[TOP_LEFT].get()->GetEdge(DOWN));
		
		if(neighbors[DOWN].HasNeighbor())
			//{children[BOTTOM_LEFT].get()->neighbors[DOWN].SetNeighbor(neighbors[DOWN].GetNeighbor(),neighbors[DOWN].GetNeighborSide());}
			{children[BOTTOM_LEFT].get()->neighbors[DOWN].SetNeighbor(neighbors[DOWN].GetNeighboringEdge());}
		
		if(neighbors[LEFT].HasNeighbor())
			//{children[BOTTOM_LEFT].get()->neighbors[LEFT].SetNeighbor(neighbors[LEFT].GetNeighbor(),neighbors[LEFT].GetNeighborSide());}
			{children[BOTTOM_LEFT].get()->neighbors[LEFT].SetNeighbor(neighbors[LEFT].GetNeighboringEdge());}
		
		children[BOTTOM_LEFT].get()->neighbors[RIGHT].SetNeighbor(children[BOTTOM_RIGHT].get()->GetEdge(LEFT));
	}
	
	void SplitNeighbors()
	{
		for(int i=0;i < 4;i++)
		{
			if(neighbors[i].HasNeighbor() /*&& !neighbors[i].GetNeighbor()->IsDisabled()*/)
			{
				for(unsigned int j=neighbors[i].GetNeighbor()->GetDepth();j < GetDepth();j++)
				{
					neighbors[i].GetNeighbor()->Split();
				}
			}
		}
	}
	
	NeighborDirection GetDirectionOf(PatchTerrain *other)
	{
		for(int i=0;i < 4;i++)
		{
			if(neighbors[i].HasNeighbor())
			{
				if(neighbors[i].GetNeighbor() == other)
					{return (NeighborDirection)i;}
			}
		}
		
		/*
		 * TODO: Lehet hogy kellene rendes hibakezelés.
		 */
		return UP;
	}
	
	void MergeNeighbors()
	{
		for(NeighborWrapper &neighbor : neighbors)
		{
			if(neighbor.HasNeighbor() && neighbor.GetNeighbor()->GetDepth() == GetDepth())
			{
				if(neighbor.GetNeighbor()->IsSplit())
				{
					PatchTerrain *leftTerrain	= neighbor.GetNeighboringEdge().GetLeft().GetTerrain();
					PatchTerrain *rightTerrain	= neighbor.GetNeighboringEdge().GetRight().GetTerrain();
					
					leftTerrain->SetNeighbor(neighbor.GetNeighbor()->GetDirectionOf(this),neighbor.GetOwnerEdge());
					rightTerrain->SetNeighbor(neighbor.GetNeighbor()->GetDirectionOf(this),neighbor.GetOwnerEdge());
					
					leftTerrain->Merge();
					rightTerrain->Merge();
				}
			}
		}
	}
	
	PatchTerrain(PatchTerrain *p_parent,const std::array<vec4,4> &p_vertices,unsigned int p_depth);
	
public:
	
	/*
	 * Quadtree funkciók
	 */
	
	unsigned int GetDepth() const
	{
		return depth;
	}
	
	ChildPosition GetPosition() const;
	
	bool IsSplit() const
	{
		return split;
	}
	
	const PatchTerrain *GetChild(unsigned int id) const
	{
		if(id < 4 && children[id].get() != nullptr)
		{
			return children[id].get();
		}
		
		return nullptr;
	}
	
	std::array<const PatchTerrain*,4> GetChildren() const
	{
		return {{children[0].get(),children[1].get(),children[2].get(),children[3].get()}};
	}
	
	const PatchTerrain *GetNeighbor(unsigned int id) const
	{
		if(id < 4 && neighbors[id].HasNeighbor())
		{
			return neighbors[id].GetNeighbor();
		}
		
		return nullptr;
	}
	
	const PatchTerrainEdge &GetEdge(NeighborDirection direction)
	{
		return edges[direction];
	}
	
	std::array<const PatchTerrain*,4> GetNeighbors() const
	{
		return {{GetNeighbor(0),GetNeighbor(1),GetNeighbor(2),GetNeighbor(3)}};
	}
	
	PatchTerrain *GetChild(unsigned int id)
	{
		if(id < 4 && children[id].get() != nullptr)
		{
			return children[id].get();
		}
		
		return nullptr;
	}
	
	std::array<PatchTerrain*,4> GetChildren()
	{
		return {{children[0].get(),children[1].get(),children[2].get(),children[3].get()}};
	}
	
	PatchTerrain *GetNeighbor(unsigned int id)
	{
		if(id < 4 && neighbors[id].HasNeighbor())
		{
			return neighbors[id].GetNeighbor();
		}
		
		return nullptr;
	}
	
	void SetNeighbor(unsigned int id,const PatchTerrainEdge &neighborEdge)
	{
		neighbors[id].SetNeighbor(neighborEdge);
	}
	
	std::array<PatchTerrain*,4> GetNeighbors()
	{
		return {{GetNeighbor(0),GetNeighbor(1),GetNeighbor(2),GetNeighbor(3)}};
	}
	
	void Split();
	
	void Merge();
	
	/*
	 * Heightmap funkciók
	 */
	
	const std::array<vec4,4> &GetVertices() const
	{
		return vertices;
	}
	
	vec4 GetCenter() const
	{
		return (vertices[0] + vertices[1] + vertices[2] + vertices[3]) / 4.0;
	}
	/*
	PatchTerrain &operator=(PatchTerrain &&other)
	{
		depth	= other.depth;
		split	= other.split;
		parent	= other.parent;
		
		for(int i=0;i < 4;i++)
		{
			children[i] = std::unique_ptr<PatchTerrain>(other.children[i].release());
			SetNeighbor(i,other.GetNeighbor(i));
		}
		
		return *this;
	}
	*/
	PatchTerrain(PatchTerrain &&other)
		:depth(other.depth),split(other.split),parent(other.parent)
	{
		for(int i=0;i < 4;i++)
		{
			children[i] = std::unique_ptr<PatchTerrain>(other.children[i].release());
			if(other.neighbors[i].HasNeighbor())
			{
				SetNeighbor(i,other.neighbors[i].GetNeighboringEdge()/*other.GetNeighbor(i),other.GetNeighborSide(i)*/);
			}
		}
	}
	
	PatchTerrain(const std::array<vec4,4> &p_vertices);
	
	~PatchTerrain();
};

/*
 * Tree traversal functions
 */

#include <functional>

void ForeachLeaf(PatchTerrain &patch_terrain,std::function<bool (PatchTerrain &terrain)> predicate,std::function<void (PatchTerrain &terrain)> operation);

void ForeachLeaf(PatchTerrain &patch_terrain,std::function<void (PatchTerrain &terrain)> operation);

#endif // PATCH_TERRAIN_H
