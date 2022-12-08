#include "ultra64.h"
#include "global.h"

void AudioHeap_InitSampleCaches(u32 persistentSize, u32 temporarySize);
SampleCacheEntry* AudioHeap_AllocTemporarySampleCacheEntry(size_t size);
SampleCacheEntry* AudioHeap_AllocPersistentSampleCacheEntry(size_t size);
void AudioHeap_DiscardSampleCacheEntry(SampleCacheEntry* entry);
void AudioHeap_UnapplySampleCache(SampleCacheEntry* entry, SoundFontSample* sample);
void AudioHeap_DiscardSampleCaches(void);
void AudioHeap_DiscardSampleBank(s32 sampleBankId);
void AudioHeap_DiscardSampleBanks(void);

f32 func_800DDE20(f32 arg0) {
    return 256.0f * gAudioContext.audioBufferParameters.unkUpdatesPerFrameScaled / arg0;
}

void func_800DDE3C(void) {
    s32 i;

    gAudioContext.unk_3520[255] = func_800DDE20(0.25f);
    gAudioContext.unk_3520[254] = func_800DDE20(0.33f);
    gAudioContext.unk_3520[253] = func_800DDE20(0.5f);
    gAudioContext.unk_3520[252] = func_800DDE20(0.66f);
    gAudioContext.unk_3520[251] = func_800DDE20(0.75f);

    for (i = 128; i < 251; i++) {
        gAudioContext.unk_3520[i] = func_800DDE20(251 - i);
    }

    for (i = 16; i < 128; i++) {
        gAudioContext.unk_3520[i] = func_800DDE20(4 * (143 - i));
    }

    for (i = 1; i < 16; i++) {
        gAudioContext.unk_3520[i] = func_800DDE20(60 * (23 - i));
    }

    gAudioContext.unk_3520[0] = 0.0f;
}

void AudioHeap_ResetLoadStatus(void) {
    s32 i;

    for (i = 0; i < 0x30; i++) {
        if (gAudioContext.fontLoadStatus[i] != 5) {
            gAudioContext.fontLoadStatus[i] = 0;
        }
    }

    for (i = 0; i < 0x30; i++) {
        if (gAudioContext.sampleFontLoadStatus[i] != 5) {
            gAudioContext.sampleFontLoadStatus[i] = 0;
        }
    }

    for (i = 0; i < MAX_SEQUENCES; i++) {
        if (gAudioContext.seqLoadStatus[i] != 5) {
            gAudioContext.seqLoadStatus[i] = 0;
        }
    }
}

void AudioHeap_DiscardFont(s32 fontId) {
    s32 i;

    for (i = 0; i < gAudioContext.numNotes; i++) {
        Note* note = &gAudioContext.notes[i];

        if (note->playbackState.fontId == fontId) {
            if (note->playbackState.unk_04 == 0 && note->playbackState.priority != 0) {
                note->playbackState.parentLayer->enabled = false;
                note->playbackState.parentLayer->finished = true;
            }
            Audio_NoteDisable(note);
            Audio_AudioListRemove(&note->listItem);
            AudioSeq_AudioListPushBack(&gAudioContext.noteFreeLists.disabled, &note->listItem);
        }
    }
}

void AudioHeap_ReleaseNotesForFont(s32 fontId) {
    s32 i;

    for (i = 0; i < gAudioContext.numNotes; i++) {
        Note* note = &gAudioContext.notes[i];
        NotePlaybackState* state = &note->playbackState;

        if (state->fontId == fontId) {
            if (state->priority != 0 && state->adsr.action.s.state == ADSR_STATE_DECAY) {
                state->priority = 1;
                state->adsr.fadeOutVel = gAudioContext.audioBufferParameters.updatesPerFrameInv;
                state->adsr.action.s.release = true;
            }
        }
    }
}

void AudioHeap_DiscardSequence(s32 seqId) {
    s32 i;

    for (i = 0; i < gAudioContext.audioBufferParameters.numSequencePlayers; i++) {
        if (gAudioContext.seqPlayers[i].enabled && gAudioContext.seqPlayers[i].seqId == seqId) {
            AudioSeq_SequencePlayerDisable(&gAudioContext.seqPlayers[i]);
        }
    }
}

void AudioHeap_WritebackDCache(void* mem, size_t size) {
    Audio_WritebackDCache(mem, size);
}

void* AudioHeap_AllocZeroedAttemptExternal(AudioAllocPool* pool, size_t size) {
    void* ret = NULL;

    if (gAudioContext.externalPool.start != 0) {
        ret = AudioHeap_AllocZeroed(&gAudioContext.externalPool, size);
    }
    if (ret == NULL) {
        ret = AudioHeap_AllocZeroed(pool, size);
    }
    return ret;
}

void* AudioHeap_AllocAttemptExternal(AudioAllocPool* pool, size_t size) {
    void* ret = NULL;

    if (gAudioContext.externalPool.start != NULL) {
        ret = AudioHeap_Alloc(&gAudioContext.externalPool, size);
    }
    if (ret == NULL) {
        ret = AudioHeap_Alloc(pool, size);
    }
    return ret;
}

void* AudioHeap_AllocDmaMemory(AudioAllocPool* pool, size_t size) {
    void* ret;

    ret = AudioHeap_Alloc(pool, size);
    if (ret != NULL) {
        AudioHeap_WritebackDCache(ret, size);
    }
    return ret;
}

void* AudioHeap_AllocDmaMemoryZeroed(AudioAllocPool* pool, size_t size) {
    void* ret;

    ret = AudioHeap_AllocZeroed(pool, size);
    if (ret != NULL) {
        AudioHeap_WritebackDCache(ret, size);
    }
    return ret;
}

void* AudioHeap_AllocZeroed(AudioAllocPool* pool, size_t size) {
    u8* ret = AudioHeap_Alloc(pool, size);
    u8* ptr;

    if (ret != NULL) {
        for (ptr = ret; ptr < pool->cur; ptr++) {
            *ptr = 0;
        }
    }

    return ret;
}

void* AudioHeap_Alloc(AudioAllocPool* pool, size_t size) {
    u32 aligned = ALIGN16(size);
    u8* ret = pool->cur;

    if (pool->start + pool->size >= pool->cur + aligned) {
        pool->cur += aligned;
    } else {
        return NULL;
    }
    pool->count++;
    return ret;
}

void AudioHeap_AllocPoolInit(AudioAllocPool* pool, void* mem, size_t size) {
    pool->cur = pool->start = (u8*)ALIGN16((uintptr_t)mem);
    pool->size = size - ((uintptr_t)mem & 0xF);
    pool->count = 0;
}

void AudioHeap_PersistentCacheClear(AudioPersistentCache* persistent) {
    persistent->pool.count = 0;
    persistent->numEntries = 0;
    persistent->pool.cur = persistent->pool.start;
}

