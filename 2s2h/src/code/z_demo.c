#include "prevent_bss_reordering.h"
#include "prevent_bss_reordering2.h"
#include "PR/ultratypes.h"

#include "z64quake.h"
#include "z64rumble.h"
#include "z64shrink_window.h"
#include "overlays/gamestates/ovl_daytelop/z_daytelop.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include <string.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include <libultraship/log/luslog.h>

s16 sCutsceneQuakeIndex;
struct CutsceneCamera sCutsceneCameraInfo;
u16 sCueTypeList[10];
u8 D_801F4DDC;
static s16 sBssPad;
u8 gDisablePlayerCsActionStartPos;
s16 gDungeonBossWarpSceneId;

// clang-format off
// Partial structs taken from "prevent_bss_reordering.h"
struct Dummy200 { int x; };
struct Dummy201 { int x; };
struct Dummy202 { int x; };
struct Dummy203 { int x; };
struct Dummy204 { int x; };
struct Dummy205 { int x; };
struct Dummy206 { int x; };
struct Dummy207 { int x; };
struct Dummy208 { int x; };
struct Dummy209 { int x; };
struct Dummy210 { int x; };
struct Dummy211 { int x; };
struct Dummy212 { int x; };
struct Dummy213 { int x; };
struct Dummy214 { int x; };
struct Dummy215 { int x; };
struct Dummy216 { int x; };
struct Dummy217 { int x; };
// clang-format on

void CutsceneHandler_DoNothing(PlayState* play, CutsceneContext* csCtx);
void CutsceneHandler_StartManual(PlayState* play, CutsceneContext* csCtx);
void CutsceneHandler_StopManual(PlayState* play, CutsceneContext* csCtx);
void CutsceneHandler_StartScript(PlayState* play, CutsceneContext* csCtx);
void CutsceneHandler_RunScript(PlayState* play, CutsceneContext* csCtx);
void CutsceneHandler_StopScript(PlayState* play, CutsceneContext* csCtx);
void Cutscene_SetupScripted(PlayState* play, CutsceneContext* csCtx);

static s32 sPad = 0;
u16 sCurTextId = 0;
u16 sCurOcarinaAction = 0;
u8 gOpeningEntranceIndex = 0;
u8 sCutsceneStoredPlayerForm = 0;

void Cutscene_InitContext(PlayState* play, CutsceneContext* csCtx) {
    s32 i;

    csCtx->state = CS_STATE_IDLE;
    csCtx->curFrame = 0;
    csCtx->timer = 0.0f;
    play->csCtx.scriptListCount = 0;
    play->csCtx.scriptIndex = 0;

    for (i = 0; i < ARRAY_COUNT(sCueTypeList); i++) {
        sCueTypeList[i] = 0;
    }

    gDisablePlayerCsActionStartPos = false;

    Audio_SetCutsceneFlag(false);
}

void Cutscene_StartManual(PlayState* play, CutsceneContext* csCtx) {
    csCtx->state = CS_STATE_START;
    csCtx->playerCue = NULL;
}

void Cutscene_StopManual(PlayState* play, CutsceneContext* csCtx) {
    if (csCtx->state != CS_STATE_RUN_UNSTOPPABLE) {
        csCtx->state = CS_STATE_STOP;
    }
}

typedef void (*CutsceneHandler)(PlayState* play, CutsceneContext* csCtx);

CutsceneHandler sManualCutsceneHandlers[] = {
    CutsceneHandler_DoNothing,   // CS_STATE_IDLE
    CutsceneHandler_StartManual, // CS_STATE_START
    CutsceneHandler_DoNothing,   // CS_STATE_RUN
    CutsceneHandler_StopManual,  // CS_STATE_STOP
    CutsceneHandler_DoNothing,   // CS_STATE_RUN_UNSTOPPABLE
};

void Cutscene_UpdateManual(PlayState* play, CutsceneContext* csCtx) {
    if (gSaveContext.save.cutsceneIndex < 0xFFF0) {
        sManualCutsceneHandlers[csCtx->state](play, csCtx);
    }
}

CutsceneHandler sScriptedCutsceneHandlers[] = {
    CutsceneHandler_DoNothing,   // CS_STATE_IDLE
    CutsceneHandler_StartScript, // CS_STATE_START
    CutsceneHandler_RunScript,   // CS_STATE_RUN
    CutsceneHandler_StopScript,  // CS_STATE_STOP
    CutsceneHandler_RunScript,   // CS_STATE_RUN_UNSTOPPABLE
};

void Cutscene_UpdateScripted(PlayState* play, CutsceneContext* csCtx) {
    if ((gSaveContext.cutsceneTrigger != 0) && (play->transitionTrigger == TRANS_TRIGGER_START)) {
        gSaveContext.cutsceneTrigger = 0;
    }

    if ((gSaveContext.cutsceneTrigger != 0) && (csCtx->state == CS_STATE_IDLE)) {
        gSaveContext.save.cutsceneIndex = 0xFFFD;
        gSaveContext.cutsceneTrigger = 1;
    }

    if (gSaveContext.save.cutsceneIndex >= 0xFFF0) {
        Cutscene_SetupScripted(play, csCtx);
        sScriptedCutsceneHandlers[csCtx->state](play, csCtx);
    }
}

void CutsceneHandler_DoNothing(PlayState* play, CutsceneContext* csCtx) {
}

s32 Cutscene_StepTimer(PlayState* play, CutsceneContext* csCtx, f32 target) {
    return Math_StepToF(&csCtx->timer, target, 0.1f);
}

void CutsceneHandler_StartManual(PlayState* play, CutsceneContext* csCtx) {
    Interface_SetHudVisibility(HUD_VISIBILITY_NONE);
    ShrinkWindow_Letterbox_SetSizeTarget(32);

    if (Cutscene_StepTimer(play, csCtx, 1.0f)) {
        Audio_SetCutsceneFlag(true);
        csCtx->state++; // CS_STATE_RUN
    }
}

void CutsceneHandler_StartScript(PlayState* play, CutsceneContext* csCtx) {
    CutsceneHandler_RunScript(play, csCtx);
    Interface_SetHudVisibility(HUD_VISIBILITY_NONE);
    ShrinkWindow_Letterbox_SetSizeTarget(32);

    if (Cutscene_StepTimer(play, csCtx, 1.0f)) {
        Audio_SetCutsceneFlag(true);
        csCtx->state++; // CS_STATE_RUN
    }
}

/* Start of command handling section */

