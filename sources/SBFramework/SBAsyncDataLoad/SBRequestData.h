#pragma once
#include <string>
#include "SBFileSystem/SBMemoryFile.h"
#include "SBRequest.h"

namespace SB
{
	class RequestPull;

	class RequestData : public SB::Request
	{
		friend class SB::RequestPull;
	public:

		MemoryFile GetFile();

	private:
		virtual void OnLoad(const void* buffer, int size);

		RequestData(const std::string& URL, RequestPull* requestPull);

		MemoryFile m_memoryFile;
	};
}