#include "global.h"
#include "z64camera.h"

#include <string.h>

#include "scenes/indoors/tokinoma/tokinoma_scene.h"
#include "scenes/overworld/spot00/spot00_scene.h"
#include "scenes/overworld/spot01/spot01_scene.h"
#include "scenes/overworld/spot02/spot02_scene.h"
#include "scenes/overworld/spot04/spot04_scene.h"
#include "scenes/overworld/spot05/spot05_scene.h"
#include "scenes/overworld/spot06/spot06_scene.h"
#include "scenes/overworld/spot07/spot07_scene.h"
#include "scenes/overworld/spot08/spot08_scene.h"
#include "scenes/overworld/spot09/spot09_scene.h"
#include "scenes/overworld/spot11/spot11_scene.h"
#include "scenes/overworld/spot12/spot12_scene.h"
#include "scenes/overworld/spot15/spot15_scene.h"
#include "scenes/overworld/spot16/spot16_scene.h"
#include "scenes/overworld/spot17/spot17_scene.h"
#include "scenes/overworld/spot18/spot18_scene.h"
#include "scenes/overworld/spot20/spot20_scene.h"

#include "scenes/dungeons/bdan/bdan_scene.h"
#include "scenes/dungeons/ddan/ddan_scene.h"
#include "scenes/dungeons/ydan/ydan_scene.h"
#include "scenes/dungeons/ganontika/ganontika_scene.h"
#include "scenes/dungeons/ganon_tou/ganon_tou_scene.h"
#include "scenes/dungeons/jyasinboss/jyasinboss_scene.h"
#include "scenes/dungeons/ice_doukutu/ice_doukutu_scene.h"

#include "scenes/misc/hakaana_ouke/hakaana_ouke_scene.h"

#include "soh/Enhancements/randomizer/randomizer_entrance.h"

u16 D_8011E1C0 = 0;
u16 D_8011E1C4 = 0;

typedef void (*CutsceneStateHandler)(PlayState*, CutsceneContext*);

void func_80064720(PlayState* play, CutsceneContext* csCtx);
void func_80064760(PlayState* play, CutsceneContext* csCtx);
void func_800647C0(PlayState* play, CutsceneContext* csCtx);
void func_80068C3C(PlayState* play, CutsceneContext* csCtx);
void func_80068D84(PlayState* play, CutsceneContext* csCtx);
void func_80068DC0(PlayState* play, CutsceneContext* csCtx);

CutsceneStateHandler sCsStateHandlers1[] = {
    func_80064720, func_80064760, func_80064720, func_80068D84, func_80064720,
};

CutsceneStateHandler sCsStateHandlers2[] = {
    func_80064720, func_800647C0, func_80068C3C, func_80068DC0, func_80068C3C,
};

u8 sTitleCsState = 0;

EntranceCutscene sEntranceCutsceneTable[] = {
    { 0x0185, 2, 0xA0, gHyruleFieldIntroCs },
    { 0x013D, 2, 0xA1, gDMTIntroCs },
    { 0x00DB, 2, 0xA3, gKakarikoVillageIntroCs },
    { 0x0108, 2, 0xA4, gZorasDomainIntroCs },
    { 0x0138, 1, 0xA5, gHyruleCastleIntroCs },
    { 0x014D, 2, 0xA6, gGoronCityIntroCs },
    { 0x0053, 2, 0xA7, gTempleOfTimeIntroCs },
    { 0x0000, 2, 0xA8, gDekuTreeIntroCs },
    { 0x028A, 0, 0x18, gHyruleFieldSouthEponaJumpCs },
    { 0x0292, 0, 0x18, gHyruleFieldEastEponaJumpCs },
    { 0x028E, 0, 0x18, gHyruleFieldWestEponaJumpCs },
    { 0x0476, 0, 0x18, gHyruleFieldGateEponaJumpCs },
    { 0x050F, 1, 0xA9, gHyruleFieldGetOoTCs },
    { 0x0102, 2, 0xB1, gLakeHyliaIntroCs },
    { 0x0117, 2, 0xB2, gGerudoValleyIntroCs },
    { 0x0129, 2, 0xB3, gGerudoFortressIntroCs },
    { 0x0157, 2, 0xB4, gLonLonRanchIntroCs },
    { 0x0028, 2, 0xB5, gJabuJabuIntroCs },
    { 0x00E4, 2, 0xB6, gGraveyardIntroCs },
    { 0x0225, 2, 0xB7, gZorasFountainIntroCs },
    { 0x0123, 2, 0xB8, gDesertColossusIntroCs },
    { 0x0147, 2, 0xB9, gDeathMountainCraterIntroCs },
    { 0x0138, 0, 0xBA, gGanonsCastleIntroCs },
    { 0x0574, 2, 0x5A, gSunSongGraveSunSongTeachPart2Cs },
    { 0x0538, 2, 0xBB, gForestBarrierCs },
    { 0x053C, 2, 0xBC, gWaterBarrierCs },
    { 0x0540, 2, 0xBD, gShadowBarrierCs },
    { 0x0544, 2, 0xBE, gFireBarrierCs },
    { 0x0548, 2, 0xBF, gLightBarrierCs },
    { 0x054C, 2, 0xAD, gSpiritBarrierCs },
    { 0x008D, 0, 0xC0, gSpiritBossNabooruKnuckleIntroCs },
    { 0x03B4, 0, 0xC7, gGerudoFortressFirstCaptureCs },
    { 0x0246, 2, 0xB9, gDeathMountainCraterIntroCs },
    { 0x05E8, 2, 0xC6, gKokiriForestDekuSproutCs },
};

// Unused, seems to be an early list of dungeon entrance cutscene locations
void* D_8011E304[] = {
    gDekuTreeIntroCs, gJabuJabuIntroCs, gDcOpeningCs, gMinuetCs, gIceCavernSerenadeCs, gTowerBarrierCs,
};

u16 D_8015FCC0;
u16 D_8015FCC2;
u16 D_8015FCC4;
s16 D_8015FCC6;
u8 D_8015FCC8;
s16 sQuakeIndex;
u16 D_8015FCCC;      // only written to, never read
char D_8015FCD0[20]; // unreferenced
u8 D_8015FCE4;       // only written to, never read

void func_80068ECC(PlayState* play, CutsceneContext* csCtx);

void Cutscene_DrawDebugInfo(PlayState* play, Gfx** dlist, CutsceneContext* csCtx) {
    GfxPrint printer;
    s32 pad[2];

    GfxPrint_Init(&printer);
    GfxPrint_Open(&printer, *dlist);

    GfxPrint_SetPos(&printer, 22, 25);
    GfxPrint_SetColor(&printer, 255, 255, 55, 32);
    GfxPrint_Printf(&printer, "%s", "FLAME ");
    GfxPrint_SetColor(&printer, 255, 255, 255, 32);
    GfxPrint_Printf(&printer, "%06d", csCtx->frames);
    GfxPrint_SetColor(&printer, 50, 255, 255, 60);
    GfxPrint_SetPos(&printer, 4, 26);
    GfxPrint_Printf(&printer, "%s", "SKIP=(START) or (Cursole Right)");

    *dlist = GfxPrint_Close(&printer);
    GfxPrint_Destroy(&printer);
}

void func_8006450C(PlayState* play, CutsceneContext* csCtx) {
    csCtx->state = CS_STATE_IDLE;
    csCtx->unk_0C = 0.0f;
}

void func_80064520(PlayState* play, CutsceneContext* csCtx) {
    csCtx->state = CS_STATE_SKIPPABLE_INIT;
    csCtx->linkAction = NULL;
}

void func_80064534(PlayState* play, CutsceneContext* csCtx) {
    if (csCtx->state != CS_STATE_UNSKIPPABLE_EXEC) {
        csCtx->state = CS_STATE_UNSKIPPABLE_INIT;
    }
}

void func_80064558(PlayState* play, CutsceneContext* csCtx) {
    if (gSaveContext.cutsceneIndex < 0xFFF0) {
        sCsStateHandlers1[csCtx->state](play, csCtx);
    }
}

void func_800645A0(PlayState* play, CutsceneContext* csCtx) {
    Input* input = &play->state.input[0];

    if (CHECK_BTN_ALL(input->press.button, BTN_DLEFT) && (csCtx->state == CS_STATE_IDLE) &&
        (gSaveContext.sceneSetupIndex >= 4)) {
        D_8015FCC8 = 0;
        gSaveContext.cutsceneIndex = 0xFFFD;
        gSaveContext.cutsceneTrigger = 1;
    }

    if (CHECK_BTN_ALL(input->press.button, BTN_DUP) && (csCtx->state == CS_STATE_IDLE) &&
        (gSaveContext.sceneSetupIndex >= 4) && !gDbgCamEnabled) {
        D_8015FCC8 = 1;
        gSaveContext.cutsceneIndex = 0xFFFD;
        gSaveContext.cutsceneTrigger = 1;
    }

    if ((gSaveContext.cutsceneTrigger != 0) && (play->sceneLoadFlag == 0x14)) {
        gSaveContext.cutsceneTrigger = 0;
    }

    if ((gSaveContext.cutsceneTrigger != 0) && (csCtx->state == CS_STATE_IDLE)) {
        osSyncPrintf("\nデモ開始要求 発令！"); // "Cutscene start request announcement!"
        gSaveContext.cutsceneIndex = 0xFFFD;
        gSaveContext.cutsceneTrigger = 1;
    }

    if (gSaveContext.cutsceneIndex >= 0xFFF0) {
        func_80068ECC(play, csCtx);
        sCsStateHandlers2[csCtx->state](play, csCtx);
    }
}