void CutsceneCmd_Misc(PlayState* play, CutsceneContext* csCtx, CsCmdMisc* cmd) {
    static u16 D_801BB15C = 0xFFFF;
    Player* player = GET_PLAYER(play);
    f32 lerp;
    u8 isFirstFrame = false;
    SceneTableEntry* loadedScene;

    if (!((csCtx->curFrame >= cmd->startFrame) &&
          ((csCtx->curFrame < cmd->endFrame) || (cmd->endFrame == cmd->startFrame)))) {
        return;
    }

    lerp = Environment_LerpWeight(cmd->endFrame - 1, cmd->startFrame, csCtx->curFrame);

    if (csCtx->curFrame == cmd->startFrame) {
        isFirstFrame = true;
    }

    switch (cmd->type) {
        case CS_MISC_RAIN:
            if (isFirstFrame) {
                Environment_PlayStormNatureAmbience(play);
                play->envCtx.precipitation[PRECIP_RAIN_MAX] = 60;
            }
            break;

        case CS_MISC_LIGHTNING:
            if (isFirstFrame) {
                Audio_SetAmbienceChannelIO(AMBIENCE_CHANNEL_LIGHTNING, CHANNEL_IO_PORT_0, 0);
                Environment_AddLightningBolts(play, 3);
                gLightningStrike.state = LIGHTNING_STRIKE_START;
            }
            break;

        case CS_MISC_LIFT_FOG:
            if (play->envCtx.adjLightSettings.zFar < 12800) {
                play->envCtx.adjLightSettings.zFar += 35;
            }
            break;

        case CS_MISC_CLOUDY_SKY:
            if (isFirstFrame) {
                play->envCtx.changeSkyboxState = CHANGE_SKYBOX_REQUESTED;
                play->envCtx.skyboxConfig = SKYBOX_CONFIG_1;
                play->envCtx.changeSkyboxNextConfig = SKYBOX_CONFIG_0;
                play->envCtx.changeSkyboxTimer = 60;
                play->envCtx.changeLightEnabled = true;
                play->envCtx.lightConfig = 0;
                play->envCtx.changeLightNextConfig = 1;
                play->envCtx.changeDuration = 60;
                play->envCtx.changeLightTimer = play->envCtx.changeDuration;
            }
            break;

        case CS_MISC_STOP_CUTSCENE:
            if (isFirstFrame && (csCtx->state != CS_STATE_RUN_UNSTOPPABLE)) {
                csCtx->state = CS_STATE_STOP;
            }
            break;

        case CS_MISC_SHOW_TITLE_CARD:
            if (isFirstFrame) {
                loadedScene = play->loadedScene;
                if (loadedScene->titleTextId != 0) {
                    Message_DisplaySceneTitleCard(play, loadedScene->titleTextId);
                }
            }
            break;

        case CS_MISC_EARTHQUAKE_MEDIUM:
            Audio_PlaySfx_2(NA_SE_EV_EARTHQUAKE_LAST - SFX_FLAG);
            if (isFirstFrame) {
                sCutsceneQuakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_6);
                Quake_SetSpeed(sCutsceneQuakeIndex, 22000);
                Quake_SetPerturbations(sCutsceneQuakeIndex, 6, 4, 0, 0);
                Quake_SetDuration(sCutsceneQuakeIndex, 800);
            }
            break;

        case CS_MISC_EARTHQUAKE_STOP:
            if (isFirstFrame) {
                Quake_Init();
            }
            break;

        case CS_MISC_VISMONO_BLACK_AND_WHITE:
            gVisMonoColor.r = 255;
            gVisMonoColor.g = 255;
            gVisMonoColor.b = 255;
            gVisMonoColor.a = 255 * lerp;
            break;

        case CS_MISC_VISMONO_SEPIA:
            gVisMonoColor.r = 255;
            gVisMonoColor.g = 180;
            gVisMonoColor.b = 100;
            gVisMonoColor.a = 255 * lerp;
            break;

        case CS_MISC_HIDE_ROOM:
            play->roomCtx.curRoom.segment = NULL;
            break;

        case CS_MISC_RED_PULSATING_LIGHTS:
            if (play->state.frames & 8) {
                if (play->envCtx.adjLightSettings.ambientColor[0] < 40) {
                    play->envCtx.adjLightSettings.ambientColor[0] += 2;
                    play->envCtx.adjLightSettings.light1Color[1] -= 3;
                    play->envCtx.adjLightSettings.light1Color[2] -= 3;
                }
            } else {
                if (play->envCtx.adjLightSettings.ambientColor[0] > 2) {
                    play->envCtx.adjLightSettings.ambientColor[0] -= 2;
                    play->envCtx.adjLightSettings.light1Color[1] += 3;
                    play->envCtx.adjLightSettings.light1Color[2] += 3;
                }
            }
            break;

        case CS_MISC_HALT_ALL_ACTORS:
            play->haltAllActors = true;
            break;

        case CS_MISC_RESUME_ALL_ACTORS:
            play->haltAllActors = false;
            break;

        case CS_MISC_SANDSTORM_FILL:
            if (isFirstFrame) {
                play->envCtx.sandstormState = SANDSTORM_FILL;
            }
            Audio_PlaySfx_2(NA_SE_EV_SAND_STORM - SFX_FLAG);
            break;

        case CS_MISC_SUNSSONG_START:
            gSaveContext.sunsSongState = SUNSSONG_START;
            break;

        case CS_MISC_FREEZE_TIME:
            if (!gSaveContext.save.isNight) {
                gSaveContext.save.time = CURRENT_TIME - (u16)R_TIME_SPEED;
            } else {
                gSaveContext.save.time = CURRENT_TIME - (u16)(2 * R_TIME_SPEED);
            }
            break;

        case CS_MISC_LONG_SCARECROW_SONG:
            AudioOcarina_PlayLongScarecrowSong();
            csCtx->curFrame = cmd->startFrame - 1; // the cutscene runs forever
            break;

        case CS_MISC_SET_CSFLAG_3:
            CutsceneFlags_Set(play, 3);
            break;

        case CS_MISC_SET_CSFLAG_4:
            CutsceneFlags_Set(play, 4);
            break;

        case CS_MISC_PLAYER_FORM_DEKU:
            gSaveContext.save.playerForm = PLAYER_FORM_DEKU;
            break;

        case CS_MISC_ENABLE_PLAYER_REFLECTION:
            player->stateFlags2 |= PLAYER_STATE2_4000000;
            break;

        case CS_MISC_DISABLE_PLAYER_REFLECTION:
            player->stateFlags2 &= ~PLAYER_STATE2_4000000;
            break;

        case CS_MISC_PLAYER_FORM_HUMAN:
            sCutsceneStoredPlayerForm = GET_PLAYER_FORM;
            gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
            gSaveContext.save.equippedMask = PLAYER_MASK_NONE;
            break;

        case CS_MISC_EARTHQUAKE_STRONG:
            Audio_PlaySfx_2(NA_SE_EV_EARTHQUAKE_LAST2 - SFX_FLAG);
            if (isFirstFrame) {
                sCutsceneQuakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_6);
                Quake_SetSpeed(sCutsceneQuakeIndex, 30000);
                Quake_SetPerturbations(sCutsceneQuakeIndex, 20, 10, 0, 0);
                Quake_SetDuration(sCutsceneQuakeIndex, 800);
            }
            break;

        case CS_MISC_DEST_MOON_CRASH_FIRE_WALL:
            if (isFirstFrame) {
                play->nextEntrance = ENTRANCE(CUTSCENE, 0);
                gSaveContext.nextCutsceneIndex = 0xFFF8;
                play->transitionTrigger = TRANS_TRIGGER_START;
                play->transitionType = TRANS_TYPE_FADE_WHITE;
            }
            break;

        case CS_MISC_MOON_CRASH_SKYBOX:
            if (isFirstFrame) {
                play->envCtx.skyboxConfig = SKYBOX_CONFIG_13;
            }
            break;

        case CS_MISC_PLAYER_FORM_RESTORED:
            gSaveContext.save.playerForm = sCutsceneStoredPlayerForm;
            break;

        case CS_MISC_DISABLE_PLAYER_CSACTION_START_POS:
            gDisablePlayerCsActionStartPos = true;
            break;

        case CS_MISC_ENABLE_PLAYER_CSACTION_START_POS:
            gDisablePlayerCsActionStartPos = false;
            break;

        case CS_MISC_SAVE_ENTER_CLOCK_TOWN:
            if (isFirstFrame) {
                Sram_SaveSpecialEnterClockTown(play);
            }
            break;

        case CS_MISC_RESET_SAVE_FROM_MOON_CRASH:
            if (isFirstFrame) {
                Sram_ResetSaveFromMoonCrash(&play->sramCtx);
            }
            break;

        case CS_MISC_TIME_ADVANCE:
            if (csCtx->curFrame != D_801BB15C) {
                D_801BB15C = csCtx->curFrame;

                if (R_TIME_SPEED != 0) {
                    gSaveContext.save.time = CURRENT_TIME + (u16)R_TIME_SPEED;
                    gSaveContext.save.time = CURRENT_TIME + (u16)((void)0, gSaveContext.save.timeSpeedOffset);
                }
            }
            break;

        case CS_MISC_EARTHQUAKE_WEAK:
            Audio_PlaySfx_2(NA_SE_EV_EARTHQUAKE_LAST - SFX_FLAG);
            if (isFirstFrame) {
                sCutsceneQuakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_6);
                Quake_SetSpeed(sCutsceneQuakeIndex, 22000);
                Quake_SetPerturbations(sCutsceneQuakeIndex, 2, 1, 0, 0);
                Quake_SetDuration(sCutsceneQuakeIndex, 800);
            }
            break;

        case CS_MISC_DAWN_OF_A_NEW_DAY:
            gSaveContext.save.day = 9; // 9 % 5 is day number 4, see `CURRENT_DAY`

            STOP_GAMESTATE(&play->state);
            SET_NEXT_GAMESTATE(&play->state, DayTelop_Init, sizeof(DayTelopState));

            Sram_SaveSpecialNewDay(play);
            break;

        case CS_MISC_PLAYER_FORM_ZORA:
            gSaveContext.save.playerForm = PLAYER_FORM_ZORA;
            break;

        case CS_MISC_FINALE:
            csCtx->curFrame = cmd->startFrame - 1; // the cutscene runs forever
            break;

        default:
            break;
    }
}

