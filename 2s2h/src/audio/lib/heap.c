#include "global.h"
#include "audio/effects.h"

void* AudioHeap_SearchRegularCaches(s32 tableType, s32 cache, s32 id);
void AudioHeap_InitSampleCaches(size_t persistentSampleCacheSize, size_t temporarySampleCacheSize);
SampleCacheEntry* AudioHeap_AllocTemporarySampleCacheEntry(size_t size);
void AudioHeap_DiscardSampleCacheEntry(SampleCacheEntry* entry);
void AudioHeap_UnapplySampleCache(SampleCacheEntry* entry, Sample* sample);
SampleCacheEntry* AudioHeap_AllocPersistentSampleCacheEntry(size_t size);
void AudioHeap_DiscardSampleCaches(void);
void AudioHeap_DiscardSampleBank(s32 sampleBankId);
void AudioHeap_ApplySampleBankCacheInternal(s32 apply, s32 sampleBankId);
void AudioHeap_DiscardSampleBanks(void);
void AudioHeap_InitReverb(s32 reverbIndex, ReverbSettings* settings, s32 isFirstInit);

extern size_t gSequenceMapSize;
extern size_t gFontMapSize;

#define gTatumsPerBeat (gAudioTatumInit[1])

/**
 * Effectively scales `updatesPerFrameInv` by the reciprocal of `scaleInv`
 * `updatesPerFrameInvScaled` is just `updatesPerFrameInv` scaled down by a factor of 256.0f
 * i.e. (256.0f * `updatesPerFrameInvScaled`) is just `updatesPerFrameInv`
 */
f32 AudioHeap_CalculateAdsrDecay(f32 scaleInv) {
    return 256.0f * gAudioCtx.audioBufferParameters.updatesPerFrameInvScaled / scaleInv;
}

/**
 * Initialize the decay rate table used for decaying notes as part of adsr
 */
void AudioHeap_InitAdsrDecayTable(void) {
    s32 i;

    gAudioCtx.adsrDecayTable[255] = AudioHeap_CalculateAdsrDecay(0.25f);
    gAudioCtx.adsrDecayTable[254] = AudioHeap_CalculateAdsrDecay(0.33f);
    gAudioCtx.adsrDecayTable[253] = AudioHeap_CalculateAdsrDecay(0.5f);
    gAudioCtx.adsrDecayTable[252] = AudioHeap_CalculateAdsrDecay(0.66f);
    gAudioCtx.adsrDecayTable[251] = AudioHeap_CalculateAdsrDecay(0.75f);

    for (i = 128; i < 251; i++) {
        gAudioCtx.adsrDecayTable[i] = AudioHeap_CalculateAdsrDecay(251 - i);
    }

    for (i = 16; i < 128; i++) {
        gAudioCtx.adsrDecayTable[i] = AudioHeap_CalculateAdsrDecay(4 * (143 - i));
    }

    for (i = 1; i < 16; i++) {
        gAudioCtx.adsrDecayTable[i] = AudioHeap_CalculateAdsrDecay(60 * (23 - i));
    }

    gAudioCtx.adsrDecayTable[0] = 0.0f;
}

void AudioHeap_ResetLoadStatus(void) {
    s32 i;

    for (i = 0; i < gFontMapSize; i++) {
        if (gAudioCtx.fontLoadStatus[i] != LOAD_STATUS_PERMANENT) {
            gAudioCtx.fontLoadStatus[i] = LOAD_STATUS_NOT_LOADED;
        }
    }

    for (i = 0; i < ARRAY_COUNT(gAudioCtx.sampleFontLoadStatus); i++) {
        if (gAudioCtx.sampleFontLoadStatus[i] != LOAD_STATUS_PERMANENT) {
            gAudioCtx.sampleFontLoadStatus[i] = LOAD_STATUS_NOT_LOADED;
        }
    }

    if (gAudioCtx.seqLoadStatus != NULL) {
        for (i = 0; i < gSequenceMapSize; i++) {
            if (gAudioCtx.seqLoadStatus[i] != LOAD_STATUS_PERMANENT) {
                gAudioCtx.seqLoadStatus[i] = LOAD_STATUS_NOT_LOADED;
            }
        }
    }
}

void AudioHeap_DiscardFont(s32 fontId) {
    s32 i;

    for (i = 0; i < gAudioCtx.numNotes; i++) {
        Note* note = &gAudioCtx.notes[i];

        if (note->playbackState.fontId == fontId) {
            if ((note->playbackState.status == PLAYBACK_STATUS_0) && (note->playbackState.priority != 0)) {
                note->playbackState.parentLayer->enabled = false;
                note->playbackState.parentLayer->finished = true;
            }

            AudioPlayback_NoteDisable(note);
            AudioPlayback_AudioListRemove(&note->listItem);
            AudioScript_AudioListPushBack(&gAudioCtx.noteFreeLists.disabled, &note->listItem);
        }
    }
}

void AudioHeap_ReleaseNotesForFont(s32 fontId) {
    s32 i;

    for (i = 0; i < gAudioCtx.numNotes; i++) {
        Note* note = &gAudioCtx.notes[i];
        NotePlaybackState* playbackState = &note->playbackState;

        if (playbackState->fontId == fontId) {
            if (playbackState->priority != 0) {
                playbackState->priority = 1;
                playbackState->adsr.fadeOutVel = gAudioCtx.audioBufferParameters.updatesPerFrameInv;
                playbackState->adsr.action.s.release = true;
            }
        }
    }
}

void AudioHeap_DiscardSequence(s32 seqId) {
    s32 i;

    for (i = 0; i < gAudioCtx.audioBufferParameters.numSequencePlayers; i++) {
        if (gAudioCtx.seqPlayers[i].enabled && gAudioCtx.seqPlayers[i].seqId == seqId) {
            AudioScript_SequencePlayerDisable(&gAudioCtx.seqPlayers[i]);
        }
    }
}

/**
 * Perform a writeback from the L1 data cache to the ram.
 */
void* AudioHeap_WritebackDCache(void* addr, size_t size) {
    return addr;
    Audio_WritebackDCache(addr, size);
    if (addr) {}

    // KSEG0 to KSEG1 (ensures data is written straight to ram instead of the data cache)
    return OS_PHYSICAL_TO_K1(OS_K0_TO_PHYSICAL(addr));
}

/**
 * Attempt to allocated space on an external device. If no external device is available,
 * then allocate space on the pool provided in the argument.
 * Afterwards, zero all data pool the new allocated space
 */
void* AudioHeap_AllocZeroedAttemptExternal(AudioAllocPool* pool, size_t size) {
    void* addr = NULL;

    if (gAudioCtx.externalPool.startAddr != NULL) {
        addr = AudioHeap_AllocZeroed(&gAudioCtx.externalPool, size);
    }

    if (addr == NULL) {
        addr = AudioHeap_AllocZeroed(pool, size);
    }

    return addr;
}

/**
 * Attempt to allocated space on an external device. If no external device is available,
 * then allocate space on the pool provided in the argument
 */
void* AudioHeap_AllocAttemptExternal(AudioAllocPool* pool, size_t size) {
    void* addr = NULL;

    if (gAudioCtx.externalPool.startAddr != NULL) {
        addr = AudioHeap_Alloc(&gAudioCtx.externalPool, size);
    }

    if (addr == NULL) {
        addr = AudioHeap_Alloc(pool, size);
    }

    return addr;
}

void* AudioHeap_AllocDmaMemory(AudioAllocPool* pool, size_t size) {
    void* addr = AudioHeap_Alloc(pool, size);

    if (addr != NULL) {
        addr = AudioHeap_WritebackDCache(addr, size);
    }

    return addr;
}

void* AudioHeap_AllocDmaMemoryZeroed(AudioAllocPool* pool, size_t size) {
    void* addr = AudioHeap_AllocZeroed(pool, size);

    if (addr != NULL) {
        addr = AudioHeap_WritebackDCache(addr, size);
    }

    return addr;
}

/**
 * Allocates space on a pool contained within the heap and sets all the allocated space to 0
 */
void* AudioHeap_AllocZeroed(AudioAllocPool* pool, size_t size) {
    u8* addr = AudioHeap_Alloc(pool, size);
    u8* ptr;

    if (addr != NULL) {
        for (ptr = addr; ptr < pool->curAddr; ptr++) {
            *ptr = 0;
        }
    }

    return addr;
}

/**
 * Tests the pool to see if the requested allocation would fit without actually allocating space.
 * Returns NULL if there is not enough space on the pool.
 * Else, return the address that would have been allocated
 */
void* AudioHeap_TestAlloc(AudioAllocPool* pool, size_t size) {
    u8* oldAddr = pool->curAddr;
    void* addr = AudioHeap_Alloc(pool, size);

    // remove allocation from pool
    if (addr != NULL) {
        pool->curAddr = oldAddr;
        pool->count--;
    }

    return addr;
}

