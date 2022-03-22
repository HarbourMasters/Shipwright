#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>
#include "../Color3b.h"
#include "../Vec2f.h"
#include "../Vec3f.h"
#include "../Vec3s.h"
#include "Stream.h"

class BinaryReader
{
public:
	BinaryReader(Stream* nStream);
	BinaryReader(std::shared_ptr<Stream> nStream);

	void Close();

	void Seek(uint32_t offset, SeekOffsetType seekType);
	uint32_t GetBaseAddress();

	void Read(char* buffer, int32_t length);
	char ReadChar();
	int8_t ReadByte();
	int16_t ReadInt16();
	int32_t ReadInt32();
	uint8_t ReadUByte();
	uint16_t ReadUInt16();
	uint32_t ReadUInt32();
	uint64_t ReadUInt64();
	float ReadSingle();
	double ReadDouble();
	Vec3f ReadVec3f();
	Vec3s ReadVec3s();
	Vec3s ReadVec3b();
	Vec2f ReadVec2f();
	Color3b ReadColor3b();
	std::string ReadString();

protected:
	std::shared_ptr<Stream> stream;
};