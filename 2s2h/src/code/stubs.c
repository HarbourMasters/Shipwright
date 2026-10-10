#include <stdio.h>
#include <string.h>
#include "z64.h"
#include <assert.h>
#include "BenPort.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
//#include <math.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH_HIRES 640
#define SCREEN_HEIGHT_HIRES 480

#define HIRES_BUFFER_WIDTH 576
#define HIRES_BUFFER_HEIGHT 454

s32 osResetType;

// AudioContext gAudioContext;
// unk_D_8016E750 D_8016E750[4];
u8 gLetterTLUT[4][32];
u8 gFontFF[999];
DmaEntry dmadata[1568];
// u8 D_80133418;
u16 gAudioSEFlagSwapSource[64];
u16 gAudioSEFlagSwapTarget[64];
u8 gAudioSEFlagSwapMode[64];

s32 osAppNMIBuffer[8];

f32 qNaN0x10000 = 0x7F810000;

u16 gFramebuffer1[SCREEN_HEIGHT][SCREEN_WIDTH]; // at 0x80000500
u16 gFramebuffer0[SCREEN_HEIGHT][SCREEN_WIDTH];

u16 gFramebufferHiRes0[HIRES_BUFFER_WIDTH][HIRES_BUFFER_HEIGHT];
u16 gFramebufferHiRes1[HIRES_BUFFER_WIDTH][HIRES_BUFFER_HEIGHT];

ActiveSequence gActiveSeqs[5];
AudioContext gAudioCtx;
s32 D_801FD120;

// u8 gSoundFontTable[5];
// u8 gSequenceFontTable[5];
// u8 gSequenceTable[5];
// u8 gSampleBankTable[5];

AudioCustomUpdateFunction gAudioCustomUpdateFunction;
AudioCustomSeqFunction gAudioCustomSeqFunction;
AudioCustomReverbFunction gAudioCustomReverbFunction;
AudioCustomSynthFunction gAudioCustomSynthFunction;

u64 aspMainTextStart[100];
u64 aspMainDataStart[100];
u64 aspMainDataEnd[100];

u8 sNumSeqRequests[5];
u32 sAudioSeqCmds[0x100];

u8 sResetAudioHeapTimer;
u16 sResetAudioHeapFadeReverbVolume;
u16 sResetAudioHeapFadeReverbVolumeStep;

u8 D_801D6200[0x400];
SeqRequest sSeqRequests[5][5];

u8 D_80025D00[100];

GfxMasterList D_0E000000;
Mtx D_01000000;
u16 D_0F000000[SCREEN_WIDTH * SCREEN_HEIGHT];

// u64 gspS2DEX_fifoTextStart[1], gspS2DEX_fifoTextEnd[1];
// u64 gspS2DEX_fifoDataStart[1], gspS2DEX_fifoDataEnd[1];
// u64 gspS2DEX_fifo_dTextStart[1], gspS2DEX_fifo_dTextEnd[1];
// u64 gspS2DEX_fifo_dDataStart[1], gspS2DEX_fifo_dDataEnd[1];
// u64 gspS2DEX2_fifoTextStart[1], gspS2DEX2_fifoTextEnd[1];
// u64 gspS2DEX2_fifoDataStart[1], gspS2DEX2_fifoDataEnd[1];
// u64 gspS2DEX2_xbusTextStart[1], gspS2DEX2_xbusTextEnd[1];
// u64 gspS2DEX2_xbusDataStart[1], gspS2DEX2_xbusDataEnd[1];
// u64 gspF3DZEX2_NoN_PosLight_fifoTextStart[1];
// u64 gspF3DZEX2_NoN_PosLight_fifoTextEnd[1];
// u64 gspF3DZEX2_NoN_PosLight_fifoDataStart[1];
// u64 gspF3DZEX2_NoN_PosLight_fifoDataEnd[1];

Vec3f gZeroVec3f = { 0.0f, 0.0f, 0.0f };
Vec3s gZeroVec3s = { 0, 0, 0 };

u64 rspbootTextStart[1];
u64 rspbootTextEnd[1];

u64 njpgdspMainTextStart[1];
u64 njpgdspMainDataStart[1];

u8 gPictoPhotoI8[PICTO_PHOTO_SIZE];
u8 D_80784600[0x56200];

OSPiHandle* __osPiTable;
OSPiHandle* __osCurrentHandle[5];
void* osRomBase;

__osHwInt __osHwIntTable[1];

OSIntMask osSetIntMask(OSIntMask a) {
    return 0;
}

s32 osProbeRumblePak(OSMesgQueue* ctrlrqueue, OSPfs* pfs, u32 channel) {
}

s32 osSetRumble(OSPfs* pfs, u32 vibrate) {
    return 0;
}

u32 __osGetFpcCsr() {
    return 0;
}

u32 __osSetFpcCsr(u32 a0) {
    return 0;
}

OSIntMask __osDisableInt(void) {
}

void __osRestoreInt(OSIntMask a0) {
}

u32 osMemSize = 1024 * 1024 * 1024;

void Audio_osInvalDCache(void* buf, s32 size) {
}

void Audio_osWritebackDCache(void* mem, s32 size) {
}

OSPiHandle* osDriveRomInit() {
}

void osSetUpMempakWrite(s32 channel, OSPifRam* buf) {
}

void osUnmapTLBAll(void) {
}

int ResourceMgr_OTRSigCheck(char* imgData);
char* ResourceMgr_LoadTexOrDListByName(const char* data);

void gSPSegment(void* value, int segNum, uintptr_t target) {
    char* imgData = (char*)target;

    int res = ResourceMgr_OTRSigCheck(imgData);

    // OTRTODO: Disabled for now to fix an issue with HD Textures.
    // With HD textures, we need to pass the path to F3D, not the raw texture data.
    // Otherwise the needed metadata is not available for proper rendering...
    // This should *not* cause any crashes, but some testing may be needed...
    // UPDATE: To maintain compatability it will still do the old behavior if the resource is a display list.
    // That should not affect HD textures.
    if (res) {
        uintptr_t desiredTarget = (uintptr_t)ResourceMgr_LoadIfDListByName(imgData);

        if (desiredTarget != NULL)
            target = desiredTarget;
    }

    __gSPSegment(value, segNum, target);
}

