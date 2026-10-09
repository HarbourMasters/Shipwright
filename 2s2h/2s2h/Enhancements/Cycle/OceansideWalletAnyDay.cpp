#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/Rando/Rando.h"

extern "C" {
#include "overlays/actors/ovl_En_Sth/z_en_sth.h"
}

#define CVAR_NAME "gEnhancements.Cycle.OceansideWalletAnyDay"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterOceansideWalletAnyDay() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, CVAR || IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);

        if (actor->id != ACTOR_EN_STH || STH_GET_TYPE(actor) != STH_TYPE_OCEANSIDE_SPIDER_HOUSE_GREET) {
            return;
        }

        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_OCEANSIDE_WALLET_UPGRADE)) {
            switch (CUR_UPG_VALUE(UPG_WALLET)) {
                case 0:
                    *item = GI_WALLET_ADULT;
                    break;
                case 1:
                    *item = GI_WALLET_GIANT;
                    break;
                default:
                    *item = GI_RUPEE_SILVER;
                    break;
            }
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_OCEANSIDE_WALLET_UPGRADE);
            STH_GI_ID(actor) = *item;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterOceansideWalletAnyDay, { CVAR_NAME, "IS_RANDO" });
