#include "global.h"
#include <libultraship/bridge/resourcebridge.h>
#include "2s2h/mixer.h"
#include <libultraship/bridge/consolevariablebridge.h>

// DMEM Addresses for the RSP
#define DMEM_TEMP 0x3B0
#define DMEM_TEMP2 0x3C0
#define DMEM_SURROUND_TEMP 0x4B0
#define DMEM_UNCOMPRESSED_NOTE 0x570
#define DMEM_HAAS_TEMP 0x5B0
#define DMEM_COMB_TEMP 0x750             // = DMEM_TEMP + DMEM_2CH_SIZE + a bit more
#define DMEM_COMPRESSED_ADPCM_DATA 0x930 // = DMEM_LEFT_CH
#define DMEM_LEFT_CH 0x930
#define DMEM_RIGHT_CH 0xAD0
#define DMEM_WET_TEMP 0x3D0
#define DMEM_WET_SCRATCH 0x710 // = DMEM_WET_TEMP + DMEM_2CH_SIZE
#define DMEM_WET_LEFT_CH 0xC70
#define DMEM_WET_RIGHT_CH 0xE10 // = DMEM_WET_LEFT_CH + DMEM_1CH_SIZE

typedef enum {
    /* 0 */ HAAS_EFFECT_DELAY_NONE,
    /* 1 */ HAAS_EFFECT_DELAY_LEFT, // Delay left channel so that right channel is heard first
    /* 2 */ HAAS_EFFECT_DELAY_RIGHT // Delay right channel so that left channel is heard first
} HaasEffectDelaySide;

Acmd* AudioSynth_SaveResampledReverbSamplesImpl(Acmd* cmd, u16 dmem, u16 size, uintptr_t startAddr);
Acmd* AudioSynth_LoadReverbSamplesImpl(Acmd* cmd, u16 dmem, u16 startPos, s32 size, SynthesisReverb* reverb);
Acmd* AudioSynth_SaveReverbSamplesImpl(Acmd* cmd, u16 dmem, u16 startPos, s32 size, SynthesisReverb* reverb);
Acmd* AudioSynth_ProcessSamples(s16* aiBuf, s32 numSamplesPerUpdate, Acmd* cmd, s32 updateIndex);
Acmd* AudioSynth_ProcessSample(s32 noteIndex, NoteSampleState* sampleState, NoteSynthesisState* synthState, s16* aiBuf,
                               s32 numSamplesPerUpdate, Acmd* cmd, s32 updateIndex);
Acmd* AudioSynth_ApplySurroundEffect(Acmd* cmd, NoteSampleState* sampleState, NoteSynthesisState* synthState,
                                     s32 numSamplesPerUpdate, s32 haasDmem, s32 flags);
Acmd* AudioSynth_FinalResample(Acmd* cmd, NoteSynthesisState* synthState, s32 size, u16 pitch, u16 inpDmem,
                               s32 resampleFlags);
Acmd* AudioSynth_ProcessEnvelope(Acmd* cmd, NoteSampleState* sampleState, NoteSynthesisState* synthState,
                                 s32 numSamplesPerUpdate, u16 dmemSrc, s32 haasEffectDelaySide, s32 flags);
Acmd* AudioSynth_LoadWaveSamples(Acmd* cmd, NoteSampleState* sampleState, NoteSynthesisState* synthState,
                                 s32 numSamplesToLoad);
Acmd* AudioSynth_ApplyHaasEffect(Acmd* cmd, NoteSampleState* sampleState, NoteSynthesisState* synthState, s32 size,
                                 s32 flags, s32 haasEffectDelaySide);

s32 D_801D5FB0 = 0;

u32 sEnvMixerOp = _SHIFTL(A_ENVMIXER, 24, 8);

// Store the left dry channel in a temp space to be delayed to produce the haas effect
u32 sEnvMixerLeftHaasDmemDests =
    AUDIO_MK_CMD(DMEM_HAAS_TEMP >> 4, DMEM_RIGHT_CH >> 4, DMEM_WET_LEFT_CH >> 4, DMEM_WET_RIGHT_CH >> 4);

// Store the right dry channel in a temp space to be delayed to produce the haas effect
u32 sEnvMixerRightHaasDmemDests =
    AUDIO_MK_CMD(DMEM_LEFT_CH >> 4, DMEM_HAAS_TEMP >> 4, DMEM_WET_LEFT_CH >> 4, DMEM_WET_RIGHT_CH >> 4);

u32 sEnvMixerDefaultDmemDests =
    AUDIO_MK_CMD(DMEM_LEFT_CH >> 4, DMEM_RIGHT_CH >> 4, DMEM_WET_LEFT_CH >> 4, DMEM_WET_RIGHT_CH >> 4);

// Unused Data
u16 D_801D5FC4[] = {
    0x7FFF, 0xD001, 0x3FFF, 0xF001, 0x5FFF, 0x9001, 0x7FFF, 0x8001,
};

u8 sNumSamplesPerWavePeriod[] = {
    WAVE_SAMPLE_COUNT / 1, // 1st harmonic
    WAVE_SAMPLE_COUNT / 2, // 2nd harmonic
    WAVE_SAMPLE_COUNT / 4, // 4th harmonic
    WAVE_SAMPLE_COUNT / 8, // 8th harmonic
};

/**
 * Add a collection of s16-samples as a single entry to the reverb buffer
 */
void AudioSynth_AddReverbBufferEntry(s32 numSamples, s32 updateIndex, s32 reverbIndex) {
    SynthesisReverb* reverb;
    ReverbBufferEntry* entry;
    s32 extraSamples;
    s32 numSamplesAfterDownsampling;
    s32 reverbBufPos;
    s32 temp_t2;
    s32 temp_t4;
    s32 count1;
    s32 count2;
    s32 nextReverbSubBufPos;

    reverb = &gAudioCtx.synthesisReverbs[reverbIndex];
    entry = &reverb->bufEntry[reverb->curFrame][updateIndex];

    numSamplesAfterDownsampling = numSamples / gAudioCtx.synthesisReverbs[reverbIndex].downsampleRate;

    // Apply resampling effect
    if (gAudioCtx.synthesisReverbs[reverbIndex].resampleEffectOn) {
        if (reverb->downsampleRate == 1) {
            count1 = 0;
            count2 = 0;

            numSamplesAfterDownsampling += reverb->resampleEffectExtraSamples;

            entry->saveResampleNumSamples = numSamplesAfterDownsampling;
            entry->loadResamplePitch = ((u16)numSamplesAfterDownsampling << 0xF) / numSamples;
            entry->saveResamplePitch = (numSamples << 0xF) / (u16)numSamplesAfterDownsampling;

            while (true) {
                temp_t2 = (entry->loadResamplePitch * numSamples * 2) + reverb->resampleEffectLoadUnk;
                temp_t4 = temp_t2 >> 0x10;

                if ((temp_t4 != numSamplesAfterDownsampling) && (count1 == 0)) {
                    entry->loadResamplePitch =
                        ((numSamplesAfterDownsampling << 0x10) - reverb->resampleEffectLoadUnk) / (numSamples * 2);
                    count1++;
                } else {
                    count1++;
                    if (temp_t4 > numSamplesAfterDownsampling) {
                        entry->loadResamplePitch--;
                    } else if (temp_t4 < numSamplesAfterDownsampling) {
                        entry->loadResamplePitch++;
                    } else {
                        break;
                    }
                }
            }

            reverb->resampleEffectLoadUnk = temp_t2 & 0xFFFF;

            while (true) {
                temp_t2 = (entry->saveResamplePitch * numSamplesAfterDownsampling * 2) + reverb->resampleEffectSaveUnk;
                temp_t4 = temp_t2 >> 0x10;

                if ((temp_t4 != numSamples) && (count2 == 0)) {
                    entry->saveResamplePitch =
                        ((numSamples << 0x10) - reverb->resampleEffectSaveUnk) / (numSamplesAfterDownsampling * 2);
                    count2++;
                } else {
                    count2++;
                    if (temp_t4 > numSamples) {
                        entry->saveResamplePitch--;
                    } else if (temp_t4 < numSamples) {
                        entry->saveResamplePitch++;
                    } else {
                        break;
                    }
                }
            }

            reverb->resampleEffectSaveUnk = temp_t2 & 0xFFFF;
        }
    }

    extraSamples = (reverb->nextReverbBufPos + numSamplesAfterDownsampling) - reverb->delayNumSamples;
    reverbBufPos = reverb->nextReverbBufPos;

    // Add a reverb entry
    if (extraSamples < 0) {
        entry->size = numSamplesAfterDownsampling * SAMPLE_SIZE;
        entry->wrappedSize = 0;
        entry->startPos = reverb->nextReverbBufPos;
        reverb->nextReverbBufPos += numSamplesAfterDownsampling;
    } else {
        // End of the buffer is reached. Loop back around
        entry->size = (numSamplesAfterDownsampling - extraSamples) * SAMPLE_SIZE;
        entry->wrappedSize = extraSamples * SAMPLE_SIZE;
        entry->startPos = reverb->nextReverbBufPos;
        reverb->nextReverbBufPos = extraSamples;
    }

    entry->numSamplesAfterDownsampling = numSamplesAfterDownsampling;
    entry->numSamples = numSamples;

    // Add a sub-reverb entry
    if (reverb->subDelay != 0) {
        nextReverbSubBufPos = reverb->subDelay + reverbBufPos;
        if (nextReverbSubBufPos >= reverb->delayNumSamples) {
            nextReverbSubBufPos -= reverb->delayNumSamples;
        }

        entry = &reverb->subBufEntry[reverb->curFrame][updateIndex];
        numSamplesAfterDownsampling = numSamples / reverb->downsampleRate;
        extraSamples = (nextReverbSubBufPos + numSamplesAfterDownsampling) - reverb->delayNumSamples;

        if (extraSamples < 0) {
            entry->size = numSamplesAfterDownsampling * SAMPLE_SIZE;
            entry->wrappedSize = 0;
            entry->startPos = nextReverbSubBufPos;
        } else {
            // End of the buffer is reached. Loop back around
            entry->size = (numSamplesAfterDownsampling - extraSamples) * SAMPLE_SIZE;
            entry->wrappedSize = extraSamples * SAMPLE_SIZE;
            entry->startPos = nextReverbSubBufPos;
        }

        entry->numSamplesAfterDownsampling = numSamplesAfterDownsampling;
        entry->numSamples = numSamples;
    }
}

/**
 * Sync the sample states between the notes and the list
 */
