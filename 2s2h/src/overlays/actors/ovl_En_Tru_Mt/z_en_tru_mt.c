/*
 * File: z_en_tru_mt.c
 * Overlay: ovl_En_Tru_Mt
 * Description: Koume's Target Game - Koume on Broom
 */

#include "z_en_tru_mt.h"
#include "overlays/actors/ovl_En_Jc_Mato/z_en_jc_mato.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnTruMt_Init(Actor* thisx, PlayState* play);
void EnTruMt_Destroy(Actor* thisx, PlayState* play);
void EnTruMt_Update(Actor* thisx, PlayState* play);
void EnTruMt_Draw(Actor* thisx, PlayState* play);

void func_80B76A64(EnTruMt* this, PlayState* play);
void func_80B76BB8(EnTruMt* this, PlayState* play);
void func_80B76C38(EnTruMt* this, PlayState* play);

ActorProfile En_Tru_Mt_Profile = {
    /**/ ACTOR_EN_TRU_MT,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_TRU,
    /**/ sizeof(EnTruMt),
    /**/ EnTruMt_Init,
    /**/ EnTruMt_Destroy,
    /**/ EnTruMt_Update,
    /**/ EnTruMt_Draw,
};

static ColliderSphereInit sSphereInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_SPHERE,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 0, { { 0, 0, 0 }, 22 }, 100 },
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0xF),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0x0),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(1, 0x0),
    /* Fire arrow     */ DMG_ENTRY(1, 0xF),
    /* Ice arrow      */ DMG_ENTRY(1, 0xF),
    /* Light arrow    */ DMG_ENTRY(1, 0xF),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, 0x0),
    /* Zora barrier   */ DMG_ENTRY(1, 0x0),
    /* Normal shield  */ DMG_ENTRY(1, 0x0),
    /* Light ray      */ DMG_ENTRY(1, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(1, 0x0),
    /* Normal Roll    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(1, 0x0),
    /* Unblockable    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(1, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0x0),
};

typedef enum KoumeMtAnimation {
    /* 0x00 */ KOUME_MT_ANIM_INJURED_LYING_DOWN,
    /* 0x01 */ KOUME_MT_ANIM_INJURED_LYING_DOWN_MORPH,
    /* 0x02 */ KOUME_MT_ANIM_TRY_GET_UP,
    /* 0x03 */ KOUME_MT_ANIM_INJURED_RAISE_HEAD,
    /* 0x04 */ KOUME_MT_ANIM_INJURED_TALK,
    /* 0x05 */ KOUME_MT_ANIM_INJURED_HEAD_UP,
    /* 0x06 */ KOUME_MT_ANIM_INJURED_HEAD_UP_MORPH,
    /* 0x07 */ KOUME_MT_ANIM_TAKE,
    /* 0x08 */ KOUME_MT_ANIM_SHAKE,
    /* 0x09 */ KOUME_MT_ANIM_DRINK,
    /* 0x0A */ KOUME_MT_ANIM_FINISHED_DRINKING,
    /* 0x0B */ KOUME_MT_ANIM_HEALED,
    /* 0x0C */ KOUME_MT_ANIM_HOVER1,
    /* 0x0D */ KOUME_MT_ANIM_TAKE_OFF,
    /* 0x0E */ KOUME_MT_ANIM_FLY,
    /* 0x0F */ KOUME_MT_ANIM_HOVER2,
    /* 0x10 */ KOUME_MT_ANIM_MAX
} KoumeMtAnimation;

