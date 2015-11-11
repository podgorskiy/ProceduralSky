#pragma once

#include <glm/matrix.hpp>
#include "aabb.h"
#include "SBCommon.h"
#include <string>

class SBScene;
class SBMesh;

class SBNode
{
public: 
	SBNode() : m_isAbsoluteIsDirty(true), m_isRenderable(true)
	{};
	SBNode(const char* name) : m_name(name),  m_isAbsoluteIsDirty(true), m_isRenderable(true)
	{};
	SBNode(const char* name, SBScene& root) : m_name(name), m_root(&root),  m_isAbsoluteIsDirty(true), m_isRenderable(true)
	{};
	virtual ~SBNode();
	
	typedef SBContainer<SBNode> SBNodeContanier;
	
	void				AddChild(SBNode& child);
	SBNode*				GetChild(int id);
	const SBNode*		GetChild(int id) const;
	int					GetChildCount() const;
	SBNode*				GetChild(const std::string& name);
	SBNode*				GetChild(const char* name);
	SBScene* 			GetScene() const				{ return m_root; };
	void 				SetScene(SBScene* scene)		{ m_root = scene; };
	const std::string&	GetName() const					{ return m_name; };
	void				SetParent(SBNode* parent)		{ m_parent = parent; };
	SBNode*				GetParent() const				{ return m_parent; };
	void				SetMesh(SBMesh* mesh)			{ m_mesh = mesh; };
	const SBMesh*		GetMesh() const					{ return m_mesh; };

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
	bool					IsRenderable()								{ return m_isRenderable; };
	void					SetRenderable(bool v)						{ m_isRenderable = v; };

protected:
	SBScene*			m_root;
	SBNode*				m_parent;
	SBNodeContanier		m_members;
	std::string			m_name;
	glm::mat4			m_relativeTransform;
	mutable glm::mat4	m_absoluteTransform;
	mutable bool		m_isAbsoluteIsDirty;
	bool				m_isRenderable;

	glm::aabb3df		m_aabb;

	SBMesh*				m_mesh;
};
