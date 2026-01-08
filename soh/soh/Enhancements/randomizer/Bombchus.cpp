#include <soh/OTRGlobals.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "src/overlays/actors/ovl_En_GirlA/z_en_girla.h"

extern "C" {
#include "z64save.h"
}

void RegisterVBOverrides() {
    bool shouldRegister = IS_RANDO && RAND_GET_OPTION(RSK_BOMBCHU_BAG) == RO_BOMBCHU_BAG_PROGRESSIVE;
    COND_VB_SHOULD(VB_CAN_BUY_BOMBCHUS, IS_RANDO, {
        EnGirlACanBuyResult* canBuy = va_arg(args, EnGirlACanBuyResult*);
        u8 bombchuBag = RAND_GET_OPTION(RSK_BOMBCHU_BAG);
        // When in rando, don't allow buying bombchus when the player doesn't have required explosives
        // If bombchus are in logic, the player needs to have bombchus; otherwise they need a bomb bag
        if ((!bombchuBag && CUR_CAPACITY(UPG_BOMB_BAG) == 0) ||
            (bombchuBag && INV_CONTENT(ITEM_BOMBCHU) == ITEM_NONE)) {
            *canBuy = CANBUY_RESULT_CANT_GET_NOW;
            *should = true;
        }
        int8_t capacity = 50;
        if (bombchuBag == RO_BOMBCHU_BAG_PROGRESSIVE) {
            capacity = OTRGlobals::Instance->gRandoContext->GetBombchuCapacity();
        }
        if (AMMO(ITEM_BOMBCHU) >= capacity) {
            *canBuy = CANBUY_RESULT_CANT_GET_NOW;
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_CHECK_BOMBCHU_CAPACITY, shouldRegister, {
        *should = false;
        uint8_t capacity = OTRGlobals::Instance->gRandoContext->GetBombchuCapacity();
        if (AMMO(ITEM_BOMBCHU) > capacity) {
            AMMO(ITEM_BOMBCHU) = capacity;
        }
    });

    COND_VB_SHOULD(VB_COLOR_AMMO_GREEN, shouldRegister, {
        int16_t i = va_arg(args, int);
        if (i == ITEM_BOMBCHU) {
            uint8_t capacity = OTRGlobals::Instance->gRandoContext->GetBombchuCapacity();
            if (AMMO(i) == capacity) {
                *should = true;
            }
        }
    });
}

static RegisterShipInitFunc initHooks(RegisterVBOverrides, { "IS_RANDO" });