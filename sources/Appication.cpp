#include "Appication.h"

#include "SBShader/SBShader.h"
#include "SBScene/SBSceneDAEConstructor.h"
#include "SBScene/SBMeshBufferConstructor.h"
#include "SBScene/SBScene.h"
#include "SBScene/SBMesh.h"
#include "SBScene/SBSceneRenderer.h"
#include "SBScene/SBSceneSerializer.h"
#include "SBScene/SBSceneUtils.h" 
#include "SBCFile.h"
#include "SBCamera.h"
#include "SBColorUtils.h"
#include "SBEventManager.h"
#include "SBTimer/SBScopeTinyProfiler.h"
#include "SBOpenGLHeaders.h"

#include "ImGuiBinding.h"
#include "CameraFreeFlightController.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <iostream>
#include <cstdlib>
#include <vector>

SB::SceneRenderer sr;

SB::Camera camera;
CameraFreeFlightController cameraController;

SB::Scene* city;
SB::SceneRenderer::RenderList cityRenderList;

void constructCity()
{
	SB::Scene* city = new SB::Scene;

	const char* cityParts[] = {
		
		"strip01.dae",
		"rich_residential.dae",
		"airport.dae",
		"casino_backstreet.dae",
		"construction.dae",
		"desert00.dae",
		"desert01.dae",
		"desert02.dae",
		"desert03.dae",
		"downtown.dae",
		"government.dae",
		"industrial.dae",
		"poor_residential.dae",
		"stadium.dae",
		"strip00.dae",
		"strip02.dae",
		"strip03.dae"
	};


	for (int i = 0; i < sizeof(cityParts) / sizeof(cityParts[0]); i++)
	{
		SB::SceneDAEConstructor sc;
		sc.OpenDAE(cityParts[i]);
		SB::Scene* scene = sc.ConstructSBScene();
		SB::Merge(city, scene);
	}

	SB::RemoveNodes(city, "additif_*");
	SB::RemoveNodes(city, "bush_*");
	SB::RemoveNodes(city, "tree_*");
	SB::RemoveNodes(city, "cactus_*");
	SB::RemoveNodes(city, "*__b0_1*");
	SB::RemoveNodes(city, "*__p0_1*");
	SB::RemoveNodes(city, "*__t0_1*");

	{
		SB::CFile file;
		SB::SceneSerializer serializer;
		file.Open("city.bdae", SB::IFile::FILE_WRITE);
		serializer.Serialize(city, &file);
	}
}

int Appication::Init()
{
	if (gl3wInit())
	{
		std::cerr << "failed to initialize OpenGL";
		return EXIT_FAILURE;
	}

	if (!gl3wIsSupported(3, 2)) {
		std::cerr << "OpenGL 3.2 not supported";
		return EXIT_FAILURE;
	}

	std::cout << "OpenGL " << glGetString(GL_VERSION)
		<< " GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LESS);
	glDepthMask(true);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SB::CFile fileTerrainShaderV("shaders/terrain.vs", SB::IFile::FILE_READ);
	SB::CFile fileTerrainShaderF("shaders/terrain.fs", SB::IFile::FILE_READ);
	m_terrainShader = new SB::Shader;
	m_terrainShader->CreateProgramFrom("terrain", &fileTerrainShaderV, &fileTerrainShaderF);

	SB::CFile fileSunShaderV("shaders/sun.vs", SB::IFile::FILE_READ);
	SB::CFile fileSunShaderF("shaders/sun.fs", SB::IFile::FILE_READ);
	m_sunShader = new SB::Shader;
	m_sunShader->CreateProgramFrom("terrain", &fileSunShaderV, &fileSunShaderF);
	
	SB::SceneDAEConstructor sc;
	sc.OpenDAE("untitled.dae");
	m_rootScene = sc.ConstructSBScene();
	
	SB::PushMeshDataToVideoMemory(m_rootScene, true);

	m_sun = &m_rootScene->GetNodeByName("Sun");
	m_sun->DetachNode();

	//constructCity();
	{
		SB::CFile file;
		SB::SceneSerializer serializer;
		file.Open("city.bdae", SB::IFile::FILE_READ);
		city = serializer.DeSerialize(&file);

		SB::PushMeshDataToVideoMemory(city, true);
		cityRenderList = sr.RegisterNodes(city);
	}
	

	SB::CFile dynamicLighteningXML("ProceduralSky.xml", SB::IFile::FILE_READ);
	m_dynamicLightening.Load(&dynamicLighteningXML);

	glm::vec3 lookAt(0.0f, 0.0f, 0.0f);
	glm::vec3 position(10.0f, 10.0f, 10.0f);
	glm::vec3 upVector(0, 0, 1);

	camera.SetFOV(60.0f / 180.0f*3.14f);
	//camera.SetNearFarPlanes(1.0f, 10000.0f);
	camera.SetNearFarPlanes(100.0f, 300000.0f);
	camera.SetPosition(position);
	camera.SetUpVector(upVector);
	camera.SetLookAtPoint(lookAt);

	m_eventManager = new SB::EventManager;
	m_imGuiBinding = new ImGuiBinding;

	m_imGuiBinding->Init();

	m_eventManager->AttachReceiver<Event::OnMouseButtonEvent>(m_imGuiBinding);
	m_eventManager->AttachReceiver<Event::OnMouseMoveEvent>(m_imGuiBinding);
	m_eventManager->AttachReceiver<Event::OnKeyEvent>(m_imGuiBinding);

	m_eventManager->AttachReceiver<Event::OnMouseButtonEvent>(&cameraController);
	m_eventManager->AttachReceiver<Event::OnMouseMoveEvent>(&cameraController);
	m_eventManager->AttachReceiver<Event::OnKeyEvent>(&cameraController);
	
	cameraController.AttachCamera(&camera);
	cameraController.SetSpeed(1000.5f);

	m_proceduralSky.Init(32, 32);
	m_proceduralSky.SetSkyDirection(glm::vec3(0.0f, 0.0f, 1.0f));
	m_sunController.SetUpVector(glm::vec3(0.0f, 0.0f, 1.0f));
	m_sunController.SetMonth(SunController::June);
	m_time = 9.0f;

	return EXIT_SUCCESS;
}

