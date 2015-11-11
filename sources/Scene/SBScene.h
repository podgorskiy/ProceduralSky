#pragma once

#include "SBNode.h"
#include <map>

class SBScene: public SBNode
{
public:
	SBScene() :m_binBuffer(NULL){
		m_root = this;
		m_map["root"] = this;
	};
	SBScene(const char* name) :m_binBuffer(NULL){
		m_name = name;
		m_root = this;
		m_map["root"] = this;
	};

	// \todo
	virtual ~SBScene()
	{
	};

	void SetBinBuffer(char* buff)
	{
		m_binBuffer = buff;
	}
	
	void AddChild(SBNode& child){
		SBNode::AddChild(child);
		AddToMap(child);
	};
	
	SBNode&		GetNodeByName(std::string& name)
	{
		return *m_map[name];
	};
	SBNode&		GetNodeByName(const char* name)
	{
		return *m_map[name];
	};
	SBNode*		GetNodeByNameSafe(const char* name)
	{
		std::map<std::string, SBNode*>::iterator n = m_map.find(name);
		if (n != m_map.end())
		{
			return n->second;
		}
		else
		{
			return NULL;
		}
	};
	std::map<std::string, SBNode*>::const_iterator GetBegin() const
	{
		return m_map.begin();
	};
	std::map<std::string, SBNode*>::const_iterator GetEnd() const
	{
		return m_map.end();
	};
protected:
friend SBNode;
	void AddToMap(SBNode& node)			
	{	
		std::string st = node.GetName();
		m_map[st] = &node; 
	};	
	std::map<std::string, SBNode*>	m_map;
	char* m_binBuffer;
};