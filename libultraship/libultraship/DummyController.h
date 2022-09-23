#pragma once
#include <vector>
#include <optional>

#include "Controller.h"

namespace Ship {
	class DummyController final : public Controller {
	public:
		DummyController(const std::string& CUID, const std::string& KeyName, bool Connected);
		std::map<std::vector<std::string>, int32_t> ReadButtonPress();
		void ReadFromSource(int32_t virtualSlot) override;
		const std::string GetControllerName() override;
		const std::string GetButtonName(int32_t virtualSlot, int32_t n64Button) override;
		void WriteToSource(int32_t slot, ControllerCallback* controller) override;
		bool Connected() const override;
		bool CanRumble() const override;
		bool CanGyro() const override;
		void ClearRawPress() override;
		int32_t ReadRawPress() override;
		bool HasPadConf() const;
		std::optional<std::string> GetPadConfSection();
		void CreateDefaultBinding(int32_t virtualSlot) override;
	protected:
		std::string ButtonName;
		bool isConnected = false;
		std::string GetControllerType();
		std::string GetConfSection();
		std::string GetBindingConfSection();
	};
}