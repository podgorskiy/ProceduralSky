#pragma once
#include <glm/matrix.hpp>

namespace SB
{
	class ColorUtils
	{
	public:
		static glm::vec3 XYZToRGB(const glm::vec3& xyz);

	private:
		static float m_XYZ2RGB[];
	};
}