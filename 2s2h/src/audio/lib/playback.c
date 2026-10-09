#include "global.h"
#include "audio/effects.h"
#include "BenPort.h"
#include <libultraship/bridge/consolevariablebridge.h>

void AudioPlayback_NoteSetResamplingRate(NoteSampleState* sampleState, f32 resamplingRateInput);
void AudioPlayback_AudioListPushFront(AudioListItem* list, AudioListItem* item);
void AudioPlayback_NoteInitForLayer(Note* note, SequenceLayer* layer);

SoundFont* ResourceMgr_LoadAudioSoundFontByName(const char* path);

extern char** gFontMap;

void AudioPlayback_InitSampleState(Note* note, NoteSampleState* sampleState, NoteSubAttributes* subAttrs) {
    f32 volLeft;
    f32 volRight;
    s32 halfPanIndex;
    u64 pad;
    u8 strongLeft;
    u8 strongRight;
    f32 velocity;
    u8 pan;
    u8 targetReverbVol;
    StereoData stereoData;
    s32 stereoHeadsetEffects = note->playbackState.stereoHeadsetEffects;

    velocity = subAttrs->velocity;
    pan = subAttrs->pan;
    targetReverbVol = subAttrs->targetReverbVol;
    stereoData = subAttrs->stereoData;

    sampleState->bitField0 = note->sampleState.bitField0;
    sampleState->bitField1 = note->sampleState.bitField1;
    sampleState->waveSampleAddr = note->sampleState.waveSampleAddr;
    sampleState->harmonicIndexCurAndPrev = note->sampleState.harmonicIndexCurAndPrev;

    AudioPlayback_NoteSetResamplingRate(sampleState, subAttrs->frequency);

    pan &= 0x7F;

    sampleState->bitField0.strongRight = false;
    sampleState->bitField0.strongLeft = false;
    sampleState->bitField0.strongReverbRight = stereoData.strongReverbRight;
    sampleState->bitField0.strongReverbLeft = stereoData.strongReverbLeft;
    if (stereoHeadsetEffects && (gAudioCtx.soundMode == SOUNDMODE_HEADSET)) {
        halfPanIndex = pan >> 1;
        if (halfPanIndex > 0x3F) {
            halfPanIndex = 0x3F;
        }

        sampleState->haasEffectRightDelaySize = gHaasEffectDelaySize[halfPanIndex];
        sampleState->haasEffectLeftDelaySize = gHaasEffectDelaySize[0x3F - halfPanIndex];
        sampleState->bitField1.useHaasEffect = true;

        volLeft = gHeadsetPanVolume[pan];
        volRight = gHeadsetPanVolume[0x7F - pan];
    } else if (stereoHeadsetEffects && (gAudioCtx.soundMode == SOUNDMODE_STEREO)) {
        strongLeft = strongRight = false;
        sampleState->haasEffectLeftDelaySize = 0;
        sampleState->haasEffectRightDelaySize = 0;
        sampleState->bitField1.useHaasEffect = false;

        volLeft = gStereoPanVolume[pan];
        volRight = gStereoPanVolume[0x7F - pan];
        if (pan < 0x20) {
            strongLeft = true;
        } else if (pan > 0x60) {
            strongRight = true;
        }

        // case 0:
        sampleState->bitField0.strongRight = strongRight;
        sampleState->bitField0.strongLeft = strongLeft;

        switch (stereoData.type) {
            case 0:
                break;

            case 1:
                sampleState->bitField0.strongRight = stereoData.strongRight;
                sampleState->bitField0.strongLeft = stereoData.strongLeft;
                break;

            case 2:
                sampleState->bitField0.strongRight = stereoData.strongRight | strongRight;
                sampleState->bitField0.strongLeft = stereoData.strongLeft | strongLeft;
                break;

            case 3:
                sampleState->bitField0.strongRight = stereoData.strongRight ^ strongRight;
                sampleState->bitField0.strongLeft = stereoData.strongLeft ^ strongLeft;
                break;

            default:
                break;
        }

    } else if (gAudioCtx.soundMode == SOUNDMODE_MONO) {
        sampleState->bitField0.strongReverbRight = false;
        sampleState->bitField0.strongReverbLeft = false;
        volLeft = 0.707f; // approx 1/sqrt(2)
        volRight = 0.707f;
    } else {
        sampleState->bitField0.strongRight = stereoData.strongRight;
        sampleState->bitField0.strongLeft = stereoData.strongLeft;
        volLeft = gDefaultPanVolume[pan];
        volRight = gDefaultPanVolume[0x7F - pan];
    }

    velocity = 0.0f > velocity ? 0.0f : velocity;
    velocity = 1.0f < velocity ? 1.0f : velocity;

    float master_vol = CVarGetFloat("gSettings.Audio.MasterVolume", 1.0f);
    sampleState->targetVolLeft = (s32)((velocity * volLeft) * (0x1000 - 0.001f)) * master_vol;
    sampleState->targetVolRight = (s32)((velocity * volRight) * (0x1000 - 0.001f)) * master_vol;

    sampleState->gain = subAttrs->gain;
    sampleState->filter = subAttrs->filter;
    sampleState->combFilterSize = subAttrs->combFilterSize;
    sampleState->combFilterGain = subAttrs->combFilterGain;
    sampleState->targetReverbVol = targetReverbVol;
    sampleState->surroundEffectIndex = subAttrs->surroundEffectIndex;
}

