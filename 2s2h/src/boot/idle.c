#include "prevent_bss_reordering.h"
#include "buffers.h"
#include "irqmgr.h"
#include "main.h"
#include "stack.h"
#include "stackcheck.h"
#include "z64thread.h"

// Variables are put before most headers as a hacky way to bypass bss reordering
IrqMgr gIrqMgr;
STACK(sIrqMgrStack, 0x500);
StackEntry sIrqMgrStackInfo;
OSThread sMainThread;
STACK(sMainStack, 0x900);
StackEntry sMainStackInfo;
OSMesg sPiMgrCmdBuff[50];
OSMesgQueue gPiMgrCmdQueue;
OSViMode gViConfigMode;
u8 gViConfigModeType;

#include "global.h"
#include "buffers.h"
#include "idle.h"

u8 D_80096B20 = 1;
vu8 gViConfigUseBlack = true;
u8 gViConfigAdditionalScanLines = 0;
u32 gViConfigFeatures = OS_VI_DITHER_FILTER_ON | OS_VI_GAMMA_OFF;
f32 gViConfigXScale = 1.0f;
f32 gViConfigYScale = 1.0f;

void Main_ClearMemory(void* begin, void* end) {
    // if (begin < end) {
    //     bzero(begin, (uintptr_t)end - (uintptr_t)begin);
    // }
}

void Main_InitFramebuffer(u32* framebuffer, size_t numBytes, u32 value) {
    // for (; numBytes > 0; numBytes -= sizeof(u32)) {
    //     *framebuffer++ = value;
    // }
}

void Main_InitScreen(void) {
#if 0
    Main_InitFramebuffer((u32*)gLoBuffer.framebuffer, sizeof(gLoBuffer.framebuffer),
                         (GPACK_RGBA5551(0, 0, 0, 1) << 16) | GPACK_RGBA5551(0, 0, 0, 1));
    ViConfig_UpdateVi(false);
    osViSwapBuffer(gLoBuffer.framebuffer);
    osViBlack(false);
#endif
}

void Main_InitMemory(void) {
#if 0
    void* memStart = (void*)0x80000400;
    void* memEnd = OS_PHYSICAL_TO_K0(osMemSize);

    Main_ClearMemory(memStart, SEGMENT_START(framebuffer_lo));

    // Clear the rest of the buffer that was not initialized by Main_InitScreen
    Main_ClearMemory(&gLoBuffer.skyboxBuffer, SEGMENT_END(framebuffer_lo));

    // Clear all the buffers after the `code` segment, up to the end of the available RAM space
    Main_ClearMemory(SEGMENT_END(code), memEnd);
#endif
}

void Main_Init(void) {
#if 0
    DmaRequest dmaReq;
    OSMesgQueue mq;
    OSMesg msg[1];
    size_t prevSize;

    osCreateMesgQueue(&mq, msg, ARRAY_COUNT(msg));

    prevSize = gDmaMgrDmaBuffSize;
    gDmaMgrDmaBuffSize = 0;

    DmaMgr_SendRequestImpl(&dmaReq, SEGMENT_START(code), SEGMENT_ROM_START(code), SEGMENT_ROM_SIZE_ALT(code), 0, &mq,
                           NULL);
    Main_InitScreen();
    Main_InitMemory();
    osRecvMesg(&mq, NULL, OS_MESG_BLOCK);

    gDmaMgrDmaBuffSize = prevSize;

    Main_ClearMemory(SEGMENT_BSS_START(code), SEGMENT_BSS_END(code));
#endif
}

void Main_ThreadEntry(void* arg) {
#if 0
    StackCheck_Init(&sIrqMgrStackInfo, sIrqMgrStack, STACK_TOP(sIrqMgrStack), 0, 0x100, "irqmgr");
    IrqMgr_Init(&gIrqMgr, STACK_TOP(sIrqMgrStack), Z_PRIORITY_IRQMGR, 1);
    DmaMgr_Start();
    Main_Init();
    Main(arg);
    DmaMgr_Stop();
#endif
}

void Idle_InitVideo(void) {
#if 0
    osCreateViManager(OS_PRIORITY_VIMGR);

    gViConfigFeatures = OS_VI_DITHER_FILTER_ON | OS_VI_GAMMA_OFF;
    gViConfigXScale = 1.0f;
    gViConfigYScale = 1.0f;

    switch (osTvType) {
        case OS_TV_NTSC:
            gViConfigModeType = OS_VI_NTSC_LAN1;
            gViConfigMode = osViModeNtscLan1;
            break;

        case OS_TV_MPAL:
            gViConfigModeType = OS_VI_MPAL_LAN1;
            gViConfigMode = osViModeMpalLan1;
            break;

        case OS_TV_PAL:
            gViConfigModeType = OS_VI_FPAL_LAN1;
            gViConfigMode = osViModeFpalLan1;
            gViConfigYScale = 0.833f;
            break;
    }

    D_80096B20 = 1;
#endif
}

void Idle_ThreadEntry(void* arg) {
    // Idle_InitVideo();
    // osCreatePiManager(OS_PRIORITY_PIMGR, &gPiMgrCmdQueue, sPiMgrCmdBuff, ARRAY_COUNT(sPiMgrCmdBuff));
    // StackCheck_Init(&sMainStackInfo, sMainStack, STACK_TOP(sMainStack), 0, 0x400, "main");
    // osCreateThread(&sMainThread, Z_THREAD_ID_MAIN, Main_ThreadEntry, arg, STACK_TOP(sMainStack), Z_PRIORITY_MAIN);
    // osStartThread(&sMainThread);
    // osSetThreadPri(NULL, OS_PRIORITY_IDLE);

    // for (;;) {}
}
