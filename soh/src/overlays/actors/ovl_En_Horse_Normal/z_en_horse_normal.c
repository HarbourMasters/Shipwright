/*
 * File: z_en_horse_normal.c
 * Overlay: ovl_En_Horse_Normal
 * Description: Non-rideable horses (Lon Lon Ranch and Stable)
 */

#include "z_en_horse_normal.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_horse_normal/object_horse_normal.h"

#define FLAGS 0

typedef struct {
    Vec3s pos;
    u8 unk_06; // this may be a s16 if the always-0 following byte is actually not padding
} EnHorseNormalUnkStruct1;

typedef struct {
    s32 len;
    EnHorseNormalUnkStruct1* items;
} EnHorseNormalUnkStruct2;

typedef enum {
    /* 0x00 */ HORSE_CYCLE_ANIMATIONS,
    /* 0x01 */ HORSE_WANDER,
    /* 0x02 */ HORSE_WAIT,
    /* 0x03 */ HORSE_WAIT_CLONE,
    /* 0x04 */ HORSE_FOLLOW_PATH
} EnHorseNormalAction;

void EnHorseNormal_Init(Actor* thisx, PlayState* play);
void EnHorseNormal_Destroy(Actor* thisx, PlayState* play);
void EnHorseNormal_Update(Actor* thisx, PlayState* play);
void EnHorseNormal_Draw(Actor* thisx, PlayState* play);

void func_80A6B91C(EnHorseNormal* this, PlayState* play);
void func_80A6BC48(EnHorseNormal* this);
void func_80A6BCEC(EnHorseNormal* this);
void func_80A6C4CC(EnHorseNormal* this);
void func_80A6C6B0(EnHorseNormal* this);

const ActorInit En_Horse_Normal_InitVars = {
    ACTOR_EN_HORSE_NORMAL,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HORSE_NORMAL,
    sizeof(EnHorseNormal),
    (ActorFunc)EnHorseNormal_Init,
    (ActorFunc)EnHorseNormal_Destroy,
    (ActorFunc)EnHorseNormal_Update,
    (ActorFunc)EnHorseNormal_Draw,
    NULL,
};

static AnimationHeader* sAnimations[] = {
    &gHorseNormalIdleAnim,      &gHorseNormalWhinnyAnim,  &gHorseNormalRefuseAnim,
    &gHorseNormalRearingAnim,   &gHorseNormalWalkingAnim, &gHorseNormalTrottingAnim,
    &gHorseNormalGallopingAnim, &gHorseNormalJumpingAnim, &gHorseNormalJumpingHighAnim,
};

static ColliderCylinderInit sCylinderInit1 = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
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
    { 40, 100, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit2 = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
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
    { 60, 100, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementsInit[] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 11, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 10, 35, 100, MASS_HEAVY };

// Unused
static EnHorseNormalUnkStruct1 D_80A6D428[] = {
    { { 1058, 1, 384 }, 7 },    { { 1653, 39, -381 }, 6 }, { { 1606, 1, -1048 }, 6 }, { { 1053, 1, -1620 }, 6 },
    { { -1012, 1, -1633 }, 7 }, { { -1655, 1, -918 }, 6 }, { { -1586, 1, -134 }, 6 }, { { -961, 1, 403 }, 7 },
};

// Unused
static EnHorseNormalUnkStruct2 D_80A6D468 = { ARRAY_COUNT(D_80A6D428), D_80A6D428 };

// Unused
static EnHorseNormalUnkStruct1 D_80A6D470[] = {
    { { 88, 0, 2078 }, 10 },       { { 2482, 376, 4631 }, 7 },    { { 2228, -28, 6605 }, 12 },
    { { 654, -100, 8864 }, 7 },    { { -297, -500, 10667 }, 12 }, { { -5303, -420, 10640 }, 10 },
    { { -6686, -500, 7760 }, 10 }, { { -5260, 100, 5411 }, 7 },   { { -3573, -269, 3893 }, 10 },
};

// Unused
static EnHorseNormalUnkStruct2 D_80A6D4B8 = { ARRAY_COUNT(D_80A6D470), D_80A6D470 };

void func_80A6B250(EnHorseNormal* this) {
    static s32 D_80A6D4C0[] = { 0, 16 };

    if (D_80A6D4C0[this->unk_200] < this->skin.skelAnime.curFrame &&
        ((this->unk_200 != 0) || !(D_80A6D4C0[1] < this->skin.skelAnime.curFrame))) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_WALK, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        this->unk_200++;
        if (this->unk_200 >= ARRAY_COUNT(D_80A6D4C0)) {
            this->unk_200 = 0;
        }
    }
}

