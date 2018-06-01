
add_executable(flat_terrain_3d_demo FlatTerrain3DMain.cpp GLShaderSrc.cpp PbrShaderSrc.cpp)
target_link_libraries(flat_terrain_3d_demo DemoFramework GL4Utils PlanetLib TerrainLib ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

add_executable(param_surf_demo ParamSurfMain.cpp)
target_link_libraries(param_surf_demo DemoFramework GL4Utils TerrainLib ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

add_executable(param_surf_noise_demo ParamSurfNoiseMain.cpp)
target_link_libraries(param_surf_noise_demo DemoFramework GL4Utils TerrainLib ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

add_executable(editor_demo EditorMain.cpp)
target_link_libraries(editor_demo DemoFramework GL4Utils TerrainLib ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

add_executable(editor_3d_demo Editor3DMain.cpp GLShaderSrc.cpp PbrShaderSrc.cpp)
target_link_libraries(editor_3d_demo DemoFramework GL4Utils PlanetLib TerrainLib ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

add_executable(noise_dft_demo NoiseDftMain.cpp)
target_link_libraries(noise_dft_demo DemoFramework GL4Utils TerrainLib ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

add_executable(noise_stationary_demo NoiseStationaryMain.cpp)
target_link_libraries(noise_stationary_demo DemoFramework GL4Utils TerrainLib ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

# Legacy

#add_executable(flat_terrain_demo Legacy/FlatTerrainMain.cpp)
#target_link_libraries(flat_terrain_demo DemoFramework GL4Utils TerrainLib ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

#add_executable(flat_noise_terrain_demo Legacy/FlatNoiseTerrainMain.cpp)
#target_link_libraries(flat_noise_terrain_demo DemoFramework GL4Utils TerrainLib ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})
