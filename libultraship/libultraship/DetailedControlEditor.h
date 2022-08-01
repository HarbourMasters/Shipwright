#pragma once

#include <string>
#include <unordered_map>
#include <list>
#include <utility>

#include "Lib/ImGui/imgui.h"

namespace Ship {
	typedef struct {
		const char* label;
		const char* cVarName;
		uint32_t defaultBtn;
	} CustomButtonMap;

	class DetailedControlEditor {
	private:
		std::list<std::pair<uint32_t, const char*>> buttons;
		std::unordered_map<uint32_t, decltype(buttons)::iterator> buttonmap;
		void addButton(uint32_t, const char*);

	public:
		bool Opened = false;
		void Init();
		void DrawHud();
		void DrawMapping(CustomButtonMap&, float labelWidth = 0, uint32_t removedButtons = 0);
		void DrawOcarinaControlPanel();
	};
}
