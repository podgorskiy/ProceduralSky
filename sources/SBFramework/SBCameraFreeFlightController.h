#pragma once
#include "SBEventManager.h"
#include "SBBasicEvents.h"
#include <glm/matrix.hpp>

namespace SB
{
	class Camera;

	class CameraFreeFlightController :
		public SB::EventReceiver<BasicEvents::OnMouseButtonEvent>,
		public SB::EventReceiver<BasicEvents::OnMouseMoveEvent>,
		public SB::EventReceiver<BasicEvents::OnKeyEvent>
	{
	public:
		CameraFreeFlightController();

		void EventReceiver_OnEvent(const BasicEvents::OnMouseButtonEvent& mouseButtonEvent);
		void EventReceiver_OnEvent(const BasicEvents::OnMouseMoveEvent& mouseButtonEvent);
		void EventReceiver_OnEvent(const BasicEvents::OnKeyEvent& keyEvent);

		void Update(float deltaTime, bool blockDrag);

		void AttachCamera(SB::Camera* camera);

		void SetSpeed(float speed);

	private:
		bool m_moveForward;
		bool m_moveBackward;
		bool m_moveLeft;
		bool m_moveRight;
		bool m_drag;

		float m_speed;

		glm::vec2 m_lastPosition;
		SB::Camera* m_camera;
	};
}