void AudioHeap_TemporaryCacheClear(AudioTemporaryCache* temporary) {
    temporary->pool.count = 0;
    temporary->pool.cur = temporary->pool.start;
    temporary->nextSide = 0;
    temporary->entries[0].ptr = temporary->pool.start;
    temporary->entries[1].ptr = temporary->pool.start + temporary->pool.size;
    temporary->entries[0].id = -1;
    temporary->entries[1].id = -1;
}

void AudioHeap_ResetPool(AudioAllocPool* pool) {
    pool->count = 0;
    pool->cur = pool->start;
}

void AudioHeap_PopCache(s32 tableType) {
    AudioCache* loadedPool;
    AudioAllocPool* persistentPool;
    AudioPersistentCache* persistent;
    void* entryPtr;
    u8* table;

    switch (tableType) {
        case SEQUENCE_TABLE:
            loadedPool = &gAudioContext.seqCache;
            table = gAudioContext.seqLoadStatus;
            break;
        case FONT_TABLE:
            loadedPool = &gAudioContext.fontCache;
            table = gAudioContext.fontLoadStatus;
            break;
        case SAMPLE_TABLE:
            loadedPool = &gAudioContext.sampleBankCache;
            table = gAudioContext.sampleFontLoadStatus;
            break;
    }

    persistent = &loadedPool->persistent;
    persistentPool = &persistent->pool;

    if (persistent->numEntries == 0) {
        return;
    }

    entryPtr = persistent->entries[persistent->numEntries - 1].ptr;
    persistentPool->cur = entryPtr;
    persistentPool->count--;

    if (tableType == SAMPLE_TABLE) {
        AudioHeap_DiscardSampleBank(persistent->entries[persistent->numEntries - 1].id);
    }
    if (tableType == FONT_TABLE) {
        AudioHeap_DiscardFont(persistent->entries[persistent->numEntries - 1].id);
    }
    table[persistent->entries[persistent->numEntries - 1].id] = 0;
    persistent->numEntries--;
}

void AudioHeap_InitMainPools(size_t initPoolSize) {
    AudioHeap_AllocPoolInit(&gAudioContext.audioInitPool, gAudioContext.audioHeap, initPoolSize);
    AudioHeap_AllocPoolInit(&gAudioContext.audioSessionPool, gAudioContext.audioHeap + initPoolSize,
                            gAudioContext.audioHeapSize - initPoolSize);
    gAudioContext.externalPool.start = NULL;
}

void AudioHeap_SessionPoolsInit(AudioPoolSplit4* split) {
    gAudioContext.audioSessionPool.cur = gAudioContext.audioSessionPool.start;
    AudioHeap_AllocPoolInit(&gAudioContext.notesAndBuffersPool,
                            AudioHeap_Alloc(&gAudioContext.audioSessionPool, split->wantSeq), split->wantSeq);
    AudioHeap_AllocPoolInit(&gAudioContext.cachePool,
                            AudioHeap_Alloc(&gAudioContext.audioSessionPool, split->wantCustom), split->wantCustom);
}

void AudioHeap_CachePoolInit(AudioPoolSplit2* split) {
    gAudioContext.cachePool.cur = gAudioContext.cachePool.start;
    AudioHeap_AllocPoolInit(&gAudioContext.persistentCommonPool,
                            AudioHeap_Alloc(&gAudioContext.cachePool, split->wantPersistent), split->wantPersistent);
    AudioHeap_AllocPoolInit(&gAudioContext.temporaryCommonPool,
                            AudioHeap_Alloc(&gAudioContext.cachePool, split->wantTemporary), split->wantTemporary);
}

void AudioHeap_PersistentCachesInit(AudioPoolSplit3* split) {
    gAudioContext.persistentCommonPool.cur = gAudioContext.persistentCommonPool.start;
    AudioHeap_AllocPoolInit(&gAudioContext.seqCache.persistent.pool,
                            AudioHeap_Alloc(&gAudioContext.persistentCommonPool, split->wantSeq), split->wantSeq);
    AudioHeap_AllocPoolInit(&gAudioContext.fontCache.persistent.pool,
                            AudioHeap_Alloc(&gAudioContext.persistentCommonPool, split->wantFont), split->wantFont);
    AudioHeap_AllocPoolInit(&gAudioContext.sampleBankCache.persistent.pool,
                            AudioHeap_Alloc(&gAudioContext.persistentCommonPool, split->wantSample), split->wantSample);
    AudioHeap_PersistentCacheClear(&gAudioContext.seqCache.persistent);
    AudioHeap_PersistentCacheClear(&gAudioContext.fontCache.persistent);
    AudioHeap_PersistentCacheClear(&gAudioContext.sampleBankCache.persistent);
}

void AudioHeap_TemporaryCachesInit(AudioPoolSplit3* split) {
    gAudioContext.temporaryCommonPool.cur = gAudioContext.temporaryCommonPool.start;
    AudioHeap_AllocPoolInit(&gAudioContext.seqCache.temporary.pool,
                            AudioHeap_Alloc(&gAudioContext.temporaryCommonPool, split->wantSeq), split->wantSeq);
    AudioHeap_AllocPoolInit(&gAudioContext.fontCache.temporary.pool,
                            AudioHeap_Alloc(&gAudioContext.temporaryCommonPool, split->wantFont), split->wantFont);
    AudioHeap_AllocPoolInit(&gAudioContext.sampleBankCache.temporary.pool,
                            AudioHeap_Alloc(&gAudioContext.temporaryCommonPool, split->wantSample), split->wantSample);
    AudioHeap_TemporaryCacheClear(&gAudioContext.seqCache.temporary);
    AudioHeap_TemporaryCacheClear(&gAudioContext.fontCache.temporary);
    AudioHeap_TemporaryCacheClear(&gAudioContext.sampleBankCache.temporary);
}

