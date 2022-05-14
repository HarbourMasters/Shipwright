#include "ultra64.h"
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

void osCreateMesgQueue(OSMesgQueue* mq, OSMesg* msgBuf, s32 count) {
    mq->validCount = 0;
    mq->first = 0;
    mq->msgCount = count;
    mq->msg = msgBuf;
    return;
}

s32 osSendMesg(OSMesgQueue* mq, OSMesg msg, s32 flag) {
    s32 index;
    if (mq->validCount >= mq->msgCount) {
        return -1;
    }
    index = (mq->first + mq->validCount) % mq->msgCount;
    mq->msg[index] = msg;
    mq->validCount++;
    return 0;
}
s32 osRecvMesg(OSMesgQueue* mq, OSMesg* msg, s32 flag) {
    if (mq->validCount == 0) {
        return -1;
    }
    if (msg != NULL) {
        *msg = *(mq->first + mq->msg);
    }
    mq->first = (mq->first + 1) % mq->msgCount;
    mq->validCount--;
    return 0;
}

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


AudioTable gSoundFontTable = { 0x0026,
                               0x0000,
                               0x00000000,
                               { 0 },
                               {
                                   {
                                       0x00000000,
                                       0x00003AA0,
                                       0x02,
                                       0x00,
                                       0x00FF,
                                       0x5C04,
                                       0x0088,
                                   },
                                   {
                                       0x00003AA0,
                                       0x000017B0,
                                       0x02,
                                       0x00,
                                       0x00FF,
                                       0x3301,
                                       0x0029,
                                   },
                                   {
                                       0x00005250,
                                       0x00000CE0,
                                       0x02,
                                       0x02,
                                       0x00FF,
                                       0x1500,
                                       0x0000,
                                   },
                                   {
                                       0x00005F30,
                                       0x000015D0,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x1040,
                                       0x0000,
                                   },
                                   {
                                       0x00007500,
                                       0x00000100,
                                       0x02,
                                       0x02,
                                       0x02FF,
                                       0x0200,
                                       0x0000,
                                   },
                                   {
                                       0x00007600,
                                       0x00000B60,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0D40,
                                       0x0000,
                                   },
                                   {
                                       0x00008160,
                                       0x00000520,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0F00,
                                       0x0000,
                                   },
                                   {
                                       0x00008680,
                                       0x00000840,
                                       0x02,
                                       0x02,
                                       0x03FF,
                                       0x0A40,
                                       0x0000,
                                   },
                                   {
                                       0x00008EC0,
                                       0x00000B20,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0A40,
                                       0x0000,
                                   },
                                   {
                                       0x000099E0,
                                       0x00000FC0,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0D40,
                                       0x0000,
                                   },
                                   {
                                       0x0000A9A0,
                                       0x00000B00,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0E40,
                                       0x0000,
                                   },
                                   {
                                       0x0000B4A0,
                                       0x00000390,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0400,
                                       0x0000,
                                   },
                                   {
                                       0x0000B830,
                                       0x00000320,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0300,
                                       0x0000,
                                   },
                                   {
                                       0x0000BB50,
                                       0x000006F0,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x1000,
                                       0x0000,
                                   },
                                   {
                                       0x0000C240,
                                       0x00000390,
                                       0x02,
                                       0x02,
                                       0x05FF,
                                       0x0500,
                                       0x0000,
                                   },
                                   {
                                       0x0000C5D0,
                                       0x00000B40,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x1000,
                                       0x0000,
                                   },
                                   {
                                       0x0000D110,
                                       0x000009E0,
                                       0x02,
                                       0x02,
                                       0x06FF,
                                       0x0A40,
                                       0x0000,
                                   },
                                   {
                                       0x0000DAF0,
                                       0x00000560,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0F00,
                                       0x0000,
                                   },
                                   {
                                       0x0000E050,
                                       0x00000CC0,
                                       0x02,
                                       0x01,
                                       0x01FF,
                                       0x0D40,
                                       0x0000,
                                   },
                                   {
                                       0x0000ED10,
                                       0x000003A0,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0400,
                                       0x0000,
                                   },
                                   {
                                       0x0000F0B0,
                                       0x00000AA0,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0A40,
                                       0x0000,
                                   },
                                   {
                                       0x0000FB50,
                                       0x00000A60,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0A40,
                                       0x0000,
                                   },
                                   {
                                       0x000105B0,
                                       0x00000BF0,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0B40,
                                       0x0000,
                                   },
                                   {
                                       0x000111A0,
                                       0x000001F0,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0400,
                                       0x0000,
                                   },
                                   {
                                       0x00011390,
                                       0x00000860,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0900,
                                       0x0000,
                                   },
                                   {
                                       0x00011BF0,
                                       0x000005B0,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0800,
                                       0x0000,
                                   },
                                   {
                                       0x000121A0,
                                       0x00000250,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0F00,
                                       0x0000,
                                   },
                                   {
                                       0x000123F0,
                                       0x000004E0,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0C00,
                                       0x0000,
                                   },
                                   {
                                       0x000128D0,
                                       0x000004C0,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0500,
                                       0x0000,
                                   },
                                   {
                                       0x00012D90,
                                       0x00000C00,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0C40,
                                       0x0000,
                                   },
                                   {
                                       0x00013990,
                                       0x00000270,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0500,
                                       0x0000,
                                   },
                                   {
                                       0x00013C00,
                                       0x00000640,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x0800,
                                       0x0000,
                                   },
                                   {
                                       0x00014240,
                                       0x00001300,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x1040,
                                       0x0000,
                                   },
                                   {
                                       0x00015540,
                                       0x000011A0,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x1040,
                                       0x0000,
                                   },
                                   {
                                       0x000166E0,
                                       0x00001720,
                                       0x02,
                                       0x02,
                                       0x01FF,
                                       0x1040,
                                       0x0000,
                                   },
                                   {
                                       0x00017E00,
                                       0x00000DE0,
                                       0x02,
                                       0x01,
                                       0x01FF,
                                       0x0F40,
                                       0x0000,
                                   },
                                   {
                                       0x00018BE0,
                                       0x00000660,
                                       0x02,
                                       0x01,
                                       0x01FF,
                                       0x0F00,
                                       0x0000,
                                   },
                                   {
                                       0x00019240,
                                       0x00012B60,
                                       0x02,
                                       0x02,
                                       0x00FF,
                                       0x0200,
                                       0x0000,
                                   },
                               }
                            };

