/*
 * File: z_kaleido_map.c
 * Overlay: ovl_kaleido_scope
 * Description: Pause Menu: Map Page
 */

#include "z_kaleido_scope.h"
#include "interface/parameter_static/parameter_static.h"
#include "interface/icon_item_field_static/icon_item_field_static.h"
#include "interface/icon_item_dungeon_static/icon_item_dungeon_static.h"
#include "interface/icon_item_jpn_static/icon_item_jpn_static.h"
#include "archives/icon_item_24_static/icon_item_24_static_yar.h"

#include "BenPort.h"

#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/Songs/Songs.h"
#include <libultraship/bridge/consolevariablebridge.h>

// 2S2H [Port] (and line 26) don't do pointer math and access the list of digits directly.
extern const char* sCounterTextures[];

// 2S2H [Port] The cursor updating logic for owl warping can get stuck in an infinite loop
// when there are no world map points registered. This can happen when using index warping and moving the cursor
// on the menu. We want to completely avoid an infinite loop on the port, so we check to see that there are no points
// and break out of the loop or soft reset as needed.
// Defined as a macro for re-use and to be able to "break" out of the parent while loop.
#define SHIP_HANDLE_OWL_CURSOR_INF_LOOP()                 \
    {                                                     \
        bool hasPoint = false;                            \
        for (int i = 0; i <= OWL_WARP_STONE_TOWER; i++) { \
            if (pauseCtx->worldMapPoints[i]) {            \
                hasPoint = true;                          \
                break;                                    \
            }                                             \
        }                                                 \
        if (!hasPoint) {                                  \
            Ship_HandleConsoleCrashAsReset();             \
            break;                                        \
        }                                                 \
    }

void KaleidoScope_DrawDungeonStrayFairyCount(PlayState* play) {
    s16 counterDigits[2];
    s16 rectLeft;
    s16 digitIndex;

    OPEN_DISPS(play->state.gfxCtx);

    // Get digits for max number of stray fairies
    counterDigits[1] = STRAY_FAIRY_SCATTERED_TOTAL;
    counterDigits[0] = counterDigits[1] / 10;
    counterDigits[1] -= (s16)(counterDigits[0] * 10);

    // Draw max number of stray fairies
    for (rectLeft = 116, digitIndex = 0; digitIndex < 2; digitIndex++, rectLeft += 8) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, 255);

        POLY_OPA_DISP = Gfx_DrawTexRectI8(POLY_OPA_DISP, sCounterTextures[counterDigits[digitIndex]], 8, 16,
                                          rectLeft + 1, 146, 8, 16, 1 << 10, 1 << 10);

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

        gSPTextureRectangle(POLY_OPA_DISP++, rectLeft * 4, 145 << 2, (rectLeft + 8) * 4, 161 << 2, G_TX_RENDERTILE, 0,
                            0, 1 << 10, 1 << 10);
    }

    // Draw Counter slash
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, 255);

    POLY_OPA_DISP =
        Gfx_DrawTexRectI8(POLY_OPA_DISP, gStrayFairyMapCounterSlashTex, 8, 16, 107, 146, 8, 16, 1 << 10, 1 << 10);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

    gSPTextureRectangle(POLY_OPA_DISP++, 106 << 2, 145 << 2, 114 << 2, 161 << 2, G_TX_RENDERTILE, 0, 0, 1 << 10,
                        1 << 10);

    // Get digits for current number of stray fairies collected
    counterDigits[1] = gSaveContext.save.saveInfo.inventory.strayFairies[(void)0, gSaveContext.dungeonSceneSharedIndex];
    counterDigits[0] = counterDigits[1] / 10;
    counterDigits[1] -= (s16)(counterDigits[0] * 10);

    // Draw digits for current number of stray fairies collected
    for (rectLeft = 88, digitIndex = 0; digitIndex < 2; digitIndex++, rectLeft += 8) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, 255);

        POLY_OPA_DISP = Gfx_DrawTexRectI8(POLY_OPA_DISP, sCounterTextures[counterDigits[digitIndex]], 8, 16,
                                          rectLeft + 1, 146, 8, 16, 1 << 10, 1 << 10);

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

        gSPTextureRectangle(POLY_OPA_DISP++, rectLeft * 4, 145 << 2, (rectLeft + 8) * 4, 161 << 2, G_TX_RENDERTILE, 0,
                            0, 1 << 10, 1 << 10);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

TexturePtr sDungeonItemTextures[] = {
    gQuestIconBossKeyTex,    // DUNGEON_BOSS_KEY
    gQuestIconCompassTex,    // DUNGEON_COMPASS
    gQuestIconDungeonMapTex, // DUNGEON_MAP
};

TexturePtr sDungeonTitleTextures[] = {
    gPauseWoodfallTitleENGTex,   // DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE
    gPauseSnowheadTitleENGTex,   // DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE
    gPauseGreatBayTitleENGTex,   // DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE
    gPauseStoneTowerTitleENGTex, // DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE
};

s16 sDungeonMapFloorIconPosY[] = { 67, 81, 95, 109, 123 };

