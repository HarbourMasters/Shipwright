#include <string.h>
#include "global.h"
#include "vt.h"
#include "../overlays/gamestates/ovl_select/z_select.c"

SpeedMeter D_801664D0;
struct_801664F0 D_801664F0;
struct_80166500 D_80166500;
VisMono sMonoColors;
ViMode sViMode;
FaultClient sGameFaultClient;
u16 sLastButtonPressed;

// Forward declared, because this in a C++ header.
int gfx_create_framebuffer(uint32_t width, uint32_t height);

void GameState_FaultPrint(void) {
    static char sBtnChars[] = "ABZSuldr*+LRudlr";
    s32 i;

    osSyncPrintf("last_button=%04x\n", sLastButtonPressed);
    FaultDrawer_DrawText(120, 180, "%08x", sLastButtonPressed);
    for (i = 0; i < ARRAY_COUNT(sBtnChars); i++) {
        if (sLastButtonPressed & (1 << i)) {
            FaultDrawer_DrawText((i * 8) + 0x78, 0xBE, "%c", sBtnChars[i]);
        }
    }
}

void GameState_SetFBFilter(Gfx** gfx) {
    Gfx* gfxP;
    gfxP = *gfx;

    if ((R_FB_FILTER_TYPE > 0) && (R_FB_FILTER_TYPE < 5)) {
        D_801664F0.type = R_FB_FILTER_TYPE;
        D_801664F0.color.r = R_FB_FILTER_PRIM_COLOR(0);
        D_801664F0.color.g = R_FB_FILTER_PRIM_COLOR(1);
        D_801664F0.color.b = R_FB_FILTER_PRIM_COLOR(2);
        D_801664F0.color.a = R_FB_FILTER_A;
        func_800ACE98(&D_801664F0, &gfxP);
    } else if ((R_FB_FILTER_TYPE == 5) || (R_FB_FILTER_TYPE == 6)) {
        D_80166500.useRgba = (R_FB_FILTER_TYPE == 6);
        D_80166500.primColor.r = R_FB_FILTER_PRIM_COLOR(0);
        D_80166500.primColor.g = R_FB_FILTER_PRIM_COLOR(1);
        D_80166500.primColor.b = R_FB_FILTER_PRIM_COLOR(2);
        D_80166500.primColor.a = R_FB_FILTER_A;
        D_80166500.envColor.r = R_FB_FILTER_ENV_COLOR(0);
        D_80166500.envColor.g = R_FB_FILTER_ENV_COLOR(1);
        D_80166500.envColor.b = R_FB_FILTER_ENV_COLOR(2);
        D_80166500.envColor.a = R_FB_FILTER_A;
        func_800AD958(&D_80166500, &gfxP);
    } else if (R_FB_FILTER_TYPE == 7) {
        sMonoColors.unk_00 = 0;
        sMonoColors.primColor.r = R_FB_FILTER_PRIM_COLOR(0);
        sMonoColors.primColor.g = R_FB_FILTER_PRIM_COLOR(1);
        sMonoColors.primColor.b = R_FB_FILTER_PRIM_COLOR(2);
        sMonoColors.primColor.a = R_FB_FILTER_A;
        sMonoColors.envColor.r = R_FB_FILTER_ENV_COLOR(0);
        sMonoColors.envColor.g = R_FB_FILTER_ENV_COLOR(1);
        sMonoColors.envColor.b = R_FB_FILTER_ENV_COLOR(2);
        sMonoColors.envColor.a = R_FB_FILTER_A;
        VisMono_Draw(&sMonoColors, &gfxP);
    }
    *gfx = gfxP;
}

