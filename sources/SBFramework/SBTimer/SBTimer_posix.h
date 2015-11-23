#include <sys/time.h>

namespace SB
{
	unsigned long long GetTicks() 
	{
		struct timeval tp;
		gettimeofday(&tp, 0);
		return tp.tv_sec * 1000000 + tp.tv_usec;
	}

	unsigned long long GetTicksPerSecond() 
	{
		return 1000000;
	}
}

