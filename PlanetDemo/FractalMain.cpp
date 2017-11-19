
#include "SDL2DemoFramework.h"

#include <PlanetDemo/Demo/FractalDemo/FractalDemo.h>
#include "Camera3D.h"

int main(int argc,char *argv[])
{
	SDL2DemoFramework DemoFramework(800,600,nullptr,new Camera3D);
	SDL2Image NoiseMap("Noise.bmp");
	
	DemoFramework.SetDemo(new FractalDemo(NoiseMap));
	
	DemoFramework.Loop();
	
	return 0;
}

