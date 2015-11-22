#pragma once
#include "SBScene/SBSceneRenderer.h"
#include "SBAsyncDataLoad/SBRequestPull.h"
#include "SBAsyncDataLoad/SBRequestData.h"
#include <memory>

namespace SB
{
	class EventManager;
	class Shader;
	class Node;
	class SceneRenderer;
	class Camera;
	class MemoryFile;
}

class CityManager
{
public:
	void Init();
	void Draw(SB::Camera* camera);

	void ConstructCityFromDAE();
	void MakeCityBatches();
	void MakeOcean();
	void LoadCityBatches();
	void LoadOcean(const SB::IFile* memfile);

	void PushCityBatch(const SB::IFile* file);

	void SetShader(SB::Shader* shader);
private:
	std::unique_ptr<SB::SceneRenderer> m_sceneRenderer;
	SB::SceneRenderer::RenderList m_renderlistCity;
	SB::SceneRenderer::RenderList m_renderlistOcean;

	SB::RequestPull m_rpull;

	SB::Shader* m_terrainShader;
};