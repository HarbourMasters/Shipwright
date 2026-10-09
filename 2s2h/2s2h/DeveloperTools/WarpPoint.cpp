#include "2s2h/BenGui/UIWidgets.hpp"
#include <ship/config/Config.h>
#include <ship/window/gui/IconsFontAwesome4.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/DeveloperTools/DeveloperTools.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/BenGui/BenGui.hpp"
#include "BenJsonConversions.hpp"

extern "C" {
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "overlays/gamestates/ovl_select/z_select.h"
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
extern GameState* gGameState;
}

typedef struct WarpPoint {
    s32 entranceId;
    s8 roomNum;
    Vec3f pos;
    s16 rotY;
    bool bootToPoint;
} WarpPoint;
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WarpPoint, entranceId, roomNum, pos, rotY, bootToPoint)
std::map<std::string, WarpPoint> warpPoints;

#define CVAR_BOOT_TO_FILE_SELECT_NAME "gEnhancements.Cutscenes.SkipToFileSelect"
#define CVAR_BOOT_TO_FILE_SELECT ((bool)CVarGetInteger(CVAR_BOOT_TO_FILE_SELECT_NAME, 0))

void LoadConfig() {
    auto allConfig = Ship::Context::GetRawInstance()->GetConfig()->GetNestedJson();
    if (allConfig.find("WarpPoints") == allConfig.end() || !allConfig["WarpPoints"].is_object()) {
        allConfig["WarpPoints"] = nlohmann::json::object();
    }
    warpPoints = allConfig["WarpPoints"];
}

void SaveConfig() {
    auto allConfig = Ship::Context::GetRawInstance()->GetConfig()->GetNestedJson();
    allConfig["WarpPoints"] = warpPoints;
    Ship::Context::GetRawInstance()->GetConfig()->SetBlock("WarpPoints", warpPoints);
    Ship::Context::GetRawInstance()->GetConfig()->Save();
}

void Warp(WarpPoint& warpPoint) {
    if (gPlayState == NULL) {
        // If gPlayState is NULL, it means the the user opted into BootToWarpPoint and the game is starting up.
        gSaveContext.gameMode = GAMEMODE_NORMAL;
        Sram_InitNewSave();
        gSaveContext.sceneLayer = 0;
        gSaveContext.save.time = CLOCK_TIME(8, 0);
        gSaveContext.save.day = 1;
        gSaveContext.save.cutsceneIndex = 0;
        gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
        gSaveContext.save.linkAge = 0;

        // Need to unset flag values that are outside of `.save`
        // Normally would happened in the MapSelect_LoadGame, but is skipped because of the dummy file num below
        // This is mostly copied from Sram_OpenSave.
        for (size_t i = 0; i < ARRAY_COUNT(gSaveContext.eventInf); i++) {
            gSaveContext.eventInf[i] = 0;
        }
        for (int i = 0; i < ARRAY_COUNT(gSaveContext.cycleSceneFlags); i++) {
            gSaveContext.cycleSceneFlags[i].chest = gSaveContext.save.saveInfo.permanentSceneFlags[i].chest;
            gSaveContext.cycleSceneFlags[i].switch0 = gSaveContext.save.saveInfo.permanentSceneFlags[i].switch0;
            gSaveContext.cycleSceneFlags[i].switch1 = gSaveContext.save.saveInfo.permanentSceneFlags[i].switch1;
            gSaveContext.cycleSceneFlags[i].clearedRoom = gSaveContext.save.saveInfo.permanentSceneFlags[i].clearedRoom;
            gSaveContext.cycleSceneFlags[i].collectible = gSaveContext.save.saveInfo.permanentSceneFlags[i].collectible;
        }

        // Using dummy file num to bypass debug save setup in map select and manually execute save init/load hooks after
        gSaveContext.fileNum = 0xFE;
        MapSelect_LoadGame((MapSelectState*)gGameState, warpPoint.entranceId, 0);
        // Then back to debug file num
        gSaveContext.fileNum = 0xFF;
        // These two lines allow randomizer to be used with BootToWarpPoint. Not sure how reliable this is, might remove
        GameInteractor_ExecuteOnSaveInit(gSaveContext.fileNum);
        GameInteractor_ExecuteOnSaveLoad(gSaveContext.fileNum);
        gSaveContext.save.entrance = warpPoint.entranceId;
    } else {
        // The else case, and the rest of this function is primarily relevant code copied from Play_SetRespawnData and
        // func_80169EFC, minus the parts that copy scene flags to scene we are warping to (this is obviously
        // undesirable)
        gPlayState->nextEntrance = Entrance_Create(warpPoint.entranceId >> 9, 0, warpPoint.entranceId & 0xF);
        gPlayState->transitionTrigger = TRANS_TRIGGER_START;
        gPlayState->transitionType = TRANS_TYPE_INSTANT;
    }
    gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance =
        Entrance_Create(warpPoint.entranceId >> 9, 0, warpPoint.entranceId & 0xF);
    gSaveContext.respawn[RESPAWN_MODE_DOWN].roomIndex = warpPoint.roomNum;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].pos = warpPoint.pos;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw = warpPoint.rotY;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D);
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK_FAST;
    gSaveContext.respawnFlag = -8;
}