void func_800C4344(GameState* gameState) {
    Input* selectedInput;
    s32 hexDumpSize;
    u16 hReg82;

    if (HREG(80) == 0x14) {
        __osMalloc_FreeBlockTest_Enable = HREG(82);
    }

    if (HREG(80) == 0xC) {
        selectedInput = &gameState->input[(u32)HREG(81) < 4U ? HREG(81) : 0];
        hReg82 = HREG(82);
        HREG(83) = selectedInput->cur.button;
        HREG(84) = selectedInput->press.button;
        HREG(85) = selectedInput->rel.stick_x;
        HREG(86) = selectedInput->rel.stick_y;
        HREG(87) = selectedInput->rel.stick_x;
        HREG(88) = selectedInput->rel.stick_y;
        HREG(89) = selectedInput->cur.stick_x;
        HREG(90) = selectedInput->cur.stick_y;
        HREG(93) = (selectedInput->cur.button == hReg82);
        HREG(94) = CHECK_BTN_ALL(selectedInput->cur.button, hReg82);
        HREG(95) = CHECK_BTN_ALL(selectedInput->press.button, hReg82);
    }

    if (gIsCtrlr2Valid) {
        func_8006390C(&gameState->input[1]);
    }

    D_80009460 = HREG(60);
    gDmaMgrDmaBuffSize = SREG(21) != 0 ? ALIGN16(SREG(21)) : 0x2000;
    gSystemArenaLogSeverity = HREG(61);
    gZeldaArenaLogSeverity = HREG(62);
    if (HREG(80) == 8) {
        if (HREG(94) != 8) {
            HREG(94) = 8;
            HREG(81) = 0;
            HREG(82) = 0;
            HREG(83) = 0;
        }
        if (HREG(81) < 0) {
            HREG(81) = 0;
            // & 0xFFFFFFFF necessary for matching.
            hexDumpSize = (HREG(83) == 0 ? 0x100 : HREG(83) * 0x10) & 0xFFFFFFFF;
            LogUtils_LogHexDump(PHYSICAL_TO_VIRTUAL(HREG(82) << 8), hexDumpSize);
        }
    }
}

void GameState_DrawInputDisplay(u16 input, Gfx** gfx) {
    static const u16 sInpDispBtnColors[] = {
        GPACK_RGBA5551(255, 255, 0, 1),   GPACK_RGBA5551(255, 255, 0, 1),   GPACK_RGBA5551(255, 255, 0, 1),
        GPACK_RGBA5551(255, 255, 0, 1),   GPACK_RGBA5551(120, 120, 120, 1), GPACK_RGBA5551(120, 120, 120, 1),
        GPACK_RGBA5551(0, 255, 255, 1),   GPACK_RGBA5551(255, 0, 255, 1),   GPACK_RGBA5551(120, 120, 120, 1),
        GPACK_RGBA5551(120, 120, 120, 1), GPACK_RGBA5551(120, 120, 120, 1), GPACK_RGBA5551(120, 120, 120, 1),
        GPACK_RGBA5551(255, 0, 0, 1),     GPACK_RGBA5551(120, 120, 120, 1), GPACK_RGBA5551(0, 255, 0, 1),
        GPACK_RGBA5551(0, 0, 255, 1),
    };
    s32 i, j, k;
    Gfx* gfxP = *gfx;

    gDPPipeSync(gfxP++);
    gDPSetOtherMode(gfxP++,
                    G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_CONV | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                        G_TD_CLAMP | G_TP_NONE | G_CYC_FILL | G_PM_NPRIMITIVE,
                    G_AC_NONE | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2);

    for (i = 0; i < 16; i++) {
        j = i;
        if (input & (1 << i)) {
            gDPSetFillColor(gfxP++, (sInpDispBtnColors[i] << 0x10) | sInpDispBtnColors[i]);
            k = i + 1;
            gDPFillRectangle(gfxP++, (j * 4) + 226, 220, (k * 4) + 225, 223);
            gDPPipeSync(gfxP++);
        }
    }

    *gfx = gfxP;
}

