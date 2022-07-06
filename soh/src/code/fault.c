#include "global.h"
#include "vt.h"
#include "alloca.h"

// data
const char* sExceptionNames[] = {
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
    "Unimplemented operation",
    "Invalid operation",
    "Division by zero",
    "Overflow",
    "Underflow",
    "Inexact operation",
};

// bss
extern FaultThreadStruct* sFaultStructPtr;
extern u8 sFaultIsWaitingForInput;
extern char sFaultStack[0x600];
extern StackEntry sFaultThreadInfo;
extern FaultThreadStruct gFaultStruct;

void Fault_SleepImpl(u32 duration) {
    u64 value = (duration * OS_CPU_COUNTER) / 1000ull;

    Sleep_Cycles(value);
}

void Fault_ClientProcessThread(void* arg) {
    FaultClientContext* ctx = (FaultClientContext*)arg;

    if (ctx->callback != 0) {
        ctx->ret = ctx->callback(ctx->param0, ctx->param1);
    }

    if (ctx->queue != NULL) {
        osSendMesg(ctx->queue, ctx->msg, OS_MESG_BLOCK);
    }
}

void Fault_ProcessClientContext(FaultClientContext* ctx) 
{
#if 0
    OSMesgQueue queue;
    OSMesg msg;
    OSMesg recMsg;
    OSThread* thread;
    OSTimer timer;
    u32 timerMsgVal;

    timerMsgVal = 666;
    thread = NULL;

    osCreateMesgQueue(&queue, &msg, 1);
    ctx->queue = &queue;
    ctx->msg = NULL;

    if (sFaultStructPtr->currClientThreadSp != 0) {
        thread = alloca(sizeof(OSThread));
        osCreateThread(thread, 2, Fault_ClientProcessThread, ctx, sFaultStructPtr->currClientThreadSp,
                       OS_PRIORITY_APPMAX - 1);
        osStartThread(thread);
    } else {
        Fault_ClientProcessThread(ctx);
    }

    while (true) {
        osSetTimer(&timer, OS_USEC_TO_CYCLES(1000000), 0, &queue, (OSMesg)timerMsgVal);
        osRecvMesg(&queue, &recMsg, OS_MESG_BLOCK);

        if (recMsg != (OSMesg)666) {
            break;
        }

        if (!sFaultIsWaitingForInput) {
            ctx->ret = -1;
            break;
        }
    }

    osStopTimer(&timer);

    if (thread != NULL) {
        osStopThread(thread);
        osDestroyThread(thread);
    }
#endif
}

u32 Fault_ProcessClient(u32 callback, u32 param0, u32 param1) {
    FaultClientContext a;

    a.callback = callback;
    a.param0 = param0;
    a.param1 = param1;
    a.ret = 0;
    Fault_ProcessClientContext(&a);
    return a.ret;
}

void Fault_AddClient(FaultClient* client, void* callback, void* param0, void* param1) 
{
#if 0
    OSIntMask mask;
    s32 alreadyExists = false;

    mask = osSetIntMask(1);

    {
        FaultClient* iter = sFaultStructPtr->clients;

        while (iter != NULL) {
            if (iter == client) {
                alreadyExists = true;
                goto end;
            }
            iter = iter->next;
        }
    }

    client->callback = callback;
    client->param1 = param0;
    client->param2 = param1;
    client->next = sFaultStructPtr->clients;
    sFaultStructPtr->clients = client;

end:
    osSetIntMask(mask);
    if (alreadyExists) {
        osSyncPrintf(VT_COL(RED, WHITE) "fault_AddClient: %08x は既にリスト中にある\n" VT_RST, client);
    }
#endif
}

