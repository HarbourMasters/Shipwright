#include <libultraship/libultra.h>
#include <stdio.h>
#include <string.h>
#include "z64.h"
#include "OTRGlobals.h"
//#include <math.h>

u32 osResetType;
u32 osTvType = OS_TV_NTSC;
//u32 osTvType = OS_TV_PAL;
OSViMode osViModeNtscLan1;
OSViMode osViModeMpalLan1;
OSViMode osViModeFpalLan1;
OSViMode osViModePalLan1;
// AudioContext gAudioContext;
// unk_D_8016E750 D_8016E750[4];
u8 gLetterTLUT[4][32];
u8 gFontFF[999];
DmaEntry gDmaDataTable[0x60C];
// u8 D_80133418;
u16 gAudioSEFlagSwapSource[64];
u16 gAudioSEFlagSwapTarget[64];
u8 gAudioSEFlagSwapMode[64];

u8 osAppNmiBuffer[2048];

f32 qNaN0x10000 = 0x7F810000;

//void gSPTextureRectangle(Gfx* pkt, s32 xl, s32 yl, s32 xh, s32 yh, u32 tile, u32 s, s32 t, u32 dsdx, u32 dtdy)
//{
//	__gSPTextureRectangle(pkt, xl, yl, xh, yh, tile, s, t, dsdx, dtdy);
//}

s32 osJamMesg(OSMesgQueue* mq, OSMesg msg, s32 flag)
{
}

void osSetEventMesg(OSEvent e, OSMesgQueue* mq, OSMesg msg)
{
}

void osViSetEvent(OSMesgQueue* mq, OSMesg msg, u32 retraceCount)
{
}

OSId osGetThreadId(OSThread* thread)
{
}

OSPri osGetThreadPri(OSThread* thread)
{
}

void osSetThreadPri(OSThread* thread, OSPri pri)
{
}

s32 osSetTimer(OSTimer* timer, OSTime countdown, OSTime interval, OSMesgQueue* mq, OSMesg msg)
{

}

void osCreatePiManager(OSPri pri, OSMesgQueue* cmdQ, OSMesg* cmdBuf, s32 cmdMsgCnt)
{

}

void osCreateViManager(OSPri pri)
{

}

s32 osMotorInit(OSMesgQueue* ctrlrqueue, OSPfs* pfs, s32 channel)
{
    return 0;
}

u32 osAiGetLength(void)
{

}

s32 osPfsFreeBlocks(OSPfs* pfs, s32* leftoverBytes)
{

}

s32 osEPiWriteIo(OSPiHandle* handle, u32 devAddr, u32 data)
{

}

s32 osPfsReadWriteFile(OSPfs* pfs, s32 fileNo, u8 flag, s32 offset, ptrdiff_t size, u8* data)
{

}

s32 osPfsDeleteFile(OSPfs* pfs, u16 companyCode, u32 gameCode, u8* gameName, u8* extName)
{

}

s32 osPfsFileState(OSPfs* pfs, s32 fileNo, OSPfsState* state)
{

}

s32 osPfsInitPak(OSMesgQueue* mq, OSPfs* pfs, s32 channel)
{

}

s32 __osPfsCheckRamArea(OSPfs* pfs)
{

}

s32 osPfsChecker(OSPfs* pfs)
{

}

s32 osPfsFindFile(OSPfs* pfs, u16 companyCode, u32 gameCode, u8* gameName, u8* extName, s32* fileNo)
{

}

s32 osPfsAllocateFile(OSPfs* pfs, u16 companyCode, u32 gameCode, u8* gameName, u8* extName, s32 length, s32* fileNo)
{

}

s32 osAiSetNextBuffer(void* buf, size_t size)
{

}

s32 __osMotorAccess(OSPfs* pfs, u32 vibrate)
{

}

OSIntMask osSetIntMask(OSIntMask a)
{
	return 0;
}

s32 osAfterPreNMI(void)
{
	return 0;
}

s32 osProbeRumblePak(OSMesgQueue* ctrlrqueue, OSPfs* pfs, u32 channel)
{

}

s32 osSetRumble(OSPfs* pfs, u32 vibrate)
{
	return 0;
}

void osCreateThread(OSThread* thread, OSId id, void (*entry)(void*), void* arg, void* sp, OSPri pri)
{

}

void osStartThread(OSThread* thread)
{

}

void osStopThread(OSThread* thread)
{

}

void osDestroyThread(OSThread* thread)
{

}

void osWritebackDCache(void* vaddr, s32 nbytes)
{

}

void osInvalICache(void* vaddr, s32 nbytes)
{

}

s32 osContStartQuery(OSMesgQueue* mq)
{

}

void osContGetQuery(OSContStatus* data)
{

}

void osViSwapBuffer(void* vaddr)
{

}

void* osViGetNextFramebuffer()
{

}

u32 __osGetFpcCsr()
{
	return 0;
}

void __osSetFpcCsr(u32 a0)
{

}

s32 __osDisableInt(void)
{

}

void __osRestoreInt(s32 a0)
{

}

OSThread* __osGetActiveQueue(void)
{

}

OSThread* __osGetCurrFaultedThread(void)
{

}

void osCartRomInit()
{

}

u32 osMemSize = 1024 * 1024 * 1024;

void Audio_osInvalDCache(void* buf, s32 size)
{

}

void Audio_osWritebackDCache(void* mem, s32 size)
{

}


s32 osAiSetFrequency(u32 freq)
{
	return 1;
}

s32 osEPiStartDma(OSPiHandle* handle, OSIoMesg* mb, s32 direction)
{

}

void osInvalDCache(void* vaddr, s32 nbytes)
{

}

void osWritebackDCacheAll(void)
{

}

void Audio_SetBGM(u32 bgmId)
{

}

s32 osContSetCh(u8 ch)
{

}

u32 osDpGetStatus(void)
{

}

void osDpSetStatus(u32 status)
{

}

u32 __osSpGetStatus()
{

}

void __osSpSetStatus(u32 status)
{

}

OSPiHandle* osDriveRomInit()
{
	
}

void osViSetMode(OSViMode* mode)
{

}

void osViBlack(u8 active)
{

}

void osViSetYScale(f32 scale)
{

}

void osViSetXScale(f32 value)
{

}

void osViSetSpecialFeatures(u32 func)
{

}

void __osInitialize_common(void)
{

}

void __osInitialize_autodetect(void)
{

}

void __osExceptionPreamble()
{

}

void __osCleanupThread(void)
{

}

s32 _Printf(PrintCallback a, void* arg, const char* fmt, va_list ap) {
    unsigned char buffer[4096];

    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    a(arg, buffer, strlen(buffer));
}

void osSpTaskLoad(OSTask* task)
{

}

void osSpTaskStartGo(OSTask* task)
{

}

void osSetUpMempakWrite(s32 channel, OSPifRam* buf)
{

}

u32 osGetMemSize(void)
{
	return 1024 * 1024 * 1024;
}

s32 osEPiReadIo(OSPiHandle* handle, u32 devAddr, u32* data)
{
	return 0;
}

u32* osViGetCurrentFramebuffer(void)
{

}

void osSpTaskYield(void)
{
}

s32 osStopTimer(OSTimer* timer)
{

}

OSYieldResult osSpTaskYielded(OSTask* task)
{

}

void osViExtendVStart(u32 arg0)
{

}
