
#include "Camera3D.h"

#include <SDL2/SDL_opengl.h>

#include <GL/glu.h>


void Camera3D::Move(float dx,float dy,float dz)
{
	//vec4 up			= {0.0,std::sin(pitch),-std::cos(pitch)};
	vec4 up				= {0.0,1.0,0.0};
	vec4 local_forward	= {0.0,0.0,-1.0};
	
	vec4 left		= {std::cos(yaw),0.0f,std::sin(yaw)};
	
	vec4 forward	= {std::cos(yaw + PI/2.0f),0.0f,std::sin(yaw + PI/2.0f)};
	
	position += dx * left;
	position += dy * up;
	position += dz * forward;
}

void Camera3D::RotateYaw(float dangle)
{
	yaw += dangle;
}

void Camera3D::RotatePitch(float dangle)
{
	pitch += dangle;
}
/*
void Camera3D::ApplyProjectionTransform(IDemo &demo)
{
	gluPerspective(90,1.0,1.0,-200.0);
}

void Camera3D::ApplyViewTransform(IDemo &demo)
{
	glRotatef(pitch * 180 / PI,1.0,0.0,0.0);
	glRotatef(yaw * 180 / PI,0.0,1.0,0.0);
	glTranslatef(-position[0],-position[1],-position[2]);
}
*/
