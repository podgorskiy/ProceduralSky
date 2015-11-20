#include <GL/glfw.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#include <iostream>
#include <cstdlib>

#include "Appication.h"
#include "SBEventManager.h"
#include "Events.h"

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
	Event::ButtonType buttonType = Event::MOUSE_BUTTON_NONE;
	Event::ActionType actionType = Event::ACTION_NONE;
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		buttonType = Event::MOUSE_BUTTON_LEFT;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		buttonType = Event::MOUSE_BUTTON_RIGHT;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		buttonType = Event::MOUSE_BUTTON_MIDDLE;
		break;
	}
	switch (action)
	{
	case GLFW_PRESS:
		actionType = Event::ACTION_PRESS;
		break;
	case GLFW_RELEASE:
		actionType = Event::ACTION_RELEASE;
		break;
	}
	Event::OnMouseButtonEvent buttonEvent;
	buttonEvent.button = buttonType;
	buttonEvent.action = actionType;
	app.OnMousePressed(buttonEvent);
}

static void MouseMoveCallback(int x, int y)
{
	Event::OnMouseMoveEvent moveEvent;
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
	Event::OnKeyEvent keyEvent;
	switch (action)
	{
	case GLFW_PRESS:
		keyEvent.action = Event::ACTION_PRESS;
		break;
	case GLFW_RELEASE:
		keyEvent.action = Event::ACTION_RELEASE;
		break;
	}	
	
	switch (key)
	{
	case 'W':
		keyEvent.key = Event::KEY_W;
		break;
	case 'A':
		keyEvent.key = Event::KEY_A;
		break;
	case 'S':
		keyEvent.key = Event::KEY_S;
		break;
	case 'D':
		keyEvent.key = Event::KEY_D;
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

	ScreenBufferSizes ScreenBufferSizes;
	glfwGetWindowSize(&ScreenBufferSizes.m_windowWidth, &ScreenBufferSizes.m_windowHeight);
	ScreenBufferSizes.m_framebufferWidth = ScreenBufferSizes.m_windowWidth;
	ScreenBufferSizes.m_framebufferHeight = ScreenBufferSizes.m_windowHeight;

	double currentFrame = glfwGetTime();
	double deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	app.Update(ScreenBufferSizes, static_cast<float>(deltaTime));
		
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
			Event::OnMouseMoveEvent moveEvent;
			moveEvent.x = tp->canvasX;
			moveEvent.y = tp->canvasY;
			app.OnMouseMove(moveEvent);
			return true;
		}
		case EMSCRIPTEN_EVENT_TOUCHSTART:
			Event::OnMouseMoveEvent moveEvent;
			moveEvent.x = tp->canvasX;
			moveEvent.y = tp->canvasY;
			app.OnMouseMove(moveEvent);
			Event::OnMouseButtonEvent buttonEvent;
			buttonEvent.button = Event::MOUSE_BUTTON_LEFT;
			buttonEvent.action = Event::ACTION_PRESS;
			app.OnMousePressed(buttonEvent);
			return true;
		case EMSCRIPTEN_EVENT_TOUCHEND:
		case EMSCRIPTEN_EVENT_TOUCHCANCEL:
		{
			Event::OnMouseMoveEvent moveEvent;
			moveEvent.x = tp->canvasX;
			moveEvent.y = tp->canvasY;
			app.OnMouseMove(moveEvent);
			Event::OnMouseButtonEvent buttonEvent;
			buttonEvent.button = Event::MOUSE_BUTTON_LEFT;
			buttonEvent.action = Event::ACTION_RELEASE;
			app.OnMousePressed(buttonEvent); 
			return true;
		}
		}
	}
	

	return false;
}
