#include "global.h"
#include "vt.h"
#include "regs.h"

#include <string.h>

#include "soh/Enhancements/gameconsole.h"
#include "soh/Enhancements/debugger/debugger.h"

#define GFXPOOL_HEAD_MAGIC 0x1234
#define GFXPOOL_TAIL_MAGIC 0x5678

OSTime sGraphUpdateTime;
OSTime sGraphSetTaskTime;
FaultClient sGraphFaultClient;
CfbInfo sGraphCfbInfos[3];
FaultClient sGraphUcodeFaultClient;

// clang-format off
UCodeInfo D_8012D230[3] = {
    //{ UCODE_F3DZEX, D_80155F50 },
    { UCODE_UNK, NULL },
    //{ UCODE_S2DEX, D_80113070 },
};

UCodeInfo D_8012D248[3] = {
    //{ UCODE_F3DZEX, D_80155F50 },
    { UCODE_UNK, NULL },
    //{ UCODE_S2DEX, D_80113070 },
};
// clang-format on

void Graph_FaultClient() {
    void* nextFb = osViGetNextFramebuffer();
    void* newFb = ((uintptr_t)SysCfb_GetFbPtr(0) != (uintptr_t)nextFb) ? SysCfb_GetFbPtr(0) : SysCfb_GetFbPtr(1);

    osViSwapBuffer(newFb);
    Fault_WaitForInput();
    osViSwapBuffer(nextFb);
}

void Graph_DisassembleUCode(Gfx* workBuf) {
    #if 0
    UCodeDisas disassembler;

    if (HREG(80) == 7 && HREG(81) != 0) {
        UCodeDisas_Init(&disassembler);
        disassembler.enableLog = HREG(83);
        UCodeDisas_RegisterUCode(&disassembler, ARRAY_COUNT(D_8012D230), D_8012D230);
        //UCodeDisas_SetCurUCode(&disassembler, D_80155F50);
        UCodeDisas_Disassemble(&disassembler, workBuf);
        HREG(93) = disassembler.dlCnt;
        HREG(84) = disassembler.tri2Cnt * 2 + disassembler.tri1Cnt + (disassembler.quadCnt * 2) + disassembler.lineCnt;
        HREG(85) = disassembler.vtxCnt;
        HREG(86) = disassembler.spvtxCnt;
        HREG(87) = disassembler.tri1Cnt;
        HREG(88) = disassembler.tri2Cnt;
        HREG(89) = disassembler.quadCnt;
        HREG(90) = disassembler.lineCnt;
        HREG(91) = disassembler.syncErr;
        HREG(92) = disassembler.loaducodeCnt;
        if (HREG(82) == 1 || HREG(82) == 2) {
            osSyncPrintf("vtx_cnt=%d\n", disassembler.vtxCnt);
            osSyncPrintf("spvtx_cnt=%d\n", disassembler.spvtxCnt);
            osSyncPrintf("tri1_cnt=%d\n", disassembler.tri1Cnt);
            osSyncPrintf("tri2_cnt=%d\n", disassembler.tri2Cnt);
            osSyncPrintf("quad_cnt=%d\n", disassembler.quadCnt);
            osSyncPrintf("line_cnt=%d\n", disassembler.lineCnt);
            osSyncPrintf("sync_err=%d\n", disassembler.syncErr);
            osSyncPrintf("loaducode_cnt=%d\n", disassembler.loaducodeCnt);
            osSyncPrintf("dl_depth=%d\n", disassembler.dlDepth);
            osSyncPrintf("dl_cnt=%d\n", disassembler.dlCnt);
        }
        UCodeDisas_Destroy(&disassembler);
    }
    #endif
}

void Graph_UCodeFaultClient(Gfx* workBuf) {
    #if 0
    UCodeDisas disassembler;

    UCodeDisas_Init(&disassembler);
    disassembler.enableLog = true;
    UCodeDisas_RegisterUCode(&disassembler, ARRAY_COUNT(D_8012D248), D_8012D248);
    //UCodeDisas_SetCurUCode(&disassembler, D_80155F50);
    UCodeDisas_Disassemble(&disassembler, workBuf);
    UCodeDisas_Destroy(&disassembler);
    #endif
}

