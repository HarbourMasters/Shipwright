#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Wood02/z_en_wood02.h"
}

extern PlayState* gPlayState;

static constexpr int32_t CVAR_TREES_DROP_STICKS_DEFAULT = 0;
#define CVAR_TREES_DROP_STICKS_NAME CVAR_ENHANCEMENT("TreesDropSticks")
#define CVAR_TREES_DROP_STICKS_VALUE CVarGetInteger(CVAR_TREES_DROP_STICKS_NAME, CVAR_TREES_DROP_STICKS_DEFAULT)

static void RegisterTreesDropSticks() {
    COND_VB_SHOULD(VB_TREE_DROP_COLLECTIBLE, CVAR_TREES_DROP_STICKS_VALUE, {
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

    COND_VB_SHOULD(VB_PREVENT_ADULT_STICK, CVAR_TREES_DROP_STICKS_VALUE, {
        if (INV_CONTENT(ITEM_STICK) != ITEM_NONE) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterTreesDropSticks, { CVAR_TREES_DROP_STICKS_NAME });
