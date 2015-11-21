#pragma once
#include "SBRequestPull.h"
#include "SBRequest.h"
#include "SBRequestData.h"
#include "SBRequestTexture.h"

using namespace SB;

void RequestPull::SetUrlPrefix(const std::string& URL)
{
	m_urlPrefix = URL;
}

const std::string& RequestPull::GetUrlPrefix()
{
	return m_urlPrefix;
}

RequestDataPtr RequestPull::CreateDataRequest(const std::string& URL)
{
	RequestData* r = new RequestData(URL, this);
	RequestDataPtr ptr(r);
	AddToPending(ptr);
	return ptr;
}

RequestTexturePtr RequestPull::CreateTextureRequest(const std::string& URL)
{
	RequestTexture* r = new RequestTexture(URL, this);
	RequestTexturePtr ptr(r);
	AddToPending(ptr);
	return ptr;
}

void RequestPull::AddToPending(RequestDataPtr request)
{
	m_pendingData[request.get()] = request;
}

void RequestPull::AddToPending(RequestTexturePtr request)
{
	m_pendingTextures[request.get()] = request;
}

void RequestPull::RemoveFromPending(Request* request)
{
	std::map<Request*, RequestTexturePtr>::iterator itT = m_pendingTextures.find(request);

	if (itT != m_pendingTextures.end())
	{
		m_pendingTextures.erase(itT);
		return;
	}

	std::map<Request*, RequestDataPtr>::iterator itD = m_pendingData.find(request);
	if (itD != m_pendingData.end())
	{
		m_pendingData.erase(itD);
	}
}