#pragma once
#include "SBDynamicData/SBDynamicData.h"
#include "SBBasicEvents.h"
#include "ProceduralSky.h"
#include "SunController.h"

namespace SB
{
	class EventManager;
	class Shader;
	class Node;
	class SceneRenderer;
	class Camera;
	class ImGuiBinding;
	class CameraFreeFlightController;
	struct ScreenBufferSizes;
}

class Appication
{
public:
	
	int Init();
	void Update(const SB::ScreenBufferSizes& screenBufferSizes, float deltaTime);
	SB::EventManager* GetEventManager();

	void OnMousePressed(const SB::BasicEvents::OnMouseButtonEvent& mouseEvent);
	void OnMouseMove(const SB::BasicEvents::OnMouseMoveEvent& mouseEvent);
	void OnKeyPressed(const SB::BasicEvents::OnKeyEvent& keyEvent);

	void DrawGUI();

	void SetUpScale();
private:
	float m_time;
	SB::Node* m_rootScene;
	SB::Node* m_sun;

	SB::SceneRenderer* m_sceneRenderer;
	SB::Camera* m_camera;
	SB::CameraFreeFlightController* m_cameraController;

	SB::EventManager* m_eventManager;
	SB::ImGuiBinding* m_imGuiBinding;
	SB::Shader* m_terrainShader;
	SB::Shader* m_sunShader;
	ProceduralSky m_proceduralSky;
	SunController m_sunController;
	SB::DynamicLighteningProperties m_dynamicLightening;

	SB::DynamicLighteningProperties::ValueID m_expositionID;
	SB::DynamicLighteningProperties::ValueID m_skyLuminanceXYZID;
	SB::DynamicLighteningProperties::ValueID m_sunLuminanceXYZID;

	bool m_touchWasConsumedByImGUI;

	float m_fontSize;
	float m_scale;
};