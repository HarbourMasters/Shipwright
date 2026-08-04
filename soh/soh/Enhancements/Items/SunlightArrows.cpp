#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/ShipInit.hpp"
#include "soh/ObjectExtension/ObjectExtension.h"

extern "C" {
#include "overlays/actors/ovl_Obj_Lightswitch/z_obj_lightswitch.h"
}

static ColliderJntSphElementInit sColliderLightArrowElementInit[] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00202000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 19 }, 100 },
    },
};
static ColliderJntSphInit sColliderLightArrowInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    sColliderLightArrowElementInit,
};

struct SunlightArrowData {
    bool activatedByLightArrow = false;
};

static ObjectExtension::Register<SunlightArrowData> SunlightArrowDataRegister;

void RegisterSunlightArrowsHooks() {
    bool shouldRegister =
        CVarGetInteger(CVAR_ENHANCEMENT("SunlightArrows"), 0) || (IS_RANDO && RAND_GET_OPTION(RSK_SUNLIGHT_ARROWS));

    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_LIGHTSWITCH, shouldRegister, [](void* actor) {
        auto* thisx = (ObjLightswitch*)actor;
        Collider_SetJntSph(gPlayState, &thisx->collider, &thisx->actor, &sColliderLightArrowInit, thisx->colliderItems);
        Collider_UpdateSpheres(0, &thisx->collider);
    });

    COND_ID_HOOK(OnActorDestroy, ACTOR_OBJ_LIGHTSWITCH, shouldRegister, [](void* actor) {
        auto* thisx = (ObjLightswitch*)actor;

        auto sunData = ObjectExtension::GetInstance().Get<SunlightArrowData>(&thisx->actor);
        if (sunData != nullptr && sunData->activatedByLightArrow) {
            switch (thisx->actor.params >> 4 & 3) {
                case OBJLIGHTSWITCH_TYPE_STAY_ON:
                case OBJLIGHTSWITCH_TYPE_2:
                case OBJLIGHTSWITCH_TYPE_1:
                    // Unset the switch flag on room exit to prevent the rock in the wall from
                    // vanishing on its own after activating the sun switch by Light Arrow
                    // Also prevents the cobra mirror from rotating to face the sun on its own
                    // Makes sun switches temporary when activated by Light Arrows (will turn off on room exit)
                    if (thisx->actor.room != 25) {
                        Flags_UnsetSwitch(gPlayState, thisx->actor.params >> 8 & 0x3F);
                    }
                    break;
                case OBJLIGHTSWITCH_TYPE_BURN:
                    break;
            }
        }
    });

    COND_VB_SHOULD(VB_LIGHTSWITCH_OFF, shouldRegister, {
        ObjLightswitch* thisx = va_arg(args, ObjLightswitch*);
        auto sunData = ObjectExtension::GetInstance().Get<SunlightArrowData>(&thisx->actor);
        if (sunData != nullptr && sunData->activatedByLightArrow) {
            *should = false;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_OBJ_LIGHTSWITCH, shouldRegister, [](void* actorPtr, bool* result) {
        ObjLightswitch* thisx = (ObjLightswitch*)actorPtr;
        if ((thisx->collider.base.acFlags & AC_HIT) && thisx->collider.base.ac != nullptr) {
            auto sunData = ObjectExtension::GetInstance().Get<SunlightArrowData>(&thisx->actor);
            if (sunData == nullptr) {
                ObjectExtension::GetInstance().Set(&thisx->actor, SunlightArrowData{});
                sunData = ObjectExtension::GetInstance().Get<SunlightArrowData>(&thisx->actor);
            }

            sunData->activatedByLightArrow = thisx->collider.base.ac->id == ACTOR_EN_ARROW;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSunlightArrowsHooks, { "IS_RANDO", CVAR_ENHANCEMENT("SunlightArrows") });
