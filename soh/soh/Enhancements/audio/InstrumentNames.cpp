#include "InstrumentNames.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <mutex>
#include <utility>
#include <vector>

namespace SOH {

namespace {
// One slot may carry up to three sample variants (low / normal / high).
// Storing them together keeps lookups single-keyed.
struct SlotSamples {
    std::string low;
    std::string normal;
    std::string high;
    uint8_t rangeLo = 0;
    uint8_t rangeHi = 127;
    bool hasRange = false;
    float lowTuning = 0.0f;
    float normalTuning = 0.0f;
    float highTuning = 0.0f;
};

struct State {
    std::mutex mutex;
    // Keyed by (fontId, instId). Small enough that std::map's cost is irrelevant.
    std::map<std::pair<uint8_t, int16_t>, SlotSamples> entries;
    // Drum slot sample names, keyed by (fontId, slot).
    std::map<std::pair<uint8_t, uint8_t>, std::string> drumSlots;
};

State& GetState() {
    static State s;
    return s;
}
} // namespace

void SetInstrumentSampleName(uint8_t fontId, int16_t instId, SampleRange range, std::string name) {
    if (instId < 0) {
        return;
    }
    auto& s = GetState();
    std::lock_guard<std::mutex> lock(s.mutex);
    auto& slot = s.entries[std::make_pair(fontId, instId)];
    switch (range) {
        case SampleRange::Low:
            slot.low = std::move(name);
            break;
        case SampleRange::Normal:
            slot.normal = std::move(name);
            break;
        case SampleRange::High:
            slot.high = std::move(name);
            break;
    }
}

void SetInstrumentTuning(uint8_t fontId, int16_t instId, SampleRange range, float tuning) {
    if (instId < 0) {
        return;
    }
    auto& s = GetState();
    std::lock_guard<std::mutex> lock(s.mutex);
    auto& slot = s.entries[std::make_pair(fontId, instId)];
    switch (range) {
        case SampleRange::Low:
            slot.lowTuning = tuning;
            break;
        case SampleRange::Normal:
            slot.normalTuning = tuning;
            break;
        case SampleRange::High:
            slot.highTuning = tuning;
            break;
    }
}

void SetInstrumentRange(uint8_t fontId, int16_t instId, uint8_t normalRangeLo, uint8_t normalRangeHi) {
    if (instId < 0) {
        return;
    }
    auto& s = GetState();
    std::lock_guard<std::mutex> lock(s.mutex);
    auto& slot = s.entries[std::make_pair(fontId, instId)];
    slot.rangeLo = normalRangeLo;
    slot.rangeHi = normalRangeHi;
    slot.hasRange = true;
}

InstrumentSampleSet GetInstrumentSampleNames(uint8_t fontId, int16_t instId) {
    InstrumentSampleSet out;
    if (instId < 0) {
        return out;
    }
    auto& s = GetState();
    std::lock_guard<std::mutex> lock(s.mutex);
    auto it = s.entries.find(std::make_pair(fontId, instId));
    if (it == s.entries.end()) {
        return out;
    }
    out.low = it->second.low;
    out.normal = it->second.normal;
    out.high = it->second.high;
    out.rangeLo = it->second.rangeLo;
    out.rangeHi = it->second.rangeHi;
    out.hasRange = it->second.hasRange;
    out.lowTuning = it->second.lowTuning;
    out.normalTuning = it->second.normalTuning;
    out.highTuning = it->second.highTuning;
    return out;
}

int SuggestedOctaveShift(float tuning) {
    if (tuning <= 0.0f) {
        return 0;
    }
    int octaves = static_cast<int>(std::lround(std::log2(tuning)));
    return std::clamp(octaves * 12, -48, 48);
}

int SuggestedTranspose(uint8_t fontId, int16_t instId, uint8_t noteLow, uint8_t noteHigh) {
    const InstrumentSampleSet set = GetInstrumentSampleNames(fontId, instId);
    // Route the range midpoint through the engine's low/normal/high split to pick
    // the sample that governs most of this range, then fall back to whatever tuning
    // is actually registered (many slots only have a normal sample).
    const int mid = (static_cast<int>(noteLow) + static_cast<int>(noteHigh)) / 2;
    float tuning;
    if (mid < set.rangeLo) {
        tuning = set.lowTuning;
    } else if (mid > set.rangeHi) {
        tuning = set.highTuning;
    } else {
        tuning = set.normalTuning;
    }
    if (tuning <= 0.0f) {
        tuning = set.normalTuning > 0.0f ? set.normalTuning : set.lowTuning > 0.0f ? set.lowTuning : set.highTuning;
    }
    return SuggestedOctaveShift(tuning);
}

const char* StripSamplePathPrefix(const std::string& path) {
    const size_t slash = path.find_last_of('/');
    if (slash == std::string::npos) {
        return path.c_str();
    }
    return path.c_str() + slash + 1;
}

std::vector<RegisteredInstrument> EnumerateRegisteredInstruments() {
    std::vector<RegisteredInstrument> out;
    auto& s = GetState();
    std::lock_guard<std::mutex> lock(s.mutex);
    out.reserve(s.entries.size());
    // std::map iterates in ascending (fontId, instOrWave) key order.
    for (const auto& kv : s.entries) {
        const SlotSamples& slot = kv.second;
        const std::string& best = !slot.normal.empty() ? slot.normal : !slot.low.empty() ? slot.low : slot.high;
        out.push_back({ kv.first.first, kv.first.second, std::string(StripSamplePathPrefix(best)) });
    }
    return out;
}

void SetDrumSlotName(uint8_t fontId, uint8_t slot, std::string name) {
    auto& s = GetState();
    std::lock_guard<std::mutex> lock(s.mutex);
    s.drumSlots[std::make_pair(fontId, slot)] = std::move(name);
}

std::vector<uint8_t> EnumerateDrumFonts() {
    std::vector<uint8_t> out;
    auto& s = GetState();
    std::lock_guard<std::mutex> lock(s.mutex);
    // Map is sorted by (fontId, slot), so distinct fonts come out ascending.
    for (const auto& kv : s.drumSlots)
        if (out.empty() || out.back() != kv.first.first)
            out.push_back(kv.first.first);
    return out;
}

std::vector<RegisteredDrumSlot> EnumerateDrumSlots(uint8_t fontId) {
    std::vector<RegisteredDrumSlot> out;
    auto& s = GetState();
    std::lock_guard<std::mutex> lock(s.mutex);
    for (const auto& kv : s.drumSlots) {
        if (kv.first.first != fontId)
            continue;
        out.push_back({ kv.first.first, kv.first.second, std::string(StripSamplePathPrefix(kv.second)) });
    }
    return out;
}

} // namespace SOH
