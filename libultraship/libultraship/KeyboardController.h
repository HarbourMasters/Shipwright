#pragma once
#include "Controller.h"
#include <string>

namespace Ship {
	class KeyboardController : public Controller {
		public:
			KeyboardController();

			void ReadFromSource(int32_t slot) override;
			void WriteToSource(int32_t slot, ControllerCallback* controller) override;
			bool Connected() const override { return true; }
			bool CanRumble() const override { return false; }
			bool CanGyro() const override { return false; }
			const char* GetControllerName() override;
			const char* GetButtonName(int slot, int n64Button) override;
			bool PressButton(int32_t dwScancode);
			bool ReleaseButton(int32_t dwScancode);

			void ClearRawPress() override {
				lastKey = -1;
			}

			int32_t ReadRawPress() override;
			void ReleaseAllButtons();

			void SetLastScancode(int32_t key) {
				lastScancode = key;
			}

			int32_t GetLastScancode() { return lastScancode; }
			void CreateDefaultBinding(int32_t slot) override;

		protected:
			int32_t lastScancode;
			int32_t lastKey = -1;
	};
}