static AnimationInfoS sAnimationInfo[KOUME_MT_ANIM_MAX] = {
    { &gKoumeInjuredLyingDownAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // KOUME_MT_ANIM_INJURED_LYING_DOWN
    { &gKoumeInjuredLyingDownAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // KOUME_MT_ANIM_INJURED_LYING_DOWN_MORPH
    { &gKoumeTryGetUpAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },         // KOUME_MT_ANIM_TRY_GET_UP
    { &gKoumeInjuredRaiseHeadAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 }, // KOUME_MT_ANIM_INJURED_RAISE_HEAD
    { &gKoumeInjuredTalkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },      // KOUME_MT_ANIM_INJURED_TALK
    { &gKoumeInjuredHeadUpAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },     // KOUME_MT_ANIM_INJURED_HEAD_UP
    { &gKoumeInjuredHeadUpAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },    // KOUME_MT_ANIM_INJURED_HEAD_UP_MORPH
    { &gKoumeTakeAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },              // KOUME_MT_ANIM_TAKE
    { &gKoumeShakeAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },             // KOUME_MT_ANIM_SHAKE
    { &gKoumeDrinkAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },            // KOUME_MT_ANIM_DRINK
    { &gKoumeFinishedDrinkingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // KOUME_MT_ANIM_FINISHED_DRINKING
    { &gKoumeHealedAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },           // KOUME_MT_ANIM_HEALED
    { &gKoumeHoverAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },             // KOUME_MT_ANIM_HOVER1
    { &gKoumeTakeOffAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },          // KOUME_MT_ANIM_TAKE_OFF
    { &gKoumeFlyAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },               // KOUME_MT_ANIM_FLY
    { &gKoumeHoverAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },             // KOUME_MT_ANIM_HOVER2
};

s32 EnTruMt_ChangeAnim(SkelAnime* skelAnime, s16 animIndex) {
    s16 endFrame;
    s16 startFrame;
    s32 didChange = false;

    if ((animIndex >= KOUME_MT_ANIM_INJURED_LYING_DOWN) && (animIndex < KOUME_MT_ANIM_MAX)) {
        endFrame = sAnimationInfo[animIndex].frameCount;
        if (endFrame < 0) {
            endFrame = Animation_GetLastFrame(sAnimationInfo[animIndex].animation);
        }

        startFrame = sAnimationInfo[animIndex].startFrame;
        if (startFrame < 0) {
            startFrame = Animation_GetLastFrame(sAnimationInfo[animIndex].animation);
        }

        Animation_Change(skelAnime, sAnimationInfo[animIndex].animation, sAnimationInfo[animIndex].playSpeed,
                         startFrame, endFrame, sAnimationInfo[animIndex].mode, sAnimationInfo[animIndex].morphFrames);
        didChange = true;
    }
    return didChange;
}

void func_80B76110(EnTruMt* this) {
    if (DECR(this->blinkTimer) == 0) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex >= 3) {
            this->blinkTimer = Rand_S16Offset(30, 30);
            this->eyeTexIndex = 0;
        }
    }
}

void func_80B76188(EnTruMt* this) {
    this->unk_38E.x = Math_CosS(this->unk_38A) * this->unk_38C;
    this->unk_38A += 0x1555;
    Math_SmoothStepToS(&this->unk_38C, 0, 4, 0x3E8, 1);
}

s32 func_80B761FC(EnTruMt* this, PlayState* play) {
    this->collider.dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider.dim.worldSphere.center.y = this->actor.world.pos.y + 16.0f;
    this->collider.dim.worldSphere.center.z = this->actor.world.pos.z;

    if ((this->collider.base.acFlags & AC_HIT) && (this->actor.colChkInfo.damageEffect == 0xF)) {
        if (!(((EnJcMato*)this->actor.child)->collider.base.acFlags & AC_HIT) &&
            (this->actor.child->colChkInfo.damageEffect != 0xF)) {
            this->collider.base.acFlags &= ~AC_HIT;
            if (this->unk_3A4 == 0) {
                this->unk_3A4 = 1;
                Actor_PlaySfx(&this->actor, NA_SE_EN_KOUME_DAMAGE);
            } else {
                this->unk_3A4 = 0;
                Actor_PlaySfx(&this->actor, NA_SE_EN_KOUME_DAMAGE2);
            }
            if (GameInteractor_Should(VB_KOUME_TAKE_DAMAGE, true)) {
                play->interfaceCtx.minigameHiddenPoints = 1;
            }
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 25);
            return true;
        }
    }

    if (this->unk_38C >= 0xB7) {
        func_80B76188(this);
    } else {
        this->unk_38E.x = 0;
    }

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    return false;
}

s32 func_80B76368(EnTruMt* this, PlayState* play) {
    Actor* bgActor = play->actorCtx.actorLists[ACTORCAT_BG].first;

    while (bgActor != NULL) {
        if (bgActor->id == ACTOR_BG_INGATE) {
            this->unk_394 = bgActor->shape.rot.y;
            this->unk_398 = bgActor->world.pos;
            return true;
        }
        bgActor = bgActor->next;
    }

    return false;
}

