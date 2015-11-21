#include <GL/glfw.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#include <iostream>
#include <cstdlib>

#include "Application.h"
#include "SBImGuiBinding.h"
#include "SBEventManager.h"
#include "SBBasicEvents.h"

Appication app;

extern  "C"
{
	void resizeModule(int w, int h)
	{
		glfwSetWindowSize(w, h);
		emscripten_set_canvas_size(w, h);
	}
}

void error_callback(int error, const char* description)
{
	std::cerr << description;
}

static EM_BOOL touchCb(int eventType, const EmscriptenTouchEvent* event, void* userData);

static void MouseButtonCallback(int button, int action)
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

static void MouseMoveCallback(int x, int y)
{
	SB::BasicEvents::OnMouseMoveEvent moveEvent;
	moveEvent.x = x;
	moveEvent.y = y;
	app.OnMouseMove(moveEvent);
}

static void ScrollCallback(int)
{
	//app.MouseWheelTurned(yoffset);
}

static void KeyCallback(int key, int action)
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
	case 'W':
		keyEvent.key = SB::BasicEvents::KEY_W;
		break;
	case 'A':
		keyEvent.key = SB::BasicEvents::KEY_A;
		break;
	case 'S':
		keyEvent.key = SB::BasicEvents::KEY_S;
		break;
	case 'D':
		keyEvent.key = SB::BasicEvents::KEY_D;
		break;
	}
	
	keyEvent.rawKey = key;
	app.OnKeyPressed(keyEvent);
}

void CharCallback(int character, int action)
{
	//ImGuiIO& io = ImGui::GetIO();
	//if (c > 0 && c < 0x10000)
	//	io.AddInputCharacter((unsigned short)c);
}

void Iteration()
{
	EM_ASM(
	{
		UpdateCanvas();
	}
	);

	static double lastFrame = 0;

	SB::ScreenBufferSizes screenBufferSizes;
	glfwGetWindowSize(&screenBufferSizes.m_windowWidth, &screenBufferSizes.m_windowHeight);
	screenBufferSizes.m_framebufferWidth = screenBufferSizes.m_windowWidth;
	screenBufferSizes.m_framebufferHeight = screenBufferSizes.m_windowHeight;

	double currentFrame = glfwGetTime();
	double deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	app.Update(screenBufferSizes, static_cast<float>(deltaTime));
		
	glfwSwapBuffers();
	glfwPollEvents();
}

int main()
{
	std::cout << "Compiled against GLFW " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "." << GLFW_VERSION_REVISION << std::endl;

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW";
		std::exit(EXIT_FAILURE);
	}

	glfwOpenWindowHint(GLFW_DEPTH_BITS, 24);
	glfwOpenWindowHint(GLFW_RED_BITS, 8);
	glfwOpenWindowHint(GLFW_GREEN_BITS, 8);
	glfwOpenWindowHint(GLFW_BLUE_BITS, 8);
	glfwOpenWindowHint(GLFW_ALPHA_BITS, 8);

	float scale = 1;
#ifdef __EMSCRIPTEN__
	float devicePixelRatio = EM_ASM_DOUBLE_V(
	{
		if (window.devicePixelRatio != undefined)
		{
			return window.devicePixelRatio;
		}
		else
		{
			return 1.0;
		}
	}
	);
	scale = devicePixelRatio;
#endif

	int default_width = 1280;
	int default_heght = 768;

	if (scale > 1.0f)
	{
		default_width *= 1.5f;
		default_heght *= 1.5f;
	}
	/*
	default_width = EM_ASM_INT_V(
	{
		Math.max(document.documentElement.clientWidth, window.innerWidth || 0)
	}
	);
	default_heght = EM_ASM_INT_V(
	{
		Math.max(document.documentElement.clientWidth, window.innerWidth || 0)
	}
	);
	*/
	printf("%d %d \n", default_width, default_heght);

	if (!glfwOpenWindow(default_width, default_heght, 8, 8, 8, 0, 24, 0, GLFW_WINDOW))
	{
		glfwTerminate();
		std::cerr << "Failed to create window";
		std::exit(EXIT_FAILURE);
	}

	emscripten_set_touchstart_callback("#canvas", NULL, true, touchCb);
	emscripten_set_touchend_callback("#canvas", NULL, true, touchCb);
	emscripten_set_touchmove_callback("#canvas", NULL, true, touchCb);
	emscripten_set_touchcancel_callback("#canvas", NULL, true, touchCb);

	glfwSetMouseButtonCallback(MouseButtonCallback);
	glfwSetMousePosCallback(MouseMoveCallback);

	glfwSetMouseWheelCallback(ScrollCallback);
	glfwSetKeyCallback(KeyCallback);
	glfwSetCharCallback(CharCallback);

	app.Init();
		
	emscripten_set_main_loop (Iteration, 0, 1);

	return 0;
}




EM_BOOL touchCb(int eventType, const EmscriptenTouchEvent* event, void* userData)
{

	if (event == NULL)
	{
		return false;
	}

	static int lastTouchCount = 0;

	static int id = 0;

	if (event->numTouches == 1)
	{
		const EmscriptenTouchPoint* tp = &event->touches[0];

		switch (eventType)
		{
		case EMSCRIPTEN_EVENT_TOUCHMOVE:
		{
			SB::BasicEvents::OnMouseMoveEvent moveEvent;
			moveEvent.x = tp->canvasX;
			moveEvent.y = tp->canvasY;
			app.OnMouseMove(moveEvent);
			return true;
		}
		case EMSCRIPTEN_EVENT_TOUCHSTART:
			SB::BasicEvents::OnMouseMoveEvent moveEvent;
			moveEvent.x = tp->canvasX;
			moveEvent.y = tp->canvasY;
			app.OnMouseMove(moveEvent);
			SB::BasicEvents::OnMouseButtonEvent buttonEvent;
			buttonEvent.button = SB::BasicEvents::MOUSE_BUTTON_LEFT;
			buttonEvent.action = SB::BasicEvents::ACTION_PRESS;
			app.OnMousePressed(buttonEvent);
			return true;
		case EMSCRIPTEN_EVENT_TOUCHEND:
		case EMSCRIPTEN_EVENT_TOUCHCANCEL:
		{
			SB::BasicEvents::OnMouseMoveEvent moveEvent;
			moveEvent.x = tp->canvasX;
			moveEvent.y = tp->canvasY;
			app.OnMouseMove(moveEvent);
			SB::BasicEvents::OnMouseButtonEvent buttonEvent;
			buttonEvent.button = SB::BasicEvents::MOUSE_BUTTON_LEFT;
			buttonEvent.action = SB::BasicEvents::ACTION_RELEASE;
			app.OnMousePressed(buttonEvent); 
			return true;
		}
		}
	}
	

	return false;
}
