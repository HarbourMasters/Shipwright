#include "MidiTranslator.h"
#include "GmInstrumentMap.h"
#include "InstrumentNames.h"
#include "soh/Enhancements/audio/MidiSynthManager.h"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>

#ifdef DEBUG_FONT_MAP
#include <cstdio>
#include <mutex>
static FILE* sDbgFile = nullptr;
static std::mutex sDbgMutex;

static void DbgLogNote(uint8_t fontId, int16_t instOrWave, uint8_t semitone, float freqScale, bool mapped,
                       uint8_t gmBank, uint8_t gmProg) {
    std::lock_guard<std::mutex> lk(sDbgMutex);
    if (!sDbgFile) {
        sDbgFile = fopen("font_map_dump.csv", "w");
        if (sDbgFile)
            fprintf(sDbgFile, "fontId,instOrWave,instOrWave_hex,semitone,expectedMidi,"
                              "freqScale,mapped,gmBank,gmProgram\n");
    }
    if (sDbgFile)
        fprintf(sDbgFile, "%u,%d,0x%02X,%u,%d,%.6f,%s,%u,%u\n", fontId, instOrWave, (uint8_t)instOrWave, semitone,
                static_cast<int>(semitone) + 21, freqScale, mapped ? "yes" : "no", gmBank, gmProg);
}
#define DBG_LOG(fontId, iow, semi, freq, mapped, bank, prog) \
    DbgLogNote((fontId), (iow), (semi), (freq), (mapped), (bank), (prog))
#else
#define DBG_LOG(...) (void)0
#endif

namespace SOH {

// Engine semitone -> MIDI note offset for melodic instruments.
//
// The engine's gNoteFrequencies table labels semitone 39 as "NOTE_C4
// (Middle C)" with freqScale 1.0, which gives the textbook offset of
// 60 - 39 = 21 against standard GM tuning.
static constexpr int kEngineSemitoneToMidiOffset = 21;

// Held near-max so the SF's default velocity attenuation modulator
// doesn't silence quiet notes. Dynamics ride CC11 instead (Authentic
// mode) or NoteOn velocity (Enhanced mode).
static constexpr uint8_t kFixedNoteOnVelocity = 100;

MidiTranslator::MidiTranslator() {
    // Reserve mEntries capacity so push_back never reallocates. The audio
    // thread reads entry fields by index without locking; a reallocation
    // would invalidate those reads.
    mEntries.reserve(kMaxEntries);
    // -1 = "no active entry" sentinel; native plays for that pair.
    for (auto& row : mActiveEntryIdx)
        for (auto& cell : row)
            cell = -1;
    // 0xFF = unallocated MIDI channel slot.
    for (auto& row : mPairChannel)
        for (auto& cell : row)
            cell = 0xFF;
    // -1 = pair not forced-drum (and not holding a discovery-pool slot).
    for (auto& row : mForcedDrumPool)
        for (auto& cell : row)
            cell = -1;
}

MidiTranslator& MidiTranslator::Instance() {
    static MidiTranslator sInstance;
    return sInstance;
}

void MidiTranslator::Reset() {
    for (NoteTranslatorState& noteState : mNoteState)
        noteState = {};
    for (ChannelState& chState : mChannelState)
        chState = {};
    for (auto& row : mSynthActiveByPair)
        for (auto& cell : row)
            cell = 0;
    for (auto& row : mNativeActiveByPair)
        for (auto& cell : row)
            cell = 0;
    for (auto& c : mEntrySynthActive)
        c = 0;
    for (auto& c : mEntryNativeActive)
        c = 0;
    auto synth = SOH::MidiSynthManager::Instance().GetActiveSynth();
    if (!synth)
        return;
    for (uint8_t ch = 0; ch < kMaxMidiChannels; ch++) {
        synth->ControlChange(ch, 123, 0); // CC 123 = All Notes Off
    }
}

static inline bool BypassIndexValid(uint8_t fontId, int16_t instOrWave) {
    return fontId < MidiTranslator::kMaxFontId && instOrWave >= 0 && instOrWave < MidiTranslator::kMaxInstOrWave;
}

// ── Discovery + active-voice counters ─────────────────────────────────────

int MidiTranslator::DiscoveredSnapshot(DiscoveredPair* out, int outCap) const {
    int n = mDiscoveredCount.load(std::memory_order_acquire);
    if (n > outCap)
        n = outCap;
    for (int i = 0; i < n; i++)
        out[i] = mDiscovered[i];
    return n;
}

void MidiTranslator::ClearDiscovered() {
    mDiscoveredCount.store(0, std::memory_order_release);
    for (auto& word : mSeenBits)
        word = 0;
    for (auto& entry : mDiscovered)
        entry = {};
}

uint8_t MidiTranslator::GetSynthActiveCount(uint8_t fontId, int16_t instOrWave) const {
    if (!BypassIndexValid(fontId, instOrWave))
        return 0;
    return mSynthActiveByPair[fontId][instOrWave];
}

uint8_t MidiTranslator::GetNativeActiveCount(uint8_t fontId, int16_t instOrWave) const {
    if (!BypassIndexValid(fontId, instOrWave))
        return 0;
    return mNativeActiveByPair[fontId][instOrWave];
}

void MidiTranslator::IncEntryActive(bool synth, int idx) {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return; // native fall-through has no entry
    uint8_t& c = synth ? mEntrySynthActive[idx] : mEntryNativeActive[idx];
    if (c < 255)
        c++;
}
void MidiTranslator::DecEntryActive(bool synth, int idx) {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return;
    uint8_t& c = synth ? mEntrySynthActive[idx] : mEntryNativeActive[idx];
    if (c > 0)
        c--;
}
uint8_t MidiTranslator::GetEntrySynthActive(int idx) const {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return 0;
    return mEntrySynthActive[idx];
}
uint8_t MidiTranslator::GetEntryNativeActive(int idx) const {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return 0;
    return mEntryNativeActive[idx];
}
void MidiTranslator::GetPairEntryActivity(uint8_t fontId, int16_t instOrWave, bool& anySynth, bool& anyNative) const {
    anySynth = false;
    anyNative = false;
    for (size_t idx = 0; idx < mEntries.size(); idx++) {
        const ConfigEntry& e = mEntries[idx];
        if (e.fontId != fontId || e.instOrWave != instOrWave)
            continue;
        if (mEntrySynthActive[idx] > 0)
            anySynth = true;
        else if (mEntryNativeActive[idx] > 0)
            anyNative = true;
    }
}

// ── DEBUG: per-pair stats accessors ──────────────────────────────────────

MidiTranslator::DebugPairStats MidiTranslator::GetDebugStats(uint8_t fontId, int16_t instOrWave) const {
    DebugPairStats out{};
    if (!BypassIndexValid(fontId, instOrWave))
        return out;
    const auto& s = mDebugStats[fontId][instOrWave];
    out.noteOns = s.noteOns.load(std::memory_order_relaxed);
    out.routedSynth = s.routedSynth.load(std::memory_order_relaxed);
    out.routedNative = s.routedNative.load(std::memory_order_relaxed);
    out.routedMute = s.routedMute.load(std::memory_order_relaxed);
    out.lastSemitone = s.lastSemitone;
    return out;
}

int MidiTranslator::GetDrumSlotHistogram(uint8_t fontId, int16_t instOrWave, uint32_t out[128]) const {
    for (int s = 0; s < kDrumHistSlots; ++s)
        out[s] = 0;
    const DrumSlotHit* hist = DrumHistFor(fontId, instOrWave);
    if (!hist)
        return 0;
    int distinct = 0;
    for (int s = 0; s < kDrumHistSlots; ++s) {
        uint32_t c = hist[s].count.load(std::memory_order_relaxed);
        out[s] = c;
        if (c > 0)
            ++distinct;
    }
    return distinct;
}

void MidiTranslator::ResetDebugStats() {
    for (int f = 0; f < kMaxFontId; ++f)
        for (int i = 0; i < kMaxInstOrWave; ++i)
            ResetDebugStatsForPair(static_cast<uint8_t>(f), static_cast<int16_t>(i));
}

void MidiTranslator::ResetDebugStatsForPair(uint8_t fontId, int16_t instOrWave) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;
    auto& s = mDebugStats[fontId][instOrWave];
    s.noteOns.store(0, std::memory_order_relaxed);
    s.routedSynth.store(0, std::memory_order_relaxed);
    s.routedNative.store(0, std::memory_order_relaxed);
    s.routedMute.store(0, std::memory_order_relaxed);
    s.lastSemitone = 0;
    if (DrumSlotHit* hist = DrumHistFor(fontId, instOrWave)) {
        for (int slot = 0; slot < kDrumHistSlots; ++slot)
            hist[slot].count.store(0, std::memory_order_relaxed);
    }
}

// ── Pack stack + sfontId resolution ───────────────────────────────────────

void MidiTranslator::SetPackLoadOrder(const std::vector<std::string>& order) {
    mPackLoadOrder = order;
}

int MidiTranslator::PackRank(const std::string& pack) const {
    // Higher index = later loaded = wins resolution. -1 if not loaded.
    for (size_t i = 0; i < mPackLoadOrder.size(); i++) {
        if (mPackLoadOrder[i] == pack)
            return static_cast<int>(i);
    }
    return -1;
}

int16_t MidiTranslator::ResolveSfontIdFromCache(const std::string& pack, int16_t bank, int16_t program) const {
    if (pack.empty() || program < 0) {
        // Placeholder entries — synthetic sfontId=0 so they resolve and
        // mute via the program<0 branch in ProcessNote.
        return 0;
    }
    for (const auto& lp : mLoadedPresets) {
        if (lp.packName == pack && lp.bank == bank && lp.program == program) {
            return static_cast<int16_t>(lp.sfontId);
        }
    }
    return -1;
}

void MidiTranslator::RefreshEntrySfontIds(const std::vector<LoadedPresetRef>& loadedPresets) {
    // Cache the list so the per-mutation entry creation path can resolve
    // sfontIds without waiting for the next pack-stack pass.
    mLoadedPresets = loadedPresets;
    // O(N*M) walk — both N (entries) and M (loaded presets) stay small in
    // practice (a few hundred each). Build a quick lookup if this ever
    // becomes hot.
    for (auto& e : mEntries) {
        e.sfontId = ResolveSfontIdFromCache(e.packName, e.bank, e.program);
    }
}

void MidiTranslator::RemoveModEntriesNotIn(const std::set<std::string>& packNamesLoaded) {
    // Erase ModSupplied entries whose pack isn't loaded anymore. We also
    // clear any active-cache slots that pointed into them; the caller is
    // expected to follow up with RecomputeAllActive() since indices shift.
    auto newEnd = std::remove_if(mEntries.begin(), mEntries.end(), [&](const ConfigEntry& e) {
        return e.source == EntrySource::ModSupplied && !packNamesLoaded.count(e.packName);
    });
    if (newEnd != mEntries.end()) {
        mEntries.erase(newEnd, mEntries.end());
        // Clear the active cache wholesale — indices may have shifted.
        for (auto& row : mActiveEntryIdx)
            for (auto& cell : row)
                cell = -1;
    }
}

void MidiTranslator::RecomputeAllActive() {
    for (auto& row : mActiveEntryIdx)
        for (auto& cell : row)
            cell = -1;
    // Recompute only pairs that actually have entries — RecomputeActive is
    // O(128 * entries-for-pair), so scanning all 64*256 cells would be
    // wasteful. The distinct-pair set is small (a few hundred at most).
    std::set<std::pair<uint8_t, int16_t>> pairs;
    for (const auto& e : mEntries)
        if (BypassIndexValid(e.fontId, e.instOrWave))
            pairs.insert({ e.fontId, e.instOrWave });
    for (const auto& p : pairs)
        RecomputeActive(p.first, p.second);
}

