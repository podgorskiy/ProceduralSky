#include "SBRequestPull.h"
#include "SBRequest.h"
#include "SBRequestData.h"
#include "SBRequestTexture.h"
#include <cassert>

using namespace SB;

RequestPull::RequestPull() :m_maxCount(1), m_runningCount(0)
{
}

void RequestPull::SetUrlPrefix(const std::string& URL)
{
	m_urlPrefix = URL;
}

const std::string& RequestPull::GetUrlPrefix()
{
	return m_urlPrefix;
}

void RequestPull::AddToPending(const RequestPtr& request)
{
	m_pending.push_back(std::static_pointer_cast<Request>(request));
}

void RequestPull::RemoveFromRunning(Request* request)
{
	std::map<Request*, RequestPtr>::iterator it = m_running.find(request);
	assert(it != m_running.end());
	m_running.erase(it);
}

void RequestPull::SetCountOfSimultaneousRequests(int count)
{
	m_maxCount = count;
}

void RequestPull::Update()
{
	m_runningCount = m_running.size();
	int pendingSize = m_pending.size();
	if (m_runningCount < m_maxCount && pendingSize > 0)
	{
		int count = m_maxCount - m_runningCount;
		if (count > pendingSize)
		{
			count = pendingSize;
		}
		for (int i = 0; i < count; ++i)
		{
			RequestPtr r = m_pending.front();
			m_pending.pop_front();
			m_running[r.get()] = r;
			r->Start();
		}
	}
}
