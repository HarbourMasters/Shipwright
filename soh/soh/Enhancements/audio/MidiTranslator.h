#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace SOH {

// Per-channel state tracked across notes on the same MIDI channel.
// 0xFF / 0xFFFF sentinels mean "never sent" so the first NoteOn always
// emits a fresh ControlChange/ProgramChange.
struct ChannelState {
    bool inited = false;
    uint16_t lastPreset = 0xFFFF; // last ProgramChange value (bank<<8|prog)
    int16_t lastPinSfont = -1;    // last ProgramSelect sfontId; -1 = unpinned
    uint8_t lastVolumeCC = 0xFF;  // last CC 7 (channel volume) sent, 7-bit
    uint8_t lastPanCC = 0xFF;     // last CC 10 (pan) sent, 7-bit
    uint8_t lastReverbCC = 0xFF;  // CC 91 (reverb send)
    uint8_t lastChorusCC = 0xFF;  // CC 93 (chorus send)
    uint8_t lastCutoffCC = 0xFF;  // CC 74 (low-pass filter cutoff)
    uint8_t lastQCC = 0xFF;       // CC 71 (low-pass filter resonance)
};

// Which path a translator slot is currently routing audio through. Slots
// transition Idle -> (Synth | Native) on first sight and back to Idle on
// NoteDisabled.
enum class SlotKind : uint8_t {
    Idle = 0,
    Synth = 1,
    Native = 2,
};

// Per-note state tracked between Audio_ProcessNotes calls.
struct NoteTranslatorState {
    SlotKind kind = SlotKind::Idle;
    uint8_t channel = 0;
    uint8_t midiNote = 0;
    uint8_t pairFontId = 0;
    int16_t pairInstOrWave = -1;
    float lastFreqScale = 0.0f;
    // Entry this note resolved to, so retire/NoteDisabled can decrement the
    // right per-entry active counter. -1 = native fall-through (no entry).
    int16_t activeEntryIdx = -1;
};

// Overall translator behaviour, set from AudioEditor and read by ProcessNote.
enum class SynthMode : uint8_t {
    Authentic = 0, // richer modulators + console-era reverb
    Enhanced = 1,  // stock SF modulators + low default reverb
};

// One entry per (fontId, instOrWave) pair the translator has actually
// seen. mapped indicates the GM mapping table had a non-kUnmapped value
// (UI hint only since the new resolution model doesn't use the table).
struct DiscoveredPair {
    uint8_t fontId;
    int16_t instOrWave;
    bool mapped;
};

// Where a ConfigEntry came from. Decides persistence eligibility and
// whether the entry is removed when its pack is unloaded.
enum class EntrySource : uint8_t {
    UserPicked = 0,  // From fluidsynth_overrides.json or a UI pick
    ModSupplied = 1, // From a pack mapping.json; not persisted, removed on
                     // pack unload
};

// Where a resolved (split) entry sends its notes once it wins resolution.
// Synth is the default — the entry references a (pack, program) FluidSynth
// plays. Native makes the entry a first-class winner whose audible path is
// the engine sample (a split range that should keep the native instrument).
// Mute is the silent "None" placeholder scoped to the range. Native vs Mute
// both lack a pack/program, so the route field is what distinguishes them.
enum class EntryRoute : uint8_t {
    Synth = 0,
    Native = 1,
    Mute = 2,
};

