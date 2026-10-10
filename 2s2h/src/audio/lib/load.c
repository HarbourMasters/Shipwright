/**
 * @file audio_load.c
 *
 * Implements the transfer of audio data from the cartridge to the audio heap.
 *
 * The audio data in ROM consists of:
 *      1) Sequences stored as a midi-based script called the music-macro language (.mus files)
 *      2) SoundFonts stored as various sound-font structs bundled togther in a custom way
 *         that is extracted through relocs (See SoundFontData, generic .o files)
 *      3) Samples consisting of raw soundbytes stored using a compressed ADPCM algorithm (.aifc files)
 */

#include "global.h"
#include "buffers.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "2s2h/Enhancements/Audio/AudioCollection.h"
#include "2s2h/Enhancements/Audio/AudioEditor.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "BenPort.h"
#include <libultraship/log/luslog.h>
// Windows deprecated the use of `strdup` it uses _strdup. Linux/Unix doesn't have _strdup.
#ifdef _MSC_VER
#define strdup _strdup
#endif

/**
 * SoundFont Notes:
 *
 */
// opaque type for soundfont data loaded into ram (should maybe get rid of this?)
typedef void SoundFontData;

typedef struct {
    /* 0x00 */ s32 sampleBankId1;
    /* 0x04 */ s32 sampleBankId2;
    /* 0x08 */ uintptr_t baseAddr1;
    /* 0x0C */ uintptr_t baseAddr2;
    /* 0x10 */ u32 medium1;
    /* 0x14 */ u32 medium2;
} SampleBankRelocInfo; // size = 0x18

void AudioLoad_DiscardFont(s32 fontId);
s32 AudioLoad_SyncInitSeqPlayerInternal(s32 playerIndex, s32 seqId, s32 arg2);
u8* AudioLoad_SyncLoadSeq(s32 seqId);
u32 AudioLoad_TrySyncLoadSampleBank(u32 sampleBankId, u32* outMedium, s32 noLoad);
SoundFontData* AudioLoad_SyncLoadFont(u32 fontId);
void* AudioLoad_SyncLoad(s32 tableType, u32 id, s32* didAllocate);
u32 AudioLoad_GetRealTableIndex(s32 tableType, u32 id);
void* AudioLoad_SearchCaches(s32 tableType, s32 id);
AudioTable* AudioLoad_GetLoadTable(s32 tableType);
void AudioLoad_SyncDma(uintptr_t devAddr, u8* ramAddr, size_t size, s32 medium);
void AudioLoad_SyncDmaUnkMedium(uintptr_t devAddr, u8* addr, size_t size, s32 unkMediumParam);
s32 AudioLoad_Dma(OSIoMesg* mesg, u32 priority, s32 direction, uintptr_t devAddr, void* ramAddr, size_t size,
                  OSMesgQueue* reqQueue, s32 medium, const char* dmaFuncType);
void* AudioLoad_AsyncLoadInner(s32 tableType, s32 id, s32 nChunks, s32 retData, OSMesgQueue* retQueue);
Sample* AudioLoad_GetFontSample(s32 fontId, s32 instId);
void AudioLoad_ProcessSlowLoads(s32 resetStatus);
void AudioLoad_DmaSlowCopy(AudioSlowLoad* slowLoad, size_t size);
void AudioLoad_DmaSlowCopyUnkMedium(intptr_t devAddr, intptr_t ramAddr, size_t size, s32 arg3);
AudioAsyncLoad* AudioLoad_StartAsyncLoadUnkMedium(s32 unkMediumParam, uintptr_t devAddr, void* ramAddr, size_t size,
                                                  s32 medium, s32 nChunks, OSMesgQueue* retQueue, s32 retMsg);
AudioAsyncLoad* AudioLoad_StartAsyncLoad(uintptr_t devAddr, void* ramAddr, size_t size, s32 medium, s32 nChunks,
                                         OSMesgQueue* retQueue, s32 retMsg);
void AudioLoad_ProcessAsyncLoads(s32 resetStatus);
void AudioLoad_ProcessAsyncLoadUnkMedium(AudioAsyncLoad* asyncLoad, s32 resetStatus);
void AudioLoad_ProcessAsyncLoad(AudioAsyncLoad* asyncLoad, s32 resetStatus);
void AudioLoad_AsyncDma(AudioAsyncLoad* asyncLoad, size_t size);
void AudioLoad_AsyncDmaRamUnloaded(AudioAsyncLoad* asyncLoad, size_t size);
void AudioLoad_AsyncDmaUnkMedium(uintptr_t devAddr, void* ramAddr, size_t size, s16 arg3);
void AudioLoad_RelocateSample(TunedSample* tunedSample, SoundFontData* fontData, SampleBankRelocInfo* sampleBankReloc,
                              int fontId);
void AudioLoad_RelocateFontAndPreloadSamples(s32 fontId, SoundFontData* fontData, SampleBankRelocInfo* sampleBankReloc,
                                             s32 isAsync);
s32 AudioLoad_ProcessSamplePreloads(s32 resetStatus);

SequenceData ResourceMgr_LoadSeqByName(const char* path);
SoundFont* ResourceMgr_LoadAudioSoundFontByName(const char* path);
SoundFont* ResourceMgr_LoadAudioSoundFontByCRC(uint64_t crc);

// TODO: what's that for? it seems to rely on an uninitizalied variable in soh
// static uint32_t fontOffsets[8192];

#define MK_ASYNC_MSG(retData, tableType, id, loadStatus) \
    (((retData) << 24) | ((tableType) << 16) | ((id) << 8) | (loadStatus))

#define ASYNC_TBLTYPE(v) ((u8)(v >> 16))
#define ASYNC_ID(v) ((u8)(v >> 8))
#define ASYNC_STATUS(v) ((u8)(v >> 0))

typedef enum {
    /* 0 */ LOAD_STATUS_WAITING,
    /* 1 */ LOAD_STATUS_START,
    /* 2 */ LOAD_STATUS_LOADING,
    /* 3 */ LOAD_STATUS_DONE
} SlowLoadStatus;

#define OS_MESG_PRI_NORMAL 0
#define OS_MESG_PRI_HIGH 1

typedef struct {
    /* 0x0 */ u16 numInstruments;
    /* 0x2 */ u16 numDrums;
    /* 0x4 */ u16 numSfx;
} UnloadedFonts; // size = 0x6

OSMesgQueue sScriptLoadQueue;
OSMesg sScriptLoadMesgBuf[0x10];
s8* sScriptLoadDonePointers[0x10];
s32 sAudioLoadPad1[2]; // file padding
s32 D_801FD1E0;

DmaHandler sDmaHandler; //= osEPiStartDma;
void* sUnusedHandler = NULL;
s32 gAudioCtxInitalized = false;

char** gSequenceMap;
size_t gSequenceMapSize;
u8 seqCachePolicyMap[MAX_AUTHENTIC_SEQID];
char** gFontMap;
size_t gFontMapSize;

void AudioLoad_DecreaseSampleDmaTtls(void) {
    u32 i;

    for (i = 0; i < gAudioCtx.sampleDmaListSize1; i++) {
        SampleDma* dma = &gAudioCtx.sampleDmas[i];

        if (dma->ttl != 0) {
            dma->ttl--;
            if (dma->ttl == 0) {
                dma->reuseIndex = gAudioCtx.sampleDmaReuseQueue1WrPos;
                gAudioCtx.sampleDmaReuseQueue1[gAudioCtx.sampleDmaReuseQueue1WrPos] = i;
                gAudioCtx.sampleDmaReuseQueue1WrPos++;
            }
        }
    }

    for (i = gAudioCtx.sampleDmaListSize1; i < gAudioCtx.sampleDmaCount; i++) {
        SampleDma* dma = &gAudioCtx.sampleDmas[i];

        if (dma->ttl != 0) {
            dma->ttl--;
            if (dma->ttl == 0) {
                dma->reuseIndex = gAudioCtx.sampleDmaReuseQueue2WrPos;
                gAudioCtx.sampleDmaReuseQueue2[gAudioCtx.sampleDmaReuseQueue2WrPos] = i;
                gAudioCtx.sampleDmaReuseQueue2WrPos++;
            }
        }
    }

    gAudioCtx.unused2648 = 0;
}

void* AudioLoad_DmaSampleData(uintptr_t devAddr, size_t size, s32 arg2, u8* dmaIndexRef, s32 medium) {
    s32 pad1;
    SampleDma* dma;
    s32 hasDma = false;
    uintptr_t dmaDevAddr;
    u32 pad2;
    u32 dmaIndex;
    u32 transfer;
    s32 bufferPos;
    u32 i;

    if (arg2 || (*dmaIndexRef >= gAudioCtx.sampleDmaListSize1)) {
        for (i = gAudioCtx.sampleDmaListSize1; i < gAudioCtx.sampleDmaCount; i++) {
            dma = &gAudioCtx.sampleDmas[i];
            bufferPos = devAddr - dma->devAddr;
            if ((0 <= bufferPos) && ((u32)bufferPos <= (dma->size - size))) {
                // We already have a DMA request for this memory range.
                if ((dma->ttl == 0) && (gAudioCtx.sampleDmaReuseQueue2RdPos != gAudioCtx.sampleDmaReuseQueue2WrPos)) {
                    // Move the DMA out of the reuse queue, by swapping it with the
                    // read pos, and then incrementing the read pos.
                    if (dma->reuseIndex != gAudioCtx.sampleDmaReuseQueue2RdPos) {
                        gAudioCtx.sampleDmaReuseQueue2[dma->reuseIndex] =
                            gAudioCtx.sampleDmaReuseQueue2[gAudioCtx.sampleDmaReuseQueue2RdPos];
                        gAudioCtx.sampleDmas[gAudioCtx.sampleDmaReuseQueue2[gAudioCtx.sampleDmaReuseQueue2RdPos]]
                            .reuseIndex = dma->reuseIndex;
                    }
                    gAudioCtx.sampleDmaReuseQueue2RdPos++;
                }
                dma->ttl = 32;
                *dmaIndexRef = (u8)i;
                return dma->ramAddr + (devAddr - dma->devAddr);
            }
        }

        if (!arg2) {
            goto search_short_lived;
        }

        if ((gAudioCtx.sampleDmaReuseQueue2RdPos != gAudioCtx.sampleDmaReuseQueue2WrPos) && arg2) {
            // Allocate a DMA from reuse queue 2, unless full.
            dmaIndex = gAudioCtx.sampleDmaReuseQueue2[gAudioCtx.sampleDmaReuseQueue2RdPos];
            gAudioCtx.sampleDmaReuseQueue2RdPos++;
            dma = gAudioCtx.sampleDmas + dmaIndex;
            hasDma = true;
        }
    } else {
    search_short_lived:
        dma = gAudioCtx.sampleDmas + *dmaIndexRef;
        i = 0;
    again:
        bufferPos = devAddr - dma->devAddr;
        if (0 <= bufferPos && (u32)bufferPos <= dma->size - size) {
            // We already have DMA for this memory range.
            if (dma->ttl == 0) {
                // Move the DMA out of the reuse queue, by swapping it with the
                // read pos, and then incrementing the read pos.
                if (dma->reuseIndex != gAudioCtx.sampleDmaReuseQueue1RdPos) {
                    gAudioCtx.sampleDmaReuseQueue1[dma->reuseIndex] =
                        gAudioCtx.sampleDmaReuseQueue1[gAudioCtx.sampleDmaReuseQueue1RdPos];
                    gAudioCtx.sampleDmas[gAudioCtx.sampleDmaReuseQueue1[gAudioCtx.sampleDmaReuseQueue1RdPos]]
                        .reuseIndex = dma->reuseIndex;
                }
                gAudioCtx.sampleDmaReuseQueue1RdPos++;
            }
            dma->ttl = 2;
            return dma->ramAddr + (devAddr - dma->devAddr);
        }
        dma = gAudioCtx.sampleDmas + i++;
        if (i <= gAudioCtx.sampleDmaListSize1) {
            goto again;
        }
    }

    if (!hasDma) {
        if (gAudioCtx.sampleDmaReuseQueue1RdPos == gAudioCtx.sampleDmaReuseQueue1WrPos) {
            return NULL;
        }
        // Allocate a DMA from reuse queue 1.
        dmaIndex = gAudioCtx.sampleDmaReuseQueue1[gAudioCtx.sampleDmaReuseQueue1RdPos++];
        dma = gAudioCtx.sampleDmas + dmaIndex;
        hasDma = true;
    }

    transfer = dma->size;
    dmaDevAddr = devAddr & ~0xF;
    dma->ttl = 3;
    dma->devAddr = dmaDevAddr;
    dma->sizeUnused = transfer;
    AudioLoad_Dma(&gAudioCtx.currAudioFrameDmaIoMesgBuf[gAudioCtx.curAudioFrameDmaCount++], OS_MESG_PRI_NORMAL, OS_READ,
                  dmaDevAddr, dma->ramAddr, transfer, &gAudioCtx.curAudioFrameDmaQueue, medium, "SUPERDMA");
    *dmaIndexRef = dmaIndex;
    return (devAddr - dmaDevAddr) + dma->ramAddr;
}