void CutsceneCmd_SetLightSetting(PlayState* play, CutsceneContext* csCtx, CsCmdLightSetting* cmd) {
    if (csCtx->curFrame == cmd->startFrame) {
        if (cmd->settingPlusOne != 32) {
            play->envCtx.lightSettingOverride = cmd->settingPlusOne - 1;
            play->envCtx.lightBlend = 1.0f;
        } else {
            play->envCtx.lightSettingOverride = LIGHT_SETTING_OVERRIDE_NONE;
        }
    }
}

void CutsceneCmd_StartSequence(PlayState* play, CutsceneContext* csCtx, CsCmdStartSeq* cmd) {
    if (csCtx->curFrame == cmd->startFrame) {
        Audio_PlaySequenceInCutscene(cmd->seqIdPlusOne - 1);
    }
}

void CutsceneCmd_StopSequence(PlayState* play, CutsceneContext* csCtx, CsCmdStopSeq* cmd) {
    if ((csCtx->curFrame >= cmd->startFrame) && (csCtx->curFrame <= cmd->endFrame)) {
        Audio_StopSequenceInCutscene(cmd->seqIdPlusOne - 1);
    }
}

void CutsceneCmd_FadeOutSequence(PlayState* play, CutsceneContext* csCtx, CsCmdFadeOutSeq* cmd) {
    if ((csCtx->curFrame == cmd->startFrame) && (csCtx->curFrame < cmd->endFrame)) {
        u8 fadeOutDuration = cmd->endFrame - cmd->startFrame;

        if (cmd->seqPlayer == CS_FADE_OUT_FANFARE) {
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, fadeOutDuration);
        } else { // CS_FADE_OUT_BGM_MAIN
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, fadeOutDuration);
        }
    }
}

void CutsceneCmd_StartAmbience(PlayState* play, CutsceneContext* csCtx, CsCmdStartAmbience* cmd) {
    if (csCtx->curFrame == cmd->startFrame) {
        Audio_PlayAmbience(play->sceneSequences.ambienceId);
    }
}

void Cutscene_SetSfxReverbIndexTo2(PlayState* play, CutsceneContext* csCtx, CsCmdSfxReverbIndexTo2* cmd) {
    if (csCtx->curFrame == cmd->startFrame) {
        Audio_SetSfxReverbIndexExceptOcarinaBank(2);
    }
}

void Cutscene_SetSfxReverbIndexTo1(PlayState* play, CutsceneContext* csCtx, CsCmdSfxReverbIndexTo1* cmd) {
    if (csCtx->curFrame == cmd->startFrame) {
        Audio_SetSfxReverbIndexExceptOcarinaBank(1);
    }
}

void CutsceneCmd_ModifySequence(PlayState* play, CutsceneContext* csCtx, CsCmdModifySeq* cmd) {
    static u16 sSeqId;
    u8 dayMinusOne;

    if (csCtx->curFrame == cmd->startFrame) {
        dayMinusOne = gSaveContext.save.day - 1;
        if (dayMinusOne >= 3) {
            dayMinusOne = 0;
        }

        switch (cmd->type) {
            case CS_MOD_SEQ_0:
                func_801A246C(SEQ_PLAYER_BGM_MAIN, 1);
                break;

            case CS_MOD_SEQ_1:
                func_801A246C(SEQ_PLAYER_BGM_MAIN, 0);
                break;

            case CS_MOD_SEQ_2:
                func_801A246C(SEQ_PLAYER_BGM_MAIN, 2);
                break;

            case CS_MOD_AMBIENCE_0:
                func_801A246C(SEQ_PLAYER_AMBIENCE, 1);
                break;

            case CS_MOD_AMBIENCE_1:
                func_801A246C(SEQ_PLAYER_AMBIENCE, 0);
                break;

            case CS_MOD_AMBIENCE_2:
                func_801A246C(SEQ_PLAYER_AMBIENCE, 2);
                break;

            case CS_MOD_SEQ_STORE:
                sSeqId = AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN);
                break;

            case CS_MOD_SEQ_RESTORE:
                if (sSeqId != NA_BGM_DISABLED) {
                    Audio_PlaySceneSequence(sSeqId, dayMinusOne);
                }
                break;

            default:
                break;
        }
    }
}

void CutsceneCmd_FadeOutAmbience(PlayState* play, CutsceneContext* csCtx, CsCmdFadeOutAmbience* cmd) {
    if ((csCtx->curFrame == cmd->startFrame) && (csCtx->curFrame < cmd->endFrame)) {
        u8 fadeOutDuration = cmd->endFrame - cmd->startFrame;

        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_AMBIENCE, fadeOutDuration);
    }
}

void CutsceneCmd_RumbleController(PlayState* play, CutsceneContext* csCtx, CsCmdRumble* cmd) {
    switch (cmd->type) {
        case CS_RUMBLE_ONCE:
            if (csCtx->curFrame == cmd->startFrame) {
                Rumble_Request(0.0f, cmd->intensity, cmd->decayTimer, cmd->decayStep);
            }
            break;

        case CS_RUMBLE_PULSE:
            if ((csCtx->curFrame >= cmd->startFrame) && (csCtx->curFrame <= cmd->endFrame)) {
                if ((csCtx->curFrame == cmd->startFrame) || (play->state.frames % 64 == 0)) {
                    Rumble_Request(0.0f, cmd->intensity, cmd->decayTimer, cmd->decayStep);
                }
            }
            break;

        default:
            break;
    }
}

void CutsceneCmd_TransitionGeneral(PlayState* play, CutsceneContext* csCtx, CsCmdTransitionGeneral* cmd) {
    if ((csCtx->curFrame >= cmd->startFrame) && (cmd->endFrame >= csCtx->curFrame)) {
        f32 alpha;

        play->envCtx.fillScreen = true;
        alpha = Environment_LerpWeight(cmd->endFrame, cmd->startFrame, csCtx->curFrame);

        if (((cmd->type == CS_TRANS_GENERAL_FILL_IN)) || (cmd->type == CS_TRANS_GENERAL_FILL_OUT)) {
            play->envCtx.screenFillColor[0] = cmd->color.r;
            play->envCtx.screenFillColor[1] = cmd->color.g;
            play->envCtx.screenFillColor[2] = cmd->color.b;

            if (cmd->type == CS_TRANS_GENERAL_FILL_OUT) {
                play->envCtx.screenFillColor[3] = (1.0f - alpha) * 255.0f;
            } else {
                play->envCtx.screenFillColor[3] = 255.0f * alpha;
            }
        }
    }
}

void CutsceneCmd_SetTime(PlayState* play, CutsceneContext* csCtx, CsCmdTime* cmd) {
    u16 hourAsMinutes;
    u16 minutes;

    if (csCtx->curFrame == cmd->startFrame) {
        hourAsMinutes = CLOCK_TIME_ALT_F(cmd->hour, 0);
        minutes = CLOCK_TIME_ALT_F(0, cmd->minute + 1);

        gSaveContext.save.time = hourAsMinutes + minutes;
        gSaveContext.skyboxTime = hourAsMinutes + minutes;
    }
}

void CutsceneCmd_DestinationDefault(PlayState* play, CutsceneContext* csCtx, CsCmdDestination* cmd) {
    csCtx->state = CS_STATE_RUN_UNSTOPPABLE;
    Play_DisableMotionBlur();
    Audio_SetCutsceneFlag(false);
    gSaveContext.cutsceneTransitionControl = 1;

    // `hudVisibilityForceButtonAlphasByStatus` has a secondary purpose, which is to signal to the title
    // screen actor that it should display immediately. This occurs when a title screen cutscene that
    // is not the main clock town scene is skipped.
    if ((gSaveContext.gameMode != GAMEMODE_NORMAL) && (csCtx->curFrame != cmd->startFrame)) {
        gSaveContext.hudVisibilityForceButtonAlphasByStatus = true;
    }

    gSaveContext.save.cutsceneIndex = 0;

    if (cmd->type == CS_DESTINATION_DEFAULT) {
        play->nextEntrance = play->csCtx.scriptList[play->csCtx.scriptIndex].nextEntrance;
        gSaveContext.nextCutsceneIndex = 0;
        play->transitionTrigger = TRANS_TRIGGER_START;

        if (gSaveContext.gameMode != GAMEMODE_TITLE_SCREEN) {
            Scene_SetExitFade(play);
        } else {
            gOpeningEntranceIndex++;
            if (gOpeningEntranceIndex >= 2) {
                gOpeningEntranceIndex = 0;
            }
            play->transitionType = TRANS_TYPE_FADE_BLACK_FAST;
        }

        if ((play->nextEntrance & 0xF) > 0) {
            gSaveContext.nextCutsceneIndex = (play->nextEntrance & 0xF) + 0xFFEF;
        }

        play->nextEntrance &= ~0xF;
    }
}

