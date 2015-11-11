#pragma once

#include <glm/matrix.hpp>
#include "../aabb.h"

#include <vector>

class SBMesh
{
	friend class SBSceneConstructor;
public:
	SBMesh() :
		m_stride(0),
		m_voffset(-1),
		m_noffset(-1),
		m_coffset(-1),
		m_toffset1(-1),
		m_toffset2(-1)
	{
	};

private:
	int m_stride;
	int m_voffset;
	int m_noffset;
	int m_coffset;
	int m_toffset1;
	int m_toffset2;

	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec3> m_color;
	std::vector<glm::vec2> m_textcoord1;
	std::vector<glm::vec2> m_textcoord2;
	std::vector<int> m_indices;
};