void gSPSegmentLoadRes(void* value, int segNum, uintptr_t target) {
    char* imgData = (char*)target;

    int res = ResourceMgr_OTRSigCheck(imgData);

    if (res) {
        target = (uintptr_t)ResourceMgr_LoadTexOrDListByName(imgData);
    }

    __gSPSegment(value, segNum, target);
}

// void gDPSetTextureImage(Gfx* pkt, u32 format, u32 size, u32 width, uintptr_t i) {
//     __gDPSetTextureImage(pkt, format, size, width, i);
// }

// void gDPSetTextureImageFB(Gfx* pkt, u32 format, u32 size, u32 width, int fb) {
//     __gDPSetTextureImageFB(pkt, format, size, width, fb);
// }

void gSPDisplayList(Gfx* pkt, Gfx* dl) {
    char* imgData = (char*)dl;

    if (ResourceMgr_OTRSigCheck(imgData) == 1) {

        // ResourceMgr_PushCurrentDirectory(imgData);
        // gsSPPushCD(pkt++, imgData);
        dl = ResourceMgr_LoadGfxByName(imgData);
    }

    __gSPDisplayList(pkt, dl);
}

void gDPSetTileSizeInterp(Gfx* pkt, int t, float uls, float ult, float lrs, float lrt) {
    __gDPSetTileSizeInterp(pkt, t, 0, 0, 0, 0);
    pkt->words.w0 = _SHIFTL(G_SETTILESIZE_INTERP, 24, 8);
    pkt++;

    pkt->words.w0 = *(u32*)&uls;
    pkt->words.w1 = *(u32*)&ult;
    pkt++;

    pkt->words.w0 = *(u32*)&lrs;
    pkt->words.w1 = *(u32*)&lrt;
    pkt++;
}

void gDPSetTileSizeLerp(Gfx* pkt, int t, float uls0, float ult0, float lrs0, float lrt0, float uls1, float ult1,
                        float lrs1, float lrt1) {
    __gDPSetTileSizeLerp(pkt, t, uls0, ult0, lrs0, lrt0, uls1, ult1, lrs1, lrt1);
}

void gSPDisplayListOffset(Gfx* pkt, Gfx* dl, int offset) {
    char* imgData = (char*)dl;

    if (ResourceMgr_OTRSigCheck(imgData) == 1)
        dl = ResourceMgr_LoadGfxByName(imgData);

    __gSPDisplayList(pkt, dl + offset);
}

void gSPVertex(Gfx* pkt, uintptr_t v, int n, int v0) {
    if (ResourceMgr_OTRSigCheck((char*)v) == 1)
        v = (uintptr_t)ResourceMgr_LoadVtxByName((char*)v);

    __gSPVertex(pkt, v, n, v0);
}

void gSPInvalidateTexCache(Gfx* pkt, uintptr_t texAddr) {
    char* imgData = (char*)texAddr;

    if (texAddr != 0 && ResourceMgr_OTRSigCheck(imgData)) {
        // Temporary solution to the mq/nonmq issue, this will be
        // handled better with LUS 1.0
        texAddr = (uintptr_t)ResourceMgr_LoadTexOrDListByName(imgData);
    }

    __gSPInvalidateTexCache(pkt, texAddr);
}

void __osDispatchThread(void) {
}

u32 __osGetCause(void) {
}
void __osSetCause(u32 p) {
}
u32 __osGetCompare(void) {
}
void __osSetCompare(u32 value) {
}
u32 __osGetConfig(void) {
}
void __osSetConfig(u32 p) {
}
u32 __osGetSR(void) {
}
void __osSetSR(u32 value) {
}

u32 __osGetWatchLo(void) {
}
void __osSetWatchLo(u32 value) {
}

