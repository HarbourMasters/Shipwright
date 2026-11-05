#include "soh/Enhancements/custom-message/MessageCatalog.h"
#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/custom-message/CustomMessageManager.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

extern "C" {
#include <variables.h>
}

// RANDOTODO: Port the rest of the behavior associated with this option here.

void BuildShopDescMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = CustomMessages::CreateMessage("enhancements.better_bombchu_shopping.build_shop_desc_message.0");
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildShopPromptMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg =
        CustomMessages::CreateMessage("enhancements.better_bombchu_shopping.build_shop_prompt_message.0");
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BetterBombchuShopping_Register() {
    COND_ID_HOOK(OnOpenText, TEXT_BUY_BOMBCHUS_10_DESC,
                 IS_RANDO || CVarGetInteger(CVAR_ENHANCEMENT("BetterBombchuShopping"), 0), BuildShopDescMessage);
    COND_ID_HOOK(OnOpenText, TEXT_BUY_BOMBCHUS_10_PROMPT,
                 IS_RANDO || CVarGetInteger(CVAR_ENHANCEMENT("BetterBombchuShopping"), 0), BuildShopPromptMessage);
}

static RegisterShipInitFunc initFunc(BetterBombchuShopping_Register,
                                     { "IS_RANDO", CVAR_ENHANCEMENT("BetterBombchuShopping") });
