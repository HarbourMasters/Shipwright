#include "soh/Enhancements/custom-message/MessageCatalog.h"
// RANDOTODO: Consider breaking these up by actor similar
// to how 2Ship does ActorBehavior stuff.
/**
 * This file is intended to handle some rando-specific
 * text changes that don't really fit anywhere else.
 */

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/randomizer.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include <variables.h>
extern PlayState* gPlayState;
}

void BuildWaterSwitchMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg;
    if (*textId == TEXT_LAKE_HYLIA_WATER_SWITCH_NAVI) {
        msg = CustomMessages::CreateMessage("randomizer.messages.miscellaneous.build_water_switch_message.0");
    } else if (*textId == TEXT_LAKE_HYLIA_WATER_SWITCH_SIGN) {
        msg = CustomMessages::CreateMessage("randomizer.messages.miscellaneous.build_water_switch_message.1");
    }
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildShootingGalleryNoBowMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg =
        CustomMessages::CreateMessage("randomizer.messages.miscellaneous.build_shooting_gallery_no_bow_message.0");
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildFixedMallonAtCastleMessage(uint16_t* textId, bool* loadFromMessageTable) {
    if (gPlayState->sceneNum == SCENE_HYRULE_CASTLE) {
        CustomMessage msg =
            CustomMessages::CreateMessage("randomizer.messages.miscellaneous.build_fixed_mallon_at_castle_message.0");
        msg.AutoFormat();
        msg.LoadIntoFont();
        *loadFromMessageTable = false;
    }
}

void BuildGerudoGuardJailOfferMessage(uint16_t* textId, bool* loadFromMessageTable) {
    Player* player = GET_PLAYER(gPlayState);

    if (gPlayState->sceneNum == SCENE_GERUDOS_FORTRESS && player->talkActor != NULL &&
        player->talkActor->id == ACTOR_EN_GE2) {
        CustomMessage msg = CustomMessages::CreateMessage(
            "randomizer.messages.miscellaneous.build_gerudo_guard_jail_offer_message.0", { QM_GREEN });
        msg.AutoFormat();
        msg.LoadIntoFont();
        *loadFromMessageTable = false;
    }
}

void RegisterMiscellaneousMessages() {
    COND_ID_HOOK(OnOpenText, TEXT_LAKE_HYLIA_WATER_SWITCH_NAVI, IS_RANDO, BuildWaterSwitchMessage);
    COND_ID_HOOK(OnOpenText, TEXT_LAKE_HYLIA_WATER_SWITCH_SIGN, IS_RANDO, BuildWaterSwitchMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SHOOTING_GALLERY_MAN_COME_BACK_WITH_BOW, IS_RANDO, BuildShootingGalleryNoBowMessage);
    COND_ID_HOOK(OnOpenText, TEXT_MALON_MEET_EPONA, IS_RANDO, BuildFixedMallonAtCastleMessage);
    COND_ID_HOOK(OnOpenText, TEXT_MALON_EPONA_IS_AFRAID, IS_RANDO, BuildFixedMallonAtCastleMessage);
    COND_ID_HOOK(OnOpenText, TEXT_MALON_LETS_SING_THIS_SONG, IS_RANDO, BuildFixedMallonAtCastleMessage);
    COND_ID_HOOK(OnOpenText, TEXT_GERUDO_GUARD_FRIENDLY, IS_RANDO, BuildGerudoGuardJailOfferMessage);
}

static RegisterShipInitFunc initFunc(RegisterMiscellaneousMessages, { "IS_RANDO" });