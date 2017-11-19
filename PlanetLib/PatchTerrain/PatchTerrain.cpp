
#include "PatchTerrain.h"

PatchTerrain::ChildPosition PatchTerrain::GetPosition() const
{
	if(parent != nullptr)
	{
		for(int i=0;i < 4;i++)
		{
			if(parent->children[i].get() == this)
				{return (ChildPosition)i;}
		}
	}
	
	return NONE;
}

void PatchTerrain::Split()
{
	if(!split /*&& !disabled*/)
	{
		split = true;
		
		SplitNeighbors();
		
		std::array<vec4,4> new_vertices =
			{{
				(vertices[0] + vertices[1]) / 2.0,
				(vertices[1] + vertices[2]) / 2.0,
				(vertices[2] + vertices[3]) / 2.0,
				(vertices[3] + vertices[0]) / 2.0,
			}};
		
		vec4 center = (vertices[0] + vertices[1] + vertices[2] + vertices[3]) / 4.0;
		
		children[TOP_LEFT]		= std::unique_ptr<PatchTerrain>(new PatchTerrain(this,{{vertices[0],new_vertices[0],center,new_vertices[3],}},depth+1));
		children[TOP_RIGHT]		= std::unique_ptr<PatchTerrain>(new PatchTerrain(this,{{new_vertices[0],vertices[1],new_vertices[1],center,}},depth+1));
		children[BOTTOM_RIGHT]	= std::unique_ptr<PatchTerrain>(new PatchTerrain(this,{{center,new_vertices[1],vertices[2],new_vertices[2],}},depth+1));
		children[BOTTOM_LEFT]	= std::unique_ptr<PatchTerrain>(new PatchTerrain(this,{{new_vertices[3],center,new_vertices[2],vertices[3],}},depth+1));
		
		/*
		 * TOP_LEFT
		 */
		
		SetupTopLeftNeighbors();
		
		/*
		 * TOP_RIGHT
		 */
		
		SetupTopRightNeighbors();
		
		/*
		 * BOTTOM_RIGHT
		 */
		
		SetupBottomRightNeighbors();
		
		/*
		 * BOTTOM_LEFT
		 */
		
		SetupBottomLeftNeighbors();
	}
}

void PatchTerrain::Merge()
{
	if(split /*&& !disabled*/)
	{
		split = false;
		
		MergeNeighbors();
		
		for(auto &child : children)
		{
			child.reset();
		}
	}
}
	
PatchTerrain::PatchTerrain(PatchTerrain *p_parent,const std::array<vec4,4> &p_vertices,unsigned int p_depth)
	:depth(p_depth),parent(p_parent),vertices(p_vertices)
{
	
}

PatchTerrain::PatchTerrain(const std::array<vec4,4> &p_vertices)
	:vertices(p_vertices)
{
	
}

PatchTerrain::~PatchTerrain()
{
	
}

/*
 * Tree traversal functions
 */

void ForeachLeaf(PatchTerrain &patch_terrain,std::function<bool (PatchTerrain &terrain)> predicate,std::function<void (PatchTerrain &terrain)> operation)
{
	if(!predicate(patch_terrain))
	{
		return;
	}
	
	if(patch_terrain.IsSplit())
	{
		for(PatchTerrain *child : patch_terrain.GetChildren())
		{
			ForeachLeaf(*child,predicate,operation);
		}
	}
	else
	{
		operation(patch_terrain);
	}
}

void ForeachLeaf(PatchTerrain &patch_terrain,std::function<void (PatchTerrain &terrain)> operation)
{
	std::function<bool(PatchTerrain &)> constTrue = [](PatchTerrain &) {return true;};
	
	ForeachLeaf(patch_terrain,constTrue,operation);
}