void func_80064720(PlayState* play, CutsceneContext* csCtx) {
}

u32 func_8006472C(PlayState* play, CutsceneContext* csCtx, f32 target) {
    return Math_StepToF(&csCtx->unk_0C, target, 0.1f);
}

void func_80064760(PlayState* play, CutsceneContext* csCtx) {
    Interface_ChangeAlpha(1);
    ShrinkWindow_SetVal(0x20);

    if (func_8006472C(play, csCtx, 1.0f)) {
        Audio_SetCutsceneFlag(1);
        csCtx->state++;
    }
}

void func_800647C0(PlayState* play, CutsceneContext* csCtx) {
    func_80068C3C(play, csCtx);
    Interface_ChangeAlpha(1);
    ShrinkWindow_SetVal(0x20);

    if (func_8006472C(play, csCtx, 1.0f)) {
        Audio_SetCutsceneFlag(1);
        csCtx->state++;
    }
}

// Command 3: Misc. Actions
void func_80064824(PlayState* play, CutsceneContext* csCtx, CsCmdBase* cmd) {
    Player* player = GET_PLAYER(play);
    f32 temp;
    u8 sp3F = 0;

    if ((csCtx->frames < cmd->startFrame) || ((csCtx->frames >= cmd->endFrame) && (cmd->endFrame != cmd->startFrame))) {
        return;
    }

    temp = Environment_LerpWeight(cmd->endFrame - 1, cmd->startFrame, csCtx->frames);

    if (csCtx->frames == cmd->startFrame) {
        sp3F = 1;
    }

    switch (cmd->base) {
        case 1:
            if (sp3F != 0) {
                Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_RAIN, CHANNEL_IO_PORT_4, 0x3F);
                Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_RAIN, CHANNEL_IO_PORT_1, 1);
                play->envCtx.unk_EE[0] = 20;
            }
            break;
        case 2:
            if (sp3F != 0) {
                Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_LIGHTNING, CHANNEL_IO_PORT_0, 0);
                Environment_AddLightningBolts(play, 3);
                gLightningStrike.state = LIGHTNING_STRIKE_START;
            }
            break;
        case 3:
            if (sp3F != 0) {
                Flags_SetEnv(play, 0);
                if (gSaveContext.entranceIndex == 0x0053 || (gSaveContext.n64ddFlag && gSaveContext.entranceIndex == 0x05F4)) {
                    Flags_SetEnv(play, 2);
                }
            }
            break;
        case 6:
            if (play->envCtx.adjFogFar < 12800) {
                play->envCtx.adjFogFar += 35;
            }
            break;
        case 7:
            if (sp3F != 0) {
                play->envCtx.unk_19 = 1;
                play->envCtx.unk_17 = 1;
                play->envCtx.unk_18 = 0;
                play->envCtx.unk_1A = 0x3C;
                play->envCtx.unk_21 = 1;
                play->envCtx.unk_1F = 0;
                play->envCtx.unk_20 = 1;
                play->envCtx.unk_22 = play->envCtx.unk_24 = 0x3C;
            }
            break;
        case 8:
            if (play->roomCtx.unk_74[0] < 0x80) {
                play->roomCtx.unk_74[0] += 4;
            }
            break;
        case 9:
            play->envCtx.unk_EE[3] = 16;
            break;
        case 10:
            Flags_SetEnv(play, 1);
            break;
        case 11:
            if (play->roomCtx.unk_74[0] < 0x672) {
                play->roomCtx.unk_74[0] += 0x14;
            }
            if (csCtx->frames == 0x30F) {
                func_80078884(NA_SE_EV_DEKU_DEATH);
            } else if (csCtx->frames == 0x2CD) {
                play->roomCtx.unk_74[0] = 0;
            }
            break;
        case 12:
            if (sp3F != 0) {
                if (csCtx->state != CS_STATE_UNSKIPPABLE_EXEC) {
                    csCtx->state = CS_STATE_UNSKIPPABLE_INIT;
                }
            }
            break;
        case 13:
            if (play->roomCtx.unk_74[1] == 0) {
                func_80078884(NA_SE_EV_TRIFORCE_FLASH);
            }
            if (play->roomCtx.unk_74[1] < 0xFF) {
                play->roomCtx.unk_74[1] += 5;
            }
            break;
        case 14:
            if (sp3F != 0) {
                func_800BC490(play, 1);
            }
            break;
        case 15:
            if (sp3F != 0) {
                TitleCard_InitPlaceName(play, &play->actorCtx.titleCtx, player->giObjectSegment, 160, 120,
                                        144, 24, 20);
            }
            break;
        case 16:
            if (sp3F != 0) {
                sQuakeIndex = Quake_Add(GET_ACTIVE_CAM(play), 6);
                Quake_SetSpeed(sQuakeIndex, 0x7FFF);
                Quake_SetQuakeValues(sQuakeIndex, 4, 0, 1000, 0);
                Quake_SetCountdown(sQuakeIndex, 800);
            }
            break;
        case 17:
            if (sp3F != 0) {
                Quake_RemoveFromIdx(sQuakeIndex);
            }
            break;
        case 18:
            play->envCtx.unk_EE[0] = 0;
            play->envCtx.gloomySkyMode = 2;
            if (gSaveContext.dayTime < 0x4AAB) {
                gSaveContext.dayTime += 30;
            }
            if (play->envCtx.unk_EE[1] == 0) {
                gWeatherMode = 0;
                Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_RAIN, CHANNEL_IO_PORT_1, 0);
            }
            break;
        case 19:
            gSaveContext.eventChkInf[6] |= 0x0020;
            break;
        case 20:
            gSaveContext.eventChkInf[6] |= 0x0080;
            break;
        case 21:
            gSaveContext.eventChkInf[6] |= 0x0200;
            break;
        case 22:
            D_801614B0.r = 255;
            D_801614B0.g = 255;
            D_801614B0.b = 255;
            D_801614B0.a = 255;
            break;
        case 23:
            D_801614B0.r = 255;
            D_801614B0.g = 180;
            D_801614B0.b = 100;
            D_801614B0.a = 255.0f * temp;
            break;
        case 24:
            play->roomCtx.curRoom.segment = NULL;
            break;
        case 25:
            gSaveContext.dayTime += 30;
            if ((gSaveContext.dayTime) > 0xCAAA) {
                gSaveContext.dayTime = 0xCAAA;
            }
            break;
        case 26:
            if ((gSaveContext.dayTime < 0x3000) || (gSaveContext.dayTime >= 0x4555)) {
                if ((gSaveContext.dayTime >= 0x4555) && (gSaveContext.dayTime < 0xAAAB)) {
                    play->envCtx.unk_BF = 1;
                } else if ((gSaveContext.dayTime >= 0xAAAB) && (gSaveContext.dayTime < 0xC556)) {
                    play->envCtx.unk_BF = 2;
                } else {
                    play->envCtx.unk_BF = 3;
                }
            }
            break;
        case 27:
            if (play->state.frames & 8) {
                if (play->envCtx.adjAmbientColor[0] < 40) {
                    play->envCtx.adjAmbientColor[0] += 2;
                    play->envCtx.adjLight1Color[1] -= 3;
                    play->envCtx.adjLight1Color[2] -= 3;
                }
            } else {
                if (play->envCtx.adjAmbientColor[0] > 2) {
                    play->envCtx.adjAmbientColor[0] -= 2;
                    play->envCtx.adjLight1Color[1] += 3;
                    play->envCtx.adjLight1Color[2] += 3;
                }
            }
            break;
        case 28:
            play->unk_11DE9 = 1;
            break;
        case 29:
            play->unk_11DE9 = 0;
            break;
        case 30:
            Flags_SetEnv(play, 3);
            break;
        case 31:
            Flags_SetEnv(play, 4);
            break;
        case 32:
            if (sp3F != 0) {
                play->envCtx.sandstormState = 1;
            }
            func_800788CC(NA_SE_EV_SAND_STORM - SFX_FLAG);
            break;
        case 33:
            gSaveContext.sunsSongState = SUNSSONG_START;
            break;
        case 34:
            if (IS_DAY) {
                gSaveContext.dayTime -= gTimeIncrement;
            } else {
                gSaveContext.dayTime -= gTimeIncrement * 2;
            }
            break;
        case 35:
            func_800EE824();
            csCtx->frames = cmd->startFrame - 1;
            break;
    }
}

// Command 4: Set Environment Lighting
void Cutscene_Command_SetLighting(PlayState* play, CutsceneContext* csCtx, CsCmdEnvLighting* cmd) {
    if (csCtx->frames == cmd->startFrame) {
        play->envCtx.unk_BF = cmd->setting - 1;
        play->envCtx.unk_D8 = 1.0f;
    }
}

// Command 0x56: Play Background Music
void Cutscene_Command_PlayBGM(PlayState* play, CutsceneContext* csCtx, CsCmdMusicChange* cmd) {
    if (csCtx->frames == cmd->startFrame) {
        func_800F595C(cmd->sequence - 1);
    }
}

// Command 0x57: Stop Background Music
void Cutscene_Command_StopBGM(PlayState* play, CutsceneContext* csCtx, CsCmdMusicChange* cmd) {
    if (csCtx->frames == cmd->startFrame) {
        func_800F59E8(cmd->sequence - 1);
    }
}

