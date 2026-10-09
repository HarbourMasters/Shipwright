#include "global.h"
#include "sys_cfb.h"
#include "z64bombers_notebook.h"
#include "z64malloc.h"

#include "interface/schedule_static/schedule_static.h"
#include "archives/schedule_dma_static/schedule_dma_static_yar.h"
#include <string.h>

#include "BenPort.h"

#define BOMBERS_NOTEBOOK_ENTRY_SIZE 3
#define BOMBERS_NOTEBOOK_ENTRY_MAX 10

#define BOMBERS_NOTEBOOK_ENTRY_GET_DAY(row, entry) (((&sBombersNotebookEntries[row][entry])[0] & 0xF00) >> 8)
#define BOMBERS_NOTEBOOK_ENTRY_GET_EVENT(row, entry) ((&sBombersNotebookEntries[row][entry])[0] & 0xFF)
#define BOMBERS_NOTEBOOK_ENTRY_GET_START_TIME(row, entry) ((&sBombersNotebookEntries[row][entry])[1])
#define BOMBERS_NOTEBOOK_ENTRY_GET_END_TIME(row, entry) ((&sBombersNotebookEntries[row][entry])[2])

#define BOMBERS_NOTEBOOK_ENTRY(pos, day, event, startTime, endTime) \
    ((pos)&0xF000) | (((day)&0xF) << 8) | ((event)&0xFF), (startTime), (endTime)
#define BOMBERS_NOTEBOOK_ENTRY_END 0x9999

#define BOMBERS_NOTEBOOK_ENTRY_POS_CENTER 0x0000
#define BOMBERS_NOTEBOOK_ENTRY_POS_BELOW 0x4000
#define BOMBERS_NOTEBOOK_ENTRY_POS_ABOVE 0x8000

typedef enum {
    /* 0 */ BOMBERS_NOTEBOOK_EVENT_ICON_EXCLAMATION_POINT,
    /* 1 */ BOMBERS_NOTEBOOK_EVENT_ICON_MASK,
    /* 2 */ BOMBERS_NOTEBOOK_EVENT_ICON_RIBBON
} BombersNotebookEventIcon;

#define BOMBERS_NOTEBOOK_EVENT_COLOR_WEEKEVENTREG_NONE 0xFFF0
#define BOMBERS_NOTEBOOK_EVENT_COLOR_WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI 0xFFF1

