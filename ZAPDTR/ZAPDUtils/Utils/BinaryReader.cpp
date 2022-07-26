#include "BinaryReader.h"
#include <cmath>
#include <stdexcept>
#include "Stream.h"

BinaryReader::BinaryReader(Stream* nStream)
{
	stream.reset(nStream);
}

BinaryReader::BinaryReader(std::shared_ptr<Stream> nStream)
{
	stream = nStream;
}

void BinaryReader::Close()
{
	stream->Close();
}

void BinaryReader::SetEndianness(Endianness endianness)
{
	this->endianness = endianness;
}

Endianness BinaryReader::GetEndianness() const
{
	return endianness;
}

void BinaryReader::Seek(uint32_t offset, SeekOffsetType seekType)
{
	stream->Seek(offset, seekType);
}

uint32_t BinaryReader::GetBaseAddress()
{
	return stream->GetBaseAddress();
}

void BinaryReader::Read(int32_t length)
{
	stream->Read(length);
}

void BinaryReader::Read(char* buffer, int32_t length)
{
	stream->Read(buffer, length);
}

char BinaryReader::ReadChar()
{
	return (char)stream->ReadByte();
}

int8_t BinaryReader::ReadByte()
{
	return stream->ReadByte();
}

uint8_t BinaryReader::ReadUByte()
{
	return (uint8_t)stream->ReadByte();
}

int16_t BinaryReader::ReadInt16()
{
	int16_t result = 0;

	stream->Read((char*)&result, sizeof(int16_t));

	if (endianness != Endianness::Native)
		result = BSWAP16(result);

	return result;
}

int32_t BinaryReader::ReadInt32()
{
	int32_t result = 0;

	stream->Read((char*)&result, sizeof(int32_t));

	if (endianness != Endianness::Native)
		result = BSWAP32(result);

	return result;
}

uint16_t BinaryReader::ReadUInt16()
{
	uint16_t result = 0;

	stream->Read((char*)&result, sizeof(uint16_t));

	if (endianness != Endianness::Native)
		result = BSWAP16(result);

	return result;
}

uint32_t BinaryReader::ReadUInt32()
{
	uint32_t result = 0;

	stream->Read((char*)&result, sizeof(uint32_t));

	if (endianness != Endianness::Native)
		result = BSWAP32(result);

	return result;
}

uint64_t BinaryReader::ReadUInt64()
{
	uint64_t result = 0;

	stream->Read((char*)&result, sizeof(uint64_t));

	if (endianness != Endianness::Native)
		result = BSWAP64(result);

	return result;
}

float BinaryReader::ReadSingle()
{
	float result = NAN;

	stream->Read((char*)&result, sizeof(float));

	if (endianness != Endianness::Native)
		result = BitConverter::ToFloatBE((uint8_t*)&result, 0);

	if (std::isnan(result))
		throw std::runtime_error("BinaryReader::ReadSingle(): Error reading stream");

	return result;
}

double BinaryReader::ReadDouble()
{
	double result = NAN;

	stream->Read((char*)&result, sizeof(double));

	if (endianness != Endianness::Native)
		result = BitConverter::ToDoubleBE((uint8_t*)&result, 0);

	if (std::isnan(result))
		throw std::runtime_error("BinaryReader::ReadDouble(): Error reading stream");

	return result;
}

Vec3f BinaryReader::ReadVec3f()
{
	return Vec3f();
}

Vec3s BinaryReader::ReadVec3s()
{
	return Vec3s(0, 0, 0);
}

Vec3s BinaryReader::ReadVec3b()
{
	return Vec3s(0, 0, 0);
}

Vec2f BinaryReader::ReadVec2f()
{
	return Vec2f();
}

Color3b BinaryReader::ReadColor3b()
{
	return Color3b();
}

std::string BinaryReader::ReadString()
{
	std::string res;
	int numChars = ReadInt32();

	for (int i = 0; i < numChars; i++)
		res += ReadChar();

	return res;
}