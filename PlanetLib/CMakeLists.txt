
set(PATCH_TERRAIN_SRC 	PatchTerrain/PatchTerrain.cpp
						PatchTerrain/PatchTerrainEdge.cpp
						PatchTerrain/NeighborWrapper.cpp
						)

add_library(PlanetLib	${PATCH_TERRAIN_SRC}
						MeshGenerator/LeafGenerator.cpp
						PlanetTerrain.cpp
						)
