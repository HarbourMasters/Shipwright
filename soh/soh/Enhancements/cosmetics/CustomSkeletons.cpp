#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/resource/type/Skeleton.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
#include "variables.h"
extern PlayState* gPlayState;
}

static void UpdateCustomSkeleton(IEvent* event) {
    if (!GameInteractor::IsSaveLoaded(true) || gPlayState == NULL) {
        return;
    }

    SOH::SkeletonPatcher::UpdateCustomSkeletons();
}

static void AlwaysUpdateCustomSkeleton(IEvent* event) {
    SOH::SkeletonPatcher::UpdateCustomSkeletons();
}

static void RegisterCustomSkeletons() {
    COND_HOOK(OnAssetAltChange, true, UpdateCustomSkeleton);
    COND_HOOK(OnLinkSkeletonInit, true, AlwaysUpdateCustomSkeleton);
    COND_HOOK(OnLinkEquipmentChange, true, AlwaysUpdateCustomSkeleton);
}

static RegisterShipInitFunc initFunc(RegisterCustomSkeletons);
