#include "z_en_ma2.h"
#include "objects/object_ma2/object_ma2.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_25)

void EnMa2_Init(Actor* thisx, GlobalContext* globalCtx);
void EnMa2_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnMa2_Update(Actor* thisx, GlobalContext* globalCtx);
void EnMa2_Draw(Actor* thisx, GlobalContext* globalCtx);

u16 func_80AA19A0(GlobalContext* globalCtx, Actor* this);
s16 func_80AA1A38(GlobalContext* globalCtx, Actor* this);

void func_80AA1AE4(EnMa2* this, GlobalContext* globalCtx);
s32 func_80AA1C68(EnMa2* this);
void EnMa2_UpdateEyes(EnMa2* this);
void func_80AA1DB4(EnMa2* this, GlobalContext* globalCtx);
void func_80AA2018(EnMa2* this, GlobalContext* globalCtx);
void func_80AA204C(EnMa2* this, GlobalContext* globalCtx);
void func_80AA20E4(EnMa2* this, GlobalContext* globalCtx);
void func_80AA21C8(EnMa2* this, GlobalContext* globalCtx);

const ActorInit En_Ma2_InitVars = {
    ACTOR_EN_MA2,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_MA2,
    sizeof(EnMa2),
    (ActorFunc)EnMa2_Init,
    (ActorFunc)EnMa2_Destroy,
    (ActorFunc)EnMa2_Update,
    (ActorFunc)EnMa2_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 18, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum {
    /* 0 */ ENMA2_ANIM_0,
    /* 1 */ ENMA2_ANIM_1,
    /* 2 */ ENMA2_ANIM_2,
    /* 3 */ ENMA2_ANIM_3,
    /* 4 */ ENMA2_ANIM_4
} EnMa2Animation;

static AnimationFrameCountInfo sAnimationInfo[] = {
    { &gMalonAdultIdleAnim, 1.0f, ANIMMODE_LOOP, 0.0f },       { &gMalonAdultIdleAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gMalonAdultStandStillAnim, 1.0f, ANIMMODE_LOOP, 0.0f }, { &gMalonAdultSingAnim, 1.0f, ANIMMODE_LOOP, 0.0f },
    { &gMalonAdultSingAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
};

u16 func_80AA19A0(GlobalContext* globalCtx, Actor* thisx) {
    u16 faceReaction = Text_GetFaceReaction(globalCtx, 23);

    if (faceReaction != 0) {
        return faceReaction;
    }
    if (gSaveContext.eventChkInf[1] & 0x100) {
        return 0x2056;
    }
    if (IS_NIGHT) {
        if (gSaveContext.infTable[8] & 0x1000) {
            return 0x2052;
        } else if (gSaveContext.infTable[8] & 0x4000) {
            return 0x2051;
        } else {
            return 0x2050;
        }
    }
    return 0x204C;
}

s16 func_80AA1A38(GlobalContext* globalCtx, Actor* thisx) {
    s16 ret = 1;

    switch (Message_GetState(&globalCtx->msgCtx)) {
        case TEXT_STATE_CLOSING:
            switch (thisx->textId) {
                case 0x2051:
                    gSaveContext.infTable[8] |= 0x1000;
                    ret = 2;
                    break;
                case 0x2053:
                    gSaveContext.infTable[8] |= 0x2000;
                    ret = 0;
                    break;
                default:
                    ret = 0;
                    break;
            }
            break;
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
        case TEXT_STATE_DONE:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_9:
            break;
    }
    return ret;
}

void func_80AA1AE4(EnMa2* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 phi_a3;

    if ((this->unk_1E0.unk_00 == 0) && (this->skelAnime.animation == &gMalonAdultSingAnim)) {
        phi_a3 = 1;
    } else {
        phi_a3 = 0;
    }

    this->unk_1E0.unk_18 = player->actor.world.pos;
    this->unk_1E0.unk_14 = 0.0f;

    func_80034A14(&this->actor, &this->unk_1E0, 0, phi_a3);
}

u16 func_80AA1B58(EnMa2* this, GlobalContext* globalCtx) {
    if (LINK_IS_CHILD) {
        return 0;
    }
    if (!(gSaveContext.eventChkInf[1] & 0x100) && (globalCtx->sceneNum == SCENE_MALON_STABLE) && IS_DAY &&
        (this->actor.shape.rot.z == 5)) {
        return 1;
    }
    if (!(gSaveContext.eventChkInf[1] & 0x100) && (globalCtx->sceneNum == SCENE_SPOT20) && IS_NIGHT &&
        (this->actor.shape.rot.z == 6)) {
        return 2;
    }
    if (!(gSaveContext.eventChkInf[1] & 0x100) || (globalCtx->sceneNum != SCENE_SPOT20)) {
        return 0;
    }
    if ((this->actor.shape.rot.z == 7) && IS_DAY) {
        return 3;
    }
    if ((this->actor.shape.rot.z == 8) && IS_NIGHT) {
        return 3;
    }
    return 0;
}

s32 func_80AA1C68(EnMa2* this) {
    if (this->skelAnime.animation != &gMalonAdultSingAnim) {
        return 0;
    }
    if (this->unk_1E0.unk_00 != 0) {
        return 0;
    }
    this->blinkTimer = 0;
    if (this->eyeIndex != 2) {
        return 0;
    }
    this->mouthIndex = 2;
    return 1;
}

void EnMa2_UpdateEyes(EnMa2* this) {
    if ((!func_80AA1C68(this)) && (DECR(this->blinkTimer) == 0)) {
        this->eyeIndex += 1;
        if (this->eyeIndex >= 3) {
            this->blinkTimer = Rand_S16Offset(30, 30);
            this->eyeIndex = 0;
        }
    }
}

void EnMa2_ChangeAnim(EnMa2* this, s32 index) {
    f32 frameCount = Animation_GetLastFrame(sAnimationInfo[index].animation);

    Animation_Change(&this->skelAnime, sAnimationInfo[index].animation, 1.0f, 0.0f, frameCount,
                     sAnimationInfo[index].mode, sAnimationInfo[index].morphFrames);
}

void func_80AA1DB4(EnMa2* this, GlobalContext* globalCtx) {
    if (this->skelAnime.animation == &gMalonAdultSingAnim) {
        if (this->unk_1E0.unk_00 == 0) {
            if (this->unk_20A != 0) {
                func_800F6584(0);
                this->unk_20A = 0;
            }
        } else {
            if (this->unk_20A == 0) {
                func_800F6584(1);
                this->unk_20A = 1;
            }
        }
    }
}

void EnMa2_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnMa2* this = (EnMa2*)thisx;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 18.0f);
    SkelAnime_InitFlex(globalCtx, &this->skelAnime, &gMalonAdultSkel, NULL, NULL, NULL, 0);
    Collider_InitCylinder(globalCtx, &this->collider);
    Collider_SetCylinder(globalCtx, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(22), &sColChkInfoInit);

    switch (func_80AA1B58(this, globalCtx)) {
        case 1:
            EnMa2_ChangeAnim(this, ENMA2_ANIM_2);
            this->actionFunc = func_80AA2018;
            break;
        case 2:
            EnMa2_ChangeAnim(this, ENMA2_ANIM_3);
            this->actionFunc = func_80AA204C;
            break;
        case 3:
            if (gSaveContext.infTable[8] & 0x2000) {
                EnMa2_ChangeAnim(this, ENMA2_ANIM_0);
            } else {
                EnMa2_ChangeAnim(this, ENMA2_ANIM_3);
            }
            this->actionFunc = func_80AA2018;
            break;
        case 0:
            Actor_Kill(&this->actor);
            return;
    }

    Actor_UpdateBgCheckInfo(globalCtx, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.targetMode = 6;
    this->unk_1E0.unk_00 = 0;
}

void EnMa2_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnMa2* this = (EnMa2*)thisx;

    SkelAnime_Free(&this->skelAnime, globalCtx);
    Collider_DestroyCylinder(globalCtx, &this->collider);
}