void AudioPlayback_NoteSetResamplingRate(NoteSampleState* sampleState, f32 resamplingRateInput) {
    f32 resamplingRate = 0.0f;

    if (resamplingRateInput < 2.0f) {
        sampleState->bitField1.hasTwoParts = false;
        resamplingRate = CLAMP_MAX(resamplingRateInput, 1.99998f);

    } else {
        sampleState->bitField1.hasTwoParts = true;
        if (resamplingRateInput > 3.99996f) {
            if (sampleState->bitField1.isSyntheticWave) {
                resamplingRate = resamplingRateInput * 0.25;
            } else {
                resamplingRate = 1.99998f;
            }
        } else {
            resamplingRate = resamplingRateInput * 0.5f;
        }
    }
    sampleState->frequencyFixedPoint = (s32)(resamplingRate * 32768.0f);
}

void AudioPlayback_NoteInit(Note* note) {
    if (note->playbackState.parentLayer->adsr.decayIndex == 0) {
        AudioEffects_InitAdsr(&note->playbackState.adsr, note->playbackState.parentLayer->channel->adsr.envelope,
                              &note->playbackState.adsrVolScaleUnused);
    } else {
        AudioEffects_InitAdsr(&note->playbackState.adsr, note->playbackState.parentLayer->adsr.envelope,
                              &note->playbackState.adsrVolScaleUnused);
    }

    note->playbackState.status = PLAYBACK_STATUS_0;
    note->playbackState.adsr.action.s.status = ADSR_STATUS_INITIAL;
    note->sampleState = gDefaultSampleState;
}

extern void aOPUSFree(struct OggOpusFile* opusFile);
void AudioPlayback_NoteDisable(Note* note) {
    if (note->sampleState.bitField0.needsInit == true) {
        note->sampleState.bitField0.needsInit = false;
    }
    note->playbackState.priority = 0;
    note->sampleState.bitField0.enabled = false;
    note->playbackState.status = PLAYBACK_STATUS_0;
    note->sampleState.bitField0.finished = false;
    note->playbackState.parentLayer = NO_LAYER;
    note->playbackState.prevParentLayer = NO_LAYER;
    note->playbackState.adsr.action.s.status = ADSR_STATUS_DISABLED;
    note->playbackState.adsr.current = 0;
    // 2S2H [Custom Audio] Free the decoded opus data if needed.
    if (note->synthesisState.opusFile != NULL) {
        aOPUSFree(note->synthesisState.opusFile);
        note->synthesisState.opusFile = NULL;
    }
}

