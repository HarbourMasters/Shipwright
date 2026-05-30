#include <soh/OTRGlobals.h>
#include "soh/Enhancements/randomizer/randomizer.h"

extern "C" {
#include "variables.h"
}

void AutoDismissSkulltulaMessage(IEvent* event) {
    OnOpenText* ev = reinterpret_cast<OnOpenText*>(event);
    if (IS_RANDO && RAND_GET_OPTION(RSK_SHUFFLE_TOKENS)) {
        *ev->loadFromMessageTable = true;
        return;
    }
    *ev->loadFromMessageTable = false;
    CustomMessage msg = CustomMessage::LoadVanillaMessageTableEntry(TEXT_GS_FREEZE);
    msg.Replace(CustomMessage::MESSAGE_END(), "\x0E\x3C");
    msg += CustomMessage::MESSAGE_END();
    msg.LoadIntoFont();
}

void NoSkulltulaFreeze_Register() {
    COND_ID_HOOK(OnOpenText, TEXT_GS_FREEZE,
                 CVarGetInteger(CVAR_ENHANCEMENT("SkulltulaFreeze"), 0) &&
                     CVarGetInteger(CVAR_ENHANCEMENT("InjectItemCounts.GoldSkulltula"), 0) == 0,
                 AutoDismissSkulltulaMessage);
    COND_ID_HOOK(OnOpenText, TEXT_GS_NO_FREEZE,
                 CVarGetInteger(CVAR_ENHANCEMENT("SkulltulaFreeze"), 0) &&
                     CVarGetInteger(CVAR_ENHANCEMENT("InjectItemCounts.GoldSkulltula"), 0) == 0,
                 AutoDismissSkulltulaMessage);
}

static RegisterShipInitFunc initFunc(NoSkulltulaFreeze_Register, { CVAR_ENHANCEMENT("SkulltulaFreeze") });