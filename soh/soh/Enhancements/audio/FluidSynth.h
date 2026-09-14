#pragma once
#if ENABLE_FLUIDSYNTH

#include "IMidiSynth.h"
#include <fluidsynth.h>
#include <mutex>
#include <vector>
#include <cstdint>

namespace SOH {

// Backend tuning supplied by the integrating game. FluidSynth has no opinion
// on what these should be for a given title, so they are parameters rather than
// hardcoded constants — a game sizes polyphony and gain for its own workload and
// mix. The defaults here are FluidSynth's own stock values, so a consumer that
// leaves a field untouched gets unsurprising upstream behavior.
struct FluidSynthConfig {
    // Audio output rate; must match the output device (typically 44100 or 48000).
    // Set before new_fluid_synth — the synth reads it once at construction.
    double sampleRate = 44100.0;

    // When true, install a softened volume curve: replaces the SF default
    // vel / CC7 / CC11 -> initial-attenuation modulators with versions that keep
    // the concave NEGATIVE shape but halve the amount (960 -> 480 cB). Maximum
    // attenuation drops from -96 dB to -48 dB, lifting quiet voices while
    // preserving dynamics shape. False preserves standard SF behavior.
    bool linearVelocity = false;

    // Maximum simultaneous voices. FluidSynth's stock default is 256; a game that
    // layers many SF voices or holds one-shot percussion voices can exhaust that
    // and drop notes. Idle voices cost almost nothing, so sizing up is cheap.
    int polyphony = 256;

    // Master output gain. FluidSynth's stock default is 0.2 — conservative to
    // avoid clipping when many voices sound at once. A game that mixes the synth
    // against a louder source may need to lift this so the two arrive balanced.
    double gain = 0.2;
};

class FluidSynth final : public IMidiSynth {
  public:
    explicit FluidSynth(const FluidSynthConfig& config);
    ~FluidSynth() override;

    // Single-shot replace: unloads every previously-loaded SF then loads
    // this one. Convenience wrapper over ClearSoundFonts + AddSoundFont*.
    void LoadSoundFont(const std::string& path) override;

    // Same shape as LoadSoundFont but takes an in-memory SF (e.g. one read
    // from a mounted .o2r archive). The buffer is copied into the synth's
    // internal storage so the caller may free their copy immediately.
    void LoadSoundFontFromMemory(const uint8_t* data, size_t size);

    // Add an SF alongside any already-loaded ones. FluidSynth's preset
    // lookup walks loaded soundfonts in REVERSE load order, so the most
    // recently added SF wins on (bank, program) collisions — matches the
    // "last loaded wins" semantics our mod stack uses elsewhere.
    //
    // Returns the FluidSynth sfont id on success, or FLUID_FAILED. The
    // memory variant copies the buffer into instance-owned storage and
    // routes through the mem:// sentinel; the path variant uses the
    // default filesystem loader.
    int AddSoundFont(const std::string& path);
    int AddSoundFontFromMemory(const uint8_t* data, size_t size);

    // Unload every loaded SF. Safe to call when none are loaded.
    void ClearSoundFonts();

    // Loaded SF ids in load order. Use to map a sfont id back to its
    // pack name on the caller side (the caller knows what it loaded;
    // FluidSynth only knows the opaque ids).
    std::vector<int> GetLoadedSfontIds();

    // One row per preset across every loaded SF (every sfont's full
    // preset list, in iteration order — which is generally the SF's
    // phdr order, grouped by sfont). Re-enumerated on demand; callers
    // typically cache the result and refresh when packs change.
    struct LoadedPreset {
        int sfontId;
        int bank;
        int program;
        std::string name;
    };
    std::vector<LoadedPreset> EnumerateLoadedPresets();
    void NoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override;
    void NoteOff(uint8_t channel, uint8_t note) override;
    void ProgramChange(uint8_t channel, uint16_t preset) override;
    bool ProgramSelect(uint8_t channel, int sfontId, uint16_t bank, uint16_t program) override;
    void PitchBend(uint8_t channel, float semitones) override;
    void ControlChange(uint8_t channel, uint8_t cc, uint16_t value) override;
    void Render(float* out, uint32_t frameCount) override;
    uint32_t GetActiveVoiceCount() const override;
    uint32_t GetPolyphonyLimit() const override;

    // Configure the synth-wide reverb. Safe to call any time after construction;
    // takes the synth mutex. Useful for per-mode presets — callers swap reverb
    // settings without having to rebuild the synth. Parameters mirror the
    // FluidSynth fluid_synth_set_reverb_* calls:
    //   roomsize : [0..1] perceived reverb tail length.
    //   damping  : [0..1] high-frequency damping.
    //   width    : [0..100] stereo spread.
    //   level    : [0..1] reverb wet level.
    void SetReverbParams(double roomsize, double damping, double width, double level);

    // Set FluidSynth's master output gain at runtime (forwards to
    // fluid_synth_set_gain). Lets the host track a global volume fader without
    // rebuilding the synth. Takes the synth mutex; safe any time after
    // construction. Mirrors FluidSynthConfig::gain, which sets the same knob at
    // construction.
    void SetMasterGain(float gain) override;

    // Pitch bend range in semitones sent to FluidSynth on channel init.
    // Must match what the MidiTranslator uses. Default: 12 semitones.
    static constexpr float kPitchBendRangeSemitones = 12.0f;

  private:
    void InitChannel(uint8_t channel);

    // Installs the softened volume curve on the freshly-created fluid_synth_t:
    // replaces the SF default vel/CC7/CC11 -> attenuation modulators with versions
    // at halved amount (480 cB). Must run after new_fluid_synth() but before any
    // LoadSoundFont() so SF instrument-level modulators layer correctly on top.
    void InstallLinearVelocityModulators();

    // Installs CC74 -> filter cutoff and CC71 -> filter resonance as default
    // modulators. Neither is an SF2.01 default, so without this the host's
    // Cutoff/Q sliders are inert on FluidSynth. Must run after new_fluid_synth()
    // but before any LoadSoundFont() so instrument-level modulators layer on top.
    void InstallFilterCcModulators();

    fluid_settings_t* mSettings = nullptr;
    fluid_synth_t* mSynth = nullptr;
    double mSampleRate;
    bool mLinearVelocity = false;

    // One entry per loaded SF, in load order. FluidSynth itself walks
    // loaded sfonts in reverse load order during preset lookup, so the
    // tail of this vector wins on collisions.
    std::vector<int> mSfontIds;

    // Backing storage for memory-loaded SFs, paired one-to-one with
    // mSfontIds entries. Filesystem-loaded SFs use the default loader
    // and the corresponding slot here stays empty. Buffers must outlive
    // the sfload call so the mem-sfloader's callbacks have stable data
    // for the duration of the load.
    std::vector<std::vector<uint8_t>> mLoadedBuffers;

    // Protects fluid_synth_* calls from concurrent access.
    // The audio thread calls Render(); the game thread calls NoteOn/Off/etc.
    mutable std::mutex mSynthMutex;

    // Which channels have had InitChannel() called. Sized to kNumChannels
    // so the translator's per-pair channel allocation can address all of
    // them; the synth setting is matched to this in the constructor.
    static constexpr int kNumChannels = 64;
    bool mChannelInited[kNumChannels] = {};
};

} // namespace SOH

#endif // ENABLE_FLUIDSYNTH
