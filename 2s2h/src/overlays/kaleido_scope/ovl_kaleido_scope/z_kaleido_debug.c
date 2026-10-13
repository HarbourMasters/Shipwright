/*
 * File: z_kaleido_debug.c
 * Overlay: ovl_kaleido_scope
 * Description: Inventory Editor
 */

#include "z_kaleido_scope.h"
#include "interface/parameter_static/parameter_static.h"

s16 sCurSection = 0;
s16 sCurRow = 0;

// These defines must line up with the indices of sSectionPositions
#define INV_EDITOR_SECTION_RUPEE 0
#define INV_EDITOR_SECTION_HEALTH_CAPACITY 1
#define INV_EDITOR_SECTION_HEALTH 2
#define INV_EDITOR_SECTION_ITEMS 3
#define INV_EDITOR_SECTION_MASKS 27
#define INV_EDITOR_SECTION_BOSS 51
#define INV_EDITOR_SECTION_SWORD 55
#define INV_EDITOR_SECTION_SHIELD 56
#define INV_EDITOR_SECTION_SONGS 57
#define INV_EDITOR_SECTION_LULLABY_INTRO 69
#define INV_EDITOR_SECTION_NOTEBOOK 70
#define INV_EDITOR_SECTION_QUIVER 71
#define INV_EDITOR_SECTION_BOMB_BAG 72
#define INV_EDITOR_SECTION_SWAMP_GOLD_SKULLS 73
#define INV_EDITOR_SECTION_OCEAN_GOLD_SKULLS 74
#define INV_EDITOR_SECTION_HEART_PIECE_COUNT 75
#define INV_EDITOR_SECTION_SMALL_KEYS 76
#define INV_EDITOR_SECTION_DUNGEON_ITEMS 80
#define INV_EDITOR_SECTION_STRAY_FAIRIES 84
#define INV_EDITOR_SECTION_DOUBLE_DEFENSE 88

// Geometry of the highlights for the selected section
typedef struct {
    /* 0x0 */ s16 screenX;
    /* 0x2 */ s16 screenY;
    /* 0x4 */ s16 width;
} SectionPosition; // size = 0x6

// clang-format off
SectionPosition sSectionPositions[] = {
    // Rupees (0)
    { 65, 14, 41 },
    // Health Capacity (1)
    { 145, 14, 21 },
    // Health (2)
    { 193, 14, 35 },
    // Items (3)
    { 42, 37, 22 },   { 65, 37, 22 },   { 88, 37, 22 }, { 111, 37, 22 },  { 134, 37, 22 },  { 157, 37, 22 }, // row 1
    { 42, 51, 22 },   { 65, 51, 22 },   { 88, 51, 22 }, { 111, 51, 22 },  { 134, 51, 22 },  { 157, 51, 22 }, // row 2
    { 42, 65, 22 },   { 65, 65, 22 },   { 88, 65, 22 }, { 111, 65, 22 },  { 134, 65, 22 },  { 157, 65, 22 }, // row 3
    { 42, 80, 22 },   { 65, 80, 22 },   { 88, 80, 22 }, { 111, 80, 22 },  { 134, 80, 22 },  { 157, 80, 22 }, // row 4
    // Masks (27)
    { 202, 37, 14 },  { 215, 37, 14 },  { 229, 37, 14 }, { 243, 37, 14 },  { 257, 37, 14 },  { 271, 37, 14 }, // row 1
    { 202, 51, 14 },  { 215, 51, 14 },  { 229, 51, 14 }, { 243, 51, 14 },  { 257, 51, 14 },  { 271, 51, 14 }, // row 2
    { 202, 65, 14 },  { 215, 65, 14 },  { 229, 65, 14 }, { 243, 65, 14 },  { 257, 65, 14 },  { 271, 65, 14 }, // row 3
    { 202, 80, 14 },  { 215, 80, 14 },  { 229, 80, 14 }, { 243, 80, 14 },  { 257, 80, 14 },  { 271, 80, 14 }, // row 4
    // Boss (51)
    { 42, 111, 14 },  { 54, 111, 14 },  { 66, 111, 14 }, { 78, 111, 14 },
    // Sword (55)
    { 114, 99, 14 },
    // Shield (56)
    { 142, 99, 14 },
    // Songs (57)
    { 188, 99, 14 },  { 202, 99, 14 },  { 216, 99, 14 }, { 230, 99, 14 },  { 244, 99, 14 },  { 258, 99, 14 },
    { 188, 113, 14 }, { 202, 113, 14 }, { 216, 113, 14 }, { 230, 113, 14 }, { 244, 113, 14 }, { 258, 113, 14 },
    // Goron Lullaby Intro (69)
    { 272, 113, 14 },
    // Bombers Notebook (70)
    { 54, 141, 14 },
    // Quiver Upgrade (71)
    { 112, 134, 14 },
    // Bomb Bag Upgrade (72)
    { 155, 134, 14 },
    // Gold Skulltula Count (73)
    { 194, 130, 32 }, { 194, 144, 32 },
    // Heart Piece Count (75)
    { 262, 134, 14 },
    // Small Keys (76)
    { 43, 168, 14 },  { 55, 168, 14 }, { 67, 168, 14 },  { 79, 168, 14 },
    // Dungeon Items (80)
    { 127, 168, 14 }, { 139, 168, 14 }, { 151, 168, 14 }, { 163, 168, 14 },
    // Stray Fairies (84)
    { 202, 168, 22 }, { 223, 168, 22 }, { 244, 168, 22 }, { 265, 168, 22 },
    // Life (88)
    { 42, 202, 14 },
    // Magic
    { 73, 202, 14 },
    // Lottery
    { 136, 202, 38 },
    // Gold Color/Bombers Code (not highlighted properly)
    { 214, 202, 74 }, { 214, 202, 74 },
};
// clang-format on

