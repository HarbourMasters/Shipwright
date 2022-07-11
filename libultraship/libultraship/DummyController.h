#pragma once
#include <vector>

#include "Controller.h"

class DummyController final : public Ship::Controller {
public:
	DummyController(int32_t dwControllerNumber) : Controller(dwControllerNumber) {}
	~DummyController(){}

	std::map<std::vector<std::string>, int32_t> ReadButtonPress();
	void ReadFromSource() {}
	const char* GetControllerName() { return "Dummy"; }
	const char* GetButtonName(int button) override { return "None"; }
	void WriteToSource(ControllerCallback* controller) { }
	bool Connected() const { return false; }
	bool CanRumble() const { return false; }

	bool HasPadConf() const { return true; }
	std::optional<std::string> GetPadConfSection() { return "Unk"; }
	Ship::DeviceProfile GetDefaultMapping() override { return { .Mappings = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} }; }
protected:
	std::string GetControllerType() { return "Unk"; }
	std::string GetConfSection() { return "Unk"; }
	std::string GetBindingConfSection() { return "Unk"; }
};