f32 func_80A6B30C(EnHorseNormal* this) {
    static f32 D_80A6D4C8[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 1.5f, 1.5f, 1.5f, 1.0f };
    f32 result;

    if (this->animationIdx == 4) {
        result = D_80A6D4C8[this->animationIdx] * this->actor.speedXZ * (1 / 2.0f);
    } else if (this->animationIdx == 5) {
        result = D_80A6D4C8[this->animationIdx] * this->actor.speedXZ * (1 / 3.0f);
    } else if (this->animationIdx == 6) {
        result = D_80A6D4C8[this->animationIdx] * this->actor.speedXZ * (1 / 5.0f);
    } else {
        result = D_80A6D4C8[this->animationIdx];
    }

    return result;
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 300, ICHAIN_STOP),
};

void EnHorseNormal_Init(Actor* thisx, PlayState* play) {
    EnHorseNormal* this = (EnHorseNormal*)thisx;
    s32 pad;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.gravity = -3.5f;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawHorse, 20.0f);
    this->actor.speedXZ = 0.0f;
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 70.0f;
    this->action = HORSE_CYCLE_ANIMATIONS;
    this->animationIdx = 0;
    Collider_InitCylinder(play, &this->bodyCollider);
    Collider_SetCylinder(play, &this->bodyCollider, &this->actor, &sCylinderInit1);
    Collider_InitJntSph(play, &this->headCollider);
    Collider_SetJntSph(play, &this->headCollider, &this->actor, &sJntSphInit, this->headElements);
    Collider_InitCylinder(play, &this->cloneCollider);
    Collider_SetCylinder(play, &this->cloneCollider, &this->actor, &sCylinderInit2);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);
    if (play->sceneNum == SCENE_SPOT20) {
        if (this->actor.world.rot.z == 0 || !IS_DAY) {
            Actor_Kill(&this->actor);
            return;
        }
        if (!LINK_IS_ADULT) {
            if (Flags_GetEventChkInf(0x14)) {
                if (this->actor.world.rot.z != 3) {
                    Actor_Kill(&this->actor);
                    return;
                }
            } else if (this->actor.world.rot.z != 1) {
                Actor_Kill(&this->actor);
                return;
            }
        } else if (Flags_GetEventChkInf(0x18) || (DREG(1) != 0)) {
            if (this->actor.world.rot.z != 7) {
                Actor_Kill(&this->actor);
                return;
            }
        } else if (this->actor.world.rot.z != 5) {
            Actor_Kill(&this->actor);
            return;
        }
        this->actor.home.rot.z = this->actor.world.rot.z = this->actor.shape.rot.z = 0;
        Skin_Init(play, &this->skin, &gHorseNormalSkel, &gHorseNormalIdleAnim);
        Animation_PlayOnce(&this->skin.skelAnime, sAnimations[this->animationIdx]);
        if ((this->actor.world.pos.x == -730.0f && this->actor.world.pos.y == 0.0f &&
             this->actor.world.pos.z == -1100.0f) ||
            (this->actor.world.pos.x == 880.0f && this->actor.world.pos.y == 0.0f &&
             this->actor.world.pos.z == -1170.0f)) {
            func_80A6C6B0(this);
            return;
        }
    } else if (play->sceneNum == SCENE_MALON_STABLE) {
        if (IS_DAY) {
            Actor_Kill(&this->actor);
            return;
        } else {
            Skin_Init(play, &this->skin, &gHorseNormalSkel, &gHorseNormalIdleAnim);
            Animation_PlayOnce(&this->skin.skelAnime, sAnimations[this->animationIdx]);
            func_80A6C6B0(this);
            return;
        }
    } else if (play->sceneNum == SCENE_SPOT12) {
        if (this->actor.world.pos.x == 3707.0f && this->actor.world.pos.y == 1413.0f &&
            this->actor.world.pos.z == -665.0f) {
            Skin_Init(play, &this->skin, &gHorseNormalSkel, &gHorseNormalIdleAnim);
            Animation_PlayOnce(&this->skin.skelAnime, sAnimations[this->animationIdx]);
            func_80A6C4CC(this);
            return;
        }
        Skin_Init(play, &this->skin, &gHorseNormalSkel, &gHorseNormalIdleAnim);
        Animation_PlayOnce(&this->skin.skelAnime, sAnimations[this->animationIdx]);
    } else {
        Skin_Init(play, &this->skin, &gHorseNormalSkel, &gHorseNormalIdleAnim);
        Animation_PlayOnce(&this->skin.skelAnime, sAnimations[this->animationIdx]);
    }
    if ((this->actor.params & 0xF0) == 0x10 && (this->actor.params & 0xF) != 0xF) {
        func_80A6B91C(this, play);
    } else {
        func_80A6BC48(this);
    }
}

