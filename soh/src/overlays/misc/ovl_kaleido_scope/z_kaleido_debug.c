#include "z_kaleido_scope.h"
#include "textures/parameter_static/parameter_static.h"

// Positions of each input section in the editor
static u16 sSectionPositions[][2] = {
    { 64, 15 },   { 144, 15 },  { 170, 15 },  { 78, 35 },   { 104, 35 },  { 130, 35 },  { 156, 35 },  { 182, 35 },
    { 208, 35 },  { 78, 50 },   { 104, 50 },  { 130, 50 },  { 156, 50 },  { 182, 50 },  { 208, 50 },  { 78, 65 },
    { 104, 65 },  { 130, 65 },  { 156, 65 },  { 182, 65 },  { 208, 65 },  { 78, 80 },   { 104, 80 },  { 130, 80 },
    { 156, 80 },  { 182, 80 },  { 208, 80 },  { 78, 98 },   { 88, 98 },   { 98, 98 },   { 108, 98 },  { 118, 98 },
    { 128, 98 },  { 138, 98 },  { 148, 98 },  { 158, 98 },  { 168, 98 },  { 178, 98 },  { 188, 98 },  { 198, 98 },
    { 208, 98 },  { 218, 98 },  { 228, 98 },  { 238, 98 },  { 78, 115 },  { 90, 115 },  { 102, 115 }, { 114, 115 },
    { 126, 115 }, { 138, 115 }, { 150, 115 }, { 162, 115 }, { 202, 115 }, { 214, 115 }, { 226, 115 }, { 238, 115 },
    { 78, 132 },  { 90, 132 },  { 102, 132 }, { 114, 132 }, { 126, 132 }, { 138, 132 }, { 150, 132 }, { 162, 132 },
    { 174, 132 }, { 186, 132 }, { 198, 132 }, { 210, 132 }, { 78, 149 },  { 90, 149 },  { 102, 149 }, { 114, 149 },
    { 126, 149 }, { 138, 149 }, { 78, 166 },  { 90, 166 },  { 102, 166 }, { 114, 166 }, { 126, 166 }, { 138, 166 },
    { 150, 166 }, { 162, 166 }, { 174, 166 }, { 186, 166 }, { 198, 166 }, { 210, 166 }, { 210, 149 }, { 222, 149 },
    { 234, 149 }, { 78, 185 },  { 90, 185 },  { 145, 185 }, { 210, 185 },
};

// First section of each row in the editor (starting from the top)
static u16 sRowFirstSections[] = {
    0x00, 0x03, 0x1B, 0x2C, 0x34, 0x38, 0x44, 0x4A, 0x56, 0x59, 0x5C,
};

// Maximum value of each upgrade type
static u8 sMaxUpgradeValues[] = {
    3, 3, 3, 2, 2, 3, 3, 3,
};

// Item ID corresponding to each slot, aside from bottles and trade items
static s16 sSlotItems[] = {
    ITEM_STICK,     ITEM_NUT,           ITEM_BOMB,    ITEM_BOW,      ITEM_ARROW_FIRE,  ITEM_DINS_FIRE,
    ITEM_SLINGSHOT, ITEM_OCARINA_FAIRY, ITEM_BOMBCHU, ITEM_HOOKSHOT, ITEM_ARROW_ICE,   ITEM_FARORES_WIND,
    ITEM_BOOMERANG, ITEM_LENS,          ITEM_BEAN,    ITEM_HAMMER,   ITEM_ARROW_LIGHT, ITEM_NAYRUS_LOVE,
};