void MidiTranslator::RecomputeActive(uint8_t fontId, int16_t instOrWave) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;

    // Total order ranking entries for a semitone (drives both the per-semitone pick
    // and the chain sort): 1) source -- user picks beat mod-supplied; 2) specificity
    // -- narrower range beats wider; 3) pack load order -- later-loaded wins.
    auto outranks = [this](int ai, int bi) {
        const ConfigEntry& a = mEntries[ai];
        const ConfigEntry& b = mEntries[bi];
        int sa = (a.source == EntrySource::UserPicked) ? 1 : 0;
        int sb = (b.source == EntrySource::UserPicked) ? 1 : 0;
        if (sa != sb)
            return sa > sb;
        int wa = static_cast<int>(a.noteHigh) - static_cast<int>(a.noteLow);
        int wb = static_cast<int>(b.noteHigh) - static_cast<int>(b.noteLow);
        if (wa != wb)
            return wa < wb; // narrower == more specific -> wins
        return PackRank(a.packName) > PackRank(b.packName);
    };

    // Per-semitone winner: the top-ranked enabled+resolvable entry whose
    // [noteLow,noteHigh] covers that engine slot. For an unsplit pair every semitone
    // resolves to the same single entry (or none), collapsing the chain to length 1.
    int winnerAt[128];
    for (int s = 0; s < 128; ++s) {
        int best = -1;
        for (size_t idx = 0; idx < mEntries.size(); idx++) {
            const ConfigEntry& e = mEntries[idx];
            if (e.fontId != fontId || e.instOrWave != instOrWave)
                continue;
            if (!e.enabled)
                continue;
            // Native/Mute splits don't reference a soundfont, so they qualify
            // without a resolved sfontId; only Synth entries need one.
            if (e.route == EntryRoute::Synth && e.sfontId < 0)
                continue;
            if (s < e.noteLow || s > e.noteHigh)
                continue;
            if (best < 0 || outranks(static_cast<int>(idx), best))
                best = static_cast<int>(idx);
        }
        winnerAt[s] = best;
    }

    // Distinct winners, deduped so each entry is linked at most once (keeps the
    // chain acyclic; the audio thread walks it by index). Sorted by the SAME
    // priority order so the walk's "first covering entry wins" reproduces winnerAt
    // even with overlapping ranges (e.g. a user split over a mod full-range): for
    // any semitone the highest-priority covering winner sorts ahead of the rest.
    std::vector<int> winners;
    for (int s = 0; s < 128; ++s) {
        int w = winnerAt[s];
        if (w < 0)
            continue;
        if (std::find(winners.begin(), winners.end(), w) == winners.end())
            winners.push_back(w);
    }
    std::sort(winners.begin(), winners.end(), [&](int a, int b) { return outranks(a, b); });

    // Reset this pair's links, relink the winners, then publish the head LAST (the
    // single int16_t store) so a concurrent audio-thread walk sees either the intact
    // old chain or the fully-built new one. The brief reset->relink window can at
    // worst play one note native.
    for (auto& e : mEntries)
        if (e.fontId == fontId && e.instOrWave == instOrWave)
            e.nextActiveSplit = -1;
    for (size_t k = 0; k + 1 < winners.size(); ++k)
        mEntries[winners[k]].nextActiveSplit = static_cast<int16_t>(winners[k + 1]);
    mActiveEntryIdx[fontId][instOrWave] = winners.empty() ? -1 : static_cast<int16_t>(winners[0]);
}

// ── Entry queries ─────────────────────────────────────────────────────────

const ConfigEntry* MidiTranslator::GetActiveEntry(uint8_t fontId, int16_t instOrWave) const {
    if (!BypassIndexValid(fontId, instOrWave))
        return nullptr;
    int idx = mActiveEntryIdx[fontId][instOrWave];
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return nullptr;
    return &mEntries[idx];
}
int MidiTranslator::GetActiveEntryIdx(uint8_t fontId, int16_t instOrWave) const {
    if (!BypassIndexValid(fontId, instOrWave))
        return -1;
    return mActiveEntryIdx[fontId][instOrWave];
}

void MidiTranslator::GetEntriesForPair(uint8_t fontId, int16_t instOrWave, std::vector<int>& outIdx) const {
    outIdx.clear();
    for (size_t idx = 0; idx < mEntries.size(); idx++) {
        const ConfigEntry& e = mEntries[idx];
        if (e.fontId == fontId && e.instOrWave == instOrWave) {
            outIdx.push_back(static_cast<int>(idx));
        }
    }
}

int MidiTranslator::FindEntry(uint8_t fontId, int16_t instOrWave, const std::string& pack, int16_t program,
                              uint8_t noteLow) const {
    for (size_t idx = 0; idx < mEntries.size(); idx++) {
        const ConfigEntry& e = mEntries[idx];
        if (e.fontId == fontId && e.instOrWave == instOrWave && e.packName == pack && e.program == program &&
            e.noteLow == noteLow) {
            return static_cast<int>(idx);
        }
    }
    return -1;
}

int MidiTranslator::CountSelectedEntries(uint8_t fontId, int16_t instOrWave) const {
    int n = 0;
    for (const auto& e : mEntries) {
        if (e.fontId == fontId && e.instOrWave == instOrWave && e.selected)
            n++;
    }
    return n;
}

int MidiTranslator::FindOrCreateEntry(uint8_t fontId, int16_t instOrWave, const std::string& pack, int16_t program,
                                      int16_t bank, const std::string& presetName, EntrySource source,
                                      uint8_t noteLow) {
    int idx = FindEntry(fontId, instOrWave, pack, program, noteLow);
    if (idx >= 0)
        return idx;
    if (mEntries.size() >= kMaxEntries) {
        SPDLOG_WARN("[MidiTranslator] Entry pool full ({}); dropping new entry "
                    "for f={}, i={}, pack='{}', program={}",
                    kMaxEntries, fontId, instOrWave, pack, program);
        return -1;
    }
    ConfigEntry e;
    e.fontId = fontId;
    e.instOrWave = instOrWave;
    e.packName = pack;
    e.program = program;
    e.bank = bank;
    e.presetName = presetName;
    e.source = source;
    e.noteLow = noteLow; // noteHigh stays 127 (full range) until a split sets it
    // Resolve sfontId immediately so the entry is eligible for resolution on the
    // next note; otherwise the filter (enabled && sfontId>=0) skips a fresh pick.
    // ResolveSfontIdFromCache returns 0 for placeholder entries so they resolve and
    // produce the silent NoteOn the user asked for.
    e.sfontId = ResolveSfontIdFromCache(pack, bank, program);
    mEntries.push_back(std::move(e));
    return static_cast<int>(mEntries.size()) - 1;
}

// ── UI row actions ────────────────────────────────────────────────────────

void MidiTranslator::PickPreset(uint8_t fontId, int16_t instOrWave, const std::string& pack, int16_t program,
                                int16_t bank, const std::string& presetName) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;

    // Capture the previous winner's pack so we know whether to clear its
    // selected flag (same SoundFont as the new pick → user moved within
    // the pack and the old entry is no longer "the one for this pack").
    std::string prevWinnerPack;
    int prevWinnerIdx = mActiveEntryIdx[fontId][instOrWave];
    if (prevWinnerIdx >= 0 && prevWinnerIdx < static_cast<int>(mEntries.size())) {
        prevWinnerPack = mEntries[prevWinnerIdx].packName;
    }

    // Disable every currently-enabled entry for this pair.
    for (auto& e : mEntries) {
        if (e.fontId == fontId && e.instOrWave == instOrWave && e.enabled) {
            e.enabled = false;
        }
    }

    // Same-SoundFont rule: if the previous winner was from the new
    // pick's pack, drop its selected flag (the user replaced it in-place).
    if (!prevWinnerPack.empty() && prevWinnerPack == pack && prevWinnerIdx >= 0) {
        ConfigEntry& prev = mEntries[prevWinnerIdx];
        if (prev.program != program) {
            prev.selected = false;
        }
    }

    const size_t entryCountBefore = mEntries.size();
    int idx = FindOrCreateEntry(fontId, instOrWave, pack, program, bank, presetName, EntrySource::UserPicked);
    if (idx < 0)
        return;
    ConfigEntry& e = mEntries[idx];
    // On a freshly created entry, seed the octave Shift from the engine sample's
    // tuning so a substitute lands in the native octave by default; user-overridable.
    // SOH [Enhancement]
    if (mEntries.size() > entryCountBefore) {
        e.transpose = static_cast<int8_t>(SuggestedTranspose(fontId, instOrWave, e.noteLow, e.noteHigh));
    }
    // Preserve gain/transpose/effects when reusing; only program/bank/pack identify
    // the entry. presetName updates so renamed presets surface. Re-resolve sfontId
    // in case the bank changed on a reused entry (FindEntry keys on pack+program).
    e.bank = bank;
    e.presetName = presetName;
    e.sfontId = ResolveSfontIdFromCache(pack, bank, program);
    e.enabled = true;
    e.selected = true;
    e.lastEnabledSeq = mNextSeq++;
    // source stays whatever it was (might be ModSupplied if the user
    // clicked a mod-shipped entry; that's fine — picking promotes it to
    // a selected entry which will be persisted).
    if (e.source == EntrySource::ModSupplied) {
        // The act of picking promotes a mod entry to user-owned so we
        // persist customisations the user makes on top of it.
        e.source = EntrySource::UserPicked;
    }
    RecomputeActive(fontId, instOrWave);
}

void MidiTranslator::ClickNative(uint8_t fontId, int16_t instOrWave) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;
    bool hasModEntry = false;
    for (auto& e : mEntries) {
        if (e.fontId == fontId && e.instOrWave == instOrWave) {
            if (e.source == EntrySource::ModSupplied)
                hasModEntry = true;
            e.enabled = false;
        }
    }
    // A pack mapping ships enabled entries that reappear -- and, under source-aware
    // resolution, win -- on every reload, so disabling our entries can't outlast it.
    // Persist the Native choice as a user-owned full-range Native marker: it
    // outranks the mod (UserPicked) and routes to the engine sample. Pairs with no
    // mod entry need no marker -- a disabled user entry already persists "off".
    if (hasModEntry) {
        int idx =
            FindOrCreateEntry(fontId, instOrWave, std::string(), -1, 0, std::string("Native"), EntrySource::UserPicked);
        if (idx >= 0) {
            ConfigEntry& m = mEntries[idx];
            m.route = EntryRoute::Native;
            m.noteLow = 0;
            m.noteHigh = 127;
            m.enabled = true;
            m.selected = true;
            m.source = EntrySource::UserPicked;
            m.lastEnabledSeq = mNextSeq++;
        }
    }
    RecomputeActive(fontId, instOrWave);
}

// A user Native marker is an empty-pack, full-range, route=Native entry created by
// ClickNative to persist "this pair plays native" over a mod that ships a preset.
static inline bool IsNativeMarker(const ConfigEntry& e) {
    return e.route == EntryRoute::Native && e.packName.empty();
}

void MidiTranslator::ClickSynth(uint8_t fontId, int16_t instOrWave) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;

    // Leaving Native: retire any user Native marker so it stops winning. The
    // candidate searches below also skip it (deselected here, but the fallback
    // ignores `selected`).
    for (auto& e : mEntries) {
        if (e.fontId == fontId && e.instOrWave == instOrWave && IsNativeMarker(e)) {
            e.enabled = false;
            e.selected = false;
        }
    }

    // Primary search: user-selected entries.
    int bestIdx = -1;
    uint32_t bestSeq = 0;
    for (size_t idx = 0; idx < mEntries.size(); idx++) {
        const ConfigEntry& e = mEntries[idx];
        if (e.fontId != fontId || e.instOrWave != instOrWave)
            continue;
        if (!e.selected)
            continue;
        if (bestIdx < 0 || e.lastEnabledSeq >= bestSeq) {
            bestIdx = static_cast<int>(idx);
            bestSeq = e.lastEnabledSeq;
        }
    }
    // Fallback (option B): any disabled-but-resolvable entry for this
    // pair. Covers the "mod ships a preset, user never picked, clicked
    // Native, now wants it back" case.
    if (bestIdx < 0) {
        for (size_t idx = 0; idx < mEntries.size(); idx++) {
            const ConfigEntry& e = mEntries[idx];
            if (e.fontId != fontId || e.instOrWave != instOrWave)
                continue;
            if (IsNativeMarker(e))
                continue; // never restore the Native marker as a synth pick
            if (e.sfontId < 0)
                continue;
            if (bestIdx < 0 || e.lastEnabledSeq >= bestSeq) {
                bestIdx = static_cast<int>(idx);
                bestSeq = e.lastEnabledSeq;
            }
        }
    }
    if (bestIdx >= 0) {
        ConfigEntry& e = mEntries[bestIdx];
        e.enabled = true;
        e.lastEnabledSeq = mNextSeq++;
        RecomputeActive(fontId, instOrWave);
        return;
    }
    // Last resort: muted placeholder entry. packName=="" tells the resolver
    // / UI that this is a synthetic "None" pick.
    int idx = FindOrCreateEntry(fontId, instOrWave, std::string(), -1, 0, std::string("None"), EntrySource::UserPicked);
    if (idx < 0)
        return;
    ConfigEntry& e = mEntries[idx];
    e.enabled = true;
    e.selected = true;
    e.lastEnabledSeq = mNextSeq++;
    // Placeholder entries get sfontId=0 in RefreshEntrySfontIds so they
    // participate in resolution; tag here as well in case Refresh hasn't
    // run since the create.
    e.sfontId = 0;
    RecomputeActive(fontId, instOrWave);
}

