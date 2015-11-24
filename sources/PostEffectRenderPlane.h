#pragma once
#include <vector>
#include <glm/matrix.hpp>

namespace SB
{
	class Shader;
	class Camera;
}

class PostEffectRenderPlane
{
	struct Vertex
	{
		float x, y;
	};

public:
	PostEffectRenderPlane();
	~PostEffectRenderPlane();

	void Init(SB::Shader* shader);
	void Draw();

private:

	unsigned int m_vboHandles[2];
	int m_indicesCount;
	SB::Shader* m_shader;
};