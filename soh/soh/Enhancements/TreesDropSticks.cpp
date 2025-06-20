#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Wood02/z_en_wood02.h"
}

extern PlayState* gPlayState;

void RegisterTreesDropSticks() {
    COND_VB_SHOULD(VB_TREE_DROP_COLLECTIBLE, CVarGetInteger(CVAR_ENHANCEMENT("TreesDropSticks"), 0), {
        if (INV_CONTENT(ITEM_STICK) != ITEM_NONE) {
            EnWood02* tree = va_arg(args, EnWood02*);
            Vec3f dropsSpawnPt = tree->actor.world.pos;
            dropsSpawnPt.y += 200.0f;

            *should = false;
            for (s32 numDrops = Rand_Next() % 4; numDrops > 0; numDrops--) {
                Item_DropCollectible(gPlayState, &dropsSpawnPt, ITEM00_STICK);
            }
        }
    });

    COND_VB_SHOULD(VB_PREVENT_ADULT_STICK, CVarGetInteger(CVAR_ENHANCEMENT("TreesDropSticks"), 0), {
        if (INV_CONTENT(ITEM_STICK) != ITEM_NONE) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterTreesDropSticks, { CVAR_ENHANCEMENT("TreesDropSticks") });