s16 sSlotItems[] = {
    // Items Row 1
    ITEM_OCARINA_OF_TIME,
    ITEM_BOW,
    ITEM_ARROW_FIRE,
    ITEM_ARROW_ICE,
    ITEM_ARROW_LIGHT,
    ITEM_OCARINA_FAIRY,
    // Items Row 2
    ITEM_BOMB,
    ITEM_BOMBCHU,
    ITEM_DEKU_STICK,
    ITEM_DEKU_NUT,
    ITEM_MAGIC_BEANS,
    ITEM_SLINGSHOT,
    // Items Row 3
    ITEM_POWDER_KEG,
    ITEM_PICTOGRAPH_BOX,
    ITEM_LENS_OF_TRUTH,
    ITEM_HOOKSHOT,
    ITEM_SWORD_GREAT_FAIRY,
    ITEM_LONGSHOT,
    // Items Row 4
    ITEM_BOTTLE,
    ITEM_POTION_RED,
    ITEM_POTION_GREEN,
    ITEM_POTION_BLUE,
    ITEM_FAIRY,
    ITEM_MUSHROOM,
    // Masks Row 1
    ITEM_MASK_POSTMAN,
    ITEM_MASK_ALL_NIGHT,
    ITEM_MASK_BLAST,
    ITEM_MASK_STONE,
    ITEM_MASK_GREAT_FAIRY,
    ITEM_MASK_DEKU,
    // Masks Row 2
    ITEM_MASK_KEATON,
    ITEM_MASK_BREMEN,
    ITEM_MASK_BUNNY,
    ITEM_MASK_DON_GERO,
    ITEM_MASK_SCENTS,
    ITEM_MASK_GORON,
    // Masks Row 3
    ITEM_MASK_ROMANI,
    ITEM_MASK_CIRCUS_LEADER,
    ITEM_MASK_KAFEIS_MASK,
    ITEM_MASK_COUPLE,
    ITEM_MASK_TRUTH,
    ITEM_MASK_ZORA,
    // Masks Row 4
    ITEM_MASK_KAMARO,
    ITEM_MASK_GIBDO,
    ITEM_MASK_GARO,
    ITEM_MASK_CAPTAIN,
    ITEM_MASK_GIANT,
    ITEM_MASK_FIERCE_DEITY,
};

s16 sRowFirstSections[] = {
    INV_EDITOR_SECTION_RUPEE,
    INV_EDITOR_SECTION_HEALTH_CAPACITY,
    INV_EDITOR_SECTION_HEALTH,
    INV_EDITOR_SECTION_ITEMS,
    INV_EDITOR_SECTION_MASKS,
    INV_EDITOR_SECTION_BOSS,
    INV_EDITOR_SECTION_SWORD,
    INV_EDITOR_SECTION_SHIELD,
    INV_EDITOR_SECTION_SONGS,
    INV_EDITOR_SECTION_NOTEBOOK,
    INV_EDITOR_SECTION_QUIVER,
    INV_EDITOR_SECTION_BOMB_BAG,
    INV_EDITOR_SECTION_SWAMP_GOLD_SKULLS,
    INV_EDITOR_SECTION_HEART_PIECE_COUNT,
    INV_EDITOR_SECTION_SMALL_KEYS,
    INV_EDITOR_SECTION_DUNGEON_ITEMS,
    INV_EDITOR_SECTION_STRAY_FAIRIES,
    INV_EDITOR_SECTION_DOUBLE_DEFENSE,
};

void KaleidoScope_DrawInventoryEditorText(Gfx** gfxP) {
    GfxPrint printer;
    s32 pad[2];

    GfxPrint_Init(&printer);
    GfxPrint_Open(&printer, *gfxP);

    // Rupees
    GfxPrint_SetPos(&printer, 4, 2);
    GfxPrint_SetColor(&printer, 255, 60, 0, 255);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ﾙﾋﾟｰ");

    // Hearts
    GfxPrint_SetPos(&printer, 15, 2);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ﾊｰﾄ");
    GfxPrint_SetPos(&printer, 15, 3);
    GfxPrint_Printf(&printer, "%s", "MAX");
    GfxPrint_SetPos(&printer, 21, 3);
    GfxPrint_Printf(&printer, "%s", "NOW");
    GfxPrint_SetPos(&printer, 28, 3);
    GfxPrint_Printf(&printer, "%s", "/4");

    // Items
    GfxPrint_SetPos(&printer, 4, 5);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｱ");
    GfxPrint_SetPos(&printer, 4, 6);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｲ");
    GfxPrint_SetPos(&printer, 4, 7);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ﾃ");
    GfxPrint_SetPos(&printer, 4, 8);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ﾑ");

    // Masks
    GfxPrint_SetPos(&printer, 24, 5);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ﾏ");
    GfxPrint_SetPos(&printer, 24, 6);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｽ");
    GfxPrint_SetPos(&printer, 24, 7);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｸ");

    // Boss (Remains)
    GfxPrint_SetPos(&printer, 4, 13);
    GfxPrint_Printf(&printer, "%s", "BOSS");

    // Sword
    GfxPrint_SetPos(&printer, 13, 13);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｹ");
    GfxPrint_SetPos(&printer, 13, 14);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ﾝ");

    // Shield
    GfxPrint_SetPos(&printer, 17, 13);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ﾀ");
    GfxPrint_SetPos(&printer, 17, 14);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ﾃ");

    // Ocarina (Songs)
    GfxPrint_SetPos(&printer, 21, 13);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｵｶ");
    GfxPrint_SetPos(&printer, 21, 14);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ﾘﾅ");

    // Schedule (Bombers Notebook)
    GfxPrint_SetPos(&printer, 4, 17);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｽｹｼﾞｭｰﾙ");

    // Quiver
    GfxPrint_SetPos(&printer, 12, 17);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ﾔ");
    GfxPrint_SetPos(&printer, 12, 18);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ﾀﾃ");

    // Bomb Bag
    GfxPrint_SetPos(&printer, 16, 17);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ﾎﾞﾑ");
    GfxPrint_SetPos(&printer, 16, 18);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ﾌｸﾛ");

    // Gold Skulls
    GfxPrint_SetPos(&printer, 22, 17);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｷﾝ");
    GfxPrint_SetPos(&printer, 22, 18);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｽﾀ");

    // Heart Pieces
    GfxPrint_SetPos(&printer, 29, 17);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ﾊｰﾄ");
    GfxPrint_SetPos(&printer, 29, 18);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｶｹﾗ");
    GfxPrint_SetPos(&printer, 34, 18);
    GfxPrint_Printf(&printer, "%s", "/4");

    // Keys
    GfxPrint_SetPos(&printer, 4, 21);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｶ");
    GfxPrint_SetPos(&printer, 4, 22);
    GfxPrint_Printf(&printer, "%s", GFXP_KATAKANA "ｷﾞ");

    // Map/Compass
    GfxPrint_SetPos(&printer, 12, 21);
    GfxPrint_Printf(&printer, "%s", "MAP");
    GfxPrint_SetPos(&printer, 12, 22);
    GfxPrint_Printf(&printer, "%s", "ｺﾝﾊﾟ");

    // Fairies
    GfxPrint_SetPos(&printer, 23, 21);
    GfxPrint_Printf(&printer, "%s", "ﾖｳ");
    GfxPrint_SetPos(&printer, 23, 22);
    GfxPrint_Printf(&printer, "%s", "ｾｲ");

    // Life (double defense)
    GfxPrint_SetPos(&printer, 4, 25);
    GfxPrint_Printf(&printer, "%s", "ｲ");
    GfxPrint_SetPos(&printer, 4, 26);
    GfxPrint_Printf(&printer, "%s", "ﾉ");
    GfxPrint_SetPos(&printer, 4, 27);
    GfxPrint_Printf(&printer, "%s", "ﾁ");

    // Magic
    GfxPrint_SetPos(&printer, 8, 25);
    GfxPrint_Printf(&printer, "%s", "ﾏ");
    GfxPrint_SetPos(&printer, 8, 26);
    GfxPrint_Printf(&printer, "%s", "ﾎ");
    GfxPrint_SetPos(&printer, 8, 27);
    GfxPrint_Printf(&printer, "%s", "ｳ");

    // Lottery
    GfxPrint_SetPos(&printer, 13, 25);
    GfxPrint_Printf(&printer, "%s", "ﾀｶﾗ");
    GfxPrint_SetPos(&printer, 13, 26);
    GfxPrint_Printf(&printer, "%s", "ｸｼﾞ");

    // Gold Color (Oceanside Spider House Mask Order)
    GfxPrint_SetPos(&printer, 23, 24);
    GfxPrint_Printf(&printer, "%s", "ｷﾝｲﾛ");

    // Bombers (code)
    GfxPrint_SetPos(&printer, 23, 26);
    GfxPrint_Printf(&printer, "%s", "ﾎﾞﾝ");
    GfxPrint_SetPos(&printer, 23, 27);
    GfxPrint_Printf(&printer, "%s", "ﾊﾞｰｽﾞ");

    *gfxP = GfxPrint_Close(&printer);
    GfxPrint_Destroy(&printer);
}
// 2S2H [Port] (and in the function) don't do pointer math and access the list of digits directly.
extern const char* sCounterTextures[];

