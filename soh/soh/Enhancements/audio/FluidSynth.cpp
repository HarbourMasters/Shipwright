#if ENABLE_FLUIDSYNTH
#include "soh/Enhancements/audio/FluidSynth.h"
#include <spdlog/spdlog.h>
#include <cstring>
#include <algorithm>
#include <cstdio>

namespace SOH {

namespace {
// Memory-backed SF loader: a custom loader keyed on the sentinel "mem://current"
// so path- and memory-based loads coexist. The open callback has no user-data slot,
// so the buffer passes through one static slot -- safe because AddSoundFontFromMemory
// runs only on the GUI thread under the synth mutex.

struct MemoryInflight {
    const uint8_t* data = nullptr;
    size_t size = 0;
};
static MemoryInflight sMemoryInflight;

struct MemoryHandle {
    const uint8_t* data;
    size_t size;
    size_t pos;
};

constexpr const char* kMemorySentinel = "mem://current";

void* MemoryOpen(const char* filename) {
    if (filename == nullptr || std::strcmp(filename, kMemorySentinel) != 0) {
        return nullptr;
    }
    if (sMemoryInflight.data == nullptr || sMemoryInflight.size == 0) {
        return nullptr;
    }
    auto* h = new MemoryHandle{ sMemoryInflight.data, sMemoryInflight.size, 0 };
    // Single-shot: clear the slot so a stray repeat sfload can't replay.
    sMemoryInflight = {};
    return h;
}

int MemoryRead(void* buf, fluid_long_long_t count, void* handle) {
    auto* h = static_cast<MemoryHandle*>(handle);
    if (count < 0 || static_cast<size_t>(count) > h->size - h->pos) {
        return FLUID_FAILED;
    }
    std::memcpy(buf, h->data + h->pos, static_cast<size_t>(count));
    h->pos += static_cast<size_t>(count);
    return FLUID_OK;
}

int MemorySeek(void* handle, fluid_long_long_t offset, int origin) {
    auto* h = static_cast<MemoryHandle*>(handle);
    fluid_long_long_t newPos;
    switch (origin) {
        case SEEK_SET:
            newPos = offset;
            break;
        case SEEK_CUR:
            newPos = static_cast<fluid_long_long_t>(h->pos) + offset;
            break;
        case SEEK_END:
            newPos = static_cast<fluid_long_long_t>(h->size) + offset;
            break;
        default:
            return FLUID_FAILED;
    }
    if (newPos < 0 || static_cast<size_t>(newPos) > h->size) {
        return FLUID_FAILED;
    }
    h->pos = static_cast<size_t>(newPos);
    return FLUID_OK;
}

fluid_long_long_t MemoryTell(void* handle) {
    return static_cast<fluid_long_long_t>(static_cast<MemoryHandle*>(handle)->pos);
}

int MemoryClose(void* handle) {
    delete static_cast<MemoryHandle*>(handle);
    return FLUID_OK;
}

// ----------------------------------------------------------------------
// Route FluidSynth's own log output into the Ship logger.
//
// FluidSynth otherwise writes straight to stderr, bypassing our log sinks
// and level filtering. We forward each message at the matching spdlog level.
// ----------------------------------------------------------------------
fluid_log_function_t FluidLogToShip(int level) {
    switch (level) {
        case FLUID_PANIC:
            return [](int, const char* message, void*) { SPDLOG_CRITICAL("[FluidSynth] {}", message); };
        case FLUID_ERR:
            return [](int, const char* message, void*) { SPDLOG_ERROR("[FluidSynth] {}", message); };
        case FLUID_WARN:
            return [](int, const char* message, void*) { SPDLOG_WARN("[FluidSynth] {}", message); };
        case FLUID_INFO:
            return [](int, const char* message, void*) { SPDLOG_INFO("[FluidSynth] {}", message); };
        case FLUID_DBG:
            return [](int, const char* message, void*) { SPDLOG_DEBUG("[FluidSynth] {}", message); };
        default:
            return [](int, const char* message, void*) { SPDLOG_INFO("[FluidSynth] {}", message); };
    }
}
} // namespace

FluidSynth::FluidSynth(const FluidSynthConfig& config)
    : mSampleRate(config.sampleRate), mLinearVelocity(config.linearVelocity) {

    static std::once_flag once;
    std::call_once(once, [] {
        // Redirect fluidsynth logs to SPDLOG at equivalent level
        for (int level = 0; level < fluid_log_level::LAST_LOG_LEVEL; ++level) {
            fluid_set_log_function(level, FluidLogToShip(level), nullptr);
        }
    });

    mSettings = new_fluid_settings();
    // Sample rate MUST be set before new_fluid_synth — the synth reads it
    // once at construction.
    fluid_settings_setnum(mSettings, "synth.sample-rate", config.sampleRate);
    // 64 channels = enough headroom for the per-pair channel allocator in
    // MidiTranslator to give each (fontId, instOrWave) pair its own MIDI
    // channel, so per-pair effect CCs (CC91/93/74/71) don't stomp each
    // other. Must be a multiple of 16.
    fluid_settings_setint(mSettings, "synth.midi-channels", kNumChannels);
    // No FluidSynth audio driver exists (all compiled out); we pull samples via
    // Render() ourselves and never call new_fluid_audio_driver.

    // Master gain. Stock FluidSynth is 0.2.
    fluid_settings_setnum(mSettings, "synth.gain", config.gain);

    // Polyphony (max simultaneous voices). Stock FluidSynth is 256; the integrating
    // game sizes this for its workload (see FluidSynthConfig::polyphony). Undersizing
    // drops notes. FluidSynth frees each voice when its sample/envelope completes (no
    // leak) and idle voices are cheap, so a generous ceiling is fine -- e.g. when a
    // game layers a full melodic mapping plus voice-holding one-shot percussion.
    fluid_settings_setint(mSettings, "synth.polyphony", config.polyphony);

    mSynth = new_fluid_synth(mSettings);
    if (!mSynth) {
        SPDLOG_ERROR("[FluidSynth] Failed to create synth");
        return;
    }

    // Verify the sample rate FluidSynth actually locked in.
    double actualRate = 0.0;
    fluid_settings_getnum(mSettings, "synth.sample-rate", &actualRate);
    SPDLOG_INFO("[FluidSynth] Synth created. Requested sample rate={} actual={} linearVelocity={} "
                "polyphony={} gain={}",
                config.sampleRate, actualRate, mLinearVelocity, config.polyphony, config.gain);

    if (mLinearVelocity) {
        InstallLinearVelocityModulators();
    }

    // Mode-neutral: make the host's Cutoff/Q sliders (CC74/CC71) audible. Stock
    // FluidSynth has no default modulator for either, so without this they do nothing.
    InstallFilterCcModulators();

    // Register the memory-backed sound-font loader alongside the default
    // filesystem loader. Loaders are tried in addition order: default
    // catches real filesystem paths, ours catches the mem:// sentinel.
    // FluidSynth takes ownership of the loader and frees it via
    // delete_fluid_synth.
    fluid_sfloader_t* memLoader = new_fluid_defsfloader(mSettings);
    if (memLoader) {
        fluid_sfloader_set_callbacks(memLoader, MemoryOpen, MemoryRead, MemorySeek, MemoryTell, MemoryClose);
        fluid_synth_add_sfloader(mSynth, memLoader);
    } else {
        SPDLOG_WARN("[FluidSynth] Memory sound-font loader unavailable; "
                    "LoadSoundFontFromMemory will fall back to default loader");
    }
}

void FluidSynth::InstallLinearVelocityModulators() {
    // Adapted from ANMP (GPL-2, github.com/derselbst/ANMP): halve the SF default
    // velocity/CC7/CC11 -> attenuation modulators (960 -> 480 cB) to lift quiet
    // voices without flattening dynamics. remove+add_default_mod (not add OVERWRITE,
    // which needs an exact source-flag match); run after new_fluid_synth but before
    // any load so instrument-level modulators still layer on top.

    fluid_mod_t* mod = new_fluid_mod();
    if (!mod) {
        SPDLOG_ERROR("[FluidSynth] new_fluid_mod() failed; velocity modulators disabled");
        return;
    }

    constexpr int kHalfAttenuationCentibels = 480; // = 960 / 2

    fluid_mod_set_source2(mod, FLUID_MOD_NONE, 0);
    fluid_mod_set_dest(mod, GEN_ATTENUATION);
    fluid_mod_set_amount(mod, kHalfAttenuationCentibels);

    // 1. NoteOn velocity → initial attenuation (concave, halved).
    fluid_mod_set_source1(mod, FLUID_MOD_VELOCITY,
                          FLUID_MOD_GC | FLUID_MOD_CONCAVE | FLUID_MOD_UNIPOLAR | FLUID_MOD_NEGATIVE);
    fluid_synth_remove_default_mod(mSynth, mod);
    fluid_synth_add_default_mod(mSynth, mod, FLUID_SYNTH_OVERWRITE);

    // 2. CC7 (channel volume) → initial attenuation (concave, halved).
    fluid_mod_set_source1(mod, 7, FLUID_MOD_CC | FLUID_MOD_CONCAVE | FLUID_MOD_UNIPOLAR | FLUID_MOD_NEGATIVE);
    fluid_synth_remove_default_mod(mSynth, mod);
    fluid_synth_add_default_mod(mSynth, mod, FLUID_SYNTH_OVERWRITE);

    // 3. CC11 (expression) → initial attenuation (concave, halved).
    fluid_mod_set_source1(mod, 11, FLUID_MOD_CC | FLUID_MOD_CONCAVE | FLUID_MOD_UNIPOLAR | FLUID_MOD_NEGATIVE);
    fluid_synth_remove_default_mod(mSynth, mod);
    fluid_synth_add_default_mod(mSynth, mod, FLUID_SYNTH_OVERWRITE);

    delete_fluid_mod(mod);

    SPDLOG_INFO("[FluidSynth] velocity modulators installed (vel/CC7/CC11 concave x 0.5)");
}

void FluidSynth::InstallFilterCcModulators() {
    // GM2/GS map CC74 (Brightness) -> filter cutoff and CC71 (Harmonic Content) ->
    // filter resonance, but neither is an SF2.01 default modulator, so stock
    // FluidSynth ignores both and the host's Cutoff/Q sliders are inert. Add them
    // as default modulators. BIPOLAR, centered at CC 64: a value of 64 contributes
    // nothing (matches the host's neutral default), <64 darkens/relaxes, >64
    // brightens/sharpens. ADD (not OVERWRITE): these sources have no default mod, and
    // instrument-level SF mods still layer on top. Amounts are deliberately moderate
    // -- tune by ear.
    fluid_mod_t* mod = new_fluid_mod();
    if (!mod) {
        SPDLOG_ERROR("[FluidSynth] new_fluid_mod() failed; filter CC modulators disabled");
        return;
    }
    fluid_mod_set_source2(mod, FLUID_MOD_NONE, 0);

    // CC74 -> filter cutoff. +/-4800 cents (+/-4 octaves) full bipolar swing.
    fluid_mod_set_source1(mod, 74, FLUID_MOD_CC | FLUID_MOD_LINEAR | FLUID_MOD_BIPOLAR | FLUID_MOD_POSITIVE);
    fluid_mod_set_dest(mod, GEN_FILTERFC);
    fluid_mod_set_amount(mod, 4800);
    fluid_synth_add_default_mod(mSynth, mod, FLUID_SYNTH_ADD);

    // CC71 -> filter resonance (Q). +/-120 cB (+/-12 dB) full bipolar swing.
    fluid_mod_set_source1(mod, 71, FLUID_MOD_CC | FLUID_MOD_LINEAR | FLUID_MOD_BIPOLAR | FLUID_MOD_POSITIVE);
    fluid_mod_set_dest(mod, GEN_FILTERQ);
    fluid_mod_set_amount(mod, 120);
    fluid_synth_add_default_mod(mSynth, mod, FLUID_SYNTH_ADD);

    delete_fluid_mod(mod);
    SPDLOG_INFO("[FluidSynth] filter CC modulators installed (CC74->cutoff, CC71->Q)");
}

FluidSynth::~FluidSynth() {
    if (mSynth)
        delete_fluid_synth(mSynth);
    if (mSettings)
        delete_fluid_settings(mSettings);
}

void FluidSynth::ClearSoundFonts() {
    std::lock_guard<std::mutex> lock(mSynthMutex);
    if (!mSynth) {
        mSfontIds.clear();
        mLoadedBuffers.clear();
        return;
    }
    for (int id : mSfontIds) {
        if (id != FLUID_FAILED)
            fluid_synth_sfunload(mSynth, id, /*reset_presets=*/1);
    }
    mSfontIds.clear();
    mLoadedBuffers.clear();
    mLoadedBuffers.shrink_to_fit();
    // reset_presets above cleared channel state inside the synth, so the
    // RPN-0 (pitch bend range) push needs to repeat on the next NoteOn.
    for (bool& inited : mChannelInited)
        inited = false;
}

int FluidSynth::AddSoundFont(const std::string& path) {
    std::lock_guard<std::mutex> lock(mSynthMutex);
    if (!mSynth)
        return FLUID_FAILED;
    // reset_presets only on the FIRST sfont — for subsequent loads we
    // want preset assignments on existing channels left alone so a
    // stacked pack doesn't blow away the prior pack's program selection.
    int resetPresets = mSfontIds.empty() ? 1 : 0;
    int id = fluid_synth_sfload(mSynth, path.c_str(), resetPresets);
    if (id == FLUID_FAILED) {
        SPDLOG_ERROR("[FluidSynth] Failed to load SF: {}", path);
        return FLUID_FAILED;
    }
    SPDLOG_INFO("[FluidSynth] Loaded SF: {} (id={})", path, id);
    mSfontIds.push_back(id);
    mLoadedBuffers.emplace_back(); // empty — filesystem load owns its own data
    if (resetPresets) {
        for (bool& inited : mChannelInited)
            inited = false;
    }
    return id;
}

int FluidSynth::AddSoundFontFromMemory(const uint8_t* data, size_t size) {
    std::lock_guard<std::mutex> lock(mSynthMutex);
    if (!mSynth || data == nullptr || size == 0)
        return FLUID_FAILED;

    // Pre-reserve the slot so we can hand its address through the static
    // sMemoryInflight pointer for the duration of sfload. Vector growth
    // is fine here because the SF buffer lives in the vector element,
    // which is itself a vector<uint8_t> (small, by-value relocations
    // don't invalidate the underlying heap-allocated data).
    mLoadedBuffers.emplace_back(data, data + size);
    auto& buf = mLoadedBuffers.back();
    sMemoryInflight = { buf.data(), buf.size() };
    int resetPresets = mSfontIds.empty() ? 1 : 0;
    int id = fluid_synth_sfload(mSynth, kMemorySentinel, resetPresets);
    sMemoryInflight = {};
    if (id == FLUID_FAILED) {
        SPDLOG_ERROR("[FluidSynth] Failed to load SF from memory ({} bytes)", size);
        mLoadedBuffers.pop_back();
        return FLUID_FAILED;
    }
    SPDLOG_INFO("[FluidSynth] Loaded SF from memory ({} bytes, id={})", size, id);
    mSfontIds.push_back(id);
    if (resetPresets) {
        for (bool& inited : mChannelInited)
            inited = false;
    }
    return id;
}

std::vector<int> FluidSynth::GetLoadedSfontIds() {
    std::lock_guard<std::mutex> lock(mSynthMutex);
    return mSfontIds;
}

std::vector<FluidSynth::LoadedPreset> FluidSynth::EnumerateLoadedPresets() {
    std::lock_guard<std::mutex> lock(mSynthMutex);
    std::vector<LoadedPreset> result;
    if (!mSynth)
        return result;
    for (int id : mSfontIds) {
        if (id == FLUID_FAILED)
            continue;
        fluid_sfont_t* sfont = fluid_synth_get_sfont_by_id(mSynth, id);
        if (!sfont)
            continue;
        fluid_sfont_iteration_start(sfont);
        while (fluid_preset_t* preset = fluid_sfont_iteration_next(sfont)) {
            LoadedPreset p;
            p.sfontId = id;
            p.bank = fluid_preset_get_banknum(preset);
            p.program = fluid_preset_get_num(preset);
            const char* nm = fluid_preset_get_name(preset);
            p.name = nm ? nm : "";
            result.push_back(std::move(p));
        }
    }
    return result;
}

void FluidSynth::LoadSoundFont(const std::string& path) {
    ClearSoundFonts();
    AddSoundFont(path);
}

void FluidSynth::LoadSoundFontFromMemory(const uint8_t* data, size_t size) {
    ClearSoundFonts();
    AddSoundFontFromMemory(data, size);
}

void FluidSynth::InitChannel(uint8_t channel) {
    if (mChannelInited[channel])
        return;
    mChannelInited[channel] = true;

    int ch = static_cast<int>(channel);

    // Set pitch-bend range via the dedicated API. The MIDI-spec equivalent (CC
    // 101/100/6/38 RPN sequence) has subtle behavior differences across FluidSynth
    // versions; the direct semitone setter avoids the ambiguity.
    fluid_synth_pitch_wheel_sens(mSynth, ch, static_cast<int>(kPitchBendRangeSemitones));

    // fluid_synth_set_gen() applies an additive (NRPN-style) offset on top of the
    // SF zone value rather than overriding it, and the absolute sibling set_gen2()
    // isn't in the 2.5.2 public API. So baked LFO-to-pitch can't be silenced
    // channel-wide; it's patched per-voice on NoteOn (common case) or at SF load
    // time. See NoteOn().
}

void FluidSynth::NoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    std::lock_guard<std::mutex> lock(mSynthMutex);
    if (!mSynth)
        return;
    InitChannel(channel);
    int result = fluid_synth_noteon(mSynth, channel, note, velocity);
    SPDLOG_TRACE("[FluidSynth] NoteOn ch={} note={} vel={} sfonts={} result={}", channel, note, velocity,
                 mSfontIds.size(), result);