/**
 * Allocates space on the pool contained within the heap. If there is not enough space on the pool, return NULL
 */
void* AudioHeap_Alloc(AudioAllocPool* pool, size_t size) {
    size_t alignedSize = ALIGN16(size);
    u8* curAddr = pool->curAddr;

    if ((pool->startAddr + pool->size) >= (pool->curAddr + alignedSize)) {
        pool->curAddr += alignedSize;
    } else {
        return NULL;
    }

    pool->count++;
    return curAddr;
}

/**
 * Initialize a pool at the requested address with the requested size.
 * Store the metadata of this pool in AudioAllocPool* pool
 */
void AudioHeap_InitPool(AudioAllocPool* pool, void* addr, size_t size) {
    pool->curAddr = pool->startAddr = (u8*)ALIGN16((uintptr_t)addr);
    pool->size = size - ((uintptr_t)addr & 0xF);
    pool->count = 0;
}

void AudioHeap_InitPersistentCache(AudioPersistentCache* persistent) {
    persistent->pool.count = 0;
    persistent->numEntries = 0;
    persistent->pool.curAddr = persistent->pool.startAddr;
}

void AudioHeap_InitTemporaryCache(AudioTemporaryCache* temporary) {
    temporary->pool.count = 0;
    temporary->pool.curAddr = temporary->pool.startAddr;
    temporary->nextSide = 0;
    temporary->entries[0].addr = temporary->pool.startAddr;
    temporary->entries[1].addr = temporary->pool.startAddr + temporary->pool.size;
    temporary->entries[0].id = -1;
    temporary->entries[1].id = -1;
}

void AudioHeap_ResetPool(AudioAllocPool* pool) {
    pool->count = 0;
    pool->curAddr = pool->startAddr;
}

void AudioHeap_PopPersistentCache(s32 tableType) {
    AudioCache* loadedCache;
    AudioAllocPool* persistentHeap;
    AudioPersistentCache* persistent;
    void* entryAddr;
    u8* loadStatus;

    switch (tableType) {
        case SEQUENCE_TABLE:
            loadedCache = &gAudioCtx.seqCache;
            loadStatus = gAudioCtx.seqLoadStatus;
            break;

        case FONT_TABLE:
            loadedCache = &gAudioCtx.fontCache;
            loadStatus = gAudioCtx.fontLoadStatus;
            break;

        case SAMPLE_TABLE:
            loadedCache = &gAudioCtx.sampleBankCache;
            loadStatus = gAudioCtx.sampleFontLoadStatus;
            break;
    }

    persistent = &loadedCache->persistent;
    persistentHeap = &persistent->pool;

    if (persistent->numEntries == 0) {
        return;
    }

    entryAddr = persistent->entries[persistent->numEntries - 1].addr;
    persistentHeap->curAddr = entryAddr;
    persistentHeap->count--;

    if (tableType == SAMPLE_TABLE) {
        AudioHeap_DiscardSampleBank(persistent->entries[persistent->numEntries - 1].id);
    }
    if (tableType == FONT_TABLE) {
        AudioHeap_DiscardFont(persistent->entries[persistent->numEntries - 1].id);
    }

    loadStatus[persistent->entries[persistent->numEntries - 1].id] = LOAD_STATUS_NOT_LOADED;
    persistent->numEntries--;
}

void AudioHeap_InitMainPool(size_t initPoolSize) {
    AudioHeap_InitPool(&gAudioCtx.initPool, gAudioCtx.audioHeap, initPoolSize);
    AudioHeap_InitPool(&gAudioCtx.sessionPool, gAudioCtx.audioHeap + initPoolSize,
                       gAudioCtx.audioHeapSize - initPoolSize);

    gAudioCtx.externalPool.startAddr = NULL;
}

void AudioHeap_InitSessionPool(AudioSessionPoolSplit* split) {
    gAudioCtx.sessionPool.curAddr = gAudioCtx.sessionPool.startAddr;

    AudioHeap_InitPool(&gAudioCtx.miscPool, AudioHeap_Alloc(&gAudioCtx.sessionPool, split->miscPoolSize),
                       split->miscPoolSize);
    AudioHeap_InitPool(&gAudioCtx.cachePool, AudioHeap_Alloc(&gAudioCtx.sessionPool, split->cachePoolSize),
                       split->cachePoolSize);
}

void AudioHeap_InitCachePool(AudioCachePoolSplit* split) {
    gAudioCtx.cachePool.curAddr = gAudioCtx.cachePool.startAddr;

    AudioHeap_InitPool(&gAudioCtx.persistentCommonPool,
                       AudioHeap_Alloc(&gAudioCtx.cachePool, split->persistentCommonPoolSize),
                       split->persistentCommonPoolSize);
    AudioHeap_InitPool(&gAudioCtx.temporaryCommonPool,
                       AudioHeap_Alloc(&gAudioCtx.cachePool, split->temporaryCommonPoolSize),
                       split->temporaryCommonPoolSize);
}

void AudioHeap_InitPersistentPoolsAndCaches(AudioCommonPoolSplit* split) {
    gAudioCtx.persistentCommonPool.curAddr = gAudioCtx.persistentCommonPool.startAddr;

    AudioHeap_InitPool(&gAudioCtx.seqCache.persistent.pool,
                       AudioHeap_Alloc(&gAudioCtx.persistentCommonPool, split->seqCacheSize), split->seqCacheSize);
    AudioHeap_InitPool(&gAudioCtx.fontCache.persistent.pool,
                       AudioHeap_Alloc(&gAudioCtx.persistentCommonPool, split->fontCacheSize), split->fontCacheSize);
    AudioHeap_InitPool(&gAudioCtx.sampleBankCache.persistent.pool,
                       AudioHeap_Alloc(&gAudioCtx.persistentCommonPool, split->sampleBankCacheSize),
                       split->sampleBankCacheSize);

    AudioHeap_InitPersistentCache(&gAudioCtx.seqCache.persistent);
    AudioHeap_InitPersistentCache(&gAudioCtx.fontCache.persistent);
    AudioHeap_InitPersistentCache(&gAudioCtx.sampleBankCache.persistent);
}

void AudioHeap_InitTemporaryPoolsAndCaches(AudioCommonPoolSplit* split) {
    gAudioCtx.temporaryCommonPool.curAddr = gAudioCtx.temporaryCommonPool.startAddr;

    AudioHeap_InitPool(&gAudioCtx.seqCache.temporary.pool,
                       AudioHeap_Alloc(&gAudioCtx.temporaryCommonPool, split->seqCacheSize), split->seqCacheSize);
    AudioHeap_InitPool(&gAudioCtx.fontCache.temporary.pool,
                       AudioHeap_Alloc(&gAudioCtx.temporaryCommonPool, split->fontCacheSize), split->fontCacheSize);
    AudioHeap_InitPool(&gAudioCtx.sampleBankCache.temporary.pool,
                       AudioHeap_Alloc(&gAudioCtx.temporaryCommonPool, split->sampleBankCacheSize),
                       split->sampleBankCacheSize);

    AudioHeap_InitTemporaryCache(&gAudioCtx.seqCache.temporary);
    AudioHeap_InitTemporaryCache(&gAudioCtx.fontCache.temporary);
    AudioHeap_InitTemporaryCache(&gAudioCtx.sampleBankCache.temporary);
}

