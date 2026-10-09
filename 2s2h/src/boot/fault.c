/**
 * @file fault.c
 *
 * This file implements the screen that may be viewed when the game crashes.
 * This is the second known version of the crash screen, an evolved version from OoT's.
 *
 * When the game crashes, a red bar will be drawn to the top-left of the screen, indicating that the
 * crash screen is available for use. Once this bar appears, it is possible to open the crash screen
 * with the following button combination:
 *
 * (DPad-Left & L & R & C-Right) & Start
 *
 * When entering this button combination, buttons that are &'d together must all be pressed together.
 *
 * "Clients" may be registered with the crash screen to extend its functionality. There are
 * two kinds of client, "Client" and "AddressConverterClient". Clients contribute one or
 * more pages to the crash debugger, while Address Converter Clients allow the crash screen to look up
 * the virtual addresses of dynamically allocated overlays.
 *
 * The crash screen has multiple pages:
 *  - Thread Context
 *      This page shows information about the thread on which the program crashed. It displays
 *      the cause of the crash, state of general-purpose registers, state of floating-point registers
 *      and the floating-point status register. If a floating-point exception caused the crash, it will
 *      be displayed next to the floating-point status register.
 *  - Stack Trace
 *      This page displays a full backtrace from the crashing function back to the start of the thread. It
 *      displays the Program Counter for each function and, if applicable, the Virtual Program Counter
 *      for relocated functions in overlays.
 *  - Client Pages
 *      After the stack trace page, currently registered clients are processed and their pages are displayed.
 *  - Memory Dump
 *      This page implements a scrollable memory dump.
 *  - End Screen
 *      This page informs you that there are no more pages to display.
 *
 * To navigate the pages, START and A may be used to advance to the next page, and L toggles whether to
 * automatically scroll to the next page after some time has passed.
 * DPad-Up may be pressed to enable sending fault pages over osSyncPrintf as well as displaying them on-screen.
 * DPad-Down disables sending fault pages over osSyncPrintf.
 */

#include "fault_internal.h"
#include "fault.h"
#include "prevent_bss_reordering.h"
#include "vt.h"
#include "PR/osint.h"
#include "stackcheck.h"
#include "z64thread.h"
#include "main.h"
#include "controller.h"
#include "macros.h"
#include "global.h"
#include <stdio.h>

#if 0
FaultMgr* sFaultInstance;
f32 sFaultTimeTotal; // read but not set anywhere

const char* sCpuExceptions[] = {
    "Interrupt",
    "TLB modification",
    "TLB exception on load",
    "TLB exception on store",
    "Address error on load",
    "Address error on store",
    "Bus error on inst.",
    "Bus error on data",
    "System call exception",
    "Breakpoint exception",
    "Reserved instruction",
    "Coprocessor unusable",
    "Arithmetic overflow",
    "Trap exception",
    "Virtual coherency on inst.",
    "Floating point exception",
    "Watchpoint exception",
    "Virtual coherency on data",
};

const char* sFpuExceptions[] = {
    "Unimplemented operation", "Invalid operation", "Division by zero", "Overflow", "Underflow", "Inexact operation",
};
#endif
#include <assert.h>
void Fault_SleepImpl(u32 duration) {
    // OSTime value = (duration * OS_CPU_COUNTER) / 1000ULL;

    // Sleep_Cycles(value);
}

/**
 * Registers a fault client.
 *
 * Clients contribute at least one page to the crash screen, drawn by `callback`.
 * Arguments are passed on to the callback through `arg0` and `arg1`.
 */
void Fault_AddClient(FaultClient* client, FaultClientCallback callback, void* arg0, void* arg1) {
#if 0
    OSIntMask mask;
    u32 alreadyExists = false;

    mask = osSetIntMask(1);

    // Ensure the client is not already registered
    {
        FaultClient* iter = sFaultInstance->clients;

        while (iter != NULL) {
            if (iter == client) {
                alreadyExists = true;
                goto end;
            }
            iter = iter->next;
        }
    }

    client->callback = callback;
    client->arg0 = arg0;
    client->arg1 = arg1;
    client->next = sFaultInstance->clients;
    sFaultInstance->clients = client;

end:
    osSetIntMask(mask);

    if (alreadyExists) {
        osSyncPrintf(VT_COL(RED, WHITE) "fault_AddClient: %08x は既にリスト中にある\n" VT_RST, client);
    }
#endif
}

/**
 * Removes a fault client so that the page is no longer displayed if a crash occurs.
 */
void Fault_RemoveClient(FaultClient* client) {
#if 0
    FaultClient* iter = sFaultInstance->clients;
    FaultClient* lastIter = NULL;
    OSIntMask mask;
    u32 listIsEmpty = false;

    mask = osSetIntMask(1);

    while (iter) {
        if (iter == client) {
            if (lastIter != NULL) {
                lastIter->next = client->next;
            } else {
                sFaultInstance->clients = client;
                if (sFaultInstance->clients) {
                    sFaultInstance->clients = client->next;
                } else {
                    listIsEmpty = 1;
                }
            }
            break;
        }

        lastIter = iter;
        iter = iter->next;
    }

    osSetIntMask(mask);

    if (listIsEmpty) {
        osSyncPrintf(VT_COL(RED, WHITE) "fault_RemoveClient: %08x リスト不整合です\n" VT_RST, client);
    }
#endif
}

/**
 * Registers an address converter client. This enables the crash screen to look up virtual
 * addresses of overlays relocated during runtime. Address conversion is carried out by
 * `callback`, which either returns a virtual address or NULL if the address could not
 * be converted.
 *
 * The callback is intended to be
 * `uintptr_t (*callback)(uintptr_t addr, void* arg)`
 * The callback may return 0 if it could not convert the address
 */
