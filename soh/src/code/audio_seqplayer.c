#include <stdlib.h>

#include "ultra64.h"
#include "global.h"

extern char* sequenceMap[MAX_SEQUENCES];

#define PORTAMENTO_IS_SPECIAL(x) ((x).mode & 0x80)
#define PORTAMENTO_MODE(x) ((x).mode & ~0x80)
#define PORTAMENTO_MODE_1 1
#define PORTAMENTO_MODE_2 2
#define PORTAMENTO_MODE_3 3
#define PORTAMENTO_MODE_4 4
#define PORTAMENTO_MODE_5 5

u8 AudioSeq_ScriptReadU8(SeqScriptState* state);
s16 AudioSeq_ScriptReadS16(SeqScriptState* state);
u16 AudioSeq_ScriptReadCompressedU16(SeqScriptState* state);

u8 AudioSeq_GetInstrument(SequenceChannel* channel, u8 instId, Instrument** instOut, AdsrSettings* adsr);

u8 D_80130520[] = {
    0x81, 0x00, 0x81, 0x01, 0x00, 0x00, 0x00, 0x81, 0x01, 0x01, 0x01, 0x42, 0x81, 0xC2, 0x00, 0x00,
    0x00, 0x01, 0x81, 0x00, 0x00, 0x00, 0x01, 0x42, 0x01, 0x01, 0x01, 0x81, 0x01, 0x01, 0x81, 0x81,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x81, 0x01, 0x01, 0x01, 0x81, 0x01,
    0x01, 0x03, 0x03, 0x01, 0x00, 0x01, 0x01, 0x81, 0x03, 0x01, 0x00, 0x02, 0x00, 0x01, 0x01, 0x82,
    0x00, 0x01, 0x01, 0x01, 0x01, 0x81, 0x00, 0x00, 0x01, 0x81, 0x81, 0x81, 0x81, 0x00, 0x00, 0x00,
};

u16 AudioSeq_GetScriptControlFlowArgument(SeqScriptState* state, u8 arg1) {
    u8 temp_v0 = D_80130520[arg1 - 0xB0];
    u8 loBits = temp_v0 & 3;
    u16 ret = 0;

    if (loBits == 1) {
        if ((temp_v0 & 0x80) == 0) {
            ret = AudioSeq_ScriptReadU8(state);
        } else {
            ret = AudioSeq_ScriptReadS16(state);
        }
    }
    return ret;
}

s32 AudioSeq_HandleScriptFlowControl(SequencePlayer* seqPlayer, SeqScriptState* state, s32 cmd, s32 arg) {
    switch (cmd) {
        case 0xFF:
            if (state->depth == 0) {
                return -1;
            }
            state->pc = state->stack[--state->depth];
            break;

        case 0xFD:
            return AudioSeq_ScriptReadCompressedU16(state);

        case 0xFE:
            return 1;

        case 0xFC:
            state->stack[state->depth++] = state->pc;
            state->pc = seqPlayer->seqData + (u16)arg;
            break;

        case 0xF8:
            state->remLoopIters[state->depth] = arg;
            state->stack[state->depth++] = state->pc;
            break;

        case 0xF7:
            state->remLoopIters[state->depth - 1]--;
            if (state->remLoopIters[state->depth - 1] != 0) {
                state->pc = state->stack[state->depth - 1];
            } else {
                state->depth--;
            }
            break;

        case 0xF6:
            state->depth--;
            break;

        case 0xF5:
        case 0xF9:
        case 0xFA:
        case 0xFB:
            if (cmd == 0xFA && state->value != 0) {
                break;
            }
            if (cmd == 0xF9 && state->value >= 0) {
                break;
            }
            if (cmd == 0xF5 && state->value < 0) {
                break;
            }
            state->pc = seqPlayer->seqData + (u16)arg;
            break;

        case 0xF2:
        case 0xF3:
        case 0xF4:
            if (cmd == 0xF3 && state->value != 0) {
                break;
            }
            if (cmd == 0xF2 && state->value >= 0) {
                break;
            }
            state->pc += (s8)(arg & 0xFF);
            break;
    }

    return 0;
}

void AudioSeq_InitSequenceChannel(SequenceChannel* channel) {
    s32 i;

    if (channel == &gAudioContext.sequenceChannelNone) {
        return;
    }

    channel->enabled = false;
    channel->finished = false;
    channel->stopScript = false;
    channel->stopSomething2 = false;
    channel->hasInstrument = false;
    channel->stereoHeadsetEffects = false;
    channel->transposition = 0;
    channel->largeNotes = false;
    channel->bookOffset = 0;
    channel->stereo.asByte = 0;
    channel->changes.asByte = 0xFF;
    channel->scriptState.depth = 0;
    channel->newPan = 0x40;
    channel->panChannelWeight = 0x80;
    channel->velocityRandomVariance = 0;
    channel->gateTimeRandomVariance = 0;
    channel->noteUnused = NULL;
    channel->reverbIndex = 0;
    channel->reverb = 0;
    channel->unk_0C = 0;
    channel->notePriority = 3;
    channel->someOtherPriority = 1;
    channel->delay = 0;
    channel->adsr.envelope = gDefaultEnvelope;
    channel->adsr.releaseRate = 0xF0;
    channel->adsr.sustain = 0;
    channel->vibratoRateTarget = 0x800;
    channel->vibratoRateStart = 0x800;
    channel->vibratoExtentTarget = 0;
    channel->vibratoExtentStart = 0;
    channel->vibratoRateChangeDelay = 0;
    channel->vibratoExtentChangeDelay = 0;
    channel->vibratoDelay = 0;
    channel->filter = NULL;
    channel->unk_20 = 0;
    channel->unk_0F = 0;
    channel->volume = 1.0f;
    channel->volumeScale = 1.0f;
    channel->freqScale = 1.0f;

    for (i = 0; i < 8; i++) {
        channel->soundScriptIO[i] = -1;
    }

    channel->unused = false;
    Audio_InitNoteLists(&channel->notePool);
}

s32 AudioSeq_SeqChannelSetLayer(SequenceChannel* channel, s32 layerIdx) {
    SequenceLayer* layer;

    if (channel->layers[layerIdx] == NULL) {
        SequenceLayer* layer;
        layer = AudioSeq_AudioListPopBack(&gAudioContext.layerFreeList);
        channel->layers[layerIdx] = layer;
        if (layer == NULL) {
            channel->layers[layerIdx] = NULL;
            return -1;
        }
    } else {
        Audio_SeqLayerNoteDecay(channel->layers[layerIdx]);
    }

    layer = channel->layers[layerIdx];
    layer->channel = channel;
    layer->adsr = channel->adsr;
    layer->adsr.releaseRate = 0;
    layer->enabled = true;
    layer->finished = false;
    layer->stopSomething = false;
    layer->continuousNotes = false;
    layer->bit3 = false;
    layer->ignoreDrumPan = false;
    layer->bit1 = false;
    layer->notePropertiesNeedInit = false;
    layer->stereo.asByte = 0;
    layer->portamento.mode = 0;
    layer->scriptState.depth = 0;
    layer->gateTime = 0x80;
    layer->pan = 0x40;
    layer->transposition = 0;
    layer->delay = 0;
    layer->gateDelay = 0;
    layer->delay2 = 0;
    layer->note = NULL;
    layer->instrument = NULL;
    layer->freqScale = 1.0f;
    layer->unk_34 = 1.0f;
    layer->velocitySquare2 = 0.0f;
    layer->instOrWave = 0xFF;
    return 0;
}

void AudioSeq_SeqLayerDisable(SequenceLayer* layer) {
    if (layer != NULL) {
        if (layer->channel != &gAudioContext.sequenceChannelNone && layer->channel->seqPlayer->finished == 1) {
            Audio_SeqLayerNoteRelease(layer);
        } else {
            Audio_SeqLayerNoteDecay(layer);
        }
        layer->enabled = false;
        layer->finished = true;
    }
}

void AudioSeq_SeqLayerFree(SequenceChannel* channel, s32 layerIdx) {
    SequenceLayer* layer = channel->layers[layerIdx];

    if (layer != NULL) {
        AudioSeq_AudioListPushBack(&gAudioContext.layerFreeList, &layer->listItem);
        AudioSeq_SeqLayerDisable(layer);
        channel->layers[layerIdx] = NULL;
    }
}