// Command 0x7C: Fade Background Music over duration
void Cutscene_Command_FadeBGM(PlayState* play, CutsceneContext* csCtx, CsCmdMusicFade* cmd) {
    u8 var1;

    if ((csCtx->frames == cmd->startFrame) && (csCtx->frames < cmd->endFrame)) {
        var1 = cmd->endFrame - cmd->startFrame;

        if (cmd->type == 3) {
            Audio_QueueSeqCmd(var1 << 0x10 | (0x1 << 28 | SEQ_PLAYER_FANFARE << 24 | 0xFF));
        } else {
            Audio_QueueSeqCmd(var1 << 0x10 | (0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0xFF));
        }
    }
}

// Command 9: ?
void Cutscene_Command_09(PlayState* play, CutsceneContext* csCtx, CsCmdUnknown9* cmd) {
    if (csCtx->frames == cmd->startFrame) {
        func_800AA000(0.0f, cmd->unk_06, cmd->unk_07, cmd->unk_08);
    }
}

// Command 0x8C: Set Time of Day & Environment Time
void func_80065134(PlayState* play, CutsceneContext* csCtx, CsCmdDayTime* cmd) {
    s16 temp1;
    s16 temp2;

    if (csCtx->frames == cmd->startFrame) {
        temp1 = (cmd->hour * 60.0f) / (360.0f / 0x4000);
        temp2 = (cmd->minute + 1) / (360.0f / 0x4000);

        gSaveContext.dayTime = temp1 + temp2;
        gSaveContext.skyboxTime = temp1 + temp2;
    }
}