void KaleidoScope_DrawDungeonMap(PlayState* play) {
    static s16 sStrayFairyIconTimer = 30;
    static s16 sStrayFairyIconIndex = 0;
    static s16 sStrayFairyIconAlphaScaleTimer = 15;
    static s16 sStrayFairyIconAlphaScaleState = 0;
    static s16 sStrayFairyIconAlpha = 255;
    static f32 sStrayFairyIconScale = 100.0f;
    static TexturePtr sStrayFairyIconTextures[][4] = {
        // DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE
        { gStrayFairyWoodfallIconTex, gDungeonStrayFairyWoodfallIconTex, gStrayFairyWoodfallIconTex,
          gDungeonStrayFairyWoodfallIconTex },
        // DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE
        { gStrayFairySnowheadIconTex, gDungeonStrayFairySnowheadIconTex, gStrayFairySnowheadIconTex,
          gDungeonStrayFairySnowheadIconTex },
        // DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE
        { gStrayFairyGreatBayIconTex, gDungeonStrayFairyGreatBayIconTex, gStrayFairyGreatBayIconTex,
          gDungeonStrayFairyGreatBayIconTex },
        // DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE
        { gStrayFairyStoneTowerIconTex, gDungeonStrayFairyStoneTowerIconTex, gStrayFairyStoneTowerIconTex,
          gDungeonStrayFairyStoneTowerIconTex },
    };
    static u8 sStrayFairyIconPrimColors[][3] = {
        { 255, 110, 160 }, // DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE
        { 90, 255, 100 },  // DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE
        { 120, 255, 255 }, // DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE
        { 245, 245, 90 },  // DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE
    };
    static u8 sStrayFairyIconEnvColors[][3] = {
        { 255, 255, 255 }, // DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE
        { 255, 255, 255 }, // DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE
        { 255, 255, 255 }, // DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE
        { 225, 170, 0 },   // DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE
    };
    static s32 sStrayFairyIconRectS[] = {
        1 << 10, // mirror texture horizontally
        0,       // default
        0,       // default
        0        // default
    };
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 i;
    s16 j;
    f32 scale;

    OPEN_DISPS(play->state.gfxCtx);

    j = (QUAD_MAP_PAGE_DUNGEON_MAP + pauseCtx->cursorSlot[PAUSE_MAP]) * 4;
    KaleidoScope_SetCursorVtxPos(pauseCtx, j, pauseCtx->mapPageVtx);

    pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
    if (pauseCtx->cursorSpecialPos == 0) {
        if (pauseCtx->cursorPoint[PAUSE_MAP] <= DUNGEON_STRAY_FAIRIES) {
            pauseCtx->cursorItem[PAUSE_MAP] = ITEM_KEY_BOSS + pauseCtx->cursorPoint[PAUSE_MAP];
        }
        pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_MAP];
    }

    // Set vertices for:
    // QUAD_MAP_PAGE_DUNGEON_TITLE,
    // QUAD_MAP_PAGE_DUNGEON_BOSS_KEY,
    // QUAD_MAP_PAGE_DUNGEON_COMPASS,
    // QUAD_MAP_PAGE_DUNGEON_MAP
    gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[QUAD_MAP_PAGE_DUNGEON_TITLE * 4], 4 * 4, 0);

    // Dungeon Title
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA, G_CC_MODULATEIA);

    // QUAD_MAP_PAGE_DUNGEON_TITLE
    POLY_OPA_DISP = Gfx_DrawTexQuadIA8(
        POLY_OPA_DISP, sDungeonTitleTextures[((void)0, gSaveContext.dungeonSceneSharedIndex)], 128, 16, 0);

    gDPPipeSync(POLY_OPA_DISP++);

    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    // Loop over dungeonItems (i) and vtxIndex (j)
    for (i = DUNGEON_BOSS_KEY, j = 4; i <= DUNGEON_STRAY_FAIRIES; i++, j += 4) {
        if (i == DUNGEON_STRAY_FAIRIES) {
            if ((pauseCtx->pageIndex == PAUSE_MAP) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE)) {
                // If (pauseCtx->state == PAUSE_STATE_MAIN), then the other conditions are redundant and
                // always return true
                if ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->state != PAUSE_STATE_SAVEPROMPT) &&
                    !IS_PAUSE_STATE_GAMEOVER(pauseCtx)) {
                    KaleidoScope_SetView(pauseCtx, 0.0f, 0.0f, 64.0f);

                    if (!sStrayFairyIconAlphaScaleState) {
                        sStrayFairyIconAlpha -= 6;
                        sStrayFairyIconScale -= 1.0f;
                    } else {
                        sStrayFairyIconAlpha += 6;
                        sStrayFairyIconScale += 1.0f;
                    }

                    sStrayFairyIconAlphaScaleTimer--;
                    if (sStrayFairyIconAlphaScaleTimer == 0) {
                        sStrayFairyIconAlphaScaleState ^= 1;
                        sStrayFairyIconAlphaScaleTimer = 15;
                    }

                    Gfx_SetupDL42_Opa(play->state.gfxCtx);

                    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0,
                                      PRIMITIVE, 0, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE,
                                      0);

                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0,
                                    sStrayFairyIconPrimColors[((void)0, gSaveContext.dungeonSceneSharedIndex)][0],
                                    sStrayFairyIconPrimColors[((void)0, gSaveContext.dungeonSceneSharedIndex)][1],
                                    sStrayFairyIconPrimColors[((void)0, gSaveContext.dungeonSceneSharedIndex)][2],
                                    sStrayFairyIconAlpha);
                    gDPSetEnvColor(POLY_OPA_DISP++,
                                   sStrayFairyIconEnvColors[((void)0, gSaveContext.dungeonSceneSharedIndex)][0],
                                   sStrayFairyIconEnvColors[((void)0, gSaveContext.dungeonSceneSharedIndex)][1],
                                   sStrayFairyIconEnvColors[((void)0, gSaveContext.dungeonSceneSharedIndex)][2], 0);

                    scale = sStrayFairyIconScale / 100.0f;

                    Matrix_Translate(-83.0f, -29.0f, -128.0f, MTXMODE_NEW);
                    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);

                    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

                    // QUAD_MAP_PAGE_DUNGEON_STRAY_FAIRY_GLOWING_CIRCLE
                    pauseCtx->mapPageVtx[76].v.ob[0] = pauseCtx->mapPageVtx[78].v.ob[0] = -16;

                    pauseCtx->mapPageVtx[77].v.ob[0] = pauseCtx->mapPageVtx[79].v.ob[0] =
                        pauseCtx->mapPageVtx[76].v.ob[0] + 32;

                    pauseCtx->mapPageVtx[76].v.ob[1] = pauseCtx->mapPageVtx[77].v.ob[1] = 12;

                    pauseCtx->mapPageVtx[78].v.ob[1] = pauseCtx->mapPageVtx[79].v.ob[1] =
                        pauseCtx->mapPageVtx[76].v.ob[1] - 24;

                    gSPVertex(POLY_OPA_DISP++,
                              &pauseCtx->mapPageVtx[QUAD_MAP_PAGE_DUNGEON_STRAY_FAIRY_GLOWING_CIRCLE * 4], 4, 0);

                    POLY_OPA_DISP =
                        Gfx_DrawTexQuad4b(POLY_OPA_DISP, gStrayFairyGlowingCircleIconTex, G_IM_FMT_I, 32, 24, 0);
                    KaleidoScope_SetView(pauseCtx, pauseCtx->eye.x, pauseCtx->eye.y, pauseCtx->eye.z);
                    Gfx_SetupDL39_Opa(play->state.gfxCtx);

                    gDPPipeSync(POLY_OPA_DISP++);

                    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, sStrayFairyIconAlpha);

                    sStrayFairyIconTimer--;

                    if (sStrayFairyIconTimer == 0) {
                        sStrayFairyIconIndex = (sStrayFairyIconIndex + 1) & 3;
                        sStrayFairyIconTimer = 34;
                    }

                    gDPLoadTextureBlock(
                        POLY_OPA_DISP++,
                        sStrayFairyIconTextures[((void)0, gSaveContext.dungeonSceneSharedIndex)][sStrayFairyIconIndex],
                        G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 24, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5,
                        G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                    gSPTextureRectangle(POLY_OPA_DISP++, 54 << 2, 140 << 2, 86 << 2, 164 << 2, G_TX_RENDERTILE,
                                        sStrayFairyIconRectS[sStrayFairyIconIndex], 0, 1 << 10, 1 << 10);

                    KaleidoScope_DrawDungeonStrayFairyCount(play);
                    Gfx_SetupDL42_Opa(play->state.gfxCtx);
                }
            }
        } else if (CHECK_DUNGEON_ITEM(i, gSaveContext.dungeonSceneSharedIndex)) {
            gDPLoadTextureBlock(POLY_OPA_DISP++, sDungeonItemTextures[i], G_IM_FMT_RGBA, G_IM_SIZ_32b, 24, 24, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);

            // QUAD_MAP_PAGE_DUNGEON_BOSS_KEY
            // QUAD_MAP_PAGE_DUNGEON_COMPASS
            // QUAD_MAP_PAGE_DUNGEON_MAP
            gSP1Quadrangle(POLY_OPA_DISP++, j, j + 2, j + 3, j + 1, 0);
        }
    }

    MapDisp_DrawDungeonFloorSelect(play);

    if ((pauseCtx->pageIndex == PAUSE_MAP) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE)) {
        // If (pauseCtx->state == PAUSE_STATE_MAIN), then the other conditions are redundant and always return
        // true
        if ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->state != PAUSE_STATE_SAVEPROMPT) &&
            !IS_PAUSE_STATE_GAMEOVER(pauseCtx)) {

            Gfx_SetupDL39_Opa(play->state.gfxCtx);

            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

            // Draw Player's face next to the dungeon floor icon currently in.
            POLY_OPA_DISP =
                Gfx_DrawTexRectRGBA16(POLY_OPA_DISP, gQuestIconLinkHumanFaceTex, 16, 16, 62,
                                      sDungeonMapFloorIconPosY[R_PLAYER_FLOOR_REVERSE_INDEX], 16, 16, 1 << 10, 1 << 10);

            // Draw skull face next to the dungeon floor icon the boss is located at.
            if (CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, gSaveContext.dungeonSceneSharedIndex)) {
                POLY_OPA_DISP = Gfx_DrawTexRectRGBA16(
                    POLY_OPA_DISP, gDungeonMapSkullTex, 16, 16, 108,
                    sDungeonMapFloorIconPosY[FLOOR_INDEX_MAX - MapDisp_GetBossRoomStorey()], 16, 16, 1 << 10, 1 << 10);
            }

            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

            Gfx_SetupDL42_Opa(play->state.gfxCtx);
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);

    gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_BILERP);

    CLOSE_DISPS(play->state.gfxCtx);
}

