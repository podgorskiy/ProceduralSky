#include <GLFW/glfw3.h>
#include <iostream>

void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int main()
{
	glfwSetErrorCallback(error_callback);

	printf("Compiled against GLFW %i.%i.%i\n",
       GLFW_VERSION_MAJOR,
       GLFW_VERSION_MINOR,
       GLFW_VERSION_REVISION);

	if (!glfwInit())
	{
		printf("Failed to initialize GLFW");
		exit(EXIT_FAILURE);
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "ProceduralSky", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		printf("Failed to create window");
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
        
		glClear(GL_COLOR_BUFFER_BIT);
		
        glfwSwapBuffers(window);
        glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}