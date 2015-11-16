#include "SBVBO.h"

#include <cstdlib>
#include <iostream>

using namespace SB;

VBO::VBO() :
		m_numOfVertices(-1), 
		m_numOfIndices(-1), 
		m_stride(-1)
{
	m_vboHandles[0] = -1;
	m_vboHandles[1] = -1;
	m_indexType = -1;
}

VBO::~VBO()
{
	DeleteBuffers();
}

void VBO::CreateBuffers()
{
	glGenBuffers(2, m_vboHandles);
}

void VBO::DeleteBuffers()
{
	if (m_vboHandles[0] != -1)
	{
		GLuint* b = m_vboHandles;
		glDeleteBuffers(2, b);
		m_vboHandles[0] = -1;
		m_vboHandles[1] = -1;
	}
}
	
void VBO::FillBuffers(
		const void*	VertexArray,
		int			numOfVertices,
		int			verticesStride,
		const void*	indexArray,
		int			numOfIndices,
		int			indexSize)
{
	if (indexSize == 2)
	{
		m_indexType = GL_UNSIGNED_SHORT;
	}
	else if (indexSize == 4)
	{
		m_indexType = GL_UNSIGNED_INT;
	}
	m_stride = verticesStride;
	FillVertexBuffer(VertexArray, numOfVertices);
	FillIndexBuffer(indexArray, numOfIndices);
}

void VBO::FillVertexBuffer(
		const void*	VertexArray,
		int			numOfVertices,
		bool		dynamic)
{
	m_numOfVertices = numOfVertices;
	glBindBuffer(GL_ARRAY_BUFFER, m_vboHandles[0]);
	int vertexBufferSize = GetVertexSize()*numOfVertices;
	if (dynamic)
	{
		glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, VertexArray, GL_STATIC_DRAW);
	}
	else
	{
		glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, VertexArray, GL_STATIC_DRAW);
	}
	//std::cout << vertexBufferSize << std::endl;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::SetIndexCount(int count)
{
	m_numOfIndices = count;
};

void VBO::FillIndexBuffer(
	const void*	indexArray,
	int			numOfIndices)
{
	m_numOfIndices = numOfIndices;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboHandles[1]);
	int indexBufferSize = GetIndexSize()*numOfIndices;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, indexArray, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//std::cout << indexBufferSize << std::endl;
};

void VBO::BindVBO() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vboHandles[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboHandles[1]);
};

void VBO::UnBindVBO() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
};

void VBO::DrawElements() const
{
	glDrawElements(GL_TRIANGLES, m_numOfIndices, m_indexType, 0);
};

int VBO::GetVertexSize() const
{ 
	return m_stride; 
};

int VBO::GetIndexSize() const
{ 
	if (m_indexType == GL_UNSIGNED_SHORT)
	{
		return 2;
	}
	else if (m_indexType == GL_UNSIGNED_INT)
	{
		return 4;
	}
	return 0;
};
