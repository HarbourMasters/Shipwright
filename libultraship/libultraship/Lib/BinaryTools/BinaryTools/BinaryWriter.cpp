#include "BinaryWriter.h"
#include "MemoryBuffer.h"
#include <filesystem>
#include <fstream>

BinaryWriter::BinaryWriter(std::string_view inputPath, bool truncate)
{
    //Can't simply exclude the truncate flag when !truncate. More details here: https://stackoverflow.com/a/57070159
    int flags = 0;
    if (truncate)
        flags = std::ofstream::out | std::ofstream::binary | std::ofstream::trunc; //Clears existing contents of the file
    else
        flags = std::ofstream::in | std::ofstream::out | std::ofstream::binary;

    //If not truncating and the file doesn't exist, then opening will fail. So we create the file first if it doesn't exist
    if (!truncate && !std::filesystem::exists(inputPath))
    {
        std::fstream f;
        f.open(inputPath, std::fstream::out);
        f.close();
    }

    stream_ = new std::ofstream(std::string(inputPath), flags);
}

BinaryWriter::BinaryWriter(char* buffer, uint32_t sizeInBytes)
{
    buffer_ = new MemoryBuffer(buffer, sizeInBytes);
    stream_ = new std::ostream(buffer_);
}

BinaryWriter::~BinaryWriter()
{
    delete stream_;
    if (buffer_)
        delete[] buffer_;
}

void BinaryWriter::Flush()
{
    stream_->flush();
}

void BinaryWriter::WriteUint8(uint8_t value)
{
    stream_->write(reinterpret_cast<const char*>(&value), 1);
}

void BinaryWriter::WriteUint16(uint16_t value)
{
    stream_->write(reinterpret_cast<const char*>(&value), 2);
}

void BinaryWriter::WriteUint32(uint32_t value)
{
    stream_->write(reinterpret_cast<const char*>(&value), 4);
}

void BinaryWriter::WriteUint64(uint64_t value)
{
    stream_->write(reinterpret_cast<const char*>(&value), 8);
}

void BinaryWriter::WriteInt8(int8_t value)
{
    stream_->write(reinterpret_cast<const char*>(&value), 1);
}

void BinaryWriter::WriteInt16(int16_t value)
{
    stream_->write(reinterpret_cast<const char*>(&value), 2);
}

void BinaryWriter::WriteInt32(int32_t value)
{
    stream_->write(reinterpret_cast<const char*>(&value), 4);
}

void BinaryWriter::WriteInt64(int64_t value)
{
    stream_->write(reinterpret_cast<const char*>(&value), 8);
}

void BinaryWriter::WriteChar(char value)
{
    stream_->write(reinterpret_cast<const char*>(&value), 1);
}

void BinaryWriter::WriteNullTerminatedString(const std::string& value)
{
    stream_->write(value.data(), value.size());
    WriteChar('\0');
}

void BinaryWriter::WriteFixedLengthString(const std::string& value)
{
    stream_->write(value.data(), value.size());
}

void BinaryWriter::WriteFloat(float value)
{
    stream_->write(reinterpret_cast<const char*>(&value), 4);
}

void BinaryWriter::WriteDouble(double value)
{
    stream_->write(reinterpret_cast<const char*>(&value), 8);
}

void BinaryWriter::WriteFromMemory(const void* data, size_t size)
{
    stream_->write(reinterpret_cast<const char*>(data), size);
}

void BinaryWriter::SeekBeg(size_t absoluteOffset)
{
    stream_->seekp(absoluteOffset, std::ifstream::beg);
}

void BinaryWriter::SeekCur(size_t relativeOffset)
{
    stream_->seekp(relativeOffset, std::ifstream::cur);
}

void BinaryWriter::Skip(size_t bytesToSkip)
{
    size_t position = Position();
    size_t length = Length();

    //If we're skipped past the end of the stream then skip what's available and write null bytes for the rest
    if (position + bytesToSkip > length)
    {
        size_t bytesAvailable = length - position;
        size_t bytesNeeded = bytesToSkip - bytesAvailable;

        stream_->seekp(bytesAvailable, std::ifstream::cur);
        WriteNullBytes(bytesNeeded);
    }
    else
        stream_->seekp(bytesToSkip, std::ifstream::cur);
}

void BinaryWriter::WriteNullBytes(size_t bytesToWrite)
{
    //Todo: See if quicker to allocate array of zeros and use WriteFromMemory
    for (size_t i = 0; i < bytesToWrite; i++)
        WriteUint8(0);
}

size_t BinaryWriter::CalcAlign(size_t position, size_t alignmentValue)
{
    const size_t remainder = position % alignmentValue;
    size_t paddingSize = remainder > 0 ? alignmentValue - remainder : 0;
    return paddingSize;
}

size_t BinaryWriter::Align(size_t alignmentValue)
{
    const size_t paddingSize = CalcAlign(stream_->tellp(), alignmentValue);
    Skip(paddingSize);
    return paddingSize;
}

size_t BinaryWriter::Position() const
{
    return stream_->tellp();
}

size_t BinaryWriter::Length()
{
    //Save current position
    size_t realPosition = Position();

    //Seek to end of file and get position (the length)
    stream_->seekp(0, std::ios::end);
    size_t endPosition = Position();

    //Seek back to real pos and return length
    if (realPosition != endPosition)
        SeekBeg(realPosition);

    return endPosition;
}