// Find a drum pair's current kit (pack/program) from any existing selected
// bank-128 slot entry, so re-running auto-split or toggling Synth preserves
// the user's kit choice. Falls back to the first loaded bank-128 preset, then
// to an empty pack (slots stay native until a kit is assigned).
void MidiTranslator::ResolveDrumKit(uint8_t fontId, int16_t instOrWave, std::string& outPack, int& outProgram) const {
    for (const auto& e : mEntries) {
        if (e.fontId == fontId && e.instOrWave == instOrWave && e.selected && e.bank == 128 && !e.packName.empty()) {
            outPack = e.packName;
            outProgram = e.program;
            return;
        }
    }
    for (const auto& lp : mLoadedPresets) {
        if (lp.bank == 128) {
            outPack = lp.packName;
            outProgram = lp.program;
            return;
        }
    }
    outPack.clear();
    outProgram = 0;
}

int MidiTranslator::FindSlotEntryIdx(uint8_t fontId, int16_t instOrWave, uint8_t slot) const {
    for (size_t idx = 0; idx < mEntries.size(); idx++) {
        const ConfigEntry& e = mEntries[idx];
        if (e.fontId == fontId && e.instOrWave == instOrWave && e.selected && e.noteLow == slot)
            return static_cast<int>(idx);
    }
    return -1;
}

const MidiTranslator::DrumSlotHit* MidiTranslator::DrumHistFor(uint8_t fontId, int16_t instOrWave) const {
    if (fontId >= kMaxFontId || instOrWave < 0 || instOrWave >= kMaxInstOrWave)
        return nullptr;
    if (instOrWave < kDrumHistInst)
        return mDrumSlotHits[fontId][instOrWave];
    int8_t pool = mForcedDrumPool[fontId][instOrWave];
    if (pool < 0 || pool >= kMaxForcedDrumPairs)
        return nullptr;
    return mForcedDrumHits[pool];
}

MidiTranslator::DrumSlotHit* MidiTranslator::DrumHistFor(uint8_t fontId, int16_t instOrWave) {
    return const_cast<DrumSlotHit*>(static_cast<const MidiTranslator*>(this)->DrumHistFor(fontId, instOrWave));
}

int MidiTranslator::AllocForcedDrumPool() {
    for (int p = 0; p < kMaxForcedDrumPairs; ++p) {
        if (!mForcedDrumPoolUsed[p]) {
            mForcedDrumPoolUsed[p] = true;
            for (int s = 0; s < kDrumHistSlots; ++s)
                mForcedDrumHits[p][s].count.store(0, std::memory_order_relaxed);
            return p;
        }
    }
    return -1;
}

void MidiTranslator::SetForcedDrum(uint8_t fontId, int16_t instOrWave, bool forced) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;
    if (instOrWave < kDrumHistInst)
        return; // 0/1 are intrinsic drum channels -- use SetDrumChannelSynth

    const bool wasForced = mForcedDrumPool[fontId][instOrWave] >= 0;
    if (forced == wasForced)
        return;

    if (forced) {
        int pool = AllocForcedDrumPool();
        if (pool < 0) {
            SPDLOG_WARN("[MidiTranslator] SetForcedDrum: pool full ({} pairs), "
                        "cannot flag font {} inst {}",
                        kMaxForcedDrumPairs, fontId, instOrWave);
            return; // toggle doesn't stick; the pool is generously sized
        }
        // A full-range melodic entry would otherwise cover every slot and
        // shadow the per-slot drum entries once discovered. Disable any enabled
        // one so the pair starts from a clean native baseline (it stays in
        // mEntries, deselected-but-present, so flipping back restores it).
        for (auto& e : mEntries) {
            if (e.fontId == fontId && e.instOrWave == instOrWave && e.enabled && e.noteLow == 0 && e.noteHigh == 127)
                e.enabled = false;
        }
        mForcedDrumPool[fontId][instOrWave] = static_cast<int8_t>(pool);
    } else {
        int8_t pool = mForcedDrumPool[fontId][instOrWave];
        if (pool >= 0 && pool < kMaxForcedDrumPairs)
            mForcedDrumPoolUsed[pool] = false;
        mForcedDrumPool[fontId][instOrWave] = -1;
        // Drop the per-slot drum entries so the pair returns to a clean melodic row.
        // Disable + deselect rather than erase: erasing shifts the cached indices
        // the audio thread reads. A deselected entry is ignored by resolution and
        // dropped on the next save; re-forcing rediscovers them by key.
        for (auto& e : mEntries) {
            if (e.fontId == fontId && e.instOrWave == instOrWave && e.noteLow == e.noteHigh) {
                e.enabled = false;
                e.selected = false;
            }
        }
    }
    // Resolution gates on the flag at play time, but disabling a full-range
    // entry (above) changes the chain, so recompute either way.
    RecomputeActive(fontId, instOrWave);
}

bool MidiTranslator::IsForcedDrum(uint8_t fontId, int16_t instOrWave) const {
    if (fontId >= kMaxFontId || instOrWave < 0 || instOrWave >= kMaxInstOrWave)
        return false;
    return instOrWave >= kDrumHistInst && mForcedDrumPool[fontId][instOrWave] >= 0;
}

void MidiTranslator::AutoSplitDrums(uint8_t fontId, int16_t instOrWave) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;
    const DrumSlotHit* hist = DrumHistFor(fontId, instOrWave);
    if (!hist)
        return; // no per-slot histogram (not a drum/SFX channel or forced pair)

    std::string kitPack;
    int kitProgram = 0;
    const int16_t kitBank = 128;
    ResolveDrumKit(fontId, instOrWave, kitPack, kitProgram);

    // Map existing selected slot entries by their slot (noteLow) so a re-run
    // updates them in place instead of duplicating (an entry created with a
    // different/empty pack wouldn't be found by the (pack,program,noteLow)
    // key, so we match on slot directly).
    int bySlot[128];
    for (int s = 0; s < 128; ++s)
        bySlot[s] = -1;
    for (size_t idx = 0; idx < mEntries.size(); idx++) {
        const ConfigEntry& e = mEntries[idx];
        // Only reuse existing single-slot entries; full-range whole-pair entries
        // (noteLow 0..127) must not be hijacked into slot 0.
        if (e.fontId == fontId && e.instOrWave == instOrWave && e.selected && e.noteLow == e.noteHigh &&
            e.noteLow < 128)
            bySlot[e.noteLow] = static_cast<int>(idx);
    }

    // One single-slot entry per fired slot. The default GM percussion note is
    // a stable per-slot spread (35 + slot, clamped) so each slot is audibly
    // distinct; the user remaps via the Drum Sound combo (item 8.1 will
    // auto-pick it later). An existing slot keeps its prior Drum Sound.
    for (int s = 0; s < kDrumHistSlots; ++s) {
        if (hist[s].count.load(std::memory_order_relaxed) == 0)
            continue;
        int idx = bySlot[s];
        const bool isNew = (idx < 0);
        if (isNew) {
            idx = FindOrCreateEntry(fontId, instOrWave, kitPack, static_cast<int16_t>(kitProgram), kitBank,
                                    std::string("Drum Kit"), EntrySource::UserPicked, static_cast<uint8_t>(s));
            if (idx < 0)
                continue;
        }
        ConfigEntry& e = mEntries[idx];
        e.noteLow = static_cast<uint8_t>(s);
        e.noteHigh = static_cast<uint8_t>(s);
        e.packName = kitPack;
        e.program = static_cast<int16_t>(kitProgram);
        e.bank = kitBank;
        if (e.fixedNote < 0) {
            e.fixedNote =
                static_cast<int16_t>(std::clamp(static_cast<int>(kGmPercussionLo) + s,
                                                static_cast<int>(kGmPercussionLo), static_cast<int>(kGmPercussionHi)));
        }
        e.route = EntryRoute::Synth;
        e.source = EntrySource::UserPicked;
        e.selected = true;
        // Discovery only POPULATES the row. A freshly discovered slot defaults
        // to Native (disabled) so switching the instrument to Synth doesn't
        // suddenly blast a guessed GM kit; the user enables each slot by
        // picking a sound. Existing slots keep their saved enabled state.
        if (isNew)
            e.enabled = false;
        if (e.enabled)
            e.lastEnabledSeq = mNextSeq++;
        // No kit loaded -> unresolved (native) rather than mis-synthing on the
        // empty-pack placeholder path (which would resolve to sfontId 0).
        e.sfontId = kitPack.empty() ? -1 : ResolveSfontIdFromCache(kitPack, kitBank, kitProgram);
    }
    RecomputeActive(fontId, instOrWave);
}

void MidiTranslator::AddDrumSlot(uint8_t fontId, int16_t instOrWave, uint8_t slot) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;
    if (slot >= kDrumHistSlots)
        return;
    // Drum-like pairs only: intrinsic drum/SFX channels or a forced-drum pair.
    if (instOrWave != 0 && instOrWave != 1 && !IsForcedDrum(fontId, instOrWave))
        return;

    std::string kitPack;
    int kitProgram = 0;
    const int16_t kitBank = 128;
    ResolveDrumKit(fontId, instOrWave, kitPack, kitProgram);

    int idx = FindSlotEntryIdx(fontId, instOrWave, slot);
    if (idx < 0) {
        idx = FindOrCreateEntry(fontId, instOrWave, kitPack, static_cast<int16_t>(kitProgram), kitBank,
                                std::string("Drum Kit"), EntrySource::UserPicked, slot);
        if (idx < 0)
            return;
    }
    ConfigEntry& e = mEntries[idx];
    e.noteLow = slot;
    e.noteHigh = slot;
    e.packName = kitPack;
    e.program = static_cast<int16_t>(kitProgram);
    e.bank = kitBank;
    if (e.fixedNote < 0) {
        e.fixedNote =
            static_cast<int16_t>(std::clamp(static_cast<int>(kGmPercussionLo) + slot, static_cast<int>(kGmPercussionLo),
                                            static_cast<int>(kGmPercussionHi)));
    }
    e.route = EntryRoute::Synth;
    e.source = EntrySource::UserPicked;
    e.selected = true;
    // Native by default (like discovery) so adding a slot never blasts a guessed
    // GM kit; the user enables it by picking a Drum Sound.
    e.enabled = false;
    e.sfontId = kitPack.empty() ? -1 : ResolveSfontIdFromCache(kitPack, kitBank, kitProgram);
    RecomputeActive(fontId, instOrWave);
}

