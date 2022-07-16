#pragma once

#include <string>

#include "Lib/ImGui/imgui.h"

namespace Ship {

	class OcarinaEditor {
	private:
		float TableCellWidth = 300.0f; //1 Col
	public:
		bool Opened = false;
		void DrawHud();
	};
}