#pragma once
#include "SBCommon.h"
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <sstream>
#include <pugixml.hpp>

namespace SB
{
	template <typename T>
	class IValueController
	{
	public:
		virtual ~IValueController(){};
		virtual void AddValue(float time, const T& val) = 0;
		virtual T GetValue(float time) const = 0;
		virtual void Clear() = 0;
	};

	template <typename T>
	class ConstValueController : public IValueController < T >
	{
	public:
		void AddValue(float time, const T& val)
		{
			m_value = val;
		};

		T GetValue(float time) const
		{
			return m_value;
		}

		void Clear(){};

	private:
		T m_value;
	};

	template <typename T>
	class DynamicValueController : public IValueController < T >
	{
		typedef std::map<float, T> KeyMap;
	public:
		DynamicValueController(){};
		~DynamicValueController(){};

		void AddValue(float time, const T& val)
		{
			m_keyMap[time] = val;
		};

		T GetValue(float time) const
		{
			typename KeyMap::const_iterator itUpper = m_keyMap.upper_bound(time + 1e-6f);  // + 1e-6f - some quick fix for some rare condition. 
			typename KeyMap::const_iterator itLower = itUpper;
			--itLower;
			if (itLower == m_keyMap.end())
			{
				return itUpper->second;
			}
			if (itUpper == m_keyMap.end())
			{
				return itLower->second;
			}
			float lowerTime = itLower->first;
			const T& lowerVal = itLower->second;
			float upperTime = itUpper->first;
			const T& upperVal = itUpper->second;
			if (lowerTime == upperTime)
			{
				return lowerVal;
			}
			return interpolate(lowerTime, lowerVal, upperTime, upperVal, time);
		};

		void Clear()
		{
			m_keyMap.clear();
		};

	private:
		template <typename A>
		const A interpolate(float lowerTime, const A& lowerVal, float upperTime, const A& upperVal, float time) const
		{
			float mix = (time - lowerTime) / (upperTime - lowerTime);
			return lowerVal * (1.0f - mix) + upperVal * mix;
		};


	private:
		KeyMap	m_keyMap;
	};

	template <> template <>
	inline const glm::quat DynamicValueController<glm::quat>::interpolate<glm::quat>(float lowerTime, const glm::quat& lowerVal, float upperTime, const glm::quat& upperVal, float time) const
	{
		float mix = (time - lowerTime) / (upperTime - lowerTime);
		glm::quat q = glm::slerp(lowerVal, upperVal, mix);
		glm::normalize(q);
		return q;
	};

	class XMLReader
	{
	public:
		XMLReader(pugi::xml_node node) :m_node(node){};

		template<typename T>
		void ReadValue(T& value);
	private:
		void ReadComponents(std::vector<std::string>& components, const char* values)
		{
			std::istringstream val(values);
			std::copy(std::istream_iterator<std::string>(val), std::istream_iterator<std::string>(), back_inserter(components));
		}

		pugi::xml_node m_node;
	};

	static float atof_(const char* string)
	{
		return static_cast<float>(atof(string));
	}

	template<>
	inline void XMLReader::ReadValue(float& value)
	{
		value = atof_(m_node.child_value());
	};

	template<>
	inline void XMLReader::ReadValue(glm::vec2& value)
	{
		std::vector<std::string> components;
		ReadComponents(components, m_node.child_value());
		ASSERT(components.size() == 2, "wrong components count");
		value = glm::vec2(atof_(components[0].c_str()), atof_(components[1].c_str()));
	};

	template<>
	inline void XMLReader::ReadValue(glm::vec3& value)
	{
		std::vector<std::string> components;
		ReadComponents(components, m_node.child_value());
		ASSERT(components.size() == 3, "wrong components count");
		value = glm::vec3(atof_(components[0].c_str()), atof_(components[1].c_str()), atof_(components[2].c_str()));
	};

	template<>
	inline void XMLReader::ReadValue(glm::vec4& value)
	{
		std::vector<std::string> components;
		ReadComponents(components, m_node.child_value());
		ASSERT(components.size() == 4, "wrong components count");
		value = glm::vec4(atof_(components[0].c_str()), atof_(components[1].c_str()), atof_(components[2].c_str()), atof_(components[3].c_str()));
	};

	template<>
	inline void XMLReader::ReadValue(glm::quat& value)
	{
		std::vector<std::string> components;
		ReadComponents(components, m_node.child_value());
		ASSERT(components.size() == 4, "wrong components count");
		value = glm::quat(atof_(components[0].c_str()), atof_(components[1].c_str()), atof_(components[2].c_str()), atof_(components[3].c_str()));
	};

	template <typename T>
	class Value
	{
	public:
		Value(const char* name) :
			m_parameterName(name),
			m_valueController(NULL),
			m_parameterID(-1)
		{
		};

