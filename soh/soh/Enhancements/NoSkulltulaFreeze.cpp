#include <soh/OTRGlobals.h>

extern "C" {
#include "variables.h"
}

#define RAND_GET_OPTION(rsk) OTRGlobals::Instance->gRandoContext->GetOption(rsk)

void AutoDismissSkulltulaMessage(uint16_t* textId, bool* loadFromMessageTable) {
    if (IS_RANDO && RAND_GET_OPTION(RSK_SHUFFLE_TOKENS).IsNot(RO_TOKENSANITY_OFF)) {
        *loadFromMessageTable = true;
        return;
    }
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

static RegisterShipInitFunc initFunc(NoSkulltulaFreeze_Register, { CVAR_ENHANCEMENT("SkulltulaFreeze") });