#ifndef AUDIO_LOAD_H
#define AUDIO_LOAD_H

#include "audio/soundfont.h"
#include "PR/ultratypes.h"
#include "libc/stddef.h"
#include "libc/stdint.h"
#include "PR/os_message.h"
#include "unk.h"
#include "PR/os.h"

struct Sample;

typedef s32 (*DmaHandler)(OSPiHandle* handle, OSIoMesg* mb, s32 direction);

typedef enum {
    /* 0 */ SEQUENCE_TABLE,
    /* 1 */ FONT_TABLE,
    /* 2 */ SAMPLE_TABLE
} SampleBankTableType;

typedef struct {
    /* 0x0 */ uintptr_t romAddr;
    /* 0x4 */ size_t size;
    /* 0x8 */ s8 medium;
    /* 0x9 */ s8 cachePolicy;
    /* 0xA */ s16 shortData1;
    /* 0xC */ s16 shortData2;
    /* 0xE */ s16 shortData3;
} AudioTableEntry; // size = 0x10

typedef struct {
    /* 0x00 */ s16 numEntries;
    /* 0x02 */ s16 unkMediumParam;
    /* 0x04 */ uintptr_t romAddr;
    /* 0x08 */ char pad[0x8];
    /* 0x10 */ AudioTableEntry entries[1]; // (dynamic size)
} AudioTable;                              // size >= 0x20

typedef enum {
    /* 0 */ LOAD_STATUS_NOT_LOADED,
    /* 1 */ LOAD_STATUS_IN_PROGRESS,
    /* 2 */ LOAD_STATUS_COMPLETE,
    /* 3 */ LOAD_STATUS_DISCARDABLE,
    /* 4 */ LOAD_STATUS_MAYBE_DISCARDABLE,
    /* 5 */ LOAD_STATUS_PERMANENT
} AudioLoadStatus;

typedef enum {
    /* 0 */ CACHE_TEMPORARY,
    /* 1 */ CACHE_PERSISTENT,
    /* 2 */ CACHE_EITHER,
    /* 3 */ CACHE_PERMANENT
} AudioCacheType;

typedef enum {
    /* 0 */ CACHE_LOAD_PERMANENT,
    /* 1 */ CACHE_LOAD_PERSISTENT,
    /* 2 */ CACHE_LOAD_TEMPORARY,
    /* 3 */ CACHE_LOAD_EITHER,
    /* 4 */ CACHE_LOAD_EITHER_NOSYNC
} AudioCacheLoadType;

typedef struct {
    /* 0x00 */ s8 status;
    /* 0x01 */ s8 delay;
    /* 0x02 */ s8 medium;
    /* 0x04 */ u8* ramAddr;
    /* 0x08 */ uintptr_t curDevAddr;
    /* 0x0C */ u8* curRamAddr;
    /* 0x10 */ size_t bytesRemaining;
    /* 0x14 */ size_t chunkSize;
    /* 0x18 */ s32 unkMediumParam;
    /* 0x1C */ u32 retMsg;
    /* 0x20 */ OSMesgQueue* retQueue;
    /* 0x24 */ OSMesgQueue msgQueue;
    /* 0x3C */ OSMesg msg;
    /* 0x40 */ OSIoMesg ioMesg;
} AudioAsyncLoad; // size = 0x58

typedef struct {
    /* 0x00 */ u8 medium;
    /* 0x01 */ u8 seqOrFontId;
    /* 0x02 */ u16 instId;
    /* 0x04 */ s32 unkMediumParam;
    /* 0x08 */ uintptr_t curDevAddr;
    /* 0x0C */ u8* curRamAddr;
    /* 0x10 */ u8* ramAddr;
    /* 0x14 */ s32 status;
    /* 0x18 */ size_t bytesRemaining;
    /* 0x1C */ s8* isDone; // TODO: rename in OoT and sync up here. This is an external status while (s32 status) is an
                           // internal status
    /* 0x20 */ struct Sample sample;
    /* 0x30 */ OSMesgQueue msgqueue;
    /* 0x48 */ OSMesg msg;
    /* 0x4C */ OSIoMesg ioMesg;
} AudioSlowLoad; // size = 0x64

typedef struct {
    /* 0x0 */ u8* ramAddr;
    /* 0x4 */ uintptr_t devAddr;
    /* 0x8 */ u16 sizeUnused;
    /* 0xA */ u16 size;
    /* 0xC */ u8 unused;
    /* 0xD */ u8 reuseIndex; // position in sSampleDmaReuseQueue1/2, if ttl == 0
    /* 0xE */ u8 ttl;        // Time To Live: duration after which the DMA can be discarded
} SampleDma;                 // size = 0x10

typedef struct {
    /* 0x00 */ u32 endAndMediumKey;
    /* 0x04 */ struct Sample* sample;
    /* 0x08 */ u8* ramAddr;
    /* 0x0C */ u32 encodedInfo;
    /* 0x10 */ s32 isFree;
} AudioPreloadReq; // size = 0x14

void AudioLoad_DecreaseSampleDmaTtls(void);
void* AudioLoad_DmaSampleData(uintptr_t devAddr, size_t size, s32 arg2, u8* dmaIndexRef, s32 medium);
void AudioLoad_InitSampleDmaBuffers(s32 numNotes);
s32 AudioLoad_IsFontLoadComplete(s32 fontId);
s32 AudioLoad_IsSeqLoadComplete(s32 seqId);
void AudioLoad_SetFontLoadStatus(s32 fontId, s32 loadStatus);
void AudioLoad_SetSeqLoadStatus(s32 seqId, s32 loadStatus);
void AudioLoad_SyncLoadSeqParts(s32 seqId, s32 arg1, s32 arg2, OSMesgQueue* arg3);
s32 AudioLoad_SyncLoadInstrument(s32 fontId, s32 instId, s32 drumId);
void AudioLoad_AsyncLoadSeq(s32 seqId, s32 arg1, s32 retData, OSMesgQueue* retQueue);
void AudioLoad_AsyncLoadSampleBank(s32 sampleBankId, s32 arg1, s32 retData, OSMesgQueue* retQueue);
void AudioLoad_AsyncLoadFont(s32 fontId, s32 arg1, s32 retData, OSMesgQueue* retQueue);
u16* AudioLoad_GetFontsForSequence(s32 seqId, u32* outNumFonts, u16* buff);
void AudioLoad_DiscardSeqFonts(s32 seqId);
void func_8018FA60(u32 tableType, u32 id, s32 type, s32 data);
s32 AudioLoad_SyncInitSeqPlayer(s32 playerIndex, s32 seqId, s32 arg2);
s32 AudioLoad_SyncInitSeqPlayerSkipTicks(s32 playerIndex, s32 seqId, s32 skipTicks);
void AudioLoad_ProcessLoads(s32 resetStatus);
void AudioLoad_SetDmaHandler(DmaHandler callback);
void AudioLoad_Init(void* heap, size_t heapSize);
void AudioLoad_InitSlowLoads(void);
s32 AudioLoad_SlowLoadSample(s32 fontId, s32 instId, s8* isDone);
s32 AudioLoad_SlowLoadSeq(s32 seqId, u8* ramAddr, s8* isDone);
void AudioLoad_InitAsyncLoads(void);
void AudioLoad_LoadPermanentSamples(void);
void AudioLoad_ScriptLoad(s32 tableType, s32 id, s8* isDone);
void AudioLoad_ProcessScriptLoads(void);
void AudioLoad_InitScriptLoads(void);

#endif