void KaleidoScope_DrawDebugEditorText(Gfx** gfxp) {
    GfxPrint printer;
    s32 pad[2];

    GfxPrint_Init(&printer);
    GfxPrint_Open(&printer, *gfxp);

    GfxPrint_SetPos(&printer, 4, 2);
    GfxPrint_SetColor(&printer, 255, 60, 0, 255);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ﾙﾋﾟｰ"); // "Rupee"
    GfxPrint_SetPos(&printer, 15, 2);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ﾊｰﾄ"); // "Heart"
    GfxPrint_SetPos(&printer, 26, 3);
    GfxPrint_Printf(&printer, "%s", "/4");
    GfxPrint_SetPos(&printer, 4, 5);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｱｲﾃﾑ"); // "Item"
    GfxPrint_SetPos(&printer, 4, 13);
    GfxPrint_Printf(&printer, "%s", "KEY");
    GfxPrint_SetPos(&printer, 4, 15);
    GfxPrint_Printf(&printer, "%s", GFXP_HIRAGANA "ｿｳﾋﾞ"); // "Equipment"
    GfxPrint_SetPos(&printer, 23, 14);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｹﾝ"); // "Sword"
    GfxPrint_SetPos(&printer, 23, 15);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ﾀﾃ"); // "Shield"
    GfxPrint_SetPos(&printer, 4, 17);
    GfxPrint_Printf(&printer, "%s", "MAP");
    GfxPrint_SetPos(&printer, 4, 19);
    GfxPrint_Printf(&printer, "%s", GFXP_HIRAGANA "ﾌｳｲﾝ"); // "Seal"
    GfxPrint_SetPos(&printer, 20, 19);
    GfxPrint_Printf(&printer, "%s", GFXP_HIRAGANA "ｾｲﾚｲｾｷ"); // "Spiritual Stone"
    GfxPrint_SetPos(&printer, 4, 21);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｵｶﾘﾅ"); // "Ocarina"
    GfxPrint_SetPos(&printer, 4, 24);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｺﾚｸﾄ"); // "Collect"
    GfxPrint_SetPos(&printer, 14, 24);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｷﾝｽﾀ"); // "Skulltula"
    GfxPrint_SetPos(&printer, 23, 24);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｶｹﾗ"); // "Gold Token"
    GfxPrint_SetPos(&printer, 28, 24);
    GfxPrint_Printf(&printer, "%s", "/4");

    *gfxp = GfxPrint_Close(&printer);
    GfxPrint_Destroy(&printer);
}

extern const char* digitTextures[];

