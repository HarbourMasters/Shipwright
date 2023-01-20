#include "GameInteractionEffect.h"
#include "GameInteractor.h"
#include <libultraship/bridge.h>

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
uint32_t GameInteractor_DisableZTargetingActive;
uint32_t GameInteractor_ReverseControlsActive;
int32_t GameInteractor_DefenseModifier;
int32_t GameInteractor_RunSpeedModifier;
uint32_t GameInteractor_GravityLevel = GRAVITY_LEVEL_NORMAL;

namespace GameInteractionEffect {

    // MARK: - AddHeartContainer
    GameInteractionEffectQueryResult AddHeartContainer::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else if (gSaveContext.healthCapacity >= 0x140) {
            return GameInteractionEffectQueryResult::NotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void AddHeartContainer::Apply() {
        GameInteractor::Actions::AddOrRemoveHealthContainers(1);
    }
    void AddHeartContainer::Remove() {}

    // MARK: - RemoveHeartContainer
    GameInteractionEffectQueryResult RemoveHeartContainer::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else if (gSaveContext.healthCapacity <= 0x10) {
            return GameInteractionEffectQueryResult::NotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void RemoveHeartContainer::Apply() {
        GameInteractor::Actions::AddOrRemoveHealthContainers(-1);
    }
    void RemoveHeartContainer::Remove() {}

    // MARK: - FillMagic
    GameInteractionEffectQueryResult FillMagic::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else if (!gSaveContext.isMagicAcquired || gSaveContext.magic >= ((gSaveContext.isDoubleMagicAcquired + 1) * 48)) {
            return GameInteractionEffectQueryResult::NotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void FillMagic::Apply() {
        GameInteractor::Actions::AddOrRemoveMagic(96);
    }
    void FillMagic::Remove() {}

    // MARK: - EmptyMagic
    GameInteractionEffectQueryResult EmptyMagic::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else if (!gSaveContext.isMagicAcquired || gSaveContext.magic <= 0) {
            return GameInteractionEffectQueryResult::NotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void EmptyMagic::Apply() {
        GameInteractor::Actions::AddOrRemoveMagic(-96);
    }
    void EmptyMagic::Remove() {}

    // MARK: - GiveRupees
    GameInteractionEffectQueryResult GiveRupees::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void GiveRupees::Apply() {
        Rupees_ChangeBy(parameter);
    }
    void GiveRupees::Remove() {}

    // MARK: - TakeRupees
    GameInteractionEffectQueryResult TakeRupees::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else if (gSaveContext.rupees <= 0) {
            return GameInteractionEffectQueryResult::NotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void TakeRupees::Apply() {
        Rupees_ChangeBy(-parameter);
    }
    void TakeRupees::Remove() {}

    // MARK: - NoUI
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

    // MARK: - HighGravity
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

    // MARK: - LowGravity
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

    // MARK: - GiveHealth
    GameInteractionEffectQueryResult GiveHealth::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else if (gSaveContext.health == gSaveContext.healthCapacity) {
            return GameInteractionEffectQueryResult::NotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void GiveHealth::Apply() {
        GameInteractor::Actions::HealOrDamagePlayer(parameter);
    }
    void GiveHealth::Remove() {}

    // MARK: - TakeHealth
    GameInteractionEffectQueryResult TakeHealth::CanBeApplied() {
        int32_t healthAfterEffect = (gSaveContext.health - (16 * parameter));
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else if (healthAfterEffect <= 0) {
            return GameInteractionEffectQueryResult::NotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void TakeHealth::Apply() {
        GameInteractor::Actions::HealOrDamagePlayer(-parameter);
    }
    void TakeHealth::Remove() {}

    // MARK: - KillPlayer
    GameInteractionEffectQueryResult KillPlayer::CanBeApplied() {
        Player* player = GET_PLAYER(gPlayState);
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused() || !PlayerGrounded(player)) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void KillPlayer::Apply() {
        GameInteractor::Actions::HealOrDamagePlayer(-50);
    }
    void KillPlayer::Remove() {}

    // MARK: - FreezePlayer
    GameInteractionEffectQueryResult FreezePlayer::CanBeApplied() {
        Player* player = GET_PLAYER(gPlayState);
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void FreezePlayer::Apply() {
        GameInteractor::Actions::FreezePlayer();
    }
    void FreezePlayer::Remove() {}

    // MARK: - BurnPlayer
    GameInteractionEffectQueryResult BurnPlayer::CanBeApplied() {
        Player* player = GET_PLAYER(gPlayState);
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused() || !PlayerGrounded(player)) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void BurnPlayer::Apply() {
        GameInteractor::Actions::BurnPlayer();
    }
    void BurnPlayer::Remove() {}

    // MARK: - ElectrocutePlayer
    GameInteractionEffectQueryResult ElectrocutePlayer::CanBeApplied() {
        Player* player = GET_PLAYER(gPlayState);
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused() || !PlayerGrounded(player)) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void ElectrocutePlayer::Apply() {
        GameInteractor::Actions::ElectrocutePlayer();
    }
    void ElectrocutePlayer::Remove() {}

    // MARK: - KnockbackPlayer
    GameInteractionEffectQueryResult KnockbackPlayer::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void KnockbackPlayer::Apply() {
        GameInteractor::Actions::KnockbackPlayer(parameter);
    }
    void KnockbackPlayer::Remove() {}

    // MARK: - GiantLink
    GameInteractionEffectQueryResult GiantLink::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void GiantLink::Apply() {
        GameInteractor::Actions::SetLinkSize(GI_LINK_SIZE_GIANT);
    }
    void GiantLink::Remove() { 
        GameInteractor::Actions::SetLinkSize(GI_LINK_SIZE_NORMAL);
    }

    // MARK: - MinishLink
    GameInteractionEffectQueryResult MinishLink::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void MinishLink::Apply() {
        GameInteractor::Actions::SetLinkSize(GI_LINK_SIZE_MINISH);
    }
    void MinishLink::Remove() {
        GameInteractor::Actions::SetLinkSize(GI_LINK_SIZE_NORMAL);
    }

    // MARK: - PaperLink
    GameInteractionEffectQueryResult PaperLink::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void PaperLink::Apply() {
        GameInteractor::Actions::SetLinkSize(GI_LINK_SIZE_PAPER);
    }
    void PaperLink::Remove() {
        GameInteractor::Actions::SetLinkSize(GI_LINK_SIZE_NORMAL);
    }

    // MARK: - InvisibleLink
    GameInteractionEffectQueryResult InvisibleLink::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void InvisibleLink::Apply() {
        GameInteractor::Actions::SetLinkInvisibility(1);
    }
    void InvisibleLink::Remove() {
        GameInteractor::Actions::SetLinkInvisibility(0);
    }

    // MARK: - PacifistMode
    GameInteractionEffectQueryResult PacifistMode::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void PacifistMode::Apply() {
        GameInteractor::Actions::SetPacifistMode(1);
    }
    void PacifistMode::Remove() {
        GameInteractor::Actions::SetPacifistMode(0);
    }

    // MARK: - DisableZTargeting
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

    // MARK: - WeatherRainstorm
    GameInteractionEffectQueryResult WeatherRainstorm::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void WeatherRainstorm::Apply() {
        GameInteractor::Actions::SetWeatherStorm(1);
    }
    void WeatherRainstorm::Remove() {
        GameInteractor::Actions::SetWeatherStorm(0);
    }

    // MARK: - ReverseControls
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

    // MARK: - ForceIronBoots
    GameInteractionEffectQueryResult ForceIronBoots::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void ForceIronBoots::Apply() {
        GameInteractor::Actions::ForceEquipBoots(PLAYER_BOOTS_IRON);
    }
    void ForceIronBoots::Remove() {
        GameInteractor::Actions::ForceEquipBoots(PLAYER_BOOTS_KOKIRI);
    }

    // MARK: - ForceHoverBoots
    GameInteractionEffectQueryResult ForceHoverBoots::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void ForceHoverBoots::Apply() {
        GameInteractor::Actions::ForceEquipBoots(PLAYER_BOOTS_HOVER);
    }
    void ForceHoverBoots::Remove() {
        GameInteractor::Actions::ForceEquipBoots(PLAYER_BOOTS_KOKIRI);
    }

    // MARK: - IncreaseRunSpeed
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

    // MARK: - DecreaseRunSpeed
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

    // MARK: - OneHitKO
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

    // MARK: - IncreaseDamageTaken
    GameInteractionEffectQueryResult IncreaseDamageTaken::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void IncreaseDamageTaken::Apply() {
        GameInteractor_DefenseModifier = -parameter;
    }
    void IncreaseDamageTaken::Remove() {
        GameInteractor_DefenseModifier = 0;
    }

    // MARK: - DecreaseDamageTaken
    GameInteractionEffectQueryResult DecreaseDamageTaken::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void DecreaseDamageTaken::Apply() {
        GameInteractor_DefenseModifier = parameter;
    }
    void DecreaseDamageTaken::Remove() {
        GameInteractor_DefenseModifier = 0;
    }

    // MARK: - GiveDekuShield
    GameInteractionEffectQueryResult GiveDekuShield::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void GiveDekuShield::Apply() {
        GameInteractor::Actions::GiveDekuShield();
    }
    void GiveDekuShield::Remove() {}

    // MARK: - SpawnCuccoStorm
    GameInteractionEffectQueryResult SpawnCuccoStorm::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void SpawnCuccoStorm::Apply() {
        GameInteractor::Actions::SpawnCuccoStorm();
    }
    void SpawnCuccoStorm::Remove() {}
}