void KaleidoScope_UpdateDungeonCursor(PlayState* play) {
    s32 pad;
    PauseContext* pauseCtx = &play->pauseCtx;
    MessageContext* msgCtx = &play->msgCtx;
    s16 i;
    s16 oldCursorPoint;

    if (pauseCtx->state == PAUSE_STATE_MAIN) {
        if ((pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) && (pauseCtx->pageIndex == PAUSE_MAP)) {
            pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_WHITE;
            oldCursorPoint = pauseCtx->cursorPoint[PAUSE_MAP];
            if (pauseCtx->stickAdjX > 30) {
                pauseCtx->cursorShrinkRate = 4.0f;
                if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
                    KaleidoScope_MoveCursorFromSpecialPos(play);
                    pauseCtx->cursorXIndex[PAUSE_MAP] = 0;
                    pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorMapDungeonItem;
                    pauseCtx->cursorPoint[PAUSE_MAP] = pauseCtx->cursorMapDungeonItem;
                } else if (pauseCtx->cursorSpecialPos == 0) {
                    if (pauseCtx->cursorXIndex[PAUSE_MAP] == 0) {
                        pauseCtx->cursorXIndex[PAUSE_MAP] = 1;
                        pauseCtx->cursorPoint[PAUSE_MAP] = DUNGEON_STRAY_FAIRIES;
                    } else {
                        if (pauseCtx->cursorPoint[PAUSE_MAP] == DUNGEON_STRAY_FAIRIES) {
                            pauseCtx->cursorPoint[PAUSE_MAP] = DUNGEON_NONE;
                        }

                        while (true) {
                            pauseCtx->cursorPoint[PAUSE_MAP]++;
                            if (pauseCtx->cursorPoint[PAUSE_MAP] == DUNGEON_STRAY_FAIRIES) {
                                KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);
                                break;
                            }
                            if (CHECK_DUNGEON_ITEM(pauseCtx->cursorPoint[PAUSE_MAP],
                                                   gSaveContext.dungeonSceneSharedIndex)) {
                                break;
                            }
                        }
                    }
                }
            } else if (pauseCtx->stickAdjX < -30) {
                pauseCtx->cursorShrinkRate = 4.0f;
                if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_RIGHT) {
                    KaleidoScope_MoveCursorFromSpecialPos(play);
                    pauseCtx->cursorXIndex[PAUSE_MAP] = 1;
                    pauseCtx->cursorPoint[PAUSE_MAP] = DUNGEON_MAP;
                    if (!CHECK_DUNGEON_ITEM(DUNGEON_MAP, gSaveContext.dungeonSceneSharedIndex)) {
                        pauseCtx->cursorPoint[PAUSE_MAP]--; // DUNGEON_COMPASS
                        if (!CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, gSaveContext.dungeonSceneSharedIndex)) {
                            pauseCtx->cursorPoint[PAUSE_MAP]--; // DUNGEON_BOSS_KEY
                            if (!CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, gSaveContext.dungeonSceneSharedIndex)) {
                                pauseCtx->cursorSlot[PAUSE_MAP] = DUNGEON_STRAY_FAIRIES;
                                pauseCtx->cursorPoint[PAUSE_MAP] = DUNGEON_STRAY_FAIRIES;
                            }
                        }
                    } else {
                        pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_MAP];
                    }
                } else if (pauseCtx->cursorSpecialPos == 0) {
                    if (pauseCtx->cursorXIndex[PAUSE_MAP] == 0) {
                        KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);
                    } else if (pauseCtx->cursorPoint[PAUSE_MAP] == DUNGEON_STRAY_FAIRIES) {
                        pauseCtx->cursorXIndex[PAUSE_MAP] = 0;
                        pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorMapDungeonItem;
                        pauseCtx->cursorPoint[PAUSE_MAP] = pauseCtx->cursorMapDungeonItem;
                    } else {
                        while (true) {
                            pauseCtx->cursorPoint[PAUSE_MAP]--;
                            if (pauseCtx->cursorPoint[PAUSE_MAP] <= DUNGEON_NONE) {
                                pauseCtx->cursorSlot[PAUSE_MAP] = DUNGEON_STRAY_FAIRIES;
                                pauseCtx->cursorPoint[PAUSE_MAP] = DUNGEON_STRAY_FAIRIES;
                                break;
                            }
                            if (CHECK_DUNGEON_ITEM(pauseCtx->cursorPoint[PAUSE_MAP],
                                                   gSaveContext.dungeonSceneSharedIndex)) {
                                break;
                            }
                        }
                    }
                }
            } else if ((pauseCtx->cursorSpecialPos == 0) && (pauseCtx->stickAdjY > 30)) {
                if (pauseCtx->cursorPoint[PAUSE_MAP] >= DUNGEON_FLOOR_INDEX_4) {
                    for (i = pauseCtx->cursorPoint[PAUSE_MAP] - (DUNGEON_FLOOR_INDEX_4 + 1); i >= 0; i--) {
                        if (GET_DUNGEON_FLOOR_VISITED(((void)0, gSaveContext.dungeonSceneSharedIndex), i) ||
                            MapDisp_IsValidStorey(FLOOR_INDEX_MAX - i)) {
                            pauseCtx->cursorPoint[PAUSE_MAP] = i + DUNGEON_FLOOR_INDEX_4;
                            pauseCtx->cursorShrinkRate = 4.0f;
                            break;
                        }
                    }
                } else if (pauseCtx->cursorPoint[PAUSE_MAP] == DUNGEON_STRAY_FAIRIES) {
                    pauseCtx->cursorXIndex[PAUSE_MAP] = 0;
                    pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorMapDungeonItem;
                    pauseCtx->cursorPoint[PAUSE_MAP] = pauseCtx->cursorMapDungeonItem;
                } else {
                    pauseCtx->cursorSlot[PAUSE_MAP] = DUNGEON_STRAY_FAIRIES;
                    pauseCtx->cursorPoint[PAUSE_MAP] = DUNGEON_STRAY_FAIRIES;
                }
            } else if ((pauseCtx->cursorSpecialPos == 0) && (pauseCtx->stickAdjY < -30)) {
                if ((pauseCtx->cursorPoint[PAUSE_MAP] >= DUNGEON_FLOOR_INDEX_4) &&
                    (pauseCtx->cursorPoint[PAUSE_MAP] <= DUNGEON_FLOOR_INDEX_1)) {
                    for (i = pauseCtx->cursorPoint[PAUSE_MAP] - (DUNGEON_FLOOR_INDEX_4 - 1); i <= DUNGEON_FLOOR_INDEX_0;
                         i++) {
                        if (GET_DUNGEON_FLOOR_VISITED(((void)0, gSaveContext.dungeonSceneSharedIndex), i) ||
                            MapDisp_IsValidStorey(FLOOR_INDEX_MAX - i)) {
                            pauseCtx->cursorPoint[PAUSE_MAP] = i + DUNGEON_FLOOR_INDEX_4;
                            pauseCtx->cursorShrinkRate = 4.0f;
                            break;
                        }
                    }

                } else if (pauseCtx->cursorXIndex[PAUSE_MAP] == 0) {
                    pauseCtx->cursorXIndex[PAUSE_MAP] = 1;
                    pauseCtx->cursorPoint[PAUSE_MAP] = DUNGEON_STRAY_FAIRIES;
                } else {
                    if (pauseCtx->cursorPoint[PAUSE_MAP] == DUNGEON_STRAY_FAIRIES) {
                        pauseCtx->cursorPoint[PAUSE_MAP] = DUNGEON_NONE;
                    }

                    while (true) {
                        pauseCtx->cursorPoint[PAUSE_MAP]++;
                        if (pauseCtx->cursorPoint[PAUSE_MAP] == DUNGEON_FLOOR_INDEX_4) {
                            KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);
                            break;
                        }
                        if (CHECK_DUNGEON_ITEM(pauseCtx->cursorPoint[PAUSE_MAP],
                                               gSaveContext.dungeonSceneSharedIndex)) {
                            break;
                        }
                    }
                }
            }
            if ((pauseCtx->cursorPoint[PAUSE_MAP] <= DUNGEON_STRAY_FAIRIES) && (pauseCtx->cursorSpecialPos == 0)) {
                if (gSaveContext.buttonStatus[EQUIP_SLOT_A] == BTN_DISABLED) {
                    gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_ENABLED;
                    gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
                    Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
                }
            } else if (gSaveContext.buttonStatus[EQUIP_SLOT_A] != BTN_DISABLED) {
                gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_DISABLED;
                gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
                Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
            }

            if ((pauseCtx->cursorXIndex[PAUSE_MAP] == 0) && (pauseCtx->cursorSpecialPos == 0)) {
                pauseCtx->cursorMapDungeonItem = pauseCtx->cursorPoint[PAUSE_MAP];
            }

            if (pauseCtx->cursorSpecialPos == 0) {
                if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A) && (msgCtx->msgLength == 0) &&
                    (pauseCtx->cursorPoint[PAUSE_MAP] == DUNGEON_STRAY_FAIRIES)) {
                    pauseCtx->itemDescriptionOn = true;
                    func_801514B0(play, 0x17AF, 1);
                } else if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A) && (msgCtx->msgLength == 0) &&
                           CHECK_DUNGEON_ITEM(pauseCtx->cursorPoint[PAUSE_MAP], gSaveContext.dungeonSceneSharedIndex)) {
                    pauseCtx->itemDescriptionOn = true;
                    func_801514B0(play, 0x17AC + pauseCtx->cursorPoint[PAUSE_MAP], 1);
                }
            }

            if (oldCursorPoint != pauseCtx->cursorPoint[PAUSE_MAP]) {
                Audio_PlaySfx(NA_SE_SY_CURSOR);
            }
        }
    }
}

