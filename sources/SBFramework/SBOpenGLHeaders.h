#pragma once
#ifdef __EMSCRIPTEN__
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>

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