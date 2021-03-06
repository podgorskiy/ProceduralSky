#include "SBCameraFreeFlightController.h"
#include "SBCamera.h" 
#include <glm/gtx/rotate_vector.hpp>

using namespace SB;

CameraFreeFlightController::CameraFreeFlightController() :
	m_camera(NULL), 
	m_moveForward(false), 
	m_moveBackward(false), 
	m_moveLeft(false), 
	m_moveRight(false),
	m_speed(1.0f),
	m_drag(false)
{}

void CameraFreeFlightController::EventReceiver_OnEvent(const BasicEvents::OnMouseButtonEvent& mouseButtonEvent)
{
	if (mouseButtonEvent.action == BasicEvents::ACTION_PRESS)
	{
		if (mouseButtonEvent.button == BasicEvents::MOUSE_BUTTON_LEFT)
		{
			m_drag = true;
		}
	}
	else if (mouseButtonEvent.action == BasicEvents::ACTION_RELEASE)
	{
		if (mouseButtonEvent.button == BasicEvents::MOUSE_BUTTON_LEFT)
		{
			m_drag = false;
		}
	}
}

void CameraFreeFlightController::EventReceiver_OnEvent(const BasicEvents::OnKeyEvent& keyEvent)
{
	if (keyEvent.action != BasicEvents::ACTION_PRESS && keyEvent.action != BasicEvents::ACTION_RELEASE)
	{
		return;
	}
	switch (keyEvent.key)
	{
	case BasicEvents::KEY_W:
		m_moveForward = keyEvent.action == BasicEvents::ACTION_PRESS;
		break;
	case BasicEvents::KEY_A:
		m_moveLeft = keyEvent.action == BasicEvents::ACTION_PRESS;
		break;
	case BasicEvents::KEY_S:
		m_moveBackward = keyEvent.action == BasicEvents::ACTION_PRESS;
		break;
	case BasicEvents::KEY_D:
		m_moveRight = keyEvent.action == BasicEvents::ACTION_PRESS;
		break;
	}
}

void CameraFreeFlightController::Update(float deltaTime, bool blockDrag)
{
	m_drag = blockDrag ? false : m_drag;
	if (m_camera != NULL)
	{
		glm::vec3 position = m_camera->GetPosition();
		glm::vec3 direction = m_camera->GetLookDirection();
		glm::vec3 xVector = glm::normalize(glm::cross(m_camera->GetUpVector(), direction));
		if (m_moveForward)
		{
			position += direction * m_speed;
		}
		if (m_moveBackward)
		{
			position -= direction * m_speed;
		}
		if (m_moveLeft)
		{
			position += xVector * m_speed;
		}
		if (m_moveRight)
		{
			position -= xVector * m_speed;
		}
		m_camera->SetPosition(position);
	}
}

void CameraFreeFlightController::EventReceiver_OnEvent(const BasicEvents::OnMouseMoveEvent& mouseButtonEvent)
{
	glm::vec2 newPosition(mouseButtonEvent.x, mouseButtonEvent.y);
	glm::vec2 delta = newPosition - m_lastPosition;

	if (m_drag && m_camera != NULL)
	{
		glm::vec3 direction = m_camera->GetLookDirection();
		glm::vec3 upvector = m_camera->GetUpVector();
		direction = glm::rotate(direction, -delta.x / 500.0f, upvector);

		glm::vec3 xVector = glm::cross(upvector, direction);
		glm::vec3 directionNew = glm::rotate(direction, delta.y / 500.0f, glm::cross(upvector, direction));

		glm::vec3 xVectorNew = glm::cross(upvector, directionNew);
		if (glm::dot(xVector, xVectorNew) > 0.0f)
		{
			direction = directionNew;
		}

		m_camera->SetLookDirection(direction);
	}

	m_lastPosition = newPosition;
}

void CameraFreeFlightController::AttachCamera(SB::Camera* camera)
{
	m_camera = camera;
};

void CameraFreeFlightController::SetSpeed(float speed)
{
	m_speed = speed;
}