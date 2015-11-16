#include "SBCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace SB;

Camera::Camera() : m_lockLookAtPosition(false)
{};

void Camera::SetFOV(float fov)
{
	m_fov = fov;
};

void Camera::SetRatio(float ratio)
{
	m_ratio = ratio;
};

void Camera::SetNearFarPlanes(float near, float far)
{
	m_near = near;
	m_far = far;
};

void Camera::SetUpVector(const glm::vec3 up)
{
	m_upVector = up;
};

void Camera::SetPosition(const glm::vec3 position)
{
	m_position = position;
};

void Camera::SetLookDirection(const glm::vec3 direction)
{
	m_zVector = -glm::normalize(direction);
	m_lookAt = m_position + direction;
};

void Camera::SetLookAtPoint(const glm::vec3 lookAt)
{
	m_lookAt = lookAt;
	m_zVector = -glm::normalize(m_lookAt - m_position);
};

void Camera::SetLockLookAtPosition(bool enabled)
{
	m_lockLookAtPosition = enabled;
};

void Camera::ComputeProjectionMatrix()
{
	m_projection = glm::perspective(m_fov, m_ratio, m_near, m_far);
};

void Camera::ComputeViewMatrix()
{
	if (m_lockLookAtPosition)
	{
		m_zVector = -glm::normalize(m_lookAt - m_position);
	}
	glm::vec3& vz = m_zVector;
	glm::vec3 vx = glm::normalize(glm::cross(m_upVector, vz));
	glm::vec3 vy = glm::cross(vz, vx);
	glm::mat4 cameraRotation = glm::mat4(glm::vec4(vx, 0), glm::vec4(vy, 0), glm::vec4(vz, 0), glm::vec4(glm::vec3(0.0), 1.0f));
	glm::mat4 viewRotation = glm::transpose(cameraRotation);
	
	glm::mat4 viewTranslation = glm::translate(glm::mat4(1.0), -m_position);
	glm::mat4 cameraTranslation = glm::translate(glm::mat4(1.0), m_position);

	m_view = viewRotation * viewTranslation;
	m_camera = cameraTranslation * cameraRotation;
}

glm::vec3 Camera::GetLookDirection() const
{
	return -m_zVector;
}

glm::vec3 Camera::GetUpVector() const
{
	return m_upVector;
}

glm::vec3 Camera::GetPosition() const
{
	return m_position;
}

const glm::mat4& Camera::GetViewMatrix() const
{
	return m_view;
}

const glm::mat4& Camera::GetCameraMatrix() const
{
	return m_camera;
}

const glm::mat4& Camera::GetProjectionMatrix() const
{
	return m_projection;
}

void Camera::Update()
{
	ComputeProjectionMatrix();
	ComputeViewMatrix();
}