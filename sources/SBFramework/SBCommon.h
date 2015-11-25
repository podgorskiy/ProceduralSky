#pragma once
#include <cstdio>
#include <cassert>
#include <cstring>

#define LOG__(...) (void)printf(__VA_ARGS__);
#define LOG_(...) (void)printf(__VA_ARGS__);printf("\n");
#define LOGW(...) printf("File:%s\nline:%d\n", __FILE__, __LINE__);printf(" W: ");(void)printf(__VA_ARGS__);printf("\n");
#define LOGE(...) printf("File:%s\nline:%d\n", __FILE__, __LINE__);printf(" E: ");(void)printf(__VA_ARGS__);printf("\n");assert(false)
#define LOGI(...) printf(" I: ");(void)printf(__VA_ARGS__);printf("\n");

#define ASSERT(Condition, ...) if(!(Condition)){LOGE(__VA_ARGS__);}

namespace SB
{
	inline bool CheckExtension(const unsigned char * extensionsList, const char* extension)
	{
		return  strstr(reinterpret_cast<const char *>(extensionsList), extension) != NULL;
	}

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
		if (p != nullptr)
		{
			delete p;
			p = nullptr;
		}
	}

	/// Template for safe deleteion of arrays of objects
	template<typename T>
	inline void SafeDeleteArray(T*& p)
	{
		if (p != nullptr)
		{
			delete[] p;
			p = nullptr;
		}
	}
}

