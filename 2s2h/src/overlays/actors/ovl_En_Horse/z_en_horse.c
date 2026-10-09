/*
 * File: z_en_horse.c
 * Overlay: ovl_En_Horse
 * Description: Epona
 */

#include "z_en_horse.h"
#include "z64horse.h"
#include "z64rumble.h"
#include "z64voice.h"
#include "overlays/actors/ovl_En_In/z_en_in.h"
#include "overlays/actors/ovl_Obj_Um/z_obj_um.h"
#include "overlays/actors/ovl_En_Horse_Game_Check/z_en_horse_game_check.h"
#include "objects/object_horse_link_child/object_horse_link_child.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnHorse_Init(Actor* thisx, PlayState* play2);
void EnHorse_Destroy(Actor* thisx, PlayState* play);
void EnHorse_Update(Actor* thisx, PlayState* play2);
void EnHorse_Draw(Actor* thisx, PlayState* play);

void func_8087D540(Actor* thisx, PlayState* play);
void EnHorse_StartMountedIdleResetAnim(EnHorse* this);
void EnHorse_StartMountedIdle(EnHorse* this);
void EnHorse_MountedIdle(EnHorse* this, PlayState* play);
void EnHorse_MountedIdleAnim(EnHorse* this);
void EnHorse_MountedIdleWhinny(EnHorse* this);
void EnHorse_StartTurning(EnHorse* this);
void EnHorse_StartWalkingFromIdle(EnHorse* this);
void EnHorse_StartWalkingInterruptable(EnHorse* this);
void EnHorse_StartWalking(EnHorse* this);
void EnHorse_StartTrotting(EnHorse* this);
void EnHorse_StartGallopingInterruptable(EnHorse* this);
void EnHorse_StartGalloping(EnHorse* this);
void EnHorse_StartBraking(EnHorse* this, PlayState* play);
void EnHorse_StartReversingInterruptable(EnHorse* this);
void EnHorse_StartReversing(EnHorse* this);
void EnHorse_StartLowJump(EnHorse* this, PlayState* play);
void EnHorse_StartHighJump(EnHorse* this, PlayState* play);
void EnHorse_InitInactive(EnHorse* this);
void EnHorse_ChangeIdleAnimation(EnHorse* this, s32 animIndex, f32 morphFrames);
void EnHorse_ResetIdleAnimation(EnHorse* this);
void EnHorse_StartIdleRidable(EnHorse* this);
void EnHorse_StartMovingAnimation(EnHorse* this, s32 animIndex, f32 morphFrames, f32 startFrames);
void EnHorse_SetFollowAnimation(EnHorse* this, PlayState* play);
void EnHorse_InitIngoHorse(EnHorse* this);
void EnHorse_UpdateIngoHorseAnim(EnHorse* this);
void func_80881290(EnHorse* this, PlayState* play);
void func_8088159C(EnHorse* this, PlayState* play);
void func_80881634(EnHorse* this);
void func_8088168C(EnHorse* this);
void EnHorse_CsMoveInit(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void EnHorse_CsMoveToPoint(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void EnHorse_CsPlayHighJumpAnim(EnHorse* this, PlayState* play);
void EnHorse_CsJumpInit(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void EnHorse_CsJump(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void EnHorse_CsRearingInit(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void EnHorse_CsRearing(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void EnHorse_WarpMoveInit(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void EnHorse_CsWarpMoveToPoint(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void EnHorse_CsWarpRearingInit(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void EnHorse_CsWarpRearing(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void EnHorse_InitCutscene(EnHorse* this, PlayState* play);
void EnHorse_InitHorsebackArchery(EnHorse* this);
void EnHorse_UpdateHbaAnim(EnHorse* this);
void func_80883BEC(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_80883CB0(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_80883D64(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_80883DE0(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_80883E10(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_80883EA0(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_80883F18(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_80883F98(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_80884010(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_808840C4(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_80884194(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_8088424C(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_80884314(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_808843B4(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_80884444(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_808844E0(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_80884564(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_80884604(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_808846B4(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_808846DC(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void func_808846F0(EnHorse* this, PlayState* play);
void func_80884994(EnHorse* this);
void func_80884D04(EnHorse* this, PlayState* play);
void EnHorse_StickDirection(Vec2f* curStick, f32* stickMag, s16* angle);
s32 EnHorse_GetMountSide(EnHorse* this, PlayState* play);

typedef struct {
    /* 0x0 */ s32 cueId;
    /* 0x4 */ s32 csFuncIdx;
} CsActionEntry; // size = 0x8

typedef struct {
    /* 0x0 */ s16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ s16 z;
    /* 0x6 */ s16 speed;
    /* 0x8 */ s16 angle;
} RaceWaypoint; // size = 0xA

typedef struct {
    /* 0x0 */ s32 numWaypoints;
    /* 0x4 */ RaceWaypoint* waypoints;
} RaceInfo; // size = 0x8

static AnimationHeader* sEponaAnimations[ENHORSE_ANIM_MAX] = {
    &gEponaIdleAnim,                      // ENHORSE_ANIM_IDLE
    &gEponaWhinnyAnim,                    // ENHORSE_ANIM_WHINNY
    &object_horse_link_child_Anim_005F64, // ENHORSE_ANIM_STOPPING
    &object_horse_link_child_Anim_004DE8, // ENHORSE_ANIM_REARING
    &gEponaWalkAnim,                      // ENHORSE_ANIM_WALK
    &gEponaTrotAnim,                      // ENHORSE_ANIM_TROT
    &gEponaGallopAnim,                    // ENHORSE_ANIM_GALLOP
    &object_horse_link_child_Anim_0035B0, // ENHORSE_ANIM_LOW_JUMP
    &object_horse_link_child_Anim_003D38, // ENHORSE_ANIM_HIGH_JUMP
};

static AnimationHeader* sHniAnimations[ENHORSE_ANIM_MAX] = {
    &gHorseIdleAnim,      // ENHORSE_ANIM_IDLE
    &gHorseShakeHeadAnim, // ENHORSE_ANIM_WHINNY
    &gHorseStopAnim,      // ENHORSE_ANIM_STOPPING
    &gHorseWhinnyAnim,    // ENHORSE_ANIM_REARING @TODO: Does not line up
    &gHorseWalkAnim,      // ENHORSE_ANIM_WALK
    &gHorseTrotAnim,      // ENHORSE_ANIM_TROT
    &gHorseGallopAnim,    // ENHORSE_ANIM_GALLOP
    &gHorseJumpLowAnim,   // ENHORSE_ANIM_LOW_JUMP
    &gHorseJumpHighAnim,  // ENHORSE_ANIM_HIGH_JUMP
};

static AnimationHeader** sAnimationHeaders[HORSE_TYPE_MAX] = {
    NULL,             // HORSE_TYPE_EPONA
    NULL,             // HORSE_TYPE_HNI
    sEponaAnimations, // HORSE_TYPE_2
    sHniAnimations,   // HORSE_TYPE_BANDIT
    sHniAnimations,   // HORSE_TYPE_DONKEY
};

static f32 sPlaybackSpeeds[ENHORSE_ANIM_MAX] = {
    2.0f / 3.0f, // ENHORSE_ANIM_IDLE
    2.0f / 3.0f, // ENHORSE_ANIM_WHINNY
    3.0f / 3.0f, // ENHORSE_ANIM_STOPPING
    3.0f / 3.0f, // ENHORSE_ANIM_REARING
    3.0f / 3.0f, // ENHORSE_ANIM_WALK
    3.0f / 3.0f, // ENHORSE_ANIM_TROT
    3.0f / 3.0f, // ENHORSE_ANIM_GALLOP
    2.0f / 3.0f, // ENHORSE_ANIM_LOW_JUMP
    2.0f / 3.0f, // ENHORSE_ANIM_HIGH_JUMP
};

static SkeletonHeader* sSkeletonHeaders[HORSE_TYPE_MAX] = {
    NULL,        // HORSE_TYPE_EPONA
    NULL,        // HORSE_TYPE_HNI
    &gEponaSkel, // HORSE_TYPE_2
    NULL,        // HORSE_TYPE_BANDIT
    NULL,        // HORSE_TYPE_DONKEY
};

ActorProfile En_Horse_Profile = {
    /**/ ACTOR_EN_HORSE,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnHorse),
    /**/ EnHorse_Init,
    /**/ EnHorse_Destroy,
    /**/ EnHorse_Update,
    /**/ EnHorse_Draw,
};

static ColliderCylinderInit sCylinderInit1 = {
    {
        COL_MATERIAL_NONE,
        AT_NONE | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000004, 0x00, 0x02 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit2 = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementsInit[] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00013820, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_NO_AT_INFO | ACELEM_NO_DAMAGE | ACELEM_NO_SWORD_SFX | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { 13, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 10, 35, 100, MASS_HEAVY };

static s32 sAnimSoundFrames[] = {
    0,
    16,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeScale, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 300, ICHAIN_STOP),
};

static u8 sResetNoInput[] = {
    false, false, false, false, false, false, true,  true,  true,  true,  true,  true,
    true,  true,  true,  false, false, false, false, false, false, false, false, false,
};

static s32 sIdleAnimIndices[] = {
    ENHORSE_ANIM_WHINNY,  ENHORSE_ANIM_REARING, ENHORSE_ANIM_IDLE,
    ENHORSE_ANIM_REARING, ENHORSE_ANIM_WHINNY,  ENHORSE_ANIM_IDLE,
};

static s16 sIngoAnimIndices[] = {
    ENIN_ANIM_7, ENIN_ANIM_6, ENIN_ANIM_2, ENIN_ANIM_2, ENIN_ANIM_1,
    ENIN_ANIM_1, ENIN_ANIM_0, ENIN_ANIM_0, ENIN_ANIM_0, ENIN_ANIM_0,
};

static EnHorseCsFunc sCutsceneInitFuncs[] = {
    NULL,
    EnHorse_CsMoveInit,
    EnHorse_CsJumpInit,
    EnHorse_CsRearingInit,
    EnHorse_WarpMoveInit,
    EnHorse_CsWarpRearingInit,
};

static EnHorseCsFunc sCutsceneActionFuncs[] = {
    NULL, EnHorse_CsMoveToPoint, EnHorse_CsJump, EnHorse_CsRearing, EnHorse_CsWarpMoveToPoint, EnHorse_CsWarpRearing,
};

static CsActionEntry sCsActionTable[] = {
    { 36, 1 }, { 37, 2 }, { 38, 3 }, { 64, 4 }, { 65, 5 },
};

static RaceWaypoint sHbaWaypoints[] = {
    { 3600, 1413, -5055, 11, 0x8001 }, { 3360, 1413, -5220, 5, 0xC000 }, { 3100, 1413, -4900, 5, 0x0000 },
    { 3600, 1413, -4100, 11, 0x0000 }, { 3600, 1413, 360, 11, 0x0000 },
};

static RaceInfo sHbaInfo = { 5, sHbaWaypoints };

static EnHorseCsFunc D_808890F0[] = {
    NULL,          func_80883BEC, func_80883D64, func_80883E10, func_80883F18, func_80884010,
    func_80884194, func_80884314, func_80884444, func_80884564, func_808846B4,
};

static EnHorseCsFunc D_8088911C[] = {
    NULL,          func_80883CB0, func_80883DE0, func_80883EA0, func_80883F98, func_808840C4,
    func_8088424C, func_808843B4, func_808844E0, func_80884604, func_808846DC,
};

void EnHorse_RaceWaypointPos(RaceWaypoint* waypoints, s32 idx, Vec3f* pos) {
    pos->x = waypoints[idx].x;
    pos->y = waypoints[idx].y;
    pos->z = waypoints[idx].z;
}

void EnHorse_RotateToPoint(EnHorse* this, PlayState* play, Vec3f* pos, s16 turnYaw) {
    Horse_RotateToPoint(&this->actor, pos, turnYaw);
}

void func_8087B7C0(EnHorse* this, PlayState* play, Path* path) {
    s32 spA4;
    Vec3s* pathPoints;
    f32 phi_f12;
    f32 phi_f14;
    Vec3f sp8C;
    Vec3f sp80;
    f32 sp7C;
    f32 sp78;
    f32 sp74;
    f32 sp70;
    s32 i;
    s32 sp68;
    f32 sp64;
    s32 sp60;
    f32 temp_f0;
    Vec3f sp50;

    spA4 = path->count;
    pathPoints = Lib_SegmentedToVirtual(path->points);
    Math_Vec3s_ToVec3f(&sp8C, &pathPoints[this->curRaceWaypoint]);

    if (this->curRaceWaypoint == 0) {
        phi_f12 = pathPoints[1].x - pathPoints[0].x;
        phi_f14 = pathPoints[1].z - pathPoints[0].z;
    } else if ((this->curRaceWaypoint + 1) == path->count) {
        phi_f12 = pathPoints[path->count - 1].x - pathPoints[path->count - 2].x;
        phi_f14 = pathPoints[path->count - 1].z - pathPoints[path->count - 2].z;
    } else {
        phi_f12 = pathPoints[this->curRaceWaypoint + 1].x - pathPoints[this->curRaceWaypoint - 1].x;
        phi_f14 = pathPoints[this->curRaceWaypoint + 1].z - pathPoints[this->curRaceWaypoint - 1].z;
    }

    Math3D_RotateXZPlane(&sp8C, Math_Atan2S(phi_f12, phi_f14), &sp7C, &sp78, &sp74);

    if (((this->actor.world.pos.x * sp7C) + (sp78 * this->actor.world.pos.z) + sp74) > 0.0f) {
        this->curRaceWaypoint++;
        if (this->curRaceWaypoint >= spA4) {
            this->curRaceWaypoint = spA4 - 1;
        }
        Math_Vec3s_ToVec3f(&sp8C, &pathPoints[this->curRaceWaypoint]);
    }

    if (this->curRaceWaypoint == 0) {
        Math_Vec3s_ToVec3f(&sp80, &pathPoints[1]);
    } else {
        Math_Vec3s_ToVec3f(&sp80, &pathPoints[this->curRaceWaypoint - 1]);
    }

    Math3D_PointDistSqToLine2D(this->actor.world.pos.x, this->actor.world.pos.z, sp80.x, sp80.z, sp8C.x, sp8C.z, &sp70);

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || (this->unk_1EC & 4)) {
        EnHorse_RotateToPoint(this, play, &sp8C, 0xC80);
        if (this->unk_1EC & 4) {
            this->unk_1EC &= ~4;
        }
    } else {
        EnHorse_RotateToPoint(this, play, &sp8C, 0x320);
        if (sp70 < SQ(100.0f)) {
            if ((this->actor.xzDistToPlayer < 100.0f) ||
                (this->colliderJntSph.elements[0].base.ocElemFlags & OCELEM_HIT)) {
                s32 pad;

                if (Math_SinS(this->actor.yawTowardsPlayer - this->actor.world.rot.y) > 0.0f) {
                    this->actor.world.rot.y -= 0x1E0;
                } else {
                    this->actor.world.rot.y += 0x1E0;
                }
            } else if (this->actor.xzDistToPlayer < 300.0f) {
                if (Math_SinS(this->actor.yawTowardsPlayer - this->actor.world.rot.y) > 0.0f) {
                    this->actor.world.rot.y += 0x1E0;
                } else {
                    this->actor.world.rot.y -= 0x1E0;
                }
            }
            this->actor.shape.rot.y = this->actor.world.rot.y;
        }
    }

    sp68 = this->curRaceWaypoint - 3;
    if (sp68 < 0) {
        sp68 = 0;
    }

    sp64 = 1.0e+38;
    sp60 = sp68 + 5;
    if (sp60 > path->count) {
        sp60 = path->count;
    }

    for (i = sp68; i < sp60; i++) {
        Math_Vec3s_ToVec3f(&sp50, &pathPoints[i]);
        temp_f0 = Math3D_Vec3f_DistXYZ(&this->actor.world.pos, &sp50);
        if (temp_f0 < sp64) {
            sp64 = temp_f0;
            sp68 = i;
        }
    }

    this->unk_398 = pathPoints[this->curRaceWaypoint].y * 0.01f;
    if ((this->unk_1EC & 0x100) && !(this->stateFlags & ENHORSE_JUMPING) &&
        ((this->colliderCylinder1.base.acFlags & AC_HIT) || (this->colliderCylinder2.base.acFlags & AC_HIT) ||
         (this->unk_58C > 0))) {
        if (this->unk_58C == 0) {
            this->unk_590 = 0x1F4;
            this->unk_58C = 0x2B;
        }
        this->unk_58C--;
        if (this->actor.speed > 5.0f) {
            this->actor.speed -= 0.5f;
        }
    } else if (this->unk_590 > 0) {
        if (this->unk_590 == 0x1F4) {
            if (this->rider != NULL) {
                Actor_PlaySfx(&this->rider->actor, NA_SE_VO_IN_CRY_0);
            }
        }
        this->unk_590--;
        if (this->actor.speed < this->unk_398) {
            this->actor.speed += 1.0f;
        } else {
            this->actor.speed -= 0.5f;
        }
    } else if (this->actor.params == ENHORSE_5) {
        s16 sp4A;

        if (sp68 >= this->curRaceWaypoint) {
            if (this->actor.speed < this->unk_398) {
                this->actor.speed += 0.5f;
            } else {
                this->actor.speed -= 0.5f;
            }
            this->unk_394 |= 1;
            return;
        }

        sp4A = Actor_WorldYawTowardActor(&this->actor, &GET_PLAYER(play)->actor) - this->actor.world.rot.y;

        if ((fabsf(Math_SinS(sp4A)) < 0.9f) && (Math_CosS(sp4A) > 0.0f)) {
            if (this->actor.speed < this->unk_398) {
                this->actor.speed += 0.5f;
            } else {
                this->actor.speed -= 0.25f;
            }
            this->unk_394 |= 1;
        } else {
            if (this->actor.speed < 13.0f) {
                this->actor.speed += 0.4f;
            } else {
                this->actor.speed -= 0.2f;
            }
            this->unk_394 &= ~1;
        }
    } else if (sp68 >= this->curRaceWaypoint) {
        if (this->actor.speed < this->unk_398) {
            this->actor.speed += 0.5f;
        } else {
            this->actor.speed -= 0.5f;
        }
        this->unk_394 |= 1;
    } else if ((sp68 + 1) == this->curRaceWaypoint) {
        s16 sp48 = Actor_WorldYawTowardActor(&this->actor, &GET_PLAYER(play)->actor) - this->actor.world.rot.y;

        if ((fabsf(Math_SinS(sp48)) < 0.9f) && (Math_CosS(sp48) > 0.0f)) {
            if (this->actor.speed < this->unk_398) {
                this->actor.speed += 0.5f;
            } else {
                this->actor.speed -= 0.25f;
            }
            this->unk_394 |= 1;
        } else {
            if (this->actor.speed < 12.0f) {
                this->actor.speed += 0.4f;
            } else {
                this->actor.speed -= 0.2f;
            }
            this->unk_394 &= ~1;
        }
    } else {
        if (this->actor.speed < 13.0f) {
            this->actor.speed += 0.4f;
        } else {
            this->actor.speed -= 0.2f;
        }
        this->unk_394 &= ~1;
    }
}

void EnHorse_PlayWalkingSound(EnHorse* this) {
    if (sAnimSoundFrames[this->soundTimer] < this->curFrame) {
        if ((this->soundTimer == 0) && (sAnimSoundFrames[1] < this->curFrame)) {
            return;
        }

        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_WALK);
        } else {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_WALK);
        }

        this->soundTimer++;
        if (this->soundTimer > 1) {
            this->soundTimer = 0;
        }
    }
}

void func_8087C178(EnHorse* this) {
    if (this->type == HORSE_TYPE_2) {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_RUN);
    } else {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_RUN);
    }
}

void func_8087C1C0(EnHorse* this) {
    if (this->type == HORSE_TYPE_2) {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_RUN);
    } else {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_RUN);
    }
}

f32 EnHorse_SlopeSpeedMultiplier(EnHorse* this, PlayState* play) {
    f32 multiplier = 1.0f;

    if ((Math_CosS(this->actor.shape.rot.x) < 0.939262f) && (Math_SinS(this->actor.shape.rot.x) < 0.0f)) {
        multiplier = 0.7f;
    }
    return multiplier;
}

void func_8087C288(PlayState* play, Vec3f* arg1, Vec3f* arg2, f32* arg3) {
    SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, arg1, arg2, arg3);
}

bool func_8087C2B8(PlayState* play, EnHorse* this, Vec3f* arg2, f32 arg3) {
    f32 phi_f14;

    if ((arg2->z > 0.0f) && (arg2->z < (this->actor.cullingVolumeDistance + this->actor.cullingVolumeScale))) {
        if (arg3 < 1.0f) {
            phi_f14 = 1.0f;
        } else {
            phi_f14 = 1.0f / arg3;
        }

        if (((fabsf(arg2->x) * phi_f14) < 1.0f) &&
            (((arg2->y + this->actor.cullingVolumeDownward) * phi_f14) > -1.0f) &&
            (((arg2->y - this->actor.cullingVolumeScale) * phi_f14) < 1.0f)) {
            return true;
        }
    }
    return false;
}

bool func_8087C38C(PlayState* play, EnHorse* this, Vec3f* arg2) {
    Vec3f sp24;
    f32 sp20;
    f32 eyeDist;

    func_8087C288(play, arg2, &sp24, &sp20);

    if (fabsf(sp20) < 0.008f) {
        return false;
    }

    eyeDist = Math3D_Vec3f_DistXYZ(arg2, &play->view.eye);

    return func_8087C2B8(play, this, &sp24, sp20) || (eyeDist < 100.0f);
}

void EnHorse_IdleAnimSounds(EnHorse* this, PlayState* play) {
    if ((this->animIndex == ENHORSE_ANIM_IDLE) &&
        (((this->curFrame > 35.0f) && (this->type == HORSE_TYPE_EPONA)) ||
         ((this->curFrame > 28.0f) && (this->type == HORSE_TYPE_HNI)) ||
         ((this->curFrame > 25.0f) && (this->type == HORSE_TYPE_2))) &&
        !(this->stateFlags & ENHORSE_SANDDUST_SOUND)) {
        this->stateFlags |= ENHORSE_SANDDUST_SOUND;
        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_SANDDUST);
        } else {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_SANDDUST);
        }
    } else if ((this->animIndex == ENHORSE_ANIM_REARING) && (this->curFrame > 25.0f) &&
               !(this->stateFlags & ENHORSE_LAND2_SOUND)) {
        this->stateFlags |= ENHORSE_LAND2_SOUND;
        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_LAND2);
        } else {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_LAND2);
        }
    }
}

s32 EnHorse_Spawn(EnHorse* this, PlayState* play) {
    s32 i;
    f32 dist;
    Path* path;
    s32 spawn = false;
    f32 minDist = 1.0e+38;
    Player* player = GET_PLAYER(play);
    Vec3f spawnPos;
    s32 pathIndex = Horse_GetJumpingFencePathIndex(play);
    s32 pathCount;
    Vec3s* pathPoints;

    if (pathIndex == PATH_INDEX_NONE) {
        return false;
    }

    path = &play->setupPathList[pathIndex];
    pathCount = path->count;
    pathPoints = Lib_SegmentedToVirtual(path->points);

    for (i = 0; i < pathCount; i++) {
        spawnPos.x = pathPoints[i].x;
        spawnPos.y = pathPoints[i].y;
        spawnPos.z = pathPoints[i].z;
        dist = Math3D_Vec3f_DistXYZ(&player->actor.world.pos, &spawnPos);

        if ((minDist < dist) || func_8087C38C(play, this, &spawnPos)) {
            continue;
        }

        minDist = dist;
        this->actor.world.pos.x = spawnPos.x;
        this->actor.world.pos.y = spawnPos.y;
        this->actor.world.pos.z = spawnPos.z;
        this->actor.prevPos = this->actor.world.pos;
        this->actor.world.rot.y = 0;
        this->actor.shape.rot.y = Actor_WorldYawTowardActor(&this->actor, &GET_PLAYER(play)->actor);
        spawn = true;
        SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &this->actor.world.pos, &this->actor.projectedPos,
                                     &this->actor.projectedW);
    }

    if (spawn == true) {
        return true;
    }

    for (i = 0; i < pathCount; i++) {
        spawnPos.x = pathPoints[i].x;
        spawnPos.y = pathPoints[i].y;
        spawnPos.z = pathPoints[i].z;
        dist = Math3D_Vec3f_DistXYZ(&player->actor.world.pos, &spawnPos);

        if (minDist < dist) {
            continue;
        }

        minDist = dist;
        this->actor.world.pos.x = spawnPos.x;
        this->actor.world.pos.y = spawnPos.y;
        this->actor.world.pos.z = spawnPos.z;
        this->actor.prevPos = this->actor.world.pos;
        this->actor.world.rot.y = 0;
        this->actor.shape.rot.y = Actor_WorldYawTowardActor(&this->actor, &GET_PLAYER(play)->actor);
        spawn = true;
        SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &this->actor.world.pos, &this->actor.projectedPos,
                                     &this->actor.projectedW);
    }

    return spawn;
}

void EnHorse_ResetCutscene(EnHorse* this, PlayState* play) {
    this->cutsceneAction = -1;
    this->cutsceneFlags = 0;
}

void EnHorse_ResetRace(EnHorse* this, PlayState* play) {
    this->inRace = false;
}

s32 EnHorse_PlayerCanMove(EnHorse* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags1 & PLAYER_STATE1_1) || (func_800B7128(GET_PLAYER(play)) == true) ||
        (player->stateFlags1 & PLAYER_STATE1_100000) ||
        (((this->stateFlags & ENHORSE_FLAG_19) || (this->stateFlags & ENHORSE_FLAG_29)) && !this->inRace) ||
        (this->action == ENHORSE_ACTION_HBA) || (player->actor.flags & ACTOR_FLAG_TALK) ||
        (play->csCtx.state != CS_STATE_IDLE) || (CutsceneManager_GetCurrentCsId() != CS_ID_NONE) ||
        (player->stateFlags1 & PLAYER_STATE1_20) || (player->csAction != PLAYER_CSACTION_NONE)) {
        return false;
    }
    return true;
}

void EnHorse_ResetHorsebackArchery(EnHorse* this, PlayState* play) {
    this->unk_39C = 0;
    this->hbaStarted = 0;
    this->hbaFlags = 0;
}

void EnHorse_ClearDustFlags(u16* dustFlags) {
    *dustFlags = 0;
}

void func_8087C9F8(EnHorse* this) {
}

void func_8087CA04(EnHorse* this, PlayState* play) {
}

void EnHorse_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnHorse* this = (EnHorse*)thisx;
    Skin* skin = &this->skin;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    EnHorse_ClearDustFlags(&this->dustFlags);
    gHorsePlayedEponasSong = false;
    Skin_Setup(&this->skin);
    this->riderPos = thisx->world.pos;
    this->unk_52C = 0;
    this->noInputTimer = 0;
    this->riderPos.y += 70.0f;
    this->noInputTimerMax = 0;
    this->unk_1EC = 0;
    this->unk_58C = 0;
    this->unk_590 = 0;
    this->unk_3E8 = 0.0f;
    this->unk_528 = 100.0f;

    if (ENHORSE_IS_DONKEY_TYPE(&this->actor)) {
        this->type = HORSE_TYPE_DONKEY;
        this->unk_528 = 80.0f;
        this->boostSpeed = 12;
        if ((this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_HA)) <= OBJECT_SLOT_NONE) {
            Actor_Kill(&this->actor);
            return;
        }
        this->unk_1EC |= 1;
        thisx->update = func_8087D540;
    } else if (ENHORSE_IS_4000_TYPE(&this->actor)) {
        this->type = HORSE_TYPE_2;
        this->unk_528 = 64.8f;
        this->boostSpeed = 15;
        if ((this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_HORSE_LINK_CHILD)) <= OBJECT_SLOT_NONE) {
            thisx->objectSlot = Object_SpawnPersistent(&play->objectCtx, OBJECT_HORSE_LINK_CHILD);
            Actor_SetObjectDependency(play, &this->actor);
            Skin_Init(&play->state, &this->skin, sSkeletonHeaders[this->type], sAnimationHeaders[this->type][0]);
            Animation_PlayOnce(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex]);
            this->unk_1EC |= 0x200;
        } else {
            thisx->update = func_8087D540;
        }
    } else if (ENHORSE_IS_BANDIT_TYPE(&this->actor)) {
        this->type = HORSE_TYPE_BANDIT;
        this->boostSpeed = 12;
        if ((this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_HA)) <= OBJECT_SLOT_NONE) {
            Actor_Kill(&this->actor);
            return;
        }
        this->unk_1EC |= 1;
        thisx->update = func_8087D540;
    } else {
        this->type = HORSE_TYPE_EPONA;
        this->boostSpeed = 15;
        Actor_Kill(&this->actor);
    }

    this->actor.params &= ~(ENHORSE_PARAM_DONKEY | ENHORSE_PARAM_4000 | ENHORSE_PARAM_BANDIT);
    if (this->actor.params == 0x1FFF) {
        this->actor.params = ENHORSE_1;
    }

    if (thisx->params == ENHORSE_3) {
        this->stateFlags = ENHORSE_FLAG_19 | ENHORSE_CANT_JUMP | ENHORSE_UNRIDEABLE;
    } else if (thisx->params == ENHORSE_8) {
        this->stateFlags = ENHORSE_FLAG_19 | ENHORSE_CANT_JUMP;
        if (CHECK_QUEST_ITEM(QUEST_SONG_EPONA)) {
            this->stateFlags &= ~ENHORSE_CANT_JUMP;
            this->stateFlags |= ENHORSE_FLAG_26;
        }
    } else if (thisx->params == ENHORSE_13) {
        this->stateFlags = ENHORSE_FLAG_29;
        this->unk_1EC |= 0x10;
    } else if (thisx->params == ENHORSE_4) {
        this->stateFlags = ENHORSE_FLAG_29 | ENHORSE_CANT_JUMP;
        thisx->flags |= ACTOR_FLAG_MINIMAP_ICON_ENABLED;
    } else if (thisx->params == ENHORSE_5) {
        this->stateFlags = ENHORSE_FLAG_29 | ENHORSE_CANT_JUMP;
        thisx->flags |= ACTOR_FLAG_MINIMAP_ICON_ENABLED;
    } else if (thisx->params == ENHORSE_15) {
        this->stateFlags = ENHORSE_UNRIDEABLE | ENHORSE_FLAG_7;
    } else if (thisx->params == ENHORSE_17) {
        this->stateFlags = 0;
        this->unk_1EC |= 8;
    } else if (thisx->params == ENHORSE_18) {
        this->stateFlags = ENHORSE_FLAG_29 | ENHORSE_CANT_JUMP;
        thisx->flags |= ACTOR_FLAG_MINIMAP_ICON_ENABLED;
    } else if (thisx->params == ENHORSE_1) {
        this->stateFlags = ENHORSE_FLAG_7;
    } else if ((thisx->params == ENHORSE_19) || (thisx->params == ENHORSE_20)) {
        this->stateFlags = ENHORSE_CANT_JUMP | ENHORSE_UNRIDEABLE;
    } else {
        this->stateFlags = 0;
    }

    if (((play->sceneId == SCENE_KOEPONARACE) &&
         (GET_WEEKEVENTREG_HORSE_RACE_STATE == WEEKEVENTREG_HORSE_RACE_STATE_START)) ||
        ((gSaveContext.save.entrance == ENTRANCE(ROMANI_RANCH, 0)) && (Cutscene_GetSceneLayer(play) != 0))) {
        this->stateFlags |= ENHORSE_FLAG_25;
    }

    thisx->gravity = -3.5f;
    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawHorse, 20.0f);
    this->action = ENHORSE_ACTION_IDLE;
    thisx->speed = 0.0f;

    if (this->type == HORSE_TYPE_2) {
        sJntSphInit.elements[0].dim.limb = 13;
    } else if ((this->type == HORSE_TYPE_BANDIT) || (this->type == HORSE_TYPE_DONKEY)) {
        sJntSphInit.elements[0].dim.limb = 10;
    }

    Collider_InitCylinder(play, &this->colliderCylinder1);
    Collider_SetCylinder(play, &this->colliderCylinder1, &this->actor, &sCylinderInit1);
    Collider_InitCylinder(play, &this->colliderCylinder2);
    Collider_SetCylinder(play, &this->colliderCylinder2, &this->actor, &sCylinderInit2);
    Collider_InitJntSph(play, &this->colliderJntSph);
    Collider_SetJntSph(play, &this->colliderJntSph, &this->actor, &sJntSphInit, this->colliderJntSphElements);

    if (this->type == HORSE_TYPE_2) {
        this->colliderCylinder1.dim.radius *= 0.8f;
        this->colliderCylinder2.dim.radius *= 0.8f;
        this->colliderJntSph.elements[0].dim.modelSphere.radius *= 0.6f;
    } else if (this->type == HORSE_TYPE_DONKEY) {
        this->colliderCylinder1.dim.radius = 50;
    }

    CollisionCheck_SetInfo(&thisx->colChkInfo, NULL, &sColChkInfoInit);

    if (this->type == HORSE_TYPE_2) {
        Actor_SetScale(&this->actor, 0.00648f);
    } else if (this->type == HORSE_TYPE_DONKEY) {
        Actor_SetScale(&this->actor, 0.008f);
    } else {
        Actor_SetScale(&this->actor, 0.01f);
    }

    thisx->focus.pos = thisx->world.pos;
    this->playerControlled = false;
    thisx->focus.pos.y += 70.0f;

    if (!(this->unk_1EC & 1) && !(this->unk_1EC & 0x200) && (thisx->update == EnHorse_Update)) {
        Skin_Init(&play->state, &this->skin, sSkeletonHeaders[this->type], sAnimationHeaders[this->type][0]);
    }

    this->animIndex = ENHORSE_ANIM_IDLE;
    this->numBoosts = 6;
    this->boostRegenTime = 0;
    this->postDrawFunc = NULL;
    this->blinkTimer = 0;

    EnHorse_ResetCutscene(this, play);
    EnHorse_ResetRace(this, play);
    EnHorse_ResetHorsebackArchery(this, play);

    if (thisx->params == ENHORSE_2) {
        this->unk_53C = 0;
        EnHorse_InitInactive(this);
    } else if (thisx->params == ENHORSE_3) {
        EnHorse_InitIngoHorse(this);
        this->rider = (EnIn*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_IN, thisx->world.pos.x, thisx->world.pos.y,
                                         thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y, 1, 1);
        this->unk_398 = 14.34f;
    } else if (thisx->params == ENHORSE_4) {
        func_80881634(this);
        this->unk_398 = 14.34f;
        this->rider = (EnIn*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_IN, thisx->world.pos.x, thisx->world.pos.y,
                                         thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y, 1, 1);
        this->unk_1EC |= 0x100;
    } else if (thisx->params == ENHORSE_5) {
        func_80881634(this);
        this->unk_398 = 14.525f;
        this->rider = (EnIn*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_IN, thisx->world.pos.x, thisx->world.pos.y,
                                         thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y, 1, 2);
        this->unk_1EC |= 0x100;
    } else if (thisx->params == ENHORSE_9) {
        EnHorse_InitCutscene(this, play);
    } else if (thisx->params == ENHORSE_10) {
        EnHorse_InitHorsebackArchery(this);
        Interface_InitMinigame(play);
    } else if (thisx->params == ENHORSE_14) {
        func_808846F0(this, play);
        if ((play->sceneId == SCENE_LOST_WOODS) && !Cutscene_IsPlaying(play)) {
            Actor_Kill(&this->actor);
        }
    } else if (thisx->params == ENHORSE_16) {
        func_8087C9F8(this);
    } else if (thisx->params == ENHORSE_15) {
        EnHorse_ResetIdleAnimation(this);
    } else if (thisx->params == ENHORSE_18) {
        func_80884994(this);
    } else if (thisx->params == ENHORSE_19) {
        EnIn* in;

        func_80884D04(this, play);
        in = (EnIn*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_IN, thisx->world.pos.x, thisx->world.pos.y,
                                thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y, 1, 1);
        this->rider = in;
        in->unk4AC |= (0x20 | 0x4);
    } else if (thisx->params == ENHORSE_20) {
        EnIn* in;

        func_80884D04(this, play);
        in = (EnIn*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_IN, thisx->world.pos.x, thisx->world.pos.y,
                                thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y, 1, 1);
        this->rider = in;
        in->unk4AC |= (0x20 | 0x8 | 0x4);
    } else {
        EnHorse_StartIdleRidable(this);
    }

    thisx->shape.rot.z = 0;
    thisx->world.rot.z = thisx->shape.rot.z;
    thisx->home.rot.z = thisx->shape.rot.z;
    this->unk_3EC = thisx->world.rot.y;
    this->unk_538 = OBJ_UM_ANIM_TROT;

    if (this->unk_1EC & 0x100) {
        this->colliderCylinder1.base.colMaterial = COL_MATERIAL_HIT3;
        this->colliderCylinder1.base.acFlags |= (AC_TYPE_PLAYER | AC_ON);
        this->colliderCylinder1.elem.acElemFlags |= ACELEM_ON;
        this->colliderCylinder1.elem.acDmgInfo.dmgFlags = 0x10000 | 0x2000 | 0x1000 | 0x800 | 0x20;
        this->colliderCylinder2.base.colMaterial = COL_MATERIAL_HIT3;
        this->colliderCylinder2.base.acFlags |= (AC_TYPE_PLAYER | AC_ON);
        this->colliderCylinder2.elem.acElemFlags |= ACELEM_ON;
        this->colliderCylinder2.elem.acDmgInfo.dmgFlags = 0x10000 | 0x2000 | 0x1000 | 0x800 | 0x20;
    }
}

// EnHorse_WaitForObject
void func_8087D540(Actor* thisx, PlayState* play) {
    EnHorse* this = (EnHorse*)thisx;

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        this->actor.update = EnHorse_Update;
        if (this->unk_1EC & 1) {
            if (this->type == HORSE_TYPE_BANDIT) {
                SkelAnime_InitFlex(play, &this->skin.skelAnime, &gHorseBanditSkel, NULL, this->jointTable,
                                   this->morphTable, HORSE_BANDIT_LIMB_MAX);
            } else {
                SkelAnime_InitFlex(play, &this->skin.skelAnime, &gDonkeySkel, NULL, this->jointTable, this->morphTable,
                                   DONKEY_LIMB_MAX);
            }
        } else {
            Skin_Init(&play->state, &this->skin, sSkeletonHeaders[this->type], sAnimationHeaders[this->type][0]);
        }
        Animation_PlayOnce(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex]);
    }
}

void EnHorse_Destroy(Actor* thisx, PlayState* play) {
    EnHorse* this = (EnHorse*)thisx;

    if (this->stateFlags & ENHORSE_DRAW) {
        AudioSfx_StopByPos(&this->unk_218);
    }
    Skin_Free(&play->state, &this->skin);
    Collider_DestroyCylinder(play, &this->colliderCylinder1);
    Collider_DestroyCylinder(play, &this->colliderCylinder2);
    Collider_DestroyJntSph(play, &this->colliderJntSph);
}

void EnHorse_RotateToPlayer(EnHorse* this, PlayState* play) {
    EnHorse_RotateToPoint(this, play, &GET_PLAYER(play)->actor.world.pos, 0x320);
    if (this->stateFlags & ENHORSE_OBSTACLE) {
        this->actor.world.rot.y += 1600;
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void EnHorse_Freeze(EnHorse* this, PlayState* play) {
    if ((this->action != ENHORSE_ACTION_CS_UPDATE) && (this->action != ENHORSE_ACTION_21) &&
        (this->action != ENHORSE_ACTION_HBA)) {
        if (sResetNoInput[this->actor.params] && (this->actor.params != ENHORSE_6)) {
            Player* player = GET_PLAYER(play);

            this->noInputTimerMax = 0;
            this->noInputTimer = 0;
            player->actor.world.pos = this->actor.world.pos;
            player->actor.world.pos.y += 70.0f;
        }
        this->prevAction = this->action;
        this->action = ENHORSE_ACTION_FROZEN;
        this->colliderCylinder1.base.ocFlags1 &= ~OC1_ON;
        this->colliderCylinder2.base.ocFlags1 &= ~OC1_ON;
        this->colliderJntSph.base.ocFlags1 &= ~OC1_ON;
        this->animIndex = ENHORSE_ANIM_IDLE;
    }
}

void EnHorse_Frozen(EnHorse* this, PlayState* play) {
    this->actor.speed = 0.0f;
    this->noInputTimer--;
    if (this->noInputTimer < 0) {
        this->colliderCylinder1.base.ocFlags1 |= OC1_ON;
        this->colliderCylinder2.base.ocFlags1 |= OC1_ON;
        this->colliderJntSph.base.ocFlags1 |= OC1_ON;
        if (this->playerControlled == true) {
            this->stateFlags &= ~ENHORSE_FLAG_7;
            if (this->actor.params == ENHORSE_6) {
                EnHorse_StartMountedIdleResetAnim(this);
            } else if (this->actor.params == ENHORSE_11) {
                this->actor.params = ENHORSE_7;
                if (play->csCtx.state != CS_STATE_IDLE) {
                    EnHorse_StartMountedIdle(this);
                } else {
                    this->actor.speed = 8.0f;
                    EnHorse_StartGalloping(this);
                }
            } else if (this->prevAction == ENHORSE_ACTION_IDLE) {
                EnHorse_StartMountedIdle(this);
            } else {
                EnHorse_StartMountedIdleResetAnim(this);
            }

            if (this->actor.params != ENHORSE_0) {
                this->actor.params = ENHORSE_0;
            }
        } else if (this->prevAction == ENHORSE_ACTION_MOUNTED_IDLE) {
            EnHorse_ChangeIdleAnimation(this, 0, 0.0f);
        } else if (this->prevAction == ENHORSE_ACTION_MOUNTED_IDLE_WHINNYING) {
            EnHorse_ChangeIdleAnimation(this, 0, 0.0f);
        } else {
            EnHorse_ChangeIdleAnimation(this, 0, 0.0f);
        }
    }
}

void EnHorse_UpdateSpeed(EnHorse* this, PlayState* play, f32 brakeDecel, f32 brakeAngle, f32 minStickMag, f32 decel,
                         f32 baseSpeed, s16 turnSpeed) {
    f32 phi_f0;
    f32 stickMag;
    s16 stickAngle;
    s16 turn;
    f32 temp_f12;

    if (!EnHorse_PlayerCanMove(this, play)) {
        if (this->actor.speed > 8.0f) {
            this->actor.speed -= decel;
        } else if (this->actor.speed < 0.0f) {
            this->actor.speed = 0.0f;
        }
        return;
    }

    baseSpeed *= EnHorse_SlopeSpeedMultiplier(this, play);
    EnHorse_StickDirection(&this->curStick, &stickMag, &stickAngle);

    if (Math_CosS(stickAngle) <= brakeAngle) {
        this->actor.speed -= brakeDecel;
        this->actor.speed = CLAMP_MIN(this->actor.speed, 0.0f);
        return;
    }

    if (stickMag < minStickMag) {
        this->stateFlags &= ~ENHORSE_BOOST;
        this->stateFlags &= ~ENHORSE_BOOST_DECEL;
        this->actor.speed -= decel;
        if (this->actor.speed < 0.0f) {
            this->actor.speed = 0.0f;
        }
        return;
    }

    if (this->stateFlags & ENHORSE_BOOST) {
        if ((16 - this->boostTimer) > 0) {
            this->actor.speed = (((EnHorse_SlopeSpeedMultiplier(this, play) * this->boostSpeed) - this->actor.speed) /
                                 (16.0f - this->boostTimer)) +
                                this->actor.speed;
        } else {
            this->actor.speed = EnHorse_SlopeSpeedMultiplier(this, play) * this->boostSpeed;
        }

        if ((EnHorse_SlopeSpeedMultiplier(this, play) * this->boostSpeed) <= this->actor.speed) {
            this->stateFlags &= ~ENHORSE_BOOST;
            this->stateFlags |= ENHORSE_BOOST_DECEL;
        }
    } else if (this->stateFlags & ENHORSE_BOOST_DECEL) {
        if (this->actor.speed > baseSpeed) {
            this->actor.speed -= 0.06f;
        } else if (this->actor.speed < baseSpeed) {
            this->actor.speed = baseSpeed;
            this->stateFlags &= ~ENHORSE_BOOST_DECEL;
        }
    } else {
        if (this->actor.speed <= (baseSpeed * (1.0f / 54.0f) * stickMag)) {
            phi_f0 = 1.0f;
        } else {
            phi_f0 = -1.0f;
        }

        this->actor.speed += phi_f0 * 50.0f * 0.01f;

        if (this->actor.speed > baseSpeed) {
            this->actor.speed -= decel;
            if (this->actor.speed < baseSpeed) {
                this->actor.speed = baseSpeed;
            }
        }
    }

    temp_f12 = stickAngle * (1 / 32236.f);
    turn = stickAngle * temp_f12 * temp_f12 * (2.2f - (this->actor.speed * (1.0f / this->boostSpeed)));
    turn = CLAMP(turn, -turnSpeed * (2.2f - (1.7f * this->actor.speed * (1.0f / this->boostSpeed))),
                 turnSpeed * (2.2f - (1.7f * this->actor.speed * (1.0f / this->boostSpeed))));
    this->actor.world.rot.y += turn;
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void EnHorse_StartMountedIdleResetAnim(EnHorse* this) {
    this->skin.skelAnime.curFrame = 0.0f;
    EnHorse_StartMountedIdle(this);
    this->stateFlags &= ~ENHORSE_SANDDUST_SOUND;
}

void EnHorse_StartMountedIdle(EnHorse* this) {
    f32 curFrame;

    this->action = ENHORSE_ACTION_MOUNTED_IDLE;
    this->animIndex = ENHORSE_ANIM_IDLE;

    if (((this->curFrame > 35.0f) && (this->type == HORSE_TYPE_EPONA)) ||
        ((this->curFrame > 28.0f) && (this->type == HORSE_TYPE_HNI))) {
        if (!(this->stateFlags & ENHORSE_SANDDUST_SOUND)) {
            this->stateFlags |= ENHORSE_SANDDUST_SOUND;
            if (this->type == HORSE_TYPE_2) {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_SANDDUST);
            } else {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_SANDDUST);
            }
        }
    }

    curFrame = this->skin.skelAnime.curFrame;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, curFrame,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_MountedIdle(EnHorse* this, PlayState* play) {
    f32 mag;
    s16 angle = 0;

    this->actor.speed = 0.0f;
    EnHorse_StickDirection(&this->curStick, &mag, &angle);
    if (mag > 10.0f) {
        if (EnHorse_PlayerCanMove(this, play) == true) {
            if (Math_CosS(angle) <= -0.5f) {
                EnHorse_StartReversingInterruptable(this);
            } else if (Math_CosS(angle) <= 0.7071f) {
                EnHorse_StartTurning(this);
            } else {
                EnHorse_StartWalkingFromIdle(this);
            }
        } else if (this->unk_3EC != this->actor.world.rot.y) {
            EnHorse_StartTurning(this);
        }
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        EnHorse_MountedIdleAnim(this);
    }
}

void EnHorse_MountedIdleAnim(EnHorse* this) {
    this->skin.skelAnime.curFrame = 0.0f;
    EnHorse_MountedIdleWhinny(this);
}

void EnHorse_MountedIdleWhinny(EnHorse* this) {
    f32 curFrame;

    this->action = ENHORSE_ACTION_MOUNTED_IDLE_WHINNYING;
    this->animIndex = ENHORSE_ANIM_WHINNY;
    curFrame = this->skin.skelAnime.curFrame;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, curFrame,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][1]), ANIMMODE_ONCE, -3.0f);
    if (this->stateFlags & ENHORSE_DRAW) {
        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_KID_HORSE_GROAN);
        } else {
            Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_HORSE_GROAN);
        }
    }
}

