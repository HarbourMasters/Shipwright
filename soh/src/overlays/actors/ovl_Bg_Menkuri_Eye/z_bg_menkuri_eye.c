/*
 * File: z_bg_menkuri_eye.c
 * Overlay: ovl_Bg_Menkuri_Eye
 * Description: Eye platform eye switches
 */

#include "z_bg_menkuri_eye.h"
#include "objects/object_menkuri_objects/object_menkuri_objects.h"

#define FLAGS ACTOR_FLAG_5

void BgMenkuriEye_Init(Actor* thisx, PlayState* play);
void BgMenkuriEye_Destroy(Actor* thisx, PlayState* play);
void BgMenkuriEye_Update(Actor* thisx, PlayState* play);
void BgMenkuriEye_Draw(Actor* thisx, PlayState* play);
void BgMenkuriEye_Reset(void);

const ActorInit Bg_Menkuri_Eye_InitVars = {
    ACTOR_BG_MENKURI_EYE,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MENKURI_OBJECTS,
    sizeof(BgMenkuriEye),
    (ActorFunc)BgMenkuriEye_Init,
    (ActorFunc)BgMenkuriEye_Destroy,
    (ActorFunc)BgMenkuriEye_Update,
    (ActorFunc)BgMenkuriEye_Draw,
    (ActorResetFunc)BgMenkuriEye_Reset,
};

s32 D_8089C1A0;

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F820, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { 1, { { 0, 0, 0 }, 14 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgMenkuriEye_Init(Actor* thisx, PlayState* play) {
    BgMenkuriEye* this = (BgMenkuriEye*)thisx;
    ColliderJntSphElement* colliderList;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderItems);
    this->collider.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider.elements[0].dim.worldSphere.center.y = this->actor.world.pos.y;
    this->collider.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z;
    colliderList = this->collider.elements;
    colliderList->dim.worldSphere.radius = colliderList->dim.modelSphere.radius;
    if (!Flags_GetSwitch(play, this->actor.params)) {
        D_8089C1A0 = 0;
    }
    this->framesUntilDisable = -1;
}

void BgMenkuriEye_Destroy(Actor* thisx, PlayState* play) {
    BgMenkuriEye* this = (BgMenkuriEye*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void BgMenkuriEye_Update(Actor* thisx, PlayState* play) {
    BgMenkuriEye* this = (BgMenkuriEye*)thisx;

    if (!Flags_GetSwitch(play, this->actor.params)) {
        if (this->framesUntilDisable != -1) {
            if (this->framesUntilDisable != 0) {
                this->framesUntilDisable -= 1;
            }
            if (this->framesUntilDisable == 0) {
                this->framesUntilDisable = -1;
                D_8089C1A0 -= 1;
            }
        }
    }
    if ((this->collider.base.acFlags & AC_HIT) &&
        (ABS((s16)(this->collider.base.ac->world.rot.y - this->actor.shape.rot.y)) > 0x5000)) {
        this->collider.base.acFlags &= ~AC_HIT;
        if (this->framesUntilDisable == -1) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_AMOS_DAMAGE);
            D_8089C1A0 += 1;
            D_8089C1A0 = CLAMP_MAX(D_8089C1A0, 4);
        }
        this->framesUntilDisable = 416;
        if (D_8089C1A0 == 4) {
            Flags_SetSwitch(play, this->actor.params);
            func_80078884(NA_SE_SY_CORRECT_CHIME);
        }
    }
    if (this->framesUntilDisable == -1) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
    Actor_SetFocus(&this->actor, 0.0f);
}

void BgMenkuriEye_Draw(Actor* thisx, PlayState* play) {
    BgMenkuriEye* this = (BgMenkuriEye*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    if (Flags_GetSwitch(play, this->actor.params)) {
        gDPSetEnvColor(POLY_XLU_DISP++, 200, 0, 0, 255);
    } else if (this->framesUntilDisable == -1) {
        gDPSetEnvColor(POLY_XLU_DISP++, 200, 0, 0, 0);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 200, 0, 0, 255);
    }
    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateZYX(this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, MTXMODE_APPLY);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_XLU_DISP++, gGTGEyeStatueEyeDL);
    CLOSE_DISPS(play->state.gfxCtx);
}

void BgMenkuriEye_Reset(void) {
    D_8089C1A0 = 0;
}