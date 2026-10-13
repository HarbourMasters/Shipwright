#include "z64debug_text.h"
#include "global.h"

typedef struct {
    /* 0x0 */ u8 colorIndex;
    /* 0x1 */ char text[11];
} DebugTextBufferEntry; // size = 0xC

typedef struct {
    /* 0x0 */ u8 x;
    /* 0x1 */ u8 y;
    /* 0x2 */ u8 colorIndex;
    /* 0x3 */ char text[41];
} DebugCamTextBufferEntry; // size = 0x2C

#define DEBUG_TEXT_DRAW_TEXT (1 << 0)
#define DEBUG_TEXT_DRAW_CAM_TEXT (1 << 1)

s32 sDebugTextDrawFlags = 0;

DebugTextBufferEntry sDebugTextBuffer[] = {
    { DEBUG_TEXT_BLACK, "          " }, { DEBUG_TEXT_BLACK, "          " }, { DEBUG_TEXT_BLACK, "          " },
    { DEBUG_TEXT_BLACK, "          " }, { DEBUG_TEXT_BLACK, "          " }, { DEBUG_TEXT_BLACK, "          " },
};

Color_RGBA8 sDebugTextColors[] = {
    { 0, 0, 0, 64 },       // DEBUG_TEXT_BLACK
    { 0, 0, 255, 64 },     // DEBUG_TEXT_BLUE
    { 255, 0, 0, 64 },     // DEBUG_TEXT_RED
    { 255, 0, 255, 64 },   // DEBUG_TEXT_PURPLE
    { 0, 255, 0, 64 },     // DEBUG_TEXT_GREEN
    { 0, 255, 255, 64 },   // DEBUG_TEXT_CYAN
    { 255, 255, 0, 64 },   // DEBUG_TEXT_YELLOW
    { 255, 255, 255, 64 }, // DEBUG_TEXT_WHITE
};

DebugCamTextBufferEntry sDebugCamTextBuffer[80];

s16 sDebugCamTextEntryCount = 0;

Color_RGBA8 sDebugCamTextColors[] = {
    { 255, 255, 32, 192 },  // DEBUG_CAM_TEXT_YELLOW
    { 255, 150, 128, 192 }, // DEBUG_CAM_TEXT_PEACH
    { 128, 96, 0, 64 },     // DEBUG_CAM_TEXT_BROWN
    { 192, 128, 16, 128 },  // DEBUG_CAM_TEXT_ORANGE
    { 255, 192, 32, 128 },  // DEBUG_CAM_TEXT_GOLD
    { 230, 230, 220, 64 },  // DEBUG_CAM_TEXT_WHITE
    { 128, 150, 255, 128 }, // DEBUG_CAM_TEXT_BLUE
    { 128, 255, 32, 128 },  // DEBUG_CAM_TEXT_GREEN
};

void Debug_ClearTextDrawFlags(void) {
    sDebugTextDrawFlags = 0;
}

void Debug_ScreenText(s32 index, s32 colorIndex, const char* text) {
    DebugTextBufferEntry* entry = &sDebugTextBuffer[index];
    char* textDest = entry->text;

    sDebugTextDrawFlags |= DEBUG_TEXT_DRAW_TEXT;
    entry->colorIndex = colorIndex;

    do {
        *textDest++ = *text;
    } while (*text++ != '\0');
}

void Debug_DrawScreenText(GfxPrint* printer) {
    DebugTextBufferEntry* entry;
    Color_RGBA8* color;
    s32 y;

    entry = sDebugTextBuffer;
    for (y = 20; y < 20 + ARRAY_COUNT(sDebugTextBuffer); y++) {
        GfxPrint_SetPos(printer, 26, y);
        color = &sDebugTextColors[entry->colorIndex];
        GfxPrint_SetColor(printer, color->r, color->g, color->b, color->a);
        GfxPrint_Printf(printer, "%s", entry->text);
        *entry->text = '\0';
        entry++;
    }
}

void DebugCamera_ScreenText(u8 x, u8 y, u8 colorIndex, const char* text) {
    DebugCamTextBufferEntry* entry = &sDebugCamTextBuffer[sDebugCamTextEntryCount];
    char* textDest;
    s16 charCount;

    sDebugTextDrawFlags |= DEBUG_TEXT_DRAW_CAM_TEXT;
    if (sDebugCamTextEntryCount < ARRAY_COUNT(sDebugCamTextBuffer)) {
        entry->x = x;
        entry->y = y;
        entry->colorIndex = colorIndex;

        // Copy text into the entry, truncating if needed
        charCount = 0;
        textDest = entry->text;

        while ((*textDest++ = *text++) != '\0') {
            if (charCount++ >= ARRAY_COUNT(entry->text)) {
                break;
            }
        }

        *textDest = '\0';

        sDebugCamTextEntryCount++;
    }
}

void DebugCamera_DrawScreenText(GfxPrint* printer) {
    DebugCamTextBufferEntry* entry;
    Color_RGBA8* color;
    s32 i;

    for (i = 0; i < sDebugCamTextEntryCount; i++) {
        entry = &sDebugCamTextBuffer[i];
        color = &sDebugCamTextColors[entry->colorIndex];

        GfxPrint_SetColor(printer, color->r, color->g, color->b, color->a);
        GfxPrint_SetPos(printer, entry->x, entry->y);
        GfxPrint_Printf(printer, "%s", entry->text);
    }
}

void Debug_DrawText(GraphicsContext* gfxCtx) {
    Gfx* gfx;
    Gfx* gfxHead;
    GfxPrint printer;

    if (THGA_GetRemaining(&gfxCtx->polyOpa) >= 0x2800) {
        GfxPrint_Init(&printer);

        OPEN_DISPS(gfxCtx);

        gfxHead = POLY_OPA_DISP;
        gfx = Graph_GfxPlusOne(gfxHead);
        gSPDisplayList(DEBUG_DISP++, gfx);

        GfxPrint_Open(&printer, gfx);

        if (sDebugTextDrawFlags & DEBUG_TEXT_DRAW_CAM_TEXT) {
            DebugCamera_DrawScreenText(&printer);
        }
        sDebugCamTextEntryCount = 0;

        if (sDebugTextDrawFlags & DEBUG_TEXT_DRAW_TEXT) {
            Debug_DrawScreenText(&printer);
        }

        gfx = GfxPrint_Close(&printer);
        gSPEndDisplayList(gfx++);
        Graph_BranchDlist(gfxHead, gfx);
        POLY_OPA_DISP = gfx;

        CLOSE_DISPS(gfxCtx);

        GfxPrint_Destroy(&printer);
    }
}
