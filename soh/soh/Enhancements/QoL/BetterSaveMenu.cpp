#include "soh/Enhancements/custom-message/CustomMessageManager.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/randomizer/randomizer_entrance.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern PlayState* gPlayState;
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "z64scene.h"
}

#define CVAR_BETTERSAVE CVAR_ENHANCEMENT("BetterSaveMenu")
#define CVAR_BETTERSAVE_DEFAULT 0
#define CVAR_BETTERSAVE_VALUE CVarGetInteger(CVAR_BETTERSAVE, CVAR_BETTERSAVE_DEFAULT)
static CustomMessage saveMsg = CustomMessage(
    "\x08Would you like to save?&&" + CustomMessage::TWO_WAY_CHOICE() + "%gYes&No%w\x09", TEXTBOX_TYPE_BLUE);
static CustomMessage continueOverworldMsg = CustomMessage(
    "\x08 Continue?&&" + CustomMessage::TWO_WAY_CHOICE() + "%gContinue&Return to Spawn%w\x09", TEXTBOX_TYPE_BLUE);
static CustomMessage continueDungeonMsg =
    CustomMessage("\x08 Continue?&" + CustomMessage::THREE_WAY_CHOICE() + "%gContinue&Restart&Return to Spawn%w\x09",
                  TEXTBOX_TYPE_BLUE);

extern "C" uint8_t Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey);

bool IsSceneDungeon(int16_t scene) {
    switch (scene) {
        case SCENE_DEKU_TREE:
        case SCENE_DEKU_TREE_BOSS:
        case SCENE_DODONGOS_CAVERN:
        case SCENE_DODONGOS_CAVERN_BOSS:
        case SCENE_JABU_JABU:
        case SCENE_JABU_JABU_BOSS:
        case SCENE_FOREST_TEMPLE:
        case SCENE_FOREST_TEMPLE_BOSS:
        case SCENE_FIRE_TEMPLE:
        case SCENE_FIRE_TEMPLE_BOSS:
        case SCENE_WATER_TEMPLE:
        case SCENE_WATER_TEMPLE_BOSS:
        case SCENE_SPIRIT_TEMPLE:
        case SCENE_SPIRIT_TEMPLE_BOSS:
        case SCENE_SHADOW_TEMPLE:
        case SCENE_SHADOW_TEMPLE_BOSS:
        case SCENE_BOTTOM_OF_THE_WELL:
        case SCENE_GERUDO_TRAINING_GROUND:
        case SCENE_ICE_CAVERN:
        case SCENE_INSIDE_GANONS_CASTLE:
        case SCENE_GANONS_TOWER:
        case SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR:
        case SCENE_GANONS_TOWER_COLLAPSE_INTERIOR:
        case SCENE_GANONDORF_BOSS:
        case SCENE_GANON_BOSS:
        case SCENE_INSIDE_GANONS_CASTLE_COLLAPSE:
            return true;
        default:
            return false;
    }
}

