/*
 * File: z_en_skjneedle.c
 * Overlay: ovl_En_Skjneedle
 * Description: Skullkid Needle Attack
 */

#include "z_en_skjneedle.h"
#include "objects/object_skj/object_skj.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_9)

void EnSkjneedle_Init(Actor* thisx, PlayState* play);
void EnSkjneedle_Destroy(Actor* thisx, PlayState* play);
void EnSkjneedle_Update(Actor* thisx, PlayState* play);
void EnSkjneedle_Draw(Actor* thisx, PlayState* play);

s32 EnSkjNeedle_CollisionCheck(EnSkjneedle* this);

const ActorInit En_Skjneedle_InitVars = {
    ACTOR_EN_SKJNEEDLE,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_SKJ,
    sizeof(EnSkjneedle),
    (ActorFunc)EnSkjneedle_Init,
    (ActorFunc)EnSkjneedle_Destroy,
    (ActorFunc)EnSkjneedle_Update,
    (ActorFunc)EnSkjneedle_Draw,
    NULL,
};

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_HIT1,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 10, 4, -2, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 30, ICHAIN_STOP),
};

void EnSkjneedle_Init(Actor* thisx, PlayState* play) {
    EnSkjneedle* this = (EnSkjneedle*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinderType1(play, &this->collider, &this->actor, &sCylinderInit);
    ActorShape_Init(&this->actor.shape, 0, ActorShadow_DrawCircle, 20.0f);
    thisx->flags &= ~ACTOR_FLAG_0;
    Actor_SetScale(&this->actor, 0.01f);
}

void EnSkjneedle_Destroy(Actor* thisx, PlayState* play) {
    EnSkjneedle* this = (EnSkjneedle*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

s32 EnSkjNeedle_CollisionCheck(EnSkjneedle* this) {
    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        return 1;
    }
    return 0;
}

void EnSkjneedle_Update(Actor* thisx, PlayState* play2) {
    EnSkjneedle* this = (EnSkjneedle*)thisx;
    PlayState* play = play2;

    this->unusedTimer1++;
    if (this->killTimer != 0) {
        this->killTimer--;
    }
    if (EnSkjNeedle_CollisionCheck(this) || this->killTimer == 0) {
        Actor_Kill(&this->actor);
    } else {
        Actor_SetScale(&this->actor, 0.01f);
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        Actor_MoveForward(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 20.0f, 7);
    }
}

void EnSkjneedle_Draw(Actor* thisx, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gSkullKidNeedleDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
