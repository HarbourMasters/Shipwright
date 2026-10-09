/*
 * File: z_en_bat.c
 * Overlay: ovl_En_Bat
 * Description: Bad Bat
 */

#include "z_en_bat.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "objects/object_bat/object_bat.h"

#define FLAGS \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_IGNORE_QUAKE | ACTOR_FLAG_CAN_ATTACH_TO_ARROW)

#define BAD_BAT_FLAP_FRAME 5

void EnBat_Init(Actor* thisx, PlayState* play);
void EnBat_Destroy(Actor* thisx, PlayState* play);
void EnBat_Update(Actor* thisx, PlayState* play);
void EnBat_Draw(Actor* thisx, PlayState* play);
void EnBat_Reset(void);

s32 EnBat_IsGraveyardOnSecondDay(PlayState* play);
void EnBat_SetupPerch(EnBat* this);
void EnBat_Perch(EnBat* this, PlayState* play);
void EnBat_SetupFlyIdle(EnBat* this);
void EnBat_FlyIdle(EnBat* this, PlayState* play);
void EnBat_SetupDiveAttack(EnBat* this);
void EnBat_DiveAttack(EnBat* this, PlayState* play);
void EnBat_Die(EnBat* this, PlayState* play);
void EnBat_Stunned(EnBat* this, PlayState* play);

ActorProfile En_Bat_Profile = {
    /**/ ACTOR_EN_BAT,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_BAT,
    /**/ sizeof(EnBat),
    /**/ EnBat_Init,
    /**/ EnBat_Destroy,
    /**/ EnBat_Update,
    /**/ EnBat_Draw,
    /**/ EnBat_Reset,
};

static ColliderSphereInit sSphereInit = {
    {
        COL_MATERIAL_HIT3,
        AT_NONE | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_SPHERE,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x04 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_HARD,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 1, { { 0, 0, 0 }, 15 }, 100 },
};