void AudioPlayback_ProcessNotes(void) {
    s32 pad;
    s32 playbackStatus;
    NoteAttributes* attrs;
    NoteSampleState* sampleState;
    NoteSampleState* noteSampleState;
    Note* note;
    NotePlaybackState* playbackState;
    NoteSubAttributes subAttrs;
    u8 bookOffset;
    f32 adsrVolumeScale;
    s32 i;

    for (i = 0; i < gAudioCtx.numNotes; i++) {
        note = &gAudioCtx.notes[i];
        sampleState = &gAudioCtx.sampleStateList[gAudioCtx.sampleStateOffset + i];
        playbackState = &note->playbackState;
        if (playbackState->parentLayer != NO_LAYER) {
#ifndef __WIIU__
            // OTRTODO: This skips playback if the pointer is below where memory on the N64 normally would be.
            // This does not translate well to modern platforms and how they map memory.
            // Considering that this check is not present in OoT/SoH, we may be able to remove this altogether.
            if ((uintptr_t)playbackState->parentLayer < 0x7FFFFFFF) {
                continue;
            }
#endif

            if ((note != playbackState->parentLayer->note) && (playbackState->status == PLAYBACK_STATUS_0)) {
                playbackState->adsr.action.s.release = true;
                playbackState->adsr.fadeOutVel = gAudioCtx.audioBufferParameters.updatesPerFrameInv;
                playbackState->priority = 1;
                playbackState->status = PLAYBACK_STATUS_2;
                goto out;
            } else if (!playbackState->parentLayer->enabled && (playbackState->status == PLAYBACK_STATUS_0) &&
                       (playbackState->priority >= 1)) {
                // do nothing
            } else if (playbackState->parentLayer->channel->seqPlayer == NULL) {
                AudioScript_SequenceChannelDisable(playbackState->parentLayer->channel);
                playbackState->priority = 1;
                playbackState->status = PLAYBACK_STATUS_1;
                continue;
            } else if (playbackState->parentLayer->channel->seqPlayer->muted &&
                       (playbackState->parentLayer->channel->muteFlags & MUTE_FLAGS_STOP_NOTES)) {
                // do nothing
            } else {
                goto out;
            }

            AudioPlayback_SeqLayerNoteRelease(playbackState->parentLayer);
            AudioPlayback_AudioListRemove(&note->listItem);
            AudioPlayback_AudioListPushFront(&note->listItem.pool->decaying, &note->listItem);
            playbackState->priority = 1;
            playbackState->status = PLAYBACK_STATUS_2;
        } else if ((playbackState->status == PLAYBACK_STATUS_0) && (playbackState->priority >= 1)) {
            continue;
        }

    out:
        if (playbackState->priority != 0) {
            //! FAKE:
            if (1) {}
            noteSampleState = &note->sampleState;
            if ((playbackState->status >= 1) || noteSampleState->bitField0.finished) {
                if ((playbackState->adsr.action.s.status == ADSR_STATUS_DISABLED) ||
                    noteSampleState->bitField0.finished) {
                    if (playbackState->wantedParentLayer != NO_LAYER) {
                        AudioPlayback_NoteDisable(note);
                        if (playbackState->wantedParentLayer->channel != NULL) {
                            AudioPlayback_NoteInitForLayer(note, playbackState->wantedParentLayer);
                            AudioEffects_InitVibrato(note);
                            AudioEffects_InitPortamento(note);
                            AudioPlayback_AudioListRemove(&note->listItem);
                            AudioScript_AudioListPushBack(&note->listItem.pool->active, &note->listItem);
                            playbackState->wantedParentLayer = NO_LAYER;
                            // don't skip
                        } else {
                            AudioPlayback_NoteDisable(note);
                            AudioPlayback_AudioListRemove(&note->listItem);
                            AudioScript_AudioListPushBack(&note->listItem.pool->disabled, &note->listItem);
                            playbackState->wantedParentLayer = NO_LAYER;
                            goto skip;
                        }
                    } else {
                        if (playbackState->parentLayer != NO_LAYER) {
                            playbackState->parentLayer->bit1 = true;
                        }
                        AudioPlayback_NoteDisable(note);
                        AudioPlayback_AudioListRemove(&note->listItem);
                        AudioScript_AudioListPushBack(&note->listItem.pool->disabled, &note->listItem);
                        continue;
                    }
                }
            } else if (playbackState->adsr.action.s.status == ADSR_STATUS_DISABLED) {
                if (playbackState->parentLayer != NO_LAYER) {
                    playbackState->parentLayer->bit1 = true;
                }
                AudioPlayback_NoteDisable(note);
                AudioPlayback_AudioListRemove(&note->listItem);
                AudioScript_AudioListPushBack(&note->listItem.pool->disabled, &note->listItem);
                continue;
            }

            adsrVolumeScale = AudioEffects_UpdateAdsr(&playbackState->adsr);
            AudioEffects_UpdatePortamentoAndVibrato(note);
            playbackStatus = playbackState->status;
            attrs = &playbackState->attributes;
            if ((playbackStatus == PLAYBACK_STATUS_1) || (playbackStatus == PLAYBACK_STATUS_2)) {
                subAttrs.frequency = attrs->freqScale;
                subAttrs.velocity = attrs->velocity;
                subAttrs.pan = attrs->pan;
                subAttrs.targetReverbVol = attrs->targetReverbVol;
                subAttrs.stereoData = attrs->stereoData;
                subAttrs.gain = attrs->gain;
                subAttrs.filter = attrs->filter;
                subAttrs.combFilterSize = attrs->combFilterSize;
                subAttrs.combFilterGain = attrs->combFilterGain;
                subAttrs.surroundEffectIndex = attrs->surroundEffectIndex;
                bookOffset = noteSampleState->bitField1.bookOffset;
            } else {
                SequenceLayer* layer = playbackState->parentLayer;
                SequenceChannel* channel = playbackState->parentLayer->channel;

                subAttrs.frequency = layer->noteFreqScale;
                subAttrs.velocity = layer->noteVelocity;
                subAttrs.pan = layer->notePan;

                if (layer->surroundEffectIndex == 0x80) {
                    subAttrs.surroundEffectIndex = channel->surroundEffectIndex;
                } else {
                    subAttrs.surroundEffectIndex = layer->surroundEffectIndex;
                }

                if (layer->stereoData.type == 0) {
                    subAttrs.stereoData = channel->stereoData;
                } else {
                    subAttrs.stereoData = layer->stereoData;
                }

                if (layer->unk_0A.s.bit_2 == 1) {
                    subAttrs.targetReverbVol = channel->targetReverbVol;
                } else {
                    subAttrs.targetReverbVol = layer->targetReverbVol;
                }

                if (layer->unk_0A.s.bit_9 == 1) {
                    subAttrs.gain = channel->gain;
                } else {
                    subAttrs.gain = 0;
                    //! FAKE:
                    if (1) {}
                }

                subAttrs.filter = channel->filter;
                subAttrs.combFilterSize = channel->combFilterSize;
                subAttrs.combFilterGain = channel->combFilterGain;
                bookOffset = channel->bookOffset & 0x7;

                if (channel->seqPlayer->muted && (channel->muteFlags & MUTE_FLAGS_STOP_SAMPLES)) {
                    subAttrs.frequency = 0.0f;
                    subAttrs.velocity = 0.0f;
                }
            }

            subAttrs.frequency *= playbackState->vibratoFreqScale * playbackState->portamentoFreqScale;
            subAttrs.frequency *= gAudioCtx.audioBufferParameters.resampleRate;
            subAttrs.velocity *= adsrVolumeScale;
            AudioPlayback_InitSampleState(note, sampleState, &subAttrs);
            noteSampleState->bitField1.bookOffset = bookOffset;
        skip:;
        }
    }
}

