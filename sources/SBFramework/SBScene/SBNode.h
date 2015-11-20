#pragma once

#include "SBaabb.h"
#include "SBNodeContanier.h"

#include <glm/matrix.hpp>
#include <string>

namespace SB
{
	class Mesh;

	class Node
	{
	public:
		Node();
		Node(const char* name);
		Node(const char* name, Node& root);

		~Node();

		typedef SBContainer<Node> SBNodeContanier;

		void				AddChild(Node& child);
		Node*				GetChild(int id);
		const Node*			GetChild(int id) const;
		int					GetChildCount() const;
		Node*				GetChild(const std::string& name);
		Node*				GetChild(const char* name);
		Node* 				GetRoot() const				{ return m_root; };
		void 				SetRoot(Node* root);
		const std::string&	GetName() const					{ return m_name; };
		void				SetName(const std::string& name){ m_name = name; };
		void				SetParent(Node* parent)			{ m_parent = parent; };
		Node*				GetParent() const				{ return m_parent; };
		void				DetachNode();
		void				SetMeshs(const std::vector<Mesh*>& meshs)			{ m_meshs = meshs; };
		const std::vector<Mesh*>& GetMeshs() const			{ return m_meshs; };
		void				PushMesh(Mesh* mesh)			{ m_meshs.push_back(mesh); };
		Node*				GetNodeByName(const std::string& name);

	public:
		//node transformation methods
		const glm::mat4& 		GetLocalTransform() const					{ return m_relativeTransform; };
		void			 		SetLocalTransform(const glm::mat4& m)		{ m_relativeTransform = m; m_isAbsoluteIsDirty = true; };
		const glm::mat4& 		GetAbsoluteTransform() const;
		void					SetDirty();

		//bounding box
		void					SetAABB(const glm::aabb3df& box)			{ m_aabb = box; };
		const glm::aabb3df&		GetAABB() const								{ return m_aabb; };

		//node rendering methods
		bool					IsRenderable() const						{ return m_isRenderable; };
		void					SetRenderable(bool v)						{ m_isRenderable = v; };

	protected:
		Node*				m_root;
		Node*				m_parent;
		SBNodeContanier		m_members;
		mutable glm::mat4	m_absoluteTransform;
		mutable bool		m_isAbsoluteIsDirty;
		glm::aabb3df		m_aabb;

		// Serializables
		std::string			m_name;
		glm::mat4			m_relativeTransform;
		std::vector<Mesh*>	m_meshs;
		bool				m_isRenderable;
	};
}
