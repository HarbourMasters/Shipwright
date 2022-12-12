#include "global.h"
#include <Cvar.h>

extern bool gUseLegacySD;

void Audio_InitNoteSub(Note* note, NoteSubEu* sub, NoteSubAttributes* attrs) {
    f32 volRight, volLeft;
    s32 smallPanIndex;
    u64 pad;
    u8 strongLeft;
    u8 strongRight;
    f32 vel;
    u8 pan;
    u8 reverbVol;
    StereoData sp24;
    s32 stereoHeadsetEffects = note->playbackState.stereoHeadsetEffects;

    vel = attrs->velocity;
    pan = attrs->pan;
    reverbVol = attrs->reverbVol;
    sp24 = attrs->stereo.s;

    sub->bitField0 = note->noteSubEu.bitField0;
    sub->bitField1 = note->noteSubEu.bitField1;
    sub->sound.samples = note->noteSubEu.sound.samples;
    sub->unk_06 = note->noteSubEu.unk_06;

    Audio_NoteSetResamplingRate(sub, attrs->frequency);

    pan &= 0x7F;

    sub->bitField0.stereoStrongRight = false;
    sub->bitField0.stereoStrongLeft = false;
    sub->bitField0.stereoHeadsetEffects = sp24.stereoHeadsetEffects;
    sub->bitField0.usesHeadsetPanEffects = sp24.usesHeadsetPanEffects;
    if (stereoHeadsetEffects && gAudioContext.soundMode == 1) {
        smallPanIndex = pan >> 1;
        if (smallPanIndex > 0x3F) {
            smallPanIndex = 0x3F;
        }

        sub->headsetPanLeft = gHeadsetPanQuantization[smallPanIndex];
        sub->headsetPanRight = gHeadsetPanQuantization[0x3F - smallPanIndex];
        sub->bitField1.usesHeadsetPanEffects2 = true;

        volLeft = gHeadsetPanVolume[pan];
        volRight = gHeadsetPanVolume[0x7F - pan];
    } else if (stereoHeadsetEffects && gAudioContext.soundMode == 0) {
        strongLeft = strongRight = 0;
        sub->headsetPanRight = 0;
        sub->headsetPanLeft = 0;
        sub->bitField1.usesHeadsetPanEffects2 = false;

        volLeft = gStereoPanVolume[pan];
        volRight = gStereoPanVolume[0x7F - pan];
        if (pan < 0x20) {
            strongLeft = 1;
        } else if (pan > 0x60) {
            strongRight = 1;
        }

        sub->bitField0.stereoStrongRight = strongRight;
        sub->bitField0.stereoStrongLeft = strongLeft;

        switch (sp24.bit2) {
            case 0:
                break;
            case 1:
                sub->bitField0.stereoStrongRight = sp24.strongRight;
                sub->bitField0.stereoStrongLeft = sp24.strongLeft;
                break;
            case 2:
                sub->bitField0.stereoStrongRight = sp24.strongRight | strongRight;
                sub->bitField0.stereoStrongLeft = sp24.strongLeft | strongLeft;
                break;
            case 3:
                sub->bitField0.stereoStrongRight = sp24.strongRight ^ strongRight;
                sub->bitField0.stereoStrongLeft = sp24.strongLeft ^ strongLeft;
                break;
        }

    } else if (gAudioContext.soundMode == 3) {
        sub->bitField0.stereoHeadsetEffects = false;
        sub->bitField0.usesHeadsetPanEffects = false;
        volLeft = 0.707f; // approx 1/sqrt(2)
        volRight = 0.707f;
    } else {
        sub->bitField0.stereoStrongRight = sp24.strongRight;
        sub->bitField0.stereoStrongLeft = sp24.strongLeft;
        volLeft = gDefaultPanVolume[pan];
        volRight = gDefaultPanVolume[0x7F - pan];
    }

    vel = 0.0f > vel ? 0.0f : vel;
    vel = 1.0f < vel ? 1.0f : vel;

    float master_vol = CVar_GetFloat("gGameMasterVolume", 1.0f);
    sub->targetVolLeft = (s32)((vel * volLeft) * (0x1000 - 0.001f)) * master_vol;
    sub->targetVolRight = (s32)((vel * volRight) * (0x1000 - 0.001f)) * master_vol;

    sub->unk_2 = attrs->unk_1;
    sub->filter = attrs->filter;
    sub->unk_07 = attrs->unk_14;
    sub->unk_0E = attrs->unk_16;
    sub->reverbVol = reverbVol;
}