void Graph_InitTHGA(GraphicsContext* gfxCtx) {
    GfxPool* pool = &gGfxPools[gfxCtx->gfxPoolIdx & 1];

    pool->headMagic = GFXPOOL_HEAD_MAGIC;
    pool->tailMagic = GFXPOOL_TAIL_MAGIC;
    THGA_Ct(&gfxCtx->polyOpa, pool->polyOpaBuffer, sizeof(pool->polyOpaBuffer));
    THGA_Ct(&gfxCtx->polyXlu, pool->polyXluBuffer, sizeof(pool->polyXluBuffer));
    THGA_Ct(&gfxCtx->worldOverlay, pool->worldOverlayBuffer, sizeof(pool->worldOverlayBuffer));
    THGA_Ct(&gfxCtx->polyKal, pool->polyKalBuffer, sizeof(pool->polyKalBuffer));
    THGA_Ct(&gfxCtx->overlay, pool->overlayBuffer, sizeof(pool->overlayBuffer));
    THGA_Ct(&gfxCtx->work, pool->workBuffer, sizeof(pool->workBuffer));

    gfxCtx->polyOpaBuffer = pool->polyOpaBuffer;
    gfxCtx->polyXluBuffer = pool->polyXluBuffer;
    gfxCtx->worldOverlayBuffer = pool->worldOverlayBuffer;
    gfxCtx->polyKalBuffer = pool->polyKalBuffer;
    gfxCtx->overlayBuffer = pool->overlayBuffer;
    gfxCtx->workBuffer = pool->workBuffer;

    gfxCtx->curFrameBuffer = (u16*)SysCfb_GetFbPtr(gfxCtx->fbIdx % 2);
    gfxCtx->unk_014 = 0;
}

GameStateOverlay* Graph_GetNextGameState(GameState* gameState) {
    void* gameStateInitFunc = GameState_GetInit(gameState);

    if (gameStateInitFunc == TitleSetup_Init) {
        return &gGameStateOverlayTable[0];
    }
    if (gameStateInitFunc == Select_Init) {
        return &gGameStateOverlayTable[1];
    }
    if (gameStateInitFunc == Title_Init) {
        return &gGameStateOverlayTable[2];
    }
    if (gameStateInitFunc == Play_Init) {
        return &gGameStateOverlayTable[3];
    }
    if (gameStateInitFunc == Opening_Init) {
        return &gGameStateOverlayTable[4];
    }
    if (gameStateInitFunc == FileChoose_Init) {
        return &gGameStateOverlayTable[5];
    }

    LOG_ADDRESS("game_init_func", gameStateInitFunc);
    return NULL;
}

void Graph_Init(GraphicsContext* gfxCtx) {
    memset(gfxCtx,0, sizeof(GraphicsContext));
    gfxCtx->gfxPoolIdx = 0;
    gfxCtx->fbIdx = 0;
    gfxCtx->viMode = NULL;
    gfxCtx->viFeatures = gViConfigFeatures;
    gfxCtx->xScale = gViConfigXScale;
    gfxCtx->yScale = gViConfigYScale;
    osCreateMesgQueue(&gfxCtx->queue, gfxCtx->msgBuff, ARRAY_COUNT(gfxCtx->msgBuff));
    func_800D31F0();
    Fault_AddClient(&sGraphFaultClient, Graph_FaultClient, 0, 0);
}

void Graph_Destroy(GraphicsContext* gfxCtx) {
    func_800D3210();
    Fault_RemoveClient(&sGraphFaultClient);
}

