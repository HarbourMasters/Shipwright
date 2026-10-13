#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Items.ExtraPowderKegs"
#define CVAR CVarGetInteger(CVAR_NAME, 0)
#define MAX_POWDER_KEGS 3

void RegisterExtraPowderKegs() {
    // When giving a powder keg, increment ammo instead of setting to 1
    COND_VB_SHOULD(VB_POWDER_KEG_SET_AMMO_ON_GIVE, CVAR, {
        *should = false;
        if (AMMO(ITEM_POWDER_KEG) < MAX_POWDER_KEGS) {
            AMMO(ITEM_POWDER_KEG)++;
        }
    });

    // When clamping ammo, use our max instead of 1
    COND_VB_SHOULD(VB_POWDER_KEG_CAP_AMMO, CVAR, {
        *should = false;
        if (AMMO(ITEM_POWDER_KEG) > MAX_POWDER_KEGS) {
            AMMO(ITEM_POWDER_KEG) = MAX_POWDER_KEGS;
        } else if (AMMO(ITEM_POWDER_KEG) < 0) {
            AMMO(ITEM_POWDER_KEG) = 0;
        }
    });

    // Allow buying more kegs if below max
    COND_VB_SHOULD(VB_POWDER_KEG_CHECK_HAS, CVAR, {
        *should = (AMMO(ITEM_POWDER_KEG) >= MAX_POWDER_KEGS) || (gPlayState->actorCtx.flags & ACTORCTX_FLAG_0);
    });

    // Show green ammo text only when at max capacity (3), not at 1
    COND_VB_SHOULD(VB_POWDER_KEG_AMMO_AT_CAPACITY, CVAR, { *should = (AMMO(ITEM_POWDER_KEG) >= MAX_POWDER_KEGS); });

    // Update Goron dialogue to reflect the higher max carry count
    // '\x1e\x3a\xbb' Goron (Oh) (Mono) SFX
    // '\x1e\x38\xfc' Goron (Oh?) (Pitched) SFX
    // '\x12' Box Break II
    COND_ID_HOOK(OnOpenText, 0x0C87, CVAR, [](u16*, bool* loadFromMessageTable) {
        CustomMessage::Entry entry;
        entry.msg = "\x1e\x3a\xbb%rPowder Kegs%w are extremely\n"
                    "dangerous. You are limited to carrying\n"
                    "%rthree%w at any given time.\n"
                    "\x12If an %rarrow%w hits one, it will %rexplode%w\n"
                    "on the spot, so use caution.";
        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
    COND_ID_HOOK(OnOpenText, 0x0C8B, CVAR, [](u16*, bool* loadFromMessageTable) {
        CustomMessage::Entry entry;
        entry.msg = "You are only allowed to take %rthree\n"
                    "Powder Kegs%w with you. After they've\n"
                    "been used, come back and get more.";
        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
    COND_ID_HOOK(OnOpenText, 0x0673, CVAR, [](u16*, bool* loadFromMessageTable) {
        CustomMessage::Entry entry;
        entry.msg = "\x1e\x38\xfcHold on, you already got %rthree%w!\n"
                    "\x12%rPowder Kegs%w are hazardous\n"
                    "explosives, so you may carry only\n"
                    "%rthree%w at a time!";
        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterExtraPowderKegs, { CVAR_NAME });