void AudioSeq_SequenceChannelDisable(SequenceChannel* channel) {
    s32 i;

    for (i = 0; i < 4; i++) {
        AudioSeq_SeqLayerFree(channel, i);
    }

    Audio_NotePoolClear(&channel->notePool);
    channel->enabled = false;
    channel->finished = true;
}

void AudioSeq_SequencePlayerSetupChannels(SequencePlayer* seqPlayer, u16 channelBits) {
    SequenceChannel* channel;
    s32 i;

    for (i = 0; i < 0x10; i++) {
        if (channelBits & 1) {
            channel = seqPlayer->channels[i];
            channel->fontId = seqPlayer->defaultFont;
            channel->muteBehavior = seqPlayer->muteBehavior;
            channel->noteAllocPolicy = seqPlayer->noteAllocPolicy;
        }
        channelBits = channelBits >> 1;
    }
}

void AudioSeq_SequencePlayerDisableChannels(SequencePlayer* seqPlayer, u16 channelBitsUnused) {
    SequenceChannel* channel;
    s32 i;

    for (i = 0; i < 0x10; i++) {
        channel = seqPlayer->channels[i];
        if (IS_SEQUENCE_CHANNEL_VALID(channel) == 1) {
            AudioSeq_SequenceChannelDisable(channel);
        }
    }
}

void AudioSeq_SequenceChannelEnable(SequencePlayer* seqPlayer, u8 channelIdx, void* script) {
    SequenceChannel* channel = seqPlayer->channels[channelIdx];
    s32 i;

    channel->enabled = true;
    channel->finished = false;
    channel->scriptState.depth = 0;
    channel->scriptState.pc = script;
    channel->delay = 0;
    for (i = 0; i < 4; i++) {
        if (channel->layers[i] != NULL) {
            AudioSeq_SeqLayerFree(channel, i);
        }
    }
}

void AudioSeq_SequencePlayerDisableAsFinished(SequencePlayer* seqPlayer) {
    seqPlayer->finished = true;
    AudioSeq_SequencePlayerDisable(seqPlayer);
}

void AudioSeq_SequencePlayerDisable(SequencePlayer* seqPlayer) {
    AudioSeq_SequencePlayerDisableChannels(seqPlayer, 0xFFFF);
    Audio_NotePoolClear(&seqPlayer->notePool);
    if (!seqPlayer->enabled) {
        return;
    }

    seqPlayer->enabled = false;
    seqPlayer->finished = true;

    if (AudioLoad_IsSeqLoadComplete(seqPlayer->seqId)) {
        AudioLoad_SetSeqLoadStatus(seqPlayer->seqId, 3);
    }
    if (AudioLoad_IsFontLoadComplete(seqPlayer->defaultFont)) {
        AudioLoad_SetFontLoadStatus(seqPlayer->defaultFont, 4);
    }

    if (seqPlayer->defaultFont == gAudioContext.fontCache.temporary.entries[0].id) {
        gAudioContext.fontCache.temporary.nextSide = 0;
    } else if (seqPlayer->defaultFont == gAudioContext.fontCache.temporary.entries[1].id) {
        gAudioContext.fontCache.temporary.nextSide = 1;
    }
}

void AudioSeq_AudioListPushBack(AudioListItem* list, AudioListItem* item) {
    if (item->prev == NULL) {
        list->prev->next = item;
        item->prev = list->prev;
        item->next = list;
        list->prev = item;
        list->u.count++;
        item->pool = list->pool;
    }
}

void* AudioSeq_AudioListPopBack(AudioListItem* list) {
    AudioListItem* item = list->prev;

    if (item == list) {
        return NULL;
    }

    item->prev->next = list;
    list->prev = item->prev;
    item->prev = NULL;
    list->u.count--;
    return item->u.value;
}

void AudioSeq_InitLayerFreelist(void) {
    s32 i;

    gAudioContext.layerFreeList.prev = &gAudioContext.layerFreeList;
    gAudioContext.layerFreeList.next = &gAudioContext.layerFreeList;
    gAudioContext.layerFreeList.u.count = 0;
    gAudioContext.layerFreeList.pool = NULL;

    for (i = 0; i < ARRAY_COUNT(gAudioContext.sequenceLayers); i++) {
        gAudioContext.sequenceLayers[i].listItem.u.value = &gAudioContext.sequenceLayers[i];
        gAudioContext.sequenceLayers[i].listItem.prev = NULL;
        AudioSeq_AudioListPushBack(&gAudioContext.layerFreeList, &gAudioContext.sequenceLayers[i].listItem);
    }
}

u8 AudioSeq_ScriptReadU8(SeqScriptState* state) {
    return *(state->pc++);
}

s16 AudioSeq_ScriptReadS16(SeqScriptState* state) {
    s16 ret = *(state->pc++) << 8;

    ret = *(state->pc++) | ret;
    return ret;
}

u16 AudioSeq_ScriptReadCompressedU16(SeqScriptState* state) {
    u16 ret = *(state->pc++);

    if (ret & 0x80) {
        ret = (ret << 8) & 0x7F00;
        ret = *(state->pc++) | ret;
    }
    return ret;
}

void AudioSeq_SeqLayerProcessScriptStep1(SequenceLayer* layer);
s32 AudioSeq_SeqLayerProcessScriptStep2(SequenceLayer* layer);
s32 AudioSeq_SeqLayerProcessScriptStep3(SequenceLayer* layer, s32 cmd);
s32 AudioSeq_SeqLayerProcessScriptStep4(SequenceLayer* layer, s32 cmd);
s32 AudioSeq_SeqLayerProcessScriptStep5(SequenceLayer* layer, s32 sameSound);

void AudioSeq_SeqLayerProcessScript(SequenceLayer* layer) {
    s32 val;

    if (!layer->enabled) {
        return;
    }

    if (layer->delay > 1) {
        layer->delay--;
        if (!layer->stopSomething && layer->delay <= layer->gateDelay) {
            Audio_SeqLayerNoteDecay(layer);
            layer->stopSomething = true;
        }
        return;
    }

    AudioSeq_SeqLayerProcessScriptStep1(layer);
    val = AudioSeq_SeqLayerProcessScriptStep2(layer);
    if (val == -1) {
        return;
    }

    val = AudioSeq_SeqLayerProcessScriptStep3(layer, val);
    if (val != -1) {
        val = AudioSeq_SeqLayerProcessScriptStep4(layer, val);
    }
    if (val != -1) {
        AudioSeq_SeqLayerProcessScriptStep5(layer, val);
    }

    if (layer->stopSomething == true) {
        if ((layer->note != NULL) || layer->continuousNotes) {
            Audio_SeqLayerNoteDecay(layer);
        }
    }
}

void AudioSeq_SeqLayerProcessScriptStep1(SequenceLayer* layer) {
    if (!layer->continuousNotes) {
        Audio_SeqLayerNoteDecay(layer);
    } else if (layer->note != NULL && layer->note->playbackState.wantedParentLayer == layer) {
        Audio_SeqLayerNoteDecay(layer);
    }

    if (PORTAMENTO_MODE(layer->portamento) == PORTAMENTO_MODE_1 ||
        PORTAMENTO_MODE(layer->portamento) == PORTAMENTO_MODE_2) {
        layer->portamento.mode = 0;
    }
    layer->notePropertiesNeedInit = true;
}

s32 AudioSeq_SeqLayerProcessScriptStep5(SequenceLayer* layer, s32 sameSound) {
    if (!layer->stopSomething && layer->sound != NULL && layer->sound->sample->codec == CODEC_S16_INMEMORY &&
        layer->sound->sample->medium != MEDIUM_RAM) {
        layer->stopSomething = true;
        return -1;
    }

    if (layer->continuousNotes == true && layer->bit1 == 1) {
        return 0;
    }

    if (layer->continuousNotes == true && layer->note != NULL && layer->bit3 && sameSound == true &&
        layer->note->playbackState.parentLayer == layer) {
        if (layer->sound == NULL) {
            Audio_InitSyntheticWave(layer->note, layer);
        }
    } else {
        if (sameSound == false) {
            Audio_SeqLayerNoteDecay(layer);
        }
        layer->note = Audio_AllocNote(layer);
        if (layer->note != NULL && layer->note->playbackState.parentLayer == layer) {
            Audio_NoteVibratoInit(layer->note);
        }
    }

    if (layer->note != NULL && layer->note->playbackState.parentLayer == layer) {
        Note* note = layer->note;
        Audio_NotePortamentoInit(note);
    }
    return 0;
}

