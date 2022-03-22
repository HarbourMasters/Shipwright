#pragma once

#include "../fs-bridge.h"

class LinuxBridge : public FSBridge {
	void InitBridge() override;
	ProcessResult LaunchProcess(std::string cmd) override;
	std::string LaunchFileExplorer(LaunchType type) override;
};