void AudioSynth_SyncSampleStates(s32 updateIndex) {
    NoteSampleState* noteSampleState;
    NoteSampleState* sampleState;
    s32 sampleStateBaseIndex;
    s32 i;

    sampleStateBaseIndex = gAudioCtx.numNotes * updateIndex;
    for (i = 0; i < gAudioCtx.numNotes; i++) {
        noteSampleState = &gAudioCtx.notes[i].sampleState;
        sampleState = &gAudioCtx.sampleStateList[sampleStateBaseIndex + i];
        if (noteSampleState->bitField0.enabled) {
            noteSampleState->bitField0.needsInit = false;
        } else {
            sampleState->bitField0.enabled = false;
        }

        noteSampleState->harmonicIndexCurAndPrev = 0;
    }
}

Acmd* AudioSynth_Update(Acmd* abiCmdStart, s32* numAbiCmds, s16* aiBufStart, s32 numSamplesPerFrame) {
    s32 numSamplesPerUpdate;
    s16* curAiBufPos;
    Acmd* curCmd = abiCmdStart;
    s32 reverseUpdateIndex;
    s32 reverbIndex;
    SynthesisReverb* reverb;

    for (reverseUpdateIndex = gAudioCtx.audioBufferParameters.updatesPerFrame; reverseUpdateIndex > 0;
         reverseUpdateIndex--) {
        AudioScript_ProcessSequences(reverseUpdateIndex - 1);
        AudioSynth_SyncSampleStates(gAudioCtx.audioBufferParameters.updatesPerFrame - reverseUpdateIndex);
    }

    curAiBufPos = aiBufStart;
    gAudioCtx.adpcmCodeBook = NULL;

    // Process/Update all samples multiple times in a single frame
    for (reverseUpdateIndex = gAudioCtx.audioBufferParameters.updatesPerFrame; reverseUpdateIndex > 0;
         reverseUpdateIndex--) {
        if (reverseUpdateIndex == 1) {
            // Final Update
            numSamplesPerUpdate = numSamplesPerFrame;
        } else if ((numSamplesPerFrame / reverseUpdateIndex) >=
                   gAudioCtx.audioBufferParameters.numSamplesPerUpdateMax) {
            numSamplesPerUpdate = gAudioCtx.audioBufferParameters.numSamplesPerUpdateMax;
        } else if ((numSamplesPerFrame / reverseUpdateIndex) <=
                   gAudioCtx.audioBufferParameters.numSamplesPerUpdateMin) {
            numSamplesPerUpdate = gAudioCtx.audioBufferParameters.numSamplesPerUpdateMin;
        } else {
            numSamplesPerUpdate = gAudioCtx.audioBufferParameters.numSamplesPerUpdate;
        }

        for (reverbIndex = 0; reverbIndex < gAudioCtx.numSynthesisReverbs; reverbIndex++) {
            if (gAudioCtx.synthesisReverbs[reverbIndex].useReverb) {
                AudioSynth_AddReverbBufferEntry(numSamplesPerUpdate,
                                                gAudioCtx.audioBufferParameters.updatesPerFrame - reverseUpdateIndex,
                                                reverbIndex);
            }
        }

        curCmd = AudioSynth_ProcessSamples(curAiBufPos, numSamplesPerUpdate, curCmd,
                                           gAudioCtx.audioBufferParameters.updatesPerFrame - reverseUpdateIndex);
        numSamplesPerFrame -= numSamplesPerUpdate;
        curAiBufPos += numSamplesPerUpdate * SAMPLE_SIZE;
    }

    // Update reverb frame info
    for (reverbIndex = 0; reverbIndex < gAudioCtx.numSynthesisReverbs; reverbIndex++) {
        if (gAudioCtx.synthesisReverbs[reverbIndex].framesToIgnore != 0) {
            gAudioCtx.synthesisReverbs[reverbIndex].framesToIgnore--;
        }
        gAudioCtx.synthesisReverbs[reverbIndex].curFrame ^= 1;
    }

    *numAbiCmds = curCmd - abiCmdStart;
    return curCmd;
}

void AudioSynth_DisableSampleStates(s32 updateIndex, s32 noteIndex) {
    NoteSampleState* sampleState;
    s32 i;

    for (i = updateIndex + 1; i < gAudioCtx.audioBufferParameters.updatesPerFrame; i++) {
        sampleState = &gAudioCtx.sampleStateList[(gAudioCtx.numNotes * i) + noteIndex];
        if (sampleState->bitField0.needsInit) {
            break;
        }
        sampleState->bitField0.enabled = false;
    }
}

/**
 * Load reverb samples from a different reverb index
 */
Acmd* AudioSynth_LoadMixedReverbSamples(Acmd* cmd, SynthesisReverb* reverb, s16 updateIndex) {
    ReverbBufferEntry* entry = &reverb->bufEntry[reverb->curFrame][updateIndex];

    cmd = AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_TEMP, entry->startPos, entry->size, reverb);
    if (entry->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd = AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_TEMP + entry->size, 0, entry->wrappedSize, reverb);
    }
    return cmd;
}

/**
 * Save reverb samples from a different reverb index
 */
Acmd* AudioSynth_SaveMixedReverbSamples(Acmd* cmd, SynthesisReverb* reverb, s16 updateIndex) {
    ReverbBufferEntry* entry = &reverb->bufEntry[reverb->curFrame][updateIndex];

    cmd = AudioSynth_SaveReverbSamplesImpl(cmd, DMEM_WET_TEMP, entry->startPos, entry->size, reverb);
    if (entry->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd = AudioSynth_SaveReverbSamplesImpl(cmd, DMEM_WET_TEMP + entry->size, 0, entry->wrappedSize, reverb);
    }
    return cmd;
}

void AudioSynth_Noop1(void) {
}

void AudioSynth_ClearBuffer(Acmd* cmd, s32 dmem, s32 size) {
    aClearBuffer(cmd, dmem, size);
}

void AudioSynth_Noop2(void) {
}

void AudioSynth_Noop3(void) {
}

void AudioSynth_Noop4(void) {
}

void AudioSynth_Mix(Acmd* cmd, size_t size, s32 gain, s32 dmemIn, s32 dmemOut) {
    aMix(cmd, size, gain, dmemIn, dmemOut);
}

void AudioSynth_Noop5(void) {
}

void AudioSynth_Noop6(void) {
}

void AudioSynth_Noop7(void) {
}

void AudioSynth_SetBuffer(Acmd* cmd, s32 flags, s32 dmemIn, s32 dmemOut, size_t size) {
    aSetBuffer(cmd, flags, dmemIn, dmemOut, size);
}

void AudioSynth_Noop8(void) {
}

void AudioSynth_Noop9(void) {
}

void AudioSynth_DMemMove(Acmd* cmd, s32 dmemIn, s32 dmemOut, size_t size) {
    aDMEMMove(cmd, dmemIn, dmemOut, size);
    // cmd->words.w0 = _SHIFTL(A_DMEMMOVE, 24, 8) | _SHIFTL(dmemIn, 0, 24);
    // cmd->words.w1 = _SHIFTL(dmemOut, 16, 16) | _SHIFTL(size, 0, 16);
}

void AudioSynth_Noop10(void) {
}

void AudioSynth_Noop11(void) {
}

void AudioSynth_Noop12(void) {
}

void AudioSynth_Noop13(void) {
}

void AudioSynth_InterL(Acmd* cmd, s32 dmemIn, s32 dmemOut, s32 numSamples) {
    aInterl(cmd, dmemIn, dmemOut, numSamples);
    // cmd->words.w0 = _SHIFTL(A_INTERL, 24, 8) | _SHIFTL(numSamples, 0, 16);
    // cmd->words.w1 = _SHIFTL(dmemIn, 16, 16) | _SHIFTL(dmemOut, 0, 16);
}

void AudioSynth_EnvSetup1(Acmd* cmd, s32 reverbVol, s32 rampReverb, s32 rampLeft, s32 rampRight) {
    aEnvSetup1(cmd, reverbVol, rampReverb, rampLeft, rampRight);
}

void AudioSynth_Noop14(void) {
}

void AudioSynth_LoadBuffer(Acmd* cmd, s32 dmemDest, s32 size, void* addrSrc) {
    aLoadBuffer(cmd, addrSrc, dmemDest, size);
}

void AudioSynth_SaveBuffer(Acmd* cmd, s32 dmemSrc, s32 size, void* addrDest) {
    aSaveBuffer(cmd, dmemSrc, addrDest, size);
}

void AudioSynth_EnvSetup2(Acmd* cmd, s32 volLeft, s32 volRight) {
    aEnvSetup2(cmd, volLeft, volRight);
    // cmd->words.w0 = _SHIFTL(A_ENVSETUP2, 24, 8);
    // cmd->words.w1 = _SHIFTL(volLeft, 16, 16) | _SHIFTL(volRight, 0, 16);
}

void AudioSynth_Noop15(void) {
}

void AudioSynth_Noop16(void) {
}

void AudioSynth_Noop17(void) {
}

void AudioSynth_S8Dec(Acmd* cmd, s32 flags, s16* state) {
    aS8Dec(cmd, flags, state);
}

void AudioSynth_HiLoGain(Acmd* cmd, s32 gain, s32 dmemIn, s32 dmemOut, s32 size) {
    aHiLoGain(cmd, gain, size, dmemIn, dmemOut);
    // cmd->words.w0 = _SHIFTL(A_HILOGAIN, 24, 8) | _SHIFTL(gain, 16, 8) | _SHIFTL(size, 0, 16);
    // cmd->words.w1 = _SHIFTL(dmemIn, 16, 16) | _SHIFTL(dmemOut, 0, 16);
}

// Remnant of OoT
void AudioSynth_UnkCmd19(Acmd* cmd, s32 dmem1, s32 dmem2, s32 size, s32 arg4) {
    aUnkCmd19(cmd, dmem1, dmem2, size, arg4);
    // cmd->words.w0 = _SHIFTL(A_SPNOOP, 24, 8) | _SHIFTL(arg4, 16, 8) | _SHIFTL(size, 0, 16);
    // cmd->words.w1 = _SHIFTL(dmem1, 16, 16) | _SHIFTL(dmem2, 0, 16);
}

void AudioSynth_Noop18(void) {
}

void AudioSynth_Noop19(void) {
}

void AudioSynth_Noop20(void) {
}

void AudioSynth_Noop21(void) {
}

void AudioSynth_Noop22(void) {
}

void AudioSynth_Noop23(void) {
}

void AudioSynth_Noop24(void) {
}

void AudioSynth_Noop25(void) {
}