s32 AudioSeq_SeqLayerProcessScriptStep2(SequenceLayer* layer) {
    SequenceChannel* channel = layer->channel;
    SeqScriptState* state = &layer->scriptState;
    SequencePlayer* seqPlayer = channel->seqPlayer;
    u16 sp3A;
    u8 cmd;

    for (;;) {
        cmd = AudioSeq_ScriptReadU8(state);
        if (cmd < 0xC1) {
            return cmd;
        }
        if (cmd >= 0xF2) {
            u16 arg = AudioSeq_GetScriptControlFlowArgument(state, cmd);

            if (AudioSeq_HandleScriptFlowControl(seqPlayer, state, cmd, arg) == 0) {
                continue;
            }
            AudioSeq_SeqLayerDisable(layer);
            return -1;
        }

        switch (cmd) {
            case 0xC1: // layer_setshortnotevelocity
            case 0xCA: // layer_setpan
            {
                u8 tempByte = *(state->pc++);

                if (cmd == 0xC1) {
                    layer->velocitySquare = (f32)(tempByte * tempByte) / 16129.0f;
                } else {
                    layer->pan = tempByte;
                }
                break;
            }

            case 0xC9: // layer_setshortnotegatetime
            case 0xC2: // layer_transpose; set transposition in semitones
            {
                u8 tempByte = *(state->pc++);

                if (cmd == 0xC9) {
                    layer->gateTime = tempByte;
                } else {
                    layer->transposition = tempByte;
                }
                break;
            }

            case 0xC4: // layer_continuousnoteson
            case 0xC5: // layer_continuousnotesoff
                if (cmd == 0xC4) {
                    layer->continuousNotes = true;
                } else {
                    layer->continuousNotes = false;
                }
                layer->bit1 = false;
                Audio_SeqLayerNoteDecay(layer);
                break;

            case 0xC3: // layer_setshortnotedefaultdelay
                sp3A = AudioSeq_ScriptReadCompressedU16(state);
                layer->shortNoteDefaultDelay = sp3A;
                break;

            case 0xC6: // layer_setinstr
                cmd = AudioSeq_ScriptReadU8(state);
                if (cmd >= 0x7E) {
                    if (cmd == 0x7E) {
                        layer->instOrWave = 1;
                    } else if (cmd == 0x7F) {
                        layer->instOrWave = 0;
                    } else {
                        layer->instOrWave = cmd;
                        layer->instrument = NULL;
                    }

                    if (cmd == 0xFF) {
                        layer->adsr.releaseRate = 0;
                    }

                    break;
                }

                if ((layer->instOrWave = AudioSeq_GetInstrument(channel, cmd, &layer->instrument, &layer->adsr)) == 0) {
                    layer->instOrWave = 0xFF;
                }
                break;

            case 0xC7: // layer_portamento
                layer->portamento.mode = AudioSeq_ScriptReadU8(state);

                cmd = AudioSeq_ScriptReadU8(state);
                cmd += channel->transposition;
                cmd += layer->transposition;
                cmd += seqPlayer->transposition;

                if (cmd >= 0x80) {
                    cmd = 0;
                }

                layer->portamentoTargetNote = cmd;

                // If special, the next param is u8 instead of var
                if (PORTAMENTO_IS_SPECIAL(layer->portamento)) {
                    layer->portamentoTime = *(state->pc++);
                    break;
                }

                sp3A = AudioSeq_ScriptReadCompressedU16(state);
                layer->portamentoTime = sp3A;
                break;

            case 0xC8: // layer_disableportamento
                layer->portamento.mode = 0;
                break;

            case 0xCB:
                sp3A = AudioSeq_ScriptReadS16(state);
                layer->adsr.envelope = (AdsrEnvelope*)(seqPlayer->seqData + sp3A);
                // fallthrough

            case 0xCF:
                layer->adsr.releaseRate = AudioSeq_ScriptReadU8(state);
                break;

            case 0xCC:
                layer->ignoreDrumPan = true;
                break;

            case 0xCD:
                layer->stereo.asByte = AudioSeq_ScriptReadU8(state);
                break;

            case 0xCE: {
                u8 tempByte = AudioSeq_ScriptReadU8(state);
                layer->unk_34 = gBendPitchTwoSemitonesFrequencies[(tempByte + 0x80) & 0xFF];
                break;
            }

            default:
                switch (cmd & 0xF0) {
                    case 0xD0: // layer_setshortnotevelocityfromtable
                        sp3A = seqPlayer->shortNoteVelocityTable[cmd & 0xF];
                        layer->velocitySquare = (f32)(sp3A * sp3A) / 16129.0f;
                        break;
                    case 0xE0: // layer_setshortnotegatetimefromtable
                        layer->gateTime = (u8)seqPlayer->shortNoteGateTimeTable[cmd & 0xF];
                        break;
                }
        }
    }
}

s32 AudioSeq_SeqLayerProcessScriptStep4(SequenceLayer* layer, s32 cmd) {
    s32 sameSound = true;
    s32 instOrWave;
    s32 speed;
    f32 temp_f14;
    f32 temp_f2;
    Portamento* portamento;
    f32 freqScale;
    f32 freqScale2;
    SoundFontSound* sound;
    Instrument* instrument;
    Drum* drum;
    s32 pad;
    SequenceChannel* channel;
    SequencePlayer* seqPlayer;
    u8 semitone = cmd;
    u16 sfxId;
    s32 semitone2;
    s32 vel;
    f32 time;
    f32 tuning;

    instOrWave = layer->instOrWave;
    channel = layer->channel;
    seqPlayer = channel->seqPlayer;

    if (instOrWave == 0xFF) {
        if (!channel->hasInstrument) {
            return -1;
        }
        instOrWave = channel->instOrWave;
    }

    switch (instOrWave) {
        case 0:
            semitone += channel->transposition + layer->transposition;
            layer->semitone = semitone;
            drum = Audio_GetDrum(channel->fontId, semitone);
            if (drum == NULL) {
                layer->stopSomething = true;
                layer->delay2 = layer->delay;
                return -1;
            }
            sound = &drum->sound;
            layer->adsr.envelope = drum->envelope;
            layer->adsr.releaseRate = (u8)drum->releaseRate;
            if (!layer->ignoreDrumPan) {
                layer->pan = drum->pan;
            }
            layer->sound = sound;
            layer->freqScale = sound->tuning;
            break;

        case 1:
            layer->semitone = semitone;
            sfxId = (layer->transposition << 6) + semitone;
            sound = Audio_GetSfx(channel->fontId, sfxId);
            if (sound == NULL) {
                layer->stopSomething = true;
                layer->delay2 = layer->delay + 1;
                return -1;
            }
            layer->sound = sound;
            layer->freqScale = sound->tuning;
            break;

        default:
            semitone += seqPlayer->transposition + channel->transposition + layer->transposition;
            semitone2 = semitone;
            layer->semitone = semitone;
            if (semitone >= 0x80) {
                layer->stopSomething = true;
                return -1;
            }
            if (layer->instOrWave == 0xFF) {
                instrument = channel->instrument;
            } else {
                instrument = layer->instrument;
            }

            if (layer->portamento.mode != 0) {
                portamento = &layer->portamento;
                vel = (semitone > layer->portamentoTargetNote) ? semitone : layer->portamentoTargetNote;

                if (instrument != NULL) {
                    sound = Audio_InstrumentGetSound(instrument, vel);
                    sameSound = (layer->sound == sound);
                    layer->sound = sound;
                    tuning = sound->tuning;
                } else {
                    layer->sound = NULL;
                    tuning = 1.0f;
                    if (instOrWave >= 0xC0) {
                        layer->sound = &gAudioContext.synthesisReverbs[instOrWave - 0xC0].sound;
                    }
                }

                temp_f2 = gNoteFrequencies[semitone2] * tuning;
                temp_f14 = gNoteFrequencies[layer->portamentoTargetNote] * tuning;

                switch (PORTAMENTO_MODE(*portamento)) {
                    case PORTAMENTO_MODE_1:
                    case PORTAMENTO_MODE_3:
                    case PORTAMENTO_MODE_5:
                        freqScale2 = temp_f2;
                        freqScale = temp_f14;
                        break;
                    case PORTAMENTO_MODE_2:
                    case PORTAMENTO_MODE_4:
                        freqScale = temp_f2;
                        freqScale2 = temp_f14;
                        break;
                    default:
                        freqScale = temp_f2;
                        freqScale2 = temp_f2;
                        break;
                }

                portamento->extent = (freqScale2 / freqScale) - 1.0f;

                if (PORTAMENTO_IS_SPECIAL(*portamento)) {
                    speed = seqPlayer->tempo * 0x8000 / gAudioContext.tempoInternalToExternal;
                    if (layer->delay != 0) {
                        speed = speed * 0x100 / (layer->delay * layer->portamentoTime);
                    }
                } else {
                    speed = 0x20000 / (layer->portamentoTime * gAudioContext.audioBufferParameters.updatesPerFrame);
                }

                if (speed >= 0x7FFF) {
                    speed = 0x7FFF;
                } else if (speed < 1) {
                    speed = 1;
                }

                portamento->speed = speed;
                portamento->cur = 0;
                layer->freqScale = freqScale;
                if (PORTAMENTO_MODE(*portamento) == PORTAMENTO_MODE_5) {
                    layer->portamentoTargetNote = semitone;
                }
                break;
            }

            if (instrument != NULL) {
                sound = Audio_InstrumentGetSound(instrument, semitone);
                sameSound = (sound == layer->sound);
                layer->sound = sound;
                layer->freqScale = gNoteFrequencies[semitone2] * sound->tuning;
            } else {
                layer->sound = NULL;
                layer->freqScale = gNoteFrequencies[semitone2];
                if (instOrWave >= 0xC0) {
                    layer->sound = &gAudioContext.synthesisReverbs[instOrWave - 0xC0].sound;
                }
            }
            break;
    }

    layer->delay2 = layer->delay;
    layer->freqScale *= layer->unk_34;
    if (layer->delay == 0) {
        if (layer->sound != NULL) {
            time = (f32)layer->sound->sample->loop->end;
        } else {
            time = 0.0f;
        }
        time *= seqPlayer->tempo;
        time *= gAudioContext.unk_2870;
        time /= layer->freqScale;
        if (time > 32766.0f) {
            time = 32766.0f;
        }
        layer->gateDelay = 0;
        layer->delay = (u16)(s32)time + 1;
        if (layer->portamento.mode != 0) {
            // (It's a bit unclear if 'portamento' has actually always been
            // set when this is reached...)
            if (PORTAMENTO_IS_SPECIAL(*portamento)) {
                s32 speed2;
                speed2 = seqPlayer->tempo * 0x8000 / gAudioContext.tempoInternalToExternal;
                speed2 = speed2 * 0x100 / (layer->delay * layer->portamentoTime);
                if (speed2 >= 0x7FFF) {
                    speed2 = 0x7FFF;
                } else if (speed2 < 1) {
                    speed2 = 1;
                }
                portamento->speed = speed2;
            }
        }
    }
    return sameSound;
}