void KaleidoScope_DrawDigit(PlayState* play, s32 digit, s32 rectLeft, s32 rectTop) {
    OPEN_DISPS(play->state.gfxCtx);

    gDPLoadTextureBlock(POLY_OPA_DISP++, sCounterTextures[digit], G_IM_FMT_I, G_IM_SIZ_8b, 8, 16, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSPTextureRectangle(POLY_OPA_DISP++, rectLeft << 2, rectTop << 2, (rectLeft + 8) << 2, (rectTop + 16) << 2,
                        G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

    CLOSE_DISPS(play->state.gfxCtx);
}

void KaleidoScope_DrawInventoryEditor(PlayState* play) {
    Gfx* gfx;
    Gfx* gfxRef;
    s32 counterDigits[4];
    s16 slot;
    s16 i;
    s16 j;
    s32 rectLeft;
    s32 rectTop;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL39_Opa(play->state.gfxCtx);

    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, 220);
    gDPFillRectangle(POLY_OPA_DISP++, 24, 12, 298, 228);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);

    gfxRef = POLY_OPA_DISP;
    gfx = Graph_GfxPlusOne(gfxRef);
    gSPDisplayList(OVERLAY_DISP++, gfx);

    KaleidoScope_DrawInventoryEditorText(&gfx);

    gSPEndDisplayList(gfx++);
    Graph_BranchDlist(gfxRef, gfx);
    POLY_OPA_DISP = gfx;

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 0, 0, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    // Current Health Quarter (X / 4)
    KaleidoScope_DrawDigit(play, (((void)0, gSaveContext.save.saveInfo.playerData.health) % 0x10) / 4, 217, 15);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

    // Rupees
    counterDigits[0] = counterDigits[1] = counterDigits[2] = 0;
    counterDigits[3] = gSaveContext.save.saveInfo.playerData.rupees;
    while (counterDigits[3] >= 1000) {
        counterDigits[0]++;
        counterDigits[3] -= 1000;
    }

    while (counterDigits[3] >= 100) {
        counterDigits[1]++;
        counterDigits[3] -= 100;
    }

    while (counterDigits[3] >= 10) {
        counterDigits[2]++;
        counterDigits[3] -= 10;
    }

    // Loop over columns (i)
    for (i = 0, rectLeft = 68; i < 4; i++, rectLeft += 9) {
        KaleidoScope_DrawDigit(play, counterDigits[i], rectLeft, 15);
    }

    // Health capacity
    counterDigits[2] = 0;
    counterDigits[3] = gSaveContext.save.saveInfo.playerData.healthCapacity / 0x10;
    while (counterDigits[3] >= 10) {
        counterDigits[2]++;
        counterDigits[3] -= 10;
    }

    KaleidoScope_DrawDigit(play, counterDigits[2], 145, 15);
    KaleidoScope_DrawDigit(play, counterDigits[3], 154, 15);

    // Health
    counterDigits[2] = 0;
    counterDigits[3] = gSaveContext.save.saveInfo.playerData.health / 0x10;
    while (counterDigits[3] >= 10) {
        counterDigits[2]++;
        counterDigits[3] -= 10;
    }

    KaleidoScope_DrawDigit(play, counterDigits[2], 195, 15);
    KaleidoScope_DrawDigit(play, counterDigits[3], 204, 15);

    // Items
    // Loop over rows (i)
    for (slot = 0, i = 0, rectTop = 38; i < ITEM_GRID_ROWS; i++, rectTop += 14) {
        // Loop over columns (j)
        for (j = 0, rectLeft = 44; j < ITEM_GRID_COLS; j++, slot++, rectLeft += 23) {
            counterDigits[3] = 0;
            counterDigits[2] = 0;
            if ((slot == SLOT_BOW) || ((slot >= SLOT_BOMB) && (slot <= SLOT_DEKU_NUT)) || (slot == SLOT_POWDER_KEG) ||
                (slot == SLOT_MAGIC_BEANS)) {
                counterDigits[3] = AMMO(gAmmoItems[slot]);
            } else if ((slot == SLOT_TRADE_DEED) || (slot == SLOT_TRADE_KEY_MAMA) || (slot == SLOT_TRADE_COUPLE)) {
                counterDigits[3] = gSaveContext.save.saveInfo.inventory.items[slot];
            } else if (slot >= SLOT_BOTTLE_1) {
                counterDigits[3] = gSaveContext.save.saveInfo.inventory.items[slot];
            } else if (gSaveContext.save.saveInfo.inventory.items[slot] != ITEM_NONE) {
                counterDigits[3] = 1;
            }

            if (counterDigits[3] != ITEM_NONE) {
                while (counterDigits[3] >= 10) {
                    counterDigits[2]++;
                    counterDigits[3] -= 10;
                }
            } else {
                counterDigits[2] = counterDigits[3] = 0;
            }

            if (counterDigits[2] != 0) {
                KaleidoScope_DrawDigit(play, counterDigits[2], rectLeft, rectTop);
            }
            KaleidoScope_DrawDigit(play, counterDigits[3], rectLeft + 9, rectTop);
        }
    }

    // Masks
    // Loop over rows (i)
    for (slot = 0, i = 0, rectTop = 38; i < MASK_GRID_ROWS; i++, rectTop += 14) {
        // Loop over columns (j)
        for (j = 0, rectLeft = 204; j < MASK_GRID_COLS; j++, slot++, rectLeft += 14) {
            counterDigits[2] = 0;

            if (gSaveContext.save.saveInfo.inventory.items[slot + ITEM_NUM_SLOTS] != ITEM_NONE) {
                counterDigits[2] = 1;
            }
            KaleidoScope_DrawDigit(play, counterDigits[2], rectLeft, rectTop);
        }
    }

    // Boss Remains
    // Loop over columns (i), (counterDigits[1] stores rectLeft)
    for (counterDigits[1] = 44, i = 0; i < 4; i++) {
        counterDigits[2] = 0;
        if (CHECK_QUEST_ITEM(QUEST_REMAINS_ODOLWA + i)) {
            counterDigits[2] = 1;
        }
        KaleidoScope_DrawDigit(play, counterDigits[2], counterDigits[1], 112);
        counterDigits[1] += 14;
    }

    // Sword
    KaleidoScope_DrawDigit(play, GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD), 116, 100);

    // Shield
    KaleidoScope_DrawDigit(play, GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD), 145, 100);

    // Songs
    // Loop over rows (j), (counterDigits[3] stores rectTop)
    for (counterDigits[3] = 100, j = 0, slot = 0; j < 2; j++) {
        // Loop over columns (i), (counterDigits[1] stores rectLeft)
        for (counterDigits[1] = 190, i = 0; i < 6; i++, slot++) {
            counterDigits[2] = 0;
            if (CHECK_QUEST_ITEM(QUEST_SONG_SONATA + slot)) {
                counterDigits[2] = 1;
            }
            KaleidoScope_DrawDigit(play, counterDigits[2], counterDigits[1], counterDigits[3]);
            counterDigits[1] += 14;
        }
        counterDigits[3] += 14;
    }

    // Goron Lullaby Intro
    counterDigits[2] = 0;
    counterDigits[3] -= 14;
    if (CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO)) {
        counterDigits[2] = 1;
    }
    KaleidoScope_DrawDigit(play, counterDigits[2], counterDigits[1], counterDigits[3]);

    // Bombers Notebook
    counterDigits[2] = 0;
    if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
        counterDigits[2] = 1;
    }
    KaleidoScope_DrawDigit(play, counterDigits[2], 56, 142);

    // Quiver
    KaleidoScope_DrawDigit(play, CUR_UPG_VALUE(UPG_QUIVER), 114, 134);

    // Bomb Bag
    KaleidoScope_DrawDigit(play, CUR_UPG_VALUE(UPG_BOMB_BAG), 156, 134);

    // Gold Skulls (Swamp Spider House)
    counterDigits[2] = 0;
    counterDigits[1] = 0;
    counterDigits[3] = Inventory_GetSkullTokenCount(SCENE_KINSTA1);

    while (counterDigits[3] >= 100) {
        counterDigits[3] -= 100;
        counterDigits[1]++;
    }

    while (counterDigits[3] >= 10) {
        counterDigits[3] -= 10;
        counterDigits[2]++;
    }

    KaleidoScope_DrawDigit(play, counterDigits[1], 196, 130);
    KaleidoScope_DrawDigit(play, counterDigits[2], 205, 130);
    KaleidoScope_DrawDigit(play, counterDigits[3], 214, 130);

    // Gold Skulls (Oceans Spider House)
    counterDigits[2] = 0;
    counterDigits[1] = 0;
    counterDigits[3] = Inventory_GetSkullTokenCount(SCENE_KINDAN2);

    while (counterDigits[3] >= 100) {
        counterDigits[3] -= 100;
        counterDigits[1]++;
    }

    while (counterDigits[3] >= 10) {
        counterDigits[3] -= 10;
        counterDigits[2]++;
    }

    KaleidoScope_DrawDigit(play, counterDigits[1], 196, 144);
    KaleidoScope_DrawDigit(play, counterDigits[2], 205, 144);
    KaleidoScope_DrawDigit(play, counterDigits[3], 214, 144);

    // Heart Pieces
    KaleidoScope_DrawDigit(play, GET_QUEST_HEART_PIECE_COUNT, 264, 134);

    // Keys
    // Loop over columns (i), (counterDigits[1] stores rectLeft)
    for (counterDigits[1] = 45, i = 0; i < 4; i++) {
        counterDigits[2] = 0;

        if ((counterDigits[3] = DUNGEON_KEY_COUNT(i)) >= 0) {
            while (counterDigits[3] >= 10) {
                counterDigits[2]++;
                counterDigits[3] -= 10;
            }
        } else {
            counterDigits[2] = counterDigits[3] = 0;
        }

        KaleidoScope_DrawDigit(play, counterDigits[3], counterDigits[1], 168);
        counterDigits[1] += 12;
    }

    // Dungeon Items
    // Loop over columns (i), (counterDigits[1] stores rectLeft)
    for (counterDigits[1] = 129, i = 0; i < 4; i++) {
        counterDigits[2] = gSaveContext.save.saveInfo.inventory.dungeonItems[i] & gEquipMasks[0];
        KaleidoScope_DrawDigit(play, counterDigits[2], counterDigits[1], 168);
        counterDigits[1] += 12;
    }

    // Stray Fairies
    // Loop over columns (i), (counterDigits[1] stores rectLeft)
    for (counterDigits[1] = 202, i = 0; i < 4; i++) {
        counterDigits[3] = gSaveContext.save.saveInfo.inventory.strayFairies[i];
        counterDigits[2] = counterDigits[3] / 10;
        counterDigits[3] -= counterDigits[2] * 10;
        if (counterDigits[2] != 0) {
            KaleidoScope_DrawDigit(play, counterDigits[2], counterDigits[1], 168);
        }
        KaleidoScope_DrawDigit(play, counterDigits[3], counterDigits[1] + 9, 168);
        counterDigits[1] += 21;
    }

    // Double Defense
    KaleidoScope_DrawDigit(play, gSaveContext.save.saveInfo.playerData.doubleDefense, 44, 202);

    // Magic
    //! @bug should be gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired
    KaleidoScope_DrawDigit(play, gSaveContext.save.saveInfo.playerData.doubleDefense, 75, 202);

    // Lottery
    // Loop over columns (i), (counterDigits[1] stores rectLeft)
    for (counterDigits[1] = 139, i = 0; i < 3; i++) {
        counterDigits[2] = gSaveContext.save.saveInfo.lotteryCodes[0][i];

        KaleidoScope_DrawDigit(play, counterDigits[2], counterDigits[1], 184);
        counterDigits[2] = gSaveContext.save.saveInfo.lotteryCodes[1][i];
        KaleidoScope_DrawDigit(play, counterDigits[2], counterDigits[1], 198);

        counterDigits[2] = gSaveContext.save.saveInfo.lotteryCodes[2][i];
        KaleidoScope_DrawDigit(play, counterDigits[2], counterDigits[1], 212);

        counterDigits[1] += 12;
    }

    // Oceanside Spider House Mask Order
    // Loop over columns (i), (counterDigits[1] stores rectLeft)
    for (counterDigits[1] = 217, i = 0; i < 6; i++) {
        counterDigits[2] = gSaveContext.save.saveInfo.spiderHouseMaskOrder[i];
        KaleidoScope_DrawDigit(play, counterDigits[2], counterDigits[1], 186);

        counterDigits[1] += 12;
    }

    // Bombers code
    // Loop over columns (i), (counterDigits[1] stores rectLeft)
    for (counterDigits[1] = 220, i = 0; i < 5; i++) {
        counterDigits[2] = gSaveContext.save.saveInfo.bomberCode[i];
        KaleidoScope_DrawDigit(play, counterDigits[2], counterDigits[1], 210);

        counterDigits[1] += 12;
    }

    // Draws a highlight on the selected section
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 200, 120);

    gDPFillRectangle(POLY_OPA_DISP++, sSectionPositions[sCurSection].screenX, sSectionPositions[sCurSection].screenY,
                     sSectionPositions[sCurSection].screenX + sSectionPositions[sCurSection].width,
                     sSectionPositions[sCurSection].screenY + 15);

    CLOSE_DISPS(play->state.gfxCtx);
}

