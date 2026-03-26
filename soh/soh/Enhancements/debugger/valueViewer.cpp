#include "valueViewer.h"
#include "soh/SohGui/UIWidgets.hpp"
#include "soh/SohGui/SohGui.hpp"
#include "soh/OTRGlobals.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include <spdlog/spdlog.h>
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
#include "soh/cvar_prefixes.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"

extern PlayState* gPlayState;
void GfxPrint_SetColor(GfxPrint* printer, u32 r, u32 g, u32 b, u32 a);
void GfxPrint_SetPos(GfxPrint* printer, s32 x, s32 y);
s32 GfxPrint_Printf(GfxPrint* printer, const char* fmt, ...);
}

#define CVAR_NAME CVAR_DEVELOPER_TOOLS("ValueViewerEnablePrinting")
#define CVAR_DEFAULT 0
#define CVAR_VALUE CVarGetInteger(CVAR_NAME, CVAR_DEFAULT)

std::map<ValueViewerEntry, ValueSetting> valueViewerSettings;

// clang-format off
std::array<ValueTableElement, VVE_MAX> valueTable = {{
    { "Time",               "gSaveContext.dayTime",           "TIME:",   TYPE_U16,   false, []() -> void* { return &gSaveContext.dayTime; }},
    { "Age",                "gSaveContext.linkAge",           "AGE:",    TYPE_S32,   false, []() -> void* { return &gSaveContext.linkAge; }},
    { "Health",             "gSaveContext.health",            "HP:",     TYPE_S16,   false, []() -> void* { return &gSaveContext.health; }},
    { "Navi Timer",         "gSaveContext.naviTimer",         "NAVI:",   TYPE_U16,   false, []() -> void* { return &gSaveContext.naviTimer; }},
    { "Scene ID",           "play->sceneNum",                 "SCENE:",  TYPE_S16,   true,  []() -> void* { return &gPlayState->sceneNum; }},
    { "Room ID",            "play->roomCtx.curRoom.num",      "ROOM:",   TYPE_S8,    true,  []() -> void* { return &gPlayState->roomCtx.curRoom.num; }},
    { "Entrance ID",        "gSaveContext.entranceIndex",     "ENTR:",   TYPE_S32,   false, []() -> void* { return &gSaveContext.entranceIndex; }},
    { "Cutscene ID",        "gSaveContext.cutsceneIndex",     "CUTS:",   TYPE_S32,   false, []() -> void* { return &gSaveContext.cutsceneIndex; }},
    { "Link X",             "Player->actor.world.pos.x",      "X:",      TYPE_FLOAT, true,  []() -> void* { return &GET_PLAYER(gPlayState)->actor.world.pos.x; }},
    { "Link Y",             "Player->actor.world.pos.y",      "Y:",      TYPE_FLOAT, true,  []() -> void* { return &GET_PLAYER(gPlayState)->actor.world.pos.y; }},
    { "Link Z",             "Player->actor.world.pos.z",      "Z:",      TYPE_FLOAT, true,  []() -> void* { return &GET_PLAYER(gPlayState)->actor.world.pos.z; }},
    { "Link Yaw",           "Player->actor.world.rot.y",      "ROT:",    TYPE_S16,   true,  []() -> void* { return &GET_PLAYER(gPlayState)->actor.world.rot.y; }},
    { "Link Velocity",      "Player->linearVelocity",         "V:",      TYPE_FLOAT, true,  []() -> void* { return &GET_PLAYER(gPlayState)->linearVelocity; }},
    { "Link X Velocity",    "Player->actor.velocity.x",       "XV:",     TYPE_FLOAT, true,  []() -> void* { return &GET_PLAYER(gPlayState)->actor.velocity.x; }},
    { "Link Y Velocity",    "Player->actor.velocity.y",       "YV:",     TYPE_FLOAT, true,  []() -> void* { return &GET_PLAYER(gPlayState)->actor.velocity.y; }},
    { "Link Z Velocity",    "Player->actor.velocity.z",       "ZV:",     TYPE_FLOAT, true,  []() -> void* { return &GET_PLAYER(gPlayState)->actor.velocity.z; }},
    { "Text ID",            "play->msgCtx.textId",            "TEXTID:", TYPE_U16,   true,  []() -> void* { return &gPlayState->msgCtx.textId; }},
    { "Analog Stick X",     "play->state.input->cur.stick_x", "AX:",     TYPE_S8,    true,  []() -> void* { return &gPlayState->state.input->cur.stick_x; }},
    { "Analog Stick Y",     "play->state.input->cur.stick_y", "AY:",     TYPE_S8,    true,  []() -> void* { return &gPlayState->state.input->cur.stick_y; }},
    { "getItemID",          "Player->getItemId",              "ITEM:",   TYPE_S16,   true,  []() -> void* { return &GET_PLAYER(gPlayState)->getItemId; }},
    { "getItemEntry",       "Player->getItemEntry",           "IE:",     TYPE_S16,   true,  []() -> void* { return &GET_PLAYER(gPlayState)->getItemEntry.itemId; }},
    { "Movement Angle",     "Player->yaw",                    "YAW:",    TYPE_S16,   true,  []() -> void* { return &GET_PLAYER(gPlayState)->yaw; }},
    { "Last Item Pressed",  "Player->heldItemButton",         "LASTI:",  TYPE_S8,    true,  []() -> void* { return &GET_PLAYER(gPlayState)->heldItemButton; }},
    { "Stick Timer",        "Player->unk_860",                "STICK:",  TYPE_S16,   true,  []() -> void* { return &GET_PLAYER(gPlayState)->unk_860; }},
    { "Last damage value",  "Player->melee[0].info.dmgFlags", "DMG:",    TYPE_U32,   true,  []() -> void* { return &GET_PLAYER(gPlayState)->meleeWeaponQuads[0].info.toucher.dmgFlags; }},
    { "Camera Angle",       "play->mainCamera.camDir.y",      "CAMY:",   TYPE_S16,   true,  []() -> void* { return &gPlayState->mainCamera.camDir.y; }},
    { "Camera XZ Speed",    "play->mainCamera.xzSpeed",       "CAMXZV:", TYPE_FLOAT, true,  []() -> void* { return &gPlayState->mainCamera.xzSpeed; }},
    { "Frame Counter",      "play->state.frames",             "FRAM:",   TYPE_S32,   true,  []() -> void* { return &gPlayState->state.frames; }},
    { "Cutscene Pointer",   "play->csCtx.segment",            "CSP:",    TYPE_PTR,   true,  []() -> void* { return &gPlayState->csCtx.segment; }},
    { "Framerate Divisor",  "R_UPDATE_RATE",                  "FRDV:",   TYPE_S16,   false, []() -> void* { return &R_UPDATE_RATE; }},
    { "Next HUD mode",      "gSaveContext.nextHudMode",       "HUD:",    TYPE_S16,   false, []() -> void* { return &gSaveContext.unk_13E8; }},
    { "Temp B Value",       "gSaveContext.buttonStatus[0]",   "TEMPB:",  TYPE_U8,    false, []() -> void* { return &gSaveContext.buttonStatus[0]; }},
    { "Blue Warp Timer",    "DoorWarp1->warpTimer",           "WARPT:",  TYPE_U16,   true,  []() -> void* { DoorWarp1 *actor = (DoorWarp1 *)Actor_Find(&gPlayState->actorCtx, ACTOR_DOOR_WARP1 ,ACTORCAT_ITEMACTION); if(actor) { return &actor->warpTimer; } else { return nullptr; }}},
    /* TODO: Find these (from GZ)
    "Last RNG Value" x32 0x80105A80
    "Analog Stick Angle" s16 0x803AA698
    */
}};
// clang-format on