// One row in the configuration. Multiple entries can share
// (fontId, instOrWave); each represents a distinct candidate keyed by
// (pack, program). Resolution at play time picks the enabled+resolvable
// candidate whose pack is last in the load order.
//
// IMPORTANT: the audio thread reads primitive fields of this struct via
// mEntries[mActiveEntryIdx[f][i]] without locking. mEntries is reserved
// at construction so push_back never reallocates. Strings are NOT touched
// from the audio thread.
struct ConfigEntry {
    uint8_t fontId = 0;
    int16_t instOrWave = -1;
    std::string packName; // "" = placeholder ("None") entry
    int16_t program = -1; // -1 = placeholder (no audible output)
    int16_t bank = 0;
    std::string presetName; // user-facing label / recovery hint
    float gain = 0.0f;      // 0.0 = use default 1.0x
    int8_t transpose = 0;
    int8_t reverb = -1; // CC 91; -1 = no override (default 0)
    int8_t chorus = -1; // CC 93; -1 = no override (default 0)
    int8_t cutoff = -1; // CC 74; -1 = no override (default 64)
    int8_t q = -1;      // CC 71; -1 = no override (default 64)
    bool enabled = false;
    bool selected = false;
    uint32_t lastEnabledSeq = 0; // process-lifetime; not persisted
    EntrySource source = EntrySource::UserPicked;
    // ── Note-range split (engine-semitone space) ─────────────────────────
    // noteLow/noteHigh bound which engine `semitone` values this entry claims
    // (inclusive); default 0..127 is an unsplit entry. fixedNote pins the OUTPUT
    // note: -1 derives it from `semitone` (normal melodic path), >= 0 plays that
    // exact MIDI note. So the slot index selects WHICH entry wins; fixedNote
    // selects WHAT it plays. route dispatches the winner (synth / sample / silent).
    uint8_t noteLow = 0;
    uint8_t noteHigh = 127;
    int16_t fixedNote = -1;
    EntryRoute route = EntryRoute::Synth;
    // Runtime: resolved sfontId for ProgramSelect. -1 when the pack isn't
    // currently loaded OR the SF doesn't have (bank, program).
    // Refreshed by AudioEditor's pack-apply path; never persisted.
    int16_t sfontId = -1;
    // Runtime: forward link of the active-split chain for this pair, sorted by
    // resolution priority (source > specificity > pack order), so the audio
    // thread's first-covering-entry walk reproduces the per-semitone winner even
    // when ranges overlap (-1 = chain tail). Built by RecomputeActive; walked by
    // index over the reserved mEntries vector. Acyclic (each entry linked at most
    // once). Never serialised.
    int16_t nextActiveSplit = -1;
};

class MidiTranslator {
  public:
    static MidiTranslator& Instance();

    // Audio-thread entry point (see audio_playback.c hook).
    bool ProcessNote(int noteIndex, float freqScale, float velocity, uint8_t pan, float channelVolume, uint8_t fontId,
                     int16_t instOrWave, uint8_t semitone, bool isFinished, uint8_t channelIdx, float resampleRate,
                     float pitchBend);
    void NoteDisabled(int noteIndex);
    void Reset();

    // Number of MIDI channels currently held by synth-routed pairs, out of
    // kMaxMidiChannels. If this reaches the cap, new pairs share channel 0 and
    // some instruments go silent. Game-thread read of an audio-thread counter,
    // intentionally unsynchronised (a stale value is fine for a UI gauge).
    uint8_t GetChannelsInUse() const;
    // Times the channel pool recycled an idle pair's channel; nonzero means
    // reclamation is working (no collapse onto channel 0). Companion gauge to
    // GetChannelsInUse() for the FluidSynth tab.
    uint32_t GetChannelReclaims() const;

    static constexpr int kMaxNotes = 64;
    static constexpr int kMaxFontId = 64;
    static constexpr int kMaxInstOrWave = 256;
    static constexpr int kMaxDiscovered = 128;
    static constexpr uint8_t kMaxMidiChannels = 64;
    // mEntries capacity reserved at construction so push_back never
    // reallocates within session bounds. The audio thread reads entry
    // fields by index without locking; reallocation would invalidate
    // those reads.
    static constexpr size_t kMaxEntries = 4096;

    // ── Discovery snapshot for the UI ────────────────────────────────────
    int DiscoveredSnapshot(DiscoveredPair* out, int outCap) const;
    void ClearDiscovered();
    uint8_t GetSynthActiveCount(uint8_t fontId, int16_t instOrWave) const;
    uint8_t GetNativeActiveCount(uint8_t fontId, int16_t instOrWave) const;
    // Per-entry (per split row) active counts, for the row activity tint.
    // A note attributes to the entry it resolved to, so a single drum slot
    // or melodic range lights up independently of the rest of its pair.
    uint8_t GetEntrySynthActive(int idx) const;
    uint8_t GetEntryNativeActive(int idx) const;
    // Aggregate per-entry activity over all of a pair's entries, to tint a SPLIT
    // parent row from its children: green if any child is synth-active, blue if any
    // is native-active and none synth-active, else uncoloured. Unlike the per-pair
    // counts above, this only sees children that attributed to an entry.
    void GetPairEntryActivity(uint8_t fontId, int16_t instOrWave, bool& anySynth, bool& anyNative) const;

