#ifndef AUDIO_HEAP_H
#define AUDIO_HEAP_H

#include "PR/ultratypes.h"
#include "libc/stddef.h"
#include "unk.h"

typedef struct {
    /* 0x0 */ size_t heapSize; // total number of bytes allocated to the audio heap. Must be <= the size of `gAudioHeap` (ideally about the same size)
    /* 0x4 */ size_t initPoolSize; // The entire audio heap is split into two pools. 
    /* 0x8 */ size_t permanentPoolSize;
} AudioHeapInitSizes; // size = 0xC

/**
 * Meta-data associated with a pool (contain within the Audio Heap)
 */
typedef struct {
    /* 0x0 */ u8* startAddr; // start addr of the pool
    /* 0x4 */ u8* curAddr; // address of the next available memory for allocation
    /* 0x8 */ size_t size; // size of the pool
    /* 0xC */ s32 count; // number of entries allocated to the pool
} AudioAllocPool; // size = 0x10

/**
 * Audio cache entry data to store a single entry containing either a sequence, soundfont, or entire sample banks
 */
typedef struct {
    /* 0x0 */ u8* addr;
    /* 0x4 */ size_t size;
    /* 0x8 */ s16 tableType;
    /* 0xA */ s16 id;
} AudioCacheEntry; // size = 0xC

/**
 * Audio cache entry data to store a single entry containing an individual sample
 */
typedef struct {
    /* 0x00 */ s8 inUse;
    /* 0x01 */ s8 origMedium;
    /* 0x02 */ u8 sampleBankId;
    /* 0x03 */ UNK_TYPE1 pad03[0x5];
    /* 0x08 */ u8* allocatedAddr;
    /* 0x0C */ void* sampleAddr;
    /* 0x10 */ size_t size;
} SampleCacheEntry; // size = 0x14

/**
 * Audio cache entry data to store individual samples
 */
typedef struct {
    /* 0x000 */ AudioAllocPool pool;
    /* 0x010 */ SampleCacheEntry entries[128];
    /* 0xA10 */ s32 numEntries;
} AudioSampleCache; // size = 0xA14

typedef struct {
    /* 0x00 */ u32 numEntries;
    /* 0x04 */ AudioAllocPool pool;
    /* 0x14 */ AudioCacheEntry entries[16];
} AudioPersistentCache; // size = 0xD4

typedef struct {
    /* 0x00 */ u32 nextSide;
    /* 0x04 */ AudioAllocPool pool;
    /* 0x14 */ AudioCacheEntry entries[2];
} AudioTemporaryCache; // size = 0x3C

typedef struct {
    /* 0x000 */ AudioPersistentCache persistent;
    /* 0x0D4 */ AudioTemporaryCache temporary;
    /* 0x100 */ UNK_TYPE1 pad100[0x10];
} AudioCache; // size = 0x110

typedef struct {
    /* 0x0 */ size_t persistentCommonPoolSize;
    /* 0x4 */ size_t temporaryCommonPoolSize;
} AudioCachePoolSplit; // size = 0x8

typedef struct {
    /* 0x0 */ size_t seqCacheSize;
    /* 0x4 */ size_t fontCacheSize;
    /* 0x8 */ size_t sampleBankCacheSize;
} AudioCommonPoolSplit; // size = 0xC

typedef struct {
    /* 0x0 */ size_t miscPoolSize;
    /* 0x4 */ size_t unusedSizes[2];
    /* 0xC */ size_t cachePoolSize; 
} AudioSessionPoolSplit; // size = 0x10

void AudioHeap_DiscardFont(s32 fontId);
void* AudioHeap_WritebackDCache(void* addr, size_t size);
void* AudioHeap_AllocAttemptExternal(AudioAllocPool* pool, size_t size);
void* AudioHeap_AllocDmaMemory(AudioAllocPool* pool, size_t size);
void* AudioHeap_AllocZeroed(AudioAllocPool* pool, size_t size);
void* AudioHeap_Alloc(AudioAllocPool* pool, size_t size);
void AudioHeap_InitPool(AudioAllocPool* pool, void* addr, size_t size);
void AudioHeap_PopPersistentCache(s32 tableType);
void AudioHeap_InitMainPool(size_t initPoolSize);
void* AudioHeap_AllocCached(s32 tableType, size_t size, s32 cache, s32 id);
void* AudioHeap_SearchCaches(s32 tableType, s32 cache, s32 id);
void AudioHeap_LoadFilter(s16* filter, s32 lowPassCutoff, s32 highPassCutoff);
s32 AudioHeap_ResetStep(void);
void AudioHeap_Init(void);
void* AudioHeap_SearchPermanentCache(s32 tableType, s32 id);
void* AudioHeap_AllocPermanent(s32 tableType, s32 id, size_t size);
void* AudioHeap_AllocSampleCache(size_t size, s32 sampleBankId, void* sampleAddr, s8 medium, s32 cache);
void AudioHeap_ApplySampleBankCache(s32 sampleBankId);
void AudioHeap_SetReverbData(s32 reverbIndex, u32 dataType, uintptr_t data, s32 isFirstInit);

#endif
