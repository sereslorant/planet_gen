#ifndef FFGL_DEMO_BASE_H
#define FFGL_DEMO_BASE_H

#include <DemoFramework/IDemo.h>

#include <SDL2/SDL_opengl.h>

#include <GL/glu.h>

class FFGLDemoBase : public IDemo
{
public:
	
	virtual void ApplyProjectionTransform() override
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		
		gluPerspective(90,1.0,1.0,-200.0);
	}
	
	virtual void ApplyViewTransform(const vec4 &position,float yaw,float pitch) override
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		glRotatef(pitch * 180 / PI,1.0,0.0,0.0);
		glRotatef(yaw * 180 / PI,0.0,1.0,0.0);
		glTranslatef(-position[0],-position[1],-position[2]);
	}
	
	FFGLDemoBase()
	{}
	
	virtual ~FFGLDemoBase() override
	{}
};

#endif // FFGL_DEMO_BASE_H