void* AudioHeap_AllocCached(s32 tableType, size_t size, s32 cache, s32 id) {
    AudioCache* loadedCache;
    AudioTemporaryCache* temporaryCache;
    AudioAllocPool* temporaryPool;
    void* persistentAddr;
    void* temporaryAddr;
    u8 loadStatusEntry0;
    u8 loadStatusEntry1;
    s32 i;
    u8* loadStatus;
    s32 side;

    switch (tableType) {
        case SEQUENCE_TABLE:
            loadedCache = &gAudioCtx.seqCache;
            loadStatus = gAudioCtx.seqLoadStatus;
            break;

        case FONT_TABLE:
            loadedCache = &gAudioCtx.fontCache;
            loadStatus = gAudioCtx.fontLoadStatus;
            break;

        case SAMPLE_TABLE:
            loadedCache = &gAudioCtx.sampleBankCache;
            loadStatus = gAudioCtx.sampleFontLoadStatus;
            break;
    }

    if (cache == CACHE_TEMPORARY) {
        temporaryCache = &loadedCache->temporary;
        temporaryPool = &temporaryCache->pool;

        if ((s32)temporaryPool->size < (s32)size) {
            return NULL;
        }

        loadStatusEntry0 =
            (temporaryCache->entries[0].id == -1) ? LOAD_STATUS_NOT_LOADED : loadStatus[temporaryCache->entries[0].id];
        loadStatusEntry1 =
            (temporaryCache->entries[1].id == -1) ? LOAD_STATUS_NOT_LOADED : loadStatus[temporaryCache->entries[1].id];

        if (tableType == FONT_TABLE) {
            if (loadStatusEntry0 == LOAD_STATUS_MAYBE_DISCARDABLE) {
                for (i = 0; i < gAudioCtx.numNotes; i++) {
                    if ((gAudioCtx.notes[i].playbackState.fontId == temporaryCache->entries[0].id) &&
                        gAudioCtx.notes[i].sampleState.bitField0.enabled) {
                        break;
                    }
                }

                if (i == gAudioCtx.numNotes) {
                    AudioLoad_SetFontLoadStatus(temporaryCache->entries[0].id, LOAD_STATUS_DISCARDABLE);
                    loadStatusEntry0 = LOAD_STATUS_DISCARDABLE;
                }
            }

            if (loadStatusEntry1 == LOAD_STATUS_MAYBE_DISCARDABLE) {
                for (i = 0; i < gAudioCtx.numNotes; i++) {
                    if ((gAudioCtx.notes[i].playbackState.fontId == temporaryCache->entries[1].id) &&
                        gAudioCtx.notes[i].sampleState.bitField0.enabled) {
                        break;
                    }
                }

                if (i == gAudioCtx.numNotes) {
                    AudioLoad_SetFontLoadStatus(temporaryCache->entries[1].id, LOAD_STATUS_DISCARDABLE);
                    loadStatusEntry1 = LOAD_STATUS_DISCARDABLE;
                }
            }
        }

        if (loadStatusEntry0 == LOAD_STATUS_NOT_LOADED) {
            temporaryCache->nextSide = 0;
        } else if (loadStatusEntry1 == LOAD_STATUS_NOT_LOADED) {
            temporaryCache->nextSide = 1;
        } else if ((loadStatusEntry0 == LOAD_STATUS_DISCARDABLE) && (loadStatusEntry1 == LOAD_STATUS_DISCARDABLE)) {
            // Use the opposite side from last time.
        } else if (loadStatusEntry0 == LOAD_STATUS_DISCARDABLE) {
            temporaryCache->nextSide = 0;
        } else if (loadStatusEntry1 == LOAD_STATUS_DISCARDABLE) {
            temporaryCache->nextSide = 1;
        } else {
            // Check if there is a side which isn't in active use, if so, evict that one.
            if (tableType == SEQUENCE_TABLE) {
                if (loadStatusEntry0 == LOAD_STATUS_COMPLETE) {
                    for (i = 0; i < gAudioCtx.audioBufferParameters.numSequencePlayers; i++) {
                        if (gAudioCtx.seqPlayers[i].enabled &&
                            gAudioCtx.seqPlayers[i].seqId == temporaryCache->entries[0].id) {
                            break;
                        }
                    }

                    if (i == gAudioCtx.audioBufferParameters.numSequencePlayers) {
                        temporaryCache->nextSide = 0;
                        goto done;
                    }
                }

                if (loadStatusEntry1 == LOAD_STATUS_COMPLETE) {
                    for (i = 0; i < gAudioCtx.audioBufferParameters.numSequencePlayers; i++) {
                        if (gAudioCtx.seqPlayers[i].enabled &&
                            gAudioCtx.seqPlayers[i].seqId == temporaryCache->entries[1].id) {
                            break;
                        }
                    }

                    if (i == gAudioCtx.audioBufferParameters.numSequencePlayers) {
                        temporaryCache->nextSide = 1;
                        goto done;
                    }
                }
            } else if (tableType == FONT_TABLE) {
                if (loadStatusEntry0 == LOAD_STATUS_COMPLETE) {
                    for (i = 0; i < gAudioCtx.numNotes; i++) {
                        if ((gAudioCtx.notes[i].playbackState.fontId == temporaryCache->entries[0].id) &&
                            gAudioCtx.notes[i].sampleState.bitField0.enabled) {
                            break;
                        }
                    }
                    if (i == gAudioCtx.numNotes) {
                        temporaryCache->nextSide = 0;
                        goto done;
                    }
                }

                if (loadStatusEntry1 == LOAD_STATUS_COMPLETE) {
                    for (i = 0; i < gAudioCtx.numNotes; i++) {
                        if ((gAudioCtx.notes[i].playbackState.fontId == temporaryCache->entries[1].id) &&
                            gAudioCtx.notes[i].sampleState.bitField0.enabled) {
                            break;
                        }
                    }
                    if (i == gAudioCtx.numNotes) {
                        temporaryCache->nextSide = 1;
                        goto done;
                    }
                }
            }

            // No such luck. Evict the side that wasn't chosen last time, except
            // if it is being loaded into.
            if (temporaryCache->nextSide == 0) {
                if (loadStatusEntry0 == LOAD_STATUS_IN_PROGRESS) {
                    if (loadStatusEntry1 == LOAD_STATUS_IN_PROGRESS) {
                        goto fail;
                    }
                    temporaryCache->nextSide = 1;
                }
            } else {
                if (loadStatusEntry1 == LOAD_STATUS_IN_PROGRESS) {
                    if (loadStatusEntry0 == LOAD_STATUS_IN_PROGRESS) {
                        goto fail;
                    }
                    temporaryCache->nextSide = 0;
                }
            }

            if (0) {
            fail:
                // Both sides are being loaded into.
                return NULL;
            }
        }
    done:

        side = temporaryCache->nextSide;

        if (temporaryCache->entries[side].id != -1) {
            if (tableType == SAMPLE_TABLE) {
                AudioHeap_DiscardSampleBank(temporaryCache->entries[side].id);
            }

            loadStatus[temporaryCache->entries[side].id] = LOAD_STATUS_NOT_LOADED;

            if (tableType == FONT_TABLE) {
                AudioHeap_DiscardFont(temporaryCache->entries[side].id);
            }
        }

        switch (side) {
            case 0:
                temporaryCache->entries[0].addr = temporaryPool->startAddr;
                temporaryCache->entries[0].id = id;
                temporaryCache->entries[0].size = size;
                temporaryPool->curAddr = temporaryPool->startAddr + size;

                if ((temporaryCache->entries[1].id != -1) &&
                    (temporaryCache->entries[1].addr < temporaryPool->curAddr)) {
                    if (tableType == SAMPLE_TABLE) {
                        AudioHeap_DiscardSampleBank(temporaryCache->entries[1].id);
                    }

                    loadStatus[temporaryCache->entries[1].id] = LOAD_STATUS_NOT_LOADED;

                    switch (tableType) {
                        case SEQUENCE_TABLE:
                            AudioHeap_DiscardSequence((s32)temporaryCache->entries[1].id);
                            break;

                        case FONT_TABLE:
                            AudioHeap_DiscardFont((s32)temporaryCache->entries[1].id);
                            break;
                    }

                    temporaryCache->entries[1].id = -1;
                    temporaryCache->entries[1].addr = temporaryPool->startAddr + temporaryPool->size;
                }

                temporaryAddr = temporaryCache->entries[0].addr;
                break;

            case 1:
                temporaryCache->entries[1].addr =
                    (u8*)((uintptr_t)(temporaryPool->startAddr + temporaryPool->size - size) & ~0xF);
                temporaryCache->entries[1].id = id;
                temporaryCache->entries[1].size = size;
                if ((temporaryCache->entries[0].id != -1) &&
                    (temporaryCache->entries[1].addr < temporaryPool->curAddr)) {
                    if (tableType == SAMPLE_TABLE) {
                        AudioHeap_DiscardSampleBank(temporaryCache->entries[0].id);
                    }

                    loadStatus[temporaryCache->entries[0].id] = LOAD_STATUS_NOT_LOADED;

                    switch (tableType) {
                        case SEQUENCE_TABLE:
                            AudioHeap_DiscardSequence(temporaryCache->entries[0].id);
                            break;

                        case FONT_TABLE:
                            AudioHeap_DiscardFont(temporaryCache->entries[0].id);
                            break;
                    }

                    temporaryCache->entries[0].id = -1;
                    temporaryPool->curAddr = temporaryPool->startAddr;
                }

                temporaryAddr = temporaryCache->entries[1].addr;
                break;

            default:
                return NULL;
        }

        temporaryCache->nextSide ^= 1;
        return temporaryAddr;
    }

    persistentAddr = AudioHeap_Alloc(&loadedCache->persistent.pool, size);
    loadedCache->persistent.entries[loadedCache->persistent.numEntries].addr = persistentAddr;

    if (persistentAddr == NULL) {
        switch (cache) {
            case CACHE_EITHER:
                return AudioHeap_AllocCached(tableType, size, CACHE_TEMPORARY, id);

            case CACHE_TEMPORARY:
            case CACHE_PERSISTENT:
                return NULL;
        }
    }

    loadedCache->persistent.entries[loadedCache->persistent.numEntries].id = id;
    loadedCache->persistent.entries[loadedCache->persistent.numEntries].size = size;

    return loadedCache->persistent.entries[loadedCache->persistent.numEntries++].addr;
}

