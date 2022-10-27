#include "BinaryReader.h"
#include <cmath>
#include "Lib/BinaryTools/BinaryTools/BinaryReader.h"

Ship::BinaryReader::BinaryReader(char* buffer, uint32_t size) 
{
    this->buffer = std::vector<char>(buffer, buffer + size);
    this->reader = std::make_shared<::BinaryReader>(this->buffer.data(), size);
}

void Ship::BinaryReader::SetEndianness(Endianness endianness) 
{
    this->endianness = endianness;
}

Ship::Endianness Ship::BinaryReader::GetEndianness() const
{
    return endianness;
}

void Ship::BinaryReader::Seek(int32_t offset, SeekOffsetType seekType) {
    switch(seekType) {
        case SeekOffsetType::Current:
            if (offset < 0) {
                reader->SeekReverse(-1 * offset);
                break;
            }
            reader->SeekCur(offset);
            break;
        case SeekOffsetType::Start:
            reader->SeekBeg(offset);
            break;
        case SeekOffsetType::End:
            reader->SeekReverse(offset);
            break;
    }
}

uint32_t Ship::BinaryReader::GetBaseAddress() {
    return reader->Position();
}

void Ship::BinaryReader::Read(int32_t length)
{
    reader->ReadFixedLengthString(length);
}

void Ship::BinaryReader::Read(char *buffer, int32_t length)
{
    reader->ReadToMemory(buffer, length);
}

char Ship::BinaryReader::ReadChar()
{
    return reader->ReadChar();
}

int8_t Ship::BinaryReader::ReadInt8()
{
    return reader->ReadInt8();
}

int16_t Ship::BinaryReader::ReadInt16()
{
    int16_t result = 0;
    this->Read((char*)&result, sizeof(int16_t));
    if (endianness != Endianness::Native)
        result = BSWAP16(result);

    return result;
}

int32_t Ship::BinaryReader::ReadInt32()
{
    int32_t result = 0;

    this->Read((char *)&result, sizeof(int32_t));

    if (endianness != Endianness::Native)
        result = BSWAP32(result);

    return result;
}

uint8_t Ship::BinaryReader::ReadUByte()
{
    return reader->ReadUint8();
}

uint16_t Ship::BinaryReader::ReadUInt16()
{
    uint16_t result = 0;

    this->Read((char *)&result, sizeof(uint16_t));

    if (endianness != Endianness::Native)
        result = BSWAP16(result);

    return result;
}

uint32_t Ship::BinaryReader::ReadUInt32()
{
    uint32_t result = 0;

    this->Read((char *)&result, sizeof(uint32_t));

    if (endianness != Endianness::Native)
        result = BSWAP32(result);

    return result;
}

uint64_t Ship::BinaryReader::ReadUInt64()
{
    uint64_t result = 0;

    this->Read((char *)&result, sizeof(uint64_t));

    if (endianness != Endianness::Native)
        result = BSWAP64(result);

    return result;
}

float Ship::BinaryReader::ReadFloat()
{
    float result = NAN;

    this->Read((char *)&result, sizeof(float));

    if (endianness != Endianness::Native)
    {
        float tmp;
        char *dst = (char *)&tmp;
        char *src = (char *)&result;
        dst[3] = src[0];
        dst[2] = src[1];
        dst[1] = src[2];
        dst[0] = src[3];
        result = tmp;
    }

    if (std::isnan(result))
        throw std::runtime_error("BinaryReader::ReadSingle(): Error reading stream");

    return result;
}

double Ship::BinaryReader::ReadDouble()
{
    double result = NAN;

    this->Read((char *)&result, sizeof(double));

    if (endianness != Endianness::Native)
    {
        double tmp;
        char *dst = (char *)&tmp;
        char *src = (char *)&result;
        dst[7] = src[0];
        dst[6] = src[1];
        dst[5] = src[2];
        dst[4] = src[3];
        dst[3] = src[4];
        dst[2] = src[5];
        dst[1] = src[6];
        dst[0] = src[7];
        result = tmp;
    }

    if (std::isnan(result))
        throw std::runtime_error("BinaryReader::ReadDouble(): Error reading stream");

    return result;
}

Vec3f Ship::BinaryReader::ReadVec3f()
{
    return Vec3f();
}

Vec3s Ship::BinaryReader::ReadVec3s()
{
    return Vec3s(0, 0, 0);
}

Vec3s Ship::BinaryReader::ReadVec3b()
{
    return Vec3s(0, 0, 0);
}

Vec2f Ship::BinaryReader::ReadVec2f()
{
    return Vec2f();
}

Color3b Ship::BinaryReader::ReadColor3b()
{
    return Color3b();
}

std::string Ship::BinaryReader::ReadString()
{
    std::string res;
    int numChars = ReadInt32();
    for (int i = 0; i < numChars; i++) {
        res += reader->ReadChar();
    }
    return res;
}

std::string Ship::BinaryReader::ReadCString()
{
    return reader->ReadNullTerminatedString();
}