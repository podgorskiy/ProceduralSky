#include "SBMemoryFile.h"
#include "SBCommon.h"

#include <cstdio>

using namespace SB;

namespace SB
{
	struct BufferHandle
	{
		BufferHandle(int size) :m_buffer(new char[size]), m_pointer(0), m_size(size)
		{};
		~BufferHandle()
		{
			SafeDeleteArray(m_buffer);
		}
		bool Read(char* destanation, int size) const
		{
			if (m_pointer + size > m_size)
			{
				return false;
			}
			else
			{
				memcpy(destanation, m_buffer + m_pointer, size);
				m_pointer += size;
				return true;
			}
		}
		bool Write(const char* destanation, int size)
		{
			if (m_pointer + size > m_size)
			{
				return false;
			}
			else
			{
				memcpy(m_buffer + m_pointer, destanation, size);
				m_pointer += size;
				return true;
			}
		}
		char* m_buffer;
		mutable int m_pointer;
		int m_size;
	};
}

MemoryFile::MemoryFile() :m_buffer(nullptr)
{
};

MemoryFile::MemoryFile(const char* buffer, int size)
{
	m_buffer = BufferHandlePtr(new BufferHandle(size));
	if (buffer != nullptr)
	{
		memcpy(m_buffer->m_buffer, buffer, size);
	}
}

MemoryFile::~MemoryFile()
{
};

char* MemoryFile::GetPointer()
{
	return m_buffer->m_buffer;
}

bool MemoryFile::Valid() const
{
	return m_buffer != nullptr;
};

void MemoryFile::Seek(int position) const
{
	m_buffer->m_pointer = position;
};

int MemoryFile::Tell() const 
{
	return m_buffer->m_pointer;
};

int MemoryFile::GetSize() const
{
	return m_buffer->m_size;
};

bool MemoryFile::Read(char* destanation, int size) const
{
	return m_buffer->Read(destanation, size);
}

bool MemoryFile::Write(const char* destanation, int size)
{
	return m_buffer->Write(destanation, size);
}