    // Suppress SF-author-baked LFO-to-pitch on the voices we just started.
    // fluid_voice_gen_set() writes the generator's `val` field directly (the SF
    // zone value), and final = val + mod + nrpn, so zeroing val drops the SF's
    // contribution. Per-voice patching is the only public path that works, since
    // the channel-wide set_gen is additive and set_gen2 isn't in the public API.
    fluid_voice_t* voices[256];
    fluid_synth_get_voicelist(mSynth, voices, 256, -1);
    for (int i = 0; i < 256 && voices[i] != nullptr; ++i) {
        if (fluid_voice_get_channel(voices[i]) != channel)
            continue;
        if (!fluid_voice_is_playing(voices[i]))
            continue;
        fluid_voice_gen_set(voices[i], GEN_VIBLFOTOPITCH, 0.0f);
        fluid_voice_gen_set(voices[i], GEN_MODLFOTOPITCH, 0.0f);
        fluid_voice_update_param(voices[i], GEN_VIBLFOTOPITCH);
        fluid_voice_update_param(voices[i], GEN_MODLFOTOPITCH);
    }
}

void FluidSynth::NoteOff(uint8_t channel, uint8_t note) {
    std::lock_guard<std::mutex> lock(mSynthMutex);
    if (!mSynth)
        return;
    fluid_synth_noteoff(mSynth, channel, note);
}