void AudioSynth_LoadFilterBuffer(Acmd* cmd, s32 flags, s32 buf, s16* addr) {
    aFilter(cmd, flags, buf, addr);
}

void AudioSynth_LoadFilterSize(Acmd* cmd, size_t size, s16* addr) {
    aFilter(cmd, 2, size, addr);
}

/**
 * Leak some audio from the left reverb channel into the right reverb channel and vice versa (pan)
 */
Acmd* AudioSynth_LeakReverb(Acmd* cmd, SynthesisReverb* reverb) {
    aDMEMMove(cmd++, DMEM_WET_LEFT_CH, DMEM_WET_SCRATCH, DMEM_1CH_SIZE);
    aMix(cmd++, DMEM_1CH_SIZE >> 4, reverb->leakRtl, DMEM_WET_RIGHT_CH, DMEM_WET_LEFT_CH);
    aMix(cmd++, DMEM_1CH_SIZE >> 4, reverb->leakLtr, DMEM_WET_SCRATCH, DMEM_WET_RIGHT_CH);

    return cmd;
}

Acmd* AudioSynth_LoadDownsampledReverbSamples(Acmd* cmd, s32 numSamplesPerUpdate, SynthesisReverb* reverb,
                                              s16 updateIndex) {
    ReverbBufferEntry* entry = &reverb->bufEntry[reverb->curFrame][updateIndex];
    s16 offsetSize = (entry->startPos & 7) * SAMPLE_SIZE;
    s16 wrappedOffsetSize = ALIGN16(offsetSize + entry->size);

    cmd = AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_TEMP, entry->startPos - (offsetSize / (s32)SAMPLE_SIZE),
                                           DMEM_1CH_SIZE, reverb);

    if (entry->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd = AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_TEMP + wrappedOffsetSize, 0,
                                               DMEM_1CH_SIZE - wrappedOffsetSize, reverb);
    }

    aSetBuffer(cmd++, 0, DMEM_WET_TEMP + offsetSize, DMEM_WET_LEFT_CH, numSamplesPerUpdate * SAMPLE_SIZE);
    aResample(cmd++, reverb->resampleFlags, reverb->downsamplePitch, reverb->leftLoadResampleBuf);
    aSetBuffer(cmd++, 0, DMEM_WET_TEMP + DMEM_1CH_SIZE + offsetSize, DMEM_WET_RIGHT_CH,
               numSamplesPerUpdate * SAMPLE_SIZE);
    aResample(cmd++, reverb->resampleFlags, reverb->downsamplePitch, reverb->rightLoadResampleBuf);

    return cmd;
}

Acmd* AudioSynth_SaveResampledReverbSamples(Acmd* cmd, SynthesisReverb* reverb, s16 updateIndex) {
    ReverbBufferEntry* entry = &reverb->bufEntry[reverb->curFrame][updateIndex];
    s16 numSamples = entry->numSamples;
    u32 size = numSamples * SAMPLE_SIZE;

    // Left Resample
    aDMEMMove(cmd++, DMEM_WET_LEFT_CH, DMEM_WET_TEMP, size);
    aSetBuffer(cmd++, 0, DMEM_WET_TEMP, DMEM_WET_SCRATCH, entry->saveResampleNumSamples * SAMPLE_SIZE);
    aResample(cmd++, reverb->resampleFlags, entry->saveResamplePitch, reverb->leftSaveResampleBuf);

    cmd = AudioSynth_SaveResampledReverbSamplesImpl(cmd, DMEM_WET_SCRATCH, entry->size,
                                                    (uintptr_t)&reverb->leftReverbBuf[entry->startPos]);

    if (entry->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd = AudioSynth_SaveResampledReverbSamplesImpl(cmd, entry->size + DMEM_WET_SCRATCH, entry->wrappedSize,
                                                        (uintptr_t)reverb->leftReverbBuf);
    }

    // Right Resample
    aDMEMMove(cmd++, DMEM_WET_RIGHT_CH, DMEM_WET_TEMP, size);
    aSetBuffer(cmd++, 0, DMEM_WET_TEMP, DMEM_WET_SCRATCH, entry->saveResampleNumSamples * SAMPLE_SIZE);
    aResample(cmd++, reverb->resampleFlags, entry->saveResamplePitch, reverb->rightSaveResampleBuf);

    cmd = AudioSynth_SaveResampledReverbSamplesImpl(cmd, DMEM_WET_SCRATCH, entry->size,
                                                    (uintptr_t)&reverb->rightReverbBuf[entry->startPos]);

    if (entry->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd = AudioSynth_SaveResampledReverbSamplesImpl(cmd, entry->size + DMEM_WET_SCRATCH, entry->wrappedSize,
                                                        (uintptr_t)reverb->rightReverbBuf);
    }

    return cmd;
}

Acmd* AudioSynth_LoadResampledReverbSamples(Acmd* cmd, s32 numSamplesPerUpdate, SynthesisReverb* reverb,
                                            s16 updateIndex) {
    ReverbBufferEntry* entry = &reverb->bufEntry[reverb->curFrame][updateIndex];
    s16 offsetSize = (entry->startPos & 7) * SAMPLE_SIZE;
    s16 wrappedOffsetSize = ALIGN16(offsetSize + entry->size);

    cmd = AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_TEMP, entry->startPos - (offsetSize / (s32)SAMPLE_SIZE),
                                           DMEM_1CH_SIZE, reverb);

    if (entry->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd = AudioSynth_LoadReverbSamplesImpl(cmd, wrappedOffsetSize + DMEM_WET_TEMP, 0,
                                               DMEM_1CH_SIZE - wrappedOffsetSize, reverb);
    }

    aSetBuffer(cmd++, 0, DMEM_WET_TEMP + offsetSize, DMEM_WET_LEFT_CH, numSamplesPerUpdate * SAMPLE_SIZE);
    aResample(cmd++, reverb->resampleFlags, entry->loadResamplePitch, reverb->leftLoadResampleBuf);
    aSetBuffer(cmd++, 0, DMEM_WET_TEMP + DMEM_1CH_SIZE + offsetSize, DMEM_WET_RIGHT_CH,
               numSamplesPerUpdate * SAMPLE_SIZE);
    aResample(cmd++, reverb->resampleFlags, entry->loadResamplePitch, reverb->rightLoadResampleBuf);

    return cmd;
}

/**
 * Apply a filter (convolution) to each reverb channel.
 */
Acmd* AudioSynth_FilterReverb(Acmd* cmd, s32 size, SynthesisReverb* reverb) {
    if (reverb->filterLeft != NULL) {
        aFilter(cmd++, 2, size, reverb->filterLeft);
        aFilter(cmd++, reverb->resampleFlags, DMEM_WET_LEFT_CH, reverb->filterLeftState);
    }

    if (reverb->filterRight != NULL) {
        aFilter(cmd++, 2, size, reverb->filterRight);
        aFilter(cmd++, reverb->resampleFlags, DMEM_WET_RIGHT_CH, reverb->filterRightState);
    }

    return cmd;
}

/**
 * Mix in reverb from a different reverb index
 */
Acmd* AudioSynth_MixOtherReverbIndex(Acmd* cmd, SynthesisReverb* reverb, s32 updateIndex) {
    SynthesisReverb* mixReverb;

    if (reverb->mixReverbIndex >= gAudioCtx.numSynthesisReverbs) {
        return cmd;
    }

    mixReverb = &gAudioCtx.synthesisReverbs[reverb->mixReverbIndex];
    if (mixReverb->downsampleRate == 1) {
        cmd = AudioSynth_LoadMixedReverbSamples(cmd, mixReverb, updateIndex);
        aMix(cmd++, DMEM_2CH_SIZE >> 4, reverb->mixReverbStrength, DMEM_WET_LEFT_CH, DMEM_WET_TEMP);
        cmd = AudioSynth_SaveMixedReverbSamples(cmd, mixReverb, updateIndex);
    }

    return cmd;
}

Acmd* AudioSynth_LoadDefaultReverbSamples(Acmd* cmd, s32 numSamplesPerUpdate, SynthesisReverb* reverb,
                                          s16 updateIndex) {
    ReverbBufferEntry* entry = &reverb->bufEntry[reverb->curFrame][updateIndex];

    cmd = AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_LEFT_CH, entry->startPos, entry->size, reverb);
    if (entry->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd = AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_LEFT_CH + entry->size, 0, entry->wrappedSize, reverb);
    }

    return cmd;
}

Acmd* AudioSynth_LoadSubReverbSamples(Acmd* cmd, s32 numSamplesPerUpdate, SynthesisReverb* reverb, s16 updateIndex) {
    ReverbBufferEntry* subEntry = &reverb->subBufEntry[reverb->curFrame][updateIndex];

    cmd = AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_LEFT_CH, subEntry->startPos, subEntry->size, reverb);
    if (subEntry->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd =
            AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_LEFT_CH + subEntry->size, 0, subEntry->wrappedSize, reverb);
    }

    return cmd;
}

Acmd* AudioSynth_SaveResampledReverbSamplesImpl(Acmd* cmd, u16 dmem, u16 size, uintptr_t startAddr) {
    s32 startAddrAlignDropped;
    uintptr_t endAddr;
    s32 endAddrAlignDropped;

    endAddr = startAddr + size;

    endAddrAlignDropped = endAddr & 0xF;
    if (endAddrAlignDropped != 0) {
        aLoadBuffer(cmd++, (endAddr - endAddrAlignDropped), DMEM_TEMP, 0x10);
        aDMEMMove(cmd++, dmem, DMEM_TEMP2, size);
        aDMEMMove(cmd++, DMEM_TEMP + endAddrAlignDropped, size + DMEM_TEMP2, 0x10 - endAddrAlignDropped);

        size += (0x10 - endAddrAlignDropped);
        dmem = DMEM_TEMP2;
    }

    startAddrAlignDropped = startAddr & 0xF;
    if (startAddrAlignDropped != 0) {
        aLoadBuffer(cmd++, startAddr - startAddrAlignDropped, DMEM_TEMP, 0x10);
        aDMEMMove(cmd++, dmem, startAddrAlignDropped + DMEM_TEMP, size);

        size += startAddrAlignDropped;
        dmem = DMEM_TEMP;
    }

    aSaveBuffer(cmd++, dmem, startAddr - startAddrAlignDropped, size);

    return cmd;
}

Acmd* AudioSynth_LoadReverbSamplesImpl(Acmd* cmd, u16 dmem, u16 startPos, s32 size, SynthesisReverb* reverb) {
    aLoadBuffer(cmd++, &reverb->leftReverbBuf[startPos], dmem, size);
    aLoadBuffer(cmd++, &reverb->rightReverbBuf[startPos], dmem + DMEM_1CH_SIZE, size);

    return cmd;
}