void* AudioHeap_SearchCaches(s32 tableType, s32 cache, s32 id) {
    void* addr;

    // Always search the permanent cache in addition to the regular ones.
    addr = AudioHeap_SearchPermanentCache(tableType, id);
    if (addr != NULL) {
        return addr;
    }
    if (cache == CACHE_PERMANENT) {
        return NULL;
    }
    return AudioHeap_SearchRegularCaches(tableType, cache, id);
}

void* AudioHeap_SearchRegularCaches(s32 tableType, s32 cache, s32 id) {
    u32 i;
    AudioCache* loadedCache;
    AudioTemporaryCache* temporary;
    AudioPersistentCache* persistent;

    switch (tableType) {
        case SEQUENCE_TABLE:
            loadedCache = &gAudioCtx.seqCache;
            break;

        case FONT_TABLE:
            loadedCache = &gAudioCtx.fontCache;
            break;

        case SAMPLE_TABLE:
            loadedCache = &gAudioCtx.sampleBankCache;
            break;
    }

    temporary = &loadedCache->temporary;

    if (cache == CACHE_TEMPORARY) {
        if (temporary->entries[0].id == id) {
            temporary->nextSide = 1;
            return temporary->entries[0].addr;
        } else if (temporary->entries[1].id == id) {
            temporary->nextSide = 0;
            return temporary->entries[1].addr;
        } else {
            return NULL;
        }
    }

    persistent = &loadedCache->persistent;

    for (i = 0; i < persistent->numEntries; i++) {
        if (persistent->entries[i].id == id) {
            return persistent->entries[i].addr;
        }
    }

    if (cache == CACHE_EITHER) {
        return AudioHeap_SearchCaches(tableType, CACHE_TEMPORARY, id);
    }
    return NULL;
}

void func_8018C4E4(f32 p, f32 q, u16* out) {
    // With the bug below fixed, this mysterious unused function computes two recurrences
    // out[0..7] = a_i, out[8..15] = b_i, where
    // a_{-2} = b_{-1} = 262159 = 2^18 + 15
    // a_{-1} = b_{-2} = 0
    // a_i = q * a_{i-1} + p * a_{i-2}
    // b_i = q * b_{i-1} + p * b_{i-2}
    // These grow exponentially if p < -1 or p + |q| > 1.
    s32 i;
    f32 tmp[16];

    tmp[0] = (f32)(q * 262159.0f);
    tmp[8] = (f32)(p * 262159.0f);
    tmp[1] = (f32)((q * p) * 262159.0f);
    tmp[9] = (f32)(((p * p) + q) * 262159.0f);

    for (i = 2; i < 8; i++) {
        //! @bug value should be stored to tmp[i] and tmp[8 + i], otherwise we read
        //! garbage in later loop iterations.
        out[i] = q * tmp[i - 2] + p * tmp[i - 1];
        out[8 + i] = q * tmp[6 + i] + p * tmp[7 + i];
    }

    for (i = 0; i < 16; i++) {
        out[i] = tmp[i];
    }
}

void AudioHeap_ClearFilter(s16* filter) {
    s32 i;

    for (i = 0; i < 8; i++) {
        filter[i] = 0;
    }
}

void AudioHeap_LoadLowPassFilter(s16* filter, s32 cutoff) {
    s32 i;
    s16* ptr = &gLowPassFilterData[8 * cutoff];

    for (i = 0; i < 8; i++) {
        filter[i] = ptr[i];
    }
}

void AudioHeap_LoadHighPassFilter(s16* filter, s32 cutoff) {
    s32 i;
    s16* ptr = &gHighPassFilterData[8 * (cutoff - 1)];

    for (i = 0; i < 8; i++) {
        filter[i] = ptr[i];
    }
}

void AudioHeap_LoadFilter(s16* filter, s32 lowPassCutoff, s32 highPassCutoff) {
    s32 i;
    s32 j;
    s32 k;
    s32 cutOff;

    //! @bug filter is never set if (lowPassCutoff == highPassCutoff) and does not equal 0
    if ((lowPassCutoff == 0) && (highPassCutoff == 0)) {
        // Identity filter
        AudioHeap_LoadLowPassFilter(filter, 0);
    } else if (highPassCutoff == 0) {
        AudioHeap_LoadLowPassFilter(filter, lowPassCutoff);
    } else if (lowPassCutoff == 0) {
        AudioHeap_LoadHighPassFilter(filter, highPassCutoff);
    } else {
        k = 0;
        j = 14;

        cutOff = lowPassCutoff;
        if (lowPassCutoff < highPassCutoff) {

            for (i = 1; i < cutOff; i++) {
                k += j;
                j--;
            }

            k += highPassCutoff - lowPassCutoff - 1;
            for (i = 0; i < 8; i++) {
                //! @bug should be gBandStopFilterData[8 * k + i];
                filter[i] = gBandStopFilterData[k + i];
            }
        } else if (highPassCutoff < lowPassCutoff) {

            cutOff = highPassCutoff;
            for (i = 1; i < cutOff; i++) {
                k += j;
                j--;
            }

            k += lowPassCutoff - highPassCutoff - 1;
            for (i = 0; i < 8; i++) {
                //! @bug should be gBandPassFilterData[8 * k + i];
                filter[i] = gBandPassFilterData[k + i];
            }
        }
    }
}

void AudioHeap_UpdateReverb(SynthesisReverb* reverb) {
}

void AudioHeap_UpdateReverbs(void) {
    s32 count;
    s32 reverbIndex;
    s32 j;

    if (gAudioCtx.audioBufferParameters.specUnk4 == 2) {
        count = 2;
    } else {
        count = 1;
    }

    for (reverbIndex = 0; reverbIndex < gAudioCtx.numSynthesisReverbs; reverbIndex++) {
        for (j = 0; j < count; j++) {
            AudioHeap_UpdateReverb(&gAudioCtx.synthesisReverbs[reverbIndex]);
        }
    }
}

/**
 * Clear the Audio Interface Buffers
 */
void AudioHeap_ClearAiBuffers(void) {
    s32 curAiBufferIndex = gAudioCtx.curAiBufferIndex;
    s32 i;

    gAudioCtx.numSamplesPerFrame[curAiBufferIndex] = gAudioCtx.audioBufferParameters.numSamplesPerFrameMin;

    for (i = 0; i < AIBUF_LEN; i++) {
        gAudioCtx.aiBuffers[curAiBufferIndex][i] = 0;
    }
}

s32 AudioHeap_ResetStep(void) {
    s32 i;
    s32 j;
    s32 count;

    if (gAudioCtx.audioBufferParameters.specUnk4 == 2) {
        count = 2;
    } else {
        count = 1;
    }

    switch (gAudioCtx.resetStatus) {
        case 5:
            for (i = 0; i < gAudioCtx.audioBufferParameters.numSequencePlayers; i++) {
                AudioScript_SequencePlayerDisableAsFinished(&gAudioCtx.seqPlayers[i]);
            }
            gAudioCtx.audioResetFadeOutFramesLeft = 2 / count;
            gAudioCtx.resetStatus--;
            break;

        case 4:
            if (gAudioCtx.audioResetFadeOutFramesLeft != 0) {
                gAudioCtx.audioResetFadeOutFramesLeft--;
                AudioHeap_UpdateReverbs();
            } else {
                for (i = 0; i < gAudioCtx.numNotes; i++) {
                    if (gAudioCtx.notes[i].sampleState.bitField0.enabled &&
                        gAudioCtx.notes[i].playbackState.adsr.action.s.status != ADSR_STATUS_DISABLED) {
                        gAudioCtx.notes[i].playbackState.adsr.fadeOutVel =
                            gAudioCtx.audioBufferParameters.updatesPerFrameInv;
                        gAudioCtx.notes[i].playbackState.adsr.action.s.release = true;
                    }
                }
                gAudioCtx.audioResetFadeOutFramesLeft = 8 / count;
                gAudioCtx.resetStatus--;
            }
            break;

        case 3:
            if (gAudioCtx.audioResetFadeOutFramesLeft != 0) {
                gAudioCtx.audioResetFadeOutFramesLeft--;
                AudioHeap_UpdateReverbs();
            } else {
                gAudioCtx.audioResetFadeOutFramesLeft = 2 / count;
                gAudioCtx.resetStatus--;
            }
            break;

        case 2:
            AudioHeap_ClearAiBuffers();
            if (gAudioCtx.audioResetFadeOutFramesLeft != 0) {
                gAudioCtx.audioResetFadeOutFramesLeft--;
            } else {
                gAudioCtx.resetStatus--;
                AudioHeap_DiscardSampleCaches();
                AudioHeap_DiscardSampleBanks();
            }
            break;

        case 1:
            AudioHeap_Init();
            gAudioCtx.resetStatus = 0;
            for (i = 0; i < ARRAY_COUNT(gAudioCtx.numSamplesPerFrame); i++) {
                gAudioCtx.numSamplesPerFrame[i] = gAudioCtx.audioBufferParameters.numSamplesPerFrameMax;
                for (j = 0; j < AIBUF_LEN; j++) {
                    gAudioCtx.aiBuffers[i][j] = 0;
                }
            }
            break;
    }

    if (gAudioCtx.resetStatus < 3) {
        return false;
    }

    return true;
}

