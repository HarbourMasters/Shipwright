/*
 * File: z_en_niw.c
 * Overlay: ovl_En_Niw
 * Description: Cucco (Chicken) (Japanese: Niwatori)
 */

#include "z_en_niw.h"
#include "overlays/actors/ovl_En_Attack_Niw/z_en_attack_niw.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_THROW_ONLY)

void EnNiw_Init(Actor* thisx, PlayState* play);
void EnNiw_Destroy(Actor* thisx, PlayState* play);
void EnNiw_Update(Actor* thisx, PlayState* play2);
void EnNiw_Draw(Actor* thisx, PlayState* play);
void EnNiw_Reset(void);

void EnNiw_SetupIdle(EnNiw* this);
void EnNiw_Idle(EnNiw* this, PlayState* play);
void EnNiw_Thrown(EnNiw* this, PlayState* play);
void EnNiw_SetupRunAway(EnNiw* this);
void EnNiw_RunAway(EnNiw* this, PlayState* play);
void EnNiw_Upset(EnNiw* this, PlayState* play);
void EnNiw_SetupCuccoStorm(EnNiw* this, PlayState* play);
void EnNiw_CuccoStorm(EnNiw* this, PlayState* play);
void EnNiw_Held(EnNiw* this, PlayState* play);
void EnNiw_UpdateFeather(EnNiw* this, PlayState* play);
void EnNiw_DrawFeathers(EnNiw* this, PlayState* play);
void EnNiw_SpawnFeather(EnNiw* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale);

s16 sCuccoStormActive = false;

// why wouldnt they just use actionFunc?
typedef enum EnNiwState {
    /* 0 */ NIW_STATE_IDLE,
    /* 1 */ NIW_STATE_ANGRY1, // 1/2/3 are stages of summoning cucco storm
    /* 2 */ NIW_STATE_ANGRY2,
    /* 3 */ NIW_STATE_ANGRY3,
    /* 4 */ NIW_STATE_HELD,
    /* 5 */ NIW_STATE_FALLING,
    /* 6 */ NIW_STATE_SWIMMING,
    /* 7 */ NIW_STATE_RUNNING,
    /* 8 */ NIW_STATE_HOPPING
} EnNiwState;

ActorProfile En_Niw_Profile = {
    /**/ ACTOR_EN_NIW,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_NIW,
    /**/ sizeof(EnNiw),
    /**/ EnNiw_Init,
    /**/ EnNiw_Destroy,
    /**/ EnNiw_Update,
    /**/ EnNiw_Draw,
    /**/ EnNiw_Reset,
};

static f32 sHeadRotations[] = { 5000.0f, -5000.0f };

static f32 sRunningAngles[] = { 5000.0f, 3000.0f };

static f32 sUnusedValue = 4000.0f;

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 15, 25, 4, { 0, 0, 0 } },
};

void EnNiw_Init(Actor* thisx, PlayState* play) {
    static InitChainEntry sInitChain[] = {
        ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_6, ICHAIN_CONTINUE),
        ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_CONTINUE),
        ICHAIN_F32(lockOnArrowOffset, 0, ICHAIN_STOP),
    };
    EnNiw* this = (EnNiw*)thisx;
    Vec3f D_808934C4 = { 90000.0f, 90000.0f, 90000.0f };

    if (this->actor.params < 0) { // all scene spawned cucco are (-1)
        this->actor.params = NIW_TYPE_REGULAR;
    }

    Math_Vec3f_Copy(&this->unk2BC, &D_808934C4);

    this->niwType = this->actor.params;
    Actor_ProcessInitChain(&this->actor, sInitChain);

    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;

    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, 25.0f);

    SkelAnime_InitFlex(play, &this->skelAnime, &gNiwSkel, &gNiwIdleAnim, this->jointTable, this->morphTable,
                       NIW_LIMB_MAX);
    Math_Vec3f_Copy(&this->unk2A4, &this->actor.world.pos);
    Math_Vec3f_Copy(&this->unk2B0, &this->actor.world.pos);

    this->unk308 = 10.0f;
    Actor_SetScale(&this->actor, 0.01f);

    if (this->niwType == NIW_TYPE_UNK1) {
        Actor_SetScale(&this->actor, (BREG(86) / 10000.0f) + 0.004f);
    }

    // random health between 10-20
    // health at zero triggers cucco storm not death
    this->actor.colChkInfo.health = Rand_ZeroFloat(9.99f) + 10.0f;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;

    if (this->niwType == NIW_TYPE_REGULAR) {
        Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    }

    if (this->niwType == NIW_TYPE_HELD) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_CHICKEN_CRY_M);
        this->sfxTimer1 = 30;
        this->heldTimer = 30;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->niwState = NIW_STATE_HELD;
        this->actionFunc = EnNiw_Held;
        this->actor.speed = 0.0f;
        this->unk2BC.z = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.gravity = 0.0f;
    } else {
        EnNiw_SetupIdle(this);
    }
}

