#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "src/overlays/actors/ovl_En_Ossan/z_en_ossan.h"
}

static void RegisterMaskSelectFixes() {
    // Fix mask quest so replacing SOLD OUT with mask select doesn't break quest
    COND_VB_SHOULD(VB_HAPPY_MASK_SHOP_CHECK_SOLD_OUT, CVarGetInteger(CVAR_ENHANCEMENT("MaskSelect"), 0), {
        Actor* actor = va_arg(args, Actor*);
        if (actor->params == OSSAN_TYPE_MASK &&
                (Flags_GetItemGetInf(ITEMGETINF_3B) && !Flags_GetEventChkInf(EVENTCHKINF_PAID_BACK_BUNNY_HOOD_FEE)) ||
            (Flags_GetItemGetInf(ITEMGETINF_3A) && !Flags_GetEventChkInf(EVENTCHKINF_PAID_BACK_SPOOKY_MASK_FEE)) ||
            (Flags_GetItemGetInf(ITEMGETINF_39) && !Flags_GetEventChkInf(EVENTCHKINF_PAID_BACK_SKULL_MASK_FEE)) ||
            (Flags_GetItemGetInf(ITEMGETINF_38) && !Flags_GetEventChkInf(EVENTCHKINF_PAID_BACK_KEATON_MASK_FEE))) {
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterMaskSelectFixes, { CVAR_ENHANCEMENT("MaskSelect") });
