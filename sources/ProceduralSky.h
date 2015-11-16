#pragma once
#include <vector>
#include <glm/matrix.hpp>

namespace SB
{
	class Shader;
	class Camera;
}

class ProceduralSky
{
	struct Vertex
	{
		float x, y;
	};

public:
	ProceduralSky();
	~ProceduralSky();

	void Init(int horizontalCount, int vericalCount);
	void Draw(SB::Camera* camera);

	void SetSkyLuminanceXYZ(const glm::vec3& skyLuminanceXYZ);
	void SetSkyDirection(const glm::vec3& skyDirection);
	void SetSunDirection(const glm::vec3& sunDirection);
	void SetExposition(float exposition);
private:
	glm::vec3 m_skyLuminanceXYZ;
	glm::vec3 m_skyDirection;
	glm::vec3 m_sunDirection;
	float m_exposition;

	unsigned int m_vboHandles[2];
	int m_indicesCount;
	SB::Shader* m_shader;
};