void Audio_NoteSetResamplingRate(NoteSubEu* noteSubEu, f32 resamplingRateInput) {
    f32 resamplingRate = 0.0f;

    if (resamplingRateInput < 2.0f) {
        noteSubEu->bitField1.hasTwoParts = false;

        if (1.99998f < resamplingRateInput) {
            resamplingRate = 1.99998f;
        } else {
            resamplingRate = resamplingRateInput;
        }

    } else {
        noteSubEu->bitField1.hasTwoParts = true;
        if (3.99996f < resamplingRateInput) {
            if (CVar_GetS32("gExperimentalOctaveDrop", 0)) {
                resamplingRate = resamplingRateInput * 0.25;
            } else {
                resamplingRate = 1.99998f;
            }
        } else {
            resamplingRate = resamplingRateInput * 0.5f;
        }
    }
    noteSubEu->resamplingRateFixedPoint = (s32)(resamplingRate * 32768.0f);
}

void Audio_NoteInit(Note* note) {
    if (note->playbackState.parentLayer->adsr.releaseRate == 0) {
        Audio_AdsrInit(&note->playbackState.adsr, note->playbackState.parentLayer->channel->adsr.envelope,
                       &note->playbackState.adsrVolScaleUnused);
    } else {
        Audio_AdsrInit(&note->playbackState.adsr, note->playbackState.parentLayer->adsr.envelope,
                       &note->playbackState.adsrVolScaleUnused);
    }

    note->playbackState.unk_04 = 0;
    note->playbackState.adsr.action.s.state = ADSR_STATE_INITIAL;
    note->noteSubEu = gDefaultNoteSub;
}

void Audio_NoteDisable(Note* note) {
    if (note->noteSubEu.bitField0.needsInit == true) {
        note->noteSubEu.bitField0.needsInit = false;
    }
    note->playbackState.priority = 0;
    note->noteSubEu.bitField0.enabled = false;
    note->playbackState.unk_04 = 0;
    note->noteSubEu.bitField0.finished = false;
    note->playbackState.parentLayer = NO_LAYER;
    note->playbackState.prevParentLayer = NO_LAYER;
    note->playbackState.adsr.action.s.state = ADSR_STATE_DISABLED;
    note->playbackState.adsr.current = 0;
}

