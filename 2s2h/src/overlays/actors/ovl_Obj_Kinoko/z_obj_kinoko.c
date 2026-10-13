/*
 * File: z_obj_kinoko.c
 * Overlay: ovl_Obj_Kinoko
 * Description: Mushroom
 */

#include "z_obj_kinoko.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "BenPort.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjKinoko_Init(Actor* thisx, PlayState* play);
void ObjKinoko_Destroy(Actor* thisx, PlayState* play);
void ObjKinoko_Update(Actor* thisx, PlayState* play);
void ObjKinoko_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Kinoko_Profile = {
    /**/ ACTOR_OBJ_KINOKO,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjKinoko),
    /**/ ObjKinoko_Init,
    /**/ ObjKinoko_Destroy,
    /**/ ObjKinoko_Update,
    /**/ ObjKinoko_Draw,
};

void ObjKinoko_Init(Actor* thisx, PlayState* play) {
    thisx->world.pos.y += 4.0f;
}

void ObjKinoko_Destroy(Actor* thisx, PlayState* play) {
}

void ObjKinoko_Update(Actor* thisx, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;

    if (player->currentMask != PLAYER_MASK_SCENTS) {
        thisx->draw = NULL;
        thisx->hintId = TATL_HINT_ID_NONE;
        thisx->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    } else {
        thisx->draw = ObjKinoko_Draw;
        thisx->hintId = TATL_HINT_ID_MUSHROOM;
        thisx->flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        if (Actor_HasParent(thisx, play)) {
            Flags_SetCollectible(play, OBJ_KINOKO_GET_FLAG(thisx));
            Actor_Kill(thisx);
            return;
        }
        Actor_OfferGetItem(thisx, play, GI_MAX, 20.0f, 10.0f);
        if (Math_SmoothStepToF(&thisx->speed, 0.0f, 0.04f, 2.0f, 0.5f) < 0.5f) {
            thisx->scale.x = 0.0f;
            thisx->speed = 110.0f;
            thisx->velocity.x = 0.2f;
        }
        if (Math_SmoothStepToF(&thisx->scale.x, thisx->velocity.x, 0.04f, 0.004f, 0.001f) <
            ((thisx->velocity.x == 0.0f) ? 0.0f : 0.05f)) {
            thisx->velocity.x = 0.0f;
        }
        thisx->scale.y = thisx->scale.x;
        thisx->scale.z = thisx->scale.x;
        thisx->shape.rot.y = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000;
    }
}

void ObjKinoko_Draw(Actor* thisx, PlayState* play) {
    Gfx* gfx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    // #region 2S2H [Port]
    // We need to first load the DL before we can index it on the port
    Gfx* gameplay_keep_DL_029D10_Data = ResourceMgr_LoadGfxByName(gameplay_keep_DL_029D10);

    gfx = POLY_XLU_DISP;
    gDPSetPrimColor(&gfx[0], 0, 0, 169, 63, 186, (u8)thisx->speed);
    gDPSetEnvColor(&gfx[1], 110, 44, 200, 100);
    gDPSetRenderMode(&gfx[2], G_RM_PASS, G_RM_ZB_CLD_SURF2);
    MATRIX_FINALIZE_AND_LOAD(&gfx[3], play->state.gfxCtx);
    // Index adjust 2 -> 4 (for gsDPPipeSync) to account for our extraction size changes
    gSPDisplayList(&gfx[4], &gameplay_keep_DL_029D10_Data[4]);
    Matrix_RotateXS(-0x4000, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(&gfx[5], play->state.gfxCtx);
    gSPDisplayList(&gfx[6], &gameplay_keep_DL_029D10_Data[4]);
    // #endregion
    POLY_XLU_DISP = &gfx[7];

    CLOSE_DISPS(play->state.gfxCtx);
}