void EnHorseNormal_Destroy(Actor* thisx, PlayState* play) {
    EnHorseNormal* this = (EnHorseNormal*)thisx;

    Skin_Free(play, &this->skin);
    Collider_DestroyCylinder(play, &this->bodyCollider);
    Collider_DestroyCylinder(play, &this->cloneCollider);
    Collider_DestroyJntSph(play, &this->headCollider);
}

void func_80A6B91C(EnHorseNormal* this, PlayState* play) {
    this->actor.flags |= ACTOR_FLAG_4;
    this->action = HORSE_FOLLOW_PATH;
    this->animationIdx = 6;
    this->waypoint = 0;
    this->actor.speedXZ = 7.0f;
    Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A6B30C(this), 0.0f,
                     Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
}

void EnHorseNormal_FollowPath(EnHorseNormal* this, PlayState* play) {
    Path* path = &play->setupPathList[this->actor.params & 0xF];
    Vec3s* pointPos = SEGMENTED_TO_VIRTUAL(path->points);
    f32 dx;
    f32 dz;
    s32 pad;

    pointPos += this->waypoint;
    dx = pointPos->x - this->actor.world.pos.x;
    dz = pointPos->z - this->actor.world.pos.z;
    Math_SmoothStepToS(&this->actor.world.rot.y, Math_FAtan2F(dx, dz) * (0x8000 / M_PI), 0xA, 0x7D0, 1);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if (SQ(dx) + SQ(dz) < 600.0f) {
        this->waypoint++;
        if (this->waypoint >= path->count) {
            this->waypoint = 0;
        }
    }
    this->skin.skelAnime.playSpeed = func_80A6B30C(this);
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A6B30C(this), 0.0f,
                         Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
        func_80A6BCEC(this);
    }
}

void EnHorseNormal_NextAnimation(EnHorseNormal* this) {
    this->action = HORSE_CYCLE_ANIMATIONS;
    this->animationIdx++;

    if (this->animationIdx >= ARRAY_COUNT(sAnimations)) {
        this->animationIdx = 0;
    }

    Animation_PlayOnce(&this->skin.skelAnime, sAnimations[this->animationIdx]);
}

