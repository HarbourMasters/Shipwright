/*
 * File: z_en_az.c
 * Overlay: ovl_En_Az
 * Description: Beaver Bros
 */

#include "prevent_bss_reordering.h"
#include "z_en_az.h"
#include "overlays/actors/ovl_En_Twig/z_en_twig.h"
#include "overlays/actors/ovl_En_Fish/z_en_fish.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_MINIMAP_ICON_ENABLED)

typedef struct {
    /* 0x0 */ s16 unk_0;
    /* 0x4 */ f32 unk_4;
} struct_80A98F94; // size = 0x8

void EnAz_Init(Actor* thisx, PlayState* play2);
void EnAz_Destroy(Actor* thisx, PlayState* play2);
void EnAz_Update(Actor* thisx, PlayState* play2);
void EnAz_Draw(Actor* thisx, PlayState* play2);

void func_80A982E0(PlayState* play, ActorPathing* actorPathing);
void func_80A98414(EnAz* this, PlayState* play);
s32 EnAz_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx);
void EnAz_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx);
void func_80A98EFC(EnAz* this, PlayState* play, u16 textId, s32 animIndex, s32 brotherAnimIndex);
void func_80A98F94(struct_80A98F94* yData, f32 frame, f32* yInterp);

void func_80A95C5C(EnAz* this, PlayState* play);
void func_80A95CEC(EnAz* this, PlayState* play);

void func_80A95DA0(EnAz* this, PlayState* play);
void func_80A95E88(EnAz* this, PlayState* play);

void func_80A95F94(EnAz* this, PlayState* play);
void func_80A95FE8(EnAz* this, PlayState* play);

void func_80A979DC(EnAz* this, PlayState* play);
void func_80A979F4(EnAz* this, PlayState* play);

void func_80A97A28(EnAz* this, PlayState* play);
void func_80A97A40(EnAz* this, PlayState* play);

void func_80A97A9C(EnAz* this, PlayState* play);
void func_80A97AB4(EnAz* this, PlayState* play);

void func_80A97C0C(EnAz* this, PlayState* play);
void func_80A97C24(EnAz* this, PlayState* play);
void func_80A97C4C(EnAz* this, PlayState* play);

void func_80A97D5C(EnAz* this, PlayState* play);
void func_80A97E48(EnAz* this, PlayState* play);

void func_80A97EAC(EnAz* this, PlayState* play);
void func_80A97F9C(EnAz* this, PlayState* play);

typedef enum {
    /* -1 */ BEAVER_ANIM_NONE = -1,
    /*  0 */ BEAVER_ANIM_IDLE,
    /*  1 */ BEAVER_ANIM_WALK,
    /*  2 */ BEAVER_ANIM_SWIM_WITH_SPINNING_TAIL,
    /*  3 */ BEAVER_ANIM_SWIM_WITH_RAISED_TAIL, // Unused
    /*  4 */ BEAVER_ANIM_TALK,
    /*  5 */ BEAVER_ANIM_TALK_WAVE_ARMS,
    /*  6 */ BEAVER_ANIM_LAUGH_RIGHT,
    /*  7 */ BEAVER_ANIM_LAUGH_LEFT,
    /*  8 */ BEAVER_ANIM_SWIM,
    /*  9 */ BEAVER_ANIM_TALK_TO_LEFT,
    /* 10 */ BEAVER_ANIM_TALK_TO_RIGHT,
    /* 11 */ BEAVER_ANIM_BOW,
    /* 12 */ BEAVER_ANIM_IDLE_FACE_LEFT,
    /* 13 */ BEAVER_ANIM_IDLE_FACE_RIGHT,
    /* 14 */ BEAVER_ANIM_IDLE_FACE_MAX
} BeaverAnimation;

static AnimationSpeedInfo sAnimationSpeedInfo[BEAVER_ANIM_IDLE_FACE_MAX] = {
    { &gBeaverIdleAnim, 1.0f, ANIMMODE_LOOP, -10.0f },                // BEAVER_ANIM_IDLE
    { &gBeaverWalkAnim, 1.0f, ANIMMODE_LOOP, -5.0f },                 // BEAVER_ANIM_WALK
    { &gBeaverSwimWithSpinningTailAnim, 1.0f, ANIMMODE_LOOP, -5.0f }, // BEAVER_ANIM_SWIM_WITH_SPINNING_TAIL
    { &gBeaverSwimWithRaisedTailAnim, 1.0f, ANIMMODE_LOOP, -5.0f },   // BEAVER_ANIM_SWIM_WITH_RAISED_TAIL
    { &gBeaverTalkAnim, 1.0f, ANIMMODE_LOOP, -5.0f },                 // BEAVER_ANIM_TALK
    { &gBeaverTalkWaveArmsAnim, 1.0f, ANIMMODE_LOOP, -5.0f },         // BEAVER_ANIM_TALK_WAVE_ARMS
    { &gBeaverLaughRightAnim, 1.0f, ANIMMODE_LOOP, -5.0f },           // BEAVER_ANIM_LAUGH_RIGHT
    { &gBeaverLaughLeftAnim, 1.0f, ANIMMODE_LOOP, -5.0f },            // BEAVER_ANIM_LAUGH_LEFT
    { &gBeaverSwimAnim, 2.0f, ANIMMODE_LOOP, -5.0f },                 // BEAVER_ANIM_SWIM
    { &gBeaverTalkToLeftAnim, 1.0f, ANIMMODE_LOOP, -5.0f },           // BEAVER_ANIM_TALK_TO_LEFT
    { &gBeaverTalkToRightAnim, 1.0f, ANIMMODE_LOOP, -5.0f },          // BEAVER_ANIM_TALK_TO_RIGHT
    { &gBeaverBowAnim, 1.0f, ANIMMODE_ONCE, -5.0f },                  // BEAVER_ANIM_BOW
    { &gBeaverIdleFaceLeftAnim, 1.0f, ANIMMODE_LOOP, -5.0f },         // BEAVER_ANIM_IDLE_FACE_LEFT
    { &gBeaverIdleFaceRightAnim, 1.0f, ANIMMODE_LOOP, -5.0f },        // BEAVER_ANIM_IDLE_FACE_RIGHT
};

ActorProfile En_Az_Profile = {
    /**/ ACTOR_EN_AZ,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_AZ,
    /**/ sizeof(EnAz),
    /**/ EnAz_Init,
    /**/ EnAz_Destroy,
    /**/ EnAz_Update,
    /**/ EnAz_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 18, 46, 0, { 0, 0, 0 } },
};

static EnAz* D_80A9913C = NULL;

Vec3f D_80A99E80;  // path point?
f32 D_80A99E8C;    // player distance to path point?
f32 D_80A99E90[2]; // unused?

void func_80A94A30(EnAz* this) {
    this->actor.velocity.y += this->actor.gravity;
    if (this->actor.velocity.y < this->actor.terminalVelocity) {
        this->actor.velocity.y = this->actor.terminalVelocity;
    }
}

void func_80A94A64(EnAz* this) {
    func_80A94A30(this);
    Actor_UpdatePos(&this->actor);
}

s32 func_80A94A90(PlayState* play, ActorPathing* actorPathing) {
    func_80A94A64((EnAz*)actorPathing->actor);
    return false;
}

void func_80A94AB8(EnAz* this, PlayState* play, s32 spawnIndex) {
    play->nextEntrance = Entrance_CreateFromSpawn(spawnIndex);
    gSaveContext.nextCutsceneIndex = 0;
    play->transitionTrigger = TRANS_TRIGGER_START;
    play->transitionType = TRANS_TYPE_FADE_WHITE;
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
}

void func_80A94B20(PlayState* play) {
    Actor* ring = NULL;

    do {
        ring = SubS_FindActor(play, ring, ACTORCAT_MISC, ACTOR_EN_TWIG);
        if (ring != NULL) {
            if (RACERING_GET_PARAM_F(ring) == 1) {
                Actor_Kill(ring);
            }
            ring = ring->next;
        }
    } while (ring != NULL);
}

s32 func_80A94B98(EnAz* this, PlayState* play) {
    EnTwig* ring;
    s32 ret = false;
    Actor* misc = NULL;

    do {
        if (1) {}
        ring = (EnTwig*)SubS_FindActor(play, misc, ACTORCAT_MISC, ACTOR_EN_TWIG);
        if ((ring != NULL) && (RACERING_GET_PARAM_F(&ring->dyna.actor) == 1) && !(ring->unk_16C & 1)) {
            ret = true;
            break;
        }
        misc = ring->dyna.actor.next;
    } while (misc != NULL);

    return ret;
}

static InitChainEntry sInitChain[3] = {
    ICHAIN_F32(cullingVolumeScale, 80, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 80, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_STOP),
};

