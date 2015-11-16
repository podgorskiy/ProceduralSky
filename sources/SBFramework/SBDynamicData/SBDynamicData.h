#pragma once
#include <glm/matrix.hpp>
#include <string>
#include <vector>
#include <map>

namespace SB
{
	class IFile;
	class Group;
	template <typename T> class Value;

	/* Provides functionality to operate with values that can be time dependent. The main perpouse is to operate with lightening parameters.
	Loads data from xml file. Supports different values types: float, vec2, vec3, vec4, (quaternion). Any variable can be either const or
	dynamic. Dynamic values depends on time passed, this values defined as timed keys. Values between time keys are linearly interpolated. */
	class DynamicLighteningProperties
	{
	public:

		/* Type for value id
		Every value has an id that provides quick access to value. **/
		typedef unsigned int ValueID;

		DynamicLighteningProperties();

		/* Loades values data from xml.**/
		void Load(IFile* file);

		/* Clears all data, apart from "value name -> id" map. **/
		void Unload();

		/* Updates all unoforms, that are used by shaders, with new data, that corresponse to the passed time. **/
		void UpdateMaterialsValues(float time);

		/* Returns value id. Once value was loaded from xml, it is assigned an id, that would not be changed in spite of subsequent data loads/unloads.
		Thus, once the id was cached, there is no need to update it. **/
		ValueID GetValueID(const std::string name) const;

		/* Returns value by id for the time that was passed. If value is constant, the value of time does not matter.
		Template parameter can be:
		float,
		glitch::core::vector2df,
		glitch::core::vector3df,
		glitch::core::vector4df,
		(glitch::core::quaternion)
		If wrong type is passed, then the assert is thrown. **/
		template<typename T> T GetValueByID(ValueID id, float time) const;

	private:
		friend class Group;

		enum ValueType
		{
			Float,
			Vec2,
			Vec3,
			Vec4,
			Quaternion,
		};

		typedef Value<float>					FloatValue;
		typedef Value<glm::vec2>	Vec2Value;
		typedef Value<glm::vec3>	Vec3Value;
		typedef Value<glm::vec4>	Vec4Value;
		typedef Value<glm::quat>	QuaternionValue;

		ValueID registerValue(std::string name, ValueID id);

		template<typename ValueType>
		const std::vector<ValueType*>& GetValuesVector() const;

		std::vector<Group*>				m_groups;
		std::vector<FloatValue*>		m_float;
		std::vector<Vec2Value*>			m_vec2;
		std::vector<Vec3Value*>			m_vec3;
		std::vector<Vec4Value*>			m_vec4;
		std::vector<QuaternionValue*>	m_quat;
		std::map<std::string, ValueID>	m_idLookupTable;
	};
}