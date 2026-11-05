#include "soh/Enhancements/custom-message/MessageCatalog.h"
#include "soh/Enhancements/Lang/TranslationCatalog.h"
/**
 * This file is for handling the Randomize Rupee Names enhancement
 */

#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/custom-message/CustomMessageManager.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/ShipInit.hpp"
#include "soh/ShipUtils.h"

extern "C" {
#include "variables.h"
}

static const std::vector<std::string>& GetEnglishRupeeNames() {
    static const auto names = CustomMessages::GetGameTextPool("en_US", "randomizer.rupees.names");
    return names;
}

static const std::vector<std::string>& GetGermanRupeeNames() {
    static const auto names = CustomMessages::GetGameTextPool("de_DE", "randomizer.rupees.names");
    return names;
}

static const std::vector<std::string>& GetFrenchRupeeNames() {
    static const auto names = CustomMessages::GetGameTextPool("fr_FR", "randomizer.rupees.names");
    return names;
}

void BuildRupeeMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = CustomMessages::CreateMessage("randomizer.messages.rupees.build_rupee_message.0");
    std::string color;
    std::string amount;
    CustomMessage rupee =
        CustomMessage(ShipUtils::RandomElement(GetEnglishRupeeNames()), ShipUtils::RandomElement(GetGermanRupeeNames()),
                      ShipUtils::RandomElement(GetFrenchRupeeNames()));
    for (const auto& locale : CustomMessages::GetGameTextCatalog().GetLanguages()) {
        if (locale != "en_US" && locale != "de_DE" && locale != "fr_FR")
            rupee.SetForLocale(
                locale, ShipUtils::RandomElement(CustomMessages::GetGameTextPool(locale, "randomizer.rupees.names")));
    }
    switch (*textId) {
        case TEXT_BLUE_RUPEE:
            color = "%b";
            amount = "5";
            break;
        case TEXT_RED_RUPEE:
            color = "%r";
            amount = "20";
            break;
        case TEXT_PURPLE_RUPEE:
            color = "%p";
            amount = "50";
            break;
        case TEXT_HUGE_RUPEE:
            color = "%y";
            amount = "200";
            break;
        default:
            assert(false);
            return;
    }
    msg.Replace("[[color]]", color);
    msg.Replace("[[amount]]", amount);
    msg.Replace("[[rupee]]", rupee);
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void RegisterRandomRupeeNames() {
    COND_ID_HOOK(OnOpenText, TEXT_BLUE_RUPEE,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandomizeRupeeNames"), 1), BuildRupeeMessage);
    COND_ID_HOOK(OnOpenText, TEXT_RED_RUPEE,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandomizeRupeeNames"), 1), BuildRupeeMessage);
    COND_ID_HOOK(OnOpenText, TEXT_PURPLE_RUPEE,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandomizeRupeeNames"), 1), BuildRupeeMessage);
    COND_ID_HOOK(OnOpenText, TEXT_HUGE_RUPEE,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandomizeRupeeNames"), 1), BuildRupeeMessage);
}

static RegisterShipInitFunc initFunc(RegisterRandomRupeeNames,
                                     { CVAR_RANDOMIZER_ENHANCEMENT("RandomizeRupeeNames"), "IS_RANDO" });