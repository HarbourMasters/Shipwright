/*
 * File: z_en_insect.c
 * Overlay: ovl_En_Insect
 * Description: Bugs
 */

#include "z_en_insect.h"
#include "vt.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS 0

void EnInsect_Init(Actor* thisx, PlayState* play);
void EnInsect_Destroy(Actor* thisx, PlayState* play);
void EnInsect_Update(Actor* thisx, PlayState* play);
void EnInsect_Draw(Actor* thisx, PlayState* play);
void EnInsect_Reset(void);

void func_80A7C3A0(EnInsect* this);
void func_80A7C3F4(EnInsect* this, PlayState* play);
void func_80A7C598(EnInsect* this);
void func_80A7C5EC(EnInsect* this, PlayState* play);
void func_80A7C818(EnInsect* this);
void func_80A7C86C(EnInsect* this, PlayState* play);
void func_80A7CAD0(EnInsect* this, PlayState* play);
void func_80A7CBC8(EnInsect* this);
void func_80A7CC3C(EnInsect* this, PlayState* play);
void func_80A7CE60(EnInsect* this);
void func_80A7CEC0(EnInsect* this, PlayState* play);
void func_80A7D1F4(EnInsect* this);
void func_80A7D26C(EnInsect* this, PlayState* play);
void func_80A7D39C(EnInsect* this);
void func_80A7D460(EnInsect* this, PlayState* play);

f32 D_80A7DEB0 = 0.0f;
s16 D_80A7DEB4 = 0;
s16 D_80A7DEB8 = 0;

const ActorInit En_Insect_InitVars = {
    ACTOR_EN_INSECT,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnInsect),
    (ActorFunc)EnInsect_Init,
    (ActorFunc)EnInsect_Destroy,
    (ActorFunc)EnInsect_Update,
    (ActorFunc)EnInsect_Draw,
    EnInsect_Reset,
};

static ColliderJntSphElementInit sColliderItemInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 5 }, 100 },
    },
};

static ColliderJntSphInit sColliderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_1,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    sColliderItemInit,
};

static u16 D_80A7DF10[] = { 0, 5, 7, 7 };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 700, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 20, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 600, ICHAIN_STOP),
};

void func_80A7BE20(EnInsect* this) {
    this->unk_314 = D_80A7DF10[this->actor.params & 3];
}

f32 EnInsect_XZDistanceSquared(Vec3f* v1, Vec3f* v2) {
    return SQ(v1->x - v2->x) + SQ(v1->z - v2->z);
}

s32 EnInsect_InBottleRange(EnInsect* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    Vec3f pos;

    if (this->actor.xzDistToPlayer < 32.0f) {
        pos.x = Math_SinS(this->actor.yawTowardsPlayer + 0x8000) * 16.0f + player->actor.world.pos.x;
        pos.y = player->actor.world.pos.y;
        pos.z = Math_CosS(this->actor.yawTowardsPlayer + 0x8000) * 16.0f + player->actor.world.pos.z;

        //! @bug: this check is superfluous: it is automatically satisfied if the coarse check is satisfied. It may have
        //! been intended to check the actor is in front of Player, but yawTowardsPlayer does not depend on Player's
        //! world rotation.
        if (EnInsect_XZDistanceSquared(&pos, &this->actor.world.pos) <= SQ(20.0f)) {
            return true;
        }
    }

    return false;
}

void func_80A7BF58(EnInsect* this) {
    Animation_Change(&this->skelAnime, &gBugCrawlAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, 0.0f);
}

/**
 * Find the nearest soft dirt patch within 6400 units in the xz plane and the current room
 *
 * @return 1 if one was found, 0 otherwise
 */
s32 EnInsect_FoundNearbySoil(EnInsect* this, PlayState* play) {
    Actor* currentActor;
    f32 currentDistance;
    f32 bestDistance;
    s32 ret;

    ret = 0;
    currentActor = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;
    bestDistance = 6400.0f;
    this->soilActor = NULL;

    while (currentActor != NULL) {
        if (currentActor->id == ACTOR_OBJ_MAKEKINSUTA) {
            currentDistance = Math3D_Dist2DSq(this->actor.world.pos.x, this->actor.world.pos.z,
                                              currentActor->world.pos.x, currentActor->world.pos.z);

            if (currentDistance < bestDistance && currentActor->room == this->actor.room) {
                ret = 1;
                bestDistance = currentDistance;
                this->soilActor = (ObjMakekinsuta*)currentActor;
            }
        }
        currentActor = currentActor->next;
    }
    return ret;
}