    // DEBUG: per-pair running stats accumulated by ProcessNote, surfaced in the
    // AudioEditor's per-row "[DBG]" popup. Read by the UI without locking;
    // best-effort, a torn read is harmless cosmetic.
    struct DebugPairStats {
        uint32_t noteOns = 0;
        uint32_t routedSynth = 0;
        uint32_t routedNative = 0;
        uint32_t routedMute = 0; // bank == 128 / engine drum-SFX / placeholder
        uint8_t lastSemitone = 0;
    };
    DebugPairStats GetDebugStats(uint8_t fontId, int16_t instOrWave) const;
    void ResetDebugStats();
    void ResetDebugStatsForPair(uint8_t fontId, int16_t instOrWave);

    // Per-slot NoteOn histogram for the engine drum (instOrWave 0) and SFX
    // (instOrWave 1) channels, where the `semitone` byte is a slot index
    // (Audio_GetDrum / Audio_GetSfx), not a pitch. Fills out[0..127] with per-slot
    // counts and returns the number of distinct slots that fired; the discovery
    // input for the drum auto-split. Returns 0 for non-drum/SFX pairs. Lock-free
    // read of audio-thread atomics.
    int GetDrumSlotHistogram(uint8_t fontId, int16_t instOrWave, uint32_t out[128]) const;
    // Recently-played signals for drum slot rows without a ConfigEntry (the
    // per-entry active counters can't cover them). Window ~250ms.
    bool DrumSlotRecentlyActive(uint8_t fontId, int16_t instOrWave, uint8_t slot) const;
    bool AnyDrumSlotRecentlyActive(uint8_t fontId, int16_t instOrWave) const;

    // ── Pack stack + entry sfontId refresh ───────────────────────────────
    // Set by AudioEditor after every SF stack change. Used as the
    // multi-enabled tiebreak rank — last entry in this list wins.
    void SetPackLoadOrder(const std::vector<std::string>& order);

    // Refresh each entry's runtime sfontId from a flat list of (pack, sfontId,
    // bank, program) tuples. Entries whose pack isn't listed, or whose (bank,
    // program) isn't in the matched SF, get sfontId=-1. Cached so PickPreset /
    // ClickSynth can resolve sfontIds for new entries without a UI callback.
    struct LoadedPresetRef {
        int sfontId;
        std::string packName;
        int bank;
        int program;
    };
    void RefreshEntrySfontIds(const std::vector<LoadedPresetRef>& loadedPresets);

    // Drop every ModSupplied entry whose pack is NOT in the supplied set,
    // so pack toggles don't leave orphan mod entries hanging with
    // sfontId=-1. User-saved entries stay (they're recoverable).
    void RemoveModEntriesNotIn(const std::set<std::string>& packNamesLoaded);

    // Rebuild mActiveEntryIdx for every (fontId, instOrWave). Called after
    // batches of mutations (pack load, file load).
    void RecomputeAllActive();

    // ── Entry queries (UI) ───────────────────────────────────────────────
    int GetEntryCount() const {
        return static_cast<int>(mEntries.size());
    }
    const ConfigEntry& GetEntry(int idx) const {
        return mEntries[idx];
    }
    const ConfigEntry* GetActiveEntry(uint8_t fontId, int16_t instOrWave) const;
    int GetActiveEntryIdx(uint8_t fontId, int16_t instOrWave) const;
    void GetEntriesForPair(uint8_t fontId, int16_t instOrWave, std::vector<int>& outIdx) const;
    int FindEntry(uint8_t fontId, int16_t instOrWave, const std::string& pack, int16_t program,
                  uint8_t noteLow = 0) const;
    int CountSelectedEntries(uint8_t fontId, int16_t instOrWave) const;