void CutsceneCmd_Destination(PlayState* play, CutsceneContext* csCtx, CsCmdDestination* cmd) {
    if (cmd->type == CS_DESTINATION_DEFAULT) {
        if (csCtx->curFrame == cmd->startFrame) {
            CutsceneCmd_DestinationDefault(play, csCtx, cmd);
        }
    } else if (cmd->type == CS_DESTINATION_BOSS_WARP) {
        if (csCtx->curFrame == cmd->startFrame) {
            Play_DisableMotionBlur();

            switch (gDungeonBossWarpSceneId) {
                case SCENE_MITURIN_BS:
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)) {
                        play->nextEntrance = ENTRANCE(WOODFALL_TEMPLE, 1);
                        play->transitionTrigger = TRANS_TRIGGER_START;
                        play->transitionType = TRANS_TYPE_FADE_WHITE;
                    } else {
                        play->nextEntrance = ENTRANCE(WOODFALL, 0);
                        gSaveContext.nextCutsceneIndex = 0xFFF0;
                        play->transitionTrigger = TRANS_TRIGGER_START;
                        play->transitionType = TRANS_TYPE_FADE_WHITE;
                    }
                    break;

                case SCENE_HAKUGIN_BS:
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE)) {
                        play->nextEntrance = ENTRANCE(MOUNTAIN_VILLAGE_SPRING, 7);
                        play->transitionTrigger = TRANS_TRIGGER_START;
                        play->transitionType = TRANS_TYPE_FADE_WHITE;
                    } else {
                        play->nextEntrance = ENTRANCE(MOUNTAIN_VILLAGE_SPRING, 0);
                        gSaveContext.nextCutsceneIndex = 0xFFF0;
                        play->transitionTrigger = TRANS_TRIGGER_START;
                        play->transitionType = TRANS_TYPE_FADE_WHITE;
                    }
                    break;

                case SCENE_SEA_BS:
                    SET_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE);
                    play->nextEntrance = ENTRANCE(ZORA_CAPE, 8);
                    gSaveContext.nextCutsceneIndex = 0xFFF0;
                    play->transitionTrigger = TRANS_TRIGGER_START;
                    play->transitionType = TRANS_TYPE_FADE_WHITE;
                    break;

                case SCENE_INISIE_BS:
                    SET_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE);
                    play->nextEntrance = ENTRANCE(IKANA_CANYON, 0);
                    gSaveContext.nextCutsceneIndex = 0xFFF1;
                    play->transitionTrigger = TRANS_TRIGGER_START;
                    play->transitionType = TRANS_TYPE_FADE_WHITE;
                    break;

                default:
                    break;
            }
        }
    }
}

// Chooses between a cutscene or a rotating mask depending on whether the player has the corresponding mask
void CutsceneCmd_ChooseCreditsScenes(PlayState* play, CutsceneContext* csCtx, CsCmdChooseCreditsScene* cmd) {
    if ((csCtx->curFrame >= cmd->startFrame) && (func_801A3950(SEQ_PLAYER_BGM_MAIN, true) != 0xFF)) {
        switch (cmd->type) {
            case CS_CREDITS_DESTINATION:
                CutsceneCmd_DestinationDefault(play, csCtx, (CsCmdDestination*)cmd);
                break;

            case CS_CREDITS_MASK_KAMARO:
                if (INV_CONTENT(ITEM_MASK_KAMARO) == ITEM_MASK_KAMARO) {
                    play->nextEntrance = ENTRANCE(MILK_BAR, 0);
                    gSaveContext.nextCutsceneIndex = 0xFFF0;
                } else {
                    play->nextEntrance = ENTRANCE(CUTSCENE, 0);
                    gSaveContext.nextCutsceneIndex = 0xFFF9;
                }
                play->transitionTrigger = TRANS_TRIGGER_START;
                break;

            case CS_CREDITS_MASK_GREAT_FAIRY:
                if (INV_CONTENT(ITEM_MASK_GREAT_FAIRY) == ITEM_MASK_GREAT_FAIRY) {
                    play->nextEntrance = ENTRANCE(FAIRY_FOUNTAIN, 0);
                    gSaveContext.nextCutsceneIndex = 0xFFF0;
                } else {
                    play->nextEntrance = ENTRANCE(CUTSCENE, 1);
                    gSaveContext.nextCutsceneIndex = 0xFFF9;
                }
                play->transitionTrigger = TRANS_TRIGGER_START;
                break;

            case CS_CREDITS_MASK_ROMANI:
                if (INV_CONTENT(ITEM_MASK_ROMANI) == ITEM_MASK_ROMANI) {
                    play->nextEntrance = ENTRANCE(ROMANI_RANCH, 0);
                    gSaveContext.nextCutsceneIndex = 0xFFF1;
                } else {
                    play->nextEntrance = ENTRANCE(CUTSCENE, 2);
                    gSaveContext.nextCutsceneIndex = 0xFFF9;
                }
                play->transitionTrigger = TRANS_TRIGGER_START;
                break;

            case CS_CREDITS_MASK_BLAST:
                if (INV_CONTENT(ITEM_MASK_BLAST) == ITEM_MASK_BLAST) {
                    play->nextEntrance = ENTRANCE(WEST_CLOCK_TOWN, 0);
                    gSaveContext.nextCutsceneIndex = 0xFFF0;
                } else {
                    play->nextEntrance = ENTRANCE(CUTSCENE, 3);
                    gSaveContext.nextCutsceneIndex = 0xFFF9;
                }
                play->transitionTrigger = TRANS_TRIGGER_START;
                break;

            case CS_CREDITS_MASK_CIRCUS_LEADER:
                if (INV_CONTENT(ITEM_MASK_CIRCUS_LEADER) == ITEM_MASK_CIRCUS_LEADER) {
                    play->nextEntrance = ENTRANCE(MILK_BAR, 0);
                    gSaveContext.nextCutsceneIndex = 0xFFF1;
                } else {
                    play->nextEntrance = ENTRANCE(CUTSCENE, 5);
                    gSaveContext.nextCutsceneIndex = 0xFFF9;
                }
                play->transitionTrigger = TRANS_TRIGGER_START;
                break;

            case CS_CREDITS_MASK_BREMEN:
                if (INV_CONTENT(ITEM_MASK_BREMEN) == ITEM_MASK_BREMEN) {
                    play->nextEntrance = ENTRANCE(MILK_BAR, 0);
                    gSaveContext.nextCutsceneIndex = 0xFFF3;
                } else {
                    play->nextEntrance = ENTRANCE(CUTSCENE, 6);
                    gSaveContext.nextCutsceneIndex = 0xFFF9;
                }
                play->transitionTrigger = TRANS_TRIGGER_START;
                break;

            case CS_CREDITS_IKANA:
                play->nextEntrance = ENTRANCE(IKANA_CANYON, 0);
                gSaveContext.nextCutsceneIndex = 0xFFF3;
                play->transitionTrigger = TRANS_TRIGGER_START;
                break;

            case CS_CREDITS_MASK_COUPLE:
                if (INV_CONTENT(ITEM_MASK_COUPLE) == ITEM_MASK_COUPLE) {
                    play->nextEntrance = ENTRANCE(TERMINA_FIELD, 0);
                    gSaveContext.nextCutsceneIndex = 0xFFF8;
                } else {
                    play->nextEntrance = ENTRANCE(CUTSCENE, 7);
                    gSaveContext.nextCutsceneIndex = 0xFFF9;
                }
                play->transitionTrigger = TRANS_TRIGGER_START;
                break;

            case CS_CREDITS_MASK_BUNNY:
                if (INV_CONTENT(ITEM_MASK_BUNNY) == ITEM_MASK_BUNNY) {
                    play->nextEntrance = ENTRANCE(CUCCO_SHACK, 0);
                    gSaveContext.nextCutsceneIndex = 0xFFF0;
                } else {
                    play->nextEntrance = ENTRANCE(CUTSCENE, 4);
                    gSaveContext.nextCutsceneIndex = 0xFFF9;
                }
                play->transitionTrigger = TRANS_TRIGGER_START;
                break;

            case CS_CREDITS_MASK_POSTMAN:
                if (INV_CONTENT(ITEM_MASK_POSTMAN) == ITEM_MASK_POSTMAN) {
                    play->nextEntrance = ENTRANCE(TERMINA_FIELD, 1);
                    gSaveContext.nextCutsceneIndex = 0xFFF8;
                } else {
                    play->nextEntrance = ENTRANCE(CUTSCENE, 8);
                    gSaveContext.nextCutsceneIndex = 0xFFF9;
                }
                play->transitionTrigger = TRANS_TRIGGER_START;
                break;

            default:
                break;
        }
    }
}