// Command 0x3E8: Code Execution (& Terminates Cutscene?)
void Cutscene_Command_Terminator(PlayState* play, CutsceneContext* csCtx, CsCmdBase* cmd) {
    Player* player = GET_PLAYER(play);
    s32 temp = 0;

    // Automatically skip certain cutscenes when in rando
    // cmd->base == 8: Traveling back/forward in time cutscene
    // cmd->base == 24: Dropping a fish for Jabu Jabu
    // cmd->base == 33: Zelda escaping with impa cutscene
    bool randoCsSkip = (gSaveContext.n64ddFlag && (cmd->base == 8 || cmd->base == 24 || cmd->base == 33));
    bool debugCsSkip = (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_START) &&
                        (gSaveContext.fileNum != 0xFEDC) && CVar_GetS32("gDebugEnabled", 0));

    if ((gSaveContext.gameMode != 0) && (gSaveContext.gameMode != 3) && (play->sceneNum != SCENE_SPOT00) &&
        (csCtx->frames > 20) &&
        (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_A) ||
         CHECK_BTN_ALL(play->state.input[0].press.button, BTN_B) ||
         CHECK_BTN_ALL(play->state.input[0].press.button, BTN_START)) &&
        (gSaveContext.fileNum != 0xFEDC) && (play->sceneLoadFlag == 0)) {
        Audio_PlaySoundGeneral(NA_SE_SY_PIECE_OF_HEART, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        temp = 1;
    }

    bool playCutscene = false;
    if (!CVar_GetS32("gCreditsFix", 1) && (cmd->startFrame == csCtx->frames)) {
        playCutscene = true;
    } else if (CVar_GetS32("gCreditsFix", 1)) {
        u16 delay = 0;
        
        // HACK:  Align visual timing with audio during credits sequence
        switch (cmd->base) {
            case 55: // Gerudo fortress (second scene of credits roll)
                delay = 20;
                break;
            case 56: // Kakariko village
                delay = 40;
                break;
            case 57: // Death mountain trail
                delay = 20;
                break;
            case 58: // Goron city
                delay = 20;
                break;
            case 59: // Lake hylia
                delay = 20;
                break;
            case 62: // Kokiri forest (houses)
                delay = 40;
                break;
            case 63: // Kokiri forest (deku tree)
                delay = 40;
                break;
            case 74: // First gorons dancing
                delay = 100;
                break;
            case 75: // Magic carpet guy and old shop keepers
                delay = 180;
                break;
            case 77: // Sad mido and king zora (plays after scene 78)
                delay = 100;
                break;
            case 78: // Second gorons dancing
                delay = 160;
                break;
        }
        if (cmd->startFrame + delay == csCtx->frames) {
            playCutscene = true;
        }
    }

    if (playCutscene || (temp != 0) || ((csCtx->frames > 20) && (randoCsSkip || debugCsSkip))) {

        csCtx->state = CS_STATE_UNSKIPPABLE_EXEC;
        Audio_SetCutsceneFlag(0);
        gSaveContext.cutsceneTransitionControl = 1;

        osSyncPrintf("\n分岐先指定！！=[%d]番", cmd->base); // "Future fork designation=No. [%d]"

        if ((gSaveContext.gameMode != 0) && (csCtx->frames != cmd->startFrame)) {
            gSaveContext.unk_13E7 = 1;
        }

        gSaveContext.cutsceneIndex = 0;

        switch (cmd->base) {
            case 1:
                play->nextEntranceIndex = 0x00A0;
                gSaveContext.cutsceneIndex = 0xFFF1;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 2:
                play->nextEntranceIndex = 0x00A0;
                gSaveContext.cutsceneIndex = 0xFFF0;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 10;
                break;
            case 3:
                play->nextEntranceIndex = 0x0117;
                gSaveContext.cutsceneIndex = 0xFFF1;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 10;
                break;
            case 4:
                play->nextEntranceIndex = 0x013D;
                gSaveContext.cutsceneIndex = 0xFFF0;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 10;
                break;
            case 5:
                play->nextEntranceIndex = 0x00EE;
                gSaveContext.cutsceneIndex = 0xFFF0;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 10;
                break;
            case 6:
                play->nextEntranceIndex = 0x00A0;
                gSaveContext.cutsceneIndex = 0xFFF2;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 10;
                break;
            case 7:
                play->nextEntranceIndex = 0x00EE;
                gSaveContext.cutsceneIndex = 0xFFF2;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 11;
                break;
            case 8:
                gSaveContext.fw.set = 0;
                gSaveContext.respawn[RESPAWN_MODE_TOP].data = 0;
                if (!(gSaveContext.eventChkInf[4] & 0x20)) {
                    gSaveContext.eventChkInf[4] |= 0x20;
                    play->nextEntranceIndex = 0x00A0;
                    play->sceneLoadFlag = 0x14;
                    gSaveContext.cutsceneIndex = 0xFFF3;
                    play->fadeTransition = 11;
                } else {
                    if (gSaveContext.sceneSetupIndex < 4) {
                        if (!LINK_IS_ADULT) {
                            play->linkAgeOnLoad = 0;
                        } else {
                            play->linkAgeOnLoad = 1;
                        }
                    }
                    play->nextEntranceIndex = 0x02CA;
                    play->sceneLoadFlag = 0x14;
                    play->fadeTransition = 3;
                    gSaveContext.nextTransitionType = 3;
                }
                break;
            case 9:
                play->nextEntranceIndex = 0x0117;
                gSaveContext.cutsceneIndex = 0xFFF0;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 12;
                break;
            case 10:
                play->nextEntranceIndex = 0x00BB;
                gSaveContext.cutsceneIndex = 0xFFF0;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 11:
                play->nextEntranceIndex = 0x00EE;
                gSaveContext.cutsceneIndex = 0xFFF3;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 3;
                break;
            case 12:
                play->nextEntranceIndex = 0x047A;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 13:
                play->nextEntranceIndex = 0x010E;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                gSaveContext.nextTransitionType = 2;
                break;
            case 14:
                play->nextEntranceIndex = 0x0457;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 15:
                play->nextEntranceIndex = 0x0053;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF4;
                play->fadeTransition = 3;
                break;
            case 16:
                play->nextEntranceIndex = 0x0053;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF5;
                play->fadeTransition = 3;
                break;
            case 17:
                play->nextEntranceIndex = 0x0053;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF6;
                play->fadeTransition = 3;
                break;
            case 18:
                gSaveContext.eventChkInf[4] |= 0x8000;
                play->nextEntranceIndex = 0x0324;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                gSaveContext.nextTransitionType = 2;
                break;
            case 19:
                play->nextEntranceIndex = 0x013D;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 4;
                gSaveContext.cutsceneIndex = 0x8000;
                break;
            case 21:
                play->nextEntranceIndex = 0x0102;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF0;
                play->fadeTransition = 3;
                break;
            case 22:
                Item_Give(play, ITEM_SONG_REQUIEM);
                play->nextEntranceIndex = 0x0123;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF0;
                play->fadeTransition = 3;
                break;
            case 23:
                play->nextEntranceIndex = 0x00A0;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF8;
                play->fadeTransition = 3;
                break;
            case 24:
                play->nextEntranceIndex = 0x0028;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 25:
                play->linkAgeOnLoad = 0;
                play->nextEntranceIndex = 0x006B;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF0;
                play->fadeTransition = 3;
                break;
            case 26:
                play->nextEntranceIndex = 0x0053;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF4;
                play->fadeTransition = 3;
                break;
            case 27:
                play->nextEntranceIndex = 0x0053;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF5;
                play->fadeTransition = 3;
                break;
            case 28:
                play->nextEntranceIndex = 0x0053;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF6;
                play->fadeTransition = 3;
                break;
            case 29:
                play->nextEntranceIndex = 0x006B;
                play->sceneLoadFlag = 0x14;
                gSaveContext.chamberCutsceneNum = 0;
                play->fadeTransition = 3;
                break;
            case 30:
                play->nextEntranceIndex = 0x006B;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 3;
                Item_Give(play, ITEM_MEDALLION_FIRE);
                gSaveContext.chamberCutsceneNum = 1;
                break;
            case 31:
                play->nextEntranceIndex = 0x006B;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 3;
                gSaveContext.chamberCutsceneNum = 2;
                break;
            case 32:
                play->linkAgeOnLoad = 1;
                play->nextEntranceIndex = 0x00CD;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF2;
                play->fadeTransition = 11;
                break;
            case 33:
                play->nextEntranceIndex = 0x00CD;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 3;
                break;
            case 34:
                play->nextEntranceIndex = 0x00A0;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF3;
                play->fadeTransition = 3;
                break;
            case 35:
                play->nextEntranceIndex = 0x00CD;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF0;
                play->fadeTransition = 4;
                break;
            case 38:
                play->nextEntranceIndex = 0x00A0;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF4;
                play->fadeTransition = 4;
                break;
            case 39:
                play->nextEntranceIndex = 0x0053;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF9;
                play->fadeTransition = 4;
                break;
            case 40:
                play->linkAgeOnLoad = 0;
                play->nextEntranceIndex = 0x0053;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFFA;
                play->fadeTransition = 4;
                break;
            case 41:
                play->nextEntranceIndex = 0x04E6;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 42:
                play->nextEntranceIndex = 0x00DB;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF2;
                play->fadeTransition = 4;
                break;
            case 43:
                play->nextEntranceIndex = 0x0503;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 4;
                break;
            case 44:
                play->nextEntranceIndex = 0x0320;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 17;
                break;
            case 46:
                gSaveContext.eventChkInf[4] |= 0x8000;
                play->nextEntranceIndex = 0x0324;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 4;
                break;
            case 47:
                Item_Give(play, ITEM_SONG_NOCTURNE);
                gSaveContext.eventChkInf[5] |= 0x10;
                play->nextEntranceIndex = 0x00DB;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF1;
                play->fadeTransition = 4;
                break;
            case 48:
                play->nextEntranceIndex = 0x01ED;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 15;
                gSaveContext.nextTransitionType = 15;
                break;
            case 49:
                play->nextEntranceIndex = 0x058C;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 4;
                break;
            case 50:
                play->nextEntranceIndex = 0x0513;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 17;
                break;
            case 51:
                play->nextEntranceIndex = 0x00CD;
                gSaveContext.cutsceneIndex = 0xFFF8;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 41;
                break;
            case 52:
                play->nextEntranceIndex = 0x0053;
                gSaveContext.cutsceneIndex = 0xFFF7;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 11;
                break;
            case 53:
                play->nextEntranceIndex = 0x050F;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 3;
                break;
            case 54:
                gSaveContext.gameMode = 3;
                Audio_SetSoundBanksMute(0x6F);
                play->linkAgeOnLoad = 1;
                play->nextEntranceIndex = 0x0117;
                gSaveContext.cutsceneIndex = 0xFFF2;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 55:
                play->nextEntranceIndex = 0x0129;
                gSaveContext.cutsceneIndex = 0xFFF1;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 56:
                play->nextEntranceIndex = 0x00DB;
                gSaveContext.cutsceneIndex = 0xFFF4;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 57:
                play->nextEntranceIndex = 0x013D;
                gSaveContext.cutsceneIndex = 0xFFF3;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 58:
                play->nextEntranceIndex = 0x014D;
                gSaveContext.cutsceneIndex = 0xFFF1;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 59:
                play->nextEntranceIndex = 0x0102;
                gSaveContext.cutsceneIndex = 0xFFF1;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 60:
                play->nextEntranceIndex = 0x010E;
                gSaveContext.cutsceneIndex = 0xFFF2;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 61:
                play->nextEntranceIndex = 0x0108;
                gSaveContext.cutsceneIndex = 0xFFF0;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 62:
                play->linkAgeOnLoad = 0;
                play->nextEntranceIndex = 0x00EE;
                gSaveContext.cutsceneIndex = 0xFFF6;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 63:
                play->nextEntranceIndex = 0x00EE;
                gSaveContext.cutsceneIndex = 0xFFF7;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 64:
                play->nextEntranceIndex = 0x00CD;
                gSaveContext.cutsceneIndex = 0xFFF5;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 65:
                play->linkAgeOnLoad = 1;
                play->nextEntranceIndex = 0x0157;
                gSaveContext.cutsceneIndex = 0xFFF2;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 66:
                play->nextEntranceIndex = 0x0554;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 67:
                play->nextEntranceIndex = 0x027E;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 68:
                play->nextEntranceIndex = 0x00A0;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF5;
                play->fadeTransition = 2;
                break;
            case 69:
                play->nextEntranceIndex = 0x05E8;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 70:
                play->nextEntranceIndex = 0x013D;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF4;
                play->fadeTransition = 2;
                gSaveContext.nextTransitionType = 2;
                break;
            case 71:
                gSaveContext.equips.equipment |= 0x0100;
                Player_SetEquipmentData(play, player);
                gSaveContext.equips.equipment |= 0x1000;
                Player_SetEquipmentData(play, player);
                play->linkAgeOnLoad = 1;
                play->nextEntranceIndex = 0x0053;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF1;
                play->fadeTransition = 2;
                break;
            case 72:
                play->nextEntranceIndex = 0x0400;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF0;
                play->fadeTransition = 2;
                gSaveContext.nextTransitionType = 2;
                break;
            case 73:
                play->linkAgeOnLoad = 1;
                play->nextEntranceIndex = 0x0157;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF2;
                play->fadeTransition = 2;
                break;
            case 74:
                play->nextEntranceIndex = 0x0157;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF3;
                play->fadeTransition = 3;
                gSaveContext.nextTransitionType = 3;
                break;
            case 75:
                play->linkAgeOnLoad = 1;
                play->nextEntranceIndex = 0x0157;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF4;
                play->fadeTransition = 2;
                break;
            case 76:
                play->linkAgeOnLoad = 0;
                play->nextEntranceIndex = 0x0157;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF5;
                play->fadeTransition = 2;
                break;
            case 77:
                play->linkAgeOnLoad = 1;
                play->nextEntranceIndex = 0x0157;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF6;
                play->fadeTransition = 2;
                break;
            case 78:
                play->nextEntranceIndex = 0x0157;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF7;
                play->fadeTransition = 2;
                break;
            case 79:
            case 80:
            case 81:
            case 82:
            case 83:
            case 84:
            case 85:
            case 86:
            case 87:
            case 88:
            case 89:
            case 90:
            case 91:
            case 92:
            case 93:
                play->nextEntranceIndex = 0x0157;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 94:
                play->nextEntranceIndex = 0x02AE;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 3;
                break;
            case 95:
                if ((gSaveContext.eventChkInf[4] & 0x100) && (gSaveContext.eventChkInf[4] & 0x200) &&
                    (gSaveContext.eventChkInf[4] & 0x400)) {
                    play->nextEntranceIndex = 0x0053;
                    play->sceneLoadFlag = 0x14;
                    gSaveContext.cutsceneIndex = 0xFFF3;
                    play->fadeTransition = 2;
                } else {
                    switch (gSaveContext.sceneSetupIndex) {
                        case 8:
                            play->nextEntranceIndex = 0x00FC;
                            play->sceneLoadFlag = 0x14;
                            play->fadeTransition = 2;
                            break;
                        case 9:
                            play->nextEntranceIndex = 0x0147;
                            play->sceneLoadFlag = 0x14;
                            play->fadeTransition = 2;
                            break;
                        case 10:
                            play->nextEntranceIndex = 0x0102;
                            play->sceneLoadFlag = 0x14;
                            gSaveContext.cutsceneIndex = 0xFFF0;
                            play->fadeTransition = 3;
                            break;
                    }
                }
                break;
            case 96:
                if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW)) {
                    play->nextEntranceIndex = 0x006B;
                    play->sceneLoadFlag = 0x14;
                    gSaveContext.cutsceneIndex = 0xFFF1;
                    play->fadeTransition = 5;
                } else {
                    gSaveContext.eventChkInf[12] |= 0x100;
                    play->nextEntranceIndex = 0x0610;
                    play->sceneLoadFlag = 0x14;
                    play->fadeTransition = 3;
                    gSaveContext.nextTransitionType = 3;
                }
                break;
            case 97:
                if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
                    play->nextEntranceIndex = 0x006B;
                    play->sceneLoadFlag = 0x14;
                    gSaveContext.cutsceneIndex = 0xFFF1;
                    play->fadeTransition = 5;
                } else {
                    play->nextEntranceIndex = 0x0580;
                    play->sceneLoadFlag = 0x14;
                    play->fadeTransition = 3;
                    gSaveContext.nextTransitionType = 3;
                }
                break;
            case 98:
                play->nextEntranceIndex = 0x0564;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 3;
                gSaveContext.nextTransitionType = 3;
                break;
            case 99:
                play->nextEntranceIndex = 0x0608;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                gSaveContext.nextTransitionType = 2;
                break;
            case 100:
                play->nextEntranceIndex = 0x00EE;
                gSaveContext.cutsceneIndex = 0xFFF8;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 3;
                gSaveContext.nextTransitionType = 3;
                break;
            case 101:
                play->nextEntranceIndex = 0x01F5;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 15;
                break;
            case 102:
                play->nextEntranceIndex = 0x0590;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 103:
                play->nextEntranceIndex = 0x00CD;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF3;
                play->fadeTransition = 2;
                break;
            case 104:
                switch (sTitleCsState) {
                    case 0:
                        play->nextEntranceIndex = 0x008D;
                        play->sceneLoadFlag = 0x14;
                        gSaveContext.cutsceneIndex = 0xFFF2;
                        play->fadeTransition = 2;
                        sTitleCsState++;
                        break;
                    case 1:
                        play->nextEntranceIndex = 0x0147;
                        play->sceneLoadFlag = 0x14;
                        gSaveContext.cutsceneIndex = 0xFFF1;
                        play->fadeTransition = 2;
                        sTitleCsState++;
                        break;
                    case 2:
                        play->nextEntranceIndex = 0x00A0;
                        play->sceneLoadFlag = 0x14;
                        gSaveContext.cutsceneIndex = 0xFFF6;
                        play->fadeTransition = 2;
                        sTitleCsState = 0;
                        break;
                }
                break;
            case 105:
                play->nextEntranceIndex = 0x00E4;
                play->sceneLoadFlag = 0x14;
                gSaveContext.cutsceneIndex = 0xFFF1;
                play->fadeTransition = 2;
                break;
            case 106:
                play->nextEntranceIndex = 0x0574;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 107:
                play->nextEntranceIndex = 0x0538;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 108:
                play->nextEntranceIndex = 0x053C;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 109:
                play->nextEntranceIndex = 0x0540;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 110:
                play->nextEntranceIndex = 0x0544;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 111:
                play->nextEntranceIndex = 0x0548;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 112:
                play->nextEntranceIndex = 0x054C;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 113:
                if (Flags_GetEventChkInf(0xBB) && Flags_GetEventChkInf(0xBC) && Flags_GetEventChkInf(0xBD) &&
                    Flags_GetEventChkInf(0xBE) && Flags_GetEventChkInf(0xBF) && Flags_GetEventChkInf(0xAD)) {
                    play->csCtx.segment = SEGMENTED_TO_VIRTUAL(gTowerBarrierCs);
                    play->csCtx.frames = 0;
                    gSaveContext.cutsceneTrigger = 1;
                    gSaveContext.cutsceneIndex = 0xFFFF;
                    csCtx->state = CS_STATE_UNSKIPPABLE_INIT;
                } else {
                    gSaveContext.cutsceneIndex = 0xFFFF;
                    csCtx->state = CS_STATE_UNSKIPPABLE_INIT;
                }
                break;
            case 114:
                play->nextEntranceIndex = 0x0185;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                break;
            case 115:
                play->nextEntranceIndex = 0x0594;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 2;
                gSaveContext.nextTransitionType = 2;
                break;
            case 116:
                if (gSaveContext.eventChkInf[12] & 0x100) {
                    play->nextEntranceIndex = 0x0580;
                    play->sceneLoadFlag = 0x14;
                    play->fadeTransition = 3;
                } else {
                    play->nextEntranceIndex = 0x0610;
                    play->sceneLoadFlag = 0x14;
                    play->fadeTransition = 3;
                }
                gSaveContext.nextTransitionType = 3;
                break;
            case 117:
                gSaveContext.gameMode = 3;
                Audio_SetSoundBanksMute(0x6F);
                play->linkAgeOnLoad = 0;
                play->nextEntranceIndex = 0x00CD;
                gSaveContext.cutsceneIndex = 0xFFF7;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 3;
                break;
            case 118:
                gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = 0x0517;
                Play_TriggerVoidOut(play);
                gSaveContext.respawnFlag = -2;
                gSaveContext.nextTransitionType = 2;
                break;
            case 119:
                gSaveContext.dayTime = 0x8000;
                gSaveContext.skyboxTime = 0x8000;
                play->nextEntranceIndex = 0x05F0;
                play->sceneLoadFlag = 0x14;
                play->fadeTransition = 3;
                break;
        }

        if (randoCsSkip) {
            Entrance_OverrideCutsceneEntrance(cmd->base);
        }
    }
}