    // ── Row UI actions ───────────────────────────────────────────────────
    // Picking a preset from the Preset combo: disable every currently-
    // enabled entry for (fontId, instOrWave); if the previous winner is
    // from the same pack as the new pick clear its selected; find-or-
    // create the picked entry; enable + select + bump lastEnabledSeq.
    void PickPreset(uint8_t fontId, int16_t instOrWave, const std::string& pack, int16_t program, int16_t bank,
                    const std::string& presetName);
    // Disables every enabled entry for (fontId, instOrWave). selected
    // flags are preserved so ClickSynth can restore.
    void ClickNative(uint8_t fontId, int16_t instOrWave);
    // Restoration order:
    //   1. selected entries for (fontId, instOrWave) -> highest seq wins
    //   2. fallback: any disabled-but-resolvable entry (sfontId >= 0) ->
    //      highest seq wins (handles the mod-only-row case where the
    //      user never picked anything)
    //   3. placeholder { packName="", program=-1, presetName="None" }
    //      (audible result: muted)
    void ClickSynth(uint8_t fontId, int16_t instOrWave);

    // Drum auto-split: for each engine slot the drum/SFX channel actually
    // fired (from the GetDrumSlotHistogram data), create a single-slot entry
    // (noteLow==noteHigh==slot) pinned to a default bank-128 kit (the first
    // loaded percussion preset) with a placeholder GM percussion note. The
    // user then refines the kit / Drum Sound per slot. Re-running it re-enables
    // existing slot entries rather than duplicating them. No-op outside the
    // drum/SFX channels.
    void AutoSplitDrums(uint8_t fontId, int16_t instOrWave);

    // Manually create a single drum slot at `slot` (engine-semitone index) for a
    // drum/SFX or forced-drum pair, without waiting for runtime discovery. Mirrors
    // one AutoSplitDrums row: pinned to the pair's kit, Native by default (the user
    // picks a Drum Sound to make it synth). Reuses an existing slot entry rather
    // than duplicating. Lets a pair be configured offline (e.g. a forced-drum pair
    // whose song hasn't played yet). No-op outside drum-like pairs.
    void AddDrumSlot(uint8_t fontId, int16_t instOrWave, uint8_t slot);

    // Drum channel Mode (master switch). Sets an explicit per-pair flag,
    // separate from per-slot `enabled`, so toggling it never rewrites the
    // per-slot state. Native (default) => every slot plays the engine drum
    // regardless of per-slot enabled (and the UI grays the slot controls);
    // Synth => per-slot enabled decides synth vs native. Switching to Synth
    // with no slots yet auto-splits to populate the rows (created Native by
    // default). IsDrumChannelSynth reads the flag (audio-thread safe).
    void SetDrumChannelSynth(uint8_t fontId, int16_t instOrWave, bool synth);
    bool IsDrumChannelSynth(uint8_t fontId, int16_t instOrWave) const;
    // "Treat as drum": generalises the drum master from the intrinsic drum/SFX
    // channels (instOrWave 0/1) to any melodic pair (>= 2). A flagged pair runs the
    // drum resolution path: each distinct incoming semitone becomes a slot mapped to
    // a GM percussion sound (fixedNote). Flagging on == Synth mode; clearing returns
    // the pair to melodic routing. IsForcedDrum reads the flag (audio-thread safe).
    // No-op for instOrWave 0/1 (use SetDrumChannelSynth).
    void SetForcedDrum(uint8_t fontId, int16_t instOrWave, bool forced);
    bool IsForcedDrum(uint8_t fontId, int16_t instOrWave) const;
    // Set the kit (bank-128 pack/program) for every selected slot entry of a
    // drum pair at once -- the GM percussion note stays per slot (fixedNote).
    void SetDrumKit(uint8_t fontId, int16_t instOrWave, const std::string& pack, int16_t program,
                    const std::string& presetName);

