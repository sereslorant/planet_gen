
#include "SDL2DemoFramework.h"

#include <PlanetDemo/Demo/GL4Demo/GL4TessDemo.h>
#include "Camera3D.h"

int main(int argc,char *argv[])
{
	Camera3D *camera = new Camera3D;
	SDL2DemoFramework DemoFramework(800,600,nullptr,camera,true,10000);
	SDL2Image NoiseMap("NoiseHiRes.bmp");
	
	DemoFramework.SetDemo(new GL4TessDemo(NoiseMap,*camera));
	
	DemoFramework.Loop();
	
	return 0;
}