void GameState_Draw(GameState* gameState, GraphicsContext* gfxCtx) {
    Gfx* newDList;
    Gfx* polyOpaP;

    OPEN_DISPS(gfxCtx);

    newDList = Graph_GfxPlusOne(polyOpaP = POLY_OPA_DISP);
    gSPDisplayList(OVERLAY_DISP++, newDList);

    if (R_ENABLE_FB_FILTER == 1) {
        GameState_SetFBFilter(&newDList);
    }

    sLastButtonPressed = gameState->input[0].press.button | gameState->input[0].cur.button;
    if (R_DISABLE_INPUT_DISPLAY == 0 && CVar_GetS32("gDebugEnabled", 0)) {
        GameState_DrawInputDisplay(sLastButtonPressed, &newDList);
    }

    if (R_ENABLE_AUDIO_DBG & 1) {
        s32 pad;
        GfxPrint printer;

        GfxPrint_Init(&printer);
        GfxPrint_Open(&printer, newDList);
        AudioDebug_Draw(&printer);
        newDList = GfxPrint_Close(&printer);
        GfxPrint_Destroy(&printer);
    }

    if (R_ENABLE_ARENA_DBG < 0) {
        s32 pad;

        DebugArena_Display();
        SystemArena_Display();
        // "%08x bytes left until the death of Hyrule (game_alloc)"
        osSyncPrintf("ハイラル滅亡まであと %08x バイト(game_alloc)\n", THA_GetSize(&gameState->tha));
        R_ENABLE_ARENA_DBG = 0;
    }

    gSPEndDisplayList(newDList++);
    Graph_BranchDlist(polyOpaP, newDList);
    POLY_OPA_DISP = newDList;

    CLOSE_DISPS(gfxCtx);

    func_80063D7C(gfxCtx);

    if (R_ENABLE_ARENA_DBG != 0) {
        SpeedMeter_DrawTimeEntries(&D_801664D0, gfxCtx);
        SpeedMeter_DrawAllocEntries(&D_801664D0, gfxCtx, gameState);
    }
}

void GameState_SetFrameBuffer(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0, 0);
    gSPSegment(POLY_OPA_DISP++, 0xF, gfxCtx->curFrameBuffer);
    gSPSegment(POLY_OPA_DISP++, 0xE, gZBuffer);
    gSPSegment(POLY_XLU_DISP++, 0, 0);
    gSPSegment(POLY_XLU_DISP++, 0xF, gfxCtx->curFrameBuffer);
    gSPSegment(POLY_XLU_DISP++, 0xE, gZBuffer);
    gSPSegment(OVERLAY_DISP++, 0, 0);
    gSPSegment(OVERLAY_DISP++, 0xF, gfxCtx->curFrameBuffer);
    gSPSegment(OVERLAY_DISP++, 0xE, gZBuffer);

    CLOSE_DISPS(gfxCtx);
}

void func_800C49F4(GraphicsContext* gfxCtx) {
    Gfx* newDlist;
    Gfx* polyOpaP;

    OPEN_DISPS(gfxCtx);

    newDlist = Graph_GfxPlusOne(polyOpaP = POLY_OPA_DISP);
    gSPDisplayList(OVERLAY_DISP++, newDlist);

    gSPEndDisplayList(newDlist++);
    Graph_BranchDlist(polyOpaP, newDlist);
    POLY_OPA_DISP = newDlist;

    CLOSE_DISPS(gfxCtx);
}

void PadMgr_RequestPadData(PadMgr*, Input*, s32);

void GameState_ReqPadData(GameState* gameState) {
    PadMgr_RequestPadData(&gPadMgr, &gameState->input[0], 1);
}

// OTRTODO
int fbTest = -1;

