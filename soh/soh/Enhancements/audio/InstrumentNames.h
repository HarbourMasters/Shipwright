#pragma once
#include <cstdint>
#include <string>

namespace SOH {

// Per-(fontId, instId) sample-name registry, populated by the SoundFont resource
// factory as it loads each font. Lets the bypass UI show a real sample name
// rather than a generic per-font label. Set runs during resource loading (any
// thread), Get on the GUI thread, so both take the mutex.

// Range identifier for the three sample variants each instrument can carry. The
// engine picks one per note pitch (low for very low notes, high for very high,
// normal for the middle range); many instruments only populate one or two.
enum class SampleRange : uint8_t { Low = 0, Normal = 1, High = 2 };

// The audio engine exposes a melodic instrument as instOrWave = instrument-array
// index + 2; instOrWave 0 and 1 are the drum and SFX channels (see
// AudioSeq_SetInstrument and MidiTranslator::kDrumHistInst). Every consumer keys
// by instOrWave, so the factory must register under (array index + this base).
inline constexpr int16_t kMelodicInstOrWaveBase = 2;

// Record one sample filename for an (instrument slot, range). Pass the raw
// string the factory loaded (with directory prefix) — the helper strips it
// on display. Empty `name` clears any prior entry for that range.
void SetInstrumentSampleName(uint8_t fontId, int16_t instId, SampleRange range, std::string name);

// Record one sample's tuning (frequency scale factor) for an (instrument slot,
// range). The engine resamples by this ratio, so a value near a power of two
// encodes a whole-octave displacement of the sample vs its nominal note; the
// bypass UI derives a suggested octave Shift from it (see SuggestedTranspose).
void SetInstrumentTuning(uint8_t fontId, int16_t instId, SampleRange range, float tuning);

// Record the engine's low/normal/high split boundaries for an instrument slot
// (in engine-semitone space): semitone < lo -> low sample, lo..hi -> normal,
// semitone > hi -> high. Lets the bypass UI mirror the engine's per-pitch
// sample routing.
void SetInstrumentRange(uint8_t fontId, int16_t instId, uint8_t normalRangeLo, uint8_t normalRangeHi);

// Snapshot of all three range names for an instrument slot; empty strings mean no
// sample is registered for that range. Lets the bypass UI disambiguate slots that
// play a different sample per pitch.
struct InstrumentSampleSet {
    std::string low;
    std::string normal;
    std::string high;
    // Engine split boundaries (see SetInstrumentRange). hasRange is false when the
    // factory never registered this slot; callers then fall back to a single
    // full-range entry.
    uint8_t rangeLo = 0;
    uint8_t rangeHi = 127;
    bool hasRange = false;
    // Per-range sample tuning (0 = no sample / not registered). See
    // SetInstrumentTuning and SuggestedTranspose.
    float lowTuning = 0.0f;
    float normalTuning = 0.0f;
    float highTuning = 0.0f;
    bool empty() const {
        return low.empty() && normal.empty() && high.empty();
    }
};
InstrumentSampleSet GetInstrumentSampleNames(uint8_t fontId, int16_t instId);

// Octave-rounded semitone shift implied by a sample tuning: round(log2(tuning))
// * 12. Only the whole-octave part is taken — the fine cents in `tuning` are the
// sample's own pitch correction, irrelevant to an in-tune GM substitute. Returns
// 0 for tuning <= 0 (no sample). Clamped to +/-48 st.
int SuggestedOctaveShift(float tuning);

// Suggested per-entry transpose for the [noteLow, noteHigh] range of an
// instrument slot: routes the range midpoint through the engine's low/normal/
// high split to pick the governing sample's tuning, then octave-rounds it.
// Returns 0 when no tuning is registered for the slot.
int SuggestedTranspose(uint8_t fontId, int16_t instId, uint8_t noteLow, uint8_t noteHigh);

// Convenience: extract the basename from a stored sample path. Returns
// the input unchanged if it has no '/' separator. The returned pointer
// references inside `path` — do not outlive it.
const char* StripSamplePathPrefix(const std::string& path);

// One engine instrument slot the SoundFont factory registered a sample for.
struct RegisteredInstrument {
    uint8_t fontId;
    int16_t instOrWave; // engine key: 0 = drum, 1 = SFX, >= 2 = melodic
    std::string name;   // best-available sample name (path stripped); may be empty
};

// Every (fontId, instOrWave) the factory registered, in ascending key order.
// This is the full engine-instrument set known at font-load time, so UIs can list
// instruments up front instead of waiting for a song to reveal them in play.
std::vector<RegisteredInstrument> EnumerateRegisteredInstruments();

// A drum lives on instrument slot 0 as an array of per-slot samples (the note
// byte is the slot index). Record one drum slot's sample filename so the UI can
// list every slot up front with a real name, instead of waiting for it to play.
// Pass the raw path the factory loaded; the helper strips it on display.
void SetDrumSlotName(uint8_t fontId, uint8_t slot, std::string name);

// One drum slot the factory registered a sample for.
struct RegisteredDrumSlot {
    uint8_t fontId;
    uint8_t slot;     // note-byte index into the font's drum array
    std::string name; // sample name (path stripped)
};

// Every registered drum slot for one font, in ascending slot order. Lets the UI
// show all of a font's drums up front rather than discovering them from playback.
std::vector<RegisteredDrumSlot> EnumerateDrumSlots(uint8_t fontId);

// Every font that registered at least one drum slot, ascending. Lets the UI list
// each font's drum channel (instOrWave 0) up front, like melodic instruments.
std::vector<uint8_t> EnumerateDrumFonts();

} // namespace SOH
