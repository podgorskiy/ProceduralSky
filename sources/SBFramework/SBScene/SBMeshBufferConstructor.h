#pragma once

#include <glm/matrix.hpp>
#include <vector>
#include <map>

namespace SB
{
	class GeneralType
	{
	public:
		enum Type
		{
			FLOAT,
			VEC2,
			VEC3,
			VEC4,
			INT,
			IVEC2,
			IVEC3,
			IVEC4
		};

		operator float();
		float& operator =(float& x);

		operator glm::vec2();
		glm::vec2& operator =(glm::vec2& x);

		operator glm::vec3();
		glm::vec3& operator =(glm::vec3& x);

		operator glm::vec4();
		glm::vec4& operator =(glm::vec4& x);

		float m_f;
		glm::vec2 m_vec2;
		glm::vec3 m_vec3;
		glm::vec4 m_vec4;
		Type m_type;
		GeneralType(float x);
		GeneralType(glm::vec2 x);
		GeneralType(glm::vec3 x);
		GeneralType(glm::vec4 x);
	};

	class SmartVertex
	{
	public:
		std::vector<GeneralType> m_vdata;
		std::vector<unsigned int> m_links;
	};

	class Mesh;

	class BufferConstructor
	{
	public:
		void ConvertMesh(Mesh* mesh);

	private:
		unsigned int PushVertex(const SmartVertex& v);

		int Hash(const std::vector<unsigned int>& links);

		std::vector<const SmartVertex> m_vertices;
		std::map<int, unsigned int> m_links;
	};
}