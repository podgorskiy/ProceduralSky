#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>

#include "Appication.h"

void error_callback(int error, const char* description)
{
	std::cerr << description;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

int main()
{
	glfwSetErrorCallback(error_callback);

	std::cout << "Compiled against GLFW " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "." << GLFW_VERSION_REVISION << std::endl;

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW";
		std::exit(EXIT_FAILURE);
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "ProceduralSky", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		std::cerr << "Failed to create window";
		std::exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	Appication app;

	if (app.Init() != EXIT_SUCCESS)
	{
		std::exit(EXIT_FAILURE);
	}

	while (!glfwWindowShouldClose(window))
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		
		app.Update(width, height);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}