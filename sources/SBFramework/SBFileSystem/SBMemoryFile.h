#pragma once
#include "IFile.h"

#include <cstdio>
#include <memory>

namespace SB
{
	struct BufferHandle;
	typedef std::shared_ptr<BufferHandle> BufferHandlePtr;

	class MemoryFile : public IFile
	{
	public:
		MemoryFile();

		MemoryFile(const char* buffer, int size);

		char* GetPointer();

		virtual ~MemoryFile();

		virtual bool Valid() const;

		virtual void Seek(int position) const;

		virtual int Tell() const;

		virtual int GetSize() const;

		virtual bool Read(char* destanation, int size) const;

		virtual bool Write(const char* destanation, int size);

	private:
		virtual bool Open(const std::string& filename, MODE mode){ return false; };

		BufferHandlePtr m_buffer;
	};
}