void GameState_Update(GameState* gameState) {
    GraphicsContext* gfxCtx = gameState->gfxCtx;

    if (fbTest == -1)
    {
        fbTest = gfx_create_framebuffer(64, 112);
        //fbTest = gfx_create_framebuffer(256, 512);
    }

    GameState_SetFrameBuffer(gfxCtx);

    gameState->main(gameState);

    func_800C4344(gameState);

    if (SREG(63) == 1u) {
        if (SREG(48) < 0) {
            SREG(48) = 0;
            gfxCtx->viMode = &gViConfigMode;
            gfxCtx->viFeatures = gViConfigFeatures;
            gfxCtx->xScale = gViConfigXScale;
            gfxCtx->yScale = gViConfigYScale;
        } else if (SREG(48) > 0) {
            ViMode_Update(&sViMode, gameState->input);
            gfxCtx->viMode = &sViMode.customViMode;
            gfxCtx->viFeatures = sViMode.viFeatures;
            gfxCtx->xScale = 1.0f;
            gfxCtx->yScale = 1.0f;
        }
    } else if (SREG(63) >= 2) {
        gfxCtx->viMode = &gViConfigMode;
        gfxCtx->viFeatures = gViConfigFeatures;
        gfxCtx->xScale = gViConfigXScale;
        gfxCtx->yScale = gViConfigYScale;
        if (SREG(63) == 6 || (SREG(63) == 2u && osTvType == OS_TV_NTSC)) {
            gfxCtx->viMode = &osViModeNtscLan1;
            gfxCtx->yScale = 1.0f;
        }

        if (SREG(63) == 5 || (SREG(63) == 2u && osTvType == OS_TV_MPAL)) {
            gfxCtx->viMode = &osViModeMpalLan1;
            gfxCtx->yScale = 1.0f;
        }

        if (SREG(63) == 4 || (SREG(63) == 2u && osTvType == OS_TV_PAL)) {
            gfxCtx->viMode = &osViModePalLan1;
            gfxCtx->yScale = 1.0f;
        }

        if (SREG(63) == 3 || (SREG(63) == 2u && osTvType == OS_TV_PAL)) {
            gfxCtx->viMode = &osViModeFpalLan1;
            gfxCtx->yScale = 0.833f;
        }
    } else {
        gfxCtx->viMode = NULL;
    }

    if (HREG(80) == 0x15) {
        if (HREG(95) != 0x15) {
            HREG(95) = 0x15;
            HREG(81) = 0;
            HREG(82) = gViConfigAdditionalScanLines;
            HREG(83) = 0;
            HREG(84) = 0;
        }

        if (HREG(82) < 0) {
            HREG(82) = 0;
        }
        if (HREG(82) > 0x30) {
            HREG(82) = 0x30;
        }

        if ((HREG(83) != HREG(82)) || HREG(84) != HREG(81)) {
            HREG(83) = HREG(82);
            HREG(84) = HREG(81);
            gViConfigAdditionalScanLines = HREG(82);
            gViConfigYScale = HREG(81) == 0 ? 240.0f / (gViConfigAdditionalScanLines + 240.0f) : 1.0f;
            D_80009430 = 1;
        }
    }

    if (R_PAUSE_MENU_MODE != 2u) {
        GameState_Draw(gameState, gfxCtx);
        func_800C49F4(gfxCtx);
    }

    // -----------------------
    // Cheats hooks
    // -----------------------

    // Inf Money
    if (CVar_GetS32("gInfiniteMoney", 0) != 0) {
        if (gSaveContext.rupees < CUR_CAPACITY(UPG_WALLET)) {
            gSaveContext.rupees = CUR_CAPACITY(UPG_WALLET);
        }
    }

    // Inf Health
    if (CVar_GetS32("gInfiniteHealth", 0) != 0) {
        if (gSaveContext.health < gSaveContext.healthCapacity) {
            gSaveContext.health = gSaveContext.healthCapacity;
        }
    }

    // Inf Ammo
    if (CVar_GetS32("gInfiniteAmmo", 0) != 0) {
        // Deku Sticks
        if (AMMO(ITEM_STICK) < CUR_CAPACITY(UPG_STICKS)) {
            AMMO(ITEM_STICK) = CUR_CAPACITY(UPG_STICKS);
        }

        // Deku Nuts
        if (AMMO(ITEM_NUT) < CUR_CAPACITY(UPG_NUTS)) {
            AMMO(ITEM_NUT) = CUR_CAPACITY(UPG_NUTS);
        }

        // Bombs
        if (AMMO(ITEM_BOMB) < CUR_CAPACITY(UPG_BOMB_BAG)) {
            AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
        }

        // Fairy Bow (Ammo)
        if (AMMO(ITEM_BOW) < CUR_CAPACITY(UPG_QUIVER)) {
            AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
        }

        // Fairy Slingshot (Ammo)
        if (AMMO(ITEM_SLINGSHOT) < CUR_CAPACITY(UPG_BULLET_BAG)) {
            AMMO(ITEM_SLINGSHOT) = CUR_CAPACITY(UPG_BULLET_BAG);
        }

        // Bombchus (max: 50, no upgrades)
        if (AMMO(ITEM_BOMBCHU) < 50) {
            AMMO(ITEM_BOMBCHU) = 50;
        }
    }

    // Inf Magic
    if (CVar_GetS32("gInfiniteMagic", 0) != 0) {
        if (gSaveContext.magicAcquired && gSaveContext.magic != (gSaveContext.doubleMagic + 1) * 0x30) {
            gSaveContext.magic = (gSaveContext.doubleMagic + 1) * 0x30;
        }
    }

    // Inf Nayru's Love Timer
    if (CVar_GetS32("gInfiniteNayru", 0) != 0) {
        gSaveContext.nayrusLoveTimer = 0x44B;
    }

    // Moon Jump On L
    if (CVar_GetS32("gMoonJumpOnL", 0) != 0) {
        if (gGlobalCtx) {
            Player* player = GET_PLAYER(gGlobalCtx);

            if (CHECK_BTN_ANY(gGlobalCtx->state.input[0].cur.button, BTN_L)) {
                player->actor.velocity.y = 6.34375f;
            }
        }
    }

    // Permanent infinite sword glitch (ISG)
    if (CVar_GetS32("gEzISG", 0) != 0) {
        if (gGlobalCtx) {
            Player* player = GET_PLAYER(gGlobalCtx);
            player->swordState = 1;
        }
    }

    // Unrestricted Items
    if (CVar_GetS32("gNoRestrictItems", 0) != 0) {
        if (gGlobalCtx) {
            u8 sunsBackup = gGlobalCtx->interfaceCtx.restrictions.sunsSong;
            memset(&gGlobalCtx->interfaceCtx.restrictions, 0, sizeof(gGlobalCtx->interfaceCtx.restrictions));
            gGlobalCtx->interfaceCtx.restrictions.sunsSong = sunsBackup;
        }
    }

    // Freeze Time
    if (CVar_GetS32("gFreezeTime", 0) != 0) {
        if (CVar_GetS32("gPrevTime", -1) == -1) {
            CVar_SetS32("gPrevTime", gSaveContext.dayTime);
        }

        int32_t prevTime = CVar_GetS32("gPrevTime", gSaveContext.dayTime);
        gSaveContext.dayTime = prevTime;
    } else {
        CVar_SetS32("gPrevTime", -1);
    }
    
    //Warp Logic
    if (gGlobalCtx) {

        //Logic for General Warping
        if (CVar_GetS32("gWarpCheat", 0) != 0) {
            int temp = CVar_GetS32("gWarpCheat", 0);
            gGlobalCtx->nextEntranceIndex = sScenes[CVar_GetS32("gWarpCheat", 0) - 1].entranceIndex;
            CVar_SetS32("gWarpCheat", 0);
            gGlobalCtx->sceneLoadFlag = 0x14;
            gGlobalCtx->fadeTransition = 11;
            gSaveContext.nextTransition = 11;
        }

        //Logic for Grotto Warping
        if (CVar_GetS32("gWarpGrottoCheat", 0) != 0) {
            //0 = Fairy Fountain - Leaving this out for now since they aren't necessary for progression. Can be added in later.
            //1 = Single Treasure Chest.(Covered in gWarpChestGrottoCheat)
            //2 = 1 skultulla, one gold skultulla. (HF near Kakoriko Village)
            //3 = 1 Deku scrub. Water puddles. (Only in HF between fences to lake hylia)
            //4 = 2 redeads. watery walls. (Only in Kakoriko closed grotto near tree)
            //5 = 3 Dekus.
            //6 = Spiderwebs on 3 walls. (Near Gerudo Valley, HF)
            //7 = Water with single octorok. (Gerudo Canyon)
            //8 = 2 Deku Scrubs (closest on left)
            //9 = Metalic room with wolfoes. (Only in Sacred Meadow)
            //10 = Rocky room with 3 bombable walls. (Only near Hyrule Castle Courtyard)
            //11 = 2 Deku Scrubs (closest on right)
            //12 = Tektite grotto. (Only in HF near tree between Hyrule Castle and Gerudo Valley.)
            //13 = Deku Theater. (Only in lost Woods)
            //14 = Single cow and a few puddles. (Only on Death Mountain Trail)

            //Chest Grottos
            if (CVar_GetS32("gWarpGrottoCheat", 0) == 21) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = 44; // Kokiri Forest
                gGlobalCtx->nextEntranceIndex = entrances[1];
            } else if (CVar_GetS32("gWarpGrottoCheat", 0) == 22) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = 20; // Lost Woods
                gGlobalCtx->nextEntranceIndex = entrances[1];
            } else if (CVar_GetS32("gWarpGrottoCheat", 0) == 23) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = 0; // HF near Marketplace
                gGlobalCtx->nextEntranceIndex = entrances[1];
            } else if (CVar_GetS32("gWarpGrottoCheat", 0) == 24) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = 23; // HF Open Grotto
                gGlobalCtx->nextEntranceIndex = entrances[1];
            } else if (CVar_GetS32("gWarpGrottoCheat", 0) == 25) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = 34; // HF SE Boulder
                gGlobalCtx->nextEntranceIndex = entrances[1];
            } else if (CVar_GetS32("gWarpGrottoCheat", 0) == 26) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = 40; // Kakoriko open grotto
                gGlobalCtx->nextEntranceIndex = entrances[1];
            } else if (CVar_GetS32("gWarpGrottoCheat", 0) == 27) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = 87; // Death mountain trail SoS grotto
                gGlobalCtx->nextEntranceIndex = entrances[1];
            } else if (CVar_GetS32("gWarpGrottoCheat", 0) == 28) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = 122; // Death Mountain Crater
                gGlobalCtx->nextEntranceIndex = entrances[1];
            } else if (CVar_GetS32("gWarpGrottoCheat", 0) == 29) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = 41; // Zora River
                gGlobalCtx->nextEntranceIndex = entrances[1];
                //Scrub Grottos
            } else if (CVar_GetS32("gWarpGrottoCheat", 0) == 31) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = (0xEB & ((1 << 8) - 1));  //Zora River
                gGlobalCtx->nextEntranceIndex = entrances[11];
            } else if (CVar_GetS32("gWarpGrottoCheat", 0) == 32) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = (0xEE & ((1 << 8) - 1)); //Sacred Forest
                gGlobalCtx->nextEntranceIndex = entrances[11];
            } else if (CVar_GetS32("gWarpGrottoCheat", 0) == 33) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = (0xEF & ((1 << 8) - 1)); //Lake Hylia
                gGlobalCtx->nextEntranceIndex = entrances[5];
            } else if (CVar_GetS32("gWarpGrottoCheat", 0) == 34) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = (0xF0 & ((1 << 8) - 1)); //Gerudo Valley
                gGlobalCtx->nextEntranceIndex = entrances[11];
            } else if (CVar_GetS32("gWarpGrottoCheat", 0) == 35) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = (0xF5 & ((1 << 8) - 1)); //Lost Woods
                gGlobalCtx->nextEntranceIndex = entrances[8];
            } else if (CVar_GetS32("gWarpGrottoCheat", 0) == 36) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = (0xF9 & ((1 << 8) - 1)); //Death Mountain Crater
                gGlobalCtx->nextEntranceIndex = entrances[5];
            } else if (CVar_GetS32("gWarpGrottoCheat", 0) == 37) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = (0xFB & ((1 << 8) - 1)); //Gerudo Fortress
                gGlobalCtx->nextEntranceIndex = entrances[5];
            } else if (CVar_GetS32("gWarpGrottoCheat", 0) == 38) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = (0xFC & ((1 << 8) - 1)); //Lon Lon Ranch
                gGlobalCtx->nextEntranceIndex = entrances[5];
            } else if (CVar_GetS32("gWarpGrottoCheat", 0) == 39) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = (0xFD & ((1 << 8) - 1)); //Desert Colossus
                gGlobalCtx->nextEntranceIndex = entrances[11];
            } else {
                gGlobalCtx->nextEntranceIndex = entrances[CVar_GetS32("gWarpGrottoCheat", 0) - 1];
            }
            CVar_SetS32("gWarpGrottoCheat", 0);
            gGlobalCtx->sceneLoadFlag = 0x14;
            gGlobalCtx->fadeTransition = 11;
            gSaveContext.nextTransition = 11;
            Gameplay_SetupRespawnPoint(gGlobalCtx, RESPAWN_MODE_RETURN, 0x4FF);
        }
    }

    //since our CVar is same value and properly default to 0 there is not problems doing this in single line.
    gSaveContext.language = CVar_GetS32("gLanguages", 0);

    gameState->frames++;
}