void EnAz_Init(Actor* thisx, PlayState* play2) {
    static s16 D_80A9914C[] = { 1, 0, 3, 2, 5, 4, -1 };
    static s16 D_80A9915C[] = { 0, 1, 0, 1, 0, 1, 1 };
    EnAz* this = (EnAz*)thisx;
    PlayState* play = play2;
    s16 sp4E;
    s32 phi_v1;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->unk_374 = 0;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.attentionRangeType = ATTENTION_RANGE_1;
    switch (BEAVER_GET_PARAM_F00(thisx)) {
        case 0:
            phi_v1 =
                (gSaveContext.save.entrance == ENTRANCE(WATERFALL_RAPIDS, 0)) && CHECK_WEEKEVENTREG(WEEKEVENTREG_93_01);
            phi_v1 = !phi_v1;
            break;

        case 2:
            phi_v1 = (gSaveContext.save.entrance != ENTRANCE(WATERFALL_RAPIDS, 1)) ||
                     !CHECK_WEEKEVENTREG(WEEKEVENTREG_24_04);
            break;

        case 4:
            phi_v1 = gSaveContext.save.entrance != ENTRANCE(WATERFALL_RAPIDS, 2);
            break;

        case 1:
            phi_v1 =
                (gSaveContext.save.entrance == ENTRANCE(WATERFALL_RAPIDS, 0)) && CHECK_WEEKEVENTREG(WEEKEVENTREG_93_01);
            phi_v1 = !phi_v1;
            break;

        case 3:
            phi_v1 =
                (gSaveContext.save.entrance != ENTRANCE(WATERFALL_RAPIDS, 1)) || CHECK_WEEKEVENTREG(WEEKEVENTREG_24_04);
            break;

        case 5:
            phi_v1 = gSaveContext.save.entrance != ENTRANCE(WATERFALL_RAPIDS, 2);
            break;

        case 6:
            phi_v1 = (gSaveContext.save.entrance == ENTRANCE(WATERFALL_RAPIDS, 0)) &&
                     !CHECK_WEEKEVENTREG(WEEKEVENTREG_93_01);
            phi_v1 = !phi_v1;
            break;

        default:
            phi_v1 = true;
            break;
    }
    if (phi_v1) {
        Actor_Kill(&this->actor);
        return;
    }

    this->unk_2F8 = D_80A9915C[BEAVER_GET_PARAM_F00(thisx)];
    if (BEAVER_GET_PARAM_F00(thisx) >= 0) {
        sp4E = D_80A9914C[BEAVER_GET_PARAM_F00(thisx)];
    } else {
        sp4E = -1;
    }
    if (this->unk_2F8 == 0) {
        this->unk_374 |= 2;
    }
    SubS_FillCutscenesList(&this->actor, this->csIdList, ARRAY_COUNT(this->csIdList));
    if (D_80A9913C == NULL) {
        D_80A9913C = (EnAz*)thisx;
        this->unk_374 |= 1;
    }
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    if (this->unk_374 & 2) {
        SkelAnime_InitFlex(play, &this->skelAnime, &gBeaverOlderBrotherSkel, &gBeaverWalkAnim, this->jointTable,
                           this->morphTable, BEAVER_OLDER_BROTHER_LIMB_MAX);
        Actor_SetScale(&this->actor, 0.012f);
    } else {
        SkelAnime_InitFlex(play, &this->skelAnime, &gBeaverYoungerBrotherSkel, &gBeaverWalkAnim, this->jointTable,
                           this->morphTable, BEAVER_YOUNGER_BROTHER_LIMB_MAX);
    }
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    if (this->unk_374 & 2) {
        this->collider.dim.radius *= 1.2f;
        this->collider.dim.height *= 1.2f;
        this->collider.dim.yShift *= 1.2f;
    }
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WATER) && (this->actor.depthInWater > 22.0f)) {
        this->unk_374 |= 0x100;
        this->unk_376 |= 0x100;
    }
    Animation_Change(&this->skelAnime, sAnimationSpeedInfo[BEAVER_ANIM_IDLE].animation, 1.0f,
                     Animation_GetLastFrame(sAnimationSpeedInfo[BEAVER_ANIM_IDLE].animation) * Rand_ZeroOne(),
                     Animation_GetLastFrame(sAnimationSpeedInfo[BEAVER_ANIM_IDLE].animation),
                     sAnimationSpeedInfo[BEAVER_ANIM_IDLE].mode, sAnimationSpeedInfo[BEAVER_ANIM_IDLE].morphFrames);
    this->unk_37E = 0;
    this->unk_380 = 0;
    this->unk_384 = 0;
    this->actor.gravity = -1.0f;
    this->unk_376 = this->unk_374;
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE, &this->animIndex);
    this->skelAnime.curFrame = Rand_ZeroOne() * this->skelAnime.endFrame;

    switch (gSaveContext.save.entrance) {
        case ENTRANCE(WATERFALL_RAPIDS, 0):
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_93_01)) {
                this->unk_2FA = 5;
                if (this->unk_374 & 2) {
                    this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
                    this->unk_374 |= 0x20;
                }
            } else {
                this->unk_2FA = 0;
                this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
                this->unk_374 |= 0x20;
            }
            func_80A94B20(play);
            if (this->unk_2FA == 5) {
                func_80A97C0C(this, play);
            } else {
                func_80A95DA0(this, play);
            }
            break;

        case ENTRANCE(WATERFALL_RAPIDS, 3):
            this->unk_2FA = 0;
            if (!(this->unk_374 & 2)) {
                this->actor.flags |=
                    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED);
            }
            if (gSaveContext.save.entrance == ENTRANCE(WATERFALL_RAPIDS, 3)) {
                this->unk_2FA = 0xA;
            }
            func_80A97C0C(this, play);
            break;

        case ENTRANCE(WATERFALL_RAPIDS, 1):
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_93_01)) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_24_04)) {
                    this->unk_2FA = 8;
                } else {
                    this->unk_2FA = 6;
                }
            } else {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_24_04)) {
                    this->unk_2FA = 3;
                } else {
                    this->unk_2FA = 1;
                }
            }
            if (this->unk_374 & 1) {
                SubS_CopyPointFromPathList(play->setupPathList, BEAVER_GET_PATH_INDEX(thisx),
                                           play->setupPathList[BEAVER_GET_PATH_INDEX(thisx)].count - 1, &D_80A99E80);
            }
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_24_04)) {
                if (this->unk_374 & 2) {
                    func_80A97D5C(this, play);
                } else {
                    func_80A979DC(this, play);
                }
            } else if (this->unk_374 & 2) {
                func_80A979DC(this, play);
            } else {
                func_80A97D5C(this, play);
            }
            break;

        case ENTRANCE(WATERFALL_RAPIDS, 2):
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_93_01)) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_24_04)) {
                    this->unk_2FA = 9;
                } else {
                    this->unk_2FA = 7;
                }
            } else {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_24_04)) {
                    this->unk_2FA = 4;
                } else {
                    this->unk_2FA = 2;
                }
            }
            if (this->unk_2FA == 2) {
                if (!(this->unk_374 & 2)) {
                    this->unk_374 |= 0x20;
                    this->actor.flags |=
                        (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED);
                    this->actionFunc = func_80A97C24;
                } else {
                    this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
                    func_80A95C5C(this, play);
                }
            } else {
                if (this->unk_374 & 2) {
                    this->unk_374 |= 0x20;
                    this->actor.flags |=
                        (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED);
                } else {
                    this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
                }
                this->actionFunc = func_80A97C24;
            }
            break;

        default:
            break;
    }
    if (sp4E >= 0) {
        this->brother = NULL;
        do {
            this->brother = (EnAz*)SubS_FindActor(play, &this->brother->actor, ACTORCAT_NPC, ACTOR_EN_AZ);
            if (this->brother != NULL) {
                if (sp4E == BEAVER_GET_PARAM_F00(&this->brother->actor)) {
                    break;
                }
                this->brother = (EnAz*)this->brother->actor.next;
            }
        } while (this->brother != NULL);
    }
}

void EnAz_Destroy(Actor* thisx, PlayState* play2) {
    EnAz* this = (EnAz*)thisx;

    if (gSaveContext.save.entrance != ENTRANCE(WATERFALL_RAPIDS, 1)) {
        gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_STOP;
    }
    Collider_DestroyCylinder(play2, &this->collider);
}

f32 func_80A954AC(EnAz* this) {
    ActorPathing* pathing = &this->unk_300;
    Vec3f sp28;
    Vec3f sp1C;

    sp28.x = pathing->curPoint.x - this->actor.world.pos.x;
    sp28.y = pathing->curPoint.y - this->actor.world.pos.y;
    sp28.z = pathing->curPoint.z - this->actor.world.pos.z;
    sp1C.x = pathing->curPoint.x - pathing->prevPoint.x;
    sp1C.y = pathing->curPoint.y - pathing->prevPoint.y;
    sp1C.z = pathing->curPoint.z - pathing->prevPoint.z;
    return Math3D_Cos(&sp28, &sp1C);
}

s32 func_80A95534(PlayState* play, ActorPathing* actorPathing) {
    EnAz* this = (EnAz*)actorPathing->actor;
    s32 ret = false;

    this->actor.world.rot.x = 0;
    Math_SmoothStepToS(&this->unk_39E, this->actor.world.rot.x, 2, 0x71C, 0);
    Math_SmoothStepToS(&this->actor.shape.rot.x, 0, 2, 0x71C, 0);
    Math_SmoothStepToS(&this->actor.world.rot.y, actorPathing->rotToCurPoint.y, 1, 0xE38, 0);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 2, 0x71C, 0);
    this->actor.gravity = -1.0f;
    this->unk_36C = 1.5f;
    if (actorPathing->curPointIndex == actorPathing->endPointIndex) {
        if (actorPathing->distSqToCurPointXZ < this->unk_36C) {
            this->unk_36C = actorPathing->distSqToCurPointXZ;
        }
    }
    Math_SmoothStepToF(&this->actor.speed, this->unk_36C, 0.8f, 2.0f, 0.0f);
    actorPathing->moveFunc = SubS_ActorPathing_MoveWithGravity;
    if (actorPathing->distSqToCurPointXZ <= this->actor.speed) {
        ret = true;
    }
    return ret;
}

