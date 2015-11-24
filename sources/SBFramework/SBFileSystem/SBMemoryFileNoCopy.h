#pragma once
#include "IFile.h"

#include <cstdio>
#include <memory>

namespace SB
{
	class MemoryFileNoCopy : public IFile
	{
	public:
		MemoryFileNoCopy(const char* buffer, int size);

		virtual const char* GetPointerConst() const;
		
		virtual ~MemoryFileNoCopy();

		virtual bool Valid() const;

		virtual void Seek(int position) const;

		virtual int Tell() const;

		virtual int GetSize() const;

		virtual bool Read(char* destanation, int size) const;

	private:
		MemoryFileNoCopy(){};

		virtual char* GetPointer(){ return nullptr; };

		virtual bool Write(const char* destanation, int size) { return false; };

		virtual bool Open(const std::string& filename, MODE mode){ return false; };

		const char* m_buffer;
		mutable int m_pointer;
		int m_size;
	};
}