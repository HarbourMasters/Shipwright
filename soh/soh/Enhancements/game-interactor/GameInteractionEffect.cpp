#include "GameInteractionEffect.h"
#include "GameInteractor.h"

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
}

// Effect State

uint32_t GameInteractor_NoUI;

// AddHeartContainer

namespace GameInteractionEffect {

    // AddHeartContainer
    GameInteractionEffectQueryResult AddHeartContainer::CanBeApplied() {
        if (gSaveContext.healthCapacity >= 0x140) {
            return GameInteractionEffectQueryResult::NotPossible;
        } else if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void AddHeartContainer::Apply() {
        Health_GiveHearts(1);
    }
    void AddHeartContainer::Remove() {}

    // RemoveHeartContainer
    GameInteractionEffectQueryResult RemoveHeartContainer::CanBeApplied() {
        if (gSaveContext.healthCapacity <= 0x10) {
            return GameInteractionEffectQueryResult::NotPossible;
        } else if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void RemoveHeartContainer::Apply() {
        Health_RemoveHearts(1);
    }
    void RemoveHeartContainer::Remove() {}

    // FillMagic
    GameInteractionEffectQueryResult FillMagic::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void FillMagic::Apply() {
        
    }
    void FillMagic::Remove() {}

    // EmptyMagic
    GameInteractionEffectQueryResult EmptyMagic::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void EmptyMagic::Apply() {
        
    }
    void EmptyMagic::Remove() {}

    // GiveRupees
    GameInteractionEffectQueryResult GiveRupees::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void GiveRupees::Apply() {
        
    }
    void GiveRupees::Remove() {}

    // TakeRupees
    GameInteractionEffectQueryResult TakeRupees::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void TakeRupees::Apply() {
        
    }
    void TakeRupees::Remove() {}

    // No UI
    GameInteractionEffectQueryResult NoUI::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void NoUI::Apply() {
        GameInteractor_NoUI = 1;
    }
    void NoUI::Remove() {
        GameInteractor_NoUI = 0;
    }

    // HighGravity
    GameInteractionEffectQueryResult HighGravity::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void HighGravity::Apply() {
        
    }
    void HighGravity::Remove() {}

    // LowGravity
    GameInteractionEffectQueryResult LowGravity::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void LowGravity::Apply() {
        
    }
    void LowGravity::Remove() {}

    // KillPlayer
    GameInteractionEffectQueryResult KillPlayer::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void KillPlayer::Apply() {
        
    }
    void KillPlayer::Remove() {}

    // FreezePlayer
    GameInteractionEffectQueryResult FreezePlayer::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void FreezePlayer::Apply() {
        
    }
    void FreezePlayer::Remove() {}

    // BurnPlayer
    GameInteractionEffectQueryResult BurnPlayer::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void BurnPlayer::Apply() {
        
    }
    void BurnPlayer::Remove() {}

    // ElectrocutePlayer
    GameInteractionEffectQueryResult ElectrocutePlayer::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void ElectrocutePlayer::Apply() {
        
    }
    void ElectrocutePlayer::Remove() {}

    // KnockbackPlayer
    GameInteractionEffectQueryResult KnockbackPlayer::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void KnockbackPlayer::Apply() {
        
    }
    void KnockbackPlayer::Remove() {}

    // GiveHealth
    GameInteractionEffectQueryResult GiveHealth::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void GiveHealth::Apply() {
        
    }
    void GiveHealth::Remove() {}

    // TakeHealth
    GameInteractionEffectQueryResult TakeHealth::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void TakeHealth::Apply() {
        
    }
    void TakeHealth::Remove() {}

    // GiantLink
    GameInteractionEffectQueryResult GiantLink::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void GiantLink::Apply() {
        
    }
    void GiantLink::Remove() {}

    // MinishLink
    GameInteractionEffectQueryResult MinishLink::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void MinishLink::Apply() {
        
    }
    void MinishLink::Remove() {}

    // InvisibleLink
    GameInteractionEffectQueryResult InvisibleLink::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void InvisibleLink::Apply() {
        
    }
    void InvisibleLink::Remove() {}

    // PaperLink
    GameInteractionEffectQueryResult PaperLink::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void PaperLink::Apply() {
        
    }
    void PaperLink::Remove() {}

    // DisableZTargeting
    GameInteractionEffectQueryResult DisableZTargeting::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void DisableZTargeting::Apply() {
        
    }
    void DisableZTargeting::Remove() {}

    // PacifistMode
    GameInteractionEffectQueryResult PacifistMode::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void PacifistMode::Apply() {
        
    }
    void PacifistMode::Remove() {}

    // WeatherRainstorm
    GameInteractionEffectQueryResult WeatherRainstorm::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void WeatherRainstorm::Apply() {
        
    }
    void WeatherRainstorm::Remove() {}

    // ReverseControls
    GameInteractionEffectQueryResult ReverseControls::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void ReverseControls::Apply() {
        
    }
    void ReverseControls::Remove() {}

    // ForceIronBoots
    GameInteractionEffectQueryResult ForceIronBoots::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void ForceIronBoots::Apply() {
        
    }
    void ForceIronBoots::Remove() {}

    // ForceHoverBoots
    GameInteractionEffectQueryResult ForceHoverBoots::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void ForceHoverBoots::Apply() {
        
    }
    void ForceHoverBoots::Remove() {}

    // GiveDekuShield
    GameInteractionEffectQueryResult GiveDekuShield::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void GiveDekuShield::Apply() {
        
    }
    void GiveDekuShield::Remove() {}

    // IncreaseRunSpeed
    GameInteractionEffectQueryResult IncreaseRunSpeed::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void IncreaseRunSpeed::Apply() {
        
    }
    void IncreaseRunSpeed::Remove() {}

    // DecreaseRunSpeed
    GameInteractionEffectQueryResult DecreaseRunSpeed::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void DecreaseRunSpeed::Apply() {
        
    }
    void DecreaseRunSpeed::Remove() {}

    // OneHitKO
    GameInteractionEffectQueryResult OneHitKO::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void OneHitKO::Apply() {
    }
    void OneHitKO::Remove() {}

    // IncreaseDamageTaken
    GameInteractionEffectQueryResult IncreaseDamageTaken::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void IncreaseDamageTaken::Apply() {
        
    }
    void IncreaseDamageTaken::Remove() {}

    // DecreaseDamageTaken
    GameInteractionEffectQueryResult DecreaseDamageTaken::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void DecreaseDamageTaken::Apply() {
        
    }
    void DecreaseDamageTaken::Remove() {}

    // SpawnEnemy
    GameInteractionEffectQueryResult SpawnEnemy::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void SpawnEnemy::Apply() {
        
    }
    void SpawnEnemy::Remove() {}
}
