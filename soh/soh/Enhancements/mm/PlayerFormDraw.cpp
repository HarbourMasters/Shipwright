#include "PlayerFormSystem.h"
#include "soh/Enhancements/mm/forms/FormsCommon.h"

#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/frame_interpolation.h"

extern "C" {
#include "soh/ResourceManagerHelpers.h"
#include "assets/mm/objects/object_link_goron/object_link_goron.h"
#include "assets/mm/objects/object_link_zora/object_link_zora.h"
}

// While transformed, the flat mask display list is suppressed and the vanilla hand/sheath/waist
// display list substitutions (which point at Link's model) are redirected to the form's own
// display lists, mirroring which limbs Player_OverrideLimbDrawGameplayDefault overrides.
// Known edge case: Dark Link shares the player draw path, so he inherits the form face textures
// and equipment suppression if the player fights him while transformed.

typedef struct FormLimbDLs {
    const char* leftHandOpen;
    const char* leftHandClosed;
    const char* rightHandOpen;
    const char* rightHandClosed;
    const char* sheath;
    const char* waist;
} FormLimbDLs;

static const FormLimbDLs sFormLimbDLs[PLAYER_SHIPFORM_MAX] = {
    {},
    { gLinkGoronLeftHandOpenDL, gLinkGoronLeftHandClosedDL, gLinkGoronRightHandOpenDL, gLinkGoronRightHandClosedDL,
      gLinkGoronSheathDL, gLinkGoronWaistDL },
    { gLinkZoraLeftHandOpenDL, gLinkZoraLeftHandClosedDL, gLinkZoraRightHandOpenDL, gLinkZoraRightHandClosedDL,
      gLinkZoraSheathDL, gLinkZoraWaistDL },
};

// OOT eye indices: open, half, closed, roll-left, roll-right, shock, unk1, unk2.
// The Goron only has open/half/closed/surprised, so the directional eyes fall back to open.
static const char* sFormEyeTextures[PLAYER_SHIPFORM_MAX][8] = {
    {},
    { gLinkGoronEyesOpenTex, gLinkGoronEyesHalfTex, gLinkGoronEyesClosedTex, gLinkGoronEyesOpenTex,
      gLinkGoronEyesOpenTex, gLinkGoronEyesSurprisedTex, gLinkGoronEyesOpenTex, gLinkGoronEyesOpenTex },
    { gLinkZoraEyesOpenTex, gLinkZoraEyesHalfTex, gLinkZoraEyesClosedTex, gLinkZoraEyesLeftTex, gLinkZoraEyesRightTex,
      gLinkZoraEyesWincingTex, gLinkZoraEyesOpenTex, gLinkZoraEyesOpenTex },
};

// The Goron head display list doesn't sample the mouth segment, so only the Zora substitutes it
static const char* sFormMouthTextures[PLAYER_SHIPFORM_MAX][4] = {
    {},
    {},
    { gLinkZoraMouthClosedTex, gLinkZoraMouthHalfTex, gLinkZoraMouthOpenTex, gLinkZoraMouthSmileTex },
};

// While Goron-rolling, the skeleton is hidden and MM's curled ball display list is drawn in
// its place with squash/stretch and the spin from shape.rot (mm z_player.c Player_Draw,
// gLinkGoronCurledDL block). This is emitted at the root limb so it inherits the player's
// lighting and runs inside the player's frame-interpolation record.
static void DrawGoronBall(PlayState* play, Player* player) {
    f32 squash = GoronForm_GetSquash();
    f32 stretchY = squash + 1.0f;
    f32 stretchXZ = 1.0f - (squash * 0.5f);
    // Ball tint ramps white -> blue with the spike charge (MM tints the spinning ball the
    // same way through D_8085D580/D_8085D584)
    f32 tint = CLAMP(GoronForm_GetChargeTint(), 0.0f, 1.0f);
    u8 r = (u8)(255.0f + ((80.0f - 255.0f) * tint));
    u8 g = (u8)(255.0f + ((80.0f - 255.0f) * tint));
    u8 b = (u8)(255.0f + ((200.0f - 255.0f) * tint));

    OPEN_DISPS(play->state.gfxCtx);

    FrameInterpolation_RecordActorPosRotMatrix();
    Matrix_Push();
    Matrix_Translate(player->actor.world.pos.x,
                     player->actor.world.pos.y + (1200.0f * player->actor.scale.y * stretchY),
                     player->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateY(BINANG_TO_RAD(player->actor.shape.rot.y), MTXMODE_APPLY);
    Matrix_RotateZ(BINANG_TO_RAD(player->actor.shape.rot.z), MTXMODE_APPLY);
    Matrix_Scale(player->actor.scale.x * stretchXZ * 1.15f, player->actor.scale.y * stretchY * 1.15f,
                 CLAMP_MIN(stretchY, stretchXZ) * player->actor.scale.z * 1.15f, MTXMODE_APPLY);
    Matrix_RotateX(BINANG_TO_RAD(player->actor.shape.rot.x), MTXMODE_APPLY);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gDPSetEnvColor(POLY_OPA_DISP++, r, g, b, 255);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)ResourceMgr_LoadGfxByName(gLinkGoronCurledDL));
    if (GoronForm_IsSpiked()) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)ResourceMgr_LoadGfxByName(gLinkGoronRollingSpikesDL));
    }
    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

