#include "SBOpenGLHeaders.h"
#include "SBPVRReader.h"
#include "SBTexture.h"

using namespace SB;

TexturePtr Texture::GenerateTexture()
{
	Texture* t = new Texture;
	t->Bind(0);
	t->UnBind();
	return TexturePtr(t);
}

Texture::Texture() :m_textureHandle(-1)
{
	glGenTextures(1, &m_textureHandle);
}


void Texture::Bind(int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_textureHandle);
}

void Texture::UnBind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
	if (m_textureHandle != -1){
		glDeleteTextures(1, &m_textureHandle);
		m_textureHandle = -1;
	}
}
