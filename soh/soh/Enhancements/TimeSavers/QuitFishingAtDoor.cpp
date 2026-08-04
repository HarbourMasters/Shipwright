#include <soh/OTRGlobals.h>

extern "C" {
#include <variables.h>
}

// TODO: Port the rest of the behavior for this enhancement here.

void BuildQuitFishingMessage(uint16_t* textId, bool* loadFromMessageTable) {
    // TODO: See about loading the vanilla message and manipulating that instead of
    // a brand new one. Might not be worth it.
    CustomMessage msg = CustomMessage(
        "Hey! Hey!&You can't take the rod out of here!&I'm serious!^Do you want to quit?\x1B%gYes&No%w",
        "Hey! Hey!&Du kannst die Angel doch nicht&einfach mitnehmen!&Ganz im Ernst!^Möchtest Du "
        "aufhören?\x1B%gJa&Nein%w",
        "Holà! Holà!&Les cannes ne sortent pas d'ici!&Je suis sérieux!^Voulez-vous arrêter?\x1B%gOui&Non%w");
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void QuitFishingAtDoor_Register() {
    COND_ID_HOOK(OnOpenText, TEXT_FISHERMAN_LEAVE, CVarGetInteger(CVAR_ENHANCEMENT("QuitFishingAtDoor"), 0),
                 BuildQuitFishingMessage);
}

static RegisterShipInitFunc initFunc(QuitFishingAtDoor_Register, { CVAR_ENHANCEMENT("QuitFishingAtDoor") });