TexturePtr sCloudTextures[] = {
    gWorldMapClockTownCloud1Tex,  // TINGLE_MAP_CLOCK_TOWN
    gWorldMapClockTownCloud2Tex,  // TINGLE_MAP_CLOCK_TOWN
    gWorldMapWoodfallCloud1Tex,   // TINGLE_MAP_WOODFALL
    gWorldMapWoodfallCloud2Tex,   // TINGLE_MAP_WOODFALL
    gWorldMapWoodfallCloud3Tex,   // TINGLE_MAP_WOODFALL
    gWorldMapSnowheadCloud1Tex,   // TINGLE_MAP_SNOWHEAD
    gWorldMapSnowheadCloud2Tex,   // TINGLE_MAP_SNOWHEAD
    gWorldMapSnowheadCloud3Tex,   // TINGLE_MAP_SNOWHEAD
    gWorldMapRomaniRanchCloudTex, // TINGLE_MAP_ROMANI_RANCH
    gWorldMapGreatBayCloud1Tex,   // TINGLE_MAP_GREAT_BAY
    gWorldMapGreatBayCloud2Tex,   // TINGLE_MAP_GREAT_BAY
    gWorldMapGreatBayCloud3Tex,   // TINGLE_MAP_GREAT_BAY
    gWorldMapGreatBayCloud4Tex,   // TINGLE_MAP_GREAT_BAY
    gWorldMapStoneTowerCloud1Tex, // TINGLE_MAP_STONE_TOWER
    gWorldMapStoneTowerCloud2Tex, // TINGLE_MAP_STONE_TOWER
};

s16 sWorldMapDotPrimColors[][3] = {
    { 0, 0, 255 },
    { 255, 255, 0 },
};

s16 sWorldMapDotEnvColors[][3] = {
    { 255, 255, 0 },
    { 0, 0, 255 },
};

s16 sWorldMapCursorsRectLeft[REGION_MAX] = {
    86,  // REGION_GREAT_BAY
    104, // REGION_ZORA_HALL
    145, // REGION_ROMANI_RANCH
    153, // REGION_DEKU_PALACE
    163, // REGION_WOODFALL
    159, // REGION_CLOCK_TOWN
    157, // REGION_SNOWHEAD
    199, // REGION_IKANA_GRAVEYARD
    208, // REGION_IKANA_CANYON
    210, // REGION_GORON_VILLAGE
    218, // REGION_STONE_TOWER
};

s16 sWorldMapCursorsRectTop[REGION_MAX] = {
    127, // REGION_GREAT_BAY
    153, // REGION_ZORA_HALL
    138, // REGION_ROMANI_RANCH
    171, // REGION_DEKU_PALACE
    146, // REGION_WOODFALL
    119, // REGION_CLOCK_TOWN
    77,  // REGION_SNOWHEAD
    106, // REGION_IKANA_GRAVEYARD
    120, // REGION_IKANA_CANYON
    73,  // REGION_GORON_VILLAGE
    99,  // REGION_STONE_TOWER
};

s16 sGreatFairySpawnRegions[] = {
    REGION_CLOCK_TOWN, REGION_WOODFALL, REGION_SNOWHEAD, REGION_GREAT_BAY, REGION_IKANA_CANYON,
    REGION_CLOCK_TOWN, REGION_WOODFALL, REGION_SNOWHEAD, REGION_GREAT_BAY, REGION_IKANA_CANYON,
};

