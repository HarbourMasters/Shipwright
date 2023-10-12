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
    void SetHintedRegion (Text region);
    void ResetVariables();
    bool IsAddedToPool();
    void AddToPool();

    private:
    Text text;
    RandomizerCheck hintedLocation;
    HintType hintType;
    Text hintedRegion;
    bool addedToPool;
};
}