void Fault_AddAddrConvClient(FaultAddrConvClient* client, FaultAddrConvClientCallback callback, void* arg) {
#if 0
    OSIntMask mask;
    s32 alreadyExists = false;

    mask = osSetIntMask(1);

    {
        FaultAddrConvClient* iter = sFaultInstance->addrConvClients;

        while (iter != NULL) {
            if (iter == client) {
                alreadyExists = true;
                goto end;
            }
            iter = iter->next;
        }
    }

    client->callback = callback;
    client->arg = arg;
    client->next = sFaultInstance->addrConvClients;
    sFaultInstance->addrConvClients = client;

end:
    osSetIntMask(mask);

    if (alreadyExists) {
        osSyncPrintf(VT_COL(RED, WHITE) "fault_AddressConverterAddClient: %08x は既にリスト中にある\n" VT_RST, client);
    }
#endif
}

void Fault_RemoveAddrConvClient(FaultAddrConvClient* client) {
#if 0
    FaultAddrConvClient* iter = sFaultInstance->addrConvClients;
    FaultAddrConvClient* lastIter = NULL;
    OSIntMask mask;
    s32 listIsEmpty = false;

    mask = osSetIntMask(1);

    while (iter) {
        if (iter == client) {
            if (lastIter != NULL) {
                lastIter->next = client->next;
            } else {
                sFaultInstance->addrConvClients = client;
                if (sFaultInstance->addrConvClients) {
                    sFaultInstance->addrConvClients = client->next;
                } else {
                    listIsEmpty = true;
                }
            }
            break;
        }

        lastIter = iter;
        iter = iter->next;
    }

    osSetIntMask(mask);

    if (listIsEmpty) {
        osSyncPrintf(VT_COL(RED, WHITE) "fault_AddressConverterRemoveClient: %08x は既にリスト中にある\n" VT_RST,
                     client);
    }
#endif
}

/**
 * Converts `addr` to a virtual address via the registered
 * address converter clients
 */
uintptr_t Fault_ConvertAddress(uintptr_t addr) {
#if 0
    uintptr_t ret;
    FaultAddrConvClient* iter = sFaultInstance->addrConvClients;

    while (iter != NULL) {
        if (iter->callback != NULL) {
            ret = iter->callback(addr, iter->arg);
            if (ret != 0) {
                return ret;
            }
        }
        iter = iter->next;
    }

    return 0;
#endif
}

void Fault_Sleep(u32 msec) {
    // Fault_SleepImpl(msec);
}

void Fault_PadCallback(Input* input) {
    // PadMgr_GetInput2(input, false);
}

void Fault_UpdatePadImpl(void) {
    // sFaultInstance->padCallback(sFaultInstance->inputs);
}

/**
 * Awaits user input
 *
 * L toggles auto-scroll
 * DPad-Up enables osSyncPrintf output
 * DPad-Down disables osSyncPrintf output
 * A and DPad-Right continues and returns true
 * DPad-Left continues and returns false
 */
u32 Fault_WaitForInputImpl(void) {
#if 0
    Input* input = &sFaultInstance->inputs[0];
    s32 count = 600;
    u32 pressedBtn;

    while (true) {
        Fault_Sleep(1000 / 60);
        Fault_UpdatePadImpl();

        pressedBtn = input->press.button;

        if (pressedBtn == BTN_L) {
            sFaultInstance->autoScroll = !sFaultInstance->autoScroll;
        }

        if (sFaultInstance->autoScroll) {
            if (count-- < 1) {
                return false;
            }
        } else {
            if ((pressedBtn == BTN_A) || (pressedBtn == BTN_DRIGHT)) {
                return false;
            }

            if (pressedBtn == BTN_DLEFT) {
                return true;
            }

            if (pressedBtn == BTN_DUP) {
                FaultDrawer_SetOsSyncPrintfEnabled(true);
            }

            if (pressedBtn == BTN_DDOWN) {
                FaultDrawer_SetOsSyncPrintfEnabled(false);
            }
        }
    }
#endif
}

void Fault_WaitForInput(void) {
    // Fault_WaitForInputImpl();
}

void Fault_DrawRec(s32 x, s32 y, s32 w, s32 h, u16 color) {
    // FaultDrawer_DrawRecImpl(x, y, x + w - 1, y + h - 1, color);
}

void Fault_FillScreenBlack(void) {
    // FaultDrawer_SetForeColor(GPACK_RGBA5551(255, 255, 255, 1));
    // FaultDrawer_SetBackColor(GPACK_RGBA5551(0, 0, 0, 1));
    // FaultDrawer_FillScreen();
    // FaultDrawer_SetBackColor(GPACK_RGBA5551(0, 0, 0, 0));
}

void Fault_FillScreenRed(void) {
    // FaultDrawer_SetForeColor(GPACK_RGBA5551(255, 255, 255, 1));
    // FaultDrawer_SetBackColor(GPACK_RGBA5551(240, 0, 0, 1));
    // FaultDrawer_FillScreen();
    // FaultDrawer_SetBackColor(GPACK_RGBA5551(0, 0, 0, 0));
}

void Fault_DrawCornerRec(u16 color) {
    // Fault_DrawRec(22, 16, 8, 1, color);
}

void Fault_PrintFReg(s32 index, f32* value) {
#if 0
    u32 raw = *(u32*)value;
    s32 exp = ((raw & 0x7F800000) >> 23) - 127;

    if (((exp > -127) && (exp <= 127)) || (raw == 0)) {
        FaultDrawer_Printf("F%02d:%14.7e ", index, *value);
    } else {
        // Print subnormal floats as their IEEE-754 hex representation
        FaultDrawer_Printf("F%02d:  %08x(16) ", index, raw);
    }
#endif
}