s32 AudioSeq_SeqLayerProcessScriptStep3(SequenceLayer* layer, s32 cmd) {
    SeqScriptState* state = &layer->scriptState;
    u16 delay;
    s32 velocity;
    SequenceChannel* channel = layer->channel;
    SequencePlayer* seqPlayer = channel->seqPlayer;
    s32 intDelta;
    f32 floatDelta;

    if (cmd == 0xC0) {
        layer->delay = AudioSeq_ScriptReadCompressedU16(state);
        layer->stopSomething = true;
        layer->bit1 = false;
        return -1;
    }

    layer->stopSomething = false;
    if (channel->largeNotes == 1) {
        switch (cmd & 0xC0) {
            case 0x00:
                delay = AudioSeq_ScriptReadCompressedU16(state);
                velocity = *(state->pc++);
                layer->gateTime = *(state->pc++);
                layer->lastDelay = delay;
                break;

            case 0x40:
                delay = AudioSeq_ScriptReadCompressedU16(state);
                velocity = *(state->pc++);
                layer->gateTime = 0;
                layer->lastDelay = delay;
                break;

            case 0x80:
                delay = layer->lastDelay;
                velocity = *(state->pc++);
                layer->gateTime = *(state->pc++);
                break;
        }

        if (velocity > 0x7F || velocity < 0) {
            velocity = 0x7F;
        }
        layer->velocitySquare = (f32)velocity * (f32)velocity / 16129.0f;
        cmd -= (cmd & 0xC0);
    } else {
        switch (cmd & 0xC0) {
            case 0x00:
                delay = AudioSeq_ScriptReadCompressedU16(state);
                layer->lastDelay = delay;
                break;

            case 0x40:
                delay = layer->shortNoteDefaultDelay;
                break;

            case 0x80:
                delay = layer->lastDelay;
                break;
        }
        cmd -= (cmd & 0xC0);
    }

    if (channel->velocityRandomVariance != 0) {
        floatDelta =
            layer->velocitySquare * (f32)(gAudioContext.audioRandom % channel->velocityRandomVariance) / 100.0f;
        if ((gAudioContext.audioRandom & 0x8000) != 0) {
            floatDelta = -floatDelta;
        }
        layer->velocitySquare2 = layer->velocitySquare + floatDelta;
        if (layer->velocitySquare2 < 0.0f) {
            layer->velocitySquare2 = 0.0f;
        } else if (layer->velocitySquare2 > 1.0f) {
            layer->velocitySquare2 = 1.0f;
        }
    } else {
        layer->velocitySquare2 = layer->velocitySquare;
    }

    layer->delay = delay;
    layer->gateDelay = (layer->gateTime * delay) >> 8;
    if (channel->gateTimeRandomVariance != 0) {
        //! @bug should probably be gateTimeRandomVariance
        intDelta = (layer->gateDelay * (gAudioContext.audioRandom % channel->velocityRandomVariance)) / 100;
        if ((gAudioContext.audioRandom & 0x4000) != 0) {
            intDelta = -intDelta;
        }
        layer->gateDelay += intDelta;
        if (layer->gateDelay < 0) {
            layer->gateDelay = 0;
        } else if (layer->gateDelay > layer->delay) {
            layer->gateDelay = layer->delay;
        }
    }

    if ((seqPlayer->muted && (channel->muteBehavior & (0x40 | 0x10)) != 0) || channel->stopSomething2) {
        layer->stopSomething = true;
        return -1;
    }
    if (seqPlayer->skipTicks != 0) {
        layer->stopSomething = true;
        return -1;
    }
    return cmd;
}

void AudioSeq_SetChannelPriorities(SequenceChannel* channel, u8 arg1) {
    if ((arg1 & 0xF) != 0) {
        channel->notePriority = arg1 & 0xF;
    }
    arg1 = arg1 >> 4;
    if (arg1 != 0) {
        channel->someOtherPriority = arg1;
    }
}

u8 AudioSeq_GetInstrument(SequenceChannel* channel, u8 instId, Instrument** instOut, AdsrSettings* adsr) {
    Instrument* inst = Audio_GetInstrumentInner(channel->fontId, instId);

    if (inst == NULL) {
        *instOut = NULL;
        return 0;
    }

    if (inst->envelope != NULL)
    {
        adsr->envelope = inst->envelope;
        adsr->releaseRate = (inst->releaseRate);
    }
    else {
        adsr->envelope = gDefaultEnvelope;
    }

    *instOut = inst;
    instId += 2;
    return instId;
}

void AudioSeq_SetInstrument(SequenceChannel* channel, u8 instId) {
    if (instId >= 0x80) {
        channel->instOrWave = instId;
        channel->instrument = NULL;
    } else if (instId == 0x7F) {
        channel->instOrWave = 0;
        channel->instrument = (Instrument*)1;
    } else if (instId == 0x7E) {
        channel->instOrWave = 1;
        channel->instrument = (Instrument*)2;
    } else if ((channel->instOrWave = AudioSeq_GetInstrument(channel, instId, &channel->instrument, &channel->adsr)) ==
               0) {
        channel->hasInstrument = false;
        return;
    }
    channel->hasInstrument = true;
}

void AudioSeq_SequenceChannelSetVolume(SequenceChannel* channel, u8 volume) {
    channel->volume = (f32)(s32)volume / 127.0f;
}

