#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "../aabb.h"

#include <vector>
#include <string>
#include <map>
#include <cassert>

#include "SBCommon.h"
#include "SBNode.h"
#include "SBScene.h"


void SBNode::AddChild(SBNode& child)
{
	if (child.GetScene() != GetScene())
	{
		child.SetScene(GetScene());
	}
	m_members.push_back(&child);
	if (GetScene() != NULL)
	{
		GetScene()->AddToMap(child);
	}
	child.SetParent(this);
}

int	SBNode::GetChildCount() const
{
	return m_members.size();
};

SBNode*	SBNode::GetChild(int id)
{
	if (static_cast<unsigned int>(id) < m_members.size())
	{
		return m_members.at(id);
	}
	else
	{
		return NULL;
	}
};

const SBNode* SBNode::GetChild(int id) const
{
	if (static_cast<unsigned int>(id) < m_members.size())
	{
		return m_members.at(id);
	}
	else
	{
		return NULL;
	}
};

void SBNode::SetDirty()
{
	m_isAbsoluteIsDirty = true;
	int count = GetChildCount();
	for (int i = 0; i < count; i++)
	{
		SBNode* n = GetChild(i);
		n->SetDirty();
	}
}

const glm::mat4& SBNode::GetAbsoluteTransform() const
{
	if (!m_isAbsoluteIsDirty)
	{
		return m_absoluteTransform;
	}

	if (this == m_root)
	{
		return GetLocalTransform();
	}

	m_absoluteTransform = GetLocalTransform() * GetParent()->GetAbsoluteTransform();

	m_isAbsoluteIsDirty = false;

	return m_absoluteTransform;
}

SBNode::~SBNode()
{
};