void Audio_ProcessNotes(void) {
    s32 pad[2];
    NoteAttributes* attrs;
    NoteSubEu* noteSubEu2;
    NoteSubEu* noteSubEu;
    Note* note;
    NotePlaybackState* playbackState;
    NoteSubAttributes subAttrs;
    u8 bookOffset;
    f32 scale;
    s32 i;

    for (i = 0; i < gAudioContext.numNotes; i++) {
        note = &gAudioContext.notes[i];
        noteSubEu2 = &gAudioContext.noteSubsEu[gAudioContext.noteSubEuOffset + i];
        playbackState = &note->playbackState;
        if (playbackState->parentLayer != NO_LAYER) {
            if (note != playbackState->parentLayer->note && playbackState->unk_04 == 0) {
                playbackState->adsr.action.s.release = true;
                playbackState->adsr.fadeOutVel = gAudioContext.audioBufferParameters.updatesPerFrameInv;
                playbackState->priority = 1;
                playbackState->unk_04 = 2;
                goto out;
            } else if (!playbackState->parentLayer->enabled && playbackState->unk_04 == 0 &&
                       playbackState->priority >= 1) {
                // do nothing
            } else if (playbackState->parentLayer->channel->seqPlayer == NULL) {
                AudioSeq_SequenceChannelDisable(playbackState->parentLayer->channel);
                playbackState->priority = 1;
                playbackState->unk_04 = 1;
                continue;
            } else if (playbackState->parentLayer->channel->seqPlayer->muted &&
                       (playbackState->parentLayer->channel->muteBehavior & 0x40)) {
                // do nothing
            } else {
                goto out;
            }

            Audio_SeqLayerNoteRelease(playbackState->parentLayer);
            Audio_AudioListRemove(&note->listItem);
            Audio_AudioListPushFront(&note->listItem.pool->decaying, &note->listItem);
            playbackState->priority = 1;
            playbackState->unk_04 = 2;
        } else if (playbackState->unk_04 == 0 && playbackState->priority >= 1) {
            continue;
        }

    out:
        if (playbackState->priority != 0) {
            noteSubEu = &note->noteSubEu;
            if (playbackState->unk_04 >= 1 || noteSubEu->bitField0.finished) {
                if (playbackState->adsr.action.s.state == ADSR_STATE_DISABLED || noteSubEu->bitField0.finished) {
                    if (playbackState->wantedParentLayer != NO_LAYER) {
                        Audio_NoteDisable(note);
                        if (playbackState->wantedParentLayer->channel != NULL) {
                            Audio_NoteInitForLayer(note, playbackState->wantedParentLayer);
                            Audio_NoteVibratoInit(note);
                            Audio_NotePortamentoInit(note);
                            Audio_AudioListRemove(&note->listItem);
                            AudioSeq_AudioListPushBack(&note->listItem.pool->active, &note->listItem);
                            playbackState->wantedParentLayer = NO_LAYER;
                            // don't skip
                        } else {
                            Audio_NoteDisable(note);
                            Audio_AudioListRemove(&note->listItem);
                            AudioSeq_AudioListPushBack(&note->listItem.pool->disabled, &note->listItem);
                            playbackState->wantedParentLayer = NO_LAYER;
                            goto skip;
                        }
                    } else {
                        if (playbackState->parentLayer != NO_LAYER) {
                            playbackState->parentLayer->bit1 = true;
                        }
                        Audio_NoteDisable(note);
                        Audio_AudioListRemove(&note->listItem);
                        AudioSeq_AudioListPushBack(&note->listItem.pool->disabled, &note->listItem);
                        continue;
                    }
                }
            } else if (playbackState->adsr.action.s.state == ADSR_STATE_DISABLED) {
                if (playbackState->parentLayer != NO_LAYER) {
                    playbackState->parentLayer->bit1 = true;
                }
                Audio_NoteDisable(note);
                Audio_AudioListRemove(&note->listItem);
                AudioSeq_AudioListPushBack(&note->listItem.pool->disabled, &note->listItem);
                continue;
            }

            scale = Audio_AdsrUpdate(&playbackState->adsr);
            Audio_NoteVibratoUpdate(note);
            attrs = &playbackState->attributes;
            if (playbackState->unk_04 == 1 || playbackState->unk_04 == 2) {
                subAttrs.frequency = attrs->freqScale;
                subAttrs.velocity = attrs->velocity;
                subAttrs.pan = attrs->pan;
                subAttrs.reverbVol = attrs->reverb;
                subAttrs.stereo = attrs->stereo;
                subAttrs.unk_1 = attrs->unk_1;
                subAttrs.filter = attrs->filter;
                subAttrs.unk_14 = attrs->unk_4;
                subAttrs.unk_16 = attrs->unk_6;
                bookOffset = noteSubEu->bitField1.bookOffset;
            } else {
                SequenceLayer* layer = playbackState->parentLayer;
                SequenceChannel* channel = layer->channel;

                subAttrs.frequency = layer->noteFreqScale;
                subAttrs.velocity = layer->noteVelocity;
                subAttrs.pan = layer->notePan;
                if (layer->stereo.asByte == 0) {
                    subAttrs.stereo = channel->stereo;
                } else {
                    subAttrs.stereo = layer->stereo;
                }
                subAttrs.reverbVol = channel->reverb;
                subAttrs.unk_1 = channel->unk_0C;
                subAttrs.filter = channel->filter;
                subAttrs.unk_14 = channel->unk_0F;
                subAttrs.unk_16 = channel->unk_20;
                bookOffset = channel->bookOffset & 0x7;

                if (channel->seqPlayer->muted && (channel->muteBehavior & 8)) {
                    subAttrs.frequency = 0.0f;
                    subAttrs.velocity = 0.0f;
                }
            }

            subAttrs.frequency *= playbackState->vibratoFreqScale * playbackState->portamentoFreqScale;

            f32 resampRate = gAudioContext.audioBufferParameters.resampleRate;

            // CUSTOM SAMPLE CHECK
            if (!noteSubEu2->bitField1.isSyntheticWave && noteSubEu2->sound.soundFontSound != NULL &&
                noteSubEu2->sound.soundFontSound->sample != NULL &&
                noteSubEu2->sound.soundFontSound->sample->sampleRateMagicValue == 'RIFF') {
                resampRate = CALC_RESAMPLE_FREQ(noteSubEu2->sound.soundFontSound->sample->sampleRate);
            }

            subAttrs.frequency *= resampRate;


            subAttrs.velocity *= scale;
            Audio_InitNoteSub(note, noteSubEu2, &subAttrs);
            noteSubEu->bitField1.bookOffset = bookOffset;
        skip:;
        }
    }
}

