#include "KeyboardController.h"

#if __APPLE__
#include <SDL_keyboard.h>
#else
#include <SDL2/SDL_keyboard.h>
#endif

#include "Hooks.h"
#include "GlobalCtx2.h"
#include "Window.h"

namespace Ship {

	KeyboardController::KeyboardController() : Controller(), lastScancode(-1) {
		GUID = "Keyboard";
	}

	bool KeyboardController::PressButton(int32_t dwScancode) {

		lastKey = dwScancode;
		
		for (int slot = 0; slot < MAXCONTROLLERS; slot++) {

			if (profiles[slot].Mappings.contains(dwScancode)) {
				dwPressedButtons[slot] |= profiles[slot].Mappings[dwScancode];
				return true;
			}
		}

		return false;
	}

	bool KeyboardController::ReleaseButton(int32_t dwScancode) {
		for (int slot = 0; slot < MAXCONTROLLERS; slot++) {
			if (profiles[slot].Mappings.contains(dwScancode)) {
				dwPressedButtons[slot] &= ~profiles[slot].Mappings[dwScancode];
				return true;
			}
		}

		return false;
	}

	void KeyboardController::ReleaseAllButtons() {
		for(int slot = 0; slot < MAXCONTROLLERS; slot++) {
			dwPressedButtons[slot] = 0;
		}
	}

	void KeyboardController::ReadFromSource(int32_t slot) {
		wStickX = 0;
		wStickY = 0;
		wCamX = 0;
		wCamY = 0;
	}

	int32_t KeyboardController::ReadRawPress() {
		return lastKey;
	}


	void KeyboardController::WriteToSource(int32_t slot, ControllerCallback* controller)
	{

	}

	const char* KeyboardController::GetButtonName(int slot, int n64Button) {
		std::map<int32_t, int32_t>& Mappings = profiles[slot].Mappings;
		const auto find = std::find_if(Mappings.begin(), Mappings.end(), [n64Button](const std::pair<int32_t, int32_t>& pair) {
			return pair.second == n64Button;
		});

		if (find == Mappings.end()) return "Unknown";
		const char* name = GlobalCtx2::GetInstance()->GetWindow()->GetKeyName(find->first);
		return strlen(name) == 0 ? "Unknown" : name;
	}


	void KeyboardController::CreateDefaultBinding(int32_t slot) {
		DeviceProfile& profile = profiles[slot];
		profile.Mappings[0x14D] = BTN_CRIGHT;
		profile.Mappings[0x14B] = BTN_CLEFT;
		profile.Mappings[0x150] = BTN_CDOWN;
		profile.Mappings[0x148] = BTN_CUP;
		profile.Mappings[0x13]  = BTN_R;
		profile.Mappings[0x12]  = BTN_L;
		profile.Mappings[0x023] = BTN_DRIGHT;
		profile.Mappings[0x021] = BTN_DLEFT;
		profile.Mappings[0x022] = BTN_DDOWN;
		profile.Mappings[0x014] = BTN_DUP;
		profile.Mappings[0x039] = BTN_START;
		profile.Mappings[0x02C] = BTN_Z;
		profile.Mappings[0x02E] = BTN_B;
		profile.Mappings[0x02D] = BTN_A;
		profile.Mappings[0x020] = BTN_STICKRIGHT;
		profile.Mappings[0x01E] = BTN_STICKLEFT;
		profile.Mappings[0x01F] = BTN_STICKDOWN;
		profile.Mappings[0x011] = BTN_STICKUP;
	}

	const char* KeyboardController::GetControllerName() {
		return "Keyboard";
	}
}
