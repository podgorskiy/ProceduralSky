#pragma once

#include <string>

namespace SB
{
	class IFile
	{
	public:

		virtual ~IFile()
		{
		};

		enum MODE
		{
			FILE_READ,
			FILE_WRITE,
			FILE_APPEND,
			FILE_READ_UPDATE,
			FILE_WRITE_UPDATE,
			FILE_APPEND_UPDATE
		};

		virtual bool Open(const std::string& filename, MODE mode) = 0;
		
		virtual bool Valid() const = 0;

		virtual void Seek(int position) const = 0;

		virtual int Tell() const = 0;

		virtual int GetSize() const = 0;

		virtual bool Read(char* destanation, int size) const = 0;

		virtual bool Write(const char* source, int size) = 0;

		bool ReadCompressed(char* destanation, int size) const;

		bool WriteCompressed(const char* source, int size);
		
		bool ReadByte(char& byte) const;

		bool ReadInt(int& integer) const;

		bool ReadFloat(float& floatValue) const;

		bool ReadString(std::string& str) const;

		bool WriteByte(char byte);

		bool WriteInt(int integer);

		bool WriteFloat(float floatValue);

		bool WriteString(const std::string& str);
			
	private:
		template<typename T>
		bool Read_(T& data) const
		{
			return Read(reinterpret_cast<char*>(&data), sizeof(T));
		}

		template<typename T>
		bool Write_(const T& data)
		{
			return Write(reinterpret_cast<const char*>(&data), sizeof(T));
		}

	};

	inline bool IFile::ReadByte(char& byte) const
	{
		return Read_(byte);
	}

	inline bool IFile::ReadInt(int& integer) const
	{
		return Read_(integer);
	}

	inline bool IFile::ReadFloat(float& floatValue) const
	{
		return Read_(floatValue);
	}
	
	inline bool IFile::WriteByte(char byte) 
	{
		return Write_(byte);
	}

	inline bool IFile::WriteInt(int integer)
	{
		return Write_(integer);
	}

	inline bool IFile::WriteFloat(float floatValue)
	{
		return Write_(floatValue);
	}
}