SoundFontSound* Audio_InstrumentGetSound(Instrument* instrument, s32 semitone) {
    SoundFontSound* sound;
    if (semitone < instrument->normalRangeLo) {
        sound = &instrument->lowNotesSound;
    } else if (semitone <= instrument->normalRangeHi) {
        sound = &instrument->normalNotesSound;
    } else {
        sound = &instrument->highNotesSound;
    }
    return sound;
}

Instrument* Audio_GetInstrumentInner(s32 fontId, s32 instId) {
    Instrument* inst;

    if (fontId == 0xFF) {
        return NULL;
    }

    if (!AudioLoad_IsFontLoadComplete(fontId)) {
        gAudioContext.audioErrorFlags = fontId + 0x10000000;
        return NULL;
    }

    int instCnt = 0;
    SoundFont* sf = ResourceMgr_LoadAudioSoundFont(fontMap[fontId]);

    if (instId >= sf->numInstruments)
        return NULL;

    inst = sf->instruments[instId];

    if (inst == NULL) {
        gAudioContext.audioErrorFlags = ((fontId << 8) + instId) + 0x1000000;
        return inst;
    }

    return inst;
}

Drum* Audio_GetDrum(s32 fontId, s32 drumId) {
    Drum* drum = NULL;

    if (fontId == 0xFF) {
        return NULL;
    }

    if (!AudioLoad_IsFontLoadComplete(fontId)) {
        gAudioContext.audioErrorFlags = fontId + 0x10000000;
        return NULL;
    }

    
    SoundFont* sf = ResourceMgr_LoadAudioSoundFont(fontMap[fontId]);
    if (drumId < sf->numDrums) {
        drum = sf->drums[drumId];
    }
    
    if (drum == NULL) {
        gAudioContext.audioErrorFlags = ((fontId << 8) + drumId) + 0x5000000;
    }

    return drum;
}

SoundFontSound* Audio_GetSfx(s32 fontId, s32 sfxId) {
    SoundFontSound* sfx = NULL;

    if (fontId == 0xFF) {
        return NULL;
    }

    if (!AudioLoad_IsFontLoadComplete(fontId)) {
        gAudioContext.audioErrorFlags = fontId + 0x10000000;
        return NULL;
    }

    SoundFont* sf = ResourceMgr_LoadAudioSoundFont(fontMap[fontId]);
    if (sfxId < sf->numSfx) {
        sfx = &sf->soundEffects[sfxId];
    }

    if (sfx == NULL) {
        gAudioContext.audioErrorFlags = ((fontId << 8) + sfxId) + 0x5000000;
    }

    if (sfx != NULL && sfx->sample == NULL) {
        return NULL;
    }

    return sfx;
}

s32 Audio_SetFontInstrument(s32 instrumentType, s32 fontId, s32 index, void* value) {
    if (fontId == 0xFF) {
        return -1;
    }

    if (!AudioLoad_IsFontLoadComplete(fontId)) {
        return -2;
    }

    switch (instrumentType) {
        case 0:
            if (index >= gAudioContext.soundFonts[fontId].numDrums) {
                return -3;
            }
            gAudioContext.soundFonts[fontId].drums[index] = value;
            break;

        case 1:
            if (index >= gAudioContext.soundFonts[fontId].numSfx) {
                return -3;
            }
            gAudioContext.soundFonts[fontId].soundEffects[index] = *(SoundFontSound*)value;
            break;

        default:
            if (index >= gAudioContext.soundFonts[fontId].numInstruments) {
                return -3;
            }
            gAudioContext.soundFonts[fontId].instruments[index] = value;
            break;
    }

    return 0;
}

