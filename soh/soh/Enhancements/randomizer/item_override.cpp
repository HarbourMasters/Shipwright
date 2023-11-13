#include "item_override.h"

namespace Rando {
ItemOverride::ItemOverride(RandomizerCheck location_, RandomizerGet looksLike_)
    : location(location_), looksLike(looksLike_) {};

RandomizerCheck ItemOverride::GetLocation() const {
    return location;
}

void ItemOverride::SetLocation(RandomizerCheck location_) {
    location = location_;
}

RandomizerGet ItemOverride::LooksLike() const {
    return looksLike;
}

RandomizerGet& ItemOverride::RefLooksLike() {
    return looksLike;
}

Text& ItemOverride::GetTrickName() {
    return trickName;
}

void ItemOverride::SetTrickName(Text trickName_) {
    trickName = trickName_;
}
} // namespace Rando