void FluidSynth::ProgramChange(uint8_t channel, uint16_t preset) {
    std::lock_guard<std::mutex> lock(mSynthMutex);
    if (!mSynth)
        return;
    InitChannel(channel);

    int bank = (preset >> 8) & 0xFF;
    int program = preset & 0xFF;

    SPDLOG_TRACE("[FluidSynth] ProgramChange ch={} bank={} program={}", channel, bank, program);

    if (bank == 128) {
        fluid_synth_set_channel_type(mSynth, channel, CHANNEL_TYPE_DRUM);
        fluid_synth_bank_select(mSynth, channel, 128);
    } else {
        fluid_synth_set_channel_type(mSynth, channel, CHANNEL_TYPE_MELODIC);
        fluid_synth_bank_select(mSynth, channel, bank);
    }

    fluid_synth_program_change(mSynth, channel, program);
}

bool FluidSynth::ProgramSelect(uint8_t channel, int sfontId, uint16_t bank, uint16_t program) {
    std::lock_guard<std::mutex> lock(mSynthMutex);
    if (!mSynth)
        return false;
    InitChannel(channel);

    // Verify the sfontId is one we loaded — fluid_synth_program_select
    // would also reject an unknown id but its log goes through
    // FluidSynth's own logger rather than ours; pre-check so we can
    // emit our SPDLOG path uniformly.
    bool known = false;
    for (int id : mSfontIds) {
        if (id == sfontId) {
            known = true;
            break;
        }
    }
    if (!known) {
        SPDLOG_TRACE("[FluidSynth] ProgramSelect ch={} sfontId={} not loaded; rejecting pin", channel, sfontId);
        return false;
    }

    // Set drum/melodic type before the select — bank 128 is the GM
    // percussion convention and FluidSynth's voice allocator branches
    // on channel type, not on the bank we're selecting into.
    if (bank == 128) {
        fluid_synth_set_channel_type(mSynth, channel, CHANNEL_TYPE_DRUM);
    } else {
        fluid_synth_set_channel_type(mSynth, channel, CHANNEL_TYPE_MELODIC);
    }

    int result = fluid_synth_program_select(mSynth, channel, static_cast<unsigned int>(sfontId),
                                            static_cast<unsigned int>(bank), static_cast<unsigned int>(program));
    if (result != FLUID_OK) {
        SPDLOG_TRACE("[FluidSynth] ProgramSelect ch={} sfontId={} bank={} prog={} -> FAILED", channel, sfontId, bank,
                     program);
        return false;
    }
    SPDLOG_TRACE("[FluidSynth] ProgramSelect ch={} sfontId={} bank={} prog={} -> OK", channel, sfontId, bank, program);
    return true;
}

