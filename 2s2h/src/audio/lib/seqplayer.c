/**
 * @file audio_seqplayer.c
 *
 * Manages audio sequence players, interprets and executes sequence instructions used to write .seq files
 *
 * Sequence Instructions:
 *   - A customized assembly language based on MIDI
 *   - All sequences are written using these instructions
 *   - There are 3 different sets of instructions
 *        1) Sequence Instructions
 *        2) Channel Instructions
 *        3) Layer Instruction
 *   - All three sets share a common pool of control flow instructions (>= 0xF2).
 *     Otherwise, each set of instructions has its own command interpreter
 */

#include <ship/utils/binarytools/endianness.h>
#include "global.h"
#include "BenPort.h"
#include "2s2h/Enhancements/Audio/AudioEditor.h"

#define PROCESS_SCRIPT_END -1

u8 AudioScript_ScriptReadU8(SeqScriptState* state);
s16 AudioScript_ScriptReadS16(SeqScriptState* state);
u16 AudioScript_ScriptReadCompressedU16(SeqScriptState* state);
void AudioScript_SeqLayerProcessScriptStep1(SequenceLayer* layer);
s32 AudioScript_SeqLayerProcessScriptStep5(SequenceLayer* layer, s32 sameTunedSample);
s32 AudioScript_SeqLayerProcessScriptStep2(SequenceLayer* layer);
s32 AudioScript_SeqLayerProcessScriptStep4(SequenceLayer* layer, s32 cmd);
s32 AudioScript_SeqLayerProcessScriptStep3(SequenceLayer* layer, s32 cmd);
u8 AudioScript_GetInstrument(SequenceChannel* channel, u8 instId, Instrument** instOut, AdsrSettings* adsr);

SequenceData ResourceMgr_LoadSeqByName(const char* path);
extern char** gSequenceMap;

/**
 * sSeqInstructionArgsTable is a table for each sequence instruction
 * that contains both how many arguments an instruction takes, as well
 * as the type of each argument
 *
 * sSeqInstructionArgsTable is bitpacked as follows:
 * abcUUUnn
 *
 * n - number of arguments that the sequence instruction takes
 *
 * a - bitFlag for the type of arg0 if it exists
 * b - bitFlag for the type of arg1 if it exists
 * c - bitFlag for the type of arg2 if it exists
 *
 * bitFlag on - argument is s16
 * bitFlag off - argument is u8
 *
 * U - Unused
 */

// CMD_ARGS_(NUMBER_OF_ARGS)
#define CMD_ARGS_0() 0
#define CMD_ARGS_1(arg0Type) (((sizeof(arg0Type) - 1) << 7) | 1)
#define CMD_ARGS_2(arg0Type, arg1Type) (((sizeof(arg0Type) - 1) << 7) | ((sizeof(arg1Type) - 1) << 6) | 2)
#define CMD_ARGS_3(arg0Type, arg1Type, arg2Type) \
    (((sizeof(arg0Type) - 1) << 7) | ((sizeof(arg1Type) - 1) << 6) | ((sizeof(arg2Type) - 1) << 5) | 3)

u8 sSeqInstructionArgsTable[] = {
    CMD_ARGS_1(s16),        // 0xA0 (channel:)
    CMD_ARGS_0(),           // 0xA1 (channel:)
    CMD_ARGS_1(s16),        // 0xA2 (channel:)
    CMD_ARGS_0(),           // 0xA3 (channel:)
    CMD_ARGS_1(u8),         // 0xA4 (channel:)
    CMD_ARGS_0(),           // 0xA5 (channel:)
    CMD_ARGS_2(u8, s16),    // 0xA6 (channel:)
    CMD_ARGS_1(u8),         // 0xA7 (channel:)
    CMD_ARGS_2(s16, s16),   // 0xA8 (channel: random range large)
    CMD_ARGS_0(),           // 0xA9 ()
    CMD_ARGS_0(),           // 0xAA ()
    CMD_ARGS_0(),           // 0xAB ()
    CMD_ARGS_0(),           // 0xAC ()
    CMD_ARGS_0(),           // 0xAD ()
    CMD_ARGS_0(),           // 0xAE ()
    CMD_ARGS_0(),           // 0xAF ()
    CMD_ARGS_1(s16),        // 0xB0 (channel: set filter)
    CMD_ARGS_0(),           // 0xB1 (channel: clear filter)
    CMD_ARGS_1(s16),        // 0xB2 (channel: dynread sequence large)
    CMD_ARGS_1(u8),         // 0xB3 (channel: load filter)
    CMD_ARGS_0(),           // 0xB4 (channel: set dyntable large)
    CMD_ARGS_0(),           // 0xB5 (channel: read dyntable large)
    CMD_ARGS_0(),           // 0xB6 (channel: read dyntable)
    CMD_ARGS_1(s16),        // 0xB7 (channel: random large)
    CMD_ARGS_1(u8),         // 0xB8 (channel: random)
    CMD_ARGS_1(u8),         // 0xB9 (channel: set velocity random variance)
    CMD_ARGS_1(u8),         // 0xBA (channel: set gatetime random variance)
    CMD_ARGS_2(u8, s16),    // 0xBB (channel:)
    CMD_ARGS_1(s16),        // 0xBC (channel: add large)
    CMD_ARGS_1(s16),        // 0xBD (channel:)
    CMD_ARGS_1(u8),         // 0xBE (channel:)
    CMD_ARGS_0(),           // 0xBF ()
    CMD_ARGS_0(),           // 0xC0 ()
    CMD_ARGS_1(u8),         // 0xC1 (channel: set instrument)
    CMD_ARGS_1(s16),        // 0xC2 (channel: set dyntable)
    CMD_ARGS_0(),           // 0xC3 (channel: large notes off)
    CMD_ARGS_0(),           // 0xC4 (channel: large notes on)
    CMD_ARGS_0(),           // 0xC5 (channel: dyn set dyntable)
    CMD_ARGS_1(u8),         // 0xC6 (channel: set soundFont)
    CMD_ARGS_2(u8, s16),    // 0xC7 (channel: write into sequence script)
    CMD_ARGS_1(u8),         // 0xC8 (channel: subtract -> set value)
    CMD_ARGS_1(u8),         // 0xC9 (channel: `bit and` -> set value)
    CMD_ARGS_1(u8),         // 0xCA (channel: set mute behavior)
    CMD_ARGS_1(s16),        // 0xCB (channel: read sequence -> set value)
    CMD_ARGS_1(u8),         // 0xCC (channel: set value)
    CMD_ARGS_1(u8),         // 0xCD (channel: disable channel)
    CMD_ARGS_1(s16),        // 0xCE (channel:)
    CMD_ARGS_1(s16),        // 0xCF (channel: write large into sequence script)
    CMD_ARGS_1(u8),         // 0xD0 (channel: stereo headset effects)
    CMD_ARGS_1(u8),         // 0xD1 (channel: set note allocation policy)
    CMD_ARGS_1(u8),         // 0xD2 (channel: set sustain)
    CMD_ARGS_1(u8),         // 0xD3 (channel: large bend pitch)
    CMD_ARGS_1(u8),         // 0xD4 (channel: set reverb)
    CMD_ARGS_1(u8),         // 0xD5 ()
    CMD_ARGS_1(u8),         // 0xD6 ()
    CMD_ARGS_1(u8),         // 0xD7 (channel: set vibrato rate)
    CMD_ARGS_1(u8),         // 0xD8 (channel: set vibrato depth)
    CMD_ARGS_1(u8),         // 0xD9 (channel: set decay index)
    CMD_ARGS_1(s16),        // 0xDA (channel: set envelope)
    CMD_ARGS_1(u8),         // 0xDB (channel: transpose)
    CMD_ARGS_1(u8),         // 0xDC (channel: set pan mix)
    CMD_ARGS_1(u8),         // 0xDD (channel: set pan)
    CMD_ARGS_1(s16),        // 0xDE (channel: set freqscale)
    CMD_ARGS_1(u8),         // 0xDF (channel: set volume)
    CMD_ARGS_1(u8),         // 0xE0 (channel: set volume scale)
    CMD_ARGS_3(u8, u8, u8), // 0xE1 (channel: set vibratorate linear)
    CMD_ARGS_3(u8, u8, u8), // 0xE2 (channel: set vibrato depth linear)
    CMD_ARGS_1(u8),         // 0xE3 (channel: set vibrato delay)
    CMD_ARGS_0(),           // 0xE4 (channel: dyncall)
    CMD_ARGS_1(u8),         // 0xE5 (channel: set reverb index)
    CMD_ARGS_1(u8),         // 0xE6 (channel: set book offset)
    CMD_ARGS_1(s16),        // 0xE7 (channel:)
    CMD_ARGS_3(u8, u8, u8), // 0xE8 (channel:)
    CMD_ARGS_1(u8),         // 0xE9 (channel: set note priority)
    CMD_ARGS_0(),           // 0xEA (channel: stop script)
    CMD_ARGS_2(u8, u8),     // 0xEB (channel: set soundFont and instrument)
    CMD_ARGS_0(),           // 0xEC (channel: reset vibrato)
    CMD_ARGS_1(u8),         // 0xED (channel: set hilo gain)
    CMD_ARGS_1(u8),         // 0xEE (channel: small bend pitch)
    CMD_ARGS_2(s16, u8),    // 0xEF ()
    CMD_ARGS_0(),           // 0xF0 (channel: unreserve notes)
    CMD_ARGS_1(u8),         // 0xF1 (channel: reserve notes)
    // Control flow instructions (>= 0xF2) can only have 0 or 1 args
    CMD_ARGS_1(u8),  // 0xF2 (branch relative if less than zero)
    CMD_ARGS_1(u8),  // 0xF3 (branch relative if equal to zero)
    CMD_ARGS_1(u8),  // 0xF4 (jump relative)
    CMD_ARGS_1(s16), // 0xF5 (branch if greater than or equal to zero)
    CMD_ARGS_0(),    // 0xF6 (break)
    CMD_ARGS_0(),    // 0xF7 (loop end)
    CMD_ARGS_1(u8),  // 0xF8 (loop)
    CMD_ARGS_1(s16), // 0xF9 (branch if less than zero)
    CMD_ARGS_1(s16), // 0xFA (branch if equal to zero)
    CMD_ARGS_1(s16), // 0xFB (jump)
    CMD_ARGS_1(s16), // 0xFC (call and jump to a function)
    CMD_ARGS_0(),    // 0xFD (delay n frames)
    CMD_ARGS_0(),    // 0xFE (delay 1 frame)
    CMD_ARGS_0(),    // 0xFF (end script)
};

/**
 * Read and return the argument from the sequence script for a control flow instruction.
 * Control flow instructions (>= 0xF2) can only have 0 or 1 args.
 *
 * @return the argument value for a control flow instruction, or 0 if there is no argument
 */
