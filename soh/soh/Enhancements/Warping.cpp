#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "functions.h"
#include "soh/SohGui/MenuTypes.h"
#include "soh/util.h"

extern "C" {
#include "z64.h"
#include "overlays/gamestates/ovl_file_choose/file_choose.h"
#include "soh/Enhancements/enhancementTypes.h"
void Sram_InitDebugSave(void);
void Select_LoadGame(SelectContext* selectContext, s32 entranceIndex);
}

#define CVAR_BOOTSEQUENCE_NAME CVAR_SETTING("BootSequence")
#define CVAR_BOOTSEQUENCE_DEFAULT BOOTSEQUENCE_DEFAULT
#define CVAR_BOOTSEQUENCE_VALUE CVarGetInteger(CVAR_BOOTSEQUENCE_NAME, CVAR_BOOTSEQUENCE_DEFAULT)

typedef struct WarpPoint {
    s32 entranceId;
    s8 roomNum;
    Vec3f pos;
    s16 rotY;
    bool bootToPoint;
} WarpPoint;
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vec3f, x, y, z)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WarpPoint, entranceId, roomNum, pos, rotY, bootToPoint)
std::map<std::string, WarpPoint> warpPoints;

void LoadConfig() {
    auto allConfig = Ship::Context::GetInstance()->GetConfig()->GetNestedJson();
    if (allConfig.find("WarpPoints") == allConfig.end() || !allConfig["WarpPoints"].is_object()) {
        allConfig["WarpPoints"] = nlohmann::json::object();
    }
    warpPoints = allConfig["WarpPoints"];
}

void SaveConfig() {
    auto allConfig = Ship::Context::GetInstance()->GetConfig()->GetNestedJson();
    allConfig["WarpPoints"] = warpPoints;
    Ship::Context::GetInstance()->GetConfig()->SetBlock("WarpPoints", warpPoints);
    Ship::Context::GetInstance()->GetConfig()->Save();
}

void Warp(WarpPoint& warpPoint) {
    if (gPlayState == NULL) {
        // If gPlayState is NULL, it means the the user opted into BootToWarpPoint and the game is starting up.
        gSaveContext.gameMode = GAMEMODE_NORMAL;
        gSaveContext.fileNum = 0xFE; // temporary file so that this will respect debug save file option
        Sram_InitDebugSave();
        gSaveContext.magicFillTarget = gSaveContext.magic;
        gSaveContext.magic = 0;
        gSaveContext.magicCapacity = 0;
        gSaveContext.magicLevel = gSaveContext.magic;
        gSaveContext.fileNum = 0xFF;
        gSaveContext.sceneSetupIndex = 0;
        gSaveContext.cutsceneIndex = 0;
        gSaveContext.linkAge = 0;
        gSaveContext.nightFlag = 0;
        gSaveContext.skyboxTime = gSaveContext.dayTime = 0x8000;

        // Copied from Select_LoadGame
        for (int buttonIndex = 0; buttonIndex < ARRAY_COUNT(gSaveContext.buttonStatus); buttonIndex++) {
            gSaveContext.buttonStatus[buttonIndex] = BTN_ENABLED;
        }
        gSaveContext.forceRisingButtonAlphas = gSaveContext.unk_13E8 = gSaveContext.unk_13EA = gSaveContext.unk_13EC =
            0;
        Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_STOP);
        gSaveContext.entranceIndex = warpPoint.entranceId;

        gSaveContext.seqId = (u8)NA_BGM_DISABLED;
        gSaveContext.natureAmbienceId = 0xFF;
        gSaveContext.showTitleCard = true;
        gWeatherMode = 0;
        gGameState->running = false;
        SET_NEXT_GAMESTATE(gGameState, Play_Init, PlayState);

        GameInteractor_ExecuteOnLoadGame(gSaveContext.fileNum);
    } else {
        gPlayState->nextEntranceIndex = warpPoint.entranceId;
        gPlayState->transitionTrigger = TRANS_TRIGGER_START;
        gPlayState->transitionType = TRANS_TYPE_INSTANT;
    }
    gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = warpPoint.entranceId;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].roomIndex = warpPoint.roomNum;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].pos = warpPoint.pos;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw = warpPoint.rotY;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = 0xDFF;
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK_FAST;
    gSaveContext.respawnFlag = 1;
    static HOOK_ID hookId = 0;
    hookId = REGISTER_VB_SHOULD(VB_INFLICT_VOID_DAMAGE, {
        *should = false;
        GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnVanillaBehavior>(hookId);
    });
}

