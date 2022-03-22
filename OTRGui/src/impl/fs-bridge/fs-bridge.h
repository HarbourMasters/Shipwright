#pragma once

#include <string>

#define NULLSTR "None"

enum class LaunchType {
	FILE, FOLDER
};

struct ProcessResult {
	int exitCode;
};

class FSBridge {
public:
	virtual void InitBridge() = 0;
	virtual ProcessResult LaunchProcess(std::string cmd) = 0;
	virtual std::string LaunchFileExplorer(LaunchType type) = 0;
};