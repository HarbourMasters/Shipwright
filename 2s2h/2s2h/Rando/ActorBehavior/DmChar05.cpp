#include "ActorBehavior.h"
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "overlays/actors/ovl_Dm_Char05/z_dm_char05.h"
}

// These come from corresponding entries in z_message.c.
#define GORON_MASK_TEXT 0x79
#define ZORA_MASK_TEXT 0x7A
#define GIBDO_MASK_TEXT 0x87
#define COUPLES_MASK_TEXT 0x85

// Replace vanilla item get text with a simple message stating what randomized item has been received
void replaceGetItemText(RandoCheckId randoCheckId, u16* textId, bool* loadFromMessageTable) {
    auto randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
    auto randoStaticItem = Rando::StaticData::Items[randoSaveCheck.randoItemId];

    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    entry.autoFormat = false;
    entry.msg.assign("You received {{itemName}}!\x1C\x02\x10");
    CustomMessage::Replace(&entry.msg, "{{itemName}}",
                           Rando::StaticData::GetItemName(randoSaveCheck.randoItemId, true, randoCheckId));
    entry.icon = 0xFE; // No icon

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

void Rando::ActorBehavior::InitDmChar05Behavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_DMCHAR05, IS_RANDO, {
        ItemId vanillaItemId = (ItemId)va_arg(args, int);
        switch (vanillaItemId) {
            case ITEM_MASK_GIBDO:
                RANDO_SAVE_CHECKS[RC_MUSIC_BOX_HOUSE_FATHER].eligible = true;
                break;
            case ITEM_MASK_GORON:
                RANDO_SAVE_CHECKS[RC_GORON_GRAVEYARD_DARMANI].eligible = true;
                break;
            case ITEM_MASK_ZORA:
                RANDO_SAVE_CHECKS[RC_GREAT_BAY_COAST_MIKAU].eligible = true;
                break;
            case ITEM_MASK_COUPLE:
                RANDO_SAVE_CHECKS[RC_STOCK_POT_INN_COUPLES_MASK].eligible = true;
                break;
            default:
                break;
        }
        *should = false;
    });

    /*
     * This only affects the "Get Item" model displayed above Link's head. Other instances of the items in question,
     * such as the Goron Mask when it falls to the ground, still displays that vanilla model.
     */
    COND_VB_SHOULD(VB_DRAW_ITEM_FROM_DMCHAR05, IS_RANDO, {
        GetItemDrawId vanillaItemId = (GetItemDrawId)va_arg(args, int);
        Actor* dmChar05 = va_arg(args, Actor*);
        switch (vanillaItemId) {
            case GID_MASK_GIBDO:
                Rando::DrawItem(RANDO_SAVE_CHECKS[RC_MUSIC_BOX_HOUSE_FATHER].randoItemId, RC_MUSIC_BOX_HOUSE_FATHER,
                                dmChar05);
                break;
            case GID_MASK_GORON:
                Rando::DrawItem(RANDO_SAVE_CHECKS[RC_GORON_GRAVEYARD_DARMANI].randoItemId, RC_GORON_GRAVEYARD_DARMANI,
                                dmChar05);
                break;
            case GID_MASK_ZORA:
                Rando::DrawItem(RANDO_SAVE_CHECKS[RC_GREAT_BAY_COAST_MIKAU].randoItemId, RC_GREAT_BAY_COAST_MIKAU,
                                dmChar05);
                break;
            case GID_MASK_COUPLE:
                Rando::DrawItem(RANDO_SAVE_CHECKS[RC_STOCK_POT_INN_COUPLES_MASK].randoItemId,
                                RC_STOCK_POT_INN_COUPLES_MASK, dmChar05);
                break;
            default:
                break;
        }
        *should = false;
    });

    COND_ID_HOOK(OnOpenText, GORON_MASK_TEXT, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        replaceGetItemText(RC_GORON_GRAVEYARD_DARMANI, textId, loadFromMessageTable);
    });

    COND_ID_HOOK(OnOpenText, ZORA_MASK_TEXT, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        replaceGetItemText(RC_GREAT_BAY_COAST_MIKAU, textId, loadFromMessageTable);
    });

    COND_ID_HOOK(OnOpenText, GIBDO_MASK_TEXT, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        replaceGetItemText(RC_MUSIC_BOX_HOUSE_FATHER, textId, loadFromMessageTable);
    });

    COND_ID_HOOK(OnOpenText, COUPLES_MASK_TEXT, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        replaceGetItemText(RC_STOCK_POT_INN_COUPLES_MASK, textId, loadFromMessageTable);
    });
}