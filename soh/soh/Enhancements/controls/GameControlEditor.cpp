#include "GameControlEditor.h"

#include <string>
#include <list>
#include <unordered_map>
#include <utility>
#include <iterator>

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <libultraship/Cvar.h>
#include <libultraship/UltraController.h>
#include <Utils/StringHelper.h>
#include <libultraship/ImGuiImpl.h>

#include "../../UIWidgets.hpp"

namespace GameControlEditor {
    const ImGuiTableFlags PANEL_TABLE_FLAGS =
        ImGuiTableFlags_BordersH |
        ImGuiTableFlags_BordersV;
    const ImGuiTableColumnFlags PANEL_TABLE_COLUMN_FLAGS =
        ImGuiTableColumnFlags_IndentEnable |
        ImGuiTableColumnFlags_NoSort;

    namespace TableHelper {
        void InitHeader(bool has_header = true) {
            if (has_header) {
                ImGui::TableHeadersRow();
            }
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding(); //This is to adjust Vertical pos of item in a cell to be normlized.
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        }

        void NextCol() {
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        }

        void NextLine() {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        }
    }

    void DrawHelpIcon(const std::string& helptext) {
        // place the ? button to the most of the right side of the cell it is using.
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 22);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 15);
        ImGui::SmallButton("?");
        UIWidgets::Tooltip(helptext.c_str());
    }

    typedef uint32_t N64ButtonMask;

    // Used together for an incomplete linked hash map implementation in order to
    // map button masks to their names and original mapping on N64
    static std::list<std::pair<N64ButtonMask, const char*>> buttons;
    static std::unordered_map<N64ButtonMask, decltype(buttons)::iterator> buttonNames;

    void addButtonName(N64ButtonMask mask, const char* name) {
        buttons.push_back(std::make_pair(mask, name));
        buttonNames[mask] = std::prev(buttons.end());
    }

    typedef struct {
        const char* label;
        const char* cVarName;
        N64ButtonMask defaultBtn;
    } CustomButtonMap;

    // Ocarina button maps
    static CustomButtonMap ocarinaD5 = {"D5", "gOcarinaD5BtnMap", BTN_CUP};
    static CustomButtonMap ocarinaB4 = {"B4", "gOcarinaB4BtnMap", BTN_CLEFT};
    static CustomButtonMap ocarinaA4 = {"A4", "gOcarinaA4BtnMap", BTN_CRIGHT};
    static CustomButtonMap ocarinaF4 = {"F4", "gOcarinaF4BtnMap", BTN_CDOWN};
    static CustomButtonMap ocarinaD4 = {"D4", "gOcarinaD4BtnMap", BTN_A};
    static CustomButtonMap ocarinaSongDisable = {"Disable songs", "gOcarinaDisableBtnMap", BTN_L};
    static CustomButtonMap ocarinaSharp = {"Pitch up", "gOcarinaSharpBtnMap", BTN_R};
    static CustomButtonMap ocarinaFlat = {"Pitch down", "gOcarinaFlatBtnMap", BTN_Z};

    void DrawUI(bool&);

    void Init() {
        SohImGui::AddWindow("Enhancements", "Game Control Editor", DrawUI);

        addButtonName(BTN_A,		"A");
        addButtonName(BTN_B,		"B");
        addButtonName(BTN_CUP,		"C Up");
        addButtonName(BTN_CDOWN,	"C Down");
        addButtonName(BTN_CLEFT,	"C Left");
        addButtonName(BTN_CRIGHT,	"C Right");
        addButtonName(BTN_L,		"L");
        addButtonName(BTN_Z,		"Z");
        addButtonName(BTN_R,		"R");
        addButtonName(BTN_START,	"Start");
        addButtonName(BTN_DUP,		"D-pad up");
        addButtonName(BTN_DDOWN,	"D-pad down");
        addButtonName(BTN_DLEFT,	"D-pad left");
        addButtonName(BTN_DRIGHT,	"D-pad right");
        addButtonName(0,			"None");
    }

    // Draw a button mapping setting consisting of a padded label and button dropdown.
    // excludedButtons indicates which buttons are unavailable to choose from.
    void DrawMapping(CustomButtonMap& mapping, float labelWidth, N64ButtonMask excludedButtons) {
        N64ButtonMask currentButton = CVar_GetS32(mapping.cVarName, mapping.defaultBtn);

        const char* preview;
        if (buttonNames.contains(currentButton)) {
            preview = buttonNames[currentButton]->second;
        } else {
            preview = "Unknown";
        }

        UIWidgets::Spacer(0);
        ImVec2 cursorPos = ImGui::GetCursorPos();
        ImVec2 textSize = ImGui::CalcTextSize(mapping.label);
        ImGui::SetCursorPosY(cursorPos.y + textSize.y / 4);
        ImGui::SetCursorPosX(cursorPos.x + abs(textSize.x - labelWidth));
        ImGui::Text("%s", mapping.label);
        ImGui::SameLine();
        ImGui::SetCursorPosY(cursorPos.y);

        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
            if (ImGui::BeginCombo(StringHelper::Sprintf("##%s", mapping.cVarName).c_str(), preview)) {
            for (auto i = buttons.begin(); i != buttons.end(); i++) {
                if ((i->first & excludedButtons) != 0) {
                    continue;
                }
                if (ImGui::Selectable(i->second, i->first == currentButton)) {
                    CVar_SetS32(mapping.cVarName, i->first);
                }
            }
            ImGui::EndCombo();
        }
        UIWidgets::Spacer(0);
    }

    void DrawOcarinaControlPanel() {
        if (!ImGui::CollapsingHeader("Ocarina Controls")) {
            return;
        }

        if (!ImGui::BeginTable("tableCustomOcarinaControls", 1, PANEL_TABLE_FLAGS)) {
            return;
        }

        ImGui::TableSetupColumn("Custom Ocarina Controls", PANEL_TABLE_COLUMN_FLAGS | ImGuiTableColumnFlags_WidthStretch);
        TableHelper::InitHeader(false);
        
        ImVec2 cursor = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(cursor.x + 5, cursor.y + 5));
        UIWidgets::EnhancementCheckbox("Customize Ocarina Controls", "gCustomOcarinaControls");

        if (CVar_GetS32("gCustomOcarinaControls", 0) == 1) {
            if (ImGui::BeginTable("tableCustomMainOcarinaControls", 2, ImGuiTableFlags_SizingStretchProp)) {
                float labelWidth;
                N64ButtonMask disableMask = BTN_B;
                if (CVar_GetS32("gDpadOcarina", 0)) {
                    disableMask |= BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT;
                }

                ImGui::TableSetupColumn("Notes##CustomOcarinaNotes", PANEL_TABLE_COLUMN_FLAGS);
                ImGui::TableSetupColumn("Modifiers##CustomOcaranaModifiers", PANEL_TABLE_COLUMN_FLAGS);
                TableHelper::InitHeader(false);

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

                TableHelper::NextCol();

                SohImGui::BeginGroupPanel("Modifiers", ImGui::GetContentRegionAvail());
                labelWidth = ImGui::CalcTextSize(ocarinaSongDisable.label).x + 10;
                DrawMapping(ocarinaSongDisable, labelWidth, disableMask);
                DrawMapping(ocarinaSharp, labelWidth, disableMask);
                DrawMapping(ocarinaFlat, labelWidth, disableMask);
                SohImGui::EndGroupPanel(cursorY - ImGui::GetCursorPosY() + 2);

                ImGui::EndTable();
            }
        } else {
            UIWidgets::Spacer(0);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
            ImGui::TextWrapped("To modify the main ocarina controls, select the \"Customize Ocarina Controls\" checkbox.");
            UIWidgets::Spacer(0);
        }

        SohImGui::BeginGroupPanel("Alternate controls", ImGui::GetContentRegionAvail());
        if (ImGui::BeginTable("tableOcarinaAlternateControls", 2, ImGuiTableFlags_SizingFixedSame)) {
            ImGui::TableSetupColumn("D-pad", PANEL_TABLE_COLUMN_FLAGS);
            ImGui::TableSetupColumn("Right stick", PANEL_TABLE_COLUMN_FLAGS);
            TableHelper::InitHeader(false);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
            UIWidgets::EnhancementCheckbox("Play with D-pad", "gDpadOcarina");
            TableHelper::NextCol();
            UIWidgets::EnhancementCheckbox("Play with camera stick", "gRStickOcarina");
            ImGui::EndTable();
        }
        SohImGui::EndGroupPanel();

        ImGui::EndTable();
    }

    // CurrentPort is indexed started at 1 here due to the Generic tab, instead of 0 like in InputEditor
    // Therefore CurrentPort - 1 must always be used inside this function instead of CurrentPort
    void DrawCustomButtons() {
        SohImGui::GetInputEditor()->DrawControllerSelect(CurrentPort - 1);
        
        SohImGui::GetInputEditor()->DrawButton("Modifier 1", BTN_MODIFIER1, CurrentPort - 1, &BtnReading);
        SohImGui::GetInputEditor()->DrawButton("Modifier 2", BTN_MODIFIER2, CurrentPort - 1, &BtnReading);
    }

    void DrawCameraControlPanel() {
        if (!ImGui::CollapsingHeader("Camera Controls")) {
            return;
        }
        
        ImVec2 cursor = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(cursor.x + 5, cursor.y + 5));
        UIWidgets::PaddedEnhancementCheckbox("Invert Camera X Axis", "gInvertXAxis");
        DrawHelpIcon("Inverts the Camera X Axis in:\n-Free camera\n-C-Up view\n-Weapon Aiming");
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
        UIWidgets::PaddedEnhancementCheckbox("Invert Camera Y Axis", "gInvertYAxis");
        DrawHelpIcon("Inverts the Camera Y Axis in:\n-Free camera\n-C-Up view\n-Weapon Aiming");
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
        UIWidgets::PaddedEnhancementCheckbox("Right Stick Aiming", "gRightStickAiming");
        DrawHelpIcon("Allows for aiming with the right stick when:\n-Aiming in the C-Up view\n-Aiming with weapons");
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
        UIWidgets::PaddedEnhancementCheckbox("Disable Auto-Centering in First Person View", "gDisableAutoCenterView");
        DrawHelpIcon("Prevents the C-Up view from auto-centering, allowing for Gyro Aiming");
    }

    void DrawUI(bool& open) {
        if (!open) {
            CVar_SetS32("gGameControlEditorEnabled", false);
            return;
        }

        ImGui::SetNextWindowSize(ImVec2(465, 430), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Game Controls Configuration", &open)) {
            ImGui::BeginTabBar("##CustomControllers");
            if (ImGui::BeginTabItem("Generic")) {
                CurrentPort = 0;
                ImGui::EndTabItem();
            }

            for (int i = 1; i <= 4; i++) {
                if (ImGui::BeginTabItem(StringHelper::Sprintf("Port %d", i).c_str())) {
                    CurrentPort = i;
                    ImGui::EndTabItem();
                }
            }

            ImGui::EndTabBar();

            if (CurrentPort == 0) {
                DrawOcarinaControlPanel();
                DrawCameraControlPanel();
            } else {
                DrawCustomButtons();
            }
        }
        ImGui::End();
    }
}
