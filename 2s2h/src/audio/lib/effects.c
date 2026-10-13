/**
 * @file effects.c
 *
 * The first half of this file processes sound on the seqPlayer, channel, and layer level
 * once the .seq file is finished for this update.
 *
 * The second half of this file implements three types of audio effects over long periods of times:
 * - Vibrato: regular, pulsating change of pitch
 * - Portamento: pitch sliding from one note to another
 * - Multi-Point ADSR Envelope: volume changing over time through Attack, Decay, Sustain, Release
 */
#include "global.h"
#include "audio/effects.h"
#include <ship/utils/binarytools/endianness.h>

void AudioScript_SequenceChannelProcessSound(SequenceChannel* channel, s32 recalculateVolume, s32 applyBend) {
    f32 channelVolume;
    f32 chanFreqScale;
    s32 i;

    if (channel->changes.s.volume || recalculateVolume) {
        // 2S2H [Port][Audio] Scale volume by our sequence player volume sliders
        channelVolume = channel->volume * channel->volumeScale * channel->seqPlayer->appliedFadeVolume *
                        channel->seqPlayer->portVolumeScale;
        if (channel->seqPlayer->muted && (channel->muteFlags & MUTE_FLAGS_SOFTEN)) {
            channelVolume = channelVolume * channel->seqPlayer->muteVolumeScale;
        }
        channel->appliedVolume = SQ(channelVolume);
    }

    if (channel->changes.s.pan) {
        channel->pan = channel->newPan * channel->panChannelWeight;
    }

    chanFreqScale = channel->freqScale;
    if (applyBend) {
        chanFreqScale *= channel->seqPlayer->bend;
        channel->changes.s.freqScale = true;
    }

    for (i = 0; i < ARRAY_COUNT(channel->layers); i++) {
        SequenceLayer* layer = channel->layers[i];

        if ((layer != NULL) && layer->enabled && (layer->note != NULL)) {
            if (layer->notePropertiesNeedInit) {
                layer->noteFreqScale = layer->freqScale * chanFreqScale;
                layer->noteVelocity = layer->velocitySquare2 * channel->appliedVolume;
                layer->notePan = (channel->pan + layer->pan * (0x80 - channel->panChannelWeight)) >> 7;
                layer->notePropertiesNeedInit = false;
            } else {
                if (channel->changes.s.freqScale) {
                    layer->noteFreqScale = layer->freqScale * chanFreqScale;
                }
                if (channel->changes.s.volume || recalculateVolume) {
                    layer->noteVelocity = layer->velocitySquare2 * channel->appliedVolume;
                }
                if (channel->changes.s.pan) {
                    layer->notePan = (channel->pan + layer->pan * (0x80 - channel->panChannelWeight)) >> 7;
                }
            }
        }
    }
    channel->changes.asByte = 0;
}

void AudioScript_SequencePlayerProcessSound(SequencePlayer* seqPlayer) {
    s32 i;

    if ((seqPlayer->fadeTimer != 0) && (seqPlayer->skipTicks == 0)) {
        seqPlayer->fadeVolume += seqPlayer->fadeVelocity;
        seqPlayer->recalculateVolume = true;

        if (seqPlayer->fadeVolume > 1.0f) {
            seqPlayer->fadeVolume = 1.0f;
        }
        if (seqPlayer->fadeVolume < 0.0f) {
            seqPlayer->fadeVolume = 0.0f;
        }

        seqPlayer->fadeTimer--;
        if ((seqPlayer->fadeTimer == 0) && (seqPlayer->state == SEQPLAYER_STATE_FADE_OUT)) {
            AudioScript_SequencePlayerDisable(seqPlayer);
            return;
        }
    }

    if (seqPlayer->recalculateVolume) {
        seqPlayer->appliedFadeVolume = seqPlayer->fadeVolume * seqPlayer->fadeVolumeScale;
    }

    for (i = 0; i < SEQ_NUM_CHANNELS; i++) {
        if (seqPlayer->channels[i]->enabled == true) {
            AudioScript_SequenceChannelProcessSound(seqPlayer->channels[i], seqPlayer->recalculateVolume,
                                                    seqPlayer->applyBend);
        }
    }

    seqPlayer->recalculateVolume = false;
}

/**
 * @return freqScale
 */
f32 AudioEffects_UpdatePortamento(Portamento* portamento) {
    u32 bendIndex;
    f32 portamentoFreq;

    portamento->cur += portamento->speed;
    bendIndex = (portamento->cur >> 8) & 0xFF;

    if (bendIndex >= 127) {
        bendIndex = 127;
        portamento->mode = PORTAMENTO_MODE_OFF;
    }

    portamentoFreq = AUDIO_LERPIMP(1.0f, gBendPitchOneOctaveFrequencies[bendIndex + 128], portamento->extent);

    return portamentoFreq;
}

