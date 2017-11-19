
#include "SDL2DemoFramework.h"

#include <PlanetDemo/Demo/PlanetDemo/PlanetDemo.h>
#include "Camera3D.h"

int main(int argc,char *argv[])
{
	Camera3D *camera = new Camera3D;
	SDL2DemoFramework DemoFramework(800,600,nullptr,camera);
	
	DemoFramework.SetDemo(new PlanetDemo(*camera));
	
	DemoFramework.Loop();
	
	return 0;
}