// Command 0x2D: Transition Effects
void Cutscene_Command_TransitionFX(PlayState* play, CutsceneContext* csCtx, CsCmdBase* cmd) {
    f32 temp;

    if ((csCtx->frames >= cmd->startFrame) && (csCtx->frames <= cmd->endFrame)) {
        play->envCtx.fillScreen = true;
        temp = Environment_LerpWeight(cmd->endFrame, cmd->startFrame, csCtx->frames);

        switch (cmd->base) {
            case 1:
            case 5:
                play->envCtx.screenFillColor[0] = 160;
                play->envCtx.screenFillColor[1] = 160;
                play->envCtx.screenFillColor[2] = 160;
                if (cmd->base == 1) {
                    play->envCtx.screenFillColor[3] = 255.0f * temp;
                    if ((temp == 0.0f) && (gSaveContext.entranceIndex == 0x006B)) {
                        Audio_PlaySoundGeneral(NA_SE_SY_WHITE_OUT_S, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                    } else if ((temp == 0.0f) &&
                               ((gSaveContext.entranceIndex == 0x0053) || (gSaveContext.entranceIndex == 0x0138) ||
                                (gSaveContext.entranceIndex == 0x0371))) {
                        Audio_PlaySoundGeneral(NA_SE_EV_WHITE_OUT, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                    } else if ((temp == 0.0f) && (play->sceneNum == SCENE_GANONTIKA)) {
                        func_800788CC(NA_SE_EV_WHITE_OUT);
                    }
                } else {
                    play->envCtx.screenFillColor[3] = (1.0f - temp) * 255.0f;
                }
                break;
            case 2:
            case 6:
                play->envCtx.screenFillColor[0] = 0;
                play->envCtx.screenFillColor[1] = 0;
                play->envCtx.screenFillColor[2] = 255;
                if (cmd->base == 2) {
                    play->envCtx.screenFillColor[3] = 255.0f * temp;
                } else {
                    play->envCtx.screenFillColor[3] = (1.0f - temp) * 255.0f;
                }
                break;
            case 3:
            case 7:
                play->envCtx.screenFillColor[0] = 255;
                play->envCtx.screenFillColor[1] = 0;
                play->envCtx.screenFillColor[2] = 0;
                if (cmd->base == 3) {
                    play->envCtx.screenFillColor[3] = (1.0f - temp) * 255.0f;
                } else {
                    play->envCtx.screenFillColor[3] = 255.0f * temp;
                }
                break;
            case 4:
            case 8:
                play->envCtx.screenFillColor[0] = 0;
                play->envCtx.screenFillColor[1] = 255;
                play->envCtx.screenFillColor[2] = 0;
                if (cmd->base == 4) {
                    play->envCtx.screenFillColor[3] = (1.0f - temp) * 255.0f;
                } else {
                    play->envCtx.screenFillColor[3] = 255.0f * temp;
                }
                break;
            case 9:
                gSaveContext.cutsceneTransitionControl = 1;
                break;
            case 10:
            case 11:
                play->envCtx.screenFillColor[0] = 0;
                play->envCtx.screenFillColor[1] = 0;
                play->envCtx.screenFillColor[2] = 0;
                if (cmd->base == 10) {
                    play->envCtx.screenFillColor[3] = (1.0f - temp) * 255.0f;
                } else {
                    play->envCtx.screenFillColor[3] = 255.0f * temp;
                }
                break;
            case 12:
                gSaveContext.cutsceneTransitionControl = 255.0f - (155.0f * temp);
                break;
            case 13:
                play->envCtx.screenFillColor[0] = 0;
                play->envCtx.screenFillColor[1] = 0;
                play->envCtx.screenFillColor[2] = 0;
                play->envCtx.screenFillColor[3] = 255.0f - ((1.0f - temp) * 155.0f);
                break;
        }
    }
}

// Command 0x1 & 0x5: Camera Positions
size_t Cutscene_Command_CameraPositions(PlayState* play, CutsceneContext* csCtx, u8* cmd, u8 relativeToLink) {
    s32 shouldContinue = true;
    CsCmdBase* cmdBase = (CsCmdBase*)cmd;
    size_t size;

    cmd += sizeof(CutsceneData)*2;
    size = sizeof(CutsceneData)*2;

    if ((cmdBase->startFrame < csCtx->frames) && (csCtx->frames < cmdBase->endFrame) &&
        ((csCtx->unk_18 < cmdBase->startFrame) || (csCtx->unk_18 >= 0xF000))) {
        csCtx->unk_1B = 1;
        csCtx->cameraPosition = (CutsceneCameraPoint*)cmd;
        if (csCtx->unk_1A != 0) {
            csCtx->unk_18 = cmdBase->startFrame;
            if (D_8015FCC8 != 0) {
                Play_CameraChangeSetting(play, csCtx->unk_14, CAM_SET_CS_0);
                Play_ChangeCameraStatus(play, D_8015FCC6, CAM_STAT_WAIT);
                Play_ChangeCameraStatus(play, csCtx->unk_14, CAM_STAT_ACTIVE);
                Camera_ResetAnim(Play_GetCamera(play, csCtx->unk_14));
                Camera_SetCSParams(Play_GetCamera(play, csCtx->unk_14), csCtx->cameraFocus,
                                   csCtx->cameraPosition, GET_PLAYER(play), relativeToLink);
            }
        }
    }

    while (shouldContinue) {
        if (((CutsceneCameraPoint*)cmd)->continueFlag == CS_CMD_STOP) {
            shouldContinue = false;
        }
        cmd += sizeof(CutsceneData) * 4;
        size += sizeof(CutsceneData) * 4;
    }

    return size;
}

// Command 0x2 & 0x6: Camera Focus Points
size_t Cutscene_Command_CameraFocus(PlayState* play, CutsceneContext* csCtx, u8* cmd, u8 relativeToLink) {
    s32 shouldContinue = true;
    CsCmdBase* cmdBase = (CsCmdBase*)cmd;
    size_t size;

    cmd += sizeof(CutsceneData) * 2;
    size = sizeof(CutsceneData) * 2;

    if ((cmdBase->startFrame < csCtx->frames) && (csCtx->frames < cmdBase->endFrame) &&
        ((D_8015FCC0 < cmdBase->startFrame) || (D_8015FCC0 >= 0xF000))) {
        csCtx->unk_1A = 1;
        csCtx->cameraFocus = (CutsceneCameraPoint*)cmd;
        if (csCtx->unk_1B != 0) {
            D_8015FCC0 = cmdBase->startFrame;
            if (D_8015FCC8 != 0) {
                Play_CameraChangeSetting(play, csCtx->unk_14, CAM_SET_CS_0);
                Play_ChangeCameraStatus(play, D_8015FCC6, CAM_STAT_WAIT);
                Play_ChangeCameraStatus(play, csCtx->unk_14, CAM_STAT_ACTIVE);
                Camera_ResetAnim(Play_GetCamera(play, csCtx->unk_14));
                Camera_SetCSParams(Play_GetCamera(play, csCtx->unk_14), csCtx->cameraFocus,
                                   csCtx->cameraPosition, GET_PLAYER(play), relativeToLink);
            }
        }
    }

    while (shouldContinue) {
        if (((CutsceneCameraPoint*)cmd)->continueFlag == CS_CMD_STOP) {
            shouldContinue = false;
        }
        cmd += sizeof(CutsceneData) * 4;
        size += sizeof(CutsceneData) * 4;
    }

    return size;
}

// Command 0x7: ? (Related to camera positons)
size_t Cutscene_Command_07(PlayState* play, CutsceneContext* csCtx, u8* cmd, u8 unused) {
    CsCmdBase* cmdBase = (CsCmdBase*)cmd;
    size_t size;
    Vec3f sp3C;
    Vec3f sp30;
    Camera* sp2C;
    f32 sp28;

    cmd += sizeof(CutsceneData) * 2;
    size = sizeof(CutsceneData) * 2;

    if ((cmdBase->startFrame < csCtx->frames) && (csCtx->frames < cmdBase->endFrame) &&
        ((D_8015FCC2 < cmdBase->startFrame) || (D_8015FCC2 >= 0xF000))) {
        csCtx->unk_1B = 1;
        csCtx->cameraPosition = (CutsceneCameraPoint*)cmd;
        if (csCtx->unk_1A != 0) {
            D_8015FCC2 = cmdBase->startFrame;
            if (D_8015FCC8 != 0) {
                sp2C = Play_GetCamera(play, csCtx->unk_14);
                sp2C->player = NULL;
                Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
                Play_ChangeCameraStatus(play, csCtx->unk_14, CAM_STAT_ACTIVE);
                Play_CameraChangeSetting(play, csCtx->unk_14, CAM_SET_FREE0);
                sp28 = csCtx->cameraFocus->cameraRoll * 1.40625f;
                Camera_SetParam(sp2C, 64, &sp28);
                sp3C.x = csCtx->cameraFocus->pos.x;
                sp3C.y = csCtx->cameraFocus->pos.y;
                sp3C.z = csCtx->cameraFocus->pos.z;
                sp30.x = csCtx->cameraPosition->pos.x;
                sp30.y = csCtx->cameraPosition->pos.y;
                sp30.z = csCtx->cameraPosition->pos.z;
                Play_CameraSetAtEye(play, csCtx->unk_14, &sp3C, &sp30);
                Play_CameraSetFov(play, csCtx->unk_14, csCtx->cameraPosition->viewAngle);
            }
        }
    }

    size += sizeof(CutsceneData) * 4;

    return size;
}

// Command 0x8: ? (Related to camera focus points)
size_t Cutscene_Command_08(PlayState* play, CutsceneContext* csCtx, u8* cmd, u8 unused) {
    CsCmdBase* cmdBase = (CsCmdBase*)cmd;
    size_t size;
    Vec3f sp3C;
    Vec3f sp30;
    Camera* sp2C;
    f32 sp28;

    cmd += sizeof(CutsceneData) * 2;
    size = sizeof(CutsceneData) * 2;

    if ((cmdBase->startFrame < csCtx->frames) && (csCtx->frames < cmdBase->endFrame) &&
        ((D_8015FCC4 < cmdBase->startFrame) || (D_8015FCC4 >= 0xF000))) {
        csCtx->unk_1A = 1;
        csCtx->cameraFocus = (CutsceneCameraPoint*)cmd;
        if (csCtx->unk_1B != 0) {
            D_8015FCC4 = cmdBase->startFrame;
            if (D_8015FCC8 != 0) {
                sp2C = Play_GetCamera(play, csCtx->unk_14);
                sp2C->player = NULL;
                Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
                Play_ChangeCameraStatus(play, csCtx->unk_14, CAM_STAT_ACTIVE);
                Play_CameraChangeSetting(play, csCtx->unk_14, CAM_SET_FREE0);
                sp3C.x = csCtx->cameraFocus->pos.x;
                sp3C.y = csCtx->cameraFocus->pos.y;
                sp3C.z = csCtx->cameraFocus->pos.z;
                sp30.x = csCtx->cameraPosition->pos.x;
                sp30.y = csCtx->cameraPosition->pos.y;
                sp30.z = csCtx->cameraPosition->pos.z;
                Play_CameraSetAtEye(play, csCtx->unk_14, &sp3C, &sp30);
                Play_CameraSetFov(play, csCtx->unk_14, csCtx->cameraPosition->viewAngle);
            }
        }
    }

    size += sizeof(CutsceneData) * 4;

    return size;
}

// Command 0x13: Textbox
void Cutscene_Command_Textbox(PlayState* play, CutsceneContext* csCtx, CsCmdTextbox* cmd) {
    u8 dialogState;
    s16 originalCsFrames;

    if ((cmd->startFrame < csCtx->frames) && (csCtx->frames <= cmd->endFrame)) {
        if (cmd->type != 2) {
            if (D_8011E1C0 != cmd->base) {
                D_8011E1C0 = cmd->base;
                if ((cmd->type == 3) && CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                    Message_StartTextbox(play, cmd->textId1, NULL);
                } else if ((cmd->type == 4) && CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                    Message_StartTextbox(play, cmd->textId1, NULL);
                } else {
                    Message_StartTextbox(play, cmd->base, NULL);
                }
                return;
            }
        } else {
            if (D_8011E1C4 != cmd->base) {
                D_8011E1C4 = cmd->base;
                func_8010BD58(play, cmd->base);
                return;
            }
        }

        if (csCtx->frames >= cmd->endFrame) {
            originalCsFrames = csCtx->frames;
            dialogState = Message_GetState(&play->msgCtx);

            if ((dialogState != TEXT_STATE_CLOSING) && (dialogState != TEXT_STATE_NONE) &&
                (dialogState != TEXT_STATE_SONG_DEMO_DONE) && (dialogState != TEXT_STATE_8)) {
                csCtx->frames--;

                if ((dialogState == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
                    if (play->msgCtx.choiceIndex == 0) {
                        if (cmd->textId1 != 0xFFFF) {
                            Message_ContinueTextbox(play, cmd->textId1);
                        } else {
                            csCtx->frames++;
                        }
                    } else {
                        if (cmd->textId2 != 0xFFFF) {
                            Message_ContinueTextbox(play, cmd->textId2);
                        } else {
                            csCtx->frames++;
                        }
                    }
                }

                if (dialogState == TEXT_STATE_9) {
                    if (cmd->textId1 != 0xFFFF) {
                        Message_ContinueTextbox(play, cmd->textId1);
                    } else {
                        csCtx->frames++;
                    }
                }

                if (dialogState == TEXT_STATE_EVENT) {
                    if (Message_ShouldAdvance(play)) {
                        func_8010BD58(play, cmd->base);
                    }
                }
            }

            if (csCtx->frames == originalCsFrames) {
                Interface_ChangeAlpha(1);
                D_8011E1C0 = 0;
                D_8011E1C4 = 0;
            }
        }
    }
}

void Cutscene_ProcessCommands(PlayState* play, CutsceneContext* csCtx, u8* cutscenePtr) {
    s16 i;
    s32 totalEntries;
    s32 cmdType;
    s32 cmdEntries;
    CsCmdBase* cmd;
    s32 cutsceneEndFrame;
    s16 j;

    if (ResourceMgr_OTRSigCheck(cutscenePtr))
        cutscenePtr = ResourceMgr_LoadCSByName(cutscenePtr);

    memcpy(&totalEntries, cutscenePtr, sizeof(CutsceneData));
    cutscenePtr += sizeof(CutsceneData);
    memcpy(&cutsceneEndFrame, cutscenePtr, sizeof(CutsceneData));
    cutscenePtr += sizeof(CutsceneData);

    if ((cutsceneEndFrame < csCtx->frames) && (csCtx->state != CS_STATE_UNSKIPPABLE_EXEC)) {
        csCtx->state = CS_STATE_UNSKIPPABLE_INIT;
        return;
    }

    if (CVar_GetS32("gDebugEnabled", 0) && CHECK_BTN_ALL(play->state.input[0].press.button, BTN_DRIGHT)) {
        csCtx->state = CS_STATE_UNSKIPPABLE_INIT;
        return;
    }

    for (i = 0; i < totalEntries; i++) {
        memcpy(&cmdType, cutscenePtr, sizeof(CutsceneData));
        cutscenePtr += sizeof(CutsceneData);

        if (cmdType == -1) {
            return;
        }

        //printf("CmdType: %04X\n", cmdType);

        switch (cmdType) {
            case CS_CMD_MISC:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    func_80064824(play, csCtx, (void*)cutscenePtr);
                    cutscenePtr += sizeof(CutsceneData) * 12;
                }
                break;
            case CS_CMD_SET_LIGHTING:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    Cutscene_Command_SetLighting(play, csCtx, (void*)cutscenePtr);
                    cutscenePtr += sizeof(CutsceneData) * 12;
                }
                break;
            case CS_CMD_PLAYBGM:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    Cutscene_Command_PlayBGM(play, csCtx, (void*)cutscenePtr);
                    cutscenePtr += sizeof(CutsceneData) * 12;
                }
                break;
            case CS_CMD_STOPBGM:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    Cutscene_Command_StopBGM(play, csCtx, (void*)cutscenePtr);
                    cutscenePtr += sizeof(CutsceneData) * 12;
                }
                break;
            case CS_CMD_FADEBGM:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    Cutscene_Command_FadeBGM(play, csCtx, (void*)cutscenePtr);
                    cutscenePtr += sizeof(CutsceneData) * 12;
                }
                break;
            case CS_CMD_09:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    Cutscene_Command_09(play, csCtx, (void*)cutscenePtr);
                    cutscenePtr += sizeof(CutsceneData) * 3;
                }
                break;
            case CS_CMD_SETTIME:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    func_80065134(play, csCtx, (void*)cutscenePtr);
                    cutscenePtr += sizeof(CutsceneData) * 3;
                }
                break;
            case CS_CMD_SET_PLAYER_ACTION:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    cmd = (CsCmdBase*)cutscenePtr;
                    if ((cmd->startFrame < csCtx->frames) && (csCtx->frames <= cmd->endFrame)) {
                        csCtx->linkAction = (void*)cutscenePtr;
                    }
                    cutscenePtr += sizeof(CutsceneData) * 12;
                }
                break;
            case CS_CMD_SET_ACTOR_ACTION_1:
            case 17:
            case 18:
            case 23:
            case 34:
            case 39:
            case 46:
            case 76:
            case 85:
            case 93:
            case 105:
            case 107:
            case 110:
            case 119:
            case 123:
            case 138:
            case 139:
            case 144:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    cmd = (CsCmdBase*)cutscenePtr;
                    if ((cmd->startFrame < csCtx->frames) && (csCtx->frames <= cmd->endFrame)) {
                        csCtx->npcActions[0] = (void*)cutscenePtr;
                    }
                    cutscenePtr += sizeof(CutsceneData) * 12;
                }
                break;
            case CS_CMD_SET_ACTOR_ACTION_2:
            case 16:
            case 24:
            case 35:
            case 40:
            case 48:
            case 64:
            case 68:
            case 70:
            case 78:
            case 80:
            case 94:
            case 116:
            case 118:
            case 120:
            case 125:
            case 131:
            case 141:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    cmd = (CsCmdBase*)cutscenePtr;
                    if ((cmd->startFrame < csCtx->frames) && (csCtx->frames <= cmd->endFrame)) {
                        csCtx->npcActions[1] = (void*)cutscenePtr;
                    }
                    cutscenePtr += sizeof(CutsceneData) * 12;
                }
                break;
            case CS_CMD_SET_ACTOR_ACTION_3:
            case 36:
            case 41:
            case 50:
            case 67:
            case 69:
            case 72:
            case 74:
            case 81:
            case 106:
            case 117:
            case 121:
            case 126:
            case 132:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    cmd = (CsCmdBase*)cutscenePtr;
                    if ((cmd->startFrame < csCtx->frames) && (csCtx->frames <= cmd->endFrame)) {
                        csCtx->npcActions[2] = (void*)cutscenePtr;
                    }
                    cutscenePtr += sizeof(CutsceneData) * 12;
                }
                break;
            case CS_CMD_SET_ACTOR_ACTION_4:
            case 37:
            case 42:
            case 51:
            case 53:
            case 63:
            case 65:
            case 66:
            case 75:
            case 82:
            case 108:
            case 127:
            case 133:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    cmd = (CsCmdBase*)cutscenePtr;
                    if ((cmd->startFrame < csCtx->frames) && (csCtx->frames <= cmd->endFrame)) {
                        csCtx->npcActions[3] = (void*)cutscenePtr;
                    }
                    cutscenePtr += sizeof(CutsceneData) * 12;
                }
                break;
            case CS_CMD_SET_ACTOR_ACTION_5:
            case 38:
            case 43:
            case 47:
            case 54:
            case 79:
            case 83:
            case 128:
            case 135:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    cmd = (CsCmdBase*)cutscenePtr;
                    if ((cmd->startFrame < csCtx->frames) && (csCtx->frames <= cmd->endFrame)) {
                        csCtx->npcActions[4] = (void*)cutscenePtr;
                    }
                    cutscenePtr += sizeof(CutsceneData) * 12;
                }
                break;
            case CS_CMD_SET_ACTOR_ACTION_6:
            case 55:
            case 77:
            case 84:
            case 90:
            case 129:
            case 136:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    cmd = (CsCmdBase*)cutscenePtr;
                    if ((cmd->startFrame < csCtx->frames) && (csCtx->frames <= cmd->endFrame)) {
                        csCtx->npcActions[5] = (void*)cutscenePtr;
                    }
                    cutscenePtr += sizeof(CutsceneData) * 12;
                }
                break;
            case CS_CMD_SET_ACTOR_ACTION_7:
            case 52:
            case 57:
            case 58:
            case 88:
            case 115:
            case 130:
            case 137:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    cmd = (CsCmdBase*)cutscenePtr;
                    if ((cmd->startFrame < csCtx->frames) && (csCtx->frames <= cmd->endFrame)) {
                        csCtx->npcActions[6] = (void*)cutscenePtr;
                    }
                    cutscenePtr += sizeof(CutsceneData) * 12;
                }
                break;
            case CS_CMD_SET_ACTOR_ACTION_8:
            case 60:
            case 89:
            case 111:
            case 114:
            case 134:
            case 142:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    cmd = (CsCmdBase*)cutscenePtr;
                    if ((cmd->startFrame < csCtx->frames) && (csCtx->frames <= cmd->endFrame)) {
                        csCtx->npcActions[7] = (void*)cutscenePtr;
                    }
                    cutscenePtr += sizeof(CutsceneData) * 12;
                }
                break;
            case CS_CMD_SET_ACTOR_ACTION_9:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    cmd = (CsCmdBase*)cutscenePtr;
                    if ((cmd->startFrame < csCtx->frames) && (csCtx->frames <= cmd->endFrame)) {
                        csCtx->npcActions[8] = (void*)cutscenePtr;
                    }
                    cutscenePtr += sizeof(CutsceneData) * 12;
                }
                break;
            case CS_CMD_SET_ACTOR_ACTION_10:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    cmd = (CsCmdBase*)cutscenePtr;
                    if ((cmd->startFrame < csCtx->frames) && (csCtx->frames <= cmd->endFrame)) {
                        csCtx->npcActions[9] = (void*)cutscenePtr;
                    }
                    cutscenePtr += sizeof(CutsceneData) * 12;
                }
                break;
            case CS_CMD_CAM_EYE:
                cutscenePtr += Cutscene_Command_CameraPositions(play, csCtx, (void*)cutscenePtr, 0);
                break;
            case CS_CMD_CAM_EYE_REL_TO_PLAYER:
                cutscenePtr += Cutscene_Command_CameraPositions(play, csCtx, (void*)cutscenePtr, 1);
                break;
            case CS_CMD_CAM_AT:
                cutscenePtr += Cutscene_Command_CameraFocus(play, csCtx, (void*)cutscenePtr, 0);
                break;
            case CS_CMD_CAM_AT_REL_TO_PLAYER:
                cutscenePtr += Cutscene_Command_CameraFocus(play, csCtx, (void*)cutscenePtr, 1);
                break;
            case CS_CMD_07:
                cutscenePtr += Cutscene_Command_07(play, csCtx, (void*)cutscenePtr, 0);
                break;
            case CS_CMD_08:
                cutscenePtr += Cutscene_Command_08(play, csCtx, (void*)cutscenePtr, 0);
                break;
            case CS_CMD_TERMINATOR:
                cutscenePtr += 4;
                Cutscene_Command_Terminator(play, csCtx, (void*)cutscenePtr);
                cutscenePtr += 8;
                break;
            case CS_CMD_TEXTBOX:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += 4;
                for (j = 0; j < cmdEntries; j++) {
                    cmd = (CsCmdBase*)cutscenePtr;
                    if (cmd->base != 0xFFFF) {
                        Cutscene_Command_Textbox(play, csCtx, (void*)cutscenePtr);
                    }
                    cutscenePtr += sizeof(CutsceneData) * 3;
                }
                break;
            case CS_CMD_SCENE_TRANS_FX:
                cutscenePtr += sizeof(CutsceneData);
                Cutscene_Command_TransitionFX(play, csCtx, (void*)cutscenePtr);
                cutscenePtr += sizeof(CutsceneData) * 2;
                break;
            default:
                memcpy(&cmdEntries, cutscenePtr, sizeof(CutsceneData));
                cutscenePtr += sizeof(CutsceneData);
                for (j = 0; j < cmdEntries; j++) {
                    cutscenePtr += sizeof(CutsceneData) * 12;
                }
                break;
        }
    }
}