void HandleSaveMenu(bool* should, PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    switch (pauseCtx->unk_1EC) {
        case 0:
            *should = false;
            Message_StartTextbox(play, TEXT_SAVE_MSG, NULL);
            pauseCtx->unk_1EC = 1;
            break;
        case 1:
            *should = false;
            if (Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(play)) {
                if (play->msgCtx.choiceIndex == 0) {
                    Audio_PlaySfxGeneral(NA_SE_SY_PIECE_OF_HEART, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                         &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                    Play_PerformSave(play);
                    pauseCtx->unk_1EC = 4;
                    if (IsSceneDungeon(gSaveContext.savedSceneNum) ||
                        CVarGetInteger(CVAR_ENHANCEMENT("RememberSaveLocation"), 0)) {
                        Message_StartTextbox(play, TEXT_CONTINUE_DUNGEON_MSG, NULL);
                    } else {
                        Message_StartTextbox(play, TEXT_CONTINUE_OVERWORLD_MSG, NULL);
                    }
                } else {
                    Interface_SetDoAction(play, DO_ACTION_NONE);
                    gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
                        gSaveContext.buttonStatus[3] = BTN_ENABLED;
                    gSaveContext.buttonStatus[5] = gSaveContext.buttonStatus[6] = gSaveContext.buttonStatus[7] =
                        gSaveContext.buttonStatus[8] = BTN_ENABLED;
                    gSaveContext.hudVisibilityMode = 0;
                    Interface_ChangeHudVisibilityMode(50);
                    pauseCtx->unk_1EC = 2;
                    WREG(2) = -6240;
                    YREG(8) = static_cast<int16_t>(pauseCtx->unk_204);
                    func_800F64E0(0);
                }
            }
            break;
        case 4:
            *should = false;
            if (Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(play)) {
                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        // Continue
                        Interface_SetDoAction(play, DO_ACTION_NONE);
                        gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
                            gSaveContext.buttonStatus[3] = BTN_ENABLED;
                        gSaveContext.buttonStatus[5] = gSaveContext.buttonStatus[6] = gSaveContext.buttonStatus[7] =
                            gSaveContext.buttonStatus[8] = BTN_ENABLED;
                        gSaveContext.hudVisibilityMode = 0;
                        Interface_ChangeHudVisibilityMode(50);
                        pauseCtx->unk_1EC = 5;
                        WREG(2) = -6240;
                        YREG(8) = static_cast<int16_t>(pauseCtx->unk_204);
                        func_800F64E0(0);
                        break;
                    case 1:
                        // Reset (Dungeon) / Return to Spawn (Overworld)
                        Audio_PlaySfxGeneral(NA_SE_SY_PIECE_OF_HEART, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                        Play_SaveSceneFlags(play);
                        Sram_OpenSave();
                        pauseCtx->promptChoice = 0;
                        pauseCtx->unk_1EC = 7;
                        break;
                    case 2:
                        // Reset to Spawn (Dungeon)
                        Audio_PlaySfxGeneral(NA_SE_SY_PIECE_OF_HEART, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                        Play_SaveSceneFlags(play);
                        Sram_OpenSave();
                        gSaveContext.ship.resetToSpawn = 1;
                        pauseCtx->promptChoice = 0;
                        pauseCtx->unk_1EC = 7;
                        break;
                }
            }
            break;
        case 7:
            if (interfaceCtx->unk_244 != 255) {
                interfaceCtx->unk_244 += 10;
                if (interfaceCtx->unk_244 >= 255) {
                    interfaceCtx->unk_244 = 255;
                    GameInteractor_ExecuteOnExitGame(gSaveContext.fileNum);
                    pauseCtx->state = 0;
                    R_UPDATE_RATE = 3;
                    R_PAUSE_MENU_MODE = 0;
                    func_800981B8(&play->objectCtx);
                    func_800418D0(&play->colCtx, play);
                    // Reset frame counter to prevent autosave on respawn
                    play->gameplayFrames = 0;
                    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
                    Audio_QueueSeqCmd(0xF << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0xA);
                    gSaveContext.healthAccumulator = 0;
                    gSaveContext.magicState = MAGIC_STATE_IDLE;
                    gSaveContext.prevMagicState = MAGIC_STATE_IDLE;
                    gSaveContext.magicCapacity = 0;
                    gSaveContext.magicFillTarget = gSaveContext.magic;
                    gSaveContext.magicLevel = gSaveContext.magic = 0;
                    play->state.running = false;
                    SET_NEXT_GAMESTATE(&play->state, Play_Init, PlayState);
                    gSaveContext.seqId = static_cast<uint8_t>(NA_BGM_DISABLED);
                    gSaveContext.natureAmbienceId = 0xFF;
                    GameInteractor_ExecuteOnLoadGame(gSaveContext.fileNum);
                    if (gSaveContext.ship.resetToSpawn) {
                        if (LINK_IS_CHILD) {
                            gSaveContext.entranceIndex =
                                Entrance_OverrideNextIndex(ENTR_LINKS_HOUSE_CHILD_SPAWN); // Child Overworld Spawn
                        } else {
                            // Adult Overworld Spawn. Normally 0x5F4 (ENTR_TEMPLE_OF_TIME_WARP_PAD), but 0x282
                            // (ENTR_HYRULE_FIELD_10) has been repurposed to differentiate from Prelude which also uses
                            // 0x5F4
                            gSaveContext.entranceIndex = Entrance_OverrideNextIndex(ENTR_HYRULE_FIELD_10);
                        }
                        gSaveContext.ship.resetToSpawn = 0;
                    }
                }
            }
            break;

        default:
            *should = true;
    }
    Message_Update(play);
}

void RegisterBetterSave() {
    saveMsg.Format();
    continueOverworldMsg.Format();
    continueDungeonMsg.Format();

    COND_VB_SHOULD(VB_LOAD_SAVE_MENU, CVAR_BETTERSAVE_VALUE, {
        PlayState* play = va_arg(args, PlayState*);
        HandleSaveMenu(should, play);
    });

    COND_VB_SHOULD(VB_DRAW_SAVE_MENU, CVAR_BETTERSAVE_VALUE, { *should = false; });

    COND_ID_HOOK(OnOpenText, TEXT_SAVE_MSG, CVAR_BETTERSAVE_VALUE, [](uint16_t* textId, bool* loadFromMessageTable) {
        saveMsg.LoadIntoFont();
        *loadFromMessageTable = false;
        return;
    });

    COND_ID_HOOK(OnOpenText, TEXT_CONTINUE_DUNGEON_MSG, CVAR_BETTERSAVE_VALUE,
                 [](uint16_t* textId, bool* loadFromMessageTable) {
                     continueDungeonMsg.LoadIntoFont();
                     *loadFromMessageTable = false;
                     return;
                 });

    COND_ID_HOOK(OnOpenText, TEXT_CONTINUE_OVERWORLD_MSG, CVAR_BETTERSAVE_VALUE,
                 [](uint16_t* textId, bool* loadFromMessageTable) {
                     continueOverworldMsg.LoadIntoFont();
                     *loadFromMessageTable = false;
                     return;
                 });
}

static RegisterShipInitFunc initFunc(RegisterBetterSave, { CVAR_BETTERSAVE });