void* AudioHeap_AllocCached(s32 tableType, ptrdiff_t size, s32 cache, s32 id) {
    AudioCache* loadedPool;
    AudioTemporaryCache* tp;
    AudioAllocPool* pool;
    void* mem;
    void* ret;
    u8 firstVal;
    u8 secondVal;
    s32 i;
    u8* table;
    s32 side;

    switch (tableType) {
        case SEQUENCE_TABLE:
            loadedPool = &gAudioContext.seqCache;
            table = gAudioContext.seqLoadStatus;
            break;
        case FONT_TABLE:
            loadedPool = &gAudioContext.fontCache;
            table = gAudioContext.fontLoadStatus;
            break;
        case SAMPLE_TABLE:
            loadedPool = &gAudioContext.sampleBankCache;
            table = gAudioContext.sampleFontLoadStatus;
            break;
    }

    if (cache == CACHE_TEMPORARY) {
        tp = &loadedPool->temporary;
        pool = &tp->pool;

        if (pool->size < size) {
            return NULL;
        }

        firstVal = (tp->entries[0].id == -1) ? 0 : table[tp->entries[0].id];
        secondVal = (tp->entries[1].id == -1) ? 0 : table[tp->entries[1].id];

        if (tableType == FONT_TABLE) {
            if (firstVal == 4) {
                for (i = 0; i < gAudioContext.numNotes; i++) {
                    if (gAudioContext.notes[i].playbackState.fontId == tp->entries[0].id &&
                        gAudioContext.notes[i].noteSubEu.bitField0.enabled != 0) {
                        break;
                    }
                }

                if (i == gAudioContext.numNotes) {
                    AudioLoad_SetFontLoadStatus(tp->entries[0].id, 3);
                    firstVal = 3;
                }
            }

            if (secondVal == 4) {
                for (i = 0; i < gAudioContext.numNotes; i++) {
                    if (gAudioContext.notes[i].playbackState.fontId == tp->entries[1].id &&
                        gAudioContext.notes[i].noteSubEu.bitField0.enabled != 0) {
                        break;
                    }
                }

                if (i == gAudioContext.numNotes) {
                    AudioLoad_SetFontLoadStatus(tp->entries[1].id, 3);
                    secondVal = 3;
                }
            }
        }

        if (firstVal == 0) {
            tp->nextSide = 0;
        } else if (secondVal == 0) {
            tp->nextSide = 1;
        } else if (firstVal == 3 && secondVal == 3) {
            // Use the opposite side from last time.
        } else if (firstVal == 3) {
            tp->nextSide = 0;
        } else if (secondVal == 3) {
            tp->nextSide = 1;
        } else {
            // Check if there is a side which isn't in active use, if so, evict that one.
            if (tableType == SEQUENCE_TABLE) {
                if (firstVal == 2) {
                    for (i = 0; i < gAudioContext.audioBufferParameters.numSequencePlayers; i++) {
                        if (gAudioContext.seqPlayers[i].enabled != 0 &&
                            gAudioContext.seqPlayers[i].seqId == tp->entries[0].id) {
                            break;
                        }
                    }

                    if (i == gAudioContext.audioBufferParameters.numSequencePlayers) {
                        tp->nextSide = 0;
                        goto done;
                    }
                }

                if (secondVal == 2) {
                    for (i = 0; i < gAudioContext.audioBufferParameters.numSequencePlayers; i++) {
                        if (gAudioContext.seqPlayers[i].enabled != 0 &&
                            gAudioContext.seqPlayers[i].seqId == tp->entries[1].id) {
                            break;
                        }
                    }

                    if (i == gAudioContext.audioBufferParameters.numSequencePlayers) {
                        tp->nextSide = 1;
                        goto done;
                    }
                }
            } else if (tableType == FONT_TABLE) {
                if (firstVal == 2) {
                    for (i = 0; i < gAudioContext.numNotes; i++) {
                        if (gAudioContext.notes[i].playbackState.fontId == tp->entries[0].id &&
                            gAudioContext.notes[i].noteSubEu.bitField0.enabled != 0) {
                            break;
                        }
                    }
                    if (i == gAudioContext.numNotes) {
                        tp->nextSide = 0;
                        goto done;
                    }
                }

                if (secondVal == 2) {
                    for (i = 0; i < gAudioContext.numNotes; i++) {
                        if (gAudioContext.notes[i].playbackState.fontId == tp->entries[1].id &&
                            gAudioContext.notes[i].noteSubEu.bitField0.enabled != 0) {
                            break;
                        }
                    }
                    if (i == gAudioContext.numNotes) {
                        tp->nextSide = 1;
                        goto done;
                    }
                }
            }

            // No such luck. Evict the side that wasn't chosen last time, except
            // if it is being loaded into.
            if (tp->nextSide == 0) {
                if (firstVal == 1) {
                    if (secondVal == 1) {
                        goto fail;
                    }
                    tp->nextSide = 1;
                }
            } else {
                if (secondVal == 1) {
                    if (firstVal == 1) {
                        goto fail;
                    }
                    tp->nextSide = 0;
                }
            }

            if (0) {
            fail:
                // Both sides are being loaded into.
                return NULL;
            }
        }
    done:

        side = tp->nextSide;

        if (tp->entries[side].id != -1) {
            if (tableType == SAMPLE_TABLE) {
                AudioHeap_DiscardSampleBank(tp->entries[side].id);
            }
            table[tp->entries[side].id] = 0;
            if (tableType == FONT_TABLE) {
                AudioHeap_DiscardFont(tp->entries[side].id);
            }
        }

        switch (side) {
            case 0:
                tp->entries[0].ptr = pool->start;
                tp->entries[0].id = id;
                tp->entries[0].size = size;
                pool->cur = pool->start + size;

                if (tp->entries[1].id != -1 && tp->entries[1].ptr < pool->cur) {
                    if (tableType == SAMPLE_TABLE) {
                        AudioHeap_DiscardSampleBank(tp->entries[1].id);
                    }

                    table[tp->entries[1].id] = 0;
                    switch (tableType) {
                        case SEQUENCE_TABLE:
                            AudioHeap_DiscardSequence((s32)tp->entries[1].id);
                            break;
                        case FONT_TABLE:
                            AudioHeap_DiscardFont((s32)tp->entries[1].id);
                            break;
                    }

                    tp->entries[1].id = -1;
                    tp->entries[1].ptr = pool->start + pool->size;
                }

                ret = tp->entries[0].ptr;
                break;

            case 1:
                tp->entries[1].ptr = (u8*)((uintptr_t)(pool->start + pool->size - size) & ~0xF);
                tp->entries[1].id = id;
                tp->entries[1].size = size;
                if (tp->entries[0].id != -1 && tp->entries[1].ptr < pool->cur) {
                    if (tableType == SAMPLE_TABLE) {
                        AudioHeap_DiscardSampleBank(tp->entries[0].id);
                    }

                    table[tp->entries[0].id] = 0;
                    switch (tableType) {
                        case SEQUENCE_TABLE:
                            AudioHeap_DiscardSequence(tp->entries[0].id);
                            break;
                        case FONT_TABLE:
                            AudioHeap_DiscardFont(tp->entries[0].id);
                            break;
                    }

                    tp->entries[0].id = -1;
                    pool->cur = pool->start;
                }
                ret = tp->entries[1].ptr;
                break;

            default:
                return NULL;
        }

        tp->nextSide ^= 1;
        return ret;
    }

    mem = AudioHeap_Alloc(&loadedPool->persistent.pool, size);
    loadedPool->persistent.entries[loadedPool->persistent.numEntries].ptr = mem;

    if (mem == NULL) {
        switch (cache) {
            case CACHE_EITHER:
                return AudioHeap_AllocCached(tableType, size, CACHE_TEMPORARY, id);

            case CACHE_TEMPORARY:
            case CACHE_PERSISTENT:
                return NULL;
        }
    }

    loadedPool->persistent.entries[loadedPool->persistent.numEntries].id = id;
    loadedPool->persistent.entries[loadedPool->persistent.numEntries].size = size;
    return loadedPool->persistent.entries[loadedPool->persistent.numEntries++].ptr;
}

