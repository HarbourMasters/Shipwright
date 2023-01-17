#include "GameInteractionEffect.h"
#include "GameInteractor.h"

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
}

uint32_t GameInteractor_NoUIActive;
uint32_t GameInteractor_GiantLinkActive;
uint32_t GameInteractor_MinishLinkActive;
uint32_t GameInteractor_PaperLinkActive;
uint32_t GameInteractor_InvisibleLinkActive;
uint32_t GameInteractor_ResetLinkScale;
uint32_t GameInteractor_OneHitKOActive;
uint32_t GameInteractor_PacifistModeActive;
uint32_t GameInteractor_DefenseModifier;
uint32_t GameInteractor_DisableZTargetingActive;
uint32_t GameInteractor_ReverseControlsActive;
uint32_t GameInteractor_RunSpeedModifier;
uint32_t GameInteractor_GravityLevel = GRAVITY_LEVEL_NORMAL;

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
        if (!gSaveContext.isMagicAcquired || gSaveContext.magic >= (gSaveContext.isDoubleMagicAcquired + 1) + 0x30) {
            return GameInteractionEffectQueryResult::NotPossible;
        } else if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void FillMagic::Apply() {
        Magic_Fill(gPlayState);
    }
    void FillMagic::Remove() {}

    // EmptyMagic
    GameInteractionEffectQueryResult EmptyMagic::CanBeApplied() {
        if (!gSaveContext.isMagicAcquired || gSaveContext.magic <= 0) {
            return GameInteractionEffectQueryResult::NotPossible;
        } else if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void EmptyMagic::Apply() {
        gSaveContext.magic = 0;
    }
    void EmptyMagic::Remove() {}

    // GiveRupees
    GameInteractionEffectQueryResult GiveRupees::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void GiveRupees::Apply() {
        Rupees_ChangeBy(value);
    }
    void GiveRupees::Remove() {}

    // TakeRupees
    GameInteractionEffectQueryResult TakeRupees::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void TakeRupees::Apply() {
        Rupees_ChangeBy(-value);
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
        GameInteractor_NoUIActive = 1;
    }
    void NoUI::Remove() {
        GameInteractor_NoUIActive = 0;
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
        GameInteractor_GravityLevel = GRAVITY_LEVEL_HEAVY;
    }
    void HighGravity::Remove() {
        GameInteractor_GravityLevel = GRAVITY_LEVEL_NORMAL;
    }

    // LowGravity
    GameInteractionEffectQueryResult LowGravity::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void LowGravity::Apply() {
        GameInteractor_GravityLevel = GRAVITY_LEVEL_LIGHT;
    }
    void LowGravity::Remove() {
        GameInteractor_GravityLevel = GRAVITY_LEVEL_NORMAL;
    }

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
        if (!GameInteractor::IsSaveLoaded()) {
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
        if (!GameInteractor::IsSaveLoaded()) {
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
        GameInteractor_GiantLinkActive = 1;
    }
    void GiantLink::Remove() { 
        GameInteractor_GiantLinkActive = 0;
        GameInteractor_ResetLinkScale = 1;
    }

    // MinishLink
    GameInteractionEffectQueryResult MinishLink::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void MinishLink::Apply() {
        GameInteractor_MinishLinkActive = 1;
    }
    void MinishLink::Remove() {
        GameInteractor_MinishLinkActive = 0;
        GameInteractor_ResetLinkScale = 1;
    }

    // InvisibleLink
    GameInteractionEffectQueryResult InvisibleLink::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void InvisibleLink::Apply() {
        GameInteractor_InvisibleLinkActive = 1;
    }
    void InvisibleLink::Remove() {
        GameInteractor_InvisibleLinkActive = 0;
    }

    // PaperLink
    GameInteractionEffectQueryResult PaperLink::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void PaperLink::Apply() {
        GameInteractor_PaperLinkActive = 1;
    }
    void PaperLink::Remove() {
        GameInteractor_PaperLinkActive = 0;
        GameInteractor_ResetLinkScale = 1;
    }

    // DisableZTargeting
    GameInteractionEffectQueryResult DisableZTargeting::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void DisableZTargeting::Apply() {
        GameInteractor_DisableZTargetingActive = 1;
    }
    void DisableZTargeting::Remove() {
        GameInteractor_DisableZTargetingActive = 0;
    }

    // PacifistMode
    GameInteractionEffectQueryResult PacifistMode::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void PacifistMode::Apply() {
        GameInteractor_PacifistModeActive = 1;
    }
    void PacifistMode::Remove() {
        GameInteractor_PacifistModeActive = 0;
    }

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
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void ReverseControls::Apply() {
        GameInteractor_ReverseControlsActive = 1;
    }
    void ReverseControls::Remove() {
        GameInteractor_ReverseControlsActive = 0;
    }

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
        if (!GameInteractor::IsSaveLoaded()) {
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
        GameInteractor_RunSpeedModifier = 2;
    }
    void IncreaseRunSpeed::Remove() {
        GameInteractor_RunSpeedModifier = 0;
    }

    // DecreaseRunSpeed
    GameInteractionEffectQueryResult DecreaseRunSpeed::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void DecreaseRunSpeed::Apply() {
        GameInteractor_RunSpeedModifier = -2;
    }
    void DecreaseRunSpeed::Remove() {
        GameInteractor_RunSpeedModifier = 0;
    }

    // OneHitKO
    GameInteractionEffectQueryResult OneHitKO::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void OneHitKO::Apply() {
        GameInteractor_OneHitKOActive = 1;
    }
    void OneHitKO::Remove() {
        GameInteractor_OneHitKOActive = 0;
    }

    // IncreaseDamageTaken
    GameInteractionEffectQueryResult IncreaseDamageTaken::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void IncreaseDamageTaken::Apply() {
        GameInteractor_DefenseModifier = -value;
    }
    void IncreaseDamageTaken::Remove() {
        GameInteractor_DefenseModifier = 0;
    }

    // DecreaseDamageTaken
    GameInteractionEffectQueryResult DecreaseDamageTaken::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void DecreaseDamageTaken::Apply() {
        GameInteractor_DefenseModifier = value;
    }
    void DecreaseDamageTaken::Remove() {
        GameInteractor_DefenseModifier = 0;
    }

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