    // ── Note-range split editing (melodic) ───────────────────────────────
    // SplitEntry bisects entry idx's range at `atSemitone`: idx keeps
    // [noteLow, atSemitone-1], a copy takes [atSemitone, noteHigh]. Returns the new
    // sibling index, or -1 if the cut is out of range / pool full. MergeWithNext
    // absorbs the adjacent higher range back into idx. SetEntryNoteRange sets an
    // entry's [lo,hi]. AutoSplitByEngineRanges splits a melodic pair's active entry
    // into the engine's low/normal/high ranges (boundaries from InstrumentNames).
    int SplitEntry(int idx, uint8_t atSemitone);
    // Split entry idx into `parts` (>=2) near-equal contiguous ranges in one shot
    // (the UI's "split into N"), clamped so no range empties. Returns idx (the
    // lowest range). Repeated bisection via SplitEntry, so siblings inherit the
    // preset; the user reassigns each range after.
    int SplitEntryEven(int idx, int parts);
    void MergeWithNext(int idx);
    void SetEntryNoteRange(int idx, uint8_t noteLow, uint8_t noteHigh);
    // Move the single boundary between two adjacent split ranges atomically:
    // lowerIdx keeps [noteLow .. boundary], upperIdx takes [boundary+1 .. noteHigh],
    // clamped so neither range empties. One chain recompute, so the audio thread
    // never sees a transient overlap/gap. The only way the split UI edits ranges,
    // keeping them contiguous and non-overlapping with the outer ends pinned.
    void SetSplitBoundary(int lowerIdx, int upperIdx, uint8_t boundary);
    void AutoSplitByEngineRanges(uint8_t fontId, int16_t instOrWave);
    // Snap a melodic pair's selected ranges to a gapless tiling of [0,127],
    // retiring rows squeezed out. Idempotent; drum/SFX pairs are left untouched.
    void NormalizeSplits(uint8_t fontId, int16_t instOrWave);
    // Collapse a melodic pair back to a single full-range entry.
    void FlattenSplits(uint8_t fontId, int16_t instOrWave);
    // Set a specific entry's melodic preset (used by a split range row's preset
    // dropdown -- scoped to that range, unlike PickPreset which is per-pair).
    // Enables + selects the entry, route=Synth, fixedNote=-1 (pitch from
    // semitone). Pass program<0 / empty pack to leave it (use SetEntryEnabled
    // false for "None/native").
    void SetEntryPreset(int idx, const std::string& pack, int16_t program, int16_t bank, const std::string& presetName);

    // ── Per-entry mutators (UI Override widgets) ─────────────────────────
    void SetEntryGain(int idx, float gain);
    void SetEntryTranspose(int idx, int8_t semis);
    void SetEntryReverb(int idx, int8_t cc);
    void SetEntryChorus(int idx, int8_t cc);
    void SetEntryFilterCutoff(int idx, int8_t cc);
    void SetEntryFilterResonance(int idx, int8_t cc);
    // Split-row mutators. fixedNote pins the output note (-1 = derive from
    // semitone); route switches a split between synth / engine-sample /
    // silent; enabled drops a split out of resolution (its slot falls back to
    // native) without deleting it. route/enabled recompute the pair's chain.
    void SetEntryFixedNote(int idx, int16_t note);
    void SetEntryRoute(int idx, EntryRoute route);
    void SetEntryEnabled(int idx, bool enabled);

    // ── Per-pair display name (row label, sparse) ────────────────────────
    std::string GetDisplayName(uint8_t fontId, int16_t instOrWave) const;
    void SetDisplayName(uint8_t fontId, int16_t instOrWave, const std::string& name);

