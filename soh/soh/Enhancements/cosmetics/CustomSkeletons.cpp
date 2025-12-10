#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/resource/type/Skeleton.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
#include "variables.h"
extern PlayState* gPlayState;
}

static void UpdateCustomSkeletonOnEquipTunic() {
    if (!GameInteractor::IsSaveLoaded() || gPlayState == NULL) {
        return;
    }

    static int8_t previousTunic = -1;
    int8_t equippedTunic = CUR_EQUIP_VALUE(EQUIP_TYPE_TUNIC);
    if (equippedTunic != previousTunic) {
        SOH::SkeletonPatcher::UpdateCustomSkeletons();
        previousTunic = equippedTunic;
    }
}

static void UpdateCustomSkeletonOnAssetAltChange() {
    if (!GameInteractor::IsSaveLoaded() || gPlayState == NULL) {
        return;
    }

    SOH::SkeletonPatcher::UpdateCustomSkeletons();
}

static void RegisterCustomSkeletons() {
    COND_HOOK(OnGameFrameUpdate, true, UpdateCustomSkeletonOnEquipTunic);
    COND_HOOK(OnAssetAltChange, true, UpdateCustomSkeletonOnAssetAltChange);
}

static RegisterShipInitFunc initFunc(RegisterCustomSkeletons);