void MidiTranslator::SetDrumChannelSynth(uint8_t fontId, int16_t instOrWave, bool synth) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;
    if (instOrWave < 0 || instOrWave >= kDrumHistInst)
        return; // master flag only meaningful for the drum/SFX channels

    // Master switch only: set the per-pair flag. We DON'T touch per-slot
    // `enabled` here -- that is each slot's own Native/Synth state, preserved
    // across instrument-mode toggles (so flipping to Native then back restores
    // exactly what the slots were).
    mDrumChannelSynth[fontId][instOrWave] = synth;

    if (synth) {
        // Going Synth needs rows to edit. If none exist yet, discover them
        // from the histogram (created Native by default -- see AutoSplitDrums).
        bool hasSlot = false;
        for (const auto& e : mEntries) {
            if (e.fontId == fontId && e.instOrWave == instOrWave && e.selected && e.noteLow == e.noteHigh) {
                hasSlot = true;
                break;
            }
        }
        if (!hasSlot)
            AutoSplitDrums(fontId, instOrWave);
    }
    // Resolution gates on the flag at play time (ProcessNote), so no chain
    // rebuild is required for the toggle itself; AutoSplitDrums recomputes if
    // it ran.
}

bool MidiTranslator::IsDrumChannelSynth(uint8_t fontId, int16_t instOrWave) const {
    if (fontId >= kMaxFontId || instOrWave < 0 || instOrWave >= kDrumHistInst)
        return false;
    return mDrumChannelSynth[fontId][instOrWave];
}

void MidiTranslator::SetDrumKit(uint8_t fontId, int16_t instOrWave, const std::string& pack, int16_t program,
                                const std::string& presetName) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;
    for (auto& e : mEntries) {
        if (e.fontId != fontId || e.instOrWave != instOrWave || !e.selected)
            continue;
        e.packName = pack;
        e.program = program;
        e.bank = 128;
        if (!presetName.empty())
            e.presetName = presetName;
        e.source = EntrySource::UserPicked;
        e.sfontId = ResolveSfontIdFromCache(pack, 128, program);
    }
    RecomputeActive(fontId, instOrWave);
}

// ── Note-range split editing (melodic) ──────────────────────────────────────

int MidiTranslator::SplitEntry(int idx, uint8_t atSemitone) {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return -1;
    // The cut must fall strictly inside the range so both halves are non-empty.
    if (atSemitone <= mEntries[idx].noteLow || atSemitone > mEntries[idx].noteHigh)
        return -1;
    if (mEntries.size() >= kMaxEntries)
        return -1;
    ConfigEntry sib = mEntries[idx]; // copy preset/gain/effects/route/etc.
    sib.noteLow = atSemitone;        // sibling takes the upper half
    sib.selected = true;
    sib.source = EntrySource::UserPicked;
    sib.lastEnabledSeq = mNextSeq++;
    mEntries[idx].noteHigh = static_cast<uint8_t>(atSemitone - 1); // idx keeps lower half
    mEntries.push_back(std::move(sib));                            // mEntries reserved -> no realloc, idx stays valid
    int newIdx = static_cast<int>(mEntries.size()) - 1;
    NormalizeSplits(mEntries[idx].fontId, mEntries[idx].instOrWave);
    RecomputeActive(mEntries[idx].fontId, mEntries[idx].instOrWave);
    return newIdx;
}

int MidiTranslator::SplitEntryEven(int idx, int parts) {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return idx;
    if (parts < 2)
        return idx;
    const int lo = mEntries[idx].noteLow;
    const int hi = mEntries[idx].noteHigh;
    const int span = hi - lo + 1;
    if (span < parts)
        parts = span; // can't make more ranges than semitones in the span
    if (parts < 2)
        return idx;
    // Bisect off the top repeatedly: cut at the start of each part, then keep
    // splitting the upper remainder. Boundaries are evenly spaced and strictly
    // increasing (span >= parts), so each SplitEntry cut lands inside its range.
    int cur = idx;
    for (int k = 1; k < parts; ++k) {
        int boundary = lo + static_cast<int>((static_cast<long long>(span) * k) / parts);
        int newIdx = SplitEntry(cur, static_cast<uint8_t>(boundary));
        if (newIdx < 0)
            break;
        cur = newIdx;
    }
    return idx;
}

void MidiTranslator::MergeWithNext(int idx) {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return;
    ConfigEntry& e = mEntries[idx];
    if (e.noteHigh >= 127)
        return; // nothing above to merge
    const int wantLow = static_cast<int>(e.noteHigh) + 1;
    int sib = -1;
    for (size_t k = 0; k < mEntries.size(); k++) {
        const ConfigEntry& c = mEntries[k];
        if (c.fontId == e.fontId && c.instOrWave == e.instOrWave && c.selected &&
            static_cast<int>(c.noteLow) == wantLow) {
            sib = static_cast<int>(k);
            break;
        }
    }
    if (sib < 0)
        return;
    // Extend idx over the sibling's range, then retire the sibling (disable +
    // deselect rather than erase -- erasing would shift every cached index and
    // risk an audio-thread read mid-move; a deselected entry is dropped on the
    // next save and ignored by resolution/UI).
    e.noteHigh = mEntries[sib].noteHigh;
    mEntries[sib].enabled = false;
    mEntries[sib].selected = false;
    NormalizeSplits(e.fontId, e.instOrWave);
    RecomputeActive(e.fontId, e.instOrWave);
}

void MidiTranslator::SetSplitBoundary(int lowerIdx, int upperIdx, uint8_t boundary) {
    const int n = static_cast<int>(mEntries.size());
    if (lowerIdx < 0 || lowerIdx >= n || upperIdx < 0 || upperIdx >= n)
        return;
    ConfigEntry& lo = mEntries[lowerIdx];
    ConfigEntry& hi = mEntries[upperIdx];
    // Keep both ranges non-empty: lo spans [lo.noteLow .. b], hi spans
    // [b+1 .. hi.noteHigh], so b must sit in [lo.noteLow, hi.noteHigh - 1].
    int b = std::clamp<int>(boundary, lo.noteLow, static_cast<int>(hi.noteHigh) - 1);
    lo.noteHigh = static_cast<uint8_t>(b);
    hi.noteLow = static_cast<uint8_t>(b + 1);
    NormalizeSplits(lo.fontId, lo.instOrWave);
    RecomputeActive(lo.fontId, lo.instOrWave);
}

void MidiTranslator::SetEntryNoteRange(int idx, uint8_t noteLow, uint8_t noteHigh) {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return;
    if (noteHigh < noteLow)
        noteHigh = noteLow;
    mEntries[idx].noteLow = noteLow;
    mEntries[idx].noteHigh = noteHigh;
    NormalizeSplits(mEntries[idx].fontId, mEntries[idx].instOrWave);
    RecomputeActive(mEntries[idx].fontId, mEntries[idx].instOrWave);
}

void MidiTranslator::NormalizeSplits(uint8_t fontId, int16_t instOrWave) {
    // Drum/SFX pairs are per-slot, not ranges -- leave them to AutoSplitDrums.
    if (instOrWave < kDrumHistInst)
        return;

    // The split set = the user's selected rows, minus the whole-pair Native
    // marker; mod rows stay selected==false, an untouched base layer below.
    std::vector<int> rows;
    for (size_t i = 0; i < mEntries.size(); i++) {
        const ConfigEntry& e = mEntries[i];
        if (e.fontId != fontId || e.instOrWave != instOrWave || !e.selected)
            continue;
        if (e.route == EntryRoute::Native && e.packName.empty())
            continue;
        rows.push_back(static_cast<int>(i));
    }
    if (rows.empty())
        return;

    std::stable_sort(rows.begin(), rows.end(), [this](int a, int b) {
        if (mEntries[a].noteLow != mEntries[b].noteLow)
            return mEntries[a].noteLow < mEntries[b].noteLow;
        return mEntries[a].noteHigh < mEntries[b].noteHigh;
    });

    // Pin each row's low to the previous high + 1; retire rows past 127 and
    // stretch the last survivor to 127.
    int nextLow = 0;
    int lastKept = -1;
    for (int idx : rows) {
        ConfigEntry& e = mEntries[idx];
        if (nextLow > 127) {
            e.selected = false;
            e.enabled = false;
            continue;
        }
        int lo = nextLow;
        int hi = std::clamp<int>(e.noteHigh, lo, 127);
        e.noteLow = static_cast<uint8_t>(lo);
        e.noteHigh = static_cast<uint8_t>(hi);
        lastKept = idx;
        nextLow = hi + 1;
    }
    if (lastKept >= 0)
        mEntries[lastKept].noteHigh = 127;
}

void MidiTranslator::FlattenSplits(uint8_t fontId, int16_t instOrWave) {
    if (instOrWave < kDrumHistInst)
        return;
    // Keep the active row at full range, retire the other selected ranges.
    int keep = mActiveEntryIdx[fontId][instOrWave];
    // Only honor the active pick if it's a real selected range (not the
    // whole-pair Native marker or stale); else adopt the first row in the loop.
    if (keep >= 0 && keep < static_cast<int>(mEntries.size())) {
        const ConfigEntry& k = mEntries[keep];
        const bool isRealRange = k.fontId == fontId && k.instOrWave == instOrWave && k.selected &&
                                 !(k.route == EntryRoute::Native && k.packName.empty());
        if (!isRealRange)
            keep = -1;
    } else {
        keep = -1;
    }
    for (size_t i = 0; i < mEntries.size(); i++) {
        ConfigEntry& e = mEntries[i];
        if (e.fontId != fontId || e.instOrWave != instOrWave || !e.selected)
            continue;
        if (e.route == EntryRoute::Native && e.packName.empty())
            continue;
        if (keep < 0)
            keep = static_cast<int>(i); // no active pick: adopt the first row
        if (static_cast<int>(i) == keep) {
            e.noteLow = 0;
            e.noteHigh = 127;
        } else {
            e.selected = false;
            e.enabled = false;
        }
    }
    RecomputeActive(fontId, instOrWave);
}

void MidiTranslator::SetEntryPreset(int idx, const std::string& pack, int16_t program, int16_t bank,
                                    const std::string& presetName) {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return;
    ConfigEntry& e = mEntries[idx];
    e.packName = pack;
    e.program = program;
    e.bank = bank;
    if (!presetName.empty())
        e.presetName = presetName;
    e.route = EntryRoute::Synth;
    e.fixedNote = -1; // melodic: derive the played pitch from the engine semitone
    e.enabled = true;
    e.selected = true;
    e.source = EntrySource::UserPicked;
    e.lastEnabledSeq = mNextSeq++;
    e.sfontId = ResolveSfontIdFromCache(pack, bank, program);
    RecomputeActive(e.fontId, e.instOrWave);
}

void MidiTranslator::AutoSplitByEngineRanges(uint8_t fontId, int16_t instOrWave) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;
    if (instOrWave < kDrumHistInst)
        return; // melodic only -- drums use AutoSplitDrums

    // The engine routes semitone < lo -> low sample, lo..hi -> normal,
    // semitone > hi -> high. Mirror those boundaries.
    InstrumentSampleSet names = GetInstrumentSampleNames(fontId, instOrWave);
    if (!names.hasRange)
        return; // no captured boundaries (older asset / mod font)
    const uint8_t lo = names.rangeLo;
    const uint8_t hi = names.rangeHi;
    if (lo == 0 && hi >= 127)
        return; // engine uses a single full range here -- nothing to split

    // Split the pair's active entry (its preset is duplicated across the
    // ranges). With no active synth entry there's nothing to mirror.
    int actIdx = mActiveEntryIdx[fontId][instOrWave];
    if (actIdx < 0 || actIdx >= static_cast<int>(mEntries.size()))
        return;
    ConfigEntry tmpl = mEntries[actIdx]; // copy the preset/gain/effects/route

    // Repurpose the active entry as the normal range [lo, hi]. Each range maps to a
    // distinct engine sample, so re-derive its octave Shift from that sample's tuning.
    // SOH [Enhancement]
    mEntries[actIdx].noteLow = lo;
    mEntries[actIdx].noteHigh = hi;
    mEntries[actIdx].transpose = static_cast<int8_t>(SuggestedTranspose(fontId, instOrWave, lo, hi));

    auto addRange = [&](uint8_t rLo, uint8_t rHi) {
        if (mEntries.size() >= kMaxEntries)
            return;
        ConfigEntry e = tmpl;
        e.noteLow = rLo;
        e.noteHigh = rHi;
        e.transpose = static_cast<int8_t>(SuggestedTranspose(fontId, instOrWave, rLo, rHi));
        e.selected = true;
        e.source = EntrySource::UserPicked;
        e.lastEnabledSeq = mNextSeq++;
        mEntries.push_back(std::move(e));
    };
    if (lo > 0)
        addRange(0, static_cast<uint8_t>(lo - 1)); // low sample range
    if (hi < 127)
        addRange(static_cast<uint8_t>(hi + 1), 127); // high sample range
    NormalizeSplits(fontId, instOrWave);
    RecomputeActive(fontId, instOrWave);
}

