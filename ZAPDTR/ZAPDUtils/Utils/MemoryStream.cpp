#include "MemoryStream.h"
#include <cstring>

#ifndef _MSC_VER
#define memcpy_s(dest, destSize, source, sourceSize) memcpy(dest, source, destSize)
#endif

MemoryStream::MemoryStream()
{
	buffer = std::vector<char>();
	//buffer.reserve(1024 * 16);
	bufferSize = 0;
	baseAddress = 0;
}

MemoryStream::MemoryStream(char* nBuffer, size_t nBufferSize) : MemoryStream()
{
	buffer = std::vector<char>(nBuffer, nBuffer + nBufferSize);
	bufferSize = nBufferSize;
	baseAddress = 0;
}

MemoryStream::~MemoryStream()
{
}

uint64_t MemoryStream::GetLength()
{
	return buffer.size();
}

void MemoryStream::Seek(int32_t offset, SeekOffsetType seekType)
{
	if (seekType == SeekOffsetType::Start)
		baseAddress = offset;
	else if (seekType == SeekOffsetType::Current)
		baseAddress += offset;
	else if (seekType == SeekOffsetType::End)
		baseAddress = bufferSize - 1 - offset;
}

std::unique_ptr<char[]> MemoryStream::Read(size_t length)
{
	std::unique_ptr<char[]> result = std::make_unique<char[]>(length);

	memcpy_s(result.get(), length, &buffer[baseAddress], length);
	baseAddress += length;

	return result;
}

void MemoryStream::Read(const char* dest, size_t length)
{
	memcpy_s((void*)dest, length, &buffer[baseAddress], length);
	baseAddress += length;
}

int8_t MemoryStream::ReadByte()
{
	return buffer[baseAddress++];
}

void MemoryStream::Write(char* srcBuffer, size_t length)
{
	if (baseAddress + length >= buffer.size())
	{
		buffer.resize(baseAddress + length);
		bufferSize += length;
	}

	memcpy_s(&buffer[baseAddress], length, srcBuffer, length);
	baseAddress += length;
}

void MemoryStream::WriteByte(int8_t value)
{
	if (baseAddress >= buffer.size())
	{
		buffer.resize(baseAddress + 1);
		bufferSize = baseAddress;
	}

	buffer[baseAddress++] = value;
}

std::vector<char> MemoryStream::ToVector()
{
	return buffer;
}

void MemoryStream::Flush()
{
}

void MemoryStream::Close()
{
}