void Fault_LogFReg(s32 idx, f32* value) {
#if 0
    u32 raw = *(u32*)value;
    s32 exp = ((raw & 0x7F800000) >> 23) - 127;

    if (((exp > -127) && (exp <= 127)) || (raw == 0)) {
        osSyncPrintf("F%02d:%14.7e ", index, *value);
    } else {
        osSyncPrintf("F%02d:  %08x(16) ", idx, *(u32*)value);
    }
#endif
}

void Fault_PrintFPCR(u32 value) {
#if 0
    s32 i;
    u32 flag = 0x20000;

    FaultDrawer_Printf("FPCSR:%08xH ", value);

    // Go through each of the six causes and print the name of
    // the first cause that is set
    for (i = 0; i < ARRAY_COUNT(sFpuExceptions); i++) {
        if (value & flag) {
            FaultDrawer_Printf("(%s)", sFpuExceptions[i]);
            break;
        }
        flag >>= 1;
    }
    FaultDrawer_Printf("\n");
#endif
}

void Fault_LogFPCSR(u32 value) {
#if 0
    s32 i;
    u32 flag = 0x20000;

    osSyncPrintf("FPCSR:%08xH  ", value);
    for (i = 0; i < ARRAY_COUNT(sFpuExceptions); i++) {
        if (value & flag) {
            osSyncPrintf("(%s)\n", sFpuExceptions[i]);
            break;
        }
        flag >>= 1;
    }
#endif
}

void Fault_PrintThreadContext(OSThread* thread) {
#if 0
    __OSThreadContext* threadCtx;
    s16 causeStrIdx = _SHIFTR((u32)thread->context.cause, 2, 5);

    if (causeStrIdx == 23) { // Watchpoint
        causeStrIdx = 16;
    }
    if (causeStrIdx == 31) { // Virtual coherency on data
        causeStrIdx = 17;
    }

    FaultDrawer_FillScreen();
    FaultDrawer_SetCharPad(-2, 4);
    FaultDrawer_SetCursor(22, 20);

    threadCtx = &thread->context;
    FaultDrawer_Printf("THREAD:%d (%d:%s)\n", thread->id, causeStrIdx, sCpuExceptions[causeStrIdx]);
    FaultDrawer_SetCharPad(-1, 0);

    FaultDrawer_Printf("PC:%08xH SR:%08xH VA:%08xH\n", (u32)threadCtx->pc, (u32)threadCtx->sr,
                       (u32)threadCtx->badvaddr);
    FaultDrawer_Printf("AT:%08xH V0:%08xH V1:%08xH\n", (u32)threadCtx->at, (u32)threadCtx->v0, (u32)threadCtx->v1);
    FaultDrawer_Printf("A0:%08xH A1:%08xH A2:%08xH\n", (u32)threadCtx->a0, (u32)threadCtx->a1, (u32)threadCtx->a2);
    FaultDrawer_Printf("A3:%08xH T0:%08xH T1:%08xH\n", (u32)threadCtx->a3, (u32)threadCtx->t0, (u32)threadCtx->t1);
    FaultDrawer_Printf("T2:%08xH T3:%08xH T4:%08xH\n", (u32)threadCtx->t2, (u32)threadCtx->t3, (u32)threadCtx->t4);
    FaultDrawer_Printf("T5:%08xH T6:%08xH T7:%08xH\n", (u32)threadCtx->t5, (u32)threadCtx->t6, (u32)threadCtx->t7);
    FaultDrawer_Printf("S0:%08xH S1:%08xH S2:%08xH\n", (u32)threadCtx->s0, (u32)threadCtx->s1, (u32)threadCtx->s2);
    FaultDrawer_Printf("S3:%08xH S4:%08xH S5:%08xH\n", (u32)threadCtx->s3, (u32)threadCtx->s4, (u32)threadCtx->s5);
    FaultDrawer_Printf("S6:%08xH S7:%08xH T8:%08xH\n", (u32)threadCtx->s6, (u32)threadCtx->s7, (u32)threadCtx->t8);
    FaultDrawer_Printf("T9:%08xH GP:%08xH SP:%08xH\n", (u32)threadCtx->t9, (u32)threadCtx->gp, (u32)threadCtx->sp);
    FaultDrawer_Printf("S8:%08xH RA:%08xH LO:%08xH\n\n", (u32)threadCtx->s8, (u32)threadCtx->ra, (u32)threadCtx->lo);

    Fault_PrintFPCR(threadCtx->fpcsr);
    FaultDrawer_Printf("\n");
    Fault_PrintFReg(0, &threadCtx->fp0.f.f_even);
    Fault_PrintFReg(2, &threadCtx->fp2.f.f_even);
    FaultDrawer_Printf("\n");
    Fault_PrintFReg(4, &threadCtx->fp4.f.f_even);
    Fault_PrintFReg(6, &threadCtx->fp6.f.f_even);
    FaultDrawer_Printf("\n");
    Fault_PrintFReg(8, &threadCtx->fp8.f.f_even);
    Fault_PrintFReg(10, &threadCtx->fp10.f.f_even);
    FaultDrawer_Printf("\n");
    Fault_PrintFReg(12, &threadCtx->fp12.f.f_even);
    Fault_PrintFReg(14, &threadCtx->fp14.f.f_even);
    FaultDrawer_Printf("\n");
    Fault_PrintFReg(16, &threadCtx->fp16.f.f_even);
    Fault_PrintFReg(18, &threadCtx->fp18.f.f_even);
    FaultDrawer_Printf("\n");
    Fault_PrintFReg(20, &threadCtx->fp20.f.f_even);
    Fault_PrintFReg(22, &threadCtx->fp22.f.f_even);
    FaultDrawer_Printf("\n");
    Fault_PrintFReg(24, &threadCtx->fp24.f.f_even);
    Fault_PrintFReg(26, &threadCtx->fp26.f.f_even);
    FaultDrawer_Printf("\n");
    Fault_PrintFReg(28, &threadCtx->fp28.f.f_even);
    Fault_PrintFReg(30, &threadCtx->fp30.f.f_even);
    FaultDrawer_Printf("\n");
    FaultDrawer_SetCharPad(0, 0);

    if (sFaultTimeTotal != 0.0f) {
        FaultDrawer_DrawText(160, 216, "%5.2f sec\n", sFaultTimeTotal);
    }
#endif
}