void AudioHeap_Init(void) {
    s32 pad1[4];
    s16* addr;
    size_t persistentSize;
    size_t temporarySize;
    size_t cachePoolSize;
    size_t miscPoolSize;
    u32 intMask;
    s32 reverbIndex;
    s32 i;
    s32 pad2;
    AudioSpec* spec = &gAudioSpecs[gAudioCtx.specId]; // Audio Specifications

    gAudioCtx.sampleDmaCount = 0;

    // audio buffer parameters
    gAudioCtx.audioBufferParameters.samplingFreq = spec->samplingFreq;
    gAudioCtx.audioBufferParameters.aiSamplingFreq = osAiSetFrequency(gAudioCtx.audioBufferParameters.samplingFreq);

    gAudioCtx.audioBufferParameters.numSamplesPerFrameTarget =
        ALIGN16(gAudioCtx.audioBufferParameters.samplingFreq / gAudioCtx.refreshRate);
    gAudioCtx.audioBufferParameters.numSamplesPerFrameMin =
        gAudioCtx.audioBufferParameters.numSamplesPerFrameTarget - 0x10;
    gAudioCtx.audioBufferParameters.numSamplesPerFrameMax =
        gAudioCtx.audioBufferParameters.numSamplesPerFrameTarget + 0x10;
    gAudioCtx.audioBufferParameters.updatesPerFrame =
        ((gAudioCtx.audioBufferParameters.numSamplesPerFrameTarget + 0x10) / 0xD0) + 1;
    gAudioCtx.audioBufferParameters.numSamplesPerUpdate =
        (gAudioCtx.audioBufferParameters.numSamplesPerFrameTarget / gAudioCtx.audioBufferParameters.updatesPerFrame) &
        ~7;
    gAudioCtx.audioBufferParameters.numSamplesPerUpdateMax = gAudioCtx.audioBufferParameters.numSamplesPerUpdate + 8;
    gAudioCtx.audioBufferParameters.numSamplesPerUpdateMin = gAudioCtx.audioBufferParameters.numSamplesPerUpdate - 8;
    gAudioCtx.audioBufferParameters.resampleRate = 32000.0f / (s32)gAudioCtx.audioBufferParameters.samplingFreq;
    gAudioCtx.audioBufferParameters.updatesPerFrameInvScaled =
        (1.0f / 256.0f) / gAudioCtx.audioBufferParameters.updatesPerFrame;
    gAudioCtx.audioBufferParameters.updatesPerFrameScaled = gAudioCtx.audioBufferParameters.updatesPerFrame / 4.0f;
    gAudioCtx.audioBufferParameters.updatesPerFrameInv = 1.0f / gAudioCtx.audioBufferParameters.updatesPerFrame;

    // sample dma size
    gAudioCtx.sampleDmaBufSize1 = spec->sampleDmaBufSize1;
    gAudioCtx.sampleDmaBufSize2 = spec->sampleDmaBufSize2;

    gAudioCtx.numNotes = spec->numNotes;
    gAudioCtx.audioBufferParameters.numSequencePlayers = spec->numSequencePlayers;

    if (gAudioCtx.audioBufferParameters.numSequencePlayers > 5) {
        gAudioCtx.audioBufferParameters.numSequencePlayers = 5;
    }

    gAudioCtx.numAbiCmdsMax = 8;
    gAudioCtx.unk_2 = spec->unk_14;
    gAudioCtx.maxTempo =
        (u32)(gAudioCtx.audioBufferParameters.updatesPerFrame * 2880000.0f / gTatumsPerBeat / gAudioCtx.unk_2960);

    gAudioCtx.unk_2870 = gAudioCtx.refreshRate;
    gAudioCtx.unk_2870 *= gAudioCtx.audioBufferParameters.updatesPerFrame;
    gAudioCtx.unk_2870 /= gAudioCtx.audioBufferParameters.aiSamplingFreq;
    gAudioCtx.unk_2870 /= gAudioCtx.maxTempo;

    gAudioCtx.audioBufferParameters.specUnk4 = spec->unk_04;
    gAudioCtx.audioBufferParameters.numSamplesPerFrameTarget *= gAudioCtx.audioBufferParameters.specUnk4;
    gAudioCtx.audioBufferParameters.numSamplesPerFrameMax *= gAudioCtx.audioBufferParameters.specUnk4;
    gAudioCtx.audioBufferParameters.numSamplesPerFrameMin *= gAudioCtx.audioBufferParameters.specUnk4;
    gAudioCtx.audioBufferParameters.updatesPerFrame *= gAudioCtx.audioBufferParameters.specUnk4;

    if (gAudioCtx.audioBufferParameters.specUnk4 >= 2) {
        gAudioCtx.audioBufferParameters.numSamplesPerFrameMax -= 0x10;
    }

    // Determine the maximum allowable number of audio command list entries for the rsp microcode
    gAudioCtx.maxAudioCmds =
        gAudioCtx.numNotes * 20 * gAudioCtx.audioBufferParameters.updatesPerFrame + spec->numReverbs * 30 + 800;

    // Calculate sizes for various caches on the audio heap
    persistentSize =
        spec->persistentSeqCacheSize + spec->persistentFontCacheSize + spec->persistentSampleBankCacheSize + 0x10;
    temporarySize =
        spec->temporarySeqCacheSize + spec->temporaryFontCacheSize + spec->temporarySampleBankCacheSize + 0x10;
    cachePoolSize = persistentSize + temporarySize;
    miscPoolSize = gAudioCtx.sessionPool.size - cachePoolSize - 0x100;

    if (gAudioCtx.externalPool.startAddr != NULL) {
        gAudioCtx.externalPool.curAddr = gAudioCtx.externalPool.startAddr;
    }

    // Session Pool Split (split into Cache and Misc heaps)
    gAudioCtx.sessionPoolSplit.miscPoolSize = miscPoolSize;
    gAudioCtx.sessionPoolSplit.cachePoolSize = cachePoolSize;
    AudioHeap_InitSessionPool(&gAudioCtx.sessionPoolSplit);

    // Cache Pool Split (Split into Persistent and Temporary heaps)
    gAudioCtx.cachePoolSplit.persistentCommonPoolSize = persistentSize;
    gAudioCtx.cachePoolSplit.temporaryCommonPoolSize = temporarySize;
    AudioHeap_InitCachePool(&gAudioCtx.cachePoolSplit);

    // Persistent Pool Split (Split into Sequences, SoundFonts, Samples)
    gAudioCtx.persistentCommonPoolSplit.seqCacheSize = spec->persistentSeqCacheSize;
    gAudioCtx.persistentCommonPoolSplit.fontCacheSize = spec->persistentFontCacheSize;
    gAudioCtx.persistentCommonPoolSplit.sampleBankCacheSize = spec->persistentSampleBankCacheSize;
    AudioHeap_InitPersistentPoolsAndCaches(&gAudioCtx.persistentCommonPoolSplit);

    // Temporary Pool Split (Split into Sequences, SoundFonts, Samples)
    gAudioCtx.temporaryCommonPoolSplit.seqCacheSize = spec->temporarySeqCacheSize;
    gAudioCtx.temporaryCommonPoolSplit.fontCacheSize = spec->temporaryFontCacheSize;
    gAudioCtx.temporaryCommonPoolSplit.sampleBankCacheSize = spec->temporarySampleBankCacheSize;
    AudioHeap_InitTemporaryPoolsAndCaches(&gAudioCtx.temporaryCommonPoolSplit);

    AudioHeap_ResetLoadStatus();

    // Initialize notes
    gAudioCtx.notes = AudioHeap_AllocZeroed(&gAudioCtx.miscPool, gAudioCtx.numNotes * sizeof(Note));
    AudioPlayback_NoteInitAll();
    AudioPlayback_InitNoteFreeList();
    gAudioCtx.sampleStateList =
        AudioHeap_AllocZeroed(&gAudioCtx.miscPool, gAudioCtx.audioBufferParameters.updatesPerFrame *
                                                       gAudioCtx.numNotes * sizeof(NoteSampleState));

    // Initialize audio binary interface command list buffer
    for (i = 0; i < ARRAY_COUNT(gAudioCtx.abiCmdBufs); i++) {
        gAudioCtx.abiCmdBufs[i] =
            AudioHeap_AllocDmaMemoryZeroed(&gAudioCtx.miscPool, gAudioCtx.maxAudioCmds * sizeof(Acmd));
    }

    // Initialize the decay rate table for ADSR
    gAudioCtx.adsrDecayTable = AudioHeap_Alloc(&gAudioCtx.miscPool, 0x100 * sizeof(f32));
    AudioHeap_InitAdsrDecayTable();

    // Initialize reverbs
    for (reverbIndex = 0; reverbIndex < ARRAY_COUNT(gAudioCtx.synthesisReverbs); reverbIndex++) {
        gAudioCtx.synthesisReverbs[reverbIndex].useReverb = 0;
    }

    gAudioCtx.numSynthesisReverbs = spec->numReverbs;
    for (reverbIndex = 0; reverbIndex < gAudioCtx.numSynthesisReverbs; reverbIndex++) {
        AudioHeap_InitReverb(reverbIndex, &spec->reverbSettings[reverbIndex], true);
    }

    // Initialize sequence players
    AudioScript_InitSequencePlayers();
    for (i = 0; i < gAudioCtx.audioBufferParameters.numSequencePlayers; i++) {
        AudioScript_InitSequencePlayerChannels(i);
        AudioScript_ResetSequencePlayer(&gAudioCtx.seqPlayers[i]);
    }

    // Initialize two additional caches on the audio heap to store individual audio samples
    AudioHeap_InitSampleCaches(spec->persistentSampleCacheSize, spec->temporarySampleCacheSize);
    AudioLoad_InitSampleDmaBuffers(gAudioCtx.numNotes);

    // Initalize Loads
    gAudioCtx.preloadSampleStackTop = 0;
    AudioLoad_InitSlowLoads();
    AudioLoad_InitScriptLoads();
    AudioLoad_InitAsyncLoads();
    gAudioCtx.unk_4 = 0x1000;
    AudioLoad_LoadPermanentSamples();

    intMask = osSetIntMask(1);
    osWritebackDCacheAll();
    osSetIntMask(intMask);
}

