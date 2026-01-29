#include <soh/OTRGlobals.h>

extern "C" {
#include <variables.h>
}

// RANDOTODO: Port the rest of the behavior for this enhancement here.

void BuildNightGuardMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = CustomMessage("You look bored. Wanna go out for a walk?\x1B%gYes&No%w",
                                      "Du siehst gelangweilt aus. Willst Du einen Spaziergang machen?\x1B%gJa&Nein%w",
                                      "Tu as l'air de t'ennuyer. Tu veux aller faire un tour?\x1B%gOui&Non%w");
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void MarketSneak_Register() {
    COND_ID_HOOK(OnOpenText, TEXT_MARKET_GUARD_NIGHT, CVarGetInteger(CVAR_ENHANCEMENT("MarketSneak"), 0),
                 BuildNightGuardMessage);
}

static RegisterShipInitFunc initFunc(MarketSneak_Register, { CVAR_ENHANCEMENT("MarketSneak") });