#include "z_kaleido_scope.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
#include "textures/icon_item_nes_static/icon_item_nes_static.h"
#include "textures/icon_item_ger_static/icon_item_ger_static.h"
#include "textures/icon_item_fra_static/icon_item_fra_static.h"
#include "textures/icon_item_field_static/icon_item_field_static.h"
#include "textures/icon_item_dungeon_static/icon_item_dungeon_static.h"
#include "textures/icon_item_nes_static/icon_item_nes_static.h"

void KaleidoScope_DrawDungeonMap(PlayState* play, GraphicsContext* gfxCtx) {
    static void* dungeonItemTexs[] = {
        gBossKeyIconTex,
        gCompassIconTex,
        gDungeonMapIconTex,
    };
    static void* dungeonTitleTexs[] = {
        gPauseDekuTitleENGTex, gPauseDodongoTitleENGTex,   gPauseJabuTitleENGTex,   gPauseForestTitleENGTex,
        gPauseFireTitleENGTex, gPauseWaterTitleENGTex,     gPauseSpiritTitleENGTex, gPauseShadowTitleENGTex,
        gPauseBotWTitleENGTex, gPauseIceCavernTitleENGTex,

        gPauseDekuTitleGERTex, gPauseDodongoTitleGERTex,   gPauseJabuTitleGERTex,   gPauseForestTitleGERTex,
        gPauseFireTitleGERTex, gPauseWaterTitleGERTex,     gPauseSpiritTitleGERTex, gPauseShadowTitleGERTex,
        gPauseBotWTitleGERTex, gPauseIceCavernTitleGERTex,

        gPauseDekuTitleFRATex, gPauseDodongoTitleFRATex,   gPauseJabuTitleFRATex,   gPauseForestTitleFRATex,
        gPauseFireTitleFRATex, gPauseWaterTitleFRATex,     gPauseSpiritTitleFRATex, gPauseShadowTitleFRATex,
        gPauseBotWTitleFRATex, gPauseIceCavernTitleFRATex,
    };
    static void* floorIconTexs[] = {
        gDungeonMapBlankFloorButtonTex, gDungeonMap8FButtonTex, gDungeonMap7FButtonTex, gDungeonMap6FButtonTex,
        gDungeonMap5FButtonTex,         gDungeonMap4FButtonTex, gDungeonMap3FButtonTex, gDungeonMap2FButtonTex,
        gDungeonMap1FButtonTex,         gDungeonMapB1ButtonTex, gDungeonMapB2ButtonTex, gDungeonMapB3ButtonTex,
        gDungeonMapB4ButtonTex,         gDungeonMapB5ButtonTex, gDungeonMapB6ButtonTex, gDungeonMapB7ButtonTex,
        gDungeonMapB8ButtonTex,
    };
    static u16 mapBgPulseColors[][3] = {
        { 0 / 8, 80 / 8, 255 / 8 },
        { 0 / 8, 200 / 8, 140 / 8 },
    };
    static s16 mapBgPulseR = 0 / 8;
    static s16 mapBgPulseG = 200 / 8;
    static s16 mapBgPulseB = 140 / 8;
    static u16 mapBgPulseTimer = 20;
    static u16 mapBgPulseStage = 0;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    PauseContext* pauseCtx = &play->pauseCtx;
    Input* input = &play->state.input[0];
    s16 i;
    s16 j;
    s16 oldCursorPoint;
    s16 stepR;
    s16 stepG;
    s16 stepB;
    u16 rgba16;
    bool dpad = CVar_GetS32("gDpadPause", 0);

    OPEN_DISPS(gfxCtx);

    if ((pauseCtx->state == 6) && (pauseCtx->unk_1E4 == 0) && (pauseCtx->pageIndex == PAUSE_MAP)) {
        pauseCtx->cursorColorSet = 0;
        oldCursorPoint = pauseCtx->cursorPoint[PAUSE_MAP];

        if (pauseCtx->cursorSpecialPos == 0) {
            if ((pauseCtx->stickRelX > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DRIGHT))) {
                if (pauseCtx->cursorX[PAUSE_MAP] != 0) {
                    KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);
                } else {
                    pauseCtx->cursorX[PAUSE_MAP] = 1;
                    pauseCtx->cursorPoint[PAUSE_MAP] = 0;
                    if (!CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, gSaveContext.mapIndex)) {
                        pauseCtx->cursorPoint[PAUSE_MAP]++;
                        if (!CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, gSaveContext.mapIndex)) {
                            pauseCtx->cursorPoint[PAUSE_MAP]++;
                            if (!CHECK_DUNGEON_ITEM(DUNGEON_MAP, gSaveContext.mapIndex)) {
                                KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);
                            }
                        }
                    }
                }
            } else if ((pauseCtx->stickRelX < -30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DLEFT))) {
                if (pauseCtx->cursorX[PAUSE_MAP] == 0) {
                    KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);
                } else {
                    pauseCtx->cursorX[PAUSE_MAP] = 0;
                    pauseCtx->cursorPoint[PAUSE_MAP] = pauseCtx->dungeonMapSlot;
                    osSyncPrintf("kscope->cursor_point=%d\n", pauseCtx->cursorPoint[PAUSE_MAP]);
                    R_MAP_TEX_INDEX =
                        R_MAP_TEX_INDEX_BASE +
                        gMapData->floorTexIndexOffset[gSaveContext.mapIndex][pauseCtx->cursorPoint[PAUSE_MAP] - 3];
                    KaleidoScope_UpdateDungeonMap(play);
                }
            }

            if (pauseCtx->cursorPoint[PAUSE_MAP] < 3) {
                if ((pauseCtx->stickRelY > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DUP))) {
                    if (pauseCtx->cursorPoint[PAUSE_MAP] != 0) {
                        for (i = pauseCtx->cursorPoint[PAUSE_MAP] - 1; i >= 0; i--) {
                            if (CHECK_DUNGEON_ITEM(i, gSaveContext.mapIndex)) {
                                pauseCtx->cursorPoint[PAUSE_MAP] = i;
                                break;
                            }
                        }
                    }
                } else {
                    if ((pauseCtx->stickRelY < -30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DDOWN))) {
                        if (pauseCtx->cursorPoint[PAUSE_MAP] != 2) {
                            for (i = pauseCtx->cursorPoint[PAUSE_MAP] + 1; i < 3; i++) {
                                if (CHECK_DUNGEON_ITEM(i, gSaveContext.mapIndex)) {
                                    pauseCtx->cursorPoint[PAUSE_MAP] = i;
                                    break;
                                }
                            }
                        }
                    }
                }
            } else {
                if ((pauseCtx->stickRelY > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DUP))) {
                    if (pauseCtx->cursorPoint[PAUSE_MAP] >= 4) {
                        for (i = pauseCtx->cursorPoint[PAUSE_MAP] - 3 - 1; i >= 0; i--) {
                            if ((gSaveContext.sceneFlags[gSaveContext.mapIndex].floors & gBitFlags[i]) ||
                                (CHECK_DUNGEON_ITEM(DUNGEON_MAP, gSaveContext.mapIndex) &&
                                 (gMapData->floorID[interfaceCtx->unk_25A][i] != 0))) {
                                pauseCtx->cursorPoint[PAUSE_MAP] = i + 3;
                                break;
                            }
                        }
                    }
                } else if ((pauseCtx->stickRelY < -30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DDOWN))) {
                    if (pauseCtx->cursorPoint[PAUSE_MAP] != 10) {
                        for (i = pauseCtx->cursorPoint[PAUSE_MAP] - 3 + 1; i < 11; i++) {
                            if ((gSaveContext.sceneFlags[gSaveContext.mapIndex].floors & gBitFlags[i]) ||
                                (CHECK_DUNGEON_ITEM(DUNGEON_MAP, gSaveContext.mapIndex) &&
                                 (gMapData->floorID[interfaceCtx->unk_25A][i] != 0))) {
                                pauseCtx->cursorPoint[PAUSE_MAP] = i + 3;
                                break;
                            }
                        }
                    }
                }

                i = R_MAP_TEX_INDEX;
                R_MAP_TEX_INDEX =
                    R_MAP_TEX_INDEX_BASE +
                    gMapData->floorTexIndexOffset[gSaveContext.mapIndex][pauseCtx->cursorPoint[PAUSE_MAP] - 3];
                pauseCtx->dungeonMapSlot = pauseCtx->cursorPoint[PAUSE_MAP];
                if (i != R_MAP_TEX_INDEX) {
                    KaleidoScope_UpdateDungeonMap(play);
                }
            }
        } else if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
            if ((pauseCtx->stickRelX > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DRIGHT))) {
                pauseCtx->nameDisplayTimer = 0;
                pauseCtx->cursorSpecialPos = 0;
                pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_MAP] = pauseCtx->dungeonMapSlot;
                pauseCtx->cursorX[PAUSE_MAP] = 0;
                j = 72 + (pauseCtx->cursorSlot[PAUSE_MAP] * 4);
                KaleidoScope_SetCursorVtx(pauseCtx, j, pauseCtx->mapPageVtx);
                Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
        } else {
            if ((pauseCtx->stickRelX < -30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DLEFT))) {
                pauseCtx->nameDisplayTimer = 0;
                pauseCtx->cursorSpecialPos = 0;
                pauseCtx->cursorX[PAUSE_MAP] = 1;
                pauseCtx->cursorPoint[PAUSE_MAP] = 0;
                if (!CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, gSaveContext.mapIndex)) {
                    pauseCtx->cursorPoint[PAUSE_MAP]++;
                    if (!CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, gSaveContext.mapIndex)) {
                        pauseCtx->cursorPoint[PAUSE_MAP]++;
                        if (!CHECK_DUNGEON_ITEM(DUNGEON_MAP, gSaveContext.mapIndex)) {
                            pauseCtx->cursorX[PAUSE_MAP] = 0;
                            pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_MAP] =
                                pauseCtx->dungeonMapSlot;
                            R_MAP_TEX_INDEX =
                                R_MAP_TEX_INDEX_BASE +
                                gMapData
                                    ->floorTexIndexOffset[gSaveContext.mapIndex][pauseCtx->cursorPoint[PAUSE_MAP] - 3];
                            KaleidoScope_UpdateDungeonMap(play);
                        }
                    }
                } else {
                    pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_MAP];
                }

                osSyncPrintf("kscope->cursor_point====%d\n", pauseCtx->cursorPoint[PAUSE_MAP]);
                j = 72 + (pauseCtx->cursorSlot[PAUSE_MAP] * 4);
                KaleidoScope_SetCursorVtx(pauseCtx, j, pauseCtx->mapPageVtx);
                Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
        }

        if (oldCursorPoint != pauseCtx->cursorPoint[PAUSE_MAP]) {
            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }

    if (pauseCtx->cursorSpecialPos == 0) {
        if (pauseCtx->cursorPoint[PAUSE_MAP] < 3) {
            pauseCtx->cursorItem[PAUSE_MAP] = ITEM_KEY_BOSS + pauseCtx->cursorPoint[PAUSE_MAP];
        } else {
            pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
        }

        pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_MAP];

        j = 72 + (pauseCtx->cursorSlot[PAUSE_MAP] * 4);
        KaleidoScope_SetCursorVtx(pauseCtx, j, pauseCtx->mapPageVtx);

        if (pauseCtx->cursorX[PAUSE_MAP] == 0) {
            pauseCtx->mapPageVtx[j + 0].v.ob[0] = pauseCtx->mapPageVtx[j + 2].v.ob[0] =
                pauseCtx->mapPageVtx[j + 0].v.ob[0] - 2;
            pauseCtx->mapPageVtx[j + 1].v.ob[0] = pauseCtx->mapPageVtx[j + 3].v.ob[0] =
                pauseCtx->mapPageVtx[j + 1].v.ob[0] + 4;
            pauseCtx->mapPageVtx[j + 0].v.ob[1] = pauseCtx->mapPageVtx[j + 1].v.ob[1] =
                pauseCtx->mapPageVtx[j + 0].v.ob[1] + 2;
            pauseCtx->mapPageVtx[j + 2].v.ob[1] = pauseCtx->mapPageVtx[j + 3].v.ob[1] =
                pauseCtx->mapPageVtx[j + 2].v.ob[1] - 4;
        }
    }

    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA, G_CC_MODULATEIA);

    gSPVertex(POLY_KAL_DISP++, &pauseCtx->mapPageVtx[68], 16, 0);

    gDPLoadTextureBlock(POLY_KAL_DISP++, dungeonTitleTexs[gSaveContext.mapIndex+(10*gSaveContext.language)], G_IM_FMT_IA, G_IM_SIZ_8b, 96, 16, 0,
                        G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSP1Quadrangle(POLY_KAL_DISP++, 0, 2, 3, 1, 0);

    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    for (i = 0, j = 4; i < 3; i++, j += 4) {
        if (CHECK_DUNGEON_ITEM(i, gSaveContext.mapIndex)) {
            gDPLoadTextureBlock(POLY_KAL_DISP++, dungeonItemTexs[i], G_IM_FMT_RGBA, G_IM_SIZ_32b, 24, 24, 0,
                                G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);

            gSP1Quadrangle(POLY_KAL_DISP++, j, j + 2, j + 3, j + 1, 0);
        }
    }

    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 200, pauseCtx->alpha);

    gSPVertex(POLY_KAL_DISP++, &pauseCtx->mapPageVtx[84], 32, 0);

    for (i = j = 0; i < 8; i++, j += 4) {
        if ((gSaveContext.sceneFlags[gSaveContext.mapIndex].floors & gBitFlags[i]) ||
            CHECK_DUNGEON_ITEM(DUNGEON_MAP, gSaveContext.mapIndex)) {
            if (i != (pauseCtx->dungeonMapSlot - 3)) {
                gDPLoadTextureBlock(POLY_KAL_DISP++, floorIconTexs[gMapData->floorID[interfaceCtx->unk_25A][i]],
                                    G_IM_FMT_IA, G_IM_SIZ_8b, 24, 16, 0, G_TX_WRAP | G_TX_NOMIRROR,
                                    G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                gSP1Quadrangle(POLY_KAL_DISP++, j, j + 2, j + 3, j + 1, 0);
            }
        }
    }

    j = (pauseCtx->dungeonMapSlot - 3) * 4;

    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 150, 150, 255, pauseCtx->alpha);

    gDPLoadTextureBlock(POLY_KAL_DISP++,
                        floorIconTexs[gMapData->floorID[interfaceCtx->unk_25A][pauseCtx->dungeonMapSlot - 3]],
                        G_IM_FMT_IA, G_IM_SIZ_8b, 24, 16, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR,
                        G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSP1Quadrangle(POLY_KAL_DISP++, j, j + 2, j + 3, j + 1, 0);

    pauseCtx->mapPageVtx[124].v.ob[0] = pauseCtx->mapPageVtx[126].v.ob[0] = pauseCtx->mapPageVtx[124].v.ob[0] + 2;
    pauseCtx->mapPageVtx[125].v.ob[0] = pauseCtx->mapPageVtx[127].v.ob[0] = pauseCtx->mapPageVtx[124].v.ob[0] + 19;
    pauseCtx->mapPageVtx[124].v.ob[1] = pauseCtx->mapPageVtx[125].v.ob[1] = pauseCtx->mapPageVtx[124].v.ob[1] - 2;
    pauseCtx->mapPageVtx[126].v.ob[1] = pauseCtx->mapPageVtx[127].v.ob[1] = pauseCtx->mapPageVtx[124].v.ob[1] - 19;

    gSPVertex(POLY_KAL_DISP++, &pauseCtx->mapPageVtx[116], 12, 0);

    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

    pauseCtx->mapPageVtx[116].v.ob[1] = pauseCtx->mapPageVtx[117].v.ob[1] = pauseCtx->offsetY - (VREG(30) * 14) + 49;
    pauseCtx->mapPageVtx[118].v.ob[1] = pauseCtx->mapPageVtx[119].v.ob[1] = pauseCtx->mapPageVtx[116].v.ob[1] - 16;

    gDPLoadTextureBlock(POLY_KAL_DISP++, gDungeonMapLinkHeadTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 16, 0,
                        G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSP1Quadrangle(POLY_KAL_DISP++, 0, 2, 3, 1, 0);

    if (CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, gSaveContext.mapIndex) &&
        (gMapData->skullFloorIconY[gSaveContext.mapIndex] != -99)) {
        pauseCtx->mapPageVtx[120].v.ob[1] = pauseCtx->mapPageVtx[121].v.ob[1] =
            gMapData->skullFloorIconY[gSaveContext.mapIndex] + pauseCtx->offsetY;
        pauseCtx->mapPageVtx[122].v.ob[1] = pauseCtx->mapPageVtx[123].v.ob[1] = pauseCtx->mapPageVtx[120].v.ob[1] - 16;

        gDPLoadTextureBlock(POLY_KAL_DISP++, gDungeonMapSkullTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 16, 0,
                            G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);

        gSP1Quadrangle(POLY_KAL_DISP++, 4, 6, 7, 5, 0);
    }

    gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

    if (GET_GS_FLAGS(gSaveContext.mapIndex) == gAreaGsFlags[gSaveContext.mapIndex]) {
        KaleidoScope_DrawQuadTextureRGBA32(gfxCtx, gGoldSkulltulaIconTex, 24, 24, 8);
    }

    if ((play->sceneNum >= SCENE_YDAN) && (play->sceneNum <= SCENE_TAKARAYA)) {
        stepR = (mapBgPulseR - mapBgPulseColors[mapBgPulseStage][0]) / mapBgPulseTimer;
        stepG = (mapBgPulseG - mapBgPulseColors[mapBgPulseStage][1]) / mapBgPulseTimer;
        stepB = (mapBgPulseB - mapBgPulseColors[mapBgPulseStage][2]) / mapBgPulseTimer;
        mapBgPulseR -= stepR;
        mapBgPulseG -= stepG;
        mapBgPulseB -= stepB;

        rgba16 = ((mapBgPulseR & 0x1F) << 11) | ((mapBgPulseG & 0x1F) << 6) | ((mapBgPulseB & 0x1F) << 1) | 1;
        interfaceCtx->mapPalette[28] = (rgba16 & 0xFF00) >> 8;
        interfaceCtx->mapPalette[29] = rgba16 & 0xFF;

        mapBgPulseTimer--;
        if (mapBgPulseTimer == 0) {
            mapBgPulseStage ^= 1;
            mapBgPulseTimer = 20;
        }
    }

    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetTextureFilter(POLY_KAL_DISP++, G_TF_POINT);
    gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

    gDPLoadTLUT_pal16(POLY_KAL_DISP++, 0, interfaceCtx->mapPalette);
    gDPSetTextureLUT(POLY_KAL_DISP++, G_TT_RGBA16);

    gSPVertex(POLY_KAL_DISP++, &pauseCtx->mapPageVtx[60], 8, 0);

    // The dungeon map textures are recreated each frame, so always invalidate them
    gSPInvalidateTexCache(POLY_KAL_DISP++, interfaceCtx->mapSegment);
    gSPInvalidateTexCache(POLY_KAL_DISP++, interfaceCtx->mapSegment + 0x800);

    gDPLoadTextureBlock_4b(POLY_KAL_DISP++, interfaceCtx->mapSegment, G_IM_FMT_CI, 48, 85, 0, G_TX_WRAP | G_TX_NOMIRROR,
                           G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSP1Quadrangle(POLY_KAL_DISP++, 0, 2, 3, 1, 0);

    gDPLoadTextureBlock_4b(POLY_KAL_DISP++, interfaceCtx->mapSegment + 0x800, G_IM_FMT_CI, 48, 85, 0,
                           G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);

    gSP1Quadrangle(POLY_KAL_DISP++, 4, 6, 7, 5, 0);

    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetTextureFilter(POLY_KAL_DISP++, G_TF_BILERP);

    CLOSE_DISPS(gfxCtx);
}

void KaleidoScope_DrawWorldMap(PlayState* play, GraphicsContext* gfxCtx) {
    static void* cloudTexs[] = {
        gWorldMapCloud16Tex, gWorldMapCloud15Tex, gWorldMapCloud14Tex, gWorldMapCloud13Tex,
        gWorldMapCloud12Tex, gWorldMapCloud11Tex, gWorldMapCloud10Tex, gWorldMapCloud9Tex,
        gWorldMapCloud8Tex,  gWorldMapCloud7Tex,  gWorldMapCloud6Tex,  gWorldMapCloud5Tex,
        gWorldMapCloud4Tex,  gWorldMapCloud3Tex,  gWorldMapCloud2Tex,  gWorldMapCloud1Tex,
    };
    static u16 cloudFlagNums[] = {
        0x05, 0x00, 0x13, 0x0E, 0x0F, 0x01, 0x02, 0x10, 0x12, 0x03, 0x07, 0x08, 0x09, 0x0C, 0x0B, 0x06,
    };
    static s16 pointPulsePrimColor[] = { 0, 0, 255 };
    static s16 pointPrimColors[][3] = {
        { 0, 0, 255 },
        { 255, 255, 0 },
    };
    static s16 pointPulseEnvColor[] = { 255, 255, 0 };
    static s16 pointEnvColors[][3] = {
        { 255, 255, 0 },
        { 0, 0, 255 },
    };
    static s16 pointPulseStage = 1;
    static s16 pointPulseTimer = 20;
    static s16 D_8082A5B8[] = { 64, 64, 64, 28 };
    static s16 areaBoxPosX[] = {
        -41, 19, 44, 40, 49, 51, -49, 83, 80, -67, 50, -109, -76, -86, -10, -6, 19, 24, 11, -17, 37, -6,
    };
    static s16 areaBoxWidths[] = {
        96, 32, 32, 48, 48, 32, 48, 32, 32, 32, 16, 32, 32, 16, 32, 32, 32, 32, 32, 32, 16, 32,
    };
    static s16 areaBoxPosY[] = {
        30, 36, 35, 26, 7, 11, -31, 30, 38, 23, 2, 42, 40, 32, 38, 50, 57, 58, 56, 12, 36, 50,
    };
    static s16 areaBoxHeights[] = {
        59, 19, 13, 19, 38, 17, 38, 17, 13, 26, 16, 26, 26, 16, 19, 17, 26, 13, 17, 17, 16, 17,
    };
    static void* areaBoxTexs[] = {
        gWorldMapAreaBox7Tex, gWorldMapAreaBox1Tex, gWorldMapAreaBox4Tex, gWorldMapAreaBox6Tex, gWorldMapAreaBox2Tex,
        gWorldMapAreaBox3Tex, gWorldMapAreaBox2Tex, gWorldMapAreaBox3Tex, gWorldMapAreaBox4Tex, gWorldMapAreaBox5Tex,
        gWorldMapAreaBox8Tex, gWorldMapAreaBox5Tex, gWorldMapAreaBox5Tex, gWorldMapAreaBox8Tex, gWorldMapAreaBox1Tex,
        gWorldMapAreaBox3Tex, gWorldMapAreaBox5Tex, gWorldMapAreaBox4Tex, gWorldMapAreaBox3Tex, gWorldMapAreaBox3Tex,
        gWorldMapAreaBox8Tex, gWorldMapAreaBox3Tex,
    };
    static void* currentPosTitleTexs[] = {
        gPauseCurrentPositionENGTex,
        gPauseCurrentPositionGERTex,
        gPauseCurrentPositionFRATex,
    };
    static u16 D_8082A6D4 = 0;
    PauseContext* pauseCtx = &play->pauseCtx;
    Input* input = &play->state.input[0];
    s16 i;
    s16 j;
    s16 t;
    s16 k;
    s16 oldCursorPoint;
    s16 stepR;
    s16 stepG;
    s16 stepB;
    bool dpad = CVar_GetS32("gDpadPause", 0);

    OPEN_DISPS(gfxCtx);

    if ((pauseCtx->state == 6) && (pauseCtx->unk_1E4 == 0) && (pauseCtx->pageIndex == PAUSE_MAP)) {
        pauseCtx->cursorColorSet = 0;
        oldCursorPoint = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];

        if (pauseCtx->cursorSpecialPos == 0) {
            if ((pauseCtx->stickRelX > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DRIGHT))) {
                D_8082A6D4 = 0;

                do {
                    pauseCtx->cursorPoint[PAUSE_WORLD_MAP]++;
                    if (pauseCtx->cursorPoint[PAUSE_WORLD_MAP] > 11) {
                        pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = 11;
                        KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);
                        break;
                    }
                } while (pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]] == 0);
            } else if ((pauseCtx->stickRelX < -30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DLEFT))) {
                D_8082A6D4 = 0;

                do {
                    pauseCtx->cursorPoint[PAUSE_WORLD_MAP]--;
                    if (pauseCtx->cursorPoint[PAUSE_WORLD_MAP] < 0) {
                        pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = 0;
                        KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);
                        break;
                    }
                } while (pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]] == 0);
            } else {
                D_8082A6D4++;
            }

            pauseCtx->cursorItem[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
            pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_WORLD_MAP] + 0x1F;
            KaleidoScope_SetCursorVtx(pauseCtx, pauseCtx->cursorSlot[PAUSE_MAP] * 4, pauseCtx->mapPageVtx);
        } else {
            pauseCtx->cursorItem[PAUSE_MAP] = gSaveContext.worldMapArea + 0x18;
            if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
                if ((pauseCtx->stickRelX > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DRIGHT))) {
                    pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = 0;
                    pauseCtx->cursorSpecialPos = 0;

                    while (pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]] == 0) {
                        pauseCtx->cursorPoint[PAUSE_WORLD_MAP]++;
                    }

                    pauseCtx->cursorItem[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
                    pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_WORLD_MAP] + 0x1F;
                    KaleidoScope_SetCursorVtx(pauseCtx, pauseCtx->cursorSlot[PAUSE_MAP] * 4, pauseCtx->mapPageVtx);
                    Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                    D_8082A6D4 = 0;
                }
            } else {
                if ((pauseCtx->stickRelX < -30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DLEFT))) {
                    pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = 11;
                    pauseCtx->cursorSpecialPos = 0;

                    while (pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]] == 0) {
                        pauseCtx->cursorPoint[PAUSE_WORLD_MAP]--;
                    }

                    pauseCtx->cursorItem[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
                    pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_WORLD_MAP] + 0x1F;
                    KaleidoScope_SetCursorVtx(pauseCtx, pauseCtx->cursorSlot[PAUSE_MAP] * 4, pauseCtx->mapPageVtx);
                    Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                    D_8082A6D4 = 0;
                }
            }
        }

        if (pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]] == 0) {
            pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
        }

        if (oldCursorPoint != pauseCtx->cursorPoint[PAUSE_WORLD_MAP]) {
            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }

    gDPPipeSync(POLY_KAL_DISP++);

    if (HREG(15) == 0) {
        gDPSetTextureFilter(POLY_KAL_DISP++, G_TF_POINT);

        gDPLoadTLUT_pal256(POLY_KAL_DISP++, gWorldMapImageTLUT);
        gDPSetTextureLUT(POLY_KAL_DISP++, G_TT_RGBA16);

        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
        gSPVertex(POLY_KAL_DISP++, &pauseCtx->mapPageVtx[188], 32, 0);

        for (j = t = i = 0; i < 8; i++, t++, j += 4) {
            gDPLoadTextureBlock(POLY_KAL_DISP++, (u8*)ResourceMgr_LoadTexByName(gWorldMapImageTex) + t * 216 * 9, G_IM_FMT_CI, G_IM_SIZ_8b, 216, 9,
                                0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);

            gSP1Quadrangle(POLY_KAL_DISP++, j, j + 2, j + 3, j + 1, 0);
        }

        gSPVertex(POLY_KAL_DISP++, &pauseCtx->mapPageVtx[220], 28, 0);

        for (j = i = 0; i < 6; i++, t++, j += 4) 
        {
            gDPLoadTextureBlock(POLY_KAL_DISP++, (u8*)ResourceMgr_LoadTexByName(gWorldMapImageTex) + t * 216 * 9, G_IM_FMT_CI, G_IM_SIZ_8b, 216, 9,
                                0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);

            gSP1Quadrangle(POLY_KAL_DISP++, j, j + 2, j + 3, j + 1, 0);
        }

        gDPLoadTextureBlock(POLY_KAL_DISP++, (u8*)ResourceMgr_LoadTexByName(gWorldMapImageTex) + t * 216 * 9, G_IM_FMT_CI, G_IM_SIZ_8b, 216, 2, 0,
                            G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);

        gSP1Quadrangle(POLY_KAL_DISP++, j, j + 2, j + 3, j + 1, 0);
    } else if (HREG(15) == 1) {
        Gfx* sp1CC = POLY_KAL_DISP;
        void* mapImage = gWorldMapImageTex;

        // gSPLoadUcodeL(sp1CC++, rspS2DEX)?
        //gSPLoadUcodeEx(sp1CC++, OS_K0_TO_PHYSICAL(D_80113070), OS_K0_TO_PHYSICAL(D_801579A0), 0x800);

        func_8009638C(&sp1CC, mapImage, gWorldMapImageTLUT, 216, 128, G_IM_FMT_CI, G_IM_SIZ_8b, 0x8000, 256,
                      HREG(13) / 100.0f, HREG(14) / 100.0f);

        // gSPLoadUcode(sp1CC++, SysUcode_GetUCode(), SysUcode_GetUCodeData())?
        gSPLoadUcodeEx(sp1CC++, SysUcode_GetUCode(), SysUcode_GetUCodeData(), 0x800);

        POLY_KAL_DISP = sp1CC;
    }

    if (HREG(15) == 2) {
        HREG(15) = 1;
        HREG(14) = 6100;
        HREG(13) = 5300;
    }

    if (ZREG(38) == 0) {
        gDPPipeSync(POLY_KAL_DISP++);
        gDPSetTextureFilter(POLY_KAL_DISP++, G_TF_BILERP);

        func_800949A8(gfxCtx);

        gDPSetCombineLERP(POLY_KAL_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                          PRIMITIVE, 0);
        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 235, 235, 235, pauseCtx->alpha);

        for (k = 0; k < 15; k += 8) {
            gSPVertex(POLY_KAL_DISP++, &pauseCtx->mapPageVtx[60 + k * 4], 32, 0);

            for (j = i = 0; i < 8; i++, j += 4) {
                if (!(gSaveContext.worldMapAreaData & gBitFlags[cloudFlagNums[k + i]])) {
                    gDPLoadTextureBlock_4b(POLY_KAL_DISP++, cloudTexs[k + i], G_IM_FMT_I, D_8082AAEC[k + i],
                                           D_8082AB2C[k + i], 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR,
                                           G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                    gSP1Quadrangle(POLY_KAL_DISP++, j, j + 2, j + 3, j + 1, 0);
                }
            }
        }
    }

    if (gSaveContext.worldMapArea < 22) {
        gDPPipeSync(POLY_KAL_DISP++);
        gDPSetTextureFilter(POLY_KAL_DISP++, G_TF_POINT);

        pauseCtx->mapPageVtx[172].v.ob[0] = pauseCtx->mapPageVtx[174].v.ob[0] =
            areaBoxPosX[((void)0, gSaveContext.worldMapArea)];

        pauseCtx->mapPageVtx[173].v.ob[0] = pauseCtx->mapPageVtx[175].v.ob[0] =
            pauseCtx->mapPageVtx[172].v.ob[0] + areaBoxWidths[((void)0, gSaveContext.worldMapArea)];

        pauseCtx->mapPageVtx[172].v.ob[1] = pauseCtx->mapPageVtx[173].v.ob[1] =
            areaBoxPosY[((void)0, gSaveContext.worldMapArea)] + pauseCtx->offsetY;

        pauseCtx->mapPageVtx[174].v.ob[1] = pauseCtx->mapPageVtx[175].v.ob[1] =
            pauseCtx->mapPageVtx[172].v.ob[1] - areaBoxHeights[((void)0, gSaveContext.worldMapArea)];

        pauseCtx->mapPageVtx[173].v.tc[0] = pauseCtx->mapPageVtx[175].v.tc[0] =
            areaBoxWidths[((void)0, gSaveContext.worldMapArea)] << 5;

        pauseCtx->mapPageVtx[174].v.tc[1] = pauseCtx->mapPageVtx[175].v.tc[1] =
            areaBoxHeights[((void)0, gSaveContext.worldMapArea)] << 5;

        gSPVertex(POLY_KAL_DISP++, &pauseCtx->mapPageVtx[172], 4, 0);

        gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 100, 255, 255, pauseCtx->alpha);

        gDPLoadTextureBlock_4b(POLY_KAL_DISP++, areaBoxTexs[((void)0, (gSaveContext.worldMapArea))], G_IM_FMT_IA,
                               areaBoxWidths[((void)0, (gSaveContext.worldMapArea))],
                               areaBoxHeights[((void)0, (gSaveContext.worldMapArea))], 0, G_TX_WRAP | G_TX_NOMIRROR,
                               G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gSP1Quadrangle(POLY_KAL_DISP++, 0, 2, 3, 1, 0);

        gDPPipeSync(POLY_KAL_DISP++);
        gDPSetTextureFilter(POLY_KAL_DISP++, G_TF_BILERP);
    }

    stepR = ABS(pointPulsePrimColor[0] - pointPrimColors[pointPulseStage][0]) / pointPulseTimer;
    stepG = ABS(pointPulsePrimColor[1] - pointPrimColors[pointPulseStage][1]) / pointPulseTimer;
    stepB = ABS(pointPulsePrimColor[2] - pointPrimColors[pointPulseStage][2]) / pointPulseTimer;
    if (pointPulsePrimColor[0] >= pointPrimColors[pointPulseStage][0]) {
        pointPulsePrimColor[0] -= stepR;
    } else {
        pointPulsePrimColor[0] += stepR;
    }
    if (pointPulsePrimColor[1] >= pointPrimColors[pointPulseStage][1]) {
        pointPulsePrimColor[1] -= stepG;
    } else {
        pointPulsePrimColor[1] += stepG;
    }
    if (pointPulsePrimColor[2] >= pointPrimColors[pointPulseStage][2]) {
        pointPulsePrimColor[2] -= stepB;
    } else {
        pointPulsePrimColor[2] += stepB;
    }

    stepR = ABS(pointPulseEnvColor[0] - pointEnvColors[pointPulseStage][0]) / pointPulseTimer;
    stepG = ABS(pointPulseEnvColor[1] - pointEnvColors[pointPulseStage][1]) / pointPulseTimer;
    stepB = ABS(pointPulseEnvColor[2] - pointEnvColors[pointPulseStage][2]) / pointPulseTimer;
    if (pointPulseEnvColor[0] >= pointEnvColors[pointPulseStage][0]) {
        pointPulseEnvColor[0] -= stepR;
    } else {
        pointPulseEnvColor[0] += stepR;
    }
    if (pointPulseEnvColor[1] >= pointEnvColors[pointPulseStage][1]) {
        pointPulseEnvColor[1] -= stepG;
    } else {
        pointPulseEnvColor[1] += stepG;
    }
    if (pointPulseEnvColor[2] >= pointEnvColors[pointPulseStage][2]) {
        pointPulseEnvColor[2] -= stepB;
    } else {
        pointPulseEnvColor[2] += stepB;
    }

    pointPulseTimer--;
    if (pointPulseTimer == 0) {
        pointPulsePrimColor[0] = pointPrimColors[pointPulseStage][0];
        pointPulsePrimColor[1] = pointPrimColors[pointPulseStage][1];
        pointPulsePrimColor[2] = pointPrimColors[pointPulseStage][2];
        pointPulseEnvColor[0] = pointEnvColors[pointPulseStage][0];
        pointPulseEnvColor[1] = pointEnvColors[pointPulseStage][1];
        pointPulseEnvColor[2] = pointEnvColors[pointPulseStage][2];
        pointPulseStage ^= 1;
        pointPulseTimer = 20;
    }

    func_800949A8(gfxCtx);

    gDPSetCombineLERP(POLY_KAL_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    gDPLoadTextureBlock(POLY_KAL_DISP++, gWorldMapDotTex, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_WRAP | G_TX_NOMIRROR,
                        G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    for (j = t = i = 0; i < 12; i++, t++, j += 4) {
        if (pauseCtx->worldMapPoints[i] != 0) {
            gDPPipeSync(POLY_KAL_DISP++);

            if (pauseCtx->worldMapPoints[i] == 1) {
                gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, pointPrimColors[0][0], pointPrimColors[0][1],
                                pointPrimColors[0][2], pauseCtx->alpha);
                gDPSetEnvColor(POLY_KAL_DISP++, pointEnvColors[0][0], pointEnvColors[0][1], pointEnvColors[0][2], 0);
            } else {
                gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, pointPulsePrimColor[0], pointPulsePrimColor[1],
                                pointPulsePrimColor[2], pauseCtx->alpha);
                gDPSetEnvColor(POLY_KAL_DISP++, pointPulseEnvColor[0], pointPulseEnvColor[1], pointPulseEnvColor[2], 0);
            }

            gSPVertex(POLY_KAL_DISP++, &pauseCtx->mapPageVtx[124 + i * 4], 4, 0);

            gSP1Quadrangle(POLY_KAL_DISP++, 0, 2, 3, 1, 0);
        }
    }

    if (pauseCtx->cursorSpecialPos == 0) {
        KaleidoScope_DrawCursor(play, PAUSE_MAP);
    }

    gSPVertex(POLY_KAL_DISP++, &pauseCtx->mapPageVtx[176], 16, 0);

    if (pauseCtx->tradeQuestLocation != 0xFF) {
        gDPPipeSync(POLY_KAL_DISP++);
        gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, pointPulsePrimColor[0], 0, pauseCtx->alpha);

        gDPLoadTextureBlock(POLY_KAL_DISP++, gWorldMapArrowTex, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                            G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);

        gSP1Quadrangle(POLY_KAL_DISP++, 0, 2, 3, 1, 0);
    }

    if (gSaveContext.worldMapArea < 22) {
        gDPPipeSync(POLY_KAL_DISP++);
        gDPSetCombineLERP(POLY_KAL_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 150, 255, 255, pauseCtx->alpha);
        gDPSetEnvColor(POLY_KAL_DISP++, 0, 0, 0, 0);


        POLY_KAL_DISP = KaleidoScope_QuadTextureIA8(POLY_KAL_DISP, pauseCtx->nameSegment + 0x400, 80, 32, 4);
    }

    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetCombineLERP(POLY_KAL_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                      PRIMITIVE, 0);
    gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 0, 0, 0, pauseCtx->alpha);

    gDPLoadTextureBlock_4b(POLY_KAL_DISP++, currentPosTitleTexs[gSaveContext.language], G_IM_FMT_I, 64, 8, 0,
                           G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);

    gSP1Quadrangle(POLY_KAL_DISP++, 8, 10, 11, 9, 0);

    gDPPipeSync(POLY_KAL_DISP++);

    CLOSE_DISPS(gfxCtx);
}