void func_80AA2018(EnMa2* this, GlobalContext* globalCtx) {
    if (this->unk_1E0.unk_00 == 2) {
        this->actor.flags &= ~ACTOR_FLAG_16;
        this->unk_1E0.unk_00 = 0;
    }
}

void func_80AA204C(EnMa2* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (player->stateFlags2 & 0x1000000) {
        player->unk_6A8 = &this->actor;
        player->stateFlags2 |= 0x2000000;
        func_8010BD58(globalCtx, OCARINA_ACTION_CHECK_EPONA);
        this->actionFunc = func_80AA20E4;
    } else if (this->actor.xzDistToPlayer < 30.0f + (f32)this->collider.dim.radius) {
        player->stateFlags2 |= 0x800000;
    }
}

void func_80AA20E4(EnMa2* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (globalCtx->msgCtx.ocarinaMode >= OCARINA_MODE_04) {
        this->actionFunc = func_80AA204C;
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
    } else if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_03) {
        Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        this->unk_208 = 0x1E;
        gSaveContext.infTable[8] |= 0x4000;
        this->actionFunc = func_80AA21C8;
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
    } else {
        player->stateFlags2 |= 0x800000;
    }
}

void func_80AA21C8(EnMa2* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (DECR(this->unk_208)) {
        player->stateFlags2 |= 0x800000;
    } else {
        if (this->unk_1E0.unk_00 == 0) {
            this->actor.flags |= ACTOR_FLAG_16;
            Message_CloseTextbox(globalCtx);
        } else {
            this->actor.flags &= ~ACTOR_FLAG_16;
            this->actionFunc = func_80AA2018;
        }
    }
}