s32 func_80A9565C(PlayState* play, ActorPathing* actorPathing) {
    EnAz* this = (EnAz*)actorPathing->actor;
    s32 ret = false;
    f32 temp_f0;

    this->actor.gravity = -1.0f;
    actorPathing->moveFunc = func_80A94A90;
    this->unk_374 |= 0x2000;
    temp_f0 = func_80A954AC(this);
    if ((actorPathing->distSqToCurPointXZ < SQ(this->actor.speed)) || (temp_f0 <= 0.0f)) {
        ret = true;
    } else {
        this->unk_39E = this->actor.world.rot.x =
            Math_Atan2S(-this->actor.velocity.y, Math_CosS(-this->actor.world.rot.x) * this->actor.speed);
    }
    return ret;
}

s32 func_80A95730(PlayState* play, ActorPathing* actorPathing) {
    EnAz* this = (EnAz*)actorPathing->actor;
    s32 ret = false;
    f32 temp_f0;
    f32 sp40;
    s32 sp3C;
    s32 sp38;
    s32 sp34;

    this->actor.gravity = 0.0f;
    temp_f0 = func_80A954AC(this);
    if ((actorPathing->distSqToCurPointXZ < SQ(this->actor.speed)) || (temp_f0 <= 0.0f)) {
        ret = true;
    } else {
        sp40 = SQ(this->actor.speed) / actorPathing->distSqToCurPoint;
        sp34 = ABS(actorPathing->rotToCurPoint.x - this->actor.world.rot.x);

        sp3C = (s32)(sp34 * sp40) + 0xAAA;
        sp34 = ABS(actorPathing->rotToCurPoint.y - this->actor.world.rot.y);

        Math_SmoothStepToS(&this->actor.world.rot.x, actorPathing->rotToCurPoint.x, 1, sp3C, 0);
        sp38 = (s32)(sp34 * sp40) + 0xAAA;
        Math_SmoothStepToS(&this->actor.world.rot.y, actorPathing->rotToCurPoint.y, 1, sp38, 0);

        Math_SmoothStepToS(&this->unk_39E, this->actor.world.rot.x, 2, sp3C, 0);
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 2, sp38, 0);
    }
    actorPathing->moveFunc = SubS_ActorPathing_MoveWithoutGravityReverse;
    return ret;
}

s32 func_80A958B0(PlayState* play, ActorPathing* actorPathing) {
    EnAz* this = (EnAz*)actorPathing->actor;
    s32 ret = false;
    f32 temp;
    f32 phi_f0;
    f32 temp1;
    f32 temp2;
    f32 sp3C;
    s32 sp2C;
    s32 sp28;
    s32 sp30;

    this->actor.gravity = 0.0f;
    temp2 = D_80A99E8C;
    temp = D_80A99E90[this->unk_2F8];

    if (temp <= temp2) {
        phi_f0 = (this->unk_374 & 2) ? 480.0f : 240.0f;

        if (this->actor.xzDistToPlayer < phi_f0) {
            Math_SmoothStepToF(&this->unk_36C, 12.0f, 0.8f, 0.5f, 0.01f);
        } else {
            Math_SmoothStepToF(&this->unk_36C, 6.0f, 0.8f, 0.5f, 0.01f);
        }
    } else {
        Math_SmoothStepToF(&this->unk_36C, 26.0f, 0.5f, 1.0f, 0.01f);
    }
    Math_SmoothStepToF(&this->actor.speed, this->unk_36C, 0.8f, 2.0f, 0.0f);
    temp1 = func_80A954AC(this);
    if ((actorPathing->distSqToCurPointXZ < SQ(this->actor.speed)) || (temp1 <= 0.0f)) {
        ret = true;
    } else {
        sp3C = SQ(this->actor.speed) / actorPathing->distSqToCurPoint;
        sp30 = ABS(actorPathing->rotToCurPoint.x - this->actor.world.rot.x);
        sp2C = (s32)(sp30 * sp3C) + 0xAAA;
        sp30 = ABS(actorPathing->rotToCurPoint.y - this->actor.world.rot.y);

        Math_SmoothStepToS(&this->actor.world.rot.x, actorPathing->rotToCurPoint.x, 1, sp2C, 0);
        sp28 = (s32)(sp30 * sp3C) + 0xAAA;
        Math_SmoothStepToS(&this->actor.world.rot.y, actorPathing->rotToCurPoint.y, 1, sp28, 0);
        Math_SmoothStepToS(&this->unk_39E, this->actor.world.rot.x, 2, sp2C, 0);
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 2, sp28, 0);
    }
    actorPathing->moveFunc = SubS_ActorPathing_MoveWithoutGravityReverse;
    return ret;
}

s32 func_80A95B34(PlayState* play, ActorPathing* actorPathing) {
    EnAz* this = (EnAz*)actorPathing->actor;
    s32 ret;

    if (this->unk_374 & 0x100) {
        if (!(this->unk_374 & 8)) {
            if (this->unk_374 & 2) {
                SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_SWIM,
                                                &this->animIndex);
            } else {
                SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                BEAVER_ANIM_SWIM_WITH_SPINNING_TAIL, &this->animIndex);
            }
            this->unk_374 |= 8;
        }
        if (this->unk_2FA == 0) {
            ret = func_80A95730(play, actorPathing);
        } else {
            ret = func_80A958B0(play, actorPathing);
        }
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->unk_374 & 8) {
            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_WALK, &this->animIndex);
            this->unk_374 &= ~8;
        }
        ret = func_80A95534(play, actorPathing);
    } else {
        ret = func_80A9565C(play, actorPathing);
    }
    return ret;
}

void func_80A95C5C(EnAz* this, PlayState* play) {
    this->actor.draw = NULL;
    this->actor.world.pos.y = this->actor.home.pos.y + 120.0f;
    this->actor.gravity = -1.0f;
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE, &this->animIndex);
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    this->actor.bgCheckFlags &= ~(BGCHECKFLAG_GROUND | BGCHECKFLAG_WATER);
    this->unk_3C0 = 0;
    this->actionFunc = func_80A95CEC;
}

void func_80A95CEC(EnAz* this, PlayState* play) {
    if (this->unk_374 & 0x8000) {
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            this->actor.shape.rot.y = this->actor.world.rot.y;
            this->actor.draw = EnAz_Draw;
            Actor_MoveWithGravity(&this->actor);
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_HONEYCOMB_FALL - SFX_FLAG);
        } else {
            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_GERUDOFT_DOWN);
            }
            if (SubS_StartCutscene(&this->actor, CS_ID_GLOBAL_TALK, this->csIdList[0], SUBS_CUTSCENE_NORMAL)) {
                func_80A97C0C(this, play);
            }
        }
    }
}

void func_80A95DA0(EnAz* this, PlayState* play) {
    ActorPathing* sp40 = &this->unk_300;

    SubS_ActorPathing_Init(play, &this->actor.world.pos, &this->actor, sp40, play->setupPathList,
                           BEAVER_GET_PATH_INDEX(&this->actor), 0, 0, 1, 1);
    this->unk_36C = 4.0f;
    this->actor.speed = 4.0f;
    this->actor.gravity = 0.0f;
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_SWIM_WITH_SPINNING_TAIL,
                                    &this->animIndex);
    this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    this->actor.bgCheckFlags &= ~(BGCHECKFLAG_GROUND | BGCHECKFLAG_WATER);
    this->unk_374 |= 0x1000;
    Math_Vec3f_Copy(&this->actor.world.pos, &sp40->curPoint);
    this->actionFunc = func_80A95E88;
}

void func_80A95E88(EnAz* this, PlayState* play) {
    SubS_ActorPathing_Update(play, &this->unk_300, func_80A982E0, func_80A95B34, SubS_ActorPathing_MoveWithGravity,
                             SubS_ActorPathing_SetNextPoint);
    if (this->actor.depthInWater > 8.0f) {
        if (this->unk_374 & 2) {
            if ((this->skelAnime.curFrame < this->skelAnime.playSpeed) && (this->skelAnime.curFrame >= 0.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EV_BEAVER_SWIM_HAND);
            }
        } else {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_BEAVER_SWIM_MOTOR - SFX_FLAG);
        }
    }
    if (!(this->unk_374 & 0x2000)) {
        SkelAnime_Update(&this->skelAnime);
    }
    this->unk_374 &= ~0x2000;
    if (this->actor.isLockedOn) {
        func_80A95F94(this, play);
    }
}

void func_80A95F94(EnAz* this, PlayState* play) {
    func_800BE33C(&this->actor.world.pos, &this->actor.home.pos, &this->actor.world.rot, false);
    this->unk_39E = 0;
    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->actionFunc = func_80A95FE8;
}

void func_80A95FE8(EnAz* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (CutsceneManager_IsNext(this->csIdList[0])) {
        CutsceneManager_StartWithPlayerCs(this->csIdList[0], &this->actor);
    } else {
        CutsceneManager_Queue(this->csIdList[0]);
    }
    if (Actor_WorldDistXYZToPoint(&this->actor, &this->actor.home.pos) > 20.0f) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_BEAVER_SWIM_MOTOR - SFX_FLAG);
        func_800BE33C(&this->actor.world.pos, &this->actor.home.pos, &this->actor.world.rot, false);
        Math_SmoothStepToS(&this->actor.shape.rot.x, this->actor.world.rot.x, 3, 0xE38, 0x38E);
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 3, 0xE38, 0x38E);
        this->actor.shape.rot.z = 0;
        Actor_MoveWithoutGravityReverse(&this->actor);
    } else {
        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE, &this->animIndex);
        this->unk_374 &= ~0x1000;
        this->actor.gravity = -1.0f;
        this->actor.speed = 0.0f;
        Math_SmoothStepToS(&this->actor.shape.rot.x, 0, 3, 0x1000, 0x100);
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 3, 0x1038, 0x100);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->actor.shape.rot.x = 0;
            this->actor.gravity = 0.0f;
            func_80A97C0C(this, play);
            CutsceneManager_Stop(this->csIdList[0]);
        }
        Actor_MoveWithGravity(&this->actor);
    }
}

