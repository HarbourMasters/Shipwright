/*
 * File: z_en_pu_box.c
 * Overlay: ovl_En_Pu_Box
 * Description: An unused stone cube
 */

#include "z_en_pu_box.h"
#include "objects/object_pu_box/object_pu_box.h"

#define FLAGS ACTOR_FLAG_4

void EnPubox_Init(Actor* thisx, PlayState* play);
void EnPubox_Destroy(Actor* thisx, PlayState* play);
void EnPubox_Update(Actor* thisx, PlayState* play);
void EnPubox_Draw(Actor* thisx, PlayState* play);

const ActorInit En_Pu_box_InitVars = {
    ACTOR_EN_PU_BOX,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_PU_BOX,
    sizeof(EnPubox),
    (ActorFunc)EnPubox_Init,
    (ActorFunc)EnPubox_Destroy,
    (ActorFunc)EnPubox_Update,
    (ActorFunc)EnPubox_Draw,
    NULL,
};

void EnPubox_Init(Actor* thisx, PlayState* play) {
    CollisionHeader* colHeader = NULL;
    EnPubox* this = (EnPubox*)thisx;

    switch (thisx->params) {
        case 0:
            Actor_SetScale(thisx, 0.0025f);
            break;
        case 1:
            Actor_SetScale(thisx, 0.005f);
            break;
        case 2:
            Actor_SetScale(thisx, 0.0075f);
            break;
        case 3:
            Actor_SetScale(thisx, 0.01f);
        default:
            break;
    }
    this->unk_164 = 1;
    thisx->colChkInfo.cylRadius = 20;
    thisx->colChkInfo.cylHeight = 50;
    thisx->uncullZoneDownward = 1200.0f;
    thisx->uncullZoneScale = 720.0f;
    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, 6.0f);
    this->dyna.unk_160 = 0;
    this->dyna.unk_15C = DPM_UNK;
    thisx->targetMode = 1;
    thisx->gravity = -2.0f;
    CollisionHeader_GetVirtual(&gBlockMediumCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, thisx, colHeader);
}

void EnPubox_Destroy(Actor* thisx, PlayState* play) {
    EnPubox* this = (EnPubox*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void EnPubox_Update(Actor* thisx, PlayState* play) {
    EnPubox* this = (EnPubox*)thisx;

    thisx->speedXZ += this->dyna.unk_150;
    thisx->world.rot.y = this->dyna.unk_158;
    thisx->speedXZ = (thisx->speedXZ < -2.5f) ? -2.5f : ((thisx->speedXZ > 2.5f) ? 2.5f : thisx->speedXZ);
    Math_SmoothStepToF(&thisx->speedXZ, 0.0f, 1.0f, 1.0f, 0.0f);
    if (thisx->speedXZ != 0.0f) {
        Audio_PlaySoundGeneral(NA_SE_EV_ROCK_SLIDE - SFX_FLAG, &thisx->projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
    this->dyna.unk_154 = 0.0f;
    this->dyna.unk_150 = 0.0f;
    Actor_MoveForward(thisx);
    Actor_UpdateBgCheckInfo(play, thisx, thisx->colChkInfo.cylHeight, thisx->colChkInfo.cylRadius,
                            thisx->colChkInfo.cylRadius, 0x1D);
    thisx->focus.pos = thisx->world.pos;
}

void EnPubox_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, gBlockMediumDL);
}
