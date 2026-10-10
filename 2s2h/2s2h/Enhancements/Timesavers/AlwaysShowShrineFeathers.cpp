#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Owl/z_en_owl.h"
}

#define CVAR_NAME "gEnhancements.Timesavers.AlwaysShowShrineFeathers"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static void RegisterAlwaysShowShrineFeathers() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_OWL, CVAR, [](Actor* actor) {
        if (ENOWL_GET_TYPE(actor) != ENOWL_GET_TYPE_2) {
            return;
        }

        s32 pathIndex = ENOWL_GET_PATH_INDEX(actor);
        if (pathIndex == ENOWL_PATH_INDEX_NONE) {
            return;
        }

        Path* path = &gPlayState->setupPathList[pathIndex];
        Vec3s* points = (Vec3s*)Lib_SegmentedToVirtual(path->points);

        for (s32 i = 0; i < path->count; i++) {
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_OWL, points[i].x, points[i].y, points[i].z, 0, 0, 0,
                        0xF00);
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_OWL, CVAR, [](Actor* actor) {
        if (ENOWL_GET_TYPE(actor) == ENOWL_GET_TYPE_30) {
            EnOwl* owl = (EnOwl*)actor;
            // Lock the despawn timer so the feather remains indefinitely
            owl->unk_3DC = 0x12C;
        }
    });

    COND_VB_SHOULD(VB_OWL_SPAWN_FEATHER, CVAR, { *should = false; });

    COND_VB_SHOULD(VB_OWL_DESPAWN_FEATHER, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterAlwaysShowShrineFeathers, { CVAR_NAME });
