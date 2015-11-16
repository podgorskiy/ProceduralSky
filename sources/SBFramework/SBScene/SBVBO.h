/*! \file SBVBO.h
\brief The VBO interface.*/
#pragma once
#include "SBOpenGLHeaders.h"

namespace SB
{
	/// Interface for interacting with vertex buffer objects
	/** Offers methods for creating, destroing, filling, binding and drawing buffers*/
	class VBO
	{
	public:

		VBO();
		~VBO();

		/// Create vertex array and index buffer handles
		void CreateBuffers();

		/// Destroy buffers
		void DeleteBuffers();

		/// Fill buffers with data
		/** \param VertexArray - pointer to vertex data
			\param numOfVertices - count of vertices
			\param verticesStride - stride
			\param idexArray - pointer to index data
			\param numOfIndices - count of indices
			\param indexSize - index size */
		void FillBuffers(
			const void*	VertexArray,
			int			numOfVertices,
			int			verticesStride,
			const void*	indexArray,
			int			numOfIndices,
			int			indexSize);

		void FillVertexBuffer(
			const void*	VertexArray,
			int			numOfVertices,
			bool		dynamic = false);

		void SetIndexCount(int count);

		void FillIndexBuffer(
			const void*	indexArray,
			int			numOfIndices);

		/// Bind buffers
		void BindVBO() const;
		void UnBindVBO() const;

		/// Draw buffers
		void DrawElements() const;

		int GetVertexSize() const;
		int GetIndexSize() const;
		int GetStride() const;

	private:
		int		m_numOfVertices;
		int		m_numOfIndices;
		int		m_stride;
		GLuint	m_indexType;
		GLuint	m_vboHandles[2];			///< buffers handles
	};
}