void osSyncPrintfThreadContext(OSThread* thread) {
#if 0
    __OSThreadContext* threadCtx;
    s16 causeStrIdx = _SHIFTR((u32)thread->context.cause, 2, 5);

    if (causeStrIdx == 23) { // Watchpoint
        causeStrIdx = 16;
    }
    if (causeStrIdx == 31) { // Virtual coherency on data
        causeStrIdx = 17;
    }

    threadCtx = &thread->context;
    osSyncPrintf("\n");
    osSyncPrintf("THREAD ID:%d (%d:%s)\n", thread->id, causeStrIdx, sCpuExceptions[causeStrIdx]);

    osSyncPrintf("PC:%08xH   SR:%08xH   VA:%08xH\n", (u32)threadCtx->pc, (u32)threadCtx->sr, (u32)threadCtx->badvaddr);
    osSyncPrintf("AT:%08xH   V0:%08xH   V1:%08xH\n", (u32)threadCtx->at, (u32)threadCtx->v0, (u32)threadCtx->v1);
    osSyncPrintf("A0:%08xH   A1:%08xH   A2:%08xH\n", (u32)threadCtx->a0, (u32)threadCtx->a1, (u32)threadCtx->a2);
    osSyncPrintf("A3:%08xH   T0:%08xH   T1:%08xH\n", (u32)threadCtx->a3, (u32)threadCtx->t0, (u32)threadCtx->t1);
    osSyncPrintf("T2:%08xH   T3:%08xH   T4:%08xH\n", (u32)threadCtx->t2, (u32)threadCtx->t3, (u32)threadCtx->t4);
    osSyncPrintf("T5:%08xH   T6:%08xH   T7:%08xH\n", (u32)threadCtx->t5, (u32)threadCtx->t6, (u32)threadCtx->t7);
    osSyncPrintf("S0:%08xH   S1:%08xH   S2:%08xH\n", (u32)threadCtx->s0, (u32)threadCtx->s1, (u32)threadCtx->s2);
    osSyncPrintf("S3:%08xH   S4:%08xH   S5:%08xH\n", (u32)threadCtx->s3, (u32)threadCtx->s4, (u32)threadCtx->s5);
    osSyncPrintf("S6:%08xH   S7:%08xH   T8:%08xH\n", (u32)threadCtx->s6, (u32)threadCtx->s7, (u32)threadCtx->t8);
    osSyncPrintf("T9:%08xH   GP:%08xH   SP:%08xH\n", (u32)threadCtx->t9, (u32)threadCtx->gp, (u32)threadCtx->sp);
    osSyncPrintf("S8:%08xH   RA:%08xH   LO:%08xH\n", (u32)threadCtx->s8, (u32)threadCtx->ra, (u32)threadCtx->lo);
    osSyncPrintf("\n");
    Fault_LogFPCSR(threadCtx->fpcsr);
    osSyncPrintf("\n");
    Fault_LogFReg(0, &threadCtx->fp0.f.f_even);
    Fault_LogFReg(2, &threadCtx->fp2.f.f_even);
    osSyncPrintf("\n");
    Fault_LogFReg(4, &threadCtx->fp4.f.f_even);
    Fault_LogFReg(6, &threadCtx->fp6.f.f_even);
    osSyncPrintf("\n");
    Fault_LogFReg(8, &threadCtx->fp8.f.f_even);
    Fault_LogFReg(10, &threadCtx->fp10.f.f_even);
    osSyncPrintf("\n");
    Fault_LogFReg(12, &threadCtx->fp12.f.f_even);
    Fault_LogFReg(14, &threadCtx->fp14.f.f_even);
    osSyncPrintf("\n");
    Fault_LogFReg(16, &threadCtx->fp16.f.f_even);
    Fault_LogFReg(18, &threadCtx->fp18.f.f_even);
    osSyncPrintf("\n");
    Fault_LogFReg(20, &threadCtx->fp20.f.f_even);
    Fault_LogFReg(22, &threadCtx->fp22.f.f_even);
    osSyncPrintf("\n");
    Fault_LogFReg(24, &threadCtx->fp24.f.f_even);
    Fault_LogFReg(26, &threadCtx->fp26.f.f_even);
    osSyncPrintf("\n");
    Fault_LogFReg(28, &threadCtx->fp28.f.f_even);
    Fault_LogFReg(30, &threadCtx->fp30.f.f_even);
    osSyncPrintf("\n");
#endif
}

/**
 * Iterates through the active thread queue for a user thread with either
 * the CPU break or Fault flag set.
 */
OSThread* Fault_FindFaultedThread(void) {
#if 0
    OSThread* iter = __osGetActiveQueue();

    while (iter->priority != OS_PRIORITY_THREADTAIL) {
        if ((iter->priority > OS_PRIORITY_IDLE) && (iter->priority < OS_PRIORITY_APPMAX) &&
            (iter->flags & (OS_FLAG_CPU_BREAK | OS_FLAG_FAULT))) {
            return iter;
        }
        iter = iter->tlnext;
    }

    return NULL;
#endif
}
void Fault_Wait5Seconds(void) {
#if 0
    s32 pad;
    OSTime start = osGetTime();

    do {
        Fault_Sleep(1000 / 60);
    } while ((osGetTime() - start) <= OS_SEC_TO_CYCLES(5));

    sFaultInstance->autoScroll = true;
#endif
}