void EnNiw_Destroy(Actor* thisx, PlayState* play) {
    EnNiw* this = (EnNiw*)thisx;

    if (this->niwType == NIW_TYPE_REGULAR) {
        Collider_DestroyCylinder(play, &this->collider);
    }
}

/**
 * Summary: instead of using SkelAnime animations, Niw modifies head+wings directly to create animations
 *
 * AttackNiw has a copy of this function that it barely uses
 */
void EnNiw_AnimateWingHead(EnNiw* this, PlayState* play, s16 animIndex) {
    f32 tempOne = 1.0f; // hopefully fake match, but no luck

    if (this->unkTimer24C == 0) {
        // targetLimbRots[0] is bodyRotY
        if (animIndex == NIW_ANIM_STILL) {
            this->targetLimbRots[0] = 0.0f;
        } else {
            this->targetLimbRots[0] = -10000.0f * tempOne;
        }

        this->unk292 += 1;
        this->unkTimer24C = 3;
        if ((this->unk292 % 2) == 0) {
            this->targetLimbRots[0] = 0.0f;
            if (animIndex == NIW_ANIM_STILL) {
                this->unkTimer24C = Rand_ZeroFloat(30.0f);
            }
        }
    }

    if (this->unkTimer24E == 0) {
        this->unkToggle296++;
        this->unkToggle296 &= 1;

        switch (animIndex) {
            case NIW_ANIM_STILL:
                this->targetLimbRots[2] = 0.0f; // both wingRotZ
                this->targetLimbRots[1] = 0.0f;
                break;

            case NIW_ANIM_HEAD_PECKING:
                this->unkTimer24E = 3;
                this->targetLimbRots[2] = 7000.0f * tempOne; // both wingRotZ
                this->targetLimbRots[1] = 7000.0f * tempOne;
                if (this->unkToggle296 == 0) {
                    this->targetLimbRots[2] = 0.0f; // both wingRotZ
                    this->targetLimbRots[1] = 0.0f;
                }
                break;

            case NIW_ANIM_PECKING_AND_WAVING:
                this->unkTimer24E = 2;
                this->targetLimbRots[2] = -10000.0f; // both wingRotZ
                this->targetLimbRots[1] = -10000.0f;
                this->targetLimbRots[7] = 25000.0f; // both wingRotY
                this->targetLimbRots[5] = 25000.0f;
                this->targetLimbRots[8] = 6000.0f; // both wingRotX
                this->targetLimbRots[6] = 6000.0f;
                if (this->unkToggle296 == 0) {
                    this->targetLimbRots[7] = 8000.0f; // both wingRotY
                    this->targetLimbRots[5] = 8000.0f;
                }
                break;

            case NIW_ANIM_PECKING_AND_FORFLAPPING:
                this->unkTimer24E = 2;
                this->targetLimbRots[5] = 10000.0f; // both wingRotY
                this->targetLimbRots[7] = 10000.0f;
                if (this->unkToggle296 == 0) {
                    this->targetLimbRots[5] = 3000.0f; // both wingRotY
                    this->targetLimbRots[7] = 3000.0f;
                }
                break;

            case NIW_ANIM_FREEZE:
                this->unkTimer24C = 5;
                break;

            case NIW_ANIM_PECKING_SLOW_FORFLAPPING:
                this->unkTimer24E = 5;
                this->targetLimbRots[5] = 14000.0f; // both wingRotY
                this->targetLimbRots[7] = 14000.0f;
                if (this->unkToggle296 == 0) {
                    this->targetLimbRots[5] = 10000.0f; // both wingRotY
                    this->targetLimbRots[7] = 10000.0f;
                }
                break;

            default:
                break;
        }
    }

    if (this->targetLimbRots[9] != this->headRotY) {
        Math_ApproachF(&this->headRotY, this->targetLimbRots[9], 0.5f, 4000.0f);
    }
    if (this->targetLimbRots[0] != this->upperBodyRotY) {
        Math_ApproachF(&this->upperBodyRotY, this->targetLimbRots[0], 0.5f, 4000.0f);
    }
    if (this->targetLimbRots[2] != this->leftWingRotZ) {
        Math_ApproachF(&this->leftWingRotZ, this->targetLimbRots[2], 0.8f, 7000.0f);
    }
    if (this->targetLimbRots[7] != this->leftWingRotY) {
        Math_ApproachF(&this->leftWingRotY, this->targetLimbRots[7], 0.8f, 7000.0f);
    }
    if (this->targetLimbRots[8] != this->leftWingRotX) {
        Math_ApproachF(&this->leftWingRotX, this->targetLimbRots[8], 0.8f, 7000.0f);
    }
    if (this->targetLimbRots[1] != this->rightWingRotZ) {
        Math_ApproachF(&this->rightWingRotZ, this->targetLimbRots[1], 0.8f, 7000.0f);
    }
    if (this->targetLimbRots[5] != this->rightWingRotY) {
        Math_ApproachF(&this->rightWingRotY, this->targetLimbRots[5], 0.8f, 7000.0f);
    }
    if (this->targetLimbRots[6] != this->rightWingRotX) {
        Math_ApproachF(&this->rightWingRotX, this->targetLimbRots[6], 0.8f, 7000.0f);
    }
}

