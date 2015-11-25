#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#include <iostream>
#include <cstdlib>

#include "Application.h"
#include "SBImGuiBinding.h"
#include "SBEventManager.h"
#include "SBBasicEvents.h"

Appication app;
SB::ScreenBufferSizes screenBufferSizes;

extern  "C"
{
	void resizeModule(int w, int h)
	{
		emscripten_set_canvas_size(w, h);
	}
}

void error_callback(int error, const char* description)
{
	std::cerr << description;
}

static EM_BOOL touchCb(int eventType, const EmscriptenTouchEvent* event, void* userData);
static EM_BOOL mouseCb(int eventType, const EmscriptenMouseEvent* event, void* userData);
static EM_BOOL keyCb(int eventType, const EmscriptenKeyboardEvent* event, void* userData);


void Iteration()
{
	EM_ASM(
	{
		UpdateCanvas();
	}
	);

	static double lastFrame = 0;
	int isFullscreen;
	emscripten_get_canvas_size(&screenBufferSizes.m_windowWidth, &screenBufferSizes.m_windowHeight, &isFullscreen);

	screenBufferSizes.m_framebufferWidth = screenBufferSizes.m_windowWidth;
	screenBufferSizes.m_framebufferHeight = screenBufferSizes.m_windowHeight;

	double currentFrame = emscripten_get_now() / 1000.0;
	double deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	app.Update(screenBufferSizes, static_cast<float>(deltaTime));
}

int main()
{
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

	
	emscripten_set_touchstart_callback("#canvas", NULL, true, touchCb);
	emscripten_set_touchend_callback("#canvas", NULL, true, touchCb);
	emscripten_set_touchmove_callback("#canvas", NULL, true, touchCb);
	emscripten_set_touchcancel_callback("#canvas", NULL, true, touchCb);

	emscripten_set_mousedown_callback("#canvas", NULL, true, mouseCb);
	emscripten_set_mouseup_callback("#canvas", NULL, true, mouseCb);
	emscripten_set_mousemove_callback("#canvas", NULL, true, mouseCb);

	emscripten_set_keypress_callback(NULL, NULL, true, keyCb);
	emscripten_set_keydown_callback(NULL, NULL, true, keyCb);
	emscripten_set_keyup_callback(NULL, NULL, true, keyCb);

	//emscripten_set_wheel_callback("#canvas", this, true, wheelCb);
	
	EmscriptenFullscreenStrategy fullscreenStrategy = {};
	fullscreenStrategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_DEFAULT;
	fullscreenStrategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_NONE;
	fullscreenStrategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;

	emscripten_request_fullscreen_strategy("#canvas", false, &fullscreenStrategy);

	EmscriptenWebGLContextAttributes contextAttrubutes;
	emscripten_webgl_init_context_attributes(&contextAttrubutes);
	contextAttrubutes.alpha = false;
	contextAttrubutes.depth = true;
	contextAttrubutes.stencil = false;
	contextAttrubutes.antialias = false;
	contextAttrubutes.premultipliedAlpha = false;
	contextAttrubutes.preserveDrawingBuffer = false;
	contextAttrubutes.preferLowPowerToHighPerformance = false;
	contextAttrubutes.failIfMajorPerformanceCaveat = false;
	contextAttrubutes.enableExtensionsByDefault = true;


	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE handle = emscripten_webgl_create_context("#canvas", &contextAttrubutes);
	emscripten_webgl_make_context_current(handle);

	int isFullscreen;

	emscripten_get_canvas_size(&screenBufferSizes.m_windowWidth, &screenBufferSizes.m_windowHeight, &isFullscreen);

	app.Init();
		
	emscripten_set_main_loop (Iteration, 0, 1);

	return 0;
}

EM_BOOL keyCb(int eventType, const EmscriptenKeyboardEvent *event, void *userData)
{
	if (event == NULL)
	{
		return false;
	}

	SB::BasicEvents::OnKeyEvent keyEvent;
	switch (eventType)
	{
	case EMSCRIPTEN_EVENT_KEYPRESS:
	case EMSCRIPTEN_EVENT_KEYDOWN:
		keyEvent.action = SB::BasicEvents::ACTION_PRESS;
		break;
	case EMSCRIPTEN_EVENT_KEYUP:
		keyEvent.action = SB::BasicEvents::ACTION_RELEASE;
		break;
	}

	switch (event->keyCode)
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

	keyEvent.rawKey = event->keyCode;
	app.OnKeyPressed(keyEvent);

	return true;
}

EM_BOOL mouseCb(int eventType, const EmscriptenMouseEvent* event, void* userData)
{
	if (event == NULL)
	{
		return false;
	}

	switch (eventType)
	{
	case EMSCRIPTEN_EVENT_MOUSEMOVE:
	{
		SB::BasicEvents::OnMouseMoveEvent moveEvent;
		moveEvent.x = event->canvasX;
		moveEvent.y = event->canvasY;
		app.OnMouseMove(moveEvent);
		return true;
	}
	case EMSCRIPTEN_EVENT_MOUSEDOWN:
		SB::BasicEvents::OnMouseMoveEvent moveEvent;
		moveEvent.x = event->canvasX;
		moveEvent.y = event->canvasY;
		app.OnMouseMove(moveEvent);
		SB::BasicEvents::OnMouseButtonEvent buttonEvent;
		buttonEvent.button = SB::BasicEvents::MOUSE_BUTTON_LEFT;
		buttonEvent.action = SB::BasicEvents::ACTION_PRESS;
		app.OnMousePressed(buttonEvent);
		return true;
	case EMSCRIPTEN_EVENT_MOUSEUP:
	case EMSCRIPTEN_EVENT_DBLCLICK:
	{
		SB::BasicEvents::OnMouseMoveEvent moveEvent;
		moveEvent.x = event->canvasX;
		moveEvent.y = event->canvasY;
		app.OnMouseMove(moveEvent);
		SB::BasicEvents::OnMouseButtonEvent buttonEvent;
		buttonEvent.button = SB::BasicEvents::MOUSE_BUTTON_LEFT;
		buttonEvent.action = SB::BasicEvents::ACTION_RELEASE;
		app.OnMousePressed(buttonEvent);
		return true;
	}
	}
	

	return false;
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
