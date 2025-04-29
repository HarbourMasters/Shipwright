#include "valueViewer.h"
#include "soh/SohGui/UIWidgets.hpp"
#include "soh/SohGui/SohGui.hpp"
#include "soh/OTRGlobals.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
#include "soh/cvar_prefixes.h"
extern PlayState* gPlayState;
void GfxPrint_SetColor(GfxPrint* printer, u32 r, u32 g, u32 b, u32 a);
void GfxPrint_SetPos(GfxPrint* printer, s32 x, s32 y);
s32 GfxPrint_Printf(GfxPrint* printer, const char* fmt, ...);
}

#define CVAR_NAME CVAR_DEVELOPER_TOOLS("ValueViewerEnablePrinting")
#define CVAR_DEFAULT 0
#define CVAR_VALUE CVarGetInteger(CVAR_NAME, CVAR_DEFAULT)

ImVec4 WHITE = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

// clang-format off
std::vector<ValueTableElement> valueTable = {
    { "Time",               "gSaveContext.dayTime",                 "TIME:",   TYPE_U16,   false, []() -> void* { return &gSaveContext.dayTime; },                      WHITE },
    { "Age",                "gSaveContext.linkAge",                 "AGE:",    TYPE_S32,   false, []() -> void* { return &gSaveContext.linkAge; },                      WHITE },
    { "Health",             "gSaveContext.health",                  "HP:",     TYPE_S16,   false, []() -> void* { return &gSaveContext.health; },                       WHITE },
    { "Navi Timer",         "gSaveContext.naviTimer",               "NAVI:",   TYPE_U16,   false, []() -> void* { return &gSaveContext.naviTimer; },                    WHITE },
    { "Scene ID",           "play->sceneNum",                       "SCENE:",  TYPE_S16,   true,  []() -> void* { return &gPlayState->sceneNum; },                      WHITE },
    { "Room ID",            "play->roomCtx.curRoom.num",            "ROOM:",   TYPE_S8,    true,  []() -> void* { return &gPlayState->roomCtx.curRoom.num; },           WHITE },
    { "Entrance ID",        "gSaveContext.entranceIndex",           "ENTR:",   TYPE_S32,   false, []() -> void* { return &gSaveContext.entranceIndex; },                WHITE },
    { "Cutscene ID",        "gSaveContext.cutsceneIndex",           "CUTS:",   TYPE_S32,   false, []() -> void* { return &gSaveContext.cutsceneIndex; },                WHITE },
    { "Link X",             "Player->actor.world.pos.x",            "X:",      TYPE_FLOAT, true,  []() -> void* { return &GET_PLAYER(gPlayState)->actor.world.pos.x; }, WHITE },
    { "Link Y",             "Player->actor.world.pos.y",            "Y:",      TYPE_FLOAT, true,  []() -> void* { return &GET_PLAYER(gPlayState)->actor.world.pos.y; }, WHITE },
    { "Link Z",             "Player->actor.world.pos.z",            "Z:",      TYPE_FLOAT, true,  []() -> void* { return &GET_PLAYER(gPlayState)->actor.world.pos.z; }, WHITE },
    { "Link Yaw",           "Player->actor.world.rot.y",            "ROT:",    TYPE_S16,   true,  []() -> void* { return &GET_PLAYER(gPlayState)->actor.world.rot.y; }, WHITE },
    { "Link Velocity",      "Player->linearVelocity",               "V:",      TYPE_FLOAT, true,  []() -> void* { return &GET_PLAYER(gPlayState)->linearVelocity; },    WHITE },
    { "Link X Velocity",    "Player->actor.velocity.x",             "XV:",     TYPE_FLOAT, true,  []() -> void* { return &GET_PLAYER(gPlayState)->actor.velocity.x; },  WHITE },
    { "Link Y Velocity",    "Player->actor.velocity.y",             "YV:",     TYPE_FLOAT, true,  []() -> void* { return &GET_PLAYER(gPlayState)->actor.velocity.y; },  WHITE },
    { "Link Z Velocity",    "Player->actor.velocity.z",             "ZV:",     TYPE_FLOAT, true,  []() -> void* { return &GET_PLAYER(gPlayState)->actor.velocity.z; },  WHITE },
    { "Text ID",            "play->msgCtx.textId",                  "TEXTID:", TYPE_U16,   true,  []() -> void* { return &gPlayState->msgCtx.textId; },                 WHITE },
    { "Analog Stick X",     "play->state.input->cur.stick_x",       "AX:",     TYPE_S8,    true,  []() -> void* { return &gPlayState->state.input->cur.stick_x; },      WHITE },
    { "Analog Stick Y",     "play->state.input->cur.stick_y",       "AY:",     TYPE_S8,    true,  []() -> void* { return &gPlayState->state.input->cur.stick_y; },      WHITE },
    { "getItemID",          "Player->getItemId",                    "ITEM:",   TYPE_S16,   true,  []() -> void* { return &GET_PLAYER(gPlayState)->getItemId; },         WHITE },
    { "getItemEntry",       "Player->getItemEntry",                 "IE:",     TYPE_S16,   true,  []() -> void* { return &GET_PLAYER(gPlayState)->getItemEntry.itemId; }, WHITE },
    /* TODO: Find these (from GZ)
    "XZ Units Traveled (Camera based speed variable)" f32 0x801C9018
    "Movement Angle" x16 0x801DBB1C
    "Camera Angle" u16 0x801C907C
    "Time of Day" x16 0x8011AC8C
    "Global Frame Counter" s32 0x801C8DFC
    "Lit Deku Stick Timer" u16 0x801DBB40
    "Cutscene Pointer" u32 0x801CAAC8
    "Get Item Value" s8 0x801DB714
    "Last RNG Value" x32 0x80105A80
    "Last Item Button Pressed" u8 0x801DB430
    "Last Damage Value" x32 0x801DB7DC
    "Temp B Value" u8 0x8011C062
    "Framerate Divisor" u8 0x801C7861
    "Heads Up Display (HUD)" u16 0x8011C068
    "Analog Stick Angle" s16 0x803AA698
    "Deku Tree Warp Timer (Reload Room)" u16 0x801F0352
    "Dodongo's Cavern Warp Timer" u16 0x801E30B2
    "Jabu-Jabu Warp Timer" u16 0x802008B2
    "Forest Temple Warp Timer" u16 0x801EC5B2
    "Fire Temple Warp Timer" u16 0x801F3E42
    "Water Temple Warp Timer" u16 0x801F8762
    "Shadow Temple Warp Timer" u16 0x801F48A2
    "Spirit Temple Warp Timer" u16 0x801FD562
    "Deku Tree Warp Timer" u16 0x801F83A2
    */
};
// clang-format on

