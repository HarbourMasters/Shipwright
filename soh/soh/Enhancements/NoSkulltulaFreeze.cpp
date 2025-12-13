#include <soh/OTRGlobals.h>

extern "C" {
#include "variables.h"
}

void AutoDismissSkulltulaMessage(uint16_t* textId, bool* loadFromMessageTable) {
    *loadFromMessageTable = false;
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

static RegisterShipInitFunc initFunc(NoSkulltulaFreeze_Register,
                                     { CVAR_ENHANCEMENT("SkulltulaFreeze"),
                                       CVAR_ENHANCEMENT("InjectItemCounts.GoldSkulltula") });