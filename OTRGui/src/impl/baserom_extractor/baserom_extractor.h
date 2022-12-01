#ifndef EXTRACT_BASEROM_H_
#define EXTRACT_BASEROM_H_

#include "src/resource/GameVersions.h"
#include <cstdio>
#include <string>

struct RomVersion {
	std::string version = "None";
	std::string error = "None";
	std::string listPath = "None";
	bool isMQ;
	int offset;
	uint32_t crc;
};

struct WriteResult {
	std::string error = "None";
};

WriteResult ExtractBaserom(const char* filePath);
RomVersion GetVersion(FILE * rom);

#endif