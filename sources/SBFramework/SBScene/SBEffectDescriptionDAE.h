#pragma once
#include <glm/matrix.hpp>
#include <string>

namespace SB
{
	struct EffectDescription
	{
	public:

		enum FixedPipeLineType
		{
			blin,
			lambert,
			phong
		};

		struct FixedParameter
		{
			enum Type
			{
				Color,
				Float,
				Texture
			};

			Type m_type;
			glm::vec4 m_color;
			float m_floatValue;
			std::string m_sampler;
		};

		FixedPipeLineType type;

		FixedParameter emission;
		FixedParameter ambient;
		FixedParameter diffuse;
		FixedParameter specular;
		FixedParameter shininess;
		FixedParameter reflective;
		FixedParameter reflectivity;
		FixedParameter transparent;
		FixedParameter transparency;
		FixedParameter index_of_refraction;
	};
}