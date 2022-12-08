#pragma once
#include <string>

enum Platforms {
	WINDOWS, LINUX
};

struct RomVersion;

void startWorker(RomVersion version);
void updateWorker(const std::string& output, RomVersion version);