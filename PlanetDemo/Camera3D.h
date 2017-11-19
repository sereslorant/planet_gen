#ifndef CAMERA3D_H
#define CAMERA3D_H

#include <PlanetDemo/Demo/IDemo.h>

#include <glmath.hpp>

class Camera3D : public ICamera
{
private:
	vec4 position = {0,0.0,0};
	
	float yaw = 0.0;
	float pitch = 0.0;
	
public:
	
	const vec4 &GetPosition() const
	{
		return position;
	}
	
	virtual void Move(float dx,float dy,float dz) override;
	
	virtual void RotateYaw(float dangle) override;
	
	virtual void RotatePitch(float dangle) override;
	
	virtual void ApplyProjectionTransform(IDemo &demo) override
	{
		demo.ApplyProjectionTransform();
	}
	
	virtual void ApplyViewTransform(IDemo &demo) override
	{
		demo.ApplyViewTransform(position,yaw,pitch);
	}
	
	Camera3D()
	{}
	
	virtual ~Camera3D() override
	{}
};

#endif // CAMERA3D_H
