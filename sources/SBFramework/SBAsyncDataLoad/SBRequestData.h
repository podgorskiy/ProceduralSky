#pragma once
#include <string>
#include "SBFileSystem/SBMemoryFile.h"
#include "SBRequest.h"

namespace SB
{
	class RequestPull;

	typedef std::shared_ptr<RequestData> RequestDataPtr;
	
	class RequestData : public SB::Request
	{
		friend class SB::RequestPull;
	public:

		MemoryFile GetFile();

		void SetCallback(const std::function<void(const IFile* file)>& callback);

	private:
		virtual void OnLoad(const void* buffer, int size);

		RequestData(const std::string& URL, bool copyData, RequestPull* requestPull);

		MemoryFile m_memoryFile;
		std::function<void(const IFile* file)> m_callback;
	};
}