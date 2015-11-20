#include "SBImGuiBinding.h"
#include "SBCFile.h"
#include "SBShader/SBShader.h"
#include "SBOpenGLHeaders.h"
#include "SBBasicEvents.h"

#include <imgui.h>
#include <GLFW/glfw3.h>

using namespace SB;

void ImGuiBinding::Init()
{
	m_shader = new SB::Shader;
	SB::CFile fileShaderV("data/shaders/ImGUI.vs", SB::IFile::FILE_READ);
	SB::CFile fileShaderF("data/shaders/ImGUI.fs", SB::IFile::FILE_READ);
	m_shader->CreateProgramFrom("ImGuiShader", &fileShaderV, &fileShaderF);

	u_texture = m_shader->GetUniformID("u_texture");
	u_projMtx = m_shader->GetUniformID("u_projMtx");

	glGenBuffers(2, m_vboHandles);

	ImGuiIO& io = ImGui::GetIO();

	// Build texture atlas
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.
	

	// Create OpenGL texture
	glGenTextures(1, &m_fontTexture);
	glBindTexture(GL_TEXTURE_2D, m_fontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (void *)(intptr_t)m_fontTexture;

	// Cleanup (don't clear the input data if you want to append new fonts later)
	io.Fonts->ClearInputData();
	io.Fonts->ClearTexData();

	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;                         // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
}

void ImGuiBinding::NewFrame(const ScreenBufferSizes& screenBufferSizes)
{
	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	io.DisplaySize = ImVec2((float)screenBufferSizes.m_windowWidth, (float)screenBufferSizes.m_windowHeight);

	io.DisplayFramebufferScale = ImVec2((float)screenBufferSizes.m_framebufferWidth / screenBufferSizes.m_windowWidth,
		(float)screenBufferSizes.m_framebufferHeight / screenBufferSizes.m_windowHeight);

	ImGui::NewFrame();
}

void ImGuiBinding::ShutDown()
{

}

void ImGuiBinding::Render()
{
	ImGui::Render();
	ImDrawData* draw_data = ImGui::GetDrawData();

	// Backup GL state
	GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	//GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
	//GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
	GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
	GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	// Handle cases of screen coordinates != from framebuffer coordinates (e.g. retina displays)
	ImGuiIO& io = ImGui::GetIO();
	float fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
	draw_data->ScaleClipRects(io.DisplayFramebufferScale);

	// Setup viewport, orthographic projection matrix
	glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
	const float ortho_projection[4][4] =
	{
		{ 2.0f / io.DisplaySize.x, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
		{ 0.0f, 0.0f, -1.0f, 0.0f },
		{ -1.0f, 1.0f, 0.0f, 1.0f },
	};
	
	m_shader->UseIt();

	glUniform1i(m_shader->GetUniformByID(u_texture), 0);
	glUniformMatrix4fv(m_shader->GetUniformByID(u_projMtx), 1, GL_FALSE, &ortho_projection[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboHandles[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboHandles[1]);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
	glEnableVertexAttribArray((int)m_shader->positionAttribute);
	glEnableVertexAttribArray((int)m_shader->colorAttribute);
	glEnableVertexAttribArray((int)m_shader->uvAttribute);
	glVertexAttribPointer((int)m_shader->positionAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
	glVertexAttribPointer((int)m_shader->uvAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
	glVertexAttribPointer((int)m_shader->colorAttribute, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = 0;

		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

		for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
		{
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
				glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}

	glDisableVertexAttribArray((int)m_shader->positionAttribute);
	glDisableVertexAttribArray((int)m_shader->colorAttribute);
	glDisableVertexAttribArray((int)m_shader->uvAttribute);

	// Restore modified GL state
	glUseProgram(last_program);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
	glBindVertexArray(last_vertex_array);
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	//glBlendFunc(last_blend_src, last_blend_dst);
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
}

void ImGuiBinding::EventReceiver_OnEvent(const BasicEvents::OnMouseButtonEvent& mouseButtonEvent)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[mouseButtonEvent.button] = mouseButtonEvent.action == BasicEvents::ACTION_PRESS;
}

void ImGuiBinding::EventReceiver_OnEvent(const BasicEvents::OnMouseMoveEvent& mouseButtonEvent)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(mouseButtonEvent.x, mouseButtonEvent.y);
}

void ImGuiBinding::EventReceiver_OnEvent(const BasicEvents::OnKeyEvent& keyEvent)
{
	ImGuiIO& io = ImGui::GetIO();
	if (keyEvent.action == BasicEvents::ACTION_PRESS)
		io.KeysDown[keyEvent.rawKey] = true;
	if (keyEvent.action == BasicEvents::ACTION_RELEASE)
		io.KeysDown[keyEvent.rawKey] = false;

	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
}