void FluidSynth::PitchBend(uint8_t channel, float semitones) {
    std::lock_guard<std::mutex> lock(mSynthMutex);
    if (!mSynth)
        return;
    float ratio = semitones / kPitchBendRangeSemitones;
    int val = static_cast<int>(ratio * 8192.0f) + 8192;
    val = std::clamp(val, 0, 16383);
    fluid_synth_pitch_bend(mSynth, channel, val);
}

void FluidSynth::ControlChange(uint8_t channel, uint8_t cc, uint16_t value) {
    std::lock_guard<std::mutex> lock(mSynthMutex);
    if (!mSynth)
        return;
    fluid_synth_cc(mSynth, channel, cc, (value >> 7) & 0x7F);
}

void FluidSynth::SetReverbParams(double roomsize, double damping, double width, double level) {
    std::lock_guard<std::mutex> lock(mSynthMutex);
    if (!mSynth)
        return;
    fluid_synth_set_reverb_group_roomsize(mSynth, -1, roomsize);
    fluid_synth_set_reverb_group_damp(mSynth, -1, damping);
    fluid_synth_set_reverb_group_width(mSynth, -1, width);
    fluid_synth_set_reverb_group_level(mSynth, -1, level);
    SPDLOG_INFO("[FluidSynth] Reverb set: roomsize={} damping={} width={} level={}", roomsize, damping, width, level);
}