// ── Per-entry mutators ────────────────────────────────────────────────────

void MidiTranslator::SetEntryGain(int idx, float gain) {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return;
    mEntries[idx].gain = gain;
}
void MidiTranslator::SetEntryTranspose(int idx, int8_t semis) {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return;
    mEntries[idx].transpose = semis;
}
static inline int8_t ClampCcOrSentinel(int v) {
    if (v < 0)
        return -1;
    if (v > 127)
        return 127;
    return static_cast<int8_t>(v);
}
void MidiTranslator::SetEntryReverb(int idx, int8_t cc) {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return;
    mEntries[idx].reverb = ClampCcOrSentinel(cc);
}
void MidiTranslator::SetEntryChorus(int idx, int8_t cc) {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return;
    mEntries[idx].chorus = ClampCcOrSentinel(cc);
}
void MidiTranslator::SetEntryFilterCutoff(int idx, int8_t cc) {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return;
    mEntries[idx].cutoff = ClampCcOrSentinel(cc);
}
void MidiTranslator::SetEntryFilterResonance(int idx, int8_t cc) {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return;
    mEntries[idx].q = ClampCcOrSentinel(cc);
}
void MidiTranslator::SetEntryFixedNote(int idx, int16_t note) {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return;
    // -1 = derive from semitone; otherwise a valid MIDI note. No chain
    // recompute: fixedNote changes WHAT a winner plays, not WHICH wins.
    mEntries[idx].fixedNote = (note < 0) ? -1 : static_cast<int16_t>(std::clamp<int>(note, 0, 127));
}
void MidiTranslator::SetEntryRoute(int idx, EntryRoute route) {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return;
    mEntries[idx].route = route;
    // Route doesn't change chain membership today, but recompute keeps the
    // cache authoritative if that ever changes; it's a cheap UI-thread call.
    RecomputeActive(mEntries[idx].fontId, mEntries[idx].instOrWave);
}
void MidiTranslator::SetEntryEnabled(int idx, bool enabled) {
    if (idx < 0 || idx >= static_cast<int>(mEntries.size()))
        return;
    ConfigEntry& e = mEntries[idx];
    e.enabled = enabled;
    if (enabled)
        e.lastEnabledSeq = mNextSeq++;
    RecomputeActive(e.fontId, e.instOrWave);
}
// ── Per-pair display name (row label) ─────────────────────────────────────

std::string MidiTranslator::GetDisplayName(uint8_t fontId, int16_t instOrWave) const {
    auto it = mDisplayName.find({ fontId, instOrWave });
    return it == mDisplayName.end() ? std::string{} : it->second;
}
void MidiTranslator::SetDisplayName(uint8_t fontId, int16_t instOrWave, const std::string& name) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;
    if (name.empty())
        mDisplayName.erase({ fontId, instOrWave });
    else
        mDisplayName[{ fontId, instOrWave }] = name;
}

// ── Session-only transient state ──────────────────────────────────────────

bool MidiTranslator::IsTemporarilyMuted(uint8_t fontId, int16_t instOrWave) const {
    return mTemporaryMute.count({ fontId, instOrWave }) > 0;
}
void MidiTranslator::SetTemporaryMute(uint8_t fontId, int16_t instOrWave, bool muted) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;
    if (muted)
        mTemporaryMute.insert({ fontId, instOrWave });
    else
        mTemporaryMute.erase({ fontId, instOrWave });
}
void MidiTranslator::ClearAllTemporaryMutes() {
    mTemporaryMute.clear();
}
bool MidiTranslator::IsTemporarilySlotMuted(uint8_t fontId, int16_t instOrWave, uint8_t noteLow) const {
    return mTemporarySlotMute.count({ fontId, instOrWave, noteLow }) > 0;
}
void MidiTranslator::SetTemporarySlotMute(uint8_t fontId, int16_t instOrWave, uint8_t noteLow, bool muted) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;
    if (muted)
        mTemporarySlotMute.insert({ fontId, instOrWave, noteLow });
    else
        mTemporarySlotMute.erase({ fontId, instOrWave, noteLow });
}
void MidiTranslator::ClearAllTemporarySlotMutes() {
    mTemporarySlotMute.clear();
}

float MidiTranslator::GetTemporaryVolume(uint8_t fontId, int16_t instOrWave) const {
    auto it = mTemporaryVolume.find({ fontId, instOrWave });
    return it == mTemporaryVolume.end() ? 1.0f : it->second;
}
void MidiTranslator::SetTemporaryVolume(uint8_t fontId, int16_t instOrWave, float vol) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;
    if (vol == 1.0f) {
        mTemporaryVolume.erase({ fontId, instOrWave });
    } else {
        if (vol < 0.0f)
            vol = 0.0f;
        if (vol > 4.0f)
            vol = 4.0f;
        mTemporaryVolume[{ fontId, instOrWave }] = vol;
    }
}
void MidiTranslator::ClearAllTemporaryVolumes() {
    mTemporaryVolume.clear();
}

// ── MIDI channel pool ─────────────────────────────────────────────────────

uint8_t MidiTranslator::GetChannelsInUse() const {
    return mChannelsAllocated;
}

uint32_t MidiTranslator::GetChannelReclaims() const {
    return mChannelReclaims;
}

uint8_t MidiTranslator::ReclaimIdleChannel(uint8_t exceptFontId, int16_t exceptInst) {
    for (uint8_t ch = 0; ch < mChannelsAllocated; ++ch) {
        ChannelOwner& o = mChannelOwner[ch];
        if (o.instOrWave < 0)
            continue; // unowned (shouldn't happen without eager release)
        if (o.fontId == exceptFontId && o.instOrWave == exceptInst)
            continue; // never evict the pair asking for a channel
        if (mSynthActiveByPair[o.fontId][o.instOrWave] != 0)
            continue; // still sounding -- leave it alone
        // Idle pair: drop its claim. The physical channel keeps its program
        // and CC state, and so does mChannelState[ch], so the new owner's
        // ProcessNote re-issues only what differs. No explicit reset needed.
        mPairChannel[o.fontId][o.instOrWave] = 0xFF;
        o.instOrWave = -1;
        return ch;
    }
    return 0xFF;
}

// 0xFF return = no channel available; caller routes the note to native.
uint8_t MidiTranslator::AllocateChannelForPair(uint8_t fontId, int16_t instOrWave) {
    if (!BypassIndexValid(fontId, instOrWave))
        return 0xFF;
    uint8_t& slot = mPairChannel[fontId][instOrWave];
    if (slot != 0xFF)
        return slot;

    // Room to grow the pool: hand out the next channel.
    if (mChannelsAllocated < kMaxMidiChannels) {
        slot = mChannelsAllocated++;
        mChannelOwner[slot] = { fontId, instOrWave };
        return slot;
    }

    // Pool full: reclaim a channel from a pair that has gone quiet (a prior
    // song's instruments). Active pairs keep theirs, so nothing sounding is
    // cut. This is what stops the long-session "instruments stop playing"
    // collapse -- new pairs get a real channel instead of sharing ch 0.
    uint8_t reclaimed = ReclaimIdleChannel(fontId, instOrWave);
    if (reclaimed != 0xFF) {
        mChannelOwner[reclaimed] = { fontId, instOrWave };
        ++mChannelReclaims;
        slot = reclaimed;
        return slot;
    }

    // Genuinely exhausted: all 64 channels are sounding distinct pairs right
    // now. Collapsing onto channel 0 would hijack whatever instrument owns it
    // and play this note with the wrong program, so signal failure instead and
    // let the caller fall back to native. mPairChannel stays unassigned, so a
    // later note retries once any pair goes quiet.
    static bool sLoggedOverflow = false;
    if (!sLoggedOverflow) {
        SPDLOG_WARN("[MidiTranslator] All {} channels sounding at once; "
                    "routing overflow pair to native",
                    (int)kMaxMidiChannels);
        sLoggedOverflow = true;
    }
    return 0xFF;
}

void MidiTranslator::RecordDiscovery(uint8_t fontId, int16_t instOrWave, bool mapped) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;
    const size_t bitIdx = static_cast<size_t>(fontId) * kMaxInstOrWave + instOrWave;
    const size_t wordIdx = bitIdx / 64;
    const uint64_t mask = uint64_t(1) << (bitIdx & 63);
    if (mSeenBits[wordIdx] & mask)
        return;
    mSeenBits[wordIdx] |= mask;

    int slot = mDiscoveredCount.load(std::memory_order_relaxed);
    if (slot >= kMaxDiscovered)
        return;
    mDiscovered[slot] = { fontId, instOrWave, mapped };
    mDiscoveredCount.store(slot + 1, std::memory_order_release);
}

// ── ResetAllOverrides + Persistence ───────────────────────────────────────

void MidiTranslator::ResetAllOverrides() {
    mEntries.clear();
    for (auto& row : mActiveEntryIdx)
        for (auto& cell : row)
            cell = -1;
    mDisplayName.clear();
    for (auto& row : mDrumChannelSynth)
        for (auto& cell : row)
            cell = false;
    for (auto& row : mForcedDrumPool)
        for (auto& cell : row)
            cell = -1;
    for (auto& used : mForcedDrumPoolUsed)
        used = false;
    // Channel allocation, discovery bits, and active-voice counters
    // intentionally survive — they're runtime state, not overrides.
}

// Route <-> JSON string. Synth is the default and is omitted on write.
static const char* RouteToString(EntryRoute r) {
    switch (r) {
        case EntryRoute::Native:
            return "native";
        case EntryRoute::Mute:
            return "mute";
        case EntryRoute::Synth:
            break;
    }
    return "synth";
}
static EntryRoute RouteFromString(const std::string& s) {
    if (s == "native")
        return EntryRoute::Native;
    if (s == "mute")
        return EntryRoute::Mute;
    return EntryRoute::Synth;
}

// Append the note-range split fields to a serialised entry, each omitted at its
// default so unsplit entries serialise compactly. Shared by SaveOverridesToFile
// and ExportPackMapping so the two writers can't drift. (noteLow is part of the
// entry key, so it is written whenever nonzero.)
static void WriteSplitFields(nlohmann::json& entry, const ConfigEntry& e) {
    if (e.noteLow != 0)
        entry["note_low"] = e.noteLow;
    if (e.noteHigh != 127)
        entry["note_high"] = e.noteHigh;
    if (e.fixedNote >= 0)
        entry["fixed_note"] = e.fixedNote;
    if (e.route != EntryRoute::Synth)
        entry["route"] = RouteToString(e.route);
}

// Read the non-key split fields (noteHigh / fixedNote / route) onto an entry.
// Missing keys keep the entry's current values, matching how gain/transpose/
// effects overlay. noteLow is applied separately via the FindOrCreateEntry
// key so distinct splits resolve to distinct entries.
static void ReadSplitFields(const nlohmann::json& entry, ConfigEntry& e) {
    if (entry.contains("note_high")) {
        int nh = entry.value("note_high", 127);
        e.noteHigh = static_cast<uint8_t>(std::clamp(nh, 0, 127));
    }
    if (entry.contains("fixed_note")) {
        int fn = entry.value("fixed_note", -1);
        e.fixedNote = static_cast<int16_t>(std::clamp(fn, -1, 127));
    }
    if (entry.contains("route"))
        e.route = RouteFromString(entry.value("route", std::string("synth")));
}