void Appication::Update(const ScreenBufferSizes& screenBufferSizes, float deltaTime)
{
	m_imGuiBinding->NewFrame(screenBufferSizes);
	
	ImGui::Begin("SomeWindow");
	
	ImGui::SliderFloat("Time", &m_time, 4.0f, 22.0f);
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
	bool blockDrag = ImGui::IsMouseHoveringAnyWindow();
	ImGui::End();

	m_sunController.Update(m_time);

	glViewport(0, 0, screenBufferSizes.m_windowWidth, screenBufferSizes.m_windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	camera.SetRatio(static_cast<float>(screenBufferSizes.m_windowWidth) / screenBufferSizes.m_windowHeight);

	cameraController.Update(deltaTime, blockDrag);
	camera.Update();

	SB::DynamicLighteningProperties::ValueID expositionID = m_dynamicLightening.GetValueID("AuxiliaryParameters.Exposition");
	float exposition = m_dynamicLightening.GetValueByID<float>(expositionID, m_time);

	SB::DynamicLighteningProperties::ValueID SkyLuminanceXYZID = m_dynamicLightening.GetValueID("Environment.SkyLuminanceXYZ");
	glm::vec3 skyLuminanceXYZ = m_dynamicLightening.GetValueByID<glm::vec3>(SkyLuminanceXYZID, m_time);
	glm::vec3 skyLuminanceRGB = SB::ColorUtils::XYZToRGB(skyLuminanceXYZ) / exposition;

	SB::DynamicLighteningProperties::ValueID SunLuminanceXYZID = m_dynamicLightening.GetValueID("Environment.SunLuminanceXYZ");
	glm::vec3 sunLuminanceXYZ = m_dynamicLightening.GetValueByID<glm::vec3>(SunLuminanceXYZID, m_time);
	glm::vec3 sunLuminanceRGB = SB::ColorUtils::XYZToRGB(sunLuminanceXYZ) / exposition;
	
	glm::vec3 sunDirection = m_sunController.GetSunDirection();
	m_proceduralSky.SetSkyLuminanceXYZ(skyLuminanceXYZ);
	m_proceduralSky.SetSunDirection(sunDirection);
	m_proceduralSky.SetExposition(exposition);

	glm::mat4 suntransform;
	suntransform = glm::translate(suntransform, camera.GetPosition() + sunDirection * 200.0f);
	m_sun->SetLocalTransform(suntransform);
	sr.RegisterNodes(m_sun);
	sr.Render(&camera, m_sunShader);

	m_terrainShader->UseIt();
	m_terrainShader->GetUniform("u_sunDirection").SetValue(sunDirection);
	m_terrainShader->GetUniform("u_sunLuminance").SetValue(sunLuminanceRGB);
	m_terrainShader->GetUniform("u_skyLuminance").SetValue(skyLuminanceRGB);

	sr.RegisterNodes(m_rootScene);
	sr.Render(&camera, m_terrainShader);


	{
		SBProfile(Render);
		sr.Render(cityRenderList, &camera, m_terrainShader);
	}

	m_proceduralSky.Draw(&camera);

	m_imGuiBinding->Render();
}

SB::EventManager* Appication::GetEventManager()
{
	return m_eventManager;
}

void Appication::OnMousePressed(const Event::OnMouseButtonEvent& mouseEvent)
{
	GetEventManager()->Dispatch(mouseEvent);
}

void Appication::OnMouseMove(const Event::OnMouseMoveEvent& mouseEvent)
{
	GetEventManager()->Dispatch(mouseEvent);
}

void Appication::OnKeyPressed(const Event::OnKeyEvent& keyEvent)
{
	GetEventManager()->Dispatch(keyEvent);
}