/**
 * Waits for the following button combination to be entered before returning:
 *
 * (DPad-Left & L & R & C-Right) & Start
 */
void Fault_WaitForButtonCombo(void) {
#if 0
    Input* input = &sFaultInstance->inputs[0];

    FaultDrawer_SetForeColor(GPACK_RGBA5551(255, 255, 255, 1));
    FaultDrawer_SetBackColor(GPACK_RGBA5551(0, 0, 0, 1));

    do {
        do {
            Fault_Sleep(1000 / 60);
            Fault_UpdatePadImpl();
        } while (!CHECK_BTN_ALL(input->press.button, BTN_RESET));
    } while (!CHECK_BTN_ALL(input->cur.button, BTN_DLEFT | BTN_L | BTN_R | BTN_CRIGHT));
#endif
}

void Fault_DrawMemDumpContents(const char* title, uintptr_t addr, u32 param_3) {
#if 0
    uintptr_t alignedAddr = addr;
    u32* writeAddr;
    s32 y;
    s32 x;

    // Ensure address is within the bounds of RDRAM (Fault_DrawMemDump has already done this)
    if (alignedAddr < K0BASE) {
        alignedAddr = K0BASE;
    }
    // 8MB RAM, leave room to display 0x100 bytes on the final page
    //! @bug The loop below draws 22 * 4 * 4 = 0x160 bytes per page. Due to this, by scrolling further than
    //! 0x807FFEA0 some invalid bytes are read from outside of 8MB RDRAM space. This does not cause a crash,
    //! however the values it displays are meaningless. On N64 hardware these invalid addresses are read as 0.
    if (alignedAddr > (K0BASE + 0x800000 - 0x100)) {
        alignedAddr = K0BASE + 0x800000 - 0x100;
    }

    // Ensure address is word-aligned
    alignedAddr &= ~3;
    writeAddr = (u32*)alignedAddr;

    Fault_FillScreenBlack();
    FaultDrawer_SetCharPad(-2, 0);

    FaultDrawer_DrawText(36, 18, "%s %08x", title ? title : "PrintDump", alignedAddr);

    if (alignedAddr >= K0BASE && alignedAddr < K2BASE) {
        for (y = 0; y < 22; y++) {
            FaultDrawer_DrawText(24, 28 + y * 9, "%06x", writeAddr);
            for (x = 0; x < 4; x++) {
                FaultDrawer_DrawText(82 + x * 52, 28 + y * 9, "%08x", *writeAddr++);
            }
        }
    }

    FaultDrawer_SetCharPad(0, 0);
#endif
}

/**
 * Draws the memory dump page.
 *
 * DPad-Up scrolls up.
 * DPad-Down scrolls down.
 * Holding A while scrolling speeds up scrolling by a factor of 0x10.
 * Holding B while scrolling speeds up scrolling by a factor of 0x100.
 *
 * L toggles auto-scrolling pages.
 * START and A move on to the next page.
 *
 * @param pc Program counter, pressing C-Up jumps to this address
 * @param sp Stack pointer, pressing C-Down jumps to this address
 * @param cLeftJump Unused parameter, pressing C-Left jumps to this address
 * @param cRightJump Unused parameter, pressing C-Right jumps to this address
 */
void Fault_DrawMemDump(uintptr_t pc, uintptr_t sp, uintptr_t cLeftJump, uintptr_t cRightJump) {
#if 0
    s32 scrollCountdown;
    s32 off;
    Input* input = &sFaultInstance->inputs[0];
    uintptr_t addr = pc;

    do {
        scrollCountdown = 0;
        // Ensure address is within the bounds of RDRAM
        if (addr < K0BASE) {
            addr = K0BASE;
        }
        // 8MB RAM, leave room to display 0x100 bytes on the final page
        if (addr > (K0BASE + 0x800000 - 0x100)) {
            addr = K0BASE + 0x800000 - 0x100;
        }

        // Align down the address to 0x10 bytes and draw the page contents
        addr &= ~0xF;
        Fault_DrawMemDumpContents("Dump", addr, 0);

        scrollCountdown = 600;
        while (sFaultInstance->autoScroll) {
            // Count down until it's time to move on to the next page
            if (scrollCountdown == 0) {
                return;
            }

            scrollCountdown--;

            Fault_Sleep(1000 / 60);
            Fault_UpdatePadImpl();

            if (CHECK_BTN_ALL(input->press.button, BTN_L)) {
                sFaultInstance->autoScroll = false;
            }
        }

        // Wait for input
        do {
            Fault_Sleep(1000 / 60);
            Fault_UpdatePadImpl();
        } while (input->press.button == 0);

        // Move to next page
        if (CHECK_BTN_ALL(input->press.button, BTN_START)) {
            return;
        }

        // Memory dump controls

        off = 0x10;
        if (CHECK_BTN_ALL(input->cur.button, BTN_A)) {
            off *= 0x10;
        }
        if (CHECK_BTN_ALL(input->cur.button, BTN_B)) {
            off *= 0x100;
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
            addr -= off;
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
            addr += off;
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
            addr = pc;
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
            addr = sp;
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
            addr = cLeftJump;
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
            addr = cRightJump;
        }

    } while (!CHECK_BTN_ALL(input->press.button, BTN_L));

    // Resume auto-scroll and move to next page
    sFaultInstance->autoScroll = true;
#endif
}