void __osEnqueueAndYield(OSThread** param_1) {
}
void __osEnqueueThread(OSThread** param_1, OSThread* param_2) {
}
OSThread* __osPopThread(OSThread** param_1) {
}
void osMapTLBRdb(void) {
}
u32 __osProbeTLB(void* param_1) {
}
s32 osAiSetFrequency(u32 frequency) {
    // this is based off the math from the original method
    /*
    s32 osAiSetFrequency(u32 frequency) {
        u8 bitrate;
        f32 dacRateF = ((f32)osViClock / frequency) + 0.5f;
        u32 dacRate = dacRateF;
        if (dacRate < 132) {
            return -1;
        }
        bitrate = (dacRate / 66);
        if (bitrate > 16) {
            bitrate = 16;
        }
        HW_REG(AI_DACRATE_REG, u32) = dacRate - 1;
        HW_REG(AI_BITRATE_REG, u32) = bitrate - 1;
        return osViClock / (s32)dacRate;
    }
    */

    // bitrate is unused

    // osViClock comes from
    // #define VI_NTSC_CLOCK 48681812 /* Hz = 48.681812 MHz */
    // s32 osViClock = VI_NTSC_CLOCK;

    // frequency was originally 32000

    // given all of that, dacRate is
    // (u32)(((f32)48681812 / 32000) + 0.5f)
    // which evaluates to 1521 (which is > 132)

    // this leaves us with a final calculation of
    // 48681812 / 1521
    // which evaluates to 32006

    return 32006;
}
s32 osContStartQuery(OSMesgQueue* mq) {
}
void osCreateThread(OSThread* thread, OSId id, void* entry, void* arg, void* sp, OSPri p) {
}
OSPri osGetThreadPri(OSThread* t) {
}
void osContGetQuery(OSContStatus* data) {
}
void osStartThread(OSThread* thread) {
}
void osViSwapBuffer(void* vaddr) {
}
void osViSetMode(OSViMode* mode) {
}
void osViSetSpecialFeatures(u32 func) {
}
void __osInitialize_common(void) {
}
void __osInitialize_autodetect(void) {
}
void __osExceptionPreamble() {
}
void __osCleanupThread(void) {
}
void osDestroyThread(OSThread* thread) {
}
s32 osContSetCh(u8 ch) {
}
void osViSetYScale(f32 scale) {
}
void osViSetXScale(f32 value) {
}
void osSpTaskYield(void) {
}
void* osViGetCurrentFramebuffer(void) {
}
OSPiHandle* osFlashInit(void) {
}
void osFlashReadId(u32* t, u32* v) {
    // We're faking these so the flashrom system will continue to work
    *t = 0x11118001; // FLASH_TYPE_MAGIC
    *v = 0x00C20000; // FLASH_VERSION_MX_PROTO_A
}
s32 osFlashSectorErase(u32 page) {
}
s32 osFlashWriteBuffer(OSIoMesg* mb, s32 priority, void* dramAddr, OSMesgQueue* mq) {
}
s32 osFlashWriteArray(u32 pageNum) {
}
s32 osFlashReadArray(OSIoMesg* mb, s32 priority, u32 pageNum, void* dramAddr, u32 pageCount, OSMesgQueue* mq) {
}
void osFlashChange(u32 flashNum) {
}
void osFlashAllEraseThrough(void) {
}
void osFlashSectorEraseThrough(u32 pageNum) {
}
s32 osFlashCheckEraseEnd(void) {
}
void osSetThreadPri(OSThread* thread, OSPri p) {
}
void guS2DInitBg(uObjBg* bg) {
    u32 size;
    s32 tmem = (bg->b.imageFmt == G_IM_FMT_CI) ? 0x100 : 0x200;
    u16 shift = (6 - bg->b.imageSiz);

    if (bg->b.imageLoad == G_BGLT_LOADBLOCK) {
        bg->b.tmemW = bg->b.imageW >> shift;
        bg->b.tmemH = (tmem / bg->b.tmemW) * 4;
        bg->b.tmemSizeW = bg->b.tmemW * 2;
        bg->b.tmemSize = bg->b.tmemH * bg->b.tmemSizeW;
        bg->b.tmemLoadSH = (bg->b.tmemSize >> 1) - 1;
        bg->b.tmemLoadTH = (0x7FF / bg->b.tmemW) + 1;
    } else { // G_BGLT_LOADTILE
        bg->b.tmemW = (bg->b.frameW >> shift) + 3;
        bg->b.tmemH = (tmem / bg->b.tmemW) * 4;
        bg->b.tmemSizeW = (bg->b.imageW >> shift) * 2;

        size = bg->b.tmemH * bg->b.tmemSizeW;
        bg->b.tmemSize = (size >> 16);
        bg->b.tmemLoadSH = (size >> 0) & 0xFFFF;
        bg->b.tmemLoadTH = bg->b.tmemH - 1;
    }
}
void* osViGetNextFramebuffer() {
}
OSYieldResult osSpTaskYielded(OSTask* task) {
}
void osViBlack(u8 active) {
}
static s16 sintable[0x400] = {
    0x0000, 0x0032, 0x0064, 0x0096, 0x00C9, 0x00FB, 0x012D, 0x0160, 0x0192, 0x01C4, 0x01F7, 0x0229, 0x025B, 0x028E,
    0x02C0, 0x02F2, 0x0324, 0x0357, 0x0389, 0x03BB, 0x03EE, 0x0420, 0x0452, 0x0484, 0x04B7, 0x04E9, 0x051B, 0x054E,
    0x0580, 0x05B2, 0x05E4, 0x0617, 0x0649, 0x067B, 0x06AD, 0x06E0, 0x0712, 0x0744, 0x0776, 0x07A9, 0x07DB, 0x080D,
    0x083F, 0x0871, 0x08A4, 0x08D6, 0x0908, 0x093A, 0x096C, 0x099F, 0x09D1, 0x0A03, 0x0A35, 0x0A67, 0x0A99, 0x0ACB,
    0x0AFE, 0x0B30, 0x0B62, 0x0B94, 0x0BC6, 0x0BF8, 0x0C2A, 0x0C5C, 0x0C8E, 0x0CC0, 0x0CF2, 0x0D25, 0x0D57, 0x0D89,
    0x0DBB, 0x0DED, 0x0E1F, 0x0E51, 0x0E83, 0x0EB5, 0x0EE7, 0x0F19, 0x0F4B, 0x0F7C, 0x0FAE, 0x0FE0, 0x1012, 0x1044,
    0x1076, 0x10A8, 0x10DA, 0x110C, 0x113E, 0x116F, 0x11A1, 0x11D3, 0x1205, 0x1237, 0x1269, 0x129A, 0x12CC, 0x12FE,
    0x1330, 0x1361, 0x1393, 0x13C5, 0x13F6, 0x1428, 0x145A, 0x148C, 0x14BD, 0x14EF, 0x1520, 0x1552, 0x1584, 0x15B5,
    0x15E7, 0x1618, 0x164A, 0x167B, 0x16AD, 0x16DF, 0x1710, 0x1741, 0x1773, 0x17A4, 0x17D6, 0x1807, 0x1839, 0x186A,
    0x189B, 0x18CD, 0x18FE, 0x1930, 0x1961, 0x1992, 0x19C3, 0x19F5, 0x1A26, 0x1A57, 0x1A88, 0x1ABA, 0x1AEB, 0x1B1C,
    0x1B4D, 0x1B7E, 0x1BAF, 0x1BE1, 0x1C12, 0x1C43, 0x1C74, 0x1CA5, 0x1CD6, 0x1D07, 0x1D38, 0x1D69, 0x1D9A, 0x1DCB,
    0x1DFC, 0x1E2D, 0x1E5D, 0x1E8E, 0x1EBF, 0x1EF0, 0x1F21, 0x1F52, 0x1F82, 0x1FB3, 0x1FE4, 0x2015, 0x2045, 0x2076,
    0x20A7, 0x20D7, 0x2108, 0x2139, 0x2169, 0x219A, 0x21CA, 0x21FB, 0x222B, 0x225C, 0x228C, 0x22BD, 0x22ED, 0x231D,
    0x234E, 0x237E, 0x23AE, 0x23DF, 0x240F, 0x243F, 0x2470, 0x24A0, 0x24D0, 0x2500, 0x2530, 0x2560, 0x2591, 0x25C1,
    0x25F1, 0x2621, 0x2651, 0x2681, 0x26B1, 0x26E1, 0x2711, 0x2740, 0x2770, 0x27A0, 0x27D0, 0x2800, 0x2830, 0x285F,
    0x288F, 0x28BF, 0x28EE, 0x291E, 0x294E, 0x297D, 0x29AD, 0x29DD, 0x2A0C, 0x2A3C, 0x2A6B, 0x2A9B, 0x2ACA, 0x2AF9,
    0x2B29, 0x2B58, 0x2B87, 0x2BB7, 0x2BE6, 0x2C15, 0x2C44, 0x2C74, 0x2CA3, 0x2CD2, 0x2D01, 0x2D30, 0x2D5F, 0x2D8E,
    0x2DBD, 0x2DEC, 0x2E1B, 0x2E4A, 0x2E79, 0x2EA8, 0x2ED7, 0x2F06, 0x2F34, 0x2F63, 0x2F92, 0x2FC0, 0x2FEF, 0x301E,
    0x304C, 0x307B, 0x30A9, 0x30D8, 0x3107, 0x3135, 0x3163, 0x3192, 0x31C0, 0x31EF, 0x321D, 0x324B, 0x3279, 0x32A8,
    0x32D6, 0x3304, 0x3332, 0x3360, 0x338E, 0x33BC, 0x33EA, 0x3418, 0x3446, 0x3474, 0x34A2, 0x34D0, 0x34FE, 0x352B,
    0x3559, 0x3587, 0x35B5, 0x35E2, 0x3610, 0x363D, 0x366B, 0x3698, 0x36C6, 0x36F3, 0x3721, 0x374E, 0x377C, 0x37A9,
    0x37D6, 0x3803, 0x3831, 0x385E, 0x388B, 0x38B8, 0x38E5, 0x3912, 0x393F, 0x396C, 0x3999, 0x39C6, 0x39F3, 0x3A20,
    0x3A4D, 0x3A79, 0x3AA6, 0x3AD3, 0x3B00, 0x3B2C, 0x3B59, 0x3B85, 0x3BB2, 0x3BDE, 0x3C0B, 0x3C37, 0x3C64, 0x3C90,
    0x3CBC, 0x3CE9, 0x3D15, 0x3D41, 0x3D6D, 0x3D99, 0x3DC5, 0x3DF1, 0x3E1D, 0x3E49, 0x3E75, 0x3EA1, 0x3ECD, 0x3EF9,
    0x3F25, 0x3F50, 0x3F7C, 0x3FA8, 0x3FD3, 0x3FFF, 0x402B, 0x4056, 0x4082, 0x40AD, 0x40D8, 0x4104, 0x412F, 0x415A,
    0x4186, 0x41B1, 0x41DC, 0x4207, 0x4232, 0x425D, 0x4288, 0x42B3, 0x42DE, 0x4309, 0x4334, 0x435F, 0x4389, 0x43B4,
    0x43DF, 0x4409, 0x4434, 0x445F, 0x4489, 0x44B4, 0x44DE, 0x4508, 0x4533, 0x455D, 0x4587, 0x45B1, 0x45DC, 0x4606,
    0x4630, 0x465A, 0x4684, 0x46AE, 0x46D8, 0x4702, 0x472C, 0x4755, 0x477F, 0x47A9, 0x47D2, 0x47FC, 0x4826, 0x484F,
    0x4879, 0x48A2, 0x48CC, 0x48F5, 0x491E, 0x4948, 0x4971, 0x499A, 0x49C3, 0x49EC, 0x4A15, 0x4A3E, 0x4A67, 0x4A90,
    0x4AB9, 0x4AE2, 0x4B0B, 0x4B33, 0x4B5C, 0x4B85, 0x4BAD, 0x4BD6, 0x4BFE, 0x4C27, 0x4C4F, 0x4C78, 0x4CA0, 0x4CC8,
    0x4CF0, 0x4D19, 0x4D41, 0x4D69, 0x4D91, 0x4DB9, 0x4DE1, 0x4E09, 0x4E31, 0x4E58, 0x4E80, 0x4EA8, 0x4ED0, 0x4EF7,
    0x4F1F, 0x4F46, 0x4F6E, 0x4F95, 0x4FBD, 0x4FE4, 0x500B, 0x5032, 0x505A, 0x5081, 0x50A8, 0x50CF, 0x50F6, 0x511D,
    0x5144, 0x516B, 0x5191, 0x51B8, 0x51DF, 0x5205, 0x522C, 0x5253, 0x5279, 0x52A0, 0x52C6, 0x52EC, 0x5313, 0x5339,
    0x535F, 0x5385, 0x53AB, 0x53D1, 0x53F7, 0x541D, 0x5443, 0x5469, 0x548F, 0x54B5, 0x54DA, 0x5500, 0x5525, 0x554B,
    0x5571, 0x5596, 0x55BB, 0x55E1, 0x5606, 0x562B, 0x5650, 0x5675, 0x569B, 0x56C0, 0x56E5, 0x5709, 0x572E, 0x5753,
    0x5778, 0x579D, 0x57C1, 0x57E6, 0x580A, 0x582F, 0x5853, 0x5878, 0x589C, 0x58C0, 0x58E5, 0x5909, 0x592D, 0x5951,
    0x5975, 0x5999, 0x59BD, 0x59E1, 0x5A04, 0x5A28, 0x5A4C, 0x5A6F, 0x5A93, 0x5AB7, 0x5ADA, 0x5AFD, 0x5B21, 0x5B44,
    0x5B67, 0x5B8B, 0x5BAE, 0x5BD1, 0x5BF4, 0x5C17, 0x5C3A, 0x5C5D, 0x5C7F, 0x5CA2, 0x5CC5, 0x5CE7, 0x5D0A, 0x5D2D,
    0x5D4F, 0x5D71, 0x5D94, 0x5DB6, 0x5DD8, 0x5DFA, 0x5E1D, 0x5E3F, 0x5E61, 0x5E83, 0x5EA5, 0x5EC6, 0x5EE8, 0x5F0A,
    0x5F2C, 0x5F4D, 0x5F6F, 0x5F90, 0x5FB2, 0x5FD3, 0x5FF4, 0x6016, 0x6037, 0x6058, 0x6079, 0x609A, 0x60BB, 0x60DC,
    0x60FD, 0x611E, 0x613E, 0x615F, 0x6180, 0x61A0, 0x61C1, 0x61E1, 0x6202, 0x6222, 0x6242, 0x6263, 0x6283, 0x62A3,
    0x62C3, 0x62E3, 0x6303, 0x6323, 0x6342, 0x6362, 0x6382, 0x63A1, 0x63C1, 0x63E0, 0x6400, 0x641F, 0x643F, 0x645E,
    0x647D, 0x649C, 0x64BB, 0x64DA, 0x64F9, 0x6518, 0x6537, 0x6556, 0x6574, 0x6593, 0x65B2, 0x65D0, 0x65EF, 0x660D,
    0x662B, 0x664A, 0x6668, 0x6686, 0x66A4, 0x66C2, 0x66E0, 0x66FE, 0x671C, 0x673A, 0x6757, 0x6775, 0x6792, 0x67B0,
    0x67CD, 0x67EB, 0x6808, 0x6825, 0x6843, 0x6860, 0x687D, 0x689A, 0x68B7, 0x68D4, 0x68F1, 0x690D, 0x692A, 0x6947,
    0x6963, 0x6980, 0x699C, 0x69B9, 0x69D5, 0x69F1, 0x6A0E, 0x6A2A, 0x6A46, 0x6A62, 0x6A7E, 0x6A9A, 0x6AB5, 0x6AD1,
    0x6AED, 0x6B08, 0x6B24, 0x6B40, 0x6B5B, 0x6B76, 0x6B92, 0x6BAD, 0x6BC8, 0x6BE3, 0x6BFE, 0x6C19, 0x6C34, 0x6C4F,
    0x6C6A, 0x6C84, 0x6C9F, 0x6CBA, 0x6CD4, 0x6CEF, 0x6D09, 0x6D23, 0x6D3E, 0x6D58, 0x6D72, 0x6D8C, 0x6DA6, 0x6DC0,
    0x6DDA, 0x6DF3, 0x6E0D, 0x6E27, 0x6E40, 0x6E5A, 0x6E73, 0x6E8D, 0x6EA6, 0x6EBF, 0x6ED9, 0x6EF2, 0x6F0B, 0x6F24,
    0x6F3D, 0x6F55, 0x6F6E, 0x6F87, 0x6FA0, 0x6FB8, 0x6FD1, 0x6FE9, 0x7002, 0x701A, 0x7032, 0x704A, 0x7062, 0x707A,
    0x7092, 0x70AA, 0x70C2, 0x70DA, 0x70F2, 0x7109, 0x7121, 0x7138, 0x7150, 0x7167, 0x717E, 0x7196, 0x71AD, 0x71C4,
    0x71DB, 0x71F2, 0x7209, 0x7220, 0x7236, 0x724D, 0x7264, 0x727A, 0x7291, 0x72A7, 0x72BD, 0x72D4, 0x72EA, 0x7300,
    0x7316, 0x732C, 0x7342, 0x7358, 0x736E, 0x7383, 0x7399, 0x73AE, 0x73C4, 0x73D9, 0x73EF, 0x7404, 0x7419, 0x742E,
    0x7443, 0x7458, 0x746D, 0x7482, 0x7497, 0x74AC, 0x74C0, 0x74D5, 0x74EA, 0x74FE, 0x7512, 0x7527, 0x753B, 0x754F,
    0x7563, 0x7577, 0x758B, 0x759F, 0x75B3, 0x75C7, 0x75DA, 0x75EE, 0x7601, 0x7615, 0x7628, 0x763B, 0x764F, 0x7662,
    0x7675, 0x7688, 0x769B, 0x76AE, 0x76C1, 0x76D3, 0x76E6, 0x76F9, 0x770B, 0x771E, 0x7730, 0x7742, 0x7754, 0x7767,
    0x7779, 0x778B, 0x779D, 0x77AF, 0x77C0, 0x77D2, 0x77E4, 0x77F5, 0x7807, 0x7818, 0x782A, 0x783B, 0x784C, 0x785D,
    0x786E, 0x787F, 0x7890, 0x78A1, 0x78B2, 0x78C3, 0x78D3, 0x78E4, 0x78F4, 0x7905, 0x7915, 0x7925, 0x7936, 0x7946,
    0x7956, 0x7966, 0x7976, 0x7985, 0x7995, 0x79A5, 0x79B5, 0x79C4, 0x79D4, 0x79E3, 0x79F2, 0x7A02, 0x7A11, 0x7A20,
    0x7A2F, 0x7A3E, 0x7A4D, 0x7A5B, 0x7A6A, 0x7A79, 0x7A87, 0x7A96, 0x7AA4, 0x7AB3, 0x7AC1, 0x7ACF, 0x7ADD, 0x7AEB,
    0x7AF9, 0x7B07, 0x7B15, 0x7B23, 0x7B31, 0x7B3E, 0x7B4C, 0x7B59, 0x7B67, 0x7B74, 0x7B81, 0x7B8E, 0x7B9B, 0x7BA8,
    0x7BB5, 0x7BC2, 0x7BCF, 0x7BDC, 0x7BE8, 0x7BF5, 0x7C02, 0x7C0E, 0x7C1A, 0x7C27, 0x7C33, 0x7C3F, 0x7C4B, 0x7C57,
    0x7C63, 0x7C6F, 0x7C7A, 0x7C86, 0x7C92, 0x7C9D, 0x7CA9, 0x7CB4, 0x7CBF, 0x7CCB, 0x7CD6, 0x7CE1, 0x7CEC, 0x7CF7,
    0x7D02, 0x7D0C, 0x7D17, 0x7D22, 0x7D2C, 0x7D37, 0x7D41, 0x7D4B, 0x7D56, 0x7D60, 0x7D6A, 0x7D74, 0x7D7E, 0x7D88,
    0x7D91, 0x7D9B, 0x7DA5, 0x7DAE, 0x7DB8, 0x7DC1, 0x7DCB, 0x7DD4, 0x7DDD, 0x7DE6, 0x7DEF, 0x7DF8, 0x7E01, 0x7E0A,
    0x7E13, 0x7E1B, 0x7E24, 0x7E2C, 0x7E35, 0x7E3D, 0x7E45, 0x7E4D, 0x7E56, 0x7E5E, 0x7E66, 0x7E6D, 0x7E75, 0x7E7D,
    0x7E85, 0x7E8C, 0x7E94, 0x7E9B, 0x7EA3, 0x7EAA, 0x7EB1, 0x7EB8, 0x7EBF, 0x7EC6, 0x7ECD, 0x7ED4, 0x7EDB, 0x7EE1,
    0x7EE8, 0x7EEE, 0x7EF5, 0x7EFB, 0x7F01, 0x7F08, 0x7F0E, 0x7F14, 0x7F1A, 0x7F20, 0x7F25, 0x7F2B, 0x7F31, 0x7F36,
    0x7F3C, 0x7F41, 0x7F47, 0x7F4C, 0x7F51, 0x7F56, 0x7F5B, 0x7F60, 0x7F65, 0x7F6A, 0x7F6F, 0x7F74, 0x7F78, 0x7F7D,
    0x7F81, 0x7F85, 0x7F8A, 0x7F8E, 0x7F92, 0x7F96, 0x7F9A, 0x7F9E, 0x7FA2, 0x7FA6, 0x7FA9, 0x7FAD, 0x7FB0, 0x7FB4,
    0x7FB7, 0x7FBA, 0x7FBE, 0x7FC1, 0x7FC4, 0x7FC7, 0x7FCA, 0x7FCC, 0x7FCF, 0x7FD2, 0x7FD4, 0x7FD7, 0x7FD9, 0x7FDC,
    0x7FDE, 0x7FE0, 0x7FE2, 0x7FE4, 0x7FE6, 0x7FE8, 0x7FEA, 0x7FEC, 0x7FED, 0x7FEF, 0x7FF1, 0x7FF2, 0x7FF3, 0x7FF5,
    0x7FF6, 0x7FF7, 0x7FF8, 0x7FF9, 0x7FFA, 0x7FFB, 0x7FFB, 0x7FFC, 0x7FFD, 0x7FFD, 0x7FFE, 0x7FFE, 0x7FFE, 0x7FFE,
    0x7FFE, 0x7FFF,
};
s16 sins(u16 x) {
    s16 value;

    x >>= 4;

    if (x & 0x400) {
        value = sintable[0x3FF - (x & 0x3FF)];
    } else {
        value = sintable[x & 0x3FF];
    }

    if (x & 0x800) {
        return -value;
    } else {
        return value;
    }
}
s16 coss(u16 angle) {
    return sins(angle + 0x4000);
}
#define FTOFRAC8(x) ((s32)MIN(((x) * (128.0f)), 127.0f) & 0xff)

