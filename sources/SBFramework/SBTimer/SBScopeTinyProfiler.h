#pragma once
#include "SBTimer/SBTimer.h"
#include "SBCommon.h"

#if defined(NDEBUG) || defined(FORCE_PROFILING)
#define SBProfile(X) SB::ScopeTinyProfiler scopeVar_##X(#X);
#else
#define SBProfile(X)
#endif 

namespace SB
{
	class ScopeTinyProfiler
	{
	public:
		ScopeTinyProfiler(const char* name)
		{
			m_name = name;
			m_startTime = GetMicroseconds();
			quiet = false;
		};

		long long GetTime()
		{
			quiet = true;
			return GetMicroseconds() - m_startTime;
		};

		~ScopeTinyProfiler()
		{
			if (quiet)
			{
				return;
			}
			unsigned long long diffrence = GetMicroseconds() - m_startTime;
			if (diffrence < 10000)
			{
				int d = static_cast<int>(diffrence);
				LOGI(" ------- %-50s: %6dus", m_name, d);
			}
			else
			{
				if (diffrence < 10000000)
				{
					int d = static_cast<int>(diffrence / 1000);
					LOGI(" ------- %-50s: %6dms", m_name, d);
				}
				else
				{
					int d = static_cast<int>(diffrence / 1000 / 1000);
					LOGI(" ------- %-50s: %6ds", m_name, d);
				}
			}
		};

		const char* m_name;
		unsigned long long m_startTime;
		bool quiet;
	};
}