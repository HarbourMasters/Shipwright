#pragma once

#include "randomizerTypes.h"
#include "3drando/text.hpp"

namespace Rando {
/// @brief Class representing overrides of individual items. Used for trick names and models for ice traps.
class ItemOverride {
  public:
    ItemOverride();
    ItemOverride(RandomizerCheck location, RandomizerGet looksLike);
    RandomizerCheck GetLocation() const;
    void SetLocation(RandomizerCheck location);
    RandomizerGet LooksLike() const;
    RandomizerGet& RefLooksLike();
    Text& GetTrickName();
    void SetTrickName (Text trickName);
  private:
    RandomizerCheck mLocation;
    RandomizerGet mLooksLike;
    Text mTrickName;
};
} // namespace Rando