void EnHorse_MountedIdleWhinnying(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle = 0;

    this->actor.speed = 0.0f;
    EnHorse_StickDirection(&this->curStick, &stickMag, &stickAngle);
    if (stickMag > 10.0f) {
        if (EnHorse_PlayerCanMove(this, play) == true) {
            if (Math_CosS(stickAngle) <= -0.5f) {
                EnHorse_StartReversingInterruptable(this);
            } else if (Math_CosS(stickAngle) <= 0.7071f) {
                EnHorse_StartTurning(this);
            } else {
                EnHorse_StartWalkingFromIdle(this);
            }
        } else if (this->unk_3EC != this->actor.world.rot.y) {
            EnHorse_StartTurning(this);
        }
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        EnHorse_StartMountedIdleResetAnim(this);
    }
}

void EnHorse_StartTurning(EnHorse* this) {
    this->action = ENHORSE_ACTION_MOUNTED_TURN;
    this->soundTimer = 0;
    this->animIndex = ENHORSE_ANIM_WALK;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][4]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_MountedTurn(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 clampedYaw;
    s16 stickAngle;

    this->actor.speed = 0.0f;
    EnHorse_PlayWalkingSound(this);
    if (EnHorse_PlayerCanMove(this, play) == true) {
        EnHorse_StickDirection(&this->curStick, &stickMag, &stickAngle);
        if (stickMag > 10.0f) {
            if (!EnHorse_PlayerCanMove(this, play)) {
                EnHorse_StartMountedIdleResetAnim(this);
            } else if (Math_CosS(stickAngle) <= -0.5f) {
                EnHorse_StartReversingInterruptable(this);
            } else if (Math_CosS(stickAngle) <= 0.7071f) {
                clampedYaw = CLAMP(stickAngle, -1600.0f, 1600.0f);
                this->actor.world.rot.y += clampedYaw;
                this->actor.shape.rot.y = this->actor.world.rot.y;
            } else {
                EnHorse_StartWalkingInterruptable(this);
            }
        }
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        if (EnHorse_PlayerCanMove(this, play) == true) {
            if (Math_CosS(stickAngle) <= 0.7071f) {
                EnHorse_StartTurning(this);
            } else {
                EnHorse_StartMountedIdleResetAnim(this);
            }
        } else if (this->unk_3EC != this->actor.world.rot.y) {
            EnHorse_StartTurning(this);
        } else {
            EnHorse_StartMountedIdleResetAnim(this);
        }
    }
}

