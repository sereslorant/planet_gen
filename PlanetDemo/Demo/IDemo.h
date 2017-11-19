#ifndef IDEMO_H
#define IDEMO_H

#include <glmath.hpp>

class IDemo
{
public:
	
	virtual void ApplyProjectionTransform() = 0;
	virtual void ApplyViewTransform(const vec4 &position,float yaw,float pitch) = 0;
	
	virtual void Logic() = 0;
	virtual void Draw() = 0;
	
	IDemo() {}
	virtual ~IDemo() {}
};

class ICamera
{
public:
	virtual void Move(float dx,float dy,float dz) = 0;
	virtual void RotateYaw(float dangle) = 0;
	virtual void RotatePitch(float dangle) = 0;
	
	virtual void ApplyProjectionTransform(IDemo &demo) = 0;
	virtual void ApplyViewTransform(IDemo &demo) = 0;
	
	ICamera()
	{}
	virtual ~ICamera()
	{}
};

#include <memory>

class GrayscaleImage
{
protected:
	unsigned int width;
	unsigned int height;
	
	std::unique_ptr<float []> array;
	
	void Resize()
	{
		array = std::unique_ptr<float []>(new float[width * height]);
	}
	
	GrayscaleImage()
	{}
	
public:
	
	unsigned int GetWidth()
	{
		return width;
	}
	
	unsigned int GetHeight()
	{
		return height;
	}
	
	float *GetData()
	{
		return array.get();
	}
	
	float GetValue(float x,float y)
	{
		//float x_idf = x * width;
		//float y_idf = y * height;
		
		unsigned int x_id = x * width;
		unsigned int y_id = y * height;
		
		x_id %= width;
		y_id %= height;
		
		//float x_weight = x_idf - x_id;
		//float y_weight = y_idf - y_id;
		
		//unsigned int x_next = x_id + 1 % width;
		//unsigned int y_next = x_id + 1 % width;
		
		return array[y_id * width + x_id];// + x_weight * array[y_id * width + x_next] + y_weight * array[y_next * width + x_id];
	}
	
	GrayscaleImage(unsigned int p_width,unsigned int p_height)
		:width(p_width),height(p_height)
	{
		Resize();
	}
	
	virtual ~GrayscaleImage()
	{}
};

constexpr float PI = 3.141592653589793;

#endif // IDEMO_H
