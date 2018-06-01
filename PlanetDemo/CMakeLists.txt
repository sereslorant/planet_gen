
add_executable(terrain_demo TerrainMain.cpp Demo/TerrainDemo/TerrainDemo.cpp)
target_link_libraries(terrain_demo PlanetLib DemoFramework ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

add_executable(planet_demo PlanetMain.cpp Demo/PlanetDemo/PlanetDemo.cpp)
target_link_libraries(planet_demo PlanetLib DemoFramework Multithreading ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

add_executable(fractal_demo FractalMain.cpp Demo/FractalDemo/FractalDemo.cpp)
target_link_libraries(fractal_demo PlanetLib DemoFramework ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

add_executable(fractal_planet_demo FractalPlanetMain.cpp Demo/FractalPlanetDemo/FractalPlanetDemo.cpp)
target_link_libraries(fractal_planet_demo PlanetLib DemoFramework Multithreading ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

add_executable(gl4_tri_demo GL4TriMain.cpp Demo/GL4Demo/GLShaderSrc.cpp)
target_link_libraries(gl4_tri_demo PlanetLib DemoFramework Multithreading GL4Utils ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

add_executable(gl4_tess_demo GL4TessMain.cpp Demo/GL4Demo/GLShaderSrc.cpp Demo/GL4Demo/PbrShaderSrc.cpp)
target_link_libraries(gl4_tess_demo PlanetLib DemoFramework Multithreading GL4Utils ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

file(COPY  ${CMAKE_SOURCE_DIR}/PlanetDemo/Noise.bmp DESTINATION ${CMAKE_BINARY_DIR}/PlanetDemo/)
file(COPY  ${CMAKE_SOURCE_DIR}/PlanetDemo/NoiseHiRes.bmp DESTINATION ${CMAKE_BINARY_DIR}/PlanetDemo/)
