#pragma once
#include <glm/matrix.hpp>

namespace SB
{
	class Camera
	{
	public:
		Camera();

		void SetFOV(float fov);

		void SetRatio(float ratio);

		void SetNearFarPlanes(float nearPlane, float farPlane);

		void SetUpVector(const glm::vec3 up);

		void SetPosition(const glm::vec3 position);

		void SetLookDirection(const glm::vec3 direction);

		void SetLookAtPoint(const glm::vec3 lookAt);

		void SetLockLookAtPosition(bool enabled);

		const glm::mat4& GetViewMatrix() const;

		const glm::mat4& GetCameraMatrix() const;

		const glm::mat4& GetProjectionMatrix() const;

		glm::vec3 GetLookDirection() const;

		glm::vec3 GetUpVector() const;

		glm::vec3 GetPosition() const;

		void Update();

	private:
		void ComputeProjectionMatrix();
		void ComputeViewMatrix();

		float m_ratio;
		float m_fov;
		float m_near;
		float m_far;

		glm::vec3 m_upVector;
		glm::vec3 m_zVector;
		glm::vec3 m_position;
		glm::vec3 m_lookAt;

		glm::mat4 m_projection;
		glm::mat4 m_camera;
		glm::mat4 m_view;

		bool m_lockLookAtPosition;
	};
}