#include "hint.h"

namespace Rando {
Hint::Hint() : text(std::move(Text())) {}
Hint::Hint(Text text_): text(std::move(text_)) {}
Hint::Hint(Text text_, RandomizerCheck hintedLocation_, HintType hintType_, Text hintedRegion_)
    : text(std::move(text_)), hintedLocation(hintedLocation_), hintType(hintType_),
      hintedRegion(std::move(hintedRegion_)) {
}

const Text& Hint::GetText() const {
    return text;
}

void Hint::SetHintedLocation(RandomizerCheck location) {
    hintedLocation = location;
}

RandomizerCheck Hint::GetHintedLocation() {
    return hintedLocation;
}

void Hint::SetHintType(HintType type) {
    hintType = type;
}

HintType Hint::GetHintType() {
    return hintType;
}

void Hint::SetHintedRegion(Text region) {
    hintedRegion = region;
}

std::string Hint::GetHintedRegion() {
    return hintedRegion.GetEnglish();
}

const Text& Hint::GetHintedRegionText() {
    return hintedRegion;
};

void Hint::ResetVariables() {
    hintedLocation = RC_UNKNOWN_CHECK;
    addedToPool = false;
}
}