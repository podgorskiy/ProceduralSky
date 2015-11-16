#pragma once

#include "SBNode.h"
#include <map>

namespace SB
{
	class Scene : public Node
	{
	public:
		Scene();
		Scene(const char* name);

		virtual ~Scene();

		void AddChild(Node& child);

		Node& GetNodeByName(std::string& name);

		Node& GetNodeByName(const char* name);

		Node* GetNodeByNameSafe(const char* name);

		std::map<std::string, Node*>::const_iterator GetBegin() const;

		std::map<std::string, Node*>::const_iterator GetEnd() const;
		
	protected:
		friend Node;

		void AddToMap(Node& node);

		std::map<std::string, Node*> m_map;
	};
}