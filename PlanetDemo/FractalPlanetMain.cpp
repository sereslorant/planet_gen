
#include "SDL2DemoFramework.h"

#include <PlanetDemo/Demo/FractalPlanetDemo/FractalPlanetDemo.h>
#include "Camera3D.h"

int main(int argc,char *argv[])
{
	Camera3D *camera = new Camera3D;
	SDL2DemoFramework DemoFramework(800,600,nullptr,camera);
	SDL2Image NoiseMap("NoiseHiRes.bmp");
	
	DemoFramework.SetDemo(new FractalPlanetDemo(NoiseMap,*camera));
	
	DemoFramework.Loop();
	
	return 0;
}