// This string does not appear to belong in context to any function between the previous string and the next string
const char D_801E030C[] = "TYPE %d:ID %d is not External Map.\n";

void AudioLoad_InitSampleDmaBuffers(s32 numNotes) {
    SampleDma* dma;
    s32 i;
    s32 t2;
    s32 j;

    gAudioCtx.sampleDmaBufSize = gAudioCtx.sampleDmaBufSize1;
    gAudioCtx.sampleDmas = AudioHeap_Alloc(&gAudioCtx.miscPool, 4 * gAudioCtx.numNotes * sizeof(SampleDma) *
                                                                    gAudioCtx.audioBufferParameters.specUnk4);
    t2 = 3 * gAudioCtx.numNotes * gAudioCtx.audioBufferParameters.specUnk4;

    // First 3/4 sampleDmas
    for (i = 0; i < t2; i++) {
        dma = &gAudioCtx.sampleDmas[gAudioCtx.sampleDmaCount];
        dma->ramAddr = AudioHeap_AllocAttemptExternal(&gAudioCtx.miscPool, gAudioCtx.sampleDmaBufSize);
        if (dma->ramAddr == NULL) {
            break;
        } else {
            AudioHeap_WritebackDCache(dma->ramAddr, gAudioCtx.sampleDmaBufSize);
            dma->size = gAudioCtx.sampleDmaBufSize;
            dma->devAddr = 0;
            dma->sizeUnused = 0;
            dma->unused = 0;
            dma->ttl = 0;
            gAudioCtx.sampleDmaCount++;
        }
    }

    for (i = 0; (u32)i < gAudioCtx.sampleDmaCount; i++) {
        gAudioCtx.sampleDmaReuseQueue1[i] = i;
        gAudioCtx.sampleDmas[i].reuseIndex = i;
    }

    for (i = gAudioCtx.sampleDmaCount; i < 0x100; i++) {
        gAudioCtx.sampleDmaReuseQueue1[i] = 0;
    }

    gAudioCtx.sampleDmaReuseQueue1RdPos = 0;
    gAudioCtx.sampleDmaReuseQueue1WrPos = gAudioCtx.sampleDmaCount;
    gAudioCtx.sampleDmaListSize1 = gAudioCtx.sampleDmaCount;
    gAudioCtx.sampleDmaBufSize = gAudioCtx.sampleDmaBufSize2;

    for (j = 0; j < gAudioCtx.numNotes; j++) {
        dma = &gAudioCtx.sampleDmas[gAudioCtx.sampleDmaCount];
        dma->ramAddr = AudioHeap_AllocAttemptExternal(&gAudioCtx.miscPool, gAudioCtx.sampleDmaBufSize);
        if (dma->ramAddr == NULL) {
            break;
        } else {
            AudioHeap_WritebackDCache(dma->ramAddr, gAudioCtx.sampleDmaBufSize);
            dma->size = gAudioCtx.sampleDmaBufSize;
            dma->devAddr = 0;
            dma->sizeUnused = 0;
            dma->unused = 0;
            dma->ttl = 0;
            gAudioCtx.sampleDmaCount++;
        }
    }

    for (i = gAudioCtx.sampleDmaListSize1; (u32)i < gAudioCtx.sampleDmaCount; i++) {
        gAudioCtx.sampleDmaReuseQueue2[i - gAudioCtx.sampleDmaListSize1] = i;
        gAudioCtx.sampleDmas[i].reuseIndex = i - gAudioCtx.sampleDmaListSize1;
    }

    for (i = gAudioCtx.sampleDmaCount; i < 0x100; i++) {
        gAudioCtx.sampleDmaReuseQueue2[i] = gAudioCtx.sampleDmaListSize1;
    }

    gAudioCtx.sampleDmaReuseQueue2RdPos = 0;
    gAudioCtx.sampleDmaReuseQueue2WrPos = gAudioCtx.sampleDmaCount - gAudioCtx.sampleDmaListSize1;
}

s32 AudioLoad_IsFontLoadComplete(s32 fontId) {
    if (fontId == 0xFFFF) {
        return true;
    } else if (gAudioCtx.fontLoadStatus[fontId] >= LOAD_STATUS_COMPLETE) {
        return true;
    } else if (gAudioCtx.fontLoadStatus[AudioLoad_GetRealTableIndex(FONT_TABLE, fontId)] >= LOAD_STATUS_COMPLETE) {
        return true;
    } else {
        return false;
    }
}

s32 AudioLoad_IsSeqLoadComplete(s32 seqId) {
    if (seqId == 0xFF) {
        return true;
    } else if (gAudioCtx.seqLoadStatus[seqId] >= LOAD_STATUS_COMPLETE) {
        return true;
    } else if (gAudioCtx.seqLoadStatus[AudioLoad_GetRealTableIndex(SEQUENCE_TABLE, seqId)] >= LOAD_STATUS_COMPLETE) {
        return true;
    } else {
        return false;
    }
}

s32 AudioLoad_IsSampleLoadComplete(s32 sampleBankId) {
    if (sampleBankId == 0xFF) {
        return true;
    } else if (gAudioCtx.sampleFontLoadStatus[sampleBankId] >= LOAD_STATUS_COMPLETE) {
        return true;
    } else if (gAudioCtx.sampleFontLoadStatus[AudioLoad_GetRealTableIndex(SAMPLE_TABLE, sampleBankId)] >=
               LOAD_STATUS_COMPLETE) {
        return true;
    } else {
        return false;
    }
}

void AudioLoad_SetFontLoadStatus(s32 fontId, s32 loadStatus) {
    if ((fontId != 0xFFFF) && (gAudioCtx.fontLoadStatus[fontId] != LOAD_STATUS_PERMANENT)) {
        gAudioCtx.fontLoadStatus[fontId] = loadStatus;
    }
}

void AudioLoad_SetSeqLoadStatus(s32 seqId, s32 loadStatus) {
    seqId = AudioEditor_GetOriginalSeq(seqId);
    // 2S2H [Custom Audio] Remove the cast because seqId is not 16 bit.
    if ((seqId != NA_BGM_DISABLED) && (gAudioCtx.seqLoadStatus[seqId] != LOAD_STATUS_PERMANENT)) {
        gAudioCtx.seqLoadStatus[seqId] = loadStatus;
    }
}

void AudioLoad_SetSampleFontLoadStatusAndApplyCaches(s32 sampleBankId, s32 loadStatus) {
    if (sampleBankId != 0xFF) {
        if (gAudioCtx.sampleFontLoadStatus[sampleBankId] != LOAD_STATUS_PERMANENT) {
            gAudioCtx.sampleFontLoadStatus[sampleBankId] = loadStatus;
        }

        if ((gAudioCtx.sampleFontLoadStatus[sampleBankId] == LOAD_STATUS_PERMANENT) ||
            (gAudioCtx.sampleFontLoadStatus[sampleBankId] == LOAD_STATUS_COMPLETE)) {
            AudioHeap_ApplySampleBankCache(sampleBankId);
        }
    }
}

void AudioLoad_SetSampleFontLoadStatus(s32 sampleBankId, s32 loadStatus) {
    if ((sampleBankId != 0xFF) && (gAudioCtx.sampleFontLoadStatus[sampleBankId] != LOAD_STATUS_PERMANENT)) {
        gAudioCtx.sampleFontLoadStatus[sampleBankId] = loadStatus;
    }
}

void AudioLoad_InitTable(AudioTable* table, uintptr_t romAddr, u16 unkMediumParam) {
    s32 i;

    table->unkMediumParam = unkMediumParam;
    table->romAddr = romAddr;

    for (i = 0; i < table->numEntries; i++) {
        if ((table->entries[i].size != 0) && (table->entries[i].medium == MEDIUM_CART)) {
            table->entries[i].romAddr += romAddr;
        }
    }
}

SoundFontData* AudioLoad_SyncLoadSeqFonts(s32 seqId, u32* outDefaultFontId) {
    s32 pad[2];
    s32 index;
    SoundFontData* fontData;
    s32 numFonts;
    s32 fontId;
    s32 i;

    if (seqId >= gAudioCtx.numSequences) {
        return NULL;
    }

    fontId = 0xFF;
    index = ((u16*)gAudioCtx.sequenceFontTable)[seqId];
    numFonts = gAudioCtx.sequenceFontTable[index++];

    while (numFonts > 0) {
        fontId = gAudioCtx.sequenceFontTable[index++];
        fontData = AudioLoad_SyncLoadFont(fontId);
        numFonts--;
    }

    *outDefaultFontId = fontId;
    return fontData;
}

void AudioLoad_SyncLoadSeqParts(s32 seqId, s32 arg1, s32 arg2, OSMesgQueue* arg3) {
    s32 pad;
    u32 defaultFontId;

    if (seqId < gAudioCtx.numSequences) {
        if (arg1 & 2) {
            AudioLoad_SyncLoadSeqFonts(seqId, &defaultFontId);
        }
        if (arg1 & 1) {
            AudioLoad_SyncLoadSeq(seqId);
        }
        if (arg2 != 0) {
            osSendMesg(arg3, OS_MESG_32(arg2 << 0x18), OS_MESG_NOBLOCK);
        }
    }
}

s32 AudioLoad_SyncLoadSample(Sample* sample, s32 fontId) {
    void* sampleAddr;

    if (sample->isRelocated == true) {
        if (sample->medium != MEDIUM_RAM) {
            sampleAddr = AudioHeap_AllocSampleCache(sample->size, fontId, (void*)sample->sampleAddr, sample->medium,
                                                    CACHE_PERSISTENT);
            if (sampleAddr == NULL) {
                return -1;
            }

            if (sample->medium == MEDIUM_UNK) {
                AudioLoad_SyncDmaUnkMedium(sample->sampleAddr, sampleAddr, sample->size,
                                           gAudioCtx.sampleBankTable->unkMediumParam);
            } else {
                AudioLoad_SyncDma(sample->sampleAddr, sampleAddr, sample->size, sample->medium);
            }
            sample->medium = MEDIUM_RAM;
            sample->sampleAddr = sampleAddr;
        }
    }
    //! @bug Missing return, but the return value is never used so it's fine.
}

s32 AudioLoad_SyncLoadInstrument(s32 fontId, s32 instId, s32 drumId) {
    if (instId < 0x7F) {
        Instrument* instrument = AudioPlayback_GetInstrumentInner(fontId, instId);

        if (instrument == NULL) {
            return -1;
        }
        if (instrument->normalRangeLo != 0) {
            AudioLoad_SyncLoadSample(instrument->lowPitchTunedSample.sample, fontId);
        }
        AudioLoad_SyncLoadSample(instrument->normalPitchTunedSample.sample, fontId);
        if (instrument->normalRangeHi != 0x7F) {
            return AudioLoad_SyncLoadSample(instrument->highPitchTunedSample.sample, fontId);
        }
        //! @bug Missing return, but the return value is never used so it's fine.
    } else if (instId == 0x7F) {
        Drum* drum = AudioPlayback_GetDrum(fontId, drumId);

        if (drum == NULL) {
            return -1;
        }
        AudioLoad_SyncLoadSample(drum->tunedSample.sample, fontId);
        return 0;
    }
    //! @bug Missing return, but the return value is never used so it's fine.
}

