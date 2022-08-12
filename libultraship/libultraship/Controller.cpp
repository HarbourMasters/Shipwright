#include "Controller.h"
#include <memory>
#include <algorithm>
#if __APPLE__
#include <SDL_events.h>
#else
#include <SDL2/SDL_events.h>
#endif

namespace Ship {

	Controller::Controller() : isRumbling(false) {
		Attachment = nullptr;

		for(int32_t virtualSlot = 0; virtualSlot < MAXCONTROLLERS; virtualSlot++) {
			profiles[virtualSlot] = std::make_shared<DeviceProfileV0>();
			ButtonData[virtualSlot] = std::make_shared<Buttons>();
		}
	}

	void Controller::Read(OSContPad* pad, int32_t virtualSlot) {
		ReadFromSource(virtualSlot);

		SDL_PumpEvents();

		// Button Inputs
		pad->button |= getPressedButtons(virtualSlot) & 0xFFFF;

		// Stick Inputs
		if (getLeftStickX(virtualSlot) == 0) {
			if (getPressedButtons(virtualSlot) & BTN_STICKLEFT) {
				pad->stick_x = -128;
			} else if (getPressedButtons(virtualSlot) & BTN_STICKRIGHT) {
				pad->stick_x = 127;
			}
		} else {
			pad->stick_x = getLeftStickX(virtualSlot);
		}

		if (getLeftStickY(virtualSlot) == 0) {
			if (getPressedButtons(virtualSlot) & BTN_STICKDOWN) {
				pad->stick_y = -128;
			} else if (getPressedButtons(virtualSlot) & BTN_STICKUP) {
				pad->stick_y = 127;
			}
		} else {
			pad->stick_y = getLeftStickY(virtualSlot);
		}

		// Stick Inputs
		if (getRightStickX(virtualSlot) == 0) {
			if (getPressedButtons(virtualSlot) & BTN_VSTICKLEFT) {
				pad->right_stick_x = -128;
			} else if (getPressedButtons(virtualSlot) & BTN_VSTICKRIGHT) {
				pad->right_stick_x = 127;
			}
		} else {
			pad->right_stick_x = getRightStickX(virtualSlot);
		}

		if (getRightStickY(virtualSlot) == 0) {
			if (getPressedButtons(virtualSlot) & BTN_VSTICKDOWN) {
				pad->right_stick_y = -128;
			} else if (getPressedButtons(virtualSlot) & BTN_VSTICKUP) {
				pad->right_stick_y = 127;
			}
		} else {
			pad->right_stick_y = getRightStickY(virtualSlot);
		}

		// Gyro
		pad->gyro_x = getGyroX(virtualSlot);
		pad->gyro_y = getGyroY(virtualSlot);
	}

	void Controller::SetButtonMapping(int32_t virtualSlot, int32_t n64Button, int32_t dwScancode) {
		std::map<int32_t, int32_t>& Mappings = getProfile(virtualSlot)->Mappings;
		std::erase_if(Mappings, [n64Button](const std::pair<int32_t, int32_t>& bin) { return bin.second == n64Button; });
		Mappings[dwScancode] = n64Button;
	}

    int32_t Controller::GetMappedButton(int32_t virtualSlot, int32_t n64Button) {
        std::map<int32_t, int32_t>& Mappings = getProfile(virtualSlot)->Mappings;

        const auto find = std::find_if(Mappings.begin(), Mappings.end(), [n64Button](const std::pair<int32_t, int32_t>& pair) {
            return pair.second == n64Button;
        });

        if (find == Mappings.end()) {
            // SDL_CONTROLLER_BUTTON_INVALID = -1
            return -1;
        }

        return find->first;
    }

	int8_t& Controller::getLeftStickX(int32_t virtualSlot) {
		return ButtonData[virtualSlot]->leftStickX;
	}

	int8_t& Controller::getLeftStickY(int32_t virtualSlot) {
		return ButtonData[virtualSlot]->leftStickY;
	}

	int8_t& Controller::getRightStickX(int32_t virtualSlot) {
		return ButtonData[virtualSlot]->rightStickX;
	}

	int8_t& Controller::getRightStickY(int32_t virtualSlot) {
		return ButtonData[virtualSlot]->rightStickY;
	}

	int32_t& Controller::getPressedButtons(int32_t virtualSlot) {
		return ButtonData[virtualSlot]->pressedButtons;
	}

	float& Controller::getGyroX(int32_t virtualSlot) {
		return ButtonData[virtualSlot]->gyroX;
	}

	float& Controller::getGyroY(int32_t virtualSlot) {
		return ButtonData[virtualSlot]->gyroY;
	}

	std::shared_ptr<DeviceProfileV0> Controller::getProfile(int32_t virtualSlot) {
		return profiles[virtualSlot];
	}
}
