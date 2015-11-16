#include <sys/time.h>

namespace SB
{
	long long GetTicks() 
	{
		struct timeval tp;
		gettimeofday(&tp, 0);
		return tp.tv_sec * 1000000 + tp.tv_usec;
	}

	long long GetTicksPerSecond() {
		return 1000000;
	}
}