void AudioLoad_AsyncLoad(s32 tableType, s32 id, s32 nChunks, s32 retData, OSMesgQueue* retQueue) {
    if (AudioLoad_AsyncLoadInner(tableType, id, nChunks, retData, retQueue) == NULL) {
        osSendMesg(retQueue, OS_MESG_32(0xFFFFFFFF), OS_MESG_NOBLOCK);
    }
}

void AudioLoad_AsyncLoadSeq(s32 seqId, s32 arg1, s32 retData, OSMesgQueue* retQueue) {
    AudioLoad_AsyncLoad(SEQUENCE_TABLE, seqId, 0, retData, retQueue);
}

void AudioLoad_AsyncLoadSampleBank(s32 sampleBankId, s32 arg1, s32 retData, OSMesgQueue* retQueue) {
    AudioLoad_AsyncLoad(SAMPLE_TABLE, sampleBankId, 0, retData, retQueue);
}

void AudioLoad_AsyncLoadFont(s32 fontId, s32 arg1, s32 retData, OSMesgQueue* retQueue) {
    AudioLoad_AsyncLoad(FONT_TABLE, fontId, 0, retData, retQueue);
}

u16* AudioLoad_GetFontsForSequence(s32 seqId, u32* outNumFonts, u16* buff) {
    // 2S2H [Custom Audio] There was a second check for `seqId == 0xFF`. Removed because it is no longer useful.
    if (seqId == NA_BGM_DISABLED) {
        return NULL;
    }

    if (seqId >= gSequenceMapSize || !gSequenceMap[seqId]) {
        return NULL;
    }

    SequenceData seqData = ResourceMgr_LoadSeqByName(gSequenceMap[seqId]);

    *outNumFonts = seqData.numFonts;
    if (seqData.numFonts == 0)
        return NULL;

    memcpy(buff, seqData.fonts, sizeof(seqData.fonts));

    return buff;
}

void AudioLoad_DiscardSeqFonts(s32 seqId) {
    s32 fontId;
    s32 index = ((u16*)gAudioCtx.sequenceFontTable)[seqId];
    s32 numFonts = gAudioCtx.sequenceFontTable[index++];

    while (numFonts > 0) {
        numFonts--;
        fontId = AudioLoad_GetRealTableIndex(FONT_TABLE, gAudioCtx.sequenceFontTable[index++]);
        if (AudioHeap_SearchPermanentCache(FONT_TABLE, fontId) == NULL) {
            AudioLoad_DiscardFont(fontId);
            AudioLoad_SetFontLoadStatus(fontId, LOAD_STATUS_NOT_LOADED);
        }
    }
}

void AudioLoad_DiscardFont(s32 fontId) {
    u32 i;
    AudioCache* pool = &gAudioCtx.fontCache;
    AudioPersistentCache* persistent;

    if (fontId == pool->temporary.entries[0].id) {
        pool->temporary.entries[0].id = -1;
    } else if (fontId == pool->temporary.entries[1].id) {
        pool->temporary.entries[1].id = -1;
    }

    persistent = &pool->persistent;
    for (i = 0; i < persistent->numEntries; i++) {
        if (fontId == persistent->entries[i].id) {
            persistent->entries[i].id = -1;
        }
    }

    AudioHeap_DiscardFont(fontId);
}

void func_8018FA60(u32 tableType, u32 id, s32 type, s32 data) {
    AudioTable* table = AudioLoad_GetLoadTable(tableType);

    if (table->entries[id].medium == MEDIUM_RAM_UNLOADED) {
        switch (type) {
            case 0:
                table->entries[id].romAddr = data;
                break;
            case 1:
                table->entries[id].size = data;
                break;
            default:
                break;
        }
    }
}

s32 AudioLoad_SyncInitSeqPlayer(s32 playerIndex, s32 seqId, s32 arg2) {
    if (gAudioCtx.resetTimer != 0) {
        return 0;
    }

    gAudioCtx.seqPlayers[playerIndex].skipTicks = 0;
    return AudioLoad_SyncInitSeqPlayerInternal(playerIndex, seqId, arg2);
}

s32 AudioLoad_SyncInitSeqPlayerSkipTicks(s32 playerIndex, s32 seqId, s32 skipTicks) {
    if (gAudioCtx.resetTimer != 0) {
        return 0;
    }

    gAudioCtx.seqPlayers[playerIndex].skipTicks = skipTicks;
    return AudioLoad_SyncInitSeqPlayerInternal(playerIndex, seqId, 0);
}

s32 AudioLoad_SyncInitSeqPlayerInternal(s32 playerIndex, s32 seqId, s32 arg2) {
    SequencePlayer* seqPlayer = &gAudioCtx.seqPlayers[playerIndex];
    u8* seqData;
    s32 index;
    s32 numFonts;
    s32 fontId;
    s8 authCachePolicy = -1; // since 0 is a valid cache policy value

    AudioScript_SequencePlayerDisable(seqPlayer);

    fontId = 0xFFFF;
    // Resetting all sounds in a state where there is silence, IE map select will crash. This feels like a band-aid fix
    // bit it works.
    if (seqId == 0x7FF) {
        return 0;
    }
    if (gAudioCtx.seqReplaced[playerIndex]) {
        authCachePolicy = seqCachePolicyMap[seqId];
        seqId = gAudioCtx.seqToPlay[playerIndex];
    }
    SequenceData seqData2 = ResourceMgr_LoadSeqByName(gSequenceMap[seqId]);
    if (authCachePolicy != -1) {
        seqData2.cachePolicy = authCachePolicy;
    }

    for (int i = 0; i < seqData2.numFonts; i++) {
        fontId = seqData2.fonts[i];
        AudioLoad_SyncLoadFont(fontId);
    }

    seqData = AudioLoad_SyncLoadSeq(seqId);
    if (seqData == NULL) {
        return 0;
    }

    AudioScript_ResetSequencePlayer(seqPlayer);
    seqPlayer->seqId = seqId;

    if (fontId != 0xFFFF) {
        seqPlayer->defaultFont = AudioLoad_GetRealTableIndex(FONT_TABLE, fontId);
    } else {
        seqPlayer->defaultFont = 0xFFFF;
    }

    seqPlayer->seqData = seqData;
    seqPlayer->enabled = true;
    seqPlayer->scriptState.pc = seqData;
    seqPlayer->scriptState.depth = 0;
    seqPlayer->delay = 0;
    seqPlayer->finished = false;
    seqPlayer->playerIndex = playerIndex;

    GameInteractor_ExecuteOnSeqPlayerInit(playerIndex, seqId);

    return 1;
    //! @bug missing return (but the return value is not used so it's not UB)
}

u8* AudioLoad_SyncLoadSeq(s32 seqId) {
    s32 pad;
    s32 didAllocate;

    if (gAudioCtx.seqLoadStatus[AudioLoad_GetRealTableIndex(SEQUENCE_TABLE, seqId)] == LOAD_STATUS_IN_PROGRESS) {
        return NULL;
    }

    return AudioLoad_SyncLoad(SEQUENCE_TABLE, seqId, &didAllocate);
}

u32 AudioLoad_GetSampleBank(u32 sampleBankId, u32* outMedium) {
    return AudioLoad_TrySyncLoadSampleBank(sampleBankId, outMedium, true);
}

u32 AudioLoad_TrySyncLoadSampleBank(u32 sampleBankId, u32* outMedium, s32 noLoad) {
    void* addr;
    AudioTable* sampleBankTable;
    u32 realTableId = AudioLoad_GetRealTableIndex(SAMPLE_TABLE, sampleBankId);
    s8 cachePolicy;

    sampleBankTable = AudioLoad_GetLoadTable(SAMPLE_TABLE);

    addr = AudioLoad_SearchCaches(SAMPLE_TABLE, realTableId);
    if (addr != NULL) {
        if (gAudioCtx.sampleFontLoadStatus[realTableId] != LOAD_STATUS_IN_PROGRESS) {
            AudioLoad_SetSampleFontLoadStatus(realTableId, LOAD_STATUS_COMPLETE);
        }

        *outMedium = MEDIUM_RAM;
        return addr;
    }

    // cachePolicy = sampleBankTable->entries[sampleBankId].cachePolicy;

    if (/*(cachePolicy == CACHE_LOAD_EITHER_NOSYNC) ||*/ (noLoad == true)) {
        *outMedium = sampleBankTable->entries[sampleBankId].medium;
        return sampleBankTable->entries[realTableId].romAddr;
    }

    addr = AudioLoad_SyncLoad(SAMPLE_TABLE, sampleBankId, &noLoad);
    if (addr != NULL) {
        *outMedium = MEDIUM_RAM;
        return addr;
    }

    *outMedium = sampleBankTable->entries[sampleBankId].medium;
    return sampleBankTable->entries[realTableId].romAddr;
}

SoundFontData* AudioLoad_SyncLoadFont(u32 fontId) {
    SoundFontData* fontData;
    s32 sampleBankId1;
    s32 sampleBankId2;
    s32 didAllocate;
    SampleBankRelocInfo sampleBankReloc;
    s32 realFontId = AudioLoad_GetRealTableIndex(FONT_TABLE, fontId);

    if (gAudioCtx.fontLoadStatus[realFontId] == LOAD_STATUS_IN_PROGRESS) {
        return NULL;
    }

    SoundFont* sf = ResourceMgr_LoadAudioSoundFontByName(gFontMap[fontId]);
    sampleBankId1 = sf->sampleBankId1;
    sampleBankId2 = sf->sampleBankId2;

    sampleBankReloc.sampleBankId1 = sampleBankId1;
    sampleBankReloc.sampleBankId2 = sampleBankId2;
    // if (sampleBankReloc.sampleBankId1 != 0xFF) {
    //     sampleBankReloc.baseAddr1 =
    //         AudioLoad_TrySyncLoadSampleBank(sampleBankReloc.sampleBankId1, &sampleBankReloc.medium1, false);
    // } else {
    sampleBankReloc.baseAddr1 = 0;
    // }

    // if (sampleBankId2 != 0xFF) {
    //     sampleBankReloc.baseAddr2 = AudioLoad_TrySyncLoadSampleBank(sampleBankId2, &sampleBankReloc.medium2, false);
    // } else {
    sampleBankReloc.baseAddr2 = 0;
    // }

    fontData = AudioLoad_SyncLoad(FONT_TABLE, fontId, &didAllocate);
    if (fontData == NULL) {
        return NULL;
    }
    if (didAllocate == true) {
        AudioLoad_RelocateFontAndPreloadSamples(realFontId, fontData, &sampleBankReloc, false);
    }

    return fontData;
}

