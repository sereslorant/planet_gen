cmake_minimum_required(VERSION 2.6)
project(planet_gen)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

set(CMAKE_CXX_STANDARD 14)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

string(STRIP ${SDL2_INCLUDE_DIRS} SDL2_INCLUDE_DIRS)
string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)

include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${SDL2_INCLUDE_DIRS})

include_directories(".")

add_subdirectory(PlanetDemo)
add_subdirectory(PlanetLib)
add_subdirectory(PlanetTest)
add_subdirectory(Multithreading)
