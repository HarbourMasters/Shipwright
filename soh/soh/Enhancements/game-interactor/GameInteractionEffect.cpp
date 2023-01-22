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
    GameInteractionEffectQueryResult AddHeartContainer::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::AddOrRemoveHealthContainers(1);
        return result;
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
    GameInteractionEffectQueryResult RemoveHeartContainer::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::AddOrRemoveHealthContainers(-1);
        return result;
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
    GameInteractionEffectQueryResult FillMagic::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::AddOrRemoveMagic(96);
        return result;
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
    GameInteractionEffectQueryResult EmptyMagic::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::AddOrRemoveMagic(-96);
        return result;
    }

    // MARK: - GiveRupees
    GameInteractionEffectQueryResult GiveRupees::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    GameInteractionEffectQueryResult GiveRupees::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        Rupees_ChangeBy(parameter);
        return result;
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
    GameInteractionEffectQueryResult TakeRupees::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        Rupees_ChangeBy(-parameter);
        return result;
    }

    // MARK: - NoUI
    GameInteractionEffectQueryResult NoUI::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    GameInteractionEffectQueryResult NoUI::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor_NoUIActive = 1;
        return result;
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
    GameInteractionEffectQueryResult HighGravity::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor_GravityLevel = GRAVITY_LEVEL_HEAVY;
        return result;
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
    GameInteractionEffectQueryResult LowGravity::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor_GravityLevel = GRAVITY_LEVEL_LIGHT;
        return result;
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
    GameInteractionEffectQueryResult GiveHealth::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::HealOrDamagePlayer(parameter);
        return result;
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
    GameInteractionEffectQueryResult TakeHealth::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::HealOrDamagePlayer(-parameter);
        return result;
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
    GameInteractionEffectQueryResult SetPlayerHealth::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::SetPlayerHealth(parameter);
        return result;
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
    GameInteractionEffectQueryResult FreezePlayer::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::FreezePlayer();
        return result;
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
    GameInteractionEffectQueryResult BurnPlayer::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::BurnPlayer();
        return result;
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
    GameInteractionEffectQueryResult ElectrocutePlayer::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::ElectrocutePlayer();
        return result;
    }

    // MARK: - KnockbackPlayer
    GameInteractionEffectQueryResult KnockbackPlayer::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    GameInteractionEffectQueryResult KnockbackPlayer::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::KnockbackPlayer(parameter);
        return result;
    }

    // MARK: - GiantLink
    GameInteractionEffectQueryResult GiantLink::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    GameInteractionEffectQueryResult GiantLink::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::SetLinkSize(GI_LINK_SIZE_GIANT);
        return result;
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
    GameInteractionEffectQueryResult MinishLink::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::SetLinkSize(GI_LINK_SIZE_MINISH);
        return result;
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
    GameInteractionEffectQueryResult PaperLink::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::SetLinkSize(GI_LINK_SIZE_PAPER);
        return result;
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
    GameInteractionEffectQueryResult InvisibleLink::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::SetLinkInvisibility(1);
        return result;
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
    GameInteractionEffectQueryResult PacifistMode::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::SetPacifistMode(1);
        return result;
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
    GameInteractionEffectQueryResult DisableZTargeting::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor_DisableZTargetingActive = 1;
        return result;
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
    GameInteractionEffectQueryResult WeatherRainstorm::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::SetWeatherStorm(1);
        return result;
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
    GameInteractionEffectQueryResult ReverseControls::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor_ReverseControlsActive = 1;
        return result;
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
    GameInteractionEffectQueryResult ForceIronBoots::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::ForceEquipBoots(PLAYER_BOOTS_IRON);
        return result;
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
    GameInteractionEffectQueryResult ForceHoverBoots::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::ForceEquipBoots(PLAYER_BOOTS_HOVER);
        return result;
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
    GameInteractionEffectQueryResult IncreaseRunSpeed::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor_RunSpeedModifier = 2;
        return result;
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
    GameInteractionEffectQueryResult DecreaseRunSpeed::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor_RunSpeedModifier = -2;
        return result;
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
    GameInteractionEffectQueryResult OneHitKO::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor_OneHitKOActive = 1;
        return result;
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
    GameInteractionEffectQueryResult IncreaseDamageTaken::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor_DefenseModifier = -parameter;
        return result;
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
    GameInteractionEffectQueryResult DecreaseDamageTaken::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor_DefenseModifier = parameter;
        return result;
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
    GameInteractionEffectQueryResult GiveDekuShield::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::GiveDekuShield();
        return result;
    }

    // MARK: - SpawnCuccoStorm
    GameInteractionEffectQueryResult SpawnCuccoStorm::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    GameInteractionEffectQueryResult SpawnCuccoStorm::Apply() {
        GameInteractionEffectQueryResult result = CanBeApplied();
        if (result != GameInteractionEffectQueryResult::Possible) {
            return result;
        }
        GameInteractor::Actions::SpawnCuccoStorm();
        return result;
    }
}
