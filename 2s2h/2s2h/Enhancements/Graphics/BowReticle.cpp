#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "variables.h"
#include "objects/gameplay_keep/gameplay_keep.h"
}

#define CVAR_NAME "gEnhancements.Graphics.BowReticle"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void DrawBowReticle(PlayState* play, Player* player, f32 bowDistance) {
    static Vec3f D_801C094C = { -500.0f, -100.0f, 0.0f };
    CollisionPoly* poly;
    s32 bgId;
    Vec3f posA;
    Vec3f posB;
    Vec3f pos;
    Vec3f projectedPos;
    f32 projectedW;
    f32 scale;

    D_801C094C.z = 0.0f;
    Matrix_MultVec3f(&D_801C094C, &posA);
    D_801C094C.z = bowDistance;
    Matrix_MultVec3f(&D_801C094C, &posB);

    // If the line test doesn't hit a valid polygon, use the "farthest" point as the reticle position.
    // This ensures that the reticle is always visible even when looking at the sky
    if (!BgCheck_ProjectileLineTest(&play->colCtx, &posA, &posB, &pos, &poly, true, true, true, true, &bgId)) {
        pos = posB;
    }

    OPEN_DISPS(play->state.gfxCtx);

    OVERLAY_DISP = Gfx_SetupDL(OVERLAY_DISP, SETUPDL_7);

    SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &pos, &projectedPos, &projectedW);

    scale = (projectedW < 500.0f) ? 0.075f : (projectedW / 500.0f) * 0.075f;

    Matrix_Translate(pos.x, pos.y, pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(OVERLAY_DISP++, play->state.gfxCtx);

    gSPSegment(OVERLAY_DISP++, 0x06, (uintptr_t)play->objectCtx.slots[player->actor.objectSlot].segment);
    gSPDisplayList(OVERLAY_DISP++, (Gfx*)gHookshotReticleDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void RegisterBowReticle() {
    COND_ID_HOOK(OnPlayerPostLimbDraw, PLAYER_LIMB_RIGHT_HAND, CVAR, [](Player* player, s32 limbIndex) {
        if (player->actor.scale.y >= 0.0f &&
            ((player->heldItemAction == PLAYER_IA_BOW_FIRE) || (player->heldItemAction == PLAYER_IA_BOW_ICE) ||
             (player->heldItemAction == PLAYER_IA_BOW_LIGHT) || (player->heldItemAction == PLAYER_IA_BOW))) {

            if (func_800B7128(player) != 0) {
                // Rotation from link's right hand that aligns with arrow projection
                Matrix_RotateZYX(0, -0x3B33, -0x4423, MTXMODE_APPLY);
                Matrix_Translate(575.0f, 345.0f, 0.0f, MTXMODE_APPLY);

                // 341000 as a value is selected roughly as a ratio of the hookshot value to maximum hookshot
                // distance from player, multiplied by maximum arrow distance from player -- (77600 / 770) * 3385
                DrawBowReticle(gPlayState, player, 341000.0f);
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterBowReticle, { CVAR_NAME });