typedef enum {
    /* 0 */ BAD_BAT_DMGEFF_NONE,
    /* 1 */ BAD_BAT_DMGEFF_STUN,
    /* 2 */ BAD_BAT_DMGEFF_FIRE,
    /* 3 */ BAD_BAT_DMGEFF_ICE,
    /* 4 */ BAD_BAT_DMGEFF_LIGHT,
    /* 5 */ BAD_BAT_DMGEFF_ELECTRIC
} BatDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, BAD_BAT_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, BAD_BAT_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(1, BAD_BAT_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, BAD_BAT_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, BAD_BAT_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, BAD_BAT_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, BAD_BAT_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(1, BAD_BAT_DMGEFF_NONE),
    /* Goron punch    */ DMG_ENTRY(1, BAD_BAT_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(1, BAD_BAT_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(1, BAD_BAT_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(2, BAD_BAT_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(2, BAD_BAT_DMGEFF_ICE),
    /* Light arrow    */ DMG_ENTRY(2, BAD_BAT_DMGEFF_LIGHT),
    /* Goron spikes   */ DMG_ENTRY(1, BAD_BAT_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(1, BAD_BAT_DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(1, BAD_BAT_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(2, BAD_BAT_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, BAD_BAT_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(0, BAD_BAT_DMGEFF_ELECTRIC),
    /* Normal shield  */ DMG_ENTRY(0, BAD_BAT_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, BAD_BAT_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, BAD_BAT_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, BAD_BAT_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, BAD_BAT_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, BAD_BAT_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, BAD_BAT_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, BAD_BAT_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, BAD_BAT_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, BAD_BAT_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, BAD_BAT_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, BAD_BAT_DMGEFF_NONE),
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 15, 30, 10 };

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, TATL_HINT_ID_BAD_BAT, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 3000, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -500, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_STOP),
};

static Gfx* sWingsDLs[] = {
    gBadBatWingsFrame0DL, gBadBatWingsFrame1DL, gBadBatWingsFrame2DL, gBadBatWingsFrame3DL, gBadBatWingsFrame4DL,
    gBadBatWingsFrame5DL, gBadBatWingsFrame6DL, gBadBatWingsFrame7DL, gBadBatWingsFrame8DL,
};

#define BAD_BAT_MAX_NUMBER_ATTACKING 3

s32 sNumberAttacking; //!< Limit number attacking player to at most `BAD_BAT_MAX_NUMBER_ATTACKING`
s32 sAlreadySpawned;  //!< used for those spawned with room -1 in Graveyard to avoid respawn on room change

void EnBat_Init(Actor* thisx, PlayState* play) {
    EnBat* this = (EnBat*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    Collider_InitAndSetSphere(play, &this->collider, thisx, &sSphereInit);
    this->collider.dim.worldSphere.radius = sSphereInit.dim.modelSphere.radius;
    CollisionCheck_SetInfo(&thisx->colChkInfo, &sDamageTable, &sColChkInfoInit);
    ActorShape_Init(&thisx->shape, 2000.0f, ActorShadow_DrawCircle, 25.0f);

    this->animationFrame = Rand_ZeroOne() * 9.0f;

    this->paramFlags = BAD_BAT_GET_PARAMFLAGS(thisx);
    this->switchFlag = BAD_BAT_GET_SWITCH_FLAG(thisx);
    thisx->params = BAD_BAT_GET_TYPE(thisx);

    thisx->depthInWater = BGCHECK_Y_MIN;
    Actor_SetFocus(thisx, 20.0f);

    if (sAlreadySpawned) {
        Actor_Kill(thisx);
    } else if (EnBat_IsGraveyardOnSecondDay(play)) {
        if (Flags_GetSwitch(play, this->switchFlag)) {
            Actor_Kill(thisx);
        } else {
            this->actor.room = -1;
        }
    }

    if (this->paramFlags & BAD_BAT_PARAMFLAG_PERCH) {
        thisx->params = 0;
        EnBat_SetupPerch(this);
    } else {
        if (thisx->params == 0x1F) {
            thisx->params = 0;
        }
        EnBat_SetupFlyIdle(this);
        while (BAD_BAT_GET_NUMBER_TO_SPAWN(thisx) > 1) {
            Actor_SpawnAsChildAndCutscene(
                &play->actorCtx, play, ACTOR_EN_BAT, thisx->world.pos.x + Rand_CenteredFloat(200.0f),
                thisx->world.pos.y + Rand_CenteredFloat(100.0f), thisx->world.pos.z + Rand_CenteredFloat(200.0f),
                Rand_CenteredFloat(0x2000), 0xFFFF * Rand_ZeroOne(), 0,
                BAD_BAT_PARAMS(this->switchFlag, this->paramFlags, 0), CS_ID_NONE, thisx->halfDaysBits, NULL);
            BAD_BAT_GET_NUMBER_TO_SPAWN(thisx)--;
        }
    }
}

void EnBat_Destroy(Actor* thisx, PlayState* play) {
    EnBat* this = (EnBat*)thisx;

    Collider_DestroySphere(play, &this->collider);
}

s32 EnBat_IsGraveyardOnSecondDay(PlayState* play) {
    if ((CURRENT_DAY == 2) && (play->sceneId == SCENE_BOTI)) {
        return true;
    } else {
        return false;
    }
}

void EnBat_StepAnimation(EnBat* this, s32 frameStep) {
    s32 prevFrame = this->animationFrame;

    this->animationFrame += frameStep;
    if (this->animationFrame >= ARRAY_COUNT(sWingsDLs)) {
        this->animationFrame -= ARRAY_COUNT(sWingsDLs);
    }
    if ((prevFrame < BAD_BAT_FLAP_FRAME) && (this->animationFrame >= BAD_BAT_FLAP_FRAME)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_FFLY_FLY);
    }
}

void EnBat_SetupPerch(EnBat* this) {
    this->collider.base.acFlags |= AC_ON;
    this->collider.base.atFlags |= AT_ON;
    this->collider.dim.worldSphere.center.x = this->actor.focus.pos.x;
    this->collider.dim.worldSphere.center.y = this->actor.focus.pos.y;
    this->collider.dim.worldSphere.center.z = this->actor.focus.pos.z;
    this->actor.speed = 0.0f;
    this->actionFunc = EnBat_Perch;
}

void EnBat_Perch(EnBat* this, PlayState* play) {
    EnBat_StepAnimation(this, 1);
}

void EnBat_SetupFlyIdle(EnBat* this) {
    this->timer = 100;
    this->collider.base.acFlags |= AC_ON;
    this->actor.speed = 3.5f;
    this->actionFunc = EnBat_FlyIdle;
}

void EnBat_FlyIdle(EnBat* this, PlayState* play) {
    s32 finishedRotStep;

    EnBat_StepAnimation(this, 1);

    finishedRotStep = Math_ScaledStepToS(&this->actor.shape.rot.y, this->yawTarget, 0x300);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
        this->yawTarget = this->actor.wallYaw;
    } else if (Math3D_Dist2DSq(this->actor.world.pos.x, this->actor.world.pos.z, this->actor.home.pos.x,
                               this->actor.home.pos.z) > SQ(300.0f)) {
        this->yawTarget = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    } else if (finishedRotStep && (Rand_ZeroOne() < 0.015f)) {
        this->yawTarget =
            this->actor.shape.rot.y + (((s32)(0x1000 * Rand_ZeroOne()) + 0x1000) * ((Rand_ZeroOne() < 0.5f) ? -1 : 1));
    }

    finishedRotStep = Math_ScaledStepToS(&this->actor.shape.rot.x, this->pitchTarget, 0x100);

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->actor.depthInWater > -40.0f)) {
        this->pitchTarget = -0x1000;
    } else if (this->actor.world.pos.y < (this->actor.home.pos.y - 100.0f)) {
        this->pitchTarget = -((s32)(0x800 * Rand_ZeroOne()) + 0x800);
    } else if ((this->actor.home.pos.y + 100.0f) < this->actor.world.pos.y) {
        this->pitchTarget = (s32)(0x800 * Rand_ZeroOne()) + 0x800;
    } else if ((finishedRotStep) && (Rand_ZeroOne() < 0.015f)) {
        this->pitchTarget += (s16)(((s32)(0x400 * Rand_ZeroOne()) + 0x400) * ((Rand_ZeroOne() < 0.5f) ? -1 : 1));
        this->pitchTarget = CLAMP(this->pitchTarget, -0x1000, 0x1000);
    }

    if (this->timer != 0) {
        this->timer--;
    }
    if ((this->actor.xzDistToPlayer < 300.0f) && (this->timer == 0) && (Player_GetMask(play) != PLAYER_MASK_STONE) &&
        (sNumberAttacking < BAD_BAT_MAX_NUMBER_ATTACKING) &&
        (!(this->paramFlags & BAD_BAT_PARAMFLAG_CHECK_HEIGHTREL) || (fabsf(this->actor.playerHeightRel) < 150.0f))) {
        EnBat_SetupDiveAttack(this);
    }
}

void EnBat_SetupDiveAttack(EnBat* this) {
    this->collider.base.atFlags |= AT_ON;
    this->timer = 300;
    this->actor.speed = 4.0f;
    sNumberAttacking++;
    this->actionFunc = EnBat_DiveAttack;
}

void EnBat_DiveAttack(EnBat* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 isFacingPlayer;
    Vec3f preyPos;

    EnBat_StepAnimation(this, 2);
    isFacingPlayer = Actor_IsFacingPlayer(&this->actor, 0x2800);

    if (isFacingPlayer) {
        s16 pitchTarget;

        preyPos.x = player->actor.world.pos.x;
        preyPos.y = player->actor.world.pos.y + 20.0f;
        preyPos.z = player->actor.world.pos.z;

        pitchTarget = Actor_WorldPitchTowardPoint(&this->actor, &preyPos);
        pitchTarget = CLAMP(pitchTarget, -0x3000, 0x3000);
        Math_SmoothStepToS(&this->actor.shape.rot.x, pitchTarget, 2, 0x400, 0x40);
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.x, -0x800, 2, 0x100, 0x10);
    }
    if (isFacingPlayer || (this->actor.xzDistToPlayer > 80.0f)) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0xC00, 0xC0);
    }

    this->timer--;

    if ((this->timer == 0) || (this->collider.base.atFlags & AT_HIT) || (Player_GetMask(play) == PLAYER_MASK_STONE) ||
        (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (player->stateFlags1 & PLAYER_STATE1_800000) ||
        (this->actor.depthInWater > -40.0f)) {
        if (this->collider.base.atFlags & AT_HIT) {
            this->collider.base.atFlags &= ~AT_HIT;
            Actor_PlaySfx(&this->actor, NA_SE_EN_FFLY_ATTACK);
        }
        this->collider.base.atFlags &= ~AT_ON;
        sNumberAttacking--;
        EnBat_SetupFlyIdle(this);
    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) &&
               (ABS_ALT(BINANG_SUB(this->actor.wallYaw, this->actor.yawTowardsPlayer)) > 0x6800)) {
        sNumberAttacking--;
        this->collider.base.atFlags &= ~AT_ON;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
        this->yawTarget = this->actor.wallYaw;
        EnBat_SetupFlyIdle(this);
    }
}