void KaleidoScope_DrawDigit(PlayState* play, s32 digit, s32 rectLeft, s32 rectTop) {
    OPEN_DISPS(play->state.gfxCtx);

    gDPLoadTextureBlock(POLY_KAL_DISP++, digitTextures[digit], G_IM_FMT_I, G_IM_SIZ_8b, 8, 16, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSPTextureRectangle(POLY_KAL_DISP++, rectLeft << 2, rectTop << 2, (rectLeft + 8) << 2, (rectTop + 16) << 2,
                        G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

    CLOSE_DISPS(play->state.gfxCtx);
}

void KaleidoScope_DrawDebugEditor(PlayState* play) {
    static s16 curSection = 0;
    static s16 curRow = 0;
    static s32 prevDBtnInput = 0;
    static s32 heldDBtnTimer = 0;
    PauseContext* pauseCtx = &play->pauseCtx;
    Input* input = &play->state.input[0];
    Gfx* gfx;
    Gfx* gfxRef;
    s16 spD8[4];
    s16 slot;
    s16 i;
    s16 j;
    s16 x;
    s16 y;
    s32 dBtnInput = input->cur.button & (BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT);

    OPEN_DISPS(play->state.gfxCtx);

    pauseCtx->stickRelX = input->rel.stick_x;
    pauseCtx->stickRelY = input->rel.stick_y;

    Gfx_SetupDL_39Kal(play->state.gfxCtx);

    gDPSetRenderMode(POLY_KAL_DISP++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineMode(POLY_KAL_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 0, 0, 0, 220);
    gDPFillRectangle(POLY_KAL_DISP++, 24, 12, 298, 228);
    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetCombineLERP(POLY_KAL_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);

    gfxRef = POLY_KAL_DISP;
    gfx = Graph_GfxPlusOne(gfxRef);
    gSPDisplayList(OVERLAY_DISP++, gfx);

    KaleidoScope_DrawDebugEditorText(&gfx);

    gSPEndDisplayList(gfx++);
    Graph_BranchDlist(gfxRef, gfx);
    POLY_KAL_DISP = gfx;

    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 0, 0, 255);
    gDPSetEnvColor(POLY_KAL_DISP++, 0, 0, 0, 0);

    // Current Health Quarter (X / 4)
    KaleidoScope_DrawDigit(play, (gSaveContext.health % 0x10) / 4, 194, 15);

    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, 255);

    // Rupees
    spD8[0] = spD8[1] = spD8[2] = 0;
    spD8[3] = gSaveContext.rupees;
    while (spD8[3] >= 1000) {
        spD8[0]++;
        spD8[3] -= 1000;
    }

    while (spD8[3] >= 100) {
        spD8[1]++;
        spD8[3] -= 100;
    }

    while (spD8[3] >= 10) {
        spD8[2]++;
        spD8[3] -= 10;
    }

    for (i = 0, x = 68; i < 4; i++, x += 10) {
        KaleidoScope_DrawDigit(play, spD8[i], x, 15);
    }

    // Health capacity
    spD8[2] = 0;
    spD8[3] = gSaveContext.healthCapacity / 0x10;
    while (spD8[3] >= 10) {
        spD8[2]++;
        spD8[3] -= 10;
    }

    KaleidoScope_DrawDigit(play, spD8[2], 146, 15);
    KaleidoScope_DrawDigit(play, spD8[3], 156, 15);

    // Health
    spD8[2] = 0;
    spD8[3] = gSaveContext.health / 0x10;
    while (spD8[3] >= 10) {
        spD8[2]++;
        spD8[3] -= 10;
    }

    KaleidoScope_DrawDigit(play, spD8[2], 172, 15);
    KaleidoScope_DrawDigit(play, spD8[3], 182, 15);

    // Inventory
    for (slot = 0, i = 0, y = 35; i < 4; i++, y += 15) {
        for (j = 0, x = 78; j < 6; j++, slot++, x += 26) {
            spD8[2] = 0;

            if ((slot <= SLOT_BOW) || (slot == SLOT_SLINGSHOT) || (slot == SLOT_BOMBCHU) || (slot == SLOT_BEAN)) {
                spD8[3] = AMMO(gAmmoItems[slot]);
            } else if (slot == SLOT_OCARINA) {
                spD8[3] = gSaveContext.inventory.items[slot];
            } else {
                spD8[3] = gSaveContext.inventory.items[slot];
            }

            if (spD8[3] != ITEM_NONE) {
                while (spD8[3] >= 10) {
                    spD8[2]++;
                    spD8[3] -= 10;
                }
            } else {
                spD8[2] = spD8[3] = 0;
            }

            KaleidoScope_DrawDigit(play, spD8[2], x, y);
            KaleidoScope_DrawDigit(play, spD8[3], x + 10, y);
        }
    }

    // Keys
    for (spD8[1] = 78, i = 0; i < 17; i++) {
        spD8[2] = 0;

        if ((spD8[3] = gSaveContext.inventory.dungeonKeys[i]) >= 0) {
            while (spD8[3] >= 10) {
                spD8[2]++;
                spD8[3] -= 10;
            }
        } else {
            spD8[2] = spD8[3] = 0;
        }

        KaleidoScope_DrawDigit(play, spD8[3], spD8[1], 98);
        spD8[1] += 10;
    }

    // Upgrades
    for (spD8[1] = 78, i = 0; i < 8; i++, spD8[1] += 12) {
        KaleidoScope_DrawDigit(play, CUR_UPG_VALUE(i), spD8[1], 115);
    }

    // Equipment
    for (spD8[1] = 202, i = 0; i < 4; i++, spD8[1] += 12) {
        KaleidoScope_DrawDigit(play, ALL_EQUIP_VALUE(i), spD8[1], 115);
    }

    // Dungeon Items
    for (spD8[1] = 78, i = 0; i < 12; i++, spD8[1] += 12) {
        spD8[2] = gSaveContext.inventory.dungeonItems[i] & gEquipMasks[0];
        KaleidoScope_DrawDigit(play, spD8[2], spD8[1], 132);
    }

    // Medallions
    for (spD8[1] = 78, i = 0; i < 6; i++, spD8[1] += 12) {
        spD8[2] = 0;
        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST + i)) {
            spD8[2] = 1;
        }
        KaleidoScope_DrawDigit(play, spD8[2], spD8[1], 149);
    }

    // Spiritual Stones
    for (spD8[1] = 210, i = 0; i < 3; i++, spD8[1] += 12) {
        spD8[2] = 0;
        if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD + i)) {
            spD8[2] = 1;
        }
        KaleidoScope_DrawDigit(play, spD8[2], spD8[1], 149);
    }

    // Songs
    for (spD8[1] = 78, i = 0; i < 12; i++, spD8[1] += 12) {
        spD8[2] = 0;
        if (CHECK_QUEST_ITEM(QUEST_SONG_MINUET + i)) {
            spD8[2] = 1;
        }
        KaleidoScope_DrawDigit(play, spD8[2], spD8[1], 166);
    }

    // Other Quest Items
    for (spD8[1] = 78, i = 0; i < 2; i++, spD8[1] += 12) {
        spD8[2] = 0;
        if (CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY + i)) {
            spD8[2] = 1;
        }
        KaleidoScope_DrawDigit(play, spD8[2], spD8[1], 185);
    }

    // GS Tokens
    spD8[3] = gSaveContext.inventory.gsTokens;
    spD8[1] = 0;
    spD8[2] = 0;
    while (spD8[3] >= 100) {
        spD8[1]++;
        spD8[3] -= 100;
    }

    while (spD8[3] >= 10) {
        spD8[2]++;
        spD8[3] -= 10;
    }

    KaleidoScope_DrawDigit(play, spD8[1], 145, 185);
    KaleidoScope_DrawDigit(play, spD8[2], 155, 185);
    KaleidoScope_DrawDigit(play, spD8[3], 165, 185);

    // Heart Pieces (X / 4)
    KaleidoScope_DrawDigit(play, ((gSaveContext.inventory.questItems & 0xF0000000) & 0xF0000000) >> 0x1C, 210,
                           185);

    // Handles navigating the menu to different sections with the D-Pad
    // When the same direction is held, registers the input periodically based on a timer
    if (dBtnInput == prevDBtnInput) {
        heldDBtnTimer--;
        if (heldDBtnTimer < 0) {
            heldDBtnTimer = 1;
        } else {
            dBtnInput ^= prevDBtnInput;
        }
    } else {
        prevDBtnInput = dBtnInput;
        heldDBtnTimer = 16;
    }

    if (CHECK_BTN_ANY(dBtnInput, BTN_DDOWN)) {
        if ((u32)++curRow > 10) {
            curRow = 0;
        }
        curSection = sRowFirstSections[curRow];
    } else if (CHECK_BTN_ANY(dBtnInput, BTN_DUP)) {
        if (--curRow < 0) {
            curRow = 22;
        }
        curSection = sRowFirstSections[curRow];
    } else if (CHECK_BTN_ANY(dBtnInput, BTN_DLEFT)) {
        if (--curSection < 0) {
            curSection = 0x5C;
        }
    } else if (CHECK_BTN_ANY(dBtnInput, BTN_DRIGHT)) {
        if (++curSection > 0x5C) {
            curSection = 0;
        }
    }

    // Handles the logic to change values based on the selected section
    switch (curSection) {
        case 0:
            if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                gSaveContext.rupees -= 100;
                if (gSaveContext.rupees < 0) {
                    gSaveContext.rupees = 0;
                }
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
                gSaveContext.rupees += 100;
                if (gSaveContext.rupees >= 9999) {
                    gSaveContext.rupees = 9999;
                }
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                gSaveContext.rupees--;
                if (gSaveContext.rupees < 0) {
                    gSaveContext.rupees = 0;
                }
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                gSaveContext.rupees++;
                if (gSaveContext.rupees >= 9999) {
                    gSaveContext.rupees = 9999;
                }
            }
            break;

        case 1:
            if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                gSaveContext.healthCapacity -= 0x10;
                if (gSaveContext.healthCapacity < 0x30) {
                    gSaveContext.healthCapacity = 0x30;
                }
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                       CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                gSaveContext.healthCapacity += 0x10;
                if (gSaveContext.healthCapacity >= 0x140) {
                    gSaveContext.healthCapacity = 0x140;
                }
            }
            break;

        case 2:
            if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                Health_ChangeBy(play, -4);
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                Health_ChangeBy(play, 4);
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                Health_ChangeBy(play, -0x10);
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
                Health_ChangeBy(play, 0x10);
            }
            break;

        case 0x5C:
            if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                if ((((gSaveContext.inventory.questItems & 0xF0000000) & 0xF0000000) >> 0x1C) != 0) {
                    gSaveContext.inventory.questItems -= 0x10000000;
                }
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                       CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                if ((gSaveContext.inventory.questItems & 0xF0000000) <= 0x40000000) {
                    gSaveContext.inventory.questItems += 0x10000000;
                }
            }
            break;

        default:
            if (curSection < 0x1B) {
                i = curSection - 3;
                if ((i <= SLOT_BOW) || (i == SLOT_SLINGSHOT) || (i == SLOT_BOMBCHU) || (i == SLOT_BEAN)) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                        Inventory_DeleteItem(gAmmoItems[i], SLOT(gAmmoItems[i]));
                        AMMO(gAmmoItems[i]) = 0;
                    }

                    if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        if (i != INV_CONTENT(gAmmoItems[i])) {
                            INV_CONTENT(gAmmoItems[i]) = gAmmoItems[i];
                        }
                        AMMO(gAmmoItems[i])++;
                        if (AMMO(gAmmoItems[i]) > 99) {
                            AMMO(gAmmoItems[i]) = 99;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        AMMO(gAmmoItems[i])--;
                        if (AMMO(gAmmoItems[i]) < 0) {
                            AMMO(gAmmoItems[i]) = 0;
                        }
                    }
                } else if (i == SLOT_OCARINA) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                        Inventory_DeleteItem(ITEM_OCARINA_FAIRY, SLOT(ITEM_OCARINA_FAIRY));
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        if (gSaveContext.inventory.items[i] == ITEM_NONE) {
                            gSaveContext.inventory.items[i] = ITEM_OCARINA_FAIRY;
                        } else if ((gSaveContext.inventory.items[i] >= ITEM_OCARINA_FAIRY) &&
                                   (gSaveContext.inventory.items[i] < ITEM_OCARINA_TIME)) {
                            gSaveContext.inventory.items[i]++;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        if (gSaveContext.inventory.items[i] == ITEM_NONE) {
                            gSaveContext.inventory.items[i] = ITEM_OCARINA_TIME;
                        } else if ((gSaveContext.inventory.items[i] > ITEM_OCARINA_FAIRY) &&
                                   (gSaveContext.inventory.items[i] <= ITEM_OCARINA_TIME)) {
                            gSaveContext.inventory.items[i]--;
                        }
                    }
                } else if (i == SLOT_HOOKSHOT) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                        Inventory_DeleteItem(ITEM_HOOKSHOT, SLOT(ITEM_HOOKSHOT));
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        if (gSaveContext.inventory.items[i] == ITEM_NONE) {
                            gSaveContext.inventory.items[i] = ITEM_HOOKSHOT;
                        } else if ((gSaveContext.inventory.items[i] >= ITEM_HOOKSHOT) &&
                                   (gSaveContext.inventory.items[i] < ITEM_LONGSHOT)) {
                            gSaveContext.inventory.items[i]++;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        if (gSaveContext.inventory.items[i] == ITEM_NONE) {
                            gSaveContext.inventory.items[i] = ITEM_LONGSHOT;
                        } else if ((gSaveContext.inventory.items[i] > ITEM_HOOKSHOT) &&
                                   (gSaveContext.inventory.items[i] <= ITEM_LONGSHOT)) {
                            gSaveContext.inventory.items[i]--;
                        }
                    }
                } else if (i == SLOT_TRADE_ADULT) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                        Inventory_DeleteItem(ITEM_POCKET_EGG, SLOT(ITEM_POCKET_EGG));
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        if (gSaveContext.inventory.items[i] == ITEM_NONE) {
                            gSaveContext.inventory.items[i] = ITEM_POCKET_EGG;
                        } else if ((gSaveContext.inventory.items[i] >= ITEM_POCKET_EGG) &&
                                   (gSaveContext.inventory.items[i] < ITEM_CLAIM_CHECK)) {
                            gSaveContext.inventory.items[i]++;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        if (gSaveContext.inventory.items[i] == ITEM_NONE) {
                            gSaveContext.inventory.items[i] = ITEM_CLAIM_CHECK;
                        } else if ((gSaveContext.inventory.items[i] > ITEM_POCKET_EGG) &&
                                   (gSaveContext.inventory.items[i] <= ITEM_CLAIM_CHECK)) {
                            gSaveContext.inventory.items[i]--;
                        }
                    }
                } else if (i == SLOT_TRADE_CHILD) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                        Inventory_DeleteItem(ITEM_WEIRD_EGG, SLOT(ITEM_WEIRD_EGG));
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        if (gSaveContext.inventory.items[i] == ITEM_NONE) {
                            gSaveContext.inventory.items[i] = ITEM_WEIRD_EGG;
                        } else if ((gSaveContext.inventory.items[i] >= ITEM_WEIRD_EGG) &&
                                   (gSaveContext.inventory.items[i] < ITEM_SOLD_OUT)) {
                            gSaveContext.inventory.items[i]++;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        if (gSaveContext.inventory.items[i] == ITEM_NONE) {
                            gSaveContext.inventory.items[i] = ITEM_SOLD_OUT;
                        } else if ((gSaveContext.inventory.items[i] > ITEM_WEIRD_EGG) &&
                                   (gSaveContext.inventory.items[i] <= ITEM_SOLD_OUT)) {
                            gSaveContext.inventory.items[i]--;
                        }
                    }
                } else if ((i >= SLOT_BOTTLE_1) && (i <= SLOT_BOTTLE_4)) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                        Inventory_DeleteItem(ITEM_BOTTLE + i - SLOT_BOTTLE_1, SLOT(ITEM_BOTTLE) + i - SLOT_BOTTLE_1);
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        if (gSaveContext.inventory.items[i] == ITEM_NONE) {
                            gSaveContext.inventory.items[i] = ITEM_BOTTLE;
                        } else if ((gSaveContext.inventory.items[i] >= ITEM_BOTTLE) &&
                                   (gSaveContext.inventory.items[i] <= ITEM_MILK_HALF)) {
                            gSaveContext.inventory.items[i]++;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        if (gSaveContext.inventory.items[i] == ITEM_NONE) {
                            gSaveContext.inventory.items[i] = ITEM_POE;
                        } else if ((gSaveContext.inventory.items[i] >= ITEM_POTION_RED) &&
                                   (gSaveContext.inventory.items[i] <= ITEM_POE)) {
                            gSaveContext.inventory.items[i]--;
                        }
                    }
                } else if (i < 0x1B) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT) ||
                        CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                        CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        if (i == SLOT_TRADE_ADULT) {
                            if (gSaveContext.inventory.items[i] == ITEM_NONE) {
                                gSaveContext.inventory.items[i] = ITEM_BEAN;
                            } else {
                                Inventory_DeleteItem(ITEM_BEAN, SLOT(ITEM_BEAN));
                            }
                        } else {
                            j = sSlotItems[i];
                            osSyncPrintf("i=%d  j=%d\n", i, j);
                            if (gSaveContext.inventory.items[i] == ITEM_NONE) {
                                gSaveContext.inventory.items[i] = j;
                            } else {
                                Inventory_DeleteItem(j, i);
                            }
                        }
                    }
                }
            } else if (curSection < 0x2C) {
                if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    i = curSection - 0x1B;
                    gSaveContext.inventory.dungeonKeys[i]--;
                    if (gSaveContext.inventory.dungeonKeys[i] < 0) {
                        gSaveContext.inventory.dungeonKeys[i] = -1;
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                           CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                    i = curSection - 0x1B;
                    if (gSaveContext.inventory.dungeonKeys[i] < 0) {
                        gSaveContext.inventory.dungeonKeys[i] = 1;
                    } else {
                        gSaveContext.inventory.dungeonKeys[i]++;
                        if (gSaveContext.inventory.dungeonKeys[i] >= 9) {
                            gSaveContext.inventory.dungeonKeys[i] = 9;
                        }
                    }
                }
            } else {
                if (curSection < 0x38) {
                    i = curSection - 0x2C;
                    if ((curSection >= 0x2C) && (curSection < 0x34)) {
                        if (CHECK_BTN_ALL(input->press.button, BTN_CUP) ||
                            CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                            if (CUR_UPG_VALUE(i) != 0) {
                                Inventory_ChangeUpgrade(i, CUR_UPG_VALUE(i) - 1);
                            }
                        } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                                   CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                            if (CUR_UPG_VALUE(i) < sMaxUpgradeValues[i]) {
                                Inventory_ChangeUpgrade(i, CUR_UPG_VALUE(i) + 1);
                            }
                        }
                    } else {
                        i = curSection - 0x34;
                        if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                            gSaveContext.inventory.equipment ^= (1 << gEquipShifts[i]);
                        }
                        if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
                            gSaveContext.inventory.equipment ^= (2 << gEquipShifts[i]);
                        }
                        if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                            gSaveContext.inventory.equipment ^= (4 << gEquipShifts[i]);
                        }
                        if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                            gSaveContext.inventory.equipment ^= (8 << gEquipShifts[i]);
                        }
                    }
                } else if (curSection < 0x44) {
                    i = curSection - 0x38;
                    if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        gSaveContext.inventory.dungeonItems[i] ^= 4;
                    }
                    if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
                        gSaveContext.inventory.dungeonItems[i] ^= 2;
                    }
                    if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        gSaveContext.inventory.dungeonItems[i] ^= 1;
                    }
                } else if (curSection == 0x5B) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        gSaveContext.inventory.gsTokens++;
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                               CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        gSaveContext.inventory.gsTokens--;
                        if (gSaveContext.inventory.gsTokens <= 0) {
                            gSaveContext.inventory.gsTokens = 0;
                        }
                    }
                } else if (curSection < 0x5C) {
                    i = curSection - 0x44;
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        gSaveContext.inventory.questItems ^= gBitFlags[i];
                    }
                }
            }
            break;
    }

    // Draws a highlight on the selected section
    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetRenderMode(POLY_KAL_DISP++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineMode(POLY_KAL_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 0, 0, 200, 120);

    if (curSection == 0) {
        gDPFillRectangle(POLY_KAL_DISP++, sSectionPositions[curSection][0], sSectionPositions[curSection][1],
                         sSectionPositions[curSection][0] + 45, sSectionPositions[curSection][1] + 16);
    } else if ((curSection >= 0x1B) || (curSection == 0x5B)) {
        gDPFillRectangle(POLY_KAL_DISP++, sSectionPositions[curSection][0] - 2, sSectionPositions[curSection][1],
                         sSectionPositions[curSection][0] + 14, sSectionPositions[curSection][1] + 16);
    } else {
        gDPFillRectangle(POLY_KAL_DISP++, sSectionPositions[curSection][0] - 4, sSectionPositions[curSection][1],
                         sSectionPositions[curSection][0] + 24, sSectionPositions[curSection][1] + 16);
    }

    // Handles exiting the inventory editor with the L button
    // The editor is opened with `debugState` set to 1, and becomes closable after a frame once `debugState` is set to 2
    s16 Debug_BTN = BTN_L;
    if (CVar_GetS32("gNGCKaleidoSwitcher", 0) != 0) {
        Debug_BTN = BTN_Z;
    }
    if (pauseCtx->debugState == 1) {
        pauseCtx->debugState = 2;
    } else if ((pauseCtx->debugState == 2) && CHECK_BTN_ALL(input->press.button, Debug_BTN)) {
        pauseCtx->debugState = 0;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