void* AudioLoad_SyncLoad(s32 tableType, u32 id, s32* didAllocate) {
    size_t size;
    AudioTable* table;
    s32 medium2;
    u32 medium;
    s32 loadStatus;
    uintptr_t romAddr;
    s32 cachePolicy;
    void* ramAddr;
    u32 realId;
    s32 mediumUnk = MEDIUM_UNK;

    realId = AudioLoad_GetRealTableIndex(tableType, id);
    ramAddr = AudioLoad_SearchCaches(tableType, realId);
    if (ramAddr != NULL) {
        *didAllocate = false;
        loadStatus = LOAD_STATUS_COMPLETE;
    } else {
        // table = AudioLoad_GetLoadTable(tableType);
        // size = table->entries[realId].size;
        // size = ALIGN16(size);
        // medium = table->entries[id].medium;
        // cachePolicy = table->entries[id].cachePolicy;
        // romAddr = table->entries[realId].romAddr;

        char* seqData = 0;
        SoundFont* fnt;

        if (tableType == SEQUENCE_TABLE) {
            SequenceData* sData = ResourceMgr_LoadSeqPtrByName(gSequenceMap[id]);
            seqData = sData->seqData;
            size = sData->seqDataSize;
            medium = sData->medium;
            cachePolicy = sData->cachePolicy;
            romAddr = 0;
        } else if (tableType == FONT_TABLE) {
            fnt = ResourceMgr_LoadAudioSoundFontByName(gFontMap[id]);
            size = sizeof(SoundFont);
            medium = 2;
            cachePolicy = 0;
            romAddr = 0;
        }

        switch (cachePolicy) {
            case CACHE_LOAD_PERMANENT:
                //! @bug UB: triggers an UB because this function is missing a return value.
                ramAddr = AudioHeap_AllocPermanent(tableType, realId, size);
                if (ramAddr == NULL) {
                    return ramAddr;
                }
                break;

            case CACHE_LOAD_PERSISTENT:
                ramAddr = AudioHeap_AllocCached(tableType, size, CACHE_PERSISTENT, realId);
                if (ramAddr == NULL) {
                    return ramAddr;
                }
                break;
            case CACHE_LOAD_TEMPORARY:
                ramAddr = AudioHeap_AllocCached(tableType, size, CACHE_TEMPORARY, realId);
                if (ramAddr == NULL) {
                    return ramAddr;
                }
                break;

            case CACHE_LOAD_EITHER:
            case CACHE_LOAD_EITHER_NOSYNC:
                ramAddr = AudioHeap_AllocCached(tableType, size, CACHE_EITHER, realId);
                if (ramAddr == NULL) {
                    return ramAddr;
                }
                break;
        }

        *didAllocate = true;

        medium2 = medium;
        if (medium == MEDIUM_RAM_UNLOADED) {
            if (romAddr == 0) {
                return NULL;
            }

            if (tableType == FONT_TABLE) {
                SoundFont* soundFont = &gAudioCtx.soundFontList[realId];

                soundFont->numInstruments = ((UnloadedFonts*)romAddr)->numInstruments;
                soundFont->numDrums = ((UnloadedFonts*)romAddr)->numDrums;
                soundFont->numSfx = ((UnloadedFonts*)romAddr)->numSfx;
                romAddr += 0x10;
                size -= 0x10;
            }

            memcpy(ramAddr, romAddr, size);
        } else if (medium2 == mediumUnk) {
            AudioLoad_SyncDmaUnkMedium(romAddr, ramAddr, size, (s16)table->unkMediumParam);
        } else {
            if (tableType == SEQUENCE_TABLE && seqData != NULL) {
                AudioLoad_SyncDma(seqData, ramAddr, size, medium);
            } else if (tableType == FONT_TABLE) {
                AudioLoad_SyncDma(fnt, ramAddr, size, medium);
            } else {
                // AudioLoad_SyncDma(romAddr, ret, size, medium);
            }
        }

        loadStatus = (cachePolicy == CACHE_LOAD_PERMANENT) ? LOAD_STATUS_PERMANENT : LOAD_STATUS_COMPLETE;
    }

    switch (tableType) {
        case SEQUENCE_TABLE:
            AudioLoad_SetSeqLoadStatus(realId, loadStatus);
            break;
        case FONT_TABLE:
            AudioLoad_SetFontLoadStatus(realId, loadStatus);
            break;
        case SAMPLE_TABLE:
            AudioLoad_SetSampleFontLoadStatusAndApplyCaches(realId, loadStatus);
            break;
        default:
            break;
    }

    return ramAddr;
}

u32 AudioLoad_GetRealTableIndex(s32 tableType, u32 id) {
    // AudioTable* table = AudioLoad_GetLoadTable(tableType);

    // if (table->entries[id].size == 0) {
    //     id = table->entries[id].romAddr;
    // }

    return id;
}

void* AudioLoad_SearchCaches(s32 tableType, s32 id) {
    void* addr;

    addr = AudioHeap_SearchPermanentCache(tableType, id);
    if (addr != NULL) {
        return addr;
    }

    addr = AudioHeap_SearchCaches(tableType, CACHE_EITHER, id);
    if (addr != NULL) {
        return addr;
    }

    return NULL;
}

AudioTable* AudioLoad_GetLoadTable(s32 tableType) {
    AudioTable* table;

    switch (tableType) {
        case SEQUENCE_TABLE:
            table = gAudioCtx.sequenceTable;
            break;
        case FONT_TABLE:
            table = gAudioCtx.soundFontTable;
            break;
        default:
            table = NULL;
            break;
        case SAMPLE_TABLE:
            table = gAudioCtx.sampleBankTable;
            break;
    }
    return table;
}

/**
 * Read and extract information from soundFont binary loaded into ram.
 * Also relocate offsets into pointers within this loaded soundFont
 *
 * @param fontId index of font being processed
 * @param fontDataStartAddr ram address of raw soundfont binary loaded into cache
 * @param sampleBankReloc information on the sampleBank containing raw audio samples
 */

static void AudioLoad_RelocateFont(s32 fontId, SoundFontData* mem, SampleBankRelocInfo* relocInfo) {
    // 2S2H [PORT] This function is not needed on PC. Made static so the compiler can remove it without needing LTO
}

void AudioLoad_SyncDma(uintptr_t devAddr, u8* ramAddr, size_t size, s32 medium) {
    OSMesgQueue* msgQueue = &gAudioCtx.syncDmaQueue;
    OSIoMesg* ioMesg = &gAudioCtx.syncDmaIoMesg;
    // size = ALIGN16(size);

    Audio_InvalDCache(ramAddr, size);

    while (true) {
        if (size < 0x400) {
            break;
        }
        AudioLoad_Dma(ioMesg, OS_MESG_PRI_HIGH, OS_READ, devAddr, ramAddr, 0x400, msgQueue, medium, "FastCopy");
        osRecvMesg(msgQueue, NULL, OS_MESG_BLOCK);
        size -= 0x400;
        devAddr += 0x400;
        ramAddr += 0x400;
    }

    if (size != 0) {
        AudioLoad_Dma(ioMesg, OS_MESG_PRI_HIGH, OS_READ, devAddr, ramAddr, size, msgQueue, medium, "FastCopy");
        osRecvMesg(msgQueue, NULL, OS_MESG_BLOCK);
    }
}

void AudioLoad_SyncDmaUnkMedium(uintptr_t devAddr, u8* ramAddr, size_t size, s32 unkMediumParam) {
}

s32 AudioLoad_Dma(OSIoMesg* mesg, u32 priority, s32 direction, uintptr_t devAddr, void* ramAddr, size_t size,
                  OSMesgQueue* reqQueue, s32 medium, const char* dmaFuncType) {

    // TODO: what's that for?
    if (gAudioCtx.resetTimer > 0x10) {
        return -1;
    }
    memcpy(ramAddr, devAddr, size);
    return 0;
}

void AudioLoad_Unused1(void) {
}

void AudioLoad_SyncLoadSimple(u32 tableType, u32 fontId) {
    s32 didAllocate;

    AudioLoad_SyncLoad(tableType, fontId, &didAllocate);
}

void* AudioLoad_AsyncLoadInner(s32 tableType, s32 id, s32 nChunks, s32 retData, OSMesgQueue* retQueue) {
    size_t size;
    AudioTable* table;
    void* ramAddr;
    s32 medium;
    s8 cachePolicy;
    uintptr_t romAddr;
    s32 loadStatus;
    SoundFont* soundFont;
    u32 realId = AudioLoad_GetRealTableIndex(tableType, id);
    s32 pad;

    switch (tableType) {
        case SEQUENCE_TABLE:
            if (gAudioCtx.seqLoadStatus[realId] == LOAD_STATUS_IN_PROGRESS) {
                return NULL;
            }
            break;

        case FONT_TABLE:
            if (gAudioCtx.fontLoadStatus[realId] == LOAD_STATUS_IN_PROGRESS) {
                return NULL;
            }
            break;

        case SAMPLE_TABLE:
            if (gAudioCtx.sampleFontLoadStatus[realId] == LOAD_STATUS_IN_PROGRESS) {
                return NULL;
            }
            break;
    }

    ramAddr = AudioLoad_SearchCaches(tableType, realId);
    if (ramAddr != NULL) {
        loadStatus = LOAD_STATUS_COMPLETE;
        osSendMesg(retQueue, OS_MESG_32(MK_ASYNC_MSG(retData, 0, 0, LOAD_STATUS_NOT_LOADED)), OS_MESG_NOBLOCK);
    } else {
        table = AudioLoad_GetLoadTable(tableType);
        size = table->entries[realId].size;
        size = ALIGN16(size);
        medium = table->entries[id].medium;
        cachePolicy = table->entries[id].cachePolicy;
        romAddr = table->entries[realId].romAddr;
        loadStatus = LOAD_STATUS_COMPLETE;

        switch (cachePolicy) {
            case CACHE_LOAD_PERMANENT:
                //! @bug UB: triggers an UB because this function is missing a return value.
                ramAddr = AudioHeap_AllocPermanent(tableType, realId, size);
                if (ramAddr == NULL) {
                    return ramAddr;
                }
                loadStatus = LOAD_STATUS_PERMANENT;
                break;

            case CACHE_LOAD_PERSISTENT:
                ramAddr = AudioHeap_AllocCached(tableType, size, CACHE_PERSISTENT, realId);
                if (ramAddr == NULL) {
                    return ramAddr;
                }
                break;

            case CACHE_LOAD_TEMPORARY:
                ramAddr = AudioHeap_AllocCached(tableType, size, CACHE_TEMPORARY, realId);
                if (ramAddr == NULL) {
                    return ramAddr;
                }
                break;

            case CACHE_LOAD_EITHER:
            case CACHE_LOAD_EITHER_NOSYNC:
                ramAddr = AudioHeap_AllocCached(tableType, size, CACHE_EITHER, realId);
                if (ramAddr == NULL) {
                    return ramAddr;
                }
                break;
        }

        if (medium == MEDIUM_RAM_UNLOADED) {
            if (romAddr == 0) {
                return NULL;
            }

            if (tableType == FONT_TABLE) {
                soundFont = &gAudioCtx.soundFontList[realId];

                soundFont->numInstruments = ((UnloadedFonts*)romAddr)->numInstruments;
                soundFont->numDrums = ((UnloadedFonts*)romAddr)->numDrums;
                soundFont->numSfx = ((UnloadedFonts*)romAddr)->numSfx;
                romAddr += 0x10;
                size += -0x10;
            }
        }

        if (medium == MEDIUM_UNK) {
            AudioLoad_StartAsyncLoadUnkMedium((s16)table->unkMediumParam, romAddr, ramAddr, size, medium, nChunks,
                                              retQueue, MK_ASYNC_MSG(retData, tableType, realId, loadStatus));
        } else {
            AudioLoad_StartAsyncLoad(romAddr, ramAddr, size, medium, nChunks, retQueue,
                                     MK_ASYNC_MSG(retData, tableType, realId, loadStatus));
        }

        loadStatus = LOAD_STATUS_IN_PROGRESS;
    }

    switch (tableType) {
        case SEQUENCE_TABLE:
            AudioLoad_SetSeqLoadStatus(realId, loadStatus);
            break;
        case FONT_TABLE:
            AudioLoad_SetFontLoadStatus(realId, loadStatus);
            break;
        case SAMPLE_TABLE:
            AudioLoad_SetSampleFontLoadStatusAndApplyCaches(realId, loadStatus);
            break;
        default:
            break;
    }

    return ramAddr;
}

void AudioLoad_ProcessLoads(s32 resetStatus) {
    AudioLoad_ProcessSlowLoads(resetStatus);
    AudioLoad_ProcessSamplePreloads(resetStatus);
    AudioLoad_ProcessAsyncLoads(resetStatus);
}

void AudioLoad_SetDmaHandler(DmaHandler callback) {
    sDmaHandler = callback;
}

void AudioLoad_SetUnusedHandler(void* callback) {
    sUnusedHandler = callback;
}

void AudioLoad_InitSoundFont(s32 fontId) {
    SoundFont* font = &gAudioCtx.soundFontList[fontId];
    AudioTableEntry* entry = &gAudioCtx.soundFontTable->entries[fontId];

    font->sampleBankId1 = (entry->shortData1 >> 8) & 0xFF;
    font->sampleBankId2 = (entry->shortData1) & 0xFF;
    font->numInstruments = (entry->shortData2 >> 8) & 0xFF;
    font->numDrums = entry->shortData2 & 0xFF;
    font->numSfx = entry->shortData3;
}

// #region 2S2H [Port] Audio assets in the archive file and custom sequenes
int strcmp_sort(const void* str1, const void* str2) {
    char* const* pp1 = str1;
    char* const* pp2 = str2;
    return strcmp(*pp1, *pp2);
}

