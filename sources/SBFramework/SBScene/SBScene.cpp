#include "SBCommon.h"
#include "SBScene.h"

using namespace SB;

Scene::Scene()
{
	m_root = this;
	m_map["root"] = this;
};

Scene::Scene(const char* name)
{
	m_name = name;
	m_root = this;
	m_map["root"] = this;
};

Scene::~Scene()
{
};

void Scene::AddChild(Node& child)
{
	Node::AddChild(child);
	AddToMap(child);
};

Node& Scene::GetNodeByName(std::string& name)
{
	return *m_map[name];
};

Node& Scene::GetNodeByName(const char* name)
{
	return *m_map[name];
};

Node* Scene::GetNodeByNameSafe(const char* name)
{
	std::map<std::string, Node*>::iterator n = m_map.find(name);
	if (n != m_map.end())
	{
		return n->second;
	}
	else
	{
		return NULL;
	}
};

std::map<std::string, Node*>::const_iterator Scene::GetBegin() const
{
	return m_map.begin();
};

std::map<std::string, Node*>::const_iterator Scene::GetEnd() const
{
	return m_map.end();
};

void Scene::AddToMap(Node& node)
{
	std::string st = node.GetName();
	m_map[st] = &node;
};
