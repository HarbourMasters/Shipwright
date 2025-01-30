#include "item_override.h"

#include <utility>

namespace Rando {
ItemOverride::ItemOverride() : mLocation(RC_UNKNOWN_CHECK), mLooksLike(RG_NONE) {}
ItemOverride::ItemOverride(const RandomizerCheck location, const RandomizerGet looksLike)
    : mLocation(location), mLooksLike(looksLike) {}

RandomizerCheck ItemOverride::GetLocation() const {
    return mLocation;
}

void ItemOverride::SetLocation(const RandomizerCheck location) {
    mLocation = location;
}

RandomizerGet ItemOverride::LooksLike() const {
    return mLooksLike;
}

RandomizerGet& ItemOverride::RefLooksLike() {
    return mLooksLike;
}

Text& ItemOverride::GetTrickName() {
    return mTrickName;
}

void ItemOverride::SetTrickName(Text trickName) {
    mTrickName = std::move(trickName);
}
} // namespace Rando