#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/ShipUtils.h"
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "variables.h"
#include "functions.h"

#include "overlays/actors/ovl_En_Elfgrp/z_en_elfgrp.h"
extern void func_80A3A398(EnElfgrp* enElfgrp, PlayState* play);
}

std::map<u8, RandoCheckId> fairyCheckMap = {
    { ENELFGRP_TYPE_POWER, RC_WOODFALL_GREAT_FAIRY },    { ENELFGRP_TYPE_WISDOM, RC_SNOWHEAD_GREAT_FAIRY },
    { ENELFGRP_TYPE_COURAGE, RC_GREAT_BAY_GREAT_FAIRY }, { ENELFGRP_TYPE_KINDNESS, RC_IKANA_GREAT_FAIRY },
    { ENELFGRP_TYPE_MAGIC, RC_CLOCK_TOWN_GREAT_FAIRY },
};

void ApplyClockTownGreatFairyHint(u16* textId, bool* loadFromMessageTable) {
    CustomMessage::Entry entry = {
        .msg = "%wPlease, find the Stray Fairy who's lost! We will reward you with %g{{item1}}%w and maybe "
               "even %g{{item2}}%w if you are worthy."
    };

    auto randoItem1Name = Rando::StaticData::GetItemName(RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY].randoItemId, true,
                                                         RC_CLOCK_TOWN_GREAT_FAIRY);

    CustomMessage::Replace(&entry.msg, "{{item1}}", randoItem1Name);

    auto randoItem2Name = Rando::StaticData::GetItemName(RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY_ALT].randoItemId,
                                                         true, RC_CLOCK_TOWN_GREAT_FAIRY_ALT);

    CustomMessage::Replace(&entry.msg, "{{item2}}", randoItem2Name);

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

void ApplyGreatFairyHint(u16* textId, bool* loadFromMessageTable, RandoCheckId randoCheckId) {
    CustomMessage::Entry entry = {
        .msg = "%wPlease, find the Stray Fairies who match our color! We will reward you with %g{{itemName}}%w."
    };
    CustomMessage::Replace(
        &entry.msg, "{{itemName}}",
        Rando::StaticData::GetItemName(RANDO_SAVE_CHECKS[randoCheckId].randoItemId, true, randoCheckId));

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

// Handles the Great Fairy checks
void Rando::ActorBehavior::InitEnElfgrpBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_STRAY_FAIRY_MANAGER, IS_RANDO, {
        *should = false;

        EnElfgrp* elfgrp = va_arg(args, EnElfgrp*);

        RandoCheckId randoCheckId = fairyCheckMap.at(elfgrp->type);

        if (!RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            RANDO_SAVE_CHECKS[randoCheckId].eligible = true;
        }

        if (elfgrp->type == ENELFGRP_TYPE_MAGIC) {
            // In the game this uses an `else`, but in rando we are okay with both of these happening at the same
            // time
            if ((INV_CONTENT(ITEM_MASK_DEKU) == ITEM_MASK_DEKU || INV_CONTENT(ITEM_MASK_ZORA) == ITEM_MASK_ZORA ||
                 INV_CONTENT(ITEM_MASK_GORON) == ITEM_MASK_GORON) &&
                !RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY_ALT].cycleObtained) {
                RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY_ALT].eligible = true;
            }
        }
    });

    // Use the RO Stray Fairy minimum threshold rather than the vanilla 15
    COND_ID_HOOK(OnActorInit, ACTOR_EN_ELFGRP, IS_RANDO, [](Actor* actor) {
        EnElfgrp* enElfgrp = (EnElfgrp*)actor;
        // Exclude the Clock Town fairy, and do not do more than once at a time
        if (enElfgrp->type != ENELFGRP_TYPE_MAGIC &&
            gSaveContext.save.saveInfo.inventory.strayFairies[enElfgrp->type - 1] >=
                RANDO_SAVE_OPTIONS[RO_STRAY_FAIRIES_REQUIRED] &&
            !RANDO_SAVE_CHECKS[fairyCheckMap.at(enElfgrp->type)].eligible) {
            enElfgrp->actionFunc = func_80A3A398;
        }
    });

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_GREAT_FAIRY, IS_RANDO, { *should = false; });

    COND_VB_SHOULD(VB_GREAT_FAIRY_GIVE_DOUBLE_DEFENSE_HEARTS, IS_RANDO, { *should = false; });

    COND_ID_HOOK(OnOpenText, 0x578, IS_RANDO, ApplyClockTownGreatFairyHint);
    COND_ID_HOOK(OnOpenText, 0x580, IS_RANDO, ApplyClockTownGreatFairyHint);

    COND_ID_HOOK(OnOpenText, 0x582, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        ApplyGreatFairyHint(textId, loadFromMessageTable, RC_WOODFALL_GREAT_FAIRY);
    });
    COND_ID_HOOK(OnOpenText, 0x583, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        ApplyGreatFairyHint(textId, loadFromMessageTable, RC_WOODFALL_GREAT_FAIRY);
    });
    COND_ID_HOOK(OnOpenText, 0x585, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        ApplyGreatFairyHint(textId, loadFromMessageTable, RC_SNOWHEAD_GREAT_FAIRY);
    });
    COND_ID_HOOK(OnOpenText, 0x586, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        ApplyGreatFairyHint(textId, loadFromMessageTable, RC_SNOWHEAD_GREAT_FAIRY);
    });
    COND_ID_HOOK(OnOpenText, 0x588, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        ApplyGreatFairyHint(textId, loadFromMessageTable, RC_GREAT_BAY_GREAT_FAIRY);
    });
    COND_ID_HOOK(OnOpenText, 0x589, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        ApplyGreatFairyHint(textId, loadFromMessageTable, RC_GREAT_BAY_GREAT_FAIRY);
    });
    COND_ID_HOOK(OnOpenText, 0x58B, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        ApplyGreatFairyHint(textId, loadFromMessageTable, RC_IKANA_GREAT_FAIRY);
    });
    COND_ID_HOOK(OnOpenText, 0x58C, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        ApplyGreatFairyHint(textId, loadFromMessageTable, RC_IKANA_GREAT_FAIRY);
    });
}