void EnNiw_SpawnAttackNiw(EnNiw* this, PlayState* play) {
    f32 xView;
    f32 yView;
    f32 zView;
    Vec3f newNiwPos;
    Actor* attackNiw;

    if ((this->attackNiwSpawnTimer == 0) && (this->attackNiwCount < 7)) {
        xView = play->view.at.x - play->view.eye.x;
        yView = play->view.at.y - play->view.eye.y;
        zView = play->view.at.z - play->view.eye.z;
        newNiwPos.x = play->view.eye.x + ((Rand_ZeroOne() - 0.5f) * xView);
        newNiwPos.y = play->view.eye.y + 50.0f + (yView * 0.5f) + Rand_CenteredFloat(0.3f);
        newNiwPos.z = play->view.eye.z + ((Rand_ZeroOne() - 0.5f) * zView);
        attackNiw = Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_ATTACK_NIW, newNiwPos.x,
                                       newNiwPos.y, newNiwPos.z, 0, 0, 0, ATTACK_NIW_REGULAR);

        if (attackNiw != NULL) {
            this->attackNiwCount++;
            this->attackNiwSpawnTimer = 10;
        }
    }
}

void EnNiw_UpdateRunning(EnNiw* this, PlayState* play, s32 isStormCucco) {
    f32 runningDirection;
    f32 targetRotY;
    f32* runningAngles = sRunningAngles;

    if (this->hopTimer == 0) {
        this->hopTimer = 3;
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->actor.velocity.y = 3.5f; // hopping up while running away
        }
    }

    if (this->runningDirectionTimer == 0) {
        this->isRunningRight++;
        this->isRunningRight &= 1;
        this->runningDirectionTimer = 5;
    }

    if (!this->isRunningRight) {
        runningDirection = runningAngles[isStormCucco];
    } else {
        runningDirection = -runningAngles[isStormCucco];
    }

    if ((isStormCucco == true) && ((this->runAwayTimer == 0) || (this->actor.bgCheckFlags & BGCHECKFLAG_WALL))) {
        this->runAwayTimer = 150;
        if (this->yawTimer == 0) {
            this->yawTimer = 70;
            this->yawTowardsPlayer = this->actor.yawTowardsPlayer;
        }
    }

    targetRotY = this->yawTowardsPlayer + runningDirection;
    Math_SmoothStepToS(&this->actor.world.rot.y, targetRotY, 3, this->unk300, 0);
    Math_ApproachF(&this->unk300, 3000.0f, 1.0f, 500.0f);
    EnNiw_AnimateWingHead(this, play, NIW_ANIM_PECKING_SLOW_FORFLAPPING);
}

void EnNiw_SetupIdle(EnNiw* this) {
    Animation_Change(&this->skelAnime, &gNiwIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gNiwIdleAnim), ANIMMODE_LOOP,
                     -10.0f);
    this->niwState = NIW_STATE_IDLE;
    this->actionFunc = EnNiw_Idle;
}

void EnNiw_Idle(EnNiw* this, PlayState* play) {
    f32 posX2;
    f32 posZ2;
    f32 posX1 = Rand_CenteredFloat(100.0f);
    f32 posZ1 = Rand_CenteredFloat(100.0f);
    s16 nextAnimIndex;

    if (this->niwType == NIW_TYPE_REGULAR) {
        if (Actor_HasParent(&this->actor, play)) {               // picked up
            Actor_PlaySfx(&this->actor, NA_SE_EV_CHICKEN_CRY_M); // crow
            this->sfxTimer1 = 30;
            this->heldTimer = 30;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->niwState = NIW_STATE_HELD;
            this->actor.speed = 0.0f;
            this->actionFunc = EnNiw_Held;
            return;
        }

        Actor_OfferCarry(&this->actor, play);
    } else { // NIW_TYPE_UNK1 || NIW_TYPE_HELD
        this->unkIdleTimer2 = 10;
    }

    nextAnimIndex = NIW_ANIM_STILL; // probably a scoped variable here, where their scope was different
    if (this->unkIdleTimer2 != 0) {
        if (Rand_ZeroFloat(3.99f) < 1.0f) {
            this->headRotationToggle++;
            this->headRotationToggle &= 1;
        }

        Math_ApproachF(&this->targetLimbRots[9], sHeadRotations[this->headRotationToggle], 0.5f, 4000.0f); // head rot
    }

    if ((this->unkIdleTimer2 == 0) && (this->unkIdleTimer == 0)) {
        this->unk298++;
        if (this->unk298 > 7) {
            this->unkIdleTimer2 = Rand_ZeroFloat(30.0f);
            this->unk298 = Rand_ZeroFloat(3.99f);

            if (posX1 < 0.0f) {
                posX1 -= 100.0f;
            } else {
                posX1 += 100.0f;
            }
            if (posZ1 < 0.0f) {
                posZ1 -= 100.0f;
            } else {
                posZ1 += 100.0f;
            }

            this->unk2B0.x = this->unk2A4.x + posX1;
            this->unk2B0.z = this->unk2A4.z + posZ1;

        } else {
            this->unkIdleTimer = 4;
            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                this->actor.speed = 0.0f;
                this->actor.velocity.y = 3.5f; // hopping up and down
            }
        }
    }

    if (this->unkIdleTimer != 0) {
        Math_ApproachZeroF(&this->targetLimbRots[9], 0.5f, 4000.0f); // head rot
        nextAnimIndex = NIW_ANIM_HEAD_PECKING;
        Math_ApproachF(&this->actor.world.pos.x, this->unk2B0.x, 1.0f, this->unk300);
        Math_ApproachF(&this->actor.world.pos.z, this->unk2B0.z, 1.0f, this->unk300);
        Math_ApproachF(&this->unk300, 3.0f, 1.0f, 0.3f);

        posX2 = this->unk2B0.x - this->actor.world.pos.x;
        posZ2 = this->unk2B0.z - this->actor.world.pos.z;

        if (fabsf(posX2) < 10.0f) {
            posX2 = 0;
        }
        if (fabsf(posZ2) < 10.0f) {
            posZ2 = 0;
        }

        if ((posX2 == 0.0f) && (posZ2 == 0.0f)) {
            this->unkIdleTimer = 0;
            this->unk298 = 7;
        }

        Math_SmoothStepToS(&this->actor.world.rot.y, Math_Atan2S(posX2, posZ2), 3, this->unk304, 0);
        Math_ApproachF(&this->unk304, 10000.0f, 1.0f, 1000.0f);
    }

    EnNiw_AnimateWingHead(this, play, nextAnimIndex);
}