void func_80068C3C(PlayState* play, CutsceneContext* csCtx) {
    Gfx* displayList;
    Gfx* prevDisplayList;

    if (gSaveContext.cutsceneIndex >= 0xFFF0) {

        if (BREG(0) != 0) {
            OPEN_DISPS(play->state.gfxCtx);

            prevDisplayList = POLY_OPA_DISP;
            displayList = Graph_GfxPlusOne(POLY_OPA_DISP);
            gSPDisplayList(OVERLAY_DISP++, displayList);
            Cutscene_DrawDebugInfo(play, &displayList, csCtx);
            gSPEndDisplayList(displayList++);
            Graph_BranchDlist(prevDisplayList, displayList);
            POLY_OPA_DISP = displayList;

            CLOSE_DISPS(play->state.gfxCtx);
        }

        csCtx->frames++;
        if (dREG(95) != 0) {
            Cutscene_ProcessCommands(play, csCtx, D_8012D1F0);
        } else {
            Cutscene_ProcessCommands(play, csCtx, play->csCtx.segment);
        }
    }
}

void func_80068D84(PlayState* play, CutsceneContext* csCtx) {
    if (func_8006472C(play, csCtx, 0.0f)) {
        Audio_SetCutsceneFlag(0);
        csCtx->state = CS_STATE_IDLE;
    }
}

