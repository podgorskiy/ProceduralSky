#include "Application.h"

#include "SBCommon.h"
#include "SBShader/SBShader.h"
#include "SBScene/SBSceneDAEConstructor.h"
#include "SBScene/SBMeshBufferConstructor.h"
#include "SBScene/SBMesh.h"
#include "SBScene/SBNode.h"
#include "SBScene/SBSceneRenderer.h"
#include "SBScene/SBSceneSerializer.h"
#include "SBScene/SBSceneUtils.h" 
#include "SBFileSystem/SBCFile.h"
#include "SBAsyncDataLoad/SBRequest.h"
#include "SBAsyncDataLoad/SBRequestTexture.h"
#include "SBCamera.h"
#include "SBColorUtils.h"
#include "SBEventManager.h"
#include "SBTimer/SBScopeTinyProfiler.h"
#include "SBOpenGLHeaders.h"
#include "SBImGuiBinding.h"
#include "SBCameraFreeFlightController.h"

#include <simpletext.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <iostream>
#include <cstdlib>
#include <vector>


int Appication::Init()
{
	gl3wInit();

	std::cout << "OpenGL " << glGetString(GL_VERSION)
		<< " GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);

	SB::CFile fileTerrainShaderV("data/shaders/terrain.vs", SB::IFile::FILE_READ);
	SB::CFile fileTerrainShaderF("data/shaders/terrain.fs", SB::IFile::FILE_READ);
	m_terrainShader = new SB::Shader;
	m_terrainShader->CreateProgramFrom("terrain", &fileTerrainShaderV, &fileTerrainShaderF);

	SB::CFile fileSunShaderV("data/shaders/sun.vs", SB::IFile::FILE_READ);
	SB::CFile fileSunShaderF("data/shaders/sun.fs", SB::IFile::FILE_READ);
	m_sunShader = new SB::Shader;
	m_sunShader->CreateProgramFrom("terrain", &fileSunShaderV, &fileSunShaderF);

	SB::SceneDAEConstructor sc;
	sc.OpenDAE("data/test_scene.dae");
	m_rootScene = sc.ConstructSBScene();
	
	SB::Utils::PushMeshDataToVideoMemory(m_rootScene, true);

	m_sun = m_rootScene->GetNodeByName("Sun");
	m_sun->DetachNode();

	SB::CFile dynamicLighteningXML("data/ProceduralSky.xml", SB::IFile::FILE_READ);
	m_dynamicLightening.Load(&dynamicLighteningXML);

	m_expositionID = m_dynamicLightening.GetValueID("AuxiliaryParameters.Exposition");
	m_skyLuminanceXYZID = m_dynamicLightening.GetValueID("Environment.SkyLuminanceXYZ");
	m_sunLuminanceXYZID = m_dynamicLightening.GetValueID("Environment.SunLuminanceXYZ");

	glm::vec3 lookAt(-14.0f, -45.0f, 12.2f);
	glm::vec3 position(-15.0f, -50.0f, 12.0f);
	glm::vec3 upVector(0.0f, 0.0f, 1.0f);

	m_camera = new SB::Camera;
	m_camera->SetFOV(60.0f / 180.0f*3.14f);
	m_camera->SetNearFarPlanes(1.0f, 1000.0f);
	
	m_camera->SetPosition(position);
	m_camera->SetUpVector(upVector);
	m_camera->SetLookAtPoint(lookAt);

	m_eventManager = new SB::EventManager;
	m_imGuiBinding = new SB::ImGuiBinding;

	SetUpScale();
	m_imGuiBinding->Init();

	m_eventManager->AttachReceiver<SB::BasicEvents::OnMouseButtonEvent>(m_imGuiBinding);
	m_eventManager->AttachReceiver<SB::BasicEvents::OnMouseMoveEvent>(m_imGuiBinding);
	m_eventManager->AttachReceiver<SB::BasicEvents::OnKeyEvent>(m_imGuiBinding);

	m_cameraController = new SB::CameraFreeFlightController;
	m_cameraController->AttachCamera(m_camera);
	m_cameraController->SetSpeed(0.5f);

	m_eventManager->AttachReceiver<SB::BasicEvents::OnMouseButtonEvent>(m_cameraController);
	m_eventManager->AttachReceiver<SB::BasicEvents::OnMouseMoveEvent>(m_cameraController);
	m_eventManager->AttachReceiver<SB::BasicEvents::OnKeyEvent>(m_cameraController);
	
	m_proceduralSky.Init(32, 32);
	m_proceduralSky.SetSkyDirection(glm::vec3(0.0f, 0.0f, 1.0f));
	m_sunController.SetUpVector(glm::vec3(0.0f, 0.0f, 1.0f));
	m_sunController.SetMonth(SunController::June);
	m_time = 6.0f;

	m_rpull.SetUrlPrefix("async_data/");
	
	const unsigned char* extensions = glGetString(GL_EXTENSIONS);

	bool DXT1_extension =	SB::CheckExtension(extensions, "GL_EXT_texture_compression_s3tc") || 
							SB::CheckExtension(extensions, "WEBGL_compressed_texture_s3tc");

	bool PVRTC_extension =	SB::CheckExtension(extensions, "GL_IMG_texture_compression_pvrtc") || 
							SB::CheckExtension(extensions, "WEBGL_compressed_texture_pvrtc");

	if (DXT1_extension)
	{
		m_lightmap = m_rpull.CreateRequest<SB::RequestTexture>("BC1/lightmap.pvr", false)->GetTexture();
	}
	else if (PVRTC_extension)
	{
		m_lightmap = m_rpull.CreateRequest<SB::RequestTexture>("PVRTC/lightmap.pvr", false)->GetTexture();
	}
	
	m_sceneRenderer = new SB::SceneRenderer;
	return EXIT_SUCCESS;
}