void Audio_SeqLayerDecayRelease(SequenceLayer* layer, s32 target) {
    Note* note;
    NoteAttributes* attrs;
    SequenceChannel* chan;
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
            note->playbackState.prevParentLayer == layer && target != ADSR_STATE_DECAY) {
            note->playbackState.adsr.fadeOutVel = gAudioContext.audioBufferParameters.updatesPerFrameInv;
            note->playbackState.adsr.action.s.release = true;
        }
        return;
    }

    if (note->playbackState.adsr.action.s.state != ADSR_STATE_DECAY) {
        attrs->freqScale = layer->noteFreqScale;
        attrs->velocity = layer->noteVelocity;
        attrs->pan = layer->notePan;

        if (layer->channel != NULL) {
            chan = layer->channel;
            attrs->reverb = chan->reverb;
            attrs->unk_1 = chan->unk_0C;
            attrs->filter = chan->filter;

            if (attrs->filter != NULL) {
                for (i = 0; i < 8; i++) {
                    attrs->filterBuf[i] = attrs->filter[i];
                }
                attrs->filter = attrs->filterBuf;
            }

            attrs->unk_6 = chan->unk_20;
            attrs->unk_4 = chan->unk_0F;
            if (chan->seqPlayer->muted && (chan->muteBehavior & 8)) {
                note->noteSubEu.bitField0.finished = true;
            }

            if (layer->stereo.asByte == 0) {
                attrs->stereo = chan->stereo;
            } else {
                attrs->stereo = layer->stereo;
            }
            note->playbackState.priority = chan->someOtherPriority;
        } else {
            attrs->stereo = layer->stereo;
            note->playbackState.priority = 1;
        }

        note->playbackState.prevParentLayer = note->playbackState.parentLayer;
        note->playbackState.parentLayer = NO_LAYER;
        if (target == ADSR_STATE_RELEASE) {
            note->playbackState.adsr.fadeOutVel = gAudioContext.audioBufferParameters.updatesPerFrameInv;
            note->playbackState.adsr.action.s.release = true;
            note->playbackState.unk_04 = 2;
        } else {
            note->playbackState.unk_04 = 1;
            note->playbackState.adsr.action.s.decay = true;
            if (layer->adsr.releaseRate == 0) {
                note->playbackState.adsr.fadeOutVel = gAudioContext.unk_3520[layer->channel->adsr.releaseRate];
            } else {
                note->playbackState.adsr.fadeOutVel = gAudioContext.unk_3520[layer->adsr.releaseRate];
            }
            note->playbackState.adsr.sustain =
                ((f32)(s32)(layer->channel->adsr.sustain) * note->playbackState.adsr.current) / 256.0f;
        }
    }

    if (target == ADSR_STATE_DECAY) {
        Audio_AudioListRemove(&note->listItem);
        Audio_AudioListPushFront(&note->listItem.pool->decaying, &note->listItem);
    }
}

void Audio_SeqLayerNoteDecay(SequenceLayer* layer) {
    Audio_SeqLayerDecayRelease(layer, ADSR_STATE_DECAY);
}

void Audio_SeqLayerNoteRelease(SequenceLayer* layer) {
    Audio_SeqLayerDecayRelease(layer, ADSR_STATE_RELEASE);
}

s32 Audio_BuildSyntheticWave(Note* note, SequenceLayer* layer, s32 waveId) {
    f32 freqScale;
    f32 ratio;
    u8 sampleCountIndex;

    if (waveId < 128) {
        waveId = 128;
    }
    if (waveId > 136) {
        waveId = 136;
    }

    freqScale = layer->freqScale;
    if (layer->portamento.mode != 0 && 0.0f < layer->portamento.extent) {
        freqScale *= (layer->portamento.extent + 1.0f);
    }
    if (freqScale < 0.99999f) {
        sampleCountIndex = 0;
        ratio = 1.0465f;
    } else if (freqScale < 1.99999f) {
        sampleCountIndex = 1;
        ratio = 0.52325f;
    } else if (freqScale < 3.99999f) {
        sampleCountIndex = 2;
        ratio = 0.26263f;
    } else {
        sampleCountIndex = 3;
        ratio = 0.13081f;
    }
    layer->freqScale *= ratio;
    note->playbackState.waveId = waveId;
    note->playbackState.sampleCountIndex = sampleCountIndex;

    note->noteSubEu.sound.samples = &gWaveSamples[waveId - 128][sampleCountIndex * 64];

    return sampleCountIndex;
}

