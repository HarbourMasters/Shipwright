#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
extern const char* D_801C0B20[28];
}

#define CVAR_NAME "gModes.HyruleWarriorsStyledLink"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

#define CHECK_KEATON_GIVEN_ON_MOON() (gSaveContext.masksGivenOnMoon[0] & 0x10)

void RegisterHyruleWarriorsStyledLink() {
    COND_ID_HOOK(OnPlayerPostLimbDraw, PLAYER_LIMB_HEAD, CVAR, [](Player* player, s32 limbIndex) {
        // This emulates the vanilla check for if the masks should be drawn, specifically around
        // z_player.c 12923 (Player_Draw)
        if (player->stateFlags1 & PLAYER_STATE1_100000) {
            Vec3f temp;
            SkinMatrix_Vec3fMtxFMultXYZ(&gPlayState->viewProjectionMtxF, &player->actor.focus.pos, &temp);
            if (temp.z < -4.0f) {
                return;
            }
        }

        if (player->currentMask == PLAYER_MASK_NONE && player->transformation == PLAYER_FORM_HUMAN &&
            INV_CONTENT(ITEM_MASK_KEATON) == ITEM_MASK_KEATON && !CHECK_KEATON_GIVEN_ON_MOON()) {
            OPEN_DISPS(gPlayState->state.gfxCtx);
            Matrix_Push();
            Matrix_RotateYS(0x38e3, MTXMODE_APPLY);
            Matrix_RotateZS(-0x12F6, MTXMODE_APPLY);
            Matrix_Translate(300.0f, -250.0f, 77.7f, MTXMODE_APPLY);
            Matrix_Scale(0.648f, 0.648f, 0.648f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gPlayState->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, (Gfx*)D_801C0B20[PLAYER_MASK_KEATON - 1]);
            Matrix_Pop();
            CLOSE_DISPS(gPlayState->state.gfxCtx);
        }
    });
    COND_ID_HOOK(OnPlayerPostLimbDraw, PLAYER_LIMB_WAIST, CVAR, [](Player* player, s32 limbIndex) {
        if (player->transformation == PLAYER_FORM_HUMAN && player->itemAction != PLAYER_IA_MASK_FIERCE_DEITY &&
            INV_CONTENT(ITEM_MASK_FIERCE_DEITY) == ITEM_MASK_FIERCE_DEITY) {
            OPEN_DISPS(gPlayState->state.gfxCtx);
            Matrix_Push();
            Matrix_RotateXS(-0x61A8, MTXMODE_APPLY);
            Matrix_RotateYS(-0x7D0, MTXMODE_APPLY);
            Matrix_RotateZS(-0x3A98, MTXMODE_APPLY);
            Matrix_Translate(-85.0f, 658.0f, -165.0f, MTXMODE_APPLY);
            Matrix_Scale(0.635f, 0.635f, 0.635f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gPlayState->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, (Gfx*)D_801C0B20[PLAYER_MASK_FIERCE_DEITY - 1]);
            Matrix_Pop();
            CLOSE_DISPS(gPlayState->state.gfxCtx);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterHyruleWarriorsStyledLink, { CVAR_NAME });