u16 sBombersNotebookEntries[BOMBERS_NOTEBOOK_PERSON_MAX][BOMBERS_NOTEBOOK_ENTRY_MAX * BOMBERS_NOTEBOOK_ENTRY_SIZE] = {
    {
        /* Bombers */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_ABOVE, 1, BOMBERS_NOTEBOOK_EVENT_LEARNED_SECRET_CODE,
                               CLOCK_TIME(6, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_BELOW, 1, BOMBERS_NOTEBOOK_EVENT_RECEIVED_BOMBERS_NOTEBOOK,
                               CLOCK_TIME(6, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_ABOVE, 2, BOMBERS_NOTEBOOK_EVENT_LEARNED_SECRET_CODE,
                               CLOCK_TIME(6, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_BELOW, 2, BOMBERS_NOTEBOOK_EVENT_RECEIVED_BOMBERS_NOTEBOOK,
                               CLOCK_TIME(6, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_ABOVE, 3, BOMBERS_NOTEBOOK_EVENT_LEARNED_SECRET_CODE,
                               CLOCK_TIME(6, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_BELOW, 3, BOMBERS_NOTEBOOK_EVENT_RECEIVED_BOMBERS_NOTEBOOK,
                               CLOCK_TIME(6, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Anju */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_ABOVE, 1, BOMBERS_NOTEBOOK_EVENT_RECEIVED_ROOM_KEY,
                               CLOCK_TIME(13, 45), CLOCK_TIME(16, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_BELOW, 1, BOMBERS_NOTEBOOK_EVENT_PROMISED_MIDNIGHT_MEETING,
                               CLOCK_TIME(14, 30), CLOCK_TIME(20, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_ABOVE, 1, BOMBERS_NOTEBOOK_EVENT_PROMISED_TO_MEET_KAFEI,
                               CLOCK_TIME(23, 30), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_BELOW, 1, BOMBERS_NOTEBOOK_EVENT_RECEIVED_LETTER_TO_KAFEI,
                               CLOCK_TIME(23, 30), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 2,
                               BOMBERS_NOTEBOOK_EVENT_DELIVERED_PENDANT_OF_MEMORIES, CLOCK_TIME(17, 0),
                               CLOCK_TIME(21, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 3,
                               BOMBERS_NOTEBOOK_EVENT_DELIVERED_PENDANT_OF_MEMORIES, CLOCK_TIME(6, 0),
                               CLOCK_TIME(12, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 3, BOMBERS_NOTEBOOK_EVENT_RECEIVED_COUPLES_MASK,
                               CLOCK_TIME(5, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Kafei */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 2,
                               BOMBERS_NOTEBOOK_EVENT_RECEIVED_PENDANT_OF_MEMORIES, CLOCK_TIME(16, 30),
                               CLOCK_TIME(22, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 3, BOMBERS_NOTEBOOK_EVENT_ESCAPED_SAKONS_HIDEOUT,
                               CLOCK_TIME(18, 0), CLOCK_TIME(19, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 3, BOMBERS_NOTEBOOK_EVENT_RECEIVED_COUPLES_MASK,
                               CLOCK_TIME(5, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Curiosity Shop Man */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_ABOVE, 3, BOMBERS_NOTEBOOK_EVENT_RECEIVED_KEATON_MASK,
                               CLOCK_TIME(13, 0), CLOCK_TIME(22, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_BELOW, 3, BOMBERS_NOTEBOOK_EVENT_RECEIVED_PRIORITY_MAIL,
                               CLOCK_TIME(13, 0), CLOCK_TIME(22, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 3, BOMBERS_NOTEBOOK_EVENT_RECEIVED_ALL_NIGHT_MASK,
                               CLOCK_TIME(22, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Old Lady from Bomb Shop */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 1, BOMBERS_NOTEBOOK_EVENT_RECEIVED_BLAST_MASK,
                               CLOCK_TIME(0, 30), CLOCK_TIME(1, 0)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Romani */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 1,
                               BOMBERS_NOTEBOOK_EVENT_PROMISED_TO_HELP_WITH_ALIENS, CLOCK_TIME(6, 0),
                               CLOCK_TIME(18, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_ABOVE, 1, BOMBERS_NOTEBOOK_EVENT_DEFENDED_AGAINST_ALIENS,
                               CLOCK_TIME(2, 30), CLOCK_TIME(5, 15)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_BELOW, 1, BOMBERS_NOTEBOOK_EVENT_RECEIVED_ALIENS_BOTTLE,
                               CLOCK_TIME(2, 30), CLOCK_TIME(5, 15)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Cremia */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_ABOVE, 2, BOMBERS_NOTEBOOK_EVENT_ESCORTED_CREMIA,
                               CLOCK_TIME(18, 0), CLOCK_TIME(21, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_BELOW, 2, BOMBERS_NOTEBOOK_EVENT_RECEIVED_ROMANIS_MASK,
                               CLOCK_TIME(18, 0), CLOCK_TIME(21, 0)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Mayor Dotour */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 1, BOMBERS_NOTEBOOK_EVENT_RECEIVED_MAYOR_HP,
                               CLOCK_TIME(8, 0), CLOCK_TIME(20, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 2, BOMBERS_NOTEBOOK_EVENT_RECEIVED_MAYOR_HP,
                               CLOCK_TIME(8, 0), CLOCK_TIME(20, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 3, BOMBERS_NOTEBOOK_EVENT_RECEIVED_MAYOR_HP,
                               CLOCK_TIME(8, 0), CLOCK_TIME(18, 0)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Madame Aroma */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 1, BOMBERS_NOTEBOOK_EVENT_RECEIVED_KAFEIS_MASK,
                               CLOCK_TIME(8, 0), CLOCK_TIME(20, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 2, BOMBERS_NOTEBOOK_EVENT_RECEIVED_KAFEIS_MASK,
                               CLOCK_TIME(8, 0), CLOCK_TIME(20, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 3, BOMBERS_NOTEBOOK_EVENT_DELIVERED_PRIORITY_MAIL,
                               CLOCK_TIME(18, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Toto */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 1,
                               BOMBERS_NOTEBOOK_EVENT_RECEIVED_CIRCUS_LEADERS_MASK, CLOCK_TIME(22, 0),
                               CLOCK_TIME(5, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 2,
                               BOMBERS_NOTEBOOK_EVENT_RECEIVED_CIRCUS_LEADERS_MASK, CLOCK_TIME(22, 0),
                               CLOCK_TIME(5, 0)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Gorman */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 1,
                               BOMBERS_NOTEBOOK_EVENT_RECEIVED_CIRCUS_LEADERS_MASK, CLOCK_TIME(22, 0),
                               CLOCK_TIME(5, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 2,
                               BOMBERS_NOTEBOOK_EVENT_RECEIVED_CIRCUS_LEADERS_MASK, CLOCK_TIME(22, 0),
                               CLOCK_TIME(5, 0)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Postman */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 1, BOMBERS_NOTEBOOK_EVENT_RECEIVED_POSTMAN_HP,
                               CLOCK_TIME(15, 0), CLOCK_TIME(0, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 1, BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI,
                               CLOCK_TIME(0, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 2, BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI,
                               CLOCK_TIME(6, 0), CLOCK_TIME(12, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 2, BOMBERS_NOTEBOOK_EVENT_RECEIVED_POSTMAN_HP,
                               CLOCK_TIME(15, 0), CLOCK_TIME(0, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 3, BOMBERS_NOTEBOOK_EVENT_RECEIVED_POSTMANS_HAT,
                               CLOCK_TIME(18, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Rosa Sisters */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 1, BOMBERS_NOTEBOOK_EVENT_RECEIVED_ROSA_SISTERS_HP,
                               CLOCK_TIME(18, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 2, BOMBERS_NOTEBOOK_EVENT_RECEIVED_ROSA_SISTERS_HP,
                               CLOCK_TIME(18, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* ??? */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 1, BOMBERS_NOTEBOOK_EVENT_RECEIVED_TOILET_HAND_HP,
                               CLOCK_TIME(0, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 2, BOMBERS_NOTEBOOK_EVENT_RECEIVED_TOILET_HAND_HP,
                               CLOCK_TIME(0, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 3, BOMBERS_NOTEBOOK_EVENT_RECEIVED_TOILET_HAND_HP,
                               CLOCK_TIME(0, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Anju's Grandmother */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_ABOVE, 1,
                               BOMBERS_NOTEBOOK_EVENT_RECEIVED_GRANDMA_SHORT_STORY_HP, CLOCK_TIME(8, 0),
                               CLOCK_TIME(18, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_BELOW, 1,
                               BOMBERS_NOTEBOOK_EVENT_RECEIVED_GRANDMA_LONG_STORY_HP, CLOCK_TIME(8, 0),
                               CLOCK_TIME(18, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_ABOVE, 2,
                               BOMBERS_NOTEBOOK_EVENT_RECEIVED_GRANDMA_SHORT_STORY_HP, CLOCK_TIME(8, 0),
                               CLOCK_TIME(18, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_BELOW, 2,
                               BOMBERS_NOTEBOOK_EVENT_RECEIVED_GRANDMA_LONG_STORY_HP, CLOCK_TIME(8, 0),
                               CLOCK_TIME(18, 0)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Kamaro */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 1, BOMBERS_NOTEBOOK_EVENT_RECEIVED_KAMAROS_MASK,
                               CLOCK_TIME(0, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 2, BOMBERS_NOTEBOOK_EVENT_RECEIVED_KAMAROS_MASK,
                               CLOCK_TIME(0, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 3, BOMBERS_NOTEBOOK_EVENT_RECEIVED_KAMAROS_MASK,
                               CLOCK_TIME(0, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Grog */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 1, BOMBERS_NOTEBOOK_EVENT_RECEIVED_BUNNY_HOOD,
                               CLOCK_TIME(6, 0), CLOCK_TIME(20, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 2, BOMBERS_NOTEBOOK_EVENT_RECEIVED_BUNNY_HOOD,
                               CLOCK_TIME(6, 0), CLOCK_TIME(20, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 3, BOMBERS_NOTEBOOK_EVENT_RECEIVED_BUNNY_HOOD,
                               CLOCK_TIME(6, 0), CLOCK_TIME(20, 0)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Gorman Brothers */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 1, BOMBERS_NOTEBOOK_EVENT_RECEIVED_GAROS_MASK,
                               CLOCK_TIME(6, 0), CLOCK_TIME(18, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 2, BOMBERS_NOTEBOOK_EVENT_RECEIVED_GAROS_MASK,
                               CLOCK_TIME(6, 0), CLOCK_TIME(18, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 2, BOMBERS_NOTEBOOK_EVENT_ESCORTED_CREMIA,
                               CLOCK_TIME(18, 0), CLOCK_TIME(21, 0)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 3, BOMBERS_NOTEBOOK_EVENT_RECEIVED_GAROS_MASK,
                               CLOCK_TIME(6, 0), CLOCK_TIME(18, 0)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Shiro */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 1, BOMBERS_NOTEBOOK_EVENT_RECEIVED_STONE_MASK,
                               CLOCK_TIME(6, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 2, BOMBERS_NOTEBOOK_EVENT_RECEIVED_STONE_MASK,
                               CLOCK_TIME(6, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 3, BOMBERS_NOTEBOOK_EVENT_RECEIVED_STONE_MASK,
                               CLOCK_TIME(6, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
    {
        /* Guru-Guru */
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 1, BOMBERS_NOTEBOOK_EVENT_RECEIVED_BREMEN_MASK,
                               CLOCK_TIME(18, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY(BOMBERS_NOTEBOOK_ENTRY_POS_CENTER, 2, BOMBERS_NOTEBOOK_EVENT_RECEIVED_BREMEN_MASK,
                               CLOCK_TIME(18, 0), CLOCK_TIME(5, 59)),
        BOMBERS_NOTEBOOK_ENTRY_END,
    },
};

s16 sBombersNotebookDayRectRectLeft[] = { 120, 120, 270, 420 };
TexturePtr sBombersNotebookDayTextures[] = {
    gBombersNotebookDay1stENGTex,
    gBombersNotebookDay2ndENGTex,
    gBombersNotebookDayFinalENGTex,
};

#define DEFINE_PERSON(_enum, _photo, _description, metEnum, _metMessage, _metFlag)
#define DEFINE_EVENT(_enum, icon, _colorFlag, _description, _completedMessage, _completedFlag) icon,

s32 sBombersNotebookEventIcons[] = {
#include "tables/notebook_table.h"
};

#undef DEFINE_PERSON
#undef DEFINE_EVENT

s32 sBombersNotebookEventIconWidths[] = { 16, 24, 32 };
s32 sBombersNotebookEventIconHeights[] = { 16, 28, 28 };

#define DEFINE_PERSON(_enum, _photo, _description, metEnum, _metMessage, _metFlag)
#define DEFINE_EVENT(_enum, _icon, colorFlag, _description, _completedMessage, _completedFlag) colorFlag,

u16 sBombersNotebookEventColorWeekEventFlags[] = {
#include "tables/notebook_table.h"
};

#undef DEFINE_PERSON
#undef DEFINE_EVENT

void BombersNotebook_DrawScisTexRect(Gfx** gfxP, s32 rxl, s32 ryl, s32 rxh, s32 ryh, s32 tile, s32 s, s32 t, s32 dsdx,
                                     s32 dtdy) {
    s32 xl = rxl - (gCfbLeftAdjust * 4);
    s32 yl = ryl - (gCfbUpperAdjust * 4);
    s32 xh = rxh - (gCfbLeftAdjust * 4);
    s32 yh = ryh - (gCfbUpperAdjust * 4);
    Gfx* gfx = *gfxP;

    gSPScisTextureRectangle(gfx++, xl, yl, xh, yh, tile, s, t, dsdx, dtdy);

    *gfxP = gfx;
}

s16 sBombersNotebookHeaderColors[][3] = {
    { 116, 134, 146 }, // Column 1
    { 158, 156, 131 }, // Column 2
    { 174, 141, 151 }, // Column 3
};

void BombersNotebook_DrawHeaders(Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    s32 i;
    s32 rectLeft;

    gDPLoadTextureBlock(gfx++, gBombersNotebookHeaderBoxTex, G_IM_FMT_I, G_IM_SIZ_8b, 8, 24, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 3, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    for (i = 0, rectLeft = 120; i < ARRAY_COUNT(sBombersNotebookHeaderColors); i++, rectLeft += 150) {
        gDPSetPrimColor(gfx++, 0, 0, sBombersNotebookHeaderColors[i][0], sBombersNotebookHeaderColors[i][1],
                        sBombersNotebookHeaderColors[i][2], 192);
        BombersNotebook_DrawScisTexRect(&gfx, rectLeft * 4, 74 * 4, (rectLeft + 143) * 4, 98 * 4, 0, 0, 0, 1 << 10,
                                        1 << 10);
    }

    gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, 255);
    for (i = 0, rectLeft = 124; i < ARRAY_COUNT(sBombersNotebookDayTextures); i++, rectLeft += 150) {
        gDPLoadTextureBlock(gfx++, sBombersNotebookDayTextures[i], G_IM_FMT_IA, G_IM_SIZ_8b, 48, 22, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        BombersNotebook_DrawScisTexRect(&gfx, rectLeft * 4, 77 * 4, (rectLeft + 48) * 4, 99 * 4, 0, 0, 0, 1 << 10,
                                        1 << 10);
    }

    rectLeft = 172;
    gDPLoadTextureBlock_4b(gfx++, gBombersNotebook1800Tex, G_IM_FMT_IA, 48, 11, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    for (i = 0; i < 3; i++) {
        BombersNotebook_DrawScisTexRect(&gfx, rectLeft * 4, 86 * 4, (rectLeft + 48) * 4, 97 * 4, 0, 0, 0, 1 << 10,
                                        1 << 10);
        rectLeft += 150;
    }

    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

// Each column/day gets subdivided into 4 sub columns, the first 2 sub columns use 1 color, the last 2 another
s16 sBombersNotebookColumnColors[][2][3] = {
    { { 165, 183, 195 }, { 140, 158, 170 } }, // Column 1
    { { 197, 195, 172 }, { 172, 170, 147 } }, // Column 2
    { { 223, 190, 200 }, { 190, 165, 175 } }, // Column 3
};

void BombersNotebook_DrawColumns(Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    s16* color;
    s32 i;
    s32 j;
    s32 k;
    s32 columnRectLeft;
    s32 subColumnRectLeft;
    s32 pad;

    gDPLoadTextureBlock_4b(gfx++, gBombersNotebookSubColumnBoxTex, G_IM_FMT_I, 48, 1, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    color = &sBombersNotebookColumnColors[0][0][0];
    for (i = 0, columnRectLeft = 120; i < ARRAY_COUNT(sBombersNotebookColumnColors); i++, columnRectLeft += 150) {
        subColumnRectLeft = columnRectLeft;
        for (j = 0; j < 2; j++) {
            gDPSetPrimColor(gfx++, 0, 0, color[0], color[1], color[2], 192);
            for (k = 0; k < 2; k++) {
                BombersNotebook_DrawScisTexRect(&gfx, subColumnRectLeft * 4, 104 * 4, (subColumnRectLeft + 48) * 4,
                                                480 * 4, 0, 0, 0, 1 << 10, 1 << 10);
                subColumnRectLeft += 36;
            }
            color += 3;
        }
    }

    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

TexturePtr sBombersNotebookEventIconTextures[] = {
    gBombersNotebookEntryIconExclamationPointTex,
    gBombersNotebookEntryIconMaskTex,
    gBombersNotebookEntryIconRibbonTex,
};
s16 sBombersNotebookEntryIconColors[][3] = {
    { 255, 255, 0 },
    { 141, 255, 182 },
    { 255, 100, 60 },
};

void BombersNotebook_DrawEntries(Gfx** gfxP, s32 row, u32 rectTop) {
    Gfx* gfx = *gfxP;
    s32 entryRectLeft;
    u32 eventIcon;
    u32 unfinishedEvent = false;
    u16 startTime;
    u16 endTime;
    s32 j = 0;
    u32 iconRectLeft;
    s32 entryRectRight;
    u32 yOffset;
    s32 entryWidth;

    while (true) {
        if (sBombersNotebookEntries[row][j] == BOMBERS_NOTEBOOK_ENTRY_END) {
            if (!unfinishedEvent) {
                gDPPipeSync(gfx++);
                gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                                  PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
                gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, 255);
                gDPSetEnvColor(gfx++, 0, 0, 0, 255);
                gDPLoadTextureBlock(gfx++, gBombersNotebookStampTex, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 48, 0,
                                    G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                    G_TX_NOLOD, G_TX_NOLOD);
                BombersNotebook_DrawScisTexRect(&gfx, 567 * 4, (rectTop + 1) * 4, 599 * 4, (rectTop + 49) * 4, 0, 0, 0,
                                                1 << 10, 1 << 10);
                gDPPipeSync(gfx++);
                gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, 255);
                gDPSetEnvColor(gfx++, 255, 0, 0, 255);
                BombersNotebook_DrawScisTexRect(&gfx, 565 * 4, (rectTop + -1) * 4, 597 * 4, (rectTop + 47) * 4, 0, 0, 0,
                                                1 << 10, 1 << 10);
                gDPPipeSync(gfx++);
                gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                gDPSetEnvColor(gfx++, 0, 0, 0, 255);
            }
            break;
        }

        if (sBombersNotebookEntries[row][j] & BOMBERS_NOTEBOOK_ENTRY_POS_ABOVE) {
            yOffset = 8;
        } else if (sBombersNotebookEntries[row][j] & BOMBERS_NOTEBOOK_ENTRY_POS_BELOW) {
            yOffset = 24;
        } else { // BOMBERS_NOTEBOOK_ENTRY_POS_CENTER
            yOffset = 16;
        }

        startTime = BOMBERS_NOTEBOOK_ENTRY_GET_START_TIME(row, j) - CLOCK_TIME(6, 0);
        endTime = BOMBERS_NOTEBOOK_ENTRY_GET_END_TIME(row, j) - CLOCK_TIME(6, 0);
        entryRectLeft =
            sBombersNotebookDayRectRectLeft[BOMBERS_NOTEBOOK_ENTRY_GET_DAY(row, j)] + (startTime / CLOCK_TIME(0, 10));
        entryRectRight =
            sBombersNotebookDayRectRectLeft[BOMBERS_NOTEBOOK_ENTRY_GET_DAY(row, j)] + (endTime / CLOCK_TIME(0, 10));
        entryWidth = entryRectRight - entryRectLeft - 8;
        if ((entryRectRight - entryRectLeft) < 8) {
            entryRectLeft = ((entryRectLeft + entryRectRight) - entryRectLeft) - 8;
            entryRectRight = entryRectLeft + 8;
        }

        gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, 255);
        gDPLoadTextureBlock(gfx++, gBombersNotebookEntryBoxEndTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 16, 0,
                            G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 2, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        BombersNotebook_DrawScisTexRect(&gfx, (entryRectLeft + 2) * 4, (rectTop + yOffset + 2) * 4,
                                        (entryRectLeft + 6) * 4, (rectTop + yOffset + 18) * 4, 0, 0, 0, 1 << 10,
                                        1 << 10);
        BombersNotebook_DrawScisTexRect(&gfx, (entryRectRight + -2) * 4, (rectTop + yOffset + 2) * 4,
                                        (entryRectRight + 2) * 4, (rectTop + yOffset + 18) * 4, 0, 0x80, 0, 1 << 10,
                                        1 << 10);
        if (entryWidth > 0) {
            gDPLoadTextureBlock(gfx++, gBombersNotebookEntryBoxMiddleTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 16, 0,
                                G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 2, G_TX_NOMASK, G_TX_NOLOD,
                                G_TX_NOLOD);
            BombersNotebook_DrawScisTexRect(&gfx, (entryRectLeft + 6) * 4, (rectTop + yOffset + 2) * 4,
                                            (entryRectLeft + 6 + entryWidth) * 4, (rectTop + yOffset + 18) * 4, 0, 0, 0,
                                            1 << 10, 1 << 10);
        }

        gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, 255);
        gDPLoadTextureBlock(gfx++, gBombersNotebookEntryBoxEndTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 16, 0,
                            G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 2, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        //! FAKE: the ^ 0
        BombersNotebook_DrawScisTexRect(&gfx, (entryRectLeft * 4) ^ 0, (rectTop + yOffset) * 4, (entryRectLeft + 4) * 4,
                                        (rectTop + yOffset + 16) * 4, 0, 0, 0, 1 << 10, 1 << 10);
        //! FAKE: the ^ 0
        BombersNotebook_DrawScisTexRect(&gfx, (entryRectRight + -4) * 4, (rectTop + yOffset) * 4,
                                        (entryRectRight * 4) ^ 0, (rectTop + yOffset + 16) * 4, 0, 0x80, 0, 1 << 10,
                                        1 << 10);
        if (entryWidth > 0) {
            gDPLoadTextureBlock(gfx++, gBombersNotebookEntryBoxMiddleTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 16, 0,
                                G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 2, G_TX_NOMASK, G_TX_NOLOD,
                                G_TX_NOLOD);
            BombersNotebook_DrawScisTexRect(&gfx, (entryRectLeft + 4) * 4, (rectTop + yOffset) * 4,
                                            (entryRectLeft + 4 + entryWidth) * 4, (rectTop + yOffset + 16) * 4, 0, 0, 0,
                                            1 << 10, 1 << 10);
        }

        if (CHECK_WEEKEVENTREG(gBombersNotebookWeekEventFlags[BOMBERS_NOTEBOOK_ENTRY_GET_EVENT(row, j)])) {
            eventIcon =
                sBombersNotebookEventIcons[BOMBERS_NOTEBOOK_ENTRY_GET_EVENT(row, j) - BOMBERS_NOTEBOOK_PERSON_MAX];
            if ((entryRectRight - entryRectLeft) < sBombersNotebookEventIconWidths[eventIcon]) {
                iconRectLeft =
                    (((entryRectLeft - sBombersNotebookEventIconWidths[eventIcon]) + entryRectRight) - entryRectLeft) +
                    3;
            } else {
                iconRectLeft = (((entryRectRight - entryRectLeft) - sBombersNotebookEventIconWidths[eventIcon]) / 2) +
                               entryRectLeft;
            }

            if ((eventIcon == BOMBERS_NOTEBOOK_EVENT_ICON_MASK) || (eventIcon == BOMBERS_NOTEBOOK_EVENT_ICON_RIBBON)) {
                if (sBombersNotebookEntries[row][j] & BOMBERS_NOTEBOOK_ENTRY_POS_ABOVE) {
                    yOffset -= 12;
                } else if (!(sBombersNotebookEntries[row][j] & BOMBERS_NOTEBOOK_ENTRY_POS_BELOW)) {
                    // BOMBERS_NOTEBOOK_ENTRY_POS_CENTER
                    yOffset -= 6;
                }
            }

            // 2S2H [Port] We need to set the render mode to XLU_SURF for alpha logic to be used in Fast3D
            // If Fast3D changes how it decides alpha in the future, we may be able to remove this line
            gDPSetRenderMode(gfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);

            gDPLoadTextureBlock(gfx++, sBombersNotebookEventIconTextures[eventIcon], G_IM_FMT_IA, G_IM_SIZ_8b,
                                sBombersNotebookEventIconWidths[eventIcon], sBombersNotebookEventIconHeights[eventIcon],
                                0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, 255);

            BombersNotebook_DrawScisTexRect(&gfx, (iconRectLeft + 2) * 4, (rectTop + yOffset + 2) * 4,
                                            (iconRectLeft + 2 + sBombersNotebookEventIconWidths[eventIcon]) * 4,
                                            (rectTop + yOffset + 2 + sBombersNotebookEventIconHeights[eventIcon]) * 4,
                                            0, 0, 0, 1 << 10, 1 << 10);
            if (sBombersNotebookEventColorWeekEventFlags[BOMBERS_NOTEBOOK_ENTRY_GET_EVENT(row, j) -
                                                         BOMBERS_NOTEBOOK_PERSON_MAX] ==
                BOMBERS_NOTEBOOK_EVENT_COLOR_WEEKEVENTREG_NONE) {
                gDPSetPrimColor(gfx++, 0, 0, sBombersNotebookEntryIconColors[eventIcon][0],
                                sBombersNotebookEntryIconColors[eventIcon][1],
                                sBombersNotebookEntryIconColors[eventIcon][2], 255);
            } else if (sBombersNotebookEventColorWeekEventFlags[BOMBERS_NOTEBOOK_ENTRY_GET_EVENT(row, j) -
                                                                BOMBERS_NOTEBOOK_PERSON_MAX] ==
                       BOMBERS_NOTEBOOK_EVENT_COLOR_WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_SOUTH_UPPER_CLOCKTOWN) ||
                    CHECK_WEEKEVENTREG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_NORTH_CLOCKTOWN) ||
                    CHECK_WEEKEVENTREG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_EAST_UPPER_CLOCKTOWN) ||
                    CHECK_WEEKEVENTREG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_EAST_LOWER_CLOCKTOWN) ||
                    CHECK_WEEKEVENTREG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_SOUTH_LOWER_CLOCKTOWN)) {
                    gDPSetPrimColor(gfx++, 0, 0, sBombersNotebookEntryIconColors[eventIcon][0],
                                    sBombersNotebookEntryIconColors[eventIcon][1],
                                    sBombersNotebookEntryIconColors[eventIcon][2], 255);
                } else {
                    gDPSetPrimColor(gfx++, 0, 0, 155, 155, 155, 255);
                }
            } else {
                if (CHECK_WEEKEVENTREG(
                        sBombersNotebookEventColorWeekEventFlags[BOMBERS_NOTEBOOK_ENTRY_GET_EVENT(row, j) -
                                                                 BOMBERS_NOTEBOOK_PERSON_MAX])) {
                    gDPSetPrimColor(gfx++, 0, 0, sBombersNotebookEntryIconColors[eventIcon][0],
                                    sBombersNotebookEntryIconColors[eventIcon][1],
                                    sBombersNotebookEntryIconColors[eventIcon][2], 255);
                } else {
                    gDPSetPrimColor(gfx++, 0, 0, 155, 155, 155, 255);
                }
            }
            BombersNotebook_DrawScisTexRect(&gfx, iconRectLeft * 4, (rectTop + yOffset) * 4,
                                            (iconRectLeft + sBombersNotebookEventIconWidths[eventIcon]) * 4,
                                            (rectTop + yOffset + sBombersNotebookEventIconHeights[eventIcon]) * 4, 0, 0,
                                            0, 1 << 10, 1 << 10);
            gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, 255);
        } else {
            unfinishedEvent = true;
        }
        j += BOMBERS_NOTEBOOK_ENTRY_SIZE;
    }

    *gfxP = gfx;
}

#define DEFINE_PERSON(_enum, photo, _description, _metEnum, _metMessage, _metFlag) photo,
#define DEFINE_EVENT(enum, _icon, _colorFlag, _description, _completedMessage, _completedFlag)

TexturePtr sBombersNotebookPhotoTextures[] = {
#include "tables/notebook_table.h"
};

#undef DEFINE_PERSON
#undef DEFINE_EVENT

void BombersNotebook_DrawRows(BombersNotebook* this, Gfx** gfxP) {
    static s16 sBarColorR = 0;
    static s16 sBarColorG = 0;
    static s16 sBarColorB = 0;
    static s16 sBarColorTimer = 10;
    static s16 sBarColorIndex = 0;
    static s16 sBarColorTargetsR[] = { 100, 0 };
    static s16 sBarColorTargetsG[] = { 80, 0 };
    static s16 sBarColorTargetsB[] = { 255, 0 };
    Gfx* gfx = *gfxP;
    s32 photoWidth;
    s32 rowEnd;
    s32 cursorPage;
    u32 barRectLeft;
    f32 photoRectDs;
    u32 photoOffset;
    s32 i;
    u32 rectTop;
    s16 colorStep;

    colorStep = ABS_ALT(sBarColorR - sBarColorTargetsR[sBarColorIndex]) / sBarColorTimer;
    if (sBarColorR >= sBarColorTargetsR[sBarColorIndex]) {
        sBarColorR -= colorStep;
    } else {
        sBarColorR += colorStep;
    }

    colorStep = ABS_ALT(sBarColorG - sBarColorTargetsG[sBarColorIndex]) / sBarColorTimer;
    if (sBarColorG >= sBarColorTargetsG[sBarColorIndex]) {
        sBarColorG -= colorStep;
    } else {
        sBarColorG += colorStep;
    }

    colorStep = ABS_ALT(sBarColorB - sBarColorTargetsB[sBarColorIndex]) / sBarColorTimer;
    if (sBarColorB >= sBarColorTargetsB[sBarColorIndex]) {
        sBarColorB -= colorStep;
    } else {
        sBarColorB += colorStep;
    }

    sBarColorTimer--;
    if (sBarColorTimer == 0) {
        sBarColorTimer = 10;
        sBarColorR = sBarColorTargetsR[sBarColorIndex];
        sBarColorG = sBarColorTargetsG[sBarColorIndex];
        sBarColorB = sBarColorTargetsB[sBarColorIndex];
        sBarColorIndex ^= 1;
    }

    rectTop = this->scrollOffset + 107;
    if (this->cursorPage >= (BOMBERS_NOTEBOOK_PERSON_MAX - 8)) {
        rowEnd = BOMBERS_NOTEBOOK_PERSON_MAX;
        i = this->cursorPage;
    } else {
        rowEnd = this->cursorPage + 8;
        i = rowEnd - 8;
    }
    cursorPage = i;
    for (; i < rowEnd; i++, rectTop += 52) {
        gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, 255);
        if (CHECK_WEEKEVENTREG(gBombersNotebookWeekEventFlags[i])) {
            gDPLoadTextureBlock(gfx++, sBombersNotebookPhotoTextures[i], G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
        } else {
            gDPLoadTextureBlock(gfx++, gBombersNotebookEmptyPhotoTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
        }
        if ((i == (this->cursorPageRow + cursorPage)) && (this->scrollAmount == 0)) {
            barRectLeft = 56;
            photoOffset = 8;
            photoRectDs = 200.0f;
            photoWidth = 64;
        } else {
            barRectLeft = 48;
            photoOffset = 0;
            photoRectDs = 150.0f;
            photoWidth = 48;
        }
        BombersNotebook_DrawScisTexRect(&gfx, (57 - photoOffset) * 4, (rectTop - photoOffset) * 4,
                                        (57 - photoOffset + photoWidth) * 4, (rectTop - photoOffset + photoWidth) * 4,
                                        0, 0, 0, 1024.0f / (photoRectDs / 100.0f), 1024.0f / (photoRectDs / 100.0f));

        if ((i == (this->cursorPageRow + cursorPage)) && (this->scrollAmount == 0)) {
            gDPSetPrimColor(gfx++, 0, 0, sBarColorR, sBarColorG, sBarColorB, 255);
        } else {
            gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, 60);
        }
        gDPLoadTextureBlock(gfx++, gBombersNotebookBarTex, G_IM_FMT_I, G_IM_SIZ_8b, 8, 4, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, 3, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        BombersNotebook_DrawScisTexRect(&gfx, (barRectLeft + 57) * 4, (rectTop + 22) * 4,
                                        (barRectLeft - photoOffset + 527) * 4, (rectTop + 26) * 4, 0, 0, 0, 1 << 10,
                                        1 << 10);
        gDPLoadTextureBlock_4b(gfx++, gBombersNotebookCircleTex, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                               G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        BombersNotebook_DrawScisTexRect(&gfx, (barRectLeft - photoOffset + 527) * 4, (rectTop + 16) * 4,
                                        (barRectLeft - photoOffset + 543) * 4, (rectTop + 32) * 4, 0, 0, 0, 1 << 10,
                                        1 << 10);

        if (CHECK_WEEKEVENTREG(gBombersNotebookWeekEventFlags[i])) {
            gDPPipeSync(gfx++);
            gDPSetRenderMode(gfx++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
            gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, 255);

            BombersNotebook_DrawEntries(&gfx, i, rectTop);

            gDPPipeSync(gfx++);
            gDPSetRenderMode(gfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        }
    }
    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

TexturePtr sBombersNotebookDigitTextures[] = {
    gBombersNotebookDigit0Tex, gBombersNotebookDigit1Tex, gBombersNotebookDigit2Tex, gBombersNotebookDigit3Tex,
    gBombersNotebookDigit4Tex, gBombersNotebookDigit5Tex, gBombersNotebookDigit6Tex, gBombersNotebookDigit7Tex,
    gBombersNotebookDigit8Tex, gBombersNotebookDigit9Tex,
};
u8 sBombersNotebookDigitRectLeftOffsets[] = { 13, 8, 12, 13, 14, 13, 12, 12, 13, 12 };
s16 sBombersNotebookDayTexRectLeftOffsets[] = { 16, 12, 9 };

#define CURRENT_DAY_CLAMP_MIN_1 ((CURRENT_DAY == 0) ? 1 : CURRENT_DAY)
void BombersNotebook_DrawTimeOfDay(Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    u32 timeOfDayRectLeft;
    u32 timeBoxRectLeft;
    u32 connectorRectLeft;
    u32 lineRectLeft;
    u32 digitsRectLeft;
    u16 time;
    s32 onesDigit;
    s32 tensDigit;
    s32 hours;

    time = CURRENT_TIME - CLOCK_TIME(6, 0);
    if (CURRENT_DAY == 0) {
        time = 0;
    }
    timeOfDayRectLeft = sBombersNotebookDayRectRectLeft[CURRENT_DAY] + (time / CLOCK_TIME(0, 10));
    if ((CURRENT_DAY_CLAMP_MIN_1 == 1) || ((CURRENT_DAY_CLAMP_MIN_1 == 2) && (CURRENT_TIME < CLOCK_TIME(12, 0)))) {
        timeOfDayRectLeft -= 32;
        lineRectLeft = timeOfDayRectLeft + 32;
    } else if ((CURRENT_DAY_CLAMP_MIN_1 == 2) && (time >= (CLOCK_TIME(17, 0) - CLOCK_TIME(6, 0))) &&
               (time <= (CLOCK_TIME(19, 0) - CLOCK_TIME(6, 0)))) {
        timeOfDayRectLeft -= 64;
        lineRectLeft = timeOfDayRectLeft + 64;
    } else {
        timeOfDayRectLeft -= 96;
        lineRectLeft = timeOfDayRectLeft + 96;
    }

    gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, 255);
    gDPLoadTextureBlock_4b(gfx++, gBombersNoteoobkTimeOfDayBoxTex, G_IM_FMT_IA, 64, 28, 0, G_TX_MIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 6, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    BombersNotebook_DrawScisTexRect(&gfx, timeOfDayRectLeft * 4, 42 * 4, (timeOfDayRectLeft + 128) * 4, 70 * 4, 0, 0, 0,
                                    1 << 10, 1 << 10);
    gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, 255);
    gDPPipeSync(gfx++);
    gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, 0, 0, 0, TEXEL0, 0, 0, 0, PRIMITIVE, 0, 0, 0, TEXEL0);
    gDPLoadTextureBlock_4b(gfx++, gBombersNotebookTimeOfDayENGTex, G_IM_FMT_I, 96, 20, 0, G_TX_MIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    BombersNotebook_DrawScisTexRect(&gfx, (timeOfDayRectLeft + 16) * 4, 47 * 4, (timeOfDayRectLeft + 112) * 4, 67 * 4,
                                    0, 0, 0, 1 << 10, 1 << 10);

    gDPPipeSync(gfx++);
    gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, 255);
    if (((CURRENT_DAY_CLAMP_MIN_1 >= 3) ||
         ((CURRENT_DAY_CLAMP_MIN_1 == 2) && (time > (CLOCK_TIME(18, 0) - CLOCK_TIME(6, 0)))))) {
        timeBoxRectLeft = timeOfDayRectLeft - 159;
        connectorRectLeft = timeOfDayRectLeft - 17;
    } else {
        timeBoxRectLeft = timeOfDayRectLeft + 130;
        connectorRectLeft = timeOfDayRectLeft + 113;
    }

    gDPLoadTextureBlock_4b(gfx++, gBombersNotebookTimeBoxLeftTex, G_IM_FMT_IA, 16, 28, 0, G_TX_MIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    BombersNotebook_DrawScisTexRect(&gfx, timeBoxRectLeft * 4, 42 * 4, (timeBoxRectLeft + 16) * 4, 70 * 4, 0, 0, 0,
                                    1 << 10, 1 << 10);
    gDPLoadTextureBlock_4b(gfx++, gBombersNotebookTimeBoxMiddleTex, G_IM_FMT_IA, 16, 28, 0, G_TX_MIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    BombersNotebook_DrawScisTexRect(&gfx, (timeBoxRectLeft + 16) * 4, 42 * 4, (timeBoxRectLeft + 141) * 4, 70 * 4, 0, 0,
                                    0, 1 << 10, 1 << 10);
    gDPLoadTextureBlock_4b(gfx++, gBombersNotebookTimeBoxRightTex, G_IM_FMT_IA, 16, 28, 0, G_TX_MIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    BombersNotebook_DrawScisTexRect(&gfx, (timeBoxRectLeft + 141) * 4, 42 * 4, (timeBoxRectLeft + 157) * 4, 70 * 4, 0,
                                    0, 0, 1 << 10, 1 << 10);
    gDPLoadTextureBlock(gfx++, sBombersNotebookDayTextures[CURRENT_DAY_CLAMP_MIN_1 - 1], G_IM_FMT_IA, G_IM_SIZ_8b, 48,
                        22, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                        G_TX_NOLOD, G_TX_NOLOD);
    BombersNotebook_DrawScisTexRect(
        &gfx, (timeBoxRectLeft + sBombersNotebookDayTexRectLeftOffsets[CURRENT_DAY_CLAMP_MIN_1 - 1]) * 4, 46 * 4,
        (timeBoxRectLeft + sBombersNotebookDayTexRectLeftOffsets[CURRENT_DAY_CLAMP_MIN_1 - 1] + 48) * 4, 68 * 4, 0, 0,
        0, 1 << 10, 1 << 10);

    gDPSetPrimColor(gfx++, 0, 0, 150, 150, 150, 255);
    gDPLoadTextureBlock_4b(gfx++, gBombersNotebookCircleTex, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    BombersNotebook_DrawScisTexRect(&gfx, (timeBoxRectLeft + 56) * 4, 48 * 4, (timeBoxRectLeft + 72) * 4, 0x100, 0, 0,
                                    0, 1 << 10, 1 << 10);
    gDPPipeSync(gfx++);
    gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, 0, 0, 0, TEXEL0, 0, 0, 0, PRIMITIVE, 0, 0, 0, TEXEL0);
    gDPSetPrimColor(gfx++, 0, 0, 0, 0, 0, 255);

    tensDigit = 0;
    onesDigit = CURRENT_TIME / CLOCK_TIME_HOUR;
    if (CURRENT_DAY == 0) {
        onesDigit = 6;
    }
    hours = onesDigit;
    do {
        if (onesDigit >= 10) {
            tensDigit++;
            onesDigit -= 10;
        }
    } while (onesDigit >= 10);

    digitsRectLeft = timeBoxRectLeft + 75;
    gDPLoadTextureBlock_4b(gfx++, sBombersNotebookDigitTextures[tensDigit], G_IM_FMT_I, 16, 17, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);
    BombersNotebook_DrawScisTexRect(&gfx, digitsRectLeft * 4, 48 * 4, (digitsRectLeft + 16) * 4, 65 * 4, 0, 0, 0,
                                    1 << 10, 1 << 10);

    digitsRectLeft += sBombersNotebookDigitRectLeftOffsets[tensDigit];
    gDPLoadTextureBlock_4b(gfx++, sBombersNotebookDigitTextures[onesDigit], G_IM_FMT_I, 16, 17, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);
    BombersNotebook_DrawScisTexRect(&gfx, digitsRectLeft * 4, 48 * 4, (digitsRectLeft + 16) * 4, 65 * 4, 0, 0, 0,
                                    1 << 10, 1 << 10);

    digitsRectLeft += sBombersNotebookDigitRectLeftOffsets[onesDigit];
    gDPLoadTextureBlock_4b(gfx++, gBombersNotebookColonTex, G_IM_FMT_I, 16, 17, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    BombersNotebook_DrawScisTexRect(&gfx, digitsRectLeft * 4, 48 * 4, (digitsRectLeft + 16) * 4, 65 * 4, 0, 0, 0,
                                    1 << 10, 1 << 10);

    digitsRectLeft += 7;
    tensDigit = 0;
    if (CURRENT_DAY == 0) {
        onesDigit = TIME_TO_MINUTES_ALT_F((CLOCK_TIME_F(6, 0) - (hours * CLOCK_TIME_HOUR_F)));
    } else {
        onesDigit = TIME_TO_MINUTES_ALT_F(CURRENT_TIME - (hours * CLOCK_TIME_HOUR_F));
    }
    do {
        if (onesDigit >= 10) {
            tensDigit++;
            onesDigit -= 10;
        }
    } while (onesDigit >= 10);

    gDPLoadTextureBlock_4b(gfx++, sBombersNotebookDigitTextures[tensDigit], G_IM_FMT_I, 16, 17, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);
    BombersNotebook_DrawScisTexRect(&gfx, digitsRectLeft * 4, 48 * 4, (digitsRectLeft + 16) * 4, 65 * 4, 0, 0, 0,
                                    1 << 10, 1 << 10);
    digitsRectLeft += sBombersNotebookDigitRectLeftOffsets[tensDigit];
    gDPLoadTextureBlock_4b(gfx++, sBombersNotebookDigitTextures[onesDigit], G_IM_FMT_I, 16, 17, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);
    BombersNotebook_DrawScisTexRect(&gfx, digitsRectLeft * 4, 48 * 4, (digitsRectLeft + 16) * 4, 65 * 4, 0, 0, 0,
                                    1 << 10, 1 << 10);

    gDPPipeSync(gfx++);
    gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, 255);
    gDPLoadTextureBlock(gfx++, gBombersNotebookConnectorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 14, 0,
                        G_TX_MIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    BombersNotebook_DrawScisTexRect(&gfx, connectorRectLeft * 4, 49 * 4, (connectorRectLeft + 32) * 4, 63 * 4, 0, 0, 0,
                                    1 << 10, 1 << 10);

    gDPSetPrimColor(gfx++, 0, 0, 242, 0, 14, 255);
    gDPLoadTextureBlock(gfx++, gBombersNotebookLineTex, G_IM_FMT_I, G_IM_SIZ_8b, 8, 1, 0, G_TX_MIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    // 2S2H [Cosmetic] The vertical red line texture is only 8 pixels wide, but the rectangle draw 32 pixels wide.
    // Because the texture is mirrored and wrap along the S axis, this caused there to be 3 lines
    // Setting it to 8 replicates the effect from console
    BombersNotebook_DrawScisTexRect(&gfx, lineRectLeft * 4, 74 * 4, (lineRectLeft + 8) * 4, 490 * 4, 0, 0, 0, 1 << 10,
                                    1 << 10);

    gDPPipeSync(gfx++);
    gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(gfx++, 200, 0, 0, 255);
    gDPLoadTextureBlock(gfx++, gBombersNotebookArrowTex, G_IM_FMT_IA, G_IM_SIZ_8b, 24, 16, 0, G_TX_MIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    BombersNotebook_DrawScisTexRect(&gfx, (lineRectLeft + -11) * 4, 70 * 4, (lineRectLeft + 13) * 4, 86 * 4, 0, 0, 0,
                                    1 << 10, 1 << 10);

    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

void BombersNotebook_DrawCursor(BombersNotebook* this, Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    s32 cursorRow;
    s32 cursorEntry;
    s32 entryRectRight;
    s32 entryRectLeft;
    s32 entryIcon;
    u32 cursorRectLeft;
    u32 cursorRectTop;
    u32 yOffset;
    u16 startTime;
    u16 endTime;

    gDPPipeSync(gfx++);
    gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(gfx++, 0, 0, 255, 205, 55, 255);
    gDPSetEnvColor(gfx++, 0, 0, 0, 255);
    gDPLoadTextureBlock_4b(gfx++, gBombersNotebookCursorTex, G_IM_FMT_IA, 16, 16, 0, G_TX_MIRROR | G_TX_WRAP,
                           G_TX_MIRROR | G_TX_WRAP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);

    if (this->cursorEntry == 0) {
        BombersNotebook_DrawScisTexRect(&gfx, 47 * 4, (this->cursorPageRow * 52 + 97) * 4, 63 * 4,
                                        (this->cursorPageRow * 52 + 113) * 4, 0, 0, 0, 1 << 10, 1 << 10);
        BombersNotebook_DrawScisTexRect(&gfx, 98 * 4, (this->cursorPageRow * 52 + 97) * 4, 114 * 4,
                                        (this->cursorPageRow * 52 + 113) * 4, 0, 0x200, 0, 1 << 10, 1 << 10);
        BombersNotebook_DrawScisTexRect(&gfx, 47 * 4, (this->cursorPageRow * 52 + 147) * 4, 63 * 4,
                                        (this->cursorPageRow * 52 + 163) * 4, 0, 0, 0x200, 1 << 10, 1 << 10);
        BombersNotebook_DrawScisTexRect(&gfx, 98 * 4, (this->cursorPageRow * 52 + 147) * 4, 114 * 4,
                                        (this->cursorPageRow * 52 + 163) * 4, 0, 0x200, 0x200, 1 << 10, 1 << 10);
    } else {
        cursorRow = this->cursorPageRow + this->cursorPage;
        cursorRectTop = (this->cursorPageRow * 52) + 107;
        cursorEntry = this->cursorEntry - BOMBERS_NOTEBOOK_ENTRY_SIZE; // Offset from photo entry
        if (sBombersNotebookEntries[cursorRow][cursorEntry] & BOMBERS_NOTEBOOK_ENTRY_POS_ABOVE) {
            yOffset = 8;
        } else if (sBombersNotebookEntries[cursorRow][cursorEntry] & BOMBERS_NOTEBOOK_ENTRY_POS_BELOW) {
            yOffset = 24;
        } else { // BOMBERS_NOTEBOOK_ENTRY_POS_CENTER
            yOffset = 16;
        }

        startTime = BOMBERS_NOTEBOOK_ENTRY_GET_START_TIME(cursorRow, cursorEntry) - CLOCK_TIME(6, 0);
        endTime = BOMBERS_NOTEBOOK_ENTRY_GET_END_TIME(cursorRow, cursorEntry) - CLOCK_TIME(6, 0);
        entryRectLeft = sBombersNotebookDayRectRectLeft[BOMBERS_NOTEBOOK_ENTRY_GET_DAY(cursorRow, cursorEntry)] +
                        (startTime / CLOCK_TIME(0, 10));
        entryRectRight = sBombersNotebookDayRectRectLeft[BOMBERS_NOTEBOOK_ENTRY_GET_DAY(cursorRow, cursorEntry)] +
                         (endTime / CLOCK_TIME(0, 10));
        if ((entryRectRight - entryRectLeft) < 8) {
            entryRectLeft = ((entryRectLeft + entryRectRight) - entryRectLeft) - 8;
            entryRectRight = entryRectLeft + 8;
        }
        entryIcon = sBombersNotebookEventIcons[BOMBERS_NOTEBOOK_ENTRY_GET_EVENT(cursorRow, cursorEntry) -
                                               BOMBERS_NOTEBOOK_PERSON_MAX];
        if ((entryRectRight - entryRectLeft) < sBombersNotebookEventIconWidths[entryIcon]) {
            cursorRectLeft =
                (((entryRectLeft - sBombersNotebookEventIconWidths[entryIcon]) + entryRectRight) - entryRectLeft) + 3;
        } else {
            cursorRectLeft =
                (((entryRectRight - entryRectLeft) - sBombersNotebookEventIconWidths[entryIcon]) / 2) + entryRectLeft;
        }
        if (entryIcon == BOMBERS_NOTEBOOK_EVENT_ICON_EXCLAMATION_POINT) {
            cursorRectLeft -= 8;
            cursorRectTop -= 8;
        } else {
            cursorRectLeft -= 9;
            cursorRectTop -= 8;
        }
        if ((entryIcon == BOMBERS_NOTEBOOK_EVENT_ICON_MASK) || (entryIcon == BOMBERS_NOTEBOOK_EVENT_ICON_RIBBON)) {
            if (sBombersNotebookEntries[cursorRow][cursorEntry] & BOMBERS_NOTEBOOK_ENTRY_POS_ABOVE) {
                yOffset -= 12;
            } else if (!(sBombersNotebookEntries[cursorRow][cursorEntry] & BOMBERS_NOTEBOOK_ENTRY_POS_BELOW)) {
                // BOMBERS_NOTEBOOK_ENTRY_POS_CENTER
                yOffset -= 6;
            }
        }
        BombersNotebook_DrawScisTexRect(&gfx, cursorRectLeft * 4, (cursorRectTop + yOffset) * 4,
                                        (cursorRectLeft + 16) * 4, (cursorRectTop + yOffset + 16) * 4, 0, 0, 0, 1 << 10,
                                        1 << 10);
        BombersNotebook_DrawScisTexRect(&gfx, (cursorRectLeft + sBombersNotebookEventIconWidths[entryIcon]) * 4,
                                        (cursorRectTop + yOffset) * 4,
                                        (cursorRectLeft + sBombersNotebookEventIconWidths[entryIcon] + 16) * 4,
                                        (cursorRectTop + yOffset + 16) * 4, 0, 0x200, 0, 1 << 10, 1 << 10);
        BombersNotebook_DrawScisTexRect(
            &gfx, cursorRectLeft * 4, (cursorRectTop + yOffset + sBombersNotebookEventIconHeights[entryIcon]) * 4,
            (cursorRectLeft + 16) * 4, (cursorRectTop + yOffset + sBombersNotebookEventIconHeights[entryIcon] + 16) * 4,
            0, 0, 0x200, 1 << 10, 1 << 10);
        BombersNotebook_DrawScisTexRect(&gfx, (cursorRectLeft + sBombersNotebookEventIconWidths[entryIcon]) * 4,
                                        (cursorRectTop + yOffset + sBombersNotebookEventIconHeights[entryIcon]) * 4,
                                        (cursorRectLeft + sBombersNotebookEventIconWidths[entryIcon] + 16) * 4,
                                        (cursorRectTop + yOffset + sBombersNotebookEventIconHeights[entryIcon] + 16) *
                                            4,
                                        0, 0x200, 0x200, 1 << 10, 1 << 10);
    }

    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

void BombersNotebook_DrawArrows(BombersNotebook* this, Gfx** gfxP) {
    static s16 sUpperArrowColorR = 0;
    static s16 sUpperArrowColorG = 100;
    static s16 sUpperArrowColorB = 255;
    static s16 sLowerArrowColorR = 0;
    static s16 sLowerArrowColorG = 100;
    static s16 sLowerArrowColorB = 255;
    static s16 sUpperArrowColorIndex = 0;
    static s16 sLowerArrowColorIndex = 0;
    static s16 sUpperArrowColorTimer = 6;
    static s16 sLowerArrowColorTimer = 6;
    static s16 sUpperArrowOffsetY = 0;
    static s16 sLowerArrowOffsetY = 0;
    static s16 sArrowColorTargetsG[] = { 175, 100 };
    static s16 sArrowColorTargetsB[] = { 205, 255 };
    Gfx* gfx = *gfxP;
    u32 rectTop;
    s16 colorStep;

    gDPPipeSync(gfx++);
    gDPLoadTextureBlock(gfx++, gBombersNotebookArrowTex, G_IM_FMT_IA, G_IM_SIZ_8b, 24, 16, 0, G_TX_MIRROR | G_TX_WRAP,
                        G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, 4, G_TX_NOLOD, G_TX_NOLOD);

    colorStep = ABS_ALT(sUpperArrowColorG - sArrowColorTargetsG[sUpperArrowColorIndex]) / sUpperArrowColorTimer;
    if (sUpperArrowColorG >= sArrowColorTargetsG[sUpperArrowColorIndex]) {
        sUpperArrowColorG -= colorStep;
    } else {
        sUpperArrowColorG += colorStep;
    }

    colorStep = ABS_ALT(sUpperArrowColorB - sArrowColorTargetsB[sUpperArrowColorIndex]) / sUpperArrowColorTimer;
    if (sUpperArrowColorB >= sArrowColorTargetsB[sUpperArrowColorIndex]) {
        sUpperArrowColorB -= colorStep;
    } else {
        sUpperArrowColorB += colorStep;
    }

    sUpperArrowColorTimer--;
    if (sUpperArrowColorTimer == 0) {
        sUpperArrowOffsetY ^= 6;
        sUpperArrowColorTimer = 6;
        sUpperArrowColorG = sArrowColorTargetsG[sUpperArrowColorIndex];
        sUpperArrowColorB = sArrowColorTargetsB[sUpperArrowColorIndex];
        sUpperArrowColorIndex ^= 1;
    }

    colorStep = ABS_ALT(sLowerArrowColorG - sArrowColorTargetsG[sLowerArrowColorIndex]) / sLowerArrowColorTimer;
    if (sLowerArrowColorG >= sArrowColorTargetsG[sLowerArrowColorIndex]) {
        sLowerArrowColorG -= colorStep;
    } else {
        sLowerArrowColorG += colorStep;
    }

    colorStep = ABS_ALT(sLowerArrowColorB - sArrowColorTargetsB[sLowerArrowColorIndex]) / sLowerArrowColorTimer;
    if (sLowerArrowColorB >= sArrowColorTargetsB[sLowerArrowColorIndex]) {
        sLowerArrowColorB -= colorStep;
    } else {
        sLowerArrowColorB += colorStep;
    }

    sLowerArrowColorTimer--;
    if (sLowerArrowColorTimer == 0) {
        sLowerArrowOffsetY ^= 6;
        sLowerArrowColorTimer = 6;
        sLowerArrowColorG = sArrowColorTargetsG[sLowerArrowColorIndex];
        sLowerArrowColorB = sArrowColorTargetsB[sLowerArrowColorIndex];
        sLowerArrowColorIndex ^= 1;
    }

    if (this->cursorPage != 0) {
        gDPPipeSync(gfx++);
        gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                          ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor(gfx++, 0, 0, sUpperArrowColorR, sUpperArrowColorG, sUpperArrowColorB, 255);
        gDPSetEnvColor(gfx++, sUpperArrowColorR, sUpperArrowColorG, sUpperArrowColorB, 255);
        rectTop = (-sUpperArrowOffsetY + 83) * 4;
        BombersNotebook_DrawScisTexRect(&gfx, 46 * 4, rectTop, 70 * 4, rectTop + (16 * 4), 0, 0, 0x200, 1 << 10,
                                        1 << 10);
    }

    if (this->cursorPage < 16) {
        gDPPipeSync(gfx++);
        gDPSetPrimColor(gfx++, 0, 0, sLowerArrowColorR, sLowerArrowColorG, sLowerArrowColorB, 255);
        gDPSetEnvColor(gfx++, sLowerArrowColorR, sLowerArrowColorG, sLowerArrowColorB, 255);
        BombersNotebook_DrawScisTexRect(&gfx, 46 * 4, (sLowerArrowOffsetY + 319) * 4, 70 * 4,
                                        (sLowerArrowOffsetY + 335) * 4, 0, 0, 0, 1 << 10, 1 << 10);
    }
    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

void BombersNotebook_Draw(BombersNotebook* this, GraphicsContext* gfxCtx) {
    Gfx* gfx;
    s32 pad[2];

    func_8012CF0C(gfxCtx, this->loadState != BOMBERS_NOTEBOOK_LOAD_STATE_DONE, false, 0, 0, 0);

    OPEN_DISPS(gfxCtx);

    gfx = POLY_OPA_DISP;

    if (this->loadState == BOMBERS_NOTEBOOK_LOAD_STATE_DONE) {
        // gSPSegment(gfx++, 0x07, this->scheduleDmaSegment);
        // gSPSegment(gfx++, 0x08, this->scheduleSegment);
        gfx = Gfx_SetupDL39(gfx);
        gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                          ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetRenderMode(gfx++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
        gDPSetPrimColor(gfx++, 0, 0, 255, 255, 215, 255);
        gDPSetEnvColor(gfx++, 100, 100, 100, 255);
        gDPLoadTextureBlock(gfx++, gBombersNotebookBackgroundTex, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

        // 2S2H [Cosmetic][Widescreen] Extend the background tiling for widescreens
        gSPWideTextureRectangle(gfx++, OTRGetRectDimensionFromLeftEdge(0) * 4, 0 * 4,
                                OTRGetRectDimensionFromRightEdge(SCREEN_WIDTH_HIRES) * 4, SCREEN_HEIGHT_HIRES * 4, 0, 0,
                                0, 0x200, 0x200);

        gDPPipeSync(gfx++);
        gDPSetCombineMode(gfx++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetRenderMode(gfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);

        BombersNotebook_DrawHeaders(&gfx);
        BombersNotebook_DrawColumns(&gfx);

        gDPSetScissor(gfx++, G_SC_NON_INTERLACE, 0, 86, 600, 450);
        BombersNotebook_DrawRows(this, &gfx);

        gDPPipeSync(gfx++);

        gSPDisplayList(gfx++, D_0E000000_TO_SEGMENTED(setScissor));

        BombersNotebook_DrawTimeOfDay(&gfx);

        if (this->scrollAmount == 0) {
            BombersNotebook_DrawCursor(this, &gfx);
        }
        BombersNotebook_DrawArrows(this, &gfx);
    }
    POLY_OPA_DISP = gfx;

    CLOSE_DISPS(gfxCtx);
}

void BombersNotebook_LoadFiles(BombersNotebook* this, s32 flag) {
    switch (this->loadState) {
        case BOMBERS_NOTEBOOK_LOAD_STATE_NONE:
// #region 2S2H [Port] Same as the comment in the update function. We don't need to use the built in DMA
// engine. We use the resource manager.
#if 0
            if (this->scheduleDmaSegment == NULL) {
                break;
            }
            CmpDma_LoadAllFiles(this->scheduleDmaSegmentStart, this->scheduleDmaSegment, this->scheduleDmaSegmentSize);
            osCreateMesgQueue(&this->loadQueue, this->loadMsg, ARRAY_COUNT(this->loadMsg));
            DmaMgr_SendRequestImpl(&this->dmaRequest, this->scheduleSegment, this->scheduleSegmentStart,
                                   this->scheduleSegmentSize, 0, &this->loadQueue, OS_MESG_PTR(NULL));
#endif
            this->loadState = BOMBERS_NOTEBOOK_LOAD_STATE_STARTED;
            // fallthrough
        case BOMBERS_NOTEBOOK_LOAD_STATE_STARTED:
            // if (osRecvMesg(&this->loadQueue, NULL, flag) == 0) {
            this->loadState = BOMBERS_NOTEBOOK_LOAD_STATE_DONE;
            //}
            break;

        default:
            break;
    }
}

#define DEFINE_PERSON(_enum, _photo, description, _metEnum, _metMessage, _metFlag) description,
#define DEFINE_EVENT(_enum, _icon, _colorFlag, description, _completedMessage, _completedFlag) description,

u16 sBombersNotebookTextIds[] = {
#include "tables/notebook_table.h"
};

#undef DEFINE_PERSON
#undef DEFINE_EVENT

void BombersNotebook_Update(PlayState* play, BombersNotebook* this, Input* input) {
    static s16 sStickXRepeatTimer = 0;
    static s16 sStickYRepeatTimer = 0;
    static s16 sStickXRepeatState = 0;
    static s16 sStickYRepeatState = 0;
    s32 stickAdjX = input->rel.stick_x;
    s32 stickAdjY = input->rel.stick_y;
    s32 cursorEntryScan;

// #region 2S2H [Port] We don't need to allocate anything because we don't use these segments.
// We also want to avoid using malloc here because using a size of zero still allocates some data.
#if 0
    this->scheduleDmaSegmentStart = SEGMENT_ROM_START(schedule_dma_static_yar);
    this->scheduleDmaSegmentSize = SEGMENT_ROM_SIZE(schedule_dma_static_syms);
    this->scheduleSegmentStart = SEGMENT_ROM_START(schedule_static);
    this->scheduleSegmentSize = SEGMENT_ROM_SIZE(schedule_static);

    if (this->scheduleDmaSegment == NULL) {
        this->scheduleDmaSegment = ZeldaArena_Malloc(this->scheduleDmaSegmentSize);
    }

    if (this->scheduleSegment == NULL) {
        this->scheduleSegment = ZeldaArena_Malloc(this->scheduleSegmentSize);
    }
#endif

    BombersNotebook_LoadFiles(this, OS_MESG_NOBLOCK);

    if (this->loadState == BOMBERS_NOTEBOOK_LOAD_STATE_DONE) {
        if (stickAdjX < -30) {
            if (sStickXRepeatState == -1) {
                sStickXRepeatTimer--;
                if (sStickXRepeatTimer < 0) {
                    sStickXRepeatTimer = 2;
                } else {
                    stickAdjX = 0;
                }
            } else {
                sStickXRepeatTimer = 10;
                sStickXRepeatState = -1;
            }
        } else if (stickAdjX > 30) {
            if (sStickXRepeatState == 1) {
                sStickXRepeatTimer--;
                if (sStickXRepeatTimer < 0) {
                    sStickXRepeatTimer = 2;
                } else {
                    stickAdjX = 0;
                }
            } else {
                sStickXRepeatTimer = 10;
                sStickXRepeatState = 1;
            }
        } else {
            sStickXRepeatState = 0;
        }

        if (stickAdjY < -30) {
            if (sStickYRepeatState == -1) {
                sStickYRepeatTimer--;
                if (sStickYRepeatTimer < 0) {
                    sStickYRepeatTimer = 2;
                } else {
                    stickAdjY = 0;
                }
            } else {
                sStickYRepeatTimer = 10;
                sStickYRepeatState = -1;
            }
        } else if (stickAdjY > 30) {
            if (sStickYRepeatState == 1) {
                sStickYRepeatTimer--;
                if (sStickYRepeatTimer < 0) {
                    sStickYRepeatTimer = 2;
                } else {
                    stickAdjY = 0;
                }
            } else {
                sStickYRepeatTimer = 10;
                sStickYRepeatState = 1;
            }
        } else {
            sStickYRepeatState = 0;
        }

        if (this->scrollAmount == 0) {
            if (CHECK_WEEKEVENTREG(gBombersNotebookWeekEventFlags[this->cursorPageRow + this->cursorPage])) {
                cursorEntryScan = this->cursorEntry;
                if (stickAdjX > 30) {
                    while (true) {
                        cursorEntryScan += BOMBERS_NOTEBOOK_ENTRY_SIZE;
                        if (sBombersNotebookEntries[this->cursorPageRow + this->cursorPage]
                                                   [cursorEntryScan - BOMBERS_NOTEBOOK_ENTRY_SIZE] ==
                            BOMBERS_NOTEBOOK_ENTRY_END) {
                            while (true) {
                                cursorEntryScan -= BOMBERS_NOTEBOOK_ENTRY_SIZE;
                                if (cursorEntryScan == 0) {
                                    Audio_PlaySfx(NA_SE_SY_ERROR);
                                    break;
                                }
                                if (CHECK_WEEKEVENTREG(gBombersNotebookWeekEventFlags[BOMBERS_NOTEBOOK_ENTRY_GET_EVENT(
                                        this->cursorPageRow + this->cursorPage,
                                        cursorEntryScan - BOMBERS_NOTEBOOK_ENTRY_SIZE)])) {
                                    Audio_PlaySfx(NA_SE_SY_ERROR);
                                    break;
                                }
                            }
                            break;
                        }
                        if (CHECK_WEEKEVENTREG(gBombersNotebookWeekEventFlags[BOMBERS_NOTEBOOK_ENTRY_GET_EVENT(
                                this->cursorPageRow + this->cursorPage,
                                cursorEntryScan - BOMBERS_NOTEBOOK_ENTRY_SIZE)])) {
                            Audio_PlaySfx(NA_SE_SY_CURSOR);
                            break;
                        }
                    }
                    this->cursorEntry = cursorEntryScan;
                } else if (stickAdjX < -30) {
                    if (cursorEntryScan != 0) {
                        do {
                            cursorEntryScan -= BOMBERS_NOTEBOOK_ENTRY_SIZE;
                            if (CHECK_WEEKEVENTREG(gBombersNotebookWeekEventFlags[BOMBERS_NOTEBOOK_ENTRY_GET_EVENT(
                                    this->cursorPageRow + this->cursorPage,
                                    cursorEntryScan - BOMBERS_NOTEBOOK_ENTRY_SIZE)])) {
                                Audio_PlaySfx(NA_SE_SY_CURSOR);
                                break;
                            }
                        } while (cursorEntryScan != 0);
                        this->cursorEntry = cursorEntryScan;
                    }
                }
                if (this->cursorEntry != 0) {
                    if (play->msgCtx.currentTextId !=
                        sBombersNotebookTextIds[BOMBERS_NOTEBOOK_ENTRY_GET_EVENT(
                            this->cursorPageRow + this->cursorPage, this->cursorEntry - BOMBERS_NOTEBOOK_ENTRY_SIZE)]) {
                        Message_ContinueTextbox(play, sBombersNotebookTextIds[BOMBERS_NOTEBOOK_ENTRY_GET_EVENT(
                                                          this->cursorPageRow + this->cursorPage,
                                                          this->cursorEntry - BOMBERS_NOTEBOOK_ENTRY_SIZE)]);
                    }
                } else {
                    if (play->msgCtx.currentTextId != sBombersNotebookTextIds[this->cursorPageRow + this->cursorPage]) {
                        Message_ContinueTextbox(play, sBombersNotebookTextIds[this->cursorPageRow + this->cursorPage]);
                    }
                }
            } else {
                Message_CloseTextbox(play);
            }

            if (stickAdjY < -30) {
                if (this->cursorPageRow < 3) {
                    Audio_PlaySfx(NA_SE_SY_CURSOR);
                    this->cursorEntry = 0;
                    this->cursorPageRow++;
                } else if (this->cursorPage < (BOMBERS_NOTEBOOK_PERSON_MAX - 4)) {
                    if (sStickYRepeatState == -1) {
                        this->scrollAmount = -24;
                    } else {
                        this->scrollAmount = -12;
                    }
                    this->scrollOffset = 0;
                    this->cursorEntry = 0;
                }
            } else if (stickAdjY > 30) {
                if (this->cursorPageRow > 0) {
                    Audio_PlaySfx(NA_SE_SY_CURSOR);
                    this->cursorEntry = 0;
                    this->cursorPageRow--;
                } else if (this->cursorPage != 0) {
                    Audio_PlaySfx(NA_SE_SY_CURSOR);
                    this->cursorPage--;
                    if (sStickYRepeatState == 1) {
                        this->scrollAmount = 24;
                    } else {
                        this->scrollAmount = 12;
                    }
                    this->scrollOffset = -48;
                    this->cursorEntry = 0;
                }
            }
        } else if (this->scrollAmount < 0) {
            this->scrollOffset += this->scrollAmount;
            if (ABS_ALT(this->scrollOffset) >= 48) {
                Audio_PlaySfx(NA_SE_SY_CURSOR);
                this->scrollOffset = 0;
                this->scrollAmount = 0;
                this->cursorPage++;
            }
        } else {
            this->scrollOffset += this->scrollAmount;
            if (this->scrollOffset == 0) {
                this->scrollOffset = 0;
                this->scrollAmount = 0;
            }
        }
    }
}

void BombersNotebook_Init(BombersNotebook* this) {
    memset(this, 0, sizeof(BombersNotebook));
}

void BombersNotebook_Destroy(BombersNotebook* this) {
    if (this->loadState == BOMBERS_NOTEBOOK_LOAD_STATE_STARTED) {
        BombersNotebook_LoadFiles(this, OS_MESG_BLOCK);
    }
#if 0
    if (this->scheduleDmaSegment != NULL) {
        ZeldaArena_Free(this->scheduleDmaSegment);
        this->scheduleDmaSegment = NULL;
    }
#endif
    //! @bug: Does not free malloc'd memory for schedule segment
    // 2S2H [Port] "Fix" the bug by not allocating anything
}