void* AudioHeap_SearchCaches(s32 tableType, s32 cache, s32 id) {
    void* ret;

    // Always search the permanent cache in addition to the regular ones.
    ret = AudioHeap_SearchPermanentCache(tableType, id);
    if (ret != NULL) {
        return ret;
    }
    if (cache == CACHE_PERMANENT) {
        return NULL;
    }
    return AudioHeap_SearchRegularCaches(tableType, cache, id);
}

void* AudioHeap_SearchRegularCaches(s32 tableType, s32 cache, s32 id) {
    u32 i;
    AudioCache* loadedPool;
    AudioTemporaryCache* temporary;
    AudioPersistentCache* persistent;

    switch (tableType) {
        case SEQUENCE_TABLE:
            loadedPool = &gAudioContext.seqCache;
            break;
        case FONT_TABLE:
            loadedPool = &gAudioContext.fontCache;
            break;
        case SAMPLE_TABLE:
            loadedPool = &gAudioContext.sampleBankCache;
            break;
    }

    temporary = &loadedPool->temporary;
    if (cache == CACHE_TEMPORARY) {
        if (temporary->entries[0].id == id) {
            temporary->nextSide = 1;
            return temporary->entries[0].ptr;
        } else if (temporary->entries[1].id == id) {
            temporary->nextSide = 0;
            return temporary->entries[1].ptr;
        } else {
            return NULL;
        }
    }

    persistent = &loadedPool->persistent;
    for (i = 0; i < persistent->numEntries; i++) {
        if (persistent->entries[i].id == id) {
            return persistent->entries[i].ptr;
        }
    }

    if (cache == CACHE_EITHER) {
        return AudioHeap_SearchCaches(tableType, CACHE_TEMPORARY, id);
    }
    return NULL;
}

void func_800DF1D8(f32 p, f32 q, u16* out) {
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
    s16* ptr = &sLowPassFilterData[8 * cutoff];

    for (i = 0; i < 8; i++) {
        filter[i] = ptr[i];
    }
}

void AudioHeap_LoadHighPassFilter(s16* filter, s32 cutoff) {
    s32 i;
    s16* ptr = &sHighPassFilterData[8 * (cutoff - 1)];

    for (i = 0; i < 8; i++) {
        filter[i] = ptr[i];
    }
}

void AudioHeap_LoadFilter(s16* filter, s32 lowPassCutoff, s32 highPassCutoff) {
    s32 i;

    if (lowPassCutoff == 0 && highPassCutoff == 0) {
        // Identity filter
        AudioHeap_LoadLowPassFilter(filter, 0);
    } else if (highPassCutoff == 0) {
        AudioHeap_LoadLowPassFilter(filter, lowPassCutoff);
    } else if (lowPassCutoff == 0) {
        AudioHeap_LoadHighPassFilter(filter, highPassCutoff);
    } else {
        s16* ptr1 = &sLowPassFilterData[8 * lowPassCutoff];
        s16* ptr2 = &sHighPassFilterData[8 * (highPassCutoff - 1)];
        for (i = 0; i < 8; i++) {
            filter[i] = (ptr1[i] + ptr2[i]) / 2;
        }
    }
}

void AudioHeap_UpdateReverb(SynthesisReverb* reverb) {
}

void AudioHeap_UpdateReverbs(void) {
    s32 count;
    s32 i;
    s32 j;

    if (gAudioContext.audioBufferParameters.specUnk4 == 2) {
        count = 2;
    } else {
        count = 1;
    }

    for (i = 0; i < gAudioContext.numSynthesisReverbs; i++) {
        for (j = 0; j < count; j++) {
            AudioHeap_UpdateReverb(&gAudioContext.synthesisReverbs[i]);
        }
    }
}

void AudioHeap_ClearAiBuffers(void) {
    s32 ind;
    s32 i;

    ind = gAudioContext.curAIBufIdx;
    gAudioContext.aiBufLengths[ind] = gAudioContext.audioBufferParameters.minAiBufferLength;

    for (i = 0; i < AIBUF_LEN; i++) {
        gAudioContext.aiBuffers[ind][i] = 0;
    }
}

s32 AudioHeap_ResetStep(void) {
    s32 i;
    s32 j;
    s32 sp24;

    if (gAudioContext.audioBufferParameters.specUnk4 == 2) {
        sp24 = 2;
    } else {
        sp24 = 1;
    }

    switch (gAudioContext.resetStatus) {
        case 5:
            for (i = 0; i < gAudioContext.audioBufferParameters.numSequencePlayers; i++) {
                AudioSeq_SequencePlayerDisableAsFinished(&gAudioContext.seqPlayers[i]);
            }
            gAudioContext.audioResetFadeOutFramesLeft = 2 / sp24;
            gAudioContext.resetStatus--;
            break;

        case 4:
            if (gAudioContext.audioResetFadeOutFramesLeft != 0) {
                gAudioContext.audioResetFadeOutFramesLeft--;
                AudioHeap_UpdateReverbs();
            } else {
                for (i = 0; i < gAudioContext.numNotes; i++) {
                    if (gAudioContext.notes[i].noteSubEu.bitField0.enabled &&
                        gAudioContext.notes[i].playbackState.adsr.action.s.state != ADSR_STATE_DISABLED) {
                        gAudioContext.notes[i].playbackState.adsr.fadeOutVel =
                            gAudioContext.audioBufferParameters.updatesPerFrameInv;
                        gAudioContext.notes[i].playbackState.adsr.action.s.release = true;
                    }
                }
                gAudioContext.audioResetFadeOutFramesLeft = 8 / sp24;
                gAudioContext.resetStatus--;
            }
            break;

        case 3:
            if (gAudioContext.audioResetFadeOutFramesLeft != 0) {
                gAudioContext.audioResetFadeOutFramesLeft--;
                AudioHeap_UpdateReverbs();
            } else {
                gAudioContext.audioResetFadeOutFramesLeft = 2 / sp24;
                gAudioContext.resetStatus--;
            }
            break;

        case 2:
            AudioHeap_ClearAiBuffers();
            if (gAudioContext.audioResetFadeOutFramesLeft != 0) {
                gAudioContext.audioResetFadeOutFramesLeft--;
            } else {
                gAudioContext.resetStatus--;
                AudioHeap_DiscardSampleCaches();
                AudioHeap_DiscardSampleBanks();
            }
            break;

        case 1:
            AudioHeap_Init();
            gAudioContext.resetStatus = 0;
            for (i = 0; i < 3; i++) {
                gAudioContext.aiBufLengths[i] = gAudioContext.audioBufferParameters.maxAiBufferLength;
                for (j = 0; j < AIBUF_LEN; j++) {
                    gAudioContext.aiBuffers[i][j] = 0;
                }
            }
            break;
    }

    if (gAudioContext.resetStatus < 3) {
        return 0;
    }

    return 1;
}