void Graph_TaskSet00(GraphicsContext* gfxCtx) {
    static Gfx* D_8012D260 = NULL;
    static s32 sGraphCfbInfoIdx = 0;

    OSTime time;
    OSTimer timer;
    OSMesg msg;
    OSTask_t* task = &gfxCtx->task.list.t;
    OSScTask* scTask = &gfxCtx->task;
    CfbInfo* cfb;
    s32 pad1;

    D_8016A528 = osGetTime() - sGraphSetTaskTime - D_8016A558;

    osSetTimer(&timer, OS_USEC_TO_CYCLES(3000000), 0, &gfxCtx->queue, OS_MESG_32(666));

    osRecvMesg(&gfxCtx->queue, &msg, OS_MESG_BLOCK);
    osStopTimer(&timer);
    //OTRTODO - Proper GFX crash handler
    #if 0
    if (msg == (OSMesg)666) {
        osSyncPrintf(VT_FGCOL(RED));
        osSyncPrintf("RCPが帰ってきませんでした。"); // "RCP did not return."
        osSyncPrintf(VT_RST);
        LogUtils_LogHexDump((void*)&HW_REG(SP_MEM_ADDR_REG, u32), 0x20);
        LogUtils_LogHexDump((void*)&DPC_START_REG, 0x20);
        LogUtils_LogHexDump(gGfxSPTaskYieldBuffer, sizeof(gGfxSPTaskYieldBuffer));

        SREG(6) = -1;
        if (D_8012D260 != NULL) {
            HREG(80) = 7;
            HREG(81) = 1;
            HREG(83) = 2;
            D_8012D260 = D_8012D260;
            Graph_DisassembleUCode(D_8012D260);
        }
        Fault_AddHungupAndCrashImpl("RCP is HUNG UP!!", "Oh! MY GOD!!");
    }
    #endif
    osRecvMesg(&gfxCtx->queue, &msg, OS_MESG_NOBLOCK);

    D_8012D260 = gfxCtx->workBuffer;
    if (gfxCtx->callback != NULL) {
        gfxCtx->callback(gfxCtx, gfxCtx->callbackParam);
    }

    time = osGetTime();
    if (D_8016A550 != 0) {
        D_8016A558 = (D_8016A558 + time) - D_8016A550;
        D_8016A550 = time;
    }
    D_8016A520 = D_8016A558;
    D_8016A558 = 0;
    sGraphSetTaskTime = osGetTime();

    task->type = M_GFXTASK;
    task->flags = OS_SC_DRAM_DLIST;
    task->ucode_boot = SysUcode_GetUCodeBoot();
    task->ucode_boot_size = SysUcode_GetUCodeBootSize();
    task->ucode = SysUcode_GetUCode();
    task->ucode_data = SysUcode_GetUCodeData();
    task->ucode_size = 0x1000;
    task->ucode_data_size = 0x800;
    task->dram_stack = (u64*)gGfxSPTaskStack;
    task->dram_stack_size = sizeof(gGfxSPTaskStack);
    task->output_buff = gGfxSPTaskOutputBuffer;
    task->output_buff_size = (u64*)((u8*)gGfxSPTaskOutputBuffer + sizeof(gGfxSPTaskOutputBuffer));
    task->data_ptr = (u64*)gfxCtx->workBuffer;

    OPEN_DISPS(gfxCtx);
    task->data_size = (uintptr_t)WORK_DISP - (uintptr_t)gfxCtx->workBuffer;
    CLOSE_DISPS(gfxCtx);

    task->yield_data_ptr = (u64*)gGfxSPTaskYieldBuffer;
    task->yield_data_size = sizeof(gGfxSPTaskYieldBuffer);

    scTask->next = NULL;
    scTask->flags = OS_SC_RCP_MASK | OS_SC_SWAPBUFFER | OS_SC_LAST_TASK;
    if (SREG(33) & 1) {
        SREG(33) &= ~1;
        scTask->flags &= ~OS_SC_SWAPBUFFER;
        gfxCtx->fbIdx--;
    }

    scTask->msgQ = &gfxCtx->queue;
    scTask->msg.ptr = NULL;

    cfb = &sGraphCfbInfos[sGraphCfbInfoIdx++];
    cfb->fb1 = gfxCtx->curFrameBuffer;
    cfb->swapBuffer = gfxCtx->curFrameBuffer;
    cfb->viMode = gfxCtx->viMode;
    cfb->features = gfxCtx->viFeatures;
    cfb->xScale = gfxCtx->xScale;
    cfb->yScale = gfxCtx->yScale;
    cfb->unk_10 = 0;
    cfb->updateRate = R_UPDATE_RATE;

    scTask->framebuffer = cfb;
    sGraphCfbInfoIdx = sGraphCfbInfoIdx % ARRAY_COUNT(sGraphCfbInfos);

    gfxCtx->schedMsgQ = &gSchedContext.cmdQ;

    osSendMesgPtr(&gSchedContext.cmdQ, scTask, OS_MESG_BLOCK);
    Sched_SendEntryMsg(&gSchedContext);
}

