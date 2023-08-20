#include "global.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"

typedef struct {
    u8 x;
    u8 y;
    u8 colorId;
    char text[0x15];
} PrintTextBuffer;

typedef struct {
    u16 push;
    u16 held;
} InputCombo;

GameInfo* gGameInfo;
s32 D_8015FA94; // no known symbols
PrintTextBuffer D_8015FA98[0x16];

s16 D_8011E0B0 = 0; // PrintTextBuffer index
Color_RGBA8 printTextColors[] = {
    { 255, 255, 32, 192 }, { 255, 150, 128, 192 }, { 128, 96, 0, 64 },     { 192, 128, 16, 128 },
    { 255, 192, 32, 128 }, { 230, 230, 220, 64 },  { 128, 150, 255, 128 }, { 128, 255, 32, 128 },
};

InputCombo inputCombos[REG_GROUPS] = {
    { BTN_L, BTN_CUP },    { BTN_L, BTN_CLEFT }, { BTN_L, BTN_CDOWN }, { BTN_L, BTN_A },          { BTN_R, BTN_CDOWN },
    { BTN_L, BTN_CRIGHT }, { BTN_L, BTN_R },     { BTN_L, BTN_DLEFT }, { BTN_L, BTN_DRIGHT },     { BTN_L, BTN_DUP },
    { BTN_L, BTN_B },      { BTN_L, BTN_Z },     { BTN_L, BTN_DDOWN }, { BTN_R, BTN_A },          { BTN_R, BTN_B },
    { BTN_R, BTN_Z },      { BTN_R, BTN_L },     { BTN_R, BTN_CUP },   { BTN_R, BTN_CRIGHT },     { BTN_R, BTN_DLEFT },
    { BTN_R, BTN_CLEFT },  { BTN_R, BTN_START }, { BTN_L, BTN_START }, { BTN_R, BTN_DRIGHT },     { BTN_R, BTN_DUP },
    { BTN_START, BTN_R },  { BTN_START, BTN_A }, { BTN_START, BTN_B }, { BTN_START, BTN_CRIGHT },
};

char regChar[] = " SOPQMYDUIZCNKXcsiWAVHGmnBdkb";

// initialize GameInfo
void func_800636C0(void) {
    s32 i;

    gGameInfo = (GameInfo*)SYSTEM_ARENA_MALLOC_DEBUG(sizeof(GameInfo));
    gGameInfo->regPage = 0;
    gGameInfo->regGroup = 0;
    gGameInfo->regCur = 0;
    gGameInfo->dpadLast = 0;
    gGameInfo->repeat = 0;
    for (i = 0; i < ARRAY_COUNT(gGameInfo->data); i++) {
        gGameInfo->data[i] = 0;
    }
}

// Called when free movement is active.
// 8011D394 to enable camera debugger
void func_8006375C(s32 arg0, s32 arg1, const char* text) {
}

// Copy Camera Debugger Text
void func_8006376C(u8 x, u8 y, u8 colorId, const char* text) {
    PrintTextBuffer* buf;
    char* bufText;
    s16 i;

    buf = &D_8015FA98[D_8011E0B0];
    if (D_8011E0B0 < 0x16) {
        buf->x = x;
        buf->y = y;
        buf->colorId = colorId;

        i = 0;
        bufText = buf->text;
        while ((*bufText++ = *text++)) {
            if (i++ > 0x14) {
                break;
            }
        }

        *bufText = '\0';
        D_8011E0B0++;
    }
}

// Draw Text
void func_80063828(GfxPrint* printer) {
    s32 i;
    Color_RGBA8* color;
    PrintTextBuffer* buffer;
    char* text;

    i = 0;
    if (D_8011E0B0 > 0) {
        do {
            buffer = &D_8015FA98[i];
            text = buffer->text;

            color = &printTextColors[buffer->colorId];
            GfxPrint_SetColor(printer, color->r, color->g, color->b, color->a);
            GfxPrint_SetPos(printer, buffer->x, buffer->y);
            GfxPrint_Printf(printer, "%s", text);
            i += 1;
        } while (i < D_8011E0B0);
    }
}