Acmd* AudioSynth_SaveReverbSamplesImpl(Acmd* cmd, u16 dmem, u16 startPos, s32 size, SynthesisReverb* reverb) {
    aSaveBuffer(cmd++, dmem, &reverb->leftReverbBuf[startPos], size);
    aSaveBuffer(cmd++, dmem + DMEM_1CH_SIZE, &reverb->rightReverbBuf[startPos], size);

    return cmd;
}

void AudioSynth_Noop26(void) {
}

Acmd* AudioSynth_LoadSubReverbSamplesWithoutDownsample(Acmd* cmd, s32 numSamplesPerUpdate, SynthesisReverb* reverb,
                                                       s16 updateIndex) {
    if (reverb->downsampleRate == 1) {
        cmd = AudioSynth_LoadSubReverbSamples(cmd, numSamplesPerUpdate, reverb, updateIndex);
    }

    return cmd;
}

Acmd* AudioSynth_LoadReverbSamples(Acmd* cmd, s32 numSamplesPerUpdate, SynthesisReverb* reverb, s16 updateIndex) {
    if (reverb->downsampleRate == 1) {
        if (reverb->resampleEffectOn) {
            cmd = AudioSynth_LoadResampledReverbSamples(cmd, numSamplesPerUpdate, reverb, updateIndex);
        } else {
            cmd = AudioSynth_LoadDefaultReverbSamples(cmd, numSamplesPerUpdate, reverb, updateIndex);
        }
    } else {
        cmd = AudioSynth_LoadDownsampledReverbSamples(cmd, numSamplesPerUpdate, reverb, updateIndex);
    }

    return cmd;
}

Acmd* AudioSynth_SaveReverbSamples(Acmd* cmd, SynthesisReverb* reverb, s16 updateIndex) {
    ReverbBufferEntry* entry = &reverb->bufEntry[reverb->curFrame][updateIndex];
    s32 downsampleRate;
    s32 numSamples;

    if (reverb->downsampleRate == 1) {
        if (reverb->resampleEffectOn) {
            cmd = AudioSynth_SaveResampledReverbSamples(cmd, reverb, updateIndex);
        } else {
            // Put the oldest samples in the ring buffer into the wet channels
            cmd = AudioSynth_SaveReverbSamplesImpl(cmd, DMEM_WET_LEFT_CH, entry->startPos, entry->size, reverb);
            if (entry->wrappedSize != 0) {
                // Ring buffer wrapped
                cmd = AudioSynth_SaveReverbSamplesImpl(cmd, DMEM_WET_LEFT_CH + entry->size, 0, entry->wrappedSize,
                                                       reverb);
            }
        }
    } else {
        //! FAKE:
        if (1) {}

        downsampleRate = reverb->downsampleRate;
        numSamples = 13 * SAMPLES_PER_FRAME;

        while (downsampleRate >= 2) {
            aInterl(cmd++, DMEM_WET_LEFT_CH, DMEM_WET_LEFT_CH, numSamples);
            aInterl(cmd++, DMEM_WET_RIGHT_CH, DMEM_WET_RIGHT_CH, numSamples);
            downsampleRate >>= 1;
            numSamples >>= 1;
        }

        if (entry->size != 0) {
            cmd = AudioSynth_SaveResampledReverbSamplesImpl(cmd, DMEM_WET_LEFT_CH, entry->size,
                                                            (uintptr_t)&reverb->leftReverbBuf[entry->startPos]);
            cmd = AudioSynth_SaveResampledReverbSamplesImpl(cmd, DMEM_WET_RIGHT_CH, entry->size,
                                                            (uintptr_t)&reverb->rightReverbBuf[entry->startPos]);
        }

        if (entry->wrappedSize != 0) {
            cmd = AudioSynth_SaveResampledReverbSamplesImpl(cmd, entry->size + DMEM_WET_LEFT_CH, entry->wrappedSize,
                                                            (uintptr_t)reverb->leftReverbBuf);
            cmd = AudioSynth_SaveResampledReverbSamplesImpl(cmd, entry->size + DMEM_WET_RIGHT_CH, entry->wrappedSize,
                                                            (uintptr_t)reverb->rightReverbBuf);
        }
    }

    reverb->resampleFlags = 0;

    return cmd;
}

Acmd* AudioSynth_SaveSubReverbSamples(Acmd* cmd, SynthesisReverb* reverb, s16 updateIndex) {
    ReverbBufferEntry* subEntry = &reverb->subBufEntry[reverb->curFrame][updateIndex];

    cmd = AudioSynth_SaveReverbSamplesImpl(cmd, DMEM_WET_LEFT_CH, subEntry->startPos, subEntry->size, reverb);
    if (subEntry->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd =
            AudioSynth_SaveReverbSamplesImpl(cmd, DMEM_WET_LEFT_CH + subEntry->size, 0, subEntry->wrappedSize, reverb);
    }

    return cmd;
}

/**
 * Process all samples embedded in a note. Every sample has numSamplesPerUpdate processed,
 * and each of those are mixed together into both DMEM_LEFT_CH and DMEM_RIGHT_CH
 */
Acmd* AudioSynth_ProcessSamples(s16* aiBuf, s32 numSamplesPerUpdate, Acmd* cmd, s32 updateIndex) {
    s32 size;
    u8 noteIndices[0x58];
    s16 noteCount = 0;
    s16 reverbIndex;
    SynthesisReverb* reverb;
    s32 useReverb;
    s32 sampleStateOffset = gAudioCtx.numNotes * updateIndex;
    s32 i;

    if (gAudioCtx.numSynthesisReverbs == 0) {
        for (i = 0; i < gAudioCtx.numNotes; i++) {
            if (gAudioCtx.sampleStateList[sampleStateOffset + i].bitField0.enabled) {
                noteIndices[noteCount++] = i;
            }
        }
    } else {
        NoteSampleState* sampleState;

        for (reverbIndex = 0; reverbIndex < gAudioCtx.numSynthesisReverbs; reverbIndex++) {
            for (i = 0; i < gAudioCtx.numNotes; i++) {
                sampleState = &gAudioCtx.sampleStateList[sampleStateOffset + i];
                if (sampleState->bitField0.enabled && (sampleState->bitField1.reverbIndex == reverbIndex)) {
                    noteIndices[noteCount++] = i;
                }
            }
        }

        for (i = 0; i < gAudioCtx.numNotes; i++) {
            sampleState = &gAudioCtx.sampleStateList[sampleStateOffset + i];
            if (sampleState->bitField0.enabled &&
                (sampleState->bitField1.reverbIndex >= gAudioCtx.numSynthesisReverbs)) {
                noteIndices[noteCount++] = i;
            }
        }
    }

    aClearBuffer(cmd++, DMEM_LEFT_CH, DMEM_2CH_SIZE);

    i = 0;
    for (reverbIndex = 0; reverbIndex < gAudioCtx.numSynthesisReverbs; reverbIndex++) {
        s32 subDelay;
        NoteSampleState* sampleState;

        reverb = &gAudioCtx.synthesisReverbs[reverbIndex];
        useReverb = reverb->useReverb;
        if (useReverb) {

            // Loads reverb samples from DRAM (ringBuffer) into DMEM (DMEM_WET_LEFT_CH)
            cmd = AudioSynth_LoadReverbSamples(cmd, numSamplesPerUpdate, reverb, updateIndex);

            // Mixes reverb sample into the main dry channel
            // reverb->volume is always set to 0x7FFF (audio spec), and DMEM_LEFT_CH is cleared before reverbs.
            // So this is essentially a DMEMmove from DMEM_WET_LEFT_CH to DMEM_LEFT_CH
            aMix(cmd++, DMEM_2CH_SIZE >> 4, reverb->volume, DMEM_WET_LEFT_CH, DMEM_LEFT_CH);

            subDelay = reverb->subDelay;
            if (subDelay != 0) {
                aDMEMMove(cmd++, DMEM_WET_LEFT_CH, DMEM_WET_TEMP, DMEM_2CH_SIZE);
            }

            // Decays reverb over time. The (+ 0x8000) here is -100%
            aMix(cmd++, DMEM_2CH_SIZE >> 4, reverb->decayRatio + 0x8000, DMEM_WET_LEFT_CH, DMEM_WET_LEFT_CH);

            if (((reverb->leakRtl != 0) || (reverb->leakLtr != 0)) && (gAudioCtx.soundMode != SOUNDMODE_MONO)) {
                cmd = AudioSynth_LeakReverb(cmd, reverb);
            }

            if (subDelay != 0) {
                if (reverb->mixReverbIndex != REVERB_INDEX_NONE) {
                    cmd = AudioSynth_MixOtherReverbIndex(cmd, reverb, updateIndex);
                }
                cmd = AudioSynth_SaveReverbSamples(cmd, reverb, updateIndex);
                cmd = AudioSynth_LoadSubReverbSamplesWithoutDownsample(cmd, numSamplesPerUpdate, reverb, updateIndex);
                aMix(cmd++, DMEM_2CH_SIZE >> 4, reverb->subVolume, DMEM_WET_TEMP, DMEM_WET_LEFT_CH);
            }
        }

        while (i < noteCount) {
            sampleState = &gAudioCtx.sampleStateList[sampleStateOffset + noteIndices[i]];
            if (sampleState->bitField1.reverbIndex != reverbIndex) {
                break;
            }
            cmd = AudioSynth_ProcessSample(noteIndices[i], sampleState, &gAudioCtx.notes[noteIndices[i]].synthesisState,
                                           aiBuf, numSamplesPerUpdate, cmd, updateIndex);
            i++;
        }

        if (useReverb) {
            if ((reverb->filterLeft != NULL) || (reverb->filterRight != NULL)) {
                cmd = AudioSynth_FilterReverb(cmd, numSamplesPerUpdate * SAMPLE_SIZE, reverb);
            }

            // Saves the wet channel sample from DMEM (DMEM_WET_LEFT_CH) into (ringBuffer) DRAM for future use
            if (subDelay != 0) {
                cmd = AudioSynth_SaveSubReverbSamples(cmd, reverb, updateIndex);
            } else {
                if (reverb->mixReverbIndex != REVERB_INDEX_NONE) {
                    cmd = AudioSynth_MixOtherReverbIndex(cmd, reverb, updateIndex);
                }
                cmd = AudioSynth_SaveReverbSamples(cmd, reverb, updateIndex);
            }
        }
    }

    while (i < noteCount) {
        cmd = AudioSynth_ProcessSample(noteIndices[i], &gAudioCtx.sampleStateList[sampleStateOffset + noteIndices[i]],
                                       &gAudioCtx.notes[noteIndices[i]].synthesisState, aiBuf, numSamplesPerUpdate, cmd,
                                       updateIndex);
        i++;
    }

    size = numSamplesPerUpdate * SAMPLE_SIZE;
    if (CVarGetInteger("gModes.MirroredWorld.State", 0)) {
        aInterleave(cmd++, DMEM_TEMP, DMEM_RIGHT_CH, DMEM_LEFT_CH, size);
    } else {
        aInterleave(cmd++, DMEM_TEMP, DMEM_LEFT_CH, DMEM_RIGHT_CH, size);
    }

    if (gAudioCustomSynthFunction != NULL) {
        cmd = gAudioCustomSynthFunction(cmd, 2 * size, updateIndex);
    }
    aSaveBuffer(cmd++, DMEM_TEMP, aiBuf, 2 * size);

    return cmd;
}

