#pragma once
#include <cstdio>
#include <cassert>

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ "File:" __FILE__ " ("__STR1__(__LINE__)") "

#define LOG__(...) (void)printf(__VA_ARGS__);
#define LOG_(...) (void)printf(__VA_ARGS__);printf("\n");
#define LOGW(...) printf(__LOC__);printf(" W: ");(void)printf(__VA_ARGS__);printf("\n");
#define LOGE(...) printf(__LOC__);printf(" E: ");(void)printf(__VA_ARGS__);printf("\n");printf("file:%s\nline:%d\n", __FILE__, __LINE__);assert(false)
#define LOGI(...) printf(" I: ");(void)printf(__VA_ARGS__);printf("\n");

#define ASSERT(Condition, ...) if(!(Condition)){LOGE(__VA_ARGS__);}

namespace SB
{
	template <typename T>
	T MAX(T a, T b)
	{
		if (a > b)
			return a;
		else
			return b;
	}

	template <typename T>
	T MIN(T a, T b)
	{
		if (a < b)
			return a;
		else
			return b;
	}

	/// Template for safe deleteion of objects
	template<typename T>
	inline void SafeDelete(T*& p)
	{
		if (p != NULL)
		{
			delete p;
			p = NULL;
		}
	}

	/// Template for safe deleteion of arrays of objects
	template<typename T>
	inline void SafeDeleteArray(T*& p)
	{
		if (p != NULL)
		{
			delete[] p;
			p = NULL;
		}
	}
}

