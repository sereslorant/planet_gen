#ifndef GL4_DEMO_BASE_H
#define GL4_DEMO_BASE_H

#include <DemoFramework/IDemo.h>

#include "GLExt.h"

class GL4DemoBase : public IDemo
{
protected:
	mat4 projection_matrix;
	mat4 view_matrix;
	
public:
	
	virtual void ApplyProjectionTransform() override
	{
		projection_matrix = perspective<float>(PI/2.0,1.0,1.0,-200.0);
	}
	
	virtual void ApplyViewTransform(const vec4 &position,float yaw,float pitch) override
	{
		mat4 rot_x = rotate_x<float>(pitch);
		mat4 rot_y = rotate_y<float>(yaw);
		mat4 move = translate<float>(-1.0 * position);
		
		//view_matrix = rot_x * rot_y * move;
		view_matrix = move * rot_y * rot_x;
	}
	
	GL4DemoBase()
	{
		GLLoadExtensions();
	}
	
	virtual ~GL4DemoBase() override
	{}
};

#endif // GL4_DEMO_BASE_H