void EnHorseNormal_CycleAnimations(EnHorseNormal* this, PlayState* play) {
    this->actor.speedXZ = 0.0f;

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        EnHorseNormal_NextAnimation(this);
    }
}

void func_80A6BC48(EnHorseNormal* this) {
    this->action = HORSE_WANDER;
    this->animationIdx = 0;
    this->unk_21C = 0;
    this->unk_21E = 0;
    this->actor.speedXZ = 0.0f;
    this->unk_218 = 0.0f;
    Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A6B30C(this), 0.0f,
                     Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
}

void func_80A6BCEC(EnHorseNormal* this) {
    if (this->animationIdx == 5) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (this->animationIdx == 6) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
}

void func_80A6BD7C(EnHorseNormal* this) {
    f32 frame = this->skin.skelAnime.curFrame;

    if (this->animationIdx == 0 && frame > 28.0f && !(this->unk_1E4 & 1)) {
        this->unk_1E4 |= 1;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_SANDDUST, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    } else if (this->animationIdx == 3 && frame > 25.0f && !(this->unk_1E4 & 2)) {
        this->unk_1E4 |= 2;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND2, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
}

void EnHorseNormal_Wander(EnHorseNormal* this, PlayState* play) {
    static s32 D_80A6D4F4[] = { 0, 1, 4, 5, 6, 2, 3 };
    static s32 D_80A6D510[] = { 0, 0, 2, 2, 1, 1, 1, 3, 3 };
    s32 phi_t0 = this->animationIdx;
    s32 pad;

    switch (D_80A6D510[this->animationIdx]) {
        case 0:
            func_80A6BD7C(this);
            this->actor.speedXZ = 0.0f;
            this->unk_218 = 0.0f;
            break;
        case 1:
            if (Rand_ZeroOne() < 0.1f) {
                this->unk_218 = 2.0f * Rand_ZeroOne() - 1.0f;
            }
            this->actor.speedXZ += this->unk_218;
            if (this->actor.speedXZ <= 0.0f) {
                this->actor.speedXZ = 0.0f;
                this->unk_218 = 0.0f;
                phi_t0 = 0;
            } else if (this->actor.speedXZ < 3.0f) {
                func_80A6B250(this);
                phi_t0 = 4;
            } else if (this->actor.speedXZ < 6.0f) {
                phi_t0 = 5;
            } else if (this->actor.speedXZ < 8.0f) {
                phi_t0 = 6;
            } else {
                this->actor.speedXZ = 8.0f;
                phi_t0 = 6;
            }
            if (Rand_ZeroOne() < 0.1f || (this->unk_21E == 0 && ((this->actor.bgCheckFlags & 8) ||
                                                                 (this->bodyCollider.base.ocFlags1 & OC1_HIT) ||
                                                                 (this->headCollider.base.ocFlags1 & OC1_HIT)))) {
                this->unk_21E += (Rand_ZeroOne() * 30.0f) - 15.0f;
                if (this->unk_21E > 50) {
                    this->unk_21E = 50;
                } else if (this->unk_21E < -50) {
                    this->unk_21E = -50;
                }
            }
            this->unk_21C += this->unk_21E;
            if (this->unk_21C < -300) {
                this->unk_21C = -300;
            } else if (this->unk_21C > 300) {
                this->unk_21C = 300;
            } else if (Rand_ZeroOne() < 0.25f && fabsf(this->unk_21C) < 100.0f) {
                this->unk_21C = 0;
                this->unk_21E = 0;
            }
            this->actor.world.rot.y += this->unk_21C;
            this->actor.shape.rot.y = this->actor.world.rot.y;
            break;
        case 2:
        case 3:
            break;
    }

    if (phi_t0 != this->animationIdx || SkelAnime_Update(&this->skin.skelAnime)) {
        if (phi_t0 != this->animationIdx) {
            this->animationIdx = phi_t0;
            this->unk_1E4 &= ~1;
            this->unk_1E4 &= ~2;
            if (phi_t0 == 1) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &this->unk_204, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            } else if (phi_t0 == 3) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->unk_204, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            } else {
                func_80A6BCEC(this);
            }
            Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A6B30C(this), 0.0f,
                             Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, -3.0f);
        } else {
            switch (D_80A6D510[this->animationIdx]) {
                case 0:
                    if (Rand_ZeroOne() < 0.25f) {
                        this->unk_218 = 1.0f;
                        phi_t0 = 4;
                    } else {
                        phi_t0 = D_80A6D4F4[(s32)(Rand_ZeroOne() * 2)];
                        this->actor.speedXZ = 0.0f;
                        this->unk_218 = 0.0f;
                    }
                    break;
                case 1:
                case 2:
                case 3:
                    break;
            }

            this->unk_1E4 &= ~1;
            this->unk_1E4 &= ~2;
            if (phi_t0 == 1) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &this->unk_204, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            } else if (phi_t0 == 3) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->unk_204, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            } else {
                func_80A6BCEC(this);
            }
            if (phi_t0 != this->animationIdx) {
                this->animationIdx = phi_t0;
                Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A6B30C(this), 0.0f,
                                 Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, -3.0f);
            } else {
                Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A6B30C(this), 0.0f,
                                 Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
            }
        }
    }
}

