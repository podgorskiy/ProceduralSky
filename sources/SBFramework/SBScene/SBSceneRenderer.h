/*! \file SBSceneManager.h
\brief Stuff for scene rendering.*/
#pragma once
#include <vector>
#include <utility>
#include <glm/matrix.hpp>

namespace SB
{
	class Camera;
	class Shader;
	class Mesh;
	class Node;

	/// Class for rendering SBScene.
	/** Handles registering and batch rendering of nodes. Handles diffrent types of projection*/
	class SceneRenderer
	{
	public:
		typedef std::pair<glm::mat4, Mesh*> Entity;
		typedef std::vector<Entity> RenderList;

		SceneRenderer();

		const RenderList& RegisterNodes(Node* scene);

		void Render(const Camera* camera, Shader* shader);
		void Render(const RenderList& renderlist, const Camera* camera, Shader* shader);

	private:
		void RegisteMesh(Node* node, Mesh* drawable);
		void RegisterNode(Node* node);

	private:
		RenderList m_renderList;
	};
}