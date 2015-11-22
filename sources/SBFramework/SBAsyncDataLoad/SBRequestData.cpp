#include "SBFileSystem/SBMemoryFile.h"
#include "SBRequest.h"
#include "SBRequestData.h"
#include <string>

using namespace SB;

RequestData::RequestData(const std::string& URL, RequestPull* requestPull) : Request(URL, requestPull)
{
};

void RequestData::OnLoad(const void* buffer, int size)
{
	m_memoryFile = MemoryFile(static_cast<const char*>(buffer), size);
	if (m_callback != nullptr)
	{
		m_callback(m_memoryFile);
	}
	Request::OnLoad(buffer, size);
}

void RequestData::SetCallback(const std::function<void(const MemoryFile& memfile)>& callback)
{
	m_callback = callback;
}

MemoryFile RequestData::GetFile()
{
	return m_memoryFile;
}