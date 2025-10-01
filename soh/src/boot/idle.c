#include "global.h"
#include "vt.h"

OSThread gMainThread;
u8 sMainStack[0x900];
StackEntry sMainStackInfo;
OSMesg sPiMgrCmdBuff[50];
OSMesgQueue gPiMgrCmdQ;
OSViMode gViConfigMode;
u8 D_80013960;

s8 D_80009430 = 1;
vu8 gViConfigUseDefault = 1;
u8 gViConfigAdditionalScanLines = 0;
u32 gViConfigFeatures = OS_VI_DITHER_FILTER_ON | OS_VI_GAMMA_OFF;
f32 gViConfigXScale = 1.0;
f32 gViConfigYScale = 1.0;

void Main_ThreadEntry(void* arg) {
    OSTime time;

    osSyncPrintf("mainx 実行開始\n");
    DmaMgr_Init();
    osSyncPrintf("codeセグメントロード中...");
    time = osGetTime();
    // DmaMgr_SendRequest1(_codeSegmentStart, (uintptr_t)_codeSegmentRomStart, _codeSegmentRomEnd -
    // _codeSegmentRomStart,
    //"../idle.c", 238);
    time -= osGetTime();
    osSyncPrintf("\rcodeセグメントロード中...完了\n");
    osSyncPrintf("転送時間 %6.3f\n");
    // bzero(_codeSegmentBssStart, _codeSegmentBssEnd - _codeSegmentBssStart);
    osSyncPrintf("codeセグメントBSSクリア完了\n");
    Main(arg);
    osSyncPrintf("mainx 実行終了\n");
}

void Idle_ThreadEntry(void* arg) {
    osSyncPrintf("アイドルスレッド(idleproc)実行開始\n");
    osSyncPrintf("作製者    : %s\n", gBuildTeam);
    osSyncPrintf("作成日時  : %s\n", gBuildDate);
    osSyncPrintf("MAKEOPTION: %s\n", gBuildMakeOption);
    osSyncPrintf(VT_FGCOL(GREEN));
    osSyncPrintf("ＲＡＭサイズは %d キロバイトです(osMemSize/osGetMemSize)\n", (s32)osMemSize / 1024);
    // osSyncPrintf("_bootSegmentEnd(%08x) 以降のＲＡＭ領域はクリアされました(boot)\n", _bootSegmentEnd);
    osSyncPrintf("Ｚバッファのサイズは %d キロバイトです\n", 0x96);
    osSyncPrintf("ダイナミックバッファのサイズは %d キロバイトです\n", 0x92);
    osSyncPrintf("ＦＩＦＯバッファのサイズは %d キロバイトです\n", 0x60);
    osSyncPrintf("ＹＩＥＬＤバッファのサイズは %d キロバイトです\n", 3);
    osSyncPrintf("オーディオヒープのサイズは %d キロバイトです\n", ((s32)gSystemHeap - (s32)gAudioHeap) / 1024);
    osSyncPrintf(VT_RST);

    osCreateViManager(OS_PRIORITY_VIMGR);

    gViConfigFeatures = OS_VI_GAMMA_OFF | OS_VI_DITHER_FILTER_ON;
    gViConfigXScale = 1.0f;
    gViConfigYScale = 1.0f;

    switch (osTvType) {
        case OS_TV_NTSC:
            D_80013960 = 2;
            gViConfigMode = osViModeNtscLan1;
            break;

        case OS_TV_MPAL:
            D_80013960 = 0x1E;
            gViConfigMode = osViModeMpalLan1;
            break;

        case OS_TV_PAL:
            D_80013960 = 0x2C;
            gViConfigMode = osViModeFpalLan1;
            gViConfigYScale = 0.833f;
            break;
    }

    D_80009430 = 1;
    osViSetMode(&gViConfigMode);
    ViConfig_UpdateVi(1);
    osViBlack(1);
    osViSwapBuffer(0x803DA80); //! @bug Invalid vram address (probably intended to be 0x803DA800)
    osCreatePiManager(OS_PRIORITY_PIMGR, &gPiMgrCmdQ, sPiMgrCmdBuff, 50);
    StackCheck_Init(&sMainStackInfo, sMainStack, sMainStack + sizeof(sMainStack), 0, 0x400, "main");
    osCreateThread(&gMainThread, 3, Main_ThreadEntry, arg, sMainStack + sizeof(sMainStack), Z_PRIORITY_MAIN);
    osStartThread(&gMainThread);
    osSetThreadPri(NULL, OS_PRIORITY_IDLE);

    while (1) {
        ;
    }
}