void EnHorse_StartWalkingFromIdle(EnHorse* this) {
    EnHorse_StartWalkingInterruptable(this);
    if (!(this->stateFlags & ENHORSE_FLAG_8) && !(this->stateFlags & ENHORSE_FLAG_9)) {
        this->stateFlags |= ENHORSE_FLAG_9;
        this->waitTimer = 8;
    } else {
        this->waitTimer = 0;
    }
}

void EnHorse_StartWalkingInterruptable(EnHorse* this) {
    this->noInputTimer = 0;
    this->noInputTimerMax = 0;
    EnHorse_StartWalking(this);
}

void EnHorse_StartWalking(EnHorse* this) {
    this->action = ENHORSE_ACTION_MOUNTED_WALK;
    this->soundTimer = 0;
    this->animIndex = ENHORSE_ANIM_WALK;
    this->waitTimer = 0;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][4]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_MountedWalkingReset(EnHorse* this) {
    this->action = ENHORSE_ACTION_MOUNTED_WALK;
    this->soundTimer = 0;
    this->animIndex = ENHORSE_ANIM_WALK;
    this->waitTimer = 0;
    Animation_PlayOnce(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex]);
}

void EnHorse_MountedWalk(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle;

    EnHorse_PlayWalkingSound(this);
    EnHorse_StickDirection(&this->curStick, &stickMag, &stickAngle);

    if ((this->noInputTimerMax == 0) ||
        ((this->noInputTimer > 0) && (this->noInputTimer < (this->noInputTimerMax - 20)))) {
        EnHorse_UpdateSpeed(this, play, 0.3f, -0.5f, 10.0f, 0.06f, 3.0f, 0x320);
    } else {
        this->actor.speed = 3.0f;
    }

    if (this->actor.speed == 0.0f) {
        this->stateFlags &= ~ENHORSE_FLAG_9;
        EnHorse_StartMountedIdleResetAnim(this);
        this->noInputTimer = 0;
        this->noInputTimerMax = 0;
    } else if (this->actor.speed > 3.0f) {
        this->stateFlags &= ~ENHORSE_FLAG_9;
        EnHorse_StartTrotting(this);
        this->noInputTimer = 0;
        this->noInputTimerMax = 0;
    }

    if (this->noInputTimer > 0) {
        this->noInputTimer--;
        if (this->noInputTimer <= 0) {
            this->noInputTimerMax = 0;
        }
    }

    if (this->waitTimer <= 0) {
        this->stateFlags &= ~ENHORSE_FLAG_9;
        this->skin.skelAnime.playSpeed = this->actor.speed * 0.75f;
        if ((SkelAnime_Update(&this->skin.skelAnime) || (this->actor.speed == 0.0f)) && (this->noInputTimer <= 0)) {
            if (this->actor.speed > 3.0f) {
                EnHorse_StartTrotting(this);
                this->noInputTimer = 0;
                this->noInputTimerMax = 0;
            } else if ((stickMag < 10.0f) || (Math_CosS(stickAngle) <= -0.5f)) {
                EnHorse_StartMountedIdleResetAnim(this);
                this->noInputTimer = 0;
                this->noInputTimerMax = 0;
            } else {
                EnHorse_MountedWalkingReset(this);
            }
        }
    } else {
        this->waitTimer--;
        this->actor.speed = 0.0f;
    }
}

void EnHorse_StartTrotting(EnHorse* this) {
    this->action = ENHORSE_ACTION_MOUNTED_TROT;
    this->animIndex = ENHORSE_ANIM_TROT;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_MountedTrotReset(EnHorse* this) {
    this->action = ENHORSE_ACTION_MOUNTED_TROT;
    this->animIndex = ENHORSE_ANIM_TROT;
    Animation_PlayOnce(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex]);
}

void EnHorse_MountedTrot(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle;

    EnHorse_UpdateSpeed(this, play, 0.3f, -0.5f, 10.0f, 0.06f, 6.0f, 800);
    EnHorse_StickDirection(&this->curStick, &stickMag, &stickAngle);
    if (this->actor.speed < 3.0f) {
        EnHorse_StartWalkingInterruptable(this);
    }

    this->skin.skelAnime.playSpeed = this->actor.speed * 0.375f;

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        func_8087C178(this);
        Rumble_Request(0.0f, 60, 8, 255);
        if (this->actor.speed >= 6.0f) {
            EnHorse_StartGallopingInterruptable(this);
        } else if (this->actor.speed < 3.0f) {
            EnHorse_StartWalkingInterruptable(this);
        } else {
            EnHorse_MountedTrotReset(this);
        }
    }
}

void EnHorse_StartGallopingInterruptable(EnHorse* this) {
    this->noInputTimer = 0;
    this->noInputTimerMax = 0;
    EnHorse_StartGalloping(this);
}

void EnHorse_StartGalloping(EnHorse* this) {
    this->action = ENHORSE_ACTION_MOUNTED_GALLOP;
    this->animIndex = ENHORSE_ANIM_GALLOP;
    this->unk_230 = 0;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_MountedGallopReset(EnHorse* this) {
    this->noInputTimer = 0;
    this->noInputTimerMax = 0;
    this->action = ENHORSE_ACTION_MOUNTED_GALLOP;
    this->animIndex = ENHORSE_ANIM_GALLOP;
    this->unk_230 = 0;
    Animation_PlayOnce(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex]);
}

void EnHorse_JumpLanding(EnHorse* this, PlayState* play) {
    Vec3s* jointTable;
    f32 y;

    this->action = ENHORSE_ACTION_MOUNTED_GALLOP;
    this->animIndex = ENHORSE_ANIM_GALLOP;
    Animation_PlayOnce(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex]);
    jointTable = this->skin.skelAnime.jointTable;
    y = jointTable[LIMB_ROOT_POS].y;
    this->riderPos.y += y * 0.01f * this->unk_528 * 0.01f;
    this->postDrawFunc = NULL;
}

void EnHorse_MountedGallop(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle;

    EnHorse_StickDirection(&this->curStick, &stickMag, &stickAngle);

    if (this->noInputTimer <= 0) {
        EnHorse_UpdateSpeed(this, play, 0.3f, -0.5f, 10.0f, 0.06f, 8.0f, 800);
    } else if (this->noInputTimer > 0) {
        this->noInputTimer--;
        this->actor.speed = 8.0f;
    }

    if (this->actor.speed < 6.0f) {
        EnHorse_StartTrotting(this);
    }

    this->skin.skelAnime.playSpeed = this->actor.speed * 0.3f;

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        func_8087C1C0(this);
        Rumble_Request(0.0f, 120, 8, 255);
        if (EnHorse_PlayerCanMove(this, play) == true) {
            if ((stickMag >= 10.0f) && (Math_CosS(stickAngle) <= -0.5f)) {
                EnHorse_StartBraking(this, play);
            } else if (this->actor.speed < 6.0f) {
                EnHorse_StartTrotting(this);
            } else {
                EnHorse_MountedGallopReset(this);
            }
        } else {
            EnHorse_MountedGallopReset(this);
        }
    }
}

void EnHorse_StartRearing(EnHorse* this) {
    this->action = ENHORSE_ACTION_MOUNTED_REARING;
    this->animIndex = ENHORSE_ANIM_REARING;

    if (sAnimationHeaders[this->type][this->animIndex] == NULL) {
        if (Rand_ZeroOne() > 0.5f) {
            if (this->stateFlags & ENHORSE_DRAW) {
                if (this->type == HORSE_TYPE_2) {
                    Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_KID_HORSE_NEIGH);
                } else {
                    Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_HORSE_NEIGH);
                }
            }
            Rumble_Request(0.0f, 180, 20, 100);
            this->stateFlags &= ~ENHORSE_STOPPING_NEIGH_SOUND;
        }
        EnHorse_StartMountedIdleResetAnim(this);
    }

    this->stateFlags &= ~ENHORSE_LAND2_SOUND;
    if (this->stateFlags & ENHORSE_DRAW) {
        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_KID_HORSE_NEIGH);
        } else {
            Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_HORSE_NEIGH);
        }
    }

    Rumble_Request(0.0f, 180, 20, 100);
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_MountedRearing(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle;

    this->actor.speed = 0.0f;
    if (this->curFrame > 25.0f) {
        if (!(this->stateFlags & ENHORSE_LAND2_SOUND)) {
            this->stateFlags |= ENHORSE_LAND2_SOUND;
            if (this->type == HORSE_TYPE_2) {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_LAND2);
            } else {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_LAND2);
            }
            Rumble_Request(0.0f, 180, 20, 100);
        }
    }

    EnHorse_StickDirection(&this->curStick, &stickMag, &stickAngle);

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        if (EnHorse_PlayerCanMove(this, play) == true) {
            if (this->stateFlags & ENHORSE_FORCE_REVERSING) {
                this->noInputTimer = 100;
                this->noInputTimerMax = 100;
                this->stateFlags &= ~ENHORSE_FORCE_REVERSING;
                EnHorse_StartReversing(this);
            } else if (this->stateFlags & ENHORSE_FORCE_WALKING) {
                this->noInputTimer = 100;
                this->noInputTimerMax = 100;
                this->stateFlags &= ~ENHORSE_FORCE_WALKING;
                EnHorse_StartWalking(this);
            } else if (Math_CosS(stickAngle) <= -0.5f) {
                EnHorse_StartReversingInterruptable(this);
            } else {
                EnHorse_StartMountedIdleResetAnim(this);
            }
        } else {
            EnHorse_StartMountedIdleResetAnim(this);
        }
    }
}

void EnHorse_StartBraking(EnHorse* this, PlayState* play) {
    this->action = ENHORSE_ACTION_STOPPING;
    this->animIndex = ENHORSE_ANIM_STOPPING;
    if (sAnimationHeaders[this->type][this->animIndex] == NULL) {
        if (Rand_ZeroOne() > 0.5f) {
            if (this->stateFlags & ENHORSE_DRAW) {
                if (this->type == HORSE_TYPE_2) {
                    Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_KID_HORSE_NEIGH);
                } else {
                    Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_HORSE_NEIGH);
                }
            }
            Rumble_Request(0.0f, 180, 20, 100);
            this->stateFlags &= ~ENHORSE_STOPPING_NEIGH_SOUND;
        }
        EnHorse_StartMountedIdleResetAnim(this);
    }

    if (this->type == HORSE_TYPE_2) {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_SLIP);
    } else {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_SLIP);
    }

    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.5f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);
    this->stateFlags |= ENHORSE_STOPPING_NEIGH_SOUND;
    this->stateFlags &= ~ENHORSE_BOOST;
}

void EnHorse_Stopping(EnHorse* this, PlayState* play) {
    if (this->actor.speed > 0.0f) {
        this->actor.speed -= 0.6f;
        if (this->actor.speed < 0.0f) {
            this->actor.speed = 0.0f;
        }
    }

    if ((this->stateFlags & ENHORSE_STOPPING_NEIGH_SOUND) && (this->skin.skelAnime.curFrame > 29.0f)) {
        this->actor.speed = 0.0f;
        if ((Rand_ZeroOne() > 0.5f) &&
            ((gSaveContext.save.entrance != ENTRANCE(ROMANI_RANCH, 0)) || (Cutscene_GetSceneLayer(play) == 0))) {
            if (this->stateFlags & ENHORSE_DRAW) {
                if (this->type == HORSE_TYPE_2) {
                    Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_KID_HORSE_NEIGH);
                } else {
                    Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_HORSE_NEIGH);
                }
            }
            Rumble_Request(0.0f, 180, 20, 100);
            this->stateFlags &= ~ENHORSE_STOPPING_NEIGH_SOUND;
        } else {
            EnHorse_StartMountedIdleResetAnim(this);
        }
    }

    if (this->skin.skelAnime.curFrame > 29.0f) {
        this->actor.speed = 0.0f;
    } else if ((this->actor.speed > 3.0f) && (this->stateFlags & ENHORSE_FORCE_REVERSING)) {
        this->actor.speed = 3.0f;
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        if (this->stateFlags & ENHORSE_FORCE_REVERSING) {
            this->noInputTimer = 100;
            this->noInputTimerMax = 100;
            EnHorse_StartReversing(this);
            this->stateFlags &= ~ENHORSE_FORCE_REVERSING;
        } else {
            EnHorse_StartMountedIdleResetAnim(this);
        }
    }
}

void EnHorse_StartReversingInterruptable(EnHorse* this) {
    this->noInputTimer = 0;
    this->noInputTimerMax = 0;
    EnHorse_StartReversing(this);
}

void EnHorse_StartReversing(EnHorse* this) {
    this->action = ENHORSE_ACTION_REVERSE;
    this->animIndex = ENHORSE_ANIM_WALK;
    this->soundTimer = 0;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_LOOP, -3.0f);
}

void EnHorse_Reverse(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle;
    s16 turnAmount;
    Player* player = GET_PLAYER(play);

    EnHorse_PlayWalkingSound(this);
    EnHorse_StickDirection(&this->curStick, &stickMag, &stickAngle);
    if (EnHorse_PlayerCanMove(this, play) == true) {
        if ((this->noInputTimerMax == 0) ||
            ((this->noInputTimer > 0) && (this->noInputTimer < (this->noInputTimerMax - 20)))) {
            if ((stickMag < 10.0f) && (this->noInputTimer <= 0)) {
                EnHorse_StartMountedIdleResetAnim(this);
                this->actor.speed = 0.0f;
                return;
            } else if (stickMag < 10.0f) {
                stickAngle = -0x7FFF;
            } else if (Math_CosS(stickAngle) > -0.5f) {
                this->noInputTimerMax = 0;
                EnHorse_StartMountedIdleResetAnim(this);
                this->actor.speed = 0.0f;
                return;
            }
        } else if (stickMag < 10.0f) {
            stickAngle = -0x7FFF;
        }
    } else if ((player->actor.flags & ACTOR_FLAG_TALK) || (play->csCtx.state != CS_STATE_IDLE) ||
               (CutsceneManager_GetCurrentCsId() != CS_ID_NONE) || (player->stateFlags1 & PLAYER_STATE1_20)) {
        EnHorse_StartMountedIdleResetAnim(this);
        this->actor.speed = 0.0f;
        return;
    } else {
        stickAngle = -0x7FFF;
    }

    this->actor.speed = -2.0f;
    turnAmount = -0x8000 - stickAngle;
    turnAmount = CLAMP(turnAmount, -2400.0f, 2400.0f);
    this->actor.world.rot.y += turnAmount;
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->noInputTimer > 0) {
        this->noInputTimer--;
        if (this->noInputTimer <= 0) {
            this->noInputTimerMax = 0;
        }
    }

    this->skin.skelAnime.playSpeed = this->actor.speed * 0.5f * 1.5f;

    if (SkelAnime_Update(&this->skin.skelAnime) && (this->noInputTimer <= 0) &&
        (EnHorse_PlayerCanMove(this, play) == true)) {
        if ((stickMag > 10.0f) && (Math_CosS(stickAngle) <= -0.5f)) {
            this->noInputTimerMax = 0;
            EnHorse_StartReversingInterruptable(this);
        } else if (stickMag < 10.0f) {
            this->noInputTimerMax = 0;
            EnHorse_StartMountedIdleResetAnim(this);
        } else {
            EnHorse_StartReversing(this);
        }
    }
}

void EnHorse_LowJumpInit(EnHorse* this, PlayState* play) {
    this->skin.skelAnime.curFrame = 0.0f;
    EnHorse_StartLowJump(this, play);
}

void EnHorse_StartLowJump(EnHorse* this, PlayState* play) {
    f32 curFrame;
    Vec3s* jointTable;
    f32 y;

    this->action = ENHORSE_ACTION_LOW_JUMP;
    this->animIndex = ENHORSE_ANIM_LOW_JUMP;
    curFrame = this->skin.skelAnime.curFrame;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.5f, curFrame,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);
    this->postDrawFunc = NULL;
    this->jumpStartY = this->actor.world.pos.y;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;

    jointTable = this->skin.skelAnime.jointTable;
    y = jointTable[LIMB_ROOT_POS].y;
    this->riderPos.y -= ((y * 0.01f) * this->unk_528) * 0.01f;

    if (this->type == HORSE_TYPE_2) {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_JUMP);
    } else {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_JUMP);
    }
    Rumble_Request(0.0f, 170, 10, 10);
}

void EnHorse_Stub1(EnHorse* this) {
}

void EnHorse_LowJump(EnHorse* this, PlayState* play) {
    Vec3f pad;
    f32 temp_f0;
    f32 curFrame;
    Vec3s* jointTable;

    this->stateFlags |= ENHORSE_JUMPING;

    if (this->actor.speed < 12.0f) {
        this->actor.speed = 12.0f;
    }

    if (this->actor.floorHeight != BGCHECK_Y_MIN) {
        CollisionPoly* colPoly;
        s32 floorBgId;
        Vec3f pos = this->actor.world.pos;

        pos.y = this->actor.floorHeight - 5.0f;
        temp_f0 = BgCheck_EntityRaycastFloor5_2(play, &play->colCtx, &colPoly, &floorBgId, &this->actor, &pos);
        if ((this->actor.floorHeight - 120.0f) < temp_f0) {
            this->actor.floorHeight = temp_f0;
            this->actor.floorPoly = colPoly;
            this->actor.floorBgId = floorBgId;
        }
    }

    curFrame = this->skin.skelAnime.curFrame;

    if (curFrame > 17.0f) {
        this->actor.gravity = -3.5f;
        if (this->actor.velocity.y == 0.0f) {
            this->actor.velocity.y = -6.0f;
        }
        if (this->actor.world.pos.y < (this->actor.floorHeight + 90.0f)) {
            this->skin.skelAnime.playSpeed = 1.5f;
        } else {
            this->skin.skelAnime.playSpeed = 0.0f;
        }
    } else {
        jointTable = this->skin.skelAnime.jointTable;
        temp_f0 = jointTable[LIMB_ROOT_POS].y;

        this->actor.world.pos.y = this->jumpStartY + (temp_f0 * 0.01f * this->unk_528 * 0.01f);
    }

    if (SkelAnime_Update(&this->skin.skelAnime) ||
        ((curFrame > 17.0f) &&
         (this->actor.world.pos.y < ((this->actor.floorHeight - this->actor.velocity.y) + 80.0f)))) {
        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_LAND);
        } else {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_LAND);
        }
        Rumble_Request(0.0f, 255, 10, 80);
        this->stateFlags &= ~ENHORSE_JUMPING;
        this->actor.gravity = -3.5f;
        this->actor.world.pos.y = this->actor.floorHeight;
        EnHorse_JumpLanding(this, play);
    }
}

void EnHorse_HighJumpInit(EnHorse* this, PlayState* play) {
    this->skin.skelAnime.curFrame = 0.0f;
    EnHorse_StartHighJump(this, play);
}

void EnHorse_StartHighJump(EnHorse* this, PlayState* play) {
    f32 curFrame;
    Vec3s* jointTable;
    f32 y;

    this->action = ENHORSE_ACTION_HIGH_JUMP;
    this->animIndex = ENHORSE_ANIM_HIGH_JUMP;
    curFrame = this->skin.skelAnime.curFrame;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.5f, curFrame,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);

    this->postDrawFunc = NULL;
    this->jumpStartY = this->actor.world.pos.y;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;

    jointTable = this->skin.skelAnime.jointTable;
    y = jointTable[LIMB_ROOT_POS].y;
    this->riderPos.y -= ((y * 0.01f) * this->unk_528) * 0.01f;

    this->stateFlags |= ENHORSE_CALC_RIDER_POS;
    if (this->type == HORSE_TYPE_2) {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_JUMP);
    } else {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_JUMP);
    }
    Rumble_Request(0.0f, 170, 10, 10);
}

