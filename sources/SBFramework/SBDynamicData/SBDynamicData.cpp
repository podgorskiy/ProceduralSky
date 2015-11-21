#include "SBDynamicData.h"
#include "SBDynamicDataValues.h"
#include "SBFileSystem/SBCFile.h"

#include <pugixml.hpp>

using namespace SB;

DynamicLighteningProperties::DynamicLighteningProperties()
{};

void DynamicLighteningProperties::Load(IFile* file)
{
	char* buffer = new char[file->GetSize()];
	file->Read(buffer, file->GetSize());
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_buffer(buffer, file->GetSize());
	pugi::xml_node node = doc.child("data");
	for (pugi::xml_node groupNode = node.child("group"); groupNode; groupNode = groupNode.next_sibling("group"))
	{
		Group* group = new Group(this, groupNode.attribute("name").value());
		group->Load(groupNode);
		m_groups.push_back(group);
	}
	delete[] buffer;
}

void DynamicLighteningProperties::Unload()
{
	DeleteDynamicDataValues(m_float);
	DeleteDynamicDataValues(m_vec2);
	DeleteDynamicDataValues(m_vec3);
	DeleteDynamicDataValues(m_vec4);
}

void DynamicLighteningProperties::UpdateMaterialsValues(float time)
{
	UpdateDynamicDataValues(time, m_float);
	UpdateDynamicDataValues(time, m_vec2);
	UpdateDynamicDataValues(time, m_vec3);
	UpdateDynamicDataValues(time, m_vec4);
};

DynamicLighteningProperties::ValueID DynamicLighteningProperties::registerValue(std::string name, ValueID id)
{
	std::pair<std::map<std::string, ValueID>::iterator, bool> result = m_idLookupTable.insert(std::make_pair(name, id));
	return result.first->second;
};

DynamicLighteningProperties::ValueID DynamicLighteningProperties::GetValueID(const std::string name) const
{
	std::map<std::string, ValueID>::const_iterator it = m_idLookupTable.find(name);
	if (it != m_idLookupTable.end())
	{
		return it->second;
	}
	return -1;
}

template<typename T>
T DynamicLighteningProperties::GetValueByID(ValueID id, float time) const
{
	unsigned int pos = id >> Group::ValueTypeOffset;
	ASSERT(Group::GetValueType< Value<T> >() == (id & Group::ValueTypeMask), "Wrong value type!!!");
	const std::vector< Value<T>* >& vec = GetValuesVector< Value<T> >();
	if (vec.size() > pos)
	{
		const Value<T>* value = vec[pos];
		if (value != NULL)
		{
			T valueAtTime = value->GetValue(time);
			return valueAtTime;
		}
	}
	ASSERT(false, "value is missing")
	return T();
}

template<>
const std::vector<FloatValue*>& DynamicLighteningProperties::GetValuesVector<FloatValue>() const
{
	return m_float;
}

template<>
const std::vector<Vec2Value*>& DynamicLighteningProperties::GetValuesVector<Vec2Value>() const
{
	return m_vec2;
}

template<>
const std::vector<Vec3Value*>& DynamicLighteningProperties::GetValuesVector<Vec3Value>() const
{
	return m_vec3;
}

template<>
const std::vector<Vec4Value*>& DynamicLighteningProperties::GetValuesVector<Vec4Value>() const
{
	return m_vec4;
}

template<>
const std::vector<QuaternionValue*>& DynamicLighteningProperties::GetValuesVector<QuaternionValue>() const
{
	return m_quat;
}

template float DynamicLighteningProperties::GetValueByID<float>(ValueID id, float time) const;

template glm::vec2 DynamicLighteningProperties::GetValueByID<glm::vec2>(ValueID id, float time) const;

template glm::vec3 DynamicLighteningProperties::GetValueByID<glm::vec3>(ValueID id, float time) const;

template glm::vec4 DynamicLighteningProperties::GetValueByID<glm::vec4>(ValueID id, float time) const;

template glm::quat DynamicLighteningProperties::GetValueByID<glm::quat>(ValueID id, float time) const;