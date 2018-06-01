
#include <DemoFramework/SDL2DemoFramework.h>
#include <DemoFramework/Camera3D.h>

#include <PlanetDemo/Demo/GL4Demo/GL4TriDemo.h>

int main(int argc,char *argv[])
{
	Camera3D *camera = new Camera3D;
	SDL2DemoFramework DemoFramework(800,600,nullptr,camera,1,10000);
	SDL2Image NoiseMap("NoiseHiRes.bmp");
	
	DemoFramework.SetDemo(new GL4TriDemo(NoiseMap,*camera));
	
	DemoFramework.Loop();
	
	return 0;
}