static std::string warpNameInput = "";

void WarpPointsWidget(WidgetInfo& info) {
    ImGui::SeparatorText("Warp Points");
    if (gPlayState != NULL && GET_PLAYER(gPlayState) != NULL) {
        UIWidgets::InputString("##WarpPointNameInput", &warpNameInput,
                               {
                                   .size = ImVec2(ImGui::GetContentRegionAvail().x - 50.0f, 0.0f),
                                   .placeholder = "Enter warp point name...",
                               });

        ImGui::SameLine();
        bool isEmpty = warpNameInput.empty();
        if (isEmpty) {
            ImGui::BeginDisabled();
        }

        if (UIWidgets::Button(ICON_FA_PLUS)) {
            Player* player = GET_PLAYER(gPlayState);

            std::string warpName = SohUtils::GetSceneName(gPlayState->sceneNum);
            if (gPlayState->roomCtx.curRoom.num != 0) {
                warpName += " (" + std::to_string(gPlayState->roomCtx.curRoom.num) + ")";
            }

            warpPoints[warpNameInput] = WarpPoint{
                .entranceId = gSaveContext.entranceIndex,
                .roomNum = gPlayState->roomCtx.curRoom.num,
                .pos = player->actor.world.pos,
                .rotY = player->actor.shape.rot.y,
            };
            SaveConfig();
            warpNameInput = "";
        }
        if (isEmpty) {
            ImGui::EndDisabled();
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
            for (auto& wp : warpPoints) {
                wp.second.bootToPoint = false;
            }
            it->second.bootToPoint = true;
            SaveConfig();
        }
        ImGui::SameLine();
        if (UIWidgets::Button(ICON_FA_TRASH, { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
            it = warpPoints.erase(it);
            SaveConfig();
            ImGui::PopID();
            continue;
            ;
        }
        ImGui::PopID();

        ++it;
    }
}

void RegisterWarping() {
    static bool loadedConfig = false;
    if (!loadedConfig) {
        LoadConfig();
        loadedConfig = true;
    }

    COND_HOOK(OnZTitleUpdate, CVAR_BOOTSEQUENCE_VALUE == BOOTSEQUENCE_DEBUGWARPSCREEN, [](void* gameState) {
        TitleContext* titleContext = (TitleContext*)gameState;

        gSaveContext.seqId = (u8)NA_BGM_DISABLED;
        gSaveContext.natureAmbienceId = 0xFF;
        gSaveContext.gameMode = GAMEMODE_NORMAL;
        titleContext->state.running = false;
        SET_NEXT_GAMESTATE(&titleContext->state, Select_Init, SelectContext);
    });

    COND_HOOK(OnZTitleUpdate, CVAR_BOOTSEQUENCE_VALUE == BOOTSEQUENCE_WARPPOINT, [](void* gameState) {
        for (auto& wp : warpPoints) {
            if (wp.second.bootToPoint) {
                Warp(wp.second);
                return;
            }
        }

        // Fallback to Debug Warp Screen if no warp point is set to boot to
        TitleContext* titleContext = (TitleContext*)gameState;

        gSaveContext.seqId = (u8)NA_BGM_DISABLED;
        gSaveContext.natureAmbienceId = 0xFF;
        gSaveContext.gameMode = GAMEMODE_NORMAL;
        titleContext->state.running = false;
        SET_NEXT_GAMESTATE(&titleContext->state, Select_Init, SelectContext);
    });
}

static RegisterShipInitFunc initFunc(RegisterWarping, { CVAR_BOOTSEQUENCE_NAME });
