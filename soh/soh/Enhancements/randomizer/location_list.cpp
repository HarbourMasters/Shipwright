#include "static_data.h"

std::array<Rando::Location, RC_MAX> StaticData::locationTable;

void StaticData::InitLocationTable() {
    using namespace Rando;
    locationTable[RC_UNKNOWN_CHECK] = Rando::Location::Base(
        RC_UNKNOWN_CHECK, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_INVALID, SCENE_ID_MAX, 0xFF, ACTOR_ID_MAX, 0x00,
        "Invalid Location", "Invalid Location", NONE, RG_NONE, {}, Rando::SpoilerCollectionCheck::None());
}