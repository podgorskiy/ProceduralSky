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
}

class ImGuiBinding;

class Appication
{
public:
	
	int Init();
	void Update(const ScreenBufferSizes& screenBufferSizes, float deltaTime);
	SB::EventManager* GetEventManager();

	void OnMousePressed(const Event::OnMouseButtonEvent& mouseEvent);
	void OnMouseMove(const Event::OnMouseMoveEvent& mouseEvent);
	void OnKeyPressed(const Event::OnKeyEvent& keyEvent);

private:
	float m_time;
	SB::Node* m_rootScene;
	SB::Node* m_sun;

	SB::EventManager* m_eventManager;
	ImGuiBinding* m_imGuiBinding;
	SB::Shader* m_terrainShader;
	SB::Shader* m_sunShader;
	ProceduralSky m_proceduralSky;
	SunController m_sunController;
	SB::DynamicLighteningProperties m_dynamicLightening;
	float m_fontSize;
	float m_scale;
};