/**
 * guLookAtHiliteF
 * This function creates the viewing matrix (floating point) and sets the LookAt/Hilite structures
 **/
void guLookAtHiliteF(f32 mf[4][4], LookAt* l, Hilite* h, f32 xEye, f32 yEye, f32 zEye, f32 xAt, f32 yAt, f32 zAt,
                     f32 xUp, f32 yUp, f32 zUp, f32 xl1, f32 yl1, f32 zl1, /* light 1 direction */
                     f32 xl2, f32 yl2, f32 zl2,                            /* light 2 direction */
                     s32 hiliteWidth, s32 hiliteHeight)                    /* size of hilite texture */
{
    f32 length;
    f32 xLook;
    f32 yLook;
    f32 zLook;
    f32 xRight;
    f32 yRight;
    f32 zRight;
    f32 xHilite;
    f32 yHilite;
    f32 zHilite;

    guMtxIdentF(mf);

    xLook = xAt - xEye;
    yLook = yAt - yEye;
    zLook = zAt - zEye;
    length = -1.0 / sqrtf(xLook * xLook + yLook * yLook + zLook * zLook);
    xLook *= length;
    yLook *= length;
    zLook *= length;

    xRight = yUp * zLook - zUp * yLook;
    yRight = zUp * xLook - xUp * zLook;
    zRight = xUp * yLook - yUp * xLook;
    length = 1.0 / sqrtf(xRight * xRight + yRight * yRight + zRight * zRight);
    xRight *= length;
    yRight *= length;
    zRight *= length;

    xUp = yLook * zRight - zLook * yRight;
    yUp = zLook * xRight - xLook * zRight;
    zUp = xLook * yRight - yLook * xRight;
    length = 1.0 / sqrtf(xUp * xUp + yUp * yUp + zUp * zUp);
    xUp *= length;
    yUp *= length;
    zUp *= length;

    /* hilite vectors */

    length = 1.0 / sqrtf(xl1 * xl1 + yl1 * yl1 + zl1 * zl1);
    xl1 *= length;
    yl1 *= length;
    zl1 *= length;

    xHilite = xl1 + xLook;
    yHilite = yl1 + yLook;
    zHilite = zl1 + zLook;

    length = sqrtf(xHilite * xHilite + yHilite * yHilite + zHilite * zHilite);

    if (length > 0.1) {
        length = 1.0 / length;
        xHilite *= length;
        yHilite *= length;
        zHilite *= length;

        h->h.x1 = hiliteWidth * 4 + (xHilite * xRight + yHilite * yRight + zHilite * zRight) * hiliteWidth * 2;

        h->h.y1 = hiliteHeight * 4 + (xHilite * xUp + yHilite * yUp + zHilite * zUp) * hiliteHeight * 2;
    } else {
        h->h.x1 = hiliteWidth * 2;
        h->h.y1 = hiliteHeight * 2;
    }

    length = 1.0 / sqrtf(xl2 * xl2 + yl2 * yl2 + zl2 * zl2);
    xl2 *= length;
    yl2 *= length;
    zl2 *= length;

    xHilite = xl2 + xLook;
    yHilite = yl2 + yLook;
    zHilite = zl2 + zLook;
    length = sqrtf(xHilite * xHilite + yHilite * yHilite + zHilite * zHilite);
    if (length > 0.1) {
        length = 1.0 / length;
        xHilite *= length;
        yHilite *= length;
        zHilite *= length;

        h->h.x2 = hiliteWidth * 4 + (xHilite * xRight + yHilite * yRight + zHilite * zRight) * hiliteWidth * 2;

        h->h.y2 = hiliteHeight * 4 + (xHilite * xUp + yHilite * yUp + zHilite * zUp) * hiliteHeight * 2;
    } else {
        h->h.x2 = hiliteWidth * 2;
        h->h.y2 = hiliteHeight * 2;
    }

    /* reflectance vectors = Up and Right */

    l->l[0].l.dir[0] = FTOFRAC8(xRight);
    l->l[0].l.dir[1] = FTOFRAC8(yRight);
    l->l[0].l.dir[2] = FTOFRAC8(zRight);
    l->l[1].l.dir[0] = FTOFRAC8(xUp);
    l->l[1].l.dir[1] = FTOFRAC8(yUp);
    l->l[1].l.dir[2] = FTOFRAC8(zUp);
    l->l[0].l.col[0] = 0x00;
    l->l[0].l.col[1] = 0x00;
    l->l[0].l.col[2] = 0x00;
    l->l[0].l.pad1 = 0x00;
    l->l[0].l.colc[0] = 0x00;
    l->l[0].l.colc[1] = 0x00;
    l->l[0].l.colc[2] = 0x00;
    l->l[0].l.pad2 = 0x00;
    l->l[1].l.col[0] = 0x00;
    l->l[1].l.col[1] = 0x80;
    l->l[1].l.col[2] = 0x00;
    l->l[1].l.pad1 = 0x00;
    l->l[1].l.colc[0] = 0x00;
    l->l[1].l.colc[1] = 0x80;
    l->l[1].l.colc[2] = 0x00;
    l->l[1].l.pad2 = 0x00;

    mf[0][0] = xRight;
    mf[1][0] = yRight;
    mf[2][0] = zRight;
    mf[3][0] = -(xEye * xRight + yEye * yRight + zEye * zRight);

    mf[0][1] = xUp;
    mf[1][1] = yUp;
    mf[2][1] = zUp;
    mf[3][1] = -(xEye * xUp + yEye * yUp + zEye * zUp);

    mf[0][2] = xLook;
    mf[1][2] = yLook;
    mf[2][2] = zLook;
    mf[3][2] = -(xEye * xLook + yEye * yLook + zEye * zLook);

    mf[0][3] = 0;
    mf[1][3] = 0;
    mf[2][3] = 0;
    mf[3][3] = 1;
}