// Edit REG
void func_8006390C(Input* input) {

    s32 dpad;
    s32 regGroup;
    s32 increment;
    InputCombo* input_combo;
    s32 i;

    if (!CVarGetInteger("gDebugEnabled", 0))
        return;

    regGroup = (gGameInfo->regGroup * REG_PAGES + gGameInfo->regPage) * REG_PER_PAGE - REG_PER_PAGE;
    dpad = input->cur.button & (BTN_DUP | BTN_DLEFT | BTN_DRIGHT | BTN_DDOWN);
    if (CHECK_BTN_ALL(input->cur.button, BTN_L) || CHECK_BTN_ALL(input->cur.button, BTN_R) ||
        CHECK_BTN_ALL(input->cur.button, BTN_START)) {
        input_combo = inputCombos;
        for (i = 0; i < REG_GROUPS; i++) {
            if (~(~input_combo->push | input->cur.button) || ~(~input_combo->held | input->press.button)) {
                input_combo++;
            } else {
                break;
            }
        }

        if (i < REG_GROUPS) {
            if (i == gGameInfo->regGroup) {
                gGameInfo->regPage = (gGameInfo->regPage + 1) % (REG_PAGES + 1);
                return;
            }
            gGameInfo->regGroup = i;
            gGameInfo->regPage = 0;
        }
    } else {
        switch (gGameInfo->regPage - 1) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:

                if (dpad == gGameInfo->dpadLast) {
                    gGameInfo->repeat--;
                    if (gGameInfo->repeat < 0) {
                        gGameInfo->repeat = 1;
                    } else {
                        dpad ^= gGameInfo->dpadLast;
                    }
                } else {
                    gGameInfo->repeat = 0x10;
                    gGameInfo->dpadLast = dpad;
                }

                increment = CHECK_BTN_ANY(dpad, BTN_DRIGHT)  ? (CHECK_BTN_ALL(input->cur.button, BTN_A | BTN_B) ? 1000
                                                                : CHECK_BTN_ALL(input->cur.button, BTN_A)       ? 100
                                                                : CHECK_BTN_ALL(input->cur.button, BTN_B)       ? 10
                                                                                                                : 1)
                            : CHECK_BTN_ANY(dpad, BTN_DLEFT) ? (CHECK_BTN_ALL(input->cur.button, BTN_A | BTN_B) ? -1000
                                                                : CHECK_BTN_ALL(input->cur.button, BTN_A)       ? -100
                                                                : CHECK_BTN_ALL(input->cur.button, BTN_B)       ? -10
                                                                                                                : -1)
                                                             : 0;

                gGameInfo->data[gGameInfo->regCur + regGroup] += increment;
                if (CHECK_BTN_ANY(dpad, BTN_DUP)) {
                    gGameInfo->regCur--;
                    if (gGameInfo->regCur < 0) {
                        gGameInfo->regCur = REG_PER_PAGE - 1;
                    }
                } else if (CHECK_BTN_ANY(dpad, BTN_DDOWN)) {
                    gGameInfo->regCur++;
                    if (gGameInfo->regCur >= REG_PER_PAGE) {
                        gGameInfo->regCur = 0;
                    }
                }
                if (iREG(0)) {
                    iREG(0) = 0;
                    func_800AA000(0, iREG(1), iREG(2), iREG(3));
                }
        }
    }
}

// Draw Memory Viewer
void func_80063C04(GfxPrint* printer) {
    s32 i;
    s32 page = (gGameInfo->regPage * REG_PER_PAGE) - REG_PER_PAGE;
    s32 regGroup = (gGameInfo->regGroup * REG_PAGES + gGameInfo->regPage) * REG_PER_PAGE - REG_PER_PAGE;
    s32 pad;
    char name[3];

    if (!CVarGetInteger("gDebugEnabled", 0))
        return;

    // set up register name string
    name[0] = 'R';
    name[1] = regChar[gGameInfo->regGroup]; // r_group type char
    name[2] = '\0';
    GfxPrint_SetColor(printer, 0, 128, 128, 128);

    for (i = 0; i != REG_PER_PAGE; i++) {
        if (i == gGameInfo->regCur) {
            GfxPrint_SetColor(printer, 0, 255, 255, 255);
        }
        GfxPrint_SetPos(printer, 3, i + 5);
        GfxPrint_Printf(printer, "%s%02d%6d", &name, page + i, gGameInfo->data[i + regGroup]);
        if (i == gGameInfo->regCur) {
            GfxPrint_SetColor(printer, 0, 128, 128, 128);
        }
    }
}

void func_80063D7C(GraphicsContext* gfxCtx) {
    Gfx* sp7C;
    Gfx* sp78;
    GfxPrint printer;
    Gfx* tempRet;

    if (!CVarGetInteger("gDebugEnabled", 0) || GameInteractor_NoUIActive()) {
        return;
    }

    OPEN_DISPS(gfxCtx);

    GfxPrint_Init(&printer);
    sp78 = POLY_OPA_DISP;
    tempRet = Graph_GfxPlusOne(POLY_OPA_DISP);
    gSPDisplayList(OVERLAY_DISP++, tempRet);
    GfxPrint_Open(&printer, tempRet);

    if ((OREG(0) == 1) || (OREG(0) == 8)) {
        func_80063828(&printer);
    }

    if (gGameInfo->regPage != 0) {
        func_80063C04(&printer);
    }

    D_8011E0B0 = 0;
    sp7C = GfxPrint_Close(&printer);
    gSPEndDisplayList(sp7C++);
    Graph_BranchDlist(sp78, sp7C);
    POLY_OPA_DISP = sp7C;

    CLOSE_DISPS(gfxCtx);

    GfxPrint_Destroy(&printer);
}