extern AudioContext gAudioCtx;
// #end region
#include <libultraship/bridge/resourcebridge.h>

void AudioLoad_Init(void* heap, size_t heapSize) {
    s32 pad1[9];
    s32 numFonts;
    s32 pad2[2];
    u8* audioCtxPtr;
    void* addr;
    s32 i;
    s32 j;

    gAudioCustomUpdateFunction = NULL;
    gAudioCustomReverbFunction = NULL;
    gAudioCustomSynthFunction = NULL;

    for (i = 0; i < ARRAY_COUNT(gAudioCtx.customSeqFunctions); i++) {
        gAudioCtx.customSeqFunctions[i] = NULL;
    }

    gAudioCtx.resetTimer = 0;
    gAudioCtx.unk_29B8 = false;

    memset(&gAudioCtx, 0, sizeof(gAudioCtx));

    switch (osTvType) {
        case OS_TV_PAL:
            gAudioCtx.unk_2960 = 20.03042f;
            gAudioCtx.refreshRate = 50;
            break;

        case OS_TV_MPAL:
            gAudioCtx.unk_2960 = 16.546f;
            gAudioCtx.refreshRate = 60;
            break;

        case OS_TV_NTSC:
        default:
            gAudioCtx.unk_2960 = 16.713f;
            gAudioCtx.refreshRate = 60;
    }

    AudioThread_InitMesgQueues();

    for (i = 0; i < ARRAY_COUNT(gAudioCtx.numSamplesPerFrame); i++) {
        gAudioCtx.numSamplesPerFrame[i] = 0xA0;
    }

    gAudioCtx.totalTaskCount = 0;
    gAudioCtx.rspTaskIndex = 0;
    gAudioCtx.curAiBufferIndex = 0;
    gAudioCtx.soundMode = SOUNDMODE_STEREO;
    gAudioCtx.curTask = NULL;
    gAudioCtx.rspTask[0].task.t.data_size = 0;
    gAudioCtx.rspTask[1].task.t.data_size = 0;

    osCreateMesgQueue(&gAudioCtx.syncDmaQueue, &gAudioCtx.syncDmaMesg, 1);
    osCreateMesgQueue(&gAudioCtx.curAudioFrameDmaQueue, gAudioCtx.currAudioFrameDmaMesgBuf,
                      ARRAY_COUNT(gAudioCtx.currAudioFrameDmaMesgBuf));
    osCreateMesgQueue(&gAudioCtx.externalLoadQueue, gAudioCtx.externalLoadMesgBuf,
                      ARRAY_COUNT(gAudioCtx.externalLoadMesgBuf));
    osCreateMesgQueue(&gAudioCtx.preloadSampleQueue, gAudioCtx.preloadSampleMesgBuf,
                      ARRAY_COUNT(gAudioCtx.preloadSampleMesgBuf));
    gAudioCtx.curAudioFrameDmaCount = 0;
    gAudioCtx.sampleDmaCount = 0;
    gAudioCtx.cartHandle = osCartRomInit();

    if (heap == NULL) {
        gAudioCtx.audioHeap = gAudioHeap;
        gAudioCtx.audioHeapSize = gAudioHeapInitSizes.heapSize;
    } else {
        void** hp = &heap;

        gAudioCtx.audioHeap = *hp;
        gAudioCtx.audioHeapSize = heapSize;
    }

    for (i = 0; i < ((s32)gAudioCtx.audioHeapSize / (s32)sizeof(u64)); i++) {
        ((u64*)gAudioCtx.audioHeap)[i] = 0;
    }

    // Main Pool Split (split entirety of audio heap into initPool and sessionPool)
    AudioHeap_InitMainPool(gAudioHeapInitSizes.initPoolSize);

    // Initialize the audio interface buffers
    for (i = 0; i < ARRAY_COUNT(gAudioCtx.aiBuffers); i++) {
        gAudioCtx.aiBuffers[i] = AudioHeap_AllocZeroed(&gAudioCtx.initPool, AIBUF_LEN * sizeof(s16));
    }

    // Connect audio tables to their tables in memory
    // gAudioCtx.sequenceTable = (AudioTable*)gSequenceTable;
    // gAudioCtx.soundFontTable = (AudioTable*)gSoundFontTable;
    // gAudioCtx.sampleBankTable = (AudioTable*)gSampleBankTable;
    // gAudioCtx.sequenceFontTable = gSequenceFontTable;
    // gAudioCtx.numSequences = gAudioCtx.sequenceTable->numEntries;

    gAudioCtx.specId = 0;
    gAudioCtx.resetStatus = 1; // Set reset to immediately initialize the audio heap
    AudioHeap_ResetStep();

    // Initialize audio tables
    // AudioLoad_InitTable(gAudioCtx.sequenceTable, SEGMENT_ROM_START(Audioseq), 0);
    // AudioLoad_InitTable(gAudioCtx.soundFontTable, SEGMENT_ROM_START(Audiobank), 0);
    // AudioLoad_InitTable(gAudioCtx.sampleBankTable, SEGMENT_ROM_START(Audiotable), 0);

    // #region 2S2H [Port] Audio in the archive and custom sequences
    // Only load the original sequences right now because custom songs may require data from sound fonts and samples
    int seqListSize = 0;
    int customSeqListSize = 0;
    char** seqList = ResourceMgr_ListFiles("audio/sequences*", &seqListSize);
    char** customSeqList = ResourceMgr_ListFiles("custom/music/*", &customSeqListSize);
    gSequenceMapSize = (size_t)(seqListSize + customSeqListSize);
    gSequenceMap = malloc(gSequenceMapSize * sizeof(char*));
    gAudioCtx.seqLoadStatus = calloc(gSequenceMapSize, sizeof(u8));

    memset(&gAudioCtx.seqLoadStatus[seqListSize], LOAD_STATUS_PERMANENT, customSeqListSize);
    for (size_t i = 0; i < seqListSize; i++) {
        SequenceData sDat = ResourceMgr_LoadSeqByName(seqList[i]);
        gSequenceMap[sDat.seqNumber] = strdup(seqList[i]);
        seqCachePolicyMap[sDat.seqNumber] = sDat.cachePolicy;
    }

    free(seqList);

    // 2S2H [Streamed Audio] We need to load the custom songs after the fonts because streamed songs will use a hash to
    // find its soundfont
    int fntListSize = 0;
    int customFntListSize = 0;
    char** fntList = ResourceMgr_ListFiles("audio/fonts*", &fntListSize);
    char** customFntList = ResourceMgr_ListFiles("custom/fonts/*", &customFntListSize);
    gAudioCtx.fontLoadStatus = calloc(customFntListSize + fntListSize, sizeof(u8));
    gFontMap = calloc(customFntListSize + fntListSize, sizeof(char*));
    gFontMapSize = customFntListSize + fntListSize;
    for (int i = 0; i < fntListSize; i++) {
        SoundFont* sf = ResourceMgr_LoadAudioSoundFontByName(fntList[i]);
        gFontMap[sf->fntIndex] = strdup(fntList[i]);
    }

    free(fntList);

    int customFontStart = fntListSize;
    for (int i = customFontStart; i < customFntListSize + fntListSize; i++) {
        SoundFont* sf = ResourceMgr_LoadAudioSoundFontByName(customFntList[i - customFontStart]);
        sf->fntIndex = i;
        gFontMap[i] = strdup(customFntList[i - customFontStart]);
    }
    free(customFntList);

    // 2S2H Port I think we need to take use seqListSize because entry 0x7A is missing.
    int startingSeqNum = seqListSize; // MAX_AUTHENTIC_SEQID; // 109 is the highest vanilla sequence
    qsort(customSeqList, customSeqListSize, sizeof(char*), strcmp_sort);

    // Because AudioCollection's sequenceMap actually has more than sequences (including instruments from 130-135 and
    // sfx in the 2000s, 6000s, 10000s, 14000s, 18000s, and 26000s), it's better here to keep track of the next empty
    // seqNum in AudioCollection instead of just skipping past the instruments at 130 with a higher MAX_AUTHENTIC_SEQID,
    // especially if those others could be added to in the future. However, this really needs to be streamlined with
    // specific ranges in AudioCollection for types, or unifying AudioCollection and the various maps in here
    int seqNum = startingSeqNum;

    for (size_t i = startingSeqNum; i < startingSeqNum + customSeqListSize; i++) {
        // ensure that what would be the next sequence number is actually unassigned in AudioCollection
        int j = i - startingSeqNum;
        SequenceData* sDat = ResourceMgr_LoadSeqPtrByName(customSeqList[j]);

        if (sDat->numFonts == -1) {
            uint64_t crc;

            memcpy(&crc, sDat->fonts, sizeof(uint64_t));
            const char* res = ResourceGetNameByCrc(crc);
            if (res == NULL) {
                // Passing a null buffer and length of 0 to snprintf will return the required numbers of characters the
                // buffer needs to be.
                int len =
                    snprintf(NULL, 0, "Could not find sound font for sequence %s. It will not be in the audio editor.",
                             customSeqList[j]);
                char* error = malloc(len + 1);
                snprintf(error, len, "Could not find sound font for sequence %s. It will not be in the audio editor.",
                         customSeqList[j]);
                LUSLOG_ERROR("%s", error);
                Messagebox_ShowErrorBox("Invalid Sequence", error);
                free(error);
                continue;
            }
            SoundFont* sf = ResourceMgr_LoadAudioSoundFontByName(res);
            memset(&sDat->fonts[0], 0, sizeof(sDat->fonts));
            sDat->fonts[0] = sf->fntIndex;
            sDat->numFonts = 1;
        }

        while (AudioCollection_HasSequenceNum(seqNum)) {
            seqNum++;
        }

        AudioCollection_AddToCollection(customSeqList[j], seqNum);

        sDat->seqNumber = seqNum;
        gSequenceMap[sDat->seqNumber] = strdup(customSeqList[j]);
        seqNum++;
    }

    free(customSeqList);

    numFonts = fntListSize;

    // Every sound font load is a permanent cache entry, so the table needs room for every font and sequence
    gAudioCtx.permanentEntriesCapacity = gFontMapSize + gSequenceMapSize;
    gAudioCtx.permanentEntries = calloc(gAudioCtx.permanentEntriesCapacity, sizeof(AudioCacheEntry));

    // #end region
    gAudioCtx.soundFontList = AudioHeap_Alloc(&gAudioCtx.initPool, numFonts * sizeof(SoundFont));

    if (addr = AudioHeap_Alloc(&gAudioCtx.initPool, gAudioHeapInitSizes.permanentPoolSize), addr == NULL) {
        // cast away const from gAudioHeapInitSizes
        *((u32*)&gAudioHeapInitSizes.permanentPoolSize) = 0;
    }

    AudioHeap_InitPool(&gAudioCtx.permanentPool, addr, gAudioHeapInitSizes.permanentPoolSize);
    gAudioCtxInitalized = true;
    osSendMesg(gAudioCtx.taskStartQueueP, OS_MESG_32(gAudioCtx.totalTaskCount), OS_MESG_NOBLOCK);
}

void AudioLoad_InitSlowLoads(void) {
    gAudioCtx.slowLoads[0].status = 0;
    gAudioCtx.slowLoads[1].status = 0;
}

s32 AudioLoad_SlowLoadSample(s32 fontId, s32 instId, s8* isDone) {
    Sample* sample;
    AudioSlowLoad* slowLoad;

    sample = AudioLoad_GetFontSample(fontId, instId);
    if (sample == NULL) {
        *isDone = 0;
        return -1;
    }

    if (sample->medium == MEDIUM_RAM) {
        *isDone = 2;
        return 0;
    }

    slowLoad = &gAudioCtx.slowLoads[gAudioCtx.slowLoadPos];
    if (slowLoad->status == LOAD_STATUS_DONE) {
        slowLoad->status = LOAD_STATUS_WAITING;
    }

    slowLoad->sample = *sample;
    slowLoad->isDone = isDone;
    slowLoad->curRamAddr =
        AudioHeap_AllocSampleCache(sample->size, fontId, sample->sampleAddr, (s32)sample->medium, CACHE_TEMPORARY);

    if (slowLoad->curRamAddr == NULL) {
        if (sample->medium == MEDIUM_UNK || sample->codec == CODEC_S16_INMEMORY) {
            *isDone = 0;
            return -1;
        } else {
            *isDone = 3;
            return -1;
        }
    }

    slowLoad->status = LOAD_STATUS_START;
    slowLoad->bytesRemaining = ALIGN16(sample->size);
    slowLoad->ramAddr = slowLoad->curRamAddr;
    slowLoad->curDevAddr = sample->sampleAddr;
    slowLoad->medium = sample->medium;
    slowLoad->seqOrFontId = fontId;
    slowLoad->instId = instId;

    if (slowLoad->medium == MEDIUM_UNK) {
        slowLoad->unkMediumParam = gAudioCtx.sampleBankTable->unkMediumParam;
    }

    gAudioCtx.slowLoadPos ^= 1;
    return 0;
}