void CutsceneCmd_MotionBlur(PlayState* play, CutsceneContext* csCtx, CsCmdMotionBlur* cmd) {
    if ((csCtx->curFrame >= cmd->startFrame) && (cmd->endFrame >= csCtx->curFrame)) {
        if ((csCtx->curFrame == cmd->startFrame) && (cmd->type == CS_MOTION_BLUR_ENABLE)) {
            Play_EnableMotionBlur(180);
        }

        if (cmd->type == CS_MOTION_BLUR_DISABLE) {
            f32 lerp = Environment_LerpWeight(cmd->endFrame, cmd->startFrame, csCtx->curFrame);

            if (lerp >= 0.9f) {
                Play_DisableMotionBlur();
            } else {
                Play_SetMotionBlurAlpha((1.0f - lerp) * 180.0f);
            }
        }
    }
}

void CutsceneCmd_GiveTatlToPlayer(PlayState* play, CutsceneContext* csCtx, CsCmdGiveTatl* cmd) {
    Player* player = GET_PLAYER(play);

    if (csCtx->curFrame == cmd->startFrame) {
        if (cmd->giveTatl == true) {
            gSaveContext.save.hasTatl = true;
            if (player->tatlActor != NULL) {
                return;
            }
            player->tatlActor =
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ELF, player->actor.world.pos.x, player->actor.world.pos.y,
                            player->actor.world.pos.z, 0, 0, 0, FAIRY_PARAMS(FAIRY_TYPE_0, false, 0));
        }
    }
}

void CutsceneCmd_Transition(PlayState* play, CutsceneContext* csCtx, CsCmdTransition* cmd) {
    if ((csCtx->curFrame >= cmd->startFrame) && (cmd->endFrame >= csCtx->curFrame)) {
        f32 lerp;

        play->envCtx.fillScreen = true;
        lerp = Environment_LerpWeight(cmd->endFrame, cmd->startFrame, csCtx->curFrame);

        switch (cmd->type) {
            case CS_TRANS_GRAY_FILL_IN:
            case CS_TRANS_GRAY_FILL_OUT:
                play->envCtx.screenFillColor[0] = 160;
                play->envCtx.screenFillColor[1] = 160;
                play->envCtx.screenFillColor[2] = 160;

                if (cmd->type == CS_TRANS_GRAY_FILL_IN) {
                    play->envCtx.screenFillColor[3] = 255.0f * lerp;
                    if (lerp == 0.0f) {
                        Audio_PlaySfx_2(NA_SE_EV_S_STONE_FLASH);
                    }
                } else {
                    play->envCtx.screenFillColor[3] = (1.0f - lerp) * 255.0f;
                }
                break;

            case CS_TRANS_BLUE_FILL_IN:
            case CS_TRANS_BLUE_FILL_OUT:
                play->envCtx.screenFillColor[0] = 0;
                play->envCtx.screenFillColor[1] = 0;
                play->envCtx.screenFillColor[2] = 255;
                if (cmd->type == CS_TRANS_BLUE_FILL_IN) {
                    play->envCtx.screenFillColor[3] = 255.0f * lerp;
                } else {
                    play->envCtx.screenFillColor[3] = (1.0f - lerp) * 255.0f;
                }
                break;

            case CS_TRANS_RED_FILL_OUT:
            case CS_TRANS_RED_FILL_IN:
                play->envCtx.screenFillColor[0] = 255;
                play->envCtx.screenFillColor[1] = 0;
                play->envCtx.screenFillColor[2] = 0;
                if (cmd->type == CS_TRANS_RED_FILL_OUT) {
                    play->envCtx.screenFillColor[3] = (1.0f - lerp) * 255.0f;
                } else {
                    play->envCtx.screenFillColor[3] = 255.0f * lerp;
                }
                break;

            case CS_TRANS_GREEN_FILL_OUT:
            case CS_TRANS_GREEN_FILL_IN:
                play->envCtx.screenFillColor[0] = 0;
                play->envCtx.screenFillColor[1] = 255;
                play->envCtx.screenFillColor[2] = 0;
                if (cmd->type == CS_TRANS_GREEN_FILL_OUT) {
                    play->envCtx.screenFillColor[3] = (1.0f - lerp) * 255.0f;
                } else {
                    play->envCtx.screenFillColor[3] = 255.0f * lerp;
                }
                break;

            case CS_TRANS_TRIGGER_INSTANCE:
                gSaveContext.cutsceneTransitionControl = 1;
                break;

            case CS_TRANS_BLACK_FILL_OUT:
            case CS_TRANS_BLACK_FILL_IN:
                play->envCtx.screenFillColor[0] = 0;
                play->envCtx.screenFillColor[1] = 0;
                play->envCtx.screenFillColor[2] = 0;
                if (cmd->type == CS_TRANS_BLACK_FILL_OUT) {
                    play->envCtx.screenFillColor[3] = (1.0f - lerp) * 255.0f;
                } else {
                    play->envCtx.screenFillColor[3] = 255.0f * lerp;
                }
                break;

            case CS_TRANS_GRAY_TO_BLACK:
                play->envCtx.screenFillColor[0] = (1.0f - lerp) * 160.0f;
                play->envCtx.screenFillColor[1] = play->envCtx.screenFillColor[0];
                play->envCtx.screenFillColor[2] = play->envCtx.screenFillColor[0];
                play->envCtx.screenFillColor[3] = 255;
                break;

            case CS_TRANS_BLACK_TO_GRAY:
                play->envCtx.screenFillColor[0] = 160.0f * lerp;
                play->envCtx.screenFillColor[1] = play->envCtx.screenFillColor[0];
                play->envCtx.screenFillColor[2] = play->envCtx.screenFillColor[0];
                play->envCtx.screenFillColor[3] = 255;
                break;

            default:
                break;
        }
    }
}

s32 CutsceneCmd_UpdateCamSpline(PlayState* play, u8* script) {
    s32 cmdBytes = 0;

    memcpy(&cmdBytes, script, sizeof(cmdBytes));
    script += sizeof(cmdBytes);

    if (!Play_IsDebugCamEnabled()) {
        CutsceneCamera_UpdateSplines(script, &sCutsceneCameraInfo);
    }
    return cmdBytes + sizeof(cmdBytes);
}

/**
 * Counts how many masks the player has
 * The count doesn't include transformation masks
 */
s32 Cutscene_CountNormalMasks(void) {
    s32 count = 0;

    if (INV_CONTENT(ITEM_MASK_TRUTH) == ITEM_MASK_TRUTH) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_KAFEIS_MASK) == ITEM_MASK_KAFEIS_MASK) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_ALL_NIGHT) == ITEM_MASK_ALL_NIGHT) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_BUNNY) == ITEM_MASK_BUNNY) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_KEATON) == ITEM_MASK_KEATON) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_GARO) == ITEM_MASK_GARO) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_ROMANI) == ITEM_MASK_ROMANI) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_CIRCUS_LEADER) == ITEM_MASK_CIRCUS_LEADER) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_POSTMAN) == ITEM_MASK_POSTMAN) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_COUPLE) == ITEM_MASK_COUPLE) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_GREAT_FAIRY) == ITEM_MASK_GREAT_FAIRY) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_GIBDO) == ITEM_MASK_GIBDO) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_DON_GERO) == ITEM_MASK_DON_GERO) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_KAMARO) == ITEM_MASK_KAMARO) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_CAPTAIN) == ITEM_MASK_CAPTAIN) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_STONE) == ITEM_MASK_STONE) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_BREMEN) == ITEM_MASK_BREMEN) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_BLAST) == ITEM_MASK_BLAST) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_SCENTS) == ITEM_MASK_SCENTS) {
        count++;
    }
    if (INV_CONTENT(ITEM_MASK_GIANT) == ITEM_MASK_GIANT) {
        count++;
    }

    return count;
}

