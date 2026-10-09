#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"

#include "overlays/actors/ovl_Obj_Moon_Stone/z_obj_moon_stone.h"
}

void ObjMoonstone_DrawCustom(Actor* thisx, PlayState* play) {
    auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_ASTRAL_OBSERVATORY_MOON_TEAR];

    // Rotate the display of the item to face the player towards the door
    Matrix_RotateYS(0x5000, MTXMODE_APPLY);

    RandoItemId randoItemId = randoSaveCheck.randoItemId;

    // When not in Astral Observatory, allow the item to convert and render with particles
    if (play->sceneId != SCENE_TENMON_DAI) {
        randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId, RC_ASTRAL_OBSERVATORY_MOON_TEAR);
        Rando::DrawItem(randoItemId, RC_ASTRAL_OBSERVATORY_MOON_TEAR, thisx);
    } else {
        Rando::DrawItem(randoItemId, RC_ASTRAL_OBSERVATORY_MOON_TEAR, thisx);
    }
}

void Rando::ActorBehavior::InitObjMoonStoneBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_MOON_STONE, IS_RANDO, [](Actor* actor) {
        // Only replace the draw if the stone would have been drawn normally
        if (actor->draw != NULL) {
            actor->draw = ObjMoonstone_DrawCustom;
        }
    });

    COND_VB_SHOULD(VB_REVEAL_MOON_STONE_IN_CRATER, IS_RANDO, {
        ObjMoonStone* objMoonStone = va_arg(args, ObjMoonStone*);
        objMoonStone->actor.draw = ObjMoonstone_DrawCustom;
        *should = false;
    });

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_MOONS_TEAR, IS_RANDO, {
        ObjMoonStone* objMoonStone = va_arg(args, ObjMoonStone*);
        if (objMoonStone->actor.xzDistToPlayer < 25.0f) {
            *should = false;
            SET_WEEKEVENTREG(WEEKEVENTREG_74_40);
            Actor_Kill(&objMoonStone->actor);
        }
    });
}
