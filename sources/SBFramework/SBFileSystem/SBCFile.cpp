#include "SBCFile.h"
#include "SBCommon.h"

#include <cstdio>

using namespace SB;

namespace SB
{
	struct FileHandle
	{
		FileHandle() :file(nullptr){};
		~FileHandle()
		{
			if (file != nullptr)
			{
				std::fclose(file);
				file = nullptr;
			}
		}
		FILE* file;
	};
}

CFile::CFile()
{
};

CFile::CFile(const std::string& filename, MODE mode)
{
	Open(filename, mode);
}

CFile::~CFile()
{
};

bool CFile::Open(const std::string& filename, MODE mode)
{
	FileHandle* fh = new FileHandle;
	fh->file = std::fopen(filename.c_str(), GetMode(mode));
	if (fh->file == nullptr)
	{
		LOGW("File not found: %s", filename.c_str());
		return false;
	}
	m_file = FileHandlePtr(fh);
	return true;
};

bool CFile::Valid() const
{	
	if (m_file != nullptr)
	{
		return m_file->file != nullptr;
	}
	return false;
};

void CFile::Seek(int position) const
{
	std::fseek(m_file->file, position, SEEK_SET);
};

int CFile::Tell() const
{
	return std::ftell(m_file->file);
};

int CFile::GetSize() const
{
	int backup = ftell(m_file->file);
	std::fseek(m_file->file, 0, SEEK_END);
	int size = ftell(m_file->file);
	std::fseek(m_file->file, backup, SEEK_SET);
	return size;
};

bool CFile::Read(char* destanation, int size) const
{
	size_t readSize = std::fread(destanation, 1, size, m_file->file);
	return readSize == size;
}

bool CFile::Write(const char* destanation, int size)
{
	size_t writeSize = std::fwrite(destanation, 1, size, m_file->file);
	return writeSize == size;
}

inline const char* CFile::GetMode(MODE mode)
{
	switch (mode)
	{
	case FILE_READ:
		return "rb";
	case FILE_WRITE:
		return "wb";
	case FILE_APPEND:
		return "ab";
	case FILE_READ_UPDATE:
		return "r+b";
	case FILE_WRITE_UPDATE:
		return "w+b";
	case FILE_APPEND_UPDATE:
		return "w+b";
	}
	return "";
}