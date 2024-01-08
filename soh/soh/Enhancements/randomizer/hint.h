#pragma once

#include "3drando/text.hpp"
#include "randomizerTypes.h"

namespace Rando {
class Hint {
    public:
    Hint();
    Hint(Text text_);
    Hint(Text text_, RandomizerCheck hintedLocation_, HintType hintType_, std::string distributionName_, RandomizerArea hintedArea_);
    const Text& GetText() const;
    RandomizerCheck GetHintedLocation();
    void SetHintedLocation (RandomizerCheck location);
    HintType GetHintType();
    void SetHintType (HintType type);
    RandomizerArea GetHintedArea();
    void SetHintedArea (RandomizerArea area);
    const std::string& GetDistribution();
    void SetDistribution (std::string distribution);
    void ResetVariables();
    bool IsAddedToPool();
    void AddToPool();

    private:
    Text text = Text();
    RandomizerCheck hintedLocation = RC_UNKNOWN_CHECK;
    HintType hintType = HINT_TYPE_STATIC;
    RandomizerArea hintedArea = RA_NONE;
    std::string distribution = "";
    bool addedToPool = false;
};
}