void Graph_Update(GraphicsContext* gfxCtx, GameState* gameState) {
    u32 problem;

    gameState->unk_A0 = 0;
    Graph_InitTHGA(gfxCtx);

    OPEN_DISPS(gfxCtx);

    gDPNoOpString(WORK_DISP++, "WORK_DISP 開始", 0);
    gDPNoOpString(POLY_OPA_DISP++, "POLY_OPA_DISP 開始", 0);
    gDPNoOpString(POLY_XLU_DISP++, "POLY_XLU_DISP 開始", 0);
    gDPNoOpString(OVERLAY_DISP++, "OVERLAY_DISP 開始", 0);

    CLOSE_DISPS(gfxCtx);

    GameState_ReqPadData(gameState);
    GameState_Update(gameState);
    Debug_Draw();

    OPEN_DISPS(gfxCtx);

    gDPNoOpString(WORK_DISP++, "WORK_DISP 終了", 0);
    gDPNoOpString(POLY_OPA_DISP++, "POLY_OPA_DISP 終了", 0);
    gDPNoOpString(POLY_XLU_DISP++, "POLY_XLU_DISP 終了", 0);
    gDPNoOpString(OVERLAY_DISP++, "OVERLAY_DISP 終了", 0);

    CLOSE_DISPS(gfxCtx);

    OPEN_DISPS(gfxCtx);

    gSPBranchList(WORK_DISP++, gfxCtx->polyOpaBuffer);
    gSPBranchList(POLY_OPA_DISP++, gfxCtx->polyXluBuffer);
    gSPBranchList(POLY_XLU_DISP++, gfxCtx->worldOverlayBuffer);
    gSPBranchList(WORLD_OVERLAY_DISP++, gfxCtx->polyKalBuffer);
    gSPBranchList(POLY_KAL_DISP++, gfxCtx->overlayBuffer);
    gDPPipeSync(OVERLAY_DISP++);
    gDPFullSync(OVERLAY_DISP++);
    gSPEndDisplayList(OVERLAY_DISP++);

    CLOSE_DISPS(gfxCtx);

    if (HREG(80) == 10 && HREG(93) == 2) {
        HREG(80) = 7;
        HREG(81) = -1;
        HREG(83) = HREG(92);
    }

    if (HREG(80) == 7 && HREG(81) != 0) {
        if (HREG(82) == 3) {
            Fault_AddClient(&sGraphUcodeFaultClient, Graph_UCodeFaultClient, gfxCtx->workBuffer, "do_count_fault");
        }

        Graph_DisassembleUCode(gfxCtx->workBuffer);

        if (HREG(82) == 3) {
            Fault_RemoveClient(&sGraphUcodeFaultClient);
        }

        if (HREG(81) < 0) {
            LogUtils_LogHexDump((void*)&HW_REG(SP_MEM_ADDR_REG, u32), 0x20);
            LogUtils_LogHexDump((void*)&DPC_START_REG, 0x20);
        }

        if (HREG(81) < 0) {
            HREG(81) = 0;
        }
    }

    problem = false;

    {
        GfxPool* pool = &gGfxPools[gfxCtx->gfxPoolIdx & 1];

        if (pool->headMagic != GFXPOOL_HEAD_MAGIC) {
            //! @bug (?) : "problem = true;" may be missing
            osSyncPrintf("%c", 7);
            // "Dynamic area head is destroyed"
            osSyncPrintf(VT_COL(RED, WHITE) "ダイナミック領域先頭が破壊されています\n" VT_RST);
            Fault_AddHungupAndCrash(__FILE__, __LINE__);
        }
        if (pool->tailMagic != GFXPOOL_TAIL_MAGIC) {
            problem = true;
            osSyncPrintf("%c", 7);
            // "Dynamic region tail is destroyed"
            osSyncPrintf(VT_COL(RED, WHITE) "ダイナミック領域末尾が破壊されています\n" VT_RST);
            Fault_AddHungupAndCrash(__FILE__, __LINE__);
        }
    }

    if (THGA_IsCrash(&gfxCtx->polyOpa)) {
        problem = true;
        osSyncPrintf("%c", 7);
        // "Zelda 0 is dead"
        osSyncPrintf(VT_COL(RED, WHITE) "ゼルダ0は死んでしまった(graph_alloc is empty)\n" VT_RST);
    }
    if (THGA_IsCrash(&gfxCtx->polyXlu)) {
        problem = true;
        osSyncPrintf("%c", 7);
        // "Zelda 1 is dead"
        osSyncPrintf(VT_COL(RED, WHITE) "ゼルダ1は死んでしまった(graph_alloc is empty)\n" VT_RST);
    }
    if (THGA_IsCrash(&gfxCtx->overlay)) {
        problem = true;
        osSyncPrintf("%c", 7);
        // "Zelda 4 is dead"
        osSyncPrintf(VT_COL(RED, WHITE) "ゼルダ4は死んでしまった(graph_alloc is empty)\n" VT_RST);
    }

    if (!problem) {
        Graph_TaskSet00(gfxCtx);
        gfxCtx->gfxPoolIdx++;
        gfxCtx->fbIdx++;
    }

    func_800F3054();

    {
        OSTime time = osGetTime();
        s32 pad[4];

        D_8016A538 = gRSPGFXTotalTime;
        D_8016A530 = gRSPAudioTotalTime;
        D_8016A540 = gRDPTotalTime;
        gRSPGFXTotalTime = 0;
        gRSPAudioTotalTime = 0;
        gRDPTotalTime = 0;

        if (sGraphUpdateTime != 0) {
            D_8016A548 = time - sGraphUpdateTime;
        }
        sGraphUpdateTime = time;
    }

    if (CVar_GetS32("gDebugEnabled", 0))
    {
        if (CHECK_BTN_ALL(gameState->input[0].press.button, BTN_Z) &&
            CHECK_BTN_ALL(gameState->input[0].cur.button, BTN_L | BTN_R)) {
            gSaveContext.gameMode = 0;
            SET_NEXT_GAMESTATE(gameState, Select_Init, SelectContext);
            gameState->running = false;
        }
    }

    if (gIsCtrlr2Valid && PreNmiBuff_IsResetting(gAppNmiBufferPtr) && !gameState->unk_A0) {
        // "To reset mode"
        osSyncPrintf(VT_COL(YELLOW, BLACK) "PRE-NMIによりリセットモードに移行します\n" VT_RST);
        SET_NEXT_GAMESTATE(gameState, PreNMI_Init, PreNMIContext);
        gameState->running = false;
    }
}

