#pragma once

#include "stdint.h"
#include "Lib/ImGui/imgui.h"

namespace Ship {

	class InputEditor {
		int32_t CurrentPort = 0;
		int32_t BtnReading = -1;
		bool Opened = false;
	public:
		void Init();
		void DrawButton(const char* label, int32_t n64Btn, int32_t currentPort, int32_t* btnReading);
		void DrawControllerSelect(int32_t currentPort);
		void DrawVirtualStick(const char* label, ImVec2 stick);
		void DrawControllerSchema();
		void DrawHud();
		bool IsOpened();
		void Open();
		void Close();
	};
}