u16 AudioScript_GetScriptControlFlowArgument(SeqScriptState* state, u8 cmd) {
    u8 highBits = sSeqInstructionArgsTable[cmd - 0xA0];
    u8 lowBits = highBits & 3;
    u16 cmdArg = 0;

    // only 1 argument
    if (lowBits == 1) {
        if (!(highBits & 0x80)) {
            cmdArg = AudioScript_ScriptReadU8(state);
        } else {
            cmdArg = AudioScript_ScriptReadS16(state);
        }
    }

    return cmdArg;
}

/**
 * Read and execute the control flow sequence instructions
 *
 * @return number of frames until next instruction. -1 signals termination
 */
s32 AudioScript_HandleScriptFlowControl(SequencePlayer* seqPlayer, SeqScriptState* state, s32 cmd, s32 cmdArg) {
    u32 depth;

    switch (cmd) {
        case 0xFF: // end script
            if (state->depth == 0) {
                return PROCESS_SCRIPT_END;
            }
            state->pc = state->stack[--state->depth];
            break;

        case 0xFD: // delay n frames
            return AudioScript_ScriptReadCompressedU16(state);

        case 0xFE: // delay 1 frame
            return 1;

        case 0xFC: // call and jump to a function
            state->stack[depth = state->depth++] = state->pc;
            state->pc = seqPlayer->seqData + (u16)cmdArg;
            break;

        case 0xF8: // loop
            state->remLoopIters[depth = state->depth] = cmdArg;
            state->stack[state->depth++] = state->pc;
            break;

        case 0xF7: // loop end
            state->remLoopIters[state->depth - 1]--;
            if (state->remLoopIters[state->depth - 1] != 0) {
                state->pc = state->stack[state->depth - 1];
            } else {
                state->depth--;
            }
            break;

        case 0xF6: // break
            state->depth--;
            break;

        case 0xF5: // branch if greater than or equal to zero
        case 0xF9: // branch if less than zero
        case 0xFA: // branch if equal to zero
        case 0xFB: // jump
            if ((cmd == 0xFA) && (state->value != 0)) {
                break;
            }
            if ((cmd == 0xF9) && (state->value >= 0)) {
                break;
            }
            if ((cmd == 0xF5) && (state->value < 0)) {
                break;
            }
            state->pc = seqPlayer->seqData + (u16)cmdArg;
            break;

        case 0xF2: // branch relative if less than zero
        case 0xF3: // branch relative if equal to zero
        case 0xF4: // jump relative
            if ((cmd == 0xF3) && (state->value != 0)) {
                break;
            }
            if ((cmd == 0xF2) && (state->value >= 0)) {
                break;
            }
            state->pc += (s8)(cmdArg & 0xFF);
            break;
    }

    return 0;
}

void AudioScript_InitSequenceChannel(SequenceChannel* channel) {
    s32 i;

    if (channel == &gAudioCtx.sequenceChannelNone) {
        return;
    }

    channel->enabled = false;
    channel->finished = false;
    channel->stopScript = false;
    channel->muted = false;
    channel->hasInstrument = false;
    channel->stereoHeadsetEffects = false;
    channel->transposition = 0;
    channel->largeNotes = false;
    channel->bookOffset = 0;
    channel->stereoData.asByte = 0;
    channel->changes.asByte = 0xFF;
    channel->scriptState.depth = 0;
    channel->newPan = 0x40;
    channel->panChannelWeight = 0x80;
    channel->surroundEffectIndex = 0xFF;
    channel->velocityRandomVariance = 0;
    channel->gateTimeRandomVariance = 0;
    channel->noteUnused = NULL;
    channel->reverbIndex = 0;
    channel->targetReverbVol = 0;
    channel->gain = 0;
    channel->notePriority = 3;
    channel->someOtherPriority = 1;
    channel->delay = 0;
    channel->adsr.envelope = gDefaultEnvelope;
    channel->adsr.decayIndex = 0xF0;
    channel->adsr.sustain = 0;
    channel->vibrato.vibratoRateTarget = 0x800;
    channel->vibrato.vibratoRateStart = 0x800;
    channel->vibrato.vibratoDepthTarget = 0;
    channel->vibrato.vibratoDepthStart = 0;
    channel->vibrato.vibratoRateChangeDelay = 0;
    channel->vibrato.vibratoDepthChangeDelay = 0;
    channel->vibrato.vibratoDelay = 0;
    channel->filter = NULL;
    channel->combFilterGain = 0;
    channel->combFilterSize = 0;
    channel->volume = 1.0f;
    channel->volumeScale = 1.0f;
    channel->freqScale = 1.0f;

    for (i = 0; i < ARRAY_COUNT(channel->seqScriptIO); i++) {
        channel->seqScriptIO[i] = SEQ_IO_VAL_NONE;
    }

    channel->unused = false;
    AudioPlayback_InitNoteLists(&channel->notePool);
    channel->startSamplePos = 0;
    channel->unk_E0 = 0;
    channel->sfxState = NULL;
}

s32 AudioScript_SeqChannelSetLayer(SequenceChannel* channel, s32 layerIndex) {
    SequenceLayer* layer;
    s32 pad;

    if (channel->layers[layerIndex] == NULL) {
        layer = AudioScript_AudioListPopBack(&gAudioCtx.layerFreeList);
        channel->layers[layerIndex] = layer;
        if (layer == NULL) {
            channel->layers[layerIndex] = NULL;
            return -1;
        }
    } else {
        AudioPlayback_SeqLayerNoteDecay(channel->layers[layerIndex]);
    }

    layer = channel->layers[layerIndex];

    layer->channel = channel;
    layer->adsr = channel->adsr;
    layer->adsr.decayIndex = 0;
    layer->targetReverbVol = channel->targetReverbVol;
    layer->enabled = true;
    layer->finished = false;
    layer->muted = false;
    layer->continuousNotes = false;
    layer->bit3 = false;
    layer->ignoreDrumPan = false;
    layer->bit1 = false;
    layer->notePropertiesNeedInit = false;
    layer->gateTime = 0x80;
    layer->surroundEffectIndex = 0x80;
    layer->stereoData.asByte = 0;
    layer->portamento.mode = PORTAMENTO_MODE_OFF;
    layer->scriptState.depth = 0;
    layer->pan = 0x40;
    layer->transposition = 0;
    layer->delay = 0;
    layer->gateDelay = 0;
    layer->delay2 = 0;
    layer->note = NULL;
    layer->instrument = NULL;
    layer->instOrWave = 0xFF;
    layer->unk_0A.asByte = 0xFFFF;
    layer->vibrato.vibratoRateTarget = 0x800;
    layer->vibrato.vibratoRateStart = 0x800;
    layer->vibrato.vibratoDepthTarget = 0;
    layer->vibrato.vibratoDepthStart = 0;
    layer->vibrato.vibratoRateChangeDelay = 0;
    layer->vibrato.vibratoDepthChangeDelay = 0;
    layer->vibrato.vibratoDelay = 0;
    layer->freqScale = 1.0f;
    layer->bend = 1.0f;
    layer->velocitySquare2 = 0.0f;

    return 0;
}

void AudioScript_SeqLayerDisable(SequenceLayer* layer) {
    if (layer != NULL) {
        if ((layer->channel != &gAudioCtx.sequenceChannelNone) && (layer->channel->seqPlayer->finished == true)) {
            AudioPlayback_SeqLayerNoteRelease(layer);
        } else {
            AudioPlayback_SeqLayerNoteDecay(layer);
        }
        layer->enabled = false;
        layer->finished = true;
    }
}

void AudioScript_SeqLayerFree(SequenceChannel* channel, s32 layerIndex) {
    SequenceLayer* layer = channel->layers[layerIndex];

    if (layer != NULL) {
        AudioScript_AudioListPushBack(&gAudioCtx.layerFreeList, &layer->listItem);
        AudioScript_SeqLayerDisable(layer);
        channel->layers[layerIndex] = NULL;
    }
}

void AudioScript_SequenceChannelDisable(SequenceChannel* channel) {
    s32 i;

    channel->finished = true;

    for (i = 0; i < 4; i++) {
        AudioScript_SeqLayerFree(channel, i);
    }

    AudioPlayback_NotePoolClear(&channel->notePool);
    channel->enabled = false;
}

void AudioScript_SequencePlayerSetupChannels(SequencePlayer* seqPlayer, u16 channelBits) {
    SequenceChannel* channel;
    s32 i;

    for (i = 0; i < SEQ_NUM_CHANNELS; i++) {
        if (channelBits & 1) {
            channel = seqPlayer->channels[i];
            channel->fontId = seqPlayer->defaultFont;
            channel->muteFlags = seqPlayer->muteFlags;
            channel->noteAllocPolicy = seqPlayer->noteAllocPolicy;
        }
        channelBits = channelBits >> 1;
    }
}

void AudioScript_SequencePlayerDisableChannels(SequencePlayer* seqPlayer, u16 channelBitsUnused) {
    SequenceChannel* channel;
    s32 i;

    for (i = 0; i < SEQ_NUM_CHANNELS; i++) {
        channel = seqPlayer->channels[i];
        if (IS_SEQUENCE_CHANNEL_VALID(channel) == 1) {
            AudioScript_SequenceChannelDisable(channel);
        }
    }
}

void AudioScript_SequenceChannelEnable(SequencePlayer* seqPlayer, u8 channelIndex, void* script) {
    SequenceChannel* channel = seqPlayer->channels[channelIndex];
    s32 i;

    channel->enabled = true;
    channel->finished = false;
    channel->scriptState.depth = 0;
    channel->scriptState.pc = script;
    channel->delay = 0;

    for (i = 0; i < ARRAY_COUNT(channel->layers); i++) {
        if (channel->layers[i] != NULL) {
            AudioScript_SeqLayerFree(channel, i);
        }
    }
}

void AudioScript_SequencePlayerDisableAsFinished(SequencePlayer* seqPlayer) {
    seqPlayer->finished = true;
    AudioScript_SequencePlayerDisable(seqPlayer);
}