void EnHorse_Stub2(EnHorse* this) {
}

void EnHorse_HighJump(EnHorse* this, PlayState* play) {
    Vec3f pad;
    f32 temp_f0;
    f32 curFrame;
    Vec3s* jointTable;

    this->stateFlags |= ENHORSE_JUMPING;

    if (this->actor.speed < 13.0f) {
        this->actor.speed = 13.0f;
    }

    if (this->actor.floorHeight != BGCHECK_Y_MIN) {
        CollisionPoly* colPoly;
        s32 floorBgId;
        Vec3f pos = this->actor.world.pos;

        pos.y = this->actor.floorHeight - 5.0f;
        temp_f0 = BgCheck_EntityRaycastFloor5_2(play, &play->colCtx, &colPoly, &floorBgId, &this->actor, &pos);
        if ((this->actor.floorHeight - 120.0f) < temp_f0) {
            this->actor.floorHeight = temp_f0;
            this->actor.floorPoly = colPoly;
            this->actor.floorBgId = floorBgId;
        }
    }

    curFrame = this->skin.skelAnime.curFrame;

    if (curFrame > 23.0f) {
        this->actor.gravity = -3.5f;
        if (this->actor.velocity.y == 0.0f) {
            this->actor.velocity.y = -10.5f;
        }
        if (this->actor.world.pos.y < (this->actor.floorHeight + 90.0f)) {
            this->skin.skelAnime.playSpeed = 1.5f;
        } else {
            this->skin.skelAnime.playSpeed = 0.0f;
        }
    } else {
        jointTable = this->skin.skelAnime.jointTable;
        temp_f0 = jointTable[LIMB_ROOT_POS].y;
        this->actor.world.pos.y = this->jumpStartY + (temp_f0 * 0.01f * this->unk_528 * 0.01f);
    }

    if (SkelAnime_Update(&this->skin.skelAnime) ||
        ((curFrame > 23.0f) &&
         (this->actor.world.pos.y < ((this->actor.floorHeight - this->actor.velocity.y) + 80.0f)))) {
        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_LAND);
        } else {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_LAND);
        }
        Rumble_Request(0.0f, 255, 10, 80);
        this->stateFlags &= ~ENHORSE_JUMPING;
        this->actor.gravity = -3.5f;
        this->actor.world.pos.y = this->actor.floorHeight;
        func_800B1598(play, 25.0f, &this->actor.world.pos);
        EnHorse_JumpLanding(this, play);
    }
}

void EnHorse_InitInactive(EnHorse* this) {
    this->colliderCylinder1.base.ocFlags1 &= ~OC1_ON;
    this->colliderCylinder2.base.ocFlags1 &= ~OC1_ON;
    this->colliderJntSph.base.ocFlags1 &= ~OC1_ON;
    this->action = ENHORSE_ACTION_INACTIVE;
    this->animIndex = ENHORSE_ANIM_WALK;
    this->stateFlags |= ENHORSE_INACTIVE;
    this->followTimer = 0;
}

void EnHorse_Inactive(EnHorse* this, PlayState* play) {
    if (gHorsePlayedEponasSong && (this->type == HORSE_TYPE_2)) {
        gHorsePlayedEponasSong = false;
        if (EnHorse_Spawn(this, play)) {
            if (this->type == HORSE_TYPE_2) {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_NEIGH);
            }
            this->stateFlags &= ~ENHORSE_INACTIVE;
        }
    }

    if (!(this->stateFlags & ENHORSE_INACTIVE)) {
        this->followTimer = 0;
        EnHorse_SetFollowAnimation(this, play);
        this->actor.params = ENHORSE_0;
        this->colliderCylinder1.base.ocFlags1 |= OC1_ON;
        this->colliderCylinder2.base.ocFlags1 |= OC1_ON;
        this->colliderJntSph.base.ocFlags1 |= OC1_ON;
    }
}

void EnHorse_PlayIdleAnimation(EnHorse* this, s32 animIndex, f32 morphFrames, f32 startFrames) {
    this->action = ENHORSE_ACTION_IDLE;
    this->actor.speed = 0.0f;

    if ((animIndex != ENHORSE_ANIM_IDLE) && (animIndex != ENHORSE_ANIM_WHINNY) && (animIndex != ENHORSE_ANIM_REARING)) {
        animIndex = ENHORSE_ANIM_IDLE;
    }

    if (sAnimationHeaders[this->type][animIndex] == NULL) {
        animIndex = ENHORSE_ANIM_IDLE;
    }

    if (this->animIndex != animIndex) {
        this->animIndex = animIndex;
        if (animIndex == ENHORSE_ANIM_IDLE) {
            this->stateFlags &= ~ENHORSE_SANDDUST_SOUND;
        } else {
            if (this->animIndex == ENHORSE_ANIM_WHINNY) {
                if (this->stateFlags & ENHORSE_DRAW) {
                    if (this->type == HORSE_TYPE_2) {
                        Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_KID_HORSE_GROAN);
                    } else {
                        Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_HORSE_GROAN);
                    }
                }
            } else if (this->animIndex == ENHORSE_ANIM_REARING) {
                if (this->stateFlags & ENHORSE_DRAW) {
                    if (this->type == HORSE_TYPE_2) {
                        Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_KID_HORSE_NEIGH);
                    } else {
                        Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_HORSE_NEIGH);
                    }
                }
                this->stateFlags &= ~ENHORSE_LAND2_SOUND;
            }
        }
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, startFrames,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE,
                         morphFrames);
    }
}

void EnHorse_ChangeIdleAnimation(EnHorse* this, s32 animIndex, f32 morphFrames) {
    EnHorse_PlayIdleAnimation(this, animIndex, morphFrames, this->curFrame);
}

void EnHorse_ResetIdleAnimation(EnHorse* this) {
    this->animIndex = ENHORSE_ANIM_WALK;
    EnHorse_PlayIdleAnimation(this, this->animIndex, 0.0f, 0.0f);
}

void EnHorse_StartIdleRidable(EnHorse* this) {
    EnHorse_ResetIdleAnimation(this);
    this->stateFlags &= ~ENHORSE_UNRIDEABLE;
}

void EnHorse_Idle(EnHorse* this, PlayState* play) {
    this->actor.speed = 0.0f;
    EnHorse_IdleAnimSounds(this, play);

    if (gHorsePlayedEponasSong && (this->type == HORSE_TYPE_2)) {
        gHorsePlayedEponasSong = false;
        if (!func_8087C38C(play, this, &this->actor.world.pos)) {
            if (EnHorse_Spawn(this, play)) {
                if (this->type == HORSE_TYPE_2) {
                    Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_NEIGH);
                } else {
                    Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_NEIGH);
                }
                this->followTimer = 0;
                EnHorse_SetFollowAnimation(this, play);
            }
        } else {
            if (this->type == HORSE_TYPE_2) {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_NEIGH);
            } else {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_NEIGH);
            }
            this->followTimer = 0;
            EnHorse_StartMovingAnimation(this, 6, -3.0f, 0.0f);
        }
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        s32 idleAnimIndex = 0;

        if (this->animIndex != ENHORSE_ANIM_IDLE) {
            if (this->animIndex == ENHORSE_ANIM_WHINNY) {
                idleAnimIndex = 1;
            } else if (this->animIndex == ENHORSE_ANIM_REARING) {
                idleAnimIndex = 2;
            }
        }
        // Play one of the two other idle animations
        EnHorse_PlayIdleAnimation(this, sIdleAnimIndices[((Rand_ZeroOne() > 0.5f) ? 0 : 1) + idleAnimIndex * 2], 0.0f,
                                  0.0f);
    }
}

void EnHorse_StartMovingAnimation(EnHorse* this, s32 animIndex, f32 morphFrames, f32 startFrames) {
    this->action = ENHORSE_ACTION_FOLLOW_PLAYER;
    this->stateFlags &= ~ENHORSE_TURNING_TO_PLAYER;

    if ((animIndex != ENHORSE_ANIM_TROT) && (animIndex != ENHORSE_ANIM_GALLOP) && (animIndex != ENHORSE_ANIM_WALK)) {
        animIndex = ENHORSE_ANIM_WALK;
    }

    if (this->animIndex != animIndex) {
        this->animIndex = animIndex;
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, startFrames,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE,
                         morphFrames);
    } else {
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, startFrames,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, 0.0f);
    }
}

void EnHorse_SetFollowAnimation(EnHorse* this, PlayState* play) {
    s32 animIndex = ENHORSE_ANIM_WALK;
    f32 distToPlayer = Actor_WorldDistXZToActor(&this->actor, &GET_PLAYER(play)->actor);

    if (distToPlayer > 400.0f) {
        animIndex = ENHORSE_ANIM_GALLOP;
    } else if (!(distToPlayer <= 300.0f) && (distToPlayer <= 400.0f)) {
        animIndex = ENHORSE_ANIM_TROT;
    }

    if (this->animIndex == ENHORSE_ANIM_GALLOP) {
        if (distToPlayer > 400.0f) {
            animIndex = ENHORSE_ANIM_GALLOP;
        } else {
            animIndex = ENHORSE_ANIM_TROT;
        }
    } else if (this->animIndex == ENHORSE_ANIM_TROT) {
        if (distToPlayer > 400.0f) {
            animIndex = ENHORSE_ANIM_GALLOP;
        } else if (distToPlayer < 300.0f) {
            animIndex = ENHORSE_ANIM_WALK;
        } else {
            animIndex = ENHORSE_ANIM_TROT;
        }
    } else if (this->animIndex == ENHORSE_ANIM_WALK) {
        if (distToPlayer > 300.0f) {
            animIndex = ENHORSE_ANIM_TROT;
        } else {
            animIndex = ENHORSE_ANIM_WALK;
        }
    }

    EnHorse_StartMovingAnimation(this, animIndex, -3.0f, 0.0f);
}

void EnHorse_FollowPlayer(EnHorse* this, PlayState* play) {
    f32 distToPlayer;

    gHorsePlayedEponasSong = false;
    distToPlayer = Actor_WorldDistXZToActor(&this->actor, &GET_PLAYER(play)->actor);

    if (((this->playerDir == PLAYER_DIR_BACK_R) || (this->playerDir == PLAYER_DIR_BACK_L)) && (distToPlayer > 300.0f) &&
        !(this->stateFlags & ENHORSE_TURNING_TO_PLAYER)) {
        f32 angleDiff;

        this->animIndex = ENHORSE_ANIM_REARING;
        this->stateFlags |= ENHORSE_TURNING_TO_PLAYER;
        this->angleToPlayer = Actor_WorldYawTowardActor(&this->actor, &GET_PLAYER(play)->actor);

        angleDiff = (f32)this->angleToPlayer - this->actor.world.rot.y;
        if (angleDiff > 0x7FFF) {
            angleDiff -= 0x7FFF;
        } else if (angleDiff < -0x7FFF) {
            angleDiff += 0x7FFF;
        }

        this->followPlayerTurnSpeed =
            angleDiff / Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]);

        Animation_PlayOnce(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex]);
        this->skin.skelAnime.playSpeed = 1.0f;
        this->stateFlags &= ~ENHORSE_LAND2_SOUND;
    } else if (this->stateFlags & ENHORSE_TURNING_TO_PLAYER) {
        this->actor.world.rot.y += this->followPlayerTurnSpeed;
        this->actor.shape.rot.y = this->actor.world.rot.y;
        if ((this->curFrame > 25.0f) && !(this->stateFlags & ENHORSE_LAND2_SOUND)) {
            this->stateFlags |= ENHORSE_LAND2_SOUND;
            if (this->type == HORSE_TYPE_2) {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_LAND2);
            } else {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_LAND2);
            }
        }
    } else {
        EnHorse_RotateToPlayer(this, play);
    }

    if (this->animIndex == ENHORSE_ANIM_GALLOP) {
        this->actor.speed = 8.0f;
        this->skin.skelAnime.playSpeed = this->actor.speed * 0.3f;
    } else {
        if (this->animIndex == ENHORSE_ANIM_TROT) {
            this->actor.speed = 6.0f;
            this->skin.skelAnime.playSpeed = this->actor.speed * 0.375f;
        } else if (this->animIndex == ENHORSE_ANIM_WALK) {
            this->actor.speed = 3.0f;
            EnHorse_PlayWalkingSound(this);
            this->skin.skelAnime.playSpeed = this->actor.speed * 0.75f;
        } else {
            this->actor.speed = 0.0f;
            this->skin.skelAnime.playSpeed = 1.0f;
        }
    }

    if (!(this->stateFlags & ENHORSE_TURNING_TO_PLAYER)) {
        this->followTimer++;
        if (this->followTimer > 300) {
            EnHorse_StartIdleRidable(this);
            if (this->stateFlags & ENHORSE_DRAW) {
                if (this->type == HORSE_TYPE_2) {
                    Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_KID_HORSE_NEIGH);
                } else {
                    Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_HORSE_NEIGH);
                }
            }
        }
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        if (this->animIndex == ENHORSE_ANIM_GALLOP) {
            func_8087C1C0(this);
        } else if (this->animIndex == ENHORSE_ANIM_TROT) {
            func_8087C178(this);
        }

        this->stateFlags &= ~ENHORSE_TURNING_TO_PLAYER;

        if (distToPlayer < 100.0f) {
            EnHorse_StartIdleRidable(this);
        } else {
            EnHorse_SetFollowAnimation(this, play);
        }
    }
}

void EnHorse_InitIngoHorse(EnHorse* this) {
    this->curRaceWaypoint = 0;
    this->soundTimer = 0;
    this->actor.speed = 0.0f;
    EnHorse_UpdateIngoHorseAnim(this);
    if (this->stateFlags & ENHORSE_DRAW) {
        Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_IT_INGO_HORSE_NEIGH);
    }
}

void EnHorse_SetIngoAnimation(s32 animIndex, f32 curFrame, s32 arg2, s16* animIndexOut) {
    *animIndexOut = sIngoAnimIndices[animIndex];
    if (arg2 == 1) {
        if (animIndex == ENHORSE_ANIM_TROT) {
            *animIndexOut = ENIN_ANIM_4;
        } else if (animIndex == ENHORSE_ANIM_GALLOP) {
            *animIndexOut = ENIN_ANIM_3;
        }
    }
}

void EnHorse_UpdateIngoHorseAnim(EnHorse* this) {
    s32 animChanged = false;
    f32 animSpeed;

    this->action = ENHORSE_ACTION_INGO_RACE;
    this->stateFlags &= ~ENHORSE_SANDDUST_SOUND;

    if (this->actor.speed == 0.0f) {
        if (this->animIndex != ENHORSE_ANIM_IDLE) {
            animChanged = true;
        }
        this->animIndex = ENHORSE_ANIM_IDLE;
    } else if (this->actor.speed <= 3.0f) {
        if (this->animIndex != ENHORSE_ANIM_WALK) {
            animChanged = true;
        }
        this->animIndex = ENHORSE_ANIM_WALK;
    } else if (this->actor.speed <= 6.0f) {
        if (this->animIndex != ENHORSE_ANIM_TROT) {
            animChanged = true;
        }
        this->animIndex = ENHORSE_ANIM_TROT;
    } else {
        if (this->animIndex != ENHORSE_ANIM_GALLOP) {
            animChanged = true;
        }
        this->animIndex = ENHORSE_ANIM_GALLOP;
    }

    if (this->animIndex == ENHORSE_ANIM_WALK) {
        animSpeed = this->actor.speed * 0.5f;
    } else if (this->animIndex == ENHORSE_ANIM_TROT) {
        animSpeed = this->actor.speed * 0.25f;
        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_RUN);
        } else {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_RUN);
        }
    } else if (this->animIndex == ENHORSE_ANIM_GALLOP) {
        animSpeed = this->actor.speed * 0.2f;
        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_RUN);
        } else {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_RUN);
        }
    } else {
        animSpeed = 1.0f;
    }

    if (animChanged == true) {
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex],
                         sPlaybackSpeeds[this->animIndex] * animSpeed * 1.5f, 0.0f,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);
    } else {
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex],
                         sPlaybackSpeeds[this->animIndex] * animSpeed * 1.5f, 0.0f,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, 0.0f);
    }
}

void EnHorse_UpdateIngoRace(EnHorse* this, PlayState* play) {
    f32 playSpeed;

    if ((this->animIndex == ENHORSE_ANIM_IDLE) || (this->animIndex == ENHORSE_ANIM_WHINNY)) {
        EnHorse_IdleAnimSounds(this, play);
    } else if (this->animIndex == ENHORSE_ANIM_WALK) {
        EnHorse_PlayWalkingSound(this);
    }

    if (!this->inRace) {
        this->actor.speed = 0.0f;
        this->rider->actor.speed = 0.0f;
        if (this->animIndex != ENHORSE_ANIM_IDLE) {
            EnHorse_UpdateIngoHorseAnim(this);
        }
    }

    if (this->animIndex == ENHORSE_ANIM_WALK) {
        playSpeed = this->actor.speed * 0.5f;
    } else if (this->animIndex == ENHORSE_ANIM_TROT) {
        playSpeed = this->actor.speed * 0.25f;
    } else if (this->animIndex == ENHORSE_ANIM_GALLOP) {
        playSpeed = this->actor.speed * 0.2f;
    } else {
        playSpeed = 1.0f;
    }
    this->skin.skelAnime.playSpeed = playSpeed;

    if (SkelAnime_Update(&this->skin.skelAnime) ||
        ((this->animIndex == ENHORSE_ANIM_IDLE) && (this->actor.speed != 0.0f))) {
        EnHorse_UpdateIngoHorseAnim(this);
    }
}

void func_8088126C(EnHorse* this, PlayState* play) {
    this->skin.skelAnime.curFrame = 0.0f;
    func_80881290(this, play);
}

void func_80881290(EnHorse* this, PlayState* play) {
    f32 curFrame;

    this->action = ENHORSE_ACTION_6;
    this->animIndex = ENHORSE_ANIM_HIGH_JUMP;
    curFrame = this->skin.skelAnime.curFrame;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.5f, curFrame,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);
    this->postDrawFunc = NULL;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->jumpStartY = this->actor.world.pos.y;
    if (this->type == HORSE_TYPE_2) {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_JUMP);
    } else {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_JUMP);
    }
    Rumble_Request(0.0f, 170, 10, 10);
}

void func_80881398(EnHorse* this, PlayState* play) {
    Vec3s* jointTable;
    f32 y;
    s32 isAnimFinished;
    f32 curFrame;

    this->stateFlags |= ENHORSE_JUMPING;
    if (this->actor.speed < 14.0f) {
        this->actor.speed = 14.0f;
    }

    isAnimFinished = SkelAnime_Update(&this->skin.skelAnime);
    curFrame = this->skin.skelAnime.curFrame;

    if (curFrame > 23.0f) {
        this->actor.gravity = -3.5f;
        if (this->actor.velocity.y == 0.0f) {
            this->actor.velocity.y = -10.5f;
        }
        if (this->actor.world.pos.y < (this->actor.floorHeight + 90.0f)) {
            this->skin.skelAnime.playSpeed = 1.5f;
        } else {
            this->skin.skelAnime.playSpeed = 0.0f;
        }
    } else {
        jointTable = this->skin.skelAnime.jointTable;
        y = jointTable[LIMB_ROOT_POS].y;
        this->actor.world.pos.y = this->jumpStartY + (y * 0.01f * this->unk_528 * 0.01f);
    }

    if (isAnimFinished || ((curFrame > 17.0f) &&
                           (this->actor.world.pos.y < ((this->actor.floorHeight - this->actor.velocity.y) + 80.0f)))) {
        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_LAND);
        } else {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_LAND);
        }
        Rumble_Request(0.0f, 255, 10, 80);
        this->stateFlags &= ~ENHORSE_JUMPING;
        this->stateFlags &= ~ENHORSE_FLAG_30;
        this->actor.gravity = -3.5f;
        this->actor.world.pos.y = this->actor.floorHeight;
        func_800B1598(play, 25.0f, &this->actor.world.pos);
        func_8088159C(this, play);
    }
}

void func_8088159C(EnHorse* this, PlayState* play) {
    Vec3s* jointTable;
    f32 y;

    this->action = ENHORSE_ACTION_5;
    this->animIndex = ENHORSE_ANIM_GALLOP;
    Animation_PlayOnce(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex]);
    jointTable = this->skin.skelAnime.jointTable;
    y = jointTable[LIMB_ROOT_POS].y;
    this->riderPos.y += y * 0.01f * this->unk_528 * 0.01f;
    this->postDrawFunc = NULL;
}

void func_80881634(EnHorse* this) {
    this->curRaceWaypoint = 0;
    this->soundTimer = 0;
    this->actor.speed = 0.0f;
    func_8088168C(this);
    if (this->stateFlags & ENHORSE_DRAW) {
        Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_IT_INGO_HORSE_NEIGH);
    }
}

void func_8088168C(EnHorse* this) {
    s32 animChanged = false;
    f32 animSpeed;
    f32 finalAnimSpeed;

    this->action = ENHORSE_ACTION_5;
    this->stateFlags &= ~ENHORSE_SANDDUST_SOUND;

    if (this->actor.speed == 0.0f) {
        if (this->animIndex != ENHORSE_ANIM_IDLE) {
            animChanged = true;
        }
        this->animIndex = ENHORSE_ANIM_IDLE;
    } else if (this->actor.speed <= 3.0f) {
        if (this->animIndex != ENHORSE_ANIM_WALK) {
            animChanged = true;
        }
        this->animIndex = ENHORSE_ANIM_WALK;
    } else if (this->actor.speed <= 6.0f) {
        if (this->animIndex != ENHORSE_ANIM_TROT) {
            animChanged = true;
        }
        this->animIndex = ENHORSE_ANIM_TROT;
    } else {
        if (this->animIndex != ENHORSE_ANIM_GALLOP) {
            animChanged = true;
        }
        this->animIndex = ENHORSE_ANIM_GALLOP;
    }

    if (this->animIndex == ENHORSE_ANIM_WALK) {
        animSpeed = this->actor.speed * 0.5f;
    } else if (this->animIndex == ENHORSE_ANIM_TROT) {
        animSpeed = this->actor.speed * 0.25f;
        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_RUN);
        } else {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_RUN);
        }
    } else if (this->animIndex == ENHORSE_ANIM_GALLOP) {
        animSpeed = this->actor.speed * 0.2f;
        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_RUN);
        } else {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_RUN);
        }
    } else {
        animSpeed = 1.0f;
    }

    finalAnimSpeed = sPlaybackSpeeds[this->animIndex] * animSpeed * 1.5f;
    if (finalAnimSpeed < 1.0f) {
        finalAnimSpeed = 1.0f;
    }

    if (animChanged == true) {
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], finalAnimSpeed, 0.0f,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);
    } else {
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], finalAnimSpeed, 0.0f,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, 0.0f);
    }
}

