/*
GameInteractionEffects is used in conjunction with GameInteractor.

It's intended to be used in places that want to interact with the game
while having checks built-in for if said effect is able to be executed.

Effects that can last for a certain amount of time (timed effects)
have functions to both enable and disable said effect.
*/

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

GameInteractionEffectQueryResult GameInteractionEffectBase::Apply() {
    GameInteractionEffectQueryResult result = CanBeApplied();
    if (result != GameInteractionEffectQueryResult::Possible) {
        return result;
    }

    _Apply();
    return result;
}

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
    void AddHeartContainer::_Apply() {
        GameInteractor::RawAction::AddOrRemoveHealthContainers(1);
    }

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
    void RemoveHeartContainer::_Apply() {
        GameInteractor::RawAction::AddOrRemoveHealthContainers(-1);
    }

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
    void FillMagic::_Apply() {
        GameInteractor::RawAction::AddOrRemoveMagic(96);
    }

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
    void EmptyMagic::_Apply() {
        GameInteractor::RawAction::AddOrRemoveMagic(-96);
    }

    // MARK: - GiveRupees
    GameInteractionEffectQueryResult GiveRupees::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void GiveRupees::_Apply() {
        Rupees_ChangeBy(parameter);
    }

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
    void TakeRupees::_Apply() {
        Rupees_ChangeBy(-parameter);
    }

    // MARK: - NoUI
    GameInteractionEffectQueryResult NoUI::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void NoUI::_Apply() {
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
    void HighGravity::_Apply() {
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
    void LowGravity::_Apply() {
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
    void GiveHealth::_Apply() {
        GameInteractor::RawAction::HealOrDamagePlayer(parameter);
    }

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
    void TakeHealth::_Apply() {
        GameInteractor::RawAction::HealOrDamagePlayer(-parameter);
    }

    // MARK: - SetPlayerHealth
    GameInteractionEffectQueryResult SetPlayerHealth::CanBeApplied() {
        Player* player = GET_PLAYER(gPlayState);
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void SetPlayerHealth::_Apply() {
        GameInteractor::RawAction::SetPlayerHealth(parameter);
    }

    // MARK: - FreezePlayer
    GameInteractionEffectQueryResult FreezePlayer::CanBeApplied() {
        Player* player = GET_PLAYER(gPlayState);
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void FreezePlayer::_Apply() {
        GameInteractor::RawAction::FreezePlayer();
    }

    // MARK: - BurnPlayer
    GameInteractionEffectQueryResult BurnPlayer::CanBeApplied() {
        Player* player = GET_PLAYER(gPlayState);
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused() || !PlayerGrounded(player)) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void BurnPlayer::_Apply() {
        GameInteractor::RawAction::BurnPlayer();
    }

    // MARK: - ElectrocutePlayer
    GameInteractionEffectQueryResult ElectrocutePlayer::CanBeApplied() {
        Player* player = GET_PLAYER(gPlayState);
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused() || !PlayerGrounded(player)) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void ElectrocutePlayer::_Apply() {
        GameInteractor::RawAction::ElectrocutePlayer();
    }

    // MARK: - KnockbackPlayer
    GameInteractionEffectQueryResult KnockbackPlayer::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void KnockbackPlayer::_Apply() {
        GameInteractor::RawAction::KnockbackPlayer(parameter);
    }

    // MARK: - GiantLink
    GameInteractionEffectQueryResult GiantLink::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void GiantLink::_Apply() {
        GameInteractor::RawAction::SetLinkSize(GI_LINK_SIZE_GIANT);
    }
    void GiantLink::Remove() { 
        GameInteractor::RawAction::SetLinkSize(GI_LINK_SIZE_NORMAL);
    }

    // MARK: - MinishLink
    GameInteractionEffectQueryResult MinishLink::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void MinishLink::_Apply() {
        GameInteractor::RawAction::SetLinkSize(GI_LINK_SIZE_MINISH);
    }
    void MinishLink::Remove() {
        GameInteractor::RawAction::SetLinkSize(GI_LINK_SIZE_NORMAL);
    }

    // MARK: - PaperLink
    GameInteractionEffectQueryResult PaperLink::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void PaperLink::_Apply() {
        GameInteractor::RawAction::SetLinkSize(GI_LINK_SIZE_PAPER);
    }
    void PaperLink::Remove() {
        GameInteractor::RawAction::SetLinkSize(GI_LINK_SIZE_NORMAL);
    }

    // MARK: - InvisibleLink
    GameInteractionEffectQueryResult InvisibleLink::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void InvisibleLink::_Apply() {
        GameInteractor::RawAction::SetLinkInvisibility(1);
    }
    void InvisibleLink::Remove() {
        GameInteractor::RawAction::SetLinkInvisibility(0);
    }

    // MARK: - PacifistMode
    GameInteractionEffectQueryResult PacifistMode::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void PacifistMode::_Apply() {
        GameInteractor::RawAction::SetPacifistMode(1);
    }
    void PacifistMode::Remove() {
        GameInteractor::RawAction::SetPacifistMode(0);
    }

    // MARK: - DisableZTargeting
    GameInteractionEffectQueryResult DisableZTargeting::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void DisableZTargeting::_Apply() {
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
    void WeatherRainstorm::_Apply() {
        GameInteractor::RawAction::SetWeatherStorm(1);
    }
    void WeatherRainstorm::Remove() {
        GameInteractor::RawAction::SetWeatherStorm(0);
    }

    // MARK: - ReverseControls
    GameInteractionEffectQueryResult ReverseControls::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void ReverseControls::_Apply() {
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
    void ForceIronBoots::_Apply() {
        GameInteractor::RawAction::ForceEquipBoots(PLAYER_BOOTS_IRON);
    }
    void ForceIronBoots::Remove() {
        GameInteractor::RawAction::ForceEquipBoots(PLAYER_BOOTS_KOKIRI);
    }

    // MARK: - ForceHoverBoots
    GameInteractionEffectQueryResult ForceHoverBoots::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void ForceHoverBoots::_Apply() {
        GameInteractor::RawAction::ForceEquipBoots(PLAYER_BOOTS_HOVER);
    }
    void ForceHoverBoots::Remove() {
        GameInteractor::RawAction::ForceEquipBoots(PLAYER_BOOTS_KOKIRI);
    }

    // MARK: - IncreaseRunSpeed
    GameInteractionEffectQueryResult IncreaseRunSpeed::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void IncreaseRunSpeed::_Apply() {
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
    void DecreaseRunSpeed::_Apply() {
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
    void OneHitKO::_Apply() {
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
    void IncreaseDamageTaken::_Apply() {
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
    void DecreaseDamageTaken::_Apply() {
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
    void GiveDekuShield::_Apply() {
        GameInteractor::RawAction::GiveDekuShield();
    }

    // MARK: - SpawnCuccoStorm
    GameInteractionEffectQueryResult SpawnCuccoStorm::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void SpawnCuccoStorm::_Apply() {
        GameInteractor::RawAction::SpawnCuccoStorm();
    }
}