void EnNiw_Held(EnNiw* this, PlayState* play) {
    Vec3f D_808934DC = { 90000.0f, 90000.0f, 90000.0f };
    s16 rotZ;

    if (this->heldTimer == 0) {
        this->unk29E = 2;
        this->heldTimer = (s32)(Rand_ZeroFloat(1.0f) * 10.0f) + 10;
    }

    this->actor.shape.rot.x = TRUNCF_BINANG(Rand_CenteredFloat(0x1388)) + this->actor.world.rot.x;
    this->actor.shape.rot.y = TRUNCF_BINANG(Rand_CenteredFloat(0x1388)) + this->actor.world.rot.y;
    this->actor.shape.rot.z = TRUNCF_BINANG(Rand_CenteredFloat(0x1388)) + this->actor.world.rot.z;
    if (this->niwType == NIW_TYPE_REGULAR) {
        if (Actor_HasNoParent(&this->actor, play)) {
            this->actor.shape.rot.z = 0;
            rotZ = this->actor.shape.rot.z;
            this->niwState = NIW_STATE_FALLING;
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->actionFunc = EnNiw_Thrown;
            this->actor.shape.rot.y = rotZ;
            this->actor.shape.rot.x = rotZ;
        }
    } else if (this->unk2BC.z != 0.0f) {
        this->actor.shape.rot.z = 0;
        rotZ = this->actor.shape.rot.z;
        this->actor.velocity.y = 8.0f;
        this->actor.speed = 4.0f;
        this->actor.gravity = -2.0f;
        this->niwState = NIW_STATE_FALLING;
        this->unk2EC = 0;
        this->niwType = NIW_TYPE_REGULAR;
        this->actor.shape.rot.y = rotZ;
        this->actor.shape.rot.x = rotZ;
        Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
        Math_Vec3f_Copy(&this->unk2BC, &D_808934DC);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actionFunc = EnNiw_Thrown;
    }

    EnNiw_AnimateWingHead(this, play, NIW_ANIM_PECKING_AND_WAVING);
}

void EnNiw_Thrown(EnNiw* this, PlayState* play) {
    if (this->unk2EC == 0) {
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            // wait until back on floor
            return;
        }
        this->unk2EC = 1;
        this->hoppingTimer = 80;
        this->actor.speed = 0.0f;
        this->actor.velocity.y = 4.0f;
    } else {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->sfxTimer1 = 0;
            this->actor.velocity.y = 4.0f; // vertical hop
            this->unk29E = 1;
        }
        if (this->hoppingTimer == 0) {
            this->runAwayTimer = 100;
            this->heldTimer = 0;
            this->unk2EC = 0;
            EnNiw_SetupRunAway(this);
            return;
        }
    }

    if (Actor_HasParent(&this->actor, play)) {
        // picked up again before could run off
        Actor_PlaySfx(&this->actor, NA_SE_EV_CHICKEN_CRY_M); // crow
        this->sfxTimer1 = 30;
        this->unk2EC = 0;
        this->heldTimer = 30;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->niwState = NIW_STATE_HELD;
        this->actionFunc = EnNiw_Held;
        this->actor.speed = 0.0f;
    } else {
        if (this->hoppingTimer > 5) {
            Actor_OfferCarry(&this->actor, play);
        }
        EnNiw_AnimateWingHead(this, play, NIW_ANIM_PECKING_AND_WAVING);
    }
}