/**
 * guLookAtHilite
 * This function creates the viewing matrix (fixed point) and sets the LookAt/Hilite structures
 * Same args as previous function
 **/
void guLookAtHilite(Mtx* m, LookAt* l, Hilite* h, f32 xEye, f32 yEye, f32 zEye, f32 xAt, f32 yAt, f32 zAt, f32 xUp,
                    f32 yUp, f32 zUp, f32 xl1, f32 yl1, f32 zl1, f32 xl2, f32 yl2, f32 zl2, s32 hiliteWidth,
                    s32 hiliteHeight) {
    f32 mf[4][4];

    guLookAtHiliteF(mf, l, h, xEye, yEye, zEye, xAt, yAt, zAt, xUp, yUp, zUp, xl1, yl1, zl1, xl2, yl2, zl2, hiliteWidth,
                    hiliteHeight);

    guMtxF2L((MtxF*)mf, m);
}
void guOrthoF(float m[4][4], float l, float r, float b, float t, float n, float f, float scale) {
    int i;
    int j;
    guMtxIdentF(m);
    m[0][0] = 2 / (r - l);
    m[1][1] = 2 / (t - b);
    m[2][2] = -2 / (f - n);
    m[3][0] = -(r + l) / (r - l);
    m[3][1] = -(t + b) / (t - b);
    m[3][2] = -(f + n) / (f - n);
    m[3][3] = 1;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            m[i][j] *= scale;
        }
    }
}