s16 AudioEffects_GetVibratoPitchChange(VibratoState* vib) {
    s32 index;

    vib->time += (s32)vib->rate;
    // 0x400 is 1 unit of time, 0x10000 is 1 period
    index = (vib->time / 0x400) % WAVE_SAMPLE_COUNT;
    return vib->curve[index];
}

/**
 * @return freqScale
 */
f32 AudioEffects_UpdateVibrato(VibratoState* vib) {
    static f32 sActiveVibratoFreqScaleSum = 0.0f;
    static s32 sActiveVibratoCount = 0;
    f32 pitchChange;
    f32 depth;
    f32 invDepth;
    f32 result;
    f32 scaledDepth;
    VibratoSubStruct* subVib = vib->vibSubStruct;

    if (vib->delay != 0) {
        vib->delay--;
        return 1.0f;
    }

    if (subVib != NULL) {
        if ((u32)vib->depthChangeTimer != 0) {
            if (vib->depthChangeTimer == 1) {
                vib->depth = (s32)subVib->vibratoDepthTarget;
            } else {
                vib->depth += ((s32)subVib->vibratoDepthTarget - vib->depth) / (s32)vib->depthChangeTimer;
            }

            vib->depthChangeTimer--;
        } else if (subVib->vibratoDepthTarget != (s32)vib->depth) {
            if ((vib->depthChangeTimer = subVib->vibratoDepthChangeDelay) == 0) {
                vib->depth = (s32)subVib->vibratoDepthTarget;
            }
        }

        if ((u32)vib->rateChangeTimer != 0) {
            if (vib->rateChangeTimer == 1) {
                vib->rate = (s32)subVib->vibratoRateTarget;
            } else {
                vib->rate += ((s32)subVib->vibratoRateTarget - vib->rate) / (s32)vib->rateChangeTimer;
            }

            vib->rateChangeTimer--;
        } else if (subVib->vibratoRateTarget != (s32)vib->rate) {
            if ((vib->rateChangeTimer = subVib->vibratoRateChangeDelay) == 0) {
                vib->rate = (s32)subVib->vibratoRateTarget;
            }
        }
    }

    if (vib->depth == 0.0f) {
        return 1.0f;
    }

    pitchChange = (f32)AudioEffects_GetVibratoPitchChange(vib) + 0x8000;
    scaledDepth = vib->depth / 4096.0f;
    depth = scaledDepth + 1.0f;
    invDepth = 1.0f / depth;

    // Inverse linear interpolation
    result = 1.0f / ((depth - invDepth) * pitchChange / 0x10000 + invDepth);

    sActiveVibratoFreqScaleSum += result;
    sActiveVibratoCount++;

    return result;
}

void AudioEffects_UpdatePortamentoAndVibrato(Note* note) {
    // Update Portamento
    if (note->playbackState.portamento.mode != PORTAMENTO_MODE_OFF) {
        note->playbackState.portamentoFreqScale = AudioEffects_UpdatePortamento(&note->playbackState.portamento);
    }
    // Update Vibrato
    if (note->playbackState.vibratoState.active) {
        note->playbackState.vibratoFreqScale = AudioEffects_UpdateVibrato(&note->playbackState.vibratoState);
    }
}

void AudioEffects_InitVibrato(Note* note) {
    NotePlaybackState* playbackState = &note->playbackState;
    VibratoState* vib = &playbackState->vibratoState;
    VibratoSubStruct* subVib;

    vib->active = true;
    vib->curve = gWaveSamples[2]; // gSineWaveSample

    if (playbackState->parentLayer->unk_0A.s.useVibrato == true) {
        vib->vibSubStruct = &playbackState->parentLayer->channel->vibrato;
    } else {
        vib->vibSubStruct = &playbackState->parentLayer->vibrato;
    }

    subVib = vib->vibSubStruct;

    if ((vib->depthChangeTimer = subVib->vibratoDepthChangeDelay) == 0) {
        vib->depth = (s32)subVib->vibratoDepthTarget;
    } else {
        vib->depth = (s32)subVib->vibratoDepthStart;
    }

    if ((vib->rateChangeTimer = subVib->vibratoRateChangeDelay) == 0) {
        vib->rate = (s32)subVib->vibratoRateTarget;
    } else {
        vib->rate = (s32)subVib->vibratoRateStart;
    }

    playbackState->vibratoFreqScale = 1.0f;
    vib->time = 0;
    vib->delay = subVib->vibratoDelay;
}

