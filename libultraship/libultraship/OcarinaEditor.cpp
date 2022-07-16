#include "OcarinaEditor.h"
#include "Lib/ImGui/imgui.h"
#include "ImGuiImpl.h"
#include "Lib/ImGui/imgui_internal.h"

#include <iostream>

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

	void OcarinaEditor::DrawHud() {
		if (!this->Opened) {
			return;
		}

		ImGui::SetNextWindowSizeConstraints(ImVec2(500, 430), ImVec2(1200, 290));
		if (!ImGui::Begin("Ocarina Configuration", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::End();
			return;
		}

		if (ImGui::BeginTable("tableOcarinaScheme", 3, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_Hideable)) {
			ImGui::TableSetupColumn("N64 Scheme", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TableCellWidth);
			ImGui::TableSetupColumn("3DS Scheme", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TableCellWidth);
			ImGui::TableSetupColumn("Custom Scheme", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TableCellWidth);
			Table_InitHeader();
			Draw_HelpIcon("Play the ocarina with A and the C buttons");
			SohImGui::EnhancementRadioButton("N64 Controls", "gOcarinaControls", 0);
			Table_NextCol();
			Draw_HelpIcon("Play the ocarina with A, X, Y, L, and R (using Nintendo's mapping)");
			SohImGui::EnhancementRadioButton("3DS Controls", "gOcarinaControls", 1);
			Table_NextCol();
			Draw_HelpIcon("Customize the ocarina controls to your liking");
			SohImGui::EnhancementRadioButton("Custom Controls", "gOcarinaControls", 2);
			ImGui::EndTable();
		}

		ImGui::End();
	}
}