AudioTable gSequenceTable = { 0x006E,
                              0x0000,
                              0x00000000,
                              { 0 },
                              {
                                  {
                                      0x00000000,
                                      0x00006A90,
                                      0x02,
                                      0x00,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00006A90,
                                      0x00001040,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00007AD0,
                                      0x00002500,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00009FD0,
                                      0x00000AB0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0000AA80,
                                      0x00000890,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0000B310,
                                      0x00000980,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0000BC90,
                                      0x00000B80,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0000C810,
                                      0x000007A0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0000CFB0,
                                      0x000005C0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0000D570,
                                      0x00000730,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0000DCA0,
                                      0x000006A0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0000E340,
                                      0x00000740,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0000EA80,
                                      0x00000900,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0000F380,
                                      0x000006F0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0000FA70,
                                      0x00000820,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00010290,
                                      0x000006B0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00010940,
                                      0x000007C0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00011100,
                                      0x00000800,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00011900,
                                      0x000009F0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000122F0,
                                      0x00000960,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00012C50,
                                      0x000005F0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00013240,
                                      0x00000650,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00013890,
                                      0x00000580,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00013E10,
                                      0x00000670,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00014480,
                                      0x00000170,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000145F0,
                                      0x00001AB0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000160A0,
                                      0x000010F0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00017190,
                                      0x00000F30,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000180C0,
                                      0x00000310,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000183D0,
                                      0x00000710,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00018AE0,
                                      0x00001100,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00019BE0,
                                      0x00000570,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0001A150,
                                      0x000001B0,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0001A300,
                                      0x00000630,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0001A930,
                                      0x00000150,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0001AA80,
                                      0x00000560,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0001AFE0,
                                      0x00000230,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0001B210,
                                      0x000004C0,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0001B6D0,
                                      0x000009A0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0001C070,
                                      0x000028D0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0001E940,
                                      0x000004D0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0001EE10,
                                      0x00000A00,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0001F810,
                                      0x00001240,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00020A50,
                                      0x00000450,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00020EA0,
                                      0x00001720,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000225C0,
                                      0x000005A0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00022B60,
                                      0x000012E0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00023E40,
                                      0x000025A0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000263E0,
                                      0x00000A20,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00026E00,
                                      0x000011A0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00027FA0,
                                      0x00000540,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000284E0,
                                      0x00000490,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00028970,
                                      0x00000510,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00028E80,
                                      0x00000790,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00029610,
                                      0x00000590,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00029BA0,
                                      0x000005B0,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0002A150,
                                      0x00001310,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0002B460,
                                      0x000001C0,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0002B620,
                                      0x00000980,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0002BFA0,
                                      0x00000390,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0002C330,
                                      0x00000F90,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0002D2C0,
                                      0x000003E0,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0002D6A0,
                                      0x00000960,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0002E000,
                                      0x00001A00,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0002FA00,
                                      0x00001A80,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00031480,
                                      0x00000660,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00031AE0,
                                      0x00001AD0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000335B0,
                                      0x000005B0,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00033B60,
                                      0x000000B0,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00033C10,
                                      0x00000140,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00033D50,
                                      0x00000170,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00033EC0,
                                      0x00000080,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00033F40,
                                      0x00000130,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00034070,
                                      0x000000D0,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00034140,
                                      0x00000480,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000345C0,
                                      0x00000410,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000349D0,
                                      0x00001020,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000359F0,
                                      0x00000B70,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00036560,
                                      0x00000E10,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00037370,
                                      0x000004A0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00037810,
                                      0x000011A0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000389B0,
                                      0x00000210,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00038BC0,
                                      0x00000A30,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000395F0,
                                      0x00000300,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000398F0,
                                      0x00000310,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00039C00,
                                      0x00000EC0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0003AAC0,
                                      0x00000470,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00000028,
                                      0x00000000,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0003AF30,
                                      0x000003A0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0003B2D0,
                                      0x00000220,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0003B4F0,
                                      0x00000720,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0003BC10,
                                      0x00001130,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0003CD40,
                                      0x00001890,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0003E5D0,
                                      0x00000490,
                                      0x02,
                                      0x01,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0003EA60,
                                      0x00000160,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0003EBC0,
                                      0x00001DC0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00040980,
                                      0x00000360,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00040CE0,
                                      0x00000AC0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000417A0,
                                      0x00000CD0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00042470,
                                      0x00000580,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000429F0,
                                      0x00001730,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00044120,
                                      0x000011B0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x000452D0,
                                      0x00000640,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00045910,
                                      0x00002610,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x00047F20,
                                      0x000034A0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0004B3C0,
                                      0x000018B0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0004CC70,
                                      0x000008A0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0004D510,
                                      0x00001370,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0004E880,
                                      0x000008A0,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                                  {
                                      0x0004F120,
                                      0x00000960,
                                      0x02,
                                      0x02,
                                      0x0000,
                                      0x0000,
                                      0x0000,
                                  },
                              }
                            };

AudioTable gSampleBankTable = { 0x0007,
                                0x0000,
                                0x00000000,
                                { 0 },
                                {
                                    {
                                        0x00000000,
                                        0x003EB2A0,
                                        0x02,
                                        0x04,
                                        0x0000,
                                        0x0000,
                                        0x0000,
                                    },
                                    {
                                        0x00000000,
                                        0x00000000,
                                        0x02,
                                        0x04,
                                        0x0000,
                                        0x0000,
                                        0x0000,
                                    },
                                    {
                                        0x003EB2A0,
                                        0x00005CD0,
                                        0x02,
                                        0x04,
                                        0x0000,
                                        0x0000,
                                        0x0000,
                                    },
                                    {
                                        0x003F0F70,
                                        0x0001D0B0,
                                        0x02,
                                        0x04,
                                        0x0000,
                                        0x0000,
                                        0x0000,
                                    },
                                    {
                                        0x0040E020,
                                        0x0000A5D0,
                                        0x02,
                                        0x04,
                                        0x0000,
                                        0x0000,
                                        0x0000,
                                    },
                                    {
                                        0x004185F0,
                                        0x0000FAB0,
                                        0x02,
                                        0x04,
                                        0x0000,
                                        0x0000,
                                        0x0000,
                                    },
                                    {
                                        0x004280A0,
                                        0x000292F0,
                                        0x02,
                                        0x04,
                                        0x0000,
                                        0x0000,
                                        0x0000,
                                    },
                                }
                            };

u8 gSequenceFontTable[0x1C0] = {
    0xDC,
    0x00,
    0xDF,
    0x00,
    0xE1,
    0x00,
    0xE3,
    0x00,
    0xE5,
    0x00,
    0xE7,
    0x00,
    0xE9,
    0x00,
    0xEB,
    0x00,
    0xED,
    0x00,
    0xEF,
    0x00,
    0xF1,
    0x00,
    0xF3,
    0x00,
    0xF5,
    0x00,
    0xF7,
    0x00,
    0xF9,
    0x00,
    0xFB,
    0x00,
    0xFD,
    0x00,
    0xFF,
    0x00,
    0x01,
    0x01,
    0x03,
    0x01,
    0x05,
    0x01,
    0x07,
    0x01,
    0x09,
    0x01,
    0x0B,
    0x01,
    0x0D,
    0x01,
    0x0F,
    0x01,
    0x11,
    0x01,
    0x13,
    0x01,
    0x15,
    0x01,
    0x17,
    0x01,
    0x19,
    0x01,
    0x1B,
    0x01,
    0x1D,
    0x01,
    0x1F,
    0x01,
    0x21,
    0x01,
    0x23,
    0x01,
    0x25,
    0x01,
    0x27,
    0x01,
    0x29,
    0x01,
    0x2B,
    0x01,
    0x2D,
    0x01,
    0x2F,
    0x01,
    0x31,
    0x01,
    0x33,
    0x01,
    0x35,
    0x01,
    0x37,
    0x01,
    0x39,
    0x01,
    0x3B,
    0x01,
    0x3D,
    0x01,
    0x3F,
    0x01,
    0x41,
    0x01,
    0x43,
    0x01,
    0x45,
    0x01,
    0x47,
    0x01,
    0x49,
    0x01,
    0x4B,
    0x01,
    0x4D,
    0x01,
    0x4F,
    0x01,
    0x51,
    0x01,
    0x53,
    0x01,
    0x55,
    0x01,
    0x57,
    0x01,
    0x59,
    0x01,
    0x5B,
    0x01,
    0x5D,
    0x01,
    0x5F,
    0x01,
    0x61,
    0x01,
    0x63,
    0x01,
    0x65,
    0x01,
    0x67,
    0x01,
    0x69,
    0x01,
    0x6B,
    0x01,
    0x6D,
    0x01,
    0x6F,
    0x01,
    0x71,
    0x01,
    0x73,
    0x01,
    0x75,
    0x01,
    0x77,
    0x01,
    0x79,
    0x01,
    0x7B,
    0x01,
    0x7D,
    0x01,
    0x7F,
    0x01,
    0x81,
    0x01,
    0x83,
    0x01,
    0x85,
    0x01,
    0x87,
    0x01,
    0x89,
    0x01,
    0x8B,
    0x01,
    0x8D,
    0x01,
    0x8F,
    0x01,
    0x91,
    0x01,
    0x93,
    0x01,
    0x95,
    0x01,
    0x97,
    0x01,
    0x99,
    0x01,
    0x9B,
    0x01,
    0x9D,
    0x01,
    0x9F,
    0x01,
    0xA1,
    0x01,
    0xA3,
    0x01,
    0xA5,
    0x01,
    0xA7,
    0x01,
    0xA9,
    0x01,
    0xAB,
    0x01,
    0xAD,
    0x01,
    0xAF,
    0x01,
    0xB1,
    0x01,
    0xB3,
    0x01,
    0xB5,
    0x01,
    0xB7,
    0x01,
    0x02, 0x01, 0x00, 0x01, 0x02, 0x01, 0x03, 0x01,
    0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03,
    0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01,
    0x03, 0x01, 0x03, 0x01, 0x0B, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01, 0x03,
    0x01, 0x23, 0x01, 0x03, 0x01, 0x23, 0x01, 0x03, 0x01, 0x23, 0x01, 0x12, 0x01, 0x07, 0x01, 0x08, 0x01, 0x09, 0x01,
    0x09, 0x01, 0x0A, 0x01, 0x03, 0x01, 0x0C, 0x01, 0x03, 0x01, 0x1E, 0x01, 0x0D, 0x01, 0x0E, 0x01, 0x03, 0x01, 0x03,
    0x01, 0x12, 0x01, 0x12, 0x01, 0x12, 0x01, 0x12, 0x01, 0x12, 0x01, 0x03, 0x01, 0x23, 0x01, 0x09, 0x01, 0x03, 0x01,
    0x0F, 0x01, 0x09, 0x01, 0x05, 0x01, 0x10, 0x01, 0x11, 0x01, 0x11, 0x01, 0x11, 0x01, 0x03, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x03, 0x01, 0x09, 0x01, 0x08, 0x01, 0x13, 0x01, 0x14, 0x01,
    0x09, 0x01, 0x15, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x16, 0x01, 0x13, 0x01, 0x09, 0x01, 0x17,
    0x01, 0x12, 0x01, 0x24, 0x01, 0x18, 0x01, 0x19, 0x01, 0x13, 0x01, 0x20, 0x01, 0x1B, 0x01, 0x1C, 0x01, 0x1D, 0x01,
    0x03, 0x01, 0x1F, 0x01, 0x20, 0x01, 0x20, 0x01, 0x09, 0x01, 0x21, 0x01, 0x22, 0x01, 0x21, 0x01, 0x09, 0x01, 0x20,
    0x01, 0x03, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
