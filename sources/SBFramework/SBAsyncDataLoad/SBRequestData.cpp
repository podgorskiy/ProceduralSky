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
	Request::OnLoad(buffer, size);
	m_memoryFile = MemoryFile(static_cast<const char*>(buffer), size);
}

MemoryFile RequestData::GetFile()
{
	return m_memoryFile;
}