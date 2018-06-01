#ifndef DEMO_UTILS_H
#define DEMO_UTILS_H

#include <SDL2/SDL_opengl.h>

#include <DemoFramework/IDemo.h>

#include <PlanetLib/Heightmap/IParametricSurface.h>

#include <PlanetLib/PatchTerrain/PatchTerrain.h>

#include <vector>

class RippleHeightmap : public IHeightmap
{
private:
	float amplitude;
	vec4 center;
	
public:
	
	virtual float GetValue(const vec4 &vec) const override
	{
		float cos_value = std::cos(length(vec - center)*300);
		
		return amplitude*cos_value;
	}
	
	RippleHeightmap(float p_amplitude,const vec4 &p_center)
		:amplitude(p_amplitude),center(p_center)
	{}
	
	virtual ~RippleHeightmap() override
	{}
};

class ImageHeightmap : public IHeightmap
{
private:
	GrayscaleImage &image;
	
public:
	
	virtual float GetValue(const vec4 &vec) const override
	{
		return image.GetValue(vec[0],vec[1]);
	}
	
	ImageHeightmap(GrayscaleImage &p_image)
		:image(p_image)
	{}
	
	virtual ~ImageHeightmap() override
	{}
};

class ScaledHeightmap : public IHeightmap
{
private:
	GrayscaleImage &image;
	
public:
	
	virtual float GetValue(const vec4 &vec) const override
	{
		return image.GetValue(vec[0],vec[1])*100000;
	}
	
	ScaledHeightmap(GrayscaleImage &p_image)
		:image(p_image)
	{}
	
	virtual ~ScaledHeightmap() override
	{}
};

/*
 * TODO: Kódduplikáció a FlatSurface osztáállyal!
 */
class ConstHeightmap : public IParametricSurface
{
private:
	const vec4 normal = {0.0,1.0f,0.0f};
	
public:
	
	virtual vec4 GetNormal(const vec4 &vec) const override
	{
		/*
		 * TODO: Meg kéne csinálni!
		 */
		return normal;
	}
	
	virtual vec4 GetValue(const vec4 &vec) const override
	{
		return {vec[0],-30.0,vec[1]};
	}
	
	ConstHeightmap()
	{}
	
	virtual ~ConstHeightmap() override
	{}
};

class WaveHeightmap : public IParametricSurface
{
private:
	vec4 center;
	
	const vec4 normal = {0.0,1.0f,0.0f};
	
public:
	
	virtual vec4 GetNormal(const vec4 &vec) const override
	{
		/*
		 * TODO: Meg kéne csinálni!
		 */
		return normal;
	}
	
	virtual vec4 GetValue(const vec4 &vec) const override
	{
		float Amplitude = 50.0;
		float CosValue = std::cos(length(center - vec4(vec[0],vec[1])) / 20.0);
		
		return {vec[0],Amplitude * CosValue,vec[1]};
	}
	
	WaveHeightmap(const vec4 &p_center)
		:center(p_center)
	{}
	
	virtual ~WaveHeightmap() override
	{}
};

class HeightmapRenderer
{
protected:
	const IParametricSurface &heightmap;
	
public:
	
	virtual void Draw(PatchTerrain &patch_terrain) = 0;
	
	HeightmapRenderer(const IParametricSurface &p_heightmap)
		:heightmap(p_heightmap)
	{
		
	}
};

class SimpleRenderer : public HeightmapRenderer
{
public:
	
	void Draw(PatchTerrain &patch_terrain)
	{
		glBegin(GL_QUADS);
		for(const vec4 &vertex : patch_terrain.GetVertices())
		{
			vec4 heightmap_point = heightmap.GetValue(vertex);
			glVertex3f(heightmap_point[0],heightmap_point[1],heightmap_point[2]);
		}
		glEnd();
	}
	
	SimpleRenderer(const IParametricSurface &p_heightmap)
		:HeightmapRenderer(p_heightmap)
	{}
};

#include <PlanetLib/MeshGenerator/LeafGenerator.h>

class NeighborTrackingRenderer : public HeightmapRenderer
{
public:
	
	void Draw(PatchTerrain &patch_terrain)
	{
		LeafGenerator leaf_vertices(patch_terrain);
		
		glBegin(GL_TRIANGLES);
		for(unsigned int i=0;i < leaf_vertices.GetIndexArraySize();i++)
		{
			vec4 heightmap_point = heightmap.GetValue(
				leaf_vertices.GetVertexArray()[leaf_vertices.GetIndexArray()[i]]
			);
			
			glVertex3f(heightmap_point[0],heightmap_point[1],heightmap_point[2]);
		}
		glEnd();
	}
	
	NeighborTrackingRenderer(const IParametricSurface &p_heightmap)
		:HeightmapRenderer(p_heightmap)
	{}
};

#endif // DEMO_UTILS_H