void func_80A7C058(EnInsect* this) {
    if (this->unk_31E > 0) {
        this->unk_31E--;
        return;
    }

    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MUSI_WALK);

    this->unk_31E = 3.0f / CLAMP_MIN(this->skelAnime.playSpeed, 0.1f);
    if (this->unk_31E < 2) {
        this->unk_31E = 2;
    }
}

void EnInsect_Init(Actor* thisx, PlayState* play2) {
    EnInsect* this = (EnInsect*)thisx;
    PlayState* play = play2;
    f32 rand;
    s16 temp_s2;
    s32 count;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    func_80A7BE20(this);

    temp_s2 = this->actor.params & 3;

    SkelAnime_Init(play, &this->skelAnime, &gBugSkel, &gBugCrawlAnim, this->jointTable, this->morphTable, 24);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sColliderInit, &this->colliderItem);

    this->actor.colChkInfo.mass = 30;

    if (this->unk_314 & 1) {
        this->actor.gravity = -0.2f;
        this->actor.minVelocityY = -2.0f;
    }

    if (this->unk_314 & 4) {
        this->unk_31C = Rand_S16Offset(200, 40);
        this->actor.flags |= ACTOR_FLAG_UPDATE_WHILE_CULLED;
    }

    if (temp_s2 == 2 || temp_s2 == 3) {
        if (EnInsect_FoundNearbySoil(this, play)) {
            this->unk_314 |= 0x10;
            D_80A7DEB0 = 0.0f;
        }

        if (temp_s2 == 2) {
            this->actor.world.rot.z = 0;
            this->actor.shape.rot.z = this->actor.world.rot.z;

            for (count = 0; count < 2; count++) {
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_INSECT, this->actor.world.pos.x,
                            this->actor.world.pos.y, this->actor.world.pos.z, this->actor.shape.rot.x,
                            this->actor.shape.rot.y, this->actor.shape.rot.z, 3, true);
            }
        }

        func_80A7D39C(this);

        // For bugs that aren't linked to a soil patch, we remove the "short lived" flag to prevent them from despawning
        // And exit early to not increment the "bugs dropped count"
        if (CVarGetInteger("gNoBugsDespawn", 0) && this->soilActor == NULL) {
            this->unk_314 &= ~4;
            return;
        }

        D_80A7DEB8++;
    } else {
        rand = Rand_ZeroOne();

        if (rand < 0.3f) {
            func_80A7C3A0(this);
        } else if (rand < 0.4f) {
            func_80A7C598(this);
        } else {
            func_80A7C818(this);
        }
    }
}

void EnInsect_Destroy(Actor* thisx, PlayState* play) {
    s16 temp_v0;
    EnInsect* this = (EnInsect*)thisx;

    temp_v0 = this->actor.params & 3;
    Collider_DestroyJntSph(play, &this->collider);
    if ((temp_v0 == 2 || temp_v0 == 3) && D_80A7DEB8 > 0) {
        D_80A7DEB8--;
    }

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

void func_80A7C3A0(EnInsect* this) {
    this->unk_31A = Rand_S16Offset(5, 35);
    func_80A7BF58(this);
    this->actionFunc = func_80A7C3F4;
    this->unk_314 |= 0x100;
}

void func_80A7C3F4(EnInsect* this, PlayState* play) {
    s32 pad[2];
    s16 sp2E;
    f32 playSpeed;

    sp2E = this->actor.params & 3;

    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 0.1f, 0.5f, 0.0f);

    playSpeed = (Rand_ZeroOne() * 0.8f) + (this->actor.speedXZ * 1.2f);
    this->skelAnime.playSpeed = CLAMP(playSpeed, 0.0f, 1.9f);

    SkelAnime_Update(&this->skelAnime);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if (this->unk_31A <= 0) {
        func_80A7C598(this);
    }

    if (((this->unk_314 & 4) && this->unk_31C <= 0) ||
        ((sp2E == 2 || sp2E == 3) && (this->unk_314 & 1) && (this->actor.bgCheckFlags & 1) && D_80A7DEB8 >= 4)) {
        func_80A7CBC8(this);
    } else if ((this->unk_314 & 1) && (this->actor.bgCheckFlags & 0x40)) {
        func_80A7CE60(this);
    } else if (this->actor.xzDistToPlayer < 40.0f) {
        func_80A7C818(this);
    }
}