s32 func_80B763C4(EnTruMt* this, PlayState* play) {
    Actor* actorIter = NULL;

    while (true) {
        actorIter = SubS_FindActor(play, actorIter, ACTORCAT_NPC, ACTOR_EN_TRU_MT);

        if (actorIter == NULL) {
            break;
        }

        if ((EnTruMt*)actorIter != this) {
            return true;
        }

        if (actorIter->next == NULL) {
            break;
        }
        actorIter = actorIter->next;
    };

    return false;
}

void func_80B76440(EnTruMt* this, PlayState* play) {
    if (func_80B76368(this, play)) {
        s16 temp_v1 = this->unk_394 - Math_Vec3f_Yaw(&this->unk_398, &this->actor.world.pos);

        if ((temp_v1 <= -0x2000) || (temp_v1 >= 0x2000)) {
            Math_ApproachF(&this->actor.speed, 7.0f, 0.2f, 1.0f);
        } else if (this->actor.xzDistToPlayer < 300.0f) {
            Math_ApproachF(&this->actor.speed, 7.0f, 0.2f, 1.0f);
        } else if (this->actor.xzDistToPlayer > 500.0f) {
            Math_ApproachF(&this->actor.speed, 2.5f, 0.2f, 1.0f);
        } else {
            Math_ApproachF(&this->actor.speed, 4.0f, 0.2f, 1.0f);
        }
    }
}

f32 func_80B76540(Path* path, s32 arg1, Vec3f* arg2, Vec3s* arg3) {
    s32 pad;
    Vec3f sp20;

    if (path != NULL) {
        Vec3s* points = Lib_SegmentedToVirtual(path->points);

        points = &points[arg1];
        sp20.x = points->x;
        sp20.y = points->y;
        sp20.z = points->z;
    }
    arg3->y = Math_Vec3f_Yaw(arg2, &sp20);
    arg3->x = Math_Vec3f_Pitch(arg2, &sp20);

    return sp20.y - arg2->y;
}

s32 EnTruMt_HasReachedPoint(EnTruMt* this, Path* path, s32 pointIndex) {
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    s32 count = path->count;
    s32 index = pointIndex;
    s32 reached = false;
    f32 diffX;
    f32 diffZ;
    f32 px;
    f32 pz;
    f32 d;
    Vec3f point;

    Math_Vec3s_ToVec3f(&point, &points[index]);

    if (index == 0) {
        diffX = points[1].x - points[0].x;
        diffZ = points[1].z - points[0].z;
    } else if (index == (count - 1)) {
        diffX = points[count - 1].x - points[count - 2].x;
        diffZ = points[count - 1].z - points[count - 2].z;
    } else {
        diffX = points[index + 1].x - points[index - 1].x;
        diffZ = points[index + 1].z - points[index - 1].z;
    }

    Math3D_RotateXZPlane(&point, RAD_TO_BINANG(Math_FAtan2F(diffX, diffZ)), &px, &pz, &d);

    if (((px * this->actor.world.pos.x) + (pz * this->actor.world.pos.z) + d) > 0.0f) {
        reached = true;
    }

    return reached;
}

void func_80B7679C(EnTruMt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp40;
    Vec3f sp34;

    Math_SmoothStepToS(&this->unk_34A, this->actor.yawTowardsPlayer - this->actor.shape.rot.y - 0x4000, 4, 0x38E0, 1);
    this->unk_34A = CLAMP(this->unk_34A, -0x38E0, 0x38E0);

    sp40 = player->actor.world.pos;
    sp40.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;

    sp34 = this->actor.world.pos;
    sp34.y += 30.0f;

    Math_SmoothStepToS(&this->unk_348, Math_Vec3f_Pitch(&sp34, &sp40), 4, 0x1C70, 1);
    this->unk_348 = CLAMP(this->unk_348, -0x1C70, 0x1C70);
}

s32 func_80B768F0(EnTruMt* this, PlayState* play) {
    if (this->unk_328 & 0x10) {
        func_80B7679C(this, play);
    }
    return true;
}

void func_80B76924(EnTruMt* this) {
    this->unk_38E.z = DEG_TO_BINANG(Math_SinS(this->unk_388) * 30.0f);
    this->unk_388 += 0x400;
}