uint64_t GetFrequency();
uint64_t GetPerfCounter();

static struct RunFrameContext {
    GraphicsContext gfxCtx;
    GameState* gameState;
    GameStateOverlay* nextOvl;
    GameStateOverlay* ovl;
    int state;
} runFrameContext;


extern AudioMgr gAudioMgr;

extern void ProcessSaveStateRequests(void);

static void RunFrame()
{
    u32 size;
    char faultMsg[0x50];
    static bool hasSetupSkybox = false;

    switch (runFrameContext.state) {
        case 0:
            break;
        case 1:
            goto nextFrame;
    }

    runFrameContext.nextOvl = &gGameStateOverlayTable[0];

    osSyncPrintf("グラフィックスレッド実行開始\n"); // "Start graphic thread execution"
    Graph_Init(&runFrameContext.gfxCtx);

    while (runFrameContext.nextOvl)
    {
        runFrameContext.ovl = runFrameContext.nextOvl;
        Overlay_LoadGameState(runFrameContext.ovl);

        size = runFrameContext.ovl->instanceSize;
        osSyncPrintf("クラスサイズ＝%dバイト\n", size); // "Class size = %d bytes"

        runFrameContext.gameState = SYSTEM_ARENA_MALLOC_DEBUG(size);

        if (!runFrameContext.gameState)
        {
            osSyncPrintf("確保失敗\n"); // "Failure to secure"

            sprintf(faultMsg, "CLASS SIZE= %d bytes", size);
            Fault_AddHungupAndCrashImpl("GAME CLASS MALLOC FAILED", faultMsg);
        }
        GameState_Init(runFrameContext.gameState, runFrameContext.ovl->init, &runFrameContext.gfxCtx);

        // Setup the normal skybox once before entering any game states to avoid the 0xabababab crash.
        // The crash is due to certain skyboxes not loading all the data they need from Skybox_Setup.
        if (!hasSetupSkybox) {
            PlayState* play = (PlayState*)runFrameContext.gameState;
            Skybox_Setup(play, &play->skyboxCtx, SKYBOX_NORMAL_SKY);
            hasSetupSkybox = true;
        }

        uint64_t freq = GetFrequency();

        while (GameState_IsRunning(runFrameContext.gameState))
        {
            uint64_t ticksA, ticksB;
            ticksA = GetPerfCounter();

            Graph_StartFrame();

            PadMgr_ThreadEntry(&gPadMgr);

            Graph_Update(&runFrameContext.gfxCtx, runFrameContext.gameState);
            ticksB = GetPerfCounter();

            Graph_ProcessGfxCommands(runFrameContext.gfxCtx.workBuffer);


            //uint64_t diff = (ticksB - ticksA) / (freq / 1000);
            //printf("Frame simulated in %ims\n", diff);
            runFrameContext.state = 1;
            ProcessSaveStateRequests();
            return;
            nextFrame:;
        }

        runFrameContext.nextOvl = Graph_GetNextGameState(runFrameContext.gameState);
        GameState_Destroy(runFrameContext.gameState);
        SYSTEM_ARENA_FREE_DEBUG(runFrameContext.gameState);
        Overlay_FreeGameState(runFrameContext.ovl);
    }
    Graph_Destroy(&runFrameContext.gfxCtx);
    osSyncPrintf("グラフィックスレッド実行終了\n"); // "End of graphic thread execution"

    //Graph_Update(gfxCtxTest, gameStateTest);
    exit(0);
}

