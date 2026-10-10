#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"

#include "overlays/actors/ovl_En_Scopenuts/z_en_scopenuts.h"
void func_80BCB078(EnScopenuts* enScopenuts, PlayState* play);

void func_80BCB1C8(EnScopenuts* enScopenuts, PlayState* play);
f32 func_80BCC448(Path* path, s32 arg1, Vec3f* arg2, Vec3s* arg3);
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// has reached point
s32 modified_func_80BCC2AC(EnScopenuts* enScopenuts, Path* path, s32 arg2_) {
    Vec3s* points = path->points;
    s32 count = path->count;
    s32 index = arg2_;
    s32 sp50 = false;
    f32 phi_f12;
    f32 phi_f14;
    f32 sp44;
    f32 sp40;
    f32 sp3C;
    Vec3f point;

    Math_Vec3s_ToVec3f(&point, &points[index]);

    if (index == 0) {
        phi_f12 = points[1].x - points[0].x;
        phi_f14 = points[1].z - points[0].z;
    } else if (index == 16) { // special case because we're skipping to this index
        point.x = 2700;
        point.z = 1200;
        phi_f12 = point.x - points[3].x;
        phi_f14 = point.z - points[3].z;
    } else if ((count - 1) == index) {
        phi_f12 = points[count - 1].x - points[count - 2].x;
        phi_f14 = points[count - 1].z - points[count - 2].z;
    } else {
        phi_f12 = points[index + 1].x - points[index - 1].x;
        phi_f14 = points[index + 1].z - points[index - 1].z;
    }

    Math3D_RotateXZPlane(&point, RAD_TO_BINANG(Math_FAtan2F(phi_f12, phi_f14)), &sp44, &sp40, &sp3C);

    if (((enScopenuts->actor.world.pos.x * sp44) + (sp40 * enScopenuts->actor.world.pos.z) + sp3C) > 0.0f) {
        sp50 = true;
    }
    return sp50;
}

void modified_func_80BCB078(EnScopenuts* enScopenuts, PlayState* play) {
    Vec3s sp30;
    if (enScopenuts->path != NULL) {
        func_80BCC448(enScopenuts->path, enScopenuts->unk_334, &enScopenuts->actor.world.pos, &sp30);
        if (enScopenuts->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            sp30.y = enScopenuts->actor.wallYaw;
        }
        Math_SmoothStepToS(&enScopenuts->actor.world.rot.y, sp30.y, 10, 300, 0);
        enScopenuts->actor.shape.rot.y = enScopenuts->actor.world.rot.y;
        enScopenuts->unk_33E = 0x1000;
        enScopenuts->unk_340 += 0x1C71;
        enScopenuts->actor.world.rot.x = -sp30.x;
        // First difference from original function
        if (modified_func_80BCC2AC(enScopenuts, enScopenuts->path, enScopenuts->unk_334)) {
            if (enScopenuts->unk_334 >= (enScopenuts->path->count - 1)) {
                enScopenuts->actionFunc = func_80BCB1C8;
                enScopenuts->actor.speed = 0.0f;
                enScopenuts->actor.gravity = -1.0f;
                return;
            }
            enScopenuts->unk_334++;
        }
    }

    if (enScopenuts->unk_334 >= (enScopenuts->path->count - 2)) {
        Math_ApproachF(&enScopenuts->actor.speed, 1.5f, 0.2f, 1.0f);
        // second diffference. additional condition to speed up this segment
    } else if (enScopenuts->unk_334 == 16) {
        Math_ApproachF(&enScopenuts->actor.speed, 7.5f, 0.2f, 1.0f);
    } else {
        Math_ApproachF(&enScopenuts->actor.speed, 5.0f, 0.2f, 1.0f);
    }
    Actor_MoveWithoutGravity(&enScopenuts->actor);
}

void RegisterSkipDekuTelescope() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_SCOPENUTS, CVAR, [](Actor* actor) {
        EnScopenuts* enScopenuts = (EnScopenuts*)actor;

        // ENSCOPENUTS_3E0_0 is Termina Field one
        if (((enScopenuts->actor.params & 0x3E0) >> 5) == 0) {
            if (enScopenuts->actionFunc == func_80BCB078 && enScopenuts->unk_334 == 3) {
                enScopenuts->actionFunc = modified_func_80BCB078;
                // adjust path point index
                enScopenuts->unk_334 = 16;
            }
        }
    });

    // Skip flying out of grotto cs (player has control during it)
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        if (gPlayState->sceneId == SCENE_KAKUSIANA) {
            if (*csId == 16) {
                *should = false;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipDekuTelescope, { CVAR_NAME });
