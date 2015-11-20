#include "Appication.h"

#include "SBShader/SBShader.h"
#include "SBScene/SBSceneDAEConstructor.h"
#include "SBScene/SBMeshBufferConstructor.h"
#include "SBScene/SBMesh.h"
#include "SBScene/SBNode.h"
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

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

SB::SceneRenderer sr;

SB::Camera camera;
CameraFreeFlightController cameraController;
SB::SceneRenderer::RenderList renderlist;

void constructCity()
{
	SB::Node* city = new SB::Node;

	const char* cityParts[] = {

		"casino_backstreet.dae",
		"strip01.dae",
		"rich_residential.dae",
		"airport.dae",
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
		sc.OpenDAE((std::string("city_source/") + cityParts[i]).c_str());
		SB::Node* scene = sc.ConstructSBScene();
		SB::Utils::RemoveNodes(scene, "additif_*");
		SB::Utils::RemoveNodes(scene, "bush_*");
		SB::Utils::RemoveNodes(scene, "tree_*");
		SB::Utils::RemoveNodes(scene, "cactus_*");
		SB::Utils::RemoveNodes(scene, "*__b0_1*");
		SB::Utils::RemoveNodes(scene, "*__p0_1*");
		SB::Utils::RemoveNodes(scene, "*__t0_1*");
		SB::Utils::RemoveNodes(scene, "road_lines_*");
		SB::Utils::Merge(city, scene);
	}
		
	{
		SB::CFile file;
		SB::Serializer serializer;
		file.Open("data/city.bdae", SB::IFile::FILE_WRITE);
		serializer.SerializeScene(city, &file);
	}

	SB::Utils::PushMeshDataToVideoMemory(city, true);
	renderlist = sr.RegisterNodes(city);
}

int Appication::Init()
{
	m_fontSize = 15;
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
	m_fontSize = m_fontSize * devicePixelRatio;
#endif

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

	SB::CFile fileTerrainShaderV("data/shaders/terrain.vs", SB::IFile::FILE_READ);
	SB::CFile fileTerrainShaderF("data/shaders/terrain.fs", SB::IFile::FILE_READ);
	m_terrainShader = new SB::Shader;
	m_terrainShader->CreateProgramFrom("terrain", &fileTerrainShaderV, &fileTerrainShaderF);

	SB::CFile fileSunShaderV("data/shaders/sun.vs", SB::IFile::FILE_READ);
	SB::CFile fileSunShaderF("data/shaders/sun.fs", SB::IFile::FILE_READ);
	m_sunShader = new SB::Shader;
	m_sunShader->CreateProgramFrom("terrain", &fileSunShaderV, &fileSunShaderF);

	//constructCity();

	/*
	{
		SB::SceneDAEConstructor sc;
		sc.OpenDAE("data/untitled.dae");
		m_rootScene = sc.ConstructSBScene();
		SB::CFile file;
		SB::SceneSerializer serializer;
		file.Open("data/untitled.bdae", SB::IFile::FILE_WRITE);
		serializer.Serialize(m_rootScene, &file);
	}*/
	{
		SB::CFile file;
		SB::Serializer serializer;
		file.Open("data/untitled.bdae", SB::IFile::FILE_READ);
		m_rootScene = serializer.DeSerializeScene(&file);
	}
	SB::Utils::PushMeshDataToVideoMemory(m_rootScene, true);

	{
		/*
		SB::CFile file;
		SB::Serializer serializer;
		file.Open("city_source/city.bdae", SB::IFile::FILE_READ);
		SB::Node* city = serializer.DeSerializeScene(&file);

		std::vector<SB::Mesh*> meshList;
		SB::Utils::MakeFlat(meshList, city);
		std::vector<SB::Mesh*> batchedMeshList;
		SB::Utils::BatchMeshes(meshList, batchedMeshList);

		SB::CFile fileBatch;
		fileBatch.Open("data/city_batch.bdae", SB::IFile::FILE_WRITE);
		SB::Serializer se;
		se.SerializeBatchList(batchedMeshList, &fileBatch);
		*/
		
		SB::CFile fileBatch;
		fileBatch.Open("data/city_batch.bdae", SB::IFile::FILE_READ);
		SB::Serializer se;
		std::vector<SB::Mesh*> batchedMeshList;
		se.DeSerializeBatchList(batchedMeshList, &fileBatch);
		
		
		SB::Utils::PushMeshDataToVideoMemory(batchedMeshList);

		for (int i = 0; i < batchedMeshList.size(); ++i)
		{
			glm::mat4 transform(1.0f);
			renderlist.push_back(SB::SceneRenderer::Entity(transform, batchedMeshList[i]));
		}
		//SB::Utils::PushMeshDataToVideoMemory(city, true);
		//renderlist = sr.RegisterNodes(city);
	}

	m_sun = m_rootScene->GetNodeByName("Sun");
	m_sun->DetachNode();

	SB::CFile dynamicLighteningXML("data/ProceduralSky.xml", SB::IFile::FILE_READ);
	m_dynamicLightening.Load(&dynamicLighteningXML);

	glm::vec3 lookAt(18146.2715f, 49058.6328f, 23362.9414f);
	glm::vec3 position(18146.1348f, 49059.5195f, 23363.3848f);
	glm::vec3 upVector(0, 0, 1);

	camera.SetFOV(60.0f / 180.0f*3.14f);
	//camera.SetNearFarPlanes(1.0f, 10000.0f);
	camera.SetNearFarPlanes(100.0f, 3000.0f * 100.0f);
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
	bool blockDrag = ImGui::IsMouseHoveringAnyWindow();
	ImGui::End();
	
	m_sunController.Update(m_time);

	glViewport(0, 0, screenBufferSizes.m_windowWidth, screenBufferSizes.m_windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LESS);
	glDepthMask(true);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
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
	sr.Render(renderlist, &camera, m_terrainShader);

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