void func_80A7C598(EnInsect* this) {
    this->unk_31A = Rand_S16Offset(10, 45);
    func_80A7BF58(this);
    this->actionFunc = func_80A7C5EC;
    this->unk_314 |= 0x100;
}

void func_80A7C5EC(EnInsect* this, PlayState* play) {
    s32 pad1;
    s32 pad2;
    s16 yaw;
    s16 sp34 = this->actor.params & 3;

    Math_SmoothStepToF(&this->actor.speedXZ, 1.5f, 0.1f, 0.5f, 0.0f);

    if (EnInsect_XZDistanceSquared(&this->actor.world.pos, &this->actor.home.pos) > 1600.0f || (this->unk_31A < 4)) {
        yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos);
        Math_ScaledStepToS(&this->actor.world.rot.y, yaw, 2000);
    } else if (this->actor.child != NULL && &this->actor != this->actor.child) {
        yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.child->world.pos);
        Math_ScaledStepToS(&this->actor.world.rot.y, yaw, 2000);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->skelAnime.playSpeed = CLAMP(this->actor.speedXZ * 1.4f, 0.7f, 1.9f);

    SkelAnime_Update(&this->skelAnime);

    if (this->unk_31A <= 0) {
        func_80A7C3A0(this);
    }

    if (((this->unk_314 & 4) && this->unk_31C <= 0) ||
        ((sp34 == 2 || sp34 == 3) && (this->unk_314 & 1) && (this->actor.bgCheckFlags & 1) && D_80A7DEB8 >= 4)) {
        func_80A7CBC8(this);
    } else if ((this->unk_314 & 1) && (this->actor.bgCheckFlags & 0x40)) {
        func_80A7CE60(this);
    } else if (this->actor.xzDistToPlayer < 40.0f) {
        func_80A7C818(this);
    }
}

void func_80A7C818(EnInsect* this) {
    this->unk_31A = Rand_S16Offset(10, 40);
    func_80A7BF58(this);
    this->actionFunc = func_80A7C86C;
    this->unk_314 |= 0x100;
}

void func_80A7C86C(EnInsect* this, PlayState* play) {
    s32 pad1;
    s32 pad2;
    s16 pad3;
    s16 frames;
    s16 yaw;
    s16 sp38 = this->actor.xzDistToPlayer < 40.0f;

    Math_SmoothStepToF(&this->actor.speedXZ, 1.8f, 0.1f, 0.5f, 0.0f);

    if (EnInsect_XZDistanceSquared(&this->actor.world.pos, &this->actor.home.pos) > 25600.0f || this->unk_31A < 4) {
        yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos);
        Math_ScaledStepToS(&this->actor.world.rot.y, yaw, 2000);
    } else if (sp38 != 0) {
        frames = play->state.frames;
        yaw = this->actor.yawTowardsPlayer + 0x8000;

        if (frames & 0x10) {
            if (frames & 0x20) {
                yaw += 0x2000;
            }
        } else {
            if (frames & 0x20) {
                yaw -= 0x2000;
            }
        }
        if (play) {} // Must be 'play'
        Math_ScaledStepToS(&this->actor.world.rot.y, yaw, 2000);
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->skelAnime.playSpeed = CLAMP(this->actor.speedXZ * 1.6f, 0.8f, 1.9f);
    SkelAnime_Update(&this->skelAnime);

    if (this->unk_31A <= 0 || !sp38) {
        func_80A7C3A0(this);
    } else if ((this->unk_314 & 1) && (this->actor.bgCheckFlags & 0x40)) {
        func_80A7CE60(this);
    }
}

void func_80A7CA64(EnInsect* this) {
    this->unk_31A = 200;

    Actor_SetScale(&this->actor, 0.001f);

    this->actor.draw = NULL;
    this->actor.speedXZ = 0.0f;

    func_80A7BF58(this);

    this->skelAnime.playSpeed = 0.3f;
    this->actionFunc = func_80A7CAD0;
    this->unk_314 &= ~0x100;
}

