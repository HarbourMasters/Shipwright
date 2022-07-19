#pragma once
#include <vector>
#include <optional>

#include "Controller.h"

class DisconnectedController final : public Ship::Controller {
public:
	DisconnectedController() {
		GUID = "Disconnected";
	}

	std::map<std::vector<std::string>, int32_t> ReadButtonPress();
	void ReadFromSource(int32_t slot) override {}
	const char* GetControllerName() override { return "Disconnected"; }
	const char* GetButtonName(int slot, int n64Button) override { return "None"; }
	void WriteToSource(int32_t slot, ControllerCallback* controller) override { }
	bool Connected() const override { return false; }
	bool CanRumble() const override { return false; }
	bool CanGyro()   const override { return false; }

	void ClearRawPress() override {}
	int32_t ReadRawPress() override { return -1; }
	bool HasPadConf() const { return true; }
	std::optional<std::string> GetPadConfSection() { return "Unk"; }
	void CreateDefaultBinding(int32_t slot) override {}
protected:
	std::string GetControllerType() { return "Unk"; }
	std::string GetConfSection() { return "Unk"; }
	std::string GetBindingConfSection() { return "Unk"; }
};
