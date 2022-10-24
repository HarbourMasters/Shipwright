#include "global.h"
#include "vt.h"
#include <soh/Enhancements/bootcommands.h>
#include "soh/OTRGlobals.h"

#include <libultraship/CrashHandler.h>
#include "soh/CrashHandlerExp.h"


s32 gScreenWidth = SCREEN_WIDTH;
s32 gScreenHeight = SCREEN_HEIGHT;
size_t gSystemHeapSize = 0;

PreNmiBuff* gAppNmiBufferPtr;
SchedContext gSchedContext;
PadMgr gPadMgr;
IrqMgr gIrqMgr;
uintptr_t gSegments[NUM_SEGMENTS];
OSThread sGraphThread;
u8 sGraphStack[0x1800];
u8 sSchedStack[0x600];
u8 sAudioStack[0x800];
u8 sPadMgrStack[0x500];
u8 sIrqMgrStack[0x500];
StackEntry sGraphStackInfo;
StackEntry sSchedStackInfo;
StackEntry sAudioStackInfo;
StackEntry sPadMgrStackInfo;
StackEntry sIrqMgrStackInfo;
AudioMgr gAudioMgr;
OSMesgQueue sSiIntMsgQ;
OSMesg sSiIntMsgBuf[1];

void Main_LogSystemHeap(void) {
    osSyncPrintf(VT_FGCOL(GREEN));
    // "System heap size% 08x (% dKB) Start address% 08x"
    osSyncPrintf("システムヒープサイズ %08x(%dKB) 開始アドレス %08x\n", gSystemHeapSize, gSystemHeapSize / 1024,
                 gSystemHeap);
    osSyncPrintf(VT_RST);
}

#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) 
#else
int main(int argc, char** argv)
#endif
{
    CrashHandler_Init(CrashHandler_PrintSohData);
    
    GameConsole_Init();
    InitOTR();
    BootCommands_Init();

    Main(0);
    DeinitOTR();
    return 0;
}

void Main(void* arg) {
    IrqMgrClient irqClient;
    OSMesgQueue irqMgrMsgQ;
    OSMesg irqMgrMsgBuf[60];
    uintptr_t sysHeap;
    uintptr_t fb;
    void* debugHeap;
    size_t debugHeapSize;
    s16* msg;

    osSyncPrintf("mainproc 実行開始\n"); // "Start running"
    gScreenWidth = SCREEN_WIDTH;
    gScreenHeight = SCREEN_HEIGHT;
    gAppNmiBufferPtr = (PreNmiBuff*)osAppNmiBuffer;
    PreNmiBuff_Init(gAppNmiBufferPtr);
    Fault_Init();
    SysCfb_Init(0);
    Heaps_Alloc();
    sysHeap = (uintptr_t)gSystemHeap;
    fb = SysCfb_GetFbPtr(0);
    gSystemHeapSize = 1024 * 1024 * 4;
    // "System heap initalization"
    osSyncPrintf("システムヒープ初期化 %08x-%08x %08x\n", sysHeap, fb, gSystemHeapSize);
    SystemHeap_Init((void*)sysHeap, gSystemHeapSize); // initializes the system heap
    if (osMemSize >= 0x800000) {
        debugHeap = (void*)SysCfb_GetFbEnd();
        debugHeapSize = (0x80600000 - (uintptr_t)debugHeap);
    } else {
        debugHeapSize = 0x400;
        debugHeap = SYSTEM_ARENA_MALLOC_DEBUG(debugHeapSize);
    }

    debugHeapSize = 1024 * 64;

    osSyncPrintf("debug_InitArena(%08x, %08x)\n", debugHeap, debugHeapSize);
    DebugArena_Init(debugHeap, debugHeapSize);
    func_800636C0();

    R_ENABLE_ARENA_DBG = 0;

    osCreateMesgQueue(&sSiIntMsgQ, sSiIntMsgBuf, 1);
    osSetEventMesg(5, &sSiIntMsgQ, OS_MESG_PTR(NULL));

    Main_LogSystemHeap();

    osCreateMesgQueue(&irqMgrMsgQ, irqMgrMsgBuf, 0x3C);
    StackCheck_Init(&sIrqMgrStackInfo, sIrqMgrStack, sIrqMgrStack + sizeof(sIrqMgrStack), 0, 0x100, "irqmgr");
    IrqMgr_Init(&gIrqMgr, &sGraphStackInfo, Z_PRIORITY_IRQMGR, 1);

    osSyncPrintf("タスクスケジューラの初期化\n"); // "Initialize the task scheduler"
    StackCheck_Init(&sSchedStackInfo, sSchedStack, sSchedStack + sizeof(sSchedStack), 0, 0x100, "sched");
    Sched_Init(&gSchedContext, &sAudioStack, Z_PRIORITY_SCHED, D_80013960, 1, &gIrqMgr);

    IrqMgr_AddClient(&gIrqMgr, &irqClient, &irqMgrMsgQ);

    StackCheck_Init(&sAudioStackInfo, sAudioStack, sAudioStack + sizeof(sAudioStack), 0, 0x100, "audio");
    AudioMgr_Init(&gAudioMgr, sAudioStack + sizeof(sAudioStack), Z_PRIORITY_AUDIOMGR, 0xA, &gSchedContext, &gIrqMgr);

    StackCheck_Init(&sPadMgrStackInfo, sPadMgrStack, sPadMgrStack + sizeof(sPadMgrStack), 0, 0x100, "padmgr");
    PadMgr_Init(&gPadMgr, &sSiIntMsgQ, &gIrqMgr, 7, Z_PRIORITY_PADMGR, &sIrqMgrStack);

    AudioMgr_Unlock(&gAudioMgr);

    StackCheck_Init(&sGraphStackInfo, sGraphStack, sGraphStack + sizeof(sGraphStack), 0, 0x100, "graph");
    osCreateThread(&sGraphThread, 4, Graph_ThreadEntry, arg, sGraphStack + sizeof(sGraphStack), Z_PRIORITY_GRAPH);
    osStartThread(&sGraphThread);
    osSetThreadPri(0, Z_PRIORITY_SCHED);

    Graph_ThreadEntry(0);

    while (true) {
        msg = NULL;
        osRecvMesg(&irqMgrMsgQ, (OSMesg*)&msg, OS_MESG_BLOCK);
        if (msg == NULL) {
            break;
        }
        if (*msg == OS_SC_PRE_NMI_MSG) {
            osSyncPrintf("main.c: リセットされたみたいだよ\n"); // "Looks like it's been reset"
            PreNmiBuff_SetReset(gAppNmiBufferPtr);
        }
    }

    osSyncPrintf("mainproc 後始末\n"); // "Cleanup"
    osDestroyThread(&sGraphThread);
    func_800FBFD8();
    osSyncPrintf("mainproc 実行終了\n"); // "End of execution"

    Heaps_Free();
}
