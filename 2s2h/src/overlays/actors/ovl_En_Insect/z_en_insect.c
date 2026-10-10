/*
 * File: z_en_insect.c
 * Overlay: ovl_En_Insect
 * Description: Single freestanding bug that doesn't burrow
 */

#include "z_en_insect.h"

#define FLAGS 0x00000000

void EnInsect_Init(Actor* thisx, PlayState* play);
void EnInsect_Destroy(Actor* thisx, PlayState* play2);
void EnInsect_Update(Actor* thisx, PlayState* play);
void EnInsect_Draw(Actor* thisx, PlayState* play);

void func_8091AC78(EnInsect* this);
void func_8091ACC4(EnInsect* this, PlayState* play);
void func_8091AE10(EnInsect* this);
void func_8091AE5C(EnInsect* this, PlayState* play);
void func_8091B030(EnInsect* this);
void func_8091B07C(EnInsect* this, PlayState* play);
void func_8091B2D8(EnInsect* this, PlayState* play);
void func_8091B3D0(EnInsect* this);
void func_8091B440(EnInsect* this, PlayState* play);
void func_8091B618(EnInsect* this);
void func_8091B670(EnInsect* this, PlayState* play);
void func_8091B928(EnInsect* this);
void func_8091B984(EnInsect* this, PlayState* play);

s16 D_8091BD60 = 0;

ActorProfile En_Insect_Profile = {
    /**/ ACTOR_EN_INSECT,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnInsect),
    /**/ EnInsect_Init,
    /**/ EnInsect_Destroy,
    /**/ EnInsect_Update,
    /**/ EnInsect_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 5 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_1,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

u16 D_8091BDB8[] = { 0, 5 };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 700, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 20, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 30, ICHAIN_STOP),
};

Vec3f D_8091BDCC = { 0.0f, 0.0f, 0.0f };

void func_8091A8A0(EnInsect* this) {
    this->unk_30C = D_8091BDB8[ENINSECT_GET_1(&this->actor)];
}

f32 EnInsect_XZDistanceSquared(Vec3f* arg0, Vec3f* arg1) {
    return SQ(arg0->x - arg1->x) + SQ(arg0->z - arg1->z);
}

s32 EnInsect_InBottleRange(EnInsect* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    Vec3f sp1C;

    if (this->actor.xzDistToPlayer < 32.0f) {
        sp1C.x = player->actor.world.pos.x + (Math_SinS(BINANG_ROT180(this->actor.yawTowardsPlayer)) * 16.0f);
        sp1C.y = player->actor.world.pos.y;
        sp1C.z = player->actor.world.pos.z + (Math_CosS(BINANG_ROT180(this->actor.yawTowardsPlayer)) * 16.0f);
        if (EnInsect_XZDistanceSquared(&sp1C, &this->actor.world.pos) <= SQ(20.0f)) {
            return true;
        }
    }
    return false;
}

void func_8091A9E4(EnInsect* this) {
    if (this->unk_316 > 0) {
        this->unk_316--;
        return;
    }

    Actor_PlaySfx(&this->actor, NA_SE_EN_MUSI_WALK);

    this->unk_316 = 3.0f / CLAMP_MIN(this->skelAnime.playSpeed, 0.1f);
    if (this->unk_316 < 2) {
        this->unk_316 = 2;
    }
}

void EnInsect_Init(Actor* thisx, PlayState* play) {
    EnInsect* this = (EnInsect*)thisx;
    f32 rand;

    this->actor.world.rot.y = Rand_Next() & 0xFFFF;
    this->actor.home.rot.y = this->actor.world.rot.y;
    this->actor.shape.rot.y = this->actor.world.rot.y;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    func_8091A8A0(this);

    SkelAnime_Init(play, &this->skelAnime, &gameplay_keep_Skel_0527A0, &gameplay_keep_Anim_05140C, this->jointTable,
                   this->morphTable, BUG_LIMB_MAX);
    Animation_Change(&this->skelAnime, &gameplay_keep_Anim_05140C, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, 0.0f);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElements);

    {
        ColliderJntSphElement* jntSphElem = &this->collider.elements[0];

        jntSphElem->dim.worldSphere.radius = jntSphElem->dim.modelSphere.radius * jntSphElem->dim.scale;
    }

    this->actor.colChkInfo.mass = 30;

    if (this->unk_30C & 1) {
        this->actor.gravity = -0.2f;
        this->actor.terminalVelocity = -2.0f;
    }

    if (this->unk_30C & 4) {
        this->unk_314 = Rand_S16Offset(200, 40);
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    }

    rand = Rand_ZeroOne();
    if (rand < 0.3f) {
        func_8091AC78(this);
    } else if (rand < 0.4f) {
        func_8091AE10(this);
    } else {
        func_8091B030(this);
    }
}

