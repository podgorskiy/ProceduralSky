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

#ifndef COMPRESSED_RGBA_S3TC_DXT3_EXT
#define COMPRESSED_RGBA_S3TC_DXT3_EXT                     0x83F2
#endif

#ifndef COMPRESSED_RGBA_S3TC_DXT5_EXT
#define COMPRESSED_RGBA_S3TC_DXT5_EXT                     0x83F3
#endif

#ifndef COMPRESSED_RGB_PVRTC_4BPPV1_IMG
#define COMPRESSED_RGB_PVRTC_4BPPV1_IMG                   0x8C00
#endif

#ifndef COMPRESSED_RGB_PVRTC_2BPPV1_IMG
#define COMPRESSED_RGB_PVRTC_2BPPV1_IMG                   0x8C01
#endif

#ifndef COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
#define COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                  0x8C02
#endif

#ifndef COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
#define COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                  0x8C03
#endif