void func_80A7CAD0(EnInsect* this, PlayState* play) {
    if (this->unk_31A == 20 && !(this->unk_314 & 4)) {
        this->actor.draw = EnInsect_Draw;
    } else if (this->unk_31A == 0) {
        if (this->unk_314 & 4) {
            Actor_Kill(&this->actor);
        } else {
            Actor_SetScale(&this->actor, 0.01f);
            func_80A7C3A0(this);
        }
    } else if (this->unk_31A < 20) {
        Actor_SetScale(&this->actor, CLAMP_MAX(this->actor.scale.x + 0.001f, 0.01f));
        SkelAnime_Update(&this->skelAnime);
    }
}

void func_80A7CBC8(EnInsect* this) {
    this->unk_31A = 60;
    func_80A7BF58(this);
    this->skelAnime.playSpeed = 1.9f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MUSI_SINK);
    Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);
    this->actionFunc = func_80A7CC3C;
    this->unk_314 &= ~0x100;
    this->unk_314 |= 0x8;
}

void func_80A7CC3C(EnInsect* this, PlayState* play) {
    static Vec3f accel = { 0.0f, 0.0f, 0.0f };
    static Vec3f unused = { 0.0f, 0.0f, 0.0f };
    s32 pad[2];
    Vec3f velocity;

    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 0.1f, 0.5f, 0.0f);
    Math_StepToS(&this->actor.shape.rot.x, 10922, 352);

    Actor_SetScale(&this->actor, CLAMP_MIN(this->actor.scale.x - 0.0002f, 0.001f));

    this->actor.shape.yOffset -= 0.8f;
    this->actor.world.pos.x = Rand_ZeroOne() + this->actor.home.pos.x - 0.5f;
    this->actor.world.pos.z = Rand_ZeroOne() + this->actor.home.pos.z - 0.5f;

    SkelAnime_Update(&this->skelAnime);

    if (this->unk_31A > 20 && Rand_ZeroOne() < 0.1f) {
        velocity.x = Math_SinS(this->actor.shape.rot.y) * -0.6f;
        velocity.y = Math_SinS(this->actor.shape.rot.x) * 0.6f;
        velocity.z = Math_CosS(this->actor.shape.rot.y) * -0.6f;
        func_800286CC(play, &this->actor.world.pos, &velocity, &accel, Rand_ZeroOne() * 5.0f + 8.0f,
                      Rand_ZeroOne() * 5.0f + 8.0f);
    }

    if (this->unk_31A <= 0) {
        if ((this->unk_314 & 0x10) && this->soilActor != NULL &&
            Math3D_Vec3fDistSq(&this->soilActor->actor.world.pos, &this->actor.world.pos) < 64.0f) {
            this->soilActor->unk_152 = 1;
        }
        Actor_Kill(&this->actor);
    }
}

void func_80A7CE60(EnInsect* this) {
    this->unk_31A = Rand_S16Offset(120, 50);
    func_80A7BF58(this);
    this->unk_316 = this->unk_318 = 0;
    this->actionFunc = func_80A7CEC0;
    this->unk_314 &= ~0x100;
}