void Graph_ThreadEntry(void* arg0) {

    Graph_ProcessFrame(RunFrame);
}

void* Graph_Alloc(GraphicsContext* gfxCtx, size_t size) {
    TwoHeadGfxArena* thga = &gfxCtx->polyOpa;

    if (HREG(59) == 1) {
        osSyncPrintf("graph_alloc siz=%d thga size=%08x bufp=%08x head=%08x tail=%08x\n", size, thga->size, thga->bufp,
                     thga->p, thga->d);
    }
    return THGA_AllocEnd(&gfxCtx->polyOpa, ALIGN16(size));
}

void* Graph_Alloc2(GraphicsContext* gfxCtx, size_t size) {
    TwoHeadGfxArena* thga = &gfxCtx->polyOpa;

    if (HREG(59) == 1) {
        osSyncPrintf("graph_alloc siz=%d thga size=%08x bufp=%08x head=%08x tail=%08x\n", size, thga->size, thga->bufp,
                     thga->p, thga->d);
    }
    return THGA_AllocEnd(&gfxCtx->polyOpa, ALIGN16(size));
}

void Graph_OpenDisps(Gfx** dispRefs, GraphicsContext* gfxCtx, const char* file, s32 line) {
    if (HREG(80) == 7 && HREG(82) != 4) {
        dispRefs[0] = gfxCtx->polyOpa.p;
        dispRefs[1] = gfxCtx->polyXlu.p;
        dispRefs[2] = gfxCtx->overlay.p;

        gDPNoOpOpenDisp(gfxCtx->polyOpa.p++, file, line);
        gDPNoOpOpenDisp(gfxCtx->polyXlu.p++, file, line);
        gDPNoOpOpenDisp(gfxCtx->overlay.p++, file, line);
    }
}

void Graph_CloseDisps(Gfx** dispRefs, GraphicsContext* gfxCtx, const char* file, s32 line) {
    if (HREG(80) == 7 && HREG(82) != 4) {
        if (dispRefs[0] + 1 == gfxCtx->polyOpa.p) {
            gfxCtx->polyOpa.p = dispRefs[0];
        } else {
            gDPNoOpCloseDisp(gfxCtx->polyOpa.p++, file, line);
        }

        if (dispRefs[1] + 1 == gfxCtx->polyXlu.p) {
            gfxCtx->polyXlu.p = dispRefs[1];
        } else {
            gDPNoOpCloseDisp(gfxCtx->polyXlu.p++, file, line);
        }

        if (dispRefs[2] + 1 == gfxCtx->overlay.p) {
            gfxCtx->overlay.p = dispRefs[2];
        } else {
            gDPNoOpCloseDisp(gfxCtx->overlay.p++, file, line);
        }
    }
}

Gfx* Graph_GfxPlusOne(Gfx* gfx) {
    return gfx + 1;
}

Gfx* Graph_BranchDlist(Gfx* gfx, Gfx* dst) {
    gSPBranchList(gfx, dst);
    return dst;
}

void* Graph_DlistAlloc(Gfx** gfx, size_t size) {
    u8* ptr;
    Gfx* dst;

    size = ((size + 7) & ~7),

    ptr = (u8*)(*gfx + 1);

    dst = (Gfx*)(ptr + size);
    gSPBranchList(*gfx, dst);

    *gfx = dst;
    return ptr;
}