Sample* AudioLoad_GetFontSample(s32 fontId, s32 instId) {
    Sample* sample;

    if (instId < 0x80) {
        Instrument* instrument = AudioPlayback_GetInstrumentInner(fontId, instId);

        if (instrument == NULL) {
            return NULL;
        }
        sample = instrument->normalPitchTunedSample.sample;
    } else if (instId < 0x100) {
        Drum* drum = AudioPlayback_GetDrum(fontId, instId - 0x80);

        if (drum == NULL) {
            return NULL;
        }
        sample = drum->tunedSample.sample;
    } else {
        SoundEffect* soundEffect = AudioPlayback_GetSoundEffect(fontId, instId - 0x100);

        if (soundEffect == NULL) {
            return NULL;
        }
        sample = soundEffect->tunedSample.sample;
    }

    return sample;
}

void AudioLoad_Unused2(void) {
}

void AudioLoad_FinishSlowLoad(AudioSlowLoad* slowLoad) {
    Sample* sample;

    if (slowLoad->sample.sampleAddr == NULL) {
        return;
    }

    sample = AudioLoad_GetFontSample(slowLoad->seqOrFontId, slowLoad->instId);
    if (sample == NULL) {
        return;
    }

    slowLoad->sample = *sample;
    sample->sampleAddr = slowLoad->ramAddr;
    sample->medium = MEDIUM_RAM;
}

void AudioLoad_ProcessSlowLoads(s32 resetStatus) {
    AudioSlowLoad* slowLoad;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(gAudioCtx.slowLoads); i++) {
        slowLoad = &gAudioCtx.slowLoads[i];
        switch (gAudioCtx.slowLoads[i].status) {
            case LOAD_STATUS_LOADING:
                if (slowLoad->medium != MEDIUM_UNK) {
                    osRecvMesg(&slowLoad->msgqueue, NULL, OS_MESG_BLOCK);
                }

                if (resetStatus != 0) {
                    slowLoad->status = LOAD_STATUS_DONE;
                    continue;
                }
            case LOAD_STATUS_START:
                slowLoad->status = LOAD_STATUS_LOADING;
                if (slowLoad->bytesRemaining == 0) {
                    AudioLoad_FinishSlowLoad(slowLoad);
                    slowLoad->status = LOAD_STATUS_DONE;
                    *slowLoad->isDone = 1;
                } else if ((s32)slowLoad->bytesRemaining < 0x400) {
                    if (slowLoad->medium == MEDIUM_UNK) {
                        size_t size = slowLoad->bytesRemaining;

                        AudioLoad_DmaSlowCopyUnkMedium(slowLoad->curDevAddr, slowLoad->curRamAddr, size,
                                                       slowLoad->unkMediumParam);
                    } else {
                        AudioLoad_DmaSlowCopy(slowLoad, slowLoad->bytesRemaining);
                    }
                    slowLoad->bytesRemaining = 0;
                } else {
                    if (slowLoad->medium == MEDIUM_UNK) {
                        AudioLoad_DmaSlowCopyUnkMedium(slowLoad->curDevAddr, slowLoad->curRamAddr, 0x400,
                                                       slowLoad->unkMediumParam);
                    } else {
                        AudioLoad_DmaSlowCopy(slowLoad, 0x400);
                    }
                    slowLoad->bytesRemaining -= 0x400;
                    slowLoad->curRamAddr += 0x400;
                    slowLoad->curDevAddr += 0x400;
                }
                break;
        }
    }
}

void AudioLoad_DmaSlowCopy(AudioSlowLoad* slowLoad, size_t size) {
    Audio_InvalDCache(slowLoad->curRamAddr, size);
    osCreateMesgQueue(&slowLoad->msgqueue, &slowLoad->msg, 1);
    AudioLoad_Dma(&slowLoad->ioMesg, OS_MESG_PRI_NORMAL, OS_READ, slowLoad->curDevAddr, slowLoad->curRamAddr, size,
                  &slowLoad->msgqueue, slowLoad->medium, "SLOWCOPY");
}

void AudioLoad_DmaSlowCopyUnkMedium(intptr_t devAddr, intptr_t ramAddr, size_t size, s32 arg3) {
}

s32 AudioLoad_SlowLoadSeq(s32 seqId, u8* ramAddr, s8* isDone) {
    AudioSlowLoad* slowLoad;
    AudioTable* seqTable;
    size_t size;

    if (seqId >= gAudioCtx.numSequences) {
        *isDone = 0;
        return -1;
    }

    seqId = AudioLoad_GetRealTableIndex(SEQUENCE_TABLE, seqId);
    // #region 2S2H [Port] Custom sequences
    u16 newSeqId = AudioEditor_GetReplacementSeq(seqId);
    if (seqId != newSeqId) {
        gAudioCtx.seqToPlay[SEQ_PLAYER_BGM_MAIN] = newSeqId;
        gAudioCtx.seqReplaced[SEQ_PLAYER_BGM_MAIN] = 1;
        // This sequence command starts playing a sequence specified by seqId on the main BGM seq player.
        // The sequence command is a bitpacked u32 where different bits of the number indicated different parameters.
        // What those parameters are is dependent on the first 8 bits which represent an operation.
        // First two digits (bits 31-24) - Sequence Command Operation (0x0 = play sequence immediately)
        // Next two digits (bits 23-16) - Index of the SeqPlayer to operate on. (0, which is the main BGM player.)
        // Next two digits (bits 15-8) - Fade Timer (0 in this case, we don't want any fade-in or out here.)
        // Last two digits (bits 7-0) - the sequence ID to play. Not actually sure why it is cast to u16 instead of u8,
        // copied this from authentic game code and adapted it. I think it might be so that you can choose to encode the
        // fade timer into the seqId if you want to for some reason.
        AudioSeq_QueueSeqCmd(0x00000000 | ((u8)SEQ_PLAYER_BGM_MAIN << 24) | ((u8)(0) << 16) | (u16)seqId);
        return 0;
    }
    // #end region
    seqTable = AudioLoad_GetLoadTable(SEQUENCE_TABLE);
    slowLoad = &gAudioCtx.slowLoads[gAudioCtx.slowLoadPos];
    if (slowLoad->status == LOAD_STATUS_DONE) {
        slowLoad->status = LOAD_STATUS_WAITING;
    }

    slowLoad->sample.sampleAddr = NULL;
    slowLoad->isDone = isDone;

    SequenceData sData = ResourceMgr_LoadSeqByName(gSequenceMap[seqId]);
    size = sData.seqDataSize;
    slowLoad->curDevAddr = sData.seqData;
    slowLoad->medium = seqTable->entries[seqId].medium;

    slowLoad->curRamAddr = ramAddr;
    slowLoad->status = LOAD_STATUS_START;
    slowLoad->bytesRemaining = size;
    slowLoad->ramAddr = ramAddr;
    slowLoad->seqOrFontId = seqId;

    if (slowLoad->medium == MEDIUM_UNK) {
        slowLoad->unkMediumParam = seqTable->unkMediumParam;
    }

    gAudioCtx.slowLoadPos ^= 1;
    return 0;
}

void AudioLoad_InitAsyncLoads(void) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(gAudioCtx.asyncLoads); i++) {
        gAudioCtx.asyncLoads[i].status = 0;
    }
}

AudioAsyncLoad* AudioLoad_StartAsyncLoadUnkMedium(s32 unkMediumParam, uintptr_t devAddr, void* ramAddr, size_t size,
                                                  s32 medium, s32 nChunks, OSMesgQueue* retQueue, s32 retMsg) {
    AudioAsyncLoad* asyncLoad;

    asyncLoad = AudioLoad_StartAsyncLoad(devAddr, ramAddr, size, medium, nChunks, retQueue, retMsg);
    if (asyncLoad == NULL) {
        return NULL;
    }

    osSendMesg(&gAudioCtx.asyncLoadUnkMediumQueue, OS_MESG_PTR(asyncLoad), OS_MESG_NOBLOCK);
    asyncLoad->unkMediumParam = unkMediumParam;
    return asyncLoad;
}

AudioAsyncLoad* AudioLoad_StartAsyncLoad(uintptr_t devAddr, void* ramAddr, size_t size, s32 medium, s32 nChunks,
                                         OSMesgQueue* retQueue, s32 retMsg) {
    AudioAsyncLoad* asyncLoad;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(gAudioCtx.asyncLoads); i++) {
        if (gAudioCtx.asyncLoads[i].status == 0) {
            asyncLoad = &gAudioCtx.asyncLoads[i];
            break;
        }
    }

    // no more available async loads
    if (i == ARRAY_COUNT(gAudioCtx.asyncLoads)) {
        return NULL;
    }

    asyncLoad->status = LOAD_STATUS_START;
    asyncLoad->curDevAddr = devAddr;
    asyncLoad->ramAddr = ramAddr;
    asyncLoad->curRamAddr = ramAddr;
    asyncLoad->bytesRemaining = size;

    if (nChunks == 0) {
        asyncLoad->chunkSize = 0x1000;
    } else if (nChunks == 1) {
        asyncLoad->chunkSize = size;
    } else {
        asyncLoad->chunkSize = ALIGN256((s32)size / nChunks);
        if (asyncLoad->chunkSize < 0x100) {
            asyncLoad->chunkSize = 0x100;
        }
    }

    asyncLoad->retQueue = retQueue;
    asyncLoad->delay = 3;
    asyncLoad->medium = medium;
    asyncLoad->retMsg = retMsg;
    osCreateMesgQueue(&asyncLoad->msgQueue, &asyncLoad->msg, 1);
    return asyncLoad;
}

void AudioLoad_ProcessAsyncLoads(s32 resetStatus) {
    AudioAsyncLoad* asyncLoad;
    s32 i;

    if (gAudioCtx.resetTimer == 1) {
        return;
    }

    if (gAudioCtx.curUnkMediumLoad == NULL) {
        if (resetStatus != 0) {
            // Clear and ignore queue if resetting.
            do {
            } while (osRecvMesg(&gAudioCtx.asyncLoadUnkMediumQueue, (OSMesg*)&asyncLoad, OS_MESG_NOBLOCK) != -1);
        } else if (osRecvMesg(&gAudioCtx.asyncLoadUnkMediumQueue, (OSMesg*)&asyncLoad, OS_MESG_NOBLOCK) == -1) {
            gAudioCtx.curUnkMediumLoad = NULL;
        } else {
            gAudioCtx.curUnkMediumLoad = asyncLoad;
        }
    }

    if (gAudioCtx.curUnkMediumLoad != NULL) {
        AudioLoad_ProcessAsyncLoadUnkMedium(gAudioCtx.curUnkMediumLoad, resetStatus);
    }

    for (i = 0; i < ARRAY_COUNT(gAudioCtx.asyncLoads); i++) {
        if (gAudioCtx.asyncLoads[i].status == 1) {
            asyncLoad = &gAudioCtx.asyncLoads[i];
            if (asyncLoad->medium != MEDIUM_UNK) {
                AudioLoad_ProcessAsyncLoad(asyncLoad, resetStatus);
            }
        }
    }
}

void AudioLoad_ProcessAsyncLoadUnkMedium(AudioAsyncLoad* asyncLoad, s32 resetStatus) {
}