void Audio_InitSyntheticWave(Note* note, SequenceLayer* layer) {
    s32 sampleCountIndex;
    s32 waveSampleCountIndex;
    s32 waveId = layer->instOrWave;

    if (waveId == 0xFF) {
        waveId = layer->channel->instOrWave;
    }

    sampleCountIndex = note->playbackState.sampleCountIndex;
    waveSampleCountIndex = Audio_BuildSyntheticWave(note, layer, waveId);

    if (waveSampleCountIndex != sampleCountIndex) {
        note->noteSubEu.unk_06 = waveSampleCountIndex * 4 + sampleCountIndex;
    }
}

void Audio_InitNoteList(AudioListItem* list) {
    list->prev = list;
    list->next = list;
    list->u.count = 0;
}

void Audio_InitNoteLists(NotePool* pool) {
    Audio_InitNoteList(&pool->disabled);
    Audio_InitNoteList(&pool->decaying);
    Audio_InitNoteList(&pool->releasing);
    Audio_InitNoteList(&pool->active);
    pool->disabled.pool = pool;
    pool->decaying.pool = pool;
    pool->releasing.pool = pool;
    pool->active.pool = pool;
}

void Audio_InitNoteFreeList(void) {
    s32 i;

    Audio_InitNoteLists(&gAudioContext.noteFreeLists);
    for (i = 0; i < gAudioContext.numNotes; i++) {
        gAudioContext.notes[i].listItem.u.value = &gAudioContext.notes[i];
        gAudioContext.notes[i].listItem.prev = NULL;
        AudioSeq_AudioListPushBack(&gAudioContext.noteFreeLists.disabled, &gAudioContext.notes[i].listItem);
    }
}

void Audio_NotePoolClear(NotePool* pool) {
    s32 i;
    AudioListItem* source;
    AudioListItem* cur;
    AudioListItem* dest;

    for (i = 0; i < 4; i++) {
        switch (i) {
            case 0:
                source = &pool->disabled;
                dest = &gAudioContext.noteFreeLists.disabled;
                break;

            case 1:
                source = &pool->decaying;
                dest = &gAudioContext.noteFreeLists.decaying;
                break;

            case 2:
                source = &pool->releasing;
                dest = &gAudioContext.noteFreeLists.releasing;
                break;

            case 3:
                source = &pool->active;
                dest = &gAudioContext.noteFreeLists.active;
                break;
        }

        for (;;) {
            cur = source->next;
            if (cur == source || cur == NULL) {
                break;
            }
            Audio_AudioListRemove(cur);
            AudioSeq_AudioListPushBack(dest, cur);
        }
    }
}

void Audio_NotePoolFill(NotePool* pool, s32 count) {
    s32 i;
    s32 j;
    Note* note;
    AudioListItem* source;
    AudioListItem* dest;

    Audio_NotePoolClear(pool);

    for (i = 0, j = 0; j < count; i++) {
        if (i == 4) {
            return;
        }

        switch (i) {
            case 0:
                source = &gAudioContext.noteFreeLists.disabled;
                dest = &pool->disabled;
                break;

            case 1:
                source = &gAudioContext.noteFreeLists.decaying;
                dest = &pool->decaying;
                break;

            case 2:
                source = &gAudioContext.noteFreeLists.releasing;
                dest = &pool->releasing;
                break;

            case 3:
                source = &gAudioContext.noteFreeLists.active;
                dest = &pool->active;
                break;
        }

        while (j < count) {
            note = AudioSeq_AudioListPopBack(source);
            if (note == NULL) {
                break;
            }
            AudioSeq_AudioListPushBack(dest, &note->listItem);
            j++;
        }
    }
}

