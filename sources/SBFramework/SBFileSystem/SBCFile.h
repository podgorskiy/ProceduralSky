#pragma once
#include "IFile.h"

#include <cstdio>
#include <memory>

namespace SB
{
	struct FileHandle;
	typedef std::shared_ptr<FileHandle> FileHandlePtr;

	class CFile : public IFile
	{
	public:
		CFile();

		CFile(const std::string& filename, MODE mode);

		virtual ~CFile();
		
		virtual bool Open(const std::string& filename, MODE mode);
		
		virtual bool Valid() const;

		virtual void Seek(int position) const;

		virtual int Tell() const;

		virtual int GetSize() const;

		virtual bool Read(char* destanation, int size) const;

		virtual bool Write(const char* destanation, int size);

	private:
		const char* GetMode(MODE mode);
		FileHandlePtr m_file;
	};
}