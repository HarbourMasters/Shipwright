#pragma once
#include <vector>
#include <optional>

#include "Controller.h"

namespace Ship {
	class DummyController final : public Controller {
	public:
		DummyController(const std::string& CUID, const std::string& KeyName, bool Connected) {
			GUID = CUID;
			isConnected = Connected;
			ButtonName = KeyName;
		}

		std::map<std::vector<std::string>, int32_t> ReadButtonPress();
		void ReadFromSource(int32_t slot) override {}
		const std::string GetControllerName() override { return GUID; }
		const std::string GetButtonName(int slot, int n64Button) override { return ButtonName; }
		void WriteToSource(int32_t slot, ControllerCallback* controller) override { }
		bool Connected() const override { return isConnected; }
		bool CanRumble() const override { return false; }
		bool CanGyro()   const override { return false; }

		void ClearRawPress() override {}
		int32_t ReadRawPress() override { return -1; }
		bool HasPadConf() const { return true; }
		std::optional<std::string> GetPadConfSection() { return "Unk"; }
		void CreateDefaultBinding(int32_t slot) override {}
	protected:
		std::string ButtonName;
		bool isConnected = false;
		std::string GetControllerType() { return "Unk"; }
		std::string GetConfSection() { return "Unk"; }
		std::string GetBindingConfSection() { return "Unk"; }
	};
}