Acmd* AudioSynth_ProcessSample(s32 noteIndex, NoteSampleState* sampleState, NoteSynthesisState* synthState, s16* aiBuf,
                               s32 numSamplesPerUpdate, Acmd* cmd, s32 updateIndex) {
    s32 pad1[2];
    void* reverbAddrSrc;
    Sample* sample;
    AdpcmLoop* loopInfo;
    s32 numSamplesUntilEnd;
    s32 numSamplesInThisIteration;
    s32 sampleFinished;
    s32 loopToPoint;
    s32 flags;
    u16 frequencyFixedPoint;
    s32 gain;
    s32 frameIndex;
    s32 skipBytes;
    void* combFilterState;
    s32 numSamplesToDecode;
    s32 numFirstFrameSamplesToIgnore;
    u8* sampleAddr;
    u32 numSamplesToLoadFixedPoint;
    s32 numSamplesToLoadAdj;
    s32 numSamplesProcessed;
    s32 sampleEndPos;
    s32 numSamplesToProcess;
    s32 dmemUncompressedAddrOffset2;
    s32 pad2[3];
    s32 numSamplesInFirstFrame;
    s32 numTrailingSamplesToIgnore;
    s32 pad3[3];
    s32 frameSize;
    s32 numFramesToDecode;
    s32 skipInitialSamples;
    s32 zeroOffset;
    u8* samplesToLoadAddr;
    s32 numParts;
    s32 curPart;
    s32 sampleDataChunkAlignPad;
    s32 haasEffectDelaySide;
    s32 numSamplesToLoadFirstPart;
    u16 sampleDmemBeforeResampling;
    s32 sampleAddrOffset;
    s32 combFilterDmem;
    s32 dmemUncompressedAddrOffset1;
    Note* note;
    u32 numSamplesToLoad;
    u16 combFilterSize;
    u16 combFilterGain;
    s16* filter;
    s32 bookOffset = sampleState->bitField1.bookOffset;
    s32 finished = sampleState->bitField0.finished;
    s32 sampleDataChunkSize;
    s16 sampleDataDmemAddr;

    note = &gAudioCtx.notes[noteIndex];
    flags = A_CONTINUE;

    // Initialize the synthesis state
    if (sampleState->bitField0.needsInit == true) {
        flags = A_INIT;
        synthState->atLoopPoint = false;
        synthState->stopLoop = false;
        synthState->samplePosInt = note->playbackState.startSamplePos;
        synthState->samplePosFrac = 0;
        synthState->curVolLeft = 0;
        synthState->curVolRight = 0;
        synthState->prevHaasEffectLeftDelaySize = 0;
        synthState->prevHaasEffectRightDelaySize = 0;
        synthState->curReverbVol = sampleState->targetReverbVol;
        synthState->numParts = 0;
        synthState->combFilterNeedsInit = true;
        note->sampleState.bitField0.finished = false;
        synthState->unk_1F = note->playbackState.unk_80; // Never set, never used
        finished = false;
    }

    // Process the sample in either one or two parts
    numParts = sampleState->bitField1.hasTwoParts + 1;

    // Determine number of samples to load based on numSamplesPerUpdate and relative frequency
    frequencyFixedPoint = sampleState->frequencyFixedPoint;
    numSamplesToLoadFixedPoint = (frequencyFixedPoint * numSamplesPerUpdate * 2) + synthState->samplePosFrac;
    numSamplesToLoad = numSamplesToLoadFixedPoint >> 16;

    if (numSamplesToLoad == 0) {
        skipBytes = false;
    }

    synthState->samplePosFrac = numSamplesToLoadFixedPoint & 0xFFFF;

    // Partially-optimized out no-op ifs required for matching. SM64 decomp
    // makes it clear that this is how it should look.
    if ((synthState->numParts == 1) && (numParts == 2)) {
    } else if ((synthState->numParts == 2) && (numParts == 1)) {
    } else {
    }

    synthState->numParts = numParts;

    if (sampleState->bitField1.isSyntheticWave) {
        cmd = AudioSynth_LoadWaveSamples(cmd, sampleState, synthState, numSamplesToLoad);
        sampleDmemBeforeResampling = DMEM_UNCOMPRESSED_NOTE + (synthState->samplePosInt * 2);
        synthState->samplePosInt += numSamplesToLoad;
    } else {
        sample = sampleState->tunedSample->sample;
        loopInfo = sample->loop;

        if (note->playbackState.status != PLAYBACK_STATUS_0) {
            synthState->stopLoop = true;
        }

        if ((loopInfo->count == 2) && synthState->stopLoop) {
            sampleEndPos = loopInfo->sampleEnd;
        } else {
            sampleEndPos = loopInfo->loopEnd;
        }

        sampleAddr = sample->sampleAddr;
        numSamplesToLoadFirstPart = 0;

        // If the frequency requested is more than double that of the raw sample,
        // then the sample processing is split into two parts.
        for (curPart = 0; curPart < numParts; curPart++) {
            numSamplesProcessed = 0;
            dmemUncompressedAddrOffset1 = 0;

            // Adjust the number of samples to load only if there are two parts and an odd number of samples
            if (numParts == 1) {
                numSamplesToLoadAdj = numSamplesToLoad;
            } else if (numSamplesToLoad & 1) {
                // round down for the first part
                // round up for the second part
                numSamplesToLoadAdj = (numSamplesToLoad & ~1) + (curPart * 2);
            } else {
                numSamplesToLoadAdj = numSamplesToLoad;
            }

            // Load the ADPCM codeBook
            if ((sample->codec == CODEC_ADPCM) || (sample->codec == CODEC_SMALL_ADPCM)) {
                if (gAudioCtx.adpcmCodeBook != sample->book->codeBook) {
                    u32 numEntries;

                    switch (bookOffset) {
                        case 1:
                            gAudioCtx.adpcmCodeBook = &gInvalidAdpcmCodeBook[1];
                            break;

                        case 2:
                        case 3:
                        default:
                            gAudioCtx.adpcmCodeBook = sample->book->codeBook;
                            break;
                    }

                    numEntries = SAMPLES_PER_FRAME * sample->book->order * sample->book->numPredictors;
                    // ResourceCheckSample(sample->sampleAddr, sample->book->codeBook);
                    aLoadADPCM(cmd++, numEntries, gAudioCtx.adpcmCodeBook);
                }
            }

            // Continue processing samples until the number of samples needed to load is reached
            while (numSamplesProcessed != numSamplesToLoadAdj) {
                sampleFinished = false;
                loopToPoint = false;
                dmemUncompressedAddrOffset2 = 0;

                numFirstFrameSamplesToIgnore = synthState->samplePosInt & 0xF;
                numSamplesUntilEnd = sampleEndPos - synthState->samplePosInt;

                // Calculate number of samples to process this loop
                numSamplesToProcess = numSamplesToLoadAdj - numSamplesProcessed;

                if ((numFirstFrameSamplesToIgnore == 0) && !synthState->atLoopPoint) {
                    numFirstFrameSamplesToIgnore = SAMPLES_PER_FRAME;
                }
                numSamplesInFirstFrame = SAMPLES_PER_FRAME - numFirstFrameSamplesToIgnore;

                // Determine the number of samples to decode based on whether the end will be reached or not.
                if (numSamplesToProcess < numSamplesUntilEnd) {
                    // The end will not be reached.
                    numFramesToDecode =
                        (s32)(numSamplesToProcess - numSamplesInFirstFrame + SAMPLES_PER_FRAME - 1) / SAMPLES_PER_FRAME;
                    numSamplesToDecode = numFramesToDecode * SAMPLES_PER_FRAME;
                    numTrailingSamplesToIgnore = numSamplesInFirstFrame + numSamplesToDecode - numSamplesToProcess;
                } else {
                    // The end will be reached.
                    numSamplesToDecode = numSamplesUntilEnd - numSamplesInFirstFrame;
                    numTrailingSamplesToIgnore = 0;
                    if (numSamplesToDecode <= 0) {
                        numSamplesToDecode = 0;
                        numSamplesInFirstFrame = numSamplesUntilEnd;
                    }
                    numFramesToDecode = (numSamplesToDecode + SAMPLES_PER_FRAME - 1) / SAMPLES_PER_FRAME;
                    if (loopInfo->count != 0) {
                        if ((loopInfo->count == 2) && synthState->stopLoop) {
                            sampleFinished = true;
                        } else {
                            // Loop around and restart
                            loopToPoint = true;
                        }
                    } else {
                        sampleFinished = true;
                    }
                }

                // Set parameters based on compression type
                switch (sample->codec) {
                    case CODEC_ADPCM:
                        // 16 2-byte samples (32 bytes) compressed into 4-bit samples (8 bytes) + 1 header byte
                        frameSize = 9;
                        skipInitialSamples = SAMPLES_PER_FRAME;
                        zeroOffset = 0;
                        break;

                    case CODEC_SMALL_ADPCM:
                        // 16 2-byte samples (32 bytes) compressed into 2-bit samples (4 bytes) + 1 header byte
                        frameSize = 5;
                        skipInitialSamples = SAMPLES_PER_FRAME;
                        zeroOffset = 0;
                        break;

                    case CODEC_UNK7:
                        // 2 2-byte samples (4 bytes) processed without decompression
                        frameSize = 4;
                        skipInitialSamples = SAMPLES_PER_FRAME;
                        zeroOffset = 0;
                        break;

                    case CODEC_S8:
                        // 16 2-byte samples (32 bytes) compressed into 8-bit samples (16 bytes)
                        frameSize = 16;
                        skipInitialSamples = SAMPLES_PER_FRAME;
                        zeroOffset = 0;
                        break;

                    case CODEC_REVERB:
                        reverbAddrSrc = (void*)0xFFFFFFFF;
                        if (gAudioCustomReverbFunction != NULL) {
                            reverbAddrSrc = gAudioCustomReverbFunction(sample, numSamplesToLoadAdj, flags, noteIndex);
                        }

                        if (reverbAddrSrc == (void*)0xFFFFFFFF) {
                            sampleFinished = true;
                        } else if (reverbAddrSrc == NULL) {
                            return cmd;
                        } else {
                            AudioSynth_LoadBuffer(cmd++, DMEM_UNCOMPRESSED_NOTE,
                                                  (numSamplesToLoadAdj + SAMPLES_PER_FRAME) * SAMPLE_SIZE,
                                                  reverbAddrSrc);
                            flags = A_CONTINUE;
                            skipBytes = 0;
                            numSamplesProcessed = numSamplesToLoadAdj;
                            dmemUncompressedAddrOffset1 = numSamplesToLoadAdj;
                        }
                        goto skip;

                    case CODEC_S16_INMEMORY:
                    case CODEC_UNK6:
                        AudioSynth_ClearBuffer(cmd++, DMEM_UNCOMPRESSED_NOTE,
                                               (numSamplesToLoadAdj + SAMPLES_PER_FRAME) * SAMPLE_SIZE);
                        flags = A_CONTINUE;
                        skipBytes = 0;
                        numSamplesProcessed = numSamplesToLoadAdj;
                        dmemUncompressedAddrOffset1 = numSamplesToLoadAdj;
                        goto skip;

                    case CODEC_OPUS:
                    case CODEC_S16:
                        AudioSynth_ClearBuffer(cmd++, DMEM_UNCOMPRESSED_NOTE,
                                               (numSamplesToLoadAdj + SAMPLES_PER_FRAME) * SAMPLE_SIZE);
                        flags = A_CONTINUE;
                        skipBytes = 0;
                        size_t bytesToRead;
                        numSamplesProcessed += numSamplesToLoadAdj;
                        dmemUncompressedAddrOffset1 = numSamplesToLoadAdj;

                        if (((synthState->samplePosInt * 2) + (numSamplesToLoadAdj)*SAMPLE_SIZE) < sample->size) {
                            bytesToRead = (numSamplesToLoadAdj)*SAMPLE_SIZE;
                        } else {
                            bytesToRead = sample->size - (synthState->samplePosInt * 2);
                        }
                        // 2S2H [Port] [Custom audio] Handle decoding OPUS data
                        if (sample->codec == CODEC_OPUS) {
                            aOPUSdecImpl(sampleAddr, DMEM_UNCOMPRESSED_NOTE, bytesToRead, &synthState->opusFile,
                                         synthState->samplePosInt, sample->fileSize);
                        } else {
                            aLoadBuffer(cmd++, sampleAddr + (synthState->samplePosInt * 2), DMEM_UNCOMPRESSED_NOTE,
                                        bytesToRead);
                        }

                        goto skip;

                    default:
                        break;
                }

                // Move the compressed raw sample data from ram into the rsp (DMEM)
                if (numFramesToDecode != 0) {
                    // Get the offset from the start of the sample to where the sample is currently playing from
                    frameIndex = (synthState->samplePosInt + skipInitialSamples - numFirstFrameSamplesToIgnore) /
                                 SAMPLES_PER_FRAME;
                    sampleAddrOffset = frameIndex * frameSize;

                    // Get the ram address of the requested sample chunk
                    if (sample->medium == MEDIUM_RAM) {
                        // Sample is already loaded into ram
                        samplesToLoadAddr = sampleAddr + (zeroOffset + sampleAddrOffset);
                    } else if (gAudioCtx.unk_29B8) { // always false
                        return cmd;
                    } else if (sample->medium == MEDIUM_UNK) {
                        // This medium is unsupported so terminate processing this note
                        return cmd;
                    } else {
                        // This medium is not in ram, so dma the requested sample into ram
                        // BEN: Assume all samples are in RAM... because they are.
                        samplesToLoadAddr = sampleAddr + (zeroOffset + sampleAddrOffset);
                    }

                    if (samplesToLoadAddr == NULL) {
                        // The ram address was unsuccessfully allocated
                        return cmd;
                    }

                    // Move the raw sample chunk from ram to the rsp
                    // DMEM at the addresses before DMEM_COMPRESSED_ADPCM_DATA
                    sampleDataChunkAlignPad = (uintptr_t)samplesToLoadAddr & 0xF;
                    sampleDataChunkSize = ALIGN16((numFramesToDecode * frameSize) + SAMPLES_PER_FRAME);
                    sampleDataDmemAddr = DMEM_COMPRESSED_ADPCM_DATA - sampleDataChunkSize;
                    // 2S2H [Port] This fixes an overflow that crashes the asan, but causes some ADPCM sounds to cut off
                    // early.
#if __SANITIZE_ADDRESS__
                    uintptr_t actualAddrLoaded = samplesToLoadAddr - sampleDataChunkAlignPad;
                    uintptr_t offset = actualAddrLoaded - (uintptr_t)sampleAddr;
                    if (offset + sampleDataChunkSize > sample->size) {
                        sampleDataChunkSize -= (offset + sampleDataChunkSize - sample->size);
                    }
#endif
                    // BEN: This will crash the asan. We can just ignore alignment since we don't have those strictures.
                    // if (sampleDataChunkSize + sampleAddrOffset > sample->size) {
                    //    sampleDataChunkSize = sample->size - sampleAddrOffset;
                    //    sampleDataDmemAddr = DMEM_COMPRESSED_ADPCM_DATA - sampleDataChunkSize;
                    //}

                    aLoadBuffer(cmd++, samplesToLoadAddr - sampleDataChunkAlignPad, sampleDataDmemAddr,
                                sampleDataChunkSize);
                } else {
                    numSamplesToDecode = 0;
                    sampleDataChunkAlignPad = 0;
                }

                if (synthState->atLoopPoint) {
                    aSetLoop(cmd++, sample->loop->predictorState);
                    flags = A_LOOP;
                    synthState->atLoopPoint = false;
                }

                numSamplesInThisIteration = numSamplesToDecode + numSamplesInFirstFrame - numTrailingSamplesToIgnore;

                if (numSamplesProcessed == 0) {
                    //! FAKE:
                    if (1) {}
                    skipBytes = numFirstFrameSamplesToIgnore * SAMPLE_SIZE;
                } else {
                    dmemUncompressedAddrOffset2 = ALIGN16(dmemUncompressedAddrOffset1 + 8 * SAMPLE_SIZE);
                }

                // Decompress the raw sample chunks in the rsp
                // Goes from adpcm (compressed) sample data to pcm (uncompressed) sample data
                switch (sample->codec) {
                    case CODEC_ADPCM:
                        sampleDataChunkSize = ALIGN16((numFramesToDecode * frameSize) + SAMPLES_PER_FRAME);
                        sampleDataDmemAddr = DMEM_COMPRESSED_ADPCM_DATA - sampleDataChunkSize;
                        aSetBuffer(cmd++, 0, sampleDataDmemAddr + sampleDataChunkAlignPad,
                                   DMEM_UNCOMPRESSED_NOTE + dmemUncompressedAddrOffset2,
                                   numSamplesToDecode * SAMPLE_SIZE);
                        aADPCMdec(cmd++, flags, synthState->synthesisBuffers->adpcmState);
                        break;

                    case CODEC_SMALL_ADPCM:
                        sampleDataChunkSize = ALIGN16((numFramesToDecode * frameSize) + SAMPLES_PER_FRAME);
                        sampleDataDmemAddr = DMEM_COMPRESSED_ADPCM_DATA - sampleDataChunkSize;
                        aSetBuffer(cmd++, 0, sampleDataDmemAddr + sampleDataChunkAlignPad,
                                   DMEM_UNCOMPRESSED_NOTE + dmemUncompressedAddrOffset2,
                                   numSamplesToDecode * SAMPLE_SIZE);
                        aADPCMdec(cmd++, flags | A_ADPCM_SHORT, synthState->synthesisBuffers->adpcmState);
                        break;

                    case CODEC_S8:
                        sampleDataChunkSize = ALIGN16((numFramesToDecode * frameSize) + SAMPLES_PER_FRAME);
                        sampleDataDmemAddr = DMEM_COMPRESSED_ADPCM_DATA - sampleDataChunkSize;
                        AudioSynth_SetBuffer(cmd++, 0, sampleDataDmemAddr + sampleDataChunkAlignPad,
                                             DMEM_UNCOMPRESSED_NOTE + dmemUncompressedAddrOffset2,
                                             numSamplesToDecode * SAMPLE_SIZE);
                        AudioSynth_S8Dec(cmd++, flags, synthState->synthesisBuffers->adpcmState);
                        break;

                    case CODEC_UNK7:
                    default:
                        // No decompression
                        break;
                }

                if (numSamplesProcessed != 0) {
                    aDMEMMove(cmd++,
                              DMEM_UNCOMPRESSED_NOTE + dmemUncompressedAddrOffset2 +
                                  (numFirstFrameSamplesToIgnore * SAMPLE_SIZE),
                              DMEM_UNCOMPRESSED_NOTE + dmemUncompressedAddrOffset1,
                              numSamplesInThisIteration * SAMPLE_SIZE);
                }

                numSamplesProcessed += numSamplesInThisIteration;

                switch (flags) {
                    case A_INIT:
                        skipBytes = SAMPLES_PER_FRAME * SAMPLE_SIZE;
                        dmemUncompressedAddrOffset1 = (numSamplesToDecode + SAMPLES_PER_FRAME) * SAMPLE_SIZE;
                        break;

                    case A_LOOP:
                        dmemUncompressedAddrOffset1 =
                            numSamplesInThisIteration * SAMPLE_SIZE + dmemUncompressedAddrOffset1;
                        break;

                    default:
                        if (dmemUncompressedAddrOffset1 != 0) {
                            dmemUncompressedAddrOffset1 =
                                numSamplesInThisIteration * SAMPLE_SIZE + dmemUncompressedAddrOffset1;
                        } else {
                            dmemUncompressedAddrOffset1 =
                                (numFirstFrameSamplesToIgnore + numSamplesInThisIteration) * SAMPLE_SIZE;
                        }
                        break;
                }

                flags = A_CONTINUE;

            skip:
                // Update what to do with the samples next
                if (sampleFinished) {
                    if ((numSamplesToLoadAdj - numSamplesProcessed) != 0) {
                        AudioSynth_ClearBuffer(cmd++, DMEM_UNCOMPRESSED_NOTE + dmemUncompressedAddrOffset1,
                                               (numSamplesToLoadAdj - numSamplesProcessed) * SAMPLE_SIZE);
                    }
                    finished = true;
                    note->sampleState.bitField0.finished = true;
                    AudioSynth_DisableSampleStates(updateIndex, noteIndex);
                    break; // break out of the for-loop
                } else if (loopToPoint) {
                    synthState->atLoopPoint = true;
                    synthState->samplePosInt = loopInfo->start;
                } else {
                    synthState->samplePosInt += numSamplesToProcess;
                }
            }

            switch (numParts) {
                case 1:
                    sampleDmemBeforeResampling = DMEM_UNCOMPRESSED_NOTE + skipBytes;
                    break;

                case 2:
                    switch (curPart) {
                        case 0:
                            AudioSynth_InterL(cmd++, DMEM_UNCOMPRESSED_NOTE + skipBytes,
                                              DMEM_TEMP + (SAMPLES_PER_FRAME * SAMPLE_SIZE),
                                              ALIGN8(numSamplesToLoadAdj / 2));
                            numSamplesToLoadFirstPart = numSamplesToLoadAdj;
                            sampleDmemBeforeResampling = DMEM_TEMP + (SAMPLES_PER_FRAME * SAMPLE_SIZE);
                            if (finished) {
                                AudioSynth_ClearBuffer(cmd++, sampleDmemBeforeResampling + numSamplesToLoadFirstPart,
                                                       numSamplesToLoadAdj + SAMPLES_PER_FRAME);
                            }
                            break;

                        case 1:
                            AudioSynth_InterL(cmd++, DMEM_UNCOMPRESSED_NOTE + skipBytes,
                                              DMEM_TEMP + (SAMPLES_PER_FRAME * SAMPLE_SIZE) + numSamplesToLoadFirstPart,
                                              ALIGN8(numSamplesToLoadAdj / 2));
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
            if (finished) {
                break;
            }
        }
    }

    // Update the flags for the signal processing below
    flags = A_CONTINUE;
    if (sampleState->bitField0.needsInit == true) {
        sampleState->bitField0.needsInit = false;
        flags = A_INIT;
    }

    // Resample the decompressed mono-signal to the correct pitch
    cmd = AudioSynth_FinalResample(cmd, synthState, numSamplesPerUpdate * SAMPLE_SIZE, frequencyFixedPoint,
                                   sampleDmemBeforeResampling, flags);

    // UnkCmd19 was removed from the audio microcode
    // This block performs no operation
    if (bookOffset == 3) {
        AudioSynth_UnkCmd19(cmd++, DMEM_TEMP, DMEM_TEMP, numSamplesPerUpdate * (s32)SAMPLE_SIZE, 0);
    }

    // Apply the gain to the mono-signal to adjust the volume
    gain = sampleState->gain;
    if (gain != 0) {
        // A gain of 0x10 (a UQ4.4 number) is equivalent to 1.0 and represents no volume change
        if (gain < 0x10) {
            gain = 0x10;
        }
        AudioSynth_HiLoGain(cmd++, gain, DMEM_TEMP, 0, (numSamplesPerUpdate + SAMPLES_PER_FRAME) * SAMPLE_SIZE);
    }

    // Apply the filter to the mono-signal
    filter = sampleState->filter;
    if (filter != 0) {
        AudioSynth_LoadFilterSize(cmd++, numSamplesPerUpdate * SAMPLE_SIZE, filter);
        AudioSynth_LoadFilterBuffer(cmd++, flags, DMEM_TEMP, synthState->synthesisBuffers->filterState);
    }

    // Apply the comb filter to the mono-signal by taking the signal with a small temporal offset,
    // and adding it back to itself
    combFilterSize = sampleState->combFilterSize;
    combFilterGain = sampleState->combFilterGain;
    combFilterState = synthState->synthesisBuffers->combFilterState;
    if ((combFilterSize != 0) && (sampleState->combFilterGain != 0)) {
        AudioSynth_DMemMove(cmd++, DMEM_TEMP, DMEM_COMB_TEMP, numSamplesPerUpdate * SAMPLE_SIZE);
        combFilterDmem = DMEM_COMB_TEMP - combFilterSize;
        if (synthState->combFilterNeedsInit) {
            AudioSynth_ClearBuffer(cmd++, combFilterDmem, combFilterSize);
            synthState->combFilterNeedsInit = false;
        } else {
            AudioSynth_LoadBuffer(cmd++, combFilterDmem, combFilterSize, combFilterState);
        }
        AudioSynth_SaveBuffer(cmd++, DMEM_TEMP + (numSamplesPerUpdate * SAMPLE_SIZE) - combFilterSize, combFilterSize,
                              combFilterState);
        AudioSynth_Mix(cmd++, (numSamplesPerUpdate * (s32)SAMPLE_SIZE) >> 4, combFilterGain, DMEM_COMB_TEMP,
                       combFilterDmem);
        AudioSynth_DMemMove(cmd++, combFilterDmem, DMEM_TEMP, numSamplesPerUpdate * SAMPLE_SIZE);
    } else {
        synthState->combFilterNeedsInit = true;
    }

    // Determine the behavior of the audio processing that leads to the haas effect
    if ((sampleState->haasEffectLeftDelaySize != 0) || (synthState->prevHaasEffectLeftDelaySize != 0)) {
        haasEffectDelaySide = HAAS_EFFECT_DELAY_LEFT;
    } else if ((sampleState->haasEffectRightDelaySize != 0) || (synthState->prevHaasEffectRightDelaySize != 0)) {
        haasEffectDelaySide = HAAS_EFFECT_DELAY_RIGHT;
    } else {
        haasEffectDelaySide = HAAS_EFFECT_DELAY_NONE;
    }

    // Apply an unknown effect based on the surround sound-mode
    if (gAudioCtx.soundMode == SOUNDMODE_SURROUND) {
        sampleState->targetVolLeft = sampleState->targetVolLeft >> 1;
        sampleState->targetVolRight = sampleState->targetVolRight >> 1;
        if (sampleState->surroundEffectIndex != 0xFF) {
            cmd = AudioSynth_ApplySurroundEffect(cmd, sampleState, synthState, numSamplesPerUpdate, DMEM_TEMP, flags);
        }
    }

    // Split the mono-signal into left and right channels:
    // Both for dry signal (to go to the speakers now)
    // and for wet signal (to go to a reverb buffer to be stored, and brought back later to produce an echo)
    cmd = AudioSynth_ProcessEnvelope(cmd, sampleState, synthState, numSamplesPerUpdate, DMEM_TEMP, haasEffectDelaySide,
                                     flags);

    // Apply the haas effect by delaying either the left or the right channel by a small amount
    if (sampleState->bitField1.useHaasEffect) {
        if (!(flags & A_INIT)) {
            flags = A_CONTINUE;
        }
        cmd = AudioSynth_ApplyHaasEffect(cmd, sampleState, synthState, numSamplesPerUpdate * (s32)SAMPLE_SIZE, flags,
                                         haasEffectDelaySide);
    }

    return cmd;
}

Acmd* AudioSynth_ApplySurroundEffect(Acmd* cmd, NoteSampleState* sampleState, NoteSynthesisState* synthState,
                                     s32 numSamplesPerUpdate, s32 haasDmem, s32 flags) {
    s32 wetGain;
    u16 dryGain;
    s64 dmem = DMEM_SURROUND_TEMP;
    f32 decayGain;

    AudioSynth_DMemMove(cmd++, haasDmem, DMEM_HAAS_TEMP, numSamplesPerUpdate * SAMPLE_SIZE);
    dryGain = synthState->surroundEffectGain;

    if (flags == A_INIT) {
        aClearBuffer(cmd++, dmem, sizeof(synthState->synthesisBuffers->surroundEffectState));
        synthState->surroundEffectGain = 0;
    } else {
        aLoadBuffer(cmd++, synthState->synthesisBuffers->surroundEffectState, dmem,
                    sizeof(synthState->synthesisBuffers->surroundEffectState));
        aMix(cmd++, (numSamplesPerUpdate * (s32)SAMPLE_SIZE) >> 4, dryGain, dmem, DMEM_LEFT_CH);
        aMix(cmd++, (numSamplesPerUpdate * (s32)SAMPLE_SIZE) >> 4, (dryGain ^ 0xFFFF), dmem, DMEM_RIGHT_CH);

        wetGain = (dryGain * synthState->curReverbVol) >> 7;

        aMix(cmd++, (numSamplesPerUpdate * (s32)SAMPLE_SIZE) >> 4, wetGain, dmem, DMEM_WET_LEFT_CH);
        aMix(cmd++, (numSamplesPerUpdate * (s32)SAMPLE_SIZE) >> 4, (wetGain ^ 0xFFFF), dmem, DMEM_WET_RIGHT_CH);
    }

    aSaveBuffer(cmd++, DMEM_SURROUND_TEMP + (numSamplesPerUpdate * SAMPLE_SIZE),
                synthState->synthesisBuffers->surroundEffectState,
                sizeof(synthState->synthesisBuffers->surroundEffectState));

    decayGain = (sampleState->targetVolLeft + sampleState->targetVolRight) * (1.0f / 0x2000);

    if (decayGain > 1.0f) {
        decayGain = 1.0f;
    }

    decayGain = decayGain * gDefaultPanVolume[127 - sampleState->surroundEffectIndex];
    synthState->surroundEffectGain = ((decayGain * 0x7FFF) + synthState->surroundEffectGain) / 2;

    AudioSynth_DMemMove(cmd++, DMEM_HAAS_TEMP, haasDmem, numSamplesPerUpdate * SAMPLE_SIZE);

    return cmd;
}

Acmd* AudioSynth_FinalResample(Acmd* cmd, NoteSynthesisState* synthState, s32 size, u16 pitch, u16 inpDmem,
                               s32 resampleFlags) {
    if (pitch == 0) {
        AudioSynth_ClearBuffer(cmd++, DMEM_TEMP, size);
    } else {
        aSetBuffer(cmd++, 0, inpDmem, DMEM_TEMP, size);
        aResample(cmd++, resampleFlags, pitch, synthState->synthesisBuffers->finalResampleState);
    }

    return cmd;
}

Acmd* AudioSynth_ProcessEnvelope(Acmd* cmd, NoteSampleState* sampleState, NoteSynthesisState* synthState,
                                 s32 numSamplesPerUpdate, u16 dmemSrc, s32 haasEffectDelaySide, s32 flags) {
    u32 dmemDests;
    u16 curVolLeft;
    u16 targetVolLeft;
    s32 curReverbVol;
    u16 curVolRight;
    s16 targetReverbVol;
    s16 rampLeft;
    s16 rampRight;
    s16 rampReverb;
    s16 curReverbVolAndFlags;
    u16 targetVolRight;
    f32 defaultPanVolume;
    s32 pad;

    targetReverbVol = sampleState->targetReverbVol;

    curVolLeft = synthState->curVolLeft;
    curVolRight = synthState->curVolRight;

    targetVolLeft = sampleState->targetVolLeft;
    targetVolLeft <<= 4;
    targetVolRight = sampleState->targetVolRight;
    targetVolRight <<= 4;

    if ((gAudioCtx.soundMode == SOUNDMODE_SURROUND) && (sampleState->surroundEffectIndex != 0xFF)) {
        defaultPanVolume = gDefaultPanVolume[sampleState->surroundEffectIndex];
        targetVolLeft *= defaultPanVolume;
        targetVolRight *= defaultPanVolume;
    }

    if (targetVolLeft != curVolLeft) {
        rampLeft = (targetVolLeft - curVolLeft) / (numSamplesPerUpdate >> 3);
    } else {
        rampLeft = 0;
    }

    if (targetVolRight != curVolRight) {
        rampRight = (targetVolRight - curVolRight) / (numSamplesPerUpdate >> 3);
    } else {
        rampRight = 0;
    }

    curReverbVolAndFlags = synthState->curReverbVol;
    curReverbVol = curReverbVolAndFlags & 0x7F;

    if (curReverbVolAndFlags != targetReverbVol) {
        rampReverb = (((targetReverbVol & 0x7F) - curReverbVol) << 9) / (numSamplesPerUpdate >> 3);
        synthState->curReverbVol = targetReverbVol;
    } else {
        rampReverb = 0;
    }

    synthState->curVolLeft = curVolLeft + (rampLeft * (numSamplesPerUpdate >> 3));
    synthState->curVolRight = curVolRight + (rampRight * (numSamplesPerUpdate >> 3));

    if (sampleState->bitField1.useHaasEffect) {
        AudioSynth_ClearBuffer(cmd++, DMEM_HAAS_TEMP, DMEM_1CH_SIZE);
        AudioSynth_EnvSetup1(cmd++, curReverbVol * 2, rampReverb, rampLeft, rampRight);
        AudioSynth_EnvSetup2(cmd++, curVolLeft, curVolRight);

        switch (haasEffectDelaySide) {
            case HAAS_EFFECT_DELAY_LEFT:
                // Store the left dry channel in a temp space to be delayed to produce the haas effect
                dmemDests = sEnvMixerLeftHaasDmemDests;
                break;

            case HAAS_EFFECT_DELAY_RIGHT:
                // Store the right dry channel in a temp space to be delayed to produce the haas effect
                dmemDests = sEnvMixerRightHaasDmemDests;
                break;

            default: // HAAS_EFFECT_DELAY_NONE
                dmemDests = sEnvMixerDefaultDmemDests;
                break;
        }
    } else {
        aEnvSetup1(cmd++, curReverbVol * 2, rampReverb, rampLeft, rampRight);
        aEnvSetup2(cmd++, curVolLeft, curVolRight);
        dmemDests = sEnvMixerDefaultDmemDests;
    }

    aEnvMixer(cmd++, dmemSrc, numSamplesPerUpdate, (curReverbVolAndFlags & 0x80) >> 7,
              sampleState->bitField0.strongReverbRight, sampleState->bitField0.strongReverbLeft,
              sampleState->bitField0.strongRight, sampleState->bitField0.strongLeft, dmemDests, sEnvMixerOp);

    return cmd;
}

Acmd* AudioSynth_LoadWaveSamples(Acmd* cmd, NoteSampleState* sampleState, NoteSynthesisState* synthState,
                                 s32 numSamplesToLoad) {
    s32 numSamplesAvailable;
    s32 harmonicIndexCurAndPrev = sampleState->harmonicIndexCurAndPrev;
    s32 samplePosInt = synthState->samplePosInt;
    s32 numDuplicates;

    if (sampleState->bitField1.bookOffset != 0) {
        // Move the noise wave (that reads compiled assembly as samples) from ram to dmem
        AudioSynth_LoadBuffer(cmd++, DMEM_UNCOMPRESSED_NOTE, ALIGN16(numSamplesToLoad * SAMPLE_SIZE), gWaveSamples[8]);
        // Offset the address for the samples read by gWaveSamples[8] to the next set of samples
        gWaveSamples[8] += numSamplesToLoad * SAMPLE_SIZE;

        return cmd;
    } else {
        // Move the synthetic wave from ram to dmem
        aLoadBuffer(cmd++, sampleState->waveSampleAddr, DMEM_UNCOMPRESSED_NOTE, WAVE_SAMPLE_COUNT * SAMPLE_SIZE);

        // If the harmonic changes, map the offset in the wave from one harmonic to another for continuity
        if (harmonicIndexCurAndPrev != 0) {
            samplePosInt = (samplePosInt * sNumSamplesPerWavePeriod[harmonicIndexCurAndPrev >> 2]) /
                           sNumSamplesPerWavePeriod[harmonicIndexCurAndPrev & 3];
        }

        // Offset in the WAVE_SAMPLE_COUNT samples of gWaveSamples to start processing the wave for continuity
        samplePosInt = (u32)samplePosInt % WAVE_SAMPLE_COUNT;
        // Number of samples in the initial WAVE_SAMPLE_COUNT samples available to be used to process
        numSamplesAvailable = WAVE_SAMPLE_COUNT - samplePosInt;

        // Require duplicates if there are more samples to load than available
        if (numSamplesToLoad > numSamplesAvailable) {
            // Duplicate (copy) the WAVE_SAMPLE_COUNT samples as many times as needed to reach numSamplesToLoad.
            // (numSamplesToLoad - numSamplesAvailable) is the number of samples missing.
            // Divide by WAVE_SAMPLE_COUNT, rounding up, to get the amount of duplicates
            numDuplicates = ((numSamplesToLoad - numSamplesAvailable + WAVE_SAMPLE_COUNT - 1) / WAVE_SAMPLE_COUNT);
            if (numDuplicates != 0) {
                aDuplicate(cmd++, numDuplicates, DMEM_UNCOMPRESSED_NOTE,
                           DMEM_UNCOMPRESSED_NOTE + (WAVE_SAMPLE_COUNT * SAMPLE_SIZE));
            }
        }
        synthState->samplePosInt = samplePosInt;
    }

    return cmd;
}

/**
 * The Haas Effect gives directionality to sound by applying a small (< 35ms) delay to either the left or right channel.
 * The delay is small enough that the sound is still perceived as one sound, but the channel that is not delayed will
 * reach our ear first and give a sense of directionality. The sound is directed towards the opposite side of the delay.
 */
Acmd* AudioSynth_ApplyHaasEffect(Acmd* cmd, NoteSampleState* sampleState, NoteSynthesisState* synthState, s32 size,
                                 s32 flags, s32 haasEffectDelaySide) {
    u16 dmemDest;
    u16 pitch;
    u8 prevHaasEffectDelaySize;
    u8 haasEffectDelaySize;

    switch (haasEffectDelaySide) {
        case HAAS_EFFECT_DELAY_LEFT:
            // Delay the sample on the left channel
            // This allows the right channel to be heard first
            dmemDest = DMEM_LEFT_CH;
            haasEffectDelaySize = sampleState->haasEffectLeftDelaySize;
            prevHaasEffectDelaySize = synthState->prevHaasEffectLeftDelaySize;
            synthState->prevHaasEffectRightDelaySize = 0;
            synthState->prevHaasEffectLeftDelaySize = haasEffectDelaySize;
            break;

        case HAAS_EFFECT_DELAY_RIGHT:
            // Delay the sample on the right channel
            // This allows the left channel to be heard first
            dmemDest = DMEM_RIGHT_CH;
            haasEffectDelaySize = sampleState->haasEffectRightDelaySize;
            prevHaasEffectDelaySize = synthState->prevHaasEffectRightDelaySize;
            synthState->prevHaasEffectRightDelaySize = haasEffectDelaySize;
            synthState->prevHaasEffectLeftDelaySize = 0;
            break;

        default: // HAAS_EFFECT_DELAY_NONE
            return cmd;
    }

    if (flags != A_INIT) {
        // Slightly adjust the sample rate in order to fit a change in sample delay
        if (haasEffectDelaySize != prevHaasEffectDelaySize) {
            pitch = (((size << 0xF) / 2) - 1) / ((size + haasEffectDelaySize - prevHaasEffectDelaySize - 2) / 2);
            aSetBuffer(cmd++, 0, DMEM_HAAS_TEMP, DMEM_TEMP, size + haasEffectDelaySize - prevHaasEffectDelaySize);
            aResampleZoh(cmd++, pitch, 0);
        } else {
            aDMEMMove(cmd++, DMEM_HAAS_TEMP, DMEM_TEMP, size);
        }

        if (prevHaasEffectDelaySize != 0) {
            aLoadBuffer(cmd++, synthState->synthesisBuffers->haasEffectDelayState, DMEM_HAAS_TEMP,
                        ALIGN16(prevHaasEffectDelaySize));
            aDMEMMove(cmd++, DMEM_TEMP, DMEM_HAAS_TEMP + prevHaasEffectDelaySize,
                      size + haasEffectDelaySize - prevHaasEffectDelaySize);
        } else {
            aDMEMMove(cmd++, DMEM_TEMP, DMEM_HAAS_TEMP, size + haasEffectDelaySize);
        }
    } else {
        // Just apply a delay directly
        aDMEMMove(cmd++, DMEM_HAAS_TEMP, DMEM_TEMP, size);
        if (haasEffectDelaySize) { // != 0
            aClearBuffer(cmd++, DMEM_HAAS_TEMP, haasEffectDelaySize);
        }
        aDMEMMove(cmd++, DMEM_TEMP, DMEM_HAAS_TEMP + haasEffectDelaySize, size);
    }

    if (haasEffectDelaySize) { // != 0
        // Save excessive samples for next iteration
        aSaveBuffer(cmd++, DMEM_HAAS_TEMP + size, synthState->synthesisBuffers->haasEffectDelayState,
                    ALIGN16(haasEffectDelaySize));
    }

    aAddMixer(cmd++, ALIGN64(size), DMEM_HAAS_TEMP, dmemDest);
    // aAddMixer(cmd++, ALIGN64(size), DMEM_HAAS_TEMP, dmemDest, 0x7FFF);

    return cmd;
}
