#include "hint.h"

namespace Rando {
Hint::Hint() : text(std::move(Text())) {}
Hint::Hint(Text text_): text(std::move(text_)) {}
Hint::Hint(Text text_, RandomizerCheck hintedLocation_, HintType hintType_, std::string distributionName_, RandomizerArea hintedArea_)
    : text(std::move(text_)), hintedLocation(hintedLocation_), hintType(hintType_),
      hintedArea(hintedArea_), distribution(std::move(distributionName_)) {
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

void Hint::SetHintedArea(RandomizerArea area) {
    hintedArea = area;
}

RandomizerArea Hint::GetHintedArea() {
    return hintedArea;
}

void Hint::SetDistribution(std::string distributionName) {
    distribution = distributionName;
}

const std::string& Hint::GetDistribution() {
    return distribution;
}


void Hint::ResetVariables() {
    hintedLocation = RC_UNKNOWN_CHECK;
    addedToPool = false;
}
}