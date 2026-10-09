#ifndef Z64JPEG_H
#define Z64JPEG_H

#include "PR/ultratypes.h"
#include "PR/os_message.h"
#include "PR/os_sched.h"

typedef struct {
    /* 0x00 */ u16 table[8*8];
} JpegQuantizationTable; // size = 0x80

typedef struct {
    /* 0x00 */ u8 codeOffs[16];
    /* 0x10 */ u16 codesA[16];
    /* 0x30 */ u16 codesB[16];
    /* 0x50 */ u8* symbols;
} JpegHuffmanTable; // size = 0x54

// this struct might be inaccurate but it's not used outside jpegutils.c
typedef struct {
    /* 0x000 */ u8 codeOffs[16];
    /* 0x010 */ u16 dcCodes[120];
    /* 0x100 */ u16 acCodes[256];
} JpegHuffmanTableOld; // size = 0x300

typedef struct {
    /* 0x00 */ void* address;
    /* 0x04 */ u32 mbCount;
    /* 0x08 */ u32 mode;
    /* 0x0C */ void* qTableYPtr;
    /* 0x10 */ void* qTableUPtr;
    /* 0x14 */ void* qTableVPtr;
    /* 0x18 */ char unk_18[0x8];
} JpegTaskData; // size = 0x20

typedef struct {
    /* 0x000 */ JpegTaskData taskData;
    /* 0x020 */ char yieldData[0x200];
    /* 0x220 */ JpegQuantizationTable qTableY;
    /* 0x2A0 */ JpegQuantizationTable qTableU;
    /* 0x320 */ JpegQuantizationTable qTableV;
    /* 0x3A0 */ u8 codesLengths[0x110];
    /* 0x4B0 */ u16 codes[0x108];
    /* 0x6C0 */ u16 data[4][0x180];
} JpegWork; // size = 0x12C0

typedef struct {
    /* 0x00 */ void* imageData;
    /* 0x04 */ u8 mode;
    /* 0x05 */ u8 unk_05;
    /* 0x08 */ JpegHuffmanTable* hTablePtrs[4];
    /* 0x18 */ u8 unk_18;
} JpegDecoder; // size = 0x1C

typedef struct {
    /* 0x00 */ u8 dqtCount;
    /* 0x04 */ u8* dqtPtr[3];
    /* 0x10 */ u8 dhtCount;
    /* 0x14 */ u8* dhtPtr[4];
    /* 0x24 */ void* imageData;
    /* 0x28 */ u8 mode; // 0 if Y V0 is 1 and 2 if Y V0 is 2
    /* 0x30 */ OSScTask scTask;
    /* 0x98 */ OSMesgQueue mq;
    /* 0xB0 */ OSMesg msg;
    /* 0xB4 */ JpegWork* workBuf;
} JpegContext; // size = 0xB8

typedef struct {
    /* 0x00 */ u32 byteIdx;
    /* 0x04 */ u8 bitIdx;
    /* 0x05 */ u8 dontSkip;
    /* 0x08 */ u32 curWord;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ s16 unk_10;
} JpegDecoderState; // size = 0x14


void Jpeg_ScheduleDecoderTask(JpegContext* jpegCtx);
void Jpeg_CopyToZbuffer(u16* src, u16* zbuffer, s32 x, s32 y);
u16 Jpeg_GetUnalignedU16(u8* ptr);
void Jpeg_ParseMarkers(u8* ptr, JpegContext* jpegCtx);
s32 Jpeg_Decode(void* data, void* zbuffer, void* work, u32 workSize);

void JpegUtils_ProcessQuantizationTable(u8* dqt, JpegQuantizationTable* qt, u8 count);
s32 JpegUtils_ParseHuffmanCodesLengths(u8* ptr, u8* codesLengths);
s32 JpegUtils_GetHuffmanCodes(u8* codesLengths, u16* codes);
s32 JpegUtils_SetHuffmanTable(u8* data, JpegHuffmanTable* ht, u16* codes);
u32 JpegUtils_ProcessHuffmanTableImpl(u8* data, JpegHuffmanTable* ht, u8* codesLengths, u16* codes, u8 isAc);
u32 JpegUtils_ProcessHuffmanTable(u8* dht, JpegHuffmanTable* ht, u8* codesLengths, u16* codes, u8 count);
void JpegUtils_SetHuffmanTableOld(u8* data, JpegHuffmanTableOld* ht, u8* codesLengths, u16* codes, s16 count, u8 isAc);
u32 JpegUtils_ProcessHuffmanTableImplOld(u8* dht, JpegHuffmanTableOld* ht, u8* codesLengths, u16* codes);

s32 JpegDecoder_Decode(JpegDecoder* decoder, u16* mcuBuff, s32 count, u8 isFollowing, JpegDecoderState* state);
s32 JpegDecoder_ProcessMcu(JpegHuffmanTable* hTable0, JpegHuffmanTable* hTable1, u16* mcu, s16* unk);
s32 JpegDecoder_ParseNextSymbol(JpegHuffmanTable* hTable, s16* outCoeff, s8* outZeroCount);
u16 JpegDecoder_ReadBits(u8 len);

#endif
