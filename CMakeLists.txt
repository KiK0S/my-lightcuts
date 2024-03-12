cmake_minimum_required(VERSION 3.9)

project(MyRenderer LANGUAGES CXX)

find_package(OpenMP REQUIRED)

add_subdirectory(External)

add_executable (
	MyRenderer
	Sources/Main.cpp
	Sources/Console.h
	Sources/Console.cpp
	Sources/Error.h
	Sources/Error.cpp
	Sources/Image.h
	Sources/Transform.h
	Sources/Camera.h
	Sources/Camera.cpp
	Sources/Mesh.h
	Sources/Mesh.cpp
	Sources/MeshLoader.h
	Sources/MeshLoader.cpp
	Sources/RayTracer.h
	Sources/RayTracer.cpp
	Sources/Rasterizer.h
	Sources/Rasterizer.cpp
	Sources/Resources.h
	Sources/ShaderProgram.h
	Sources/ShaderProgram.cpp
	Sources/Scene.h
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





