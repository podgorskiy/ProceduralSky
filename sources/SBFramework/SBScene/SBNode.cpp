#include "SBNode.h"
#include "SBaabb.h"
#include "SBCommon.h"
#include "SBScene.h"

using namespace SB;

Node::Node() : m_isAbsoluteIsDirty(true), m_isRenderable(true), m_parent(NULL)
{};

Node::Node(const char* name) : m_name(name), m_isAbsoluteIsDirty(true), m_isRenderable(true), m_parent(NULL)
{};

Node::Node(const char* name, Scene& root) : m_name(name), m_root(&root), m_isAbsoluteIsDirty(true), m_isRenderable(true), m_parent(NULL)
{};

void Node::AddChild(Node& child)
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
	child.DetachNode();
	child.SetParent(this);
}

void Node::SetScene(Scene* scene)
{
	m_root = scene;
	int count = GetChildCount();
	for (int i = 0; i < count; i++)
	{
		Node* n = GetChild(i);
		n->SetScene(scene);
	}
}

int	Node::GetChildCount() const
{
	return m_members.size();
};

Node* Node::GetChild(int id)
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

const Node* Node::GetChild(int id) const
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

void Node::DetachNode()
{
	if (m_parent != NULL)
	{
		for (SBNodeContanier::iterator it = m_parent->m_members.begin(); it != m_parent->m_members.end(); ++it)
		{
			Node* node = *it;
			if (node == this)
			{
				m_parent->m_members.erase(it);
				return;
			}
		}
	}
}

void Node::SetDirty()
{
	m_isAbsoluteIsDirty = true;
	int count = GetChildCount();
	for (int i = 0; i < count; i++)
	{
		Node* n = GetChild(i);
		n->SetDirty();
	}
}

const glm::mat4& Node::GetAbsoluteTransform() const
{
	if (!m_isAbsoluteIsDirty)
	{
		return m_absoluteTransform;
	}

	if (this == m_root)
	{
		return GetLocalTransform();
	}

	m_absoluteTransform = GetParent()->GetAbsoluteTransform() * GetLocalTransform();

	m_isAbsoluteIsDirty = false;

	return m_absoluteTransform;
}

Node::~Node()
{
};
