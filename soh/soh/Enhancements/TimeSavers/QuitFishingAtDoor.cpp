#include "soh/Enhancements/custom-message/MessageCatalog.h"
#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/custom-message/CustomMessageManager.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

// TODO: Port the rest of the behavior for this enhancement here.

void BuildQuitFishingMessage(uint16_t* textId, bool* loadFromMessageTable) {
    // TODO: See about loading the vanilla message and manipulating that instead of
    // a brand new one. Might not be worth it.
    CustomMessage msg = CustomMessages::CreateMessage("enhancements.quit_fishing_at_door.build_quit_fishing_message.0");
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void QuitFishingAtDoor_Register() {
    COND_ID_HOOK(OnOpenText, TEXT_FISHERMAN_LEAVE, CVarGetInteger(CVAR_ENHANCEMENT("QuitFishingAtDoor"), 0),
                 BuildQuitFishingMessage);

    COND_VB_SHOULD(VB_FISHING_QUIT_AT_DOOR, CVarGetInteger(CVAR_ENHANCEMENT("QuitFishingAtDoor"), 0),
                   { *should = true; });
}

static RegisterShipInitFunc initFunc(QuitFishingAtDoor_Register, { CVAR_ENHANCEMENT("QuitFishingAtDoor") });