void GameState_InitArena(GameState* gameState, size_t size) {
    void* arena;

    osSyncPrintf("ハイラル確保 サイズ＝%u バイト\n"); // "Hyrule reserved size = %u bytes"
    arena = GAMESTATE_MALLOC_DEBUG(&gameState->alloc, size);
    if (arena != NULL) {
        THA_Ct(&gameState->tha, arena, size);
        osSyncPrintf("ハイラル確保成功\n"); // "Successful Hyral"
    } else {
        THA_Ct(&gameState->tha, NULL, 0);
        osSyncPrintf("ハイラル確保失敗\n"); // "Failure to secure Hyrule"
        Fault_AddHungupAndCrash(__FILE__, __LINE__);
    }
}

void GameState_Realloc(GameState* gameState, size_t size) {
    GameAlloc* alloc = &gameState->alloc;
    void* gameArena;
    u32 systemMaxFree;
    u32 systemFree;
    u32 systemAlloc;
    void* thaBufp = gameState->tha.bufp;

    THA_Dt(&gameState->tha);
    GameAlloc_Free(alloc, thaBufp);
    osSyncPrintf("ハイラル一時解放!!\n"); // "Hyrule temporarily released!!"
    SystemArena_GetSizes(&systemMaxFree, &systemFree, &systemAlloc);
    if ((systemMaxFree - 0x10) < size) {
        osSyncPrintf("%c", 7);
        osSyncPrintf(VT_FGCOL(RED));

        // "Not enough memory. Change the hyral size to the largest possible value"
        osSyncPrintf("メモリが足りません。ハイラルサイズを可能な最大値に変更します\n");
        osSyncPrintf("(hyral=%08x max=%08x free=%08x alloc=%08x)\n", size, systemMaxFree, systemFree, systemAlloc);
        osSyncPrintf(VT_RST);
        size = systemMaxFree - 0x10;
    }

    osSyncPrintf("ハイラル再確保 サイズ＝%u バイト\n", size); // "Hyral reallocate size = %u bytes"
    gameArena = GAMESTATE_MALLOC_DEBUG(alloc, size);
    if (gameArena != NULL) {
        THA_Ct(&gameState->tha, gameArena, size);
        osSyncPrintf("ハイラル再確保成功\n"); // "Successful reacquisition of Hyrule"
    } else {
        THA_Ct(&gameState->tha, NULL, 0);
        osSyncPrintf("ハイラル再確保失敗\n"); // "Failure to secure Hyral"
        SystemArena_Display();
        Fault_AddHungupAndCrash(__FILE__, __LINE__);
    }
}