void AudioScript_SequencePlayerDisable(SequencePlayer* seqPlayer) {
    AudioScript_SequencePlayerDisableChannels(seqPlayer, 0xFFFF);
    AudioPlayback_NotePoolClear(&seqPlayer->notePool);
    if (!seqPlayer->enabled) {
        return;
    }

    seqPlayer->enabled = false;
    seqPlayer->finished = true;

    if (AudioLoad_IsSeqLoadComplete(seqPlayer->seqId)) {
        AudioLoad_SetSeqLoadStatus(seqPlayer->seqId, LOAD_STATUS_DISCARDABLE);
    }

    if (AudioLoad_IsFontLoadComplete(seqPlayer->defaultFont)) {
        AudioLoad_SetFontLoadStatus(seqPlayer->defaultFont, LOAD_STATUS_MAYBE_DISCARDABLE);
    }

    if (seqPlayer->defaultFont == gAudioCtx.fontCache.temporary.entries[0].id) {
        gAudioCtx.fontCache.temporary.nextSide = 1;
    } else if (seqPlayer->defaultFont == gAudioCtx.fontCache.temporary.entries[1].id) {
        gAudioCtx.fontCache.temporary.nextSide = 0;
    }
}

void AudioScript_AudioListPushBack(AudioListItem* list, AudioListItem* item) {
    if (item->prev == NULL) {
        list->prev->next = item;
        item->prev = list->prev;
        item->next = list;
        list->prev = item;
        list->u.count++;
        item->pool = list->pool;
    }
}

