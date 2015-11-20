#include "SBMesh.h"
#include "SBShader/SBShader.h"
#include "SBCommon.h"

using namespace SB;

Mesh::Mesh() :
m_stride(0),
m_voffset(-1),
m_noffset(-1),
m_coffset(-1),
m_toffset1(-1),
m_toffset2(-1),
m_rawIntIndices(NULL),
m_rawShortIndices(NULL),
m_rawbuffer(NULL),
m_verticesMaps(NULL)
{
};

Mesh::~Mesh()
{
	SafeDelete(m_rawIntIndices);
	SafeDelete(m_rawShortIndices);
	SafeDelete(m_rawbuffer);
	SafeDelete(m_verticesMaps);
}

void Mesh::CreateVBO()
{
	m_VBO.CreateBuffers();
	if (m_rawIntIndices != NULL)
	{
		for (int i = 0; i < m_indeciesCount; ++i)
		{
			if (m_rawIntIndices[i] < 0 || m_rawIntIndices[i] >= m_verticesCount)
			{
				LOGE("m_rawIntIndices[i] < 0 || m_rawIntIndices[i] >= m_verticesCount");
				assert(false);
			} 
		}
		m_VBO.FillBuffers(m_rawbuffer, m_verticesCount, m_stride, m_rawIntIndices, m_indeciesCount, sizeof(*m_rawIntIndices));
		delete m_rawIntIndices;
		m_rawIntIndices = NULL;
	}
	else if (m_rawShortIndices != NULL)
	{
		for (int i = 0; i < m_indeciesCount; ++i)
		{
			if (m_rawShortIndices[i] < 0 || m_rawShortIndices[i] >= m_verticesCount)
			{
				LOGE("m_rawIntIndices[i] < 0 || m_rawIntIndices[i] >= m_verticesCount");
				assert(false);
			}
		}
		m_VBO.FillBuffers(m_rawbuffer, m_verticesCount, m_stride, m_rawShortIndices, m_indeciesCount, sizeof(*m_rawShortIndices));
		delete m_rawShortIndices;
		m_rawShortIndices = NULL;
	}
	delete m_rawbuffer;
	m_rawbuffer = NULL;
}

void Mesh::Draw(Shader& shader)
{
	m_VBO.BindVBO();

	shader.UseIt();
	
	if (shader.positionAttribute.Valid() && m_voffset != -1)
	{
		glEnableVertexAttribArray((int)shader.positionAttribute);
		glVertexAttribPointer((int)shader.positionAttribute, 3, GL_FLOAT, GL_FALSE, m_stride, (void*)(m_voffset));
	}
	if (shader.normalAttribute.Valid() && m_noffset != -1)
	{
		glEnableVertexAttribArray((int)shader.normalAttribute);
		glVertexAttribPointer((int)shader.normalAttribute, 3, GL_BYTE, GL_TRUE, m_stride, (void*)(m_noffset));
	}
	if (shader.colorAttribute.Valid() && m_coffset != -1)
	{
		glEnableVertexAttribArray((int)shader.colorAttribute);
		glVertexAttribPointer((int)shader.colorAttribute, 3, GL_FLOAT, GL_FALSE, m_stride, (void*)(m_coffset));
	}
	if (shader.uvAttribute.Valid() && m_toffset1 != -1)
	{
		glEnableVertexAttribArray((int)shader.uvAttribute);
		glVertexAttribPointer((int)shader.uvAttribute, 2, GL_FLOAT, GL_FALSE, m_stride, (void*)(m_toffset1));
	}
	if (shader.uvAttribute2.Valid() && m_toffset2 != -1)
	{
		glEnableVertexAttribArray((int)shader.uvAttribute2);
		glVertexAttribPointer((int)shader.uvAttribute2, 2, GL_FLOAT, GL_FALSE, m_stride, (void*)(m_toffset2));
	}

	if (shader.u_WVP.Valid())
	{
		shader.u_WVP.SetValue(m_worldViewProjectionMatrix);
	}
	if (shader.u_W.Valid())
	{
		shader.u_W.SetValue(m_worldMatrix);
	}

	m_VBO.DrawElements();

	if (shader.positionAttribute.Valid() && m_voffset != -1)
	{
		glDisableVertexAttribArray((int)shader.positionAttribute);
	}
	if (shader.normalAttribute.Valid() && m_noffset != -1)
	{
		glDisableVertexAttribArray((int)shader.normalAttribute);
	}
	if (shader.colorAttribute.Valid() && m_coffset != -1)
	{
		glDisableVertexAttribArray((int)shader.colorAttribute);
	}
	if (shader.uvAttribute.Valid() && m_toffset1 != -1)
	{
		glDisableVertexAttribArray((int)shader.uvAttribute);
	}
	if (shader.uvAttribute2.Valid() && m_toffset2 != -1)
	{
		glDisableVertexAttribArray((int)shader.uvAttribute2);
	}

	m_VBO.UnBindVBO();
}


void Mesh::SetWorldViewProjectionMatrix(const glm::mat4& matrix)
{
	m_worldViewProjectionMatrix = matrix;
}

void Mesh::SetWorldMatrix(const glm::mat4& matrix)
{
	m_worldMatrix = matrix;
}