void func_80A6C4CC(EnHorseNormal* this) {
    this->action = HORSE_WAIT;
    this->animationIdx = 0;
    this->unk_21C = 0;
    this->unk_21E = 0;
    this->actor.speedXZ = 0.0f;
    this->unk_218 = 0.0f;
    Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A6B30C(this), 0.0f,
                     Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
}

void EnHorseNormal_Wait(EnHorseNormal* this, PlayState* play) {
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        f32 rand = Rand_ZeroOne();

        if (rand < 0.4f) {
            this->animationIdx = 0;
        } else if (rand < 0.8f) {
            this->animationIdx = 1;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &this->unk_204, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        } else {
            this->animationIdx = 3;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->unk_204, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }

        Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A6B30C(this), 0.0f,
                         Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
    }
}

void func_80A6C6B0(EnHorseNormal* this) {
    this->action = HORSE_WAIT_CLONE;
    this->animationIdx = 0;
    this->unk_21C = 0;
    this->unk_21E = 0;
    this->actor.flags |= ACTOR_FLAG_4 | ACTOR_FLAG_5;
    this->actor.speedXZ = 0.0f;
    this->unk_218 = 0.0f;
    Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A6B30C(this), 0.0f,
                     Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
}

void EnHorseNormal_WaitClone(EnHorseNormal* this, PlayState* play) {
    func_80A6BD7C(this);

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        f32 rand = Rand_ZeroOne();

        if (rand < 0.4f) {
            this->animationIdx = 0;
        } else if (rand < 0.8f) {
            this->animationIdx = 1;
            this->unk_1E4 |= 0x20;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &this->unk_204, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        } else {
            this->animationIdx = 3;
            this->unk_1E4 |= 0x20;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->unk_204, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }

        Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A6B30C(this), 0.0f,
                         Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, 0.0f);

        this->unk_1E4 &= ~1;
        this->unk_1E4 &= ~2;
        this->unk_1E4 &= ~8;
        this->unk_1E4 &= ~0x10;
    }
}

void func_80A6C8E0(EnHorseNormal* this, PlayState* play) {
    s32 pad;
    CollisionPoly* sp38;
    s32 pad2;
    Vec3f sp28;
    s32 sp24;

    sp28.x = (Math_SinS(this->actor.shape.rot.y) * 30.0f) + this->actor.world.pos.x;
    sp28.y = this->actor.world.pos.y + 60.0f;
    sp28.z = (Math_CosS(this->actor.shape.rot.y) * 30.0f) + this->actor.world.pos.z;
    this->unk_220 = BgCheck_EntityRaycastFloor3(&play->colCtx, &sp38, &sp24, &sp28);
    this->actor.shape.rot.x = Math_FAtan2F(this->actor.world.pos.y - this->unk_220, 30.0f) * (0x8000 / M_PI);
}

