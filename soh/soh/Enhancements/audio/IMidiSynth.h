#pragma once
#include <cstdint>
#include <string>
#include <cmath>

namespace SOH {

// MIDI-shaped soft synth interface. An implementation that is installed
// on the MidiSynthManager replaces the engine's native audio synthesis
// path: the audio thread fills its output buffer by calling Render()
// instead of running the native synth.
//
// When no implementation is installed, the manager returns nullptr and
// the audio thread falls back to native synthesis.
class IMidiSynth {
  public:
    virtual ~IMidiSynth() = default;

    // Load an SF soundfont from disk. Implementations that do not use
    // SF may treat this as a no-op.
    virtual void LoadSoundFont(const std::string& path) = 0;

    // MIDI-like note events. channel index is implementation-defined; the
    // current FluidSynth backend exposes 64 channels. Standard MIDI drum
    // semantics are NOT pinned to channel 9 — drum vs melodic is decided
    // by the bank passed to ProgramChange (bank 128 = drum kit), and the
    // implementation flips the channel type per call.
    virtual void NoteOn(uint8_t channel, uint8_t note, uint8_t velocity) = 0;
    virtual void NoteOff(uint8_t channel, uint8_t note) = 0;

    // preset encodes both bank (high byte) and program (low byte).
    // The synth's preset lookup resolves this against the union of every
    // loaded soundfont — typically with last-loaded-wins precedence.
    virtual void ProgramChange(uint8_t channel, uint16_t preset) = 0;

    // Like ProgramChange but pins the channel to a SPECIFIC loaded soundfont via
    // its `sfontId`, bypassing the cross-soundfont preset lookup. Use when the
    // caller knows exactly which SF the preset must come from, even if another
    // loaded SF also has that (bank, program).
    //
    // Returns true when the pin succeeds (the sfontId is valid and has the
    // (bank, program) tuple), false otherwise. Failure signals the caller to fall
    // back to native synthesis for this entry.
    virtual bool ProgramSelect(uint8_t channel, int sfontId, uint16_t bank, uint16_t program) = 0;

    // semitones is a signed float: +1.0 = one semitone up. The
    // implementation owns the usable range and clamps out-of-range values
    // (the FluidSynth backend clamps to its configured pitch-wheel range,
    // approximately +/-12 semitones), so callers need not pre-clamp.
    virtual void PitchBend(uint8_t channel, float semitones) = 0;

    // Convenience: bend by a frequency RATIO instead of semitones.
    // 1.0 = no bend, 2.0 = +1 octave, 0.5 = -1 octave. Handy for engines
    // that express pitch as a frequency/resampling scale rather than in
    // semitones. Forwards to PitchBend, which owns the range clamp.
    void PitchBendFactor(uint8_t channel, float freqRatio) {
        PitchBend(channel, 12.0f * std::log2(freqRatio > 0.0f ? freqRatio : 1e-6f));
    }

    // Convenience: start a note already pitch-bent by `freqRatio` (same
    // convention as PitchBendFactor). The bend is applied BEFORE the NoteOn
    // so the voice attacks at the bent pitch in one step, rather than
    // sounding at concert pitch until the next bend update lands.
    void NoteOnPitchFactor(uint8_t channel, uint8_t note, uint8_t velocity, float freqRatio) {
        PitchBendFactor(channel, freqRatio);
        NoteOn(channel, note, velocity);
    }

    // Standard MIDI CC. value is 0-16383 (14-bit).
    virtual void ControlChange(uint8_t channel, uint8_t cc, uint16_t value) = 0;

    // Fill `out` with `frameCount` stereo interleaved float32 samples.
    // Called from the audio thread; must be real-time safe.
    virtual void Render(float* out, uint32_t frameCount) = 0;

    // Current number of audible voices held by the synth. Used by host UIs
    // as a real-time diagnostic — when this approaches GetPolyphonyLimit(),
    // new NoteOns will steal existing voices and the host can correlate
    // user-reported "cuts" with voice exhaustion. Implementations without
    // a voice pool may return 0.
    virtual uint32_t GetActiveVoiceCount() const = 0;
    virtual uint32_t GetPolyphonyLimit() const = 0;

    // Set the synth-wide master output gain (linear; 1.0 = unity). The host
    // uses this to apply a global volume fader to the synth's contribution
    // without rebuilding it — e.g. tracking a Master Volume slider. Safe to
    // call from the game thread. Implementations without a controllable master
    // gain may treat this as a no-op.
    virtual void SetMasterGain(float gain) {
    }
};

} // namespace SOH
