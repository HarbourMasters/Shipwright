#pragma once

#include "randomizerTypes.h"
#include "3drando/text.hpp"

namespace Rando {
/// @brief Class representing overrides of individual items. Used for trick names and models for ice traps.
class ItemOverride {
  public:
    ItemOverride() = default;
    ItemOverride(RandomizerCheck location_, RandomizerGet looksLike_);
    RandomizerCheck GetLocation() const;
    RandomizerGet LooksLike() const;
    const Text& GetTrickName() const;
    void SetTrickName (Text trickName);
  private:
    RandomizerCheck location;
    RandomizerGet looksLike;
    Text trickName;
};
} // namespace Rando