static EnHorseNormalActionFunc sActionFuncs[] = {
    EnHorseNormal_CycleAnimations, EnHorseNormal_Wander,     EnHorseNormal_Wait,
    EnHorseNormal_WaitClone,       EnHorseNormal_FollowPath,
};

void EnHorseNormal_Update(Actor* thisx, PlayState* play) {
    EnHorseNormal* this = (EnHorseNormal*)thisx;
    s32 pad;

    sActionFuncs[this->action](this, play);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 35.0f, 100.0f, 0x1D);
    if (play->sceneNum == SCENE_SPOT20 && this->actor.world.pos.z < -2400.0f) {
        this->actor.world.pos.z = -2400.0f;
    }
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 70.0f;
    this->unk_204 = this->actor.projectedPos;
    this->unk_204.y += 120.0f;
    Collider_UpdateCylinder(&this->actor, &this->bodyCollider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);
    if (this->actor.speedXZ == 0.0f) {
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    } else {
        this->actor.colChkInfo.mass = MASS_HEAVY;
    }
}

void EnHorseNormal_PostDraw(Actor* thisx, PlayState* play, Skin* skin) {
    Vec3f sp4C;
    Vec3f sp40;
    EnHorseNormal* this = (EnHorseNormal*)thisx;
    s32 i;

    for (i = 0; i < this->headCollider.count; i++) {
        sp4C.x = this->headCollider.elements[i].dim.modelSphere.center.x;
        sp4C.y = this->headCollider.elements[i].dim.modelSphere.center.y;
        sp4C.z = this->headCollider.elements[i].dim.modelSphere.center.z;
        Skin_GetLimbPos(skin, this->headCollider.elements[i].dim.limb, &sp4C, &sp40);
        this->headCollider.elements[i].dim.worldSphere.center.x = sp40.x;
        this->headCollider.elements[i].dim.worldSphere.center.y = sp40.y;
        this->headCollider.elements[i].dim.worldSphere.center.z = sp40.z;
        this->headCollider.elements[i].dim.worldSphere.radius =
            this->headCollider.elements[i].dim.modelSphere.radius * this->headCollider.elements[i].dim.scale;
    }

    //! @bug see relevant comment in `EnHorse_SkinCallback1`
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->headCollider.base);
}

