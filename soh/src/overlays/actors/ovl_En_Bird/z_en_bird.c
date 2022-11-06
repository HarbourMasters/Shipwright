/*
 * File: z_en_bird.c
 * Overlay: ovl_En_Bird
 * Description: An unused brown bird
 */

#include "z_en_bird.h"
#include "objects/object_bird/object_bird.h"

#define FLAGS 0

void EnBird_Init(Actor* thisx, PlayState* play);
void EnBird_Destroy(Actor* thisx, PlayState* play);
void EnBird_Update(Actor* thisx, PlayState* play);
void EnBird_Draw(Actor* thisx, PlayState* play);

void func_809C1E00(EnBird* this, s16 params);
void func_809C1E40(EnBird* this, PlayState* play);
void func_809C1D60(EnBird* this, PlayState* play);
void func_809C1CAC(EnBird* this, s16 params);

const ActorInit En_Bird_InitVars = {
    ACTOR_EN_BIRD,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_BIRD,
    sizeof(EnBird),
    (ActorFunc)EnBird_Init,
    (ActorFunc)EnBird_Destroy,
    (ActorFunc)EnBird_Update,
    (ActorFunc)EnBird_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 5600, ICHAIN_STOP),
};

void EnBird_SetupAction(EnBird* this, EnBirdActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnBird_Init(Actor* thisx, PlayState* play) {
    EnBird* this = (EnBird*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Actor_SetScale(&this->actor, 0.01);
    SkelAnime_Init(play, &this->skelAnime, &gBirdSkel, &gBirdFlyAnim, NULL, NULL, 0);
    ActorShape_Init(&this->actor.shape, 5500, ActorShadow_DrawCircle, 4);
    this->unk_194 = 0;
    this->unk_198 = 0;
    this->unk_1C0 = 0x9C4;
    this->actor.colChkInfo.mass = 0;
    this->unk_1A8 = 1.5f;
    this->unk_1AC = 0.5f;
    this->unk_1A0 = 0.0f;
    this->unk_1A4 = 0.0f;
    this->unk_1B8 = 0.0f;
    this->unk_1B0 = 40.0f;
    this->unk_1BC = 70.0f;
    func_809C1CAC(this, this->actor.params);
}

void EnBird_Destroy(Actor* thisx, PlayState* play) {
}

void func_809C1CAC(EnBird* this, s16 params) {
    f32 frameCount = Animation_GetLastFrame(&gBirdFlyAnim);
    f32 playbackSpeed = this->unk_19C ? 0.0f : 1.0f;
    AnimationHeader* anim = &gBirdFlyAnim;

    this->unk_198 = Rand_S16Offset(5, 0x23);
    Animation_Change(&this->skelAnime, anim, playbackSpeed, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    EnBird_SetupAction(this, func_809C1D60);
}

void func_809C1D60(EnBird* this, PlayState* play) {
    f32 fVar2 = sinf(this->unk_1B4);

    this->actor.shape.yOffset = this->actor.shape.yOffset + fVar2 * this->unk_1A0;
    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 0.1f, 0.5f, 0.0f);

    if (this->unk_19C != 0) {
        this->skelAnime.playSpeed = this->actor.speedXZ + this->actor.speedXZ;
    }

    SkelAnime_Update(&this->skelAnime);
    this->unk_198 -= 1;

    if (this->unk_198 <= 0) {
        func_809C1E00(this, this->actor.params);
    }
}

void func_809C1E00(EnBird* this, s16 params) {
    this->unk_198 = Rand_S16Offset(0x14, 0x2D);
    EnBird_SetupAction(this, func_809C1E40);
}

void func_809C1E40(EnBird* this, PlayState* play) {
    f32 fVar4 = sinf(this->unk_1B4);

    this->actor.shape.yOffset += fVar4 * this->unk_1A0;
    Math_SmoothStepToF(&this->actor.speedXZ, this->unk_1A8, 0.1f, this->unk_1AC, 0.0f);

    if (this->unk_1B0 < Math_Vec3f_DistXZ(&this->actor.world.pos, &this->actor.home.pos) || this->unk_198 < 4) {
        Math_StepToAngleS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos),
                          this->unk_1C0);
    } else {
        fVar4 = sinf(this->unk_1B4);
        this->actor.world.rot.y += (s16)(fVar4 * this->unk_1A4);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    SkelAnime_Update(&this->skelAnime);
    this->unk_198 -= 1;
    if (this->unk_198 < 0) {
        func_809C1CAC(this, this->actor.params);
    }
}

void EnBird_Update(Actor* thisx, PlayState* play) {
    EnBird* this = (EnBird*)thisx;

    this->unk_1B4 += this->unk_1B8;
    this->actionFunc(this, play);
}

void EnBird_Draw(Actor* thisx, PlayState* play) {
    EnBird* this = (EnBird*)thisx;

    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, NULL);
}
