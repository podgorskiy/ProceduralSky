#define _USE_MATH_DEFINES

#include "SunController.h"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

SunController::SunController() :
	m_latitude(50.0f), 
	m_northDirection(1.0f, 0.0f, 0.0f), 
	m_sunDirection(1.0f, 1.0f, 1.0f),
	m_eclipticObliquity(static_cast<float>(23.4 * M_PI / 180.0)),
	m_delta(0.0f)
{
}

void SunController::CalculateSunOrbit()
{
	float day = 30.0f * static_cast<int>(m_month)+15.0f;
	float lambda = 280.46f + 0.9856474f * day;

	lambda *= static_cast<float>(M_PI) / 180.0f;

	/*
	float alpha = std::atan2(
	std::sin(lambda)*std::cos(eclipticObliquity),
	std::cos(lambda)
	);
	*/

	m_delta = std::asin(std::sin(m_eclipticObliquity) * std::sin(lambda));
}

void SunController::UpdateSunPosition(float hour)
{
	float latitude_ = m_latitude * static_cast<float>(M_PI) / 180.0f;
	float h = hour * static_cast<float>(M_PI) / 12.0f;
	float azimuth = std::atan2(
		std::sin(h),
		std::cos(h) * std::sin(latitude_) - std::tan(m_delta) * std::cos(latitude_)
		);

	float altitude = std::asin(
		std::sin(latitude_) * std::sin(m_delta) + std::cos(latitude_) * std::cos(m_delta) * std::cos(h)
		);

	glm::vec3 direction = glm::rotate(m_northDirection, -azimuth, m_upvector);
	glm::vec3 v = glm::cross(direction, m_upvector);
	m_sunDirection = glm::rotate(direction, altitude, v);
}

void SunController::Update(float time)
{
	CalculateSunOrbit();
	UpdateSunPosition(time - 12.0f);
}

void SunController::SetMonth(Month month)
{
	m_month = month;
}

void SunController::SetLatitude(float latitude)
{
	m_latitude = latitude;
}

void SunController::SetNorthDirection(glm::vec3 direction)
{
	m_northDirection = direction;
}

void SunController::SetUpVector(glm::vec3 up)
{
	m_upvector = up;
}

SunController::Month SunController::GetMonth()
{
	return m_month;
}

float SunController::GetLatitude()
{
	return m_latitude;
}

glm::vec3 SunController::GetSunDirection()
{
	return glm::normalize(m_sunDirection);
}