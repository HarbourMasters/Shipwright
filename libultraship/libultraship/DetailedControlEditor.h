#pragma once

#include <string>
#include <unordered_map>
#include <list>
#include <utility>

#include "Lib/ImGui/imgui.h"

namespace Ship {

	class DetailedControlEditor {
	private:
		std::list<std::pair<uint32_t, const char*>> buttons;
		std::unordered_map<uint32_t, decltype(buttons)::iterator> buttonmap;
		void addButton(uint32_t, const char*);

	public:
		bool Opened = false;
		void Init();
		void DrawHud();
		void DrawOcarinaMapping(const char*, const char*, uint32_t, float);
		void DrawOcarinaControlPanel();
	};
}