s32 func_80A9617C(EnAz* this, PlayState* play) {
    s32 pad[2];
    s32 ret = 2;
    EnAz* brother = this->brother;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
        case TEXT_STATE_DONE:
            if ((play->msgCtx.currentTextId == 0x10DD) && (this->unk_374 & 0x8000)) {
                if (SubS_StartCutscene(&brother->actor, brother->csIdList[0], CS_ID_GLOBAL_TALK,
                                       SUBS_CUTSCENE_NORMAL)) {
                    brother->unk_374 |= 0x8000;
                    play->msgCtx.msgMode = MSGMODE_PAUSED;
                    ret = 0;
                }
            } else if (Message_ShouldAdvance(play)) {
                ret = 3;
                switch (play->msgCtx.currentTextId) {
                    case 0x70:
                    case 0xCD:
                        CLEAR_WEEKEVENTREG(WEEKEVENTREG_24_01);
                        this->actor.textId = 0x10F2;
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_BOW,
                                                        &this->animIndex);
                        SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_BOW,
                                                        &brother->animIndex);
                        break;

                    case 0x10CE:
                        this->actor.textId = 0x10CF;
                        SET_WEEKEVENTREG(WEEKEVENTREG_16_40);
                        break;

                    case 0x10CF:
                        this->actor.textId = 0x10D0;
                        break;

                    case 0x10D0:
                        this->actor.textId = 0x10D1;
                        ret = 3;
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                        BEAVER_ANIM_TALK_WAVE_ARMS, &this->animIndex);
                        break;

                    case 0x10D1:
                        this->actor.textId = 0x10D2;
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_TALK,
                                                        &this->animIndex);
                        break;

                    case 0x10D2:
                        if (play->msgCtx.choiceIndex == 0) {
                            Audio_PlaySfx_MessageDecide();
                            this->actor.textId = 0x10D6;
                            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                            BEAVER_ANIM_TALK_WAVE_ARMS, &this->animIndex);
                        } else {
                            Audio_PlaySfx_MessageCancel();
                            this->actor.textId = 0x10D3;
                            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_BOW,
                                                            &this->animIndex);
                        }
                        break;

                    case 0x10D3:
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &this->animIndex);
                        this->unk_374 |= 0x20;
                        ret = 0;
                        break;

                    case 0x10D4:
                        this->actor.textId = 0x10D2;
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_TALK,
                                                        &this->animIndex);
                        break;

                    case 0x10D6:
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &this->animIndex);
                        func_80A979DC(this, play);
                        this->unk_2FA = 1;
                        ret = 0;
                        break;

                    case 0x10D7:
                        this->actor.textId = 0x10D8;
                        break;

                    case 0x10D8:
                        if (play->msgCtx.choiceIndex == 0) {
                            Audio_PlaySfx_MessageDecide();
                            switch (this->unk_2FA) {
                                case 2:
                                    this->unk_2FA = 1;
                                    break;

                                case 4:
                                    this->unk_2FA = 3;
                                    break;

                                case 7:
                                    this->unk_2FA = 6;
                                    break;

                                case 9:
                                    this->unk_2FA = 8;
                                    break;

                                default:
                                    break;
                            }
                            ret = 0;
                        } else {
                            Audio_PlaySfx_MessageCancel();
                            this->actor.textId = 0x10D9;
                        }
                        break;

                    case 0x10D9:
                        if ((this->unk_2FA == 3) || (this->unk_2FA == 8)) {
                            CLEAR_WEEKEVENTREG(WEEKEVENTREG_24_04);
                        }
                        func_80A94AB8(this, play, 0);
                        func_80A979DC(this, play);
                        ret = 0;
                        break;

                    case 0x10DA:
                        this->actor.textId = 0x10DB;
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_TALK,
                                                        &this->animIndex);
                        break;

                    case 0x10DB:
                        if (play->msgCtx.choiceIndex == 0) {
                            Audio_PlaySfx_MessageDecide();
                            play->msgCtx.msgMode = MSGMODE_PAUSED;
                            this->unk_2FA = 1;
                            ret = 0;
                        } else {
                            Audio_PlaySfx_MessageCancel();
                            this->actor.textId = 0x10DC;
                            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_BOW,
                                                            &this->animIndex);
                        }
                        break;

                    case 0x10DC:
                        func_80A94AB8(this, play, 0);
                        func_80A979DC(this, play);
                        this->unk_374 |= 0x20;
                        ret = 0;
                        break;

                    case 0x10DD:
                        func_80A98EFC(this, play, 0x10DE, BEAVER_ANIM_IDLE_FACE_RIGHT, BEAVER_ANIM_TALK_TO_LEFT);
                        this->unk_374 |= 0x8000;
                        ret = 2;
                        break;

                    case 0x10DE:
                        func_80A98EFC(this, play, 0x10DF, BEAVER_ANIM_IDLE_FACE_LEFT, BEAVER_ANIM_TALK_TO_RIGHT);
                        ret = 0;
                        break;

                    case 0x10DF:
                        func_80A98EFC(this, play, 0x10E0, BEAVER_ANIM_IDLE_FACE_RIGHT, BEAVER_ANIM_TALK_TO_LEFT);
                        ret = 0;
                        break;

                    case 0x10E0:
                        func_80A98EFC(this, play, 0x10E1, BEAVER_ANIM_IDLE_FACE_LEFT, BEAVER_ANIM_TALK_TO_RIGHT);
                        ret = 0;
                        break;

                    case 0x10E1:
                        func_80A98EFC(this, play, 0x10E2, BEAVER_ANIM_IDLE_FACE_RIGHT, BEAVER_ANIM_TALK_TO_LEFT);
                        ret = 0;
                        break;

                    case 0x10E2:
                        this->actor.textId = 0x10E3;
                        ret = 3;
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_TALK,
                                                        &this->animIndex);
                        break;

                    case 0x10E3:
                        func_80A98EFC(this, play, 0x10E4, BEAVER_ANIM_IDLE, BEAVER_ANIM_TALK);
                        ret = 0;
                        break;

                    case 0x10E4:
                        func_80A98EFC(this, play, 0x10E5, BEAVER_ANIM_IDLE, BEAVER_ANIM_TALK);
                        ret = 0;
                        break;

                    case 0x10E5:
                        if (play->msgCtx.choiceIndex == 0) {
                            Audio_PlaySfx_MessageDecide();
                            this->actor.textId = 0x10E8;
                        } else {
                            Audio_PlaySfx_MessageCancel();
                            this->actor.textId = 0x10E6;
                            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                            BEAVER_ANIM_TALK_TO_LEFT, &this->animIndex);
                            SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo,
                                                            BEAVER_ANIM_IDLE_FACE_RIGHT, &brother->animIndex);
                        }
                        break;

                    case 0x10E6:
                        this->actor.textId = 0x10E7;
                        func_80A98EFC(this, play, 0x10E7, BEAVER_ANIM_IDLE, BEAVER_ANIM_TALK_TO_RIGHT);
                        ret = 0;
                        break;

                    case 0x10E7:
                        CLEAR_WEEKEVENTREG(WEEKEVENTREG_24_04);
                        func_80A94AB8(this, play, 0);
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &this->animIndex);
                        SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &brother->animIndex);
                        func_80A979DC(this, play);
                        ret = 0;
                        break;

                    case 0x10E8:
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &this->animIndex);
                        SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &brother->animIndex);
                        this->unk_2FA = 3;
                        ret = 0;
                        break;

                    case 0x10E9:
                        func_80A98EFC(this, play, 0x10EA, BEAVER_ANIM_IDLE_FACE_LEFT, BEAVER_ANIM_TALK);
                        ret = 0;
                        break;

                    case 0x10EA:
                        func_80A98EFC(this, play, 0x10EB, BEAVER_ANIM_IDLE, BEAVER_ANIM_TALK);
                        ret = 0;
                        break;

                    case 0x10EB:
                        if (play->msgCtx.choiceIndex == 0) {
                            play->msgCtx.msgMode = MSGMODE_PAUSED;
                            Audio_PlaySfx_MessageDecide();
                            switch (this->unk_2FA) {
                                case 4:
                                    this->unk_2FA = 3;
                                    break;

                                case 7:
                                    this->unk_2FA = 6;
                                    break;

                                case 9:
                                default:
                                    this->unk_2FA = 8;

                                    break;
                            }
                            ret = 0;
                        } else {
                            Audio_PlaySfx_MessageCancel();
                            this->actor.textId = 0x10EC;
                            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                            BEAVER_ANIM_LAUGH_LEFT, &this->animIndex);
                            SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo,
                                                            BEAVER_ANIM_IDLE_FACE_RIGHT, &brother->animIndex);
                        }
                        break;

                    case 0x10EC:
                        func_80A98EFC(this, play, 0x10ED, BEAVER_ANIM_NONE, BEAVER_ANIM_LAUGH_RIGHT);
                        ret = 0;
                        break;

                    case 0x10ED:
                        if ((this->unk_2FA == 4) || (this->unk_2FA == 9)) {
                            CLEAR_WEEKEVENTREG(WEEKEVENTREG_24_04);
                        }
                        func_80A94AB8(this, play, 0);
                        func_80A979DC(this, play);
                        ret = 0;
                        break;

                    case 0x10EE:
                        func_80A98EFC(this, play, 0x10EF, BEAVER_ANIM_IDLE_FACE_LEFT, BEAVER_ANIM_TALK_TO_RIGHT);
                        ret = 0;
                        break;

                    case 0x10EF:
                        func_80A98EFC(this, play, 0x10F0, BEAVER_ANIM_IDLE_FACE_RIGHT, BEAVER_ANIM_TALK_TO_LEFT);
                        ret = 0;
                        break;

                    case 0x10F0:
                        func_80A98EFC(this, play, 0x10F1, BEAVER_ANIM_IDLE_FACE_LEFT, BEAVER_ANIM_TALK_TO_RIGHT);
                        ret = 0;
                        break;

                    case 0x10F1:
                        SET_WEEKEVENTREG(WEEKEVENTREG_93_01);
                        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BEAVER_RACE_BOTTLE)) {
                            this->getItemId = GI_RUPEE_RED;
                        } else {
                            this->getItemId = GI_BOTTLE;
                            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BEAVER_RACE_BOTTLE);
                        }
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &this->animIndex);
                        SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &brother->animIndex);
                        ret = 7;
                        break;

                    case 0x10F2:
                    case 0x1109:
                        CLEAR_WEEKEVENTREG(WEEKEVENTREG_24_04);
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &this->animIndex);
                        SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &brother->animIndex);
                        func_80A94AB8(this, play, 0);
                        func_80A979DC(this, play);
                        ret = 0;
                        break;

                    case 0x10F3:
                        func_80A98EFC(this, play, 0x10F4, BEAVER_ANIM_IDLE_FACE_LEFT, BEAVER_ANIM_TALK_TO_RIGHT);
                        ret = 0;
                        break;

                    case 0x10F4:
                        func_80A98EFC(this, play, 0x10F5, BEAVER_ANIM_IDLE_FACE_RIGHT, BEAVER_ANIM_TALK_TO_LEFT);
                        ret = 0;
                        break;

                    case 0x10F5:
                        func_80A98EFC(this, play, 0x10F6, BEAVER_ANIM_IDLE_FACE_LEFT, BEAVER_ANIM_TALK_TO_RIGHT);
                        ret = 0;
                        break;

                    case 0x10F6:
                        func_80A98EFC(this, play, 0x10F7, BEAVER_ANIM_IDLE_FACE_RIGHT, BEAVER_ANIM_TALK_WAVE_ARMS);
                        ret = 0;
                        break;

                    case 0x10F7:
                        this->actor.textId = 0x10F8;
                        ret = 3;
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_TALK,
                                                        &this->animIndex);
                        break;

                    case 0x10F8:
                        if (play->msgCtx.choiceIndex == 0) {
                            Audio_PlaySfx_MessageDecide();
                            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BEAVER_BROS_HEART_PIECE)) {
                                this->actor.textId = 0x1107;
                            } else {
                                this->actor.textId = 0x10FA;
                            }
                            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                            BEAVER_ANIM_TALK_WAVE_ARMS, &this->animIndex);
                        } else {
                            Audio_PlaySfx_MessageCancel();
                            this->actor.textId = 0x10F9;
                            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_BOW,
                                                            &this->animIndex);
                            SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_BOW,
                                                            &brother->animIndex);
                        }
                        break;

                    case 0x10F9:
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &this->animIndex);
                        SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &brother->animIndex);
                        this->unk_374 |= 0x20;
                        ret = 0;
                        break;

                    case 0x10FA:
                    case 0x1107:
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &this->animIndex);
                        SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &brother->animIndex);
                        this->unk_2FA = 6;
                        ret = 0;
                        break;

                    case 0x10FB:
                        func_80A98EFC(this, play, 0x10FC, BEAVER_ANIM_IDLE_FACE_LEFT, BEAVER_ANIM_TALK_TO_RIGHT);
                        ret = 0;
                        break;

                    case 0x10FC:
                        func_80A98EFC(this, play, 0x10FD, BEAVER_ANIM_IDLE_FACE_RIGHT, BEAVER_ANIM_TALK);
                        ret = 0;
                        break;

                    case 0x10FD:
                        this->actor.textId = 0x10FE;
                        ret = 3;
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_TALK,
                                                        &this->animIndex);
                        SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &brother->animIndex);
                        break;

                    case 0x10FE:
                        if (play->msgCtx.choiceIndex == 0) {
                            Audio_PlaySfx_MessageDecide();
                            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BEAVER_BROS_HEART_PIECE)) {
                                this->actor.textId = 0x1108;
                            } else {
                                this->actor.textId = 0x1101;
                            }
                            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_TALK,
                                                            &this->animIndex);
                        } else {
                            Audio_PlaySfx_MessageCancel();
                            this->actor.textId = 0x10FF;
                            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                            BEAVER_ANIM_TALK_TO_LEFT, &this->animIndex);
                            SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo,
                                                            BEAVER_ANIM_IDLE_FACE_RIGHT, &brother->animIndex);
                        }
                        break;

                    case 0x10FF:
                        func_80A98EFC(this, play, 0x1100, BEAVER_ANIM_IDLE_FACE_LEFT, BEAVER_ANIM_TALK_TO_RIGHT);
                        ret = 0;
                        break;

                    case 0x1100:
                        CLEAR_WEEKEVENTREG(WEEKEVENTREG_24_04);
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &this->animIndex);
                        SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &brother->animIndex);
                        func_80A94AB8(this, play, 0);
                        func_80A979DC(this, play);
                        ret = 0;
                        break;

                    case 0x1101:
                    case 0x1108:
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &this->animIndex);
                        SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &brother->animIndex);
                        this->unk_2FA = 8;
                        ret = 0;
                        break;

                    case 0x1102:
                        func_80A98EFC(this, play, 0x1103, BEAVER_ANIM_IDLE_FACE_LEFT, BEAVER_ANIM_TALK_TO_RIGHT);
                        ret = 0;
                        break;

                    case 0x1103:
                        func_80A98EFC(this, play, 0x1104, BEAVER_ANIM_IDLE_FACE_RIGHT, BEAVER_ANIM_TALK_TO_LEFT);
                        ret = 0;
                        break;

                    case 0x1104:
                        func_80A98EFC(this, play, 0x1105, BEAVER_ANIM_IDLE_FACE_LEFT, BEAVER_ANIM_TALK_TO_RIGHT);
                        ret = 0;
                        break;

                    case 0x1105:
                        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BEAVER_BROS_HEART_PIECE)) {
                            this->getItemId = GI_RUPEE_PURPLE;
                        } else {
                            this->getItemId = GI_HEART_PIECE;
                            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BEAVER_BROS_HEART_PIECE);
                        }
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &this->animIndex);
                        SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &brother->animIndex);
                        ret = 7;
                        break;

                    case 0x1106:
                        CLEAR_WEEKEVENTREG(WEEKEVENTREG_24_04);
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &this->animIndex);
                        SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &brother->animIndex);
                        func_80A94AB8(this, play, 0);
                        func_80A979DC(this, play);
                        ret = 0;
                        break;

                    case 0x10D5:
                    default:
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE,
                                                        &this->animIndex);
                        this->unk_374 |= 0x20;
                        ret = 0;
                        break;
                }
            }
            break;

        case TEXT_STATE_NONE:
        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_FADING:
        default:
            break;
    }
    return ret;
}

