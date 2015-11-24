#include "SBMemoryFileNoCopy.h"
#include "SBCommon.h"

#include <cstdio>

using namespace SB;

MemoryFileNoCopy::MemoryFileNoCopy(const char* buffer, int size)
{
	m_buffer = buffer;
	m_size = size;
}

MemoryFileNoCopy::~MemoryFileNoCopy()
{
};

const char* MemoryFileNoCopy::GetPointerConst() const
{
	return m_buffer;
}

bool MemoryFileNoCopy::Valid() const
{
	return m_buffer != nullptr;
};

void MemoryFileNoCopy::Seek(int position) const
{
	m_pointer = position;
};

int MemoryFileNoCopy::Tell() const
{
	return m_pointer;
};

int MemoryFileNoCopy::GetSize() const
{
	return m_size;
};

bool MemoryFileNoCopy::Read(char* destanation, int size) const
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