void AudioSeq_SequenceChannelProcessScript(SequenceChannel* channel) {
    s32 i;
    u8* data;
    u8* test;
    SequencePlayer* seqPlayer;

    if (channel->stopScript) {
        goto exit_loop;
    }
    seqPlayer = channel->seqPlayer;
    if (seqPlayer->muted && (channel->muteBehavior & 0x80)) {
        return;
    }

    if (channel->delay >= 2) {
        channel->delay--;
        goto exit_loop;
    }

    while (true) {
        SeqScriptState* scriptState = &channel->scriptState;
        s32 param;
        s16 pad1;
        u16 offset;
        u32 parameters[3];
        s8 signedParam;
        u8 command = AudioSeq_ScriptReadU8(scriptState);
        u8 lowBits;
        u8 highBits;
        s32 result;
        s32 pad2;

        if (command >= 0xB0) {
            highBits = D_80130520[(s32)command - 0xB0];
            lowBits = highBits & 3;

            for (i = 0; i < lowBits; i++, highBits <<= 1) {
                if (!(highBits & 0x80)) {
                    parameters[i] = AudioSeq_ScriptReadU8(scriptState);
                } else {
                    parameters[i] = AudioSeq_ScriptReadS16(scriptState);
                }
            }
            if (command >= 0xF2) {
                result = AudioSeq_HandleScriptFlowControl(seqPlayer, scriptState, command, parameters[0]);

                if (result != 0) {
                    if (result == -1) {
                        AudioSeq_SequenceChannelDisable(channel);
                    } else {
                        channel->delay = result;
                    }
                    break;
                }
            } else {
                switch (command) {
                    case 0xEA:
                        channel->stopScript = true;
                        goto exit_loop;
                    case 0xF1:
                        Audio_NotePoolClear(&channel->notePool);
                        command = (u8)parameters[0];
                        Audio_NotePoolFill(&channel->notePool, command);
                        break;
                    case 0xF0:
                        Audio_NotePoolClear(&channel->notePool);
                        break;
                    case 0xC2:
                        offset = (u16)parameters[0];
                        channel->dynTable = (void*)&seqPlayer->seqData[offset];
                        break;
                    case 0xC5:
                        if (scriptState->value != -1) {

                            data = (*channel->dynTable)[scriptState->value];
                            offset = (u16)((data[0] << 8) + data[1]);

                            channel->dynTable = (void*)&seqPlayer->seqData[offset];
                        }
                        break;
                    case 0xEB:
                        result = (u8)parameters[0];
                        command = (u8)parameters[0];

                        if (seqPlayer->defaultFont != 0xFF) 
                        {
                            if (gAudioContext.seqReplaced[seqPlayer->playerIdx]) {
                                seqPlayer->seqId = gAudioContext.seqToPlay[seqPlayer->playerIdx];
                                gAudioContext.seqReplaced[seqPlayer->playerIdx] = 0;
                            }
                            u16 seqId = SfxEditor_GetReplacementSeq(seqPlayer->seqId);
                            SequenceData sDat = ResourceMgr_LoadSeqByName(sequenceMap[seqId]);
                            command = sDat.fonts[sDat.numFonts - result - 1];
                        }

                        if (AudioHeap_SearchCaches(FONT_TABLE, CACHE_EITHER, command)) 
                        {
                            channel->fontId = command;
                        }

                        parameters[0] = parameters[1];
                        // NOTE: Intentional fallthrough
                    case 0xC1:
                        command = (u8)parameters[0];
                        AudioSeq_SetInstrument(channel, command);
                        break;
                    case 0xC3:
                        channel->largeNotes = false;
                        break;
                    case 0xC4:
                        channel->largeNotes = true;
                        break;
                    case 0xDF:
                        command = (u8)parameters[0];
                        AudioSeq_SequenceChannelSetVolume(channel, command);
                        channel->changes.s.volume = true;
                        break;
                    case 0xE0:
                        command = (u8)parameters[0];
                        channel->volumeScale = (f32)(s32)command / 128.0f;
                        channel->changes.s.volume = true;
                        break;
                    case 0xDE:
                        offset = (u16)parameters[0];
                        channel->freqScale = (f32)(s32)offset / 32768.0f;
                        channel->changes.s.freqScale = true;
                        break;
                    case 0xD3:
                        command = (u8)parameters[0];
                        command += 0x80;
                        channel->freqScale = gBendPitchOneOctaveFrequencies[command];
                        channel->changes.s.freqScale = true;
                        break;
                    case 0xEE:
                        command = (u8)parameters[0];
                        command += 0x80;
                        channel->freqScale = gBendPitchTwoSemitonesFrequencies[command];
                        channel->changes.s.freqScale = true;
                        break;
                    case 0xDD:
                        command = (u8)parameters[0];
                        channel->newPan = command;
                        channel->changes.s.pan = true;
                        break;
                    case 0xDC:
                        command = (u8)parameters[0];
                        channel->panChannelWeight = command;
                        channel->changes.s.pan = true;
                        break;
                    case 0xDB:
                        signedParam = (s8)parameters[0];
                        channel->transposition = signedParam;
                        break;
                    case 0xDA:
                        offset = (u16)parameters[0];
                        channel->adsr.envelope = (AdsrEnvelope*)&seqPlayer->seqData[offset];
                        break;
                    case 0xD9:
                        command = (u8)parameters[0];
                        channel->adsr.releaseRate = command;
                        break;
                    case 0xD8:
                        command = (u8)parameters[0];
                        channel->vibratoExtentTarget = command * 8;
                        channel->vibratoExtentStart = 0;
                        channel->vibratoExtentChangeDelay = 0;
                        break;
                    case 0xD7:
                        command = (u8)parameters[0];
                        channel->vibratoRateChangeDelay = 0;
                        channel->vibratoRateTarget = command * 32;
                        channel->vibratoRateStart = command * 32;
                        break;
                    case 0xE2:
                        command = (u8)parameters[0];
                        channel->vibratoExtentStart = command * 8;
                        command = (u8)parameters[1];
                        channel->vibratoExtentTarget = command * 8;
                        command = (u8)parameters[2];
                        channel->vibratoExtentChangeDelay = command * 16;
                        break;
                    case 0xE1:
                        command = (u8)parameters[0];
                        channel->vibratoRateStart = command * 32;
                        command = (u8)parameters[1];
                        channel->vibratoRateTarget = command * 32;
                        command = (u8)parameters[2];
                        channel->vibratoRateChangeDelay = command * 16;
                        break;
                    case 0xE3:
                        command = (u8)parameters[0];
                        channel->vibratoDelay = command * 16;
                        break;
                    case 0xD4:
                        command = (u8)parameters[0];
                        channel->reverb = command;
                        break;
                    case 0xC6:
                        result = (u8)parameters[0];
                        command = (u8)parameters[0];

                        if (seqPlayer->defaultFont != 0xFF) 
                        {
                            if (gAudioContext.seqReplaced[seqPlayer->playerIdx]) {
                                seqPlayer->seqId = gAudioContext.seqToPlay[seqPlayer->playerIdx];
                                gAudioContext.seqReplaced[seqPlayer->playerIdx] = 0;
                            }
                            u16 seqId = SfxEditor_GetReplacementSeq(seqPlayer->seqId);
                            SequenceData sDat = ResourceMgr_LoadSeqByName(sequenceMap[seqId]);

                            // The game apparantely would sometimes do negative array lookups, the result of which would get rejected by AudioHeap_SearchCaches, never
                            // changing the actual fontid.
                            if (result > sDat.numFonts)
                                break;

                            command = sDat.fonts[(sDat.numFonts - result - 1)];
                        }

                        if (AudioHeap_SearchCaches(FONT_TABLE, CACHE_EITHER, command)) 
                        {
                            channel->fontId = command;
                        }

                        break;
                    case 0xC7:
                        command = (u8)parameters[0];
                        offset = (u16)parameters[1];
                        test = &seqPlayer->seqData[offset];
                        test[0] = (u8)scriptState->value + command;
                        break;
                    case 0xC8:
                    case 0xCC:
                    case 0xC9:
                        signedParam = (s8)parameters[0];

                        if (command == 0xC8) {
                            scriptState->value -= signedParam;
                        } else if (command == 0xCC) {
                            scriptState->value = signedParam;
                        } else {
                            scriptState->value &= signedParam;
                        }
                        break;
                    case 0xCD:
                        command = (u8)parameters[0];
                        AudioSeq_SequenceChannelDisable(seqPlayer->channels[command]);
                        break;
                    case 0xCA:
                        command = (u8)parameters[0];
                        channel->muteBehavior = command;
                        channel->changes.s.volume = true;
                        break;
                    case 0xCB:
                        offset = (u16)parameters[0];

                        scriptState->value = *(seqPlayer->seqData + (uintptr_t)(offset + scriptState->value));
                        break;
                    case 0xCE:
                        offset = (u16)parameters[0];
                        channel->unk_22 = offset;
                        break;
                    case 0xCF:
                        offset = (u16)parameters[0];
                        test = &seqPlayer->seqData[offset];
                        test[0] = (channel->unk_22 >> 8) & 0xFF;
                        test[1] = channel->unk_22 & 0xFF;
                        break;
                    case 0xD0:
                        command = (u8)parameters[0];
                        if (command & 0x80) {
                            channel->stereoHeadsetEffects = true;
                        } else {
                            channel->stereoHeadsetEffects = false;
                        }
                        channel->stereo.asByte = command & 0x7F;
                        break;
                    case 0xD1:
                        command = (u8)parameters[0];
                        channel->noteAllocPolicy = command;
                        break;
                    case 0xD2:
                        command = (u8)parameters[0];
                        channel->adsr.sustain = command;
                        break;
                    case 0xE5:
                        command = (u8)parameters[0];
                        channel->reverbIndex = command;
                        break;
                    case 0xE4:
                        if (scriptState->value != -1) {
                            data = (*channel->dynTable)[scriptState->value];
                            //! @bug: Missing a stack depth check here
                            scriptState->stack[scriptState->depth++] = scriptState->pc;
                            offset = (u16)((data[0] << 8) + data[1]);
                            scriptState->pc = seqPlayer->seqData + offset;
                        }
                        break;
                    case 0xE6:
                        command = (u8)parameters[0];
                        channel->bookOffset = command;
                        break;
                    case 0xE7:
                        offset = (u16)parameters[0];
                        data = &seqPlayer->seqData[offset];
                        channel->muteBehavior = data[0];
                        data += 3;
                        channel->noteAllocPolicy = data[-2];
                        AudioSeq_SetChannelPriorities(channel, data[-1]);
                        channel->transposition = (s8)data[0];
                        data += 4;
                        channel->newPan = data[-3];
                        channel->panChannelWeight = data[-2];
                        channel->reverb = data[-1];
                        channel->reverbIndex = data[0];
                        //! @bug: Not marking reverb state as changed
                        channel->changes.s.pan = true;
                        break;
                    case 0xE8:
                        channel->muteBehavior = parameters[0];
                        channel->noteAllocPolicy = parameters[1];
                        command = (u8)parameters[2];
                        AudioSeq_SetChannelPriorities(channel, command);
                        channel->transposition = (s8)AudioSeq_ScriptReadU8(scriptState);
                        channel->newPan = AudioSeq_ScriptReadU8(scriptState);
                        channel->panChannelWeight = AudioSeq_ScriptReadU8(scriptState);
                        channel->reverb = AudioSeq_ScriptReadU8(scriptState);
                        channel->reverbIndex = AudioSeq_ScriptReadU8(scriptState);
                        //! @bug: Not marking reverb state as changed
                        channel->changes.s.pan = true;
                        break;
                    case 0xEC:
                        channel->vibratoExtentTarget = 0;
                        channel->vibratoExtentStart = 0;
                        channel->vibratoExtentChangeDelay = 0;
                        channel->vibratoRateTarget = 0;
                        channel->vibratoRateStart = 0;
                        channel->vibratoRateChangeDelay = 0;
                        channel->filter = NULL;
                        channel->unk_0C = 0;
                        channel->adsr.sustain = 0;
                        channel->velocityRandomVariance = 0;
                        channel->gateTimeRandomVariance = 0;
                        channel->unk_0F = 0;
                        channel->unk_20 = 0;
                        channel->bookOffset = 0;
                        channel->freqScale = 1.0f;
                        break;
                    case 0xE9:
                        AudioSeq_SetChannelPriorities(channel, (u8)parameters[0]);
                        break;
                    case 0xED:
                        command = (u8)parameters[0];
                        channel->unk_0C = command;
                        break;
                    case 0xB0:
                        offset = (u16)parameters[0];
                        data = seqPlayer->seqData + offset;
                        channel->filter = (s16*)data;
                        break;
                    case 0xB1:
                        channel->filter = NULL;
                        break;
                    case 0xB3:
                        command = parameters[0];

                        if (channel->filter != NULL) {
                            lowBits = (command >> 4) & 0xF;
                            command &= 0xF;
                            AudioHeap_LoadFilter(channel->filter, lowBits, command);
                        }
                        break;
                    case 0xB2:
                        offset = (u16)parameters[0];
                        channel->unk_22 = BE16SWAP(*(u16*)(seqPlayer->seqData + (uintptr_t)(offset + scriptState->value * 2)));
                        break;
                    case 0xB4:
                        channel->dynTable = (void*)&seqPlayer->seqData[channel->unk_22];
                        break;
                    case 0xB5:
                        channel->unk_22 = BE16SWAP(((u16*)(channel->dynTable))[scriptState->value]);
                        break;
                    case 0xB6:
                        scriptState->value = (*channel->dynTable)[0][scriptState->value];
                        break;
                    case 0xB7:
                        channel->unk_22 = (parameters[0] == 0) ? gAudioContext.audioRandom & 0xFFFF
                                                               : gAudioContext.audioRandom % parameters[0];
                        break;
                    case 0xB8:
                        scriptState->value = (parameters[0] == 0) ? gAudioContext.audioRandom & 0xFFFF
                                                                  : gAudioContext.audioRandom % parameters[0];
                        break;
                    case 0xBD: {
                        result = Audio_NextRandom();
                        channel->unk_22 = (parameters[0] == 0) ? (u32)result & 0xFFFF : (u32)result % parameters[0];
                        channel->unk_22 += parameters[1];
                        pad2 = (channel->unk_22 / 0x100) + 0x80; // i is wrong here
                        param = channel->unk_22 % 0x100;
                        channel->unk_22 = (pad2 << 8) | param;
                    } break;
                    case 0xB9:
                        channel->velocityRandomVariance = parameters[0];
                        break;
                    case 0xBA:
                        channel->gateTimeRandomVariance = parameters[0];
                        break;
                    case 0xBB:
                        channel->unk_0F = parameters[0];
                        channel->unk_20 = parameters[1];
                        break;
                    case 0xBC:
                        channel->unk_22 += parameters[0];
                        break;
                }
            }
        } else if (command >= 0x70) {
            lowBits = command & 0x7;

            if ((command & 0xF8) != 0x70 && lowBits >= 4) {
                lowBits = 0;
            }

            switch (command & 0xF8) {
                case 0x80:
                    if (channel->layers[lowBits] != NULL) {
                        scriptState->value = channel->layers[lowBits]->finished;
                    } else {
                        scriptState->value = -1;
                    }
                    break;
                case 0x88:
                    offset = AudioSeq_ScriptReadS16(scriptState);
                    if (!AudioSeq_SeqChannelSetLayer(channel, lowBits)) {
                        channel->layers[lowBits]->scriptState.pc = &seqPlayer->seqData[offset];
                    }
                    break;
                case 0x90:
                    AudioSeq_SeqLayerFree(channel, lowBits);
                    break;
                case 0x98:
                    if (scriptState->value == -1 || AudioSeq_SeqChannelSetLayer(channel, lowBits) == -1) {
                        break;
                    }

                    data = (*channel->dynTable)[scriptState->value];
                    offset = (data[0] << 8) + data[1];
                    channel->layers[lowBits]->scriptState.pc = &seqPlayer->seqData[offset];
                    break;
                case 0x70:
                    channel->soundScriptIO[lowBits] = scriptState->value;
                    break;
                case 0x78:
                    pad1 = AudioSeq_ScriptReadS16(scriptState);
                    if (!AudioSeq_SeqChannelSetLayer(channel, lowBits)) {
                        channel->layers[lowBits]->scriptState.pc = &scriptState->pc[pad1];
                    }
                    break;
            }
        } else {
            lowBits = command & 0xF;

            switch (command & 0xF0) {
                case 0x00: {
                    channel->delay = lowBits;
                    goto exit_loop;
                }
                case 0x10:
                    if (lowBits < 8) {
                        channel->soundScriptIO[lowBits] = -1;
                        if (AudioLoad_SlowLoadSample(channel->fontId, scriptState->value,
                                                     &channel->soundScriptIO[lowBits]) == -1) {
                            break;
                        }
                    } else {
                        lowBits -= 8;
                        channel->soundScriptIO[lowBits] = -1;
                        if (AudioLoad_SlowLoadSample(channel->fontId, channel->unk_22 + 0x100,
                                                     &channel->soundScriptIO[lowBits]) == -1) {
                            break;
                        }
                    }
                    break;
                case 0x60:
                    scriptState->value = channel->soundScriptIO[lowBits];
                    if (lowBits < 2) {
                        channel->soundScriptIO[lowBits] = -1;
                    }
                    break;
                case 0x50:
                    scriptState->value -= channel->soundScriptIO[lowBits];
                    break;
                case 0x20:
                    offset = AudioSeq_ScriptReadS16(scriptState);
                    AudioSeq_SequenceChannelEnable(seqPlayer, lowBits, &seqPlayer->seqData[offset]);
                    break;
                case 0x30:
                    command = AudioSeq_ScriptReadU8(scriptState);
                    seqPlayer->channels[lowBits]->soundScriptIO[command] = scriptState->value;
                    break;
                case 0x40:
                    command = AudioSeq_ScriptReadU8(scriptState);
                    scriptState->value = seqPlayer->channels[lowBits]->soundScriptIO[command];
                    break;
            }
        }
    }
exit_loop:

    for (i = 0; i < ARRAY_COUNT(channel->layers); i++) {
        if (channel->layers[i] != NULL) {
            AudioSeq_SeqLayerProcessScript(channel->layers[i]);
        }
    }
}

