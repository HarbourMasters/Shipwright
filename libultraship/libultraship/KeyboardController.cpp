#include "KeyboardController.h"

#if __APPLE__
#include <SDL_keyboard.h>
#else
#include <SDL2/SDL_keyboard.h>
#endif

#include "Hooks.h"

#include "Window.h"

namespace Ship {

	KeyboardController::KeyboardController() : Controller(), lastScancode(-1) {
		GUID = "Keyboard";
	}

	bool KeyboardController::PressButton(int32_t dwScancode) {
		lastKey = dwScancode;
		
		for (int32_t virtualSlot = 0; virtualSlot < MAXCONTROLLERS; virtualSlot++) {

			if (getProfile(virtualSlot)->Mappings.contains(dwScancode)) {
				getPressedButtons(virtualSlot) |= getProfile(virtualSlot)->Mappings[dwScancode];
				return true;
			}
		}

		return false;
	}

	bool KeyboardController::ReleaseButton(int32_t dwScancode) {
		for (int32_t virtualSlot = 0; virtualSlot < MAXCONTROLLERS; virtualSlot++) {
			if (getProfile(virtualSlot)->Mappings.contains(dwScancode)) {
				getPressedButtons(virtualSlot) &= ~getProfile(virtualSlot)->Mappings[dwScancode];
				return true;
			}
		}

		return false;
	}

	void KeyboardController::ReleaseAllButtons() {
		for(int32_t virtualSlot = 0; virtualSlot < MAXCONTROLLERS; virtualSlot++) {
			getPressedButtons(virtualSlot) = 0;
		}
	}

	void KeyboardController::ReadFromSource(int32_t virtualSlot) {
		getLeftStickX(virtualSlot) = 0;
		getLeftStickY(virtualSlot) = 0;
		getRightStickX(virtualSlot) = 0;
		getRightStickY(virtualSlot) = 0;
	}

	int32_t KeyboardController::ReadRawPress() {
		return lastKey;
	}


	void KeyboardController::WriteToSource(int32_t virtualSlot, ControllerCallback* controller) {

	}

	const std::string KeyboardController::GetButtonName(int32_t virtualSlot, int32_t n64Button) {
		std::map<int32_t, int32_t>& Mappings = getProfile(virtualSlot)->Mappings;
		const auto find = std::find_if(Mappings.begin(), Mappings.end(), [n64Button](const std::pair<int32_t, int32_t>& pair) {
			return pair.second == n64Button;
		});

		if (find == Mappings.end()) return "Unknown";
		const char* name = Window::GetInstance()->GetKeyName(find->first);
		return strlen(name) == 0 ? "Unknown" : name;
	}

	void KeyboardController::CreateDefaultBinding(int32_t virtualSlot) {
		auto profile = getProfile(virtualSlot);
		profile->Mappings[0x14D] = BTN_CRIGHT;
		profile->Mappings[0x14B] = BTN_CLEFT;
		profile->Mappings[0x150] = BTN_CDOWN;
		profile->Mappings[0x148] = BTN_CUP;
		profile->Mappings[0x13] = BTN_R;
		profile->Mappings[0x12] = BTN_L;
		profile->Mappings[0x023] = BTN_DRIGHT;
		profile->Mappings[0x021] = BTN_DLEFT;
		profile->Mappings[0x022] = BTN_DDOWN;
		profile->Mappings[0x014] = BTN_DUP;
		profile->Mappings[0x039] = BTN_START;
		profile->Mappings[0x02C] = BTN_Z;
		profile->Mappings[0x02E] = BTN_B;
		profile->Mappings[0x02D] = BTN_A;
		profile->Mappings[0x020] = BTN_STICKRIGHT;
		profile->Mappings[0x01E] = BTN_STICKLEFT;
		profile->Mappings[0x01F] = BTN_STICKDOWN;
		profile->Mappings[0x011] = BTN_STICKUP;
		profile->Mappings[0x02A] = BTN_MODIFIER1;
		profile->Mappings[0x036] = BTN_MODIFIER2;
	}

	const std::string KeyboardController::GetControllerName() {
		return "Keyboard";
	}

	bool KeyboardController::Connected() const {
		return true;
	}

	bool KeyboardController::CanRumble() const {
		return false;
	}

	bool KeyboardController::CanGyro() const {
		return false;
	}

	void KeyboardController::ClearRawPress() {
		lastKey = -1;
	}

	void KeyboardController::SetLastScancode(int32_t key) {
		lastScancode = key;
	}

	int32_t KeyboardController::GetLastScancode() {
		return lastScancode;
	}
}