void AudioLoad_FinishAsyncLoad(AudioAsyncLoad* asyncLoad) {
    u32 retMsg = asyncLoad->retMsg;
    u32 fontId;
    s32 pad;
    OSMesg doneMsg;
    u32 sampleBankId1;
    u32 sampleBankId2;
    SampleBankRelocInfo sampleBankReloc;

    if (1) {}
    switch (ASYNC_TBLTYPE(retMsg)) {
        case SEQUENCE_TABLE:
            AudioLoad_SetSeqLoadStatus(ASYNC_ID(retMsg), ASYNC_STATUS(retMsg));
            break;

        case SAMPLE_TABLE:
            AudioLoad_SetSampleFontLoadStatusAndApplyCaches(ASYNC_ID(retMsg), ASYNC_STATUS(retMsg));
            break;

        case FONT_TABLE:
            fontId = ASYNC_ID(retMsg);
            sampleBankId1 = gAudioCtx.soundFontList[fontId].sampleBankId1;
            sampleBankId2 = gAudioCtx.soundFontList[fontId].sampleBankId2;
            sampleBankReloc.sampleBankId1 = sampleBankId1;
            sampleBankReloc.sampleBankId2 = sampleBankId2;
            sampleBankReloc.baseAddr1 =
                (sampleBankId1 != 0xFF) ? AudioLoad_GetSampleBank(sampleBankId1, &sampleBankReloc.medium1) : 0;
            sampleBankReloc.baseAddr2 =
                (sampleBankId2 != 0xFF) ? AudioLoad_GetSampleBank(sampleBankId2, &sampleBankReloc.medium2) : 0;
            AudioLoad_SetFontLoadStatus(fontId, ASYNC_STATUS(retMsg));
            AudioLoad_RelocateFontAndPreloadSamples(fontId, asyncLoad->ramAddr, &sampleBankReloc, true);
            break;

        default:
            break;
    }

    // doneMsg = asyncLoad->retMsg;
    if (1) {}
    asyncLoad->status = LOAD_STATUS_WAITING;
    // osSendMesg(asyncLoad->retQueue, doneMsg, OS_MESG_NOBLOCK);
}

void AudioLoad_ProcessAsyncLoad(AudioAsyncLoad* asyncLoad, s32 resetStatus) {
    AudioTable* sampleBankTable = gAudioCtx.sampleBankTable;

    if (asyncLoad->delay >= 2) {
        asyncLoad->delay--;
        return;
    }

    if (asyncLoad->delay == 1) {
        asyncLoad->delay = 0;
    } else if (resetStatus != 0) {
        // Await the previous DMA response synchronously, then return.
        osRecvMesg(&asyncLoad->msgQueue, NULL, OS_MESG_BLOCK);
        asyncLoad->status = LOAD_STATUS_WAITING;
        return;
    } else if (osRecvMesg(&asyncLoad->msgQueue, NULL, OS_MESG_NOBLOCK) == -1) {
        // If the previous DMA step isn't done, return.
        return;
    }

    if (asyncLoad->bytesRemaining == 0) {
        AudioLoad_FinishAsyncLoad(asyncLoad);
        return;
    }

    if (asyncLoad->bytesRemaining < asyncLoad->chunkSize) {
        if (asyncLoad->medium == MEDIUM_UNK) {
            AudioLoad_AsyncDmaUnkMedium(asyncLoad->curDevAddr, asyncLoad->curRamAddr, asyncLoad->bytesRemaining,
                                        sampleBankTable->unkMediumParam);
        } else if (asyncLoad->medium == MEDIUM_RAM_UNLOADED) {
            AudioLoad_AsyncDmaRamUnloaded(asyncLoad, asyncLoad->bytesRemaining);
        } else {
            AudioLoad_AsyncDma(asyncLoad, asyncLoad->bytesRemaining);
        }
        asyncLoad->bytesRemaining = 0;
        return;
    }

    if (asyncLoad->medium == MEDIUM_UNK) {
        AudioLoad_AsyncDmaUnkMedium(asyncLoad->curDevAddr, asyncLoad->curRamAddr, asyncLoad->chunkSize,
                                    sampleBankTable->unkMediumParam);
    } else if (asyncLoad->medium == MEDIUM_RAM_UNLOADED) {
        AudioLoad_AsyncDmaRamUnloaded(asyncLoad, asyncLoad->chunkSize);
    } else {
        AudioLoad_AsyncDma(asyncLoad, asyncLoad->chunkSize);
    }

    asyncLoad->bytesRemaining -= asyncLoad->chunkSize;
    asyncLoad->curDevAddr += asyncLoad->chunkSize;
    asyncLoad->curRamAddr += asyncLoad->chunkSize;
}

void AudioLoad_AsyncDma(AudioAsyncLoad* asyncLoad, size_t size) {
    if (size) {}
    size = ALIGN16(size);
    Audio_InvalDCache(asyncLoad->curRamAddr, size);
    osCreateMesgQueue(&asyncLoad->msgQueue, &asyncLoad->msg, 1);
    AudioLoad_Dma(&asyncLoad->ioMesg, OS_MESG_PRI_NORMAL, OS_READ, asyncLoad->curDevAddr, asyncLoad->curRamAddr, size,
                  &asyncLoad->msgQueue, asyncLoad->medium, "BGCOPY");
}

void AudioLoad_AsyncDmaRamUnloaded(AudioAsyncLoad* asyncLoad, size_t size) {
    if (size) {}
    size = ALIGN16(size);
    Audio_InvalDCache(asyncLoad->curRamAddr, size);
    osCreateMesgQueue(&asyncLoad->msgQueue, &asyncLoad->msg, 1);
    memcpy(asyncLoad->curRamAddr, asyncLoad->curDevAddr, size);
    osSendMesg(&asyncLoad->msgQueue, OS_MESG_PTR(NULL), OS_MESG_NOBLOCK);
}

void AudioLoad_AsyncDmaUnkMedium(uintptr_t devAddr, void* ramAddr, size_t size, s16 arg3) {
}

/**
 * Read and extract information from TunedSample and its Sample
 * contained in the soundFont binary loaded into ram
 * TunedSample contains metadata on a sample used by a particular instrument/drum/sfx
 * Also relocate offsets into pointers within this loaded TunedSample
 *
 * @param fontId index of font being processed
 * @param fontData ram address of raw soundfont binary loaded into cache
 * @param sampleBankReloc information on the sampleBank containing raw audio samples
 */
void AudioLoad_RelocateSample(TunedSample* tunedSample, SoundFontData* fontData, SampleBankRelocInfo* sampleBankReloc,
                              int fontId) {
}

#undef AUDIO_RELOC

/**
 * @param fontId index of font being processed
 * @param fontData ram address of raw soundfont binary loaded into cache
 * @param sampleBankReloc information on the sampleBank containing raw audio samples
 * @param isAsync bool for whether this is an asynchronous load or not
 */
void AudioLoad_RelocateFontAndPreloadSamples(s32 fontId, SoundFontData* fontData, SampleBankRelocInfo* sampleBankReloc,
                                             s32 isAsync) {
    AudioPreloadReq* preload;
    AudioPreloadReq* topPreload;
    Sample* sample;
    size_t size;
    s32 nChunks;
    u8* sampleRamAddr;
    s32 preloadInProgress;
    s32 i;

    preloadInProgress = false;
    if (gAudioCtx.preloadSampleStackTop != 0) {
        preloadInProgress = true;
    } else {
        D_801FD1E0 = 0;
    }

    gAudioCtx.numUsedSamples = 0;
    AudioLoad_RelocateFont(fontId, fontData, sampleBankReloc);

    size = 0;
    for (i = 0; i < gAudioCtx.numUsedSamples; i++) {
        size += ALIGN16(gAudioCtx.usedSamples[i]->size);
    }
    if (size && size) {}

    for (i = 0; i < gAudioCtx.numUsedSamples; i++) {
        if (gAudioCtx.preloadSampleStackTop == 120) {
            break;
        }

        sample = gAudioCtx.usedSamples[i];
        sampleRamAddr = NULL;
        switch (isAsync) {
            case false:
                if (sample->medium == sampleBankReloc->medium1) {
                    sampleRamAddr = AudioHeap_AllocSampleCache(sample->size, sampleBankReloc->sampleBankId1,
                                                               sample->sampleAddr, sample->medium, CACHE_PERSISTENT);
                } else if (sample->medium == sampleBankReloc->medium2) {
                    sampleRamAddr = AudioHeap_AllocSampleCache(sample->size, sampleBankReloc->sampleBankId2,
                                                               sample->sampleAddr, sample->medium, CACHE_PERSISTENT);
                } else if (sample->medium == MEDIUM_DISK_DRIVE) {
                    sampleRamAddr = AudioHeap_AllocSampleCache(sample->size, 0xFE, sample->sampleAddr, sample->medium,
                                                               CACHE_PERSISTENT);
                }
                break;

            case true:
                if (sample->medium == sampleBankReloc->medium1) {
                    sampleRamAddr = AudioHeap_AllocSampleCache(sample->size, sampleBankReloc->sampleBankId1,
                                                               sample->sampleAddr, sample->medium, CACHE_TEMPORARY);
                } else if (sample->medium == sampleBankReloc->medium2) {
                    sampleRamAddr = AudioHeap_AllocSampleCache(sample->size, sampleBankReloc->sampleBankId2,
                                                               sample->sampleAddr, sample->medium, CACHE_TEMPORARY);
                } else if (sample->medium == MEDIUM_DISK_DRIVE) {
                    sampleRamAddr = AudioHeap_AllocSampleCache(sample->size, 0xFE, sample->sampleAddr, sample->medium,
                                                               CACHE_TEMPORARY);
                }
                break;

            default:
                break;
        }
        if (sampleRamAddr == NULL) {
            continue;
        }

        switch (isAsync) {
            case false:
                if (sample->medium == MEDIUM_UNK) {
                    AudioLoad_SyncDmaUnkMedium((uintptr_t)sample->sampleAddr, sampleRamAddr, sample->size,
                                               gAudioCtx.sampleBankTable->unkMediumParam);
                    sample->sampleAddr = sampleRamAddr;
                    sample->medium = MEDIUM_RAM;
                } else {
                    AudioLoad_SyncDma((uintptr_t)sample->sampleAddr, sampleRamAddr, sample->size, sample->medium);
                    sample->sampleAddr = sampleRamAddr;
                    sample->medium = MEDIUM_RAM;
                }
                if (sample->medium == MEDIUM_DISK_DRIVE) {}
                break;

            case true:
                preload = &gAudioCtx.preloadSampleStack[gAudioCtx.preloadSampleStackTop];
                preload->sample = sample;
                preload->ramAddr = sampleRamAddr;
                preload->encodedInfo = (gAudioCtx.preloadSampleStackTop << 24) | 0xFFFFFF;
                preload->isFree = false;
                preload->endAndMediumKey = (uintptr_t)sample->sampleAddr + sample->size + sample->medium;
                gAudioCtx.preloadSampleStackTop++;
                break;

            default:
                break;
        }
    }
    gAudioCtx.numUsedSamples = 0;

    if ((gAudioCtx.preloadSampleStackTop != 0) && !preloadInProgress) {
        topPreload = &gAudioCtx.preloadSampleStack[gAudioCtx.preloadSampleStackTop - 1];
        sample = topPreload->sample;
        nChunks = (sample->size >> 12) + 1;
        AudioLoad_StartAsyncLoad((uintptr_t)sample->sampleAddr, topPreload->ramAddr, sample->size, sample->medium,
                                 nChunks, &gAudioCtx.preloadSampleQueue, topPreload->encodedInfo);
    }
}