void func_808819D8(EnHorse* this, PlayState* play) {
    Path* path;
    f32 animSpeed;

    if ((this->animIndex == ENHORSE_ANIM_IDLE) || (this->animIndex == ENHORSE_ANIM_WHINNY)) {
        EnHorse_IdleAnimSounds(this, play);
    } else if (this->animIndex == ENHORSE_ANIM_WALK) {
        EnHorse_PlayWalkingSound(this);
    }

    if (!this->inRace) {
        this->actor.speed = 0.0f;
        this->rider->actor.speed = 0.0f;
        if (this->animIndex != ENHORSE_ANIM_IDLE) {
            EnHorse_UpdateIngoHorseAnim(this);
        }
    }

    if (this->actor.params == ENHORSE_4) {
        path = &play->setupPathList[0];
        func_8087B7C0(this, play, path);
    } else if (this->actor.params == ENHORSE_5) {
        path = &play->setupPathList[1];
        func_8087B7C0(this, play, path);
    }

    if (!this->inRace) {
        this->actor.speed = 0.0f;
        if (this->animIndex != ENHORSE_ANIM_IDLE) {
            func_8088168C(this);
        }
    }

    if (this->animIndex == ENHORSE_ANIM_WALK) {
        animSpeed = this->actor.speed * 0.5f;
    } else if (this->animIndex == ENHORSE_ANIM_TROT) {
        animSpeed = this->actor.speed * 0.25f;
    } else if (this->animIndex == ENHORSE_ANIM_GALLOP) {
        animSpeed = this->actor.speed * 0.2f;
    } else {
        animSpeed = 1.0f;
    }
    this->skin.skelAnime.playSpeed = animSpeed;

    if (SkelAnime_Update(&this->skin.skelAnime) ||
        ((this->animIndex == ENHORSE_ANIM_IDLE) && (this->actor.speed != 0.0f))) {
        func_8088168C(this);
    }

    if (GET_WEEKEVENTREG_HORSE_RACE_STATE == WEEKEVENTREG_HORSE_RACE_STATE_3) {
        this->rider->animIndex2 = ENIN_ANIM2_7;
    } else {
        EnHorse_SetIngoAnimation(this->animIndex, this->skin.skelAnime.curFrame, this->unk_394 & 1,
                                 &this->rider->animIndex2);
    }
}

void EnHorse_CsMoveInit(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    this->animIndex = ENHORSE_ANIM_GALLOP;
    this->cutsceneAction = 1;
    Animation_PlayOnceSetSpeed(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex],
                               this->actor.speed * 0.2f * 1.5f);
}

void EnHorse_CsMoveToPoint(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    Vec3f endPos;
    s32 pad;

    endPos.x = cue->endPos.x;
    endPos.y = cue->endPos.y;
    endPos.z = cue->endPos.z;

    if (Math3D_Vec3f_DistXYZ(&endPos, &this->actor.world.pos) > 8.0f) {
        EnHorse_RotateToPoint(this, play, &endPos, 0x320);
        this->actor.speed = 8.0f;
        this->skin.skelAnime.playSpeed = this->actor.speed * 0.3f;
    } else {
        this->actor.world.pos = endPos;
        this->actor.speed = 0.0f;
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        func_8087C1C0(this);
        Rumble_Request(0.0f, 120, 8, 255);
        Animation_PlayOnceSetSpeed(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex],
                                   this->actor.speed * 0.3f);
    }
}

void EnHorse_CsSetAnimHighJump(EnHorse* this, PlayState* play) {
    this->skin.skelAnime.curFrame = 0.0f;
    EnHorse_CsPlayHighJumpAnim(this, play);
}

void EnHorse_CsPlayHighJumpAnim(EnHorse* this, PlayState* play) {
    f32 curFrame;
    f32 y;
    Vec3s* jointTable;

    this->animIndex = ENHORSE_ANIM_HIGH_JUMP;
    curFrame = this->skin.skelAnime.curFrame;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.5f, curFrame,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);
    this->postDrawFunc = NULL;
    this->jumpStartY = this->actor.world.pos.y;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;

    jointTable = this->skin.skelAnime.jointTable;
    y = jointTable[LIMB_ROOT_POS].y;
    this->riderPos.y -= y * 0.01f * this->unk_528 * 0.01f;

    this->stateFlags |= ENHORSE_ANIM_HIGH_JUMP;
    if (this->type == HORSE_TYPE_2) {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_JUMP);
    } else {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_JUMP);
    }
    Rumble_Request(0.0f, 170, 10, 10);
}

void EnHorse_CsJumpInit(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    EnHorse_CsSetAnimHighJump(this, play);
    this->cutsceneAction = 2;
    this->cutsceneFlags &= ~1;
}

void EnHorse_CsJump(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    f32 curFrame;
    f32 y;
    Vec3s* jointTable;
    s32 pad[2];

    if (this->cutsceneFlags & 1) {
        EnHorse_CsMoveToPoint(this, play, cue);
        return;
    }

    curFrame = this->skin.skelAnime.curFrame;
    this->actor.speed = 13.0f;
    this->stateFlags |= ENHORSE_JUMPING;

    if (curFrame > 19.0f) {
        this->actor.gravity = -3.5f;
        if (this->actor.velocity.y == 0.0f) {
            this->actor.velocity.y = -10.5f;
        }
        if (this->actor.world.pos.y < (this->actor.floorHeight + 90.0f)) {
            this->skin.skelAnime.playSpeed = 1.5f;
        } else {
            this->skin.skelAnime.playSpeed = 0.0f;
        }
    } else {
        jointTable = this->skin.skelAnime.jointTable;
        y = jointTable[LIMB_ROOT_POS].y;

        this->actor.world.pos.y = this->jumpStartY + (y * 0.01f * this->unk_528 * 0.01f);
    }

    if (SkelAnime_Update(&this->skin.skelAnime) ||
        ((curFrame > 19.0f) &&
         (this->actor.world.pos.y < ((this->actor.floorHeight - this->actor.velocity.y) + 80.0f)))) {
        this->cutsceneFlags |= 1;
        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_LAND);
        } else {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_LAND);
        }
        Rumble_Request(0.0f, 255, 10, 80);
        this->stateFlags &= ~ENHORSE_JUMPING;
        this->actor.gravity = -3.5f;
        this->actor.velocity.y = 0.0f;
        this->actor.world.pos.y = this->actor.floorHeight;
        func_800B1598(play, 25.0f, &this->actor.world.pos);
        this->animIndex = ENHORSE_ANIM_GALLOP;
        Animation_PlayOnceSetSpeed(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex],
                                   sPlaybackSpeeds[6]);

        jointTable = this->skin.skelAnime.jointTable;
        y = jointTable[LIMB_ROOT_POS].y;
        this->riderPos.y += y * 0.01f * this->unk_528 * 0.01f;

        this->postDrawFunc = NULL;
    }
}

void EnHorse_CsRearingInit(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    this->animIndex = ENHORSE_ANIM_REARING;
    this->cutsceneAction = 3;
    this->cutsceneFlags &= ~4;
    this->stateFlags &= ~ENHORSE_LAND2_SOUND;
    if (this->stateFlags & ENHORSE_DRAW) {
        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_KID_HORSE_NEIGH);
        } else {
            Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_HORSE_NEIGH);
        }
    }
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_CsRearing(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    this->actor.speed = 0.0f;
    if (this->curFrame > 25.0f) {
        if (!(this->stateFlags & ENHORSE_LAND2_SOUND)) {
            this->stateFlags |= ENHORSE_LAND2_SOUND;
            if (this->type == HORSE_TYPE_2) {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_LAND2);
            } else {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_LAND2);
            }
        }
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        this->animIndex = ENHORSE_ANIM_IDLE;
        if (!(this->cutsceneFlags & 4)) {
            this->cutsceneFlags |= 4;
            Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE,
                             -3.0f);
        } else {
            Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_LOOP,
                             0.0f);
        }
    }
}

void EnHorse_WarpMoveInit(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    this->actor.world.pos.x = cue->startPos.x;
    this->actor.world.pos.y = cue->startPos.y;
    this->actor.world.pos.z = cue->startPos.z;
    this->actor.prevPos = this->actor.world.pos;

    this->actor.world.rot.y = cue->rot.y;
    this->actor.shape.rot = this->actor.world.rot;

    this->animIndex = ENHORSE_ANIM_GALLOP;
    this->cutsceneAction = 4;
    Animation_PlayOnceSetSpeed(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex],
                               this->actor.speed * 0.3f);
}

void EnHorse_CsWarpMoveToPoint(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    Vec3f endPos;
    s32 pad;

    endPos.x = cue->endPos.x;
    endPos.y = cue->endPos.y;
    endPos.z = cue->endPos.z;

    if (Math3D_Vec3f_DistXYZ(&endPos, &this->actor.world.pos) > 8.0f) {
        EnHorse_RotateToPoint(this, play, &endPos, 0x320);
        this->actor.speed = 8.0f;
        this->skin.skelAnime.playSpeed = this->actor.speed * 0.3f;
    } else {
        this->actor.world.pos = endPos;
        this->actor.speed = 0.0f;
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        func_8087C1C0(this);
        Rumble_Request(0.0f, 120, 8, 255);
        Animation_PlayOnceSetSpeed(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex],
                                   this->actor.speed * 0.3f);
    }
}

void EnHorse_CsWarpRearingInit(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    this->actor.world.pos.x = cue->startPos.x;
    this->actor.world.pos.y = cue->startPos.y;
    this->actor.world.pos.z = cue->startPos.z;
    this->actor.prevPos = this->actor.world.pos;

    this->actor.world.rot.y = cue->rot.y;
    this->actor.shape.rot = this->actor.world.rot;

    this->animIndex = ENHORSE_ANIM_REARING;
    this->cutsceneAction = 5;
    this->cutsceneFlags &= ~4;
    this->stateFlags &= ~ENHORSE_LAND2_SOUND;

    if (this->stateFlags & ENHORSE_DRAW) {
        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_KID_HORSE_NEIGH);
        } else {
            Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_HORSE_NEIGH);
        }
    }
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_CsWarpRearing(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    this->actor.speed = 0.0f;
    if (this->curFrame > 25.0f) {
        if (!(this->stateFlags & ENHORSE_LAND2_SOUND)) {
            this->stateFlags |= ENHORSE_LAND2_SOUND;
            if (this->type == HORSE_TYPE_2) {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_LAND2);
            } else {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_LAND2);
            }
        }
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        this->animIndex = ENHORSE_ANIM_IDLE;
        if (!(this->cutsceneFlags & 4)) {
            this->cutsceneFlags |= 4;
            Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE,
                             -3.0f);
        } else {
            Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_LOOP,
                             0.0f);
        }
    }
}

void EnHorse_InitCutscene(EnHorse* this, PlayState* play) {
    this->playerControlled = false;
    this->action = ENHORSE_ACTION_CS_UPDATE;
    this->cutsceneAction = 0;
    this->actor.speed = 0.0f;
}

s32 EnHorse_GetCutsceneFunctionIndex(s32 cueId) {
    s32 numActions = ARRAY_COUNT(sCsActionTable); // prevents unrolling
    s32 i;

    for (i = 0; i < numActions; i++) {
        if (cueId == sCsActionTable[i].cueId) {
            return sCsActionTable[i].csFuncIdx;
        }

        if (cueId < sCsActionTable[i].cueId) {
            return 0;
        }
    }
    return 0;
}

void EnHorse_CutsceneUpdate(EnHorse* this, PlayState* play) {
    s32 csFunctionIdx;
    CsCmdActorCue* playerCue = play->csCtx.playerCue;

    if (play->csCtx.state == CS_STATE_STOP) {
        this->playerControlled = true;
        this->actor.params = ENHORSE_12;
        this->action = ENHORSE_ACTION_IDLE;
        EnHorse_Freeze(this, play);
        return;
    }

    if (playerCue != NULL) {
        csFunctionIdx = EnHorse_GetCutsceneFunctionIndex(playerCue->id);
        if (csFunctionIdx != 0) {
            if (csFunctionIdx != this->cutsceneAction) {
                if (this->cutsceneAction == 0) {
                    this->actor.world.pos.x = playerCue->startPos.x;
                    this->actor.world.pos.y = playerCue->startPos.y;
                    this->actor.world.pos.z = playerCue->startPos.z;

                    this->actor.world.rot.y = playerCue->rot.y;
                    this->actor.shape.rot = this->actor.world.rot;
                    this->actor.prevPos = this->actor.world.pos;
                }
                this->cutsceneAction = csFunctionIdx;
                sCutsceneInitFuncs[csFunctionIdx](this, play, playerCue);
            }
            sCutsceneActionFuncs[this->cutsceneAction](this, play, playerCue);
        }
    }
}

s32 EnHorse_UpdateHbaRaceInfo(EnHorse* this, PlayState* play, RaceInfo* raceInfo) {
    Vec3f pos;
    f32 px;
    f32 pz;
    f32 d;

    EnHorse_RaceWaypointPos(raceInfo->waypoints, this->curRaceWaypoint, &pos);
    Math3D_RotateXZPlane(&pos, raceInfo->waypoints[this->curRaceWaypoint].angle, &px, &pz, &d);

    if ((this->curRaceWaypoint >= (raceInfo->numWaypoints - 1)) &&
        (Math3D_Vec3f_DistXYZ(&pos, &this->actor.world.pos) < DREG(8))) {
        this->hbaFlags |= 2;
    }

    if (((this->actor.world.pos.x * px) + (pz * this->actor.world.pos.z) + d) > 0.0f) {
        this->curRaceWaypoint++;
        if (this->curRaceWaypoint >= raceInfo->numWaypoints) {
            this->hbaFlags |= 1;
            return true;
        }
    }

    if (!(this->hbaFlags & 1)) {
        EnHorse_RotateToPoint(this, play, &pos, 0x640);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    if ((this->actor.speed < raceInfo->waypoints[this->curRaceWaypoint].speed) && !(this->hbaFlags & 1)) {
        this->actor.speed += 0.4f;
    } else {
        this->actor.speed -= 0.4f;
        if (this->actor.speed < 0.0f) {
            this->actor.speed = 0.0f;
        }
    }
    return false;
}

void EnHorse_InitHorsebackArchery(EnHorse* this) {
    this->hbaStarted = 0;
    this->soundTimer = 0;
    this->curRaceWaypoint = 0;
    this->hbaTimer = 0;
    this->actor.speed = 0.0f;
    EnHorse_UpdateHbaAnim(this);
}

void EnHorse_UpdateHbaAnim(EnHorse* this) {
    s32 animChanged = false;
    f32 animSpeed;

    this->action = ENHORSE_ACTION_HBA;
    if (this->actor.speed == 0.0f) {
        if (this->animIndex != ENHORSE_ANIM_IDLE) {
            animChanged = true;
        }
        this->animIndex = ENHORSE_ANIM_IDLE;
    } else if (this->actor.speed <= 3.0f) {
        if (this->animIndex != ENHORSE_ANIM_WALK) {
            animChanged = true;
        }
        this->animIndex = ENHORSE_ANIM_WALK;
    } else if (this->actor.speed <= 6.0f) {
        if (this->animIndex != ENHORSE_ANIM_TROT) {
            animChanged = true;
        }
        this->animIndex = ENHORSE_ANIM_TROT;
    } else {
        if (this->animIndex != ENHORSE_ANIM_GALLOP) {
            animChanged = true;
        }
        this->animIndex = ENHORSE_ANIM_GALLOP;
    }

    if (this->animIndex == ENHORSE_ANIM_WALK) {
        animSpeed = this->actor.speed * 0.5f;
    } else if (this->animIndex == ENHORSE_ANIM_TROT) {
        animSpeed = this->actor.speed * 0.25f;
        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_RUN);
        } else {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_RUN);
        }
        Rumble_Request(0.0f, 60, 8, 255);
    } else if (this->animIndex == ENHORSE_ANIM_GALLOP) {
        animSpeed = this->actor.speed * 0.2f;
        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_RUN);
        } else {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_RUN);
        }
        Rumble_Request(0.0f, 120, 8, 255);
    } else {
        animSpeed = 1.0f;
    }

    if (animChanged == true) {
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex],
                         sPlaybackSpeeds[this->animIndex] * animSpeed * 1.5f, 0.0f,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);
    } else {
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex],
                         sPlaybackSpeeds[this->animIndex] * animSpeed * 1.5f, 0.0f,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, 0.0f);
    }
}

void EnHorse_UpdateHorsebackArchery(EnHorse* this, PlayState* play) {
    f32 playSpeed;
    s32 sp28;

    if (this->animIndex == ENHORSE_ANIM_WALK) {
        EnHorse_PlayWalkingSound(this);
    }

    if (play->interfaceCtx.minigameAmmo == 0) {
        this->hbaTimer++;
    }

    sp28 = Audio_IsSequencePlaying(NA_BGM_HORSE_GOAL);
    EnHorse_UpdateHbaRaceInfo(this, play, &sHbaInfo);

    if (((this->hbaFlags & 1) || (this->hbaTimer > 45)) && (sp28 != 1) &&
        (gSaveContext.minigameStatus != MINIGAME_STATUS_END)) {
        gSaveContext.save.cutsceneIndex = 0;
        play->transitionTrigger = TRANS_TRIGGER_START;
        play->transitionType = TRANS_TYPE_64;
    }

    if (play->interfaceCtx.minigameAmmo) {}

    if (((play->interfaceCtx.minigameAmmo == 0) || (this->hbaFlags & 2)) && (this->hbaFlags & 4)) {
        this->hbaFlags &= ~4;
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_HORSE_GOAL | SEQ_FLAG_ASYNC);
    }

    if (this->hbaStarted == 0) {
        this->actor.speed = 0.0f;
        if (this->animIndex != ENHORSE_ANIM_IDLE) {
            EnHorse_UpdateHbaAnim(this);
        }
    }

    if (this->animIndex == ENHORSE_ANIM_WALK) {
        playSpeed = this->actor.speed * 0.5f;
    } else if (this->animIndex == ENHORSE_ANIM_TROT) {
        playSpeed = this->actor.speed * 0.25f;
    } else if (this->animIndex == ENHORSE_ANIM_GALLOP) {
        playSpeed = this->actor.speed * 0.2f;
    } else {
        playSpeed = 1.0f;
    }
    this->skin.skelAnime.playSpeed = playSpeed;

    if (SkelAnime_Update(&this->skin.skelAnime) ||
        ((this->animIndex == ENHORSE_ANIM_IDLE) && (this->actor.speed != 0.0f))) {
        EnHorse_UpdateHbaAnim(this);
    }
}

void EnHorse_FleePlayer(EnHorse* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 distToHome;
    f32 playerDistToHome;
    f32 distToPlayer;
    s32 nextAnimIndex = this->animIndex;
    s32 isAnimFinished;
    s16 yaw;

    if (gHorsePlayedEponasSong || (this->type == HORSE_TYPE_HNI)) {
        EnHorse_StartIdleRidable(this);
        if (this->type == HORSE_TYPE_2) {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_NEIGH);
        } else {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_NEIGH);
        }
    }

    distToHome = Math3D_Vec3f_DistXYZ(&this->actor.home.pos, &this->actor.world.pos);
    playerDistToHome = Math3D_Vec3f_DistXYZ(&player->actor.world.pos, &this->actor.home.pos);
    distToPlayer = Math3D_Vec3f_DistXYZ(&player->actor.world.pos, &this->actor.world.pos);

    if (playerDistToHome > 300.0f) {
        if (distToHome > 150.0f) {
            this->actor.speed += 0.4f;
            if (this->actor.speed > 8.0f) {
                this->actor.speed = 8.0f;
            }
        } else {
            this->actor.speed -= 0.5f;
            if (this->actor.speed < 0.0f) {
                this->actor.speed = 0.0f;
            }
        }
    } else if (distToPlayer < 300.0f) {
        this->actor.speed += 0.4f;
        if (this->actor.speed > 8.0f) {
            this->actor.speed = 8.0f;
        }
    } else {
        this->actor.speed -= 0.5f;
        if (this->actor.speed < 0.0f) {
            this->actor.speed = 0.0f;
        }
    }

    if (this->actor.speed >= 6.0f) {
        this->skin.skelAnime.playSpeed = this->actor.speed * 0.3f;
        nextAnimIndex = ENHORSE_ANIM_GALLOP;
    } else if (this->actor.speed >= 3.0f) {
        this->skin.skelAnime.playSpeed = this->actor.speed * 0.375f;
        nextAnimIndex = ENHORSE_ANIM_TROT;
    } else if (this->actor.speed > 0.1f) {
        this->skin.skelAnime.playSpeed = this->actor.speed * 0.75f;
        nextAnimIndex = ENHORSE_ANIM_WALK;
        EnHorse_PlayWalkingSound(this);
    } else {
        if (Rand_ZeroOne() > 0.5f) {
            nextAnimIndex = ENHORSE_ANIM_WHINNY;
        } else {
            nextAnimIndex = ENHORSE_ANIM_IDLE;
        }
        EnHorse_IdleAnimSounds(this, play);
        this->skin.skelAnime.playSpeed = 1.0f;
    }

    if ((nextAnimIndex == ENHORSE_ANIM_GALLOP) || (nextAnimIndex == ENHORSE_ANIM_TROT) ||
        (nextAnimIndex == ENHORSE_ANIM_WALK)) {
        if (playerDistToHome < 300.0f) {
            yaw = player->actor.shape.rot.y;
            yaw += ((Actor_WorldYawTowardActor(&this->actor, &player->actor) > 0) ? 1 : -1) * 0x3FFF;
        } else {
            yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos) - this->actor.world.rot.y;
        }

        if (yaw > 800.0f) {
            this->actor.world.rot.y += 0x320;
        } else if (yaw < -800.0f) {
            this->actor.world.rot.y += -0x320;
        } else {
            this->actor.world.rot.y += yaw;
        }
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    isAnimFinished = SkelAnime_Update(&this->skin.skelAnime);

    if (((this->animIndex == ENHORSE_ANIM_IDLE) || (this->animIndex == ENHORSE_ANIM_WHINNY)) &&
        ((nextAnimIndex == ENHORSE_ANIM_GALLOP) || (nextAnimIndex == ENHORSE_ANIM_TROT) ||
         (nextAnimIndex == ENHORSE_ANIM_WALK))) {
        this->animIndex = nextAnimIndex;
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][nextAnimIndex]), ANIMMODE_ONCE, -3.0f);
        if (this->animIndex == ENHORSE_ANIM_GALLOP) {
            func_8087C1C0(this);
        } else if (this->animIndex == ENHORSE_ANIM_TROT) {
            func_8087C178(this);
        }
    } else if (isAnimFinished) {
        if (nextAnimIndex == ENHORSE_ANIM_GALLOP) {
            func_8087C1C0(this);
        } else if (nextAnimIndex == ENHORSE_ANIM_TROT) {
            func_8087C178(this);
        }

        if ((this->animIndex == ENHORSE_ANIM_IDLE) || (this->animIndex == ENHORSE_ANIM_WHINNY)) {
            if (nextAnimIndex != this->animIndex) {
                this->animIndex = nextAnimIndex;
                Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                                 Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE,
                                 -3.0f);
            } else {
                if (Rand_ZeroOne() > 0.5f) {
                    this->animIndex = ENHORSE_ANIM_IDLE;
                    this->stateFlags &= ~ENHORSE_SANDDUST_SOUND;
                } else {
                    this->animIndex = ENHORSE_ANIM_WHINNY;
                    if (this->stateFlags & ENHORSE_DRAW) {
                        if (this->type == HORSE_TYPE_2) {
                            Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_KID_HORSE_GROAN);
                        } else {
                            Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_HORSE_GROAN);
                        }
                    }
                }
                Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                                 Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE,
                                 -3.0f);
            }
        } else if (nextAnimIndex != this->animIndex) {
            this->animIndex = nextAnimIndex;
            Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE,
                             -3.0f);
        } else {
            Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE,
                             0.0f);
        }
    } else if ((this->animIndex == ENHORSE_ANIM_WALK) &&
               ((nextAnimIndex == ENHORSE_ANIM_IDLE) || (nextAnimIndex == ENHORSE_ANIM_WHINNY))) {
        this->animIndex = nextAnimIndex;
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);
    }
}