void AudioHeap_Init(void) {
    s32 pad1[4];
    s16* mem;
    s32 persistentMem;
    s32 temporaryMem;
    s32 totalMem;
    s32 wantMisc;
    OSIntMask intMask;
    s32 i;
    s32 j;
    s32 pad2;
    AudioSpec* spec;

    spec = &gAudioSpecs[gAudioContext.audioResetSpecIdToLoad];
    gAudioContext.sampleDmaCount = 0;
    gAudioContext.audioBufferParameters.frequency = spec->frequency;
    gAudioContext.audioBufferParameters.aiFrequency = osAiSetFrequency(gAudioContext.audioBufferParameters.frequency);
    gAudioContext.audioBufferParameters.samplesPerFrameTarget =
        ((gAudioContext.audioBufferParameters.frequency / gAudioContext.refreshRate) + 0xF) & 0xFFF0;
    gAudioContext.audioBufferParameters.minAiBufferLength =
        gAudioContext.audioBufferParameters.samplesPerFrameTarget - 0x10;
    gAudioContext.audioBufferParameters.maxAiBufferLength =
        gAudioContext.audioBufferParameters.samplesPerFrameTarget + 0x10;
    gAudioContext.audioBufferParameters.updatesPerFrame =
        ((gAudioContext.audioBufferParameters.samplesPerFrameTarget + 0x10) / 0xD0) + 1;
    gAudioContext.audioBufferParameters.samplesPerUpdate = (gAudioContext.audioBufferParameters.samplesPerFrameTarget /
                                                            gAudioContext.audioBufferParameters.updatesPerFrame) &
                                                           ~7;
    gAudioContext.audioBufferParameters.samplesPerUpdateMax = gAudioContext.audioBufferParameters.samplesPerUpdate + 8;
    gAudioContext.audioBufferParameters.samplesPerUpdateMin = gAudioContext.audioBufferParameters.samplesPerUpdate - 8;
    gAudioContext.audioBufferParameters.resampleRate = 32000.0f / (s32)gAudioContext.audioBufferParameters.frequency;
    gAudioContext.audioBufferParameters.unkUpdatesPerFrameScaled =
        (1.0f / 256.0f) / gAudioContext.audioBufferParameters.updatesPerFrame;
    gAudioContext.audioBufferParameters.unk_24 = gAudioContext.audioBufferParameters.updatesPerFrame * 0.25f;
    gAudioContext.audioBufferParameters.updatesPerFrameInv = 1.0f / gAudioContext.audioBufferParameters.updatesPerFrame;
    gAudioContext.sampleDmaBufSize1 = spec->sampleDmaBufSize1;
    gAudioContext.sampleDmaBufSize2 = spec->sampleDmaBufSize2;

    gAudioContext.numNotes = spec->numNotes;
    gAudioContext.audioBufferParameters.numSequencePlayers = spec->numSequencePlayers;
    if (gAudioContext.audioBufferParameters.numSequencePlayers > 4) {
        gAudioContext.audioBufferParameters.numSequencePlayers = 4;
    }
    gAudioContext.unk_2 = spec->unk_14;
    gAudioContext.tempoInternalToExternal = (u32)(gAudioContext.audioBufferParameters.updatesPerFrame * 2880000.0f /
                                                  gTatumsPerBeat / gAudioContext.unk_2960);

    gAudioContext.unk_2870 = gAudioContext.refreshRate;
    gAudioContext.unk_2870 *= gAudioContext.audioBufferParameters.updatesPerFrame;
    gAudioContext.unk_2870 /= gAudioContext.audioBufferParameters.aiFrequency;
    gAudioContext.unk_2870 /= gAudioContext.tempoInternalToExternal;

    gAudioContext.audioBufferParameters.specUnk4 = spec->unk_04;
    gAudioContext.audioBufferParameters.samplesPerFrameTarget *= gAudioContext.audioBufferParameters.specUnk4;
    gAudioContext.audioBufferParameters.maxAiBufferLength *= gAudioContext.audioBufferParameters.specUnk4;
    gAudioContext.audioBufferParameters.minAiBufferLength *= gAudioContext.audioBufferParameters.specUnk4;
    gAudioContext.audioBufferParameters.updatesPerFrame *= gAudioContext.audioBufferParameters.specUnk4;

    if (gAudioContext.audioBufferParameters.specUnk4 >= 2) {
        gAudioContext.audioBufferParameters.maxAiBufferLength -= 0x10;
    }

    gAudioContext.maxAudioCmds = gAudioContext.numNotes * 0x10 * gAudioContext.audioBufferParameters.updatesPerFrame +
                                 spec->numReverbs * 0x18 + 0x140;

    persistentMem = spec->persistentSeqMem + spec->persistentFontMem + spec->persistentSampleMem + 0x10;
    temporaryMem = spec->temporarySeqMem + spec->temporaryFontMem + spec->temporarySampleMem + 0x10;
    totalMem = persistentMem + temporaryMem;
    wantMisc = gAudioContext.audioSessionPool.size - totalMem - 0x100;

    if (gAudioContext.externalPool.start != NULL) {
        gAudioContext.externalPool.cur = gAudioContext.externalPool.start;
    }

    gAudioContext.sessionPoolSplit.wantSeq = wantMisc;
    gAudioContext.sessionPoolSplit.wantCustom = totalMem;
    AudioHeap_SessionPoolsInit(&gAudioContext.sessionPoolSplit);
    gAudioContext.cachePoolSplit.wantPersistent = persistentMem;
    gAudioContext.cachePoolSplit.wantTemporary = temporaryMem;
    AudioHeap_CachePoolInit(&gAudioContext.cachePoolSplit);
    gAudioContext.persistentCommonPoolSplit.wantSeq = spec->persistentSeqMem;
    gAudioContext.persistentCommonPoolSplit.wantFont = spec->persistentFontMem;
    gAudioContext.persistentCommonPoolSplit.wantSample = spec->persistentSampleMem;
    AudioHeap_PersistentCachesInit(&gAudioContext.persistentCommonPoolSplit);
    gAudioContext.temporaryCommonPoolSplit.wantSeq = spec->temporarySeqMem;
    gAudioContext.temporaryCommonPoolSplit.wantFont = spec->temporaryFontMem;
    gAudioContext.temporaryCommonPoolSplit.wantSample = spec->temporarySampleMem;
    AudioHeap_TemporaryCachesInit(&gAudioContext.temporaryCommonPoolSplit);

    AudioHeap_ResetLoadStatus();
    gAudioContext.notes =
        AudioHeap_AllocZeroed(&gAudioContext.notesAndBuffersPool, gAudioContext.numNotes * sizeof(Note));
    Audio_NoteInitAll();
    Audio_InitNoteFreeList();
    gAudioContext.noteSubsEu =
        AudioHeap_AllocZeroed(&gAudioContext.notesAndBuffersPool, gAudioContext.audioBufferParameters.updatesPerFrame *
                                                                      gAudioContext.numNotes * sizeof(NoteSubEu));

    for (i = 0; i != 2; i++) {
        gAudioContext.abiCmdBufs[i] = AudioHeap_AllocDmaMemoryZeroed(&gAudioContext.notesAndBuffersPool,
                                                                     gAudioContext.maxAudioCmds * sizeof(u64));
    }

    gAudioContext.unk_3520 = AudioHeap_Alloc(&gAudioContext.notesAndBuffersPool, 0x100 * sizeof(f32));
    func_800DDE3C();
    for (i = 0; i < 4; i++) {
        gAudioContext.synthesisReverbs[i].useReverb = 0;
    }

    gAudioContext.numSynthesisReverbs = spec->numReverbs;
    for (i = 0; i < gAudioContext.numSynthesisReverbs; i++) {
        ReverbSettings* settings = &spec->reverbSettings[i];
        SynthesisReverb* reverb = &gAudioContext.synthesisReverbs[i];
        reverb->downsampleRate = settings->downsampleRate;
        reverb->windowSize = settings->windowSize * 64;
        reverb->windowSize /= reverb->downsampleRate;
        reverb->unk_0C = settings->unk_4;
        reverb->unk_0A = settings->unk_A;
        reverb->unk_14 = settings->unk_6 * 64;
        reverb->unk_16 = settings->unk_8;
        reverb->unk_18 = 0;
        reverb->leakRtl = settings->leakRtl;
        reverb->leakLtr = settings->leakLtr;
        reverb->unk_05 = settings->unk_10;
        reverb->unk_08 = settings->unk_12;
        reverb->useReverb = 8;
        reverb->leftRingBuf =
            AudioHeap_AllocZeroedAttemptExternal(&gAudioContext.notesAndBuffersPool, reverb->windowSize * sizeof(s16));
        reverb->rightRingBuf =
            AudioHeap_AllocZeroedAttemptExternal(&gAudioContext.notesAndBuffersPool, reverb->windowSize * sizeof(s16));
        reverb->nextRingBufPos = 0;
        reverb->unk_20 = 0;
        reverb->curFrame = 0;
        reverb->bufSizePerChan = reverb->windowSize;
        reverb->framesToIgnore = 2;
        reverb->resampleFlags = 1;
        reverb->sound.sample = &reverb->sample;
        reverb->sample.loop = &reverb->loop;
        reverb->sound.tuning = 1.0f;
        reverb->sample.codec = CODEC_REVERB;
        reverb->sample.medium = MEDIUM_RAM;
        reverb->sample.size = reverb->windowSize * 2;
        reverb->sample.sampleAddr = (u8*)reverb->leftRingBuf;
        reverb->loop.start = 0;
        reverb->loop.count = 1;
        reverb->loop.end = reverb->windowSize;

        if (reverb->downsampleRate != 1) {
            reverb->unk_0E = 0x8000 / reverb->downsampleRate;
            reverb->unk_30 = AudioHeap_AllocZeroed(&gAudioContext.notesAndBuffersPool, 0x20);
            reverb->unk_34 = AudioHeap_AllocZeroed(&gAudioContext.notesAndBuffersPool, 0x20);
            reverb->unk_38 = AudioHeap_AllocZeroed(&gAudioContext.notesAndBuffersPool, 0x20);
            reverb->unk_3C = AudioHeap_AllocZeroed(&gAudioContext.notesAndBuffersPool, 0x20);
            for (j = 0; j < gAudioContext.audioBufferParameters.updatesPerFrame; j++) {
                mem = AudioHeap_AllocZeroedAttemptExternal(&gAudioContext.notesAndBuffersPool, 0x340);
                reverb->items[0][j].toDownsampleLeft = mem;
                reverb->items[0][j].toDownsampleRight = mem + 0x1A0 / sizeof(s16);
                mem = AudioHeap_AllocZeroedAttemptExternal(&gAudioContext.notesAndBuffersPool, 0x340);
                reverb->items[1][j].toDownsampleLeft = mem;
                reverb->items[1][j].toDownsampleRight = mem + 0x1A0 / sizeof(s16);
            }
        }

        if (settings->lowPassFilterCutoffLeft != 0) {
            reverb->filterLeftState = AudioHeap_AllocDmaMemoryZeroed(&gAudioContext.notesAndBuffersPool, 0x40);
            reverb->filterLeft = AudioHeap_AllocDmaMemory(&gAudioContext.notesAndBuffersPool, 8 * sizeof(s16));
            AudioHeap_LoadLowPassFilter(reverb->filterLeft, settings->lowPassFilterCutoffLeft);
        } else {
            reverb->filterLeft = NULL;
        }

        if (settings->lowPassFilterCutoffRight != 0) {
            reverb->filterRightState = AudioHeap_AllocDmaMemoryZeroed(&gAudioContext.notesAndBuffersPool, 0x40);
            reverb->filterRight = AudioHeap_AllocDmaMemory(&gAudioContext.notesAndBuffersPool, 8 * sizeof(s16));
            AudioHeap_LoadLowPassFilter(reverb->filterRight, settings->lowPassFilterCutoffRight);
        } else {
            reverb->filterRight = NULL;
        }
    }

    AudioSeq_InitSequencePlayers();
    for (j = 0; j < gAudioContext.audioBufferParameters.numSequencePlayers; j++) {
        AudioSeq_InitSequencePlayerChannels(j);
        AudioSeq_ResetSequencePlayer(&gAudioContext.seqPlayers[j]);
    }

    AudioHeap_InitSampleCaches(spec->persistentSampleCacheMem, spec->temporarySampleCacheMem);
    AudioLoad_InitSampleDmaBuffers(gAudioContext.numNotes);
    gAudioContext.preloadSampleStackTop = 0;
    AudioLoad_InitSlowLoads();
    AudioLoad_InitScriptLoads();
    AudioLoad_InitAsyncLoads();
    gAudioContext.unk_4 = 0x1000;
    AudioLoad_LoadPermanentSamples();
    intMask = osSetIntMask(1);
    osWritebackDCacheAll();
    osSetIntMask(intMask);
}

