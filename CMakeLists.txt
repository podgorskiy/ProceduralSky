cmake_minimum_required (VERSION 2.8)
project (ProceduralSky)

option (GLFW_BUILD_EXAMPLES OFF)
option (GLFW_BUILD_TESTS OFF)
option (GLFW_INSTALL OFF)

message("Executing gl3w_gen.py")
execute_process(COMMAND python gl3w_gen.py WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/gl3w)

add_subdirectory (libs/glfw)
add_subdirectory (libs/gl3w)
add_subdirectory (libs/pugixml/scripts)

include_directories(libs/glfw/include)
include_directories(libs/gl3w/include)
include_directories(libs/glm)
include_directories(libs/imgui)
include_directories(libs/pugixml/src)
include_directories(libs/lz4/lib)
include_directories(libs/SimpleText/include)
include_directories(sources/SBFramework)

find_package(OpenGL)
if (OPENGL_FOUND)
	message("OpenGL Correctly Found")
	include_directories(${OPENGL_INCLUDE_DIR})
else (OPENGL_FOUND)
	message("OpenGL environment missing")
endif (OPENGL_FOUND)

include_directories("${PROJECT_BINARY_DIR}")

file(GLOB_RECURSE SOURCES sources/*.cpp sources/*.h)
file(GLOB SOURCES_IMGUI libs/imgui/*.cpp libs/imgui/*.h)

set(LZ4_DIR libs/lz4/lib/)
set(SOURCES_LZ4 ${LZ4_DIR}lz4.c ${LZ4_DIR}lz4hc.c ${LZ4_DIR}lz4.h ${LZ4_DIR}lz4hc.h ${LZ4_DIR}xxhash.c ${LZ4_DIR}xxhash.h)

file(GLOB SOURCES_SBFRAMEWORK sources/SBFramework/*.cpp sources/SBFramework/*.h)
file(GLOB SOURCES_SBFRAMEWORK_SBSCENE sources/SBFramework/SBScene/*.cpp sources/SBFramework/SBScene/*.h)
file(GLOB SOURCES_SBFRAMEWORK_SBSHADER sources/SBFramework/SBShader/*.cpp sources/SBFramework/SBShader/*.h)
file(GLOB SOURCES_SBFRAMEWORK_SBDYNAMICDATA sources/SBFramework/SBDynamicData/*.cpp sources/SBFramework/SBDynamicData/*.h)
file(GLOB SOURCES_SBFRAMEWORK_SBTIMER sources/SBFramework/SBTimer/*.cpp sources/SBFramework/SBTimer/*.h)
file(GLOB SOURCES_SBFRAMEWORK_SBASYNCDATA sources/SBFramework/SBAsyncDataLoad/*.cpp sources/SBFramework/SBAsyncDataLoad/*.h)
file(GLOB SOURCES_SBFRAMEWORK_SBFILESYSTEM sources/SBFramework/SBFileSystem/*.cpp sources/SBFramework/SBFileSystem/*.h)

source_group("" FILES ${SOURCES})
source_group(SBFramework   FILES ${SOURCES_SBFRAMEWORK})
source_group(SBFramework\\SBScene   FILES ${SOURCES_SBFRAMEWORK_SBSCENE})
source_group(SBFramework\\SBShader   FILES ${SOURCES_SBFRAMEWORK_SBSHADER})
source_group(SBFramework\\SBDynamicData   FILES ${SOURCES_SBFRAMEWORK_SBDYNAMICDATA})
source_group(SBFramework\\SBTimer   FILES ${SOURCES_SBFRAMEWORK_SBTIMER})
source_group(SBFramework\\SBAsyncDataLoad   FILES ${SOURCES_SBFRAMEWORK_SBASYNCDATA})
source_group(SBFramework\\SBFileSystem   FILES ${SOURCES_SBFRAMEWORK_SBFILESYSTEM})

add_library(imgui ${SOURCES_IMGUI})
add_library(lz4 ${SOURCES_LZ4})

add_definitions(-DFORCE_PROFILING)
add_executable(ProceduralSky ${SOURCES})

target_link_libraries(ProceduralSky glfw gl3w imgui pugixml lz4 ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES})

set(EXECUTABLE_NAME "ProceduralSky")