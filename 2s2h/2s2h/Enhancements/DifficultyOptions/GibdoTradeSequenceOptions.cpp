#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/ShipInit.hpp"
#include <stdarg.h>

#define CVAR_NAME "gEnhancements.DifficultyOptions.GibdoTradeSequence"
#define CVAR CVarGetInteger(CVAR_NAME, GIBDO_TRADE_SEQUENCE_VANILLA)

extern "C" {
#include "functions.h"
#include "variables.h"
#include "overlays/actors/ovl_En_Talk_Gibud/z_en_talk_gibud.h"

// redefinition
typedef struct {
    /* 0x0 */ PlayerItemAction itemAction;
    /* 0x4 */ ItemId item;
    /* 0x8 */ s32 amount;
    /* 0xC */ s16 isBottledItem;
} EnTalkGibudRequestedItem; // size = 0x10
}

static EnTalkGibudRequestedItem redPotionRequestedItem = { PLAYER_IA_BOTTLE_POTION_RED, ITEM_POTION_RED, 1, true };

void RegisterGibdoTradeSequenceOptions() {
    COND_VB_SHOULD(VB_GIBDO_TRADE_SEQUENCE_SUFFICIENT_QUANTITY_PRESENTED, CVAR != GIBDO_TRADE_SEQUENCE_VANILLA, {
        ItemId requestedItemId = (ItemId)va_arg(args, int);
        if (AMMO(requestedItemId) >= 1) {
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_GIBDO_TRADE_SEQUENCE_ACCEPT_RED_POTION, CVAR == GIBDO_TRADE_SEQUENCE_MM3D, {
        PlayerItemAction requestedItemAction = (PlayerItemAction)va_arg(args, int);
        PlayerItemAction presentedItemAction = (PlayerItemAction)va_arg(args, int);

        EnTalkGibudRequestedItem** requestedItem = va_arg(args, EnTalkGibudRequestedItem**);

        if (requestedItemAction == PLAYER_IA_BOTTLE_POTION_BLUE &&
            presentedItemAction ==
                PLAYER_IA_BOTTLE_POTION_RED) { // If requested blue potion, but presented red potion, switch out
                                               // requested item from blue potion to red potion.
            *should = true;
            *requestedItem = &redPotionRequestedItem;
        }
    });

    COND_VB_SHOULD(VB_GIBDO_TRADE_SEQUENCE_TAKE_MORE_THAN_ONE_ITEM, CVAR != GIBDO_TRADE_SEQUENCE_VANILLA, {
        *should = false;

        EnTalkGibudRequestedItem* requestedItem = va_arg(args, EnTalkGibudRequestedItem*);
        if (CVAR == GIBDO_TRADE_SEQUENCE_MM3D) {
            Inventory_ChangeAmmo(requestedItem->item, -1);
        }
    });

    COND_VB_SHOULD(VB_GIBDO_TRADE_SEQUENCE_DO_TRADE, CVAR == GIBDO_TRADE_SEQUENCE_NO_TRADE, {
        *should = false;

        EnTalkGibud* gibudCtx = va_arg(args, EnTalkGibud*);
        bool doEndTradeMessage = (bool)va_arg(args, int);

        if (doEndTradeMessage) {
            Message_StartTextbox(gPlayState, 0x138A, &gibudCtx->actor);
            gibudCtx->textId = 0x138A;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterGibdoTradeSequenceOptions, { CVAR_NAME });