void GameState_Init(GameState* gameState, GameStateFunc init, GraphicsContext* gfxCtx) {
    OSTime startTime;
    OSTime endTime;

    osSyncPrintf("game コンストラクタ開始\n"); // "game constructor start"
    gameState->gfxCtx = gfxCtx;
    gameState->frames = 0;
    gameState->main = NULL;
    gameState->destroy = NULL;
    gameState->running = 1;
    startTime = osGetTime();
    gameState->size = 0;
    gameState->init = NULL;
    endTime = osGetTime();

    // "game_set_next_game_null processing time %d us"
    osSyncPrintf("game_set_next_game_null 処理時間 %d us\n", OS_CYCLES_TO_USEC(endTime - startTime));
    startTime = endTime;
    GameAlloc_Init(&gameState->alloc);

    endTime = osGetTime();
    // "gamealloc_init processing time %d us"
    osSyncPrintf("gamealloc_init 処理時間 %d us\n", OS_CYCLES_TO_USEC(endTime - startTime));

    startTime = endTime;
    GameState_InitArena(gameState, 0x100000);
    R_UPDATE_RATE = 3;
    init(gameState);

    endTime = osGetTime();
    // "init processing time %d us"
    osSyncPrintf("init 処理時間 %d us\n", OS_CYCLES_TO_USEC(endTime - startTime));

    startTime = endTime;
    LOG_CHECK_NULL_POINTER("this->cleanup", gameState->destroy);
    func_800ACE70(&D_801664F0);
    func_800AD920(&D_80166500);
    VisMono_Init(&sMonoColors);
    if (SREG(48) == 0) {
        ViMode_Init(&sViMode);
    }
    SpeedMeter_Init(&D_801664D0);
    func_800AA0B4();
    osSendMesgPtr(&gameState->gfxCtx->queue, NULL, OS_MESG_BLOCK);

    endTime = osGetTime();
    // "Other initialization processing time %d us"
    osSyncPrintf("その他初期化 処理時間 %d us\n", OS_CYCLES_TO_USEC(endTime - startTime));

    Fault_AddClient(&sGameFaultClient, GameState_FaultPrint, NULL, NULL);

    osSyncPrintf("game コンストラクタ終了\n"); // "game constructor end"
}