void KaleidoScope_UpdateInventoryEditor(PlayState* play) {
    static s32 sPrevDBtnInput = 0;
    static s32 sHeldDBtnTimer = 0;
    PauseContext* pauseCtx = &play->pauseCtx;
    Input* input = CONTROLLER1(&play->state);
    s16 slot;
    s16 value;
    s32 dBtnInput = input->cur.button & (BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT);

    pauseCtx->stickAdjX = input->rel.stick_x;
    pauseCtx->stickAdjY = input->rel.stick_y;

    // Handles navigating the menu to different sections with the D-Pad
    // When the same direction is held, registers the input periodically based on a timer
    if (dBtnInput == sPrevDBtnInput) {
        sHeldDBtnTimer--;
        if (sHeldDBtnTimer < 0) {
            sHeldDBtnTimer = 1;
        } else {
            dBtnInput ^= sPrevDBtnInput;
        }
    } else {
        sPrevDBtnInput = dBtnInput;
        sHeldDBtnTimer = 16;
    }

    if (CHECK_BTN_ANY(dBtnInput, BTN_DDOWN)) {
        sCurRow++;
        if (sCurRow > 17) {
            sCurRow = 0;
        }
        sCurSection = sRowFirstSections[sCurRow];
    } else if (CHECK_BTN_ANY(dBtnInput, BTN_DUP)) {
        sCurRow--;
        if (sCurRow < 0) {
            sCurRow = 17;
        }
        sCurSection = sRowFirstSections[sCurRow];
    } else if (CHECK_BTN_ANY(dBtnInput, BTN_DLEFT)) {
        sCurSection--;
        if (sCurSection < 0) {
            sCurSection = ARRAY_COUNT(sSectionPositions) - 1;
        }
    } else if (CHECK_BTN_ANY(dBtnInput, BTN_DRIGHT)) {
        sCurSection++;
        if (sCurSection > (ARRAY_COUNT(sSectionPositions) - 1)) {
            sCurSection = 0;
        }
    }

    // Handles the logic to change values based on the selected section
    switch (sCurSection) {
        case INV_EDITOR_SECTION_RUPEE:
            // Rupees
            if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                gSaveContext.save.saveInfo.playerData.rupees -= 100;
                if (gSaveContext.save.saveInfo.playerData.rupees < 0) {
                    gSaveContext.save.saveInfo.playerData.rupees = 0;
                }
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
                gSaveContext.save.saveInfo.playerData.rupees += 100;
                if (gSaveContext.save.saveInfo.playerData.rupees >= 9999) {
                    gSaveContext.save.saveInfo.playerData.rupees = 9999;
                }
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                gSaveContext.save.saveInfo.playerData.rupees--;
                if (gSaveContext.save.saveInfo.playerData.rupees < 0) {
                    gSaveContext.save.saveInfo.playerData.rupees = 0;
                }
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                gSaveContext.save.saveInfo.playerData.rupees++;
                if (gSaveContext.save.saveInfo.playerData.rupees >= 9999) {
                    gSaveContext.save.saveInfo.playerData.rupees = 9999;
                }
            }
            break;

        case INV_EDITOR_SECTION_HEALTH_CAPACITY:
            // Health Capacity
            if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                gSaveContext.save.saveInfo.playerData.healthCapacity -= 0x10;
                if (gSaveContext.save.saveInfo.playerData.healthCapacity < 0x30) {
                    gSaveContext.save.saveInfo.playerData.healthCapacity = 0x30;
                }
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                       CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                gSaveContext.save.saveInfo.playerData.healthCapacity += 0x10;
                if (gSaveContext.save.saveInfo.playerData.healthCapacity >= 0x140) {
                    gSaveContext.save.saveInfo.playerData.healthCapacity = 0x140;
                }
            }
            break;

        case INV_EDITOR_SECTION_HEALTH:
            // Health
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

        case INV_EDITOR_SECTION_HEART_PIECE_COUNT:
            // Heart Piece Count
            if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                if (GET_QUEST_HEART_PIECE_COUNT != 0) {
                    DECREMENT_QUEST_HEART_PIECE_COUNT;
                }
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                       CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                if (LEQ_MAX_QUEST_HEART_PIECE_COUNT) {
                    INCREMENT_QUEST_HEART_PIECE_COUNT;
                }
            }
            break;

        default:
            if (sCurSection < INV_EDITOR_SECTION_BOSS) {
                // Items
                slot = sCurSection - INV_EDITOR_SECTION_ITEMS;
                if ((slot == SLOT_BOW) || ((slot >= SLOT_BOMB) && (slot <= SLOT_DEKU_NUT)) ||
                    (slot == SLOT_POWDER_KEG) || (slot == SLOT_MAGIC_BEANS)) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                        Inventory_DeleteItem(gAmmoItems[slot], SLOT(gAmmoItems[slot]));
                        AMMO(gAmmoItems[slot]) = 0;
                    }

                    if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        if (slot != INV_CONTENT(gAmmoItems[slot])) {
                            INV_CONTENT(gAmmoItems[slot]) = gAmmoItems[slot];
                        }
                        AMMO(gAmmoItems[slot])++;
                        if (AMMO(gAmmoItems[slot]) > 99) {
                            AMMO(gAmmoItems[slot]) = 99;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        AMMO(gAmmoItems[slot])--;
                        if (AMMO(gAmmoItems[slot]) < 0) {
                            AMMO(gAmmoItems[slot]) = 0;
                        }
                    }
                } else if ((slot == SLOT_TRADE_DEED) || (slot == SLOT_TRADE_KEY_MAMA) || (slot == SLOT_TRADE_COUPLE)) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                        value = sSlotItems[slot];
                        Inventory_DeleteItem(value, slot);
                    } else if (slot == SLOT_TRADE_DEED) {
                        if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                            if (INV_CONTENT(ITEM_MOONS_TEAR) == ITEM_NONE) {
                                gSaveContext.save.saveInfo.inventory.items[slot] = ITEM_MOONS_TEAR;
                            } else if ((INV_CONTENT(ITEM_MOONS_TEAR) >= ITEM_MOONS_TEAR) &&
                                       (INV_CONTENT(ITEM_MOONS_TEAR) <= ITEM_DEED_MOUNTAIN)) {
                                gSaveContext.save.saveInfo.inventory.items[slot] = INV_CONTENT(ITEM_MOONS_TEAR) + 1;
                            }
                        } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                            if (INV_CONTENT(ITEM_MOONS_TEAR) == ITEM_NONE) {
                                gSaveContext.save.saveInfo.inventory.items[slot] = ITEM_DEED_OCEAN;
                            } else if ((INV_CONTENT(ITEM_MOONS_TEAR) >= ITEM_DEED_LAND) &&
                                       (INV_CONTENT(ITEM_MOONS_TEAR) <= ITEM_DEED_OCEAN)) {
                                gSaveContext.save.saveInfo.inventory.items[slot] = INV_CONTENT(ITEM_MOONS_TEAR) - 1;
                            }
                        }
                    } else if (slot == SLOT_TRADE_KEY_MAMA) {
                        if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                            if (INV_CONTENT(ITEM_ROOM_KEY) == ITEM_NONE) {
                                gSaveContext.save.saveInfo.inventory.items[slot] = ITEM_ROOM_KEY;
                            } else if ((INV_CONTENT(ITEM_ROOM_KEY) >= ITEM_ROOM_KEY) &&
                                       (INV_CONTENT(ITEM_ROOM_KEY) <= ITEM_ROOM_KEY)) {
                                gSaveContext.save.saveInfo.inventory.items[slot] = INV_CONTENT(ITEM_ROOM_KEY) + 1;
                            }
                        } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                            if (INV_CONTENT(ITEM_ROOM_KEY) == ITEM_NONE) {
                                gSaveContext.save.saveInfo.inventory.items[slot] = ITEM_LETTER_MAMA;
                            } else if ((INV_CONTENT(ITEM_ROOM_KEY) >= ITEM_LETTER_MAMA) &&
                                       (INV_CONTENT(ITEM_ROOM_KEY) <= ITEM_LETTER_MAMA)) {
                                gSaveContext.save.saveInfo.inventory.items[slot] = INV_CONTENT(ITEM_ROOM_KEY) - 1;
                            }
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        if (INV_CONTENT(ITEM_LETTER_TO_KAFEI) == ITEM_NONE) {
                            gSaveContext.save.saveInfo.inventory.items[slot] = ITEM_LETTER_TO_KAFEI;
                        } else if ((INV_CONTENT(ITEM_LETTER_TO_KAFEI) >= ITEM_LETTER_TO_KAFEI) &&
                                   (INV_CONTENT(ITEM_LETTER_TO_KAFEI) <= ITEM_LETTER_TO_KAFEI)) {
                            gSaveContext.save.saveInfo.inventory.items[slot] = INV_CONTENT(ITEM_LETTER_TO_KAFEI) + 1;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        if (INV_CONTENT(ITEM_LETTER_TO_KAFEI) == ITEM_NONE) {
                            gSaveContext.save.saveInfo.inventory.items[slot] = ITEM_PENDANT_OF_MEMORIES;
                        } else if ((INV_CONTENT(ITEM_LETTER_TO_KAFEI) >= ITEM_PENDANT_OF_MEMORIES) &&
                                   (INV_CONTENT(ITEM_LETTER_TO_KAFEI) <= ITEM_PENDANT_OF_MEMORIES)) {
                            gSaveContext.save.saveInfo.inventory.items[slot] = INV_CONTENT(ITEM_LETTER_TO_KAFEI) - 1;
                        }
                    }
                } else if ((slot >= SLOT_BOTTLE_1) && (slot <= SLOT_BOTTLE_6)) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                        value = ITEM_BOTTLE + slot - SLOT_BOTTLE_1;
                        Inventory_DeleteItem(value, SLOT(ITEM_BOTTLE) + slot - SLOT_BOTTLE_1);
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        if (gSaveContext.save.saveInfo.inventory.items[slot] == ITEM_NONE) {
                            gSaveContext.save.saveInfo.inventory.items[slot] = ITEM_BOTTLE;
                        } else if ((gSaveContext.save.saveInfo.inventory.items[slot] >= ITEM_BOTTLE) &&
                                   (gSaveContext.save.saveInfo.inventory.items[slot] <= ITEM_HYLIAN_LOACH)) {
                            gSaveContext.save.saveInfo.inventory.items[slot] =
                                gSaveContext.save.saveInfo.inventory.items[slot] + 1;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        if (gSaveContext.save.saveInfo.inventory.items[slot] == ITEM_NONE) {
                            gSaveContext.save.saveInfo.inventory.items[slot] = ITEM_OBABA_DRINK;
                        } else if ((gSaveContext.save.saveInfo.inventory.items[slot] >= ITEM_POTION_RED) &&
                                   (gSaveContext.save.saveInfo.inventory.items[slot] <= ITEM_OBABA_DRINK)) {
                            gSaveContext.save.saveInfo.inventory.items[slot] =
                                gSaveContext.save.saveInfo.inventory.items[slot] - 1;
                        }
                    }
                } else {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT) ||
                        CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                        CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        value = sSlotItems[slot];
                        if (gSaveContext.save.saveInfo.inventory.items[slot] == ITEM_NONE) {
                            INV_CONTENT(value) = value;
                        } else {
                            Inventory_DeleteItem(value, slot);
                        }
                    }
                }

            } else if (sCurSection == INV_EDITOR_SECTION_SWORD) {
                // Sword
                value = GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD);
                if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    value--;
                    if (value < EQUIP_VALUE_SWORD_NONE) {
                        value = EQUIP_VALUE_SWORD_NONE;
                    }

                    SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, value);

                    if (value != 0) {
                        CUR_FORM_EQUIP(EQUIP_SLOT_B) = value + (ITEM_SWORD_KOKIRI - 1);
                        gSaveContext.save.saveInfo.playerData.swordHealth = 100;
                    } else {
                        CUR_FORM_EQUIP(EQUIP_SLOT_B) = ITEM_NONE;
                    }

                } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                           CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                    value++;
                    if (value > EQUIP_VALUE_SWORD_DIETY) {
                        value = EQUIP_VALUE_SWORD_DIETY;
                    }

                    SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, value);

                    CUR_FORM_EQUIP(EQUIP_SLOT_B) = value + (ITEM_SWORD_KOKIRI - 1);
                    gSaveContext.save.saveInfo.playerData.swordHealth = 100;
                }

                Interface_LoadItemIconImpl(play, EQUIP_SLOT_B);

            } else if (sCurSection == INV_EDITOR_SECTION_SHIELD) {
                // Shield
                value = GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD);
                if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    value--;
                    if (value < EQUIP_VALUE_SHIELD_NONE) {
                        value = EQUIP_VALUE_SHIELD_NONE;
                    }

                    SET_EQUIP_VALUE(EQUIP_TYPE_SHIELD, value);

                } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                           CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                    value++;
                    if (value > EQUIP_VALUE_SHIELD_MAX) {
                        value = EQUIP_VALUE_SHIELD_MAX;
                    }
                    SET_EQUIP_VALUE(EQUIP_TYPE_SHIELD, value);
                }

            } else if (sCurSection == INV_EDITOR_SECTION_QUIVER) {
                // Quiver
                value = GET_CUR_UPG_VALUE(UPG_QUIVER);
                if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    value--;
                    if (value < 0) {
                        value = 0;
                    }
                    Inventory_ChangeUpgrade(UPG_QUIVER, value);
                } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                           CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                    value++;
                    if (value > 3) {
                        value = 3;
                    }
                    Inventory_ChangeUpgrade(UPG_QUIVER, value);
                }

            } else if (sCurSection == INV_EDITOR_SECTION_BOMB_BAG) {
                // Bomb Bag
                value = GET_CUR_UPG_VALUE(UPG_BOMB_BAG);
                if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    value--;
                    if (value < 0) {
                        value = 0;
                    }
                    Inventory_ChangeUpgrade(UPG_BOMB_BAG, value);
                } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                           CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                    value++;
                    if (value > 3) {
                        value = 3;
                    }
                    Inventory_ChangeUpgrade(UPG_BOMB_BAG, value);
                }

            } else if (sCurSection == INV_EDITOR_SECTION_SWAMP_GOLD_SKULLS) {
                // Gold Skulls (Swamp Spider House)
                if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    if (((gSaveContext.save.saveInfo.skullTokenCount & 0xFFFF0000) >> 0x10) != 0) {
                        gSaveContext.save.saveInfo.skullTokenCount =
                            ((u16)(((gSaveContext.save.saveInfo.skullTokenCount & 0xFFFF0000) >> 0x10) - 1) << 0x10) |
                            (gSaveContext.save.saveInfo.skullTokenCount & 0xFFFF);
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                           CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                    gSaveContext.save.saveInfo.skullTokenCount =
                        ((u16)(((gSaveContext.save.saveInfo.skullTokenCount & 0xFFFF0000) >> 0x10) + 1) << 0x10) |
                        (gSaveContext.save.saveInfo.skullTokenCount & 0xFFFF);
                }

            } else if (sCurSection == INV_EDITOR_SECTION_OCEAN_GOLD_SKULLS) {
                // Gold Skulls (Oceans Spider House)
                if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    if (((u16)gSaveContext.save.saveInfo.skullTokenCount) != 0) {
                        gSaveContext.save.saveInfo.skullTokenCount =
                            (((u16)gSaveContext.save.saveInfo.skullTokenCount - 1) & 0xFFFF) |
                            (gSaveContext.save.saveInfo.skullTokenCount & 0xFFFF0000);
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                           CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                    gSaveContext.save.saveInfo.skullTokenCount =
                        (((u16)gSaveContext.save.saveInfo.skullTokenCount + 1) & 0xFFFF) |
                        (gSaveContext.save.saveInfo.skullTokenCount & 0xFFFF0000);
                }

            } else if (sCurSection == INV_EDITOR_SECTION_NOTEBOOK) {
                // Bombers Notebook
                if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    TOGGLE_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK);
                }

            } else if (sCurSection == INV_EDITOR_SECTION_LULLABY_INTRO) {
                // Goron Lullaby Intro
                if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    TOGGLE_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO);
                }

            } else if (sCurSection < INV_EDITOR_SECTION_LULLABY_INTRO) {
                // Boss Remains and Songs
                //! Note: quest items must align with section, and all cases below INV_EDITOR_SECTION_LULLABY_INTRO must
                //! have also been taken
                slot = sCurSection - INV_EDITOR_SECTION_BOSS;
                if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    TOGGLE_QUEST_ITEM(slot);
                }

            } else if (sCurSection < INV_EDITOR_SECTION_DUNGEON_ITEMS) {
                // Dungeon Small Keys
                slot = sCurSection - INV_EDITOR_SECTION_SMALL_KEYS;
                if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    DUNGEON_KEY_COUNT(slot)--;
                    if (DUNGEON_KEY_COUNT(slot) < 0) {
                        DUNGEON_KEY_COUNT(slot) = -1;
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                           CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                    if (DUNGEON_KEY_COUNT(slot) < 0) {
                        DUNGEON_KEY_COUNT(slot) = 1;
                    } else {
                        DUNGEON_KEY_COUNT(slot)++;
                        if (DUNGEON_KEY_COUNT(slot) >= 9) {
                            DUNGEON_KEY_COUNT(slot) = 9;
                        }
                    }
                }

            } else if (sCurSection < INV_EDITOR_SECTION_STRAY_FAIRIES) {
                // Dungeon Items
                slot = sCurSection - INV_EDITOR_SECTION_DUNGEON_ITEMS;
                if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    gSaveContext.save.saveInfo.inventory.dungeonItems[slot] ^= (1 << DUNGEON_MAP);
                }
                if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
                    gSaveContext.save.saveInfo.inventory.dungeonItems[slot] ^= (1 << DUNGEON_COMPASS);
                }
                if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                    gSaveContext.save.saveInfo.inventory.dungeonItems[slot] ^= (1 << DUNGEON_BOSS_KEY);
                }

            } else if (sCurSection < INV_EDITOR_SECTION_DOUBLE_DEFENSE) {
                // Stray Fairies
                slot = sCurSection - INV_EDITOR_SECTION_STRAY_FAIRIES;
                if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    gSaveContext.save.saveInfo.inventory.strayFairies[slot]--;
                    if (gSaveContext.save.saveInfo.inventory.strayFairies[slot] < 0) {
                        gSaveContext.save.saveInfo.inventory.strayFairies[slot] = 0;
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                           CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                    gSaveContext.save.saveInfo.inventory.strayFairies[slot]++;
                    if (gSaveContext.save.saveInfo.inventory.strayFairies[slot] >= 99) {
                        gSaveContext.save.saveInfo.inventory.strayFairies[slot] = 99;
                    }
                }

            } else {
                // Double Defense
                if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT) ||
                    CHECK_BTN_ALL(input->press.button, BTN_CDOWN) || CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                    gSaveContext.save.saveInfo.playerData.doubleDefense ^= 1;
                    if (!gSaveContext.save.saveInfo.playerData.doubleDefense) {
                        gSaveContext.save.saveInfo.inventory.defenseHearts = 0;
                    } else {
                        gSaveContext.save.saveInfo.inventory.defenseHearts = 20;
                    }
                }
            }
            break;
    }

    // Handles exiting the inventory editor with the L button
    // The editor is opened with `debugEditor` set to DEBUG_EDITOR_INVENTORY_INIT,
    // and becomes closable after a frame once `debugEditor` is set to DEBUG_EDITOR_INVENTORY
    if (pauseCtx->debugEditor == DEBUG_EDITOR_INVENTORY_INIT) {
        pauseCtx->debugEditor = DEBUG_EDITOR_INVENTORY;
    } else if ((pauseCtx->debugEditor == DEBUG_EDITOR_INVENTORY) && CHECK_BTN_ALL(input->press.button, BTN_L)) {
        pauseCtx->debugEditor = DEBUG_EDITOR_NONE;
    }
}
