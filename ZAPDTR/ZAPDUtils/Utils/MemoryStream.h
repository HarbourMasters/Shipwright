#pragma once

#include <memory>
#include <vector>
#include "Stream.h"

class MemoryStream : public Stream
{
public:
	MemoryStream();
	MemoryStream(char* nBuffer, size_t nBufferSize);
	~MemoryStream();

	uint64_t GetLength() override;

	void Seek(int32_t offset, SeekOffsetType seekType) override;

	std::unique_ptr<char[]> Read(size_t length) override;
	void Read(const char* dest, size_t length) override;
	int8_t ReadByte() override;

	void Write(char* srcBuffer, size_t length) override;
	void WriteByte(int8_t value) override;

	std::vector<char> ToVector();

	void Flush() override;
	void Close() override;

protected:
	std::vector<char> buffer;
	std::size_t bufferSize;
};