void func_80A7CEC0(EnInsect* this, PlayState* play) {
    f32 temp_f0;
    s16 temp_v1;
    s16 pad;
    s16 sp4E;
    Vec3f sp40;
    s32 phi_v0;
    s32 phi_v0_2;

    sp4E = this->actor.params & 3;

    if (this->unk_31A >= 81) {
        Math_StepToF(&this->actor.speedXZ, 0.6f, 0.08f);
    } else {
        Math_StepToF(&this->actor.speedXZ, 0.0f, 0.02f);
    }
    this->actor.velocity.y = 0.0f;
    this->actor.world.pos.y += this->actor.yDistToWater;
    this->skelAnime.playSpeed = CLAMP(this->unk_31A * 0.018f, 0.1f, 1.9f);

    SkelAnime_Update(&this->skelAnime);

    if (this->unk_31A >= 81) {
        this->unk_316 += Rand_S16Offset(-50, 100);
        this->unk_318 += Rand_S16Offset(-300, 600);
    }

    temp_v1 = this->skelAnime.playSpeed * 200.0f;

    if (this->unk_316 < -temp_v1) {
        this->unk_316 = -temp_v1;
    } else {
        if (temp_v1 < this->unk_316) {
            phi_v0 = temp_v1;
        } else {
            phi_v0 = this->unk_316;
        }
        this->unk_316 = phi_v0;
    }
    this->actor.world.rot.y += this->unk_316;

    temp_v1 = this->skelAnime.playSpeed * 1000.0f;
    if (this->unk_318 < -temp_v1) {
        this->unk_318 = -temp_v1;
    } else {
        if (temp_v1 < this->unk_318) {
            phi_v0_2 = temp_v1;
        } else {
            phi_v0_2 = this->unk_318;
        }
        this->unk_318 = phi_v0_2;
    }
    this->actor.shape.rot.y += this->unk_318;

    Math_ScaledStepToS(&this->actor.world.rot.x, 0, 3000);
    this->actor.shape.rot.x = this->actor.world.rot.x;

    if (Rand_ZeroOne() < 0.03f) {
        sp40.x = this->actor.world.pos.x;
        sp40.y = this->actor.world.pos.y + this->actor.yDistToWater;
        sp40.z = this->actor.world.pos.z;
        EffectSsGRipple_Spawn(play, &sp40, 20, 100, 4);
        EffectSsGRipple_Spawn(play, &sp40, 40, 200, 8);
    }

    if (this->unk_31A <= 0 || ((this->unk_314 & 4) && this->unk_31C <= 0) ||
        ((sp4E == 2 || sp4E == 3) && (this->unk_314 & 1) && D_80A7DEB8 >= 4)) {
        func_80A7D1F4(this);
    } else if (!(this->actor.bgCheckFlags & 0x40)) {
        if (this->unk_314 & 0x10) {
            func_80A7D39C(this);
        } else {
            func_80A7C3A0(this);
        }
    }
}

void func_80A7D1F4(EnInsect* this) {
    this->unk_31A = 100;
    func_80A7BF58(this);
    this->actor.velocity.y = 0.0f;
    this->actor.speedXZ = 0.0f;
    this->actor.minVelocityY = -0.8f;
    this->actor.gravity = -0.04f;
    this->unk_314 &= ~0x3;
    this->actionFunc = func_80A7D26C;
    this->unk_314 &= ~0x100;
    this->unk_314 |= 8;
}

void func_80A7D26C(EnInsect* this, PlayState* play) {
    this->actor.shape.rot.x -= 500;
    this->actor.shape.rot.y += 200;
    Actor_SetScale(&this->actor, CLAMP_MIN(this->actor.scale.x - 0.00005f, 0.001f));

    if (this->actor.yDistToWater > 5.0f && this->actor.yDistToWater < 30.0f && Rand_ZeroOne() < 0.3f) {
        EffectSsBubble_Spawn(play, &this->actor.world.pos, -5.0f, 5.0f, 5.0f, (Rand_ZeroOne() * 0.04f) + 0.02f);
    }

    if (this->unk_31A <= 0) {
        Actor_Kill(&this->actor);
    }
}

void func_80A7D39C(EnInsect* this) {
    func_80A7BF58(this);
    this->unk_31A = 100;
    this->unk_324 = 1.5f;
    this->unk_328 = Rand_ZeroOne() * (0xFFFF + 0.5f);
    this->unk_316 = (Rand_ZeroOne() - 0.5f) * 1500.0f;
    this->actor.world.rot.y = Rand_ZeroOne() * (0xFFFF + 0.5f);
    Actor_SetScale(&this->actor, 0.003f);
    this->actionFunc = func_80A7D460;
    this->unk_314 |= 0x100;
}