void EnBat_SetupDie(EnBat* this, PlayState* play) {
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Enemy_StartFinishingBlow(play, &this->actor);
    this->actor.speed *= Math_CosS(this->actor.world.rot.x);
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    this->actor.velocity.y = 0.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_FFLY_DEAD);

    if (this->actor.colChkInfo.damageEffect == BAD_BAT_DMGEFF_ICE) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
        this->drawDmgEffAlpha = 1.0f;
        this->drawDmgEffFrozenSteamScale = 60.0f * (0.45f / 40.0f);
        this->drawDmgEffScale = 0.45f;
    } else if (this->actor.colChkInfo.damageEffect == BAD_BAT_DMGEFF_LIGHT) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
        this->drawDmgEffAlpha = 4.0f;
        this->drawDmgEffScale = 0.45f;
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->collider.elem.acDmgInfo.hitPos.x,
                    this->collider.elem.acDmgInfo.hitPos.y, this->collider.elem.acDmgInfo.hitPos.z, 0, 0, 0,
                    CLEAR_TAG_PARAMS(CLEAR_TAG_SMALL_LIGHT_RAYS));
    } else if (this->actor.colChkInfo.damageEffect == BAD_BAT_DMGEFF_FIRE) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->drawDmgEffAlpha = 4.0f;
        this->drawDmgEffScale = 0.45f;
    }

    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 40);

    if (this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW) {
        this->actor.speed = 0.0f;
    }

    this->collider.base.acFlags &= ~AC_ON;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actionFunc = EnBat_Die;
}