void CutsceneCmd_Text(PlayState* play, CutsceneContext* csCtx, CsCmdText* cmd) {
    static s32 sCutsceneTextboxType = CS_TEXT_TYPE_DEFAULT;
    u8 talkState;
    s32 pad;
    u16 endFrame;

    if ((csCtx->curFrame <= cmd->startFrame) || (csCtx->curFrame > cmd->endFrame)) {
        return;
    }

    if (cmd->type != CS_TEXT_OCARINA_ACTION) {
        if (sCurTextId != cmd->textId) {
            if (sCutsceneTextboxType == CS_TEXT_TYPE_3) {
                csCtx->curFrame--;
            }
            sCutsceneTextboxType = CS_TEXT_TYPE_1;
            sCurTextId = cmd->textId;
            if (cmd->type == CS_TEXT_TYPE_BOSSES_REMAINS) {
                if (GameInteractor_Should(VB_MEET_MOON_REQUIREMENTS, CHECK_QUEST_ITEM(QUEST_REMAINS_ODOLWA) &&
                                                                         CHECK_QUEST_ITEM(QUEST_REMAINS_GOHT) &&
                                                                         CHECK_QUEST_ITEM(QUEST_REMAINS_GYORG) &&
                                                                         CHECK_QUEST_ITEM(QUEST_REMAINS_TWINMOLD))) {
                    if (cmd->altTextId1 != 0xFFFF) {
                        Message_StartTextbox(play, cmd->altTextId1, NULL);
                    }
                } else {
                    Message_StartTextbox(play, cmd->textId, NULL);
                }
            } else if (cmd->type == CS_TEXT_TYPE_ALL_NORMAL_MASKS) {
                if (Cutscene_CountNormalMasks() == 20) {
                    if (cmd->altTextId1 != 0xFFFF) {
                        Message_StartTextbox(play, cmd->altTextId1, NULL);
                    }
                } else {
                    Message_StartTextbox(play, cmd->textId, NULL);
                }
            } else {
                Message_StartTextbox(play, cmd->textId, NULL);
            }
            //! FAKE: return;
            goto end;
        }
    } else {
        if (sCurOcarinaAction != cmd->textId) {
            sCutsceneTextboxType = CS_TEXT_OCARINA_ACTION;
            sCurOcarinaAction = cmd->textId;
            Message_DisplayOcarinaStaff(play, cmd->textId);
            return;
        }
    }

    if (csCtx->curFrame >= cmd->endFrame) {
        // The Textbox command can change the current cutscene frame, mainly to prevent advancing the cutscene when
        // a textbox that is expected to be closed by the user is still open.
        endFrame = csCtx->curFrame;
        talkState = Message_GetState(&play->msgCtx);
        if ((talkState != TEXT_STATE_CLOSING) && (talkState != TEXT_STATE_NONE) &&
            (talkState != TEXT_STATE_SONG_DEMO_DONE) && (talkState != TEXT_STATE_8)) {
            csCtx->curFrame--;

            if ((talkState == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
                if (play->msgCtx.choiceIndex == 0) {
                    if (cmd->textId == 0x33BD) {
                        // Gorman Track: do you understand?
                        Audio_PlaySfx_MessageCancel();
                    }

                    if (cmd->altTextId1 != 0xFFFF) {
                        Message_ContinueTextbox(play, cmd->altTextId1);
                        if (cmd->type == CS_TEXT_TYPE_3) {
                            sCutsceneTextboxType = CS_TEXT_TYPE_3;
                            if (cmd->altTextId2 != 0xFFFF) {
                                csCtx->curFrame++;
                            }
                        }
                    } else {
                        Message_CloseTextbox(play);
                        csCtx->curFrame++;
                    }
                } else {
                    if (cmd->textId == 0x33BD) {
                        // Gorman Track: do you understand?
                        Audio_PlaySfx_MessageDecide();
                    }

                    if (cmd->altTextId2 != 0xFFFF) {
                        Message_ContinueTextbox(play, cmd->altTextId2);
                        if (cmd->type == CS_TEXT_TYPE_3) {
                            sCutsceneTextboxType = CS_TEXT_TYPE_3;
                            if (cmd->altTextId1 != 0xFFFF) {
                                csCtx->curFrame++;
                            }
                        }
                    } else {
                        Message_CloseTextbox(play);
                        csCtx->curFrame++;
                    }
                }
            }

            if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_DisplayOcarinaStaff(play, cmd->textId);
            }
        }

        if ((talkState == TEXT_STATE_CLOSING) && (sCutsceneTextboxType == CS_TEXT_TYPE_3)) {
            csCtx->curFrame--;
            sCurTextId++;
        }

        if (endFrame == csCtx->curFrame) {
            Interface_SetHudVisibility(HUD_VISIBILITY_NONE);
            sCurTextId = 0;
            sCurOcarinaAction = 0;
            CutsceneCamera_Reset();
        } else {
            CutsceneCamera_SetState(CS_CAM_STATE_UPDATE_SPLINE);
        }
    }
end:;
}

void Cutscene_SetActorCue(CutsceneContext* csCtx, u8** script, s16 cueChannel) {
    s32 i;
    s32 numCues;

    memcpy(&numCues, *script, sizeof(numCues));
    *script += sizeof(numCues);

    for (i = 0; i < numCues; i++) {
        CsCmdActorCue* cue = (CsCmdActorCue*)(*script);

        if ((csCtx->curFrame >= cue->startFrame) && (csCtx->curFrame < cue->endFrame)) {
            csCtx->actorCues[cueChannel] = cue;
        }

        *script += sizeof(CsCmdActorCue);
    }
}

/**
 * Loops over the cutscene data itself (`script`), applying the effects of each command instantaneously (for most
 * commands).
 *
 * The cutscene data is an irregularly-structured array of words, which is made up of
 * - A beginning, which contains the number of command lists of this cutscene and the ending frame (see
 * `CS_BEGIN_CUTSCENE`).
 * - Any number of cutscene command lists (which should be the number specified in the beginning), each one of which
 * contains commands of the corresponding category.
 * - A end marker (see `CS_END`).
 *
 * This function iterates over each command list until either it has processed the number of command lists stated on
 * `CS_BEGIN_CUTSCENE` or until it finds a end marker.
 *
 * A command list starts with a pair of words containing the command category and a count of how many commands this list
 * has (N). This is followed by N commands of the said category. (The exception is Camera, which has the length of the
 * list in bytes instead of the number of commands).
 *
 * For most command lists categories (all but the actorCues and Camera commands):
 * - For each command list found, read the number of commands and loop over them, passing each one to the corresponding
 * function which handles the command, applying its effects and checking the frame range stored in the command.
 *
 * This function is invoked once per frame that a cutscene is active.
 *
 * TODO: consider changing the type of `script` to `uintptr_t` when this function matches.
 */