void AudioEffects_InitPortamento(Note* note) {
    note->playbackState.portamentoFreqScale = 1.0f;
    note->playbackState.portamento = note->playbackState.parentLayer->portamento;
}

void AudioEffects_InitAdsr(AdsrState* adsr, EnvelopePoint* envelope, s16* volOut) {
    adsr->action.asByte = 0;
    adsr->delay = 0;
    adsr->envelope = envelope;
    adsr->sustain = 0.0f;
    adsr->current = 0.0f;
    adsr->velocity = 0.0f;
    // (An older versions of the audio engine used in Super Mario 64 did
    // adsr->volOut = volOut. That line and associated struct member were
    // removed, but the function parameter was forgotten and remains.)
}

/**
 * @return volumeScale
 */
f32 AudioEffects_UpdateAdsr(AdsrState* adsr) {
    u8 status = adsr->action.s.status;

    switch (status) {
        case ADSR_STATUS_DISABLED:
            return 0.0f;

        case ADSR_STATUS_INITIAL:
            if (adsr->action.s.hang) {
                adsr->action.s.status = ADSR_STATUS_HANG;
                break;
            }
            // fallthrough
        case ADSR_STATUS_START_LOOP:
            adsr->envelopeIndex = 0;
            adsr->action.s.status = ADSR_STATUS_LOOP;
            // fallthrough
        retry:
        case ADSR_STATUS_LOOP:
            adsr->delay = (s16)BE16SWAP(adsr->envelope[adsr->envelopeIndex].delay);
            switch (adsr->delay) {
                case ADSR_DISABLE:
                    adsr->action.s.status = ADSR_STATUS_DISABLED;
                    break;

                case ADSR_HANG:
                    adsr->action.s.status = ADSR_STATUS_HANG;
                    break;

                case ADSR_GOTO:
                    adsr->envelopeIndex = (s16)BE16SWAP(adsr->envelope[adsr->envelopeIndex].arg);
                    goto retry;

                case ADSR_RESTART:
                    adsr->action.s.status = ADSR_STATUS_INITIAL;
                    break;

                default:
                    adsr->delay *= gAudioCtx.audioBufferParameters.updatesPerFrameScaled;
                    if (adsr->delay == 0) {
                        adsr->delay = 1;
                    }

                    adsr->target = (s16)BE16SWAP(adsr->envelope[adsr->envelopeIndex].arg) / 32767.0f;
                    adsr->target = SQ(adsr->target);
                    adsr->velocity = (adsr->target - adsr->current) / adsr->delay;
                    adsr->action.s.status = ADSR_STATUS_FADE;
                    adsr->envelopeIndex++;
                    break;
            }
            if (adsr->action.s.status != ADSR_STATUS_FADE) {
                break;
            }
            // fallthrough
        case ADSR_STATUS_FADE:
            adsr->current += adsr->velocity;
            adsr->delay--;
            if (adsr->delay <= 0) {
                adsr->action.s.status = ADSR_STATUS_LOOP;
            }
            // fallthrough
        case ADSR_STATUS_HANG:
            break;

        case ADSR_STATUS_DECAY:
        case ADSR_STATUS_RELEASE:
            adsr->current -= adsr->fadeOutVel;
            if ((adsr->sustain != 0.0f) && (status == ADSR_STATUS_DECAY)) {
                if (adsr->current < adsr->sustain) {
                    adsr->current = adsr->sustain;
                    adsr->delay = 128;
                    adsr->action.s.status = ADSR_STATUS_SUSTAIN;
                }
                break;
            }

            if (adsr->current < 0.00001f) {
                adsr->current = 0.0f;
                adsr->action.s.status = ADSR_STATUS_DISABLED;
            }
            break;

        case ADSR_STATUS_SUSTAIN:
            adsr->delay--;
            if (adsr->delay == 0) {
                adsr->action.s.status = ADSR_STATUS_RELEASE;
            }
            break;
    }

    if (adsr->action.s.decay) {
        adsr->action.s.status = ADSR_STATUS_DECAY;
        adsr->action.s.decay = false;
    }

    if (adsr->action.s.release) {
        adsr->action.s.status = ADSR_STATUS_RELEASE;
        adsr->action.s.release = false;
    }

    if (adsr->current < 0.0f) {
        return 0.0f;
    }

    if (adsr->current > 1.0f) {
        return 1.0f;
    }

    return adsr->current;
}