void EnBat_Die(EnBat* this, PlayState* play) {
    Math_StepToF(&this->actor.speed, 0.0f, 0.5f);
    this->actor.colorFilterTimer = 40;

    if (!(this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW)) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            Math_ScaledStepToS(&this->actor.shape.rot.x, 0x4000, 0x200);
            this->actor.shape.rot.z += 0x1780;
        }

        if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->actor.floorHeight == BGCHECK_Y_MIN)) {
            if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
                Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, BAD_BAT_BODYPART_MAX, 2, 0.2f, 0.2f);
            }

            func_800B3030(play, &this->actor.world.pos, &gZeroVec3f, &gZeroVec3f, 100, 0, 0);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 11, NA_SE_EN_EXTINCT);
            Actor_Kill(&this->actor);

            if (this->actor.room == -1) {
                Actor* enemy = NULL;

                // Search for other EnBats. If find none, set switch flag.
                do {
                    enemy = SubS_FindActor(play, enemy, ACTORCAT_ENEMY, ACTOR_EN_BAT);
                    if (enemy != NULL) {
                        if (enemy != &this->actor) {
                            break;
                        }
                        enemy = enemy->next;
                    }
                } while (enemy != NULL);

                if (enemy == NULL) {
                    Flags_SetSwitch(play, this->switchFlag);
                }
            }
        }
    }
}

void EnBat_SetupStunned(EnBat* this) {
    if (this->actionFunc != EnBat_Stunned) {
        this->actor.shape.yOffset = 700.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.speed = 0.0f;
        this->actor.world.pos.y += 13.0f;
    }

    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, this->timer);
    this->actionFunc = EnBat_Stunned;
}

void EnBat_Stunned(EnBat* this, PlayState* play) {
    Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x100);
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->actor.floorHeight == BGCHECK_Y_MIN)) {
        if (this->timer != 0) {
            this->timer--;
        }
        if (this->timer == 0) {
            EnBat_SetupFlyIdle(this);
        }
    } else {
        this->actor.colorFilterTimer = 40;
        if (this->drawDmgEffAlpha > 0.0f) {
            this->drawDmgEffAlpha = 2.0f;
        }
    }
}

void EnBat_UpdateDamage(EnBat* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;

        Actor_SetDropFlag(&this->actor, &this->collider.elem);
        this->collider.base.atFlags &= ~AT_ON;

        if (this->actionFunc == EnBat_DiveAttack) {
            sNumberAttacking--;
        }

        if (this->actor.colChkInfo.damageEffect == BAD_BAT_DMGEFF_ELECTRIC) {
            this->timer = 40;
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_LARGE;
            this->drawDmgEffAlpha = 2.0f;
            this->drawDmgEffScale = 0.45f;
            EnBat_SetupStunned(this);
        } else if (this->actor.colChkInfo.damageEffect == BAD_BAT_DMGEFF_STUN) {
            this->timer = 40;
            EnBat_SetupStunned(this);
        } else {
            Actor_ApplyDamage(&this->actor);
            EnBat_SetupDie(this, play);
        }
    }
}

