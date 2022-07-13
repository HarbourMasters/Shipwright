#include "Controller.h"
#include <memory>

namespace Ship {

	Controller::Controller() : isRumbling(false), wStickX(0), wStickY(0), dwPressedButtons(0), wGyroX(0), wGyroY(0) {
		Attachment = nullptr;
		profiles.resize(MAXCONTROLLERS);
		for(int slot = 0; slot < MAXCONTROLLERS; slot++) {
			dwPressedButtons.push_back(0);
		}
	}

	void Controller::Read(OSContPad* pad, int32_t slot) {
		ReadFromSource(slot);

		pad->button |= dwPressedButtons[slot] & 0xFFFF;

		if (pad->stick_x == 0) {
			if (dwPressedButtons[slot] & BTN_STICKLEFT) {
				pad->stick_x = -128;
			}
			else if (dwPressedButtons[slot] & BTN_STICKRIGHT) {
				pad->stick_x = 127;
			}
			else {
				pad->stick_x = wStickX;
			}
		}

		if (pad->stick_y == 0) {
			if (dwPressedButtons[slot] & BTN_STICKDOWN) {
				pad->stick_y = -128;
			}
			else if (dwPressedButtons[slot] & BTN_STICKUP) {
				pad->stick_y = 127;
			}
			else {
				pad->stick_y = wStickY;
			}
		}

		pad->gyro_x = wGyroX;
		pad->gyro_y = wGyroY;
	}

	void Controller::SetButtonMapping(int slot, int32_t n64Button, int32_t dwScancode) {
		std::unordered_map<int32_t, int32_t>& Mappings = profiles[slot].Mappings;
		std::erase_if(Mappings, [n64Button](const std::pair<int32_t, int32_t>& bin) { return bin.second == n64Button; });
		Mappings[dwScancode] = n64Button;
	}
}