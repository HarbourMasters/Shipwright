#include "item_override.h"

namespace Rando {
ItemOverride::ItemOverride(RandomizerCheck location_, RandomizerGet looksLike_)
    : location(location_), looksLike(looksLike_) {};

RandomizerCheck ItemOverride::GetLocation() const {
    return location;
}

RandomizerGet ItemOverride::LooksLike() const {
    return looksLike;
}

const Text& ItemOverride::GetTrickName() const {
    return trickName;
}

void ItemOverride::SetTrickName(Text trickName_) {
    trickName = trickName_;
}
} // namespace Rando