void FluidSynth::SetMasterGain(float gain) {
    std::lock_guard<std::mutex> lock(mSynthMutex);
    if (!mSynth)
        return;
    fluid_synth_set_gain(mSynth, gain);
}

void FluidSynth::Render(float* out, uint32_t frameCount) {
    std::lock_guard<std::mutex> lock(mSynthMutex);
    if (!mSynth || mSfontIds.empty()) {
        std::memset(out, 0, frameCount * 2 * sizeof(float));
        return;
    }

    fluid_synth_write_float(mSynth, static_cast<int>(frameCount), out, 0, 2, out, 1, 2);
}

uint32_t FluidSynth::GetActiveVoiceCount() const {
    std::lock_guard<std::mutex> lock(mSynthMutex);
    if (!mSynth)
        return 0;
    int n = fluid_synth_get_active_voice_count(mSynth);
    return n < 0 ? 0u : static_cast<uint32_t>(n);
}

uint32_t FluidSynth::GetPolyphonyLimit() const {
    std::lock_guard<std::mutex> lock(mSynthMutex);
    if (!mSynth)
        return 0;
    int n = fluid_synth_get_polyphony(mSynth);
    return n < 0 ? 0u : static_cast<uint32_t>(n);
}

} // namespace SOH
#endif // ENABLE_FLUIDSYNTH