void Audio_AudioListPushFront(AudioListItem* list, AudioListItem* item) {
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

void Audio_AudioListRemove(AudioListItem* item) {
    // remove 'item' from the list it's in, if any
    if (item->prev != NULL) {
        item->prev->next = item->next;
        item->next->prev = item->prev;
        item->prev = NULL;
    }
}

Note* Audio_FindNodeWithPrioLessThan(AudioListItem* list, s32 limit) {
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

void Audio_NoteInitForLayer(Note* note, SequenceLayer* layer) {
    s32 pad[3];
    s16 instId;
    NotePlaybackState* playback = &note->playbackState;
    NoteSubEu* sub = &note->noteSubEu;

    note->playbackState.prevParentLayer = NO_LAYER;
    note->playbackState.parentLayer = layer;
    playback->priority = layer->channel->notePriority;
    layer->notePropertiesNeedInit = true;
    layer->bit3 = true;
    layer->note = note;
    layer->channel->noteUnused = note;
    layer->channel->layerUnused = layer;
    layer->noteVelocity = 0.0f;
    Audio_NoteInit(note);
    instId = layer->instOrWave;

    if (instId == 0xFF) {
        instId = layer->channel->instOrWave;
    }
    sub->sound.soundFontSound = layer->sound;

    if (instId >= 0x80 && instId < 0xC0) {
        sub->bitField1.isSyntheticWave = true;
    } else {
        sub->bitField1.isSyntheticWave = false;
    }

    if (sub->bitField1.isSyntheticWave) {
        Audio_BuildSyntheticWave(note, layer, instId);
    }

    playback->fontId = layer->channel->fontId;
    playback->stereoHeadsetEffects = layer->channel->stereoHeadsetEffects;
    sub->bitField1.reverbIndex = layer->channel->reverbIndex & 3;
}

void func_800E82C0(Note* note, SequenceLayer* layer) {
    // similar to Audio_NoteReleaseAndTakeOwnership, hard to say what the difference is
    Audio_SeqLayerNoteRelease(note->playbackState.parentLayer);
    note->playbackState.wantedParentLayer = layer;
}

void Audio_NoteReleaseAndTakeOwnership(Note* note, SequenceLayer* layer) {
    note->playbackState.wantedParentLayer = layer;
    note->playbackState.priority = layer->channel->notePriority;

    note->playbackState.adsr.fadeOutVel = gAudioContext.audioBufferParameters.updatesPerFrameInv;
    note->playbackState.adsr.action.s.release = true;
}

Note* Audio_AllocNoteFromDisabled(NotePool* pool, SequenceLayer* layer) {
    Note* note = AudioSeq_AudioListPopBack(&pool->disabled);
    if (note != NULL) {
        Audio_NoteInitForLayer(note, layer);
        Audio_AudioListPushFront(&pool->active, &note->listItem);
    }
    return note;
}

Note* Audio_AllocNoteFromDecaying(NotePool* pool, SequenceLayer* layer) {
    Note* note = AudioSeq_AudioListPopBack(&pool->decaying);
    if (note != NULL) {
        Audio_NoteReleaseAndTakeOwnership(note, layer);
        AudioSeq_AudioListPushBack(&pool->releasing, &note->listItem);
    }
    return note;
}

Note* Audio_AllocNoteFromActive(NotePool* pool, SequenceLayer* layer) {
    Note* rNote;
    Note* aNote;
    s32 rPriority;
    s32 aPriority;

    rPriority = aPriority = 0x10;
    rNote = Audio_FindNodeWithPrioLessThan(&pool->releasing, layer->channel->notePriority);

    if (rNote != NULL) {
        rPriority = rNote->playbackState.priority;
    }

    aNote = Audio_FindNodeWithPrioLessThan(&pool->active, layer->channel->notePriority);

    if (aNote != NULL) {
        aPriority = aNote->playbackState.priority;
    }

    if (rNote == NULL && aNote == NULL) {
        return NULL;
    }

    if (aPriority < rPriority) {
        Audio_AudioListRemove(&aNote->listItem);
        func_800E82C0(aNote, layer);
        AudioSeq_AudioListPushBack(&pool->releasing, &aNote->listItem);
        aNote->playbackState.priority = layer->channel->notePriority;
        return aNote;
    }
    rNote->playbackState.wantedParentLayer = layer;
    rNote->playbackState.priority = layer->channel->notePriority;
    return rNote;
}

Note* Audio_AllocNote(SequenceLayer* layer) {
    Note* ret;
    u32 policy = layer->channel->noteAllocPolicy;

    if (policy & 1) {
        ret = layer->note;
        if (ret != NULL && ret->playbackState.prevParentLayer == layer &&
            ret->playbackState.wantedParentLayer == NO_LAYER) {
            Audio_NoteReleaseAndTakeOwnership(ret, layer);
            Audio_AudioListRemove(&ret->listItem);
            AudioSeq_AudioListPushBack(&ret->listItem.pool->releasing, &ret->listItem);
            return ret;
        }
    }

    if (policy & 2) {
        if (!(ret = Audio_AllocNoteFromDisabled(&layer->channel->notePool, layer)) &&
            !(ret = Audio_AllocNoteFromDecaying(&layer->channel->notePool, layer)) &&
            !(ret = Audio_AllocNoteFromActive(&layer->channel->notePool, layer))) {
            goto null_return;
        }
        return ret;
    }

    if (policy & 4) {
        if (!(ret = Audio_AllocNoteFromDisabled(&layer->channel->notePool, layer)) &&
            !(ret = Audio_AllocNoteFromDisabled(&layer->channel->seqPlayer->notePool, layer)) &&
            !(ret = Audio_AllocNoteFromDecaying(&layer->channel->notePool, layer)) &&
            !(ret = Audio_AllocNoteFromDecaying(&layer->channel->seqPlayer->notePool, layer)) &&
            !(ret = Audio_AllocNoteFromActive(&layer->channel->notePool, layer)) &&
            !(ret = Audio_AllocNoteFromActive(&layer->channel->seqPlayer->notePool, layer))) {
            goto null_return;
        }
        return ret;
    }

    if (policy & 8) {
        if (!(ret = Audio_AllocNoteFromDisabled(&gAudioContext.noteFreeLists, layer)) &&
            !(ret = Audio_AllocNoteFromDecaying(&gAudioContext.noteFreeLists, layer)) &&
            !(ret = Audio_AllocNoteFromActive(&gAudioContext.noteFreeLists, layer))) {
            goto null_return;
        }
        return ret;
    }

    if (!(ret = Audio_AllocNoteFromDisabled(&layer->channel->notePool, layer)) &&
        !(ret = Audio_AllocNoteFromDisabled(&layer->channel->seqPlayer->notePool, layer)) &&
        !(ret = Audio_AllocNoteFromDisabled(&gAudioContext.noteFreeLists, layer)) &&
        !(ret = Audio_AllocNoteFromDecaying(&layer->channel->notePool, layer)) &&
        !(ret = Audio_AllocNoteFromDecaying(&layer->channel->seqPlayer->notePool, layer)) &&
        !(ret = Audio_AllocNoteFromDecaying(&gAudioContext.noteFreeLists, layer)) &&
        !(ret = Audio_AllocNoteFromActive(&layer->channel->notePool, layer)) &&
        !(ret = Audio_AllocNoteFromActive(&layer->channel->seqPlayer->notePool, layer)) &&
        !(ret = Audio_AllocNoteFromActive(&gAudioContext.noteFreeLists, layer))) {
        goto null_return;
    }
    return ret;

null_return:
    layer->bit3 = true;
    return NULL;
}

void Audio_NoteInitAll(void) {
    Note* note;
    s32 i;

    for (i = 0; i < gAudioContext.numNotes; i++) {
        note = &gAudioContext.notes[i];
        note->noteSubEu = gZeroNoteSub;
        note->playbackState.priority = 0;
        note->playbackState.unk_04 = 0;
        note->playbackState.parentLayer = NO_LAYER;
        note->playbackState.wantedParentLayer = NO_LAYER;
        note->playbackState.prevParentLayer = NO_LAYER;
        note->playbackState.waveId = 0;
        note->playbackState.attributes.velocity = 0.0f;
        note->playbackState.adsrVolScaleUnused = 0;
        note->playbackState.adsr.action.asByte = 0;
        note->vibratoState.active = 0;
        note->portamento.cur = 0;
        note->portamento.speed = 0;
        note->playbackState.stereoHeadsetEffects = false;
        note->unk_BC = 0;
        note->synthesisState.synthesisBuffers = AudioHeap_AllocDmaMemory(&gAudioContext.notesAndBuffersPool, 0x1E0);
    }
}
