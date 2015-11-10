#include <GL/gl3w.h>
#include <iostream>

#include "Appication.h"

int Appication::Init()
{
	if (gl3wInit()) 
	{
		std::cerr << "failed to initialize OpenGL";
		return EXIT_FAILURE;
	}
	
	if (!gl3wIsSupported(3, 2)) {
		std::cerr << "OpenGL 3.2 not supported";
		return EXIT_FAILURE;
	}

	std::cout	<< "OpenGL " << glGetString(GL_VERSION) 
				<< " GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return EXIT_SUCCESS;
}

void Appication::Update(int width, int height)
{
	glViewport(0, 0, width, height);		
	glClear(GL_COLOR_BUFFER_BIT);
}