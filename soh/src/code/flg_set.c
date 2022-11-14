#include "global.h"

void FlagSet_Update(PlayState* play) {
    static s32 entryIdx = 0;
    static u32 curBit = 0;
    static s32 timer = 0;
    static s32 bitIdx; // ? this doesn't need to be static

    FlagSetEntry entries[53] = {
        { &gSaveContext.eventChkInf[0], "event_chk_inf[0]" },   { &gSaveContext.eventChkInf[1], "event_chk_inf[1]" },
        { &gSaveContext.eventChkInf[2], "event_chk_inf[2]" },   { &gSaveContext.eventChkInf[3], "event_chk_inf[3]" },
        { &gSaveContext.eventChkInf[4], "event_chk_inf[4]" },   { &gSaveContext.eventChkInf[5], "event_chk_inf[5]" },
        { &gSaveContext.eventChkInf[6], "event_chk_inf[6]" },   { &gSaveContext.eventChkInf[7], "event_chk_inf[7]" },
        { &gSaveContext.eventChkInf[8], "event_chk_inf[8]" },   { &gSaveContext.eventChkInf[9], "event_chk_inf[9]" },
        { &gSaveContext.eventChkInf[10], "event_chk_inf[10]" }, { &gSaveContext.eventChkInf[11], "event_chk_inf[11]" },
        { &gSaveContext.eventChkInf[12], "event_chk_inf[12]" }, { &gSaveContext.eventChkInf[13], "event_chk_inf[13]" },
        { &gSaveContext.itemGetInf[0], "item_get_inf[0]" },     { &gSaveContext.itemGetInf[1], "item_get_inf[1]" },
        { &gSaveContext.itemGetInf[2], "item_get_inf[2]" },     { &gSaveContext.itemGetInf[3], "item_get_inf[3]" },
        { &gSaveContext.infTable[0], "inf_table[0]" },          { &gSaveContext.infTable[1], "inf_table[1]" },
        { &gSaveContext.infTable[2], "inf_table[2]" },          { &gSaveContext.infTable[3], "inf_table[3]" },
        { &gSaveContext.infTable[4], "inf_table[4]" },          { &gSaveContext.infTable[5], "inf_table[5]" },
        { &gSaveContext.infTable[6], "inf_table[6]" },          { &gSaveContext.infTable[7], "inf_table[7]" },
        { &gSaveContext.infTable[8], "inf_table[8]" },          { &gSaveContext.infTable[9], "inf_table[9]" },
        { &gSaveContext.infTable[10], "inf_table[10]" },        { &gSaveContext.infTable[11], "inf_table[11]" },
        { &gSaveContext.infTable[12], "inf_table[12]" },        { &gSaveContext.infTable[13], "inf_table[13]" },
        { &gSaveContext.infTable[14], "inf_table[14]" },        { &gSaveContext.infTable[15], "inf_table[15]" },
        { &gSaveContext.infTable[16], "inf_table[16]" },        { &gSaveContext.infTable[17], "inf_table[17]" },
        { &gSaveContext.infTable[18], "inf_table[18]" },        { &gSaveContext.infTable[19], "inf_table[19]" },
        { &gSaveContext.infTable[20], "inf_table[20]" },        { &gSaveContext.infTable[21], "inf_table[21]" },
        { &gSaveContext.infTable[22], "inf_table[22]" },        { &gSaveContext.infTable[23], "inf_table[23]" },
        { &gSaveContext.infTable[24], "inf_table[24]" },        { &gSaveContext.infTable[25], "inf_table[25]" },
        { &gSaveContext.infTable[26], "inf_table[26]" },        { &gSaveContext.infTable[27], "inf_table[27]" },
        { &gSaveContext.infTable[28], "inf_table[28]" },        { &gSaveContext.infTable[29], "inf_table[29]" },
        { &gSaveContext.eventInf[0], "event_inf[0]" },          { &gSaveContext.eventInf[1], "event_inf[1]" },
        { &gSaveContext.eventInf[2], "event_inf[2]" },          { &gSaveContext.eventInf[3], "event_inf[3]" },
    };

    GraphicsContext* gfxCtx = play->state.gfxCtx;
    Input* input = &play->state.input[0];
    Gfx* gfx;
    Gfx* polyOpa;

    OPEN_DISPS(gfxCtx);

    {
        GfxPrint printer;
        s32 pad;

        polyOpa = POLY_OPA_DISP;
        gfx = Graph_GfxPlusOne(polyOpa);
        gSPDisplayList(OVERLAY_DISP++, gfx);

        GfxPrint_Init(&printer);
        GfxPrint_Open(&printer, gfx);
        GfxPrint_SetColor(&printer, 250, 50, 50, 255);
        GfxPrint_SetPos(&printer, 4, 13);
        GfxPrint_Printf(&printer, entries[entryIdx].name);
        GfxPrint_SetPos(&printer, 4, 15);

        for (bitIdx = 15; bitIdx >= 0; bitIdx--) {
            if ((u32)bitIdx == curBit) {
                GfxPrint_SetColor(&printer, 200, 200, 200, 255);
            } else {
                GfxPrint_SetColor(&printer, 100, 100, 100, 255);
            }

            if (*entries[entryIdx].value & (1 << bitIdx)) {
                GfxPrint_Printf(&printer, "1");
            } else {
                GfxPrint_Printf(&printer, "0");
            }

            if ((bitIdx % 4) == 0) {
                GfxPrint_Printf(&printer, " ");
            }
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_DLEFT)) {
            timer = 10;
            curBit++;
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_DRIGHT)) {
            curBit--;
            timer = 10;
        }

        if (timer == 0) {
            if (CHECK_BTN_ALL(input->cur.button, BTN_DLEFT)) {
                curBit++;
                timer = 2;
            }
            if (CHECK_BTN_ALL(input->cur.button, BTN_DRIGHT)) {
                curBit--;
                timer = 2;
            }
        }

        curBit %= 16;
        if (CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
            entryIdx--;
            if (entryIdx < 0) {
                entryIdx = 0;
            }
            timer = 10;
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
            timer = 10;
            entryIdx++;
            if (!entries[entryIdx].value) {
                entryIdx--;
            }
        }

        if (timer == 0) {
            if (CHECK_BTN_ALL(input->cur.button, BTN_DUP)) {
                entryIdx--;
                timer = 2;
                if (entryIdx < 0) {
                    entryIdx = 0;
                }
            }
            if (CHECK_BTN_ALL(input->cur.button, BTN_DDOWN)) {
                timer = 2;
                entryIdx++;
                if (!entries[entryIdx].value) {
                    entryIdx--;
                }
            }
        }

        if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
            *entries[entryIdx].value ^= (1 << curBit);
        }

        if (timer != 0) {
            timer--;
        }

        gfx = GfxPrint_Close(&printer);
        GfxPrint_Destroy(&printer);

        gSPEndDisplayList(gfx++);
        Graph_BranchDlist(polyOpa, gfx);
        POLY_OPA_DISP = gfx;
    }

    if (CHECK_BTN_ALL(input->press.button, BTN_L)) {
        play->pauseCtx.debugState = 0;
    }

    CLOSE_DISPS(gfxCtx);
}
