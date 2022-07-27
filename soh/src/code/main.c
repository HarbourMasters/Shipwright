#include "global.h"
#include "vt.h"
#include <soh/Enhancements/bootcommands.h>
#include "soh/OTRGlobals.h"

#ifdef _MSC_VER
#define NOGDI
#include <Windows.h>
#include <DbgHelp.h>
#include <inttypes.h>
#include <stdio.h>
#include <winnt.h>
#pragma comment(lib, "Dbghelp.lib")
#endif
#pragma optimize("", off)
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



__declspec(noinline) void Main1(int argc, char** argv) {
    GameConsole_Init();
    InitOTR();
    BootCommands_Init();

    BootCommands_ParseBootArgs(argc - 1, (char**)&argv[1]);
    Main(0);
}

 __declspec(noinline) void main(int argc, char** argv) {
    __try {
        Main1(argc, argv);
    } __except (seh_filter(GetExceptionInformation())) { puts("UH OH"); }
}

__declspec(noinline) void Main(void* arg) {
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
    sysHeap = gSystemHeap;
    fb = SysCfb_GetFbPtr(0);
    gSystemHeapSize = 1024 * 1024 * 4;
    // "System heap initalization"
    osSyncPrintf("システムヒープ初期化 %08x-%08x %08x\n", sysHeap, fb, gSystemHeapSize);
    SystemHeap_Init(sysHeap, gSystemHeapSize); // initializes the system heap
    if (osMemSize >= 0x800000) {
        debugHeap = SysCfb_GetFbEnd();
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



void printStack(CONTEXT* ctx) {
    BOOL result;
    HANDLE process;
    HANDLE thread;
    HMODULE hModule;
    ULONG frame;
    DWORD64 displacement;
    DWORD disp;

#if defined(_M_AMD64)
    STACKFRAME64 stack;
    memset(&stack, 0, sizeof(STACKFRAME64));
#else
    STACKFRAME stack;
    memset(&stack, 0, sizeof(STACKFRAME));
#endif

    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME + sizeof(TCHAR)];
    // char name[256];
    char module[512];

    PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;

    CONTEXT ctx2;
    memcpy(&ctx2, ctx, sizeof(CONTEXT));

    process = GetCurrentProcess();
    thread = GetCurrentThread();
    SymInitialize(process, NULL, true);
    LUS_PRINT(5, "Register dump");

    const DWORD machineType =
#if defined(_M_AMD64)
        IMAGE_FILE_MACHINE_AMD64;
#else
        IMAGE_FILE_MACHINE_I386;
#endif
    displacement = 0;
#if !defined(_M_AMD64)
    stack.AddrPC.Offset = (*ctx).Eip;
    stack.AddrPC.Mode = AddrModeFlat;
    stack.AddrStack.Offset = (*ctx).Esp;
    stack.AddrStack.Mode = AddrModeFlat;
    stack.AddrFrame.Offset = (*ctx).Ebp;
    stack.AddrFrame.Mode = AddrModeFlat;
    LUS_PRINT(5, "Edi: 0x% " PRIX32, ctx->Edi);
    LUS_PRINT(5, "Esi: 0x% " PRIX32, ctx->Esi);
    LUS_PRINT(5, "Ebx: 0x% " PRIX32, ctx->Ebx);
    LUS_PRINT(5, "Ecx: 0x% " PRIX32, ctx->Ecx);
    LUS_PRINT(5, "Eax: 0x% " PRIX32, ctx->Eax);
    LUS_PRINT(5, "Ebp: 0x% " PRIX32, ctx->Ebp);
    LUS_PRINT(5, "Esp: 0x% " PRIX32, ctx->Esp);
    LUS_PRINT(5, "EFlags: 0x% " PRIX32, ctx->EFlags);
    LUS_PRINT(5, "Eip: 0x% " PRIX32, ctx->Eip);
#else

    LUS_PRINT(5, "Rax: 0x% " PRIX64 , ctx->Rax);
    LUS_PRINT(5, "Rcx: 0x% " PRIX64 , ctx->Rcx);
    LUS_PRINT(5, "Rdx: 0x% " PRIX64 , ctx->Rdx);
    LUS_PRINT(5, "Rbx: 0x% " PRIX64 , ctx->Rbx);
    LUS_PRINT(5, "Rsp: 0x% " PRIX64 , ctx->Rsp);
    LUS_PRINT(5, "Rbp: 0x% " PRIX64 , ctx->Rbp);
    LUS_PRINT(5, "Rsi: 0x% " PRIX64 , ctx->Rsi);
    LUS_PRINT(5, "Rdi: 0x% " PRIX64 , ctx->Rdi);
    LUS_PRINT(5, "R9: 0x% " PRIX64 , ctx->R9);
    LUS_PRINT(5, "R10: 0x% " PRIX64 , ctx->R10);
    LUS_PRINT(5, "R11: 0x% " PRIX64 , ctx->R11);
    LUS_PRINT(5, "R12: 0x% " PRIX64 , ctx->R12);
    LUS_PRINT(5, "R13: 0x% " PRIX64 , ctx->R13);
    LUS_PRINT(5, "R14: 0x% " PRIX64 , ctx->R14);
    LUS_PRINT(5, "R15: 0x% " PRIX64 , ctx->R15);
    LUS_PRINT(5, "PC: 0x%" PRIX64 , ctx->Rip);
    LUS_PRINT(5, "EFlags: 0x%" PRIX32 , ctx->EFlags);
#endif

    for (frame = 0;; frame++) {
        result = StackWalk(machineType, process, thread, &stack, &ctx2, NULL, SymFunctionTableAccess,
                           SymGetModuleBase, NULL);
        if (!result) {
            break;
        }
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;
        SymFromAddr(process, (ULONG64)stack.AddrPC.Offset, &displacement, symbol);
#if defined(_M_AMD64)
        IMAGEHLP_LINE64 line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
#else
        IMAGEHLP_LINE line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE);
#endif
        if (SymGetLineFromAddr(process, stack.AddrPC.Offset, &disp, &line)) {
            LUS_PRINT(5, "%s in %s: line: %lu: ", symbol->Name, line.FileName, line.LineNumber);
        }
        
        else {
            LUS_PRINT(5, "at % s, addr 0x%0X", symbol->Name, symbol->Address);
            hModule = NULL;
            GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                              (LPCTSTR)(stack.AddrPC.Offset), &hModule);

            if (hModule != NULL) {
                GetModuleFileNameA(hModule, module, 512 - 1);
            }
            LUS_PRINT(5, "In %s\n", module);
        }
    }
    puts("AT SHUTDOWN");
    lusprintShutdown();

}

int seh_filter(EXCEPTION_POINTERS* ex) {
    puts("EXCEPTION");
    LUS_PRINT(5, "EXCEPTION 0x%x occured\n", ex->ExceptionRecord->ExceptionCode);
    printStack(ex->ContextRecord);
    return EXCEPTION_EXECUTE_HANDLER;
}
#pragma optimize("", on)