#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_Bg_Breakwall/z_bg_breakwall.h"
#include "overlays/actors/ovl_Bg_Ice_Shelter/z_bg_ice_shelter.h"
extern PlayState* gPlayState;
}

static u32 GetBlueFireArrowDmgFlags() {
    if (IS_RANDO) {
        auto option = RAND_GET_OPTION(RSK_BLUE_FIRE_ARROWS);
        if (option.Is(RO_BLUE_FIRE_ARROW_FIRE)) {
            return DMG_ARROW_FIRE;
        } else if (option.Is(RO_BLUE_FIRE_ARROW_ICE)) {
            return DMG_ARROW_ICE;
        } else {
            return 0;
        }
    }

    switch (CVarGetInteger(CVAR_ENHANCEMENT("BlueFireArrows"), BLUE_FIRE_ARROW_NONE)) {
        case BLUE_FIRE_ARROW_ICE:
            return DMG_ARROW_ICE;
        case BLUE_FIRE_ARROW_FIRE:
            return DMG_ARROW_FIRE;
        default:
            return 0;
    };
}

static void UpdateBlueFireCollidersBgBreakwall(void* actorPtr) {
    BgBreakwall* thisx = (BgBreakwall*)actorPtr;
    thisx->collider.info.bumper.dmgFlags |= GetBlueFireArrowDmgFlags();
}

static void UpdateBlueFireCollidersBgIceShelter(void* actorPtr) {
    BgIceShelter* thisx = (BgIceShelter*)actorPtr;
    thisx->cylinder1.base.acFlags |= AC_TYPE_PLAYER;
    thisx->cylinder1.info.bumper.dmgFlags |= GetBlueFireArrowDmgFlags();
    thisx->cylinder2.base.acFlags |= AC_TYPE_PLAYER;
    thisx->cylinder2.info.bumper.dmgFlags |= GetBlueFireArrowDmgFlags();
}

static bool HitByBlueFireArrow(ColliderCylinder* cylinder) {
    return cylinder->info.acHitInfo != NULL &&
           (cylinder->info.acHitInfo->toucher.dmgFlags & GetBlueFireArrowDmgFlags());
}

static void RegisterBlueFireArrowsHooks() {
    bool shouldRegister = CVarGetInteger(CVAR_ENHANCEMENT("BlueFireArrows"), BLUE_FIRE_ARROW_NONE) ||
                          (IS_RANDO && RAND_GET_OPTION(RSK_BLUE_FIRE_ARROWS));

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

        if (HitByBlueFireArrow(&thisx->cylinder1) || HitByBlueFireArrow(&thisx->cylinder2)) {
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterBlueFireArrowsHooks, { "IS_RANDO", CVAR_ENHANCEMENT("BlueFireArrows") });
