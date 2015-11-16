#include "ProceduralSky.h"
#include "SBShader/SBShader.h"
#include "SBCFile.h"
#include "SBCamera.h"
#include <glm/matrix.hpp>

ProceduralSky::ProceduralSky() : m_shader(new SB::Shader)
{
}

ProceduralSky::~ProceduralSky()
{
	delete m_shader;
}

void ProceduralSky::Init(int horizontalCount, int verticalCount)
{
	SB::CFile fileShaderV("shaders/sky.vs", SB::IFile::FILE_READ);
	SB::CFile fileShaderF("shaders/sky.fs", SB::IFile::FILE_READ);
	m_shader->CreateProgramFrom("sky", &fileShaderV, &fileShaderF);

	std::vector<Vertex> screenSpaceMeshVB;
	std::vector<short> screenSpaceMeshIB;

	for (int i = 0; i < verticalCount; i++)
	{
		for (int j = 0; j < horizontalCount; j++)
		{
			Vertex v;
			v.x = float(j) / (horizontalCount - 1) * 2.0f - 1.0f;
			v.y = float(i) / (verticalCount - 1) * 2.0f - 1.0f;
			screenSpaceMeshVB.push_back(v);
		}
	}

	for (int i = 0; i < verticalCount - 1; i++)
	{
		for (int j = 0; j < horizontalCount - 1; j++)
		{
			screenSpaceMeshIB.push_back(j + 0 + horizontalCount * (i + 0));
			screenSpaceMeshIB.push_back(j + 0 + horizontalCount * (i + 1));
			screenSpaceMeshIB.push_back(j + 1 + horizontalCount * (i + 0));

			screenSpaceMeshIB.push_back(j + 1 + horizontalCount * (i + 0));
			screenSpaceMeshIB.push_back(j + 0 + horizontalCount * (i + 1));
			screenSpaceMeshIB.push_back(j + 1 + horizontalCount * (i + 1));
		}
	}

	m_indicesCount = screenSpaceMeshIB.size();

	glGenBuffers(2, m_vboHandles);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboHandles[0]);
	glBufferData(GL_ARRAY_BUFFER, screenSpaceMeshVB.size() * sizeof(Vertex), screenSpaceMeshVB.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboHandles[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, screenSpaceMeshIB.size() * sizeof(short), screenSpaceMeshIB.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ProceduralSky::Draw(SB::Camera* camera)
{
	m_shader->UseIt();
	
	glm::mat4 cameraTransform = camera->GetCameraMatrix();
	glm::mat4 projectionMatrix = camera->GetProjectionMatrix();
	glm::vec2 projectionParameters(projectionMatrix[0][0], projectionMatrix[1][1]);

	m_shader->GetUniform("u_skyDirection").SetValue(m_skyDirection);
	m_shader->GetUniform("u_celestialBodyDirection").SetValue(m_sunDirection);
	m_shader->GetUniform("u_skyLuminanceXYZ").SetValue(m_skyLuminanceXYZ);
	m_shader->GetUniform("u_Exposition").SetValue(m_exposition);
	m_shader->GetUniform("u_cameraTransform").SetValue(cameraTransform);
	m_shader->GetUniform("u_projectionParameters").SetValue(projectionParameters);
	
	GLfloat depthRange[2];
	glGetFloatv(GL_DEPTH_RANGE, depthRange);
	GLboolean depthWriteMask;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWriteMask);
	GLint depthFunc;
	glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
	
	glDepthRange(1.0, 1.0);
	glDepthMask(false);
	glDepthFunc(GL_LEQUAL);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vboHandles[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboHandles[1]);

	SB::Shader::AttributeGuard enablePosition(m_shader->positionAttribute);
	glVertexAttribPointer(m_shader->positionAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_SHORT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	glDepthRange(depthRange[0], depthRange[1]);
	glDepthMask(depthWriteMask);
	glDepthFunc(depthFunc);
}

void ProceduralSky::SetSkyLuminanceXYZ(const glm::vec3& skyLuminanceXYZ)
{
	m_skyLuminanceXYZ = skyLuminanceXYZ;
}

void ProceduralSky::SetSkyDirection(const glm::vec3& skyDirection)
{
	m_skyDirection = skyDirection;
}

void ProceduralSky::SetSunDirection(const glm::vec3& sunDirection)
{
	m_sunDirection = sunDirection;
}

void ProceduralSky::SetExposition(float exposition)
{
	m_exposition = exposition;
}