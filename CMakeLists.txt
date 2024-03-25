cmake_minimum_required(VERSION 3.9)

project(MyRenderer LANGUAGES CXX)

find_package(OpenMP REQUIRED)

add_subdirectory(External)

add_executable (
	MyRenderer
	Sources/Main.cpp
	Sources/Console.cpp
	Sources/Error.cpp
	Sources/Camera.cpp
	Sources/Mesh.cpp
	Sources/MeshLoader.cpp
	Sources/LightSource.cpp
	Sources/LightCut.cpp
	Sources/BoundingBox.cpp
	Sources/BRDF.cpp
	Sources/Random.cpp
	Sources/Ray.cpp
	Sources/RayTracer.cpp
	Sources/Rasterizer.cpp
	Sources/ShaderProgram.cpp
)

set_target_properties(MyRenderer PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Copy the shader files in the binary location.

include_directories(${CMAKE_CURRENT_SOURCE_DIR} External/stb_image/)

target_link_libraries(MyRenderer LINK_PRIVATE glad)

target_link_libraries(MyRenderer LINK_PRIVATE glfw)

target_link_libraries(MyRenderer LINK_PRIVATE glm)

target_link_libraries(MyRenderer PRIVATE OpenMP::OpenMP_CXX)





