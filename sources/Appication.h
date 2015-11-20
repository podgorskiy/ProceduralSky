#pragma once
#include "SBDynamicData/SBDynamicData.h"
#include "Events.h"
#include "ProceduralSky.h"
#include "SunController.h"

struct ScreenBufferSizes
{
	int m_framebufferWidth;
	int m_framebufferHeight;
	int m_windowWidth;
	int m_windowHeight;
};

namespace SB
{
	class EventManager;
	class Shader;
	class Node;
	class SceneRenderer;
	class Camera;
}

class ImGuiBinding;
class CameraFreeFlightController;

class Appication
{
public:
	
	int Init();
	void Update(const ScreenBufferSizes& screenBufferSizes, float deltaTime);
	SB::EventManager* GetEventManager();

	void OnMousePressed(const Event::OnMouseButtonEvent& mouseEvent);
	void OnMouseMove(const Event::OnMouseMoveEvent& mouseEvent);
	void OnKeyPressed(const Event::OnKeyEvent& keyEvent);

	void DrawGUI();

	void SetUpScale();
private:
	float m_time;
	SB::Node* m_rootScene;
	SB::Node* m_sun;

	SB::SceneRenderer* m_sceneRenderer;
	SB::Camera* m_camera;
	CameraFreeFlightController* m_cameraController;

	SB::EventManager* m_eventManager;
	ImGuiBinding* m_imGuiBinding;
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