TunedSample* AudioPlayback_GetInstrumentTunedSample(Instrument* instrument, s32 semitone) {
    TunedSample* tunedSample;

    if (semitone < instrument->normalRangeLo) {
        tunedSample = &instrument->lowPitchTunedSample;
    } else if (semitone <= instrument->normalRangeHi) {
        tunedSample = &instrument->normalPitchTunedSample;
    } else {
        tunedSample = &instrument->highPitchTunedSample;
    }

    return tunedSample;
}

Instrument* AudioPlayback_GetInstrumentInner(s32 fontId, s32 instId) {
    Instrument* inst;

    if (fontId == 0xFFFF) {
        return NULL;
    }

    if (!AudioLoad_IsFontLoadComplete(fontId)) {
        gAudioCtx.audioErrorFlags = AUDIO_ERROR(0, fontId, AUDIO_ERROR_FONT_NOT_LOADED);
        return NULL;
    }

    int instCnt = 0;
    // 2S2H [Port] Audio assets in the archive
    SoundFont* sf = ResourceMgr_LoadAudioSoundFontByName(gFontMap[fontId]);

    if (instId >= sf->numInstruments)
        return NULL;

    inst = sf->instruments[instId];

    if (inst == NULL) {
        gAudioCtx.audioErrorFlags = AUDIO_ERROR(fontId, instId, AUDIO_ERROR_NO_INST);
        return inst;
    }

    return inst;
}

Drum* AudioPlayback_GetDrum(s32 fontId, s32 drumId) {
    Drum* drum = NULL;

    if (fontId == 0xFFFF) {
        return NULL;
    }

    if (!AudioLoad_IsFontLoadComplete(fontId)) {
        gAudioCtx.audioErrorFlags = AUDIO_ERROR(0, fontId, AUDIO_ERROR_FONT_NOT_LOADED);
        return NULL;
    }
    // 2S2H [Port] Audio assets in the archive
    SoundFont* sf = ResourceMgr_LoadAudioSoundFontByName(gFontMap[fontId]);
    if (drumId < sf->numDrums) {
        drum = sf->drums[drumId];
    }

    if (drum == NULL) {
        gAudioCtx.audioErrorFlags = AUDIO_ERROR(fontId, drumId, AUDIO_ERROR_NO_DRUM_SFX);
    }

    return drum;
}

SoundEffect* AudioPlayback_GetSoundEffect(s32 fontId, s32 sfxId) {
    SoundEffect* soundEffect;

    if (fontId == 0xFFFF) {
        return NULL;
    }

    if (!AudioLoad_IsFontLoadComplete(fontId)) {
        gAudioCtx.audioErrorFlags = AUDIO_ERROR(0, fontId, AUDIO_ERROR_FONT_NOT_LOADED);
        return NULL;
    }
    // 2S2H [Port] Audio assets in the archive
    SoundFont* sf = ResourceMgr_LoadAudioSoundFontByName(gFontMap[fontId]);
    if (sfxId < sf->numSfx) {
        soundEffect = &sf->soundEffects[sfxId];
    }

    if (soundEffect == NULL) {
        gAudioCtx.audioErrorFlags = AUDIO_ERROR(fontId, sfxId, AUDIO_ERROR_NO_DRUM_SFX);
    }

    if (soundEffect != NULL && soundEffect->tunedSample.sample == NULL) {
        return NULL;
    }

    return soundEffect;
}

s32 AudioPlayback_SetFontInstrument(s32 instrumentType, s32 fontId, s32 index, void* value) {
    if (fontId == 0xFFFF) {
        return -1;
    }

    if (!AudioLoad_IsFontLoadComplete(fontId)) {
        return -2;
    }

    switch (instrumentType) {
        case 0:
            if (index >= gAudioCtx.soundFontList[fontId].numDrums) {
                return -3;
            }
            gAudioCtx.soundFontList[fontId].drums[index] = value;
            break;

        case 1:
            if (index >= gAudioCtx.soundFontList[fontId].numSfx) {
                return -3;
            }
            gAudioCtx.soundFontList[fontId].soundEffects[index] = *(SoundEffect*)value;
            break;

        default:
            if (index >= gAudioCtx.soundFontList[fontId].numInstruments) {
                return -3;
            }
            gAudioCtx.soundFontList[fontId].instruments[index] = value;
            break;
    }

    return 0;
}

