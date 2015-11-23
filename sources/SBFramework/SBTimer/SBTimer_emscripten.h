#include <emscripten.h>
#include <cmath>

namespace SB
{
	unsigned long long GetTicks() 
	{
		double time = emscripten_get_now();
		long long ms = static_cast<long long>(floor(time));
		long long us = static_cast<long long>((time - ms) * 1000.0);
		return ms * 1000 + us;
	}

	unsigned long long GetTicksPerSecond() 
	{
		return 1000000;
	}
}