void* AudioHeap_SearchPermanentCache(s32 tableType, s32 id) {
    s32 i;

    for (i = 0; i < gAudioCtx.permanentPool.count; i++) {
        if (gAudioCtx.permanentEntries[i].tableType == tableType && gAudioCtx.permanentEntries[i].id == id) {
            return gAudioCtx.permanentEntries[i].addr;
        }
    }
    return NULL;
}

void* AudioHeap_AllocPermanent(s32 tableType, s32 id, size_t size) {
    void* addr;
    s32 index = gAudioCtx.permanentPool.count;

    // 2S2H [Custom Audio] permanentEntries is heap allocated (see AudioLoad_Init). Fail the load instead of oob
    if (index >= gAudioCtx.permanentEntriesCapacity) {
        return NULL;
    }

    addr = AudioHeap_Alloc(&gAudioCtx.permanentPool, size);
    gAudioCtx.permanentEntries[index].addr = addr;

    if (addr == NULL) {
        return NULL;
    }

    gAudioCtx.permanentEntries[index].tableType = tableType;
    gAudioCtx.permanentEntries[index].id = id;
    gAudioCtx.permanentEntries[index].size = size;
    //! @bug UB: missing return. "addr" is in v0 at this point, but doing an
    // explicit return uses an additional register.
    // #ifdef AVOID_UB
    return addr;
    // #endif
}

void* AudioHeap_AllocSampleCache(size_t size, s32 sampleBankId, void* sampleAddr, s8 medium, s32 cache) {
    SampleCacheEntry* entry;

    if (cache == CACHE_TEMPORARY) {
        entry = AudioHeap_AllocTemporarySampleCacheEntry(size);
    } else {
        entry = AudioHeap_AllocPersistentSampleCacheEntry(size);
    }

    if (entry != NULL) {
        entry->sampleBankId = sampleBankId;
        entry->sampleAddr = sampleAddr;
        entry->origMedium = medium;
        return entry->allocatedAddr;
    }
    return NULL;
}

/**
 * Initializes the persistent and temporary caches used for individual samples. Will attempt to use heap space available
 * on the external pool. If no external pool is provided, then default to using space on the misc heap.
 */
void AudioHeap_InitSampleCaches(size_t persistentSampleCacheSize, size_t temporarySampleCacheSize) {
    void* addr;

    addr = AudioHeap_AllocAttemptExternal(&gAudioCtx.miscPool, persistentSampleCacheSize);
    if (addr == NULL) {
        gAudioCtx.persistentSampleCache.pool.size = 0;
    } else {
        AudioHeap_InitPool(&gAudioCtx.persistentSampleCache.pool, addr, persistentSampleCacheSize);
    }

    addr = AudioHeap_AllocAttemptExternal(&gAudioCtx.miscPool, temporarySampleCacheSize);
    if (addr == NULL) {
        gAudioCtx.temporarySampleCache.pool.size = 0;
    } else {
        AudioHeap_InitPool(&gAudioCtx.temporarySampleCache.pool, addr, temporarySampleCacheSize);
    }

    gAudioCtx.persistentSampleCache.numEntries = 0;
    gAudioCtx.temporarySampleCache.numEntries = 0;
}

SampleCacheEntry* AudioHeap_AllocTemporarySampleCacheEntry(size_t size) {
    s32 pad2[2];
    void* addr;
    s32 pad3[2];
    u8* allocAfter;
    u8* allocBefore;
    s32 pad1;
    s32 index;
    s32 i;
    SampleCacheEntry* entry;
    AudioPreloadReq* preload;
    AudioSampleCache* cache;
    u8* startAddr;
    u8* endAddr;

    cache = &gAudioCtx.temporarySampleCache;
    allocBefore = cache->pool.curAddr;
    addr = AudioHeap_Alloc(&cache->pool, size);
    if (addr == NULL) {
        // Reset the heap and try again. We still keep pointers to within the
        // heap, so we have to be careful to discard existing overlapping
        // allocations further down.
        u8* oldAddr = cache->pool.curAddr;

        cache->pool.curAddr = cache->pool.startAddr;
        addr = AudioHeap_Alloc(&cache->pool, size);
        if (addr == NULL) {
            cache->pool.curAddr = oldAddr;
            return NULL;
        }
        allocBefore = cache->pool.startAddr;
    }

    allocAfter = cache->pool.curAddr;

    index = -1;
    for (i = 0; i < gAudioCtx.preloadSampleStackTop; i++) {
        preload = &gAudioCtx.preloadSampleStack[i];
        if (preload->isFree == false) {
            startAddr = preload->ramAddr;
            endAddr = preload->ramAddr + preload->sample->size - 1;

            if ((endAddr < allocBefore) && (startAddr < allocBefore)) {
                continue;
            }
            if ((endAddr >= allocAfter) && (startAddr >= allocAfter)) {
                continue;
            }

            // Overlap, skip this preload.
            preload->isFree = true;
        }
    }

    for (i = 0; i < cache->numEntries; i++) {
        if (!cache->entries[i].inUse) {
            continue;
        }

        startAddr = cache->entries[i].allocatedAddr;
        endAddr = startAddr + cache->entries[i].size - 1;

        if ((endAddr < allocBefore) && (startAddr < allocBefore)) {
            continue;
        }
        if ((endAddr >= allocAfter) && (startAddr >= allocAfter)) {
            continue;
        }

        // Overlap, discard existing entry.
        AudioHeap_DiscardSampleCacheEntry(&cache->entries[i]);
        cache->entries[i].inUse = false;
        if (index == -1) {
            index = i;
        }
    }

    if (index == -1) {
        for (i = 0; i < cache->numEntries; i++) {
            if (!cache->entries[i].inUse) {
                break;
            }
        }

        index = i;
        if (index == cache->numEntries) {
            if (cache->numEntries == 128) {
                return NULL;
            }
            cache->numEntries++;
        }
    }

    entry = &cache->entries[index];
    entry->inUse = 1;
    entry->allocatedAddr = addr;
    entry->size = size;

    return entry;
}

void AudioHeap_UnapplySampleCacheForFont(SampleCacheEntry* entry, s32 fontId) {
    Drum* drum;
    Instrument* inst;
    SoundEffect* soundEffect;
    s32 instId;
    s32 drumId;
    s32 sfxId;

    for (instId = 0; instId < gAudioCtx.soundFontList[fontId].numInstruments; instId++) {
        inst = AudioPlayback_GetInstrumentInner(fontId, instId);
        if (inst != NULL) {
            if (inst->normalRangeLo != 0) {
                AudioHeap_UnapplySampleCache(entry, inst->lowPitchTunedSample.sample);
            }
            if (inst->normalRangeHi != 0x7F) {
                AudioHeap_UnapplySampleCache(entry, inst->highPitchTunedSample.sample);
            }
            AudioHeap_UnapplySampleCache(entry, inst->normalPitchTunedSample.sample);
        }
    }

    for (drumId = 0; drumId < gAudioCtx.soundFontList[fontId].numDrums; drumId++) {
        drum = AudioPlayback_GetDrum(fontId, drumId);
        if (drum != NULL) {
            AudioHeap_UnapplySampleCache(entry, drum->tunedSample.sample);
        }
    }

    for (sfxId = 0; sfxId < gAudioCtx.soundFontList[fontId].numSfx; sfxId++) {
        soundEffect = AudioPlayback_GetSoundEffect(fontId, sfxId);
        if (soundEffect != NULL) {
            AudioHeap_UnapplySampleCache(entry, soundEffect->tunedSample.sample);
        }
    }
}