		void Load(pugi::xml_node node)
		{
			SafeDelete(m_valueController);
			if (node.child("key") == NULL)
			{
				m_valueController = new ConstValueController<T>();
				T value;
				XMLReader(node).ReadValue(value);
				m_valueController->AddValue(0.0f, value);
			}
			else
			{
				m_valueController = new DynamicValueController<T>();
				for (pugi::xml_node key = node.child("key"); key; key = key.next_sibling("key"))
				{
					float time = key.attribute("time").as_float();
					T value;
					XMLReader(key).ReadValue(value);
					m_valueController->AddValue(time, value);
				}
			}
		};

		~Value()
		{
			SafeDelete(m_valueController);
		};

		T GetValue(float time) const
		{
			return m_valueController->GetValue(time);
		};

		void UpdateMaterialValue(float time) const
		{
			/* UPDATE TODO!!!
			if (m_parameterID == -1)
			{
				m_parameterID = m_materialManager->getParameterID(m_parameterName.c_str());
			}
			m_materialManager->setParameter(m_parameterID, GetValue(time));
			*/
		};

		std::string GetName() const
		{
			return m_parameterName;
		};

	private:

		std::string m_parameterName;
		mutable int m_parameterID;
		IValueController<T>* m_valueController;
	};

	typedef Value<float> FloatValue;
	typedef Value<glm::vec2> Vec2Value;
	typedef Value<glm::vec3> Vec3Value;
	typedef Value<glm::vec4> Vec4Value;
	typedef Value<glm::quat> QuaternionValue;

	class Group
	{
	public:
		enum
		{
			ValueTypeOffset = 3,
			ValueTypeMask = (1 << ValueTypeOffset) - 1
		};

		Group(DynamicLighteningProperties *dynaimcLightening, const char* name) : m_dynaimcLightening(dynaimcLightening), m_name(name){};
		~Group()
		{
		}

		void Load(pugi::xml_node node)
		{
			Load<FloatValue>(node, m_dynaimcLightening->m_float, "float");
			Load<Vec2Value>(node, m_dynaimcLightening->m_vec2, "vec2");
			Load<Vec3Value>(node, m_dynaimcLightening->m_vec3, "vec3");
			Load<Vec4Value>(node, m_dynaimcLightening->m_vec4, "vec4");
			Load<QuaternionValue>(node, m_dynaimcLightening->m_quat, "quat");
		};

		template<typename T>
		void Load(const pugi::xml_node& node, std::vector<T*>& vec, const char* typeName)
		{
			for (pugi::xml_node parameter = node.child(typeName); parameter; parameter = parameter.next_sibling(typeName))
			{
				T* newValue = new T(parameter.attribute("name").value());
				newValue->Load(parameter);
				int pos = static_cast<int>(vec.size());
				DynamicLighteningProperties::ValueID id = (pos << ValueTypeOffset) + GetValueType<T>();

				// if we have already registered this value, then this method would return old id, thus old cached ids would still be valid
				unsigned int idOld = m_dynaimcLightening->registerValue(m_name + "." + newValue->GetName(), id);
				unsigned int posOld = idOld >> ValueTypeOffset;
				if (posOld >= vec.size())
				{
					vec.resize(posOld + 1, NULL);
				}
				vec[posOld] = newValue;
			}
		};

		template < typename T >
		static DynamicLighteningProperties::ValueType GetValueType();

	private:
		DynamicLighteningProperties *m_dynaimcLightening;
		std::string m_name;
	};

	template <>
	inline DynamicLighteningProperties::ValueType Group::GetValueType<FloatValue>()
	{
		return DynamicLighteningProperties::Float;
	};

	template <>
	inline DynamicLighteningProperties::ValueType Group::GetValueType<Vec2Value>()
	{
		return DynamicLighteningProperties::Vec2;
	};

	template <>
	inline DynamicLighteningProperties::ValueType Group::GetValueType<Vec3Value>()
	{
		return DynamicLighteningProperties::Vec3;
	};

	template <>
	inline DynamicLighteningProperties::ValueType Group::GetValueType<Vec4Value>()
	{
		return DynamicLighteningProperties::Vec4;
	};

	template <>
	inline DynamicLighteningProperties::ValueType Group::GetValueType<QuaternionValue>()
	{
		return DynamicLighteningProperties::Quaternion;
	};


	template<typename T>
	void UpdateDynamicDataValues(float time, const std::vector<T*>& parameters)
	{
		for (typename std::vector<T*>::const_iterator it = parameters.begin(); it != parameters.end(); ++it)
		{
			if (*it != NULL)
			{
				(*it)->UpdateMaterialValue(time);
			}
		}
	};

	template<typename T>
	void DeleteDynamicDataValues(std::vector<T*>& parameters)
	{
		for (typename std::vector<T*>::const_iterator it = parameters.begin(); it != parameters.end(); delete (*it++));
		parameters.clear();
	};
}