void AudioPlayback_SeqLayerDecayRelease(SequenceLayer* layer, s32 target) {
    Note* note;
    NoteAttributes* attrs;
    SequenceChannel* channel;
    s32 i;

    if (layer == NO_LAYER) {
        return;
    }

    layer->bit3 = false;

    if (layer->note == NULL) {
        return;
    }

    note = layer->note;
    attrs = &note->playbackState.attributes;

    if (note->playbackState.wantedParentLayer == layer) {
        note->playbackState.wantedParentLayer = NO_LAYER;
    }

    if (note->playbackState.parentLayer != layer) {
        if (note->playbackState.parentLayer == NO_LAYER && note->playbackState.wantedParentLayer == NO_LAYER &&
            note->playbackState.prevParentLayer == layer && target != ADSR_STATUS_DECAY) {
            note->playbackState.adsr.fadeOutVel = gAudioCtx.audioBufferParameters.updatesPerFrameInv;
            note->playbackState.adsr.action.s.release = true;
        }
        return;
    }

    if (note->playbackState.adsr.action.s.status != ADSR_STATUS_DECAY) {
        attrs->freqScale = layer->noteFreqScale;
        attrs->velocity = layer->noteVelocity;
        attrs->pan = layer->notePan;

        if (layer->channel != NULL) {
            channel = layer->channel;

            if (layer->unk_0A.s.bit_2 == 1) {
                attrs->targetReverbVol = channel->targetReverbVol;
            } else {
                attrs->targetReverbVol = layer->targetReverbVol;
            }

            if (layer->surroundEffectIndex == 0x80) {
                attrs->surroundEffectIndex = channel->surroundEffectIndex;
            } else {
                attrs->surroundEffectIndex = layer->surroundEffectIndex;
            }

            if (layer->unk_0A.s.bit_9 == 1) {
                attrs->gain = channel->gain;
            } else {
                attrs->gain = 0;
            }

            attrs->filter = channel->filter;

            if (attrs->filter != NULL) {
                for (i = 0; i < 8; i++) {
                    attrs->filterBuf[i] = attrs->filter[i];
                }
                attrs->filter = attrs->filterBuf;
            }

            attrs->combFilterGain = channel->combFilterGain;
            attrs->combFilterSize = channel->combFilterSize;
            if (channel->seqPlayer->muted && (channel->muteFlags & MUTE_FLAGS_STOP_SAMPLES)) {
                note->sampleState.bitField0.finished = true;
            }

            if (layer->stereoData.asByte == 0) {
                attrs->stereoData = channel->stereoData;
            } else {
                attrs->stereoData = layer->stereoData;
            }
            note->playbackState.priority = channel->someOtherPriority;
        } else {
            attrs->stereoData = layer->stereoData;
            note->playbackState.priority = 1;
        }

        note->playbackState.prevParentLayer = note->playbackState.parentLayer;
        note->playbackState.parentLayer = NO_LAYER;
        if (target == ADSR_STATUS_RELEASE) {
            note->playbackState.adsr.fadeOutVel = gAudioCtx.audioBufferParameters.updatesPerFrameInv;
            note->playbackState.adsr.action.s.release = true;
            note->playbackState.status = PLAYBACK_STATUS_2;
        } else {
            note->playbackState.status = PLAYBACK_STATUS_1;
            note->playbackState.adsr.action.s.decay = true;
            if (layer->adsr.decayIndex == 0) {
                note->playbackState.adsr.fadeOutVel = gAudioCtx.adsrDecayTable[layer->channel->adsr.decayIndex];
            } else {
                note->playbackState.adsr.fadeOutVel = gAudioCtx.adsrDecayTable[layer->adsr.decayIndex];
            }
            note->playbackState.adsr.sustain =
                ((f32)(s32)(layer->channel->adsr.sustain) * note->playbackState.adsr.current) / 256.0f;
        }
    }

    if (target == ADSR_STATUS_DECAY) {
        AudioPlayback_AudioListRemove(&note->listItem);
        AudioPlayback_AudioListPushFront(&note->listItem.pool->decaying, &note->listItem);
    }
}

void AudioPlayback_SeqLayerNoteDecay(SequenceLayer* layer) {
    AudioPlayback_SeqLayerDecayRelease(layer, ADSR_STATUS_DECAY);
}

void AudioPlayback_SeqLayerNoteRelease(SequenceLayer* layer) {
    AudioPlayback_SeqLayerDecayRelease(layer, ADSR_STATUS_RELEASE);
}

/**
 * Extract the synthetic wave to use from gWaveSamples and update corresponding frequencies
 *
 * @param note
 * @param layer
 * @param waveId the index of the type of synthetic wave to use, offset by 128
 * @return harmonicIndex, the index of the harmonic for the synthetic wave contained in gWaveSamples
 */
