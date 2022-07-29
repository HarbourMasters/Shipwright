#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>
#include "BitConverter.h"
#include "Stream.h"

class BinaryWriter
{
public:
	BinaryWriter(Stream* nStream);
	BinaryWriter(std::shared_ptr<Stream> nStream);

	void SetEndianness(Endianness endianness);

	std::shared_ptr<Stream> GetStream();
	uint64_t GetBaseAddress();
	uint64_t GetLength();
	void Seek(int32_t offset, SeekOffsetType seekType);
	void Close();

	void Write(int8_t value);
	void Write(uint8_t value);
	void Write(int16_t value);
	void Write(uint16_t value);
	void Write(int32_t value);
	void Write(int32_t valueA, int32_t valueB);
	void Write(uint32_t value);
	void Write(int64_t value);
	void Write(uint64_t value);
	void Write(float value);
	void Write(double value);
	void Write(const std::string& str);
	void Write(char* srcBuffer, size_t length);

protected:
	std::shared_ptr<Stream> stream;
	Endianness endianness = Endianness::Native;
};