void KaleidoScope_DrawWorldMap(PlayState* play) {
    s16 sceneId;
    s16 t;
    s16 n;
    s16 j;
    s16 k;
    s16 i;
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 rectLeft;
    s16 rectRight;

    OPEN_DISPS(play->state.gfxCtx);

    // Use matrix scaling to flip the entire overworld map for mirror world
    if (CVarGetInteger("gModes.MirroredWorld.State", 0)) {
        // Invert culling to counter act the matrix flip
        gSPSetExtraGeometryMode(POLY_OPA_DISP++, G_EX_INVERT_CULLING);
        Matrix_Push();
        // We need to shift the matrix a little to the left before flipping it so that it aligns in the same spot
        Matrix_Translate(-2.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_Scale(-1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        Matrix_Pop();
    }

    KaleidoScope_SetCursorVtxPos(pauseCtx, pauseCtx->cursorSlot[PAUSE_MAP] * 4, pauseCtx->mapPageVtx);

    // Draw the world map image
    if ((pauseCtx->pageIndex == PAUSE_MAP) && (pauseCtx->state == PAUSE_STATE_MAIN) &&
        ((pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) || (pauseCtx->mainState == PAUSE_MAIN_STATE_EQUIP_ITEM)) &&
        YREG(6) && (pauseCtx->state != PAUSE_STATE_SAVEPROMPT) && !IS_PAUSE_STATE_GAMEOVER(pauseCtx)) {

        // Draw the world map image flat
        // Because it is flat, the texture is loaded by filling it in 8 rows at a time.
        // 8 is chosen because it is smaller than `TMEM_SIZE / 2 / textureWidth` and divides the texture's height.
        // Each loaded chunk must have `size <= TMEM_SIZE / 2`
        // because the texture is color-indexed, so the TLUT uses the other half of TMEM.

        Gfx_SetupDL39_Opa(play->state.gfxCtx);

        gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_POINT);
        gDPLoadTLUT_pal256(POLY_OPA_DISP++, gWorldMapImageTLUT);
        gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_RGBA16);

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

        // Process the 128 rows of pixels for gWorldMapImageTex, 8 rows at a time over 16 iterations
        // Loop over yPos (t), textureIndex (j)
        for (t = 62, j = 0; j < 16; j++, t += 8) {
            // 2S2H [Port][HD Textures] Here and below we use LoadMultTile and SetTileSize to allow us to pass the
            // resource path to the renderer and calculate the tile offset after the renderer loads it
            /*
            gDPLoadTextureBlock(POLY_OPA_DISP++, (u8*)gWorldMapImageTex + j * (WORLD_MAP_IMAGE_WIDTH * 8), G_IM_FMT_CI,
                                G_IM_SIZ_8b, WORLD_MAP_IMAGE_WIDTH, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            */
            gDPLoadMultiTile(POLY_OPA_DISP++, gWorldMapImageTex, 0, G_TX_RENDERTILE, G_IM_FMT_CI, G_IM_SIZ_8b,
                             WORLD_MAP_IMAGE_WIDTH, WORLD_MAP_IMAGE_HEIGHT, 0, j * 8, WORLD_MAP_IMAGE_WIDTH - 1,
                             (j + 1) * 8 - 1, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                             G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gDPSetTileSize(POLY_OPA_DISP++, G_TX_RENDERTILE, 0, 0, (WORLD_MAP_IMAGE_WIDTH - 1) << G_TEXTURE_IMAGE_FRAC,
                           (8 - 1) << G_TEXTURE_IMAGE_FRAC);

            rectLeft = 51 << 2;
            rectRight = rectLeft + (WORLD_MAP_IMAGE_WIDTH << 2);
            gSPTextureRectangle(POLY_OPA_DISP++, rectLeft, t << 2, rectRight, (t << 2) + (8 << 2), G_TX_RENDERTILE, 0,
                                0, 1 << 10, 1 << 10);
        }

        Gfx_SetupDL42_Opa(play->state.gfxCtx);

    } else {
        // Draw the world map angled
        // Because it is at an angle, vertices are used to place it.
        // The structure of the loops here is to satisfy the constraints of both TMEM and the size of the vertex cache.
        // - Each loop iteration loads 9 rows, because 9 is the largest number smaller than
        //   `TMEM_SIZE / 2 / textureWidth`.
        // - Each loop is at most 8 iterations long because each row uses 4 vertices and the vertex cache has size
        //   `32 = 8 * 4`.
        // .
        // Hence there is one loop of length 8, one of length 6, and then the remaining `128 - (8 + 6) * 9 = 2` rows are
        // drawn at the end.

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_POINT);
        gDPLoadTLUT_pal256(POLY_OPA_DISP++, gWorldMapImageTLUT);
        gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_RGBA16);

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

        // Set the vertices for the first 8 quads attached to the world map texture.
        gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[QUAD_MAP_PAGE_WORLD_IMAGE_FIRST * 4], 8 * 4, 0);

        // Process the first 72 rows of pixels for gWorldMapImageTex, 9 rows at a time over 8 iterations
        // Loop over quadIndex of this loop (i), quadIndex of the entire texture (k), vtxIndex (j)
        for (i = 0, k = 0, j = 0; i < 8; i++, k++, j += 4) {
            /*
            gDPLoadTextureBlock(
                POLY_OPA_DISP++, (u8*)gWorldMapImageTex + k * (WORLD_MAP_IMAGE_WIDTH * WORLD_MAP_IMAGE_FRAG_HEIGHT),
                G_IM_FMT_CI, G_IM_SIZ_8b, WORLD_MAP_IMAGE_WIDTH, WORLD_MAP_IMAGE_FRAG_HEIGHT, 0,
                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
             */
            gDPLoadMultiTile(POLY_OPA_DISP++, gWorldMapImageTex, 0, G_TX_RENDERTILE, G_IM_FMT_CI, G_IM_SIZ_8b,
                             WORLD_MAP_IMAGE_WIDTH, WORLD_MAP_IMAGE_HEIGHT, 0, k * 9, WORLD_MAP_IMAGE_WIDTH - 1,
                             (k + 1) * 9 - 1, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                             G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gDPSetTileSize(POLY_OPA_DISP++, G_TX_RENDERTILE, 0, 0, (WORLD_MAP_IMAGE_WIDTH - 1) << G_TEXTURE_IMAGE_FRAC,
                           (9 - 1) << G_TEXTURE_IMAGE_FRAC);

            gSP1Quadrangle(POLY_OPA_DISP++, j, j + 2, j + 3, j + 1, 0);
        }

        // Set the vertices for the last 7 quads attached to the world map texture:
        // 6 quads with a height of 9, 1 quad with a height of 2
        gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[(QUAD_MAP_PAGE_WORLD_IMAGE_FIRST + 8) * 4], (6 + 1) * 4, 0);

        // Process the next 54 rows of pixels for gWorldMapImageTex, 9 rows at a time over 6 iterations
        // Loop over quadIndex of this loop (i), quadIndex of the entire texture (k), vtxIndex (j)
        for (i = 0, j = 0; i < 6; i++, k++, j += 4) {
            /*
            gDPLoadTextureBlock(
                POLY_OPA_DISP++, (u8*)gWorldMapImageTex + k * (WORLD_MAP_IMAGE_WIDTH * WORLD_MAP_IMAGE_FRAG_HEIGHT),
                G_IM_FMT_CI, G_IM_SIZ_8b, WORLD_MAP_IMAGE_WIDTH, WORLD_MAP_IMAGE_FRAG_HEIGHT, 0,
                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            */
            gDPLoadMultiTile(POLY_OPA_DISP++, gWorldMapImageTex, 0, G_TX_RENDERTILE, G_IM_FMT_CI, G_IM_SIZ_8b,
                             WORLD_MAP_IMAGE_WIDTH, WORLD_MAP_IMAGE_HEIGHT, 0, k * 9, WORLD_MAP_IMAGE_WIDTH - 1,
                             (k + 1) * 9 - 1, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                             G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gDPSetTileSize(POLY_OPA_DISP++, G_TX_RENDERTILE, 0, 0, (WORLD_MAP_IMAGE_WIDTH - 1) << G_TEXTURE_IMAGE_FRAC,
                           (9 - 1) << G_TEXTURE_IMAGE_FRAC);

            gSP1Quadrangle(POLY_OPA_DISP++, j, j + 2, j + 3, j + 1, 0);
        }

        // Process the last 2 rows of pixels for gWorldMapImageTex
        /*
        gDPLoadTextureBlock(
            POLY_OPA_DISP++, (u8*)gWorldMapImageTex + k * (WORLD_MAP_IMAGE_WIDTH * WORLD_MAP_IMAGE_FRAG_HEIGHT),
            G_IM_FMT_CI, G_IM_SIZ_8b, WORLD_MAP_IMAGE_WIDTH, WORLD_MAP_IMAGE_HEIGHT % WORLD_MAP_IMAGE_FRAG_HEIGHT, 0,
            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        */
        gDPLoadMultiTile(POLY_OPA_DISP++, gWorldMapImageTex, 0, G_TX_RENDERTILE, G_IM_FMT_CI, G_IM_SIZ_8b,
                         WORLD_MAP_IMAGE_WIDTH, WORLD_MAP_IMAGE_HEIGHT, 0, k * 9, WORLD_MAP_IMAGE_WIDTH - 1,
                         (k * 9 + 2) - 1, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                         G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gDPSetTileSize(POLY_OPA_DISP++, G_TX_RENDERTILE, 0, 0, (WORLD_MAP_IMAGE_WIDTH - 1) << G_TEXTURE_IMAGE_FRAC,
                       (2 - 1) << G_TEXTURE_IMAGE_FRAC);

        gSP1Quadrangle(POLY_OPA_DISP++, j, j + 2, j + 3, j + 1, 0);
    }

    Gfx_SetupDL42_Opa(play->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);

    gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_BILERP);

    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 215, 235, 235, pauseCtx->alpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 40, 60, 100, 0);

    // Draw clouds over the world map
    // Iterate over cloud bits (n)
    for (n = 0; n < WORLD_MAP_NUM_CLOUDS; n++) {
        if (!(((void)0, gSaveContext.save.saveInfo.worldMapCloudVisibility) & gBitFlags[n])) {

            gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[(QUAD_MAP_PAGE_WORLD_CLOUDS_FIRST + n) * 4], 4, 0);

            POLY_OPA_DISP = Gfx_DrawTexQuadIA8(POLY_OPA_DISP, sCloudTextures[n], gVtxPageMapWorldQuadsWidth[n],
                                               gVtxPageMapWorldQuadsHeight[n], 0);
        }
    }

    if (IS_PAUSE_STATE_OWL_WARP(pauseCtx)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetRenderMode(POLY_OPA_DISP++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, R_PAUSE_OWL_WARP_ALPHA);
        gDPFillRectangle(POLY_OPA_DISP++, 50, 62, 270, 190);
    }

    Gfx_SetupDL42_Opa(play->state.gfxCtx);

    if (!IS_PAUSE_STATE_OWL_WARP(pauseCtx) && !PauseOwlWarp_IsOwlWarpEnabled()) {
        // Browsing the world map regions on the pause menu
        gDPLoadTextureBlock(POLY_OPA_DISP++, gWorldMapDotTex, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWorldMapDotPrimColors[0][0], sWorldMapDotPrimColors[0][1],
                        sWorldMapDotPrimColors[0][2], pauseCtx->alpha);
        gDPSetEnvColor(POLY_OPA_DISP++, sWorldMapDotEnvColors[0][0], sWorldMapDotEnvColors[0][1],
                       sWorldMapDotEnvColors[0][2], 0);

        if (R_PAUSE_DBG_MAP_CLOUD_ON) {
            gSaveContext.save.saveInfo.worldMapCloudVisibility |= (u16)~0x8000;

            // QUAD_MAP_PAGE_WORLD_REGION_FIRST
            pauseCtx->mapPageVtx[120].v.ob[0] = pauseCtx->mapPageVtx[122].v.ob[0] = R_PAUSE_DBG_MAP_CLOUD_X;

            pauseCtx->mapPageVtx[121].v.ob[0] = pauseCtx->mapPageVtx[123].v.ob[0] =
                pauseCtx->mapPageVtx[120].v.ob[0] + 8;

            pauseCtx->mapPageVtx[120].v.ob[1] = pauseCtx->mapPageVtx[121].v.ob[1] = R_PAUSE_DBG_MAP_CLOUD_Y;

            pauseCtx->mapPageVtx[122].v.ob[1] = pauseCtx->mapPageVtx[123].v.ob[1] =
                pauseCtx->mapPageVtx[120].v.ob[1] - 8;
        }

        // Loop over RegionId (i), unused vtxIndex (j), unused (k)
        for (i = 0, j = 0; i < REGION_MAX; i++, k++, j += 4) {
            if (pauseCtx->worldMapPoints[i]) {
                gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[(QUAD_MAP_PAGE_WORLD_REGION_FIRST + i) * 4], 4, 0);
                gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
            }
        }

    } else {
        // Selecting an owl warp
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPLoadTextureBlock(POLY_OPA_DISP++, gWorldMapOwlFaceTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 24, 12, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

        if (R_PAUSE_DBG_MAP_CLOUD_ON) {
            gSaveContext.save.saveInfo.worldMapCloudVisibility |= (u16)~0x8000;

            // QUAD_MAP_PAGE_WORLD_WARP_FIRST
            pauseCtx->mapPageVtx[164].v.ob[0] = pauseCtx->mapPageVtx[166].v.ob[0] = R_PAUSE_DBG_MAP_CLOUD_X;

            pauseCtx->mapPageVtx[165].v.ob[0] = pauseCtx->mapPageVtx[167].v.ob[0] =
                pauseCtx->mapPageVtx[164].v.ob[0] + 24;

            pauseCtx->mapPageVtx[164].v.ob[1] = pauseCtx->mapPageVtx[165].v.ob[1] = R_PAUSE_DBG_MAP_CLOUD_Y;

            pauseCtx->mapPageVtx[166].v.ob[1] = pauseCtx->mapPageVtx[167].v.ob[1] =
                pauseCtx->mapPageVtx[164].v.ob[1] - 12;
        }

        // Loop over OwlWarpId (i), unused vtxIndex (j), unused (k)
        for (i = 0, j = 0; i < OWL_WARP_ENTRANCE; i++, k++, j += 4) {
            if (pauseCtx->worldMapPoints[i]) {
                gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[(QUAD_MAP_PAGE_WORLD_WARP_FIRST + i) * 4], 4, 0);
                gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
            }
        }
    }

    // Find and draw Player's face at the current region based on the current scene
    // If (pauseCtx->state == PAUSE_STATE_MAIN), then the other pauseCtx->state conditions are redundant
    // and always return true
    if ((pauseCtx->pageIndex == PAUSE_MAP) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) &&
        (pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->state != PAUSE_STATE_SAVEPROMPT) &&
        !IS_PAUSE_STATE_GAMEOVER(pauseCtx)) {
        j = 0;
        n = 0;

        sceneId = play->sceneId;

        // Map grottos/shrines to sceneId's to be used in different regions
        if (sceneId == SCENE_KAKUSIANA) {
            if (play->roomCtx.curRoom.num == 5) {
                sceneId = SCENE_11GORONNOSATO;
            } else if ((play->roomCtx.curRoom.num == 6) || (play->roomCtx.curRoom.num == 8) ||
                       (play->roomCtx.curRoom.num == 12)) {
                sceneId = SCENE_22DEKUCITY;
            } else {
                sceneId = Entrance_GetSceneIdAbsolute(((void)0, gSaveContext.respawn[RESPAWN_MODE_UNK_3].entrance));
            }
        }

        // Find the region that player is currently in
        // Loop over region (n) and regionIndex (j)
        while (true) {
            if (gSceneIdsPerRegion[n][j] == 0xFFFF) {
                n++;
                j = 0;
                if (n == REGION_MAX) {
                    n = 0;

                    // Special case for fairy fountains
                    if (sceneId == SCENE_YOUSEI_IZUMI) {
                        j = play->curSpawn;
                        n = sGreatFairySpawnRegions[j];
                        break;
                    }

                    while (true) {
                        if (gSceneIdsPerRegion[n][j] == 0xFFFF) {
                            n++;
                            if (n == REGION_MAX) {
                                break;
                            }
                            j = 0;
                            if (Entrance_GetSceneIdAbsolute(
                                    ((void)0, gSaveContext.respawn[RESPAWN_MODE_UNK_3].entrance)) ==
                                gSceneIdsPerRegion[n][j]) {
                                break;
                            }
                        }
                        j++;
                    }
                    break;
                }
            }

            if (sceneId == gSceneIdsPerRegion[n][j]) {
                break;
            }
            j++;
        }

        // Draw Player's face at the current region
        if (n != REGION_MAX) {
            KaleidoScope_SetView(pauseCtx, pauseCtx->eye.x, pauseCtx->eye.y, pauseCtx->eye.z);
            Gfx_SetupDL39_Opa(play->state.gfxCtx);

            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

            if (CVarGetInteger("gModes.MirroredWorld.State", 0)) {
                POLY_OPA_DISP = Gfx_DrawTexRectRGBA16(POLY_OPA_DISP, gQuestIconLinkHumanFaceTex, 16, 16,
                                                      (SCREEN_WIDTH - sWorldMapCursorsRectLeft[n] - 16 - 1),
                                                      sWorldMapCursorsRectTop[n], 16, 16, 1 << 10, 1 << 10);
            } else {
                POLY_OPA_DISP = Gfx_DrawTexRectRGBA16(POLY_OPA_DISP, gQuestIconLinkHumanFaceTex, 16, 16,
                                                      sWorldMapCursorsRectLeft[n], sWorldMapCursorsRectTop[n], 16, 16,
                                                      1 << 10, 1 << 10);
            }
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);

    if (CVarGetInteger("gModes.MirroredWorld.State", 0)) {
        gSPClearExtraGeometryMode(POLY_OPA_DISP++, G_EX_INVERT_CULLING);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

u16 sOwlWarpPauseItems[] = {
    ITEM_MAP_POINT_GREAT_BAY_COAST,  // OWL_WARP_GREAT_BAY_COAST
    ITEM_MAP_POINT_ZORA_CAPE,        // OWL_WARP_ZORA_CAPE
    ITEM_MAP_POINT_SNOWHEAD,         // OWL_WARP_SNOWHEAD
    ITEM_MAP_POINT_MOUNTAIN_VILLAGE, // OWL_WARP_MOUNTAIN_VILLAGE
    ITEM_MAP_POINT_CLOCK_TOWN,       // OWL_WARP_CLOCK_TOWN
    ITEM_MAP_POINT_MILK_ROAD,        // OWL_WARP_MILK_ROAD
    ITEM_MAP_POINT_WOODFALL,         // OWL_WARP_WOODFALL
    ITEM_MAP_POINT_SOUTHERN_SWAMP,   // OWL_WARP_SOUTHERN_SWAMP
    ITEM_MAP_POINT_IKANA_CANYON,     // OWL_WARP_IKANA_CANYON
    ITEM_MAP_POINT_STONE_TOWER,      // OWL_WARP_STONE_TOWER
};

// 2S2H [Enhancement] Same as KaleidoScope_UpdateWorldMapCursor but with behavior and controls inverted to account for
// mirrored world being used
void Ship_UpdateWorldMapCursorMirrorWorld(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 oldCursorPoint;

    bool goingLeft = pauseCtx->stickAdjX < -30;
    bool goingRight = pauseCtx->stickAdjX > 30;

    if ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) &&
        (pauseCtx->pageIndex == PAUSE_MAP) && !PauseOwlWarp_IsOwlWarpEnabled()) {
        pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_WHITE;
        oldCursorPoint = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];

        if (gSaveContext.buttonStatus[EQUIP_SLOT_A] != BTN_DISABLED) {
            gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_DISABLED;
            gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
            Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
        }

        if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT && goingRight) {
            pauseCtx->cursorSpecialPos = 0;
            pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = REGION_MAX;
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        } else if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_RIGHT && goingLeft) {
            pauseCtx->cursorSpecialPos = 0;
            pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = REGION_NONE;
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }

        if (pauseCtx->cursorSpecialPos == 0 && (goingLeft || goingRight)) {
            pauseCtx->cursorShrinkRate = 4.0f;
            goingLeft = !goingLeft;
            goingRight = !goingRight;
        }

        if (goingRight) {
            do {
                pauseCtx->cursorPoint[PAUSE_WORLD_MAP]++;
                if (pauseCtx->cursorPoint[PAUSE_WORLD_MAP] >= REGION_MAX) {
                    KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);
                    pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
                    break;
                }
            } while (!pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]);
        } else if (goingLeft) {
            do {
                pauseCtx->cursorPoint[PAUSE_WORLD_MAP]--;
                if (pauseCtx->cursorPoint[PAUSE_WORLD_MAP] <= REGION_NONE) {
                    KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);
                    pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
                    break;
                }
            } while (!pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]);
        }

        if (pauseCtx->cursorSpecialPos == 0) {
            pauseCtx->cursorItem[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
            // Used as cursor vtxIndex
            pauseCtx->cursorSlot[PAUSE_MAP] = 31 + pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
        }

        if (!pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]) {
            pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
        }
        if (oldCursorPoint != pauseCtx->cursorPoint[PAUSE_WORLD_MAP]) {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    } else if (GameInteractor_Should(VB_OWL_WARP_MENU_USE_LINEAR_CURSOR,
                                     pauseCtx->state == PAUSE_STATE_OWL_WARP_SELECT)) {
        pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_BLUE;
        oldCursorPoint = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];

        if (goingLeft || goingRight) {
            pauseCtx->cursorShrinkRate = 4.0f;
            goingLeft = !goingLeft;
            goingRight = !goingRight;
        }

        if (goingRight) {
            do {
                SHIP_HANDLE_OWL_CURSOR_INF_LOOP();
                pauseCtx->cursorPoint[PAUSE_WORLD_MAP]++;
                if (pauseCtx->cursorPoint[PAUSE_WORLD_MAP] > OWL_WARP_STONE_TOWER) {
                    pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = OWL_WARP_GREAT_BAY_COAST;
                }
            } while (!pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]);
        } else if (goingLeft) {
            do {
                SHIP_HANDLE_OWL_CURSOR_INF_LOOP();
                pauseCtx->cursorPoint[PAUSE_WORLD_MAP]--;
                if (pauseCtx->cursorPoint[PAUSE_WORLD_MAP] < OWL_WARP_GREAT_BAY_COAST) {
                    pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = OWL_WARP_STONE_TOWER;
                }
            } while (!pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]);
        }

        // Offset from `ITEM_MAP_POINT_GREAT_BAY` is to get the correct ordering in `map_name_static`
        pauseCtx->cursorItem[PAUSE_MAP] =
            sOwlWarpPauseItems[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]] - ITEM_MAP_POINT_GREAT_BAY;
        // Used as cursor vtxIndex
        pauseCtx->cursorSlot[PAUSE_MAP] = 31 + pauseCtx->cursorPoint[PAUSE_WORLD_MAP];

        if (oldCursorPoint != pauseCtx->cursorPoint[PAUSE_WORLD_MAP]) {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    }
}