void Cutscene_ProcessScript(PlayState* play, CutsceneContext* csCtx, u8* script) {
    s32 i;
    s32 j;
    s32 pad;
    s32 totalEntries;
    u32 cmdType;
    s32 cmdEntries;
    s32 pad2;
    s32 csFrameCount;
    void* cmd;

    // Read the command list count and the ending frame for this cutscene
    memcpy(&totalEntries, script, sizeof(totalEntries));
    script += sizeof(totalEntries);

    memcpy(&csFrameCount, script, sizeof(csFrameCount));
    script += sizeof(csFrameCount);

    if ((csCtx->curFrame > (u16)csFrameCount) && (play->transitionTrigger != TRANS_TRIGGER_START) &&
        (csCtx->state != CS_STATE_RUN_UNSTOPPABLE)) {
        csCtx->state = CS_STATE_STOP;
        return;
    }

    // Loop over every command list
    for (i = 0; i < totalEntries; i++) {
        // Read the command type of the current command list.
        memcpy(&cmdType, script, sizeof(cmdType));
        script += sizeof(cmdType);

        if (cmdType == CS_CAM_STOP) {
            break;
        }

        if (((cmdType >= CS_CMD_ACTOR_CUE_100) && (cmdType <= CS_CMD_ACTOR_CUE_149)) ||
            (cmdType == CS_CMD_ACTOR_CUE_201) ||
            ((cmdType >= CS_CMD_ACTOR_CUE_450) && (cmdType <= CS_CMD_ACTOR_CUE_599))) {
            for (j = 0; j < ARRAY_COUNT(sCueTypeList); j = (s16)(j + 1)) {
                if (sCueTypeList[j] == (u16)cmdType) {
                    Cutscene_SetActorCue(csCtx, &script, j);
                    cmdType = CS_CMD_ACTOR_CUE_POST_PROCESS;
                    break;
                }

                if (sCueTypeList[j] == 0) {
                    sCueTypeList[j] = cmdType;
                    Cutscene_SetActorCue(csCtx, &script, j);
                    cmdType = CS_CMD_ACTOR_CUE_POST_PROCESS;
                    break;
                }
            }
        }

        switch (cmdType) {
            case CS_CMD_MISC:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    CutsceneCmd_Misc(play, csCtx, (CsCmdMisc*)script);
                    script += sizeof(CsCmdMisc);
                }
                break;

            case CS_CMD_LIGHT_SETTING:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    CutsceneCmd_SetLightSetting(play, csCtx, (CsCmdLightSetting*)script);
                    script += sizeof(CsCmdLightSetting);
                }
                break;

            case CS_CMD_START_SEQ:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    CutsceneCmd_StartSequence(play, csCtx, (CsCmdStartSeq*)script);
                    script += sizeof(CsCmdStartSeq);
                }
                break;

            case CS_CMD_STOP_SEQ:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    CutsceneCmd_StopSequence(play, csCtx, (CsCmdStopSeq*)script);
                    script += sizeof(CsCmdStopSeq);
                }
                break;

            case CS_CMD_FADE_OUT_SEQ:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    CutsceneCmd_FadeOutSequence(play, csCtx, (CsCmdFadeOutSeq*)script);
                    script += sizeof(CsCmdFadeOutSeq);
                }
                break;

            case CS_CMD_START_AMBIENCE:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    CutsceneCmd_StartAmbience(play, csCtx, (CsCmdStartAmbience*)script);
                    script += sizeof(CsCmdStartAmbience);
                }
                break;

            case CS_CMD_FADE_OUT_AMBIENCE:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    CutsceneCmd_FadeOutAmbience(play, csCtx, (CsCmdFadeOutAmbience*)script);
                    script += sizeof(CsCmdFadeOutAmbience);
                }
                break;

            case CS_CMD_SFX_REVERB_INDEX_2:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    Cutscene_SetSfxReverbIndexTo2(play, csCtx, (CsCmdSfxReverbIndexTo2*)script);
                    script += sizeof(CsCmdSfxReverbIndexTo2);
                }
                break;

            case CS_CMD_SFX_REVERB_INDEX_1:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    Cutscene_SetSfxReverbIndexTo1(play, csCtx, (CsCmdSfxReverbIndexTo1*)script);
                    script += sizeof(CsCmdSfxReverbIndexTo1);
                }
                break;

            case CS_CMD_MODIFY_SEQ:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    CutsceneCmd_ModifySequence(play, csCtx, (CsCmdModifySeq*)script);
                    script += sizeof(CsCmdModifySeq);
                }
                break;

            case CS_CMD_RUMBLE:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    CutsceneCmd_RumbleController(play, csCtx, (CsCmdRumble*)script);
                    script += sizeof(CsCmdRumble);
                }
                break;

            case CS_CMD_TRANSITION_GENERAL:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    CutsceneCmd_TransitionGeneral(play, csCtx, (CsCmdTransitionGeneral*)script);
                    script += sizeof(CsCmdTransitionGeneral);
                }
                break;

            case CS_CMD_TIME:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    CutsceneCmd_SetTime(play, csCtx, (CsCmdTime*)script);
                    script += sizeof(CsCmdTime);
                }
                break;

            case CS_CMD_PLAYER_CUE:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    cmd = script;

                    if ((((CsCmdActorCue*)cmd)->startFrame <= csCtx->curFrame) &&
                        (csCtx->curFrame < ((CsCmdActorCue*)cmd)->endFrame)) {
                        csCtx->playerCue = (CsCmdActorCue*)cmd;
                    }
                    script += sizeof(CsCmdActorCue);
                }
                break;

            case CS_CMD_CAMERA_SPLINE:
                script += CutsceneCmd_UpdateCamSpline(play, script);
                break;

            case CS_CMD_DESTINATION:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    CutsceneCmd_Destination(play, csCtx, (CsCmdDestination*)script);
                    script += sizeof(CsCmdDestination);
                }
                break;

            case CS_CMD_CHOOSE_CREDITS_SCENES:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    CutsceneCmd_ChooseCreditsScenes(play, csCtx, (CsCmdChooseCreditsScene*)script);
                    script += sizeof(CsCmdChooseCreditsScene);
                }
                break;

            case CS_CMD_TEXT:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    cmd = script;

                    if (((CsCmdText*)cmd)->textId != 0xFFFF) {
                        CutsceneCmd_Text(play, csCtx, (CsCmdText*)cmd);
                    }
                    script += sizeof(CsCmdText);
                }
                break;

            case CS_CMD_TRANSITION:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    CutsceneCmd_Transition(play, csCtx, (CsCmdTransition*)script);
                    script += sizeof(CsCmdTransition);
                }
                break;

            case CS_CMD_MOTION_BLUR:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    CutsceneCmd_MotionBlur(play, csCtx, (CsCmdMotionBlur*)script);
                    script += sizeof(CsCmdMotionBlur);
                }
                break;

            case CS_CMD_GIVE_TATL:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    CutsceneCmd_GiveTatlToPlayer(play, csCtx, (CsCmdGiveTatl*)script);
                    script += sizeof(CsCmdGiveTatl);
                }
                break;

            case CS_CMD_ACTOR_CUE_POST_PROCESS:
                break;

            default:
                memcpy(&cmdEntries, script, sizeof(cmdEntries));
                script += sizeof(cmdEntries);

                for (j = 0; j < cmdEntries; j++) {
                    script += sizeof(CsCmdUnimplemented);
                }
                break;
        }
    }
}

/* End of command handling section */

void CutsceneHandler_RunScript(PlayState* play, CutsceneContext* csCtx) {
    if (gSaveContext.save.cutsceneIndex >= 0xFFF0) {
        csCtx->curFrame++;
        Cutscene_ProcessScript(play, csCtx, (u8*)play->csCtx.script);
    }
}

void CutsceneHandler_StopManual(PlayState* play, CutsceneContext* csCtx) {
    if (Cutscene_StepTimer(play, csCtx, 0.0f)) {
        Audio_SetCutsceneFlag(false);
        csCtx->state = CS_STATE_IDLE;
    }
}

void CutsceneHandler_StopScript(PlayState* play, CutsceneContext* csCtx) {
    if (Cutscene_StepTimer(play, csCtx, 0.0f)) {
        s16 i;

        csCtx->playerCue = NULL;

        for (i = 0; i < ARRAY_COUNT(csCtx->actorCues); i++) {
            csCtx->actorCues[i] = NULL;
        }

        gSaveContext.save.cutsceneIndex = 0;
        gSaveContext.gameMode = GAMEMODE_NORMAL;

        CutsceneManager_Stop(CS_ID_GLOBAL_END);
        Audio_SetCutsceneFlag(false);
        csCtx->state = CS_STATE_IDLE;
    }
}

void Cutscene_SetupScripted(PlayState* play, CutsceneContext* csCtx) {
    if ((gSaveContext.cutsceneTrigger != 0) && (csCtx->state == CS_STATE_IDLE) && !Player_InCsMode(play)) {
        gSaveContext.save.cutsceneIndex = 0xFFFD;
    }

    if ((gSaveContext.save.cutsceneIndex >= 0xFFF0) && (csCtx->state == CS_STATE_IDLE)) {
        s16 i;

        sCurTextId = 0;
        sCurOcarinaAction = 0;
        csCtx->playerCue = NULL;

        for (i = 0; i < ARRAY_COUNT(csCtx->actorCues); i++) {
            csCtx->actorCues[i] = NULL;
        }

        csCtx->state++; // CS_STATE_START

        if (csCtx->state == CS_STATE_START) {
            Audio_SetCutsceneFlag(true);

            csCtx->curFrame = 0xFFFF;

            csCtx->subCamId = CutsceneManager_GetCurrentSubCamId(CS_ID_GLOBAL_END);
            CutsceneCamera_Init(Play_GetCamera(play, csCtx->subCamId), &sCutsceneCameraInfo);

            // OoT Remnant
            csCtx->camEyeSplinePointsAppliedFrame = CS_CAM_DATA_NOT_APPLIED;

            if (gSaveContext.cutsceneTrigger == 0) {
                Interface_SetHudVisibility(HUD_VISIBILITY_NONE);
                ShrinkWindow_Letterbox_SetSizeTarget(32);
                ShrinkWindow_Letterbox_SetSize(32);
                csCtx->state++; // CS_STATE_RUN
            }

            CutsceneHandler_RunScript(play, csCtx);
        }

        gSaveContext.cutsceneTrigger = 0;
    }
}

