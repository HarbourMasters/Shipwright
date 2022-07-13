#pragma once
#include <vector>

#include "Controller.h"

class DummyController final : public Ship::Controller {
public:
	DummyController() {
		GUID = "Disconnected";
	}

	std::map<std::vector<std::string>, int32_t> ReadButtonPress();
	void ReadFromSource(int32_t slot) {}
	const char* GetControllerName() { return "Disconnected"; }
	const char* GetButtonName(int slot, int n64Button) override { return "None"; }
	void WriteToSource(int32_t slot, ControllerCallback* controller) { }
	bool Connected() const { return false; }
	bool CanRumble() const { return false; }

	int32_t ReadRawPress() override { return -1; }
	bool HasPadConf() const { return true; }
	std::optional<std::string> GetPadConfSection() { return "Unk"; }
	void CreateDefaultBinding(int32_t slot){}
protected:
	std::string GetControllerType() { return "Unk"; }
	std::string GetConfSection() { return "Unk"; }
	std::string GetBindingConfSection() { return "Unk"; }
};