s32 AudioPlayback_BuildSyntheticWave(Note* note, SequenceLayer* layer, s32 waveId) {
    f32 freqScale;
    f32 freqRatio;
    u8 harmonicIndex;

    if (waveId < 128) {
        waveId = 128;
    }

    freqScale = layer->freqScale;
    if ((layer->portamento.mode != PORTAMENTO_MODE_OFF) && (layer->portamento.extent > 0.0f)) {
        freqScale *= (layer->portamento.extent + 1.0f);
    }

    // Map frequency to the harmonic to use from gWaveSamples
    if (freqScale < 0.99999f) {
        harmonicIndex = 0;
        freqRatio = 1.0465f;
    } else if (freqScale < 1.99999f) {
        harmonicIndex = 1;
        freqRatio = 1.0465f / 2;
    } else if (freqScale < 3.99999f) {
        harmonicIndex = 2;
        freqRatio = 1.0465f / 4 + 1.005E-3;
    } else {
        harmonicIndex = 3;
        freqRatio = 1.0465f / 8 - 2.5E-6;
    }

    // Update results
    layer->freqScale *= freqRatio;
    note->playbackState.waveId = waveId;
    note->playbackState.harmonicIndex = harmonicIndex;

    // Save the pointer to the synthethic wave
    // waveId index starts at 128, there are WAVE_SAMPLE_COUNT samples to read from
    note->sampleState.waveSampleAddr = &gWaveSamples[waveId - 128][harmonicIndex * WAVE_SAMPLE_COUNT];

    return harmonicIndex;
}

void AudioPlayback_InitSyntheticWave(Note* note, SequenceLayer* layer) {
    s32 prevHarmonicIndex;
    s32 curHarmonicIndex;
    s32 waveId = layer->instOrWave;

    if (waveId == 0xFF) {
        waveId = layer->channel->instOrWave;
    }

    prevHarmonicIndex = note->playbackState.harmonicIndex;
    curHarmonicIndex = AudioPlayback_BuildSyntheticWave(note, layer, waveId);

    if (curHarmonicIndex != prevHarmonicIndex) {
        note->sampleState.harmonicIndexCurAndPrev = (curHarmonicIndex << 2) + prevHarmonicIndex;
    }
}

void AudioPlayback_InitNoteList(AudioListItem* list) {
    list->prev = list;
    list->next = list;
    list->u.count = 0;
}

void AudioPlayback_InitNoteLists(NotePool* pool) {
    AudioPlayback_InitNoteList(&pool->disabled);
    AudioPlayback_InitNoteList(&pool->decaying);
    AudioPlayback_InitNoteList(&pool->releasing);
    AudioPlayback_InitNoteList(&pool->active);
    pool->disabled.pool = pool;
    pool->decaying.pool = pool;
    pool->releasing.pool = pool;
    pool->active.pool = pool;
}

void AudioPlayback_InitNoteFreeList(void) {
    s32 i;

    AudioPlayback_InitNoteLists(&gAudioCtx.noteFreeLists);
    for (i = 0; i < gAudioCtx.numNotes; i++) {
        gAudioCtx.notes[i].listItem.u.value = &gAudioCtx.notes[i];
        gAudioCtx.notes[i].listItem.prev = NULL;
        AudioScript_AudioListPushBack(&gAudioCtx.noteFreeLists.disabled, &gAudioCtx.notes[i].listItem);
    }
}

void AudioPlayback_NotePoolClear(NotePool* pool) {
    s32 i;
    AudioListItem* source;
    AudioListItem* cur;
    AudioListItem* dest;

    for (i = 0; i < 4; i++) {
        switch (i) {
            case 0:
                source = &pool->disabled;
                dest = &gAudioCtx.noteFreeLists.disabled;
                break;

            case 1:
                source = &pool->decaying;
                dest = &gAudioCtx.noteFreeLists.decaying;
                break;

            case 2:
                source = &pool->releasing;
                dest = &gAudioCtx.noteFreeLists.releasing;
                break;

            case 3:
                source = &pool->active;
                dest = &gAudioCtx.noteFreeLists.active;
                break;

            default:
                break;
        }

        while (true) {
            cur = source->next;
            if ((cur == source) || (cur == NULL)) {
                break;
            }
            AudioPlayback_AudioListRemove(cur);
            AudioScript_AudioListPushBack(dest, cur);
        }
    }
}

void AudioPlayback_NotePoolFill(NotePool* pool, s32 count) {
    s32 i;
    s32 j;
    Note* note;
    AudioListItem* source;
    AudioListItem* dest;

    AudioPlayback_NotePoolClear(pool);

    for (i = 0, j = 0; j < count; i++) {
        if (i == 4) {
            return;
        }

        switch (i) {
            case 0:
                source = &gAudioCtx.noteFreeLists.disabled;
                dest = &pool->disabled;
                break;

            case 1:
                source = &gAudioCtx.noteFreeLists.decaying;
                dest = &pool->decaying;
                break;

            case 2:
                source = &gAudioCtx.noteFreeLists.releasing;
                dest = &pool->releasing;
                break;

            case 3:
                source = &gAudioCtx.noteFreeLists.active;
                dest = &pool->active;
                break;
        }

        while (j < count) {
            note = AudioScript_AudioListPopBack(source);
            if (note == NULL) {
                break;
            }
            AudioScript_AudioListPushBack(dest, &note->listItem);
            j++;
        }
    }
}

void AudioPlayback_AudioListPushFront(AudioListItem* list, AudioListItem* item) {
    // add 'item' to the front of the list given by 'list', if it's not in any list
    if (item->prev == NULL) {
        item->prev = list;
        item->next = list->next;
        list->next->prev = item;
        list->next = item;
        list->u.count++;
        item->pool = list->pool;
    }
}

