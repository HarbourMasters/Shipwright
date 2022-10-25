#pragma once
#include "Controller.h"
#include <string>

namespace Ship {
	class KeyboardController : public Controller {
		public:
			KeyboardController();

			void ReadFromSource(int32_t virtualSlot) override;
			void WriteToSource(int32_t virtualSlot, ControllerCallback* controller) override;
			const std::string GetControllerName() override;
			const std::string GetButtonName(int32_t virtualSlot, int32_t n64Button) override;
			bool PressButton(int32_t dwScancode);
			bool ReleaseButton(int32_t dwScancode);
			bool Connected() const override;
			bool CanRumble() const override;
			bool CanGyro() const override;
			void ClearRawPress() override;
			int32_t ReadRawPress() override;
			void ReleaseAllButtons();
			void SetLastScancode(int32_t key);
			int32_t GetLastScancode();
			void CreateDefaultBinding(int32_t virtualSlot) override;

		protected:
			int32_t lastScancode;
			int32_t lastKey = -1;
	};
}
