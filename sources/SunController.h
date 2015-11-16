#pragma once
#include <glm/matrix.hpp>

class SunController
{
public:
	enum Month
	{
		January = 0,
		February,
		March,
		April,
		May,
		June,
		July,
		August,
		September,
		October,
		November,
		December
	};

	SunController();
				
	void Update(float time);

	void SetMonth(Month month);
	
	void SetLatitude(float latitude);

	void SetNorthDirection(glm::vec3 direction);

	void SetUpVector(glm::vec3 up);

	Month GetMonth();

	float GetLatitude();

	const glm::vec3& GetSunDirection();

private:
	void CalculateSunOrbit();
	void UpdateSunPosition(float hour);

	glm::vec3 m_northDirection;
	glm::vec3 m_sunDirection;
	glm::vec3 m_upvector;
	float m_latitude;
	float m_eclipticObliquity;
	float m_delta;
	Month m_month;
};