void func_80A97114(EnAz* this, PlayState* play) {
    EnAz* brother = this->brother;
    s32 sp20 = false;

    this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    switch (this->actor.textId) {
        case 0x10DA:
        case 0x10DD:
        case 0x10E9:
            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_TALK, &this->animIndex);
            break;

        case 0x10EE:
        case 0x10F3:
        case 0x10FB:
        case 0x1102:
            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_TALK_TO_LEFT,
                                            &this->animIndex);
            sp20 = true;
            break;

        case 0x10F2:
        case 0x1106:
        case 0x1109:
            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_BOW, &this->animIndex);
            if (brother != NULL) {
                SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_BOW,
                                                &brother->animIndex);
            }
            break;

        default:
            break;
    }
    if ((brother != NULL) && sp20) {
        if (this->unk_374 & 2) {
            SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE_FACE_RIGHT,
                                            &brother->animIndex);
        } else {
            SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE_FACE_LEFT,
                                            &brother->animIndex);
        }
    }
    this->unk_374 &= ~0x20;
    this->unk_378 = 2;
}

s32 func_80A97274(EnAz* this, PlayState* play) {
    s32 textId;

    if (this->unk_374 & 0x4000) {
        return 0x10D7;
    }
    switch (this->unk_2FA) {
        case 0:
        default:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_24_04);
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_16_40)) {
                textId = 0x10CE;
            } else {
                textId = 0x10D4;
            }
            break;

        case 2:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_24_01)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_24_04);
                this->unk_3C0 = 0;
                textId = 0x10DD;
            } else {
                textId = 0x10DA;
            }
            break;

        case 4:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_24_01)) {
                textId = 0x10EE;
            } else {
                textId = 0x10E9;
            }
            break;

        case 5:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_24_04);
            textId = 0x10F3;
            break;

        case 7:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_24_01)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_24_04);
                textId = 0x10FB;
            } else {
                textId = 0x10E9;
            }
            break;

        case 9:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_24_01)) {
                textId = 0x1102;
            } else {
                textId = 0x10E9;
            }
            break;
    }
    return textId;
}

s32 func_80A973B4(EnAz* this, PlayState* play) {
    s32 ret = 0;

    if (this->getItemId != GI_RUPEE_RED) {
        if ((this->getItemId != GI_RUPEE_PURPLE) && (this->getItemId != GI_HEART_PIECE)) {
            if (this->getItemId == GI_BOTTLE) {
                ret = 0x10F2;
            }
        } else {
            ret = 0x1106;
        }
    } else {
        ret = 0x1109;
    }
    this->unk_374 |= 0x20;

    return ret;
}