void AudioSeq_SequencePlayerProcessSequence(SequencePlayer* seqPlayer) {
    u8 command;
    u8 commandLow;
    SeqScriptState* seqScript = &seqPlayer->scriptState;
    s16 tempS;
    u16 temp;
    s32 i;
    s32 value;
    u8* data;
    u8* data2;
    u8* data3;
    s32 pad3;
    s32 dummy;

    if (!seqPlayer->enabled) {
        return;
    }

    if (!AudioLoad_IsSeqLoadComplete(seqPlayer->seqId) || !AudioLoad_IsFontLoadComplete(seqPlayer->defaultFont)) {
        AudioSeq_SequencePlayerDisable(seqPlayer);
        return;
    }

    AudioLoad_SetSeqLoadStatus(seqPlayer->seqId, 2);
    AudioLoad_SetFontLoadStatus(seqPlayer->defaultFont, 2);

    if (seqPlayer->muted && (seqPlayer->muteBehavior & 0x80)) {
        return;
    }

    seqPlayer->scriptCounter++;
    seqPlayer->tempoAcc += seqPlayer->tempo;
    seqPlayer->tempoAcc += (s16)seqPlayer->unk_0C;

    if (seqPlayer->tempoAcc < gAudioContext.tempoInternalToExternal) {
        return;
    }

    seqPlayer->tempoAcc -= (u16)gAudioContext.tempoInternalToExternal;

    if (seqPlayer->stopScript == true) {
        return;
    }

    if (seqPlayer->delay > 1) {
        seqPlayer->delay--;
    } else {
        seqPlayer->recalculateVolume = true;

        while (true) {
            command = AudioSeq_ScriptReadU8(seqScript);

            // 0xF2 and above are "flow control" commands, including termination.
            if (command >= 0xF2) {
                s32 scriptHandled = AudioSeq_HandleScriptFlowControl(
                    seqPlayer, seqScript, command,
                    AudioSeq_GetScriptControlFlowArgument(&seqPlayer->scriptState, command));

                if (scriptHandled != 0) {
                    if (scriptHandled == -1) {
                        AudioSeq_SequencePlayerDisable(seqPlayer);
                    } else {
                        seqPlayer->delay = (u16)scriptHandled;
                    }
                    break;
                }
            } else if (command >= 0xC0) {
                switch (command) {
                    case 0xF1:
                        Audio_NotePoolClear(&seqPlayer->notePool);
                        command = AudioSeq_ScriptReadU8(seqScript);
                        Audio_NotePoolFill(&seqPlayer->notePool, command);
                        // Fake-match: the asm has two breaks in a row here,
                        // which the compiler normally optimizes out.
                        dummy = -1;
                        if (dummy < 0) {
                            dummy = 0;
                        }
                        if (dummy > 1) {
                            dummy = 1;
                        }
                        if (dummy) {}
                        break;
                    case 0xF0:
                        Audio_NotePoolClear(&seqPlayer->notePool);
                        break;
                    case 0xDF:
                        seqPlayer->transposition = 0;
                        // Note: intentional fallthrough, also executes below command
                    case 0xDE:
                        seqPlayer->transposition += (s8)AudioSeq_ScriptReadU8(seqScript);
                        break;
                    case 0xDD:
                        seqPlayer->tempo = AudioSeq_ScriptReadU8(seqScript) * 48;
                        if (seqPlayer->tempo > gAudioContext.tempoInternalToExternal) {
                            seqPlayer->tempo = (u16)gAudioContext.tempoInternalToExternal;
                        }
                        if ((s16)seqPlayer->tempo <= 0) {
                            seqPlayer->tempo = 1;
                        }
                        break;
                    case 0xDC:
                        seqPlayer->unk_0C = (s8)AudioSeq_ScriptReadU8(seqScript) * 48;
                        break;
                    case 0xDA:
                        command = AudioSeq_ScriptReadU8(seqScript);
                        temp = AudioSeq_ScriptReadS16(seqScript);
                        switch (command) {
                            case 0:
                            case 1:
                                if (seqPlayer->state != 2) {
                                    seqPlayer->fadeTimerUnkEu = temp;
                                    seqPlayer->state = command;
                                }
                                break;
                            case 2:
                                seqPlayer->fadeTimer = temp;
                                seqPlayer->state = command;
                                seqPlayer->fadeVelocity = (0 - seqPlayer->fadeVolume) / (s32)seqPlayer->fadeTimer;
                                break;
                        }
                        break;
                    case 0xDB:
                        value = AudioSeq_ScriptReadU8(seqScript);
                        switch (seqPlayer->state) {
                            case 1:
                                seqPlayer->state = 0;
                                seqPlayer->fadeVolume = 0.0f;
                                // NOTE: Intentional fallthrough
                            case 0:
                                seqPlayer->fadeTimer = seqPlayer->fadeTimerUnkEu;
                                if (seqPlayer->fadeTimerUnkEu != 0) {
                                    seqPlayer->fadeVelocity =
                                        ((value / 127.0f) - seqPlayer->fadeVolume) / (s32)(seqPlayer->fadeTimer);
                                } else {
                                    seqPlayer->fadeVolume = (s32)value / 127.0f;
                                }
                                break;
                            case 2:
                                break;
                        }
                        break;
                    case 0xD9:
                        seqPlayer->fadeVolumeScale = (s8)AudioSeq_ScriptReadU8(seqScript) / 127.0f;
                        break;
                    case 0xD7:
                        temp = AudioSeq_ScriptReadS16(seqScript);
                        AudioSeq_SequencePlayerSetupChannels(seqPlayer, temp);
                        break;
                    case 0xD6:
                        AudioSeq_ScriptReadS16(seqScript);
                        break;
                    case 0xD5:
                        seqPlayer->muteVolumeScale = (s8)AudioSeq_ScriptReadU8(seqScript) / 127.0f;
                        break;
                    case 0xD4:
                        seqPlayer->muted = true;
                        break;
                    case 0xD3:
                        seqPlayer->muteBehavior = AudioSeq_ScriptReadU8(seqScript);
                        break;
                    case 0xD1:
                    case 0xD2:
                        temp = AudioSeq_ScriptReadS16(seqScript);
                        data3 = &seqPlayer->seqData[temp];
                        if (command == 0xD2) {
                            seqPlayer->shortNoteVelocityTable = data3;
                        } else {
                            seqPlayer->shortNoteGateTimeTable = data3;
                        }
                        break;
                    case 0xD0:
                        seqPlayer->noteAllocPolicy = AudioSeq_ScriptReadU8(seqScript);
                        break;
                    case 0xCE:
                        command = AudioSeq_ScriptReadU8(seqScript);
                        if (command == 0) {
                            seqScript->value = (gAudioContext.audioRandom >> 2) & 0xFF;
                        } else {
                            seqScript->value = (gAudioContext.audioRandom >> 2) % command;
                        }
                        break;
                    case 0xCD: {
                        temp = AudioSeq_ScriptReadS16(seqScript);

                        if ((seqScript->value != -1) && (seqScript->depth != 3)) {
                            data = seqPlayer->seqData + (u32)(temp + (seqScript->value << 1));
                            seqScript->stack[seqScript->depth] = seqScript->pc;
                            seqScript->depth++;

                            temp = (data[0] << 8) + data[1];
                            seqScript->pc = &seqPlayer->seqData[temp];
                        }
                        break;
                    }
                    case 0xCC:
                        seqScript->value = AudioSeq_ScriptReadU8(seqScript);
                        break;
                    case 0xC9:
                        seqScript->value &= AudioSeq_ScriptReadU8(seqScript);
                        break;
                    case 0xC8:
                        seqScript->value -= AudioSeq_ScriptReadU8(seqScript);
                        break;
                    case 0xC7:
                        command = AudioSeq_ScriptReadU8(seqScript);
                        temp = AudioSeq_ScriptReadS16(seqScript);
                        data2 = &seqPlayer->seqData[temp];
                        *data2 = (u8)seqScript->value + command;
                        break;
                    case 0xC6:
                        seqPlayer->stopScript = true;
                        return;
                    case 0xC5:
                        seqPlayer->scriptCounter = (u16)AudioSeq_ScriptReadS16(seqScript);
                        break;
                    case 0xEF:
                        AudioSeq_ScriptReadS16(seqScript);
                        AudioSeq_ScriptReadU8(seqScript);
                        break;
                    case 0xC4:
                        command = AudioSeq_ScriptReadU8(seqScript);
                        if (command == 0xFF) {
                            command = seqPlayer->playerIdx;
                        }
                        commandLow = AudioSeq_ScriptReadU8(seqScript);
                        AudioLoad_SyncInitSeqPlayer(command, commandLow, 0);
                        if (command == (u8)seqPlayer->playerIdx) {
                            return;
                        }
                        break;
                }
            } else {
                commandLow = command & 0x0F;

                switch (command & 0xF0) {
                    case 0x00:
                        seqScript->value = seqPlayer->channels[commandLow]->enabled ^ 1;
                        break;
                    case 0x50:
                        seqScript->value -= seqPlayer->soundScriptIO[commandLow];
                        break;
                    case 0x70:
                        seqPlayer->soundScriptIO[commandLow] = seqScript->value;
                        break;
                    case 0x80:
                        seqScript->value = seqPlayer->soundScriptIO[commandLow];
                        if (commandLow < 2) {
                            seqPlayer->soundScriptIO[commandLow] = -1;
                        }
                        break;
                    case 0x40:
                        AudioSeq_SequenceChannelDisable(seqPlayer->channels[commandLow]);
                        break;
                    case 0x90:
                        temp = AudioSeq_ScriptReadS16(seqScript);
                        AudioSeq_SequenceChannelEnable(seqPlayer, commandLow, (void*)&seqPlayer->seqData[temp]);
                        break;
                    case 0xA0:
                        tempS = AudioSeq_ScriptReadS16(seqScript);
                        AudioSeq_SequenceChannelEnable(seqPlayer, commandLow, (void*)&seqScript->pc[tempS]);
                        break;
                    case 0xB0:
                        command = AudioSeq_ScriptReadU8(seqScript);
                        temp = AudioSeq_ScriptReadS16(seqScript);
                        data2 = &seqPlayer->seqData[temp];
                        AudioLoad_SlowLoadSeq(command, data2, &seqPlayer->soundScriptIO[commandLow]);
                        break;
                    case 0x60: {
                        command = AudioSeq_ScriptReadU8(seqScript);
                        value = command;
                        temp = AudioSeq_ScriptReadU8(seqScript);

                        AudioLoad_ScriptLoad(value, temp, &seqPlayer->soundScriptIO[commandLow]);
                        break;
                    }
                }
            }
        }
    }

    for (i = 0; i < ARRAY_COUNT(seqPlayer->channels); i++) {
        if (seqPlayer->channels[i]->enabled) {
            AudioSeq_SequenceChannelProcessScript(seqPlayer->channels[i]);
        }
    }
}