void func_80068DC0(PlayState* play, CutsceneContext* csCtx) {
    s16 i;

    if (func_8006472C(play, csCtx, 0.0f)) {
        csCtx->linkAction = NULL;

        for (i = 0; i < 10; i++) {
            csCtx->npcActions[i] = NULL;
        }

        osSyncPrintf("\n\n\n\n\nやっぱりここかいな"); // "Right here, huh"
        gSaveContext.cutsceneIndex = 0;
        gSaveContext.gameMode = 0;

        if (D_8015FCC8 != 0) {
            switch (gSaveContext.entranceIndex) {
                case 0x028A:
                case 0x028E:
                case 0x0292:
                case 0x0476:
                    Play_CopyCamera(play, D_8015FCC6, csCtx->unk_14);
            }

            Play_ChangeCameraStatus(play, D_8015FCC6, CAM_STAT_ACTIVE);
            Play_ClearCamera(play, csCtx->unk_14);
            func_8005B1A4(play->cameraPtrs[D_8015FCC6]);
        }

        Audio_SetCutsceneFlag(0);
        csCtx->state = CS_STATE_IDLE;
    }
}

void func_80068ECC(PlayState* play, CutsceneContext* csCtx) {
    u8 i;

    if ((gSaveContext.cutsceneTrigger != 0) && (csCtx->state == CS_STATE_IDLE) && !Player_InCsMode(play)) {
        gSaveContext.cutsceneIndex = 0xFFFD;
    }

    if ((gSaveContext.cutsceneIndex >= 0xFFF0) && (csCtx->state == CS_STATE_IDLE)) {
        Flags_UnsetEnv(play, 0);

        D_8011E1C0 = 0;
        D_8011E1C4 = 0;
        csCtx->unk_12 = 0;
        csCtx->linkAction = NULL;

        for (i = 0; i < 10; i++) {
            csCtx->npcActions[i] = NULL;
        }

        csCtx->state++;

        if (csCtx->state == CS_STATE_SKIPPABLE_INIT) {
            Audio_SetCutsceneFlag(1);

            csCtx->frames = 0xFFFF;
            csCtx->unk_18 = 0xFFFF;
            D_8015FCC0 = 0xFFFF;
            D_8015FCC2 = 0xFFFF;
            D_8015FCC4 = 0xFFFF;
            csCtx->unk_1A = 0;
            csCtx->unk_1B = 0;
            D_8015FCC6 = play->activeCamera;

            if (D_8015FCC8 != 0) {
                csCtx->unk_14 = Play_CreateSubCamera(play);
            }

            if (gSaveContext.cutsceneTrigger == 0) {
                Interface_ChangeAlpha(1);
                ShrinkWindow_SetVal(0x20);
                ShrinkWindow_SetCurrentVal(0x20);
                csCtx->state++;
            }

            func_80068C3C(play, csCtx);
        }

        gSaveContext.cutsceneTrigger = 0;
    }
}

