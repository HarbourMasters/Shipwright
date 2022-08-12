#pragma once

#include "Lib/ImGui/imgui.h"
#include "Controller.h"
#include "UltraController.h"

namespace Ship {

	class InputEditor {
		int32_t CurrentPort = 0;
		int32_t BtnReading = -1;

		// values from UltraController.h
		// TODO: figure out why including UltraController.h and using the defined values isn't working
		typedef enum {
			PRIMARY_ANALOG_STICK_UP = 0x80000,
			PRIMARY_ANALOG_STICK_DOWN = 0x40000,
			PRIMARY_ANALOG_STICK_LEFT = 0x10000,
			PRIMARY_ANALOG_STICK_RIGHT = 0x20000,
			SECONDARY_ANALOG_STICK_UP = 0x100000,
			SECONDARY_ANALOG_STICK_DOWN = 0x200000,
			SECONDARY_ANALOG_STICK_LEFT = 0x400000,
			SECONDARY_ANALOG_STICK_RIGHT = 0x800000,
			GYRO_X,
			GYRO_Y
		} InputAxisDirection;

	public:
		bool Opened = false;
		void Init();
		void DrawButton(const char* label, int n64Btn);
		void DrawVirtualStick(const char* label,
							  int32_t CurrentPort,
							  InputAxisDirection yAxisUp,
							  InputAxisDirection yAxisDown,
							  InputAxisDirection xAxisLeft,
							  InputAxisDirection xAxisRight);
		void DrawControllerSchema();
		void DrawHud();
	};
}
