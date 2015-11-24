#include "SBFileSystem/SBMemoryFile.h"
#include "SBFileSystem/SBMemoryFileNoCopy.h"
#include "SBRequest.h"
#include "SBRequestData.h"
#include <string>

using namespace SB;

RequestData::RequestData(const std::string& URL, bool copyData, RequestPull* requestPull) : Request(URL, copyData, requestPull)
{
};

void RequestData::OnLoad(const void* buffer, int size)
{
	if (m_copyData)
	{
		m_memoryFile = MemoryFile(static_cast<const char*>(buffer), size);
		if (m_callback != nullptr)
		{
			m_callback(&m_memoryFile);
		}
	}
	else
	{
		SB::MemoryFileNoCopy memoryFile = MemoryFileNoCopy(static_cast<const char*>(buffer), size);
		if (m_callback != nullptr)
		{
			m_callback(&memoryFile);
		}
	}
	Request::OnLoad(buffer, size);
}

void RequestData::SetCallback(const std::function<void(const IFile* file)>& callback)
{
	m_callback = callback;
}

MemoryFile RequestData::GetFile()
{
	return m_memoryFile;
}