void EnNiw_Swimming(EnNiw* this, PlayState* play) {
    Vec3f ripplePos;

    // even if hitting water, keep calling for reinforcements
    // this should just be in update
    if (this->isStormActive) {
        EnNiw_SpawnAttackNiw(this, play); // spawn attack niw
    }

    this->actor.speed = 2.0f;
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
        this->actor.gravity = 0.0f;
        if (this->actor.depthInWater > 15.0f) {
            this->actor.world.pos.y += 2.0f;
        }
        if (this->swimRippleTimer == 0) {
            this->swimRippleTimer = 30;
            Math_Vec3f_Copy(&ripplePos, &this->actor.world.pos);
            ripplePos.y += this->actor.depthInWater;

            EffectSsGRipple_Spawn(play, &ripplePos, 100, 500, 30);
        }
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            this->actor.velocity.y = 10.0f; // fly up in straight line
            this->actor.speed = 1.0f;
        }
    } else {
        this->actor.gravity = -2.0f;
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            this->actor.velocity.y = 10.0f; // fly up in straight line
            this->actor.speed = 1.0f;
            this->actor.gravity = 0.0f;
        } else {
            this->actor.speed = 4.0f;
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->actor.gravity = -2.0f;
            this->runAwayTimer = 100;
            this->swimRippleTimer = 0;
            this->actor.velocity.y = 0.0f;
            if (!this->isStormActive) {
                EnNiw_SetupRunAway(this);
            } else {
                this->niwState = NIW_STATE_ANGRY3;
                this->actionFunc = EnNiw_CuccoStorm;
            }
        }
    }

    EnNiw_AnimateWingHead(this, play, NIW_ANIM_PECKING_AND_WAVING);
}

void EnNiw_Trigger(EnNiw* this, PlayState* play) {
    this->cuccoStormTimer = 10;
    this->nextAnimIndex = NIW_ANIM_HEAD_PECKING;
    this->niwState = NIW_STATE_ANGRY1;
    this->actionFunc = EnNiw_Upset;
}

void EnNiw_Upset(EnNiw* this, PlayState* play) {
    // assumption: CuccoStorm is split into smaller parts because it used to be a cutscene in OOT
    this->sfxTimer1 = 100;
    if (this->cuccoStormTimer == 0) {
        this->cuccoStormTimer = 60;
        this->unkTimer24C = 10;
        this->nextAnimIndex = NIW_ANIM_FREEZE;
        this->niwState = NIW_STATE_ANGRY2;
        this->actionFunc = EnNiw_SetupCuccoStorm;
    }

    EnNiw_AnimateWingHead(this, play, this->nextAnimIndex);
}

// the long crow with head back before they descend
void EnNiw_SetupCuccoStorm(EnNiw* this, PlayState* play) {
    f32 viewY;

    this->sfxTimer1 = 100;
    if (this->cuccoStormTimer == 40) {
        viewY = 14000.0f;
        this->targetLimbRots[0] = 10000.0f; // body rot
        this->targetLimbRots[7] = this->targetLimbRots[5] = viewY;
        this->targetLimbRots[6] = 0.0f;
        this->targetLimbRots[8] = 0.0f;
        this->targetLimbRots[1] = 0.0f;
        this->targetLimbRots[2] = 0.0f;
        this->unkTimer24C = 10;
        Actor_PlaySfx(&this->actor, NA_SE_EV_CHICKEN_CRY_M);
    }

    if (this->cuccoStormTimer == 0) {
        this->cuccoStormTimer = 10;
        this->yawTowardsPlayer = this->actor.yawTowardsPlayer;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->niwState = NIW_STATE_ANGRY3;
        this->actionFunc = EnNiw_CuccoStorm;
    }

    EnNiw_AnimateWingHead(this, play, this->nextAnimIndex);
}

void EnNiw_CuccoStorm(EnNiw* this, PlayState* play) {
    EnNiw_SpawnAttackNiw(this, play);
    if (this->cuccoStormTimer == 1) { // not countdown to 0? mistype?
        this->actor.speed = 3.0f;
        this->isRunningRight = Rand_ZeroFloat(1.99f);
        this->generalTimer1 = 0;
        this->unkTimer24E = this->generalTimer1;
        this->unkTimer24C = this->generalTimer1;
    } else {
        EnNiw_UpdateRunning(this, play, true);
    }
}

void EnNiw_SetupRunAway(EnNiw* this) {
    Animation_Change(&this->skelAnime, &gNiwIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gNiwIdleAnim), ANIMMODE_LOOP,
                     -10.0f);
    this->isRunningRight = Rand_ZeroFloat(1.99f);
    this->niwState = NIW_STATE_RUNNING;
    this->actionFunc = EnNiw_RunAway;
    this->actor.speed = 4.0f;
}

