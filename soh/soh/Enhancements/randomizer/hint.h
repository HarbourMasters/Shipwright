#pragma once

#include "3drando/text.hpp"
#include "randomizerTypes.h"

namespace Rando {
class Hint {
    public:
    Hint();
    Hint(Text text_);
    Hint(Text text_, RandomizerCheck hintedLocation_, HintType hintType_, Text hintedRegion_);
    const Text& GetText() const;
    RandomizerCheck GetHintedLocation();
    void SetHintedLocation (RandomizerCheck location);
    HintType GetHintType();
    void SetHintType (HintType type);
    std::string GetHintedRegion();
    const Text& GetHintedRegionText();
    void SetHintedRegion (Text region);
    void ResetVariables();
    bool IsAddedToPool();
    void AddToPool();

    private:
    Text text = Text();
    RandomizerCheck hintedLocation = RC_UNKNOWN_CHECK;
    HintType hintType = HINT_TYPE_STATIC;
    Text hintedRegion = Text();
    bool addedToPool = false;
};
}