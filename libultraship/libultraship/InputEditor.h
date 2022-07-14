#pragma once

#include <string>

#include "Lib/ImGui/imgui.h"

namespace Ship {

	class InputEditor {
		int CurrentPort = 0;
		int BtnReading = -1;
	public:
		bool Opened = false;
		void Init();
		void DrawButton(const char* label, int n64Btn);
		void DrawVirtualStick(const char* label, ImVec2 stick);
		void DrawControllerSchema();
		void DrawHud();
	};
}