void guOrtho(Mtx* m, float l, float r, float b, float t, float n, float f, float scale) {
    float mf[4][4];
    guOrthoF(mf, l, r, b, t, n, f, scale);
    FrameInterpolation_RecordOpenChild("ortho", 0);
    Matrix_MtxFToMtx((MtxF*)mf, m);
    FrameInterpolation_RecordCloseChild();
    // guMtxF2L(mf, m);
}

#define GU_PI 3.1415926

void guPerspectiveF(f32 mf[4][4], u16* perspNorm, f32 fovy, f32 aspect, f32 near, f32 far, f32 scale) {
    f32 yscale;
    s32 row;
    s32 col;

    guMtxIdentF(mf);

    fovy *= GU_PI / 180.0;
    yscale = cosf(fovy / 2) / sinf(fovy / 2);
    mf[0][0] = yscale / aspect;
    mf[1][1] = yscale;
    mf[2][2] = (near + far) / (near - far);
    mf[2][3] = -1;
    mf[3][2] = 2 * near * far / (near - far);
    mf[3][3] = 0.0f;

    for (row = 0; row < 4; row++) {
        for (col = 0; col < 4; col++) {
            mf[row][col] *= scale;
        }
    }

    if (perspNorm != NULL) {
        if (near + far <= 2.0) {
            *perspNorm = 65535;
        } else {
            *perspNorm = (f64)(1 << 17) / (near + far);
            if (*perspNorm <= 0) {
                *perspNorm = 1;
            }
        }
    }
}

