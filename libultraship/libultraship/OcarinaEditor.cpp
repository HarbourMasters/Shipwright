#include "OcarinaEditor.h"
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
	void OcarinaEditor::addButton(uint32_t mask, const char* name) {
		buttons.push_back(std::make_pair(mask, name));
		buttonmap[mask] = std::prev(buttons.end());
	}

	void OcarinaEditor::Init() {
		addButton(BTN_A,		"A");
		addButton(BTN_CUP,		"C Up");
		addButton(BTN_CDOWN,	"C Down");
		addButton(BTN_CLEFT,	"C Left");
		addButton(BTN_CRIGHT,	"C Right");
		addButton(BTN_L,		"L");
		addButton(BTN_Z,		"Z");
		addButton(BTN_R,		"R");
		addButton(BTN_START,	"Start");
		addButton(0,			"None");
	}

	void OcarinaEditor::DrawMapping(const char* label, const char* id, uint32_t n64Btn, float width) {
		const char* cVar = StringHelper::Sprintf("gOcarina%sBtnMap", id).c_str();
		uint32_t currentButton = CVar_GetS32(cVar, n64Btn);

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

		ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
		if (ImGui::BeginCombo(StringHelper::Sprintf("##OcarinaMap%s", id).c_str(), preview)) {
			for (auto i = buttons.begin(); i != buttons.end(); i++) {
				if (ImGui::Selectable(i->second, i->first == currentButton)) {
					CVar_SetS32(cVar, i->first);
				}
			}
			ImGui::EndCombo();
		}
	}

	void OcarinaEditor::DrawHud() {
		if (!this->Opened) {
			return;
		}

		ImGui::SetNextWindowSizeConstraints(ImVec2(300, 0), ImVec2(1200, 290));
		if (!ImGui::Begin("Ocarina Configuration", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::End();
			return;
		}

		if (ImGui::BeginTable("tableOcarinaScheme", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_Hideable)) {
			ImGui::TableSetupColumn("N64 Scheme", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TableCellWidth);
			ImGui::TableSetupColumn("Custom Scheme", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TableCellWidth);
			Table_InitHeader();
			Draw_HelpIcon("Play the ocarina with A and the C buttons");
			SohImGui::EnhancementRadioButton("N64 Controls", "gOcarinaControls", 0);
			Table_NextCol();
			Draw_HelpIcon("Customize the ocarina controls to your liking");
			SohImGui::EnhancementRadioButton("Custom Controls", "gOcarinaControls", 1);
			ImGui::EndTable();
		}

		if (CVar_GetS32("gOcarinaControls", 0) == 1) {
			float width;

			SohImGui::BeginGroupPanel("Notes", ImVec2(158, 20));
				width = ImGui::CalcTextSize("D5").x + 10;
				DrawMapping("D5", "D5", BTN_CUP, width);
				DrawMapping("B4", "B4", BTN_CLEFT, width);
				DrawMapping("A4", "A4", BTN_CRIGHT, width);
				DrawMapping("F4", "F4", BTN_CDOWN, width);
				DrawMapping("D4", "D4", BTN_A, width);
				ImGui::Dummy(ImVec2(0, 5));
			SohImGui::EndGroupPanel();

			ImGui::SameLine();

			SohImGui::BeginGroupPanel("Misc.", ImVec2(158, 20));
				width = ImGui::CalcTextSize("Play sharp note").x + 10;
				DrawMapping("Disable songs", "Disable", BTN_L, width);
				DrawMapping("Play sharp note", "Sharp", BTN_R, width);
				DrawMapping("Play flat note", "Flat", BTN_Z, width);
				ImGui::Dummy(ImVec2(0, 5));
			SohImGui::EndGroupPanel(48);
		} else {
			ImGui::TextWrapped("To modify ocarina controls, select \"Custom Controls\" from the menu at the top.");
		}

		ImGui::End();
	}
}