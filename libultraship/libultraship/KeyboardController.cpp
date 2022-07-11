#include "KeyboardController.h"

#include <SDL2/SDL_keyboard.h>

#include "Hooks.h"
#include "GlobalCtx2.h"

namespace Ship {
	KeyboardController::KeyboardController(int32_t dwControllerNumber) : Controller(dwControllerNumber) {
		LoadBinding();
	}

	KeyboardController::~KeyboardController() {
		
	}

	bool KeyboardController::PressButton(int32_t dwScancode) {
		ModInternal::ExecuteHooks<ModInternal::ControllerRawInput>(this, dwScancode);
		if (ButtonMapping.contains(dwScancode)) {
			dwPressedButtons |= ButtonMapping[dwScancode];
			return true;
		}

		return false;
	}

	bool KeyboardController::ReleaseButton(int32_t dwScancode) {
		if (ButtonMapping.contains(dwScancode)) {
			dwPressedButtons &= ~ButtonMapping[dwScancode];
			return true;
		}

		return false;
	}

	void KeyboardController::ReleaseAllButtons() {
		dwPressedButtons = 0;
	}

	void KeyboardController::ReadFromSource() {
		wStickX = 0;
		wStickY = 0;
	}

	void KeyboardController::WriteToSource(ControllerCallback* controller)
	{

	}

	const char* KeyboardController::GetButtonName(int button) {
		return SDL_GetScancodeName(static_cast<SDL_Scancode>(button));
	}


	DeviceProfile KeyboardController::GetDefaultMapping() {
		return {
			.Mappings = {
				SDL_SCANCODE_V, SDL_SCANCODE_B, SDL_SCANCODE_G, SDL_SCANCODE_H, SDL_SCANCODE_N, SDL_SCANCODE_RETURN,
				SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
				SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D,
				SDL_SCANCODE_I, SDL_SCANCODE_K, SDL_SCANCODE_J, SDL_SCANCODE_L
			}
		};
	}

	std::string KeyboardController::GetControllerType() {
		return "KEYBOARD";
	}

	std::string KeyboardController::GetConfSection() {
		return GetControllerType() + " CONTROLLER " + std::to_string(GetControllerNumber() + 1);
	}

	std::string KeyboardController::GetBindingConfSection() {
		return GetControllerType() + " CONTROLLER BINDING " + std::to_string(GetControllerNumber() + 1);
	}
}