bool MidiTranslator::SaveOverridesToFile(const std::string& path) const {
    nlohmann::json j;
    j["version"] = 2;
    j["entries"] = nlohmann::json::array();

    // Save criteria: an entry is persisted when the user has expressed
    // intent in it (selected=true) OR the pair has a display name
    // override. Mod-shipped entries the user never touched are NOT
    // persisted — they reload from each pack's mapping.json next session.
    for (const auto& e : mEntries) {
        bool hasDisplayName = false;
        auto dispIt = mDisplayName.find({ e.fontId, e.instOrWave });
        if (dispIt != mDisplayName.end() && !dispIt->second.empty())
            hasDisplayName = true;

        if (!e.selected && !hasDisplayName)
            continue;
        if (e.source != EntrySource::UserPicked)
            continue; // safety

        nlohmann::json entry;
        entry["fontId"] = e.fontId;
        entry["instOrWave"] = e.instOrWave;
        entry["pack"] = e.packName;
        entry["program"] = e.program;
        entry["bank"] = e.bank;
        if (!e.presetName.empty())
            entry["preset_name"] = e.presetName;
        if (e.gain != 0.0f)
            entry["gain"] = e.gain;
        if (e.transpose != 0)
            entry["transpose"] = e.transpose;
        if (e.reverb >= 0)
            entry["reverb"] = e.reverb;
        if (e.chorus >= 0)
            entry["chorus"] = e.chorus;
        if (e.cutoff >= 0)
            entry["filter_cutoff"] = e.cutoff;
        if (e.q >= 0)
            entry["filter_q"] = e.q;
        WriteSplitFields(entry, e);
        entry["enabled"] = e.enabled;
        entry["selected"] = e.selected;
        if (hasDisplayName)
            entry["display_name"] = dispIt->second;
        j["entries"].push_back(std::move(entry));
    }

    // Display-name-only pairs (no matching selected entry) still need a
    // home in the JSON so the label survives a restart. Write a stub
    // entry with display_name set but no pack/program/etc.
    for (const auto& kv : mDisplayName) {
        if (kv.second.empty())
            continue;
        bool alreadyEmitted = false;
        for (const auto& e : mEntries) {
            if (e.fontId == kv.first.first && e.instOrWave == kv.first.second && e.selected &&
                e.source == EntrySource::UserPicked) {
                alreadyEmitted = true;
                break;
            }
        }
        if (alreadyEmitted)
            continue;
        nlohmann::json entry;
        entry["fontId"] = kv.first.first;
        entry["instOrWave"] = kv.first.second;
        entry["display_name"] = kv.second;
        j["entries"].push_back(std::move(entry));
    }

    // Per-pair drum channel mode (the per-instrument Native/Synth master).
    // Separate from per-slot entries, so persisted as its own list of pairs
    // currently in Synth mode (absent => Native).
    j["drum_channels_synth"] = nlohmann::json::array();
    for (int f = 0; f < kMaxFontId; ++f)
        for (int i = 0; i < kDrumHistInst; ++i)
            if (mDrumChannelSynth[f][i])
                j["drum_channels_synth"].push_back({ { "fontId", f }, { "instOrWave", i } });

    // Forced-drum ("Treat as drum") pairs: a per-pair list, mirroring the drum
    // channel-mode list above. The histogram pool slot is runtime-only; only
    // the flag (instOrWave >= kDrumHistInst) is persisted.
    j["forced_drums"] = nlohmann::json::array();
    for (int f = 0; f < kMaxFontId; ++f)
        for (int i = kDrumHistInst; i < kMaxInstOrWave; ++i)
            if (mForcedDrumPool[f][i] >= 0)
                j["forced_drums"].push_back({ { "fontId", f }, { "instOrWave", i } });

    std::ofstream out(path);
    if (!out.is_open()) {
        SPDLOG_WARN("[MidiTranslator] SaveOverridesToFile: cannot open {}", path);
        return false;
    }
    out << j.dump(2);
    return out.good();
}

// Shared predicate for "ship this entry inside a pack mapping?". Same gate used by
// ExportPackMapping and CountExportableEntries so the preview count matches what
// gets written. Exports every entry currently enabled and resolvable for the pack
// (its effective mapping now), regardless of source or `selected`. PickPreset
// keeps at most one entry enabled per (pair, split), so this never double-emits.
static bool ExportEntryMatches(const ConfigEntry& e, const std::string& packNameFilter) {
    if (!e.enabled)
        return false;
    if (e.program < 0)
        return false; // None placeholder — not shippable
    if (e.packName.empty())
        return false;
    if (!packNameFilter.empty() && e.packName != packNameFilter)
        return false;
    return true;
}

int MidiTranslator::CountExportableEntries(const std::string& packNameFilter) const {
    int n = 0;
    for (const auto& e : mEntries)
        if (ExportEntryMatches(e, packNameFilter))
            ++n;
    return n;
}

std::string MidiTranslator::BuildPackMappingJson(const std::string& packNameFilter, int& outWritten) const {
    nlohmann::json j;
    j["version"] = 2;
    // The pack name lives once, in this header. Entries do NOT repeat a
    // per-entry "pack": the loader takes ownership from the pack's discovered
    // name (and falls back to this header), so the field would be redundant.
    if (!packNameFilter.empty())
        j["pack_name"] = packNameFilter;
    j["entries"] = nlohmann::json::array();

    int written = 0;
    for (const auto& e : mEntries) {
        if (!ExportEntryMatches(e, packNameFilter))
            continue;

        nlohmann::json entry;
        entry["fontId"] = e.fontId;
        entry["instOrWave"] = e.instOrWave;
        entry["program"] = e.program;
        entry["bank"] = e.bank;
        if (!e.presetName.empty())
            entry["preset_name"] = e.presetName;
        if (e.gain != 0.0f)
            entry["gain"] = e.gain;
        if (e.transpose != 0)
            entry["transpose"] = e.transpose;
        if (e.reverb >= 0)
            entry["reverb"] = e.reverb;
        if (e.chorus >= 0)
            entry["chorus"] = e.chorus;
        if (e.cutoff >= 0)
            entry["filter_cutoff"] = e.cutoff;
        if (e.q >= 0)
            entry["filter_q"] = e.q;
        WriteSplitFields(entry, e);
        // Pack mapping consumers default enabled=true, selected=false at load
        // time (ApplyOverridesFromString), so we omit both flags here — the
        // file represents "this is the pack's recommended preset for the
        // pair", not "this is currently picked by the user".
        auto dispIt = mDisplayName.find({ e.fontId, e.instOrWave });
        if (dispIt != mDisplayName.end() && !dispIt->second.empty())
            entry["display_name"] = dispIt->second;
        j["entries"].push_back(std::move(entry));
        ++written;
    }

    // Forced-drum flags for the pairs that actually shipped a slot entry above,
    // so a downstream user loads the pack with those pairs already routing as
    // drums (the slot entries alone don't imply the master flag). Scoped to
    // written pairs to avoid leaking unrelated forced pairs into a single-pack
    // export.
    nlohmann::json forced = nlohmann::json::array();
    for (int f = 0; f < kMaxFontId; ++f) {
        for (int i = kDrumHistInst; i < kMaxInstOrWave; ++i) {
            if (mForcedDrumPool[f][i] < 0)
                continue;
            bool shipped = false;
            for (const auto& e : mEntries) {
                if (e.fontId == f && e.instOrWave == i && ExportEntryMatches(e, packNameFilter)) {
                    shipped = true;
                    break;
                }
            }
            if (shipped)
                forced.push_back({ { "fontId", f }, { "instOrWave", i } });
        }
    }
    if (!forced.empty())
        j["forced_drums"] = std::move(forced);

    outWritten = written;
    return j.dump(2);
}

int MidiTranslator::ExportPackMapping(const std::string& path, const std::string& packNameFilter) const {
    int written = 0;
    std::string text = BuildPackMappingJson(packNameFilter, written);

    std::error_code ec;
    auto parent = std::filesystem::path(path).parent_path();
    if (!parent.empty())
        std::filesystem::create_directories(parent, ec);

    std::ofstream out(path);
    if (!out.is_open()) {
        SPDLOG_WARN("[MidiTranslator] ExportPackMapping: cannot open {}", path);
        return -1;
    }
    out << text;
    if (!out.good()) {
        SPDLOG_WARN("[MidiTranslator] ExportPackMapping: write failed for {}", path);
        return -1;
    }
    SPDLOG_INFO("[MidiTranslator] ExportPackMapping: wrote {} entries to {}", written, path);
    return written;
}

bool MidiTranslator::ApplyOverridesFromString(const std::string& json, const std::string& packName) {
    nlohmann::json j;
    try {
        j = nlohmann::json::parse(json);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("[MidiTranslator] ApplyOverridesFromString: parse error: {}", e.what());
        return false;
    }
    // Owner resolution order: the caller's discovered name (authoritative) ->
    // the file's "pack_name" header -> the per-entry "pack" (legacy files).
    std::string headerPack = !packName.empty() ? packName : j.value("pack_name", std::string{});
    auto entries = j.value("entries", nlohmann::json::array());
    int applied = 0;
    for (const auto& entry : entries) {
        int fontId = entry.value("fontId", -1);
        int instOrWave = entry.value("instOrWave", -1);
        if (fontId < 0 || fontId >= kMaxFontId || instOrWave < 0 || instOrWave >= kMaxInstOrWave)
            continue;

        if (entry.contains("display_name")) {
            std::string dn = entry.value("display_name", std::string{});
            SetDisplayName(static_cast<uint8_t>(fontId), static_cast<int16_t>(instOrWave), dn);
        }
        // A routing entry needs a program. Pairs without one are display-name
        // -only stubs (handled just above) -> skip the routing path. The owner
        // pack comes from the header/caller, falling back to a per-entry "pack"
        // only for legacy files that still carry it.
        if (!entry.contains("program"))
            continue;
        std::string pack = !headerPack.empty() ? headerPack : entry.value("pack", std::string{});
        if (pack.empty())
            continue;
        int program = entry.value("program", -1);
        int bank = entry.value("bank", 0);
        if (program < -1 || program > 127)
            continue;
        if (bank < 0)
            bank = 0;
        if (bank > 255)
            bank = 255;
        std::string presetName = entry.value("preset_name", std::string{});
        // note_low is part of the entry key, so it must be resolved before
        // find-or-create or distinct splits collapse into one entry.
        uint8_t noteLow = static_cast<uint8_t>(std::clamp(entry.value("note_low", 0), 0, 127));

        int idx = FindOrCreateEntry(static_cast<uint8_t>(fontId), static_cast<int16_t>(instOrWave), pack,
                                    static_cast<int16_t>(program), static_cast<int16_t>(bank), presetName,
                                    EntrySource::ModSupplied, noteLow);
        if (idx < 0)
            continue;
        ConfigEntry& e = mEntries[idx];
        // Overlay fields. Missing keys keep current values.
        e.bank = static_cast<int16_t>(bank);
        if (!presetName.empty())
            e.presetName = presetName;
        if (entry.contains("gain"))
            e.gain = entry.value("gain", 0.0f);
        if (entry.contains("transpose")) {
            int t = entry.value("transpose", 0);
            t = std::clamp(t, -127, 127);
            e.transpose = static_cast<int8_t>(t);
        }
        if (entry.contains("reverb"))
            e.reverb = ClampCcOrSentinel(entry.value("reverb", -1));
        if (entry.contains("chorus"))
            e.chorus = ClampCcOrSentinel(entry.value("chorus", -1));
        if (entry.contains("filter_cutoff"))
            e.cutoff = ClampCcOrSentinel(entry.value("filter_cutoff", -1));
        if (entry.contains("filter_q"))
            e.q = ClampCcOrSentinel(entry.value("filter_q", -1));
        ReadSplitFields(entry, e);
        // Pack mapping.json: enabled by default (mods publish active
        // presets), selected stays false (user hasn't picked).
        e.enabled = entry.value("enabled", true);
        e.selected = entry.value("selected", false);
        applied++;
    }
    if (j.contains("drum_channels_synth")) {
        for (const auto& d : j["drum_channels_synth"]) {
            int f = d.value("fontId", -1);
            int i = d.value("instOrWave", -1);
            if (f >= 0 && f < kMaxFontId && i >= 0 && i < kDrumHistInst)
                mDrumChannelSynth[f][i] = true;
        }
    }
    if (j.contains("forced_drums")) {
        for (const auto& d : j["forced_drums"]) {
            int f = d.value("fontId", -1);
            int i = d.value("instOrWave", -1);
            if (f >= 0 && f < kMaxFontId && i >= kDrumHistInst && i < kMaxInstOrWave)
                SetForcedDrum(static_cast<uint8_t>(f), static_cast<int16_t>(i), true);
        }
    }
    SPDLOG_INFO("[MidiTranslator] ApplyOverridesFromString: applied {} entries", applied);
    return true;
}

