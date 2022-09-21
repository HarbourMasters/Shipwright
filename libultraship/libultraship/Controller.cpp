#include "Controller.h"
#include <memory>
#include <algorithm>
#include "Cvar.h"
#if __APPLE__
#include <SDL_events.h>
#else
#include <SDL2/SDL_events.h>
#endif

namespace Ship {

	Controller::Controller() : isRumbling(false) {
		Attachment = nullptr;

		for(int32_t virtualSlot = 0; virtualSlot < MAXCONTROLLERS; virtualSlot++) {
			profiles[virtualSlot] = std::make_shared<DeviceProfile>();
			ButtonData[virtualSlot] = std::make_shared<Buttons>();
		}
	}

	void Controller::Read(OSContPad* pad, int32_t virtualSlot) {
		ReadFromSource(virtualSlot);

		OSContPad padToBuffer = { 0 };

#ifndef __WIIU__
		SDL_PumpEvents();
#endif

		// Button Inputs
		padToBuffer.button |= getPressedButtons(virtualSlot) & 0xFFFF;

		// Stick Inputs
		if (getLeftStickX(virtualSlot) == 0) {
			if (getPressedButtons(virtualSlot) & BTN_STICKLEFT) {
				padToBuffer.stick_x = -128;
			} else if (getPressedButtons(virtualSlot) & BTN_STICKRIGHT) {
				padToBuffer.stick_x = 127;
			}
		} else {
			padToBuffer.stick_x = getLeftStickX(virtualSlot);
		}

		if (getLeftStickY(virtualSlot) == 0) {
			if (getPressedButtons(virtualSlot) & BTN_STICKDOWN) {
				padToBuffer.stick_y = -128;
			} else if (getPressedButtons(virtualSlot) & BTN_STICKUP) {
				padToBuffer.stick_y = 127;
			}
		} else {
			padToBuffer.stick_y = getLeftStickY(virtualSlot);
		}

		// Stick Inputs
		if (getRightStickX(virtualSlot) == 0) {
			if (getPressedButtons(virtualSlot) & BTN_VSTICKLEFT) {
				padToBuffer.right_stick_x = -128;
			} else if (getPressedButtons(virtualSlot) & BTN_VSTICKRIGHT) {
				padToBuffer.right_stick_x = 127;
			}
		} else {
			padToBuffer.right_stick_x = getRightStickX(virtualSlot);
		}

		if (getRightStickY(virtualSlot) == 0) {
			if (getPressedButtons(virtualSlot) & BTN_VSTICKDOWN) {
				padToBuffer.right_stick_y = -128;
			} else if (getPressedButtons(virtualSlot) & BTN_VSTICKUP) {
				padToBuffer.right_stick_y = 127;
			}
		} else {
			padToBuffer.right_stick_y = getRightStickY(virtualSlot);
		}

		// Gyro
		padToBuffer.gyro_x = getGyroX(virtualSlot);
		padToBuffer.gyro_y = getGyroY(virtualSlot);

		padBuffer.push_front(padToBuffer);
		if (pad != nullptr) {
			auto &padFromBuffer = padBuffer[std::min(padBuffer.size() - 1, (size_t)CVar_GetS32("gSimulatedInputLag", 0))];
			pad->button |= padFromBuffer.button;
			if (pad->stick_x == 0) pad->stick_x = padFromBuffer.stick_x;
			if (pad->stick_y == 0) pad->stick_y = padFromBuffer.stick_y;
			if (pad->gyro_x == 0) pad->gyro_x = padFromBuffer.gyro_x;
			if (pad->gyro_y == 0) pad->gyro_y = padFromBuffer.gyro_y;
			if (pad->right_stick_x == 0) pad->right_stick_x = padFromBuffer.right_stick_x;
			if (pad->right_stick_y == 0) pad->right_stick_y = padFromBuffer.right_stick_y;
		}

		while (padBuffer.size() > 6) {
			padBuffer.pop_back();
		}
	}

	void Controller::SetButtonMapping(int32_t virtualSlot, int32_t n64Button, int32_t dwScancode) {
		std::map<int32_t, int32_t>& Mappings = getProfile(virtualSlot)->Mappings;
		std::erase_if(Mappings, [n64Button](const std::pair<int32_t, int32_t>& bin) { return bin.second == n64Button; });
		Mappings[dwScancode] = n64Button;
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

	std::shared_ptr<DeviceProfile> Controller::getProfile(int32_t virtualSlot) {
		return profiles[virtualSlot];
	}

	std::shared_ptr<ControllerAttachment> Controller::GetAttachment() {
		return Attachment;
	}

	bool Controller::IsRumbling() {
		return isRumbling;
	}

	std::string Controller::GetGuid() {
		return GUID;
	}
}
