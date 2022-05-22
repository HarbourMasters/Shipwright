#include "Controller.h"
#include "GlobalCtx2.h"
#include "stox.h"
#include <memory>
#include <SDL2/SDL_events.h>

namespace Ship {
	Controller::Controller(int32_t dwControllerNumber) : dwControllerNumber(dwControllerNumber) {
		dwPressedButtons = 0;
		wStickX = 0;
		wStickY = 0;
		wGyroX = 0;
		wGyroY = 0;
		Attachment = nullptr;
	}

	SDL_Event ev;


	void Controller::Read(OSContPad* pad) {
		ReadFromSource();

		// Touch Inputs
		int x, y;
		Uint32 buttons;
		SDL_PumpEvents();
		buttons = SDL_GetMouseState(&x, &y);
		wTouchX = x;
		wTouchY = y;

		// Click Inputs
		if ((buttons & SDL_BUTTON_LMASK) != 0) {
			wLeftClick = 1;
		}
		else {
			wLeftClick = 0;
		}
		if ((buttons & SDL_BUTTON_RMASK) != 0) {
			wRightClick = 1;
		}
		else {
			wRightClick = 0;
		}

		// Mouse Inputs
		int x2, y2;
		Uint32 buttons2;
		SDL_PumpEvents();
		buttons2 = SDL_GetRelativeMouseState(&x2, &y2);
		wMouseMoveX = x2;
		wMouseMoveY = y2;

		// Button Inputs
		pad->button |= dwPressedButtons & 0xFFFF;

		// Stick Inputs
		if (pad->stick_x == 0) {
			if (dwPressedButtons & BTN_STICKLEFT) {
				pad->stick_x = -128;
			}
			else if (dwPressedButtons & BTN_STICKRIGHT) {
				pad->stick_x = 127;
			}
			else {
				pad->stick_x = wStickX;
			}
		}

		if (pad->stick_y == 0) {
			if (dwPressedButtons & BTN_STICKDOWN) {
				pad->stick_y = -128;
			}
			else if (dwPressedButtons & BTN_STICKUP) {
				pad->stick_y = 127;
			}
			else {
				pad->stick_y = wStickY;
			}
		}

		// Gyro
		pad->gyro_x = wGyroX;
		pad->gyro_y = wGyroY;

		// Right Stick
		pad->cam_x = wCamX;
		pad->cam_y = wCamY;
		
		// Click/Touch
		pad->touch_x = wTouchX;
		pad->touch_y = wTouchY;
		pad->left_click = wLeftClick;
		pad->right_click = wRightClick;

		// Mouse
		pad->mouse_move_x = wMouseMoveX;
		pad->mouse_move_y = wMouseMoveY;
	}

	void Controller::SetButtonMapping(const std::string& szButtonName, int32_t dwScancode) {
		// Update the config value.
		std::string ConfSection = GetBindingConfSection();
		std::shared_ptr<ConfigFile> pConf = GlobalCtx2::GetInstance()->GetConfig();
		ConfigFile& Conf = *pConf.get();
		Conf[ConfSection][szButtonName] = dwScancode;

		// Reload the button mapping from Config
		LoadBinding();
	}

	void Controller::LoadBinding() {
		std::string ConfSection = GetBindingConfSection();
		std::shared_ptr<ConfigFile> pConf = GlobalCtx2::GetInstance()->GetConfig();
		ConfigFile& Conf = *pConf.get();

		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_CRIGHT)])] = BTN_CRIGHT;
		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_CLEFT)])] = BTN_CLEFT;
		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_CDOWN)])] = BTN_CDOWN;
		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_CUP)])] = BTN_CUP;
		//ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_CRIGHT + "_2")])] = BTN_CRIGHT;
		//ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_CLEFT + "_2")])] = BTN_CLEFT;
		//ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_CDOWN + "_2")])] = BTN_CDOWN;
		//ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_CUP + "_2")])] = BTN_CUP;
		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_R)])] = BTN_R;
		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_L)])] = BTN_L;
		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_DRIGHT)])] = BTN_DRIGHT;
		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_DLEFT)])] = BTN_DLEFT;
		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_DDOWN)])] = BTN_DDOWN;
		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_DUP)])] = BTN_DUP;
		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_START)])] = BTN_START;
		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_Z)])] = BTN_Z;
		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_B)])] = BTN_B;
		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_A)])] = BTN_A;
		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_STICKRIGHT)])] = BTN_STICKRIGHT;
		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_STICKLEFT)])] = BTN_STICKLEFT;
		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_STICKDOWN)])] = BTN_STICKDOWN;
		ButtonMapping[Ship::stoi(Conf[ConfSection][STR(BTN_STICKUP)])] = BTN_STICKUP;
	}

	std::string Controller::GetConfSection() {
		return GetControllerType() + " CONTROLLER " + std::to_string(GetControllerNumber() + 1);
	}

	std::string Controller::GetBindingConfSection() {
		return GetControllerType() + " CONTROLLER BINDING " + std::to_string(GetControllerNumber() + 1);
	}
}
