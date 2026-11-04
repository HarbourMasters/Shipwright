#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/randomizer.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Hy/z_en_hy.h"
}

void Fishing_GiveHatItem(uint16_t* textId, bool* loadFromMessageTable) {
    if (Flags_GetRandomizerInf(RAND_INF_FISHERMANS_HAT)) {
        return;
    }
    Flags_SetRandomizerInf(RAND_INF_FISHERMANS_HAT);
    *loadFromMessageTable = true;
}

void RegisterShuffleFishermansHat() {
    bool shouldRegister = IS_RANDO && Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_FISHERMANS_HAT).Get();

    COND_ID_HOOK(OnOpenText, TEXT_FISHING_HAT_RETURNED, shouldRegister, Fishing_GiveHatItem);
}

void Rando::StaticData::RegisterFishermansHatLocation() {
    static bool registered = false;
    if (registered)
        return;
    registered = true;
    // clang-format off
    //            Randomizer Check          Randomizer Check                            Quest            RCType               Area                 Actor ID       Scene ID                    Params                         Short Name                  Hint Text Key                   Vanilla Item        Spoiler Collection Check
    locationTable[RC_FISHERMANS_HAT]        = Location::Base(RC_FISHERMANS_HAT,         RCQUEST_BOTH,    RCTYPE_FISH_HAT,     RCAREA_LAKE_HYLIA,   ACTOR_FISHING, SCENE_FISHING_POND,         TEXT_FISHING_HAT_RETURNED,     "Fisherman's Hat",          RHT_FISHERMANS_HAT,             RG_NONE,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_FISHERMANS_HAT));
    // clang-format on
}

static RegisterShipInitFunc registerShuffleFishermansHat(RegisterShuffleFishermansHat, { "IS_RANDO" });
static RegisterShipInitFunc registerFishermansHatLocation(Rando::StaticData::RegisterFishermansHatLocation);