void func_80A6CC88(PlayState* play, EnHorseNormal* this, Vec3f* arg2) {
    f32 curFrame = this->skin.skelAnime.curFrame;
    f32 wDest;

    SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, arg2, &this->unk_1E8, &wDest);
    this->unk_1F4 = this->unk_1E8;
    this->unk_1F4.y += 120.0f;

    if (this->animationIdx == 0 && curFrame > 28.0f && !(this->unk_1E4 & 8)) {
        this->unk_1E4 |= 8;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_SANDDUST, &this->unk_1E8, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (this->animationIdx == 3 && curFrame > 25.0f && !(this->unk_1E4 & 0x10)) {
        this->unk_1E4 |= 0x10;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND2, &this->unk_1E8, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (this->animationIdx == 3 && this->unk_1E4 & 0x20) {
        this->unk_1E4 &= ~0x20;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->unk_1F4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (this->animationIdx == 1 && this->unk_1E4 & 0x20) {
        this->unk_1E4 &= ~0x20;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &this->unk_1F4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
}

void EnHorseNormal_Draw(Actor* thisx, PlayState* play) {
    EnHorseNormal* this = (EnHorseNormal*)thisx;
    Mtx* mtx2;

    OPEN_DISPS(play->state.gfxCtx);

    if (play->sceneNum != SCENE_SPOT20 || play->sceneNum != SCENE_MALON_STABLE) {
        func_80A6C8E0(this, play);
    }
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    func_800A6330(&this->actor, play, &this->skin, EnHorseNormal_PostDraw, true);

    if (this->action == HORSE_WAIT_CLONE) {
        MtxF skinMtx;
        Mtx* mtx1;
        Vec3f clonePos = { 0.0f, 0.0f, 0.0f };
        s16 cloneRotY;
        f32 distFromGround = this->actor.world.pos.y - this->actor.floorHeight;
        f32 temp_f0_4;

        if (play->sceneNum == SCENE_MALON_STABLE) {
            if (this->actor.world.pos.x == 355.0f && this->actor.world.pos.y == 0.0f &&
                this->actor.world.pos.z == -245.0f) {
                clonePos.x = 235.0f;
                clonePos.y = 0.0f;
                clonePos.z = 100.0f;
                cloneRotY = 0x7FFF;
            } else if (this->actor.world.pos.x == 238.0f && this->actor.world.pos.y == 0.0f &&
                       this->actor.world.pos.z == -245.0f) {
                clonePos.x = 478.0f;
                clonePos.y = 0.0f;
                clonePos.z = 100.0f;
                cloneRotY = 0x7FFF;
            }
        } else if (play->sceneNum == SCENE_SPOT20) {
            if (this->actor.world.pos.x == -730.0f && this->actor.world.pos.y == 0.0f &&
                this->actor.world.pos.z == -1100.0f) {
                clonePos.x = 780.0f;
                clonePos.y = 0.0f;
                clonePos.z = -80.0f;
                cloneRotY = 0;
            } else if (this->actor.world.pos.x == 880.0f && this->actor.world.pos.y == 0.0f &&
                       this->actor.world.pos.z == -1170.0f) {
                clonePos.x = -1000.0f;
                clonePos.y = 0.0f;
                clonePos.z = -70.0f;
                cloneRotY = 0;
            }
        }
        func_80A6CC88(play, this, &clonePos);
        SkinMatrix_SetTranslateRotateYXZScale(&skinMtx, this->actor.scale.x, this->actor.scale.y, this->actor.scale.z,
                                              this->actor.shape.rot.x, cloneRotY, this->actor.shape.rot.z, clonePos.x,
                                              (this->actor.shape.yOffset * this->actor.scale.y) + clonePos.y,
                                              clonePos.z);
        mtx1 = SkinMatrix_MtxFToNewMtx(play->state.gfxCtx, &skinMtx);
        if (mtx1 == NULL) {
            return;
        }
        gSPMatrix(POLY_OPA_DISP++, &gMtxClear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPMatrix(POLY_OPA_DISP++, mtx1, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        func_800A63CC(&this->actor, play, &this->skin, NULL, NULL, true, 0,
                      SKIN_DRAW_FLAG_CUSTOM_TRANSFORMS | SKIN_DRAW_FLAG_CUSTOM_MATRIX);
        this->cloneCollider.dim.pos.x = clonePos.x;
        this->cloneCollider.dim.pos.y = clonePos.y;
        this->cloneCollider.dim.pos.z = clonePos.z;
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->cloneCollider.base);
        Gfx_SetupDL_44Xlu(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 255);
        Matrix_Translate(clonePos.x, clonePos.y, clonePos.z, MTXMODE_NEW);
        temp_f0_4 = (1.0f - (distFromGround * 0.01f)) * this->actor.shape.shadowScale;
        Matrix_Scale(this->actor.scale.x * temp_f0_4, 1.0f, this->actor.scale.z * temp_f0_4, MTXMODE_APPLY);
        Matrix_RotateY(cloneRotY * (2.0f * M_PI / 0x10000), MTXMODE_APPLY);
        mtx2 = MATRIX_NEWMTX(play->state.gfxCtx);
        if (mtx2 != NULL) {
            gSPMatrix(POLY_XLU_DISP++, mtx2, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gHorseShadowDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