bool MidiTranslator::ApplyOverridesFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        SPDLOG_INFO("[MidiTranslator] ApplyOverridesFromFile: no file at {} (first run?)", path);
        return false;
    }
    nlohmann::json j;
    try {
        in >> j;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("[MidiTranslator] ApplyOverridesFromFile: parse error: {}", e.what());
        return false;
    }
    int version = j.value("version", 1);
    auto entries = j.value("entries", nlohmann::json::array());
    int applied = 0;
    for (const auto& entry : entries) {
        int fontId = entry.value("fontId", -1);
        int instOrWave = entry.value("instOrWave", -1);
        if (fontId < 0 || fontId >= kMaxFontId || instOrWave < 0 || instOrWave >= kMaxInstOrWave)
            continue;

        if (entry.contains("display_name")) {
            std::string dn = entry.value("display_name", std::string{});
            SetDisplayName(static_cast<uint8_t>(fontId), static_cast<int16_t>(instOrWave), dn);
        }
        if (!entry.contains("pack"))
            continue;
        std::string pack = entry.value("pack", std::string{});
        int program = entry.value("program", -1);
        int bank = entry.value("bank", 0);
        if (program < -1 || program > 127)
            continue;
        if (bank < 0)
            bank = 0;
        if (bank > 255)
            bank = 255;
        std::string presetName = entry.value("preset_name", std::string{});
        // note_low is part of the entry key, so it must be resolved before
        // find-or-create or distinct splits collapse into one entry.
        uint8_t noteLow = static_cast<uint8_t>(std::clamp(entry.value("note_low", 0), 0, 127));

        int idx = FindOrCreateEntry(static_cast<uint8_t>(fontId), static_cast<int16_t>(instOrWave), pack,
                                    static_cast<int16_t>(program), static_cast<int16_t>(bank), presetName,
                                    EntrySource::UserPicked, noteLow);
        if (idx < 0)
            continue;
        ConfigEntry& e = mEntries[idx];
        e.bank = static_cast<int16_t>(bank);
        if (!presetName.empty())
            e.presetName = presetName;
        if (entry.contains("gain"))
            e.gain = entry.value("gain", 0.0f);
        if (entry.contains("transpose")) {
            int t = entry.value("transpose", 0);
            t = std::clamp(t, -127, 127);
            e.transpose = static_cast<int8_t>(t);
        }
        if (entry.contains("reverb"))
            e.reverb = ClampCcOrSentinel(entry.value("reverb", -1));
        if (entry.contains("chorus"))
            e.chorus = ClampCcOrSentinel(entry.value("chorus", -1));
        if (entry.contains("filter_cutoff"))
            e.cutoff = ClampCcOrSentinel(entry.value("filter_cutoff", -1));
        if (entry.contains("filter_q"))
            e.q = ClampCcOrSentinel(entry.value("filter_q", -1));
        ReadSplitFields(entry, e);
        // File overlay = user file. A missing key defaults to a user pick:
        // enabled+selected unless the file says otherwise. Older files without
        // these keys migrate to enabled, selected user picks.
        e.enabled = entry.value("enabled", true);
        e.selected = entry.value("selected", true);
        // Promote to UserPicked even if a mod entry already exists for this key;
        // the user file is the source of truth for source attribution.
        e.source = EntrySource::UserPicked;
        applied++;
    }
    if (j.contains("drum_channels_synth")) {
        for (const auto& d : j["drum_channels_synth"]) {
            int f = d.value("fontId", -1);
            int i = d.value("instOrWave", -1);
            if (f >= 0 && f < kMaxFontId && i >= 0 && i < kDrumHistInst)
                mDrumChannelSynth[f][i] = true;
        }
    }
    if (j.contains("forced_drums")) {
        for (const auto& d : j["forced_drums"]) {
            int f = d.value("fontId", -1);
            int i = d.value("instOrWave", -1);
            if (f >= 0 && f < kMaxFontId && i >= kDrumHistInst && i < kMaxInstOrWave)
                SetForcedDrum(static_cast<uint8_t>(f), static_cast<int16_t>(i), true);
        }
    }
    // Repair the split cover on load: older/hand-edited files may carry gaps.
    std::set<std::pair<uint8_t, int16_t>> melodicPairs;
    for (const ConfigEntry& e : mEntries)
        if (e.selected && e.instOrWave >= kDrumHistInst)
            melodicPairs.insert({ e.fontId, e.instOrWave });
    for (const auto& p : melodicPairs) {
        NormalizeSplits(p.first, p.second);
        RecomputeActive(p.first, p.second);
    }
    SPDLOG_INFO("[MidiTranslator] ApplyOverridesFromFile: applied {} entries from {} (v{})", applied, path, version);
    return true;
}

// ── ProcessNote ───────────────────────────────────────────────────────────

