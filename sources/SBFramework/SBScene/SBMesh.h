#pragma once
#include "SBaabb.h"
#include "SBVBO.h"

#include <glm/matrix.hpp>
#include <vector>
#include <string>
#include <map>

namespace SB
{
	class Shader;
	class Node;

	class Mesh
	{
		friend class SceneDAEConstructor;
		friend class BufferConstructor;
		friend class Serializer;
		friend class Utils;
	public:

		struct VerticesMaps
		{
			std::vector<glm::vec3> m_vertices;
			std::vector<glm::vec3> m_normals;
			std::vector<glm::vec3> m_color;
			std::vector<glm::vec2> m_textcoord1;
			std::vector<glm::vec2> m_textcoord2;
			std::vector<int> m_indices;
		};

		Mesh();
		~Mesh();

		void CreateVBO();
		void Draw(Shader& shader);

		/*Remove from here*/
		void SetWorldViewProjectionMatrix(const glm::mat4& matrix);
		void SetWorldMatrix(const glm::mat4& m);
		void SetEyePosition(const glm::vec3& v);
		void SetTime(float t);
		void SetTexture(const std::string& texture);
		const std::string& GetTexture() const;
		void SetTexture2(const std::string& texture);
		const std::string& GetTexture2() const;

		void SetMaterialName(const std::string& materialName);
		const std::string& GetMaterialName() const;
	private:
		
		VerticesMaps* m_verticesMaps;

		/*Remove from here*/
		glm::mat4 m_worldMatrix;
		glm::mat4 m_worldViewProjectionMatrix;
		glm::vec3 m_eyePosition;
		float m_time;
		std::string m_textureFileName;
		std::string m_textureFileName2;

		VBO m_VBO;
		
		// Serializables
		unsigned int* m_rawIntIndices;
		unsigned short* m_rawShortIndices;
		float* m_rawbuffer;

		int m_verticesCount;
		int m_indeciesCount;

		int m_stride;
		int m_voffset;
		int m_noffset;
		int m_coffset;
		int m_toffset1;
		int m_toffset2;

		std::string m_materialName;
	};
}