void func_80883B70(EnHorse* this, CsCmdActorCue* cue) {
    this->actor.world.pos.x = cue->startPos.x;
    this->actor.world.pos.y = cue->startPos.y;
    this->actor.world.pos.z = cue->startPos.z;

    this->actor.world.rot.y = cue->rot.y;
    this->actor.shape.rot = this->actor.world.rot;

    this->actor.prevPos = this->actor.world.pos;
}

void func_80883BEC(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    func_80883B70(this, cue);
    this->animIndex = ENHORSE_ANIM_IDLE;
    this->cueId = 1;
    this->stateFlags &= ~ENHORSE_SANDDUST_SOUND;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, 0.0f);
    this->stateFlags |= ENHORSE_SANDDUST_SOUND;
}

void func_80883CB0(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    EnHorse_IdleAnimSounds(this, play);
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        this->animIndex = ENHORSE_ANIM_IDLE;
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, 0.0f);
    }
}

void func_80883D64(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    func_80883B70(this, cue);
    this->cueId = 2;
    Animation_Change(&this->skin.skelAnime, &object_horse_link_child_Anim_00A8DC, 0.0f, 0.0f,
                     Animation_GetLastFrame(&object_horse_link_child_Anim_00A8DC), ANIMMODE_ONCE, 0.0f);
}

void func_80883DE0(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    SkelAnime_Update(&this->skin.skelAnime);
}

void func_80883E10(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    func_80883B70(this, cue);
    this->cueId = 3;
    Animation_Change(&this->skin.skelAnime, &object_horse_link_child_Anim_00A8DC, 1.0f, 0.0f,
                     Animation_GetLastFrame(&object_horse_link_child_Anim_00A8DC), ANIMMODE_ONCE, -3.0f);
    Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_KID_HORSE_NEIGH);
}

void func_80883EA0(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        Animation_Change(&this->skin.skelAnime, &object_horse_link_child_Anim_00B3E0, 1.0f, 11.0f,
                         Animation_GetLastFrame(&object_horse_link_child_Anim_00B3E0), ANIMMODE_ONCE, 0.0f);
    }
}

void func_80883F18(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    func_80883B70(this, cue);
    this->cueId = 4;
    Animation_Change(&this->skin.skelAnime, &object_horse_link_child_Anim_00AD08, 1.0f, 0.0f,
                     Animation_GetLastFrame(&object_horse_link_child_Anim_00AD08), ANIMMODE_ONCE, -3.0f);
}

void func_80883F98(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    if (Animation_OnFrame(&this->skin.skelAnime, Animation_GetLastFrame(&object_horse_link_child_Anim_00AD08) - 1.0f)) {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_LAND2);
    }
    SkelAnime_Update(&this->skin.skelAnime);
}

void func_80884010(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    func_80883B70(this, cue);
    this->cueId = 5;
    this->animIndex = ENHORSE_ANIM_WALK;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);
}

void func_808840C4(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    EnHorse_PlayWalkingSound(this);
    Cutscene_ActorTranslateAndYaw(&this->actor, play, this->cueChannel);
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, 0.0f);
    }
}

void func_80884194(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    func_80883B70(this, cue);
    this->cueId = 6;
    this->animIndex = ENHORSE_ANIM_GALLOP;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);
    func_8087C1C0(this);
}

void func_8088424C(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    Cutscene_ActorTranslateAndYaw(&this->actor, play, this->cueChannel);
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 1.0f, 0.0f,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, 0.0f);
        func_8087C1C0(this);
    }
}

void func_80884314(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 20.0f);
    func_80883B70(this, cue);
    this->cueId = 7;
    Animation_Change(&this->skin.skelAnime, &object_horse_link_child_Anim_00D178, 1.0f, 0.0f,
                     Animation_GetLastFrame(&object_horse_link_child_Anim_00D178), ANIMMODE_ONCE, 0.0f);
    Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_KID_HORSE_NEIGH);
}

void func_808843B4(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    SkelAnime_Update(&this->skin.skelAnime);
    if (this->curFrame > 42.0f) {
        if (((s32)this->curFrame % 11) == 0) {
            func_8087C1C0(this);
        }
        Cutscene_ActorTranslateAndYaw(&this->actor, play, this->cueChannel);
    }
}

void func_80884444(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 20.0f);
    func_80883B70(this, cue);
    this->cutsceneAction = 8;
    Animation_Change(&this->skin.skelAnime, &object_horse_link_child_Anim_00D4E8, 1.0f, 0.0f,
                     Animation_GetLastFrame(&object_horse_link_child_Anim_00D4E8), ANIMMODE_ONCE, 0.0f);
    func_8087C1C0(this);
}

void func_808844E0(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    Cutscene_ActorTranslateAndYaw(&this->actor, play, this->cueChannel);
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        Animation_Change(&this->skin.skelAnime, &object_horse_link_child_Anim_00D4E8, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_horse_link_child_Anim_00D4E8), ANIMMODE_ONCE, 0.0f);
        func_8087C1C0(this);
    }
}

void func_80884564(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 20.0f);
    func_80883B70(this, cue);
    this->cutsceneAction = 8;
    Animation_Change(&this->skin.skelAnime, &object_horse_link_child_Anim_00BDE0, 1.0f, 0.0f,
                     Animation_GetLastFrame(&object_horse_link_child_Anim_00BDE0), ANIMMODE_ONCE, -3.0f);
    func_8087C1C0(this);
}

void func_80884604(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    Cutscene_ActorTranslateAndYaw(&this->actor, play, this->cueChannel);
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        Animation_Change(&this->skin.skelAnime, &object_horse_link_child_Anim_00BDE0, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_horse_link_child_Anim_00BDE0), ANIMMODE_ONCE, 0.0f);
    }

    if (((s32)this->curFrame % 11) == 0) {
        func_8087C1C0(this);
    }
}

void func_808846B4(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    Actor_Kill(&this->actor);
}

void func_808846DC(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
}

void func_808846F0(EnHorse* this, PlayState* play) {
    this->playerControlled = false;
    this->action = ENHORSE_ACTION_21;
    this->cueId = -1;
    this->actor.speed = 0.0f;
}

void func_80884718(EnHorse* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_112)) {
        this->cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_112);
        cue = play->csCtx.actorCues[this->cueChannel];

        this->unk_1EC |= 0x20;
        if (this->cueId != cue->id) {
            if (this->cueId == -1) {
                this->actor.world.pos.x = cue->startPos.x;
                this->actor.world.pos.y = cue->startPos.y;
                this->actor.world.pos.z = cue->startPos.z;

                this->actor.world.rot.y = cue->rot.y;
                this->actor.shape.rot = this->actor.world.rot;

                this->actor.prevPos = this->actor.world.pos;
            }

            this->cueId = cue->id;
            if (D_808890F0[this->cueId] != NULL) {
                D_808890F0[this->cueId](this, play, cue);
            }
        }

        if (D_8088911C[this->cueId] != NULL) {
            D_8088911C[this->cueId](this, play, cue);
        }
    }
}

void func_80884868(EnHorse* this) {
    this->action = ENHORSE_ACTION_23;
    this->animIndex = ENHORSE_ANIM_WALK;
    Animation_PlayLoop(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex]);
    this->stateFlags |= ENHORSE_UNRIDEABLE;
}

void func_808848C8(EnHorse* this, PlayState* play) {
    Vec3f sp24 = { -1916.0f, -106.0f, -523.0f };

    EnHorse_PlayWalkingSound(this);
    this->actor.speed = 4.0f;
    Horse_RotateToPoint(&this->actor, &sp24, 0x7D0);
    this->skin.skelAnime.playSpeed = this->actor.speed * 0.75f;
    SkelAnime_Update(&this->skin.skelAnime);
    if (Math3D_Vec3f_DistXYZ(&sp24, &this->actor.world.pos) < 30.0f) {
        this->stateFlags &= ~ENHORSE_UNRIDEABLE;
        EnHorse_StartIdleRidable(this);
    }
}

void func_80884994(EnHorse* this) {
    this->unk_534 = -1;
    this->animIndex = ENHORSE_ANIM_TROT;
    this->action = ENHORSE_ACTION_24;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 2.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, -3.0f);
}

void func_80884A40(EnHorse* this, PlayState* play) {
    s32 sp44[] = { ENHORSE_ANIM_TROT, ENHORSE_ANIM_GALLOP, ENHORSE_ANIM_IDLE };
    s32 sp40;
    s32 temp_v0;

    if (this->animIndex == ENHORSE_ANIM_WHINNY) {
        temp_v0 = ENHORSE_ANIM_IDLE;
    } else {
        temp_v0 = this->animIndex;
    }

    sp40 = sp44[this->unk_538] != temp_v0;

    if (SkelAnime_Update(&this->skin.skelAnime) || sp40) {
        this->animIndex = sp44[this->unk_538];
        if ((this->animIndex == ENHORSE_ANIM_IDLE) && (Rand_ZeroOne() < 0.5f)) {
            this->animIndex = ENHORSE_ANIM_WHINNY;
        }

        if (sp40) {
            if (this->animIndex == ENHORSE_ANIM_IDLE) {
                Animation_MorphToPlayOnce(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], -3.0f);
            } else {
                Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 2.0f, 0.0f,
                                 Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE,
                                 -3.0f);
            }
        } else if (this->animIndex == ENHORSE_ANIM_IDLE) {
            Animation_PlayOnce(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex]);
        } else {
            Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex], 2.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE,
                             0.0f);
        }

        if ((this->unk_538 == OBJ_UM_ANIM_TROT) || (this->unk_538 == OBJ_UM_ANIM_GALLOP)) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_HORSE_RUN);
        } else if (this->unk_538 == OBJ_UM_ANIM_IDLE) {
            if (this->animIndex == ENHORSE_ANIM_IDLE) {
                EnHorse_IdleAnimSounds(this, play);
            } else if (this->animIndex == ENHORSE_ANIM_WHINNY) {
                Actor_PlaySfx(&this->actor, NA_SE_EV_HORSE_GROAN);
            }
        }
    }
}

void func_80884D04(EnHorse* this, PlayState* play) {
    f32 playSpeed;

    this->actor.speed = 10.0f;
    this->action = ENHORSE_ACTION_25;
    this->unk_540 = this->actor.world.pos;
    Actor_PlaySfx(&this->actor, NA_SE_IT_INGO_HORSE_NEIGH);
    this->animIndex = ENHORSE_ANIM_GALLOP;
    playSpeed = this->actor.speed * 0.2f;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex],
                     sPlaybackSpeeds[this->animIndex] * playSpeed * 2.5f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, 0.0f);
}

// Action func: EnHorse_UpdateBandit?
void func_80884E0C(EnHorse* this, PlayState* play) {
    f32 playSpeed = (this->unk_56C * 0.2f * 0.5f) + 1.0f;
    Vec3f pos;
    CollisionPoly* sp3C;

    this->skin.skelAnime.playSpeed = playSpeed;

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animIndex],
                         sPlaybackSpeeds[this->animIndex] * playSpeed * 2.5f, 0.0f,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animIndex]), ANIMMODE_ONCE, 0.0f);
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_HORSE_RUN);
    }

    this->unk_57C = this->actor.world.pos;
    this->actor.world.pos.x = this->banditPosition.x;
    this->actor.world.pos.z = this->banditPosition.z;

    this->unk_56C = Math3D_Vec3f_DistXYZ(&this->actor.world.pos, &this->actor.prevPos);
    if (((this->unk_550 == 5) || (this->unk_550 == 7)) && (Player_GetMask(play) != PLAYER_MASK_CIRCUS_LEADER)) {
        this->rider->animIndex2 = ENIN_ANIM2_7;
    } else {
        EnHorse_SetIngoAnimation(this->animIndex, this->skin.skelAnime.curFrame, this->unk_394 & 1,
                                 &this->rider->animIndex2);
    }

    pos = this->actor.world.pos;
    pos.y += 10.0f;

    if (BgCheck_EntityRaycastFloor1(&play->colCtx, &sp3C, &pos) != BGCHECK_Y_MIN) {
        this->actor.velocity.y += this->actor.gravity;
        if (this->actor.velocity.y < this->actor.terminalVelocity) {
            this->actor.velocity.y = this->actor.terminalVelocity;
        }
        this->actor.world.pos.y += this->actor.velocity.y * 0.5f;
    } else {
        this->actor.world.pos.y = this->banditPosition.y;
    }
}

void EnHorse_Vec3fOffset(Vec3f* src, s16 yaw, f32 dist, f32 height, Vec3f* dst) {
    dst->x = (Math_SinS(yaw) * dist) + src->x;
    dst->y = src->y + height;
    dst->z = (Math_CosS(yaw) * dist) + src->z;
}

s32 EnHorse_CalcFloorHeight(EnHorse* this, PlayState* play, Vec3f* pos, CollisionPoly** polyFloor, f32* floorHeight,
                            s32* bgId) {
    f32 waterY;
    WaterBox* waterbox;

    *polyFloor = NULL;
    *floorHeight = BgCheck_EntityRaycastFloor3(&play->colCtx, polyFloor, bgId, pos);
    if (*floorHeight == BGCHECK_Y_MIN) {
        return 1; // No floor
    }

    if ((WaterBox_GetSurface1_2(play, &play->colCtx, pos->x, pos->z, &waterY, &waterbox) == 1) &&
        (*floorHeight < waterY)) {
        return 2; // Water
    }

    if ((COLPOLY_GET_NORMAL((*polyFloor)->normal.y) < 0.81915206f) ||
        SurfaceType_IsHorseBlocked(&play->colCtx, *polyFloor, *bgId) ||
        (SurfaceType_GetFloorType(&play->colCtx, *polyFloor, *bgId) == FLOOR_TYPE_7)) {
        return 3; // Horse blocked surface
    }

    return 0;
}

/**
 * obstacleType:
 *  1: Water in front
 *  2: Water behind?
 *  3: ?
 *  4: Obstructed in front
 *  5: Obstructed behind
 */
void EnHorse_ObstructMovement(EnHorse* this, PlayState* play, s32 obstacleType, s32 galloping) {
    if (this->action != ENHORSE_ACTION_CS_UPDATE) {
        if ((this->action == ENHORSE_ACTION_5) || (this->action == ENHORSE_ACTION_6)) {
            this->actor.world.pos = this->actor.prevPos;
            this->actor.world.rot.y -= 0x640;
            this->actor.speed = 0.0f;
            this->actor.shape.rot.y = this->actor.world.rot.y;
            this->unk_1EC |= 4;
        } else if (this->action == ENHORSE_ACTION_25) {
            this->unk_1EC |= 0x80;
            this->actor.world.pos = this->actor.prevPos;
            this->actor.speed = 0.0f;
        } else if ((play->sceneId != SCENE_KOEPONARACE) || (this->unk_1EC & 2)) {
            this->unk_1EC &= ~2;
            this->actor.world.pos = this->lastPos;
            this->stateFlags |= ENHORSE_OBSTACLE;
            if (this->playerControlled == 0) {
                if (this->animIndex != ENHORSE_ANIM_REARING) {}
            } else if (this->action != ENHORSE_ACTION_MOUNTED_REARING) {
                if (this->stateFlags & ENHORSE_JUMPING) {
                    this->stateFlags &= ~ENHORSE_JUMPING;
                    this->actor.gravity = -3.5f;
                    this->actor.world.pos.y = this->actor.floorHeight;
                }

                if ((obstacleType == 1) || (obstacleType == 4)) {
                    this->stateFlags |= ENHORSE_FORCE_REVERSING;
                } else if ((obstacleType == 2) || (obstacleType == 5)) {
                    this->stateFlags |= ENHORSE_FORCE_WALKING;
                }

                if (galloping == true) {
                    EnHorse_StartRearing(this);
                }
            }
        }
    }
}

void EnHorse_CheckFloors(EnHorse* this, PlayState* play) {
    s32 pad;
    CollisionPoly* frontFloor;
    CollisionPoly* backFloor;
    s32 bgId;
    s16 floorSlope;
    Vec3f frontPos;
    Vec3f backPos;
    Vec3f pos;
    f32 nx;
    f32 ny;
    f32 nz;
    s32 galloping = this->actor.speed > 8.0f;
    s32 status;
    f32 waterHeight;
    WaterBox* waterbox;
    f32 dist;

    if ((WaterBox_GetSurface1_2(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &waterHeight,
                                &waterbox) == true) &&
        (this->actor.floorHeight < waterHeight)) {
        EnHorse_ObstructMovement(this, play, 1, galloping);
        return;
    }

    if (this->type != HORSE_TYPE_2) {
        EnHorse_Vec3fOffset(&this->actor.world.pos, this->actor.shape.rot.y, 30.0f, 55.0f, &frontPos);
    } else {
        EnHorse_Vec3fOffset(&this->actor.world.pos, this->actor.shape.rot.y, 15.0f, 30.0f, &frontPos);
    }

    status = EnHorse_CalcFloorHeight(this, play, &frontPos, &frontFloor, &this->yFront, &this->unk_24C);
    if (status == 1) {
        this->actor.shape.rot.x = 0;
        EnHorse_ObstructMovement(this, play, 4, galloping);
        return;
    }

    if (status == 3) {
        this->unk_1EC |= 2;
        EnHorse_ObstructMovement(this, play, 4, galloping);
        return;
    }

    if (this->type != HORSE_TYPE_2) {
        EnHorse_Vec3fOffset(&this->actor.world.pos, this->actor.shape.rot.y, -30.0f, 55.0f, &backPos);
    } else {
        EnHorse_Vec3fOffset(&this->actor.world.pos, this->actor.shape.rot.y, -15.0f, 30.0f, &backPos);
    }

    status = EnHorse_CalcFloorHeight(this, play, &backPos, &backFloor, &this->yBack, &bgId);
    if (status == 1) {
        this->actor.shape.rot.x = 0;
        EnHorse_ObstructMovement(this, play, 5, galloping);
        return;
    }

    if (status == 3) {
        this->unk_1EC |= 2;
        EnHorse_ObstructMovement(this, play, 5, galloping);
        return;
    }

    floorSlope = Math_Atan2S(this->yBack - this->yFront, 60.0f);
    if (this->actor.floorPoly != NULL) {
        nx = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.x);
        ny = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.y);
        nz = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.z);

        pos = frontPos;
        pos.y = this->yFront;

        dist = Math3D_DistPlaneToPos(nx, ny, nz, this->actor.floorPoly->dist, &pos);
        if ((frontFloor != this->actor.floorPoly) && (this->actor.speed >= 0.0f) &&
            ((!(this->stateFlags & ENHORSE_JUMPING) && (dist < -40.0f)) ||
             ((this->stateFlags & ENHORSE_JUMPING) && (dist < -200.0f)))) {
            EnHorse_ObstructMovement(this, play, 4, galloping);
            return;
        }

        pos = backPos;
        pos.y = this->yBack;

        dist = Math3D_DistPlaneToPos(nx, ny, nz, this->actor.floorPoly->dist, &pos);
        if ((backFloor != this->actor.floorPoly) && (this->actor.speed <= 0.0f) &&
            ((!(this->stateFlags & ENHORSE_JUMPING) && (dist < -40.0f)) ||
             ((this->stateFlags & ENHORSE_JUMPING) && (dist < -200.0f)))) {
            EnHorse_ObstructMovement(this, play, 5, galloping);
            return;
        }

        if ((ny < 0.81915206f) ||
            SurfaceType_IsHorseBlocked(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) ||
            (SurfaceType_GetFloorType(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) == FLOOR_TYPE_7)) {
            if (this->actor.speed >= 0.0f) {
                EnHorse_ObstructMovement(this, play, 4, galloping);
            } else {
                EnHorse_ObstructMovement(this, play, 5, galloping);
            }
        } else if (this->stateFlags & 4) {
            this->actor.shape.rot.x = 0;
        } else if ((this->actor.floorHeight + 4.0f) < this->actor.world.pos.y) {
            this->actor.shape.rot.x = 0;
        } else if (!(fabsf(floorSlope) > 0x1FFF)) {
            this->actor.shape.rot.x = floorSlope;
            this->actor.shape.yOffset =
                ((this->yFront + (((this->yBack - this->yFront) * 20.0f) / 45.0f)) - this->actor.floorHeight) +
                this->unk_3E8;
        }
    }
}

void EnHorse_MountDismount(EnHorse* this, PlayState* play) {
    s32 mountSide = EnHorse_GetMountSide(this, play);

    if ((mountSide != 0) && !(this->stateFlags & ENHORSE_UNRIDEABLE)) {
        Actor_SetRideActor(play, &this->actor, mountSide);
    }

    if ((this->playerControlled == false) && (Actor_HasRider(play, &this->actor) == true)) {
        this->noInputTimer = 26;
        this->noInputTimerMax = 26;
        this->playerControlled = true;
        EnHorse_Freeze(this, play);
    } else if ((this->playerControlled == true) && (Actor_HasNoRider(play, &this->actor) == true)) {
        this->noInputTimer = 35;
        this->noInputTimerMax = 35;
        this->stateFlags &= ~ENHORSE_UNRIDEABLE;
        this->playerControlled = false;
        EnHorse_Freeze(this, play);
    }
}

void EnHorse_StickDirection(Vec2f* curStick, f32* stickMag, s16* angle) {
    *stickMag = sqrtf(SQ(curStick->x) + SQ(curStick->z));
    *stickMag = CLAMP_MAX(*stickMag, 60.0f);
    *angle = Math_Atan2S(-curStick->x, curStick->z);
}

void EnHorse_UpdateStick(EnHorse* this, PlayState* play) {
    Input* input = &play->state.input[this->unk_52C];

    this->lastStick = this->curStick;
    this->curStick.x = input->rel.stick_x;
    this->curStick.z = input->rel.stick_y;

    this->curStick.x *= GameInteractor_InvertControl(GI_INVERT_HORSE_X);
}

void EnHorse_ResolveCollision(EnHorse* this, PlayState* play, CollisionPoly* colPoly) {
    f32 dist;
    f32 nx;
    f32 ny;
    f32 nz;
    f32 offset;

    nx = COLPOLY_GET_NORMAL(colPoly->normal.x);
    ny = COLPOLY_GET_NORMAL(colPoly->normal.y);
    nz = COLPOLY_GET_NORMAL(colPoly->normal.z);

    if (!(Math_CosS(BINANG_ROT180(this->actor.world.rot.y - Math_Atan2S(colPoly->normal.x, colPoly->normal.z))) <
          0.7071f)) { // cos(45 degrees)
        dist = Math3D_DistPlaneToPos(nx, ny, nz, colPoly->dist, &this->actor.world.pos);
        offset = (1.0f / sqrtf(SQ(nx) + SQ(nz)));
        offset = (30.0f - dist) * offset;
        this->actor.world.pos.x += offset * nx;
        this->actor.world.pos.z += offset * nz;
    }
}