void EnInsect_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInsect* this = (EnInsect*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void func_8091AC78(EnInsect* this) {
    this->unk_312 = Rand_S16Offset(5, 35);
    this->actionFunc = func_8091ACC4;
    this->unk_30C |= 0x100;
}

void func_8091ACC4(EnInsect* this, PlayState* play) {
    f32 temp_f2;

    Math_SmoothStepToF(&this->actor.speed, 0.0f, 0.1f, 0.5f, 0.0f);

    temp_f2 = (Rand_ZeroOne() * 0.8f) + (this->actor.speed * 1.2f);
    if (temp_f2 < 0.0f) {
        this->skelAnime.playSpeed = 0.0f;
    } else {
        f32 clamped = CLAMP_MAX(temp_f2, 1.9f);

        this->skelAnime.playSpeed = clamped;
    }

    SkelAnime_Update(&this->skelAnime);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->unk_312 <= 0) {
        func_8091AE10(this);
    }

    if ((this->unk_30C & 4) && (this->unk_314 <= 0)) {
        func_8091B3D0(this);
    } else if ((this->unk_30C & 1) && (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH)) {
        func_8091B618(this);
    } else if (this->actor.xzDistToPlayer < 40.0f) {
        func_8091B030(this);
    }
}

void func_8091AE10(EnInsect* this) {
    this->unk_312 = Rand_S16Offset(10, 45);
    this->actionFunc = func_8091AE5C;
    this->unk_30C |= 0x100;
}

void func_8091AE5C(EnInsect* this, PlayState* play) {
    s32 pad;
    f32 temp_f0;

    Math_SmoothStepToF(&this->actor.speed, 1.5f, 0.1f, 0.5f, 0.0f);

    if ((EnInsect_XZDistanceSquared(&this->actor.world.pos, &this->actor.home.pos) > SQ(40.0f)) ||
        (this->unk_312 < 4)) {
        Math_ScaledStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos),
                           0x7D0);
    } else if ((this->actor.child != NULL) && (&this->actor != this->actor.child)) {
        Math_ScaledStepToS(&this->actor.world.rot.y,
                           Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.child->world.pos), 0x7D0);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;

    temp_f0 = this->actor.speed * 1.4f;
    this->skelAnime.playSpeed = CLAMP(temp_f0, 0.7f, 1.9f);
    SkelAnime_Update(&this->skelAnime);

    if (this->unk_312 <= 0) {
        func_8091AC78(this);
    }

    if ((this->unk_30C & 4) && (this->unk_314 <= 0)) {
        func_8091B3D0(this);
    } else if ((this->unk_30C & 1) && (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH)) {
        func_8091B618(this);
    } else if (this->actor.xzDistToPlayer < 40.0f) {
        func_8091B030(this);
    }
}

void func_8091B030(EnInsect* this) {
    this->unk_312 = Rand_S16Offset(10, 40);
    this->actionFunc = func_8091B07C;
    this->unk_30C |= 0x100;
}

void func_8091B07C(EnInsect* this, PlayState* play) {
    s32 pad;
    f32 speed;
    s16 frames;
    s16 yaw;
    s32 sp38 = this->actor.xzDistToPlayer < 40.0f;

    Math_SmoothStepToF(&this->actor.speed, 1.8f, 0.1f, 0.5f, 0.0f);

    if ((EnInsect_XZDistanceSquared(&this->actor.world.pos, &this->actor.home.pos) > SQ(160.0f)) ||
        (this->unk_312 < 4)) {
        Math_ScaledStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos),
                           0x7D0);
    } else if (sp38) {
        frames = play->state.frames;
        yaw = BINANG_ROT180(this->actor.yawTowardsPlayer);
        if ((frames & 0x10) != 0) {
            if ((frames & 0x20) != 0) {
                yaw += 0x2000;
            }
        } else if ((frames & 0x20) != 0) {
            yaw -= 0x2000;
        }

        //! FAKE:
        if (play) {}

        Math_ScaledStepToS(&this->actor.world.rot.y, yaw, 0x7D0);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;

    speed = this->actor.speed * 1.6f;
    this->skelAnime.playSpeed = CLAMP(speed, 0.8f, 1.9f);
    SkelAnime_Update(&this->skelAnime);

    if ((this->unk_312 <= 0) || !sp38) {
        func_8091AC78(this);
    } else if ((this->unk_30C & 1) && (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH)) {
        func_8091B618(this);
    }
}

