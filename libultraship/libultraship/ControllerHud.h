#pragma once

#include <string>

#include "Lib/ImGui/imgui.h"

namespace Ship {

	enum HudButtons {
		B_A, B_B, B_L, B_R, B_Z, B_Start,
		D_Up, D_Down, D_Left, D_Right,
		S_Up, S_Down, S_Left, S_Right,
		C_Up, C_Down, C_Left, C_Right
	};

	class ControllerHud {
	private:
		int CurrentPort = 0;
		int BtnReading = -1;
	public:
		bool Opened = false;
		void Init();
		void DrawButton(const char* label, int n64Btn);
		void DrawVirtualStick(ImVec2 stick);
		void DrawControllerSchema();
		void DrawHud();
	};
}
