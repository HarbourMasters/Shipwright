#pragma once

#include "Lib/ImGui/imgui.h"
#include "Controller.h"

namespace Ship {

	class InputEditor {
		int CurrentPort = 0;
		int BtnReading = -1;

		typedef enum {
		/* 0 */ LEFT_ANALOG_STICK,
		/* 1 */ RIGHT_ANALOG_STICK
		} AnalogStick;
	public:
		bool Opened = false;
		void Init();
		void DrawButton(const char* label, int n64Btn);
		void DrawVirtualStick(const char* label, std::shared_ptr<Ship::Controller> physicalDevice, AnalogStick stick);
		void DrawControllerSchema();
		void DrawHud();
	};
}