bool MidiTranslator::ProcessNote(int noteIndex, float freqScale, float velocity, uint8_t pan, float channelVolume,
                                 uint8_t fontId, int16_t instOrWave, uint8_t semitone, bool isFinished,
                                 uint8_t channelIdx, float resampleRate, float pitchBend) {
    (void)resampleRate;
    (void)channelIdx;

    if (noteIndex < 0 || noteIndex >= kMaxNotes)
        return false;

    auto synth = SOH::MidiSynthManager::Instance().GetActiveSynth();
    if (!synth)
        return false;

    NoteTranslatorState& state = mNoteState[noteIndex];

    // Discovery uses GetGmPreset purely as a "did the built-in GM table
    // know about this pair?" hint for the UI rows. Resolution itself runs
    // off mActiveEntryIdx now.
    GmPreset builtinGm = GetGmPreset(fontId, instOrWave);
    RecordDiscovery(fontId, instOrWave, builtinGm.program != kUnmapped);

    // DEBUG: per-pair stats. Detect a fresh NoteOn (Idle → playing transition);
    // record the engine semitone. Route counters are incremented at the
    // dispatch decision below — guarded by `wasIdleStart` so continuation
    // frames of the same note don't double-count.
    const bool wasIdleStart =
        BypassIndexValid(fontId, instOrWave) && state.kind == SlotKind::Idle && velocity > 0.0f && !isFinished;
    if (wasIdleStart) {
        auto& dbg = mDebugStats[fontId][instOrWave];
        dbg.noteOns.fetch_add(1, std::memory_order_relaxed);
        dbg.lastSemitone = semitone;
        // Drum/SFX slot histogram: on those channels `semitone` is a slot
        // index, so this is the per-slot fire count the drum auto-split reads.
        // For a forced-drum melodic pair `semitone` is a real pitch, but the
        // drum UI treats each distinct incoming value as a slot all the same, so
        // the same histogram (a pool slot, via DrumHistFor) drives discovery.
        if (semitone < kDrumHistSlots) {
            if (DrumSlotHit* hist = DrumHistFor(fontId, instOrWave))
                hist[semitone].count.fetch_add(1, std::memory_order_relaxed);
        }
    }
    // Helper for the route-decision counters below. Only counts on a fresh
    // NoteOn so per-frame continuation calls don't inflate the totals.
    auto bumpRoute = [&](std::atomic<uint32_t> DebugSlot::*counter) {
        if (wasIdleStart && BypassIndexValid(fontId, instOrWave))
            (mDebugStats[fontId][instOrWave].*counter).fetch_add(1, std::memory_order_relaxed);
    };
    DBG_LOG(fontId, instOrWave, semitone, freqScale, builtinGm.program != kUnmapped, builtinGm.bank, builtinGm.program);

    auto retireSlot = [&]() {
        if (state.kind == SlotKind::Synth) {
            synth->NoteOff(state.channel, state.midiNote);
            if (BypassIndexValid(state.pairFontId, state.pairInstOrWave) &&
                mSynthActiveByPair[state.pairFontId][state.pairInstOrWave] > 0) {
                mSynthActiveByPair[state.pairFontId][state.pairInstOrWave]--;
            }
            DecEntryActive(true, state.activeEntryIdx);
        } else if (state.kind == SlotKind::Native) {
            if (BypassIndexValid(state.pairFontId, state.pairInstOrWave) &&
                mNativeActiveByPair[state.pairFontId][state.pairInstOrWave] > 0) {
                mNativeActiveByPair[state.pairFontId][state.pairInstOrWave]--;
            }
            DecEntryActive(false, state.activeEntryIdx);
        }
        state.kind = SlotKind::Idle;
        state.pairInstOrWave = -1;
        state.activeEntryIdx = -1;
    };

    // entryIdx attributes the native note to a split row (route=Native) so its
    // row lights up; -1 for the plain "no entry covers this" fall-through.
    auto adoptNative = [&](int entryIdx) {
        if (state.kind != SlotKind::Native || state.pairFontId != fontId || state.pairInstOrWave != instOrWave ||
            state.activeEntryIdx != entryIdx) {
            retireSlot();
            state.kind = SlotKind::Native;
            state.pairFontId = fontId;
            state.pairInstOrWave = instOrWave;
            state.activeEntryIdx = static_cast<int16_t>(entryIdx);
            if (BypassIndexValid(fontId, instOrWave) && mNativeActiveByPair[fontId][instOrWave] < 255) {
                mNativeActiveByPair[fontId][instOrWave]++;
            }
            IncEntryActive(false, entryIdx);
        }
    };

    // Transient mute (Solo button) silences BOTH paths.
    if (mTemporaryMute.count({ fontId, instOrWave })) {
        retireSlot();
        return true;
    }

    // Drum-like routing covers the intrinsic drum/SFX channels (instOrWave 0/1)
    // and any pair the user flagged "Treat as drum". For both, the incoming
    // `semitone` indexes a slot row whose fixedNote picks the played sound.
    const bool isForcedDrum = IsForcedDrum(fontId, instOrWave);
    const bool drumLike = (instOrWave == 0 || instOrWave == 1) || isForcedDrum;

    // Per-slot transient mute for drum-like pairs, BEFORE resolution: a slot's
    // semitone IS its noteLow, so this catches native slots too (which skip the
    // post-resolution check by falling through). Melodic ranges are still
    // handled post-resolution by the covering entry's noteLow.
    if (drumLike && semitone < 128 &&
        mTemporarySlotMute.count({ fontId, instOrWave, static_cast<uint8_t>(semitone) })) {
        retireSlot();
        return true;
    }

    // Engine drum (instOrWave==0) and SFX (instOrWave==1): the `semitone` byte is a
    // slot index (Audio_GetDrum / Audio_GetSfx), not a chromatic pitch. The per-
    // instrument master mode gates the channel: in Native mode every slot plays the
    // engine drum; only in Synth mode does per-slot resolution decide synth vs native.
    if ((instOrWave == 0 || instOrWave == 1) && !mDrumChannelSynth[fontId][instOrWave]) {
        int nativeAttrib = FindSlotEntryIdx(fontId, instOrWave, static_cast<uint8_t>(semitone & 0x7F));
        if (isFinished || velocity <= 0.0f)
            retireSlot();
        else
            adoptNative(nativeAttrib);
        bumpRoute(&DebugSlot::routedNative);
        return false;
    }

    // ── Resolution: walk the active-split chain ─────────────────────────
    if (!BypassIndexValid(fontId, instOrWave)) {
        if (isFinished || velocity <= 0.0f)
            retireSlot();
        else
            adoptNative(-1);
        // bumpRoute is a no-op outside the bypass range — fine.
        return false;
    }
    // Find the entry whose engine-semitone range covers this note. The chain is
    // sorted by resolution priority, so the FIRST covering entry is the winner even
    // when ranges overlap (RecomputeActive links each entry at most once -> acyclic);
    // an unsplit pair has a length-1 chain so this is the same single lookup as before.
    int activeIdx = mActiveEntryIdx[fontId][instOrWave];
    while (activeIdx >= 0 && activeIdx < static_cast<int>(mEntries.size())) {
        const ConfigEntry& cand = mEntries[activeIdx];
        if (semitone >= cand.noteLow && semitone <= cand.noteHigh)
            break;
        activeIdx = cand.nextActiveSplit;
    }
    if (activeIdx < 0 || activeIdx >= static_cast<int>(mEntries.size())) {
        // No enabled entry covers this slot → native plays. For a drum-like
        // slot, attribute the native note to its slot row (the disabled entry)
        // so the row's activity tint lights even in Native mode.
        int nativeAttrib = -1;
        if (drumLike)
            nativeAttrib = FindSlotEntryIdx(fontId, instOrWave, static_cast<uint8_t>(semitone & 0x7F));
        // A slot explicitly set to None silences even when the channel is in
        // Native mode (its entry is disabled, so it lands here, not in the
        // route==Mute branch below). This is how "mute note 0 while the rest
        // play native" works.
        if (nativeAttrib >= 0 && mEntries[nativeAttrib].route == EntryRoute::Mute) {
            retireSlot();
            bumpRoute(&DebugSlot::routedMute);
            return true;
        }
        if (isFinished || velocity <= 0.0f)
            retireSlot();
        else
            adoptNative(nativeAttrib);
        bumpRoute(&DebugSlot::routedNative);
        return false;
    }
    const ConfigEntry& e = mEntries[activeIdx];

    // Per-split transient mute (Solo/Mute on an individual drum slot or melodic
    // range). Silences both paths for this slot, like the pair-level mute.
    if (mTemporarySlotMute.count({ fontId, instOrWave, e.noteLow })) {
        retireSlot();
        return true;
    }

    // Native-route split: this entry won its range but the audible path is
    // the engine sample (a range the author wants to keep native while a
    // sibling range synths). First-class winner, not the "no entry" fall-
    // through, so a wider synth split can't shadow it.
    if (e.route == EntryRoute::Native) {
        if (isFinished || velocity <= 0.0f)
            retireSlot();
        else
            adoptNative(activeIdx);
        bumpRoute(&DebugSlot::routedNative);
        return false;
    }

    // Placeholder ("None") / Mute entry: explicit user intent to mute the
    // synth path while not letting native sneak in. Same return value as the
    // Solo-button mute — true tells the C hook to suppress the engine.
    if (e.route == EntryRoute::Mute || e.program < 0) {
        retireSlot();
        bumpRoute(&DebugSlot::routedMute);
        return true;
    }

    const uint8_t gmBank = static_cast<uint8_t>(e.bank);
    const uint8_t gmProgram = static_cast<uint8_t>(e.program);
    const int16_t pinSfont = e.sfontId;

    // GM percussion (bank 128) now synths: the entry's range claims the
    // engine drum slot and `fixedNote` (below) picks the GM percussion note
    // to fire. ProgramSelect(bank=128) sets the channel to drum type, so the
    // per-pair channel allocator handles it like any other preset.

    // Per-pair channel allocation — same channel for the life of a pair
    // so per-pair effect CCs survive across notes.
    uint8_t targetChannel = AllocateChannelForPair(fontId, instOrWave);
    if (targetChannel == 0xFF) {
        // Channel pool momentarily exhausted (all 64 sounding) -> native, so
        // the instrument still plays instead of corrupting another channel.
        if (isFinished || velocity <= 0.0f)
            retireSlot();
        else
            adoptNative(-1);
        bumpRoute(&DebugSlot::routedNative);
        return false;
    }

    // Output MIDI note. A fixedNote (>= 0) pins it directly -- a GM percussion
    // note for a bank-128 entry, or a tuned pitch -- because on a drum slot
    // the engine `semitone` is a slot index, not a playable pitch. Melodic
    // entries (fixedNote < 0) derive it from semitone + offset + transpose.
    const bool hasFixedNote = (e.fixedNote >= 0);
    int transpose = static_cast<int>(e.transpose);
    int midiRaw = hasFixedNote ? static_cast<int>(e.fixedNote)
                               : static_cast<int>(semitone) + kEngineSemitoneToMidiOffset + transpose;
    uint8_t midiNote = static_cast<uint8_t>(std::clamp(midiRaw, 0, 127));
    // A pinned note must not ride the engine's per-sample resampling bend --
    // for a drum slot that ratio is computed against a slot index and is
    // meaningless -- so neutralise the wheel for fixed-note entries.
    const float effectiveBend = hasFixedNote ? 1.0f : pitchBend;

    uint16_t preset = (static_cast<uint16_t>(gmBank) << 8) | gmProgram;
    ChannelState& chState = mChannelState[targetChannel];

    if (isFinished || velocity <= 0.0f) {
        retireSlot();
        return true;
    }

    // ── ProgramSelect (pinned to entry's sfontId) ────────────────────────
    if (preset != chState.lastPreset || pinSfont != chState.lastPinSfont) {
        bool ok = synth->ProgramSelect(targetChannel, pinSfont, gmBank, gmProgram);
        if (!ok) {
            // Pin failed mid-session — fall back to native for this note.
            retireSlot();
            adoptNative(-1);
            bumpRoute(&DebugSlot::routedNative);
            return false;
        }
        chState.lastPreset = preset;
        chState.lastPinSfont = pinSfont;
    }

    uint8_t cc7val = static_cast<uint8_t>(std::clamp(channelVolume * 127.0f, 0.0f, 127.0f));
    if (cc7val != chState.lastVolumeCC) {
        synth->ControlChange(targetChannel, 7, static_cast<uint16_t>(cc7val) << 7);
        chState.lastVolumeCC = cc7val;
    }

    uint8_t cc10val = static_cast<uint8_t>(pan & 0x7F);
    if (cc10val != chState.lastPanCC) {
        synth->ControlChange(targetChannel, 10, static_cast<uint16_t>(cc10val) << 7);
        chState.lastPanCC = cc10val;
    }

    auto resolveEffectCc = [](int8_t ovr, uint8_t fallback) -> uint8_t {
        return (ovr >= 0) ? static_cast<uint8_t>(ovr) : fallback;
    };
    uint8_t reverbVal = resolveEffectCc(e.reverb, 0);
    uint8_t chorusVal = resolveEffectCc(e.chorus, 0);
    uint8_t cutoffVal = resolveEffectCc(e.cutoff, 64);
    uint8_t qVal = resolveEffectCc(e.q, 64);
    if (reverbVal != chState.lastReverbCC) {
        synth->ControlChange(targetChannel, 91, static_cast<uint16_t>(reverbVal) << 7);
        chState.lastReverbCC = reverbVal;
    }
    if (chorusVal != chState.lastChorusCC) {
        synth->ControlChange(targetChannel, 93, static_cast<uint16_t>(chorusVal) << 7);
        chState.lastChorusCC = chorusVal;
    }
    if (cutoffVal != chState.lastCutoffCC) {
        synth->ControlChange(targetChannel, 74, static_cast<uint16_t>(cutoffVal) << 7);
        chState.lastCutoffCC = cutoffVal;
    }
    if (qVal != chState.lastQCC) {
        synth->ControlChange(targetChannel, 71, static_cast<uint16_t>(qVal) << 7);
        chState.lastQCC = qVal;
    }
    chState.inited = true;

    // ── Velocity shaping ──
    float entryGain = e.gain;
    if (entryGain == 0.0f)
        entryGain = 1.0f;
    float tempVol = GetTemporaryVolume(fontId, instOrWave);
    float shaped = std::clamp(sqrtf(std::max(0.0f, velocity)) * mGlobalGain * entryGain * tempVol, 0.0f, 1.0f);

    uint8_t noteOnVel;
    uint8_t cc11val;
    if (mSynthMode == SynthMode::Enhanced) {
        noteOnVel = static_cast<uint8_t>(shaped * 127.0f);
        cc11val = 127;
    } else {
        noteOnVel = kFixedNoteOnVelocity;
        cc11val = static_cast<uint8_t>(shaped * 127.0f);
    }
    synth->ControlChange(targetChannel, 11, static_cast<uint16_t>(cc11val) << 7);

    // `pitchBend` is the engine's per-note pitch deviation from the nominal note, as
    // a frequency ratio (1.0 = no bend). audio_playback.c already stripped the non-
    // pitch factors (sample tuning, resampleRate, and the note itself, which
    // FluidSynth plays as midiNote), so hand the ratio straight to the synth.

    if (state.kind != SlotKind::Synth || state.pairFontId != fontId || state.pairInstOrWave != instOrWave ||
        midiNote != state.midiNote) {
        retireSlot();
        // Drums: cut any voice already sounding this percussion note on the shared
        // channel before retriggering. Percussion samples are one-shot (FluidSynth
        // ignores NoteOff), so rapid hits would otherwise stack voices and overflow
        // the voice ring. Melodic notes (no fixedNote) keep their polyphony intact.
        if (hasFixedNote)
            synth->NoteOff(targetChannel, midiNote);
        synth->NoteOnPitchFactor(targetChannel, midiNote, noteOnVel, effectiveBend);
        state.kind = SlotKind::Synth;
        state.channel = targetChannel;
        state.midiNote = midiNote;
        state.pairFontId = fontId;
        state.pairInstOrWave = instOrWave;
        state.activeEntryIdx = static_cast<int16_t>(activeIdx);
        state.lastFreqScale = freqScale;
        if (mSynthActiveByPair[fontId][instOrWave] < 255) {
            mSynthActiveByPair[fontId][instOrWave]++;
        }
        IncEntryActive(true, activeIdx);
        bumpRoute(&DebugSlot::routedSynth);
        return true;
    }

    if (!hasFixedNote && fabsf(freqScale - state.lastFreqScale) > 1e-6f) {
        synth->PitchBendFactor(state.channel, pitchBend);
        state.lastFreqScale = freqScale;
    }
    return true;
}

void MidiTranslator::NoteDisabled(int noteIndex) {
    if (noteIndex < 0 || noteIndex >= kMaxNotes)
        return;
    NoteTranslatorState& state = mNoteState[noteIndex];
    if (state.kind == SlotKind::Synth) {
        auto synth = SOH::MidiSynthManager::Instance().GetActiveSynth();
        if (synth)
            synth->NoteOff(state.channel, state.midiNote);
        if (BypassIndexValid(state.pairFontId, state.pairInstOrWave) &&
            mSynthActiveByPair[state.pairFontId][state.pairInstOrWave] > 0) {
            mSynthActiveByPair[state.pairFontId][state.pairInstOrWave]--;
        }
        DecEntryActive(true, state.activeEntryIdx);
    } else if (state.kind == SlotKind::Native) {
        if (BypassIndexValid(state.pairFontId, state.pairInstOrWave) &&
            mNativeActiveByPair[state.pairFontId][state.pairInstOrWave] > 0) {
            mNativeActiveByPair[state.pairFontId][state.pairInstOrWave]--;
        }
        DecEntryActive(false, state.activeEntryIdx);
    }
    state.kind = SlotKind::Idle;
    state.pairInstOrWave = -1;
    state.activeEntryIdx = -1;
}

} // namespace SOH

// ─── C-linkage entry points called from audio_playback.c ───────────────────

extern "C" {

bool SOH_MidiTranslator_ProcessNote(int noteIndex, float freqScale, float velocity, uint8_t pan, float channelVolume,
                                    uint8_t fontId, int16_t instOrWave, uint8_t semitone, bool isFinished,
                                    uint8_t channelIdx, float resampleRate, float pitchBend) {
    return SOH::MidiTranslator::Instance().ProcessNote(noteIndex, freqScale, velocity, pan, channelVolume, fontId,
                                                       instOrWave, semitone, isFinished, channelIdx, resampleRate,
                                                       pitchBend);
}

void SOH_MidiTranslator_NoteDisabled(int noteIndex) {
    SOH::MidiTranslator::Instance().NoteDisabled(noteIndex);
}

void SOH_MidiTranslator_Reset() {
    SOH::MidiTranslator::Instance().Reset();
}

} // extern "C"
