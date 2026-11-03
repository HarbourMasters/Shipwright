#include "MidiTranslator.h"
#include "GmInstrumentMap.h"
#include "InstrumentNames.h"
#include "soh/Enhancements/audio/MidiSynthManager.h"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <chrono>
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

// Fonts are persisted by resource name: modded fonts get their numeric id from
// archive listing order (audio_load.c), so the number shifts with the mod set,
// while the name follows the font. Vanilla ids (< kBuiltinFontCount) are baked
// into the resource and stable.
static const char* FontPathForId(uint8_t fontId) {
    if (fontMap != nullptr && fontId < fontMapSize)
        return fontMap[fontId]; // may be null for holes
    return nullptr;
}
static int ResolveFontIdByPath(const std::string& path) {
    if (fontMap == nullptr || path.empty())
        return -1;
    for (size_t i = 0; i < fontMapSize; i++)
        if (fontMap[i] != nullptr && path == fontMap[i])
            return static_cast<int>(i);
    return -1;
}

// Engine semitone -> MIDI note offset for melodic instruments.
//
// The engine's gNoteFrequencies table labels semitone 39 as "NOTE_C4
// (Middle C)" with freqScale 1.0, which gives the textbook offset of
// 60 - 39 = 21 against standard GM tuning.
static constexpr int kEngineSemitoneToMidiOffset = 21;

// Held near-max so the SF's default velocity attenuation modulator
// doesn't silence quiet notes. A note's own dynamics ride key pressure
// instead (Authentic mode) or NoteOn velocity (Enhanced mode).
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
    for (auto& row : mKeyHold)
        for (auto& cell : row)
            cell = 0;
    for (uint8_t& count : mChannelNotes)
        count = 0;
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

// Monotonic ms, nonzero (0 means "never hit" in DrumSlotHit::lastHitMs).
static uint32_t NowMs() {
    using namespace std::chrono;
    static const steady_clock::time_point start = steady_clock::now();
    return static_cast<uint32_t>(duration_cast<milliseconds>(steady_clock::now() - start).count()) | 1u;
}

bool MidiTranslator::DrumSlotRecentlyActive(uint8_t fontId, int16_t instOrWave, uint8_t slot) const {
    if (slot >= kDrumHistSlots)
        return false;
    const DrumSlotHit* hist = DrumHistFor(fontId, instOrWave);
    if (!hist)
        return false;
    uint32_t t = hist[slot].lastHitMs.load(std::memory_order_relaxed);
    return t != 0 && NowMs() - t < 250;
}

