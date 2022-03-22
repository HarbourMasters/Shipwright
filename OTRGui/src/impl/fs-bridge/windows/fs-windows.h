#pragma once

#include "../fs-bridge.h"

class WindowsBridge : public FSBridge {
	void InitBridge() override;
	ProcessResult LaunchProcess(std::string cmd) override;
	std::string LaunchFileExplorer(LaunchType type) override;
};