void* AudioHeap_SearchPermanentCache(s32 tableType, s32 id) {
    s32 i;

    for (i = 0; i < gAudioContext.permanentPool.count; i++) {
        if (gAudioContext.permanentCache[i].tableType == tableType && gAudioContext.permanentCache[i].id == id) {
            return gAudioContext.permanentCache[i].ptr;
        }
    }
    return NULL;
}

void* AudioHeap_AllocPermanent(s32 tableType, s32 id, size_t size) {
    void* ret;
    s32 index;

    index = gAudioContext.permanentPool.count;

    ret = AudioHeap_Alloc(&gAudioContext.permanentPool, size);
    gAudioContext.permanentCache[index].ptr = ret;
    if (ret == NULL) {
        return NULL;
    }
    gAudioContext.permanentCache[index].tableType = tableType;
    gAudioContext.permanentCache[index].id = id;
    gAudioContext.permanentCache[index].size = size;
    //! @bug UB: missing return. "ret" is in v0 at this point, but doing an
    // explicit return uses an additional register.
    return ret;
}

void* AudioHeap_AllocSampleCache(size_t size, s32 fontId, void* sampleAddr, s8 medium, s32 cache) {
    SampleCacheEntry* entry;

    if (cache == CACHE_TEMPORARY) {
        entry = AudioHeap_AllocTemporarySampleCacheEntry(size);
    } else {
        entry = AudioHeap_AllocPersistentSampleCacheEntry(size);
    }
    if (entry != NULL) {
        //! @bug Should use sampleBankId, not fontId
        entry->sampleBankId = fontId;
        entry->sampleAddr = sampleAddr;
        entry->origMedium = medium;
        return entry->allocatedAddr;
    }
    return NULL;
}

