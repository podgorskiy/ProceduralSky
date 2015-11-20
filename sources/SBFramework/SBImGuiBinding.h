#pragma once
#include "SBEventManager.h"
#include "Events.h"

namespace SB
{
	class Shader;
}

struct ScreenBufferSizes;

class ImGuiBinding :
	public SB::EventReceiver<Event::OnMouseButtonEvent>,
	public SB::EventReceiver<Event::OnMouseMoveEvent>,
	public SB::EventReceiver<Event::OnKeyEvent>
{
public:
	void Init();
	void NewFrame(const ScreenBufferSizes& screenBufferSizes);
	void ShutDown();
	void Render();

	void EventReceiver_OnEvent(const Event::OnMouseButtonEvent& mouseButtonEvent);
	void EventReceiver_OnEvent(const Event::OnMouseMoveEvent& mouseButtonEvent);
	void EventReceiver_OnEvent(const Event::OnKeyEvent& keyEvent);

private:
	SB::Shader* m_shader;
	int u_texture;
	int u_projMtx;
	unsigned int m_vboHandles[2];
	unsigned int m_fontTexture;
};
