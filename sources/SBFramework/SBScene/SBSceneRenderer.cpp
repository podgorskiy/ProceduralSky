#include "SBSceneRenderer.h"

#include "SBMesh.h"
#include "SBCommon.h"
#include "SBScene.h"
#include "SBCamera.h"
#include "SBShader/SBShader.h"

using namespace SB;

SceneRenderer::SceneRenderer()
{
}

const SceneRenderer::RenderList& SceneRenderer::RegisterNodes(Node* scene)
{
	RegisterNode(scene);
	return m_renderList;
}

void SceneRenderer::Render(const SceneRenderer::RenderList& renderlist, const Camera* camera, Shader* shader)
{
	glm::mat4 viewProjection = camera->GetProjectionMatrix() * camera->GetViewMatrix();
	for (SceneRenderer::RenderList::const_iterator it = renderlist.begin(); it != renderlist.end(); ++it)
	{
		it->second->SetWorldMatrix(it->first);
		it->second->SetWorldViewProjectionMatrix(viewProjection * it->first);
		it->second->Draw(/*camera*/*shader);
	}
}

void SceneRenderer::Render(const Camera* camera, Shader* shader)
{
	Render(m_renderList, camera, shader);
	m_renderList.clear();
}

void SceneRenderer::RegisterNode(Node* node)
{
	const std::vector<Mesh*>& meshs = node->GetMeshs();

	for(std::vector<Mesh*>::const_iterator it = meshs.begin(); it != meshs.end(); ++it)
	{
		RegisteMesh(node, *it);
	}

	int count = node->GetChildCount();
	for (int i = 0; i < count; i++)
	{
		Node* n = node->GetChild(i);
		if (n->IsRenderable())
		{
			RegisterNode(n);
		}
	}
}

void SceneRenderer::RegisteMesh(Node* node, Mesh* drawable)
{
	glm::mat4 transform = node->GetAbsoluteTransform();
	m_renderList.push_back(Entity(transform, drawable));
}