void func_80A7D460(EnInsect* this, PlayState* play) {
    s32 temp_a0;
    s32 sp50;
    f32 phi_f0;
    EnInsect* thisTemp = this;
    s32 temp_a1;
    f32 sp40;
    f32 phi_f2;
    s16 sp3A;
    s16 sp38;
    f32 sp34;

    sp50 = 0;
    sp3A = this->actor.params & 3;

    if (this->soilActor != NULL) {
        sp40 = Math3D_Vec3fDistSq(&this->actor.world.pos, &this->soilActor->actor.world.pos);
    } else {
        if (this->unk_314 & 0x10) {
            osSyncPrintf(VT_COL(YELLOW, BLACK));
            // "warning: target Actor is NULL"
            osSyncPrintf("warning:目標 Actor が NULL (%s %d)\n", __FILE__, __LINE__);
            osSyncPrintf(VT_RST);
        }
        sp40 = 40.0f;
    }

    D_80A7DEB0 += 0.99999994f / 300.0f;
    if (D_80A7DEB0 > 1.0f) {
        D_80A7DEB0 = 1.0f;
    }

    if (D_80A7DEB0 > 0.999f) {
        phi_f2 = 0.0f;
    } else {
        if (sp40 > 900.0f) {
            phi_f2 = ((1.1f - D_80A7DEB0) * 100.0f) + 20.0f;
        } else {
            phi_f2 = (1.0f - D_80A7DEB0) * 10.0f;
        }
    }

    if (this->soilActor != NULL && Rand_ZeroOne() < 0.07f) {
        this->actor.home.pos.x = (Rand_ZeroOne() - 0.5f) * phi_f2 + thisTemp->soilActor->actor.world.pos.x;
        this->actor.home.pos.y = thisTemp->soilActor->actor.world.pos.y;
        this->actor.home.pos.z = (Rand_ZeroOne() - 0.5f) * phi_f2 + thisTemp->soilActor->actor.world.pos.z;
    }

    if (D_80A7DEB0 > 0.999f) {
        this->unk_328 = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos);
        this->unk_324 = Rand_ZeroOne() * 0.6f + 0.6f;
    } else if (Rand_ZeroOne() < 0.07f) {
        if (this->unk_324 > 1.0f) {
            this->unk_324 = 0.1f;
        } else {
            this->unk_324 = Rand_ZeroOne() * 0.8f + 1.0f;
        }

        sp34 = 1.3f - D_80A7DEB0;
        if (sp34 < 0.0f) {
            sp34 = 0.0f;
        } else {
            if (sp34 > 1.0f) {
                phi_f0 = 1.0f;
            } else {
                phi_f0 = sp34;
            }
            sp34 = phi_f0;
        }

        sp38 = (Rand_ZeroOne() - 0.5f) * 65535.0f * sp34;
        this->unk_328 = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos) + sp38;
    }

    Actor_SetScale(&this->actor, CLAMP_MAX(thisTemp->actor.scale.x + 0.0008f, 0.01f));

    if (this->actor.bgCheckFlags & 1) {
        Math_SmoothStepToF(&this->actor.speedXZ, this->unk_324, 0.1f, 0.5f, 0.0f);
        Math_ScaledStepToS(&this->actor.world.rot.y, this->unk_328, 2000);
        sp50 = Math_ScaledStepToS(&this->actor.world.rot.x, 0, 2000);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        this->actor.shape.rot.x = this->actor.world.rot.x;
    } else {
        Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 0.1f, 0.5f, 0.0f);
        this->actor.speedXZ += (Rand_ZeroOne() - 0.5f) * 0.14f;
        this->actor.velocity.y += Rand_ZeroOne() * 0.12f;
        this->actor.world.rot.y += this->unk_316;
        this->actor.shape.rot.y = this->actor.world.rot.y;
        this->actor.shape.rot.x -= 2000;
    }

    phi_f2 = Rand_ZeroOne() * 0.5f + this->actor.speedXZ * 1.3f;
    if (phi_f2 < 0.0f) {
        this->skelAnime.playSpeed = 0.0f;
    } else {
        if (phi_f2 > 1.9f) {
            phi_f0 = 1.9f;
        } else {
            phi_f0 = phi_f2;
        }
        this->skelAnime.playSpeed = phi_f0;
    }

    SkelAnime_Update(&this->skelAnime);
    if (!(this->unk_314 & 0x40) && (this->unk_314 & 1) && (this->actor.bgCheckFlags & 1)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_MUSI_LAND);
        this->unk_314 |= 0x40;
    }

    if (sp3A == 2 && (this->unk_314 & 0x10) && !(this->unk_314 & 0x80)) {
        if (this->unk_32A >= 15) {
            if (this->soilActor != NULL) {
                if (!(GET_GS_FLAGS(((this->soilActor->actor.params >> 8) & 0x1F) - 1) &
                      (this->soilActor->actor.params & 0xFF))) {
                    func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
                }
            }
            this->unk_314 |= 0x80;
        } else {
            this->unk_32A++;
        }
    }

    if ((this->unk_314 & 1) && (this->actor.bgCheckFlags & 0x40)) {
        func_80A7CE60(this);
    } else if (this->unk_314 & 0x10) {
        if (sp40 < 9.0f) {
            func_80A7CBC8(this);
        } else if (this->unk_31A <= 0 || this->unk_31C <= 0 ||
                   ((this->unk_314 & 1) && (this->actor.bgCheckFlags & 1) && D_80A7DEB8 >= 4 &&
                    (sp3A == 2 || sp3A == 3))) {
            func_80A7CBC8(this);
        } else {
            if (sp40 < 900.0f) {
                this->unk_31C++;
                this->unk_314 |= 0x20;
            } else {
                this->unk_31A = 100;
            }
        }
    } else if (sp50 != 0) {
        func_80A7C3A0(this);
    } else if ((sp3A == 2 || sp3A == 3) && (this->unk_314 & 1) && this->unk_31C <= 0 && this->unk_31A <= 0 &&
               this->actor.floorHeight < BGCHECK_Y_MIN + 10.0f) {
        osSyncPrintf(VT_COL(YELLOW, BLACK));
        // "BG missing? To do Actor_delete"
        osSyncPrintf("BG 抜け？ Actor_delete します(%s %d)\n", __FILE__, __LINE__);
        osSyncPrintf(VT_RST);
        Actor_Kill(&this->actor);
    }
}