void GameState_Destroy(GameState* gameState) {
    osSyncPrintf("game デストラクタ開始\n"); // "game destructor start"
    func_800C3C20();
    func_800F3054();
    osRecvMesg(&gameState->gfxCtx->queue, NULL, OS_MESG_BLOCK);
    LOG_CHECK_NULL_POINTER("this->cleanup", gameState->destroy);
    if (gameState->destroy != NULL) {
        gameState->destroy(gameState);
    }
    func_800AA0F0();
    SpeedMeter_Destroy(&D_801664D0);
    func_800ACE90(&D_801664F0);
    func_800AD950(&D_80166500);
    VisMono_Destroy(&sMonoColors);
    if (SREG(48) == 0) {
        ViMode_Destroy(&sViMode);
    }
    THA_Dt(&gameState->tha);
    GameAlloc_Cleanup(&gameState->alloc);
    SystemArena_Display();
    Fault_RemoveClient(&sGameFaultClient);

    osSyncPrintf("game デストラクタ終了\n"); // "game destructor end"
}

GameStateFunc GameState_GetInit(GameState* gameState) {
    return gameState->init;
}

size_t GameState_GetSize(GameState* gameState) {
    return gameState->size;
}

u32 GameState_IsRunning(GameState* gameState) {
    return gameState->running;
}