void EnHorse_BgCheckSlowMoving(EnHorse* this, PlayState* play) {
    f32 yOffset = 40.0f;
    Vec3f start;
    Vec3f end;
    Vec3f intersect;
    CollisionPoly* colPoly;
    s32 bgId;

    Math_Vec3f_Copy(&start, &this->actor.world.pos);
    start.y += yOffset;

    Math_Vec3f_Copy(&end, &start);
    end.x += 30.0f * Math_SinS(this->actor.world.rot.y);
    end.y += 30.0f * Math_SinS(-this->actor.shape.rot.x);
    end.z += 30.0f * Math_CosS(this->actor.world.rot.y);
    if (BgCheck_EntityLineTest1(&play->colCtx, &start, &end, &intersect, &colPoly, true, false, false, true, &bgId)) {
        EnHorse_ResolveCollision(this, play, colPoly);
    }
}

void EnHorse_UpdateBgCheckInfo(EnHorse* this, PlayState* play) {
    s32 pad2[2];
    Vec3f startPos;
    Vec3f endPos;
    Vec3f obstaclePos;
    f32 temp_f0;
    f32 intersectDist;
    CollisionPoly* wall = NULL;
    CollisionPoly* obstacleFloor = NULL;
    s32 bgId;
    f32 obstacleHeight;
    f32 behindObstacleHeight;
    DynaPolyActor* horseJump;
    s32 movingFast;
    s16 sp7E = 0;
    DynaPolyActor* horseGameCheck = NULL;
    s32 pad6;
    s32 pad;
    Vec3f intersect;
    Vec3f obstacleTop;
    s32 pad5;
    s32 pad4;

    if ((this->actor.params != ENHORSE_4) && (this->actor.params != ENHORSE_5) && (this->actor.params != ENHORSE_19) &&
        (this->actor.params != ENHORSE_20) && (this->actor.params != ENHORSE_18)) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 40.0f, 35.0f, 100.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                    UPDBGCHECKINFO_FLAG_10);
    } else {
        Actor_UpdateBgCheckInfo(play, &this->actor, 40.0f, 35.0f, 100.0f,
                                UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
    }

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) &&
        (Math_CosS(this->actor.wallYaw - this->actor.world.rot.y) < -0.3f)) {
        if (this->actor.speed > 4.0f) {
            this->actor.speed -= 1.0f;
            if (this->type == HORSE_TYPE_2) {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_SANDDUST);
            } else {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_SANDDUST);
            }
        }
    }

    horseJump = DynaPoly_GetActor(&play->colCtx, this->actor.floorBgId);

    if (!(this->stateFlags & ENHORSE_JUMPING)) {
        if ((horseJump != NULL) && (horseJump->actor.id == ACTOR_EN_HORSE_GAME_CHECK) &&
            ((horseJump->actor.params & 0xFF) == 5)) {
            this->stateFlags |= ENHORSE_FLAG_28;
            this->unk_3E8 += ((-10.0f / this->actor.scale.y) - this->unk_3E8) * 0.5f;
            if (this->actor.speed > 2.0f) {
                this->actor.speed -= 1.0f;
                if (this->type == HORSE_TYPE_2) {
                    Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_SANDDUST);
                } else {
                    Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_SANDDUST);
                }
            }
        } else {
            this->stateFlags &= ~ENHORSE_FLAG_28;
            this->unk_3E8 *= 0.5f;
        }
    }

    if ((this->stateFlags & ENHORSE_JUMPING) || (this->stateFlags & ENHORSE_FLAG_28) || (this->actor.speed < 0.0f) ||
        (this->action == ENHORSE_ACTION_STOPPING) || (this->action == ENHORSE_ACTION_MOUNTED_REARING)) {
        return;
    }

    if (this->actor.speed > 8.0f) {
        if (this->actor.speed < 12.8f) {
            intersectDist = 160.0f;
            movingFast = false;
        } else {
            intersectDist = 230.0f;
            movingFast = true;
        }
    } else {
        EnHorse_BgCheckSlowMoving(this, play);
        return;
    }

    startPos = this->actor.world.pos;
    startPos.y += 19.0f;

    endPos = startPos;
    endPos.x += intersectDist * Math_SinS(this->actor.world.rot.y);
    endPos.y += intersectDist * Math_SinS(-this->actor.shape.rot.x);
    endPos.z += intersectDist * Math_CosS(this->actor.world.rot.y);

    intersect = endPos;
    wall = NULL;

    if (BgCheck_EntityLineTest1(&play->colCtx, &startPos, &endPos, &intersect, &wall, true, false, false, true,
                                &bgId) == true) {
        intersectDist = sqrtf(Math3D_Vec3fDistSq(&startPos, &intersect));
        this->stateFlags |= ENHORSE_OBSTACLE;
    }

    if (wall != NULL) {
        if (intersectDist < 30.0f) {
            EnHorse_ResolveCollision(this, play, wall);
        }

        sp7E = BINANG_ROT180(this->actor.world.rot.y - Math_Atan2S(wall->normal.x, wall->normal.z));

        if ((Math_CosS(sp7E) < 0.5f) || SurfaceType_IsHorseBlocked(&play->colCtx, wall, bgId)) {
            return;
        }

        if (((movingFast == false) && (intersectDist < 80.0f)) || ((movingFast == true) && (intersectDist < 150.0f))) {
            if ((play->sceneId != SCENE_KOEPONARACE) && (Math_CosS(sp7E) < 0.9f) && (this->playerControlled == true)) {
                if (movingFast == false) {
                    this->stateFlags |= ENHORSE_FORCE_REVERSING;
                } else if (movingFast == true) {
                    this->stateFlags |= ENHORSE_FORCE_REVERSING;
                    EnHorse_StartBraking(this, play);
                }
            }
            return;
        }

        horseJump = DynaPoly_GetActor(&play->colCtx, bgId);
        if ((this->stateFlags & ENHORSE_FLAG_26) &&
            (((horseJump != NULL) && (horseJump->actor.id != ACTOR_BG_UMAJUMP)) || (horseJump == NULL))) {
            if (this->playerControlled == true) {
                if (movingFast == false) {
                    this->stateFlags |= ENHORSE_FORCE_REVERSING;
                } else if (movingFast == true) {
                    this->stateFlags |= ENHORSE_FORCE_REVERSING;
                    EnHorse_StartBraking(this, play);
                }
            }
            return;
        }
    }

    intersectDist += 5.0f;
    obstaclePos = startPos;

    obstaclePos.x += intersectDist * Math_SinS(this->actor.world.rot.y);
    obstaclePos.y = this->actor.world.pos.y + 120.0f;
    obstaclePos.z += intersectDist * Math_CosS(this->actor.world.rot.y);

    obstacleTop = obstaclePos;
    obstacleTop.y = BgCheck_EntityRaycastFloor3(&play->colCtx, &obstacleFloor, &bgId, &obstaclePos);

    if (obstacleTop.y == BGCHECK_Y_MIN) {
        return;
    }

    obstacleHeight = obstacleTop.y - this->actor.world.pos.y;

    if ((this->actor.floorPoly == NULL) || (obstacleFloor == NULL)) {
        return;
    }

    if ((Math3D_DistPlaneToPos(COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.x),
                               COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.y),
                               COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.z), this->actor.floorPoly->dist,
                               &obstacleTop) < -40.0f) &&
        (Math3D_DistPlaneToPos(COLPOLY_GET_NORMAL(obstacleFloor->normal.x), COLPOLY_GET_NORMAL(obstacleFloor->normal.y),
                               COLPOLY_GET_NORMAL(obstacleFloor->normal.z), obstacleFloor->dist,
                               &this->actor.world.pos) > 40.0f)) {
        if ((movingFast == true) && (this->playerControlled == true) && (this->action != ENHORSE_ACTION_STOPPING) &&
            (play->sceneId != SCENE_KOEPONARACE)) {
            this->stateFlags |= ENHORSE_FORCE_REVERSING;
            EnHorse_StartBraking(this, play);
        }
        this->stateFlags |= ENHORSE_OBSTACLE;
        return;
    }

    temp_f0 = COLPOLY_GET_NORMAL(obstacleFloor->normal.y);
    if ((temp_f0 < 0.81915206f) || SurfaceType_IsHorseBlocked(&play->colCtx, obstacleFloor, bgId) ||
        (SurfaceType_GetFloorType(&play->colCtx, obstacleFloor, bgId) == FLOOR_TYPE_7)) {
        if ((Math_CosS(sp7E) < 0.9f) && (movingFast == true) && (this->playerControlled == true) &&
            (this->action != ENHORSE_ACTION_STOPPING) && (play->sceneId != SCENE_KOEPONARACE)) {
            this->stateFlags |= ENHORSE_FORCE_REVERSING;
            EnHorse_StartBraking(this, play);
        }
        return;
    }

    if (wall == NULL) {
        horseGameCheck = DynaPoly_GetActor(&play->colCtx, bgId);
        if ((horseGameCheck == NULL) || (horseGameCheck->actor.id != ACTOR_EN_HORSE_GAME_CHECK) ||
            ((horseGameCheck->actor.params & 0xFF) != 5)) {
            return;
        }
    }

    if (((obstacleTop.y < intersect.y) &&
         ((horseGameCheck == NULL) || (horseGameCheck->actor.id != ACTOR_EN_HORSE_GAME_CHECK) ||
          ((horseGameCheck->actor.params & 0xFF) != 5))) ||
        ((this->stateFlags & ENHORSE_CANT_JUMP) && (this->action != ENHORSE_ACTION_5))) {
        return;
    }

    if ((this->action == ENHORSE_ACTION_5) && !(this->stateFlags & ENHORSE_FLAG_30)) {
        horseGameCheck = DynaPoly_GetActor(&play->colCtx, bgId);
        if ((horseGameCheck != NULL) && (horseGameCheck->actor.id == ACTOR_EN_HORSE_GAME_CHECK) &&
            ((horseGameCheck->actor.params & 0xFF) == 5)) {
            this->stateFlags |= ENHORSE_FLAG_30;
            this->postDrawFunc = func_8088126C;
        }
    }

    if (this->playerControlled == false) {
        return;
    }

    obstaclePos = startPos;
    obstaclePos.y = this->actor.world.pos.y + 120.0f;

    if (movingFast == false) {
        obstaclePos.x += 276.0f * Math_SinS(this->actor.world.rot.y);
        obstaclePos.z += 276.0f * Math_CosS(this->actor.world.rot.y);
    } else {
        obstaclePos.x += 390.0f * Math_SinS(this->actor.world.rot.y);
        obstaclePos.z += 390.0f * Math_CosS(this->actor.world.rot.y);
    }

    obstacleTop = obstaclePos;
    obstacleTop.y = BgCheck_EntityRaycastFloor3(&play->colCtx, &obstacleFloor, &bgId, &obstaclePos);

    if (obstacleTop.y == BGCHECK_Y_MIN) {
        return;
    }

    behindObstacleHeight = obstacleTop.y - this->actor.world.pos.y;

    if (obstacleFloor == NULL) {
        return;
    }

    temp_f0 = COLPOLY_GET_NORMAL(obstacleFloor->normal.y);
    if ((temp_f0 < 0.81915206f) || SurfaceType_IsHorseBlocked(&play->colCtx, obstacleFloor, bgId) ||
        (SurfaceType_GetFloorType(&play->colCtx, obstacleFloor, bgId) == FLOOR_TYPE_7)) {
        if ((movingFast == true) && (this->playerControlled == true) && (this->action != ENHORSE_ACTION_STOPPING) &&
            (play->sceneId != SCENE_KOEPONARACE)) {
            this->stateFlags |= ENHORSE_FORCE_REVERSING;
            EnHorse_StartBraking(this, play);
        }
    } else if (behindObstacleHeight < -70.0f) {
        if ((movingFast == true) && (this->playerControlled == true) && (this->action != ENHORSE_ACTION_STOPPING) &&
            (play->sceneId != SCENE_KOEPONARACE)) {
            this->stateFlags |= ENHORSE_FORCE_REVERSING;
            EnHorse_StartBraking(this, play);
        }
    } else {
        temp_f0 = (this->actor.scale.y * 100.0f);

        if ((horseGameCheck == NULL) || (horseGameCheck->actor.id != ACTOR_EN_HORSE_GAME_CHECK) ||
            ((horseGameCheck->actor.params & 0xFF) != 5)) {
            if ((movingFast == false) && ((19.0f * temp_f0) < obstacleHeight) &&
                (obstacleHeight <= (40.0f * temp_f0))) {
                EnHorse_Stub1(this);
                this->postDrawFunc = EnHorse_LowJumpInit;
            } else if ((movingFast == true) &&
                       (((this->actor.speed < 13.8f) && ((19.0f * temp_f0) < obstacleHeight) &&
                         (obstacleHeight <= (72.0f * temp_f0))) ||
                        ((this->actor.speed > 13.8f) && (obstacleHeight <= (112.0f * temp_f0))))) {
                EnHorse_Stub2(this);
                this->postDrawFunc = EnHorse_HighJumpInit;
            }
        } else if (movingFast == false) {
            EnHorse_Stub1(this);
            this->postDrawFunc = EnHorse_LowJumpInit;
        } else if (movingFast == true) {
            EnHorse_Stub2(this);
            this->postDrawFunc = EnHorse_HighJumpInit;
        }
    }
}

void func_80886C00(EnHorse* this, PlayState* play) {
    Input* input = &play->state.input[this->unk_52C];

    if (((this->action == ENHORSE_ACTION_MOUNTED_WALK) || (this->action == ENHORSE_ACTION_MOUNTED_TROT) ||
         (this->action == ENHORSE_ACTION_MOUNTED_GALLOP)) &&
        (CHECK_BTN_ALL(input->press.button, BTN_A) || (AudioVoice_GetWord() == VOICE_WORD_ID_HIYA)) &&
        (play->interfaceCtx.aButtonDoActionDelayed == DO_ACTION_FASTER) && !(this->stateFlags & ENHORSE_BOOST) &&
        !(this->stateFlags & ENHORSE_FLAG_8) && !(this->stateFlags & ENHORSE_FLAG_9)) {
        if (this->numBoosts > 0) {
            Rumble_Request(0.0f, 180, 20, 100);
            this->stateFlags |= ENHORSE_BOOST;
            this->stateFlags |= ENHORSE_FIRST_BOOST_REGEN;
            this->stateFlags |= ENHORSE_FLAG_8;
            if (GameInteractor_Should(VB_CONSUME_EPONA_CARROT, true)) {
                this->numBoosts--;
            }
            this->boostTimer = 0;
            if (this->numBoosts == 0) {
                this->boostRegenTime = 140;
            } else if (this->type == HORSE_TYPE_EPONA) {
                if (this->stateFlags & ENHORSE_FIRST_BOOST_REGEN) {
                    this->boostRegenTime = 60;
                    this->stateFlags &= ~ENHORSE_FIRST_BOOST_REGEN;
                } else {
                    this->boostRegenTime = 8;
                }
            } else {
                this->boostRegenTime = 70;
            }
        } else if ((this->stateFlags & ENHORSE_DRAW) && (Rand_ZeroOne() < 0.1f)) {
            if (this->type == HORSE_TYPE_2) {
                Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_KID_HORSE_NEIGH);
            } else {
                Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_HORSE_NEIGH);
            }
        }
    }
}

void EnHorse_RegenBoost(EnHorse* this, PlayState* play) {
    if ((this->numBoosts < 6) && (this->numBoosts > 0)) {
        this->boostRegenTime--;
        this->boostTimer++;
        if (this->boostRegenTime <= 0) {
            this->numBoosts++;

            if (!EN_HORSE_CHECK_4(this)) {
                Audio_PlaySfx(NA_SE_SY_CARROT_RECOVER);
            }

            if (this->numBoosts < 6) {
                this->boostRegenTime = 11;
            }
        }
    } else if (this->numBoosts == 0) {
        this->boostRegenTime--;
        this->boostTimer++;
        if (this->boostRegenTime <= 0) {
            this->boostRegenTime = 0;
            this->numBoosts = 6;

            if (!EN_HORSE_CHECK_4(this)) {
                Audio_PlaySfx(NA_SE_SY_CARROT_RECOVER);
            }
        }
    }

    if (this->boostTimer == 8) {
        if ((Rand_ZeroOne() < 0.25f) && (this->stateFlags & ENHORSE_DRAW)) {
            if (this->type == HORSE_TYPE_2) {
                Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_KID_HORSE_NEIGH);
            } else {
                Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_HORSE_NEIGH);
            }
        }
    }

    play->interfaceCtx.numHorseBoosts = this->numBoosts;
}

void EnHorse_UpdatePlayerDir(EnHorse* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 angle = Actor_WorldYawTowardActor(&this->actor, &player->actor) - this->actor.world.rot.y;
    f32 s = Math_SinS(angle);
    f32 c = Math_CosS(angle);

    if (s > 0.8660254f) { // sin(60 degrees)
        this->playerDir = PLAYER_DIR_SIDE_L;
    } else if (s < -0.8660254f) { // -sin(60 degrees)
        this->playerDir = PLAYER_DIR_SIDE_R;
    } else if (c > 0.0f) {
        if (s > 0.0f) {
            this->playerDir = PLAYER_DIR_FRONT_L;
        } else {
            this->playerDir = PLAYER_DIR_FRONT_R;
        }
    } else if (s > 0.0f) {
        this->playerDir = PLAYER_DIR_BACK_L;
    } else {
        this->playerDir = PLAYER_DIR_BACK_R;
    }
}

void EnHorse_TiltBody(EnHorse* this, PlayState* play) {
    f32 speed;
    f32 rollDiff;
    s32 targetRoll;
    s16 turnVel;

    speed = this->actor.speed / this->boostSpeed;
    turnVel = this->actor.shape.rot.y - this->lastYaw;
    targetRoll = -TRUNCF_BINANG((2730.0f * speed) * (turnVel / 960.00006f));
    rollDiff = targetRoll - this->actor.world.rot.z;

    if (fabsf(targetRoll) < 100.0f) {
        this->actor.world.rot.z = 0;
    } else if (fabsf(rollDiff) < 100.0f) {
        this->actor.world.rot.z = targetRoll;
    } else if (rollDiff > 0.0f) {
        this->actor.world.rot.z += 0x64;
    } else {
        this->actor.world.rot.z -= 0x64;
    }

    this->actor.shape.rot.z = this->actor.world.rot.z;
}

