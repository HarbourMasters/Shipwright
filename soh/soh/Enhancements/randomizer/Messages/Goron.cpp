#include "soh/Enhancements/custom-message/MessageCatalog.h"
/**
 * Handles the randomized silly messages that the Fire Temple
 * trapped Gorons have when you free them.
 */

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/custom-message/CustomMessageManager.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/ShipInit.hpp"
#include "soh/ShipUtils.h"

extern "C" {
#include <variables.h>
}

static auto& GetFireTempleGoronMessages() {
    static CustomMessage FireTempleGoronMessages[] = {
        CustomMessages::CreateMessage("randomizer.messages.goron.messages.0"),
        CustomMessages::CreateMessage("randomizer.messages.goron.messages.1"),
        CustomMessages::CreateMessage("randomizer.messages.goron.messages.2"),
        CustomMessages::CreateMessage("randomizer.messages.goron.messages.3"),
        CustomMessages::CreateMessage("randomizer.messages.goron.messages.4"),
        CustomMessages::CreateMessage("randomizer.messages.goron.messages.5"),
        CustomMessages::CreateMessage("randomizer.messages.goron.messages.6"),
        CustomMessages::CreateMessage("randomizer.messages.goron.messages.7"),
        CustomMessages::CreateMessage("randomizer.messages.goron.messages.8"),
        CustomMessages::CreateMessage("randomizer.messages.goron.messages.9"),
        CustomMessages::CreateMessage("randomizer.messages.goron.messages.10"),
    };
    return FireTempleGoronMessages;
}

void BuildGoronMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = ShipUtils::RandomElement(GetFireTempleGoronMessages());
    msg.Replace("[[days]]", std::to_string(gSaveContext.totalDays));
    msg.Replace("[[a_btn]]", std::to_string(gSaveContext.ship.stats.count[COUNT_BUTTON_PRESSES_A]));
    msg.Format();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void RegisterGoronMessages() {
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_OWE_YOU_BIG_TIME, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_FALLING_DOORS_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_FIRE_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_FLAME_DANCER_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_SWITCH_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_FLAME_DANCER_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_SWITCH_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_OCARINA_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_PILLAR_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_HIDDEN_DOOR_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_SOUNDS_DIFFERENT_SECRET, IS_RANDO, BuildGoronMessage);
}

static RegisterShipInitFunc initFunc(RegisterGoronMessages, { "IS_RANDO" });