void func_80069048(PlayState* play) {
    s16 i;

    D_8015FCCC = 0;
    for (i = 0; i < 20; i++) {
        ; // Empty Loop
    }
    D_8015FCE4 = 0;
}

void func_8006907C(PlayState* play) {
    if (D_8015FCCC != 0) {
        D_8015FCCC = 0;
    }
}

void Cutscene_HandleEntranceTriggers(PlayState* play) {
    EntranceCutscene* entranceCutscene;
    u8 requiredAge;
    s16 i;

    if (gSaveContext.n64ddFlag &&
        // don't skip epona escape cutscenes 
        gSaveContext.entranceIndex != 650 &&
        gSaveContext.entranceIndex != 654 &&
        gSaveContext.entranceIndex != 658 &&
        gSaveContext.entranceIndex != 1142 &&
        // don't skip nabooru iron knuckle cs
        gSaveContext.entranceIndex != 141) {
        gSaveContext.showTitleCard = false;
        return;
    }

    for (i = 0; i < ARRAY_COUNT(sEntranceCutsceneTable); i++) {
        entranceCutscene = &sEntranceCutsceneTable[i];

        requiredAge = entranceCutscene->ageRestriction;
        if (requiredAge == 2) {
            requiredAge = gSaveContext.linkAge;
        }

        if ((gSaveContext.entranceIndex == entranceCutscene->entrance) &&
            (!Flags_GetEventChkInf(entranceCutscene->flag) || (entranceCutscene->flag == 0x18)) &&
            (gSaveContext.cutsceneIndex < 0xFFF0) && ((u8)gSaveContext.linkAge == requiredAge) &&
            (gSaveContext.respawnFlag <= 0)) {
            Flags_SetEventChkInf(entranceCutscene->flag);
            Cutscene_SetSegment(play, entranceCutscene->segAddr);
            gSaveContext.cutsceneTrigger = 2;
            gSaveContext.showTitleCard = false;
            break;
        }
    }
}

void Cutscene_HandleConditionalTriggers(PlayState* play) {
    osSyncPrintf("\ngame_info.mode=[%d] restart_flag", ((void)0, gSaveContext.respawnFlag));

    if ((gSaveContext.gameMode == 0) && (gSaveContext.respawnFlag <= 0) && (gSaveContext.cutsceneIndex < 0xFFF0)) {
        const bool bShouldTowerRandoSkip =
            (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SKIP_TOWER_ESCAPE));
        if ((gSaveContext.entranceIndex == 0x01E1) && !Flags_GetEventChkInf(0xAC)) {
            if (!gSaveContext.n64ddFlag) {
                Flags_SetEventChkInf(0xAC);
                gSaveContext.entranceIndex = 0x0123;
                gSaveContext.cutsceneIndex = 0xFFF0;
            }
        } else if ((gSaveContext.entranceIndex == 0x00DB) && LINK_IS_ADULT && (gSaveContext.eventChkInf[4] & 0x0100) &&
                   (gSaveContext.eventChkInf[4] & 0x0200) && (gSaveContext.eventChkInf[4] & 0x0400) &&
                   !Flags_GetEventChkInf(0xAA)) {
            if (!gSaveContext.n64ddFlag) {
                Flags_SetEventChkInf(0xAA);
                gSaveContext.cutsceneIndex = 0xFFF0;
            }
        } else if ((gSaveContext.entranceIndex == 0x05E0) && !Flags_GetEventChkInf(0xC1)) {
            if (!gSaveContext.n64ddFlag) {
                Flags_SetEventChkInf(0xC1);
                Item_Give(play, ITEM_OCARINA_FAIRY);
                gSaveContext.entranceIndex = 0x011E;
                gSaveContext.cutsceneIndex = 0xFFF0;
            }
        } else if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) && CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) &&
                   LINK_IS_ADULT && !Flags_GetEventChkInf(0xC4) &&
                   (gEntranceTable[((void)0, gSaveContext.entranceIndex)].scene == SCENE_TOKINOMA)) {
            if (!gSaveContext.n64ddFlag) {
                Flags_SetEventChkInf(0xC4);
                gSaveContext.entranceIndex = 0x0053;
                gSaveContext.cutsceneIndex = 0xFFF8;
            }
        } else if ((!Flags_GetEventChkInf(0xC7) &&
                       gEntranceTable[((void)0, gSaveContext.entranceIndex)].scene == SCENE_GANON_DEMO) ||
                   (bShouldTowerRandoSkip &&
                    gEntranceTable[((void)0, gSaveContext.entranceIndex)].scene == SCENE_GANON_FINAL)) {
            Flags_SetEventChkInf(0xC7);
            gSaveContext.entranceIndex = 0x0517;
            // If we are rando and tower escape skip is on, then set the flag to say we saw the towers fall
            // and exit.
            if (bShouldTowerRandoSkip) {
                return;
            }
            gSaveContext.cutsceneIndex = 0xFFF0;
        }
    }
}

void Cutscene_SetSegment(PlayState* play, void* segment) {
    if (SEGMENT_NUMBER(segment) != 0)
    {
        play->csCtx.segment = SEGMENTED_TO_VIRTUAL(segment);
    } else {
        play->csCtx.segment = segment;
    }
}
