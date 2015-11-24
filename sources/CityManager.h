#pragma once
#include "SBScene/SBSceneRenderer.h"
#include "SBAsyncDataLoad/SBRequestPull.h"
#include "SBAsyncDataLoad/SBRequestData.h"
#include "SBTexture/SBTexture.h"
#include <memory>

namespace SB
{
	class EventManager;
	class Shader;
	class Node;
	class SceneRenderer;
	class Camera;
	class MemoryFile;
	class DynamicLighteningProperties;
}

class CityManager
{
public:
	void Init();
	void Draw(SB::Camera* camera, float time);

	void ConstructCityFromDAE();
	void MakeCityBatches();
	void MakeOcean();
	void LoadCityBatches();
	void LoadOcean(const SB::IFile* memfile);

	void PushCityBatch(const SB::IFile* file);

	void SetShader(SB::Shader* shader);

	void SetSunDirection(const glm::vec3& v);
	void SetSunLuminance(const glm::vec3& v);
	void SetSkyLuminance(const glm::vec3& v);

	void SetDynamicLightening(SB::DynamicLighteningProperties* dl);
private:
	std::unique_ptr<SB::SceneRenderer> m_sceneRenderer;
	SB::SceneRenderer::RenderList m_renderlistCity;
	SB::SceneRenderer::RenderList m_renderlistOcean;

	SB::RequestPull m_rpull;

	SB::Shader* m_terrainShader;
	SB::Shader* m_waterShader;

	glm::vec3 skyLuminance;
	glm::vec3 sunLuminance;
	glm::vec3 sunDirection;

	std::map<std::string, SB::TexturePtr> m_textures;

	SB::DynamicLighteningProperties* m_dynamicLightening;
};