void func_8091B274(EnInsect* this) {
    this->unk_312 = 200;
    Actor_SetScale(&this->actor, 0.001f);
    this->actor.draw = NULL;
    this->actor.speed = 0.0f;
    this->skelAnime.playSpeed = 0.3f;
    this->actionFunc = func_8091B2D8;
    this->unk_30C &= ~0x100;
}

void func_8091B2D8(EnInsect* this, PlayState* play) {
    if ((this->unk_312 == 20) && !(this->unk_30C & 4)) {
        this->actor.draw = EnInsect_Draw;
    } else if (this->unk_312 == 0) {
        if (this->unk_30C & 4) {
            Actor_Kill(&this->actor);
            return;
        }

        Actor_SetScale(&this->actor, 0.01f);
        func_8091AC78(this);
    } else if (this->unk_312 < 20) {
        Actor_SetScale(&this->actor, CLAMP_MAX(this->actor.scale.x + 0.001f, 0.01f));
        SkelAnime_Update(&this->skelAnime);
    }
}

void func_8091B3D0(EnInsect* this) {
    this->unk_312 = 60;
    this->skelAnime.playSpeed = 1.9f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_STALTURA_BOUND);
    Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);
    this->actionFunc = func_8091B440;
    this->unk_30C &= ~0x100;
    this->unk_30C |= 8;
}

void func_8091B440(EnInsect* this, PlayState* play) {
    s32 pad[2];
    Vec3f sp34;

    Math_SmoothStepToF(&this->actor.speed, 0.0f, 0.1f, 0.5f, 0.0f);
    Math_StepToS(&this->actor.shape.rot.x, 0x2AAA, 0x160);
    Actor_SetScale(&this->actor, CLAMP_MIN(this->actor.scale.x - 0.0002f, 0.001f));

    this->actor.shape.yOffset -= 0.8f;
    this->actor.world.pos.x = (Rand_ZeroOne() + this->actor.home.pos.x) - 0.5f;
    this->actor.world.pos.z = (Rand_ZeroOne() + this->actor.home.pos.z) - 0.5f;

    SkelAnime_Update(&this->skelAnime);

    if ((this->unk_312 > 20) && (Rand_ZeroOne() < 0.1f)) {
        sp34.x = Math_SinS(this->actor.shape.rot.y) * -0.6f;
        sp34.y = Math_SinS(this->actor.shape.rot.x) * 0.6f;
        sp34.z = Math_CosS(this->actor.shape.rot.y) * -0.6f;
        func_800B1210(play, &this->actor.world.pos, &sp34, &D_8091BDCC, (Rand_ZeroOne() * 5.0f) + 8.0f,
                      (Rand_ZeroOne() * 5.0f) + 8.0f);
    }

    if (this->unk_312 <= 0) {
        Actor_Kill(&this->actor);
    }
}

void func_8091B618(EnInsect* this) {
    this->unk_312 = Rand_S16Offset(120, 50);
    this->unk_310 = 0;
    this->unk_30E = this->unk_310;
    this->actionFunc = func_8091B670;
    this->unk_30C &= ~0x100;
}

void func_8091B670(EnInsect* this, PlayState* play) {
    s32 pad[2];
    s16 temp;
    Vec3f sp40;

    if (this->unk_312 > 80) {
        Math_StepToF(&this->actor.speed, 0.6f, 0.08f);
    } else {
        Math_StepToF(&this->actor.speed, 0.0f, 0.02f);
    }

    this->actor.velocity.y = 0.0f;
    this->actor.world.pos.y += this->actor.depthInWater;

    this->skelAnime.playSpeed = CLAMP(this->unk_312 * 0.018f, 0.1f, 1.9f);
    SkelAnime_Update(&this->skelAnime);

    if (this->unk_312 > 80) {
        this->unk_30E += Rand_S16Offset(-50, 100);
        this->unk_310 += Rand_S16Offset(-300, 600);
    }

    temp = this->skelAnime.playSpeed * 200.0f;
    this->unk_30E = CLAMP(this->unk_30E, -temp, temp);
    this->actor.world.rot.y += this->unk_30E;

    temp = this->skelAnime.playSpeed * 1000.0f;
    this->unk_310 = CLAMP(this->unk_310, -temp, temp);

    this->actor.shape.rot.y += this->unk_310;
    Math_ScaledStepToS(&this->actor.world.rot.x, 0, 0xBB8);
    this->actor.shape.rot.x = this->actor.world.rot.x;

    if (Rand_ZeroOne() < 0.03f) {
        sp40.x = this->actor.world.pos.x;
        sp40.y = this->actor.world.pos.y + this->actor.depthInWater;
        sp40.z = this->actor.world.pos.z;
        EffectSsGRipple_Spawn(play, &sp40, 40, 200, 4);
    }

    if ((this->unk_312 <= 0) || ((this->unk_30C & 4) && (this->unk_314 <= 0))) {
        func_8091B928(this);
    } else if (!(this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH)) {
        func_8091AC78(this);
    }
}

