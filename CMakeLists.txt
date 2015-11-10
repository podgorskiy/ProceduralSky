cmake_minimum_required (VERSION 2.8)
project (ProceduralSky)

option (GLFW_BUILD_EXAMPLES OFF)
option (GLFW_BUILD_TESTS OFF)
option (GLFW_INSTALL OFF)

add_subdirectory (libs/glfw)
include_directories(libs/glfw/include)

message("Executing gl3w_gen.py")
execute_process(COMMAND python gl3w_gen.py 
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/gl3w)
add_subdirectory (libs/gl3w)
include_directories(libs/gl3w/include)

include_directories(libs/glm)
 
find_package(OpenGL)
if (OPENGL_FOUND)
	message("OpenGL Correctly Found")
	include_directories(${OPENGL_INCLUDE_DIR})
else (OPENGL_FOUND)
	message("OpenGL environment missing")
endif (OPENGL_FOUND)

include_directories("${PROJECT_BINARY_DIR}")

file(GLOB SOURCES sources/*.cpp sources/*.h)

add_executable(ProceduralSky ${SOURCES})

target_link_libraries(ProceduralSky glfw gl3w ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES})

set(EXECUTABLE_NAME "ProceduralSky")