    // ── Session-only transient state ─────────────────────────────────────
    bool IsTemporarilyMuted(uint8_t fontId, int16_t instOrWave) const;
    void SetTemporaryMute(uint8_t fontId, int16_t instOrWave, bool muted);
    void ClearAllTemporaryMutes();
    // Per-split transient mute, keyed by (font, inst, noteLow) so it survives
    // entry reordering. Lets a single drum slot / melodic range be isolated.
    // Checked after resolution: a muted split silences both paths for notes
    // whose covering entry starts at noteLow.
    bool IsTemporarilySlotMuted(uint8_t fontId, int16_t instOrWave, uint8_t noteLow) const;
    void SetTemporarySlotMute(uint8_t fontId, int16_t instOrWave, uint8_t noteLow, bool muted);
    void ClearAllTemporarySlotMutes();
    float GetTemporaryVolume(uint8_t fontId, int16_t instOrWave) const;
    void SetTemporaryVolume(uint8_t fontId, int16_t instOrWave, float vol);
    void ClearAllTemporaryVolumes();

    // ── Persistence + reset ──────────────────────────────────────────────
    void ResetAllOverrides();
    bool SaveOverridesToFile(const std::string& path) const;
    // String overlay = pack mapping.json: new entries land with source=ModSupplied,
    // selected=false, enabled=true. `packName`, when non-empty, is the pack's
    // discovered name and the authoritative owner for every entry, so mapping.json
    // needn't repeat a per-entry "pack" field. When empty, the owner falls back to
    // the per-entry "pack", then the file's top-level "pack_name".
    bool ApplyOverridesFromString(const std::string& json, const std::string& packName = "");
    // File overlay = user fluidsynth_overrides.json: entries land with
    // source=UserPicked. enabled / selected come from the file; missing
    // fields default to true.
    bool ApplyOverridesFromFile(const std::string& path);

    // Publish-as-pack-mapping. Writes a mapping.json shaped for distribution
    // inside a synth pack: only enabled+selected entries belonging to
    // `packNameFilter` are emitted, runtime fields are stripped, and the
    // file carries a top-level "pack_name" field. Returns the number of
    // entries written (or -1 on I/O failure). When packNameFilter is empty,
    // every selected entry is emitted regardless of its source pack.
    int ExportPackMapping(const std::string& path, const std::string& packNameFilter) const;

    // Same content ExportPackMapping writes, built in memory (no file I/O).
    // `outWritten` receives the entry count. Used by the one-click .o2r
    // bundler so the JSON and the soundfont go straight into the archive.
    std::string BuildPackMappingJson(const std::string& packNameFilter, int& outWritten) const;

    // Helper: how many entries `ExportPackMapping` would emit for a given
    // pack filter. Same predicate as the writer; lets the UI show a preview
    // count before the user clicks Export.
    int CountExportableEntries(const std::string& packNameFilter) const;

    void SetSynthMode(SynthMode mode) {
        mSynthMode = mode;
    }
    SynthMode GetSynthMode() const {
        return mSynthMode;
    }

    // Effective global synth gain, configured upfront by the AudioEditor.
    void SetGlobalGain(float gain) {
        mGlobalGain = gain;
    }

  private:
    MidiTranslator();

    // Entries whose font (identified by resource name) isn't loaded this session,
    // kept as raw JSON so they survive saves untouched and spring back when the
    // owning mod returns. Their instruments play native meanwhile.
    std::vector<std::string> mParkedEntries;
    std::vector<std::string> mParkedDrumFlags;
    std::vector<std::string> mParkedForcedDrums;