void Fault_RemoveClient(FaultClient* client) 
{
#if 0
    FaultClient* iter;
    FaultClient* lastIter;
    OSIntMask mask;
    u32 listIsEmpty;

    iter = sFaultStructPtr->clients;
    listIsEmpty = 0;
    lastIter = NULL;

    mask = osSetIntMask(1);

    while (iter != NULL) {
        if (iter == client) {
            if (lastIter != NULL) {
                lastIter->next = client->next;
            } else {
                sFaultStructPtr->clients = client;
                if (sFaultStructPtr->clients) {
                    sFaultStructPtr->clients = client->next;
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

void Fault_AddAddrConvClient(FaultAddrConvClient* client, void* callback, void* param) 
{
#if 0
    OSIntMask mask;
    u32 alreadyExists = false;

    mask = osSetIntMask(1);

    {
        FaultAddrConvClient* iter = sFaultStructPtr->addrConvClients;

        while (iter != NULL) {
            if (iter == client) {
                alreadyExists = true;
                goto end;
            }
            iter = iter->next;
        }
    }

    client->callback = callback;
    client->param = param;
    client->next = sFaultStructPtr->addrConvClients;
    sFaultStructPtr->addrConvClients = client;

end:
    osSetIntMask(mask);
    if (alreadyExists) {
        osSyncPrintf(VT_COL(RED, WHITE) "fault_AddressConverterAddClient: %08x は既にリスト中にある\n" VT_RST, client);
    }
#endif
}

void Fault_RemoveAddrConvClient(FaultAddrConvClient* client) 
{
#if 0
    FaultAddrConvClient* iter;
    FaultAddrConvClient* lastIter;
    OSIntMask mask;
    u32 listIsEmpty;

    iter = sFaultStructPtr->addrConvClients;
    listIsEmpty = 0;
    lastIter = NULL;

    mask = osSetIntMask(1);

    while (iter != NULL) {
        if (iter == client) {
            if (lastIter != NULL) {
                lastIter->next = client->next;
            } else {
                sFaultStructPtr->addrConvClients = client;
                if (sFaultStructPtr->addrConvClients != NULL) {
                    sFaultStructPtr->addrConvClients = client->next;
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
        osSyncPrintf(VT_COL(RED, WHITE) "fault_AddressConverterRemoveClient: %08x は既にリスト中にある\n" VT_RST,
                     client);
    }
#endif
}

u32 Fault_ConvertAddress(FaultAddrConvClient* client) 
{
#if 0
    u32 ret;
    FaultAddrConvClient* iter = sFaultStructPtr->addrConvClients;

    while (iter != NULL) {
        if (iter->callback != 0) {
            ret = Fault_ProcessClient(iter->callback, client, iter->param);
            if ((s32)ret == -1) {
                Fault_RemoveAddrConvClient(iter);
            } else if (ret != 0) {
                return ret;
            }
        }
        iter = iter->next;
    }

#endif
    return 0;
}

void Fault_Sleep(u32 duration) {
    Fault_SleepImpl(duration);
}

void Fault_PadCallback(Input* input) {
    //! @bug This function is not called correctly and thus will crash from reading a bad pointer at 0x800C7E4C
    PadMgr_RequestPadData(input, 0);
}

void Fault_UpdatePadImpl() 
{
#if 0
    sFaultStructPtr->padCallback(&sFaultStructPtr->padInput);
#endif
}

u32 Fault_WaitForInputImpl() 
{
#if 0
    Input* input = &sFaultStructPtr->padInput;
    s32 count = 600;
    u32 kDown;

    while (true) {
        Fault_Sleep(0x10);
        Fault_UpdatePadImpl();

        kDown = input->press.button;

        if (kDown == BTN_L) {
            sFaultStructPtr->faultActive = !sFaultStructPtr->faultActive;
        }

        if (sFaultStructPtr->faultActive) {
            if (count-- < 1) {
                return false;
            }
        } else {
            if (kDown == BTN_A || kDown == BTN_DRIGHT) {
                return false;
            }

            if (kDown == BTN_DLEFT) {
                return true;
            }

            if (kDown == BTN_DUP) {
                FaultDrawer_SetOsSyncPrintfEnabled(true);
            }

            if (kDown == BTN_DDOWN) {
                FaultDrawer_SetOsSyncPrintfEnabled(false);
            }
        }
    }
#endif
}

void Fault_WaitForInput() {
#if 0
    sFaultIsWaitingForInput = 1;
    Fault_WaitForInputImpl();
    sFaultIsWaitingForInput = 0;
#endif
}

void Fault_DrawRec(s32 x, s32 y, s32 w, s32 h, u16 color) {
    FaultDrawer_DrawRecImpl(x, y, x + w - 1, y + h - 1, color);
}

void Fault_FillScreenBlack() {
    FaultDrawer_SetForeColor(0xFFFF);
    FaultDrawer_SetBackColor(1);
    FaultDrawer_FillScreen();
    FaultDrawer_SetBackColor(0);
}

void Fault_FillScreenRed() {
    FaultDrawer_SetForeColor(0xFFFF);
    FaultDrawer_SetBackColor(0xF001);
    FaultDrawer_FillScreen();
    FaultDrawer_SetBackColor(0);
}

void Fault_DrawCornerRec(u16 color) {
    Fault_DrawRec(0x16, 0x10, 8, 1, color);
}

void Fault_PrintFReg(s32 idx, f32* value) {
    u32 raw = *(u32*)value;
    s32 v0 = ((raw & 0x7F800000) >> 0x17) - 0x7F;

    if ((v0 >= -0x7E && v0 < 0x80) || raw == 0) {
        FaultDrawer_Printf("F%02d:%14.7e ", idx, *value);
    } else {
        FaultDrawer_Printf("F%02d:  %08x(16) ", idx, raw);
    }
}

void Fault_LogFReg(s32 idx, f32* value) {
    u32 raw = *(u32*)value;
    s32 v0 = ((raw & 0x7F800000) >> 0x17) - 0x7F;

    if ((v0 >= -0x7E && v0 < 0x80) || raw == 0) {
        osSyncPrintf("F%02d:%14.7e ", idx, *value);
    } else {
        osSyncPrintf("F%02d:  %08x(16) ", idx, *(u32*)value);
    }
}

void Fault_PrintFPCR(u32 value) {
    s32 i;
    u32 flag = 0x20000;

    FaultDrawer_Printf("FPCSR:%08xH ", value);
    for (i = 0; i < 6; i++) {
        if (value & flag) {
            FaultDrawer_Printf("(%s)", sExceptionNames[i + 18]);
            break;
        }
        flag >>= 1;
    }
    FaultDrawer_Printf("\n");
}

void Fault_LogFPCR(u32 value) {
    s32 i;
    u32 flag = 0x20000;

    osSyncPrintf("FPCSR:%08xH  ", value);
    for (i = 0; i < 6; i++) {
        if (value & flag) {
            osSyncPrintf("(%s)\n", sExceptionNames[i + 18]);
            break;
        }
        flag >>= 1;
    }
}

void Fault_PrintThreadContext(OSThread* t) {
    __OSThreadContext* ctx;
    s32 causeStrIdx = (s32)((((u32)t->context.cause >> 2) & 0x1F) << 0x10) >> 0x10;

    if (causeStrIdx == 0x17) {
        causeStrIdx = 0x10;
    }
    if (causeStrIdx == 0x1F) {
        causeStrIdx = 0x11;
    }

    FaultDrawer_FillScreen();
    FaultDrawer_SetCharPad(-2, 4);
    FaultDrawer_SetCursor(0x16, 0x14);

    ctx = &t->context;
    FaultDrawer_Printf("THREAD:%d (%d:%s)\n", t->id, causeStrIdx, sExceptionNames[causeStrIdx]);
    FaultDrawer_SetCharPad(-1, 0);

    FaultDrawer_Printf("PC:%08xH SR:%08xH VA:%08xH\n", (u32)ctx->pc, (u32)ctx->sr, (u32)ctx->badvaddr);
    FaultDrawer_Printf("AT:%08xH V0:%08xH V1:%08xH\n", (u32)ctx->at, (u32)ctx->v0, (u32)ctx->v1);
    FaultDrawer_Printf("A0:%08xH A1:%08xH A2:%08xH\n", (u32)ctx->a0, (u32)ctx->a1, (u32)ctx->a2);
    FaultDrawer_Printf("A3:%08xH T0:%08xH T1:%08xH\n", (u32)ctx->a3, (u32)ctx->t0, (u32)ctx->t1);
    FaultDrawer_Printf("T2:%08xH T3:%08xH T4:%08xH\n", (u32)ctx->t2, (u32)ctx->t3, (u32)ctx->t4);
    FaultDrawer_Printf("T5:%08xH T6:%08xH T7:%08xH\n", (u32)ctx->t5, (u32)ctx->t6, (u32)ctx->t7);
    FaultDrawer_Printf("S0:%08xH S1:%08xH S2:%08xH\n", (u32)ctx->s0, (u32)ctx->s1, (u32)ctx->s2);
    FaultDrawer_Printf("S3:%08xH S4:%08xH S5:%08xH\n", (u32)ctx->s3, (u32)ctx->s4, (u32)ctx->s5);
    FaultDrawer_Printf("S6:%08xH S7:%08xH T8:%08xH\n", (u32)ctx->s6, (u32)ctx->s7, (u32)ctx->t8);
    FaultDrawer_Printf("T9:%08xH GP:%08xH SP:%08xH\n", (u32)ctx->t9, (u32)ctx->gp, (u32)ctx->sp);
    //FaultDrawer_Printf("S8:%08xH RA:%08xH LO:%08xH\n\n", (u32)ctx->s8, (u32)ctx->ra, (u32)ctx->lo);

    Fault_PrintFPCR(ctx->fpcsr);
    FaultDrawer_Printf("\n");
    Fault_PrintFReg(0, &ctx->fp0.f.f_even);
    Fault_PrintFReg(2, &ctx->fp2.f.f_even);
    FaultDrawer_Printf("\n");
    Fault_PrintFReg(4, &ctx->fp4.f.f_even);
    Fault_PrintFReg(6, &ctx->fp6.f.f_even);
    FaultDrawer_Printf("\n");
    Fault_PrintFReg(8, &ctx->fp8.f.f_even);
    Fault_PrintFReg(0xA, &ctx->fp10.f.f_even);
    FaultDrawer_Printf("\n");
    Fault_PrintFReg(0xC, &ctx->fp12.f.f_even);
    Fault_PrintFReg(0xE, &ctx->fp14.f.f_even);
    FaultDrawer_Printf("\n");
    Fault_PrintFReg(0x10, &ctx->fp16.f.f_even);
    Fault_PrintFReg(0x12, &ctx->fp18.f.f_even);
    FaultDrawer_Printf("\n");
    Fault_PrintFReg(0x14, &ctx->fp20.f.f_even);
    Fault_PrintFReg(0x16, &ctx->fp22.f.f_even);
    FaultDrawer_Printf("\n");
    Fault_PrintFReg(0x18, &ctx->fp24.f.f_even);
    Fault_PrintFReg(0x1A, &ctx->fp26.f.f_even);
    FaultDrawer_Printf("\n");
    Fault_PrintFReg(0x1C, &ctx->fp28.f.f_even);
    Fault_PrintFReg(0x1E, &ctx->fp30.f.f_even);
    FaultDrawer_Printf("\n");
    FaultDrawer_SetCharPad(0, 0);
}

void Fault_LogThreadContext(OSThread* t) {
    __OSThreadContext* ctx;
    s32 causeStrIdx = (s32)((((u32)t->context.cause >> 2) & 0x1F) << 0x10) >> 0x10;

    if (causeStrIdx == 0x17) {
        causeStrIdx = 0x10;
    }
    if (causeStrIdx == 0x1F) {
        causeStrIdx = 0x11;
    }

    ctx = &t->context;
    osSyncPrintf("\n");
    osSyncPrintf("THREAD ID:%d (%d:%s)\n", t->id, causeStrIdx, sExceptionNames[causeStrIdx]);

    osSyncPrintf("PC:%08xH   SR:%08xH   VA:%08xH\n", (u32)ctx->pc, (u32)ctx->sr, (u32)ctx->badvaddr);
    osSyncPrintf("AT:%08xH   V0:%08xH   V1:%08xH\n", (u32)ctx->at, (u32)ctx->v0, (u32)ctx->v1);
    osSyncPrintf("A0:%08xH   A1:%08xH   A2:%08xH\n", (u32)ctx->a0, (u32)ctx->a1, (u32)ctx->a2);
    osSyncPrintf("A3:%08xH   T0:%08xH   T1:%08xH\n", (u32)ctx->a3, (u32)ctx->t0, (u32)ctx->t1);
    osSyncPrintf("T2:%08xH   T3:%08xH   T4:%08xH\n", (u32)ctx->t2, (u32)ctx->t3, (u32)ctx->t4);
    osSyncPrintf("T5:%08xH   T6:%08xH   T7:%08xH\n", (u32)ctx->t5, (u32)ctx->t6, (u32)ctx->t7);
    osSyncPrintf("S0:%08xH   S1:%08xH   S2:%08xH\n", (u32)ctx->s0, (u32)ctx->s1, (u32)ctx->s2);
    osSyncPrintf("S3:%08xH   S4:%08xH   S5:%08xH\n", (u32)ctx->s3, (u32)ctx->s4, (u32)ctx->s5);
    osSyncPrintf("S6:%08xH   S7:%08xH   T8:%08xH\n", (u32)ctx->s6, (u32)ctx->s7, (u32)ctx->t8);
    osSyncPrintf("T9:%08xH   GP:%08xH   SP:%08xH\n", (u32)ctx->t9, (u32)ctx->gp, (u32)ctx->sp);
    //osSyncPrintf("S8:%08xH   RA:%08xH   LO:%08xH\n", (u32)ctx->s8, (u32)ctx->ra, (u32)ctx->lo);
    osSyncPrintf("\n");
    Fault_LogFPCR(ctx->fpcsr);
    osSyncPrintf("\n");
    Fault_LogFReg(0, &ctx->fp0.f.f_even);
    Fault_LogFReg(2, &ctx->fp2.f.f_even);
    osSyncPrintf("\n");
    Fault_LogFReg(4, &ctx->fp4.f.f_even);
    Fault_LogFReg(6, &ctx->fp6.f.f_even);
    osSyncPrintf("\n");
    Fault_LogFReg(8, &ctx->fp8.f.f_even);
    Fault_LogFReg(0xA, &ctx->fp10.f.f_even);
    osSyncPrintf("\n");
    Fault_LogFReg(0xC, &ctx->fp12.f.f_even);
    Fault_LogFReg(0xE, &ctx->fp14.f.f_even);
    osSyncPrintf("\n");
    Fault_LogFReg(0x10, &ctx->fp16.f.f_even);
    Fault_LogFReg(0x12, &ctx->fp18.f.f_even);
    osSyncPrintf("\n");
    Fault_LogFReg(0x14, &ctx->fp20.f.f_even);
    Fault_LogFReg(0x16, &ctx->fp22.f.f_even);
    osSyncPrintf("\n");
    Fault_LogFReg(0x18, &ctx->fp24.f.f_even);
    Fault_LogFReg(0x1A, &ctx->fp26.f.f_even);
    osSyncPrintf("\n");
    Fault_LogFReg(0x1C, &ctx->fp28.f.f_even);
    Fault_LogFReg(0x1E, &ctx->fp30.f.f_even);
    osSyncPrintf("\n");
}

OSThread* Fault_FindFaultedThread() {
    OSThread* iter = __osGetActiveQueue();

    while (iter->priority != -1) {
        if (iter->priority > 0 && iter->priority < 0x7F && (iter->flags & 3)) {
            return iter;
        }
        iter = iter->tlnext;
    }
    return NULL;
}

void Fault_Wait5Seconds(void) 
{
#if 0
    OSTime start[2]; // to make the function allocate 0x28 bytes of stack instead of 0x20

    start[0] = osGetTime();
    do {
        Fault_Sleep(0x10);
    } while ((osGetTime() - start[0]) < OS_USEC_TO_CYCLES(5000000) + 1);

    sFaultStructPtr->faultActive = true;
#endif
}

void Fault_WaitForButtonCombo() 
{
#if 0
    Input* input = &sFaultStructPtr->padInput;
    s32 state;
    u32 s1;
    u32 s2;
    u32 kDown;
    u32 kCur;

    osSyncPrintf(
        VT_FGCOL(WHITE) "KeyWaitB (ＬＲＺ " VT_FGCOL(WHITE) "上" VT_FGCOL(YELLOW) "下 " VT_FGCOL(YELLOW) "上" VT_FGCOL(WHITE) "下 " VT_FGCOL(WHITE) "左" VT_FGCOL(
            YELLOW) "左 " VT_FGCOL(YELLOW) "右" VT_FGCOL(WHITE) "右 " VT_FGCOL(GREEN) "Ｂ" VT_FGCOL(BLUE) "Ａ" VT_FGCOL(RED) "START" VT_FGCOL(WHITE) ")" VT_RST
                                                                                                                                                     "\n");
    osSyncPrintf(VT_FGCOL(WHITE) "KeyWaitB'(ＬＲ左" VT_FGCOL(YELLOW) "右 +" VT_FGCOL(RED) "START" VT_FGCOL(
        WHITE) ")" VT_RST "\n");

    FaultDrawer_SetForeColor(0xFFFF);
    FaultDrawer_SetBackColor(1);

    state = 0;
    s1 = 0;
    s2 = 1;

    while (state != 11) {
        Fault_Sleep(0x10);
        Fault_UpdatePadImpl();

        kDown = input->press.button;
        kCur = input->cur.button;

        if ((kCur == 0) && (s1 == s2)) {
            s1 = 0;
        } else if (kDown != 0) {
            if (s1 == s2) {
                state = 0;
            }

            switch (state) {
                case 0:
                    if (kCur == (BTN_Z | BTN_L | BTN_R) && kDown == BTN_Z) {
                        state = s2;
                        s1 = s2;
                    }
                    break;
                case 1:
                    if (kDown == BTN_DUP) {
                        state = 2;
                    } else {
                        state = 0;
                    }
                    break;
                case 2:
                    if (kDown == BTN_CDOWN) {
                        state = 3;
                        s1 = s2;
                    } else {
                        state = 0;
                    }
                    break;
                case 3:
                    if (kDown == BTN_CUP) {
                        state = 4;
                    } else {
                        state = 0;
                    }
                    break;
                case 4:
                    if (kDown == BTN_DDOWN) {
                        state = 5;
                        s1 = s2;
                    } else {
                        state = 0;
                    }
                    break;
                case 5:
                    if (kDown == BTN_DLEFT) {
                        state = 6;
                    } else {
                        state = 0;
                    }
                    break;
                case 6:
                    if (kDown == BTN_CLEFT) {
                        state = 7;
                        s1 = s2;
                    } else {
                        state = 0;
                    }
                    break;
                case 7:
                    if (kDown == BTN_CRIGHT) {
                        state = 8;
                    } else {
                        state = 0;
                    }
                    break;
                case 8:
                    if (kDown == BTN_DRIGHT) {
                        state = 9;
                        s1 = s2;
                    } else {
                        state = 0;
                    }
                    break;
                case 9:
                    if (kDown == (BTN_A | BTN_B)) {
                        state = 10;
                    } else if (kDown == BTN_A) {
                        state = 0x5B;
                    } else if (kDown == BTN_B) {
                        state = 0x5C;
                    } else {
                        state = 0;
                    }
                    break;
                case 0x5B:
                    if (kDown == BTN_B) {
                        state = 10;
                    } else {
                        state = 0;
                    }
                    break;
                case 0x5C:
                    if (kDown == BTN_A) {
                        state = 10;
                    } else {
                        state = 0;
                    }
                    break;
                case 10:
                    if (kDown == BTN_START) {
                        state = 11;
                    } else {
                        state = 0;
                    }
                    break;
            }
        }

        osWritebackDCacheAll();
    }
#endif
}

void Fault_DrawMemDumpPage(const char* title, u32* addr, u32 param_3) {
    u32* alignedAddr;
    u32* writeAddr;
    s32 y;
    s32 x;

    alignedAddr = addr;

    if (alignedAddr < (u32*)0x80000000) {
        alignedAddr = (u32*)0x80000000;
    }
    if (alignedAddr > (u32*)0x807FFF00) {
        alignedAddr = (u32*)0x807FFF00;
    }

    alignedAddr = (u32*)((u32)alignedAddr & ~3);
    writeAddr = alignedAddr;
    Fault_FillScreenBlack();
    FaultDrawer_SetCharPad(-2, 0);

    FaultDrawer_DrawText(0x24, 0x12, "%s %08x", title != NULL ? title : "PrintDump", alignedAddr);
    if (alignedAddr >= (u32*)0x80000000 && alignedAddr < (u32*)0xC0000000) {
        for (y = 0x1C; y != 0xE2; y += 9) {
            FaultDrawer_DrawText(0x18, y, "%06x", writeAddr);
            for (x = 0x52; x != 0x122; x += 0x34) {
                FaultDrawer_DrawText(x, y, "%08x", *writeAddr++);
            }
        }
    }

    FaultDrawer_SetCharPad(0, 0);
}

void Fault_DrawMemDump(u32 pc, u32 sp, u32 unk0, u32 unk1) 
{
#if 0
    Input* input = &sFaultStructPtr->padInput;
    u32 addr = pc;
    s32 count;
    u32 off;

    do {
        count = 0;
        if (addr < 0x80000000) {
            addr = 0x80000000;
        }
        if (addr > 0x807FFF00) {
            addr = 0x807FFF00;
        }

        addr &= ~0xF;
        Fault_DrawMemDumpPage("Dump", (u32*)addr, 0);
        count = 600;

        while (sFaultStructPtr->faultActive) {
            if (count == 0) {
                return;
            }

            count--;
            Fault_Sleep(0x10);
            Fault_UpdatePadImpl();
            if (CHECK_BTN_ALL(input->press.button, BTN_L)) {
                sFaultStructPtr->faultActive = false;
            }
        }

        do {
            Fault_Sleep(0x10);
            Fault_UpdatePadImpl();
        } while (input->press.button == 0);

        if (CHECK_BTN_ALL(input->press.button, BTN_START) || CHECK_BTN_ALL(input->cur.button, BTN_A)) {
            return;
        }

        off = 0x10;
        if (CHECK_BTN_ALL(input->cur.button, BTN_Z)) {
            off = 0x100;
        }

        if (CHECK_BTN_ALL(input->cur.button, BTN_B)) {
            off <<= 8;
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
            addr = unk0;
        }
        if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
            addr = unk1;
        }
    } while (!CHECK_BTN_ALL(input->press.button, BTN_L));

    sFaultStructPtr->faultActive = true;
#endif
}

void Fault_WalkStack(u32* spPtr, u32* pcPtr, u32* raPtr) {
    u32 sp = *spPtr;
    u32 pc = *pcPtr;
    u32 ra = *raPtr;
    s32 count = 0x10000;
    u32 lastOpc;
    u32 opc;
    u16 opcHi;
    s16 opcLo;
    u32 imm;

    if (sp & 3 || sp < 0x80000000 || sp >= 0xA0000000 || ra & 3 || ra < 0x80000000 || ra >= 0xA0000000) {
        *spPtr = 0;
        *pcPtr = 0;
        *raPtr = 0;
        return;
    }

    if (pc & 3 || pc < 0x80000000 || pc >= 0xA0000000) {
        *pcPtr = ra;
        return;
    }

    lastOpc = 0;
    while (true) {
        opc = HW_REG(pc, u32);
        opcHi = opc >> 16;
        opcLo = opc & 0xFFFF;
        imm = opcLo;
        if (opcHi == 0x8FBF) {
            ra = HW_REG(sp + imm, u32);
        } else if (opcHi == 0x27BD) {
            sp += imm;
        } else if (opc == 0x42000018) {
            sp = 0;
            pc = 0;
            ra = 0;
            goto end;
        }
        if (lastOpc == 0x3E00008) {
            pc = ra;
            goto end;
        } else if (lastOpc >> 26 == 2) {
            pc = pc >> 28 << 28 | lastOpc << 6 >> 4;
            goto end;
        }
        lastOpc = opc;
        pc += 4;
        if (count == 0) {
            break;
        }
        count--;
    }
    sp = 0;
    pc = 0;
    ra = 0;

end:
    *spPtr = sp;
    *pcPtr = pc;
    *raPtr = ra;
}

void Fault_DrawStackTrace(OSThread* thread, s32 x, s32 y, s32 height) {
    s32 line;
    u32 sp = thread->context.sp;
    u32 ra = thread->context.ra;
    u32 pc = thread->context.pc;
    u32 addr;

    FaultDrawer_DrawText(x, y, "SP       PC       (VPC)");
    for (line = 1; line < height && (ra != 0 || sp != 0) && pc != (u32)__osCleanupThread; line++) {
        FaultDrawer_DrawText(x, y + line * 8, "%08x %08x", sp, pc);
        addr = Fault_ConvertAddress(pc);
        if (addr != 0) {
            FaultDrawer_Printf(" -> %08x", addr);
        }
        Fault_WalkStack(&sp, &pc, &ra);
    }
}

void Fault_LogStackTrace(OSThread* thread, s32 height) {
    s32 line;
    u32 sp = thread->context.sp;
    u32 ra = thread->context.ra;
    u32 pc = thread->context.pc;
    u32 addr;
    s32 pad;

    osSyncPrintf("STACK TRACE\nSP       PC       (VPC)\n");
    for (line = 1; line < height && (ra != 0 || sp != 0) && pc != (u32)__osCleanupThread; line++) {
        osSyncPrintf("%08x %08x", sp, pc);
        addr = Fault_ConvertAddress(pc);
        if (addr != 0) {
            osSyncPrintf(" -> %08x", addr);
        }
        osSyncPrintf("\n");
        Fault_WalkStack(&sp, &pc, &ra);
    }
}

void Fault_ResumeThread(OSThread* t) {
    t->context.cause = 0;
    t->context.fpcsr = 0;
    t->context.pc += 4;
    *(u32*)t->context.pc = 0xD;
    osWritebackDCache(t->context.pc, 4);
    osInvalICache(t->context.pc, 4);
    osStartThread(t);
}

void Fault_CommitFB() {
    u16* fb;

#if 0
    osViSetYScale(1.0f);
    osViSetMode(&osViModeNtscLan1);
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF | OS_VI_DITHER_FILTER_ON);
    osViBlack(false);

    if (sFaultStructPtr->fb) {
        fb = sFaultStructPtr->fb;
    } else {
        fb = (u16*)osViGetNextFramebuffer();
        if ((u32)fb == 0x80000000) {
            fb = (u16*)((osMemSize | 0x80000000) - sizeof(u16[SCREEN_HEIGHT][SCREEN_WIDTH]));
        }
    }

    osViSwapBuffer(fb);
    FaultDrawer_SetDrawerFB(fb, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif
}

void Fault_ProcessClients(void) 
{
#if 0
    FaultClient* iter = sFaultStructPtr->clients;
    s32 idx = 0;

    while (iter != NULL) {
        if (iter->callback != 0) {
            Fault_FillScreenBlack();
            FaultDrawer_SetCharPad(-2, 0);
            FaultDrawer_Printf("\x1a"
                               "8CallBack (%d) %08x %08x %08x\n"
                               "\x1a"
                               "7",
                               idx++, iter, iter->param1, iter->param2);
            FaultDrawer_SetCharPad(0, 0);
            Fault_ProcessClient(iter->callback, iter->param1, iter->param2);
            Fault_WaitForInput();
            Fault_CommitFB();
        }
        iter = iter->next;
    }
#endif
}

void Fault_UpdatePad() {
    Fault_UpdatePadImpl();
}

void Fault_ThreadEntry(void* arg) {
    OSMesg msg;
    OSThread* faultedThread;
    s32 pad;

#if 0
    osSetEventMesg(OS_EVENT_CPU_BREAK, &sFaultStructPtr->queue, 1);
    osSetEventMesg(OS_EVENT_FAULT, &sFaultStructPtr->queue, 2);

    while (true) {
        do {
            osRecvMesg(&sFaultStructPtr->queue, &msg, OS_MESG_BLOCK);

            if (msg == (OSMesg)1) {
                sFaultStructPtr->msgId = 1;
                osSyncPrintf("フォルトマネージャ:OS_EVENT_CPU_BREAKを受信しました\n");
            } else if (1 && msg == (OSMesg)2) {
                sFaultStructPtr->msgId = 2;
                osSyncPrintf("フォルトマネージャ:OS_EVENT_FAULTを受信しました\n");
            } else if (msg == (OSMesg)3) {
                Fault_UpdatePad();
                faultedThread = NULL;
                continue;
            } else {
                sFaultStructPtr->msgId = 3;
                osSyncPrintf("フォルトマネージャ:不明なメッセージを受信しました\n");
            }

            faultedThread = __osGetCurrFaultedThread();
            osSyncPrintf("__osGetCurrFaultedThread()=%08x\n", faultedThread);

            if (faultedThread == NULL) {
                faultedThread = Fault_FindFaultedThread();
                osSyncPrintf("FindFaultedThread()=%08x\n", faultedThread);
            }
        } while (faultedThread == NULL);

        __osSetFpcCsr(__osGetFpcCsr() & -0xF81);
        sFaultStructPtr->faultedThread = faultedThread;

        while (!sFaultStructPtr->faultHandlerEnabled) {
            Fault_Sleep(1000);
        }

        Fault_Sleep(500);
        Fault_CommitFB();

        if (sFaultStructPtr->faultActive) {
            Fault_Wait5Seconds();
        } else {
            Fault_DrawCornerRec(0xF801);
            Fault_WaitForButtonCombo();
        }

        sFaultStructPtr->faultActive = true;
        FaultDrawer_SetForeColor(0xFFFF);
        FaultDrawer_SetBackColor(0);

        do {
            Fault_PrintThreadContext(faultedThread);
            Fault_LogThreadContext(faultedThread);
            Fault_WaitForInput();
            Fault_FillScreenBlack();
            FaultDrawer_DrawText(0x78, 0x10, "STACK TRACE");
            Fault_DrawStackTrace(faultedThread, 0x24, 0x18, 0x16);
            Fault_LogStackTrace(faultedThread, 0x32);
            Fault_WaitForInput();
            Fault_ProcessClients();
            Fault_DrawMemDump(faultedThread->context.pc - 0x100, (u32)faultedThread->context.sp, 0, 0);
            Fault_FillScreenRed();
            FaultDrawer_DrawText(0x40, 0x50, "    CONGRATURATIONS!    ");
            FaultDrawer_DrawText(0x40, 0x5A, "All Pages are displayed.");
            FaultDrawer_DrawText(0x40, 0x64, "       THANK YOU!       ");
            FaultDrawer_DrawText(0x40, 0x6E, " You are great debugger!");
            Fault_WaitForInput();
        } while (!sFaultStructPtr->exitDebugger);

        while (!sFaultStructPtr->exitDebugger) {}

        Fault_ResumeThread(faultedThread);
    }
#endif
}

void Fault_SetFB(void* fb, u16 w, u16 h) 
{
#if 0
    sFaultStructPtr->fb = fb;
    FaultDrawer_SetDrawerFB(fb, w, h);
#endif
}

void Fault_Init(void) 
{
#if 0
    sFaultStructPtr = &gFaultStruct;
    bzero(sFaultStructPtr, sizeof(FaultThreadStruct));
    FaultDrawer_SetDefault();
    FaultDrawer_SetInputCallback(Fault_WaitForInput);
    sFaultStructPtr->exitDebugger = false;
    sFaultStructPtr->msgId = 0;
    sFaultStructPtr->faultHandlerEnabled = false;
    sFaultStructPtr->faultedThread = NULL;
    sFaultStructPtr->padCallback = Fault_PadCallback;
    sFaultStructPtr->clients = NULL;
    sFaultStructPtr->faultActive = false;
    gFaultStruct.faultHandlerEnabled = true;
    osCreateMesgQueue(&sFaultStructPtr->queue, &sFaultStructPtr->msg, 1);
    StackCheck_Init(&sFaultThreadInfo, &sFaultStack, sFaultStack + sizeof(sFaultStack), 0, 0x100, "fault");
    osCreateThread(&sFaultStructPtr->thread, 2, Fault_ThreadEntry, 0, sFaultStack + sizeof(sFaultStack),
                   OS_PRIORITY_APPMAX);
    osStartThread(&sFaultStructPtr->thread);
#endif
}

void Fault_HangupFaultClient(const char* arg0, const char* arg1) 
{
#if 0
    osSyncPrintf("HungUp on Thread %d\n", osGetThreadId(0));
    osSyncPrintf("%s\n", arg0 != NULL ? arg0 : "(NULL)");
    osSyncPrintf("%s\n", arg1 != NULL ? arg1 : "(NULL)");
    FaultDrawer_Printf("HungUp on Thread %d\n", osGetThreadId(0));
    FaultDrawer_Printf("%s\n", arg0 != NULL ? arg0 : "(NULL)");
    FaultDrawer_Printf("%s\n", arg1 != NULL ? arg1 : "(NULL)");
#endif
}

void Fault_AddHungupAndCrashImpl(const char* arg0, const char* arg1) {
    FaultClient client;
    s32 pad;

    // TODO: DO A PROPER EXCEPTION HANDLER HERE

    Fault_AddClient(&client, Fault_HangupFaultClient, (void*)arg0, (void*)arg1);
    *(u32*)0x11111111 = 0; // trigger an exception
}

void Fault_AddHungupAndCrash(const char* filename, u32 line) {
    char msg[256];

    sprintf(msg, "HungUp %s:%d", filename, line);
    Fault_AddHungupAndCrashImpl(msg, NULL);
}
