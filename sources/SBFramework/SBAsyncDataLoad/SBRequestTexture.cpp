#include "SBFileSystem/SBMemoryFile.h"
#include "SBFileSystem/SBMemoryFileNoCopy.h"
#include "SBRequest.h"
#include "SBRequestData.h"
#include "SBRequestTexture.h"
#include "SBTexture/SBPVRReader.h"
#include <string>

using namespace SB;

RequestTexture::RequestTexture(const std::string& URL, bool copyData, RequestPull* requestPull) : Request(URL, false, requestPull)
{
	m_texture = SB::Texture::GenerateTexture();
};

void RequestTexture::OnLoad(const void* buffer, int size)
{
	SB::MemoryFileNoCopy memoryFile = MemoryFileNoCopy(static_cast<const char*>(buffer), size);
	SB::PVRReader reader;
	reader.DeSerializeTexture(m_texture.get(), &memoryFile);
	Request::OnLoad(buffer, size);
}

SB::TexturePtr RequestTexture::GetTexture()
{
	return m_texture;
}