void guPerspective(Mtx* m, u16* perspNorm, float fovy, float aspect, float near, float far, float scale) {
    float mf[4][4];

    guPerspectiveF(mf, perspNorm, fovy, aspect, near, far, scale);
    Matrix_MtxFToMtx((MtxF*)mf, m);
    // guPerspectiveF(mf, perspNorm, fovy, aspect, near, far, scale);
    // guMtxF2L(mf, m);
}
#include "global.h"

void guLookAtF(f32 mf[4][4], f32 xEye, f32 yEye, f32 zEye, f32 xAt, f32 yAt, f32 zAt, f32 xUp, f32 yUp, f32 zUp) {
    f32 length;
    f32 xLook;
    f32 yLook;
    f32 zLook;
    f32 xRight;
    f32 yRight;
    f32 zRight;

    guMtxIdentF(mf);

    xLook = xAt - xEye;
    yLook = yAt - yEye;
    zLook = zAt - zEye;
    length = -1.0 / sqrtf(SQ(xLook) + SQ(yLook) + SQ(zLook));
    xLook *= length;
    yLook *= length;
    zLook *= length;

    xRight = yUp * zLook - zUp * yLook;
    yRight = zUp * xLook - xUp * zLook;
    zRight = xUp * yLook - yUp * xLook;
    length = 1.0 / sqrtf(SQ(xRight) + SQ(yRight) + SQ(zRight));
    xRight *= length;
    yRight *= length;
    zRight *= length;

    xUp = yLook * zRight - zLook * yRight;
    yUp = zLook * xRight - xLook * zRight;
    zUp = xLook * yRight - yLook * xRight;
    length = 1.0 / sqrtf(SQ(xUp) + SQ(yUp) + SQ(zUp));
    xUp *= length;
    yUp *= length;
    zUp *= length;

    mf[0][0] = xRight;
    mf[1][0] = yRight;
    mf[2][0] = zRight;
    mf[3][0] = -(xEye * xRight + yEye * yRight + zEye * zRight);

    mf[0][1] = xUp;
    mf[1][1] = yUp;
    mf[2][1] = zUp;
    mf[3][1] = -(xEye * xUp + yEye * yUp + zEye * zUp);

    mf[0][2] = xLook;
    mf[1][2] = yLook;
    mf[2][2] = zLook;
    mf[3][2] = -(xEye * xLook + yEye * yLook + zEye * zLook);

    mf[0][3] = 0;
    mf[1][3] = 0;
    mf[2][3] = 0;
    mf[3][3] = 1;
}