void LoadValueConfig() {
    auto allConfig = Ship::Context::GetInstance()->GetConfig()->GetNestedJson();
    if (allConfig.find("ValueViewer") == allConfig.end() || !allConfig["ValueViewer"].is_array()) {
        allConfig["ValueViewer"] = nlohmann::json::array();
    }
    valueViewerSettings = allConfig["ValueViewer"];
}

void SaveValueConfig() {
    auto allConfig = Ship::Context::GetInstance()->GetConfig()->GetNestedJson();
    allConfig["ValueViewer"] = valueViewerSettings;
    Ship::Context::GetInstance()->GetConfig()->SetBlock("ValueViewer", valueViewerSettings);
    Ship::Context::GetInstance()->GetConfig()->Save();
}

extern "C" void ValueViewer_Draw(GfxPrint* printer) {
    for (size_t i = 0; i < VVE_MAX; i++) {
        ValueTableElement& element = valueTable[i];
        if (!valueViewerSettings.contains((ValueViewerEntry)i) || (gPlayState == NULL && element.requiresPlayState))
            continue;
        ValueSetting& setting = valueViewerSettings[(ValueViewerEntry)i];
        if (!setting.isPrinted)
            continue;
        void* elementValue = element.valueFn();
        if (elementValue == NULL)
            continue;
        GfxPrint_SetColor(printer, setting.color.x * 255, setting.color.y * 255, setting.color.z * 255,
                          setting.color.w * 255);
        GfxPrint_SetPos(printer, setting.x, setting.y);
        switch (element.type) {
            case TYPE_S8:
                GfxPrint_Printf(printer, (setting.typeFormat ? "%s0x%x" : "%s%d"), setting.prefix.c_str(),
                                *(s8*)elementValue);
                break;
            case TYPE_U8:
                GfxPrint_Printf(printer, (setting.typeFormat ? "%s0x%x" : "%s%u"), setting.prefix.c_str(),
                                *(u8*)elementValue);
                break;
            case TYPE_S16:
                GfxPrint_Printf(printer, (setting.typeFormat ? "%s0x%x" : "%s%d"), setting.prefix.c_str(),
                                *(s16*)elementValue);
                break;
            case TYPE_U16:
                GfxPrint_Printf(printer, (setting.typeFormat ? "%s0x%x" : "%s%u"), setting.prefix.c_str(),
                                *(u16*)elementValue);
                break;
            case TYPE_S32:
                GfxPrint_Printf(printer, (setting.typeFormat ? "%s0x%x" : "%s%d"), setting.prefix.c_str(),
                                *(s32*)elementValue);
                break;
            case TYPE_U32:
                GfxPrint_Printf(printer, (setting.typeFormat ? "%s0x%x" : "%s%u"), setting.prefix.c_str(),
                                *(u32*)elementValue);
                break;
            case TYPE_PTR:
                GfxPrint_Printf(printer, "%s%p", setting.prefix.c_str(), *(void**)elementValue);
                break;
            case TYPE_CHAR:
                GfxPrint_Printf(printer, "%s%c", setting.prefix.c_str(), *(char*)elementValue);
                break;
            case TYPE_STRING:
                GfxPrint_Printf(printer, "%s%s", setting.prefix.c_str(), (char*)elementValue);
                break;
            case TYPE_FLOAT:
                GfxPrint_Printf(printer, (setting.typeFormat ? "%s%4.1f" : "%s%f"), setting.prefix.c_str(),
                                *(float*)elementValue);
                break;
            default:
                SPDLOG_ERROR("ValueViewer_Draw reached `default`, got {}", static_cast<int>(element.type));
                assert(false);
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

static RegisterShipInitFunc initFunc(RegisterValueViewerHooks, { CVAR_NAME });

void ValueViewerWindow::DrawElement() {
    ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
    UIWidgets::CVarCheckbox("Enable Printing", CVAR_NAME, UIWidgets::CheckboxOptions().Color(THEME_COLOR));

    ImGui::BeginGroup();
    static size_t selectedElement = -1;
    std::string selectedElementText = (selectedElement == -1) ? "Select a value"
                                                              : (std::string(valueTable[selectedElement].name) + " (" +
                                                                 std::string(valueTable[selectedElement].path) + ")");
    UIWidgets::PushStyleCombobox(THEME_COLOR);
    if (ImGui::BeginCombo("##valueViewerElement", selectedElementText.c_str())) {
        for (size_t i = 0; i < valueTable.size(); i++) {
            if (valueViewerSettings.contains((ValueViewerEntry)i))
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
        valueViewerSettings.insert(
            { (ValueViewerEntry)selectedElement,
              { valueTable[selectedElement].prefix, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), false, false, 0, 0 } });
        selectedElement = -1;
        SaveValueConfig();
    }
    UIWidgets::PopStyleButton();
    ImGui::EndGroup();

    for (size_t i = 0; i < valueTable.size(); i++) {
        ValueTableElement& element = valueTable[i];
        if (!valueViewerSettings.contains((ValueViewerEntry)i) || (gPlayState == NULL && element.requiresPlayState))
            continue;
        void* elementValue = element.valueFn();
        if (elementValue == nullptr)
            continue;
        UIWidgets::PushStyleButton(THEME_COLOR);
        UIWidgets::PushStyleCheckbox(THEME_COLOR);
        ImGui::AlignTextToFramePadding();
        if (ImGui::Button((ICON_FA_TIMES + std::string("##") + std::string(element.name)).c_str())) {
            valueViewerSettings.erase((ValueViewerEntry)i);
            UIWidgets::PopStyleCheckbox();
            UIWidgets::PopStyleButton();
            SaveValueConfig();
            continue;
        }
        UIWidgets::PopStyleCheckbox();
        UIWidgets::PopStyleButton();
        ValueSetting& setting = valueViewerSettings[(ValueViewerEntry)i];
        ImGui::SameLine();
        ImGui::Text("%s:", element.name);
        ImGui::SameLine();
        switch (element.type) {
            case TYPE_S8:
                ImGui::Text(setting.typeFormat ? "0x%x" : "%d", *(s8*)elementValue);
                break;
            case TYPE_U8:
                ImGui::Text(setting.typeFormat ? "0x%x" : "%u", *(u8*)elementValue);
                break;
            case TYPE_S16:
                ImGui::Text(setting.typeFormat ? "0x%x" : "%d", *(s16*)elementValue);
                break;
            case TYPE_U16:
                ImGui::Text(setting.typeFormat ? "0x%x" : "%u", *(u16*)elementValue);
                break;
            case TYPE_S32:
                ImGui::Text(setting.typeFormat ? "0x%x" : "%d", *(s32*)elementValue);
                break;
            case TYPE_U32:
                ImGui::Text(setting.typeFormat ? "0x%x" : "%u", *(u32*)elementValue);
                break;
            case TYPE_PTR:
                ImGui::Text("%p", *(void**)elementValue);
                break;
            case TYPE_CHAR:
                ImGui::Text("%c", *(char*)elementValue);
                break;
            case TYPE_STRING:
                ImGui::Text("%s", (char*)elementValue);
                break;
            case TYPE_FLOAT:
                ImGui::Text(setting.typeFormat ? "%4.1f" : "%f", *(float*)elementValue);
                break;
            default:
                SPDLOG_ERROR("ValueViewerWindow::DrawElement reached `default`, got {}",
                             static_cast<int>(element.type));
                assert(false);
        }
        ImGui::SameLine();
        UIWidgets::PushStyleCheckbox(THEME_COLOR);
        if (element.type <= TYPE_U32) {
            if (ImGui::Checkbox(("Hex##" + std::string(element.name)).c_str(), &setting.typeFormat)) {
                SaveValueConfig();
            }
            ImGui::SameLine();
        } else if (element.type == TYPE_FLOAT) {
            if (ImGui::Checkbox(("Trim##" + std::string(element.name)).c_str(), &setting.typeFormat)) {
                SaveValueConfig();
            }
            ImGui::SameLine();
        }
        UIWidgets::PopStyleCheckbox();

        ImGui::BeginGroup();
        if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ValueViewerEnablePrinting"), 0)) {
            UIWidgets::PushStyleCheckbox(THEME_COLOR);
            if (ImGui::Checkbox(("Print##" + std::string(element.name)).c_str(), &setting.isPrinted)) {
                SaveValueConfig();
            }
            UIWidgets::PopStyleCheckbox();
            if (setting.isPrinted) {
                char* prefix = (char*)element.prefix.c_str();
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80.0f);
                UIWidgets::PushStyleInput(THEME_COLOR);
                if (ImGui::InputText(("Prefix##" + std::string(element.name)).c_str(), prefix, 10)) {
                    setting.prefix = prefix;
                    SaveValueConfig();
                }
                UIWidgets::PopStyleInput();
                ImGui::SameLine();
                if (ImGui::ColorEdit3(("##color" + std::string(element.name)).c_str(), (float*)&setting.color,
                                      ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {

                    SaveValueConfig();
                }
                ImGui::SameLine();
                UIWidgets::PushStyleCheckbox(THEME_COLOR);
                if (ImGui::Button(("Position##" + std::string(element.name)).c_str())) {
                    ImGui::OpenPopup(("Position Picker##" + std::string(element.name)).c_str());
                }
                UIWidgets::PopStyleCheckbox();
                if (ImGui::BeginPopup(("Position Picker##" + std::string(element.name)).c_str())) {
                    if (ImGui::DragInt("X", (int*)&setting.x, 1.0f, 0, 44)) {
                        SaveValueConfig();
                    }
                    if (ImGui::DragInt("Y", (int*)&setting.y, 1.0f, 0, 29)) {
                        SaveValueConfig();
                    }
                    ImGui::EndPopup();
                }
            }
        }
        ImGui::EndGroup();
    }
    ImGui::EndDisabled();
}

void ValueViewerWindow::InitElement() {
    static bool loadedConfig = false;
    if (!loadedConfig) {
        LoadValueConfig();
        loadedConfig = true;
    }
}