void EnMa2_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnMa2* this = (EnMa2*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    SkelAnime_Update(&this->skelAnime);
    EnMa2_UpdateEyes(this);
    this->actionFunc(this, globalCtx);
    func_80AA1DB4(this, globalCtx);
    func_80AA1AE4(this, globalCtx);
    if (this->actionFunc != func_80AA20E4) {
        func_800343CC(globalCtx, &this->actor, &this->unk_1E0.unk_00, (f32)this->collider.dim.radius + 30.0f,
                      func_80AA19A0, func_80AA1A38);
    }
}

s32 EnMa2_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnMa2* this = (EnMa2*)thisx;
    Vec3s vec;

    if ((limbIndex == MALON_ADULT_LEFT_THIGH_LIMB) || (limbIndex == MALON_ADULT_RIGHT_THIGH_LIMB)) {
        *dList = NULL;
    }
    if (limbIndex == MALON_ADULT_HEAD_LIMB) {
        Matrix_Translate(1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        vec = this->unk_1E0.unk_08;
        Matrix_RotateX((vec.y / 32768.0f) * M_PI, MTXMODE_APPLY);
        Matrix_RotateZ((vec.x / 32768.0f) * M_PI, MTXMODE_APPLY);
        Matrix_Translate(-1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limbIndex == MALON_ADULT_CHEST_AND_NECK_LIMB) {
        vec = this->unk_1E0.unk_0E;
        Matrix_RotateY((-vec.y / 32768.0f) * M_PI, MTXMODE_APPLY);
        Matrix_RotateX((-vec.x / 32768.0f) * M_PI, MTXMODE_APPLY);
    }
    if ((limbIndex == MALON_ADULT_CHEST_AND_NECK_LIMB) || (limbIndex == MALON_ADULT_LEFT_SHOULDER_LIMB) ||
        (limbIndex == MALON_ADULT_RIGHT_SHOULDER_LIMB)) {
        rot->y += Math_SinS(this->unk_212[limbIndex].y) * 200.0f;
        rot->z += Math_CosS(this->unk_212[limbIndex].z) * 200.0f;
    }
    return false;
}

void EnMa2_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnMa2* this = (EnMa2*)thisx;
    Vec3f vec = { 900.0f, 0.0f, 0.0f };

    OPEN_DISPS(globalCtx->state.gfxCtx, __FILE__, __LINE__);

    if (limbIndex == MALON_ADULT_HEAD_LIMB) {
        Matrix_MultVec3f(&vec, &this->actor.focus.pos);
    }
    if ((limbIndex == MALON_ADULT_LEFT_HAND_LIMB) && (this->skelAnime.animation == &gMalonAdultStandStillAnim)) {
        gSPDisplayList(POLY_OPA_DISP++, gMalonAdultBasketDL);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, __FILE__, __LINE__);
}

void EnMa2_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static void* sMouthTextures[] = { gMalonAdultMouthNeutralTex, gMalonAdultMouthSadTex, gMalonAdultMouthHappyTex };
    static void* sEyeTextures[] = { gMalonAdultEyeOpenTex, gMalonAdultEyeHalfTex, gMalonAdultEyeClosedTex };

    EnMa2* this = (EnMa2*)thisx;
    Camera* camera;
    f32 someFloat;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, __FILE__, __LINE__);

    camera = GET_ACTIVE_CAM(globalCtx);
    someFloat = Math_Vec3f_DistXZ(&this->actor.world.pos, &camera->eye);
    func_800F6268(someFloat, NA_BGM_LONLON);
    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sMouthTextures[this->mouthIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[this->eyeIndex]));

    SkelAnime_DrawFlexOpa(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnMa2_OverrideLimbDraw, EnMa2_PostLimbDraw, this);

    CLOSE_DISPS(globalCtx->state.gfxCtx, __FILE__, __LINE__);
}
