// RANDOTODO: Consider breaking these up by actor similar
// to how 2Ship does ActorBehavior stuff.
/**
 * This file is intended to handle some rando-specific
 * text changes that don't really fit anywhere else.
 */

#include <soh/OTRGlobals.h>

extern "C" {
#include <variables.h>
}

void BuildWaterSwitchMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg;
    if (*textId == TEXT_LAKE_HYLIA_WATER_SWITCH_NAVI) {
        msg = CustomMessage("%cThis switch is rustier than you think.^%cSomething must be wrong with the pipe system "
                            "in the %bWater Temple%c.",
                            "%cDieser Schalter scheint rostiger zu sein als er aussieht.^%cEtwas muss mit dem "
                            "Leitungssystem im %bWassertempel%c nicht stimmen.",
                            "%cCet interrupteur est très rouillé.^%cIl doit y avoir un problème avec la tuyauterie du "
                            "%bTemple de l'Eau%c.");
    } else if (*textId == TEXT_LAKE_HYLIA_WATER_SWITCH_SIGN) {
        msg = CustomMessage("Water level control system.&Keep away!", "Wasserstand Kontrollsystem&Finger weg!",
                            "Système de contrôle du niveau&d'eau.&Ne pas toucher!");
    }
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildShootingGalleryNoBowMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg =
        CustomMessage("Come back when you have your own bow and you'll get a %rdifferent prize%w!",
                      "Komm wieder sobald Du Deinen eigenen Bogen hast, um einen %rspeziellen Preis%w zu erhalten!",
                      "J'aurai %rune autre récompense%w pour toi lorsque tu auras ton propre arc.");
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void RegisterMiscellaneousMessages() {
    COND_ID_HOOK(OnOpenText, TEXT_LAKE_HYLIA_WATER_SWITCH_NAVI, IS_RANDO, BuildWaterSwitchMessage);
    COND_ID_HOOK(OnOpenText, TEXT_LAKE_HYLIA_WATER_SWITCH_SIGN, IS_RANDO, BuildWaterSwitchMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SHOOTING_GALLERY_MAN_COME_BACK_WITH_BOW, IS_RANDO, BuildShootingGalleryNoBowMessage);
}

static RegisterShipInitFunc initFunc(RegisterMiscellaneousMessages, { "IS_RANDO" });