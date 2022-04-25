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
			bool CanRumble() const { return false; }
			bool Connected() { return true; }

			bool PressButton(int32_t dwScancode);
			bool ReleaseButton(int32_t dwScancode);
			void ReleaseAllButtons();

			std::optional<std::string> GetPadConfSection() { return {}; }
			bool HasPadConf() { return false; };

		protected:
			std::string GetControllerType();
			std::string GetConfSection();
			std::string GetBindingConfSection();
	};
}