void func_8091B928(EnInsect* this) {
    this->unk_312 = 100;
    this->actor.velocity.y = 0.0f;
    this->actor.speed = 0.0f;
    this->actor.terminalVelocity = -0.8f;
    this->actor.gravity = -0.04f;
    this->unk_30C &= ~1;
    this->actionFunc = func_8091B984;
    this->unk_30C &= ~0x100;
    this->unk_30C |= 8;
}

void func_8091B984(EnInsect* this, PlayState* play) {
    this->actor.shape.rot.x -= 0x1F4;
    this->actor.shape.rot.y += 0xC8;
    Actor_SetScale(&this->actor, CLAMP_MIN(this->actor.scale.x - 0.00005f, 0.001f));

    if ((this->actor.depthInWater > 5.0f) && (this->actor.depthInWater < 30.0f) && (Rand_ZeroOne() < 0.3f)) {
        EffectSsBubble_Spawn(play, &this->actor.world.pos, -5.0f, 5.0f, 5.0f, (Rand_ZeroOne() * 0.04f) + 0.02f);
    }

    if (this->unk_312 <= 0) {
        Actor_Kill(&this->actor);
    }
}

void EnInsect_Update(Actor* thisx, PlayState* play) {
    EnInsect* this = (EnInsect*)thisx;
    s32 updBgCheckInfoFlags;

    if ((this->actor.child != NULL) && (this->actor.child->update == NULL) && (&this->actor != this->actor.child)) {
        this->actor.child = NULL;
    }

    if (this->unk_312 > 0) {
        this->unk_312--;
    }

    if (this->unk_314 > 0) {
        this->unk_314--;
    }

    this->actionFunc(this, play);

    if (this->actor.update != NULL) {
        Actor_MoveWithGravity(&this->actor);
        if (this->unk_30C & 0x100) {
            if (this->unk_30C & 1) {
                if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                    func_8091A9E4(this);
                }
            } else {
                func_8091A9E4(this);
            }
        }

        updBgCheckInfoFlags = 0;
        if (this->unk_30C & 1) {
            updBgCheckInfoFlags = UPDBGCHECKINFO_FLAG_4;
        }

        if (updBgCheckInfoFlags != 0) {
            updBgCheckInfoFlags |= UPDBGCHECKINFO_FLAG_40;
            Actor_UpdateBgCheckInfo(play, &this->actor, 8.0f, 5.0f, 0.0f, updBgCheckInfoFlags);
        }

        if (Actor_HasParent(&this->actor, play)) {
            this->actor.parent = NULL;
            func_8091B274(this);
        } else if ((this->actor.xzDistToPlayer < 50.0f) && (this->actionFunc != func_8091B2D8)) {
            if (!(this->unk_30C & 0x20) && (this->unk_314 < 180)) {
                ColliderJntSphElement* jntSphElem = &this->collider.elements[0];

                jntSphElem->dim.worldSphere.center.x = this->actor.world.pos.x;
                jntSphElem->dim.worldSphere.center.y = this->actor.world.pos.y;
                jntSphElem->dim.worldSphere.center.z = this->actor.world.pos.z;
                CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
            }

            if (!(this->unk_30C & 8) && (D_8091BD60 < 4) && EnInsect_InBottleRange(this, play) &&
                Actor_OfferGetItem(&this->actor, play, GI_MAX, 60.0f, 30.0f)) {
                D_8091BD60++;
            }
        }

        Actor_SetFocus(&this->actor, 0.0f);
    }
}

void EnInsect_Draw(Actor* thisx, PlayState* play) {
    EnInsect* this = (EnInsect*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, NULL);
    D_8091BD60 = 0;
}