void* GameState_Alloc(GameState* gameState, size_t size, char* file, s32 line)
{
    void* ret;

    if (THA_IsCrash(&gameState->tha)) {
        osSyncPrintf("ハイラルは滅亡している\n");
        ret = NULL;
    } else if ((uintptr_t)THA_GetSize(&gameState->tha) < size) {
        // "Hyral on the verge of extinction does not have %d bytes left (%d bytes until extinction)"
        osSyncPrintf("滅亡寸前のハイラルには %d バイトの余力もない（滅亡まであと %d バイト）\n", size,
                     THA_GetSize(&gameState->tha));
        ret = NULL;
    } else {
        ret = THA_AllocEndAlign16(&gameState->tha, size);
        if (THA_IsCrash(&gameState->tha)) {
            osSyncPrintf("ハイラルは滅亡してしまった\n"); // "Hyrule has been destroyed"
            ret = NULL;
        }
    }
    if (ret != NULL) {
        osSyncPrintf(VT_FGCOL(GREEN));
        osSyncPrintf("game_alloc(%08x) %08x-%08x [%s:%d]\n", size, ret, (uintptr_t)ret + size, file, line);
        osSyncPrintf(VT_RST);
    }
    return ret;
}

void* GameState_AllocEndAlign16(GameState* gameState, size_t size) {
    return THA_AllocEndAlign16(&gameState->tha, size);
}

s32 GameState_GetArenaSize(GameState* gameState) {
    return THA_GetSize(&gameState->tha);
}