void guLookAt(Mtx* m, f32 xEye, f32 yEye, f32 zEye, f32 xAt, f32 yAt, f32 zAt, f32 xUp, f32 yUp, f32 zUp) {
    f32 mf[4][4];

    guLookAtF(mf, xEye, yEye, zEye, xAt, yAt, zAt, xUp, yUp, zUp);

    Matrix_MtxFToMtx((MtxF*)mf, m);
    // guMtxF2L(mf, m);
}
void guRotateF(float m[4][4], float a, float x, float y, float z) {
    static float D_80097F90 = M_PIf / 180.0f;
    float sine;
    float cosine;
    float ab;
    float bc;
    float ca;
    float t;
    float xs;
    float ys;
    float zs;

    guNormalize(&x, &y, &z);

    a = a * D_80097F90;

    sine = sinf(a);
    cosine = cosf(a);

    ab = x * y * (1 - cosine);
    bc = y * z * (1 - cosine);
    ca = z * x * (1 - cosine);

    guMtxIdentF(m);

    xs = x * sine;
    ys = y * sine;
    zs = z * sine;

    t = x * x;
    m[0][0] = (1 - t) * cosine + t;
    m[2][1] = bc - xs;
    m[1][2] = bc + xs;
    t = y * y;
    m[1][1] = (1 - t) * cosine + t;
    m[2][0] = ca + ys;
    m[0][2] = ca - ys;
    t = z * z;
    m[2][2] = (1 - t) * cosine + t;
    m[1][0] = ab - zs;
    m[0][1] = ab + zs;
}

void guRotate(Mtx* m, float a, float x, float y, float z) {
    float mf[4][4];
    guRotateF(mf, a, x, y, z);
    guMtxF2L(mf, m);
}

/*
void guNormalize(f32* x, f32* y, f32* z) {
    f32 tmp = 1.0f / sqrtf(*x * *x + *y * *y + *z * *z);
    *x = *x * tmp;
    *y = *y * tmp;
    *z = *z * tmp;
}
*/

void osSpTaskLoad(OSTask* task) {
}
void osSpTaskStartGo(OSTask* task) {
}
void osViExtendVStart(u32 arg0) {
}