/**
 * Searches a single function's stack frame for the function it was called from.
 * There are two cases that must be covered: Leaf and non-leaf functions.
 *
 * A leaf function is one that does not call any other function, in this case the
 * return address need not be saved to the stack. Since a leaf function does not
 * call other functions, only the function the stack trace begins in could possibly
 * be a leaf function, in which case the return address is in the thread context's
 * $ra already, as it never left.
 *
 * The procedure is therefore
 *  - Iterate instructions
 *  - Once jr $ra is found, set pc to $ra
 *  - Done after delay slot
 *
 * A non-leaf function calls other functions, it is necessary for the return address
 * to be saved to the stack. In these functions, it is important to keep track of the
 * stack frame size of each function.
 *
 * The procedure is therefore
 *  - Iterate instructions
 *  - If lw $ra <imm>($sp) is found, fetch the saved $ra from stack memory
 *  - If addiu $sp, $sp, <imm> is found, modify $sp by the immediate value
 *  - If jr $ra is found, set pc to $ra
 *  - Done after delay slot
 *
 * Note that searching for one jr $ra is sufficient, as only leaf functions can have
 * multiple jr $ra in the same function.
 *
 * There is also additional handling for eret and j. Neither of these instructions
 * appear in IDO compiled C, however do show up in the exception handler. It is not
 * possible to backtrace through an eret as an interrupt can occur at any time, so
 * there is no choice but to give up here. For j instructions, they can be followed
 * and the backtrace may continue as normal.
 */
void Fault_WalkStack(uintptr_t* spPtr, uintptr_t* pcPtr, uintptr_t* raPtr) {
#if 0
    uintptr_t sp = *spPtr;
    uintptr_t pc = *pcPtr;
    uintptr_t ra = *raPtr;
    u32 lastInsn;
    u16 insnHi;
    s16 insnLo;
    u32 imm;

    if ((sp % 4 != 0) || (sp < K0BASE) || (sp >= K2BASE) || (ra % 4 != 0) || (ra < K0BASE) || (ra >= K2BASE)) {
        *spPtr = 0;
        *pcPtr = 0;
        *raPtr = 0;
        return;
    }

    if ((pc % 4 != 0) || (pc < K0BASE) || (pc >= K2BASE)) {
        *pcPtr = ra;
        return;
    }

    lastInsn = 0;
    while (true) {
        insnHi = *(uintptr_t*)pc >> 16;
        insnLo = *(uintptr_t*)pc & 0xFFFF;
        imm = insnLo;

        if (insnHi == 0x8FBF) {
            // lw $ra, <imm>($sp)
            // read return address saved on the stack
            ra = *(uintptr_t*)(sp + imm);
        } else if (insnHi == 0x27BD) {
            // addiu $sp, $sp, <imm>
            // stack pointer increment or decrement
            sp += imm;
        } else if (*(uintptr_t*)pc == 0x42000018) {
            // eret
            // cannot backtrace through an eret, give up
            sp = 0;
            pc = 0;
            ra = 0;
            goto done;
        }
        if (lastInsn == 0x3E00008) {
            // jr $ra
            // return to previous function
            pc = ra;
            goto done;
        } else if (lastInsn >> 26 == 2) {
            // j <target>
            // extract jump target
            pc = (pc >> 28 << 28) | (lastInsn << 6 >> 4);
            goto done;
        }

        lastInsn = *(uintptr_t*)pc;
        pc += sizeof(u32);
    }

done:
    *spPtr = sp;
    *pcPtr = pc;
    *raPtr = ra;
#endif
}

/**
 * Draws the stack trace page contents for the specified thread
 */
void Fault_DrawStackTrace(OSThread* thread, u32 flags) {
#if 0
    s32 line;
    uintptr_t sp = thread->context.sp;
    uintptr_t ra = thread->context.ra;
    uintptr_t pc = thread->context.pc;
    s32 pad;
    uintptr_t addr;

    Fault_FillScreenBlack();
    FaultDrawer_DrawText(120, 16, "STACK TRACE");
    FaultDrawer_DrawText(36, 24, "SP       PC       (VPC)");

    for (line = 1; (line < 22) && (((ra != 0) || (sp != 0)) && (pc != (uintptr_t)__osCleanupThread)); line++) {
        FaultDrawer_DrawText(0x24, line * 8 + 24, "%08x %08x", sp, pc);

        if (flags & 1) {
            // Try to convert the relocated program counter to the corresponding unrelocated virtual address
            addr = Fault_ConvertAddress(pc);
            if (addr != 0) {
                FaultDrawer_Printf(" -> %08x", addr);
            }
        } else {
            FaultDrawer_Printf(" -> ????????");
        }

        Fault_WalkStack(&sp, &pc, &ra);
    }
#endif
}

void Fault_LogStackTrace(OSThread* thread, u32 flags) {
#if 0
    s32 line;
    uintptr_t sp = thread->context.sp;
    uintptr_t ra = thread->context.ra;
    uintptr_t pc = thread->context.pc;
    uintptr_t addr;

    osSyncPrintf("STACK TRACE");
    osSyncPrintf("SP       PC       (VPC)\n");

    for (line = 1; (line < 22) && (((ra != 0) || (sp != 0)) && (pc != (uintptr_t)__osCleanupThread)); line++) {
        osSyncPrintf("%08x %08x", sp, pc);

        if (flags & 1) {
            // Try to convert the relocated program counter to the corresponding unrelocated virtual address
            addr = Fault_ConvertAddress(pc);
            if (addr != 0) {
                osSyncPrintf(" -> %08x", addr);
            }
        } else {
            osSyncPrintf(" -> ????????");
        }
        osSyncPrintf("\n");

        Fault_WalkStack(&sp, &pc, &ra);
    }
#endif
}