void EnNiw_RunAway(EnNiw* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f D_808934E8 = { 90000.0f, 90000.0f, 90000.0f };
    s16 temp298;
    f32 dX;
    f32 dZ;

    if (this->runAwayTimer == 0) {
        this->unk2A4.x = this->unk2B0.x = this->actor.world.pos.x;
        this->unk2A4.y = this->unk2B0.y = this->actor.world.pos.y;
        this->unk2A4.z = this->unk2B0.z = this->actor.world.pos.z;
        this->generalTimer2 = this->generalTimer1 = this->unk298 = 0;
        this->unk300 = this->unk304 = 0;
        this->actor.speed = 0;
        this->targetLimbRots[8] = 0;
        this->targetLimbRots[6] = 0;
        this->targetLimbRots[5] = 0;
        this->targetLimbRots[7] = 0;
        Math_Vec3f_Copy(&this->unk2BC, &D_808934E8);

        EnNiw_SetupIdle(this);

    } else {
        if (this->unk2BC.x != 90000.0f) {
            dX = this->actor.world.pos.x - this->unk2BC.x;
            dZ = this->actor.world.pos.z - this->unk2BC.z;
        } else {
            dX = this->actor.world.pos.x - player->actor.world.pos.x;
            dZ = this->actor.world.pos.z - player->actor.world.pos.z;
        }
        this->yawTowardsPlayer = Math_Atan2S(dX, dZ);
        EnNiw_UpdateRunning(this, play, false);
        EnNiw_AnimateWingHead(this, play, NIW_ANIM_PECKING_AND_WAVING);
    }
}

void EnNiw_LandBeforeIdle(EnNiw* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        EnNiw_SetupIdle(this);
    }
}

void EnNiw_CheckRage(EnNiw* this, PlayState* play) {
    if (!this->isStormActive && (this->iframeTimer == 0) && (this->niwType == NIW_TYPE_REGULAR)) {

        // is this used? this is before we even know if we've been hit
        if ((this->niwState != NIW_STATE_RUNNING) && (this->unk2BC.x != 90000.0f)) {
            this->iframeTimer = 10;
            this->sfxTimer1 = 30;
            this->unk29E = 1;
            Actor_PlaySfx(&this->actor, NA_SE_EV_CHICKEN_CRY_M); // crow
            this->runAwayTimer = 100;
            this->unk2EC = 0;
            EnNiw_SetupRunAway(this);
        }

        if (this->collider.base.acFlags & AC_HIT) {
            this->collider.base.acFlags &= ~AC_HIT;
            // health gets used as a hit counter until cucco storm
            if (this->actor.colChkInfo.health > 0) {
                this->actor.colChkInfo.health--;
            }

            if (!sCuccoStormActive && (this->actor.colChkInfo.health == 0)) {
                this->runAwayTimer = 100; // main cucco will run away after storm starts
                sCuccoStormActive = true;
                this->unk298 = 0;
                this->sfxTimer1 = 10000;
                this->unk2A4.x = this->unk2B0.x = this->actor.world.pos.x;
                this->unk2A4.y = this->unk2B0.y = this->actor.world.pos.y;
                this->unk2A4.z = this->unk2B0.z = this->actor.world.pos.z;
                this->generalTimer2 = this->generalTimer1 = this->unk298;

                this->targetLimbRots[8] = 0.0f;
                this->targetLimbRots[6] = 0.0f;
                this->targetLimbRots[5] = 0.0f;
                this->targetLimbRots[7] = 0.0f;
                this->isStormActive = true;
                this->actionFunc = EnNiw_Trigger;
                this->unk304 = 0.0f;
                this->unk300 = 0.0f;
                this->actor.speed = 0.0f;

            } else {
                this->iframeTimer = 10;
                this->sfxTimer1 = 30;
                this->unk29E = 1;
                Actor_PlaySfx(&this->actor, NA_SE_EV_CHICKEN_CRY_M); // crow
                this->runAwayTimer = 100;
                this->unk2EC = 0;
                EnNiw_SetupRunAway(this);
            }
        }
    }
}

