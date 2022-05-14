#include <math.h>
#include "ultra64.h"
#include "global.h"

#define ROUND(num) floorf((num) * 100) / 100;

void Audio_SequenceChannelProcessSound(SequenceChannel* channel, s32 recalculateVolume, s32 b) {
    f32 channelVolume;
    f32 chanFreqScale;
    s32 i;

    if (channel->changes.s.volume || recalculateVolume) {
        channelVolume = ROUND((channel->volume * (channel->volumeScale * channel->seqPlayer->appliedFadeVolume)) *
                              channel->seqPlayer->gameVolume);
        if (channel->seqPlayer->muted && (channel->muteBehavior & 0x20)) {
            channelVolume = channel->seqPlayer->muteVolumeScale * channelVolume;
        }
        channel->appliedVolume = (channelVolume * channelVolume);
    }

    if (channel->changes.s.pan) {
        channel->pan = channel->newPan * channel->panChannelWeight;
    }

    chanFreqScale = channel->freqScale;
    if (b != 0) {
        chanFreqScale *= channel->seqPlayer->unk_34;
        channel->changes.s.freqScale = true;
    }

    for (i = 0; i < 4; i++) {
        SequenceLayer* layer = channel->layers[i];
        if (layer != NULL && layer->enabled && layer->note != NULL) {
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

void Audio_SequencePlayerProcessSound(SequencePlayer* seqPlayer) {
    s32 i;

    if (seqPlayer->fadeTimer != 0) {
        seqPlayer->fadeVolume += seqPlayer->fadeVelocity;
        seqPlayer->recalculateVolume = true;

        if (seqPlayer->fadeVolume > 1.0f) {
            seqPlayer->fadeVolume = 1.0f;
        }
        if (seqPlayer->fadeVolume < 0) {
            seqPlayer->fadeVolume = 0;
        }

        if (--seqPlayer->fadeTimer == 0 && seqPlayer->state == 2) {
            AudioSeq_SequencePlayerDisable(seqPlayer);
            return;
        }
    }

    if (seqPlayer->recalculateVolume) {
        seqPlayer->appliedFadeVolume = seqPlayer->fadeVolume * seqPlayer->fadeVolumeScale;
    }

    for (i = 0; i < 16; i++) {
        if (seqPlayer->channels[i]->enabled == 1) {
            Audio_SequenceChannelProcessSound(seqPlayer->channels[i], seqPlayer->recalculateVolume, seqPlayer->unk_0b1);
        }
    }

    seqPlayer->recalculateVolume = false;
}

f32 Audio_GetPortamentoFreqScale(Portamento* p) {
    u32 loResCur;
    f32 result;

    p->cur += p->speed;
    loResCur = (p->cur >> 8) & 0xFF;

    if (loResCur >= 127) {
        loResCur = 127;
        p->mode = 0;
    }

    result = 1.0f + p->extent * (gBendPitchOneOctaveFrequencies[loResCur + 128] - 1.0f);
    return result;
}

s16 Audio_GetVibratoPitchChange(VibratoState* vib) {
    s32 index;
    vib->time += (s32)vib->rate;
    index = (vib->time >> 10) & 0x3F;
    return vib->curve[index];
}

f32 Audio_GetVibratoFreqScale(VibratoState* vib) {
    static f32 D_80130510 = 0.0f;
    static s32 D_80130514 = 0;
    f32 pitchChange;
    f32 extent;
    f32 invExtent;
    f32 result;
    f32 temp;
    SequenceChannel* channel = vib->channel;

    if (vib->delay != 0) {
        vib->delay--;
        return 1;
    }

    //! @bug this probably meant to compare with gAudioContext.sequenceChannelNone.
    //! -1 isn't used as a channel pointer anywhere else.
    if (channel != ((SequenceChannel*)(-1))) {
        if (vib->extentChangeTimer) {
            if (vib->extentChangeTimer == 1) {
                vib->extent = (s32)channel->vibratoExtentTarget;
            } else {
                vib->extent += ((s32)channel->vibratoExtentTarget - vib->extent) / (s32)vib->extentChangeTimer;
            }

            vib->extentChangeTimer--;
        } else if (channel->vibratoExtentTarget != (s32)vib->extent) {
            if ((vib->extentChangeTimer = channel->vibratoExtentChangeDelay) == 0) {
                vib->extent = (s32)channel->vibratoExtentTarget;
            }
        }

        if (vib->rateChangeTimer) {
            if (vib->rateChangeTimer == 1) {
                vib->rate = (s32)channel->vibratoRateTarget;
            } else {
                vib->rate += ((s32)channel->vibratoRateTarget - vib->rate) / (s32)vib->rateChangeTimer;
            }

            vib->rateChangeTimer--;
        } else if (channel->vibratoRateTarget != (s32)vib->rate) {
            if ((vib->rateChangeTimer = channel->vibratoRateChangeDelay) == 0) {
                vib->rate = (s32)channel->vibratoRateTarget;
            }
        }
    }

    if (vib->extent == 0) {
        return 1.0f;
    }

    pitchChange = Audio_GetVibratoPitchChange(vib) + 32768.0f;
    temp = vib->extent / 4096.0f;
    extent = temp + 1.0f;
    invExtent = 1.0f / extent;

    result = 1.0f / ((extent - invExtent) * pitchChange / 65536.0f + invExtent);

    D_80130510 += result;
    D_80130514++;

    return result;
}

void Audio_NoteVibratoUpdate(Note* note) {
    if (note->portamento.mode != 0) {
        note->playbackState.portamentoFreqScale = Audio_GetPortamentoFreqScale(&note->portamento);
    }
    if (note->vibratoState.active) {
        note->playbackState.vibratoFreqScale = Audio_GetVibratoFreqScale(&note->vibratoState);
    }
}

void Audio_NoteVibratoInit(Note* note) {
    VibratoState* vib;
    SequenceChannel* channel;

    note->playbackState.vibratoFreqScale = 1.0f;

    vib = &note->vibratoState;

    vib->active = 1;
    vib->time = 0;

    vib->curve = gWaveSamples[2];
    vib->channel = note->playbackState.parentLayer->channel;
    channel = vib->channel;
    if ((vib->extentChangeTimer = channel->vibratoExtentChangeDelay) == 0) {
        vib->extent = (s32)channel->vibratoExtentTarget;
    } else {
        vib->extent = (s32)channel->vibratoExtentStart;
    }

    if ((vib->rateChangeTimer = channel->vibratoRateChangeDelay) == 0) {
        vib->rate = (s32)channel->vibratoRateTarget;
    } else {
        vib->rate = (s32)channel->vibratoRateStart;
    }
    vib->delay = channel->vibratoDelay;
}

void Audio_NotePortamentoInit(Note* note) {
    note->playbackState.portamentoFreqScale = 1.0f;
    note->portamento = note->playbackState.parentLayer->portamento;
}

void Audio_AdsrInit(AdsrState* adsr, AdsrEnvelope* envelope, s16* volOut) {
    adsr->action.asByte = 0;
    adsr->delay = 0;
    adsr->envelope = envelope;
    adsr->sustain = 0.0f;
    adsr->current = 0.0f;
    // (An older versions of the audio engine used in Super Mario 64 did
    // adsr->volOut = volOut. That line and associated struct member were
    // removed, but the function parameter was forgotten and remains.)
}

f32 Audio_AdsrUpdate(AdsrState* adsr) {
    u8 state = adsr->action.s.state;
    switch (state) {
        case ADSR_STATE_DISABLED:
            return 0.0f;

        case ADSR_STATE_INITIAL: {
            if (adsr->action.s.hang) {
                adsr->action.s.state = ADSR_STATE_HANG;
                break;
            }
            // fallthrough
        }

        case ADSR_STATE_START_LOOP:
            adsr->envIndex = 0;
            adsr->action.s.state = ADSR_STATE_LOOP;
            // fallthrough

        retry:
        case ADSR_STATE_LOOP:
            adsr->delay = (s16)BOMSWAP16(adsr->envelope[adsr->envIndex].delay);
            switch (adsr->delay) {
                case ADSR_DISABLE:
                    adsr->action.s.state = ADSR_STATE_DISABLED;
                    break;
                case ADSR_HANG:
                    adsr->action.s.state = ADSR_STATE_HANG;
                    break;
                case ADSR_GOTO:
                    adsr->envIndex = (s16)BOMSWAP16(adsr->envelope[adsr->envIndex].arg);
                    goto retry;
                case ADSR_RESTART:
                    adsr->action.s.state = ADSR_STATE_INITIAL;
                    break;

                default:
                    adsr->delay *= gAudioContext.audioBufferParameters.unk_24;
                    if (adsr->delay == 0) {
                        adsr->delay = 1;
                    }
                    adsr->target = (s16)BOMSWAP16(adsr->envelope[adsr->envIndex].arg) / 32767.0f;
                    adsr->target = adsr->target * adsr->target;
                    adsr->velocity = (adsr->target - adsr->current) / adsr->delay;
                    adsr->action.s.state = ADSR_STATE_FADE;
                    adsr->envIndex++;
                    break;
            }
            if (adsr->action.s.state != ADSR_STATE_FADE) {
                break;
            }
            // fallthrough

        case ADSR_STATE_FADE:
            adsr->current += adsr->velocity;
            if (--adsr->delay <= 0) {
                adsr->action.s.state = ADSR_STATE_LOOP;
            }
            // fallthrough

        case ADSR_STATE_HANG:
            break;

        case ADSR_STATE_DECAY:
        case ADSR_STATE_RELEASE: {
            adsr->current -= adsr->fadeOutVel;
            if (adsr->sustain != 0.0f && state == ADSR_STATE_DECAY) {
                if (adsr->current < adsr->sustain) {
                    adsr->current = adsr->sustain;
                    adsr->delay = 128;
                    adsr->action.s.state = ADSR_STATE_SUSTAIN;
                }
                break;
            }

            if (adsr->current < 0.00001f) {
                adsr->current = 0.0f;
                adsr->action.s.state = ADSR_STATE_DISABLED;
            }
            break;
        }

        case ADSR_STATE_SUSTAIN:
            adsr->delay -= 1;
            if (adsr->delay == 0) {
                adsr->action.s.state = ADSR_STATE_RELEASE;
            }
            break;
    }

    if (adsr->action.s.decay) {
        adsr->action.s.state = ADSR_STATE_DECAY;
        adsr->action.s.decay = false;
    }

    if (adsr->action.s.release) {
        adsr->action.s.state = ADSR_STATE_RELEASE;
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