extern "C" void ValueViewer_Draw(GfxPrint* printer) {
    for (int i = 0; i < valueTable.size(); i++) {
        ValueTableElement& element = valueTable[i];
        if (!element.isActive || !element.isPrinted || (gPlayState == NULL && element.requiresPlayState))
            continue;
        GfxPrint_SetColor(printer, element.color.x * 255, element.color.y * 255, element.color.z * 255,
                          element.color.w * 255);
        GfxPrint_SetPos(printer, element.x, element.y);
        switch (element.type) {
            case TYPE_S8:
                GfxPrint_Printf(printer, (element.typeFormat ? "%s0x%x" : "%s%d"), element.prefix.c_str(),
                                *(s8*)element.valueFn());
                break;
            case TYPE_U8:
                GfxPrint_Printf(printer, (element.typeFormat ? "%s0x%x" : "%s%u"), element.prefix.c_str(),
                                *(u8*)element.valueFn());
                break;
            case TYPE_S16:
                GfxPrint_Printf(printer, (element.typeFormat ? "%s0x%x" : "%s%d"), element.prefix.c_str(),
                                *(s16*)element.valueFn());
                break;
            case TYPE_U16:
                GfxPrint_Printf(printer, (element.typeFormat ? "%s0x%x" : "%s%u"), element.prefix.c_str(),
                                *(u16*)element.valueFn());
                break;
            case TYPE_S32:
                GfxPrint_Printf(printer, (element.typeFormat ? "%s0x%x" : "%s%d"), element.prefix.c_str(),
                                *(s32*)element.valueFn());
                break;
            case TYPE_U32:
                GfxPrint_Printf(printer, (element.typeFormat ? "%s0x%x" : "%s%u"), element.prefix.c_str(),
                                *(u32*)element.valueFn());
                break;
            case TYPE_CHAR:
                GfxPrint_Printf(printer, "%s%c", element.prefix.c_str(), *(char*)element.valueFn());
                break;
            case TYPE_STRING:
                GfxPrint_Printf(printer, "%s%s", element.prefix.c_str(), (char*)element.valueFn());
                break;
            case TYPE_FLOAT:
                GfxPrint_Printf(printer, (element.typeFormat ? "%s%4.1f" : "%s%f"), element.prefix.c_str(),
                                *(float*)element.valueFn());
                break;
        }
    }
}

