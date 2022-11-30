/*
 * File: z_en_hs2.c
 * Overlay: ovl_En_Hs2
 * Description: Carpenter's Son (Child Link version)
 */

#include "z_en_hs2.h"
#include "vt.h"
#include "objects/object_hs/object_hs.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnHs2_Init(Actor* thisx, PlayState* play);
void EnHs2_Destroy(Actor* thisx, PlayState* play);
void EnHs2_Update(Actor* thisx, PlayState* play);
void EnHs2_Draw(Actor* thisx, PlayState* play);
void func_80A6F1A4(EnHs2* this, PlayState* play);

const ActorInit En_Hs2_InitVars = {
    ACTOR_EN_HS2,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_HS,
    sizeof(EnHs2),
    (ActorFunc)EnHs2_Init,
    (ActorFunc)EnHs2_Destroy,
    (ActorFunc)EnHs2_Update,
    (ActorFunc)EnHs2_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 40, 40, 0, { 0, 0, 0 } },
};

void EnHs2_Init(Actor* thisx, PlayState* play) {
    EnHs2* this = (EnHs2*)thisx;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_hs_Skel_006260, &object_hs_Anim_0005C0, this->jointTable,
                       this->morphTable, 16);
    Animation_PlayLoop(&this->skelAnime, &object_hs_Anim_0005C0);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->actor, 0.01f);
    osSyncPrintf(VT_FGCOL(CYAN) " ヒヨコの店(子人の時) \n" VT_RST);
    this->actionFunc = func_80A6F1A4;
    this->unk_2A8 = 0;
    this->actor.targetMode = 6;
}

void EnHs2_Destroy(Actor* thisx, PlayState* play) {
    EnHs2* this = (EnHs2*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

s32 func_80A6F0B4(EnHs2* this, PlayState* play, u16 textId, EnHs2ActionFunc actionFunc) {
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->actionFunc = actionFunc;
        return 1;
    }

    this->actor.textId = textId;
    if (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) < 0x2151 &&
        this->actor.xzDistToPlayer < 100.0f) {
        this->unk_2A8 |= 0x1;
        func_8002F2CC(&this->actor, play, 100.0f);
    }
    return 0;
}

void func_80A6F164(EnHs2* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->actionFunc = func_80A6F1A4;
    }
    this->unk_2A8 |= 0x1;
}

void func_80A6F1A4(EnHs2* this, PlayState* play) {
    u16 textId;

    textId = Text_GetFaceReaction(play, 9);
    if (textId == 0) {
        textId = 0x5069;
    }

    func_80A6F0B4(this, play, textId, func_80A6F164);
}

void EnHs2_Update(Actor* thisx, PlayState* play) {
    EnHs2* this = (EnHs2*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    if (SkelAnime_Update(&this->skelAnime) != 0) {
        this->skelAnime.curFrame = 0.0f;
    }
    this->actionFunc(this, play);
    if (this->unk_2A8 & 0x1) {
        func_80038290(play, &this->actor, &this->unk_29C, &this->unk_2A2, this->actor.focus.pos);
        this->unk_2A8 &= ~1;
    } else {
        Math_SmoothStepToS(&this->unk_29C.x, 12800, 6, 6200, 100);
        Math_SmoothStepToS(&this->unk_29C.y, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->unk_2A2.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->unk_2A2.y, 0, 6, 6200, 100);
    }
}

s32 EnHs2_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnHs2* this = (EnHs2*)thisx;

    switch (limbIndex) {
        case 12:
        case 13:
            *dList = NULL;
            return false;
        case 9:
            rot->x += this->unk_29C.y;
            rot->z += this->unk_29C.x;
            break;
        case 10:
            *dList = NULL;
            return false;
        case 11:
            *dList = NULL;
            return false;
    }
    return false;
}

void EnHs2_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f D_80A6F4CC = { 300.0f, 1000.0f, 0.0f };
    EnHs2* this = (EnHs2*)thisx;

    if (limbIndex == 9) {
        Matrix_MultVec3f(&D_80A6F4CC, &this->actor.focus.pos);
    }
}

void EnHs2_Draw(Actor* thisx, PlayState* play) {
    EnHs2* this = (EnHs2*)thisx;

    Gfx_SetupDL_37Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnHs2_OverrideLimbDraw, EnHs2_PostLimbDraw, this);
}
