#include "SBColorUtils.h"

using namespace SB;

float ColorUtils::m_XYZ2RGB[] =
{
	3.240479f, -0.969256f, 0.055648f,
	-1.53715f, 1.875991f, -0.204043f,
	-0.49853f, 0.041556f, 1.057311f 
};

glm::vec3 ColorUtils::XYZToRGB(const glm::vec3& xyz)
{
	glm::vec3 rgb(
		m_XYZ2RGB[0] * xyz.x + m_XYZ2RGB[3] * xyz.y + m_XYZ2RGB[6] * xyz.z,
		m_XYZ2RGB[1] * xyz.x + m_XYZ2RGB[4] * xyz.y + m_XYZ2RGB[7] * xyz.z,
		m_XYZ2RGB[2] * xyz.x + m_XYZ2RGB[5] * xyz.y + m_XYZ2RGB[8] * xyz.z);
	return rgb;
};