void AudioPlayback_AudioListRemove(AudioListItem* item) {
    // remove 'item' from the list it's in, if any
    if (item->prev != NULL) {
        item->prev->next = item->next;
        item->next->prev = item->prev;
        item->prev = NULL;
    }
}

Note* AudioPlayback_FindNodeWithPrioLessThan(AudioListItem* list, s32 limit) {
    AudioListItem* cur = list->next;
    AudioListItem* best;

    if (cur == list) {
        return NULL;
    }

    for (best = cur; cur != list; cur = cur->next) {
        if (((Note*)best->u.value)->playbackState.priority >= ((Note*)cur->u.value)->playbackState.priority) {
            best = cur;
        }
    }

    if (best == NULL) {
        return NULL;
    }

    if (limit <= ((Note*)best->u.value)->playbackState.priority) {
        return NULL;
    }

    return best->u.value;
}

void AudioPlayback_NoteInitForLayer(Note* note, SequenceLayer* layer) {
    s32 pad[3];
    s16 instId;
    SequenceChannel* channel = layer->channel;
    NotePlaybackState* playbackState = &note->playbackState;
    NoteSampleState* noteSampleState = &note->sampleState;

    playbackState->prevParentLayer = NO_LAYER;
    playbackState->parentLayer = layer;
    playbackState->priority = channel->notePriority;
    layer->notePropertiesNeedInit = true;
    layer->bit3 = true;
    layer->note = note;
    channel->noteUnused = note;
    channel->layerUnused = layer;
    layer->noteVelocity = 0.0f;
    AudioPlayback_NoteInit(note);
    instId = layer->instOrWave;

    if (instId == 0xFF) {
        instId = channel->instOrWave;
    }
    noteSampleState->tunedSample = layer->tunedSample;

    if (instId >= 0x80 && instId < 0xC0) {
        noteSampleState->bitField1.isSyntheticWave = true;
    } else {
        noteSampleState->bitField1.isSyntheticWave = false;
    }

    if (noteSampleState->bitField1.isSyntheticWave) {
        AudioPlayback_BuildSyntheticWave(note, layer, instId);
    } else if (channel->startSamplePos == 1) {
        playbackState->startSamplePos = noteSampleState->tunedSample->sample->loop->start;
    } else {
        playbackState->startSamplePos = channel->startSamplePos;
        if (playbackState->startSamplePos >= noteSampleState->tunedSample->sample->loop->loopEnd) {
            playbackState->startSamplePos = 0;
        }
    }

    playbackState->fontId = channel->fontId;
    playbackState->stereoHeadsetEffects = channel->stereoHeadsetEffects;
    noteSampleState->bitField1.reverbIndex = channel->reverbIndex & 3;
}

void func_801963E8(Note* note, SequenceLayer* layer) {
    // similar to Audio_NoteReleaseAndTakeOwnership, hard to say what the difference is
    AudioPlayback_SeqLayerNoteRelease(note->playbackState.parentLayer);
    note->playbackState.wantedParentLayer = layer;
}

void AudioPlayback_NoteReleaseAndTakeOwnership(Note* note, SequenceLayer* layer) {
    note->playbackState.wantedParentLayer = layer;
    note->playbackState.priority = layer->channel->notePriority;

    note->playbackState.adsr.fadeOutVel = gAudioCtx.audioBufferParameters.updatesPerFrameInv;
    note->playbackState.adsr.action.s.release = true;
}

Note* AudioPlayback_AllocNoteFromDisabled(NotePool* pool, SequenceLayer* layer) {
    Note* note = AudioScript_AudioListPopBack(&pool->disabled);

    if (note != NULL) {
        AudioPlayback_NoteInitForLayer(note, layer);
        AudioPlayback_AudioListPushFront(&pool->active, &note->listItem);
    }
    return note;
}

Note* AudioPlayback_AllocNoteFromDecaying(NotePool* pool, SequenceLayer* layer) {
    Note* note = AudioPlayback_FindNodeWithPrioLessThan(&pool->decaying, layer->channel->notePriority);

    if (note != NULL) {
        AudioPlayback_NoteReleaseAndTakeOwnership(note, layer);
        AudioPlayback_AudioListRemove(&note->listItem);
        AudioScript_AudioListPushBack(&pool->releasing, &note->listItem);
    }
    return note;
}

Note* AudioPlayback_AllocNoteFromActive(NotePool* pool, SequenceLayer* layer) {
    Note* rNote;
    Note* aNote;
    s32 rPriority;
    s32 aPriority;

    rPriority = aPriority = 0x10;
    rNote = AudioPlayback_FindNodeWithPrioLessThan(&pool->releasing, layer->channel->notePriority);

    if (rNote != NULL) {
        rPriority = rNote->playbackState.priority;
    }

    aNote = AudioPlayback_FindNodeWithPrioLessThan(&pool->active, layer->channel->notePriority);

    if (aNote != NULL) {
        aPriority = aNote->playbackState.priority;
    }

    if ((rNote == NULL) && (aNote == NULL)) {
        return NULL;
    }

    if (aPriority < rPriority) {
        AudioPlayback_AudioListRemove(&aNote->listItem);
        func_801963E8(aNote, layer);
        AudioScript_AudioListPushBack(&pool->releasing, &aNote->listItem);
        aNote->playbackState.priority = layer->channel->notePriority;
        return aNote;
    }
    rNote->playbackState.wantedParentLayer = layer;
    rNote->playbackState.priority = layer->channel->notePriority;
    return rNote;
}

