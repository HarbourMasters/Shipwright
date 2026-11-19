#include "soh/Enhancements/custom-message/MessageCatalog.h"
/**
 * This file is responsible for the messages
 * for the Rando-Relevant Navi Hints enhancement.
 */

#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/custom-message/CustomMessageManager.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/ShipInit.hpp"
#include "soh/ShipUtils.h"

extern "C" {
#include <variables.h>
}

static auto& GetNaviMessages() {
    static CustomMessage NaviMessages[] = {

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.0"),

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.1"),

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.2"),

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.3"),

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.4"),

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.5"),

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.6"),

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.7"),

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.8"),

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.9"),

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.10"),

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.11"),

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.12"),

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.13"),

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.14"),

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.15"),

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.16"),

        CustomMessages::CreateMessage("randomizer.messages.navi.messages.17")
    };
    return NaviMessages;
}

void BuildNaviMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = ShipUtils::RandomElement(GetNaviMessages());
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void RegisterNaviMessages() {
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_DEKU_TREE_SUMMONS,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_CMON_BE_BRAVE,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_VISIT_THE_PRINCESS,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_FIND_MALONS_FATHER,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_FIND_THE_PRINCESS,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_WHAT_WOULD_SARIA_SAY,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_IMPA_SAID_DEATH_MOUNTAIN,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_USE_BOMB_FLOWER,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_FAIRY_LIVES_ON_DEATH_MOUNTAIN,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_SARIA_KNOWS_ABOUT_STONES,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_RUTO_INSIDE_JABUS_BELLY,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_COLLECTED_THREE_STONES,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_THREW_SOMETHING_IN_MOAT,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_CHECK_TEMPLE_OF_TIME,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_SHOULD_WE_BELIEVE_SHEIK,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_WHATS_GOING_ON_IN_FOREST,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_CLOUD_OVER_DEATH_MOUNTAIN,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_ARTIC_WIND_IS_BLOWING,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_IRON_BOOTS_WEIGH_A_TON,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_LOOK_FOR_SOMONE_WHO_KNOWS,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_IT_CAME_OUT_OF_THE_WELL,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_WHO_BUILT_THE_SPIRIT_TEMPLE,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_HAVE_YOU_EVER_PLAYED_NOCTURNE,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_WHERE_GANONDORF_WAS_BORN,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_EQUIP_THE_SILVER_GAUNTLETS,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_WHO_IS_WAITING_FOR_US,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_SAVE_PRINCESS_ZELDA,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_TRY_TO_KEEP_MOVING,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
}

static RegisterShipInitFunc initFunc(RegisterNaviMessages,
                                     { CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), "IS_RANDO" });