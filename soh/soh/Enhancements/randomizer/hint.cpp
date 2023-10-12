#include "hint.h"

RandoHint::RandoHint() {}
RandoHint::RandoHint(Text text_): text(std::move(text_)) {}
RandoHint::RandoHint(Text text_, RandomizerCheck hintedLocation_, HintType hintType_, Text hintedRegion_)
    : text(std::move(text_)), hintedLocation(hintedLocation_), hintType(hintType_),
      hintedRegion(std::move(hintedRegion_)) {
}

const Text& RandoHint::GetText() const {
    return text;
}

void RandoHint::SetHintedLocation(RandomizerCheck location) {
    hintedLocation = location;
}

RandomizerCheck RandoHint::GetHintedLocation() {
    return hintedLocation;
}

void RandoHint::SetHintType(HintType type) {
    hintType = type;
}

HintType RandoHint::GetHintType() {
    return hintType;
}

void RandoHint::SetHintedRegion(Text region) {
    hintedRegion = region;
}

std::string RandoHint::GetHintedRegion() {
    return hintedRegion.GetEnglish();
}

void RandoHint::ResetVariables() {
    hintedLocation = RC_UNKNOWN_CHECK;
    addedToPool = false;
}