s32 EnHorse_UpdateConveyors(EnHorse* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 conveyorDir;

    if ((this->actor.floorPoly == NULL) || (&this->actor != player->rideActor) ||
        (SurfaceType_GetConveyorSpeed(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) ==
         CONVEYOR_SPEED_DISABLED)) {
        return false;
    }

    conveyorDir = SurfaceType_GetConveyorDirection(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
    conveyorDir = CONVEYOR_DIRECTION_TO_BINANG(conveyorDir) - this->actor.world.rot.y;
    if (conveyorDir > 0x640) {
        this->actor.world.rot.y += 0x640;
    } else if (conveyorDir < -0x640) {
        this->actor.world.rot.y -= 0x640;
    } else {
        this->actor.world.rot.y += conveyorDir;
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
    return true;
}

s32 EnHorse_RandInt(f32 arg0) {
    return Rand_ZeroOne() * arg0;
}

static EnHorseActionFunc sActionFuncs[] = {
    EnHorse_Frozen,                 // ENHORSE_ACTION_FROZEN
    EnHorse_Inactive,               // ENHORSE_ACTION_INACTIVE
    EnHorse_Idle,                   // ENHORSE_ACTION_IDLE
    EnHorse_FollowPlayer,           // ENHORSE_ACTION_FOLLOW_PLAYER
    EnHorse_UpdateIngoRace,         // ENHORSE_ACTION_INGO_RACE
    func_808819D8,                  // ENHORSE_ACTION_5
    func_80881398,                  // ENHORSE_ACTION_6
    EnHorse_MountedIdle,            // ENHORSE_ACTION_MOUNTED_IDLE
    EnHorse_MountedIdleWhinnying,   // ENHORSE_ACTION_MOUNTED_IDLE_WHINNYING
    EnHorse_MountedTurn,            // ENHORSE_ACTION_MOUNTED_TURN
    EnHorse_MountedWalk,            // ENHORSE_ACTION_MOUNTED_WALK
    EnHorse_MountedTrot,            // ENHORSE_ACTION_MOUNTED_TROT
    EnHorse_MountedGallop,          // ENHORSE_ACTION_MOUNTED_GALLOP
    EnHorse_MountedRearing,         // ENHORSE_ACTION_MOUNTED_REARING
    EnHorse_Stopping,               // ENHORSE_ACTION_STOPPING
    EnHorse_Reverse,                // ENHORSE_ACTION_REVERSE
    EnHorse_LowJump,                // ENHORSE_ACTION_LOW_JUMP
    EnHorse_HighJump,               // ENHORSE_ACTION_HIGH_JUMP
    EnHorse_CutsceneUpdate,         // ENHORSE_ACTION_CS_UPDATE
    EnHorse_UpdateHorsebackArchery, // ENHORSE_ACTION_HBA
    EnHorse_FleePlayer,             // ENHORSE_ACTION_FLEE_PLAYER
    func_80884718,                  // ENHORSE_ACTION_21
    func_8087CA04,                  // ENHORSE_ACTION_22
    func_808848C8,                  // ENHORSE_ACTION_23
    func_80884A40,                  // ENHORSE_ACTION_24
    func_80884E0C,                  // ENHORSE_ACTION_25
};
void EnHorse_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnHorse* this = (EnHorse*)thisx;
    Vec3f dustAcc = { 0.0f, 0.0f, 0.0f };
    Vec3f dustVel = { 0.0f, 1.0f, 0.0f };
    Player* player = GET_PLAYER(play);

    if (this->type == HORSE_TYPE_2) {
        Actor_SetScale(&this->actor, 0.00648f);
    } else if (this->type == HORSE_TYPE_DONKEY) {
        Actor_SetScale(&this->actor, 0.008f);
    } else {
        Actor_SetScale(&this->actor, 0.01f);
    }

    this->lastYaw = thisx->shape.rot.y;
    EnHorse_UpdateStick(this, play);
    EnHorse_UpdatePlayerDir(this, play);

    if (!(this->stateFlags & ENHORSE_INACTIVE)) {
        EnHorse_MountDismount(this, play);
    }

    if (this->stateFlags & ENHORSE_FLAG_19) {
        if ((this->stateFlags & ENHORSE_FLAG_20) && (this->inRace == true)) {
            this->stateFlags &= ~ENHORSE_FLAG_20;
            EnHorse_StartRearing(this);
        } else if (!(this->stateFlags & ENHORSE_FLAG_20) && (this->stateFlags & ENHORSE_FLAG_21) &&
                   (this->action != ENHORSE_ACTION_MOUNTED_REARING) && (this->inRace == true)) {
            this->stateFlags &= ~ENHORSE_FLAG_21;
            EnHorse_StartRearing(this);
        }
    }

    sActionFuncs[this->action](this, play);

    this->stateFlags &= ~ENHORSE_OBSTACLE;
    this->unk_3EC = thisx->world.rot.y;
    if ((this->animIndex == ENHORSE_ANIM_STOPPING) || (this->animIndex == ENHORSE_ANIM_REARING)) {
        this->skin.skelAnime.jointTable[LIMB_ROOT_POS].y += 0x154;
    }

    this->curFrame = this->skin.skelAnime.curFrame;
    this->lastPos = thisx->world.pos;

    if (!(this->stateFlags & ENHORSE_INACTIVE)) {
        if ((this->action == ENHORSE_ACTION_MOUNTED_GALLOP) || (this->action == ENHORSE_ACTION_MOUNTED_TROT) ||
            (this->action == ENHORSE_ACTION_MOUNTED_WALK)) {
            func_80886C00(this, play);
        }

        if (this->playerControlled == true) {
            EnHorse_RegenBoost(this, play);
        }

        if (CutsceneManager_GetCurrentCsId() != CS_ID_NONE) {
            thisx->speed = 0.0f;
        }

        if (this->action != ENHORSE_ACTION_25) {
            Actor_MoveWithGravity(&this->actor);
        }

        if (this->rider != NULL) {
            if ((this->action == ENHORSE_ACTION_INGO_RACE) || (this->action == ENHORSE_ACTION_5) ||
                (this->action == ENHORSE_ACTION_25)) {
                this->rider->actor.world.pos.x = thisx->world.pos.x;
                this->rider->actor.world.pos.y = thisx->world.pos.y + 10.0f;
                this->rider->actor.world.pos.z = thisx->world.pos.z;
                this->rider->actor.shape.rot.x = thisx->shape.rot.x;
                this->rider->actor.shape.rot.y = thisx->shape.rot.y;
            } else if (this->action == ENHORSE_ACTION_6) {
                EnIn* in = this->rider;
                s16 jnt = in->jointTable[LIMB_ROOT_POS].y;

                in->actor.world.pos.x = this->riderPos.x;
                in->actor.world.pos.y = this->riderPos.y - (jnt * 0.01f * this->unk_528 * 0.01f);
                in->actor.world.pos.z = this->riderPos.z;
                in->actor.shape.rot.x = thisx->shape.rot.x;
                in->actor.shape.rot.y = thisx->shape.rot.y;
            }
        }

        if (this->colliderJntSph.elements[0].base.ocElemFlags & OCELEM_HIT) {
            if (thisx->speed > 10.0f) {
                thisx->speed -= 1.0f;
            }
        }

        if ((this->colliderJntSph.base.acFlags & AC_HIT) && (this->stateFlags & ENHORSE_DRAW)) {
            if (this->type == HORSE_TYPE_2) {
                Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_KID_HORSE_NEIGH);
            } else {
                Audio_PlaySfx_AtPos(&this->unk_218, NA_SE_EV_HORSE_NEIGH);
            }
        }

        if ((this->action != ENHORSE_ACTION_INGO_RACE) && (this->action != ENHORSE_ACTION_5) &&
            (this->action != ENHORSE_ACTION_6)) {
            EnHorse_TiltBody(this, play);
        }

        if ((this->playerControlled == false) && (this->unk_1EC & 8)) {
            if ((this->colliderJntSph.elements[0].base.ocElemFlags & OCELEM_HIT) &&
                (this->colliderJntSph.base.oc->id == ACTOR_EN_IN)) {
                func_80884868(this);
            }

            if ((this->colliderCylinder1.base.ocFlags1 & OC1_HIT) &&
                (this->colliderCylinder1.base.oc->id == ACTOR_EN_IN)) {
                func_80884868(this);
            }

            if ((this->colliderCylinder2.base.ocFlags1 & OC1_HIT) &&
                (this->colliderCylinder2.base.oc->id == ACTOR_EN_IN)) {
                func_80884868(this);
            }
        }

        Collider_UpdateCylinder(&this->actor, &this->colliderCylinder1);
        Collider_UpdateCylinder(&this->actor, &this->colliderCylinder2);

        if (this->type == HORSE_TYPE_2) {
            this->colliderCylinder1.dim.pos.x =
                TRUNCF_BINANG(Math_SinS(thisx->shape.rot.y) * 11.0f) + this->colliderCylinder1.dim.pos.x;
            this->colliderCylinder1.dim.pos.z =
                TRUNCF_BINANG(Math_CosS(thisx->shape.rot.y) * 11.0f) + this->colliderCylinder1.dim.pos.z;
            this->colliderCylinder2.dim.pos.x =
                TRUNCF_BINANG(Math_SinS(thisx->shape.rot.y) * -18.0f) + this->colliderCylinder2.dim.pos.x;
            this->colliderCylinder2.dim.pos.z =
                TRUNCF_BINANG(Math_CosS(thisx->shape.rot.y) * -18.0f) + this->colliderCylinder2.dim.pos.z;
        } else {
            this->colliderCylinder1.dim.pos.x =
                TRUNCF_BINANG(Math_SinS(thisx->shape.rot.y) * 6.6000004f) + this->colliderCylinder1.dim.pos.x;
            this->colliderCylinder1.dim.pos.z =
                TRUNCF_BINANG(Math_CosS(thisx->shape.rot.y) * 6.6000004f) + this->colliderCylinder1.dim.pos.z;
            this->colliderCylinder2.dim.pos.x =
                TRUNCF_BINANG(Math_SinS(thisx->shape.rot.y) * -10.8f) + this->colliderCylinder2.dim.pos.x;
            this->colliderCylinder2.dim.pos.z =
                TRUNCF_BINANG(Math_CosS(thisx->shape.rot.y) * -10.8f) + this->colliderCylinder2.dim.pos.z;
        }

        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderCylinder1.base);
        if (!(this->stateFlags & ENHORSE_JUMPING) && !(this->unk_1EC & 0x20)) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderCylinder1.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderCylinder2.base);
        } else {
            this->unk_1EC &= ~0x20;
        }

        if (this->unk_1EC & 0x100) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderCylinder1.base);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderCylinder2.base);
        }

        if ((player->stateFlags1 & PLAYER_STATE1_1) && (player->rideActor != NULL)) {
            EnHorse_UpdateConveyors(this, play);
        }

        EnHorse_UpdateBgCheckInfo(this, play);
        EnHorse_CheckFloors(this, play);
        if (thisx->world.pos.y < this->yFront) {
            if (thisx->world.pos.y < this->yBack) {
                if (this->yBack < this->yFront) {
                    thisx->world.pos.y = this->yBack;
                } else {
                    thisx->world.pos.y = this->yFront;
                }
            }
        }

        thisx->focus.pos = thisx->world.pos;
        thisx->focus.pos.y += 70.0f;

        if ((Rand_ZeroOne() < 0.025f) && (this->blinkTimer == 0)) {
            this->blinkTimer++;
        } else if (this->blinkTimer > 0) {
            this->blinkTimer++;
            if (this->blinkTimer > 3) {
                this->blinkTimer = 0;
            }
        }

        if ((thisx->speed == 0.0f) && !(this->stateFlags & ENHORSE_FLAG_19)) {
            thisx->colChkInfo.mass = MASS_IMMOVABLE;
        } else {
            thisx->colChkInfo.mass = MASS_HEAVY;
        }

        if (thisx->speed >= 5.0f) {
            this->colliderCylinder1.base.atFlags |= AT_ON;
        } else {
            this->colliderCylinder1.base.atFlags &= ~AT_ON;
        }

        if (this->dustFlags & 1) {
            this->dustFlags &= ~1;
            func_800B12F0(play, &this->frontRightHoof, &dustVel, &dustAcc, EnHorse_RandInt(100.0f) + 200,
                          EnHorse_RandInt(10.0f) + 30, EnHorse_RandInt(20.0f) + 30);
        } else if (this->dustFlags & 2) {
            this->dustFlags &= ~2;
            func_800B12F0(play, &this->frontLeftHoof, &dustVel, &dustAcc, EnHorse_RandInt(100.0f) + 200,
                          EnHorse_RandInt(10.0f) + 30, EnHorse_RandInt(20.0f) + 30);
        } else if (this->dustFlags & 4) {
            this->dustFlags &= ~4;
            func_800B12F0(play, &this->backRightHoof, &dustVel, &dustAcc, EnHorse_RandInt(100.0f) + 200,
                          EnHorse_RandInt(10.0f) + 30, EnHorse_RandInt(20.0f) + 30);
        } else if (this->dustFlags & 8) {
            this->dustFlags &= ~8;
            func_800B12F0(play, &this->backLeftHoof, &dustVel, &dustAcc, EnHorse_RandInt(100.0f) + 200,
                          EnHorse_RandInt(10.0f) + 30, EnHorse_RandInt(20.0f) + 30);
        }
        this->stateFlags &= ~ENHORSE_DRAW;
    }
}

s32 EnHorse_PlayerDirToMountSide(EnHorse* this, PlayState* play, Player* player) {
    if (this->playerDir == PLAYER_DIR_SIDE_L) {
        return -1;
    }

    if (this->playerDir == PLAYER_DIR_SIDE_R) {
        return 1;
    }

    return 0;
}

s32 EnHorse_MountSideCheck(EnHorse* this, PlayState* play, Player* player) {
    s32 mountSide;

    if (Actor_WorldDistXZToActor(&this->actor, &player->actor) > 75.0f) {
        return 0;
    }

    if (fabsf(this->actor.world.pos.y - player->actor.world.pos.y) > 30.0f) {
        return 0;
    }

    if (Math_CosS(Actor_WorldYawTowardActor(&player->actor, &this->actor) - player->actor.world.rot.y) <
        0.17364818f) { // cos(80 degrees)
        return 0;
    }

    mountSide = EnHorse_PlayerDirToMountSide(this, play, player);
    if (mountSide == -1) {
        return -1;
    }

    if (mountSide == 1) {
        return 1;
    }

    return 0;
}

s32 EnHorse_GetMountSide(EnHorse* this, PlayState* play) {
    if (this->action != ENHORSE_ACTION_IDLE) {
        return 0;
    }

    if ((this->animIndex != ENHORSE_ANIM_IDLE) && (this->animIndex != ENHORSE_ANIM_WHINNY)) {
        return 0;
    }

    return EnHorse_MountSideCheck(this, play, GET_PLAYER(play));
}

void EnHorse_RandomOffset(Vec3f* src, f32 dist, Vec3f* dst) {
    dst->x = ((Rand_ZeroOne() * (2.0f * dist)) + src->x) - dist;
    dst->y = ((Rand_ZeroOne() * (2.0f * dist)) + src->y) - dist;
    dst->z = ((Rand_ZeroOne() * (2.0f * dist)) + src->z) - dist;
}

void EnHorse_PostDraw(Actor* thisx, PlayState* play, Skin* skin) {
    s32 pad;
    EnHorse* this = (EnHorse*)thisx;
    Vec3f sp7C = { 0.0f, 0.0f, 0.0f };
    Vec3f hoofOffset = { 5.0f, -4.0f, 5.0f };
    Vec3f sp64;
    f32 curFrame = this->skin.skelAnime.curFrame;
    Vec3f center;
    Vec3f newCenter;
    s32 i;
    Vec3f sp38;
    f32 sp34;

    if (!(this->stateFlags & ENHORSE_CALC_RIDER_POS)) {
        if (this->type == HORSE_TYPE_2) {
            Skin_GetVertexPos(skin, 5, 120, &this->riderPos);
            this->riderPos.y += 13.0f;
        } else {
            Vec3f riderOffset = { 600.0f, -1670.0f, 0.0f };

            Skin_GetLimbPos(skin, 30, &riderOffset, &this->riderPos);
        }
        this->riderPos.x -= this->actor.world.pos.x;
        this->riderPos.y -= this->actor.world.pos.y;
        this->riderPos.z -= this->actor.world.pos.z;
    } else {
        this->stateFlags &= ~ENHORSE_CALC_RIDER_POS;
    }

    if (this->type == HORSE_TYPE_2) {
        Skin_GetLimbPos(skin, 13, &sp7C, &sp38);
    } else {
        Skin_GetLimbPos(skin, 13, &sp7C, &sp38);
    }

    SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &sp38, &this->unk_218, &sp34);

    if ((this->animIndex == ENHORSE_ANIM_IDLE) && (this->action != ENHORSE_ACTION_FROZEN) &&
        (((curFrame > 40.0f) && (curFrame < 45.0f) && (this->type == 0)) ||
         ((curFrame > 28.0f) && (curFrame < 33.0f) && (this->type == 1)))) {
        if (Rand_ZeroOne() < 0.02f) {
            this->dustFlags |= 1;
            Skin_GetLimbPos(skin, 28, &hoofOffset, &this->frontRightHoof);
            this->frontRightHoof.y -= 5.0f;
        }
    } else {
        if (this->action == ENHORSE_ACTION_STOPPING) {
            if (((curFrame > 10.0f) && (curFrame < 13.0f)) || ((curFrame > 25.0f) && (curFrame < 33.0f))) {
                if (Rand_ZeroOne() < 0.02f) {
                    this->dustFlags |= 2;
                    if (this->type == HORSE_TYPE_2) {
                        Skin_GetLimbPos(skin, 20, &hoofOffset, &sp64);
                    } else {
                        Skin_GetLimbPos(skin, 20, &hoofOffset, &sp64);
                    }
                    EnHorse_RandomOffset(&sp64, 10.0f, &this->frontLeftHoof);
                }

                if (Rand_ZeroOne() < 0.02f) {
                    this->dustFlags |= 1;
                    if (this->type == HORSE_TYPE_2) {
                        Skin_GetLimbPos(skin, 28, &hoofOffset, &sp64);
                    } else {
                        Skin_GetLimbPos(skin, 28, &hoofOffset, &sp64);
                    }
                    EnHorse_RandomOffset(&sp64, 10.0f, &this->frontRightHoof);
                }
            }

            if (((curFrame > 6.0f) && (curFrame < 10.0f)) || ((curFrame > 23.0f) && (curFrame < 29.0f))) {
                if (Rand_ZeroOne() < 0.02f) {
                    this->dustFlags |= 8;
                    if (this->type == HORSE_TYPE_2) {
                        Skin_GetLimbPos(skin, 36, &hoofOffset, &sp64);
                    } else {
                        Skin_GetLimbPos(skin, 37, &hoofOffset, &sp64);
                    }
                    EnHorse_RandomOffset(&sp64, 10.0f, &this->backLeftHoof);
                }
            }

            if ((((curFrame > 7.0f) && (curFrame < 14.0f)) || ((curFrame > 26.0f) && (curFrame < 30.0f))) &&
                (Rand_ZeroOne() < 0.02f)) {
                this->dustFlags |= 4;
                if (this->type == HORSE_TYPE_2) {
                    Skin_GetLimbPos(skin, 44, &hoofOffset, &sp64);
                } else {
                    Skin_GetLimbPos(skin, 45, &hoofOffset, &sp64);
                }
                EnHorse_RandomOffset(&sp64, 10.0f, &this->backRightHoof);
            }
        } else if (this->animIndex == ENHORSE_ANIM_GALLOP) {
            if ((curFrame > 14.0f) && (curFrame < 16.0f) && (Rand_ZeroOne() < 0.02f)) {
                this->dustFlags |= 1;
                if (this->type == HORSE_TYPE_2) {
                    Skin_GetLimbPos(skin, 28, &hoofOffset, &sp64);
                } else {
                    Skin_GetLimbPos(skin, 28, &hoofOffset, &sp64);
                }
                EnHorse_RandomOffset(&sp64, 5.0f, &this->frontRightHoof);
            } else if ((curFrame > 8.0f) && (curFrame < 10.0f) && (Rand_ZeroOne() < 0.02f)) {
                this->dustFlags |= 2;
                if (this->type == HORSE_TYPE_2) {
                    Skin_GetLimbPos(skin, 20, &hoofOffset, &sp64);
                } else {
                    Skin_GetLimbPos(skin, 20, &hoofOffset, &sp64);
                }
                EnHorse_RandomOffset(&sp64, 10.0f, &this->frontLeftHoof);
            } else if ((curFrame > 1.0f) && (curFrame < 3.0f) && (Rand_ZeroOne() < 0.02f)) {
                this->dustFlags |= 4;
                if (this->type == HORSE_TYPE_2) {
                    Skin_GetLimbPos(skin, 44, &hoofOffset, &sp64);
                } else {
                    Skin_GetLimbPos(skin, 45, &hoofOffset, &sp64);
                }
                EnHorse_RandomOffset(&sp64, 10.0f, &this->backRightHoof);
            } else if ((curFrame > 26.0f) && (curFrame < 28.0f) && (Rand_ZeroOne() < 0.02f)) {
                this->dustFlags |= 8;
                if (this->type == HORSE_TYPE_2) {
                    Skin_GetLimbPos(skin, 36, &hoofOffset, &sp64);
                } else {
                    Skin_GetLimbPos(skin, 37, &hoofOffset, &sp64);
                }
                EnHorse_RandomOffset(&sp64, 10.0f, &this->backLeftHoof);
            }
        } else if ((this->action == ENHORSE_ACTION_LOW_JUMP) && (curFrame > 6.0f) &&
                   (Rand_ZeroOne() < (1.0f - ((curFrame - 6.0f) * (1.0f / 17.0f))))) {
            if (Rand_ZeroOne() < 0.05f) {
                this->dustFlags |= 8;
                if (this->type == HORSE_TYPE_2) {
                    Skin_GetLimbPos(skin, 36, &hoofOffset, &sp64);
                } else {
                    Skin_GetLimbPos(skin, 37, &hoofOffset, &sp64);
                }
                EnHorse_RandomOffset(&sp64, 10.0f, &this->backLeftHoof);
            }
            if (Rand_ZeroOne() < 0.02f) {
                this->dustFlags |= 4;
                if (this->type == HORSE_TYPE_2) {
                    Skin_GetLimbPos(skin, 44, &hoofOffset, &sp64);
                } else {
                    Skin_GetLimbPos(skin, 45, &hoofOffset, &sp64);
                }
                EnHorse_RandomOffset(&sp64, 10.0f, &this->backRightHoof);
            }
        } else if ((this->action == ENHORSE_ACTION_HIGH_JUMP) && (curFrame > 5.0f)) {
            if (Rand_ZeroOne() < (1.0f - ((curFrame - 5.0f) * 0.04f))) {
                if (Rand_ZeroOne() < 0.05f) {
                    this->dustFlags |= 8;
                    if (this->type == HORSE_TYPE_2) {
                        Skin_GetLimbPos(skin, 36, &hoofOffset, &sp64);
                    } else {
                        Skin_GetLimbPos(skin, 37, &hoofOffset, &sp64);
                    }
                    EnHorse_RandomOffset(&sp64, 10.0f, &this->backLeftHoof);
                }

                if (Rand_ZeroOne() < 0.02f) {
                    this->dustFlags |= 4;
                    if (this->type == HORSE_TYPE_2) {
                        Skin_GetLimbPos(skin, 44, &hoofOffset, &sp64);
                    } else {
                        Skin_GetLimbPos(skin, 45, &hoofOffset, &sp64);
                    }
                    EnHorse_RandomOffset(&sp64, 10.0f, &this->backRightHoof);
                }
            }
        }
    }

    for (i = 0; i < this->colliderJntSph.count; i++) {
        center.x = this->colliderJntSph.elements[i].dim.modelSphere.center.x;
        center.y = this->colliderJntSph.elements[i].dim.modelSphere.center.y;
        center.z = this->colliderJntSph.elements[i].dim.modelSphere.center.z;

        Skin_GetLimbPos(skin, this->colliderJntSph.elements[i].dim.limb, &center, &newCenter);

        this->colliderJntSph.elements[i].dim.worldSphere.center.x = newCenter.x;
        this->colliderJntSph.elements[i].dim.worldSphere.center.y = newCenter.y;
        this->colliderJntSph.elements[i].dim.worldSphere.center.z = newCenter.z;

        this->colliderJntSph.elements[i].dim.worldSphere.radius =
            this->colliderJntSph.elements[i].dim.modelSphere.radius * this->colliderJntSph.elements[i].dim.scale;
    }

    //! @bug Setting colliders in a draw function allows for duplicate entries to be added to their respective lists
    //! under certain conditions, like when pausing and unpausing the game.
    //! Actors will draw for a couple of frames between the pauses, but some important logic updates will not occur.
    //! In the case of OC, this can cause unwanted effects such as a very large amount of displacement being applied to
    //! a colliding actor.
    if (!(this->stateFlags & ENHORSE_JUMPING)) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderJntSph.base);
    }
}

s32 EnHorse_OverrideLimbDraw(Actor* thisx, PlayState* play, s32 limbIndex, Skin* skin) {
    static TexturePtr D_80889204[] = {
        gEponaEyeOpenTex,
        gEponaEyeHalfTex,
        gEponaEyeClosedTex,
    };
    static u8 D_80889210[] = { 0, 1, 2, 1 };
    EnHorse* this = (EnHorse*)thisx;
    s32 drawOriginalLimb = true;

    OPEN_DISPS(play->state.gfxCtx);

    if ((limbIndex != 13) || (this->type != HORSE_TYPE_EPONA)) {
        if ((limbIndex == 13) && (this->type == HORSE_TYPE_2)) {
            u8 idx = D_80889210[this->blinkTimer];

            gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_80889204[idx]));
        } else if ((this->type == HORSE_TYPE_HNI) && (this->stateFlags & ENHORSE_FLAG_18) && (limbIndex == 30)) {
            drawOriginalLimb = false;
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);

    return drawOriginalLimb;
}

s32 func_80888D18(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    Vec3f sp1C = { -98.0f, -1454.0f, 0.0f };
    EnHorse* this = (EnHorse*)thisx;

    if (limbIndex == 3) {
        Matrix_MultVec3f(&sp1C, &this->riderPos);
    }
    return false;
}

void EnHorse_Draw(Actor* thisx, PlayState* play) {
    EnHorse* this = (EnHorse*)thisx;

    if (!(this->stateFlags & ENHORSE_INACTIVE) && (this->actor.update != func_8087D540)) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        this->stateFlags |= ENHORSE_DRAW;
        if (!(this->unk_1EC & 1)) {
            if (this->stateFlags & ENHORSE_JUMPING) {
                func_80138258(&this->actor, play, &this->skin, EnHorse_PostDraw, EnHorse_OverrideLimbDraw, false);
            } else {
                func_80138258(&this->actor, play, &this->skin, EnHorse_PostDraw, EnHorse_OverrideLimbDraw, true);
            }
        } else {
            if (this->stateFlags & ENHORSE_JUMPING) {
                this->skin.skelAnime.jointTable[LIMB_ROOT_POS].x = 0;
                this->skin.skelAnime.jointTable[LIMB_ROOT_POS].y = 0;
                this->skin.skelAnime.jointTable[LIMB_ROOT_POS].z = 0;
            }
            SkelAnime_DrawFlexOpa(play, this->skin.skelAnime.skeleton, this->skin.skelAnime.jointTable,
                                  this->skin.skelAnime.dListCount, func_80888D18, NULL, &this->actor);
        }

        if (this->postDrawFunc != NULL) {
            this->postDrawFunc(this, play);
        }
    }
}
