#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "SBRequestPull.h"
#include "SBRequest.h"
#include "SBFileSystem/SBCFile.h"

namespace SB
{
	void OnErrorRequest(void* arg)
	{
		Request* r = static_cast<Request*>(arg);
		r->OnError();
	}

	void OnloadRequest(void* arg, void* buffer, int size)
	{
		Request* r = static_cast<Request*>(arg);
		r->OnLoad(buffer, size);
	}
}

using namespace SB;

Request::Request(const std::string& URL, bool copyData, RequestPull* requestPull) : m_done(false), m_copyData(copyData), m_url(URL), m_requestPull(requestPull)
{
};

void Request::Start()
{
#ifdef __EMSCRIPTEN__
	std::string urlFull = m_requestPull->GetUrlPrefix() + m_url;
	emscripten_async_wget_data(urlFull.c_str(), this, OnloadRequest, OnErrorRequest);
#endif
#ifndef __EMSCRIPTEN__
	{
		SB::CFile file(m_requestPull->GetUrlPrefix() + m_url, SB::IFile::FILE_READ);
		if (file.Valid())
		{
			int size = file.GetSize();
			char* buffer = new char[size];
			file.Read(buffer, size);
			OnloadRequest(this, buffer, size);
			delete[] buffer;
		}
	}
#endif
}

void Request::OnError()
{
	m_done = true;
	m_failed = true;
	m_requestPull->RemoveFromRunning(this);
}

void Request::OnLoad(const void* buffer, int size)
{
	m_done = true;
	m_failed = false;
	m_requestPull->RemoveFromRunning(this);
}

bool Request::IsReady()
{
	return m_done;
}

bool Request::IsSuceeded()
{
	return IsReady() && !m_failed;
}

bool Request::IsFailed()
{
	return IsReady() && m_failed;
}