void AudioHeap_DiscardSampleCacheEntry(SampleCacheEntry* entry) {
    s32 numFonts;
    s32 sampleBankId1;
    s32 sampleBankId2;
    s32 fontId;

    numFonts = gAudioCtx.soundFontTable->numEntries;
    for (fontId = 0; fontId < numFonts; fontId++) {
        sampleBankId1 = gAudioCtx.soundFontList[fontId].sampleBankId1;
        sampleBankId2 = gAudioCtx.soundFontList[fontId].sampleBankId2;
        if (((sampleBankId1 != 0xFF) && (entry->sampleBankId == sampleBankId1)) ||
            ((sampleBankId2 != 0xFF) && (entry->sampleBankId == sampleBankId2)) || (entry->sampleBankId == 0) ||
            (entry->sampleBankId == 0xFE)) {
            if (AudioHeap_SearchCaches(FONT_TABLE, CACHE_EITHER, fontId) == NULL) {
                continue;
            }
            if (AudioLoad_IsFontLoadComplete(fontId) != 0) {
                AudioHeap_UnapplySampleCacheForFont(entry, fontId);
            }
        }
    }
}

void AudioHeap_UnapplySampleCache(SampleCacheEntry* entry, Sample* sample) {
    if (sample != NULL) {
        if (sample->sampleAddr == entry->allocatedAddr) {
            sample->sampleAddr = entry->sampleAddr;
            sample->medium = entry->origMedium;
        }
    }
}

SampleCacheEntry* AudioHeap_AllocPersistentSampleCacheEntry(size_t size) {
    AudioSampleCache* cache;
    SampleCacheEntry* entry;
    void* addr;

    cache = &gAudioCtx.persistentSampleCache;
    addr = AudioHeap_Alloc(&cache->pool, size);
    if (addr == NULL) {
        return NULL;
    }

    if (cache->numEntries == 128) {
        return NULL;
    }

    entry = &cache->entries[cache->numEntries];
    entry->inUse = true;
    entry->allocatedAddr = addr;
    entry->size = size;
    cache->numEntries++;

    return entry;
}

void AudioHeap_DiscardSampleCacheForFont(SampleCacheEntry* entry, s32 sampleBankId1, s32 sampleBankId2, s32 fontId) {
    if ((entry->sampleBankId == sampleBankId1) || (entry->sampleBankId == sampleBankId2) ||
        (entry->sampleBankId == 0)) {
        AudioHeap_UnapplySampleCacheForFont(entry, fontId);
    }
}

void AudioHeap_DiscardSampleCaches(void) {
    return;

    s32 numFonts;
    s32 sampleBankId1;
    s32 sampleBankId2;
    s32 fontId;
    s32 j;

    numFonts = gAudioCtx.soundFontTable->numEntries;
    for (fontId = 0; fontId < numFonts; fontId++) {
        sampleBankId1 = gAudioCtx.soundFontList[fontId].sampleBankId1;
        sampleBankId2 = gAudioCtx.soundFontList[fontId].sampleBankId2;
        if ((sampleBankId1 == 0xFF) && (sampleBankId2 == 0xFF)) {
            continue;
        }
        if ((AudioHeap_SearchCaches(FONT_TABLE, CACHE_PERMANENT, fontId) == NULL) ||
            !AudioLoad_IsFontLoadComplete(fontId)) {
            continue;
        }

        for (j = 0; j < gAudioCtx.persistentSampleCache.numEntries; j++) {
            AudioHeap_DiscardSampleCacheForFont(&gAudioCtx.persistentSampleCache.entries[j], sampleBankId1,
                                                sampleBankId2, fontId);
        }
        for (j = 0; j < gAudioCtx.temporarySampleCache.numEntries; j++) {
            AudioHeap_DiscardSampleCacheForFont(&gAudioCtx.temporarySampleCache.entries[j], sampleBankId1,
                                                sampleBankId2, fontId);
        }
    }
}

typedef struct {
    /* 0x0 */ uintptr_t oldAddr;
    /* 0x4 */ uintptr_t newAddr;
    /* 0x8 */ size_t size;
    /* 0xC */ u8 newMedium;
} StorageChange; // size = 0x10

void AudioHeap_ChangeStorage(StorageChange* change, Sample* sample) {
    if ((sample != NULL) && ((sample->medium == change->newMedium) || (D_801FD120 != 1)) &&
        ((sample->medium == MEDIUM_RAM) || (D_801FD120 != 0))) {
        uintptr_t startAddr = change->oldAddr;
        uintptr_t endAddr = change->oldAddr + change->size;

        if (((uintptr_t)sample->sampleAddr >= startAddr) && ((uintptr_t)sample->sampleAddr < endAddr)) {
            sample->sampleAddr += -startAddr + change->newAddr;
            if (D_801FD120 == 0) {
                sample->medium = change->newMedium;
            } else {
                sample->medium = MEDIUM_RAM;
            }
        }
    }
}

void AudioHeap_DiscardSampleBank(s32 sampleBankId) {
    D_801FD120 = 0;
    AudioHeap_ApplySampleBankCacheInternal(false, sampleBankId);
}

void AudioHeap_ApplySampleBankCache(s32 sampleBankId) {
    D_801FD120 = 1;
    AudioHeap_ApplySampleBankCacheInternal(true, sampleBankId);
}

void AudioHeap_ApplySampleBankCacheInternal(s32 apply, s32 sampleBankId) {
    AudioTable* sampleBankTable;
    AudioTableEntry* entry;
    s32 numFonts;
    s32 instId;
    s32 drumId;
    s32 sfxId;
    StorageChange change;
    s32 sampleBankId1;
    s32 sampleBankId2;
    s32 fontId;
    Drum* drum;
    Instrument* inst;
    SoundEffect* soundEffect;
    uintptr_t* newAddr;
    s32 pad[4];

    sampleBankTable = gAudioCtx.sampleBankTable;
    numFonts = gAudioCtx.soundFontTable->numEntries;
    change.oldAddr = (uintptr_t)AudioHeap_SearchCaches(SAMPLE_TABLE, CACHE_EITHER, sampleBankId);
    if (change.oldAddr == 0) {
        return;
    }

    entry = &sampleBankTable->entries[sampleBankId];
    change.size = entry->size;
    change.newMedium = entry->medium;
    change.newAddr = entry->romAddr;

    newAddr = &change.oldAddr;
    if (apply && (apply == true)) {
        uintptr_t oldAddr;

        oldAddr = change.newAddr;

        change.newAddr = *newAddr;
        change.oldAddr = oldAddr;
    }

    for (fontId = 0; fontId < numFonts; fontId++) {
        sampleBankId1 = gAudioCtx.soundFontList[fontId].sampleBankId1;
        sampleBankId2 = gAudioCtx.soundFontList[fontId].sampleBankId2;
        if ((sampleBankId1 != 0xFF) || (sampleBankId2 != 0xFF)) {
            if (!AudioLoad_IsFontLoadComplete(fontId) ||
                AudioHeap_SearchCaches(FONT_TABLE, CACHE_EITHER, fontId) == NULL) {
                continue;
            }

            if (sampleBankId1 == sampleBankId) {
            } else if (sampleBankId2 == sampleBankId) {
            } else {
                continue;
            }

            for (instId = 0; instId < gAudioCtx.soundFontList[fontId].numInstruments; instId++) {
                inst = AudioPlayback_GetInstrumentInner(fontId, instId);
                if (inst != NULL) {
                    if (inst->normalRangeLo != 0) {
                        AudioHeap_ChangeStorage(&change, inst->lowPitchTunedSample.sample);
                    }
                    if (inst->normalRangeHi != 0x7F) {
                        AudioHeap_ChangeStorage(&change, inst->highPitchTunedSample.sample);
                    }
                    AudioHeap_ChangeStorage(&change, inst->normalPitchTunedSample.sample);
                }
            }

            for (drumId = 0; drumId < gAudioCtx.soundFontList[fontId].numDrums; drumId++) {
                drum = AudioPlayback_GetDrum(fontId, drumId);
                if (drum != NULL) {
                    AudioHeap_ChangeStorage(&change, drum->tunedSample.sample);
                }
            }

            for (sfxId = 0; sfxId < gAudioCtx.soundFontList[fontId].numSfx; sfxId++) {
                soundEffect = AudioPlayback_GetSoundEffect(fontId, sfxId);
                if (soundEffect != NULL) {
                    AudioHeap_ChangeStorage(&change, soundEffect->tunedSample.sample);
                }
            }
        }
    }
}

