#pragma once

#ifdef __EMSCRIPTEN__
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>
#include <emscripten/emscripten.h>

inline int gl3wInit()
{
	return 0;
}

inline bool gl3wIsSupported(int, int)
{
	return true;
}

#else
#include <GL/gl3w.h>
#endif

#ifndef COMPRESSED_RGB_S3TC_DXT1_EXT
#define COMPRESSED_RGB_S3TC_DXT1_EXT                      0x83F0
#endif

#ifndef COMPRESSED_RGBA_S3TC_DXT1_EXT
#define COMPRESSED_RGBA_S3TC_DXT1_EXT                     0x83F1
#endif