void Fault_ResumeThread(OSThread* thread) {
#if 0
    thread->context.cause = 0;
    thread->context.fpcsr = 0;
    thread->context.pc += sizeof(u32);
    *(u32*)thread->context.pc = 0x0000000D; // write in a break instruction
    osWritebackDCache((void*)thread->context.pc, 4);
    osInvalICache((void*)thread->context.pc, 4);
    osStartThread(thread);
#endif
}

void Fault_DisplayFrameBuffer(void) {
#if 0
    void* fb;

    osViSetYScale(1.0f);
    osViSetMode(&osViModeNtscLan1);
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF | OS_VI_DITHER_FILTER_ON);
    osViBlack(false);

    if (sFaultInstance->fb) {
        fb = sFaultInstance->fb;
    } else {
        fb = osViGetNextFramebuffer();
        if ((uintptr_t)fb == K0BASE) {
            fb = (void*)(PHYS_TO_K0(osMemSize) - SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(u16));
        }
    }

    osViSwapBuffer(fb);
    FaultDrawer_SetDrawerFrameBuffer(fb, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif
}

/**
 * Runs all registered fault clients. Each fault client displays a page
 * on the crash screen.
 */
void Fault_ProcessClients(void) {
#if 0
    FaultClient* client = sFaultInstance->clients;
    s32 idx = 0;

    while (client != NULL) {
        if (client->callback != NULL) {
            Fault_FillScreenBlack();
            FaultDrawer_SetCharPad(-2, 0);
            FaultDrawer_Printf(FAULT_COLOR(DARK_GRAY) "CallBack (%d) %08x %08x %08x\n" FAULT_COLOR(WHITE), idx++,
                               client, client->arg0, client->arg1);
            FaultDrawer_SetCharPad(0, 0);
            client->callback(client->arg0, client->arg1);
            Fault_WaitForInput();
            Fault_DisplayFrameBuffer();
        }
        client = client->next;
    }
#endif
}

void Fault_SetOptionsFromController3(void) {
#if 0
    static u32 faultCustomOptions;
    Input* input3 = &sFaultInstance->inputs[3];
    u32 pad;
    uintptr_t pc;
    uintptr_t ra;
    uintptr_t sp;

    // BTN_RESET is the "neutral reset". Corresponds to holding L+R and pressing S
    if (CHECK_BTN_ALL(input3->press.button, BTN_RESET)) {
        faultCustomOptions = !faultCustomOptions;
    }

    if (faultCustomOptions) {
        pc = gGraphThread.context.pc;
        ra = gGraphThread.context.ra;
        sp = gGraphThread.context.sp;
        if (CHECK_BTN_ALL(input3->cur.button, BTN_R)) {
            static u32 faultCopyToLog;

            faultCopyToLog = !faultCopyToLog;
            FaultDrawer_SetOsSyncPrintfEnabled(faultCopyToLog);
        }
        if (CHECK_BTN_ALL(input3->cur.button, BTN_A)) {
            osSyncPrintf("GRAPH PC=%08x RA=%08x STACK=%08x\n", pc, ra, sp);
        }
        if (CHECK_BTN_ALL(input3->cur.button, BTN_B)) {
            FaultDrawer_SetDrawerFrameBuffer(osViGetNextFramebuffer(), 0x140, 0xF0);
            Fault_DrawRec(0, 0xD7, 0x140, 9, 1);
            FaultDrawer_SetCharPad(-2, 0);
            FaultDrawer_DrawText(0x20, 0xD8, "GRAPH PC %08x RA %08x SP %08x", pc, ra, sp);
        }
    }
#endif
}

void Fault_UpdatePad(void) {
    // Fault_UpdatePadImpl();
    // Fault_SetOptionsFromController3();
}

//#define FAULT_MSG_CPU_BREAK ((OSMesg)1)
//#define FAULT_MSG_FAULT ((OSMesg)2)
//#define FAULT_MSG_UNK ((OSMesg)3)