static std::string warpNameInput = "";

void RenderWarpPointSection() {
    ImGui::SeparatorText("Warp Points");
    if (gPlayState != NULL && GET_PLAYER(gPlayState) != NULL) {
        UIWidgets::InputString("##WarpPointNameInput", &warpNameInput,
                               {
                                   .labelPosition = UIWidgets::LabelPosition::None,
                                   .size = ImVec2(ImGui::GetContentRegionAvail().x - 50.0f, 0.0f),
                                   .placeholder = "Enter warp point name...",
                               });

        ImGui::SameLine();
        if (UIWidgets::Button(ICON_FA_PLUS)) {
            Player* player = GET_PLAYER(gPlayState);

            bool isEmpty = warpNameInput.empty();
            if (warpNameInput.empty()) {
                warpNameInput = Ship_GetSceneName(gPlayState->sceneId);
                if (gPlayState->roomCtx.curRoom.num != 0) {
                    warpNameInput += " (" + std::to_string(gPlayState->roomCtx.curRoom.num) + ")";
                }
            }

            warpPoints[warpNameInput] = WarpPoint{
                .entranceId = gSaveContext.save.entrance,
                .roomNum = gPlayState->roomCtx.curRoom.num,
                .pos = player->actor.world.pos,
                .rotY = player->actor.shape.rot.y,
            };
            SaveConfig();
            warpNameInput = "";
        }
    }
    // List of warp points, showing just their name, a button to warp and a button to delete
    for (auto it = warpPoints.begin(); it != warpPoints.end();) {
        ImGui::PushID(it->first.c_str());

        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", it->first.c_str());
        if (it->second.bootToPoint) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.85f, 0.55f, 0.0f, 1.0f), "[Boot]");
        }
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 115.0f);
        if (gPlayState == NULL)
            ImGui::BeginDisabled();
        if (UIWidgets::Button(ICON_FA_PLANE, { .size = UIWidgets::Sizes::Inline })) {
            // Warp to this point
            Warp(it->second);
        }
        if (gPlayState == NULL)
            ImGui::EndDisabled();
        ImGui::SameLine();
        if (UIWidgets::Button(ICON_FA_REFRESH,
                              { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Orange })) {
            bool wasEnabled = it->second.bootToPoint;
            for (auto& wp : warpPoints) {
                wp.second.bootToPoint = false;
            }
            it->second.bootToPoint = !wasEnabled;
            SaveConfig();
        }
        ImGui::SameLine();
        if (UIWidgets::Button(ICON_FA_TRASH, { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
            it = warpPoints.erase(it);
            SaveConfig();
            ImGui::PopID();
            continue;
        }
        ImGui::PopID();

        ++it;
    }
}

void RegisterWarpPoints() {
    static bool loadedConfig = false;
    if (!loadedConfig) {
        LoadConfig();
        loadedConfig = true;
    }

    // Warp to point that has bootToWarp enabled. Do not run if Skip to File Select is enabled.
    COND_HOOK(OnConsoleLogoUpdate, !CVAR_BOOT_TO_FILE_SELECT, []() {
        // Normally called on console logo screen
        for (auto& wp : warpPoints) {
            if (wp.second.bootToPoint) {
                Warp(wp.second);
                break;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterWarpPoints, { CVAR_BOOT_TO_FILE_SELECT_NAME });
