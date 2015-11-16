#include <windows.h>

namespace SB
{
	unsigned long long GetTicks()
	{
		LARGE_INTEGER ticks;
		QueryPerformanceCounter(&ticks);
		return ticks.QuadPart;
	}

	unsigned long long GetTicksPerSecond()
	{
		LARGE_INTEGER tickPerSeconds;
		QueryPerformanceFrequency(&tickPerSeconds);
		return tickPerSeconds.QuadPart;
	}
}