Note* AudioPlayback_AllocNote(SequenceLayer* layer) {
    Note* note;
    u32 policy = layer->channel->noteAllocPolicy;

    if (policy & 1) {
        note = layer->note;
        if ((note != NULL) && (note->playbackState.prevParentLayer == layer) &&
            (note->playbackState.wantedParentLayer == NO_LAYER)) {
            AudioPlayback_NoteReleaseAndTakeOwnership(note, layer);
            AudioPlayback_AudioListRemove(&note->listItem);
            AudioScript_AudioListPushBack(&note->listItem.pool->releasing, &note->listItem);
            return note;
        }
    }

    if (policy & 2) {
        if (!(note = AudioPlayback_AllocNoteFromDisabled(&layer->channel->notePool, layer)) &&
            !(note = AudioPlayback_AllocNoteFromDecaying(&layer->channel->notePool, layer)) &&
            !(note = AudioPlayback_AllocNoteFromActive(&layer->channel->notePool, layer))) {
            goto null_return;
        }
        return note;
    }

    if (policy & 4) {
        if (!(note = AudioPlayback_AllocNoteFromDisabled(&layer->channel->notePool, layer)) &&
            !(note = AudioPlayback_AllocNoteFromDisabled(&layer->channel->seqPlayer->notePool, layer)) &&
            !(note = AudioPlayback_AllocNoteFromDecaying(&layer->channel->notePool, layer)) &&
            !(note = AudioPlayback_AllocNoteFromDecaying(&layer->channel->seqPlayer->notePool, layer)) &&
            !(note = AudioPlayback_AllocNoteFromActive(&layer->channel->notePool, layer)) &&
            !(note = AudioPlayback_AllocNoteFromActive(&layer->channel->seqPlayer->notePool, layer))) {
            goto null_return;
        }
        return note;
    }

    if (policy & 8) {
        if (!(note = AudioPlayback_AllocNoteFromDisabled(&gAudioCtx.noteFreeLists, layer)) &&
            !(note = AudioPlayback_AllocNoteFromDecaying(&gAudioCtx.noteFreeLists, layer)) &&
            !(note = AudioPlayback_AllocNoteFromActive(&gAudioCtx.noteFreeLists, layer))) {
            goto null_return;
        }
        return note;
    }

    if (!(note = AudioPlayback_AllocNoteFromDisabled(&layer->channel->notePool, layer)) &&
        !(note = AudioPlayback_AllocNoteFromDisabled(&layer->channel->seqPlayer->notePool, layer)) &&
        !(note = AudioPlayback_AllocNoteFromDisabled(&gAudioCtx.noteFreeLists, layer)) &&
        !(note = AudioPlayback_AllocNoteFromDecaying(&layer->channel->notePool, layer)) &&
        !(note = AudioPlayback_AllocNoteFromDecaying(&layer->channel->seqPlayer->notePool, layer)) &&
        !(note = AudioPlayback_AllocNoteFromDecaying(&gAudioCtx.noteFreeLists, layer)) &&
        !(note = AudioPlayback_AllocNoteFromActive(&layer->channel->notePool, layer)) &&
        !(note = AudioPlayback_AllocNoteFromActive(&layer->channel->seqPlayer->notePool, layer)) &&
        !(note = AudioPlayback_AllocNoteFromActive(&gAudioCtx.noteFreeLists, layer))) {
        goto null_return;
    }
    return note;

null_return:
    layer->bit3 = true;
    return NULL;
}

void AudioPlayback_NoteInitAll(void) {
    Note* note;
    s32 i;

    for (i = 0; i < gAudioCtx.numNotes; i++) {
        note = &gAudioCtx.notes[i];
        note->sampleState = gZeroedSampleState;
        note->playbackState.priority = 0;
        note->playbackState.status = PLAYBACK_STATUS_0;
        note->playbackState.parentLayer = NO_LAYER;
        note->playbackState.wantedParentLayer = NO_LAYER;
        note->playbackState.prevParentLayer = NO_LAYER;
        note->playbackState.waveId = 0;
        note->playbackState.attributes.velocity = 0.0f;
        note->playbackState.adsrVolScaleUnused = 0;
        note->playbackState.adsr.action.asByte = 0;
        note->playbackState.vibratoState.active = false;
        note->playbackState.portamento.cur = 0;
        note->playbackState.portamento.speed = 0;
        note->playbackState.stereoHeadsetEffects = false;
        note->playbackState.startSamplePos = 0;
        note->synthesisState.synthesisBuffers =
            AudioHeap_AllocDmaMemory(&gAudioCtx.miscPool, sizeof(NoteSynthesisBuffers));
        note->playbackState.attributes.filterBuf = AudioHeap_AllocDmaMemory(&gAudioCtx.miscPool, FILTER_SIZE);
    }
}