void AudioSeq_ProcessSequences(s32 arg0) {
    SequencePlayer* seqPlayer;
    u32 i;

    gAudioContext.noteSubEuOffset =
        (gAudioContext.audioBufferParameters.updatesPerFrame - arg0 - 1) * gAudioContext.numNotes;
    for (i = 0; i < (u32)gAudioContext.audioBufferParameters.numSequencePlayers; i++) {
        seqPlayer = &gAudioContext.seqPlayers[i];
        if (seqPlayer->enabled == 1) {
            AudioSeq_SequencePlayerProcessSequence(seqPlayer);
            Audio_SequencePlayerProcessSound(seqPlayer);
        }
    }
    Audio_ProcessNotes();
}

void AudioSeq_SkipForwardSequence(SequencePlayer* seqPlayer) {
    while (seqPlayer->skipTicks > 0) {
        AudioSeq_SequencePlayerProcessSequence(seqPlayer);
        Audio_SequencePlayerProcessSound(seqPlayer);
        seqPlayer->skipTicks--;
    }
}

void AudioSeq_ResetSequencePlayer(SequencePlayer* seqPlayer) {
    s32 i;

    AudioSeq_SequencePlayerDisable(seqPlayer);
    seqPlayer->stopScript = false;
    seqPlayer->delay = 0;
    seqPlayer->state = 1;
    seqPlayer->fadeTimer = 0;
    seqPlayer->fadeTimerUnkEu = 0;
    seqPlayer->tempoAcc = 0;
    seqPlayer->tempo = 120 * TATUMS_PER_BEAT; // 120 BPM
    seqPlayer->unk_0C = 0;
    seqPlayer->transposition = 0;
    seqPlayer->noteAllocPolicy = 0;
    seqPlayer->shortNoteVelocityTable = gDefaultShortNoteVelocityTable;
    seqPlayer->shortNoteGateTimeTable = gDefaultShortNoteGateTimeTable;
    seqPlayer->scriptCounter = 0;
    seqPlayer->fadeVolume = 1.0f;
    seqPlayer->fadeVelocity = 0.0f;
    seqPlayer->volume = 0.0f;
    seqPlayer->muteVolumeScale = 0.5f;

    for (i = 0; i < 0x10; i++) {
        AudioSeq_InitSequenceChannel(seqPlayer->channels[i]);
    }
}