bool MidiTranslator::AnyDrumSlotRecentlyActive(uint8_t fontId, int16_t instOrWave) const {
    const DrumSlotHit* hist = DrumHistFor(fontId, instOrWave);
    if (!hist)
        return false;
    // Restrict the real drum channel to its sampled slots: sequences also fire
    // control slots the font leaves empty (silent natively), which would keep
    // the tint permanently on.
    bool sampledOnly[kDrumHistSlots] = {};
    bool restrict = false;
    if (instOrWave == 0) {
        for (const auto& d : EnumerateDrumSlots(fontId)) {
            if (d.slot < kDrumHistSlots)
                sampledOnly[d.slot] = true;
            restrict = true;
        }
    }
    const uint32_t now = NowMs();
    for (int s = 0; s < kDrumHistSlots; ++s) {
        if (restrict && !sampledOnly[s])
            continue;
        uint32_t t = hist[s].lastHitMs.load(std::memory_order_relaxed);
        if (t != 0 && now - t < 250)
            return true;
    }
    return false;
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

// ── Mod stack + sfontId resolution ────────────────────────────────────────

void MidiTranslator::SetModOrder(const std::vector<std::string>& order) {
    mModOrder = order;
}

int MidiTranslator::ModRank(const std::string& mod) const {
    for (size_t i = 0; i < mModOrder.size(); i++) {
        if (mModOrder[i] == mod)
            return static_cast<int>(i);
    }
    return -1;
}

int16_t MidiTranslator::ResolveSfontIdFromCache(const std::string& mod, const std::string& pack, int16_t bank,
                                                int16_t program) const {
    if (pack.empty() || program < 0) {
        // Placeholder entries — synthetic sfontId=0 so they resolve and
        // mute via the program<0 branch in ProcessNote.
        return 0;
    }
    for (const auto& lp : mLoadedPresets) {
        if (lp.modName == mod && lp.packName == pack && lp.bank == bank && lp.program == program) {
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
        e.sfontId = ResolveSfontIdFromCache(e.modName, e.packName, e.bank, e.program);
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
    // A flag can outlive its pair's rows, and must then be cleared.
    for (int f = 0; f < kMaxFontId; ++f)
        for (int i = 0; i < kMaxInstOrWave; ++i)
            if ((i < kDrumHistInst && mDrumChannelSynth[f][i]) || mForcedDrumPool[f][i] >= 0)
                pairs.insert({ static_cast<uint8_t>(f), static_cast<int16_t>(i) });
    for (const auto& [mod, f, i] : mModDrumChannelSynth)
        pairs.insert({ f, i });
    for (const auto& [mod, f, i] : mModForcedDrums)
        pairs.insert({ f, i });
    for (const auto& p : pairs)
        RecomputeActive(p.first, p.second);
}

// Total order ranking entries for a semitone (drives both the per-semitone pick
// and the chain sort): 1) specificity -- narrower range beats wider; 2) mod
// order -- later-mounted wins; 3) entry order -- later wins. Nothing here may
// depend on the session, or what plays would differ from what the mods load as.
bool MidiTranslator::Outranks(int ai, int bi) const {
    const ConfigEntry& a = mEntries[ai];
    const ConfigEntry& b = mEntries[bi];
    int wa = static_cast<int>(a.noteHigh) - static_cast<int>(a.noteLow);
    int wb = static_cast<int>(b.noteHigh) - static_cast<int>(b.noteLow);
    if (wa != wb)
        return wa < wb; // narrower == more specific -> wins
    int ra = ModRank(a.modName);
    int rb = ModRank(b.modName);
    if (ra != rb)
        return ra > rb;
    return ai > bi;
}

static inline bool IsResolvable(const ConfigEntry& e) {
    // Native/Mute splits don't reference a soundfont, so they qualify
    // without a resolved sfontId; only Synth entries need one.
    return e.selected && e.enabled && (e.route != EntryRoute::Synth || e.sfontId >= 0);
}

std::vector<int> MidiTranslator::ResolveWinners(const std::vector<int>& candidates) const {
    // Per-semitone winner: the top-ranked candidate whose [noteLow,noteHigh]
    // covers that engine slot. For an unsplit pair every semitone resolves to
    // the same single entry (or none), collapsing the chain to length 1.
    int winnerAt[128];
    for (int s = 0; s < 128; ++s) {
        int best = -1;
        for (int idx : candidates) {
            const ConfigEntry& e = mEntries[idx];
            if (s < e.noteLow || s > e.noteHigh)
                continue;
            if (best < 0 || Outranks(idx, best))
                best = idx;
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
    std::sort(winners.begin(), winners.end(), [this](int a, int b) { return Outranks(a, b); });
    return winners;
}

void MidiTranslator::RecomputeActive(uint8_t fontId, int16_t instOrWave) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;

    std::vector<int> candidates;
    for (size_t idx = 0; idx < mEntries.size(); idx++) {
        const ConfigEntry& e = mEntries[idx];
        if (e.fontId == fontId && e.instOrWave == instOrWave && IsResolvable(e))
            candidates.push_back(static_cast<int>(idx));
    }
    const std::vector<int> winners = ResolveWinners(candidates);

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
    RecomputePairFlags(fontId, instOrWave);
}

std::vector<int> MidiTranslator::ModWinners(const std::string& mod, uint8_t fontId, int16_t instOrWave) const {
    std::vector<int> candidates;
    for (size_t idx = 0; idx < mEntries.size(); idx++) {
        const ConfigEntry& e = mEntries[idx];
        if (e.fontId == fontId && e.instOrWave == instOrWave && e.modName == mod && IsResolvable(e))
            candidates.push_back(static_cast<int>(idx));
    }
    return ResolveWinners(candidates);
}

const std::string* MidiTranslator::OwnerMod(uint8_t fontId, int16_t instOrWave) const {
    const std::string* owner = nullptr;
    int ownerRank = -2;
    auto consider = [&](const std::string& mod) {
        const int rank = ModRank(mod);
        if (rank > ownerRank) {
            owner = &mod;
            ownerRank = rank;
        }
    };
    for (const auto& e : mEntries)
        if (e.fontId == fontId && e.instOrWave == instOrWave && e.selected)
            consider(e.modName);
    for (const auto& key : mModDrumChannelSynth)
        if (std::get<1>(key) == fontId && std::get<2>(key) == instOrWave)
            consider(std::get<0>(key));
    for (const auto& key : mModForcedDrums)
        if (std::get<1>(key) == fontId && std::get<2>(key) == instOrWave)
            consider(std::get<0>(key));
    return owner;
}

void MidiTranslator::RecomputePairFlags(uint8_t fontId, int16_t instOrWave) {
    const std::string* owner = OwnerMod(fontId, instOrWave);
    const auto flagged = [&](const std::set<ModPair>& flags) {
        return owner != nullptr && flags.contains({ *owner, fontId, instOrWave });
    };
    if (instOrWave < kDrumHistInst)
        mDrumChannelSynth[fontId][instOrWave] = flagged(mModDrumChannelSynth);
    else
        ApplyForcedDrumFlag(fontId, instOrWave, flagged(mModForcedDrums));
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
const ConfigEntry* MidiTranslator::GetViewEntry(uint8_t fontId, int16_t instOrWave) const {
    const int idx = GetViewEntryIdx(fontId, instOrWave);
    return idx < 0 ? nullptr : &mEntries[idx];
}

int MidiTranslator::GetViewEntryIdx(uint8_t fontId, int16_t instOrWave) const {
    if (!BypassIndexValid(fontId, instOrWave))
        return -1;
    if (mEditMod.empty())
        return mActiveEntryIdx[fontId][instOrWave];
    const std::vector<int> winners = ModWinners(mEditMod, fontId, instOrWave);
    return winners.empty() ? -1 : winners.front();
}

void MidiTranslator::GetEntriesForPair(uint8_t fontId, int16_t instOrWave, std::vector<int>& outIdx) const {
    outIdx.clear();
    std::string viewMod = mEditMod;
    if (viewMod.empty()) {
        const ConfigEntry* active = GetActiveEntry(fontId, instOrWave);
        if (active == nullptr)
            return;
        viewMod = active->modName;
    }
    for (size_t idx = 0; idx < mEntries.size(); idx++) {
        const ConfigEntry& e = mEntries[idx];
        if (e.fontId == fontId && e.instOrWave == instOrWave && e.selected && e.modName == viewMod) {
            outIdx.push_back(static_cast<int>(idx));
        }
    }
}

void MidiTranslator::SetEditMod(const std::string& modName) {
    mEditMod = modName;
}

bool MidiTranslator::InEditMod(const ConfigEntry& e, uint8_t fontId, int16_t instOrWave) const {
    return e.fontId == fontId && e.instOrWave == instOrWave && e.selected && !mEditMod.empty() && e.modName == mEditMod;
}

bool MidiTranslator::IsEditable(int idx) const {
    return idx >= 0 && idx < static_cast<int>(mEntries.size()) && !mEditMod.empty() && mEntries[idx].selected &&
           mEntries[idx].modName == mEditMod;
}

int MidiTranslator::FindOrCreateEntry(const std::string& mod, uint8_t fontId, int16_t instOrWave,
                                      const std::string& pack, int16_t program, int16_t bank,
                                      const std::string& presetName, uint8_t noteLow) {
    int retired = -1;
    for (size_t idx = 0; idx < mEntries.size(); idx++) {
        const ConfigEntry& e = mEntries[idx];
        if (e.modName == mod && e.fontId == fontId && e.instOrWave == instOrWave && e.packName == pack &&
            e.program == program && e.noteLow == noteLow) {
            if (e.selected)
                return static_cast<int>(idx);
            retired = static_cast<int>(idx);
        }
    }
    // Rows are never erased, so switching presets back and forth would fill the
    // pool; bringing a retired row back also restores its gain and shift.
    if (retired >= 0) {
        mEntries[retired].selected = true;
        return retired;
    }
    if (mEntries.size() >= kMaxEntries) {
        SPDLOG_WARN("[MidiTranslator] Entry pool full ({}); dropping new entry "
                    "for f={}, i={}, pack='{}', program={}",
                    kMaxEntries, fontId, instOrWave, pack, program);
        return -1;
    }
    ConfigEntry e;
    e.fontId = fontId;
    e.instOrWave = instOrWave;
    e.modName = mod;
    e.packName = pack;
    e.program = program;
    e.bank = bank;
    e.presetName = presetName;
    e.selected = true;
    e.noteLow = noteLow; // noteHigh stays 127 (full range) until a split sets it
    // Resolve sfontId immediately so the entry is eligible for resolution on the
    // next note. ResolveSfontIdFromCache returns 0 for placeholder entries so they
    // resolve and produce the silent NoteOn the user asked for.
    e.sfontId = ResolveSfontIdFromCache(mod, pack, bank, program);
    mEntries.push_back(std::move(e));
    return static_cast<int>(mEntries.size()) - 1;
}

// ── UI row actions ────────────────────────────────────────────────────────

void MidiTranslator::PickPreset(uint8_t fontId, int16_t instOrWave, const std::string& pack, int16_t program,
                                int16_t bank, const std::string& presetName) {
    if (!BypassIndexValid(fontId, instOrWave) || mEditMod.empty())
        return;

    const size_t entryCountBefore = mEntries.size();
    int idx = FindOrCreateEntry(mEditMod, fontId, instOrWave, pack, program, bank, presetName);
    if (idx < 0)
        return;
    for (size_t other = 0; other < mEntries.size(); other++) {
        ConfigEntry& e = mEntries[other];
        if (static_cast<int>(other) != idx && InEditMod(e, fontId, instOrWave)) {
            e.enabled = false;
            e.selected = false;
        }
    }
    ConfigEntry& e = mEntries[idx];
    // On a freshly created entry, seed the octave Shift from the engine sample's
    // tuning so a substitute lands in the native octave by default; user-overridable.
    // SOH [Enhancement]
    if (mEntries.size() > entryCountBefore) {
        e.transpose = static_cast<int8_t>(SuggestedTranspose(fontId, instOrWave, e.noteLow, e.noteHigh));
    }
    // Preserve gain/transpose/effects when reusing; only program/bank/pack identify
    // the entry. presetName updates so renamed presets surface.
    e.bank = bank;
    e.presetName = presetName;
    e.sfontId = ResolveSfontIdFromCache(mEditMod, pack, bank, program);
    e.noteLow = 0;
    e.noteHigh = 127;
    e.fixedNote = -1;
    e.route = EntryRoute::Synth;
    e.enabled = true;
    RecomputeActive(fontId, instOrWave);
}

void MidiTranslator::RemovePairRows(uint8_t fontId, int16_t instOrWave) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;
    for (auto& e : mEntries) {
        if (InEditMod(e, fontId, instOrWave)) {
            e.enabled = false;
            e.selected = false;
        }
    }
    RecomputeActive(fontId, instOrWave);
}

void MidiTranslator::SetAuditionNative(uint8_t fontId, int16_t instOrWave, bool native) {
    if (BypassIndexValid(fontId, instOrWave))
        mAuditionNative[fontId][instOrWave] = native;
}

bool MidiTranslator::IsAuditionNative(uint8_t fontId, int16_t instOrWave) const {
    return BypassIndexValid(fontId, instOrWave) && mAuditionNative[fontId][instOrWave];
}

// Find a drum pair's current kit (pack/program) from the edited mod's bank-128
// slot entries, so re-running auto-split or toggling Synth preserves the kit
// choice. Falls back to the mod's first bank-128 preset, then to an empty pack
// (slots stay native until a kit is assigned).
void MidiTranslator::ResolveDrumKit(uint8_t fontId, int16_t instOrWave, std::string& outPack, int& outProgram) const {
    for (const auto& e : mEntries) {
        if (InEditMod(e, fontId, instOrWave) && e.bank == 128 && !e.packName.empty()) {
            outPack = e.packName;
            outProgram = e.program;
            return;
        }
    }
    for (const auto& lp : mLoadedPresets) {
        if (lp.bank == 128 && lp.modName == mEditMod) {
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
    if (!BypassIndexValid(fontId, instOrWave) || mEditMod.empty())
        return;
    if (instOrWave < kDrumHistInst)
        return; // 0/1 are intrinsic drum channels -- use SetDrumChannelSynth

    const ModPair key{ mEditMod, fontId, instOrWave };
    if (forced == mModForcedDrums.contains(key))
        return;

    if (forced) {
        mModForcedDrums.insert(key);
        // A full-range melodic entry would otherwise cover every slot and
        // shadow the per-slot drum entries once discovered.
        for (auto& e : mEntries) {
            if (InEditMod(e, fontId, instOrWave) && e.noteLow == 0 && e.noteHigh == 127) {
                e.enabled = false;
                e.selected = false;
            }
        }
    } else {
        mModForcedDrums.erase(key);
        // Drop the per-slot drum entries so the pair returns to a clean melodic row.
        for (auto& e : mEntries) {
            if (InEditMod(e, fontId, instOrWave) && e.noteLow == e.noteHigh) {
                e.enabled = false;
                e.selected = false;
            }
        }
    }
    RecomputeActive(fontId, instOrWave);
}

void MidiTranslator::ApplyForcedDrumFlag(uint8_t fontId, int16_t instOrWave, bool forced) {
    const bool wasForced = mForcedDrumPool[fontId][instOrWave] >= 0;
    if (forced == wasForced)
        return;
    if (forced) {
        int pool = AllocForcedDrumPool();
        if (pool < 0) {
            SPDLOG_WARN("[MidiTranslator] forced drum pool full ({} pairs), "
                        "cannot flag font {} inst {}",
                        kMaxForcedDrumPairs, fontId, instOrWave);
            return;
        }
        mForcedDrumPool[fontId][instOrWave] = static_cast<int8_t>(pool);
    } else {
        int8_t pool = mForcedDrumPool[fontId][instOrWave];
        if (pool >= 0 && pool < kMaxForcedDrumPairs)
            mForcedDrumPoolUsed[pool] = false;
        mForcedDrumPool[fontId][instOrWave] = -1;
    }
}

bool MidiTranslator::IsForcedDrum(uint8_t fontId, int16_t instOrWave) const {
    if (fontId >= kMaxFontId || instOrWave < 0 || instOrWave >= kMaxInstOrWave)
        return false;
    return instOrWave >= kDrumHistInst && mForcedDrumPool[fontId][instOrWave] >= 0;
}

bool MidiTranslator::IsForcedDrumInView(uint8_t fontId, int16_t instOrWave) const {
    if (!mEditMod.empty())
        return mModForcedDrums.contains({ mEditMod, fontId, instOrWave });
    return IsForcedDrum(fontId, instOrWave);
}

void MidiTranslator::AutoSplitDrums(uint8_t fontId, int16_t instOrWave) {
    if (!BypassIndexValid(fontId, instOrWave) || mEditMod.empty())
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
        if (InEditMod(e, fontId, instOrWave) && e.noteLow == e.noteHigh && e.noteLow < 128)
            bySlot[e.noteLow] = static_cast<int>(idx);
    }

    // Sequences fire hits on drum indices the font leaves empty (e.g. slot 0 in
    // the vanilla fonts); the engine drops those notes (Audio_GetDrum -> NULL ->
    // stopSomething), so voicing them on the synth adds beats the game never
    // plays. Skip them when the registry knows this font's real slots.
    std::set<uint8_t> sampledSlots;
    if (instOrWave == 0)
        for (const auto& d : EnumerateDrumSlots(fontId))
            sampledSlots.insert(d.slot);

    // One single-slot entry per fired slot. The default GM percussion note is
    // a stable per-slot spread (35 + slot, clamped) so each slot is audibly
    // distinct; the user remaps via the Drum Sound combo. An existing slot
    // keeps its prior Drum Sound.
    for (int s = 0; s < kDrumHistSlots; ++s) {
        if (hist[s].count.load(std::memory_order_relaxed) == 0)
            continue;
        if (!sampledSlots.empty() && !sampledSlots.count(static_cast<uint8_t>(s)))
            continue;
        int idx = bySlot[s];
        const bool isNew = (idx < 0);
        if (isNew) {
            idx = FindOrCreateEntry(mEditMod, fontId, instOrWave, kitPack, static_cast<int16_t>(kitProgram), kitBank,
                                    std::string("Drum Kit"), static_cast<uint8_t>(s));
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
        // Discovery only POPULATES the row. A freshly discovered slot defaults
        // to Native (disabled) so switching the instrument to Synth doesn't
        // suddenly blast a guessed GM kit; the user enables each slot by
        // picking a sound. Existing slots keep their saved enabled state.
        if (isNew)
            e.enabled = false;
        // No kit loaded -> unresolved (native) rather than mis-synthing on the
        // empty-pack placeholder path (which would resolve to sfontId 0).
        e.sfontId = kitPack.empty() ? -1 : ResolveSfontIdFromCache(mEditMod, kitPack, kitBank, kitProgram);
    }
    RecomputeActive(fontId, instOrWave);
}

void MidiTranslator::AddDrumSlot(uint8_t fontId, int16_t instOrWave, uint8_t slot) {
    if (!BypassIndexValid(fontId, instOrWave) || mEditMod.empty())
        return;
    if (slot >= kDrumHistSlots)
        return;
    // Drum-like pairs only: intrinsic drum/SFX channels or a forced-drum pair.
    if (instOrWave != 0 && instOrWave != 1 && !IsForcedDrumInView(fontId, instOrWave))
        return;

    std::string kitPack;
    int kitProgram = 0;
    const int16_t kitBank = 128;
    ResolveDrumKit(fontId, instOrWave, kitPack, kitProgram);

    int idx = -1;
    for (size_t k = 0; k < mEntries.size() && idx < 0; k++) {
        if (InEditMod(mEntries[k], fontId, instOrWave) && mEntries[k].noteLow == slot)
            idx = static_cast<int>(k);
    }
    if (idx < 0) {
        idx = FindOrCreateEntry(mEditMod, fontId, instOrWave, kitPack, static_cast<int16_t>(kitProgram), kitBank,
                                std::string("Drum Kit"), slot);
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
    // Native by default (like discovery) so adding a slot never blasts a guessed
    // GM kit; the user enables it by picking a Drum Sound.
    e.enabled = false;
    e.sfontId = kitPack.empty() ? -1 : ResolveSfontIdFromCache(mEditMod, kitPack, kitBank, kitProgram);
    RecomputeActive(fontId, instOrWave);
}

void MidiTranslator::SetDrumChannelSynth(uint8_t fontId, int16_t instOrWave, bool synth) {
    if (!BypassIndexValid(fontId, instOrWave) || mEditMod.empty())
        return;
    if (instOrWave >= kDrumHistInst)
        return; // master flag only meaningful for the drum/SFX channels

    // Master switch only: per-slot `enabled` is each slot's own Native/Synth
    // state, preserved across instrument-mode toggles.
    const ModPair key{ mEditMod, fontId, instOrWave };
    if (synth)
        mModDrumChannelSynth.insert(key);
    else
        mModDrumChannelSynth.erase(key);

    if (synth) {
        // Going Synth needs rows to edit. If none exist yet, discover them
        // from the histogram (created Native by default -- see AutoSplitDrums).
        bool hasSlot = false;
        for (const auto& e : mEntries) {
            if (InEditMod(e, fontId, instOrWave) && e.noteLow == e.noteHigh) {
                hasSlot = true;
                break;
            }
        }
        if (!hasSlot)
            AutoSplitDrums(fontId, instOrWave);
    }
    RecomputeActive(fontId, instOrWave);
}

bool MidiTranslator::IsDrumChannelSynth(uint8_t fontId, int16_t instOrWave) const {
    if (fontId >= kMaxFontId || instOrWave < 0 || instOrWave >= kDrumHistInst)
        return false;
    if (!mEditMod.empty())
        return mModDrumChannelSynth.contains({ mEditMod, fontId, instOrWave });
    return mDrumChannelSynth[fontId][instOrWave];
}

void MidiTranslator::SetDrumKit(uint8_t fontId, int16_t instOrWave, const std::string& pack, int16_t program,
                                const std::string& presetName) {
    if (!BypassIndexValid(fontId, instOrWave))
        return;
    for (auto& e : mEntries) {
        if (!InEditMod(e, fontId, instOrWave))
            continue;
        e.packName = pack;
        e.program = program;
        e.bank = 128;
        if (!presetName.empty())
            e.presetName = presetName;
        e.sfontId = ResolveSfontIdFromCache(mEditMod, pack, 128, program);
    }
    RecomputeActive(fontId, instOrWave);
}

// ── Note-range split editing (melodic) ──────────────────────────────────────

int MidiTranslator::SplitEntry(int idx, uint8_t atSemitone) {
    if (!IsEditable(idx))
        return -1;
    // The cut must fall strictly inside the range so both halves are non-empty.
    if (atSemitone <= mEntries[idx].noteLow || atSemitone > mEntries[idx].noteHigh)
        return -1;
    if (mEntries.size() >= kMaxEntries)
        return -1;
    ConfigEntry sib = mEntries[idx];                               // copy preset/gain/effects/route/etc.
    sib.noteLow = atSemitone;                                      // sibling takes the upper half
    mEntries[idx].noteHigh = static_cast<uint8_t>(atSemitone - 1); // idx keeps lower half
    mEntries.push_back(std::move(sib));                            // mEntries reserved -> no realloc, idx stays valid
    int newIdx = static_cast<int>(mEntries.size()) - 1;
    NormalizeSplits(mEntries[idx].fontId, mEntries[idx].instOrWave);
    RecomputeActive(mEntries[idx].fontId, mEntries[idx].instOrWave);
    return newIdx;
}

int MidiTranslator::SplitEntryEven(int idx, int parts) {
    if (!IsEditable(idx))
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
    if (!IsEditable(idx))
        return;
    ConfigEntry& e = mEntries[idx];
    if (e.noteHigh >= 127)
        return; // nothing above to merge
    const int wantLow = static_cast<int>(e.noteHigh) + 1;
    int sib = -1;
    for (size_t k = 0; k < mEntries.size(); k++) {
        const ConfigEntry& c = mEntries[k];
        if (InEditMod(c, e.fontId, e.instOrWave) && static_cast<int>(c.noteLow) == wantLow) {
            sib = static_cast<int>(k);
            break;
        }
    }
    if (sib < 0)
        return;
    // Extend idx over the sibling's range, then retire the sibling rather than
    // erase it: erasing would shift every cached index the audio thread reads.
    e.noteHigh = mEntries[sib].noteHigh;
    mEntries[sib].enabled = false;
    mEntries[sib].selected = false;
    NormalizeSplits(e.fontId, e.instOrWave);
    RecomputeActive(e.fontId, e.instOrWave);
}

void MidiTranslator::SetSplitBoundary(int lowerIdx, int upperIdx, uint8_t boundary) {
    if (!IsEditable(lowerIdx) || !IsEditable(upperIdx))
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
    if (!IsEditable(idx))
        return;
    if (noteHigh < noteLow)
        noteHigh = noteLow;
    mEntries[idx].noteLow = noteLow;
    mEntries[idx].noteHigh = noteHigh;
    NormalizeSplits(mEntries[idx].fontId, mEntries[idx].instOrWave);
    RecomputeActive(mEntries[idx].fontId, mEntries[idx].instOrWave);
}

void MidiTranslator::NormalizeSplits(uint8_t fontId, int16_t instOrWave) {
    // Drum/SFX and forced-drum pairs are per-slot, not ranges -- normalizing
    // would snap slot entries into a bogus [0,127] tiling (and retire them all
    // when a full-range melodic entry is present).
    if (instOrWave < kDrumHistInst || IsForcedDrumInView(fontId, instOrWave))
        return;

    std::vector<int> rows;
    for (size_t i = 0; i < mEntries.size(); i++) {
        if (InEditMod(mEntries[i], fontId, instOrWave))
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
    // Keep the edited mod's winning row at full range, retire its other ranges.
    int keep = GetViewEntryIdx(fontId, instOrWave);
    for (size_t i = 0; i < mEntries.size(); i++) {
        ConfigEntry& e = mEntries[i];
        if (!InEditMod(e, fontId, instOrWave))
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
    if (!IsEditable(idx))
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
    e.sfontId = ResolveSfontIdFromCache(e.modName, pack, bank, program);
    RecomputeActive(e.fontId, e.instOrWave);
}

void MidiTranslator::AutoSplitByEngineRanges(uint8_t fontId, int16_t instOrWave) {
    if (!BypassIndexValid(fontId, instOrWave) || mEditMod.empty())
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
    int actIdx = GetViewEntryIdx(fontId, instOrWave);
    if (!IsEditable(actIdx))
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
    if (!IsEditable(idx))
        return;
    mEntries[idx].gain = gain;
}
void MidiTranslator::SetEntryTranspose(int idx, int8_t semis) {
    if (!IsEditable(idx))
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
    if (!IsEditable(idx))
        return;
    mEntries[idx].reverb = ClampCcOrSentinel(cc);
}
void MidiTranslator::SetEntryChorus(int idx, int8_t cc) {
    if (!IsEditable(idx))
        return;
    mEntries[idx].chorus = ClampCcOrSentinel(cc);
}
void MidiTranslator::SetEntryFilterCutoff(int idx, int8_t cc) {
    if (!IsEditable(idx))
        return;
    mEntries[idx].cutoff = ClampCcOrSentinel(cc);
}
void MidiTranslator::SetEntryFilterResonance(int idx, int8_t cc) {
    if (!IsEditable(idx))
        return;
    mEntries[idx].q = ClampCcOrSentinel(cc);
}
void MidiTranslator::SetEntryFixedNote(int idx, int16_t note) {
    if (!IsEditable(idx))
        return;
    // -1 = derive from semitone; otherwise a valid MIDI note. No chain
    // recompute: fixedNote changes WHAT a winner plays, not WHICH wins.
    mEntries[idx].fixedNote = (note < 0) ? -1 : static_cast<int16_t>(std::clamp<int>(note, 0, 127));
}
void MidiTranslator::SetEntryRoute(int idx, EntryRoute route) {
    if (!IsEditable(idx))
        return;
    mEntries[idx].route = route;
    // Route doesn't change chain membership today, but recompute keeps the
    // cache authoritative if that ever changes; it's a cheap UI-thread call.
    RecomputeActive(mEntries[idx].fontId, mEntries[idx].instOrWave);
}
void MidiTranslator::SetEntryEnabled(int idx, bool enabled) {
    if (!IsEditable(idx))
        return;
    ConfigEntry& e = mEntries[idx];
    e.enabled = enabled;
    RecomputeActive(e.fontId, e.instOrWave);
}
// ── Per-pair display name (row label) ─────────────────────────────────────

std::string MidiTranslator::GetDisplayName(uint8_t fontId, int16_t instOrWave) const {
    if (!mEditMod.empty()) {
        auto it = mDisplayName.find({ mEditMod, fontId, instOrWave });
        return it == mDisplayName.end() ? std::string{} : it->second;
    }
    // The playing mod's label, else the highest-ranked mod that names the pair.
    if (const std::string* owner = OwnerMod(fontId, instOrWave)) {
        auto it = mDisplayName.find({ *owner, fontId, instOrWave });
        if (it != mDisplayName.end())
            return it->second;
    }
    const std::string* best = nullptr;
    int bestRank = -2;
    for (const auto& [key, name] : mDisplayName) {
        if (std::get<1>(key) == fontId && std::get<2>(key) == instOrWave && ModRank(std::get<0>(key)) > bestRank) {
            best = &name;
            bestRank = ModRank(std::get<0>(key));
        }
    }
    return best == nullptr ? std::string{} : *best;
}
void MidiTranslator::SetDisplayName(uint8_t fontId, int16_t instOrWave, const std::string& name) {
    if (!BypassIndexValid(fontId, instOrWave) || mEditMod.empty())
        return;
    if (name.empty())
        mDisplayName.erase({ mEditMod, fontId, instOrWave });
    else
        mDisplayName[{ mEditMod, fontId, instOrWave }] = name;
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

// ── MIDI channel pool ─────────────────────────────────────────────────────

uint8_t MidiTranslator::GetChannelsInUse() const {
    return mChannelsAllocated;
}

uint32_t MidiTranslator::GetChannelReclaims() const {
    return mChannelReclaims;
}

uint8_t MidiTranslator::AllocateChannel(uint8_t seqPlayer, uint8_t seqChannel, int16_t entryIdx) {
    for (uint8_t ch = 0; ch < mChannelsAllocated; ++ch) {
        const ChannelOwner& o = mChannelOwner[ch];
        if (o.seqPlayer == seqPlayer && o.seqChannel == seqChannel && o.entryIdx == entryIdx)
            return ch;
    }

    if (mChannelsAllocated < kMaxMidiChannels) {
        const uint8_t ch = mChannelsAllocated++;
        mChannelOwner[ch] = { seqPlayer, seqChannel, entryIdx };
        return ch;
    }

    // The physical channel keeps its program and CCs, and so does mChannelState,
    // so the new owner re-issues only what differs.
    for (uint8_t ch = 0; ch < kMaxMidiChannels; ++ch) {
        if (mChannelNotes[ch] == 0) {
            mChannelOwner[ch] = { seqPlayer, seqChannel, entryIdx };
            ++mChannelReclaims;
            return ch;
        }
    }

    static bool sLoggedOverflow = false;
    if (!sLoggedOverflow) {
        SPDLOG_WARN("[MidiTranslator] All {} channels sounding at once; routing overflow to native",
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
    mModDrumChannelSynth.clear();
    mModForcedDrums.clear();
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
// default so unsplit entries serialise compactly. (noteLow is part of the entry
// key, so it is written whenever nonzero.)
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

std::string MidiTranslator::BuildPackMappingJson(const std::string& modName, const std::string& packName,
                                                 bool ownsUnmapped, int& outWritten) const {
    nlohmann::json j;
    j["version"] = 2;
    j["entries"] = nlohmann::json::array();

    // Which of the mod's soundfonts carries each pair's flags and label.
    std::map<std::pair<uint8_t, int16_t>, std::string> pairPack;
    for (const auto& e : mEntries)
        if (e.selected && e.modName == modName)
            pairPack.try_emplace({ e.fontId, e.instOrWave }, e.packName);
    auto ownsPair = [&](uint8_t fontId, int16_t instOrWave) {
        auto it = pairPack.find({ fontId, instOrWave });
        return it == pairPack.end() ? ownsUnmapped : it->second == packName;
    };
    std::set<int> winners;
    for (const auto& [pair, pack] : pairPack)
        for (int idx : ModWinners(modName, pair.first, pair.second))
            winners.insert(idx);
    auto withFont = [](nlohmann::json& o, uint8_t fontId, int16_t instOrWave) {
        o["fontId"] = fontId;
        if (const char* fp = FontPathForId(fontId))
            o["font"] = fp;
        o["instOrWave"] = instOrWave;
    };

    int written = 0;
    for (size_t idx = 0; idx < mEntries.size(); idx++) {
        const ConfigEntry& e = mEntries[idx];
        if (!e.selected || e.modName != modName || e.packName != packName || e.program < 0)
            continue;
        // A playable row the mod's own rows shadow never sounds; writing it is how
        // duplicates used to pile up.
        if (IsResolvable(e) && !winners.contains(static_cast<int>(idx)))
            continue;

        nlohmann::json entry;
        withFont(entry, e.fontId, e.instOrWave);
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
        if (!e.enabled)
            entry["enabled"] = false;
        j["entries"].push_back(std::move(entry));
        ++written;
    }

    for (const auto& [key, name] : mDisplayName) {
        const auto& [mod, fontId, instOrWave] = key;
        if (mod != modName || !ownsPair(fontId, instOrWave))
            continue;
        nlohmann::json entry;
        withFont(entry, fontId, instOrWave);
        entry["display_name"] = name;
        j["entries"].push_back(std::move(entry));
    }
    auto writeFlags = [&](const char* field, const std::set<ModPair>& flags) {
        nlohmann::json list = nlohmann::json::array();
        for (const auto& [mod, fontId, instOrWave] : flags) {
            if (mod != modName || !ownsPair(fontId, instOrWave))
                continue;
            nlohmann::json d;
            withFont(d, fontId, instOrWave);
            list.push_back(std::move(d));
        }
        if (!list.empty())
            j[field] = std::move(list);
    };
    writeFlags("drum_channels_synth", mModDrumChannelSynth);
    writeFlags("forced_drums", mModForcedDrums);

    outWritten = written;
    return j.dump(2);
}

bool MidiTranslator::ApplyModMapping(const std::string& json, const std::string& modName, const std::string& packName) {
    nlohmann::json j;
    try {
        j = nlohmann::json::parse(json);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("[MidiTranslator] ApplyModMapping: {}/{}: parse error: {}", modName, packName, e.what());
        return false;
    }
    // Fonts resolve by name; a modded font referenced only by numeric id is
    // skipped (ids shift with the mod set), and so is a name that isn't
    // loaded -- the instrument plays native.
    auto resolveFont = [](const nlohmann::json& o) -> int {
        int fontId = o.value("fontId", -1);
        if (fontMap == nullptr)
            return fontId;
        if (o.contains("font"))
            return ResolveFontIdByPath(o.value("font", std::string{}));
        return fontId < kBuiltinFontCount ? fontId : -1;
    };
    int applied = 0;
    int replaced = 0;
    for (const auto& entry : j.value("entries", nlohmann::json::array())) {
        int fontId = resolveFont(entry);
        int instOrWave = entry.value("instOrWave", -1);
        if (fontId < 0 || fontId >= kMaxFontId || instOrWave < 0 || instOrWave >= kMaxInstOrWave)
            continue;
        const ModPair key{ modName, static_cast<uint8_t>(fontId), static_cast<int16_t>(instOrWave) };

        if (entry.contains("display_name")) {
            std::string dn = entry.value("display_name", std::string{});
            if (!dn.empty())
                mDisplayName[key] = dn;
        }
        if (!entry.contains("program"))
            continue;
        int program = entry.value("program", -1);
        int bank = std::clamp(entry.value("bank", 0), 0, 255);
        if (program < -1 || program > 127)
            continue;
        std::string presetName = entry.value("preset_name", std::string{});
        // note_low is part of the entry key, so it must be resolved before
        // find-or-create or distinct splits collapse into one entry.
        uint8_t noteLow = static_cast<uint8_t>(std::clamp(entry.value("note_low", 0), 0, 127));

        int idx = FindOrCreateEntry(modName, static_cast<uint8_t>(fontId), static_cast<int16_t>(instOrWave), packName,
                                    static_cast<int16_t>(program), static_cast<int16_t>(bank), presetName, noteLow);
        if (idx < 0)
            continue;
        ConfigEntry& e = mEntries[idx];
        if (entry.contains("gain"))
            e.gain = entry.value("gain", 0.0f);
        if (entry.contains("transpose"))
            e.transpose = static_cast<int8_t>(std::clamp(entry.value("transpose", 0), -127, 127));
        if (entry.contains("reverb"))
            e.reverb = ClampCcOrSentinel(entry.value("reverb", -1));
        if (entry.contains("chorus"))
            e.chorus = ClampCcOrSentinel(entry.value("chorus", -1));
        if (entry.contains("filter_cutoff"))
            e.cutoff = ClampCcOrSentinel(entry.value("filter_cutoff", -1));
        if (entry.contains("filter_q"))
            e.q = ClampCcOrSentinel(entry.value("filter_q", -1));
        ReadSplitFields(entry, e);
        e.enabled = entry.value("enabled", true);

        // A mod keeps one row per range: older files listed a re-picked preset
        // beside the one it replaced, and the later row is the newer pick.
        for (size_t other = 0; other < mEntries.size(); other++) {
            ConfigEntry& o = mEntries[other];
            if (static_cast<int>(other) != idx && o.selected && o.modName == modName && o.fontId == e.fontId &&
                o.instOrWave == e.instOrWave && o.noteLow == e.noteLow && o.noteHigh == e.noteHigh) {
                o.selected = false;
                o.enabled = false;
                replaced++;
            }
        }
        applied++;
    }
    auto readFlags = [&](const char* field, std::set<ModPair>& flags, bool drumChannel) {
        for (const auto& d : j.value(field, nlohmann::json::array())) {
            int f = resolveFont(d);
            int i = d.value("instOrWave", -1);
            const bool valid = drumChannel ? (i >= 0 && i < kDrumHistInst) : (i >= kDrumHistInst && i < kMaxInstOrWave);
            if (f >= 0 && f < kMaxFontId && valid)
                flags.insert({ modName, static_cast<uint8_t>(f), static_cast<int16_t>(i) });
        }
    };
    readFlags("drum_channels_synth", mModDrumChannelSynth, true);
    readFlags("forced_drums", mModForcedDrums, false);
    if (replaced > 0)
        SPDLOG_WARN("[MidiTranslator] {}/{}: {} rows repeated a later row's range and were dropped; saving the mod "
                    "removes them",
                    modName, packName, replaced);
    SPDLOG_INFO("[MidiTranslator] ApplyModMapping: {}/{}: applied {} entries", modName, packName, applied);
    return true;
}

// ── ProcessNote ───────────────────────────────────────────────────────────

bool MidiTranslator::ProcessNote(const EngineNote& engineNote) {
    const int noteIndex = engineNote.noteIndex;
    const float freqScale = engineNote.freqScale;
    const float strike = engineNote.strike;
    const uint8_t pan = engineNote.pan;
    const uint8_t reverb = engineNote.reverb;
    const uint8_t fontId = engineNote.fontId;
    const int16_t instOrWave = engineNote.instOrWave;
    const uint8_t semitone = engineNote.semitone;
    const bool isFinished = engineNote.isFinished;
    const float pitchBend = engineNote.pitchBend;

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
        BypassIndexValid(fontId, instOrWave) && state.kind == SlotKind::Idle && strike > 0.0f && !isFinished;
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
            if (DrumSlotHit* hist = DrumHistFor(fontId, instOrWave)) {
                hist[semitone].count.fetch_add(1, std::memory_order_relaxed);
                hist[semitone].lastHitMs.store(NowMs(), std::memory_order_relaxed);
            }
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
            if (!state.released)
                ReleaseKeyHold(synth.get(), state.channel, state.midiNote);
            if (mChannelNotes[state.channel] > 0)
                mChannelNotes[state.channel]--;
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
        state.released = false;
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
    if (IsAuditionNative(fontId, instOrWave)) {
        if (isFinished || strike <= 0.0f)
            retireSlot();
        else
            adoptNative(-1);
        bumpRoute(&DebugSlot::routedNative);
        return false;
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
        if (isFinished || strike <= 0.0f)
            retireSlot();
        else
            adoptNative(nativeAttrib);
        bumpRoute(&DebugSlot::routedNative);
        return false;
    }

    // ── Resolution: walk the active-split chain ─────────────────────────
    if (!BypassIndexValid(fontId, instOrWave)) {
        if (isFinished || strike <= 0.0f)
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
        if (isFinished || strike <= 0.0f)
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
        if (isFinished || strike <= 0.0f)
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

    uint8_t targetChannel =
        AllocateChannel(engineNote.seqPlayer, engineNote.seqChannel, static_cast<int16_t>(activeIdx));
    if (targetChannel == 0xFF) {
        // Channel pool momentarily exhausted (all 64 sounding) -> native, so
        // the instrument still plays instead of corrupting another channel.
        if (isFinished || strike <= 0.0f)
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

    if (isFinished || strike <= 0.0f) {
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

    float entryGain = e.gain;
    if (entryGain == 0.0f)
        entryGain = 1.0f;
    // Square roots: the engine's amplitudes are squares, the synth's curves expect the control value.
    const float channelLevel = sqrtf(std::max(0.0f, engineNote.channelVolume)) * mGlobalGain * entryGain;
    const uint8_t cc7val = static_cast<uint8_t>(std::clamp(channelLevel * 127.0f, 0.0f, 127.0f));
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
    // The engine's top bit swaps the reverb send's channels; the low 7 bits are the amount.
    uint8_t reverbVal = resolveEffectCc(e.reverb, reverb & 0x7F);
    uint8_t chorusVal = resolveEffectCc(e.chorus, 0);
    uint8_t cutoffVal = resolveEffectCc(e.cutoff, 64);
    uint8_t qVal = resolveEffectCc(e.q, 64);
    const uint8_t reverbSend = reverbVal | (reverb & 0x80);
    if (reverbSend != chState.lastReverbCC) {
        synth->ControlChange(targetChannel, 91, static_cast<uint16_t>(reverbVal) << 7);
        synth->SetReverbSendSwapped(targetChannel, (reverb & 0x80) != 0);
        chState.lastReverbCC = reverbSend;
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

    const uint8_t strikeVal = static_cast<uint8_t>(std::clamp(sqrtf(std::max(0.0f, strike)) * 127.0f, 0.0f, 127.0f));
    const uint8_t noteOnVel = mSynthMode == SynthMode::Enhanced ? strikeVal : kFixedNoteOnVelocity;

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
        if (mSynthMode == SynthMode::Authentic)
            synth->KeyPressure(targetChannel, midiNote, strikeVal);
        synth->NoteOnPitchFactor(targetChannel, midiNote, noteOnVel, effectiveBend);
        if (targetChannel < kMaxMidiChannels && midiNote < 128 && mKeyHold[targetChannel][midiNote] < 255)
            mKeyHold[targetChannel][midiNote]++;
        state.kind = SlotKind::Synth;
        state.channel = targetChannel;
        state.midiNote = midiNote;
        state.pairFontId = fontId;
        state.pairInstOrWave = instOrWave;
        state.activeEntryIdx = static_cast<int16_t>(activeIdx);
        state.lastFreqScale = freqScale;
        if (mChannelNotes[targetChannel] < 255)
            mChannelNotes[targetChannel]++;
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

void MidiTranslator::ReleaseKeyHold(IMidiSynth* synth, uint8_t channel, uint8_t midiNote) {
    if (channel >= kMaxMidiChannels || midiNote >= 128)
        return;
    uint8_t& h = mKeyHold[channel][midiNote];
    if (h > 0)
        --h;
    if (h == 0)
        synth->NoteOff(channel, midiNote);
}

bool MidiTranslator::NoteReleased(int noteIndex) {
    if (noteIndex < 0 || noteIndex >= kMaxNotes)
        return false;
    NoteTranslatorState& state = mNoteState[noteIndex];
    if (state.kind != SlotKind::Synth)
        return false;
    if (!state.released) {
        auto synth = SOH::MidiSynthManager::Instance().GetActiveSynth();
        if (!synth)
            return false;
        ReleaseKeyHold(synth.get(), state.channel, state.midiNote);
        state.released = true;
    }
    return true;
}

void MidiTranslator::NoteDisabled(int noteIndex) {
    if (noteIndex < 0 || noteIndex >= kMaxNotes)
        return;
    NoteTranslatorState& state = mNoteState[noteIndex];
    if (state.kind == SlotKind::Synth) {
        auto synth = SOH::MidiSynthManager::Instance().GetActiveSynth();
        if (synth && !state.released)
            ReleaseKeyHold(synth.get(), state.channel, state.midiNote);
        if (mChannelNotes[state.channel] > 0)
            mChannelNotes[state.channel]--;
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
    state.released = false;
}

} // namespace SOH

// ─── C-linkage entry points called from audio_playback.c ───────────────────

extern "C" {

void SOH_MidiTranslator_NoteDisabled(int noteIndex) {
    SOH::MidiTranslator::Instance().NoteDisabled(noteIndex);
}

void SOH_MidiTranslator_Reset() {
    SOH::MidiTranslator::Instance().Reset();
}

} // extern "C"