    void RecordDiscovery(uint8_t fontId, int16_t instOrWave, bool mapped);
    // Balanced inc/dec of the per-entry active counters from ProcessNote's
    // activation paths and retire/NoteDisabled. idx<0 (native fall-through) is
    // a no-op so error/fallback paths can pass -1 freely.
    void IncEntryActive(bool synth, int idx);
    void DecEntryActive(bool synth, int idx);
    // Current kit (pack/program) for a drum pair: an existing selected bank-128
    // slot entry if any, else the first loaded bank-128 preset, else empty.
    void ResolveDrumKit(uint8_t fontId, int16_t instOrWave, std::string& outPack, int& outProgram) const;
    // Index of a selected entry whose range starts at `slot` (its noteLow),
    // enabled or not. Lets a native-routed drum note attribute its activity to
    // the slot's row even when the entry is disabled. -1 if none.
    int FindSlotEntryIdx(uint8_t fontId, int16_t instOrWave, uint8_t slot) const;
    // Per-slot NoteOn histogram backing this pair's slot discovery, or nullptr if
    // the pair has none (not a drum/SFX channel and not a forced-drum pair).
    // Centralises the "drum/SFX use the static grid, forced pairs use a pool
    // slot" choice so every reader/writer agrees. Points at [0..127].
    struct DrumSlotHit;
    DrumSlotHit* DrumHistFor(uint8_t fontId, int16_t instOrWave);
    const DrumSlotHit* DrumHistFor(uint8_t fontId, int16_t instOrWave) const;
    // Claim a free forced-drum histogram pool slot, zeroing it; -1 if the pool
    // is full (>= kMaxForcedDrumPairs flagged at once).
    int AllocForcedDrumPool();
    uint8_t AllocateChannelForPair(uint8_t fontId, int16_t instOrWave);
    // Frees the channel of the first idle pair found (no active synth notes,
    // and not the requesting pair) and returns it, or 0xFF if every channel
    // is currently sounding. Caller assigns ownership of the returned slot.
    uint8_t ReclaimIdleChannel(uint8_t exceptFontId, int16_t exceptInst);

    // Find an entry by primary key; create a new one if not present.
    // Caller fills bank/presetName even when creating, and decides
    // source / selected / enabled after. Returns the entry index.
    int FindOrCreateEntry(uint8_t fontId, int16_t instOrWave, const std::string& pack, int16_t program, int16_t bank,
                          const std::string& presetName, EntrySource source, uint8_t noteLow = 0);

    // Recompute mActiveEntryIdx[fontId][instOrWave]. Resolution: filter
    // mEntries by (fontId, instOrWave) && enabled && sfontId>=0; pick the
    // candidate whose pack ranks highest in mPackLoadOrder (last loaded
    // wins). Placeholder entries (program<0) participate normally — they
    // resolve only when explicitly enabled and produce muted output.
    void RecomputeActive(uint8_t fontId, int16_t instOrWave);
    int PackRank(const std::string& pack) const; // -1 if not in load order

    // Walks the cached loaded-preset list and returns the sfontId for
    // (pack, bank, program), or -1 if the pack isn't loaded / the SF
    // doesn't have that preset. Used at entry-create time so a fresh
    // pick is immediately resolvable without waiting for the next
    // pack-stack change.
    int16_t ResolveSfontIdFromCache(const std::string& pack, int16_t bank, int16_t program) const;

    NoteTranslatorState mNoteState[kMaxNotes] = {};
    ChannelState mChannelState[kMaxMidiChannels] = {};

    uint8_t mSynthActiveByPair[kMaxFontId][kMaxInstOrWave] = {};
    uint8_t mNativeActiveByPair[kMaxFontId][kMaxInstOrWave] = {};

    // Per-entry active counts (indexed by entry index), for the per-split-row
    // activity tint. Written by the audio thread on note activation/retire,
    // read lock-free by the UI. Index shifts on entry erase leave these
    // briefly stale until notes replay -- cosmetic only.
    uint8_t mEntrySynthActive[kMaxEntries] = {};
    uint8_t mEntryNativeActive[kMaxEntries] = {};

    uint8_t mPairChannel[kMaxFontId][kMaxInstOrWave];
    uint8_t mChannelsAllocated = 0;

    // Reverse map for the reclaim-on-exhaustion path: which pair owns each
    // allocated MIDI channel. instOrWave == -1 marks an unowned slot. Lets
    // ReclaimIdleChannel scan 64 channels instead of the full pair grid.
    struct ChannelOwner {
        uint8_t fontId = 0;
        int16_t instOrWave = -1; // -1 = unowned
    };
    ChannelOwner mChannelOwner[kMaxMidiChannels] = {};
    // Count of times an idle pair's channel was reclaimed for a new pair. Pure
    // diagnostic: nonzero means the pool wrapped and recycling kicked in.
    uint32_t mChannelReclaims = 0;

    std::array<uint64_t, (kMaxFontId * kMaxInstOrWave) / 64> mSeenBits{};
    DiscoveredPair mDiscovered[kMaxDiscovered] = {};
    std::atomic<int> mDiscoveredCount{ 0 };