s32 AudioLoad_ProcessSamplePreloads(s32 resetStatus) {
    Sample* sample;
    AudioPreloadReq* preload;
    u32 preloadIndex;
    u32 key;
    u32 nChunks;
    s32 pad;

    if (gAudioCtx.preloadSampleStackTop > 0) {
        if (resetStatus != 0) {
            // Clear result queue and preload stack and return.
            osRecvMesg(&gAudioCtx.preloadSampleQueue, (OSMesg*)&preloadIndex, OS_MESG_NOBLOCK);
            gAudioCtx.preloadSampleStackTop = 0;
            return false;
        }
        if (osRecvMesg(&gAudioCtx.preloadSampleQueue, (OSMesg*)&preloadIndex, OS_MESG_NOBLOCK) == -1) {
            // Previous preload is not done yet.
            return false;
        }

        preloadIndex >>= 24;
        preload = &gAudioCtx.preloadSampleStack[preloadIndex];

        if (preload->isFree == false) {
            sample = preload->sample;
            key = (uintptr_t)sample->sampleAddr + sample->size + sample->medium;
            if (key == preload->endAndMediumKey) {
                // Change storage for sample to the preloaded version.
                sample->sampleAddr = preload->ramAddr;
                sample->medium = MEDIUM_RAM;
            }
            preload->isFree = true;
        }

        // Pop requests with isFree = true off the stack, as far as possible,
        // and dispatch the next DMA.
        for (;;) {
            if (gAudioCtx.preloadSampleStackTop <= 0) {
                break;
            }
            preload = &gAudioCtx.preloadSampleStack[gAudioCtx.preloadSampleStackTop - 1];
            if (preload->isFree == true) {
                gAudioCtx.preloadSampleStackTop--;
                continue;
            }

            sample = preload->sample;
            nChunks = (sample->size >> 12) + 1;
            key = (uintptr_t)sample->sampleAddr + sample->size + sample->medium;
            if (key != preload->endAndMediumKey) {
                preload->isFree = true;
                gAudioCtx.preloadSampleStackTop--;
            } else {
                AudioLoad_StartAsyncLoad((uintptr_t)sample->sampleAddr, preload->ramAddr, sample->size, sample->medium,
                                         nChunks, &gAudioCtx.preloadSampleQueue, preload->encodedInfo);
                break;
            }
        }
    }
    return true;
}

s32 AudioLoad_AddToSampleSet(Sample* sample, s32 numSamples, Sample** sampleSet) {
    s32 i;

    for (i = 0; i < numSamples; i++) {
        if (sample->sampleAddr == sampleSet[i]->sampleAddr) {
            break;
        }
    }

    if (i == numSamples) {
        sampleSet[numSamples] = sample;
        numSamples++;
    }

    return numSamples;
}

s32 AudioLoad_GetSamplesForFont(s32 fontId, Sample** sampleSet) {
    s32 i;
    s32 numSamples = 0;
    s32 numDrums = gAudioCtx.soundFontList[fontId].numDrums;
    s32 numInstruments = gAudioCtx.soundFontList[fontId].numInstruments;

    for (i = 0; i < numDrums; i++) {
        Drum* drum = AudioPlayback_GetDrum(fontId, i);

        if (drum == NULL) {
            continue;
        }

        numSamples = AudioLoad_AddToSampleSet(drum->tunedSample.sample, numSamples, sampleSet);
    }

    for (i = 0; i < numInstruments; i++) {
        Instrument* instrument = AudioPlayback_GetInstrumentInner(fontId, i);

        if (instrument == NULL) {
            continue;
        }

        if (instrument->normalRangeLo != 0) {
            numSamples = AudioLoad_AddToSampleSet(instrument->lowPitchTunedSample.sample, numSamples, sampleSet);
        }

        if (instrument->normalRangeHi != 0x7F) {
            numSamples = AudioLoad_AddToSampleSet(instrument->highPitchTunedSample.sample, numSamples, sampleSet);
        }

        numSamples = AudioLoad_AddToSampleSet(instrument->normalPitchTunedSample.sample, numSamples, sampleSet);
    }

    // Should really also process sfx, but this method is never called
    return numSamples;
}

void AudioLoad_AddUsedSample(TunedSample* tunedSample) {
    Sample* sample = tunedSample->sample;

    if (sample && (sample->size != 0) && (sample->unk_bit26) && (sample->medium != MEDIUM_RAM)) {
        gAudioCtx.usedSamples[gAudioCtx.numUsedSamples++] = sample;
    }
}

void AudioLoad_PreloadSamplesForFont(s32 fontId, s32 async, SampleBankRelocInfo* sampleBankReloc) {
    s32 numDrums;
    s32 numInstruments;
    s32 numSfx;
    Drum* drum;
    Instrument* instrument;
    SoundEffect* soundEffect;
    AudioPreloadReq* preload;
    AudioPreloadReq* topPreload;
    u8* addr;
    size_t size;
    s32 i;
    Sample* sample;
    s32 preloadInProgress;
    s32 nChunks;

    preloadInProgress = false;
    if (gAudioCtx.preloadSampleStackTop != 0) {
        preloadInProgress = true;
    }

    gAudioCtx.numUsedSamples = 0;

    SoundFont* sf = ResourceMgr_LoadAudioSoundFontByName(gFontMap[fontId]);
    numDrums = sf->numDrums;
    numInstruments = sf->numInstruments;
    numSfx = sf->numSfx;

    for (i = 0; i < numInstruments; i++) {
        instrument = AudioPlayback_GetInstrumentInner(fontId, i);
        if (instrument != NULL) {
            if (instrument->normalRangeLo != 0) {
                AudioLoad_AddUsedSample(&instrument->lowPitchTunedSample);
            }
            if (instrument->normalRangeHi != 0x7F) {
                AudioLoad_AddUsedSample(&instrument->highPitchTunedSample);
            }
            AudioLoad_AddUsedSample(&instrument->normalPitchTunedSample);
        }
    }

    for (i = 0; i < numDrums; i++) {
        drum = AudioPlayback_GetDrum(fontId, i);
        if (drum != NULL) {
            AudioLoad_AddUsedSample(&drum->tunedSample);
        }
    }

    for (i = 0; i < numSfx; i++) {
        soundEffect = AudioPlayback_GetSoundEffect(fontId, i);
        if (soundEffect != NULL) {
            AudioLoad_AddUsedSample(&soundEffect->tunedSample);
        }
    }

    if (gAudioCtx.numUsedSamples == 0) {
        return;
    }

    size = 0;
    for (i = 0; i < gAudioCtx.numUsedSamples; i++) {
        size += ALIGN16(gAudioCtx.usedSamples[i]->size);
    }
    if (size) {}

    for (i = 0; i < gAudioCtx.numUsedSamples; i++) {
        if (gAudioCtx.preloadSampleStackTop == 120) {
            break;
        }

        sample = gAudioCtx.usedSamples[i];
        if (sample->medium == MEDIUM_RAM) {
            continue;
        }

        switch (async) {
            case false:
                if (sample->medium == sampleBankReloc->medium1) {
                    addr = AudioHeap_AllocSampleCache(sample->size, sampleBankReloc->sampleBankId1, sample->sampleAddr,
                                                      sample->medium, CACHE_PERSISTENT);
                } else if (sample->medium == sampleBankReloc->medium2) {
                    addr = AudioHeap_AllocSampleCache(sample->size, sampleBankReloc->sampleBankId2, sample->sampleAddr,
                                                      sample->medium, CACHE_PERSISTENT);
                }
                break;

            case true:
                if (sample->medium == sampleBankReloc->medium1) {
                    addr = AudioHeap_AllocSampleCache(sample->size, sampleBankReloc->sampleBankId1, sample->sampleAddr,
                                                      sample->medium, CACHE_TEMPORARY);
                } else if (sample->medium == sampleBankReloc->medium2) {
                    addr = AudioHeap_AllocSampleCache(sample->size, sampleBankReloc->sampleBankId2, sample->sampleAddr,
                                                      sample->medium, CACHE_TEMPORARY);
                }
                break;
        }
        if (addr == NULL) {
            continue;
        }

        switch (async) {
            case false:
                if (sample->medium == MEDIUM_UNK) {
                    AudioLoad_SyncDmaUnkMedium((uintptr_t)sample->sampleAddr, addr, sample->size,
                                               gAudioCtx.sampleBankTable->unkMediumParam);
                    sample->sampleAddr = addr;
                    sample->medium = MEDIUM_RAM;
                } else {
                    AudioLoad_SyncDma((uintptr_t)sample->sampleAddr, addr, sample->size, sample->medium);
                    sample->sampleAddr = addr;
                    sample->medium = MEDIUM_RAM;
                }
                break;

            case true:
                preload = &gAudioCtx.preloadSampleStack[gAudioCtx.preloadSampleStackTop];
                preload->sample = sample;
                preload->ramAddr = addr;
                preload->encodedInfo = (gAudioCtx.preloadSampleStackTop << 24) | 0xFFFFFF;
                preload->isFree = false;
                preload->endAndMediumKey = (uintptr_t)sample->sampleAddr + sample->size + sample->medium;
                gAudioCtx.preloadSampleStackTop++;
                break;
        }
    }
    gAudioCtx.numUsedSamples = 0;

    if ((gAudioCtx.preloadSampleStackTop != 0) && !preloadInProgress) {
        topPreload = &gAudioCtx.preloadSampleStack[gAudioCtx.preloadSampleStackTop - 1];
        sample = topPreload->sample;
        nChunks = (sample->size >> 12) + 1;
        AudioLoad_StartAsyncLoad((uintptr_t)sample->sampleAddr, topPreload->ramAddr, sample->size, sample->medium,
                                 nChunks, &gAudioCtx.preloadSampleQueue, topPreload->encodedInfo);
    }
}

void AudioLoad_LoadPermanentSamples(void) {
    s32 pad;
    u32 fontId;
    AudioTable* sampleBankTable;
    s32 pad2;
    s32 i;

    // sampleBankTable = AudioLoad_GetLoadTable(SAMPLE_TABLE);
    for (i = 0; i < gAudioCtx.permanentPool.count; i++) {
        SampleBankRelocInfo sampleBankReloc;

        if (gAudioCtx.permanentEntries[i].tableType == FONT_TABLE) {
            fontId = AudioLoad_GetRealTableIndex(FONT_TABLE, gAudioCtx.permanentEntries[i].id);
            // 2S2H [Port] Audio assets in the archive
            SoundFont* sf = ResourceMgr_LoadAudioSoundFontByName(gFontMap[fontId]);
            sampleBankReloc.sampleBankId1 = sf->sampleBankId1;
            sampleBankReloc.sampleBankId2 = sf->sampleBankId2;
            // sampleBankReloc.sampleBankId1 = gAudioCtx.soundFontList[fontId].sampleBankId1;
            // sampleBankReloc.sampleBankId2 = gAudioCtx.soundFontList[fontId].sampleBankId2;

            // if (sampleBankReloc.sampleBankId1 != 0xFF) {
            //     sampleBankReloc.sampleBankId1 =
            //         AudioLoad_GetRealTableIndex(SAMPLE_TABLE, sampleBankReloc.sampleBankId1);
            //     sampleBankReloc.medium1 = sampleBankTable->entries[sampleBankReloc.sampleBankId1].medium;
            // }

            // if (sampleBankReloc.sampleBankId2 != 0xFF) {
            //     sampleBankReloc.sampleBankId2 =
            //         AudioLoad_GetRealTableIndex(SAMPLE_TABLE, sampleBankReloc.sampleBankId2);
            //     sampleBankReloc.medium2 = sampleBankTable->entries[sampleBankReloc.sampleBankId2].medium;
            // }
            AudioLoad_PreloadSamplesForFont(fontId, false, &sampleBankReloc);
        }
    }
}

void AudioLoad_Unused3(void) {
}

void AudioLoad_Unused4(void) {
}

void AudioLoad_Unused5(void) {
}

void AudioLoad_ScriptLoad(s32 tableType, s32 id, s8* isDone) {
    static u32 sLoadIndex = 0;

    sScriptLoadDonePointers[sLoadIndex] = isDone;
    AudioLoad_AsyncLoad(tableType, id, 0, sLoadIndex, &sScriptLoadQueue);
    sLoadIndex++;
    if (sLoadIndex == 0x10) {
        sLoadIndex = 0;
    }
}

void AudioLoad_ProcessScriptLoads(void) {
    u32 temp;
    u32 sp20;
    s8* isDone;

    if (osRecvMesg(&sScriptLoadQueue, (OSMesg*)&sp20, OS_MESG_NOBLOCK) != -1) {
        temp = sp20 >> 24;
        isDone = sScriptLoadDonePointers[temp];
        if (isDone != NULL) {
            *isDone = false;
        }
    }
}

void AudioLoad_InitScriptLoads(void) {
    osCreateMesgQueue(&sScriptLoadQueue, sScriptLoadMesgBuf, ARRAY_COUNT(sScriptLoadMesgBuf));
}