void AudioHeap_InitSampleCaches(u32 persistentSize, u32 temporarySize) {
    void* mem;

    mem = AudioHeap_AllocAttemptExternal(&gAudioContext.notesAndBuffersPool, persistentSize);
    if (mem == NULL) {
        gAudioContext.persistentSampleCache.pool.size = 0;
    } else {
        AudioHeap_AllocPoolInit(&gAudioContext.persistentSampleCache.pool, mem, persistentSize);
    }
    mem = AudioHeap_AllocAttemptExternal(&gAudioContext.notesAndBuffersPool, temporarySize);
    if (mem == NULL) {
        gAudioContext.temporarySampleCache.pool.size = 0;
    } else {
        AudioHeap_AllocPoolInit(&gAudioContext.temporarySampleCache.pool, mem, temporarySize);
    }
    gAudioContext.persistentSampleCache.size = 0;
    gAudioContext.temporarySampleCache.size = 0;
}

SampleCacheEntry* AudioHeap_AllocTemporarySampleCacheEntry(size_t size) {
    u8* allocAfter;
    u8* allocBefore;
    void* mem;
    s32 index;
    s32 i;
    SampleCacheEntry* ret;
    AudioPreloadReq* preload;
    AudioSampleCache* pool;
    u8* start;
    u8* end;

    pool = &gAudioContext.temporarySampleCache;
    allocBefore = pool->pool.cur;
    mem = AudioHeap_Alloc(&pool->pool, size);
    if (mem == NULL) {
        // Reset the pool and try again. We still keep pointers to within the
        // pool, so we have to be careful to discard existing overlapping
        // allocations further down.
        u8* old = pool->pool.cur;
        pool->pool.cur = pool->pool.start;
        mem = AudioHeap_Alloc(&pool->pool, size);
        if (mem == NULL) {
            pool->pool.cur = old;
            return NULL;
        }
        allocBefore = pool->pool.start;
    }

    allocAfter = pool->pool.cur;

    index = -1;
    for (i = 0; i < gAudioContext.preloadSampleStackTop; i++) {
        preload = &gAudioContext.preloadSampleStack[i];
        if (preload->isFree == false) {
            start = preload->ramAddr;
            end = preload->ramAddr + preload->sample->size - 1;

            if (end < allocBefore && start < allocBefore) {
                continue;
            }
            if (end >= allocAfter && start >= allocAfter) {
                continue;
            }

            // Overlap, skip this preload.
            preload->isFree = true;
        }
    }

    for (i = 0; i < pool->size; i++) {
        if (pool->entries[i].inUse == false) {
            continue;
        }

        start = pool->entries[i].allocatedAddr;
        end = start + pool->entries[i].size - 1;

        if (end < allocBefore && start < allocBefore) {
            continue;
        }
        if (end >= allocAfter && start >= allocAfter) {
            continue;
        }

        // Overlap, discard existing entry.
        AudioHeap_DiscardSampleCacheEntry(&pool->entries[i]);
        if (index == -1) {
            index = i;
        }
    }

    if (index == -1) {
        index = pool->size++;
    }

    ret = &pool->entries[index];
    ret->inUse = true;
    ret->allocatedAddr = mem;
    ret->size = size;
    return ret;
}

void AudioHeap_UnapplySampleCacheForFont(SampleCacheEntry* entry, s32 fontId) {
    Drum* drum;
    Instrument* inst;
    SoundFontSound* sfx;
    size_t instId;
    size_t drumId;
    size_t sfxId;

    for (instId = 0; instId < gAudioContext.soundFonts[fontId].numInstruments; instId++) {
        inst = Audio_GetInstrumentInner(fontId, instId);
        if (inst != NULL) {
            if (inst->normalRangeLo != 0) {
                AudioHeap_UnapplySampleCache(entry, inst->lowNotesSound.sample);
            }
            if (inst->normalRangeHi != 0x7F) {
                AudioHeap_UnapplySampleCache(entry, inst->highNotesSound.sample);
            }
            AudioHeap_UnapplySampleCache(entry, inst->normalNotesSound.sample);
        }
    }

    for (drumId = 0; drumId < gAudioContext.soundFonts[fontId].numDrums; drumId++) {
        drum = Audio_GetDrum(fontId, drumId);
        if (drum != NULL) {
            AudioHeap_UnapplySampleCache(entry, drum->sound.sample);
        }
    }

    for (sfxId = 0; sfxId < gAudioContext.soundFonts[fontId].numSfx; sfxId++) {
        sfx = Audio_GetSfx(fontId, sfxId);
        if (sfx != NULL) {
            AudioHeap_UnapplySampleCache(entry, sfx->sample);
        }
    }
}

void AudioHeap_DiscardSampleCacheEntry(SampleCacheEntry* entry) {
    s32 numFonts;
    s32 sampleBankId1;
    s32 sampleBankId2;
    s32 fontId;

    numFonts = gAudioContext.soundFontTable->numEntries;
    for (fontId = 0; fontId < numFonts; fontId++) {
        sampleBankId1 = gAudioContext.soundFonts[fontId].sampleBankId1;
        sampleBankId2 = gAudioContext.soundFonts[fontId].sampleBankId2;
        if (((sampleBankId1 != 0xFF) && (entry->sampleBankId == sampleBankId1)) ||
            ((sampleBankId2 != 0xFF) && (entry->sampleBankId == sampleBankId2)) || entry->sampleBankId == 0) {
            if (AudioHeap_SearchCaches(FONT_TABLE, CACHE_EITHER, fontId) != NULL) {
                if (AudioLoad_IsFontLoadComplete(fontId) != 0) {
                    AudioHeap_UnapplySampleCacheForFont(entry, fontId);
                }
            }
        }
    }
}

void AudioHeap_UnapplySampleCache(SampleCacheEntry* entry, SoundFontSample* sample)
{
}

