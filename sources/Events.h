#pragma once

namespace Event
{
	enum ButtonType
	{
		MOUSE_BUTTON_NONE = -1,
		MOUSE_BUTTON_LEFT = 0,
		MOUSE_BUTTON_RIGHT = 1,
		MOUSE_BUTTON_MIDDLE = 2,

		KEY_W,
		KEY_A,
		KEY_S,
		KEY_D
	};
	enum ActionType
	{
		ACTION_NONE = -1,
		ACTION_PRESS = 0,
		ACTION_RELEASE = 1
	};
	struct OnMouseButtonEvent
	{
		ButtonType button;
		ActionType action;
	};

	struct OnMouseMoveEvent
	{
		float x;
		float y;
	};
	struct OnKeyEvent
	{
		ButtonType key;
		int rawKey;
		ActionType action;
	};
}