#pragma once
#include "SBEventManager.h"
#include "SBBasicEvents.h"

namespace SB
{
	struct ScreenBufferSizes
	{
		int m_framebufferWidth;
		int m_framebufferHeight;
		int m_windowWidth;
		int m_windowHeight;
	};

	class Shader;

	class ImGuiBinding :
		public SB::EventReceiver<BasicEvents::OnMouseButtonEvent>,
		public SB::EventReceiver<BasicEvents::OnMouseMoveEvent>,
		public SB::EventReceiver<BasicEvents::OnKeyEvent>
	{
	public:
		void Init();
		void NewFrame(const ScreenBufferSizes& screenBufferSizes);
		void ShutDown();
		void Render();

		void EventReceiver_OnEvent(const BasicEvents::OnMouseButtonEvent& mouseButtonEvent);
		void EventReceiver_OnEvent(const BasicEvents::OnMouseMoveEvent& mouseButtonEvent);
		void EventReceiver_OnEvent(const BasicEvents::OnKeyEvent& keyEvent);

	private:
		SB::Shader* m_shader;
		int u_texture;
		int u_projMtx;
		unsigned int m_vboHandles[2];
		unsigned int m_fontTexture;
	};
}