void func_80B76980(EnTruMt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (GameInteractor_Should(VB_FAIL_BOAT_ARCHERY, gSaveContext.minigameHiddenScore >= 10)) {
        Message_StartTextbox(play, 0x87F, &this->actor);
        SET_EVENTINF(EVENTINF_36);
        SET_EVENTINF(EVENTINF_40);
        player->stateFlags3 &= ~PLAYER_STATE3_400;
        this->actor.speed = 0.0f;
        this->actionFunc = func_80B76BB8;
    } else if (CHECK_EVENTINF(EVENTINF_40)) {
        if (((void)0, gSaveContext.minigameScore) > HS_GET_BOAT_ARCHERY_HIGH_SCORE()) {
            HS_SET_BOAT_ARCHERY_HIGH_SCORE((u32)((void)0, gSaveContext.minigameScore));
            SET_EVENTINF(EVENTINF_37);
        }
    }
}

void func_80B76A64(EnTruMt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3s sp34;

    func_80B76924(this);
    func_80B76980(this, play);
    player->stateFlags3 |= PLAYER_STATE3_400;

    if (this->path != NULL) {
        func_80B76540(this->path, this->unk_36C, &this->actor.world.pos, &sp34);
        Math_SmoothStepToS(&this->actor.world.rot.y, sp34.y, 4, 0x3E8, 1);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        Math_SmoothStepToS(&this->actor.shape.rot.x, sp34.x, 4, 0x3E8, 1);

        this->actor.world.rot.x = -this->actor.shape.rot.x;
        this->actor.shape.rot.z = CLAMP(this->actor.world.rot.y, -0x1770, 0x1770);
        this->actor.world.rot.z = this->actor.shape.rot.z;

        if (EnTruMt_HasReachedPoint(this, this->path, this->unk_36C)) {
            if (this->unk_36C >= (this->path->count - 1)) {
                this->actionFunc = func_80B76C38;
                this->actor.speed = 0.0f;
                return;
            }
            this->unk_36C++;
        }
        func_80B76440(this, play);
    }
}

void func_80B76BB8(EnTruMt* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) {
        if (Message_ShouldAdvance(play)) {
            play->nextEntrance = ENTRANCE(TOURIST_INFORMATION, 1);
            play->transitionType = TRANS_TYPE_FADE_WHITE;
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
            play->transitionTrigger = TRANS_TRIGGER_START;
        }
    }
}

void func_80B76C38(EnTruMt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    func_80B76924(this);
    func_80B76980(this, play);
    player->stateFlags3 |= PLAYER_STATE3_400;
}

void EnTruMt_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnTruMt* this = (EnTruMt*)thisx;

    if (!CHECK_EVENTINF(EVENTINF_35)) {
        Actor_Kill(&this->actor);
        return;
    }

    if (func_80B763C4(this, play)) {
        Actor_Kill(&this->actor);
        return;
    }

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gKoumeSkel, NULL, this->jointTable, this->morphTable, KOUME_LIMB_MAX);

    Collider_InitSphere(play, &this->collider);
    Collider_SetSphere(play, &this->collider, &this->actor, &sSphereInit);

    this->collider.dim.worldSphere.radius = 22;
    this->actor.colChkInfo.damageTable = &sDamageTable;
    this->path = SubS_GetPathByIndex(play, ENTRUMT_GET_FF(&this->actor), ENTRUMT_PATH_INDEX_NONE);
    this->actor.attentionRangeType = ATTENTION_RANGE_0;

    Actor_SetScale(&this->actor, 0.008f);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);

    this->unk_328 = 0;
    this->actor.room = -1;
    this->path = SubS_GetPathByIndex(play, ENTRUMT_GET_FC00(&this->actor), ENTRUMT_PATH_INDEX_NONE);
    EnTruMt_ChangeAnim(&this->skelAnime, KOUME_MT_ANIM_FLY);
    this->actionFunc = func_80B76A64;
}

void EnTruMt_Destroy(Actor* thisx, PlayState* play) {
    EnTruMt* this = (EnTruMt*)thisx;

    Collider_DestroySphere(play, &this->collider);
}

void EnTruMt_Update(Actor* thisx, PlayState* play) {
    EnTruMt* this = (EnTruMt*)thisx;

    func_80B768F0(this, play);
    SkelAnime_Update(&this->skelAnime);

    this->actionFunc(this, play);

    func_80B76110(this);
    Actor_SetFocus(&this->actor, 34.0f);

    if (!CHECK_EVENTINF(EVENTINF_40)) {
        func_80B761FC(this, play);
    }

    Actor_MoveWithoutGravity(&this->actor);
}