    std::set<std::pair<uint8_t, int16_t>> mTemporaryMute;
    std::set<std::tuple<uint8_t, int16_t, uint8_t>> mTemporarySlotMute;
    std::map<std::pair<uint8_t, int16_t>, float> mTemporaryVolume;
    std::map<std::pair<uint8_t, int16_t>, std::string> mDisplayName;

    // DEBUG: per-pair stats backing storage. Counters use relaxed atomics; read
    // from the UI without synchronisation (best-effort diagnostic).
    struct DebugSlot {
        std::atomic<uint32_t> noteOns{ 0 };
        std::atomic<uint32_t> routedSynth{ 0 };
        std::atomic<uint32_t> routedNative{ 0 };
        std::atomic<uint32_t> routedMute{ 0 };
        uint8_t lastSemitone = 0;
    };
    DebugSlot mDebugStats[kMaxFontId][kMaxInstOrWave];

    // Per-slot NoteOn counts for the drum (instOrWave 0) and SFX (1) channels only,
    // scoped to 2 instOrWaves to keep the table small. Written by the audio thread
    // on every fresh NoteOn; read lock-free by the UI / GetDrumSlotHistogram. Index
    // is [fontId][instOrWave (0|1)][semitone 0..127].
    static constexpr int kDrumHistInst = 2; // instOrWave 0 (drum) + 1 (SFX)
    static constexpr int kDrumHistSlots = 128;
    struct DrumSlotHit {
        std::atomic<uint16_t> count{ 0 };
        // Monotonic ms of the last fresh NoteOn (0 = never), for the UI's
        // recently-played tint on slots that have no ConfigEntry to count on.
        std::atomic<uint32_t> lastHitMs{ 0 };
    };
    DrumSlotHit mDrumSlotHits[kMaxFontId][kDrumHistInst][kDrumHistSlots];

    // Per-pair drum channel mode (the per-instrument Native/Synth master), separate
    // from per-slot `enabled` so setting a slot Native doesn't flip the instrument.
    // Native (false, default) => all slots play the engine drum; Synth (true) =>
    // per-slot enabled decides. Audio thread reads per note, UI writes; torn bool
    // read is benign. Index [font][0|1].
    bool mDrumChannelSynth[kMaxFontId][kDrumHistInst] = {};

    // ── Forced-drum ("Treat as drum") pairs ──────────────────────────────
    // A flagged melodic pair routes through the drum path. The grid doubles as the
    // flag AND the histogram-pool index: -1 = not forced; 0..kMaxForcedDrumPairs-1 =
    // forced, value is the pool slot recording the pair's incoming notes. Audio
    // thread reads per note, UI writes (torn int8 read benign). Only instOrWave >=
    // kDrumHistInst is flagged. Initialised to -1 in the constructor.
    static constexpr int kMaxForcedDrumPairs = 32;
    int8_t mForcedDrumPool[kMaxFontId][kMaxInstOrWave];
    bool mForcedDrumPoolUsed[kMaxForcedDrumPairs] = {};
    DrumSlotHit mForcedDrumHits[kMaxForcedDrumPairs][kDrumHistSlots];

    // ── Entry-based config storage ────────────────────────────────────────
    std::vector<ConfigEntry> mEntries;
    // -1 = no active entry for this pair; native plays.
    int16_t mActiveEntryIdx[kMaxFontId][kMaxInstOrWave];
    uint32_t mNextSeq = 1;
    std::vector<std::string> mPackLoadOrder;
    // Cached copy of the AudioEditor's sLoadedPresets list. Refreshed by
    // RefreshEntrySfontIds; consumed by ResolveSfontIdFromCache so any
    // mutator (PickPreset, ClickSynth) can set a new entry's sfontId
    // immediately without going through another pack-stack pass.
    std::vector<LoadedPresetRef> mLoadedPresets;

    SynthMode mSynthMode = SynthMode::Authentic;
    // Set by the AudioEditor; default is a safe fallback until the first apply.
    float mGlobalGain = 0.70f;
};

} // namespace SOH
