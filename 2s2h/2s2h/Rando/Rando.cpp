#include "Rando.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "Rando/ActorBehavior/ActorBehavior.h"
#include "Rando/MiscBehavior/MiscBehavior.h"
#include "Rando/MiscBehavior/ClockShuffle.h"
#include "Rando/Spoiler/Spoiler.h"
#include "Rando/CheckTracker/CheckTracker.h"
#include "2s2h/ShipInit.hpp"
#include <ship/window/FileDropMgr.h>
#include <ship/Context.h>

// When a save is loaded, we want to unregister all hooks and re-register them if it's a rando save
void OnSaveLoadHandler(s16 fileNum) {
    Rando::MiscBehavior::OnFileLoad();
    Rando::ActorBehavior::OnFileLoad();
    Rando::CheckTracker::OnFileLoad();
    Rando::ClockShuffle::OnFileLoad();

    // Re-initalizes enhancements that are effected by the save being rando or not
    ShipInit::Init("IS_RANDO");
}

// Entry point for the module, run once on game boot
void Rando::Init() {
    Rando::Spoiler::RefreshOptions();
    Rando::MiscBehavior::Init();
    Rando::ActorBehavior::Init();
    Rando::CheckTracker::Init();
    Ship::Context::GetRawInstance()->GetFileDropMgr()->RegisterDropHandler(Rando::Spoiler::HandleFileDropped);

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveLoad>(OnSaveLoadHandler);
}

RandoCheckId Rando::FindItemPlacement(RandoItemId randoItemId) {
    for (auto& [randoCheckId, check] : Rando::StaticData::Checks) {
        if (RANDO_SAVE_CHECKS[randoCheckId].randoItemId == randoItemId) {
            return randoCheckId;
        }
    }

    return RC_UNKNOWN;
}

std::vector<RandoCheckId> Rando::FindMultiItemPlacement(RandoItemId randoItemId) {
    std::vector<RandoCheckId> itemPlacements;
    for (auto& [randocheckId, check] : Rando::StaticData::Checks) {
        if (RANDO_SAVE_CHECKS[randocheckId].randoItemId == randoItemId) {
            itemPlacements.push_back(randocheckId);
        }
    }
    return itemPlacements;
}
