#if ENABLE_FLUIDSYNTH
#include "MidiTranslator.h"

#include <iterator>

#include <libultraship/libultra.h>
#include "variables.h"

namespace {
uint8_t SeqChannelIndex(const SequenceChannel* channel) {
    const SequencePlayer* seqPlayer = channel->seqPlayer;
    for (uint8_t i = 0; i < std::size(seqPlayer->channels); i++) {
        if (seqPlayer->channels[i] == channel) {
            return i;
        }
    }
    return 0;
}
} // namespace

extern "C" void SOH_MidiTranslator_ProcessNote(Note* note, NoteSubEu* noteSubEu, const NoteSubAttributes* subAttrs) {
    auto& translator = SOH::MidiTranslator::Instance();
    const int noteIndex = static_cast<int>(note - gAudioContext.notes);
    const NotePlaybackState& playbackState = note->playbackState;
    SequenceLayer* layer = playbackState.parentLayer;

    // From release on, the engine plays the note off its attributes, detached from the layer.
    if (layer == NO_LAYER || layer->channel == nullptr || playbackState.unk_04 != 0) {
        if (translator.NoteReleased(noteIndex)) {
            noteSubEu->bitField0.enabled = false;
        }
        return;
    }

    const SequenceChannel* channel = layer->channel;
    const SequencePlayer* seqPlayer = channel->seqPlayer;
    const f32 resampleRate = gAudioContext.audioBufferParameters.resampleRate;

    SOH::EngineNote engineNote;
    engineNote.noteIndex = noteIndex;
    engineNote.seqPlayer = static_cast<uint8_t>(seqPlayer - gAudioContext.seqPlayers);
    engineNote.seqChannel = SeqChannelIndex(channel);
    engineNote.fontId = static_cast<uint8_t>(playbackState.fontId);
    engineNote.instOrWave = layer->instOrWave == static_cast<int16_t>(0xFF) ? channel->instOrWave : layer->instOrWave;
    engineNote.semitone = layer->semitone;
    engineNote.freqScale = subAttrs->frequency;
    // FluidSynth runs its own envelope, so it takes the note's velocity before the engine's.
    engineNote.strike = layer->velocitySquare2;
    engineNote.channelVolume = seqPlayer->muted && (channel->muteBehavior & 8) ? 0.0f : channel->appliedVolume;
    engineNote.pan = subAttrs->pan;
    engineNote.reverb = subAttrs->reverbVol;
    engineNote.isFinished = noteSubEu->bitField0.finished;
    // Vibrato and portamento ride on the final frequency; dividing by the nominal note's
    // cancels sample tuning and the resample rate, leaving only the pitch modulation.
    if (layer->sound != nullptr && layer->semitone < 0x80 && subAttrs->frequency > 0.0f) {
        const f32 nominal = gNoteFrequencies[layer->semitone] * layer->sound->tuning * resampleRate;
        if (nominal > 0.0f) {
            engineNote.pitchBend = subAttrs->frequency / nominal;
        }
    }

    if (translator.ProcessNote(engineNote)) {
        noteSubEu->bitField0.enabled = false;
    }
}
#endif