void Appication::Update(const SB::ScreenBufferSizes& screenBufferSizes, float deltaTime)
{
	m_imGuiBinding->NewFrame(screenBufferSizes);
	DrawGUI();
	
	m_sunController.Update(m_time);

	glViewport(0, 0, screenBufferSizes.m_windowWidth, screenBufferSizes.m_windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthFunc(GL_LESS);
	glDepthMask(true);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	m_camera->SetRatio(static_cast<float>(screenBufferSizes.m_windowWidth) / screenBufferSizes.m_windowHeight);

	m_cameraController->Update(deltaTime, m_touchWasConsumedByImGUI);
	m_camera->Update();

	float exposition = m_dynamicLightening.GetValueByID<float>(m_expositionID, m_time);

	glm::vec3 skyLuminanceXYZ = m_dynamicLightening.GetValueByID<glm::vec3>(m_skyLuminanceXYZID, m_time);
	glm::vec3 skyLuminanceRGB = SB::ColorUtils::XYZToRGB(skyLuminanceXYZ) / exposition;

	glm::vec3 sunLuminanceXYZ = m_dynamicLightening.GetValueByID<glm::vec3>(m_sunLuminanceXYZID, m_time);
	glm::vec3 sunLuminanceRGB = SB::ColorUtils::XYZToRGB(sunLuminanceXYZ) / exposition;
	
	glm::vec3 sunDirection = m_sunController.GetSunDirection();
	m_proceduralSky.SetSkyLuminanceXYZ(skyLuminanceXYZ);
	m_proceduralSky.SetSunDirection(sunDirection);
	m_proceduralSky.SetExposition(exposition);

	glm::mat4 suntransform;
	suntransform = glm::translate(suntransform, m_camera->GetPosition() + sunDirection * 200.0f);
	m_sun->SetLocalTransform(suntransform);
	m_sceneRenderer->RegisterNodes(m_sun);
	m_sceneRenderer->Render(m_camera, m_sunShader);

	m_terrainShader->UseIt();
	m_terrainShader->GetUniform("u_lightmap").SetValue(0);
	m_lightmap->Bind(0);
	m_terrainShader->GetUniform("u_sunDirection").SetValue(sunDirection);
	m_terrainShader->GetUniform("u_sunLuminance").SetValue(sunLuminanceRGB);
	m_terrainShader->GetUniform("u_skyLuminance").SetValue(skyLuminanceRGB);

	m_sceneRenderer->RegisterNodes(m_rootScene);
	m_sceneRenderer->Render(m_camera, m_terrainShader);
	
	m_proceduralSky.Draw(m_camera);

	m_imGuiBinding->Render();

	m_rpull.Update();
}

void Appication::DrawGUI()
{		 
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScrollbarSize = 30 * m_scale;
	style.FramePadding = ImVec2(10 * m_scale, 5 * m_scale);
	style.ItemInnerSpacing = ImVec2(10 * m_scale, 5 * m_scale);

	ImGui::Begin("ProceduralSky");
	ImGui::SetWindowSize(ImVec2(350 * m_scale, 120 * m_scale));
	ImGui::SliderFloat("Time", &m_time, 5.0f, 22.0f);
	float latitude = m_sunController.GetLatitude();
	ImGui::SliderFloat("Latitude ", &latitude, -90.0f, 90.0f);
	m_sunController.SetLatitude(latitude);

	const char* items[] = {
		"January",
		"February",
		"March",
		"April",
		"May",
		"June",
		"July",
		"August",
		"September",
		"October",
		"November",
		"December"
	};
	int item = m_sunController.GetMonth();
	ImGui::Combo("Month", &item, items, 12);
	m_sunController.SetMonth(static_cast<SunController::Month>(item));

	m_touchWasConsumedByImGUI = ImGui::IsMouseHoveringAnyWindow();
	
	ImGui::End();
}

void Appication::SetUpScale()
{
	m_scale = 1;

#ifdef __EMSCRIPTEN__
	float devicePixelRatio = EM_ASM_DOUBLE_V(
	{
		if (window.devicePixelRatio != undefined)
		{
			return window.devicePixelRatio;
		}
		else
		{
			return 1.0;
		}
	}
	);
	m_scale = devicePixelRatio;
#endif
}

SB::EventManager* Appication::GetEventManager()
{
	return m_eventManager;
}

void Appication::OnMousePressed(const SB::BasicEvents::OnMouseButtonEvent& mouseEvent)
{
	GetEventManager()->Dispatch(mouseEvent);
}

void Appication::OnMouseMove(const SB::BasicEvents::OnMouseMoveEvent& mouseEvent)
{
	GetEventManager()->Dispatch(mouseEvent);
}

void Appication::OnKeyPressed(const SB::BasicEvents::OnKeyEvent& keyEvent)
{
	GetEventManager()->Dispatch(keyEvent);
}

