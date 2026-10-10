#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "variables.h"
#include "z64snap.h"
s32 Snap_RecordPictographedActors(PlayState* play);
}

#define CVAR_NAME "gEnhancements.Equipment.BetterPictoMessage"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterBetterPictoMessage() {
    COND_ID_HOOK(OnOpenText, 0xF8, CVAR, [](u16* textId, bool* loadFromMessageTable) {
        if (!CHECK_QUEST_ITEM(QUEST_PICTOGRAPH)) {
            Snap_RecordPictographedActors(gPlayState);
        }

        std::string target = "";

        std::vector<std::string> actorsInPicture;
        if (Snap_CheckFlag(PICTO_VALID_IN_SWAMP))
            target = "the Swamp";
        if (Snap_CheckFlag(PICTO_VALID_MONKEY))
            target = "a Monkey";
        if (Snap_CheckFlag(PICTO_VALID_BIG_OCTO))
            target = "a Big Octo";
        if (Snap_CheckFlag(PICTO_VALID_LULU_HEAD) && Snap_CheckFlag(PICTO_VALID_LULU_RIGHT_ARM) &&
            Snap_CheckFlag(PICTO_VALID_LULU_LEFT_ARM)) {
            target = "Lulu";
        }
        if (Snap_CheckFlag(PICTO_VALID_SCARECROW))
            target = "a Scarecrow";
        if (Snap_CheckFlag(PICTO_VALID_TINGLE))
            target = "Tingle";
        if (Snap_CheckFlag(PICTO_VALID_PIRATE_GOOD))
            target = "a Pirate";
        if (Snap_CheckFlag(PICTO_VALID_DEKU_KING))
            target = "the Deku King";

        if (target == "") {
            return;
        }

        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "Keep this %rpicture of {{target}}%w?%g\n\xC2Yes\nNo";

        CustomMessage::Replace(&entry.msg, "{{target}}", target);

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterBetterPictoMessage, { CVAR_NAME });
