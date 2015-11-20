#ifdef __EMSCRIPTEN__
#include "main_emscripten.inl"
#else
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>

#include "Appication.h"
#include "SBEventManager.h"
#include "SBBasicEvents.h"
#include "SBImGuiBinding.h"

#ifdef WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

Appication app;

std::string ExePath() 
{
	char buffer[FILENAME_MAX];
	GetCurrentDir(buffer, FILENAME_MAX);
	return buffer;
}

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

static void MouseButtonCallback(GLFWwindow*, int button, int action, int /*mods*/)
{
	SB::BasicEvents::ButtonType buttonType = SB::BasicEvents::MOUSE_BUTTON_NONE;
	SB::BasicEvents::ActionType actionType = SB::BasicEvents::ACTION_NONE;
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		buttonType = SB::BasicEvents::MOUSE_BUTTON_LEFT;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		buttonType = SB::BasicEvents::MOUSE_BUTTON_RIGHT;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		buttonType = SB::BasicEvents::MOUSE_BUTTON_MIDDLE;
		break;
	}
	switch (action)
	{
	case GLFW_PRESS:
		actionType = SB::BasicEvents::ACTION_PRESS;
		break;
	case GLFW_RELEASE:
		actionType = SB::BasicEvents::ACTION_RELEASE;
		break;
	}
	SB::BasicEvents::OnMouseButtonEvent buttonEvent;
	buttonEvent.button = buttonType;
	buttonEvent.action = actionType;
	app.OnMousePressed(buttonEvent);
}

static void MouseMoveCallback(GLFWwindow*, double x, double y)
{
	SB::BasicEvents::OnMouseMoveEvent moveEvent;
	moveEvent.x = x;
	moveEvent.y = y;
	app.OnMouseMove(moveEvent);
}

static void ScrollCallback(GLFWwindow*, double /*xoffset*/, double yoffset)
{
	//app.MouseWheelTurned(yoffset);
}

static void KeyCallback(GLFWwindow*, int key, int, int action, int /*mods*/)
{
	SB::BasicEvents::OnKeyEvent keyEvent;
	switch (action)
	{
	case GLFW_PRESS:
		keyEvent.action = SB::BasicEvents::ACTION_PRESS;
		break;
	case GLFW_RELEASE:
		keyEvent.action = SB::BasicEvents::ACTION_RELEASE;
		break;
	}	
	switch (key)
	{
	case GLFW_KEY_W:
		keyEvent.key = SB::BasicEvents::KEY_W;
		break;
	case GLFW_KEY_A:
		keyEvent.key = SB::BasicEvents::KEY_A;
		break;
	case GLFW_KEY_S:
		keyEvent.key = SB::BasicEvents::KEY_S;
		break;
	case GLFW_KEY_D:
		keyEvent.key = SB::BasicEvents::KEY_D;
		break;
	}
	keyEvent.rawKey = key;
	app.OnKeyPressed(keyEvent);
}

void CharCallback(GLFWwindow*, unsigned int c)
{
	//ImGuiIO& io = ImGui::GetIO();
	//if (c > 0 && c < 0x10000)
	//	io.AddInputCharacter((unsigned short)c);
}

int main(int argc, char **argv)
{	
	for (int i = 0; i < argc; ++i)
	{
		std::cout << argv[i] << std::endl;
	}
	
	std::cout << "Working directory: " << ExePath() << std::endl;

	glfwSetErrorCallback(error_callback);

	std::cout << "Compiled against GLFW " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "." << GLFW_VERSION_REVISION << std::endl;

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW";
		std::exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);

	GLFWwindow* window = glfwCreateWindow(1280, 768, "ProceduralSky", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		std::cerr << "Failed to create window";
		std::exit(EXIT_FAILURE);
	}

	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, MouseMoveCallback);

	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCharCallback(window, CharCallback);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	
	if (app.Init() != EXIT_SUCCESS)
	{
		std::exit(EXIT_FAILURE);
	}

	double lastFrame = 0;

	while (!glfwWindowShouldClose(window))
	{
		SB::ScreenBufferSizes ScreenBufferSizes;

		glfwGetFramebufferSize(window, &ScreenBufferSizes.m_framebufferWidth, &ScreenBufferSizes.m_framebufferHeight);
		glfwGetWindowSize(window, &ScreenBufferSizes.m_windowWidth, &ScreenBufferSizes.m_windowHeight);

		double currentFrame = glfwGetTime();
		double deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		app.Update(ScreenBufferSizes, static_cast<float>(deltaTime));
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

#endif