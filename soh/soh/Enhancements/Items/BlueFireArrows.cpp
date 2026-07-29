#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_Bg_Breakwall/z_bg_breakwall.h"
#include "overlays/actors/ovl_Bg_Ice_Shelter/z_bg_ice_shelter.h"

extern PlayState* gPlayState;
}

static void UpdateBlueFireCollidersBgBreakwall(void* actorPtr) {
    BgBreakwall* thisx = (BgBreakwall*)actorPtr;
    thisx->collider.info.bumper.dmgFlags |= DMG_ARROW_ICE;
}

static void UpdateBlueFireCollidersBgIceShelter(void* actorPtr) {
    BgIceShelter* thisx = (BgIceShelter*)actorPtr;
    thisx->cylinder1.base.acFlags |= AC_TYPE_PLAYER;
    thisx->cylinder1.info.bumper.dmgFlags |= DMG_ARROW_ICE;
    thisx->cylinder2.base.acFlags |= AC_TYPE_PLAYER;
    thisx->cylinder2.info.bumper.dmgFlags |= DMG_ARROW_ICE;
}

static bool CheckAC(Actor* ac) {
    return ac != NULL && ac->id == ACTOR_EN_ARROW && ac->child != NULL && ac->child->id == ACTOR_ARROW_ICE;
}

void RegisterBlueFireArrowsHooks() {
    bool shouldRegister =
        CVarGetInteger(CVAR_ENHANCEMENT("BlueFireArrows"), 0) || (IS_RANDO && RAND_GET_OPTION(RSK_BLUE_FIRE_ARROWS));

    COND_ID_HOOK(OnActorInit, ACTOR_BG_BREAKWALL, shouldRegister, UpdateBlueFireCollidersBgBreakwall);
    COND_ID_HOOK(OnActorInit, ACTOR_BG_ICE_SHELTER, shouldRegister, UpdateBlueFireCollidersBgIceShelter);

    // fix bug where cylinder2 never checks acFlags
    COND_VB_SHOULD(VB_BG_ICE_SHELTER_HIT, shouldRegister, {
        BgIceShelter* thisx = va_arg(args, BgIceShelter*);

        if (thisx->cylinder2.base.acFlags & AC_HIT) {
            thisx->cylinder2.base.acFlags &= ~AC_HIT;
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_BG_ICE_SHELTER_MELT, shouldRegister, {
        BgIceShelter* thisx = va_arg(args, BgIceShelter*);

        if (CheckAC(thisx->cylinder1.base.ac) || CheckAC(thisx->cylinder2.base.ac)) {
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterBlueFireArrowsHooks, { "IS_RANDO", CVAR_ENHANCEMENT("BlueFireArrows") });
