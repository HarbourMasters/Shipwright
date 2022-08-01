#include "DetailedControlEditor.h"
#include "Lib/ImGui/imgui.h"
#include "ImGuiImpl.h"
#include "Lib/ImGui/imgui_internal.h"
#include "Cvar.h"
#include "UltraController.h"
#include "Utils/StringHelper.h"

namespace Ship {

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
	
	void Table_NextLine() {
		ImGui::TableNextRow();
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

	// Ocarina custom button maps
	static CustomButtonMap ocarinaD5 = {"D5", "gOcarinaD5BtnMap", BTN_CUP};
	static CustomButtonMap ocarinaB4 = {"B4", "gOcarinaB4BtnMap", BTN_CLEFT};
	static CustomButtonMap ocarinaA4 = {"A4", "gOcarinaA4BtnMap", BTN_CRIGHT};
	static CustomButtonMap ocarinaF4 = {"F4", "gOcarinaF4BtnMap", BTN_CDOWN};
	static CustomButtonMap ocarinaD4 = {"D4", "gOcarinaD4BtnMap", BTN_A};
	static CustomButtonMap ocarinaSongDisable = {"Disable songs", "gOcarinaD5BtnMap", BTN_L};
	static CustomButtonMap ocarinaSharp = {"Pitch up", "gOcarinaD5BtnMap", BTN_R};
	static CustomButtonMap ocarinaFlat = {"Pitch down", "gOcarinaD5BtnMap", BTN_Z};

	// Assumes only addition since this is only used to construct the button map
	// So this does not work like a proper linked hash map's insertion
	void DetailedControlEditor::addButton(uint32_t mask, const char* name) {
		buttons.push_back(std::make_pair(mask, name));
		buttonmap[mask] = std::prev(buttons.end());
	}

	void DetailedControlEditor::Init() {
		addButton(BTN_A,		"A");
		addButton(BTN_B,		"B");
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

	// Draw a button mapping setting consisting of a padded label and button dropdown.
	// removedButtons is a mask that indicates which buttons are unavailable to choose from.
	void DetailedControlEditor::DrawMapping(CustomButtonMap& mapping, float labelWidth, uint32_t removedButtons) {
		uint32_t currentButton = CVar_GetS32(mapping.cVarName, mapping.defaultBtn);

		const char* preview;
		if (buttonmap.contains(currentButton)) {
			preview = buttonmap[currentButton]->second;
		} else {
			preview = "Unknown";
		}

		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImVec2 textSize = ImGui::CalcTextSize(mapping.label);
		ImGui::SetCursorPosY(cursorPos.y + textSize.y / 4);
		ImGui::SetCursorPosX(cursorPos.x + abs(textSize.x - labelWidth));
		ImGui::Text(mapping.label);
		ImGui::SameLine();
		ImGui::SetCursorPosY(cursorPos.y);

		ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
		if (ImGui::BeginCombo(mapping.cVarName, preview)) {
			for (auto i = buttons.begin(); i != buttons.end(); i++) {
				if ((i->first & removedButtons) != 0) {
					continue;
				}
				if (ImGui::Selectable(i->second, i->first == currentButton)) {
					CVar_SetS32(mapping.cVarName, i->first);
				}
			}
			ImGui::EndCombo();
		}
	}

	void DetailedControlEditor::DrawOcarinaControlPanel() {
		if (!ImGui::CollapsingHeader("Ocarina Controls")) {
			return;
		}

		if (!ImGui::BeginTable("tableCustomOcarinaControls", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
			return;
		}

		ImGui::TableSetupColumn("Custom Ocarina Controls", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort);
		Table_InitHeader(false);
		
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
		SohImGui::EnhancementCheckbox("Customize Ocarina Controls", "gCustomOcarinaControls");

		if (CVar_GetS32("gCustomOcarinaControls", 0) == 1) {
			if (ImGui::BeginTable("tableCustomMainOcarinaControls", 2, ImGuiTableFlags_SizingStretchProp)) {
				float labelWidth;
				uint32_t disableMask = BTN_B;
				if (CVar_GetS32("gDpadOcarina", 0)) {
					disableMask |= BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT;
				}

				ImGui::TableSetupColumn("Custom Ocarina Notes", ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort);
				ImGui::TableSetupColumn("Custom Ocarina Modifiers", ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort);
				Table_InitHeader(false);

				SohImGui::BeginGroupPanel("Notes", ImGui::GetContentRegionAvail());
				labelWidth = ImGui::CalcTextSize("D5").x + 10;
				DrawMapping(ocarinaD5, labelWidth, disableMask);
				DrawMapping(ocarinaB4, labelWidth, disableMask);
				DrawMapping(ocarinaA4, labelWidth, disableMask);
				DrawMapping(ocarinaF4, labelWidth, disableMask);
				DrawMapping(ocarinaD4, labelWidth, disableMask);
				ImGui::Dummy(ImVec2(0, 5));
				float cursorY = ImGui::GetCursorPosY();
				SohImGui::EndGroupPanel();

				Table_NextCol();

				SohImGui::BeginGroupPanel("Modifiers", ImGui::GetContentRegionAvail());
				labelWidth = ImGui::CalcTextSize(ocarinaSongDisable.label).x + 10;
				DrawMapping(ocarinaSongDisable, labelWidth, disableMask);
				DrawMapping(ocarinaSharp, labelWidth, disableMask);
				DrawMapping(ocarinaFlat, labelWidth, disableMask);
				ImGui::Dummy(ImVec2(0, 5));
				SohImGui::EndGroupPanel(cursorY - ImGui::GetCursorPosY() + 2);

				ImGui::EndTable();
			}
		} else {
			ImGui::Dummy(ImVec2(0, 5));
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
			ImGui::TextWrapped("To modify the main ocarina controls, select the \"Customize Ocarina Controls\" checkbox.");
			ImGui::Dummy(ImVec2(0, 5));
		}

		SohImGui::BeginGroupPanel("Alternate controls", ImGui::GetContentRegionAvail());
		if (ImGui::BeginTable("tableOcarinaAlternateControls", 2, ImGuiTableFlags_SizingFixedSame)) {
			ImGui::TableSetupColumn("D-pad", ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort);
			ImGui::TableSetupColumn("Right stick", ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort);
			Table_InitHeader(false);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
			SohImGui::EnhancementCheckbox("Play with D-pad", "gDpadOcarina");
			Table_NextCol();
			SohImGui::EnhancementCheckbox("Play with camera stick", "gRStickOcarina");
			ImGui::EndTable();
		}
		ImGui::Dummy(ImVec2(0, 5));
		SohImGui::EndGroupPanel();

		ImGui::EndTable();
	}

	void DetailedControlEditor::DrawHud() {
		if (!this->Opened) {
			return;
		}

		ImGui::SetNextWindowSize(ImVec2(420, 300), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Detailed Control Configuration", &this->Opened)) {
			DrawOcarinaControlPanel();
		}
		ImGui::End();
	}
}
