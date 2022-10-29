#pragma once

#include <stdint.h>
#include <vector>
#include <map>
#include <string>

class ZRom
{
public:
	ZRom(std::string romPath);

	std::vector<uint8_t> GetFile(std::string fileName);
    bool IsMQ();

protected:
	std::vector<uint8_t> romData;
	std::map<std::string, std::vector<uint8_t>> files;
};

struct RomVersion
{
	std::string version = "None";
	std::string error = "None";
	std::string listPath = "None";
	int offset;
	uint32_t crc;
};