void* AudioScript_AudioListPopBack(AudioListItem* list) {
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

void AudioScript_InitLayerFreelist(void) {
    s32 i;

    gAudioCtx.layerFreeList.prev = &gAudioCtx.layerFreeList;
    gAudioCtx.layerFreeList.next = &gAudioCtx.layerFreeList;
    gAudioCtx.layerFreeList.u.count = 0;
    gAudioCtx.layerFreeList.pool = NULL;

    for (i = 0; i < ARRAY_COUNT(gAudioCtx.sequenceLayers); i++) {
        gAudioCtx.sequenceLayers[i].listItem.u.value = &gAudioCtx.sequenceLayers[i];
        gAudioCtx.sequenceLayers[i].listItem.prev = NULL;
        AudioScript_AudioListPushBack(&gAudioCtx.layerFreeList, &gAudioCtx.sequenceLayers[i].listItem);
    }
}

u8 AudioScript_ScriptReadU8(SeqScriptState* state) {
    return *(state->pc++);
}

s16 AudioScript_ScriptReadS16(SeqScriptState* state) {
    s16 ret = *(state->pc++) << 8;

    ret = *(state->pc++) | ret;
    return ret;
}

u16 AudioScript_ScriptReadCompressedU16(SeqScriptState* state) {
    u16 ret = *(state->pc++);

    if (ret & 0x80) {
        ret = (ret << 8) & 0x7F00;
        ret = *(state->pc++) | ret;
    }
    return ret;
}

void AudioScript_SeqLayerProcessScript(SequenceLayer* layer) {
    s32 cmd;

    if (!layer->enabled) {
        return;
    }

    if (layer->delay > 1) {
        layer->delay--;
        if (!layer->muted && (layer->delay <= layer->gateDelay)) {
            AudioPlayback_SeqLayerNoteDecay(layer);
            layer->muted = true;
        }
        return;
    }

    AudioScript_SeqLayerProcessScriptStep1(layer);

    do {
        cmd = AudioScript_SeqLayerProcessScriptStep2(layer);
        if (cmd == PROCESS_SCRIPT_END) {
            return;
        }

        cmd = AudioScript_SeqLayerProcessScriptStep3(layer, cmd);

    } while ((cmd == -1) && (layer->delay == 0));

    if (cmd != PROCESS_SCRIPT_END) {
        // returns `sameTunedSample` instead of a command
        cmd = AudioScript_SeqLayerProcessScriptStep4(layer, cmd);
    }

    if (cmd != PROCESS_SCRIPT_END) {
        AudioScript_SeqLayerProcessScriptStep5(layer, cmd);
    }

    if (layer->muted == true) {
        if ((layer->note != NULL) || layer->continuousNotes) {
            AudioPlayback_SeqLayerNoteDecay(layer);
        }
    }
}

void AudioScript_SeqLayerProcessScriptStep1(SequenceLayer* layer) {
    if (!layer->continuousNotes) {
        AudioPlayback_SeqLayerNoteDecay(layer);
    } else if ((layer->note != NULL) && (layer->note->playbackState.wantedParentLayer == layer)) {
        AudioPlayback_SeqLayerNoteDecay(layer);
    }

    if ((PORTAMENTO_MODE(layer->portamento) == PORTAMENTO_MODE_1) ||
        (PORTAMENTO_MODE(layer->portamento) == PORTAMENTO_MODE_2)) {
        layer->portamento.mode = PORTAMENTO_MODE_OFF;
    }
    layer->notePropertiesNeedInit = true;
}

s32 AudioScript_SeqLayerProcessScriptStep5(SequenceLayer* layer, s32 sameTunedSample) {
    Note* note;

    if ((layer->continuousNotes == true) && (layer->bit1 == true)) {
        return 0;
    }

    if ((layer->continuousNotes == true) && (layer->note != NULL) && layer->bit3 && (sameTunedSample == true) &&
        (layer->note->playbackState.parentLayer == layer)) {
        if (layer->tunedSample == NULL) {
            AudioPlayback_InitSyntheticWave(layer->note, layer);
        }
    } else {
        if (!sameTunedSample) {
            AudioPlayback_SeqLayerNoteDecay(layer);
        }

        layer->note = AudioPlayback_AllocNote(layer);

        if (layer->note != NULL) {
            note = layer->note;

            if (note->playbackState.parentLayer == layer) {
                AudioEffects_InitVibrato(note);
            }
        }
    }

    if ((layer->note != NULL) && (layer->note->playbackState.parentLayer == layer)) {
        note = layer->note;

        AudioEffects_InitPortamento(note);
    }

    return 0;
}

s32 AudioScript_SeqLayerProcessScriptStep2(SequenceLayer* layer) {
    SequenceChannel* channel = layer->channel;
    SeqScriptState* state = &layer->scriptState;
    SequencePlayer* seqPlayer = channel->seqPlayer;
    u8 cmd;
    u8 cmdArg8;
    u16 cmdArg16;
    u16 velocity;

    while (true) {
        cmd = AudioScript_ScriptReadU8(state);

        // Note Commands
        // To be processed in AudioScript_SeqLayerProcessScriptStep3
        if (cmd <= 0xC0) {
            return cmd;
        }

        // Control Flow Commands
        if (cmd >= 0xF2) {
            cmdArg16 = AudioScript_GetScriptControlFlowArgument(state, cmd);

            if (AudioScript_HandleScriptFlowControl(seqPlayer, state, cmd, cmdArg16) == 0) {
                continue;
            }
            AudioScript_SeqLayerDisable(layer);
            return PROCESS_SCRIPT_END;
        }

        switch (cmd) {
            case 0xC1: // layer: set short note velocity
            case 0xCA: // layer: set pan
                cmdArg8 = *(state->pc++);
                if (cmd == 0xC1) {
                    layer->velocitySquare = SQ(cmdArg8) / SQ(127.0f);
                } else {
                    layer->pan = cmdArg8;
                }
                break;

            case 0xC9: // layer: set short note gatetime
            case 0xC2: // layer: set transposition in semitones
                cmdArg8 = *(state->pc++);
                if (cmd == 0xC9) {
                    layer->gateTime = cmdArg8;
                } else {
                    layer->transposition = cmdArg8;
                }
                break;

            case 0xC4: // layer: continuous notes on
            case 0xC5: // layer: continuous notes off
                if (cmd == 0xC4) {
                    layer->continuousNotes = true;
                } else {
                    layer->continuousNotes = false;
                }
                layer->bit1 = false;
                AudioPlayback_SeqLayerNoteDecay(layer);
                break;

            case 0xC3: // layer: set short note default delay
                cmdArg16 = AudioScript_ScriptReadCompressedU16(state);
                layer->shortNoteDefaultDelay = cmdArg16;
                break;

            case 0xC6: // layer: set instrument
                cmd = AudioScript_ScriptReadU8(state);
                if (cmd >= 0x7E) {
                    if (cmd == 0x7E) {
                        // Sfxs
                        layer->instOrWave = 1;
                    } else if (cmd == 0x7F) {
                        // Drums
                        layer->instOrWave = 0;
                    } else {
                        // Synthetic Wave
                        layer->instOrWave = cmd;
                        layer->instrument = NULL;
                    }

                    if (cmd == 0xFF) {
                        layer->adsr.decayIndex = 0;
                    }
                } else {
                    // Instrument
                    if ((layer->instOrWave =
                             AudioScript_GetInstrument(channel, cmd, &layer->instrument, &layer->adsr)) == 0) {
                        layer->instOrWave = 0xFF;
                    }
                }
                break;

            case 0xC7: // layer: enable portamento
                layer->portamento.mode = AudioScript_ScriptReadU8(state);

                cmd = AudioScript_ScriptReadU8(state);
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

                cmdArg16 = AudioScript_ScriptReadCompressedU16(state);
                layer->portamentoTime = cmdArg16;
                break;

            case 0xC8: // layer: disable portamento
                layer->portamento.mode = PORTAMENTO_MODE_OFF;
                break;

            case 0xCB: // layer: set envelope and decay index
                cmdArg16 = AudioScript_ScriptReadS16(state);
                layer->adsr.envelope = (EnvelopePoint*)(seqPlayer->seqData + cmdArg16);
                // fallthrough
            case 0xCF: // layer: set decay index
                layer->adsr.decayIndex = AudioScript_ScriptReadU8(state);
                break;

            case 0xCC: // layer: ignore drum pan
                layer->ignoreDrumPan = true;
                break;

            case 0xCD: // layer: stereo effects
                layer->stereoData.asByte = AudioScript_ScriptReadU8(state);
                break;

            case 0xCE: // layer: bend pitch
                cmdArg8 = AudioScript_ScriptReadU8(state);
                layer->bend = gBendPitchTwoSemitonesFrequencies[(u8)(cmdArg8 + 0x80)];
                break;

            case 0xF0: // layer:
                cmdArg16 = AudioScript_ScriptReadS16(state);
                layer->unk_0A.asByte &= (cmdArg16 ^ 0xFFFF);
                break;

            case 0xF1: // layer:
                layer->surroundEffectIndex = AudioScript_ScriptReadU8(state);
                break;

            default:
                switch (cmd & 0xF0) {
                    case 0xD0: // layer: set short note velocity from table
                        velocity = seqPlayer->shortNoteVelocityTable[cmd & 0xF];
                        layer->velocitySquare = SQ(velocity) / SQ(127.0f);
                        break;

                    case 0xE0: // layer: set short note gatetime from table
                        layer->gateTime = seqPlayer->shortNoteGateTimeTable[cmd & 0xF];
                        break;
                }
        }
    }
}

s32 AudioScript_SeqLayerProcessScriptStep4(SequenceLayer* layer, s32 cmd) {
    s32 sameTunedSample = true;
    s32 instOrWave;
    s32 speed;
    f32 temp_f14;
    f32 temp_f2;
    Portamento* portamento;
    f32 freqScale;
    f32 freqScale2;
    TunedSample* tunedSample;
    Instrument* instrument;
    Drum* drum;
    SoundEffect* soundEffect;
    SequenceChannel* channel;
    SequencePlayer* seqPlayer;
    u8 semitone = cmd;
    u16 sfxId;
    s32 semitone2;
    s32 velocity;
    f32 time;
    f32 tuning;
    s32 speed2;

    instOrWave = layer->instOrWave;
    channel = layer->channel;
    seqPlayer = channel->seqPlayer;

    if (instOrWave == 0xFF) {
        if (!channel->hasInstrument) {
            return PROCESS_SCRIPT_END;
        }
        instOrWave = channel->instOrWave;
    }

    switch (instOrWave) {
        case 0:
            // Drums
            semitone += channel->transposition + layer->transposition;
            layer->semitone = semitone;

            drum = AudioPlayback_GetDrum(channel->fontId, semitone);
            if (drum == NULL) {
                layer->muted = true;
                layer->delay2 = layer->delay;
                return PROCESS_SCRIPT_END;
            }

            tunedSample = &drum->tunedSample;
            layer->adsr.envelope = drum->envelope;
            layer->adsr.decayIndex = drum->adsrDecayIndex;
            if (!layer->ignoreDrumPan) {
                layer->pan = drum->pan;
            }

            layer->tunedSample = tunedSample;
            layer->freqScale = tunedSample->tuning;
            break;

        case 1:
            // Sfxs
            layer->semitone = semitone;
            sfxId = (layer->transposition << 6) + semitone;

            soundEffect = AudioPlayback_GetSoundEffect(channel->fontId, sfxId);
            if (soundEffect == NULL) {
                layer->muted = true;
                layer->delay2 = layer->delay + 1;
                return PROCESS_SCRIPT_END;
            }

            tunedSample = &soundEffect->tunedSample;
            layer->tunedSample = tunedSample;
            layer->freqScale = tunedSample->tuning;
            break;

        default:
            semitone += seqPlayer->transposition + channel->transposition + layer->transposition;
            semitone2 = semitone;

            layer->semitone = semitone;
            if (semitone >= 0x80) {
                layer->muted = true;
                return PROCESS_SCRIPT_END;
            }

            if (layer->instOrWave == 0xFF) {
                instrument = channel->instrument;
            } else {
                instrument = layer->instrument;
            }

            if (layer->portamento.mode != PORTAMENTO_MODE_OFF) {
                portamento = &layer->portamento;
                velocity = (semitone > layer->portamentoTargetNote) ? semitone : layer->portamentoTargetNote;

                if (instrument != NULL) {
                    tunedSample = AudioPlayback_GetInstrumentTunedSample(instrument, velocity);
                    sameTunedSample = (layer->tunedSample == tunedSample);
                    layer->tunedSample = tunedSample;
                    tuning = tunedSample->tuning;
                } else {
                    layer->tunedSample = NULL;
                    tuning = 1.0f;
                    if (instOrWave >= 0xC0) {
                        layer->tunedSample = &gAudioCtx.synthesisReverbs[instOrWave - 0xC0].tunedSample;
                    }
                }

                temp_f2 = gPitchFrequencies[semitone2] * tuning;
                temp_f14 = gPitchFrequencies[layer->portamentoTargetNote] * tuning;

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
                    speed = seqPlayer->tempo * 0x8000 / gAudioCtx.maxTempo;
                    if (layer->delay != 0) {
                        speed = speed * 0x100 / (layer->delay * layer->portamentoTime);
                    }
                } else {
                    speed = 0x20000 / (layer->portamentoTime * gAudioCtx.audioBufferParameters.updatesPerFrame);
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
                tunedSample = AudioPlayback_GetInstrumentTunedSample(instrument, semitone);
                sameTunedSample = (tunedSample == layer->tunedSample);
                layer->tunedSample = tunedSample;
                layer->freqScale = gPitchFrequencies[semitone2] * tunedSample->tuning;
            } else {
                layer->tunedSample = NULL;
                layer->freqScale = gPitchFrequencies[semitone2];
                if (instOrWave >= 0xC0) {
                    layer->tunedSample = &gAudioCtx.synthesisReverbs[instOrWave - 0xC0].tunedSample;
                }
            }
            break;
    }

    layer->delay2 = layer->delay;
    layer->freqScale *= layer->bend;

    if (layer->delay == 0) {
        if (layer->tunedSample != NULL) {
            time = layer->tunedSample->sample->loop->loopEnd;
        } else {
            time = 0.0f;
        }
        time *= seqPlayer->tempo;
        time *= gAudioCtx.unk_2870;
        time /= layer->freqScale;
        //! FAKE:
        if (1) {}
        if (time > 0x7FFE) {
            time = 0x7FFE;
        }

        layer->gateDelay = 0;
        layer->delay = (u16)(s32)time + 1;

        if (layer->portamento.mode != PORTAMENTO_MODE_OFF) {
            // (It's a bit unclear if 'portamento' has actually always been
            // set when this is reached...)
            if (PORTAMENTO_IS_SPECIAL(*portamento)) {
                speed2 = seqPlayer->tempo * 0x8000 / gAudioCtx.maxTempo;
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
    return sameTunedSample;
}

s32 AudioScript_SeqLayerProcessScriptStep3(SequenceLayer* layer, s32 cmd) {
    SeqScriptState* state = &layer->scriptState;
    u16 delay;
    s32 velocity;
    SequenceChannel* channel = layer->channel;
    SequencePlayer* seqPlayer = channel->seqPlayer;
    s32 intDelta;
    f32 floatDelta;

    if (cmd == 0xC0) { // layer: delay
        layer->delay = AudioScript_ScriptReadCompressedU16(state);
        layer->muted = true;
        layer->bit1 = false;
        return PROCESS_SCRIPT_END;
    }

    layer->muted = false;

    if (channel->largeNotes == true) {
        switch (cmd & 0xC0) {
            case 0x00: // layer: large note 0
                delay = AudioScript_ScriptReadCompressedU16(state);
                velocity = *(state->pc++);
                layer->gateTime = *(state->pc++);
                layer->lastDelay = delay;
                break;

            case 0x40: // layer: large note 1
                delay = AudioScript_ScriptReadCompressedU16(state);
                velocity = *(state->pc++);
                layer->gateTime = 0;
                layer->lastDelay = delay;
                break;

            case 0x80: // layer: large note 2
                delay = layer->lastDelay;
                velocity = *(state->pc++);
                layer->gateTime = *(state->pc++);
                break;
        }

        if ((velocity > 0x7F) || (velocity < 0)) {
            velocity = 0x7F;
        }
        layer->velocitySquare = SQ((f32)velocity) / SQ(127.0f);
        cmd -= (cmd & 0xC0);
    } else {
        switch (cmd & 0xC0) {
            case 0x00: // layer: small note 0
                delay = AudioScript_ScriptReadCompressedU16(state);
                layer->lastDelay = delay;
                break;

            case 0x40: // layer: small note 1
                delay = layer->shortNoteDefaultDelay;
                break;

            case 0x80: // layer: small note 2
                delay = layer->lastDelay;
                break;
        }
        cmd -= (cmd & 0xC0);
    }

    if (channel->velocityRandomVariance != 0) {
        floatDelta = layer->velocitySquare * (gAudioCtx.audioRandom % channel->velocityRandomVariance) / 100.0f;
        if ((gAudioCtx.audioRandom & 0x8000) != 0) {
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
        intDelta = (layer->gateDelay * (gAudioCtx.audioRandom % channel->velocityRandomVariance)) / 100;
        if ((gAudioCtx.audioRandom & 0x4000) != 0) {
            intDelta = -intDelta;
        }

        layer->gateDelay += intDelta;
        if (layer->gateDelay < 0) {
            layer->gateDelay = 0;
        } else if (layer->gateDelay > layer->delay) {
            layer->gateDelay = layer->delay;
        }
    }

    if ((seqPlayer->muted && (channel->muteFlags & (MUTE_FLAGS_STOP_NOTES | MUTE_FLAGS_STOP_LAYER))) ||
        channel->muted) {
        layer->muted = true;
        return PROCESS_SCRIPT_END;
    }

    if (seqPlayer->skipTicks != 0) {
        layer->muted = true;
        return PROCESS_SCRIPT_END;
    }

    return cmd;
}

void AudioScript_SetChannelPriorities(SequenceChannel* channel, u8 priority) {
    if ((priority & 0xF) != 0) {
        channel->notePriority = priority & 0xF;
    }

    priority = priority >> 4;
    if (priority != 0) {
        channel->someOtherPriority = priority;
    }
}

u8 AudioScript_GetInstrument(SequenceChannel* channel, u8 instId, Instrument** instOut, AdsrSettings* adsr) {
    Instrument* inst = AudioPlayback_GetInstrumentInner(channel->fontId, instId);

    if (inst == NULL) {
        *instOut = NULL;
        return 0;
    }

    adsr->envelope = inst->envelope;
    adsr->decayIndex = inst->adsrDecayIndex;

    *instOut = inst;

    // temporarily offset instrument id by 2 so that instId 0, 1
    // can be reserved by drums and sfxs respectively.
    instId += 2;

    return instId;
}

void AudioScript_SetInstrument(SequenceChannel* channel, u8 instId) {
    if (instId >= 0x80) {
        // Synthetic Waves
        channel->instOrWave = instId;
        channel->instrument = NULL;
    } else if (instId == 0x7F) {
        // Drums
        channel->instOrWave = 0;
        channel->instrument = (Instrument*)1; // invalid pointer, never dereferenced
    } else if (instId == 0x7E) {
        // Sfxs
        channel->instOrWave = 1;
        channel->instrument = (Instrument*)2; // invalid pointer, never dereferenced
    } else {
        // Instruments
        if ((channel->instOrWave = AudioScript_GetInstrument(channel, instId, &channel->instrument, &channel->adsr)) ==
            0) {
            channel->hasInstrument = false;
            return;
        }
    }

    channel->hasInstrument = true;
}

void AudioScript_SequenceChannelSetVolume(SequenceChannel* channel, u8 volume) {
    channel->volume = (s32)volume / 127.0f;
}

void AudioScript_SequenceChannelProcessScript(SequenceChannel* channel) {
    s32 i;
    u8* data;
    u32 rand;
    SequencePlayer* seqPlayer;

    if (channel->stopScript) {
        goto exit_loop;
    }

    seqPlayer = channel->seqPlayer;
    if (seqPlayer->muted && (channel->muteFlags & MUTE_FLAGS_STOP_SCRIPT)) {
        return;
    }

    if (channel->delay >= 2) {
        channel->delay--;
        goto exit_loop;
    }

    while (true) {
        SeqScriptState* scriptState = &channel->scriptState;
        s32 param;
        s16 temp1;
        u16 cmdArgU16;
        u32 cmdArgs[3];
        s8 cmdArgS8;
        u8 cmd = AudioScript_ScriptReadU8(scriptState);
        u8 lowBits;
        u8 highBits;
        s32 delay;
        s32 temp2;
        u8 phi_v0_3;
        u8 new_var;
        u8 depth;
        u8* seqData = seqPlayer->seqData;
        u32 new_var2;

        // Commands 0xA0 - 0xFF
        if (cmd >= 0xA0) {
            highBits = sSeqInstructionArgsTable[cmd - 0xA0];
            lowBits = highBits & 3;

            // read in arguments for the instruction
            for (i = 0; i < lowBits; i++, highBits <<= 1) {
                if (!(highBits & 0x80)) {
                    cmdArgs[i] = AudioScript_ScriptReadU8(scriptState);
                } else {
                    cmdArgs[i] = AudioScript_ScriptReadS16(scriptState);
                }
            }

            // Control Flow Commands
            if (cmd >= 0xF2) {
                delay = AudioScript_HandleScriptFlowControl(seqPlayer, scriptState, cmd, cmdArgs[0]);

                if (delay != 0) {
                    if (delay == PROCESS_SCRIPT_END) {
                        AudioScript_SequenceChannelDisable(channel);
                    } else {
                        channel->delay = delay;
                    }
                    break;
                }
                continue;
            }

            switch (cmd) {
                case 0xEA: // channel: stop script
                    channel->stopScript = true;
                    goto exit_loop;

                case 0xF1: // channel: reserve notes
                    AudioPlayback_NotePoolClear(&channel->notePool);
                    cmd = (u8)cmdArgs[0];
                    AudioPlayback_NotePoolFill(&channel->notePool, cmd);
                    break;

                case 0xF0: // channel: unreserve notes
                    AudioPlayback_NotePoolClear(&channel->notePool);
                    break;

                case 0xC2: // channel: set dyntable
                    cmdArgU16 = (u16)cmdArgs[0];
                    channel->dynTable = (void*)&seqPlayer->seqData[cmdArgU16];
                    break;

                case 0xC5: // channel: dyn set dyntable
                    if (scriptState->value != -1) {
                        data = (*channel->dynTable)[scriptState->value];
                        cmdArgU16 = (u16)((data[0] << 8) + data[1]);
                        scriptState->pc = (void*)&seqPlayer->seqData[cmdArgU16];
                    }
                    break;

                case 0xEB: { // channel: set soundFont and instrument
                    // #region 2S2H [Port] Custom sequences
                    uint16_t result = (uint16_t)cmdArgs[0];

                    if (seqPlayer->defaultFont != 0xFFFF) {
                        if (gAudioCtx.seqReplaced[seqPlayer->playerIndex]) {
                            seqPlayer->seqId = gAudioCtx.seqToPlay[seqPlayer->playerIndex];
                            gAudioCtx.seqReplaced[seqPlayer->playerIndex] = 0;
                        }
                        u16 seqId = AudioEditor_GetReplacementSeq(seqPlayer->seqId);
                        SequenceData sDat = ResourceMgr_LoadSeqByName(gSequenceMap[seqId]);
                        result = sDat.fonts[sDat.numFonts - result - 1];
                    }
                    // #end region
                    if (AudioHeap_SearchCaches(FONT_TABLE, CACHE_EITHER, result)) {
                        channel->fontId = result;
                    }

                    cmdArgs[0] = cmdArgs[1];
                }
                    // fallthrough
                case 0xC1: // channel: set instrument
                    cmd = (u8)cmdArgs[0];
                    AudioScript_SetInstrument(channel, cmd);
                    break;

                case 0xC3: // channel: large notes off
                    channel->largeNotes = false;
                    break;

                case 0xC4: // channel: large notes on
                    channel->largeNotes = true;
                    break;

                case 0xDF: // channel: set volume
                    cmd = (u8)cmdArgs[0];
                    AudioScript_SequenceChannelSetVolume(channel, cmd);
                    channel->changes.s.volume = true;
                    break;

                case 0xE0: // channel: set volume scale
                    cmd = (u8)cmdArgs[0];
                    channel->volumeScale = (f32)(s32)cmd / 128.0f;
                    channel->changes.s.volume = true;
                    break;

                case 0xDE: // channel: set freqscale
                    cmdArgU16 = (u16)cmdArgs[0];
                    channel->freqScale = (f32)(s32)cmdArgU16 / 0x8000;
                    channel->changes.s.freqScale = true;
                    break;

                case 0xD3: // channel: large bend pitch
                    cmd = (u8)cmdArgs[0];
                    cmd += 0x80;
                    channel->freqScale = gBendPitchOneOctaveFrequencies[cmd];
                    channel->changes.s.freqScale = true;
                    break;

                case 0xEE: // channel: small bend pitch
                    cmd = (u8)cmdArgs[0];
                    cmd += 0x80;
                    channel->freqScale = gBendPitchTwoSemitonesFrequencies[cmd];
                    channel->changes.s.freqScale = true;
                    break;

                case 0xDD: // channel: set pan
                    cmd = (u8)cmdArgs[0];
                    channel->newPan = cmd;
                    channel->changes.s.pan = true;
                    break;

                case 0xDC: // channel: set pan mix
                    cmd = (u8)cmdArgs[0];
                    channel->panChannelWeight = cmd;
                    channel->changes.s.pan = true;
                    break;

                case 0xDB: // channel: transpose
                    cmdArgS8 = (s8)cmdArgs[0];
                    channel->transposition = cmdArgS8;
                    break;

                case 0xDA: // channel: set envelope
                    cmdArgU16 = (u16)cmdArgs[0];
                    channel->adsr.envelope = (EnvelopePoint*)&seqPlayer->seqData[cmdArgU16];
                    break;

                case 0xD9: // channel: set decay index
                    cmd = (u8)cmdArgs[0];
                    channel->adsr.decayIndex = cmd;
                    break;

                case 0xD8: // channel: set vibrato depth
                    cmd = (u8)cmdArgs[0];
                    channel->vibrato.vibratoDepthTarget = cmd * 8;
                    channel->vibrato.vibratoDepthStart = 0;
                    channel->vibrato.vibratoDepthChangeDelay = 0;
                    break;

                case 0xD7: // channel: set vibrato rate
                    cmd = (u8)cmdArgs[0];
                    channel->vibrato.vibratoRateChangeDelay = 0;
                    channel->vibrato.vibratoRateTarget = cmd * 32;
                    channel->vibrato.vibratoRateStart = cmd * 32;
                    break;

                case 0xE2: // channel: set vibrato depth linear
                    cmd = (u8)cmdArgs[0];
                    channel->vibrato.vibratoDepthStart = cmd * 8;
                    cmd = (u8)cmdArgs[1];
                    channel->vibrato.vibratoDepthTarget = cmd * 8;
                    cmd = (u8)cmdArgs[2];
                    channel->vibrato.vibratoDepthChangeDelay = cmd * 16;
                    break;

                case 0xE1: // channel: set vibratorate linear
                    cmd = (u8)cmdArgs[0];
                    channel->vibrato.vibratoRateStart = cmd * 32;
                    cmd = (u8)cmdArgs[1];
                    channel->vibrato.vibratoRateTarget = cmd * 32;
                    cmd = (u8)cmdArgs[2];
                    channel->vibrato.vibratoRateChangeDelay = cmd * 16;
                    break;

                case 0xE3: // channel: set vibrato delay
                    cmd = (u8)cmdArgs[0];
                    channel->vibrato.vibratoDelay = cmd * 16;
                    break;

                case 0xD4: // channel: set reverb volume
                    cmd = (u8)cmdArgs[0];
                    channel->targetReverbVol = cmd;
                    break;

                case 0xC6: { // channel: set soundFont
                    // #region 2S2H [Port] Audio assets in the archive and custom sequences
                    uint16_t result = (uint16_t)cmdArgs[0];
                    if (seqPlayer->defaultFont != 0xFFFF) {
                        if (gAudioCtx.seqReplaced[seqPlayer->playerIndex]) {
                            seqPlayer->seqId = gAudioCtx.seqToPlay[seqPlayer->playerIndex];
                            gAudioCtx.seqReplaced[seqPlayer->playerIndex] = 0;
                        }
                        u16 seqId = AudioEditor_GetReplacementSeq(seqPlayer->seqId);
                        SequenceData sDat = ResourceMgr_LoadSeqByName(gSequenceMap[seqId]);

                        // The game apparantely would sometimes do negative array lookups, the result of which would get
                        // rejected by AudioHeap_SearchCaches, never changing the actual fontid.
                        if (result > sDat.numFonts)
                            break;
                        // #end region
                        result = sDat.fonts[(sDat.numFonts - result - 1)];
                    }

                    if (AudioHeap_SearchCaches(FONT_TABLE, CACHE_EITHER, result)) {
                        channel->fontId = result;
                    }
                    break;
                }
                case 0xC7: // channel: write into sequence script
                    cmd = (u8)cmdArgs[0];
                    cmdArgU16 = (u16)cmdArgs[1];
                    seqData = &seqPlayer->seqData[cmdArgU16];
                    seqData[0] = (u8)scriptState->value + cmd;
                    break;

                case 0xC8: // channel: subtract -> set value
                case 0xCC: // channel: set value
                case 0xC9: // channel: `bit and` -> set value
                    cmdArgS8 = (s8)cmdArgs[0];

                    if (cmd == 0xC8) {
                        scriptState->value -= cmdArgS8;
                    } else if (cmd == 0xCC) {
                        scriptState->value = cmdArgS8;
                    } else {
                        scriptState->value &= cmdArgS8;
                    }
                    break;

                case 0xCD: // channel: disable channel
                    cmd = (u8)cmdArgs[0];
                    AudioScript_SequenceChannelDisable(seqPlayer->channels[cmd]);
                    break;

                case 0xCA: // channel: set mute behavior
                    cmd = (u8)cmdArgs[0];
                    channel->muteFlags = cmd;
                    channel->changes.s.volume = true;
                    break;

                case 0xCB: // channel: read sequence -> set value
                    cmdArgU16 = (u16)cmdArgs[0];
                    scriptState->value = *(seqPlayer->seqData + (u32)(cmdArgU16 + scriptState->value));
                    break;

                case 0xCE: // channel:
                    cmdArgU16 = (u16)cmdArgs[0];
                    channel->unk_22 = cmdArgU16;
                    break;

                case 0xCF: // channel: write large into sequence script
                    cmdArgU16 = (u16)cmdArgs[0];
                    seqData = &seqPlayer->seqData[cmdArgU16];
                    seqData[0] = (channel->unk_22 >> 8) & 0xFF;
                    seqData[1] = channel->unk_22 & 0xFF;
                    break;

                case 0xD0: // channel: stereo headset effects
                    cmd = (u8)cmdArgs[0];
                    if (cmd & 0x80) {
                        channel->stereoHeadsetEffects = true;
                    } else {
                        channel->stereoHeadsetEffects = false;
                    }
                    channel->stereoData.asByte = cmd & 0x7F;
                    break;

                case 0xD1: // channel: set note allocation policy
                    cmd = (u8)cmdArgs[0];
                    channel->noteAllocPolicy = cmd;
                    break;

                case 0xD2: // channel: set sustain
                    cmd = (u8)cmdArgs[0];
                    channel->adsr.sustain = cmd;
                    break;

                case 0xE5: // channel: set reverb index
                    cmd = (u8)cmdArgs[0];
                    channel->reverbIndex = cmd;
                    break;

                case 0xE4: // channel: dyncall
                    if (scriptState->value != -1) {
                        data = (*channel->dynTable)[scriptState->value];
                        depth = scriptState->depth;
                        //! @bug: Missing a stack depth check here
                        scriptState->stack[depth] = scriptState->pc;
                        scriptState->depth++;
                        cmdArgU16 = (u16)((data[0] << 8) + data[1]);
                        scriptState->pc = seqPlayer->seqData + cmdArgU16;
                    }
                    break;

                case 0xE6: // channel: set book offset
                    cmd = (u8)cmdArgs[0];
                    channel->bookOffset = cmd;
                    break;

                case 0xE7: // channel:
                    cmdArgU16 = (u16)cmdArgs[0];
                    data = &seqPlayer->seqData[cmdArgU16];
                    channel->muteFlags = *data++;
                    channel->noteAllocPolicy = *data++;
                    AudioScript_SetChannelPriorities(channel, *data++);
                    channel->transposition = (s8)*data++;
                    channel->newPan = *data++;
                    channel->panChannelWeight = *data++;
                    channel->targetReverbVol = *data++;
                    channel->reverbIndex = *data++;
                    //! @bug: Not marking reverb state as changed
                    channel->changes.s.pan = true;
                    break;

                case 0xE8: // channel:
                    channel->muteFlags = cmdArgs[0];
                    channel->noteAllocPolicy = cmdArgs[1];
                    cmd = (u8)cmdArgs[2];
                    AudioScript_SetChannelPriorities(channel, cmd);
                    channel->transposition = (s8)AudioScript_ScriptReadU8(scriptState);
                    channel->newPan = AudioScript_ScriptReadU8(scriptState);
                    channel->panChannelWeight = AudioScript_ScriptReadU8(scriptState);
                    channel->targetReverbVol = AudioScript_ScriptReadU8(scriptState);
                    channel->reverbIndex = AudioScript_ScriptReadU8(scriptState);
                    //! @bug: Not marking reverb state as changed
                    channel->changes.s.pan = true;
                    break;

                case 0xEC: // channel: reset vibrato
                    channel->vibrato.vibratoDepthTarget = 0;
                    channel->vibrato.vibratoDepthStart = 0;
                    channel->vibrato.vibratoDepthChangeDelay = 0;
                    channel->vibrato.vibratoRateTarget = 0;
                    channel->vibrato.vibratoRateStart = 0;
                    channel->vibrato.vibratoRateChangeDelay = 0;
                    channel->filter = NULL;
                    channel->gain = 0;
                    channel->adsr.sustain = 0;
                    channel->velocityRandomVariance = 0;
                    channel->gateTimeRandomVariance = 0;
                    channel->combFilterSize = 0;
                    channel->combFilterGain = 0;
                    channel->bookOffset = 0;
                    channel->startSamplePos = 0;
                    channel->unk_E0 = 0;
                    channel->freqScale = 1.0f;
                    break;

                case 0xE9: // channel: set note priority
                    AudioScript_SetChannelPriorities(channel, (u8)cmdArgs[0]);
                    break;

                case 0xED: // channel: set hilo gain
                    cmd = (u8)cmdArgs[0];
                    channel->gain = cmd;
                    break;

                case 0xB0: // channel: set filter
                    cmdArgU16 = (u16)cmdArgs[0];
                    data = seqPlayer->seqData + cmdArgU16;
                    channel->filter = (s16*)data;
                    break;

                case 0xB1: // channel: clear filter
                    channel->filter = NULL;
                    break;

                case 0xB3: // channel: load filter
                    cmd = cmdArgs[0];

                    if (channel->filter != NULL) {
                        lowBits = (cmd >> 4) & 0xF; // LowPassCutoff
                        cmd &= 0xF;                 // HighPassCutoff
                        AudioHeap_LoadFilter(channel->filter, lowBits, cmd);
                    }
                    break;

                case 0xB2: // channel: dynread sequence large
                    cmdArgU16 = (u16)cmdArgs[0];
                    channel->unk_22 = BE16SWAP(*(u16*)(seqPlayer->seqData + (u32)(cmdArgU16 + scriptState->value * 2)));
                    break;

                case 0xB4: // channel: set dyntable large
                    channel->dynTable = (void*)&seqPlayer->seqData[channel->unk_22];
                    break;

                case 0xB5: // channel: read dyntable large
                    channel->unk_22 = BE16SWAP(((u16*)(channel->dynTable))[scriptState->value]);
                    break;

                case 0xB6: // channel: read dyntable
                    scriptState->value = (*channel->dynTable)[0][scriptState->value];
                    break;

                case 0xB7: // channel: random large
                    channel->unk_22 =
                        (cmdArgs[0] == 0) ? (gAudioCtx.audioRandom & 0xFFFF) : (gAudioCtx.audioRandom % cmdArgs[0]);
                    break;

                case 0xB8: // channel: random value
                    scriptState->value =
                        (cmdArgs[0] == 0) ? (gAudioCtx.audioRandom & 0xFFFF) : (gAudioCtx.audioRandom % cmdArgs[0]);
                    break;

                case 0xA8: // channel: random range large (only cmd that differs from OoT)
                    rand = AudioThread_NextRandom();
                    channel->unk_22 = (cmdArgs[0] == 0) ? (rand & 0xFFFF) : (rand % cmdArgs[0]);
                    channel->unk_22 += cmdArgs[1];
                    temp2 = (channel->unk_22 / 0x100) + 0x80;
                    param = channel->unk_22 % 0x100;
                    channel->unk_22 = (temp2 << 8) | param;
                    break;

                case 0xB9: // channel: set velocity random variance
                    channel->velocityRandomVariance = cmdArgs[0];
                    break;

                case 0xBA: // channel: set gatetime random variance
                    channel->gateTimeRandomVariance = cmdArgs[0];
                    break;

                case 0xBB: // channel:
                    channel->combFilterSize = cmdArgs[0];
                    channel->combFilterGain = cmdArgs[1];
                    break;

                case 0xBC: // channel: add large
                    channel->unk_22 += cmdArgs[0];
                    break;

                case 0xBD: // channel:
                    channel->startSamplePos = cmdArgs[0];
                    break;

                case 0xBE: // channel:
                    if (cmdArgs[0] < 5) {
                        if (1) {}
                        if (gAudioCtx.customSeqFunctions[cmdArgs[0]] != NULL) {
                            gAudioCustomSeqFunction = gAudioCtx.customSeqFunctions[cmdArgs[0]];
                            scriptState->value = gAudioCustomSeqFunction(scriptState->value, channel);
                        }
                    }
                    break;

                case 0xA0: // channel: read from SfxChannelState using arg
                case 0xA1: // channel: read from SfxChannelState using unk_22
                case 0xA2: // channel: write to SfxChannelState using arg
                case 0xA3: // channel: write to SfxChannelState using unk_22
                    if ((cmd == 0xA0) || (cmd == 0xA2)) {
                        cmdArgU16 = (u16)cmdArgs[0];
                    } else {
                        cmdArgU16 = channel->unk_22;
                    }

                    if (channel->sfxState != NULL) {
                        if ((cmd == 0xA0) || (cmd == 0xA1)) {
                            scriptState->value = channel->sfxState[cmdArgU16];
                        } else {
                            channel->sfxState[cmdArgU16] = scriptState->value;
                        }
                    }
                    break;

                case 0xA4: // channel:
                    channel->surroundEffectIndex = cmdArgs[0];
                    break;

                case 0xA5: // channel:
                    scriptState->value += channel->channelIndex;
                    break;

                case 0xA6: // channel:
                    cmd = (u8)cmdArgs[0];
                    cmdArgU16 = (u16)cmdArgs[1];
                    seqData = seqPlayer->seqData + (u32)(cmdArgU16 + channel->channelIndex);
                    seqData[0] = (u8)scriptState->value + cmd;
                    break;

                case 0xA7: // channel:
                    new_var2 = (cmdArgs[0] & 0x80);
                    new_var = (scriptState->value & 0x80);

                    if (!new_var2) {
                        phi_v0_3 = scriptState->value << (cmdArgs[0] & 0xF);
                    } else {
                        phi_v0_3 = scriptState->value >> (cmdArgs[0] & 0xF);
                    }

                    if (cmdArgs[0] & 0x40) {
                        phi_v0_3 &= (u8)~0x80;
                        phi_v0_3 |= new_var;
                    }

                    scriptState->value = phi_v0_3;
                    break;
            }
            continue;
        }

        // Commands 0x70 - 0x9F
        if (cmd >= 0x70) {
            lowBits = cmd & 0x7;

            if (((cmd & 0xF8) != 0x70) && (lowBits >= 4)) {
                lowBits = 0;
            }

            switch (cmd & 0xF8) {
                case 0x80: // channel: test layer is finished
                    if (channel->layers[lowBits] != NULL) {
                        scriptState->value = channel->layers[lowBits]->finished;
                    } else {
                        scriptState->value = -1;
                    }
                    break;

                case 0x88: // channel: set layer
                    cmdArgU16 = AudioScript_ScriptReadS16(scriptState);
                    if (!AudioScript_SeqChannelSetLayer(channel, lowBits)) {
                        channel->layers[lowBits]->scriptState.pc = &seqPlayer->seqData[cmdArgU16];
                    }
                    break;

                case 0x90: // channel: free layer
                    AudioScript_SeqLayerFree(channel, lowBits);
                    break;

                case 0x98: // channel: dynset layer
                    if ((scriptState->value != -1) && (AudioScript_SeqChannelSetLayer(channel, lowBits) != -1)) {
                        data = (*channel->dynTable)[scriptState->value];
                        cmdArgU16 = (data[0] << 8) + data[1];
                        channel->layers[lowBits]->scriptState.pc = &seqPlayer->seqData[cmdArgU16];
                    }
                    break;

                case 0x70: // channel: io write value
                    channel->seqScriptIO[lowBits] = scriptState->value;
                    break;

                case 0x78: // channel: set layer relative
                    temp1 = AudioScript_ScriptReadS16(scriptState);
                    if (!AudioScript_SeqChannelSetLayer(channel, lowBits)) {
                        channel->layers[lowBits]->scriptState.pc = &scriptState->pc[temp1];
                    }
                    break;
            }
            continue;
        }

        // Commands 0x00 - 0x6F
        lowBits = cmd & 0xF;

        switch (cmd & 0xF0) {
            case 0x00: // channel: delay short
                channel->delay = lowBits;
                if (lowBits == 0) {
                    break;
                }
                goto exit_loop;

            case 0x10: // channel: load sample
                if (lowBits < 8) {
                    channel->seqScriptIO[lowBits] = SEQ_IO_VAL_NONE;
                    if (AudioLoad_SlowLoadSample(channel->fontId, scriptState->value, &channel->seqScriptIO[lowBits]) ==
                        -1) {}
                } else {
                    lowBits -= 8;
                    channel->seqScriptIO[lowBits] = SEQ_IO_VAL_NONE;
                    if (AudioLoad_SlowLoadSample(channel->fontId, channel->unk_22 + 0x100,
                                                 &channel->seqScriptIO[lowBits]) == -1) {}
                }
                break;

            case 0x60: // channel: io read value
                scriptState->value = channel->seqScriptIO[lowBits];
                if (lowBits < 2) {
                    channel->seqScriptIO[lowBits] = SEQ_IO_VAL_NONE;
                }
                break;

            case 0x50: // channel: io read value subtract
                scriptState->value -= channel->seqScriptIO[lowBits];
                break;

            case 0x20: // channel: start channel
                cmdArgU16 = AudioScript_ScriptReadS16(scriptState);
                AudioScript_SequenceChannelEnable(seqPlayer, lowBits, &seqPlayer->seqData[cmdArgU16]);
                break;

            case 0x30: // channel: io write value 2
                cmd = AudioScript_ScriptReadU8(scriptState);
                seqPlayer->channels[lowBits]->seqScriptIO[cmd] = scriptState->value;
                break;

            case 0x40: // channel: io read value 2
                cmd = AudioScript_ScriptReadU8(scriptState);
                scriptState->value = seqPlayer->channels[lowBits]->seqScriptIO[cmd];
                break;
        }
    }
exit_loop:

    for (i = 0; i < ARRAY_COUNT(channel->layers); i++) {
        if (channel->layers[i] != NULL) {
            AudioScript_SeqLayerProcessScript(channel->layers[i]);
        }
    }
}

void AudioScript_SequencePlayerProcessSequence(SequencePlayer* seqPlayer) {
    u8 cmd;
    u8 cmdLowBits;
    SeqScriptState* seqScript = &seqPlayer->scriptState;
    s16 tempS;
    u16 temp;
    s32 i;
    s32 value;
    u8* data1;
    u8* data2;
    u8* data3;
    u8* data4;
    s32 tempoChange;
    s32 j;
    SequenceChannel* channel;
    u16* new_var;
    s32 delay;

    if (!seqPlayer->enabled) {
        return;
    }

    if (!AudioLoad_IsSeqLoadComplete(seqPlayer->seqId) || !AudioLoad_IsFontLoadComplete(seqPlayer->defaultFont)) {
        // These function calls serve no purpose
        if (AudioLoad_IsSeqLoadComplete(seqPlayer->seqId)) {}
        if (AudioLoad_IsSeqLoadComplete(seqPlayer->defaultFont)) {}

        AudioScript_SequencePlayerDisable(seqPlayer);
        return;
    }

    AudioLoad_SetSeqLoadStatus(seqPlayer->seqId, LOAD_STATUS_COMPLETE);
    AudioLoad_SetFontLoadStatus(seqPlayer->defaultFont, LOAD_STATUS_COMPLETE);

    if (seqPlayer->muted && (seqPlayer->muteFlags & MUTE_FLAGS_STOP_SCRIPT)) {
        return;
    }

    seqPlayer->scriptCounter++;

    tempoChange = seqPlayer->tempo + seqPlayer->tempoChange;
    if (tempoChange > gAudioCtx.maxTempo) {
        tempoChange = gAudioCtx.maxTempo;
    }

    seqPlayer->tempoAcc += tempoChange;

    if (seqPlayer->tempoAcc < gAudioCtx.maxTempo) {
        return;
    }

    seqPlayer->tempoAcc -= (u16)gAudioCtx.maxTempo;
    seqPlayer->unk_16++;

    if (seqPlayer->stopScript == true) {
        return;
    }

    if (seqPlayer->delay > 1) {
        seqPlayer->delay--;
    } else {
        seqPlayer->recalculateVolume = true;

        while (true) {
            cmd = AudioScript_ScriptReadU8(seqScript);

            // 0xF2 and above are "flow control" commands, including termination.
            if (cmd >= 0xF2) {
                delay = AudioScript_HandleScriptFlowControl(
                    seqPlayer, seqScript, cmd, AudioScript_GetScriptControlFlowArgument(&seqPlayer->scriptState, cmd));

                if (delay != 0) {
                    if (delay == -1) {
                        AudioScript_SequencePlayerDisable(seqPlayer);
                    } else {
                        seqPlayer->delay = delay;
                    }
                    break;
                }
                continue;
            }

            // Commands 0xC0 - 0xF1
            if (cmd >= 0xC0) {
                switch (cmd) {
                    case 0xF1: // seqPlayer: reserve notes
                        AudioPlayback_NotePoolClear(&seqPlayer->notePool);
                        cmd = AudioScript_ScriptReadU8(seqScript);
                        AudioPlayback_NotePoolFill(&seqPlayer->notePool, cmd);
                        break;

                    case 0xF0: // seqPlayer: unreserve notes
                        AudioPlayback_NotePoolClear(&seqPlayer->notePool);
                        break;

                    case 0xDF: // seqPlayer: transpose
                        seqPlayer->transposition = 0;
                        // fallthrough
                    case 0xDE: // seqPlayer: transpose relative
                        seqPlayer->transposition += (s8)AudioScript_ScriptReadU8(seqScript);
                        break;

                    case 0xDD: // seqPlayer: set tempo
                        seqPlayer->tempo = AudioScript_ScriptReadU8(seqScript) * TATUMS_PER_BEAT;
                        if (seqPlayer->tempo > gAudioCtx.maxTempo) {
                            seqPlayer->tempo = gAudioCtx.maxTempo;
                        }

                        if ((s16)seqPlayer->tempo <= 0) {
                            seqPlayer->tempo = 1;
                        }
                        break;

                    case 0xDC: // seqPlayer: add tempo
                        seqPlayer->tempoChange = (s8)AudioScript_ScriptReadU8(seqScript) * TATUMS_PER_BEAT;
                        break;

                    case 0xDA: // seqPlayer: change volume
                        cmd = AudioScript_ScriptReadU8(seqScript);
                        temp = AudioScript_ScriptReadS16(seqScript);
                        switch (cmd) {
                            case SEQPLAYER_STATE_0:
                            case SEQPLAYER_STATE_FADE_IN:
                                if (seqPlayer->state != SEQPLAYER_STATE_FADE_OUT) {
                                    seqPlayer->storedFadeTimer = temp;
                                    seqPlayer->state = cmd;
                                }
                                break;

                            case SEQPLAYER_STATE_FADE_OUT:
                                seqPlayer->fadeTimer = temp;
                                seqPlayer->state = cmd;
                                seqPlayer->fadeVelocity = (0.0f - seqPlayer->fadeVolume) / (s32)seqPlayer->fadeTimer;
                                break;
                        }
                        break;

                    case 0xDB: // seqPlayer: set volume
                        value = AudioScript_ScriptReadU8(seqScript);
                        switch (seqPlayer->state) {
                            case SEQPLAYER_STATE_FADE_IN:
                                seqPlayer->state = SEQPLAYER_STATE_0;
                                seqPlayer->fadeVolume = 0.0f;
                                // fallthrough
                            case SEQPLAYER_STATE_0:
                                seqPlayer->fadeTimer = seqPlayer->storedFadeTimer;
                                if (seqPlayer->storedFadeTimer != 0) {
                                    seqPlayer->fadeVelocity =
                                        ((value / 127.0f) - seqPlayer->fadeVolume) / (s32)seqPlayer->fadeTimer;
                                } else {
                                    seqPlayer->fadeVolume = value / 127.0f;
                                }
                                break;

                            case SEQPLAYER_STATE_FADE_OUT:
                                break;
                        }
                        break;

                    case 0xD9: // seqPlayer: set volume scale
                        seqPlayer->fadeVolumeScale = (s8)AudioScript_ScriptReadU8(seqScript) / 127.0f;
                        break;

                    case 0xD7: // seqPlayer: initialize channels
                        temp = AudioScript_ScriptReadS16(seqScript);
                        AudioScript_SequencePlayerSetupChannels(seqPlayer, temp);
                        break;

                    case 0xD6: // seqPlayer: disable channels
                        AudioScript_ScriptReadS16(seqScript);
                        break;

                    case 0xD5: // seqPlayer: set mute scale
                        seqPlayer->muteVolumeScale = (s8)AudioScript_ScriptReadU8(seqScript) / 127.0f;
                        break;

                    case 0xD4: // seqPlayer: mute
                        seqPlayer->muted = true;
                        break;

                    case 0xD3: // seqPlayer: set mute behavior
                        seqPlayer->muteFlags = AudioScript_ScriptReadU8(seqScript);
                        break;

                    case 0xD1: // seqPlayer: set short note gatetime table
                    case 0xD2: // seqPlayer: set short note velocity table
                        temp = AudioScript_ScriptReadS16(seqScript);
                        data3 = &seqPlayer->seqData[temp];
                        if (cmd == 0xD2) {
                            seqPlayer->shortNoteVelocityTable = data3;
                        } else {
                            seqPlayer->shortNoteGateTimeTable = data3;
                        }
                        break;

                    case 0xD0: // seqPlayer: set note allocation policy
                        seqPlayer->noteAllocPolicy = AudioScript_ScriptReadU8(seqScript);
                        break;

                    case 0xCE: // seqPlayer: random value
                        cmd = AudioScript_ScriptReadU8(seqScript);
                        if (cmd == 0) {
                            seqScript->value = (gAudioCtx.audioRandom >> 2) & 0xFF;
                        } else {
                            seqScript->value = (gAudioCtx.audioRandom >> 2) % cmd;
                        }
                        break;

                    case 0xCD: // seqPlayer: dyncall
                        temp = AudioScript_ScriptReadS16(seqScript);
                        if ((seqScript->value != -1) && (seqScript->depth != 3)) {
                            data1 = seqPlayer->seqData + (u32)(temp + (seqScript->value << 1));
                            seqScript->stack[seqScript->depth] = seqScript->pc;
                            seqScript->depth++;
                            temp = (data1[0] << 8) + data1[1];
                            seqScript->pc = &seqPlayer->seqData[temp];
                        }
                        break;

                    case 0xCC: // seqPlayer: set value
                        seqScript->value = AudioScript_ScriptReadU8(seqScript);
                        break;

                    case 0xC9: // seqPlayer: `bit and` -> set value
                        seqScript->value &= AudioScript_ScriptReadU8(seqScript);
                        break;

                    case 0xC8: // seqPlayer: subtract -> set value
                        seqScript->value -= AudioScript_ScriptReadU8(seqScript);
                        break;

                    case 0xC7: // seqPlayer: write into sequence script
                        cmd = AudioScript_ScriptReadU8(seqScript);
                        temp = AudioScript_ScriptReadS16(seqScript);
                        data2 = &seqPlayer->seqData[temp];
                        *data2 = (u8)seqScript->value + cmd;
                        break;

                    case 0xC2: // seqPlayer:
                        temp = AudioScript_ScriptReadS16(seqScript);
                        if (seqScript->value != -1) {
                            data4 = seqPlayer->seqData + (u32)(temp + (seqScript->value << 1));

                            temp = (data4[0] << 8) + data4[1];
                            seqScript->pc = &seqPlayer->seqData[temp];
                        }
                        break;

                    case 0xC6: // seqPlayer: stop script
                        seqPlayer->stopScript = true;
                        return;

                    case 0xC5: // seqPlayer:
                        seqPlayer->unk_16 = AudioScript_ScriptReadS16(seqScript);
                        break;

                    case 0xEF: // seqPlayer:
                        AudioScript_ScriptReadS16(seqScript);
                        AudioScript_ScriptReadU8(seqScript);
                        break;

                    case 0xC4: // seqPlayer: start sequence
                        cmd = AudioScript_ScriptReadU8(seqScript);
                        if (cmd == 0xFF) {
                            cmd = seqPlayer->playerIndex;
                            if (seqPlayer->state == SEQPLAYER_STATE_FADE_OUT) {
                                break;
                            }
                        }

                        cmdLowBits = AudioScript_ScriptReadU8(seqScript);
                        AudioLoad_SyncInitSeqPlayer(cmd, cmdLowBits, 0);
                        if (cmd == (u8)seqPlayer->playerIndex) {
                            return;
                        }
                        break;

                    case 0xC3: // seqPlayer:
                        temp = AudioScript_ScriptReadS16(seqScript);
                        if (seqScript->value != -1) {
                            new_var = (u16*)(seqPlayer->seqData + (u32)(temp + seqScript->value * 2));
                            temp = *new_var;

                            for (i = 0; i < ARRAY_COUNT(seqPlayer->channels); i++) {
                                seqPlayer->channels[i]->muted = temp & 1;
                                temp = temp >> 1;
                            }
                        }
                        break;
                }
                continue;
            }

            // Commands 0x00 - 0xBF
            cmdLowBits = cmd & 0x0F;

            switch (cmd & 0xF0) {
                case 0x00: // seqPlayer: test channel disabled
                    seqScript->value = seqPlayer->channels[cmdLowBits]->enabled ^ 1;
                    break;

                case 0x50: // seqPlayer: io read value subtract
                    seqScript->value -= seqPlayer->seqScriptIO[cmdLowBits];
                    break;

                case 0x70: // seqPlayer: io write value
                    seqPlayer->seqScriptIO[cmdLowBits] = seqScript->value;
                    break;

                case 0x80: // seqPlayer: io read value
                    seqScript->value = seqPlayer->seqScriptIO[cmdLowBits];
                    if (cmdLowBits < 2) {
                        seqPlayer->seqScriptIO[cmdLowBits] = SEQ_IO_VAL_NONE;
                    }
                    break;

                case 0x40: // seqPlayer: disable channel
                    AudioScript_SequenceChannelDisable(seqPlayer->channels[cmdLowBits]);
                    break;

                case 0x90: // seqPlayer: start channel
                    temp = AudioScript_ScriptReadS16(seqScript);
                    AudioScript_SequenceChannelEnable(seqPlayer, cmdLowBits, (void*)&seqPlayer->seqData[temp]);
                    break;

                case 0xA0: // seqPlayer: start channel relative
                    tempS = AudioScript_ScriptReadS16(seqScript);
                    AudioScript_SequenceChannelEnable(seqPlayer, cmdLowBits, (void*)&seqScript->pc[tempS]);
                    break;

                case 0xB0: // seqPlayer: load sequence
                    cmd = AudioScript_ScriptReadU8(seqScript);
                    temp = AudioScript_ScriptReadS16(seqScript);
                    data2 = &seqPlayer->seqData[temp];
                    AudioLoad_SlowLoadSeq(cmd, data2, &seqPlayer->seqScriptIO[cmdLowBits]);
                    break;

                case 0x60: // seqPlayer: async load
                    cmd = AudioScript_ScriptReadU8(seqScript);
                    value = cmd;
                    temp = AudioScript_ScriptReadU8(seqScript);
                    AudioLoad_ScriptLoad(value, temp, &seqPlayer->seqScriptIO[cmdLowBits]);
                    break;
            }
        }
    }

    for (j = 0; j < SEQ_NUM_CHANNELS; j++) {
        channel = seqPlayer->channels[j];
        if (channel->enabled) {
            AudioScript_SequenceChannelProcessScript(channel);
        }
    }
}

void AudioScript_ProcessSequences(s32 arg0) {
    SequencePlayer* seqPlayer;
    u32 i;

    gAudioCtx.sampleStateOffset = (gAudioCtx.audioBufferParameters.updatesPerFrame - arg0 - 1) * gAudioCtx.numNotes;

    for (i = 0; i < (u32)gAudioCtx.audioBufferParameters.numSequencePlayers; i++) {
        seqPlayer = &gAudioCtx.seqPlayers[i];
        if (seqPlayer->enabled == true) {
            AudioScript_SequencePlayerProcessSequence(seqPlayer);
            AudioScript_SequencePlayerProcessSound(seqPlayer);
        }
    }

    AudioPlayback_ProcessNotes();
}

void AudioScript_SkipForwardSequence(SequencePlayer* seqPlayer) {
    while (seqPlayer->skipTicks > 0) {
        AudioScript_SequencePlayerProcessSequence(seqPlayer);
        AudioScript_SequencePlayerProcessSound(seqPlayer);
        seqPlayer->skipTicks--;
    }
}

void AudioScript_ResetSequencePlayer(SequencePlayer* seqPlayer) {
    s32 channelIndex;

    AudioScript_SequencePlayerDisable(seqPlayer);
    seqPlayer->stopScript = false;
    seqPlayer->delay = 0;
    seqPlayer->state = SEQPLAYER_STATE_FADE_IN;
    seqPlayer->fadeTimer = 0;
    seqPlayer->storedFadeTimer = 0;
    seqPlayer->tempoAcc = 0;
    seqPlayer->tempo = 120 * TATUMS_PER_BEAT; // 120 BPM
    seqPlayer->tempoChange = 0;
    seqPlayer->transposition = 0;
    seqPlayer->noteAllocPolicy = 0;
    seqPlayer->shortNoteVelocityTable = gDefaultShortNoteVelocityTable;
    seqPlayer->shortNoteGateTimeTable = gDefaultShortNoteGateTimeTable;
    seqPlayer->scriptCounter = 0;
    seqPlayer->unk_16 = 0;
    seqPlayer->fadeVolume = 1.0f;
    seqPlayer->fadeVelocity = 0.0f;
    seqPlayer->volume = 0.0f;
    seqPlayer->muteVolumeScale = 0.5f;

    for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
        AudioScript_InitSequenceChannel(seqPlayer->channels[channelIndex]);
    }
}

void AudioScript_InitSequencePlayerChannels(s32 seqPlayerIndex) {
    SequenceChannel* channel;
    SequencePlayer* seqPlayer = &gAudioCtx.seqPlayers[seqPlayerIndex];
    s32 channelIndex;
    s32 layerIndex;

    for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
        seqPlayer->channels[channelIndex] = AudioHeap_AllocZeroed(&gAudioCtx.miscPool, sizeof(SequenceChannel));
        if (seqPlayer->channels[channelIndex] == NULL) {
            seqPlayer->channels[channelIndex] = &gAudioCtx.sequenceChannelNone;
        } else {
            channel = seqPlayer->channels[channelIndex];
            channel->seqPlayer = seqPlayer;
            channel->enabled = false;
            channel->channelIndex = channelIndex;
            for (layerIndex = 0; layerIndex < ARRAY_COUNT(channel->layers); layerIndex++) {
                channel->layers[layerIndex] = NULL;
            }
        }

        AudioScript_InitSequenceChannel(seqPlayer->channels[channelIndex]);
    }
}

void AudioScript_InitSequencePlayer(SequencePlayer* seqPlayer) {
    s32 i;
    s32 j;

    for (i = 0; i < SEQ_NUM_CHANNELS; i++) {
        seqPlayer->channels[i] = &gAudioCtx.sequenceChannelNone;
    }

    seqPlayer->enabled = false;
    seqPlayer->muted = false;
    seqPlayer->fontDmaInProgress = false;
    seqPlayer->seqDmaInProgress = false;
    seqPlayer->applyBend = false;

    for (j = 0; j < ARRAY_COUNT(seqPlayer->seqScriptIO); j++) {
        seqPlayer->seqScriptIO[j] = SEQ_IO_VAL_NONE;
    }

    seqPlayer->muteFlags = MUTE_FLAGS_SOFTEN | MUTE_FLAGS_STOP_NOTES;
    seqPlayer->fadeVolumeScale = 1.0f;
    seqPlayer->bend = 1.0f;

    AudioPlayback_InitNoteLists(&seqPlayer->notePool);
    AudioScript_ResetSequencePlayer(seqPlayer);
}

void AudioScript_InitSequencePlayers(void) {
    s32 i;

    AudioScript_InitLayerFreelist();

    for (i = 0; i < ARRAY_COUNT(gAudioCtx.sequenceLayers); i++) {
        gAudioCtx.sequenceLayers[i].channel = NULL;
        gAudioCtx.sequenceLayers[i].enabled = false;
    }

    for (i = 0; i < ARRAY_COUNT(gAudioCtx.seqPlayers); i++) {
        AudioScript_InitSequencePlayer(&gAudioCtx.seqPlayers[i]);
    }
}
