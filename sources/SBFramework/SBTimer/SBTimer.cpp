#include "SBTimer.h"

#ifdef WIN32
#include "SBTimer/SBTimer_win32.h"
#endif
#ifdef IPHONE
#include "SBTimer/SBTimer_iphone.h"
#endif
#ifdef __EMSCRIPTEN__
#include "SBTimer/SBTimer_emscripten.h"
#endif

unsigned long long SB::GetMilliseconds()
{
	return (GetTicks() * 1000) / GetTicksPerSecond();
}

unsigned long long SB::GetMicroseconds()
{
	return (GetTicks() * 1000 * 1000) / GetTicksPerSecond();
}

unsigned long long SB::GetNanoseconds()
{
	return (GetTicks() * 1000 * 1000 * 1000) / GetTicksPerSecond();
}
