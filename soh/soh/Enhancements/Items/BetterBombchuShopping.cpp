#include <soh/OTRGlobals.h>

extern "C" {
#include <variables.h>
}

// RANDOTODO: Port the rest of the behavior associated with this option here.

void BuildShopDescMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg =
        CustomMessage("\x08%rBombchu  10 pieces  99 Rupees&%wThis looks like a toy mouse, but&it's actually a "
                      "self-propelled time&bomb!\x09\x0A",
                      "\x08%rKrabbelmine  10 Stück  99 Rubine&%wDas ist eine praktische Zeitbombe,&die Du als "
                      "Distanzwaffe&einsetzen kannst!\x09\x0A",
                      "\x08%rMissile  10 unités  99 Rubis&%wProfilée comme une souris&mécanique, cette arme est "
                      "&destructrice!!!\x09\x0A");
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildShopPromptMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = CustomMessage("\010Bombchu  10 pieces   99 Rupees\x09&&\x1B%gBuy&Don't buy%w",
                                      "\010Krabbelmine  10 Stück  99 Rubine\x09&&\x1B%gKaufen!&Nicht kaufen!%w",
                                      "\010Missiles  10 unités   99 Rubis\x09&&\x1B%gAcheter&Ne pas acheter%w");
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
