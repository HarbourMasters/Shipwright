#pragma once
#include "Controller.h"
#include <string>

namespace Ship {
	class KeyboardController : public Controller {
		public:
			KeyboardController(int32_t dwControllerNumber);
			~KeyboardController();

			void ReadFromSource();
			void WriteToSource(ControllerCallback* controller);
			bool Connected() const { return true; }
			bool CanRumble() const { return false; }

			const char* GetButtonName(int button) override;
			bool PressButton(int32_t dwScancode);
			bool ReleaseButton(int32_t dwScancode);
			void ReleaseAllButtons();

			DeviceProfile GetDefaultMapping() override;

			void SetLastScancode(int32_t key) {
				lastScancode = key;
			}

			int32_t GetLastScancode() { return lastScancode; }
			bool HasPadConf() const { return false; }
			std::optional<std::string> GetPadConfSection() { return {}; }

		protected:
			int32_t lastScancode;
			std::string GetControllerType();
			std::string GetConfSection();
			std::string GetBindingConfSection();
	};
}