void KaleidoScope_UpdateWorldMapCursor(PlayState* play) {
    static u16 sStickAdjTimer = 0; // unused timer that counts up every frame. Resets on reading a stickAdj.
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 oldCursorPoint;

    if (CVarGetInteger("gModes.MirroredWorld.State", 0)) {
        Ship_UpdateWorldMapCursorMirrorWorld(play);
        return;
    }

    if ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) &&
        (pauseCtx->pageIndex == PAUSE_MAP) && !PauseOwlWarp_IsOwlWarpEnabled()) {
        pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_WHITE;
        oldCursorPoint = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];

        if (gSaveContext.buttonStatus[EQUIP_SLOT_A] != BTN_DISABLED) {
            gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_DISABLED;
            gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
            Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
        }

        if (pauseCtx->cursorSpecialPos == 0) {
            if (pauseCtx->stickAdjX > 30) {
                pauseCtx->cursorShrinkRate = 4.0f;
                sStickAdjTimer = 0;

                while (true) {
                    pauseCtx->cursorPoint[PAUSE_WORLD_MAP]++;
                    if (pauseCtx->cursorPoint[PAUSE_WORLD_MAP] >= REGION_MAX) {
                        KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);
                        pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
                        break;
                    }
                    if (pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]) {
                        break;
                    }
                }
            } else if (pauseCtx->stickAdjX < -30) {
                pauseCtx->cursorShrinkRate = 4.0f;
                sStickAdjTimer = 0;

                while (true) {
                    pauseCtx->cursorPoint[PAUSE_WORLD_MAP]--;
                    if (pauseCtx->cursorPoint[PAUSE_WORLD_MAP] <= REGION_NONE) {
                        KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);
                        pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
                        break;
                    }
                    if (pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]) {
                        break;
                    }
                }
            } else {
                sStickAdjTimer++;
            }

            if (pauseCtx->cursorSpecialPos == 0) {
                pauseCtx->cursorItem[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
                // Used as cursor vtxIndex
                pauseCtx->cursorSlot[PAUSE_MAP] = 31 + pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
            }
        } else {
            pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
            if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
                if (pauseCtx->stickAdjX > 30) {
                    pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = REGION_NONE;
                    pauseCtx->cursorSpecialPos = 0;
                    pauseCtx->cursorShrinkRate = 4.0f;

                    while (true) {
                        pauseCtx->cursorPoint[PAUSE_WORLD_MAP]++;
                        if (pauseCtx->cursorPoint[PAUSE_WORLD_MAP] >= REGION_MAX) {
                            KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);
                            pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
                            break;
                        }
                        if (pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]) {
                            break;
                        }
                    }

                    if (pauseCtx->cursorSpecialPos == 0) {
                        pauseCtx->cursorItem[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
                        // Used as cursor vtxIndex
                        pauseCtx->cursorSlot[PAUSE_MAP] = 31 + pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
                    }
                    Audio_PlaySfx(NA_SE_SY_CURSOR);
                    sStickAdjTimer = 0;
                }
            } else if (pauseCtx->stickAdjX < -30) {
                pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = REGION_MAX;
                pauseCtx->cursorSpecialPos = 0;
                pauseCtx->cursorShrinkRate = 4.0f;

                while (true) {
                    pauseCtx->cursorPoint[PAUSE_WORLD_MAP]--;
                    if (pauseCtx->cursorPoint[PAUSE_WORLD_MAP] <= REGION_NONE) {
                        KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);
                        pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
                        break;
                    }
                    if (pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]) {
                        break;
                    }
                }

                if (pauseCtx->cursorSpecialPos == 0) {
                    pauseCtx->cursorItem[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
                    // Used as cursor vtxIndex
                    pauseCtx->cursorSlot[PAUSE_MAP] = 31 + pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
                }
                Audio_PlaySfx(NA_SE_SY_CURSOR);
                sStickAdjTimer = 0;
            }
        }

        if (!pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]) {
            pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
        }
        if (oldCursorPoint != pauseCtx->cursorPoint[PAUSE_WORLD_MAP]) {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    } else if (GameInteractor_Should(VB_OWL_WARP_MENU_USE_LINEAR_CURSOR,
                                     pauseCtx->state == PAUSE_STATE_OWL_WARP_SELECT)) {
        pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_BLUE;
        oldCursorPoint = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];

        if (pauseCtx->stickAdjX > 30) {
            pauseCtx->cursorShrinkRate = 4.0f;
            sStickAdjTimer = 0;
            do {
                SHIP_HANDLE_OWL_CURSOR_INF_LOOP();
                pauseCtx->cursorPoint[PAUSE_WORLD_MAP]++;
                if (pauseCtx->cursorPoint[PAUSE_WORLD_MAP] > OWL_WARP_STONE_TOWER) {
                    pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = OWL_WARP_GREAT_BAY_COAST;
                }
            } while (!pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]);
        } else if (pauseCtx->stickAdjX < -30) {
            pauseCtx->cursorShrinkRate = 4.0f;
            sStickAdjTimer = 0;
            do {
                SHIP_HANDLE_OWL_CURSOR_INF_LOOP();
                pauseCtx->cursorPoint[PAUSE_WORLD_MAP]--;
                if (pauseCtx->cursorPoint[PAUSE_WORLD_MAP] < OWL_WARP_GREAT_BAY_COAST) {
                    pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = OWL_WARP_STONE_TOWER;
                }
            } while (!pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]]);
        } else {
            sStickAdjTimer++;
        }

        // Offset from `ITEM_MAP_POINT_GREAT_BAY` is to get the correct ordering in `map_name_static`
        pauseCtx->cursorItem[PAUSE_MAP] =
            sOwlWarpPauseItems[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]] - ITEM_MAP_POINT_GREAT_BAY;
        // Used as cursor vtxIndex
        pauseCtx->cursorSlot[PAUSE_MAP] = 31 + pauseCtx->cursorPoint[PAUSE_WORLD_MAP];

        if (oldCursorPoint != pauseCtx->cursorPoint[PAUSE_WORLD_MAP]) {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    }
}