void EnNiw_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnNiw* this = (EnNiw*)thisx;
    Player* player = GET_PLAYER(play);
    s16 i;
    s16 featherCount;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    s32 pad[11];
    f32 featherScale;
    f32 viewAtToEyeNormY;
    f32 floorHeight;
    f32 dist = 20.0f;

    this->unusedCounter28C++;

    if (this->niwType == NIW_TYPE_UNK1) {
        this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.parent->shape.rot.y;
    }

    if (this->niwState != NIW_STATE_IDLE) {
        this->targetLimbRots[9] = 0.0f; // head rot
    }

    if (this->unk29E != 0) {
        featherCount = NIW_FEATHER_COUNT;
        if (this->unk29E == 2) {
            featherCount = 4;
        }
        for (i = 0; i < featherCount; i++) {
            pos.x = this->actor.world.pos.x + Rand_CenteredFloat(10.0f);
            pos.y = this->actor.world.pos.y + this->unk308 + Rand_CenteredFloat(10.0f);
            pos.z = this->actor.world.pos.z + Rand_CenteredFloat(10.0f);
            featherScale = Rand_ZeroFloat(6.0f) + 6.0f;

            if ((this->unk29E == 2) && (this->unk308 != 0)) {
                pos.y += 10.0f;
            }

            if (this->unk308 == 0) {
                featherScale = Rand_ZeroFloat(2.0f) + 2.0f;
            }
            velocity.x = Rand_CenteredFloat(3.0f);
            velocity.y = Rand_ZeroFloat(2.0f) * 0.5f + 2.0f;
            velocity.z = Rand_CenteredFloat(3.0f);
            accel.z = accel.x = 0.0f;
            accel.y = -0.15f;

            EnNiw_SpawnFeather(this, &pos, &velocity, &accel, featherScale);
        }
        this->unk29E = 0;
    }

    EnNiw_UpdateFeather(this, play);

    DECR(this->unkTimer24C);
    DECR(this->unkTimer24E);
    DECR(this->generalTimer1);
    DECR(this->generalTimer2);
    DECR(this->runAwayTimer);
    DECR(this->sfxTimer1);
    DECR(this->flutterSfxTimer);
    DECR(this->unusedTimer25A);
    DECR(this->yawTimer);
    DECR(this->unkAttackNiwTimer);
    DECR(this->iframeTimer);

    this->actor.shape.rot = this->actor.world.rot;
    this->actor.shape.shadowScale = 15.0f;
    this->actionFunc(this, play);
    Actor_SetFocus(&this->actor, this->unk308);
    Actor_MoveWithGravity(&this->actor);

    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 60.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);

    // if cucco is off the map
    if ((this->actor.floorHeight <= BGCHECK_Y_MIN) || (this->actor.floorHeight >= BGCHECK_Y_MAX)) {
        Vec3f viewAtToEye;

        // Direction vector for the direction the camera is facing
        viewAtToEye.x = play->view.at.x - play->view.eye.x;
        viewAtToEye.y = play->view.at.y - play->view.eye.y;
        viewAtToEye.z = play->view.at.z - play->view.eye.z;
        viewAtToEyeNormY = viewAtToEye.y / sqrtf(SQXYZ(viewAtToEye));

        this->actor.world.pos.x = this->actor.home.pos.x;
        this->actor.world.pos.z = this->actor.home.pos.z;
        this->actor.world.pos.y = this->actor.home.pos.y + play->view.eye.y + (viewAtToEyeNormY * 160.0f);

        if (this->actor.world.pos.y < this->actor.home.pos.y) {
            this->actor.world.pos.y = this->actor.home.pos.y + 300.0f;
        }

        this->actor.speed = 0.0f;
        this->actor.gravity = -2.0f;
        Math_Vec3f_Copy(&this->unk2A4, &this->actor.home.pos);
        Math_Vec3f_Copy(&this->unk2B0, &this->actor.home.pos);

        this->unk304 = 0.0f;
        this->unk300 = 0.0f;
        this->unusedFloat2FC = 0.0f; // assigned here but never used after
        this->unusedFloat2F8 = 0.0f;
        this->unusedFloat2F4 = 0.0f;
        this->rightWingRotX = 0.0f;
        this->rightWingRotY = 0.0f;
        this->rightWingRotZ = 0.0f;
        this->leftWingRotX = 0.0f;
        this->leftWingRotY = 0.0f;
        this->leftWingRotZ = 0.0f;
        this->upperBodyRotY = 0.0f;
        this->headRotY = 0.0f;

        // clang-format off
        this->isStormActive = this->unusedCounter28C = this->unk292 = this->unk29E = this->unk298 = this->isRunningRight = this->nextAnimIndex = 0;
        // clang-format on

        for (i = 0; i < ARRAY_COUNT(this->targetLimbRots); i++) {
            this->targetLimbRots[i] = 0.0f;
        }

        this->niwState = NIW_STATE_HOPPING;
        this->isStormActive = false;
        this->actionFunc = EnNiw_LandBeforeIdle;
        return;
    }

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WATER) && (this->actor.depthInWater > 15.0f) &&
        (this->niwState != NIW_STATE_SWIMMING)) {
        this->actor.velocity.y = 0.0f;
        this->actor.gravity = 0.0f;
        Math_Vec3f_Copy(&pos, &this->actor.world.pos);
        pos.y += this->actor.depthInWater;
        this->swimRippleTimer = 30;
        EffectSsGSplash_Spawn(play, &pos, 0, 0, 0, 400);
        this->generalTimer2 = 0;
        this->niwState = NIW_STATE_SWIMMING;
        this->actionFunc = EnNiw_Swimming;
        return;
    }

    if (this->isStormActive && (this->actor.xyzDistToPlayerSq < SQ(dist)) && (player->invincibilityTimer == 0)) {
        func_800B8D50(play, &this->actor, 2.0f, this->actor.world.rot.y, 0.0f, 0x10);
    }

    EnNiw_CheckRage(this, play);

    if ((this->flutterSfxTimer == 0) && (this->niwState == NIW_STATE_HELD)) {
        this->flutterSfxTimer = 7;
        Actor_PlaySfx(&this->actor, NA_SE_EN_CHICKEN_FLUTTER);
    }

    if (this->sfxTimer1 == 0) {
        if (this->niwState != NIW_STATE_IDLE) {
            this->sfxTimer1 = 30;
            Actor_PlaySfx(&this->actor, NA_SE_EV_CHICKEN_CRY_A);
        } else {
            this->sfxTimer1 = 300;
            Actor_PlaySfx(&this->actor, NA_SE_EV_CHICKEN_CRY_N);
        }
    }

    if (!this->isStormActive && (this->niwType == NIW_TYPE_REGULAR)) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);

        if ((this->niwState != NIW_STATE_HELD) && (this->niwState != NIW_STATE_FALLING)) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