static void OverrideFormLimbDraw(s32 limbIndex, Gfx** dList, Player* player, bool inPauseMenu) {
    const FormLimbDLs& dls = sFormLimbDLs[PlayerForm_GetApplied()];
    const char* path = nullptr;

    // Mirrors the closed-hand condition in Player_OverrideLimbDrawGameplayDefault
    bool closedHands =
        !inPauseMenu && (player->actor.speedXZ > 2.0f) && !(player->stateFlags1 & PLAYER_STATE1_IN_WATER);

    switch (limbIndex) {
        case PLAYER_LIMB_L_HAND:
            path = closedHands ? dls.leftHandClosed : dls.leftHandOpen;
            break;
        case PLAYER_LIMB_R_HAND:
            path = closedHands ? dls.rightHandClosed : dls.rightHandOpen;
            break;
        case PLAYER_LIMB_SHEATH:
            path = dls.sheath;
            break;
        case PLAYER_LIMB_WAIST:
            path = dls.waist;
            break;
        default:
            return;
    }

    *dList = ResourceMgr_LoadGfxByName(path);
}

static void RegisterPlayerFormDraw() {
    // Hide the flat mask display list while transformed into the corresponding form
    REGISTER_VB_SHOULD(VB_DRAW_PLAYER_MASK, {
        PlayerMask currentMask = (PlayerMask)va_arg(args, int);
        PlayerShipForm applied = PlayerForm_GetApplied();

        if ((applied == PLAYER_SHIPFORM_GORON && currentMask == PLAYER_MASK_GORON) ||
            (applied == PLAYER_SHIPFORM_ZORA && currentMask == PLAYER_MASK_ZORA)) {
            *should = false;
        }
    });

    // Gauntlets, Goron bracelet and iron/hover boots rig to Link's limb matrices
    REGISTER_VB_SHOULD(VB_DRAW_PLAYER_STRENGTH_AND_BOOTS, {
        if (PlayerForm_GetApplied() != PLAYER_SHIPFORM_HUMAN) {
            *should = false;
        }
    });

    // The MM skeletons' limbs only have near display lists (the Goron's are StandardLimbs, and
    // reading LodLimb::dLists[1] from them is out of bounds), so never use the far LOD
    REGISTER_VB_SHOULD(VB_PLAYER_USE_MAX_LOD, {
        if (PlayerForm_GetApplied() != PLAYER_SHIPFORM_HUMAN) {
            *should = true;
        }
    });

    REGISTER_VB_SHOULD(VB_PLAYER_OVERRIDE_LIMB_DRAW, {
        if (PlayerForm_GetApplied() == PLAYER_SHIPFORM_HUMAN) {
            return;
        }

        s32 limbIndex = va_arg(args, s32);
        Gfx** dList = va_arg(args, Gfx**);
        Player* player = (Player*)va_arg(args, void*);

        // First-person keeps Link's arm display lists in phase 1, and other Player-struct
        // actors (Dark Link) keep Link's limbs
        if (gPlayState == NULL || player != GET_PLAYER(gPlayState) ||
            (player->stateFlags1 & PLAYER_STATE1_FIRST_PERSON)) {
            return;
        }

        // While curled into the Goron ball, hide every limb and draw the ball instead
        if (GoronForm_IsRolling()) {
            *dList = NULL;
            if (limbIndex == PLAYER_LIMB_ROOT) {
                DrawGoronBall(gPlayState, player);
            }
            return;
        }

        OverrideFormLimbDraw(limbIndex, dList, player, false);
    });

    REGISTER_VB_SHOULD(VB_PLAYER_OVERRIDE_LIMB_DRAW_PAUSE, {
        if (PlayerForm_GetApplied() == PLAYER_SHIPFORM_HUMAN) {
            return;
        }

        s32 limbIndex = va_arg(args, s32);
        Gfx** dList = va_arg(args, Gfx**);
        Player* player = (Player*)va_arg(args, void*);

        OverrideFormLimbDraw(limbIndex, dList, player, true);
    });

    REGISTER_VB_SHOULD(VB_PLAYER_APPLY_FACE_TEXTURES, {
        PlayerShipForm applied = PlayerForm_GetApplied();

        if (applied == PLAYER_SHIPFORM_HUMAN) {
            return;
        }

        void** eyeTexture = va_arg(args, void**);
        void** mouthTexture = va_arg(args, void**);
        s32 eyeIndex = va_arg(args, s32);
        s32 mouthIndex = va_arg(args, s32);

        *eyeTexture = (void*)sFormEyeTextures[applied][eyeIndex];
        if (sFormMouthTextures[applied][mouthIndex] != nullptr) {
            *mouthTexture = (void*)sFormMouthTextures[applied][mouthIndex];
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterPlayerFormDraw);