void AudioHeap_DiscardSampleBanks(void) {
    AudioCache* cache;
    AudioPersistentCache* persistent;
    AudioTemporaryCache* temporary;
    u32 i;

    cache = &gAudioCtx.sampleBankCache;
    temporary = &cache->temporary;

    if (temporary->entries[0].id != -1) {
        AudioHeap_DiscardSampleBank(temporary->entries[0].id);
    }

    if (temporary->entries[1].id != -1) {
        AudioHeap_DiscardSampleBank(temporary->entries[1].id);
    }

    persistent = &cache->persistent;
    for (i = 0; i < persistent->numEntries; i++) {
        AudioHeap_DiscardSampleBank(persistent->entries[i].id);
    }
}

void AudioHeap_SetReverbData(s32 reverbIndex, u32 dataType, uintptr_t data, s32 isFirstInit) {
    s32 delayNumSamples;
    SynthesisReverb* reverb = &gAudioCtx.synthesisReverbs[reverbIndex];

    switch (dataType) {
        case REVERB_DATA_TYPE_SETTINGS:
            AudioHeap_InitReverb(reverbIndex, (ReverbSettings*)data, false);
            break;

        case REVERB_DATA_TYPE_DELAY:
            if (data < 4) {
                data = 4;
            }

            delayNumSamples = data * 64;
            if (delayNumSamples < (16 * SAMPLES_PER_FRAME)) {
                delayNumSamples = 16 * SAMPLES_PER_FRAME;
            }

            delayNumSamples /= reverb->downsampleRate;

            if (!isFirstInit) {
                if (reverb->delayNumSamplesAfterDownsampling < (data / reverb->downsampleRate)) {
                    break;
                }
                if ((reverb->nextReverbBufPos >= delayNumSamples) || (reverb->delayNumSamplesUnk >= delayNumSamples)) {
                    reverb->nextReverbBufPos = 0;
                    reverb->delayNumSamplesUnk = 0;
                }
            }

            reverb->delayNumSamples = delayNumSamples;

            if ((reverb->downsampleRate != 1) || reverb->resampleEffectOn) {
                reverb->downsamplePitch = 0x8000 / reverb->downsampleRate;
                if (reverb->leftLoadResampleBuf == NULL) {
                    reverb->leftLoadResampleBuf = AudioHeap_AllocZeroed(&gAudioCtx.miscPool, sizeof(RESAMPLE_STATE));
                    reverb->rightLoadResampleBuf = AudioHeap_AllocZeroed(&gAudioCtx.miscPool, sizeof(RESAMPLE_STATE));
                    reverb->leftSaveResampleBuf = AudioHeap_AllocZeroed(&gAudioCtx.miscPool, sizeof(RESAMPLE_STATE));
                    reverb->rightSaveResampleBuf = AudioHeap_AllocZeroed(&gAudioCtx.miscPool, sizeof(RESAMPLE_STATE));
                    if (reverb->rightSaveResampleBuf == NULL) {
                        reverb->downsampleRate = 1;
                    }
                }
            }
            break;

        case REVERB_DATA_TYPE_DECAY:
            gAudioCtx.synthesisReverbs[reverbIndex].decayRatio = data;
            break;

        case REVERB_DATA_TYPE_SUB_VOLUME:
            gAudioCtx.synthesisReverbs[reverbIndex].subVolume = data;
            break;

        case REVERB_DATA_TYPE_VOLUME:
            gAudioCtx.synthesisReverbs[reverbIndex].volume = data;
            break;

        case REVERB_DATA_TYPE_LEAK_RIGHT:
            gAudioCtx.synthesisReverbs[reverbIndex].leakRtl = data;
            break;

        case REVERB_DATA_TYPE_LEAK_LEFT:
            gAudioCtx.synthesisReverbs[reverbIndex].leakLtr = data;
            break;

        case REVERB_DATA_TYPE_FILTER_LEFT:
            if (data != 0) {
                if (isFirstInit || (reverb->filterLeftInit == NULL)) {
                    reverb->filterLeftState =
                        AudioHeap_AllocDmaMemoryZeroed(&gAudioCtx.miscPool, 2 * (FILTER_BUF_PART1 + FILTER_BUF_PART2));
                    reverb->filterLeftInit = AudioHeap_AllocDmaMemory(&gAudioCtx.miscPool, FILTER_SIZE);
                }

                reverb->filterLeft = reverb->filterLeftInit;
                if (reverb->filterLeft != NULL) {
                    AudioHeap_LoadLowPassFilter(reverb->filterLeft, data);
                }
            } else {
                reverb->filterLeft = NULL;

                if (isFirstInit) {
                    reverb->filterLeftInit = NULL;
                }
            }
            break;

        case REVERB_DATA_TYPE_FILTER_RIGHT:
            if (data != 0) {
                if (isFirstInit || (reverb->filterRightInit == NULL)) {
                    reverb->filterRightState =
                        AudioHeap_AllocDmaMemoryZeroed(&gAudioCtx.miscPool, 2 * (FILTER_BUF_PART1 + FILTER_BUF_PART2));
                    reverb->filterRightInit = AudioHeap_AllocDmaMemory(&gAudioCtx.miscPool, FILTER_SIZE);
                }
                reverb->filterRight = reverb->filterRightInit;
                if (reverb->filterRight != NULL) {
                    AudioHeap_LoadLowPassFilter(reverb->filterRight, data);
                }
            } else {
                reverb->filterRight = NULL;
                if (isFirstInit) {
                    reverb->filterRightInit = NULL;
                }
            }
            break;

        case REVERB_DATA_TYPE_9:
            reverb->resampleEffectExtraSamples = data;
            if (data == 0) {
                reverb->resampleEffectOn = false;
            } else {
                reverb->resampleEffectOn = true;
            }
            break;

        default:
            break;
    }
}

void AudioHeap_InitReverb(s32 reverbIndex, ReverbSettings* settings, s32 isFirstInit) {
    SynthesisReverb* reverb = &gAudioCtx.synthesisReverbs[reverbIndex];

    if (isFirstInit) {
        reverb->delayNumSamplesAfterDownsampling = settings->delayNumSamples / settings->downsampleRate;
        reverb->leftLoadResampleBuf = NULL;
    } else if (reverb->delayNumSamplesAfterDownsampling < (settings->delayNumSamples / settings->downsampleRate)) {
        return;
    }

    reverb->downsampleRate = settings->downsampleRate;
    reverb->resampleEffectOn = false;
    reverb->resampleEffectExtraSamples = 0;
    reverb->resampleEffectLoadUnk = 0;
    reverb->resampleEffectSaveUnk = 0;
    AudioHeap_SetReverbData(reverbIndex, REVERB_DATA_TYPE_DELAY, settings->delayNumSamples, isFirstInit);
    reverb->decayRatio = settings->decayRatio;
    reverb->volume = settings->volume;
    reverb->subDelay = settings->subDelay * 64;
    reverb->subVolume = settings->subVolume;
    reverb->leakRtl = settings->leakRtl;
    reverb->leakLtr = settings->leakLtr;
    reverb->mixReverbIndex = settings->mixReverbIndex;
    reverb->mixReverbStrength = settings->mixReverbStrength;
    reverb->useReverb = 8; // used as a boolean

    if (isFirstInit) {
        reverb->leftReverbBuf = AudioHeap_AllocZeroedAttemptExternal(&gAudioCtx.miscPool, reverb->delayNumSamples * 2);
        reverb->rightReverbBuf = AudioHeap_AllocZeroedAttemptExternal(&gAudioCtx.miscPool, reverb->delayNumSamples * 2);
        reverb->resampleFlags = 1;
        reverb->nextReverbBufPos = 0;
        reverb->delayNumSamplesUnk = 0;
        reverb->curFrame = 0;
        reverb->framesToIgnore = 2;
    }

    reverb->tunedSample.sample = &reverb->sample;
    reverb->sample.loop = &reverb->loop;
    reverb->tunedSample.tuning = 1.0f;
    reverb->sample.codec = CODEC_REVERB;
    reverb->sample.medium = MEDIUM_RAM;
    reverb->sample.size = reverb->delayNumSamples * SAMPLE_SIZE;
    reverb->sample.sampleAddr = (u8*)reverb->leftReverbBuf;
    reverb->loop.start = 0;
    reverb->loop.count = 1;
    reverb->loop.loopEnd = reverb->delayNumSamples;

    AudioHeap_SetReverbData(reverbIndex, REVERB_DATA_TYPE_FILTER_LEFT, settings->lowPassFilterCutoffLeft, isFirstInit);
    AudioHeap_SetReverbData(reverbIndex, REVERB_DATA_TYPE_FILTER_RIGHT, settings->lowPassFilterCutoffRight,
                            isFirstInit);
}