void func_80B76ED4(s16 arg0, s16 arg1, Vec3f* arg2, Vec3s* arg3, s32 arg4) {
    Vec3f sp7C;
    Vec3f sp70 = gZeroVec3f;
    Vec3s sp68;
    MtxF sp28;

    Matrix_MultVec3f(&sp70, &sp7C);
    Matrix_Get(&sp28);
    Matrix_MtxFToYXZRot(&sp28, &sp68, false);

    *arg2 = sp7C;

    if (arg4) {
        sp68.x += arg0;
        sp68.y += arg1;
        Math_SmoothStepToS(&arg3->x, sp68.x, 4, 0x1FFE, 1);
        Math_SmoothStepToS(&arg3->y, sp68.y, 4, 0x1FFE, 1);
        Math_SmoothStepToS(&arg3->z, sp68.z, 4, 0x1FFE, 1);
    } else {
        arg3->x = sp68.x;
        arg3->y = sp68.y;
        arg3->z = sp68.z;
    }
}

s32 EnTruMt_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80B7765C = { 3000.0f, -800.0f, 0.0f };
    EnTruMt* this = (EnTruMt*)thisx;

    if (limbIndex == KOUME_LIMB_HEAD) {
        Matrix_MultVec3f(&gZeroVec3f, &this->unk_35C);
    }

    if (limbIndex == KOUME_LIMB_HEAD) {
        Matrix_MultVec3f(&D_80B7765C, &this->unk_350);
    }
    return false;
}

void EnTruMt_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80B77668 = { 0.0f, 0.0f, -3000.0f };
    s32 pad;
    EnTruMt* this = (EnTruMt*)thisx;
    MtxF* sp54;
    s32 phi_v0;

    if (limbIndex == KOUME_LIMB_HEAD) {
        if (this->unk_328 & 0x10) {
            phi_v0 = true;
        } else {
            phi_v0 = false;
        }
        func_80B76ED4(this->unk_348, this->unk_34A, &this->unk_33C, &this->unk_336, phi_v0);
        Matrix_Translate(this->unk_33C.x, this->unk_33C.y, this->unk_33C.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateYS(this->unk_336.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_336.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->unk_336.z, MTXMODE_APPLY);
    }

    if (limbIndex == KOUME_LIMB_BROOM) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        OPEN_DISPS(play->state.gfxCtx);

        Matrix_MultVec3f(&D_80B77668, &this->unk_370);
        Matrix_Translate(this->unk_370.x, this->unk_370.y, this->unk_370.z, MTXMODE_NEW);
        Matrix_RotateYS(BINANG_ROT180(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play))), MTXMODE_APPLY);
        Matrix_RotateZS(this->unk_38E.z, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_38E.x, MTXMODE_APPLY);
        Matrix_Scale(0.008f, 0.008f, 0.008f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gKoumeChainDL);

        CLOSE_DISPS(play->state.gfxCtx);

        sp54 = Matrix_GetCurrent();
        if ((this->actor.child == NULL) || (this->actor.child->update == NULL)) {
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_JC_MATO, sp54->xw, sp54->yw, sp54->zw,
                               this->unk_38E.x, BINANG_ROT180(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play))),
                               this->unk_38E.z, -1);
        } else if (!((EnJcMato*)this->actor.child)->hitFlag) {
            this->actor.child->world.pos.x = sp54->xw;
            this->actor.child->world.pos.y = sp54->yw;
            this->actor.child->world.pos.z = sp54->zw;

            this->actor.child->world.rot = this->unk_38E;
            this->actor.child->world.rot.y = BINANG_ROT180(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)));

            this->actor.child->shape.rot = this->actor.child->world.rot;
        }
    }
}

void EnTruMt_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    EnTruMt* this = (EnTruMt*)thisx;

    if (limbIndex == KOUME_LIMB_HEAD) {
        Matrix_Translate(this->unk_33C.x, this->unk_33C.y, this->unk_33C.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateYS(this->unk_336.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_336.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->unk_336.z, MTXMODE_APPLY);
    }
}

void EnTruMt_Draw(Actor* thisx, PlayState* play) {
    EnTruMt* this = (EnTruMt*)thisx;
    TexturePtr eyeTextures[] = {
        gKoumeEyeOpenTex,
        gKoumeEyeHalfTex,
        gKoumeEyeClosedTex,
    };

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(eyeTextures[this->eyeTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(eyeTextures[this->eyeTexIndex]));

    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, EnTruMt_OverrideLimbDraw, EnTruMt_PostLimbDraw,
                                   EnTruMt_TransformLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
