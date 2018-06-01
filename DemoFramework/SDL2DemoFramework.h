#ifndef SDL2_DEMO_FRAMEWORK_H
#define SDL2_DEMO_FRAMEWORK_H

#include "IDemo.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <iostream>

class SDL2Image : public GrayscaleImage
{
public:
	
	SDL2Image(const std::string &filename)
	{
		SDL_Surface *surface = SDL_LoadBMP(filename.c_str());
		
		width	= surface->w;
		height	= surface->h;
		
		Resize();
		
		for(int i=0;i < width*height;i++)
		{
			array[i] = ((unsigned char *)surface->pixels)[i*3 + 1] / 255.0;
		}
		
		SDL_FreeSurface(surface);
	}
	
	virtual ~SDL2Image() override
	{}
};

class SDL2DemoFramework
{
private:
	SDL_Window *Window;
    SDL_GLContext GLContext;
	
	unsigned int ScreenWidth = 0;
	unsigned int ScreenHeight = 0;
	
	bool Running = true;
	
	bool CameraForward = false;
	bool CameraBack = false;
	bool CameraLeft = false;
	bool CameraRight = false;
	bool CameraUp = false;
	bool CameraDown = false;
	bool CameraRotLeft = false;
	bool CameraRotRight = false;
	bool CameraRotUp = false;
	bool CameraRotDown = false;
	
	bool accelerate_camera = false;
	bool decelerate_camera = false;
	
	float camera_speed;// = 20.0;
	
	IDemo *demo = nullptr;
	ICamera *Camera = nullptr;
	
	void Input()
	{
		SDL_Event Event;
		while(SDL_PollEvent(&Event))
		{
			if(Event.type == SDL_QUIT)
			{
				Running = false;
			}
			
			if(Event.type == SDL_KEYDOWN)
			{
				if(Event.key.keysym.sym == SDLK_w)
				{CameraForward = true;}
				if(Event.key.keysym.sym == SDLK_s)
				{CameraBack = true;}
				if(Event.key.keysym.sym == SDLK_a)
				{CameraLeft = true;}
				if(Event.key.keysym.sym == SDLK_d)
				{CameraRight = true;}
				if(Event.key.keysym.sym == SDLK_r)
				{CameraUp = true;}
				if(Event.key.keysym.sym == SDLK_f)
				{CameraDown = true;}
				
				if(Event.key.keysym.sym == SDLK_q)
				{CameraRotLeft = true;}
				if(Event.key.keysym.sym == SDLK_e)
				{CameraRotRight = true;}
				
				if(Event.key.keysym.sym == SDLK_t)
				{CameraRotUp = true;}
				if(Event.key.keysym.sym == SDLK_g)
				{CameraRotDown = true;}
				
				if(Event.key.keysym.sym == SDLK_u)
				{accelerate_camera = true;}
				if(Event.key.keysym.sym == SDLK_j)
				{decelerate_camera = true;}
			}
			
			if(Event.type == SDL_KEYUP)
			{
				if(Event.key.keysym.sym == SDLK_w)
				{CameraForward = false;}
				if(Event.key.keysym.sym == SDLK_s)
				{CameraBack = false;}
				if(Event.key.keysym.sym == SDLK_a)
				{CameraLeft = false;}
				if(Event.key.keysym.sym == SDLK_d)
				{CameraRight = false;}
				if(Event.key.keysym.sym == SDLK_r)
				{CameraUp = false;}
				if(Event.key.keysym.sym == SDLK_f)
				{CameraDown = false;}
				
				if(Event.key.keysym.sym == SDLK_q)
				{CameraRotLeft = false;}
				if(Event.key.keysym.sym == SDLK_e)
				{CameraRotRight = false;}
				
				if(Event.key.keysym.sym == SDLK_t)
				{CameraRotUp = false;}
				if(Event.key.keysym.sym == SDLK_g)
				{CameraRotDown = false;}
				
				if(Event.key.keysym.sym == SDLK_u)
				{accelerate_camera = false;}
				if(Event.key.keysym.sym == SDLK_j)
				{decelerate_camera = false;}
			}
		}
	}
	
	void Logic()
	{
		if(CameraForward)
			{Camera->Move(0.0,0.0,-camera_speed);}
		if(CameraBack)
			{Camera->Move(0.0,0.0, camera_speed);}
		if(CameraLeft)
			{Camera->Move(-camera_speed,0.0,0.0);}
		if(CameraRight)
			{Camera->Move( camera_speed,0.0,0.0);}
		if(CameraUp)
			{Camera->Move(0.0, camera_speed,0.0);}
		if(CameraDown)
			{Camera->Move(0.0,-camera_speed,0.0);}
		
		if(CameraRotLeft)
			{Camera->RotateYaw(-PI / 20.0);}
		if(CameraRotRight)
			{Camera->RotateYaw( PI / 20.0);}
		
		if(CameraRotUp)
			{Camera->RotatePitch(-PI / 20.0);}
		if(CameraRotDown)
			{Camera->RotatePitch( PI / 20.0);}
		
		if(accelerate_camera)
			{camera_speed += 5;}
		
		if(decelerate_camera)
		{
			camera_speed -= 5;
			
			if(camera_speed < 0.0)
				{camera_speed = 0.0;}
		}
		
		demo->Logic();
	}
	
	void Rendering()
	{
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glViewport(0, 0, ScreenWidth, ScreenHeight);
		
		Camera->ApplyProjectionTransform(*demo);
		Camera->ApplyViewTransform(*demo);
		
		demo->Draw();
		
		SDL_GL_SwapWindow(Window);
	}
	
public:
	
	void SetDemo(IDemo *p_demo)
	{
		demo = p_demo;
	}
	
	void Loop()
	{
		while(Running)
		{
			int BeginTime = SDL_GetTicks();
			
			Input();
			Logic();
			Rendering();
			
			int EndTime = SDL_GetTicks();
			std::cout << "Frame time: " << (EndTime-BeginTime) << "ms\nFps: " << 1000.0/(EndTime-BeginTime) << std::endl;
		}
	}
	
	SDL2DemoFramework(int width,int height,IDemo *demo,ICamera *camera,int gl_version = 0,float p_cam_speed = 20.0)
		:ScreenWidth(width),ScreenHeight(height),demo(demo),Camera(camera),camera_speed(p_cam_speed)
	{
		SDL_Init(SDL_INIT_EVERYTHING);
		
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
		//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		
		if(gl_version == 2)
		{
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		}
		else if(gl_version == 1)
		{
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		}
		else
		{
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		}
		
		Window = SDL_CreateWindow("FuGenDemo",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,SDL_WINDOW_OPENGL);
		if(Window == nullptr)
		{
			return;
		}
		
		GLContext = SDL_GL_CreateContext(Window);
		if(GLContext == nullptr)
		{
			return;
		}
		
		SDL_ShowWindow(Window);
	}
	
	~SDL2DemoFramework()
	{
		delete demo;
		delete Camera;
		
		SDL_GL_DeleteContext(GLContext);
		SDL_DestroyWindow(Window);
		
		SDL_Quit();
	}
};

#endif // SDL2_DEMO_FRAMEWORK_H
