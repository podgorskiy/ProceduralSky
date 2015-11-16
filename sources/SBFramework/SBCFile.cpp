#include "SBCFile.h"

#include <cstdio>

using namespace SB;

CFile::CFile() :file(NULL)
{};

CFile::CFile(const std::string& filename, MODE mode) :file(NULL)
{
	Open(filename, mode);
}

CFile::~CFile()
{
	if (file != NULL)
	{
		std::fclose(file);
	}
};

bool CFile::Open(const std::string& filename, MODE mode)
{
	if (file != NULL)
	{
		std::fclose(file);
	}
	file = std::fopen(filename.c_str(), GetMode(mode));
	return file != NULL;
};

void CFile::Close()
{
	std::fclose(file);
	file == NULL;
};

bool CFile::Valid()
{
	return file != NULL;
};

void CFile::Seek(int position)
{
	std::fseek(file, position, SEEK_SET);
};

int CFile::Tell()
{
	return std::ftell(file);
};

int CFile::GetSize()
{
	int backup = ftell(file);
	std::fseek(file, 0, SEEK_END);
	int size = ftell(file);
	std::fseek(file, backup, SEEK_SET);
	return size;
};

bool CFile::Read(char* destanation, int size)
{
	size_t readSize = std::fread(destanation, 1, size, file);
	return readSize == size;
}

bool CFile::Write(const char* destanation, int size)
{
	size_t writeSize = std::fwrite(destanation, 1, size, file);
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
}