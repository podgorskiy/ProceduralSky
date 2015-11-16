#include "SBTimer/SBTimer.h"
#include <sys/time.h>
#include <mach/mach_time.h>

namespace SB
{
	unsigned long long GetTicks()
	{
		static unsigned long long s_baseTime = mach_absolute_time();
		return mach_absolute_time() - s_baseTime;
	}

	unsigned long long InitTicksPerSeconds()
	{
		mach_timebase_info_data_t info;
		mach_timebase_info(&info);
		return 1000 * 1000 * 1000 * info.denom / info.numer;
	}

	unsigned long long GetTicksPerSecond()
	{
		static unsigned long  s_cyclePerSeconds = InitTicksPerSeconds();
		return s_cyclePerSeconds;
	}
}