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

GameInteractionEffectQueryResult GameInteractionEffectBase::Apply() {
    GameInteractionEffectQueryResult result = CanBeApplied();
    if (result != GameInteractionEffectQueryResult::Possible) {
        return result;
    }

    _Apply();
    return result;
}

/// For most effects, CanBeRemoved is the same as CanBeApplied. When its not: please override `CanBeRemoved`.
GameInteractionEffectQueryResult GameInteractionEffectBase::CanBeRemoved() {
    return CanBeApplied();
}

GameInteractionEffectQueryResult GameInteractionEffectBase::Remove() {
    GameInteractionEffectQueryResult result = CanBeRemoved();
    if (result != GameInteractionEffectQueryResult::Possible) {
        return result;
    }

    _Remove();
    return result;
}

namespace GameInteractionEffect {

    // MARK: - ModifyHeartContainers
    GameInteractionEffectQueryResult ModifyHeartContainers::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else if (
                (parameter > 0 && (gSaveContext.healthCapacity + (parameter * 0x10) > 0x140)) ||
                (parameter < 0 && (gSaveContext.healthCapacity + (parameter * 0x10) < 0x10))
            ) {
            return GameInteractionEffectQueryResult::NotPossible;
        }

        return GameInteractionEffectQueryResult::Possible;
    }

    void ModifyHeartContainers::_Apply() {
        GameInteractor::RawAction::AddOrRemoveHealthContainers(parameter);
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

    // MARK: - ModifyRupees
    GameInteractionEffectQueryResult ModifyRupees::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else if (
            (parameter < 0 && gSaveContext.rupees <= 0) ||
            (parameter > 0 && gSaveContext.rupees >= CUR_CAPACITY(UPG_WALLET))
        ) {
            return GameInteractionEffectQueryResult::NotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void ModifyRupees::_Apply() {
        Rupees_ChangeBy(parameter);
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
        GameInteractor::State::NoUIActive = 1;
    }
    void NoUI::_Remove() {
        GameInteractor::State::NoUIActive = 0;
    }

    // MARK: - ModifyGravity
    GameInteractionEffectQueryResult ModifyGravity::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void ModifyGravity::_Apply() {
        GameInteractor::State::GravityLevel = (GIGravityLevel)parameter;
    }
    void ModifyGravity::_Remove() {
        GameInteractor::State::GravityLevel = GI_GRAVITY_LEVEL_NORMAL;
    }

    // MARK: - ModifyHealth
    GameInteractionEffectQueryResult ModifyHealth::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else if (
            (parameter > 0 && gSaveContext.health == gSaveContext.healthCapacity)
            || (parameter < 0 && (gSaveContext.health + (16 * parameter) <= 0))
        ) {
            return GameInteractionEffectQueryResult::NotPossible;
        }

        return GameInteractionEffectQueryResult::Possible;
    }
    void ModifyHealth::_Apply() {
        GameInteractor::RawAction::HealOrDamagePlayer(parameter);
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

    // MARK: - ModifyLinkSize
    GameInteractionEffectQueryResult ModifyLinkSize::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void ModifyLinkSize::_Apply() {
        GameInteractor::State::LinkSize = (GILinkSize)parameter;
    }
    void ModifyLinkSize::_Remove() {
        GameInteractor::State::LinkSize = GI_LINK_SIZE_RESET;
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
        GameInteractor::RawAction::SetLinkInvisibility(true);
    }
    void InvisibleLink::_Remove() {
        GameInteractor::RawAction::SetLinkInvisibility(false);
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
        GameInteractor::State::SetPacifistMode(true);
    }
    void PacifistMode::_Remove() {
        GameInteractor::State::SetPacifistMode(false);
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
        GameInteractor::State::DisableZTargetingActive = 1;
    }
    void DisableZTargeting::_Remove() {
        GameInteractor::State::DisableZTargetingActive = 0;
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
        GameInteractor::RawAction::SetWeatherStorm(true);
    }
    void WeatherRainstorm::_Remove() {
        GameInteractor::RawAction::SetWeatherStorm(false);
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
        GameInteractor::State::ReverseControlsActive = 1;
    }
    void ReverseControls::_Remove() {
        GameInteractor::State::ReverseControlsActive = 0;
    }

    // MARK: - ForceEquipBoots
    GameInteractionEffectQueryResult ForceEquipBoots::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void ForceEquipBoots::_Apply() {
        GameInteractor::RawAction::ForceEquipBoots(parameter);
    }
    void ForceEquipBoots::_Remove() {
        GameInteractor::RawAction::ForceEquipBoots(PLAYER_BOOTS_KOKIRI);
    }

    // MARK: - ModifyRunSpeedModifier
    GameInteractionEffectQueryResult ModifyRunSpeedModifier::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void ModifyRunSpeedModifier::_Apply() {
        GameInteractor::State::RunSpeedModifier = parameter;
    }
    void ModifyRunSpeedModifier::_Remove() {
        GameInteractor::State::RunSpeedModifier = 0;
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
        GameInteractor::State::OneHitKOActive = 1;
    }
    void OneHitKO::_Remove() {
        GameInteractor::State::OneHitKOActive = 0;
    }

    // MARK: - IncreaseDamageTaken
    GameInteractionEffectQueryResult ModifyDefenseModifier::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void ModifyDefenseModifier::_Apply() {
        GameInteractor::State::DefenseModifier = parameter;
    }
    void ModifyDefenseModifier::_Remove() {
        GameInteractor::State::DefenseModifier = 0;
    }

    // MARK: - GiveDekuShield
    GameInteractionEffectQueryResult GiveDekuShield::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else if ((Item_CheckObtainability(ITEM_SHIELD_DEKU) != ITEM_NONE)) {
            return GameInteractionEffectQueryResult::NotPossible;
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
