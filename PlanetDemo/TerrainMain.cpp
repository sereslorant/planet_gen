
#include "SDL2DemoFramework.h"

#include <PlanetDemo/Demo/TerrainDemo/TerrainDemo.h>
#include "Camera3D.h"

int main(int argc,char *argv[])
{
	SDL2DemoFramework DemoFramework(800,600,nullptr,new Camera3D);
	
	DemoFramework.SetDemo(new TerrainDemo);
	
	DemoFramework.Loop();
	
	return 0;
}

