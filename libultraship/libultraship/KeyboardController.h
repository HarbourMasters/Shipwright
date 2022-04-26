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

			bool PressButton(int32_t dwScancode);
			bool ReleaseButton(int32_t dwScancode);
			void ReleaseAllButtons();

			bool HasPadConf() const { return false; }
			std::optional<std::string> GetPadConfSection() { return {}; }

		protected:
			std::string GetControllerType();
			std::string GetConfSection();
			std::string GetBindingConfSection();
	};
}
