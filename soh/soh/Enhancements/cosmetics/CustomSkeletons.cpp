#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/resource/type/Skeleton.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
#include "variables.h"
extern PlayState* gPlayState;
}

static void UpdateCustomSkeleton() {
    if (!GameInteractor::IsSaveLoaded(true) || gPlayState == NULL) {
        return;
    }

    SOH::SkeletonPatcher::UpdateCustomSkeletons();
}

static void RegisterCustomSkeletons() {
    COND_HOOK(OnAssetAltChange, true, UpdateCustomSkeleton);
    COND_HOOK(OnLinkSkeletonInit, true, SOH::SkeletonPatcher::UpdateCustomSkeletons);
    COND_HOOK(OnLinkEquipmentChange, true, SOH::SkeletonPatcher::UpdateCustomSkeletons);
}

static RegisterShipInitFunc initFunc(RegisterCustomSkeletons);