s32 EnNiw_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnNiw* this = (EnNiw*)thisx;

    if (limbIndex == NIW_LIMB_UPPER_BODY) {
        rot->y += TRUNCF_BINANG(this->upperBodyRotY);
    }
    if (limbIndex == NIW_LIMB_HEAD) {
        rot->y += TRUNCF_BINANG(this->headRotY);
    }
    if (limbIndex == NIW_LIMB_RIGHT_WING_ROOT) {
        rot->x += TRUNCF_BINANG(this->rightWingRotX);
        rot->y += TRUNCF_BINANG(this->rightWingRotY);
        rot->z += TRUNCF_BINANG(this->rightWingRotZ);
    }
    if (limbIndex == NIW_LIMB_LEFT_WING_ROOT) {
        rot->x += TRUNCF_BINANG(this->leftWingRotX);
        rot->y += TRUNCF_BINANG(this->leftWingRotY);
        rot->z += TRUNCF_BINANG(this->leftWingRotZ);
    }
    return false;
}

void EnNiw_Draw(Actor* thisx, PlayState* play) {
    EnNiw* this = (EnNiw*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnNiw_OverrideLimbDraw, NULL, &this->actor);
    EnNiw_DrawFeathers(this, play);
}

void EnNiw_SpawnFeather(EnNiw* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale) {
    s16 i;
    EnNiwFeather* feather = &this->feathers[0];

    for (i = 0; i < ARRAY_COUNT(this->feathers); i++, feather++) {
        if (!feather->isEnabled) {
            feather->isEnabled = true;
            feather->pos = *pos;
            feather->velocity = *velocity;
            feather->accel = *accel;
            feather->timer = 0;
            feather->scale = scale / 1000.0f;
            feather->life = Rand_ZeroFloat(20.0f) + 40.0f;
            feather->zRotStart = Rand_ZeroFloat(1000.0f);
            break;
        }
    }
}

void EnNiw_UpdateFeather(EnNiw* this, PlayState* play) {
    EnNiwFeather* feather = &this->feathers[0];
    f32 featherVelocityGoal = 0.05f;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(this->feathers); i++, feather++) {
        if (feather->isEnabled) {
            feather->timer++;
            feather->pos.x += feather->velocity.x;
            feather->pos.y += feather->velocity.y;
            feather->pos.z += feather->velocity.z;
            feather->velocity.x += feather->accel.x;
            feather->velocity.y += feather->accel.y;
            feather->velocity.z += feather->accel.z;
            if (feather->isEnabled == true) {
                feather->zRotStart++;
                Math_ApproachF(&feather->velocity.x, 0.0f, 1.0f, featherVelocityGoal);
                Math_ApproachF(&feather->velocity.z, 0.0f, 1.0f, featherVelocityGoal);
                if (feather->velocity.y < -0.5f) {
                    feather->velocity.y = -0.5f;
                }

                feather->zRot = Math_SinS(feather->zRotStart * 0xBB8) * M_PIf * 0.2f;

                if (feather->life < feather->timer) {
                    feather->isEnabled = false;
                }
            }
        }
    }
}

void EnNiw_DrawFeathers(EnNiw* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    u8 isMaterialApplied = false;
    EnNiwFeather* feather = &this->feathers[0];
    s16 i;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(this->feathers); i++, feather++) {
        if (feather->isEnabled == true) {
            // Apply the feather material if it has not already been applied.
            if (!isMaterialApplied) {
                gSPDisplayList(POLY_XLU_DISP++, gNiwFeatherMaterialDL);
                isMaterialApplied++;
            }

            Matrix_Translate(feather->pos.x, feather->pos.y, feather->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(feather->scale, feather->scale, 1.0f, MTXMODE_APPLY);
            Matrix_RotateZF(feather->zRot, MTXMODE_APPLY);
            Matrix_Translate(0.0f, -1000.0f, 0.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);

            gSPDisplayList(POLY_XLU_DISP++, gNiwFeatherDL);
        }
    }

    CLOSE_DISPS(gfxCtx);
}

void EnNiw_Reset(void) {
    sCuccoStormActive = false;
}
