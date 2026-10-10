#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_Obj_Bean/z_obj_bean.h"
}

#define CVAR_NAME "gEnhancements.Restorations.SoilPatch"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static void RegisterSoilPatchRestoration() {
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        Actor* actor = va_arg(args, Actor*);

        if (*csId == -1 || actor == NULL) {
            return;
        }

        if (actor->id == ACTOR_OBJ_MAKEKINSUTA ||
            (actor->id == ACTOR_OBJ_BEAN && OBJBEAN_GET_C000(actor) != ENOBJBEAN_GET_C000_0)) {
            actor->csId = -1;
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_COUNT_BURROWED_BUGS, CVAR, {
        ObjBean* bean = va_arg(args, ObjBean*);
        if (*should) {
            if (bean->unk_1E4 == 2 && bean->unk_1E0 < 2) {
                bean->unk_1E0 = 2;
            }
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSoilPatchRestoration, { CVAR_NAME });
