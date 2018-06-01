
#include <TerrainLib/LineStrip.h>

#include <iostream>

void TestPushBackPoint()
{
	LineStrip test_line_strip;
	
	Vertex &new_vertex = test_line_strip.PushBackPoint({0.2,0.4},1.0,0.3);
	Vertex &back_vertex = test_line_strip.PushBackPoint({0.1,0.0},2.0,0.4);
	
	bool Success =	std::abs(new_vertex.position[0] - 0.2) < 1e-4 &&
					std::abs(new_vertex.position[1] - 0.4) < 1e-4 &&
					std::abs(new_vertex.height - 1.0) < 1e-4 &&
					std::abs(new_vertex.width - 0.3) < 1e-4 &&
					std::abs(back_vertex.position[0] - 0.1) < 1e-4 &&
					std::abs(back_vertex.position[1] - 0.0) < 1e-4 &&
					std::abs(back_vertex.height - 2.0) < 1e-4 &&
					std::abs(back_vertex.width - 0.4) < 1e-4 &&
					&back_vertex == &test_line_strip.GetBack();
	
	std::cout << "Testing PushBackPoint: " << Success << std::endl;
}

void TestPushFrontPoint()
{
	LineStrip test_line_strip;
	
	Vertex &new_vertex = test_line_strip.PushFrontPoint({0.2,0.4},1.0,0.3);
	Vertex &front_vertex = test_line_strip.PushFrontPoint({0.1,0.0},2.0,0.4);
	
	bool Success =	std::abs(new_vertex.position[0] - 0.2) < 1e-4 &&
					std::abs(new_vertex.position[1] - 0.4) < 1e-4 &&
					std::abs(new_vertex.height - 1.0) < 1e-4 &&
					std::abs(new_vertex.width - 0.3) < 1e-4 &&
					std::abs(front_vertex.position[0] - 0.1) < 1e-4 &&
					std::abs(front_vertex.position[1] - 0.0) < 1e-4 &&
					std::abs(front_vertex.height - 2.0) < 1e-4 &&
					std::abs(front_vertex.width - 0.4) < 1e-4 &&
					&front_vertex == &test_line_strip.GetFront();
	
	std::cout << "Testing PushFrontPoint: " << Success << std::endl;
}

int main(int argc,char *argv[])
{
	TestPushBackPoint();
	TestPushFrontPoint();
	
	return 0;
}