void Cutscene_HandleEntranceTriggers(PlayState* play) {
    s32 pad;
    s16 csId;
    SceneTableEntry* scene;
    s32 scriptIndex;

    if (((gSaveContext.gameMode == GAMEMODE_NORMAL) || (gSaveContext.gameMode == GAMEMODE_TITLE_SCREEN)) &&
        (gSaveContext.respawnFlag <= 0)) {
        // Try to find an actor cutscene that's triggered by the current spawn
        csId = CutsceneManager_FindEntranceCsId();
        if (csId != CS_ID_NONE) {
            scriptIndex = CutsceneManager_GetCutsceneScriptIndex(csId);
            if (scriptIndex != CS_SCRIPT_ID_NONE) {
                // A scripted cutscene is triggered by a spawn
                if ((play->csCtx.scriptList[scriptIndex].spawnFlags != CS_SPAWN_FLAG_NONE) &&
                    (gSaveContext.respawnFlag == 0)) {
                    if (play->csCtx.scriptList[scriptIndex].spawnFlags == CS_SPAWN_FLAG_ALWAYS) {
                        // Entrance cutscenes that always run
                        CutsceneManager_Start(csId, NULL);
                        gSaveContext.showTitleCard = false;

                    } else if (!CHECK_CS_SPAWN_FLAG_WEEKEVENTREG(play->csCtx.scriptList[scriptIndex].spawnFlags)) {
                        // Entrance cutscenes that only run once
                        SET_CS_SPAWN_FLAG_WEEKEVENTREG(play->csCtx.scriptList[scriptIndex].spawnFlags);
                        if (GameInteractor_Should(VB_PLAY_ENTRANCE_CS, true)) {
                            CutsceneManager_Start(csId, NULL);
                        }
                        // The title card will be used by the cs misc command if necessary.
                        gSaveContext.showTitleCard = false;
                    }
                }
            } else {
                // A non-scripted cutscene is triggered by a spawn
                CutsceneManager_StartWithPlayerCs(csId, NULL);
            }
        }
    }

    if ((gSaveContext.respawnFlag == 0) || (gSaveContext.respawnFlag == -2)) {
        scene = play->loadedScene;
        if ((scene->titleTextId != 0) && GameInteractor_Should(VB_SHOW_TITLE_CARD, gSaveContext.showTitleCard)) {
            if ((Entrance_GetTransitionFlags(((void)0, gSaveContext.save.entrance) +
                                             ((void)0, gSaveContext.sceneLayer)) &
                 0x4000) != 0) {
                Message_DisplaySceneTitleCard(play, scene->titleTextId);
            }
        }

        gSaveContext.showTitleCard = true;
    }
}

// 2S2H [Enhancements] This is equivalent to Cutscene_HandleConditionalTriggers in OoT. It's unused normally, but we
// use it for VB. In VB changing *should to false has no effect, but you instead modify the entrance & cutsceneIndex to
// override the behavior.
void func_800EDDB0(PlayState* play) {
    LUSLOG_INFO("Cutscene_HandleConditionalTriggers: entrance: %d, cutsceneIndex: 0x%X", gSaveContext.save.entrance,
                gSaveContext.save.cutsceneIndex);

    if (!GameInteractor_Should(VB_PLAY_TRANSITION_CS, true)) {
        return;
    }
}

void func_800EDDBC(UNK_TYPE arg0, UNK_TYPE arg1) {
}

void Cutscene_StartScripted(PlayState* play, u8 scriptIndex) {
    if (!R_USE_DEBUG_CUTSCENE) {
        play->csCtx.scriptIndex = scriptIndex;
        play->csCtx.script = Lib_SegmentedToVirtual(play->csCtx.scriptList[scriptIndex].script);
    }

    gSaveContext.cutsceneTrigger = 1;
}

/* Start of actor utilities section */

/**
 * Interpolates the actor's position based on the corresponding actor action's position
 * and the current cutscene frame
 */
void Cutscene_ActorTranslate(Actor* actor, PlayState* play, s32 cueChannel) {
    Vec3f startPos;
    Vec3f endPos;
    CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];
    f32 lerp;

    startPos.x = cue->startPos.x;
    startPos.y = cue->startPos.y;
    startPos.z = cue->startPos.z;
    endPos.x = cue->endPos.x;
    endPos.y = cue->endPos.y;
    endPos.z = cue->endPos.z;

    lerp = Environment_LerpWeight(cue->endFrame, cue->startFrame, play->csCtx.curFrame);

    VEC3F_LERPIMPDST(&actor->world.pos, &startPos, &endPos, lerp);
}

/**
 * Interpolates the actor's position based on the corresponding actor action's position
 * and the current cutscene frame, and sets the actor's yaw using the actor action yaw
 */
void Cutscene_ActorTranslateAndYaw(Actor* actor, PlayState* play, s32 cueChannel) {
    Cutscene_ActorTranslate(actor, play, cueChannel);

    actor->world.rot.y = play->csCtx.actorCues[cueChannel]->rot.y;
    actor->shape.rot.y = actor->world.rot.y;
}

/**
 * Interpolates the actor's position and yaw based on the corresponding actor action's
 * position and the current cutscene frame
 */
void Cutscene_ActorTranslateAndYawSmooth(Actor* actor, PlayState* play, s32 cueChannel) {
    Vec3f startPos;
    Vec3f endPos;
    CsCmdActorCue* cue;
    f32 lerp;

    startPos.x = play->csCtx.actorCues[cueChannel]->startPos.x;
    startPos.y = play->csCtx.actorCues[cueChannel]->startPos.y;
    startPos.z = play->csCtx.actorCues[cueChannel]->startPos.z;
    endPos.x = play->csCtx.actorCues[cueChannel]->endPos.x;
    endPos.y = play->csCtx.actorCues[cueChannel]->endPos.y;
    endPos.z = play->csCtx.actorCues[cueChannel]->endPos.z;

    cue = play->csCtx.actorCues[cueChannel];
    lerp = Environment_LerpWeight(cue->endFrame, cue->startFrame, play->csCtx.curFrame);

    VEC3F_LERPIMPDST(&actor->world.pos, &startPos, &endPos, lerp);

    Math_SmoothStepToS(&actor->world.rot.y, Math_Vec3f_Yaw(&startPos, &endPos), 10, 0x3E8, 1);
    actor->shape.rot.y = actor->world.rot.y;
}

/**
 * Interpolates the actor's XZ position and yaw based on the corresponding actor action's
 * position and the current cutscene frame
 */
void Cutscene_ActorTranslateXZAndYawSmooth(Actor* actor, PlayState* play, s32 cueChannel) {
    Vec3f startPos;
    Vec3f endPos;
    CsCmdActorCue* cue;
    f32 lerp;

    startPos.x = play->csCtx.actorCues[cueChannel]->startPos.x;
    startPos.z = play->csCtx.actorCues[cueChannel]->startPos.z;
    endPos.x = play->csCtx.actorCues[cueChannel]->endPos.x;
    endPos.z = play->csCtx.actorCues[cueChannel]->endPos.z;

    cue = play->csCtx.actorCues[cueChannel];
    lerp = Environment_LerpWeight(cue->endFrame, cue->startFrame, play->csCtx.curFrame);

    actor->world.pos.x = startPos.x + (endPos.x - startPos.x) * lerp;
    actor->world.pos.z = startPos.z + (endPos.z - startPos.z) * lerp;

    Math_SmoothStepToS(&actor->world.rot.y, Math_Vec3f_Yaw(&startPos, &endPos), 10, 0x3E8, 1);
    actor->shape.rot.y = actor->world.rot.y;
}

s32 Cutscene_GetSceneLayer(PlayState* play) {
    s32 sceneLayer = 0;

    if (gSaveContext.sceneLayer > 0) {
        sceneLayer = gSaveContext.sceneLayer;
    }
    return sceneLayer;
}

/**
 * @param play See `PlayState`
 * @param cueType See `cmdType`
 * @return cue channel
 */
s32 Cutscene_GetCueChannel(PlayState* play, u16 cueType) {
    s32 i;
    s32 cueChannel = -1;

    for (i = 0; i < ARRAY_COUNT(sCueTypeList); i++) {
        if (cueType == sCueTypeList[i]) {
            cueChannel = i;
        }
    }

    return cueChannel;
}

/**
 * @param play See `PlayState`
 * @param cueType See `cmdType`
 * @return is cuetype in a channel
 */
s32 Cutscene_IsCueInChannel(PlayState* play, u16 cueType) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        s32 cueChannel = Cutscene_GetCueChannel(play, cueType);

        if (cueChannel != -1) {
            return play->csCtx.actorCues[cueChannel] != NULL;
        }
    }

    return false;
}

u8 Cutscene_IsPlaying(PlayState* play) {
    return (gSaveContext.cutsceneTrigger != 0) || (play->csCtx.state != CS_STATE_IDLE);
}

/* End of actor utilities section */
