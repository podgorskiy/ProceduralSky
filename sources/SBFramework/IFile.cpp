#include "IFile.h"
#include <lz4.h>
#include <lz4hc.h>
#include <cassert>

using namespace SB;

bool IFile::ReadString(std::string& str)
{
	int size = 0;
	if (!ReadInt(size))
	{
		return false;
	}
	str.resize(size);
	if (Read(&str[0], size))
	{
		return true;
	}
	return false;
}

bool IFile::WriteString(const std::string& str)
{
	if (!WriteInt(str.size()))
	{
		return false;
	}
	if (Write(&str[0], str.size()))
	{
		return true;
	}
	return false;
}

bool IFile::ReadCompressed(char* destanation, int size)
{
	int compressedSize;
	ReadInt(compressedSize);
	if (compressedSize > 0)
	{
		char* buffer = new char[compressedSize];
		bool result = Read(buffer, compressedSize);
		if (result)
		{
			int uncompressedSize = LZ4_decompress_fast(buffer, destanation, size);
			//assert(uncompressedSize == size);
		}
		delete[] buffer;
		return result;
	}
	else
	{
		bool result = Read(destanation, size);
		return result;
	}
}

bool IFile::WriteCompressed(const char* source, int size)
{
	int compressedSizeWorst = LZ4_compressBound(size) + 4;
	char* buffer = new char[compressedSizeWorst];
	int compressedSize = LZ4_compress_HC(source, buffer, size, compressedSizeWorst, 9);
	bool result = false;
	if (compressedSize < size)
	{
		WriteInt(compressedSize);
		result = Write(buffer, compressedSize);
	}
	else
	{
		WriteInt(-1);
		result = Write(source, size);
	}
	delete[] buffer;
	return result;
}