void func_80A97410(EnAz* this, PlayState* play) {
    s16 screenPosX;
    s16 screenPosY;
    s32 temp_a0;

    if (this->unk_378 != 0) {
        Vec3f* thisPos = &this->actor.world.pos;

        if ((this->unk_2FA == 0) && !(this->unk_374 & 2)) {
            Math_SmoothStepToS(&this->unk_3D4, 0, 2, 0x71C, 0xA);
            Math_SmoothStepToS(&this->unk_3D6, 0, 3, 0x71C, 0xA);
        }
        if (this->brother != NULL) {
            Vec3f* broPos = &this->brother->actor.world.pos;

            if (this->unk_374 & 0x40) {
                this->actor.focus.pos.x = (thisPos->x * 0.5f) + (broPos->x * 0.5f);
                this->actor.focus.pos.y = (thisPos->y * 0.5f) + (broPos->y * 0.5f) + 45.0f;
                this->actor.focus.pos.z = (thisPos->z * 0.5f) + (broPos->z * 0.5f);
            } else {
                this->actor.focus.pos.x = (thisPos->x * 0.7f) + (broPos->x * 0.3f);
                this->actor.focus.pos.y = (thisPos->y * 0.7f) + (broPos->y * 0.3f) + 45.0f;
                this->actor.focus.pos.z = (thisPos->z * 0.7f) + (broPos->z * 0.3f);
            }
        } else {
            this->actor.focus.pos.x = thisPos->x;
            this->actor.focus.pos.y = thisPos->y;
            this->actor.focus.pos.z = thisPos->z;
        }
        this->actor.focus.rot.x = this->actor.world.rot.x;
        this->actor.focus.rot.y = this->actor.world.rot.y;
        this->actor.focus.rot.z = this->actor.world.rot.z;
    }
    if (this->unk_378 == 2) {
        this->unk_378 = func_80A9617C(this, play);
        if (this->unk_378 == 0) {
            this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        }
    }
    if (this->unk_378 == 3) {
        Message_ContinueTextbox(play, this->actor.textId);
        this->unk_378 = 2;
    } else if (this->unk_378 == 5) {
        Message_StartTextbox(play, this->actor.textId, &this->actor);
        this->unk_378 = 2;
    } else {
        if ((this->unk_378 == 6) || (this->unk_378 == 7)) {
            if (Actor_HasParent(&this->actor, play)) {
                this->actor.parent = NULL;
                if (this->unk_378 == 7) {
                    this->unk_378 = 1;
                    this->unk_374 |= 0x20;
                } else {
                    this->unk_378 = 0;
                }
            } else {
                Actor_OfferGetItem(&this->actor, play, this->getItemId, this->actor.xzDistToPlayer,
                                   this->actor.playerHeightRel);
            }
        }
        if (this->unk_378 == 9) {
            temp_a0 = Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38, 0x222);
            this->actor.world.rot.y = this->actor.shape.rot.y;
            if (temp_a0 == 0) {
                switch (this->unk_3D2) {
                    case 0x10CE:
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_TALK,
                                                        &this->animIndex);
                        break;

                    case 0x10D4:
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                        BEAVER_ANIM_TALK_WAVE_ARMS, &this->animIndex);
                        break;

                    default:
                        break;
                }
                Message_StartTextbox(play, this->unk_3D2, &this->actor);
                this->actor.textId = this->unk_3D2;
                this->unk_378 = 2;
            }
        } else if (((this->unk_378 == 0) || (this->unk_378 == 1)) && (this->unk_374 & 0x20)) {
            if (this->unk_378 == 1) {
                if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
                    func_80A97114(this, play);
                    this->unk_378 = 2;
                } else if (Actor_OfferTalkExchange(&this->actor, play, this->actor.xzDistToPlayer,
                                                   this->actor.playerHeightRel, PLAYER_IA_MINUS1)) {
                    this->actor.textId = func_80A973B4(this, play);
                }
            } else {
                if ((this->unk_2FA == 0) && !(this->unk_374 & 2)) {
                    Player* player = GET_PLAYER(play);
                    Vec3f sp38;
                    s16 temp_a1_2 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
                    s16 temp_v0_7;

                    if (ABS(temp_a1_2) < 0x1800) {
                        Math_SmoothStepToS(&this->unk_3D4, temp_a1_2, 2, 0x71C, 0);
                    } else {
                        Math_SmoothStepToS(&this->unk_3D4, 0, 2, 0x71C, 0);
                    }
                    Math_Vec3f_Copy(&sp38, &player->actor.world.pos);
                    sp38.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
                    temp_v0_7 = Math_Vec3f_Pitch(&this->actor.focus.pos, &sp38);
                    if (ABS(temp_v0_7) < 0x800) {
                        Math_SmoothStepToS(&this->unk_3D6, temp_v0_7, 3, 0x71C, 0);
                    } else {
                        Math_SmoothStepToS(&this->unk_3D6, 0, 3, 0x71C, 0);
                    }
                }
                if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
                    func_80A97114(this, play);
                    this->unk_378 = 2;
                    if ((this->unk_3D2 == 0x10CE) || (this->unk_3D2 == 0x10D4)) {
                        this->unk_378 = 9;
                    }
                } else {
                    Actor_GetScreenPos(play, &this->actor, &screenPosX, &screenPosY);
                    if ((screenPosX >= 0) && (screenPosX <= SCREEN_WIDTH) && (screenPosY >= 0) &&
                        (screenPosY <= SCREEN_HEIGHT) &&
                        Actor_OfferTalkExchange(&this->actor, play, 120.0f, 120.0f, PLAYER_IA_NONE)) {
                        this->unk_3D2 = func_80A97274(this, play);
                        if ((this->unk_3D2 == 0x10CE) || (this->unk_3D2 == 0x10D4)) {
                            this->actor.textId = 0;
                        } else {
                            this->actor.textId = this->unk_3D2;
                        }
                    }
                }
            }
        }
    }
}

void func_80A979DC(EnAz* this, PlayState* play) {
    this->actionFunc = func_80A979F4;
}

void func_80A979F4(EnAz* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Actor_MoveWithGravity(&this->actor);
}

void func_80A97A28(EnAz* this, PlayState* play) {
    this->actionFunc = func_80A97A40;
}

void func_80A97A40(EnAz* this, PlayState* play) {
    if (SubS_StartCutscene(&this->actor, 0, CS_ID_NONE, SUBS_CUTSCENE_WITH_PLAYER)) {
        play->msgCtx.msgMode = MSGMODE_NONE;
        play->msgCtx.msgLength = 0;
        func_80A97A9C(this, play);
    }
}

void func_80A97A9C(EnAz* this, PlayState* play) {
    this->actionFunc = func_80A97AB4;
}

void func_80A97AB4(EnAz* this, PlayState* play) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
            Message_StartTextbox(play, 0x10D7, NULL);
            break;

        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0x10D7:
                        Message_ContinueTextbox(play, 0x10D8);
                        break;

                    case 0x10D8:
                        if (play->msgCtx.choiceIndex == 0) {
                            Audio_PlaySfx_MessageDecide();
                            play->msgCtx.msgMode = MSGMODE_PAUSED;
                            Environment_ForcePlaySequence(NA_BGM_TIMED_MINI_GAME);
                            func_80A94AB8(this, play, 1);
                            func_80A979DC(this, play);
                        } else {
                            Audio_PlaySfx_MessageCancel();
                            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_24_04)) {
                                CLEAR_WEEKEVENTREG(WEEKEVENTREG_24_04);
                            }
                            Message_ContinueTextbox(play, 0x10D9);
                        }
                        break;

                    case 0x10D9:
                        func_80A94AB8(this, play, 0);
                        func_80A979DC(this, play);
                        break;

                    default:
                        break;
                }
            }
        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
        default:
            break;
    }
}

void func_80A97C0C(EnAz* this, PlayState* play) {
    this->actionFunc = func_80A97C4C;
}

void func_80A97C24(EnAz* this, PlayState* play) {
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->actionFunc = func_80A97C4C;
}

void func_80A97C4C(EnAz* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime) && (this->animIndex == BEAVER_ANIM_BOW)) {
        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_IDLE, &this->animIndex);
    }
    func_80A97410(this, play);
    if ((this->unk_2FA == 1) || (this->unk_2FA == 3) || (this->unk_2FA == 6) || (this->unk_2FA == 8)) {
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_24_01);
        Environment_ForcePlaySequence(NA_BGM_TIMED_MINI_GAME);
        play->nextEntrance = Entrance_CreateFromSpawn(1);
        gSaveContext.nextCutsceneIndex = 0;
        play->transitionTrigger = TRANS_TRIGGER_START;
        play->transitionType = TRANS_TYPE_80;
        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
        func_80A979DC(this, play);
    } else {
        Actor_MoveWithGravity(&this->actor);
    }
}

void func_80A97D5C(EnAz* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->stateFlags1 |= PLAYER_STATE1_20;
    Interface_InitMinigame(play);
    gSaveContext.minigameScore = (this->unk_374 & 2) ? 25 : 20;
    play->interfaceCtx.minigameState = MINIGAME_STATE_COUNTDOWN_SETUP_3;
    if ((this->unk_2FA == 1) || (this->unk_2FA == 3)) {
        Interface_StartTimer(TIMER_ID_MINIGAME_2, 120);
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BEAVER_BROS_HEART_PIECE)) {
        Interface_StartTimer(TIMER_ID_MINIGAME_2, 100);
    } else {
        Interface_StartTimer(TIMER_ID_MINIGAME_2, 110);
    }
    this->actionFunc = func_80A97E48;
}

