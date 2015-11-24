#include "PostEffectRenderPlane.h"
#include "SBShader/SBShader.h"
#include "SBFileSystem/SBCFile.h"
#include "SBCamera.h"
#include <glm/matrix.hpp>

PostEffectRenderPlane::PostEffectRenderPlane() : m_shader(NULL)
{
}

PostEffectRenderPlane::~PostEffectRenderPlane()
{
}

void PostEffectRenderPlane::Init(SB::Shader* shader)
{
	m_shader = shader;
	std::vector<Vertex> screenSpaceMeshVB;
	std::vector<short> screenSpaceMeshIB;

	int verticalCount = 2;
	int horizontalCount = 2;
	
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
			screenSpaceMeshIB.push_back(j + 1 + horizontalCount * (i + 0));
			screenSpaceMeshIB.push_back(j + 0 + horizontalCount * (i + 1));

			screenSpaceMeshIB.push_back(j + 1 + horizontalCount * (i + 0));
			screenSpaceMeshIB.push_back(j + 1 + horizontalCount * (i + 1));
			screenSpaceMeshIB.push_back(j + 0 + horizontalCount * (i + 1));
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

void PostEffectRenderPlane::Draw()
{
	m_shader->UseIt();
	m_shader->GetUniform("u_texture").SetValue(0);
	GLboolean depthWriteMask;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWriteMask);
	
	glDepthMask(false);
	glDisable(GL_DEPTH_TEST);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vboHandles[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboHandles[1]);

	SB::Shader::AttributeGuard enablePosition(m_shader->positionAttribute);
	glVertexAttribPointer(m_shader->positionAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_SHORT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDepthMask(depthWriteMask);
}