SampleCacheEntry* AudioHeap_AllocPersistentSampleCacheEntry(size_t size) {
    AudioSampleCache* pool;
    SampleCacheEntry* entry;
    void* mem;

    pool = &gAudioContext.persistentSampleCache;
    mem = AudioHeap_Alloc(&pool->pool, size);
    if (mem == NULL) {
        return NULL;
    }
    entry = &pool->entries[pool->size];
    entry->inUse = true;
    entry->allocatedAddr = mem;
    entry->size = size;
    pool->size++;
    return entry;
}

void AudioHeap_DiscardSampleCacheForFont(SampleCacheEntry* entry, s32 sampleBankId1, s32 sampleBankId2, s32 fontId) {
    if ((entry->sampleBankId == sampleBankId1) || (entry->sampleBankId == sampleBankId2) ||
        (entry->sampleBankId == 0)) {
        AudioHeap_UnapplySampleCacheForFont(entry, fontId);
    }
}

void AudioHeap_DiscardSampleCaches(void) {
    s32 numFonts;
    s32 sampleBankId1;
    s32 sampleBankId2;
    s32 fontId;
    s32 j;

    return;

    numFonts = gAudioContext.soundFontTable->numEntries;
    for (fontId = 0; fontId < numFonts; fontId++) {
        sampleBankId1 = gAudioContext.soundFonts[fontId].sampleBankId1;
        sampleBankId2 = gAudioContext.soundFonts[fontId].sampleBankId2;
        if ((sampleBankId1 == 0xFF) && (sampleBankId2 == 0xFF)) {
            continue;
        }
        if (AudioHeap_SearchCaches(FONT_TABLE, CACHE_PERMANENT, fontId) == NULL ||
            !AudioLoad_IsFontLoadComplete(fontId)) {
            continue;
        }

        for (j = 0; j < gAudioContext.persistentSampleCache.size; j++) {
            AudioHeap_DiscardSampleCacheForFont(&gAudioContext.persistentSampleCache.entries[j], sampleBankId1,
                                                sampleBankId2, fontId);
        }
        for (j = 0; j < gAudioContext.temporarySampleCache.size; j++) {
            AudioHeap_DiscardSampleCacheForFont(&gAudioContext.temporarySampleCache.entries[j], sampleBankId1,
                                                sampleBankId2, fontId);
        }
    }
}

typedef struct {
    uintptr_t oldAddr;
    uintptr_t newAddr;
    size_t size;
    u8 newMedium;
} StorageChange;

void AudioHeap_ChangeStorage(StorageChange* change, SoundFontSample* sample) {
    if (sample != NULL) {
        uintptr_t start = change->oldAddr;
        uintptr_t end = change->oldAddr + change->size;

        if (start <= sample->sampleAddr && sample->sampleAddr < end) {
            sample->sampleAddr = sample->sampleAddr - start + change->newAddr;
            sample->medium = change->newMedium;
        }
    }
}

void AudioHeap_ApplySampleBankCacheInternal(s32 apply, s32 id);

void AudioHeap_DiscardSampleBank(s32 sampleBankId) {
    AudioHeap_ApplySampleBankCacheInternal(false, sampleBankId);
}

void AudioHeap_ApplySampleBankCache(s32 sampleBankId) {
    AudioHeap_ApplySampleBankCacheInternal(true, sampleBankId);
}

void AudioHeap_ApplySampleBankCacheInternal(s32 apply, s32 sampleBankId) {
    AudioTable* sampleBankTable;
    AudioTableEntry* entry;
    s32 numFonts;
    s32 instId;
    s32 drumId;
    size_t sfxId;
    StorageChange change;
    s32 sampleBankId1;
    s32 sampleBankId2;
    s32 fontId;
    Drum* drum;
    Instrument* inst;
    SoundFontSound* sfx;
    u32* fakematch;
    s32 pad[4];

    return;

    sampleBankTable = gAudioContext.sampleBankTable;
    numFonts = gAudioContext.soundFontTable->numEntries;
    change.oldAddr = AudioHeap_SearchCaches(SAMPLE_TABLE, CACHE_EITHER, sampleBankId);
    if (change.oldAddr == 0) {
        return;
    }

    entry = &sampleBankTable->entries[sampleBankId];
    change.size = entry->size;
    change.newMedium = entry->medium;

    if ((change.newMedium == MEDIUM_CART) || (change.newMedium == MEDIUM_DISK_DRIVE)) {
        change.newAddr = entry->romAddr;
    } else {
        change.newAddr = 0;
    }

    fakematch = &change.oldAddr;
    if ((apply != false) && (apply == true)) {
        u32 temp = change.newAddr;
        change.newAddr = *fakematch; // = change.oldAddr
        change.oldAddr = temp;
        change.newMedium = MEDIUM_RAM;
    }

    for (fontId = 0; fontId < numFonts; fontId++) {
        sampleBankId1 = gAudioContext.soundFonts[fontId].sampleBankId1;
        sampleBankId2 = gAudioContext.soundFonts[fontId].sampleBankId2;
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

            for (instId = 0; instId < gAudioContext.soundFonts[fontId].numInstruments; instId++) {
                inst = Audio_GetInstrumentInner(fontId, instId);
                if (inst != NULL) {
                    if (inst->normalRangeLo != 0) {
                        AudioHeap_ChangeStorage(&change, inst->lowNotesSound.sample);
                    }
                    if (inst->normalRangeHi != 0x7F) {
                        AudioHeap_ChangeStorage(&change, inst->highNotesSound.sample);
                    }
                    AudioHeap_ChangeStorage(&change, inst->normalNotesSound.sample);
                }
            }

            for (drumId = 0; drumId < gAudioContext.soundFonts[fontId].numDrums; drumId++) {
                drum = Audio_GetDrum(fontId, drumId);
                if (drum != NULL) {
                    AudioHeap_ChangeStorage(&change, drum->sound.sample);
                }
            }

            for (sfxId = 0; sfxId < gAudioContext.soundFonts[fontId].numSfx; sfxId++) {
                sfx = Audio_GetSfx(fontId, sfxId);
                if (sfx != NULL) {
                    AudioHeap_ChangeStorage(&change, sfx->sample);
                }
            }
        }
    }
}

void AudioHeap_DiscardSampleBanks(void) {
    AudioCache* pool;
    AudioPersistentCache* persistent;
    AudioTemporaryCache* temporary;
    u32 i;

    pool = &gAudioContext.sampleBankCache;
    temporary = &pool->temporary;

    if (temporary->entries[0].id != -1) {
        AudioHeap_DiscardSampleBank(temporary->entries[0].id);
    }

    if (temporary->entries[1].id != -1) {
        AudioHeap_DiscardSampleBank(temporary->entries[1].id);
    }

    persistent = &pool->persistent;
    for (i = 0; i < persistent->numEntries; i++) {
        AudioHeap_DiscardSampleBank(persistent->entries[i].id);
    }
}