void func_80A97E48(EnAz* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->interfaceCtx.minigameState >= MINIGAME_STATE_COUNTDOWN_GO) {
        player->stateFlags1 &= ~PLAYER_STATE1_20;
        func_80A97EAC(this, play);
    }
    Actor_MoveWithGravity(&this->actor);
    SkelAnime_Update(&this->skelAnime);
}

void func_80A97EAC(EnAz* this, PlayState* play) {
    SubS_ActorPathing_Init(play, &this->actor.world.pos, &this->actor, &this->unk_300, play->setupPathList,
                           BEAVER_GET_PATH_INDEX(&this->actor), 0, 0, 1, 0);
    this->unk_36C = 8.0f;
    this->actor.speed = 8.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 6.0f;
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, BEAVER_ANIM_SWIM_WITH_SPINNING_TAIL,
                                    &this->animIndex);
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    this->actor.bgCheckFlags &= ~(BGCHECKFLAG_GROUND | BGCHECKFLAG_WATER);
    this->unk_374 |= 0x1000;
    this->unk_3C2 = 0;
    this->unk_3C4 = 0;
    this->actionFunc = func_80A97F9C;
}

void func_80A97F9C(EnAz* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    D_80A99E8C = Math3D_Dist2DSq(player->actor.world.pos.x, player->actor.world.pos.z, D_80A99E80.x, D_80A99E80.z);
    if (Math3D_Dist2DSq(this->actor.world.pos.x, this->actor.world.pos.z, D_80A99E80.x, D_80A99E80.z) >= SQ(1000.0f)) {
        this->unk_374 |= 0x1000;
    }
    if (!(this->unk_300.flags & ACTOR_PATHING_REACHED_END_PERMANENT)) {
        SubS_ActorPathing_Update(play, &this->unk_300, func_80A982E0, func_80A95B34, SubS_ActorPathing_MoveWithGravity,
                                 SubS_ActorPathing_SetNextPoint);
    }
    if (!(this->unk_374 & 0x10) &&
        SurfaceType_IsHorseBlocked(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId)) {
        this->unk_374 |= 0x10;
    }
    if (SurfaceType_IsHorseBlocked(&play->colCtx, player->actor.floorPoly, player->actor.floorBgId)) {
        if (func_80A94B98(this, play)) {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_24_01);
        } else {
            SET_WEEKEVENTREG(WEEKEVENTREG_24_01);
        }
        gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_STOP;
        this->unk_374 &= ~0x10;
        play->nextEntrance = Entrance_CreateFromSpawn(2);
        gSaveContext.nextCutsceneIndex = 0;
        play->transitionTrigger = TRANS_TRIGGER_START;
        play->transitionType = TRANS_TYPE_FADE_WHITE;
        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
        this->actor.speed = 0.0f;
        func_80A979DC(this, play);
    } else {
        if (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] == SECONDS_TO_TIMER(0)) {
            gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_STOP;
            this->unk_374 |= 0x4000;
            func_80A97A28(this, play);
        }
        if (this->unk_374 & 0x100) {
            if (this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) {
                func_80A98414(this, play);
            }
            if ((DECR(this->unk_37A) == 0) && (this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME)) {
                EffectSsBubble_Spawn(play, &this->actor.world.pos, 0.0f, 20.0f, 20.0f, 0.35f);
                this->unk_37A = (Rand_ZeroOne() * 70.0f) + 10.0f;
            }
        }
        if (this->actor.depthInWater > 8.0f) {
            if (this->unk_374 & 2) {
                if ((this->skelAnime.curFrame < this->skelAnime.playSpeed) && (this->skelAnime.curFrame >= 0.0f)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EV_BEAVER_SWIM_HAND);
                }
            } else {
                Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_BEAVER_SWIM_MOTOR - SFX_FLAG);
            }
        }
        SkelAnime_Update(&this->skelAnime);
    }
}

void func_80A982E0(PlayState* play, ActorPathing* actorPathing) {
    EnAz* this = (EnAz*)actorPathing->actor;
    Vec3f sp28;

    actorPathing->curPoint.x = actorPathing->points[actorPathing->curPointIndex].x;
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        actorPathing->curPoint.y = actorPathing->points[actorPathing->curPointIndex].y;
    } else {
        actorPathing->curPoint.y = actorPathing->points[actorPathing->curPointIndex].y - this->unk_3A4;
    }
    actorPathing->curPoint.z = actorPathing->points[actorPathing->curPointIndex].z;
    sp28.x = actorPathing->curPoint.x - actorPathing->worldPos->x;
    sp28.y = actorPathing->curPoint.y - actorPathing->worldPos->y;
    sp28.z = actorPathing->curPoint.z - actorPathing->worldPos->z;
    actorPathing->distSqToCurPointXZ = Math3D_Dist1DSq(sp28.x, sp28.z);
    actorPathing->distSqToCurPoint = Math3D_Vec3fMagnitudeSq(&sp28);
    actorPathing->rotToCurPoint.y = Math_Atan2S_XY(sp28.z, sp28.x);
    actorPathing->rotToCurPoint.x = Math_Atan2S_XY(sqrtf(actorPathing->distSqToCurPointXZ), -sp28.y);
    actorPathing->rotToCurPoint.z = 0;
}

void func_80A98414(EnAz* this, PlayState* play) {
    Actor* itemAction = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].first;

    while (itemAction != NULL) {
        if (itemAction->id == ACTOR_EN_FISH) {
            EnFish* fish = (EnFish*)itemAction;

            if ((fish->actor.params < 0) && (fish->actor.room == this->actor.room) &&
                (Math3D_Vec3fDistSq(&this->actor.world.pos, &fish->actor.world.pos) < SQ(200.0f))) {
                fish->unk_276 = 0x14;
                fish->unk_274 = Actor_WorldYawTowardActor(&fish->actor, &this->actor);
            }
        }
        itemAction = itemAction->next;
    }
}

void EnAz_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnAz* this = (EnAz*)thisx;

    this->unk_374 &= ~0x100;
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WATER) && (this->actor.depthInWater > 22.0f)) {
        if (!(this->unk_376 & 0x100)) {
            this->unk_374 |= 0x200;
        }
        this->unk_374 |= 0x100;
    } else if (this->unk_376 & 0x100) {
        this->unk_374 |= 0x400;
    }
    this->actionFunc(this, play);
    Actor_SetFocus(&this->actor, 40.0f);
    if (this->unk_374 & 0x200) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_DIVE_INTO_WATER);
    }
    if (this->unk_374 & 0x400) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_JUMP_OUT_WATER);
    }
    this->unk_37E++;
    if (this->unk_37E >= 4) {
        this->unk_37E = 0;
    }
    this->unk_380++;
    if (this->unk_380 >= 3) {
        this->unk_380 = 0;
    }
    this->unk_384--;
    if (this->unk_384 < 0) {
        this->unk_384 = 2;
    }
    this->unk_39C++;
    if (this->unk_39C >= 16) {
        this->unk_39C = 0;
    }
    if (!(this->unk_374 & 0x1000)) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 20.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    } else {
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 20.0f, UPDBGCHECKINFO_FLAG_400);
        this->unk_374 &= ~0x1000;
    }
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

    this->unk_376 = this->unk_374;
    this->unk_374 &= ~0x600;

    if (GET_ACTIVE_CAM(play)->stateFlags & CAM_STATE_UNDERWATER) {
        this->unk_374 |= 0x800;
    } else {
        this->unk_374 &= ~0x800;
    }
}