void EnBat_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBat* this = (EnBat*)thisx;

    if (this->actor.room == -1) {
        sAlreadySpawned = true;
    }

    EnBat_UpdateDamage(this, play);
    this->actionFunc(this, play);

    if (this->actionFunc != EnBat_Stunned) {
        Math_StepToF(&this->actor.shape.yOffset, 2000.0f, 200.0f);
    }

    if (this->actionFunc != EnBat_Perch) {
        Vec3f prevPos;

        this->actor.world.rot.y = this->actor.shape.rot.y;
        this->actor.world.rot.x = -this->actor.shape.rot.x;
        Math_Vec3f_Copy(&prevPos, &this->actor.prevPos);

        if ((this->actor.colChkInfo.health != 0) && (this->actionFunc != EnBat_Stunned)) {
            Actor_MoveWithoutGravity(&this->actor);
        } else {
            Actor_MoveWithGravity(&this->actor);
        }

        if (this->actionFunc == EnBat_DiveAttack) {
            Actor_UpdateBgCheckInfo(play, &this->actor, 12.0f, 15.0f, 50.0f,
                                    UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
        } else if ((this->actionFunc != EnBat_FlyIdle) ||
                   ((this->actor.xzDistToPlayer < 400.0f) && (this->actor.projectedPos.z > 0.0f))) {
            if (this->paramFlags & BAD_BAT_PARAMFLAG_0) {
                Actor_UpdateBgCheckInfo(play, &this->actor, 12.0f, 15.0f, 50.0f,
                                        UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
            } else {
                Actor_UpdateBgCheckInfo(play, &this->actor, 12.0f, 15.0f, 50.0f, UPDBGCHECKINFO_FLAG_4);
            }
        } else {
            Math_Vec3f_Copy(&this->actor.prevPos, &prevPos);
        }

        Actor_SetFocus(&this->actor, this->actor.shape.yOffset * 0.01f);
        this->collider.dim.worldSphere.center.x = this->actor.focus.pos.x;
        this->collider.dim.worldSphere.center.y = this->actor.focus.pos.y;
        this->collider.dim.worldSphere.center.z = this->actor.focus.pos.z;
    }

    if (this->collider.base.atFlags & AT_ON) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }
    if (this->actionFunc == EnBat_DiveAttack) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
    if (this->collider.base.acFlags & AC_ON) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->drawDmgEffAlpha > 0.0f) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
            this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.225f;
            this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 0.45f);
        } else if (!Math_StepToF(&this->drawDmgEffFrozenSteamScale, 0.45f, 0.45f / 40.0f)) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        }
    }
}

void EnBat_Draw(Actor* thisx, PlayState* play) {
    EnBat* this = (EnBat*)thisx;
    Gfx* gfx;

    // Draw body and wings
    if (this->actor.projectedPos.z > 0.0f) {
        OPEN_DISPS(play->state.gfxCtx);

        gfx = POLY_OPA_DISP;

        gSPDisplayList(&gfx[0], gSetupDLs[SETUPDL_25]);
        MATRIX_FINALIZE_AND_LOAD(&gfx[1], play->state.gfxCtx);
        gSPDisplayList(&gfx[2], gBadBatSetupDL);
        gSPDisplayList(&gfx[3], gBadBatBodyDL);
        gSPDisplayList(&gfx[4], sWingsDLs[this->animationFrame]);

        POLY_OPA_DISP = &gfx[5];

        CLOSE_DISPS(play->state.gfxCtx);
    }

    // Draw damage effects
    if (this->drawDmgEffAlpha > 0.0f) {
        s16 rollAngle;

        if (this->animationFrame < BAD_BAT_FLAP_FRAME - 1) {
            rollAngle = this->animationFrame * (15 * (0x10000 / 360));
        } else {
            rollAngle = (this->animationFrame >= BAD_BAT_FLAP_FRAME)
                            ? (this->animationFrame * (15 * (0x10000 / 360))) - (120 * (0x10000 / 360))
                            : 0;
        }
        Matrix_MultZero(&this->bodyPartsPos[BAD_BAT_BODYPART_0]);
        Matrix_RotateZS(rollAngle, MTXMODE_APPLY);
        Matrix_MultVecX(1700.0f, &this->bodyPartsPos[BAD_BAT_BODYPART_1]);
        Matrix_RotateZS(-2 * rollAngle, MTXMODE_APPLY);
        Matrix_MultVecX(-1700.0f, &this->bodyPartsPos[BAD_BAT_BODYPART_2]);
        Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, BAD_BAT_BODYPART_MAX, this->drawDmgEffScale,
                                this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha, this->drawDmgEffType);
    }
}

void EnBat_Reset(void) {
    sNumberAttacking = 0;
    sAlreadySpawned = false;
}
