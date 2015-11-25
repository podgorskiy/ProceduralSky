#include "SBSceneRenderer.h"

#include "SBMesh.h"
#include "SBCommon.h"
#include "SBNode.h"
#include "SBCamera.h"
#include "SBShader/SBShader.h"
#include "SBTimer/SBTimer.h"
#include "SBTexture/SBTexture.h"

using namespace SB;

SceneRenderer::SceneRenderer()
{
	m_lasttime = static_cast<double>(SB::GetMilliseconds()) / 1000.0;
	m_time = 0.0f;
}

const SceneRenderer::RenderList& SceneRenderer::RegisterNodes(Node* scene)
{
	RegisterNode(scene);
	return m_renderList;
}

void SceneRenderer::Render(const SceneRenderer::RenderList& renderlist, const Camera* camera, Shader* shader, const std::map<std::string, SB::TexturePtr>* textures)
{
	double dtime = static_cast<double>(SB::GetMilliseconds()) / 1000.0;
	float delta = dtime - m_lasttime;
	m_lasttime = dtime;
	m_time += delta;

	glm::mat4 viewProjection = camera->GetProjectionMatrix() * camera->GetViewMatrix();
	for (SceneRenderer::RenderList::const_iterator it = renderlist.begin(); it != renderlist.end(); ++it)
	{
		if (textures != nullptr)
		{
			std::map<std::string, SB::TexturePtr>::const_iterator itt = textures->find(it->second->GetTexture());
			if (itt != textures->end())
			{
				(*itt).second->Bind(0);
			}
			//std::map<std::string, SB::TexturePtr>::const_iterator ittl = textures->find(it->second->GetTexture2());
			//if (ittl != textures->end())
			//{
			//	(*ittl).second->Bind(1);
			//}
		}
		it->second->SetWorldMatrix(it->first);
		it->second->SetWorldViewProjectionMatrix(viewProjection * it->first);
		it->second->SetEyePosition(camera->GetPosition());
		it->second->SetTime(m_time * 10.0f);
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