static Gfx* D_80A9916C[] = {
    gBeaverOlderBrotherTailVortex1DL, gBeaverOlderBrotherTailVortex2DL, gBeaverOlderBrotherTailVortex3DL,
    gBeaverOlderBrotherTailVortex4DL, gBeaverOlderBrotherTailVortex5DL,
};
static AnimatedMaterial* D_80A99180[] = {
    gBeaverOlderBrotherTailVortex1TexAnim, gBeaverOlderBrotherTailVortex2TexAnim, gBeaverOlderBrotherTailVortex3TexAnim,
    gBeaverOlderBrotherTailVortex4TexAnim, gBeaverOlderBrotherTailVortex5TexAnim,
};
static u8 D_80A99194[] = {
    95, 135, 175, 215, 255,
};
static u8 D_80A9919C[] = {
    31, 45, 58, 73, 85,
};
static struct_80124618 D_80A991A4[5][9] = {
    {
        { 0, { 200, 160, 160 } },
        { 2, { 250, 210, 210 } },
        { 4, { 200, 160, 160 } },
        { 6, { 250, 210, 210 } },
        { 8, { 200, 160, 160 } },
        { 10, { 250, 210, 210 } },
        { 12, { 200, 160, 160 } },
        { 14, { 250, 210, 210 } },
        { 16, { 200, 160, 160 } },
    },
    {
        { 0, { 330, 200, 200 } },
        { 2, { 280, 150, 150 } },
        { 4, { 330, 200, 200 } },
        { 6, { 280, 150, 150 } },
        { 8, { 330, 200, 200 } },
        { 10, { 280, 150, 150 } },
        { 12, { 330, 200, 200 } },
        { 14, { 280, 150, 150 } },
        { 16, { 330, 200, 200 } },
    },
    {
        { 0, { 350, 170, 120 } },
        { 2, { 400, 220, 170 } },
        { 4, { 350, 170, 120 } },
        { 6, { 400, 220, 170 } },
        { 8, { 350, 170, 120 } },
        { 10, { 400, 220, 170 } },
        { 12, { 350, 170, 120 } },
        { 14, { 400, 220, 170 } },
        { 16, { 350, 170, 120 } },
    },
    {
        { 0, { 480, 216, 156 } },
        { 2, { 540, 216, 156 } },
        { 4, { 450, 300, 216 } },
        { 6, { 540, 216, 156 } },
        { 8, { 450, 300, 216 } },
        { 10, { 540, 216, 156 } },
        { 12, { 450, 300, 216 } },
        { 14, { 540, 300, 156 } },
        { 16, { 450, 250, 216 } },
    },
    {
        { 0, { 600, 375, 160 } },
        { 2, { 640, 510, 200 } },
        { 4, { 600, 375, 160 } },
        { 6, { 640, 510, 200 } },
        { 8, { 600, 375, 160 } },
        { 10, { 640, 510, 200 } },
        { 12, { 600, 375, 160 } },
        { 14, { 640, 510, 200 } },
        { 16, { 600, 375, 160 } },
    },
};
static struct_80A98F94 D_80A9930C[5][4] = {
    { { 0, 2100.0f }, { 7, 1500.0f }, { 16, 2100.0f }, { 0, 0.0f } },
    { { 0, 1900.0f }, { 7, 2200.0f }, { 10, 1600.0f }, { 16, 1900.0f } },
    { { 0, 1700.0f }, { 7, 2200.0f }, { 16, 1700.0f }, { 0, 0.0f } },
    { { 0, 1900.0f }, { 4, 1600.0f }, { 10, 2200.0f }, { 16, 1900.0f } },
    { { 0, 1900.0f }, { 7, 1400.0f }, { 16, 1900.0f }, { 0, 0.0f } },
};
static Vec3f D_80A993AC[] = {
    { 1.0f, 1.0f, 0.0f },
    { 0.9f, 0.9f, 120.0f },
    { 0.95f, 0.95f, 240.0f },
};
static Vec3f D_80A993D0[] = {
    { 1.5f, 1.5f, 0.0f },
    { 1.2f, 1.2f, 120.0f },
    { 1.35f, 1.35f, 240.0f },
};
static TexturePtr sYoungerBrotherEyeTextures[] = {
    gBeaverYoungerBrotherEye1Tex,
    gBeaverYoungerBrotherEye2Tex,
    gBeaverYoungerBrotherEye3Tex,
    gBeaverYoungerBrotherEye4Tex,
};
static TexturePtr sYoungerBrotherBeltTextures[] = {
    gBeaverYoungerBrotherBeltRedTex,
    gBeaverYoungerBrotherBeltGreenTex,
    gBeaverYoungerBrotherBeltBlueTex,
};

void EnAz_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnAz* this = (EnAz*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    POLY_OPA_DISP = Gfx_SetupDL(POLY_OPA_DISP, SETUPDL_25);

    CLOSE_DISPS(play->state.gfxCtx);

    if (this->unk_374 & 2) {
        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              EnAz_OverrideLimbDraw, EnAz_PostLimbDraw, &this->actor);
    } else {
        OPEN_DISPS(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sYoungerBrotherEyeTextures[this->unk_37E]));
        gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sYoungerBrotherBeltTextures[this->unk_380]));
        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              EnAz_OverrideLimbDraw, EnAz_PostLimbDraw, &this->actor);

        CLOSE_DISPS(play->state.gfxCtx);
    }
    OPEN_DISPS(play->state.gfxCtx);

    if ((this->actor.depthInWater >= 28.0f) && (this->actor.speed > 0.5f)) {
        Matrix_Translate(this->unk_3B4.x, this->unk_3B4.y, this->unk_3B4.z, MTXMODE_NEW);
        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->actor.shape.rot.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->actor.shape.rot.z, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_39E, MTXMODE_APPLY);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        if (this->unk_374 & 2) {
            s32 i;
            Vec3f sp98;
            f32 sp94;

            Matrix_Translate(0.0f, 0.0f, -2000.0f, MTXMODE_APPLY);
            for (i = 0; i < ARRAY_COUNT(D_80A99180); i++) {
                Matrix_Push();
                func_80A98F94(D_80A9930C[i], this->unk_39C, &sp94);
                AnimatedMat_Draw(play, Lib_SegmentedToVirtual(D_80A99180[i]));
                Matrix_Translate(0.0f, sp94 * 100.0f, i * -930.0f, MTXMODE_APPLY);
                Matrix_Scale(1.1f, 0.95f, 1.0f, MTXMODE_APPLY);
                func_80124618(D_80A991A4[i], this->unk_39C, &sp98);
                Matrix_Scale(sp98.x, sp98.y, sp98.z, MTXMODE_APPLY);
                if (this->unk_374 & 0x800) {
                    gSPSegment(POLY_XLU_DISP++, 0x09,
                               Gfx_PrimColor(play->state.gfxCtx, 0x80, 255, 255, 255, D_80A99194[i]));
                } else {
                    gSPSegment(POLY_XLU_DISP++, 0x09,
                               Gfx_PrimColor(play->state.gfxCtx, 0x80, 255, 255, 255, D_80A9919C[i]));
                }
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, D_80A9916C[i]);
                Matrix_Pop();
            }
        } else {
            Matrix_Push();
            Matrix_Translate(0.0f, 2000.0f, -2000.0f, MTXMODE_APPLY);
            Matrix_RotateZS(DEG_TO_BINANG(D_80A993D0[this->unk_384].z), MTXMODE_APPLY);
            Matrix_Scale(D_80A993AC[this->unk_384].x, D_80A993AC[this->unk_384].y, 0.0f, MTXMODE_APPLY);
            if (this->unk_374 & 0x800) {
                gSPSegment(POLY_XLU_DISP++, 0x08, Gfx_PrimColor(play->state.gfxCtx, 0x80, 255, 255, 255, 255));
            } else {
                gSPSegment(POLY_XLU_DISP++, 0x08, Gfx_PrimColor(play->state.gfxCtx, 0x80, 255, 255, 255, 85));
            }
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gBeaverYoungerBrotherTailVortexDL);
            Matrix_Pop();
            Matrix_Translate(0.0f, 2000.0f, -2100.0f, MTXMODE_APPLY);
            Matrix_RotateZS(DEG_TO_BINANG(D_80A993D0[this->unk_384].z), MTXMODE_APPLY);
            Matrix_Scale(D_80A993D0[this->unk_384].x, D_80A993D0[this->unk_384].y, 0.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gBeaverYoungerBrotherTailSplashDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnAz_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnAz* this = (EnAz*)thisx;

    if ((limbIndex == BEAVER_OLDER_BROTHER_LIMB_NONE) && ((play->gameplayFrames % 2) != 0)) {
        *dList = NULL;
    }
    if (limbIndex == BEAVER_OLDER_BROTHER_LIMB_NONE) {
        // Set to itself
        rot->x = rot->x;
        rot->y = rot->y;
        rot->z = rot->z;
    }
    if (limbIndex == BEAVER_OLDER_BROTHER_LIMB_PELVIS) {
        rot->y -= this->unk_39E;
    } else {
        // this space intentionally left blank
    }
    if (limbIndex == BEAVER_OLDER_BROTHER_LIMB_HEAD_ROOT) {
        rot->x += this->unk_3D4;
        rot->y -= this->unk_3D6;
    }
    return false;
}

void EnAz_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80A99410 = { 700.0f, 0.0f, 0.0f };
    static Vec3f D_80A9941C = { -500.0f, 0.0f, 0.0f };
    static Vec3f D_80A99428 = { -1200.0f, 0.0f, 1000.0f };
    EnAz* this = (EnAz*)thisx;

    if (limbIndex == BEAVER_OLDER_BROTHER_LIMB_PELVIS) {
        Matrix_MultVec3f(&D_80A99410, &this->unk_3A8);
        Math_SmoothStepToF(&this->unk_3A4, this->unk_3A8.y - this->actor.world.pos.y, 0.8f, 10.0f, 0.01f);
        if (this->unk_374 & 2) {
            Matrix_MultVec3f(&D_80A9941C, &this->unk_3B4);
        } else {
            Matrix_MultVec3f(&D_80A99428, &this->unk_3B4);
        }
    }
}

void func_80A98EFC(EnAz* this, PlayState* play, u16 textId, s32 animIndex, s32 brotherAnimIndex) {
    EnAz* brother = this->brother;

    Actor_ChangeFocus(&this->actor, play, &brother->actor);
    if (animIndex > BEAVER_ANIM_NONE) {
        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, animIndex, &this->animIndex);
    }
    this->actor.textId = 0;
    brother->actor.textId = textId;
    brother->unk_378 = 5;
    if ((brotherAnimIndex > BEAVER_ANIM_NONE) && (brotherAnimIndex != brother->animIndex)) {
        SubS_ChangeAnimationBySpeedInfo(&brother->skelAnime, sAnimationSpeedInfo, brotherAnimIndex,
                                        &brother->animIndex);
    }
    this->unk_378 = 0;
}

void func_80A98F94(struct_80A98F94* yData, f32 frame, f32* yInterp) {
    f32 nextFrame;
    f32 prevFrame;
    f32 weight;

    do {
        yData++;
        nextFrame = yData[0].unk_0;
    } while (nextFrame < frame);

    prevFrame = yData[-1].unk_0;
    weight = LERPWEIGHT(frame, prevFrame, nextFrame);
    *yInterp = LERPIMP(yData[-1].unk_4, yData[0].unk_4, weight) * 0.01f;
}
