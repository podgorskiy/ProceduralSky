cmake_minimum_required (VERSION 2.6)
project (ProceduralSky)

option (GLFW_BUILD_EXAMPLES OFF)
option (GLFW_BUILD_TESTS OFF)
option (GLFW_INSTALL OFF)

add_subdirectory (libs/glfw)

include_directories(libs/glfw/include)
 
find_package(OpenGL)
if (OPENGL_FOUND)
	MESSAGE("OpenGL Correctly Found")
	include_directories(${OPENGL_INCLUDE_DIR})
else (OPENGL_FOUND)
	MESSAGE("OpenGL environment missing")
endif (OPENGL_FOUND)

include_directories("${PROJECT_BINARY_DIR}")

add_executable(ProceduralSky sources/main.cpp)

target_link_libraries(ProceduralSky glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES})

set(EXECUTABLE_NAME "ProceduralSky")