void EnInsect_Update(Actor* thisx, PlayState* play) {
    EnInsect* this = (EnInsect*)thisx;
    s32 phi_v0;

    if (this->actor.child != NULL) {
        if (this->actor.child->update == NULL) {
            if (&this->actor != this->actor.child) {
                this->actor.child = NULL;
            }
        }
    }

    if (this->unk_31A > 0) {
        this->unk_31A--;
    }

    if (this->unk_31C > 0) {
        this->unk_31C--;
    }

    this->actionFunc(this, play);

    if (this->actor.update != NULL) {
        Actor_MoveForward(&this->actor);
        if (this->unk_314 & 0x100) {
            if (this->unk_314 & 1) {
                if (this->actor.bgCheckFlags & 1) {
                    func_80A7C058(this);
                }
            } else {
                func_80A7C058(this);
            }
        }

        phi_v0 = 0;

        if (this->unk_314 & 1) {
            phi_v0 = 4;
        }

        if (this->unk_314 & 2) {
            phi_v0 |= 1;
        }

        if (phi_v0 != 0) {
            phi_v0 |= 0x40;
            Actor_UpdateBgCheckInfo(play, &this->actor, 8.0f, 5.0f, 0.0f, phi_v0);
        }

        if (Actor_HasParent(&this->actor, play)) {
            this->actor.parent = NULL;
            phi_v0 = this->actor.params & 3;

            if (phi_v0 == 2 || phi_v0 == 3) {
                Actor_Kill(&this->actor);
            } else {
                func_80A7CA64(this);
            }
        } else if (this->actor.xzDistToPlayer < 50.0f && this->actionFunc != func_80A7CAD0) {
            if (!(this->unk_314 & 0x20) && this->unk_31C < 180) {
                CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
            }

            if (!(this->unk_314 & 8) && D_80A7DEB4 < 4 && EnInsect_InBottleRange(this, play) &&
                // GI_MAX in this case allows the player to catch the actor in a bottle
                func_8002F434(&this->actor, play, GI_MAX, 60.0f, 30.0f)) {
                D_80A7DEB4++;
            }
        }

        Actor_SetFocus(&this->actor, 0.0f);
    }
}

void EnInsect_Draw(Actor* thisx, PlayState* play) {
    EnInsect* this = (EnInsect*)thisx;

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, NULL, NULL, NULL);
    Collider_UpdateSpheres(0, &this->collider);
    D_80A7DEB4 = 0;
}

void EnInsect_Reset(void) {
    D_80A7DEB0 = 0.0f;
    D_80A7DEB4 = 0;
    D_80A7DEB8 = 0;
}