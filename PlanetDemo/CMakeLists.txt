
add_executable(terrain_demo TerrainMain.cpp Demo/TerrainDemo/TerrainDemo.cpp Camera3D.cpp)
target_link_libraries(terrain_demo PlanetLib ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

add_executable(planet_demo PlanetMain.cpp Demo/PlanetDemo/PlanetDemo.cpp Camera3D.cpp)
target_link_libraries(planet_demo PlanetLib ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES} Multithreading)

add_executable(fractal_demo FractalMain.cpp Demo/FractalDemo/FractalDemo.cpp Camera3D.cpp)
target_link_libraries(fractal_demo PlanetLib ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

add_executable(fractal_planet_demo FractalPlanetMain.cpp Demo/FractalPlanetDemo/FractalPlanetDemo.cpp Camera3D.cpp)
target_link_libraries(fractal_planet_demo PlanetLib ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES} Multithreading)

add_executable(gl4_tri_demo GL4TriMain.cpp Demo/GL4Demo/GLExt.cpp Demo/GL4Demo/GLShaderSrc.cpp Camera3D.cpp)
target_link_libraries(gl4_tri_demo PlanetLib ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES} Multithreading)

add_executable(gl4_tess_demo GL4TessMain.cpp Demo/GL4Demo/GLExt.cpp Demo/GL4Demo/GLShaderSrc.cpp Demo/GL4Demo/PbrShaderSrc.cpp Camera3D.cpp)
target_link_libraries(gl4_tess_demo PlanetLib ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES} Multithreading)

file(COPY  ${CMAKE_SOURCE_DIR}/PlanetDemo/Noise.bmp DESTINATION ${CMAKE_BINARY_DIR}/PlanetDemo/)
file(COPY  ${CMAKE_SOURCE_DIR}/PlanetDemo/NoiseHiRes.bmp DESTINATION ${CMAKE_BINARY_DIR}/PlanetDemo/)
