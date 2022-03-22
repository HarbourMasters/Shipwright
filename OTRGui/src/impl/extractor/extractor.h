#pragma once
#include <string>

enum Platforms {
	WINDOWS, LINUX
};

void startWorker();
void updateWorker(const std::string& output);