void AudioSeq_InitSequencePlayerChannels(s32 playerIdx) {
    SequenceChannel* channel;
    SequencePlayer* seqPlayer = &gAudioContext.seqPlayers[playerIdx];
    s32 i, j;

    for (i = 0; i < 0x10; i++) {
        seqPlayer->channels[i] = AudioHeap_AllocZeroed(&gAudioContext.notesAndBuffersPool, sizeof(SequenceChannel));
        if (seqPlayer->channels[i] == NULL) {
            seqPlayer->channels[i] = &gAudioContext.sequenceChannelNone;
        } else {
            channel = seqPlayer->channels[i];
            channel->seqPlayer = seqPlayer;
            channel->enabled = false;
            for (j = 0; j < 4; j++) {
                channel->layers[j] = NULL;
            }
        }
        AudioSeq_InitSequenceChannel(seqPlayer->channels[i]);
    }
}

void AudioSeq_InitSequencePlayer(SequencePlayer* seqPlayer) {
    s32 i, j;

    for (i = 0; i < 0x10; i++) {
        seqPlayer->channels[i] = &gAudioContext.sequenceChannelNone;
    }

    seqPlayer->enabled = false;
    seqPlayer->muted = false;
    seqPlayer->fontDmaInProgress = false;
    seqPlayer->seqDmaInProgress = false;
    seqPlayer->unk_0b1 = false;

    for (j = 0; j < 8; j++) {
        seqPlayer->soundScriptIO[j] = -1;
    }
    seqPlayer->muteBehavior = 0x40 | 0x20;
    seqPlayer->fadeVolumeScale = 1.0f;
    seqPlayer->unk_34 = 1.0f;
    Audio_InitNoteLists(&seqPlayer->notePool);
    AudioSeq_ResetSequencePlayer(seqPlayer);
}

void AudioSeq_InitSequencePlayers(void) {
    s32 i;

    AudioSeq_InitLayerFreelist();
    for (i = 0; i < 64; i++) {
        gAudioContext.sequenceLayers[i].channel = NULL;
        gAudioContext.sequenceLayers[i].enabled = false;
    }

    for (i = 0; i < 4; i++) {
        AudioSeq_InitSequencePlayer(&gAudioContext.seqPlayers[i]);
    }
}