void Fault_ThreadEntry(void* arg) {
#if 0
    OSMesg msg;
    u32 pad;
    OSThread* faultedThread;

    // Direct OS event messages to the fault event queue
    osSetEventMesg(OS_EVENT_CPU_BREAK, &sFaultInstance->queue, FAULT_MSG_CPU_BREAK);
    osSetEventMesg(OS_EVENT_FAULT, &sFaultInstance->queue, FAULT_MSG_FAULT);

    while (true) {
        do {
            // Wait for a thread to hit a fault
            osRecvMesg(&sFaultInstance->queue, &msg, OS_MESG_BLOCK);

            if (msg == FAULT_MSG_CPU_BREAK) {
                sFaultInstance->msgId = (u32)FAULT_MSG_CPU_BREAK;
                // "Fault manager: OS_EVENT_CPU_BREAK received"
                osSyncPrintf("フォルトマネージャ:OS_EVENT_CPU_BREAKを受信しました\n");
            } else if (msg == FAULT_MSG_FAULT) {
                sFaultInstance->msgId = (u32)FAULT_MSG_FAULT;
                // "Fault manager: OS_EVENT_FAULT received"
                osSyncPrintf("フォルトマネージャ:OS_EVENT_FAULTを受信しました\n");
            } else if (msg == FAULT_MSG_UNK) {
                Fault_UpdatePad();
                faultedThread = NULL;
                continue;
            } else {
                sFaultInstance->msgId = (u32)FAULT_MSG_UNK;
                // "Fault manager: received an unknown message"
                osSyncPrintf("フォルトマネージャ:不明なメッセージを受信しました\n");
            }

            faultedThread = __osGetCurrFaultedThread();
            osSyncPrintf("__osGetCurrFaultedThread()=%08x\n", faultedThread);

            if (faultedThread == NULL) {
                faultedThread = Fault_FindFaultedThread();
                osSyncPrintf("FindFaultedThread()=%08x\n", faultedThread);
            }
        } while (faultedThread == NULL);

        __osSetFpcCsr(__osGetFpcCsr() & ~(FPCSR_EV | FPCSR_EZ | FPCSR_EO | FPCSR_EU | FPCSR_EI));
        sFaultInstance->faultedThread = faultedThread;

        while (!sFaultInstance->faultHandlerEnabled) {
            Fault_Sleep(1000);
        }
        Fault_Sleep(1000 / 2);

        // Show fault framebuffer
        Fault_DisplayFrameBuffer();

        if (sFaultInstance->autoScroll) {
            Fault_Wait5Seconds();
        } else {
            // Draw error bar signifying the crash screen is available
            Fault_DrawCornerRec(GPACK_RGBA5551(255, 0, 0, 1));
            Fault_WaitForButtonCombo();
        }

        // Set auto-scrolling and default colors
        sFaultInstance->autoScroll = true;
        FaultDrawer_SetForeColor(GPACK_RGBA5551(255, 255, 255, 1));
        FaultDrawer_SetBackColor(GPACK_RGBA5551(0, 0, 0, 0));

        // Draw pages
        do {
            // Thread context page
            Fault_PrintThreadContext(faultedThread);
            osSyncPrintfThreadContext(faultedThread);
            Fault_WaitForInput();

            // Stack trace page
            Fault_DrawStackTrace(faultedThread, 0);
            Fault_LogStackTrace(faultedThread, 0);
            Fault_WaitForInput();

            // Client pages
            Fault_ProcessClients();

            // Memory dump page
            Fault_DrawMemDump(faultedThread->context.pc - 0x100, faultedThread->context.sp, 0, 0);
            Fault_DrawStackTrace(faultedThread, 1);
            Fault_LogStackTrace(faultedThread, 1);
            Fault_WaitForInput();

            // End page
            Fault_FillScreenRed();
            FaultDrawer_DrawText(64, 80, "    CONGRATURATIONS!    ");
            FaultDrawer_DrawText(64, 90, "All Pages are displayed.");
            FaultDrawer_DrawText(64, 100, "       THANK YOU!       ");
            FaultDrawer_DrawText(64, 110, " You are great debugger!");
            Fault_WaitForInput();
        } while (!sFaultInstance->exit);

        while (!sFaultInstance->exit) {}

        Fault_ResumeThread(faultedThread);
    }
#endif
}

void Fault_SetFrameBuffer(void* fb, u16 w, u16 h) {
    // sFaultInstance->fb = fb;
    // FaultDrawer_SetDrawerFrameBuffer(fb, w, h);
}

// STACK(sFaultStack, 0x600);
// StackEntry sFaultStackInfo;
// FaultMgr gFaultMgr;

void Fault_Init(void) {
#if 0
    sFaultInstance = &gFaultMgr;
    bzero(sFaultInstance, sizeof(FaultMgr));
    FaultDrawer_Init();
    FaultDrawer_SetInputCallback(Fault_WaitForInput);
    sFaultInstance->exit = false;
    sFaultInstance->msgId = 0;
    sFaultInstance->faultHandlerEnabled = false;
    sFaultInstance->faultedThread = NULL;
    sFaultInstance->padCallback = Fault_PadCallback;
    sFaultInstance->clients = NULL;
    sFaultInstance->autoScroll = false;
    gFaultMgr.faultHandlerEnabled = true;
    osCreateMesgQueue(&sFaultInstance->queue, sFaultInstance->msg, ARRAY_COUNT(sFaultInstance->msg));
    StackCheck_Init(&sFaultStackInfo, sFaultStack, STACK_TOP(sFaultStack), 0, 0x100, "fault");
    osCreateThread(&sFaultInstance->thread, Z_THREAD_ID_FAULT, Fault_ThreadEntry, NULL, STACK_TOP(sFaultStack),
                   Z_PRIORITY_FAULT);
    osStartThread(&sFaultInstance->thread);
#endif
}

/**
 * Fault page for Hungup crashes. Displays the thread id and two messages
 * specified in arguments to `Fault_AddHungupAndCrashImpl`.
 */
void Fault_HangupFaultClient(const char* exp1, const char* exp2) {
#if 0
    osSyncPrintf("HungUp on Thread %d\n", osGetThreadId(NULL));
    osSyncPrintf("%s\n", exp1 != NULL ? exp1 : "(NULL)");
    osSyncPrintf("%s\n", exp2 != NULL ? exp2 : "(NULL)");
    FaultDrawer_Printf("HungUp on Thread %d\n", osGetThreadId(NULL));
    FaultDrawer_Printf("%s\n", exp1 != NULL ? exp1 : "(NULL)");
    FaultDrawer_Printf("%s\n", exp2 != NULL ? exp2 : "(NULL)");
#endif
}

/**
 * Immediately crashes the current thread, for cases where an irrecoverable
 * error occurs. The parameters specify two messages detailing the error, one
 * or both may be NULL.
 */
void Fault_AddHungupAndCrashImpl(const char* exp1, const char* exp2) {
    assert(0);
    // FaultClient client;
    // s32 pad;
    //
    // Fault_AddClient(&client, (void*)Fault_HangupFaultClient, (void*)exp1, (void*)exp2);
    //*(u32*)0x11111111 = 0; // trigger an exception via unaligned memory access
}

/**
 * Like `Fault_AddHungupAndCrashImpl`, however provides a fixed message containing
 * file and line number
 */
void Fault_AddHungupAndCrash(const char* file, s32 line) {
    char msg[0x100];

    sprintf(msg, "HungUp %s:%d", file, line);
    Fault_AddHungupAndCrashImpl(msg, NULL);
}
