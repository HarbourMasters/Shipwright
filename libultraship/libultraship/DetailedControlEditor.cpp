#include "DetailedControlEditor.h"
#include "Lib/ImGui/imgui.h"
#include "ImGuiImpl.h"
#include "Lib/ImGui/imgui_internal.h"
#include "Cvar.h"
#include "UltraController.h"
#include "Utils/StringHelper.h"

namespace Ship {

	float TableCellWidth = 300.0f; //1 Col

	void Table_InitHeader(bool has_header = true) {
		if (has_header) {
			ImGui::TableHeadersRow();
		}
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding(); //This is to adjust Vertical pos of item in a cell to be normlized.
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	}

	void Table_NextCol() {
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	}

	void Draw_HelpIcon(const std::string& helptext, bool sameline = true, int Pos = 0) {
		// place the ? button to the most of the right side of the cell it is using.
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 15);
		ImGui::SmallButton("?");
		SohImGui::Tooltip(helptext.c_str());
		if (sameline) {
			//I do not use ImGui::SameLine(); because it make some element vanish.
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 22);
		}
	}

	// Assumes only addition since this is only used to construct the button map
	// So this does not work like a proper linked hash map's insertion
	void DetailedControlEditor::addButton(uint32_t mask, const char* name) {
		buttons.push_back(std::make_pair(mask, name));
		buttonmap[mask] = std::prev(buttons.end());
	}

	void DetailedControlEditor::Init() {
		addButton(BTN_A,		"A");
		addButton(BTN_CUP,		"C Up");
		addButton(BTN_CDOWN,	"C Down");
		addButton(BTN_CLEFT,	"C Left");
		addButton(BTN_CRIGHT,	"C Right");
		addButton(BTN_L,		"L");
		addButton(BTN_Z,		"Z");
		addButton(BTN_R,		"R");
		addButton(BTN_START,	"Start");
		addButton(BTN_DUP,		"D-pad up");
		addButton(BTN_DDOWN,	"D-pad down");
		addButton(BTN_DLEFT,	"D-pad left");
		addButton(BTN_DRIGHT,	"D-pad right");
		addButton(0,			"None");
	}

	void DetailedControlEditor::DrawOcarinaMapping(const char* label, const char* id, uint32_t n64Btn, float width) {
		std::string cVar = StringHelper::Sprintf("gOcarina%sBtnMap", id);
		uint32_t currentButton = CVar_GetS32(cVar.c_str(), n64Btn);

		const char* preview;
		if (buttonmap.contains(currentButton)) {
			preview = buttonmap[currentButton]->second;
		} else {
			preview = "";
		}

		ImVec2 len = ImGui::CalcTextSize(label);
		ImVec2 pos = ImGui::GetCursorPos();
		ImGui::SetCursorPosY(pos.y + len.y / 4);
		ImGui::SetCursorPosX(pos.x + abs(len.x - width));
		ImGui::Text("%s", label);
		ImGui::SameLine();
		ImGui::SetCursorPosY(pos.y);

		bool dpad = CVar_GetS32("gDpadOcarina", 0);
		uint32_t dpadMask = BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT;

		ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
		if (ImGui::BeginCombo(StringHelper::Sprintf("##OcarinaMap%s", id).c_str(), preview)) {
			for (auto i = buttons.begin(); i != buttons.end(); i++) {
				if (dpad && (i->first & dpadMask) != 0) {  // Hide d-pad options if d-pad check button set
					continue;
				}
				if (ImGui::Selectable(i->second, i->first == currentButton)) {
					CVar_SetS32(cVar.c_str(), i->first);
				}
			}
			ImGui::EndCombo();
		}
	}

	void DetailedControlEditor::DrawOcarinaControlPanel() {
		SohImGui::BeginGroupPanel("Ocarina Controls");

		float column2x;
		if (ImGui::BeginTable("tableOcarinaScheme", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_Hideable)) {
			ImGui::TableSetupColumn("N64 Scheme", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TableCellWidth);
			ImGui::TableSetupColumn("Custom Scheme", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TableCellWidth);
			Table_InitHeader();
			Draw_HelpIcon("Play the ocarina with A and the C buttons");
			SohImGui::EnhancementRadioButton("N64 Controls", "gCustomOcarinaControls", 0);
			Table_NextCol();
			column2x = ImGui::GetCursorPosX();
			Draw_HelpIcon("Customize the ocarina controls to your liking");
			SohImGui::EnhancementRadioButton("Custom Controls", "gCustomOcarinaControls", 1);
			ImGui::EndTable();
		}

		if (CVar_GetS32("gCustomOcarinaControls", 0) == 1) {
			float width;

			SohImGui::BeginGroupPanel("Notes", ImVec2(158, 20));
				width = ImGui::CalcTextSize("D5").x + 10;
				DrawOcarinaMapping("D5", "D5", BTN_CUP, width);
				DrawOcarinaMapping("B4", "B4", BTN_CLEFT, width);
				DrawOcarinaMapping("A4", "A4", BTN_CRIGHT, width);
				DrawOcarinaMapping("F4", "F4", BTN_CDOWN, width);
				DrawOcarinaMapping("D4", "D4", BTN_A, width);
				ImGui::Dummy(ImVec2(0, 5));
			SohImGui::EndGroupPanel(33);

			ImGui::SameLine();

			const ImVec2 cursor = ImGui::GetCursorPos();
			SohImGui::BeginGroupPanel("Misc.", ImVec2(158, 20));
				width = ImGui::CalcTextSize("Disable songs").x + 10;
				DrawOcarinaMapping("Disable songs", "Disable", BTN_L, width);
				DrawOcarinaMapping("Pitch up", "Sharp", BTN_R, width);
				DrawOcarinaMapping("Pitch down", "Flat", BTN_Z, width);
				ImGui::Dummy(ImVec2(0, 5));
			SohImGui::EndGroupPanel();

			ImGui::SetCursorPosX(cursor.x);
			ImGui::SetCursorPosY(cursor.y + 100);
			SohImGui::BeginGroupPanel("Alternate controls", ImVec2(225.5, 20));
				float cursorX = ImGui::GetCursorPosX() + 10;
				ImGui::SetCursorPosX(cursorX);
				SohImGui::EnhancementCheckbox("Play with D-pad", "gDpadOcarina");
				ImGui::SetCursorPosX(cursorX);
				SohImGui::EnhancementCheckbox("Play with camera stick", "gRStickOcarina");
				ImGui::Dummy(ImVec2(0, 5));
			SohImGui::EndGroupPanel();
		} else {
			ImGui::Dummy(ImVec2(0, 5));
			SohImGui::EnhancementCheckbox("Play with D-pad", "gDpadOcarina");
			ImGui::SameLine();
			ImGui::SetCursorPosX(column2x);
			SohImGui::EnhancementCheckbox("Play with camera stick", "gRStickOcarina");
			ImGui::Dummy(ImVec2(0, 5));
			ImGui::TextWrapped("To further modify ocarina controls, select \"Custom Controls\" from the buttons above.");
		}

		SohImGui::EndGroupPanel(6);
	}

	void DetailedControlEditor::DrawHud() {
		if (!this->Opened) {
			return;
		}

		ImGui::SetNextWindowSizeConstraints(ImVec2(420, 0), ImVec2(1200, 290));
		if (ImGui::Begin("Detailed Control Configuration", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
			DrawOcarinaControlPanel();
		}
		ImGui::End();
	}
}