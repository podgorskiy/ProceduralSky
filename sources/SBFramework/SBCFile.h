#pragma once
#include "IFile.h"

#include <cstdio>

namespace SB
{
	class CFile : public IFile
	{
	public:
		CFile();

		CFile(const std::string& filename, MODE mode);

		virtual ~CFile();
		
		virtual bool Open(const std::string& filename, MODE mode);

		virtual void Close();

		virtual bool Valid();

		virtual void Seek(int position);

		virtual int Tell();

		virtual int GetSize();

		virtual bool Read(char* destanation, int size);

		virtual bool Write(const char* destanation, int size);

	private:
		const char* GetMode(MODE mode);
		FILE* file;
	};
}