extern "C" void ValueViewer_SetupDraw() {
    OPEN_DISPS(gGameState->gfxCtx);

    Gfx* gfx;
    Gfx* polyOpa;
    GfxPrint printer;

    polyOpa = POLY_OPA_DISP;
    gfx = Graph_GfxPlusOne(polyOpa);
    gSPDisplayList(OVERLAY_DISP++, gfx);

    GfxPrint_Init(&printer);
    GfxPrint_Open(&printer, gfx);

    ValueViewer_Draw(&printer);

    gfx = GfxPrint_Close(&printer);
    GfxPrint_Destroy(&printer);

    gSPEndDisplayList(gfx++);
    Graph_BranchDlist(polyOpa, gfx);
    POLY_OPA_DISP = gfx;

    CLOSE_DISPS(gGameState->gfxCtx);
}

void RegisterValueViewerHooks() {
    COND_HOOK(OnGameFrameUpdate, CVAR_VALUE, []() { ValueViewer_SetupDraw(); });
}

RegisterShipInitFunc initFunc(RegisterValueViewerHooks, { CVAR_NAME });

void ValueViewerWindow::DrawElement() {
    UIWidgets::CVarCheckbox("Enable Printing", CVAR_NAME, UIWidgets::CheckboxOptions().Color(THEME_COLOR));

    ImGui::BeginGroup();
    static int selectedElement = -1;
    std::string selectedElementText = (selectedElement == -1) ? "Select a value"
                                                              : (std::string(valueTable[selectedElement].name) + " (" +
                                                                 std::string(valueTable[selectedElement].path) + ")");
    UIWidgets::PushStyleCombobox(THEME_COLOR);
    if (ImGui::BeginCombo("##valueViewerElement", selectedElementText.c_str())) {
        for (int i = 0; i < valueTable.size(); i++) {
            if (valueTable[i].isActive)
                continue;
            bool isSelected = (selectedElement == i);
            std::string elementText = (std::string(valueTable[i].name) + " (" + std::string(valueTable[i].path) + ")");
            if (ImGui::Selectable(elementText.c_str(), isSelected)) {
                selectedElement = i;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    UIWidgets::PopStyleCombobox();
    ImGui::SameLine();
    UIWidgets::PushStyleButton(THEME_COLOR);
    if (selectedElement != -1 && ImGui::Button("+")) {
        valueTable[selectedElement].isActive = true;
        selectedElement = -1;
    }
    UIWidgets::PopStyleButton();
    ImGui::EndGroup();

    for (int i = 0; i < valueTable.size(); i++) {
        ValueTableElement& element = valueTable[i];
        if (!element.isActive || (gPlayState == NULL && element.requiresPlayState))
            continue;
        UIWidgets::PushStyleButton(THEME_COLOR);
        UIWidgets::PushStyleCheckbox(THEME_COLOR);
        ImGui::AlignTextToFramePadding();
        if (ImGui::Button((ICON_FA_TIMES + std::string("##") + std::string(element.name)).c_str())) {
            element.isActive = false;
            element.isPrinted = false;
        }
        UIWidgets::PopStyleCheckbox();
        UIWidgets::PopStyleButton();
        ImGui::SameLine();
        ImGui::Text("%s:", element.name);
        ImGui::SameLine();
        switch (element.type) {
            case TYPE_S8:
                ImGui::Text(element.typeFormat ? "0x%x" : "%d", *(s8*)element.valueFn());
                break;
            case TYPE_U8:
                ImGui::Text(element.typeFormat ? "0x%x" : "%u", *(u8*)element.valueFn());
                break;
            case TYPE_S16:
                ImGui::Text(element.typeFormat ? "0x%x" : "%d", *(s16*)element.valueFn());
                break;
            case TYPE_U16:
                ImGui::Text(element.typeFormat ? "0x%x" : "%u", *(u16*)element.valueFn());
                break;
            case TYPE_S32:
                ImGui::Text(element.typeFormat ? "0x%x" : "%d", *(s32*)element.valueFn());
                break;
            case TYPE_U32:
                ImGui::Text(element.typeFormat ? "0x%x" : "%u", *(u32*)element.valueFn());
                break;
            case TYPE_CHAR:
                ImGui::Text("%c", *(char*)element.valueFn());
                break;
            case TYPE_STRING:
                ImGui::Text("%s", (char*)element.valueFn());
                break;
            case TYPE_FLOAT:
                ImGui::Text(element.typeFormat ? "%4.1f" : "%f", *(float*)element.valueFn());
                break;
        }
        ImGui::SameLine();
        UIWidgets::PushStyleCheckbox(THEME_COLOR);
        if (element.type <= TYPE_U32) {
            ImGui::Checkbox(("Hex##" + std::string(element.name)).c_str(), &element.typeFormat);
            ImGui::SameLine();
        } else if (element.type == TYPE_FLOAT) {
            ImGui::Checkbox(("Trim##" + std::string(element.name)).c_str(), &element.typeFormat);
            ImGui::SameLine();
        }
        UIWidgets::PopStyleCheckbox();

        ImGui::BeginGroup();
        if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ValueViewerEnablePrinting"), 0)) {
            UIWidgets::PushStyleCheckbox(THEME_COLOR);
            ImGui::Checkbox(("Print##" + std::string(element.name)).c_str(), &element.isPrinted);
            UIWidgets::PopStyleCheckbox();
            if (element.isPrinted) {
                char* prefix = (char*)element.prefix.c_str();
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80.0f);
                UIWidgets::PushStyleInput(THEME_COLOR);
                if (ImGui::InputText(("Prefix##" + std::string(element.name)).c_str(), prefix, 10)) {
                    element.prefix = prefix;
                }
                UIWidgets::PopStyleInput();
                ImGui::SameLine();
                ImGui::ColorEdit3(("##color" + std::string(element.name)).c_str(), (float*)&element.color,
                                  ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                ImGui::SameLine();
                UIWidgets::PushStyleCheckbox(THEME_COLOR);
                if (ImGui::Button(("Position##" + std::string(element.name)).c_str())) {
                    ImGui::OpenPopup(("Position Picker##" + std::string(element.name)).c_str());
                }
                UIWidgets::PopStyleCheckbox();
                if (ImGui::BeginPopup(("Position Picker##" + std::string(element.name)).c_str())) {
                    ImGui::DragInt("X", (int*)&element.x, 1.0f, 0, 44);
                    ImGui::DragInt("Y", (int*)&element.y, 1.0f, 0, 29);
                    ImGui::EndPopup();
                }
            }
        }
        ImGui::EndGroup();
    }
}

void ValueViewerWindow::InitElement() {
}
