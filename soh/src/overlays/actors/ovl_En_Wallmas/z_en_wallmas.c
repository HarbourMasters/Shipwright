/*
 * File: z_en_wallmas
 * Overlay: En_Wallmas
 * Description: Wallmaster (Ceiling monster)
 */

#include "z_en_wallmas.h"
#include "objects/object_wallmaster/object_wallmaster.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

#define TIMER_SCALE ((f32)OS_CLOCK_RATE / 10000000000)
#define DEGREE_60_RAD (60.0f * M_PI / 180.0f)
#define DEGREE_15_RAD (15.0f * M_PI / 180.0f)

#define DAMAGE_EFFECT_BURN 2
#define DAMAGE_EFFECT_STUN_WHITE 4
#define DAMAGE_EFFECT_STUN_BLUE 1

void EnWallmas_Init(Actor* thisx, PlayState* play);
void EnWallmas_Destroy(Actor* thisx, PlayState* play);
void EnWallmas_Update(Actor* thisx, PlayState* play);
void EnWallmas_Draw(Actor* thisx, PlayState* play);

void EnWallmas_TimerInit(EnWallmas* this, PlayState* play);
void EnWallmas_ProximityOrSwitchInit(EnWallmas* this);
void EnWallmas_WaitToDrop(EnWallmas* this, PlayState* play);
void EnWallmas_Drop(EnWallmas* this, PlayState* play);
void EnWallmas_Land(EnWallmas* this, PlayState* play);
void EnWallmas_Stand(EnWallmas* this, PlayState* play);
void EnWallmas_JumpToCeiling(EnWallmas* this, PlayState* play);
void EnWallmas_ReturnToCeiling(EnWallmas* this, PlayState* play);
void EnWallmas_TakeDamage(EnWallmas* this, PlayState* play);
void EnWallmas_Cooldown(EnWallmas* this, PlayState* play);
void EnWallmas_Die(EnWallmas* this, PlayState* play);
void EnWallmas_TakePlayer(EnWallmas* this, PlayState* play);
void EnWallmas_WaitForProximity(EnWallmas* this, PlayState* play);
void EnWallmas_WaitForSwitchFlag(EnWallmas* this, PlayState* play);
void EnWallmas_Stun(EnWallmas* this, PlayState* play);
void EnWallmas_Walk(EnWallmas* this, PlayState* play);

const ActorInit En_Wallmas_InitVars = {
    ACTOR_EN_WALLMAS,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_WALLMASTER,
    sizeof(EnWallmas),
    (ActorFunc)EnWallmas_Init,
    (ActorFunc)EnWallmas_Destroy,
    (ActorFunc)EnWallmas_Update,
    (ActorFunc)EnWallmas_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
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
    { 30, 40, 0, { 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 4, 30, 40, 150 };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(4, 0x2),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(4, 0x4),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(4, 0x2),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(4, 0x4),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x30, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 5500, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1500, ICHAIN_STOP),
};

void EnWallmas_Init(Actor* thisx, PlayState* play) {
    EnWallmas* this = (EnWallmas*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    ActorShape_Init(&thisx->shape, 0, NULL, 0.5f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gWallmasterSkel, &gWallmasterWaitAnim, this->jointTable,
                       this->morphTable, 25);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, thisx, &sCylinderInit);
    CollisionCheck_SetInfo(&thisx->colChkInfo, &sDamageTable, &sColChkInfoInit);
    this->switchFlag = (u8)(thisx->params >> 0x8);
    thisx->params = thisx->params & 0xFF;

    if (thisx->params == WMT_FLAG) {
        if (Flags_GetSwitch(play, this->switchFlag) != 0) {
            Actor_Kill(thisx);
            return;
        }

        EnWallmas_ProximityOrSwitchInit(this);
    } else if (thisx->params == WMT_PROXIMITY) {
        EnWallmas_ProximityOrSwitchInit(this);
    } else {
        EnWallmas_TimerInit(this, play);
    }
}

void EnWallmas_Destroy(Actor* thisx, PlayState* play) {
    EnWallmas* this = (EnWallmas*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnWallmas_TimerInit(EnWallmas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actor.flags &= ~ACTOR_FLAG_0;
    this->actor.flags |= ACTOR_FLAG_5;
    this->timer = 0x82;
    this->actor.velocity.y = 0.0f;
    this->actor.world.pos.y = player->actor.world.pos.y;
    this->actor.floorHeight = player->actor.floorHeight;
    this->actor.draw = EnWallmas_Draw;
    this->actionFunc = EnWallmas_WaitToDrop;
}

void EnWallmas_SetupDrop(EnWallmas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    AnimationHeader* objSegChangee = &gWallmasterLungeAnim;

    Animation_Change(&this->skelAnime, objSegChangee, 0.0f, 20.0f, Animation_GetLastFrame(&gWallmasterLungeAnim),
                     ANIMMODE_ONCE, 0.0f);

    this->yTarget = player->actor.world.pos.y;
    this->actor.world.pos.y = player->actor.world.pos.y + 300.0f;
    this->actor.world.rot.y = player->actor.shape.rot.y + 0x8000;
    this->actor.floorHeight = player->actor.floorHeight;
    this->actor.flags |= ACTOR_FLAG_0;
    this->actor.flags &= ~ACTOR_FLAG_5;
    this->actionFunc = EnWallmas_Drop;
}

void EnWallmas_SetupLand(EnWallmas* this, PlayState* play) {
    AnimationHeader* objSegFrameCount = &gWallmasterJumpAnim;
    AnimationHeader* objSegChangee = &gWallmasterJumpAnim;

    Animation_Change(&this->skelAnime, objSegChangee, 1.0f, 41.0f, Animation_GetLastFrame(objSegFrameCount),
                     ANIMMODE_ONCE, -3.0f);

    Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 15.0f, 6, 20.0f, 300, 100, true);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_FALL_LAND);
    this->actionFunc = EnWallmas_Land;
}

void EnWallmas_SetupStand(EnWallmas* this) {
    Animation_PlayOnce(&this->skelAnime, &gWallmasterStandUpAnim);
    this->actionFunc = EnWallmas_Stand;
}

void EnWallmas_SetupWalk(EnWallmas* this) {
    Animation_PlayOnceSetSpeed(&this->skelAnime, &gWallmasterWalkAnim, 3.0f);
    this->actionFunc = EnWallmas_Walk;
    this->actor.speedXZ = 3.0f;
}

void EnWallmas_SetupJumpToCeiling(EnWallmas* this) {
    Animation_PlayOnce(&this->skelAnime, &gWallmasterStopWalkAnim);
    this->actionFunc = EnWallmas_JumpToCeiling;
    this->actor.speedXZ = 0.0f;
}
void EnWallmas_SetupReturnToCeiling(EnWallmas* this) {
    AnimationHeader* objSegFrameCount = &gWallmasterJumpAnim;
    AnimationHeader* objSegChangee = &gWallmasterJumpAnim;

    this->timer = 0;
    this->actor.speedXZ = 0.0f;

    Animation_Change(&this->skelAnime, objSegChangee, 3.0f, 0.0f, Animation_GetLastFrame(objSegFrameCount),
                     ANIMMODE_ONCE, -3.0f);

    this->actionFunc = EnWallmas_ReturnToCeiling;
}

void EnWallmas_SetupTakeDamage(EnWallmas* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gWallmasterDamageAnim, -3.0f);
    if (this->collider.info.acHitInfo->toucher.dmgFlags & 0x0001F824) {
        this->actor.world.rot.y = this->collider.base.ac->world.rot.y;
    } else {
        this->actor.world.rot.y = Actor_WorldYawTowardActor(&this->actor, this->collider.base.ac) + 0x8000;
    }

    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 0x14);
    this->actionFunc = EnWallmas_TakeDamage;
    this->actor.speedXZ = 5.0f;
    this->actor.velocity.y = 10.0f;
}

void EnWallmas_SetupCooldown(EnWallmas* this) {
    Animation_PlayOnce(&this->skelAnime, &gWallmasterRecoverFromDamageAnim);
    this->actor.speedXZ = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = EnWallmas_Cooldown;
}

void EnWallmas_SetupDie(EnWallmas* this, PlayState* play) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    this->actor.speedXZ = 0.0f;
    this->actor.velocity.y = 0.0f;

    EffectSsDeadDb_Spawn(play, &this->actor.world.pos, &zeroVec, &zeroVec, 250, -10, 255, 255, 255, 255, 0, 0, 255,
                         1, 9, true);

    Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0xC0);
    this->actionFunc = EnWallmas_Die;
}

void EnWallmas_SetupTakePlayer(EnWallmas* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gWallmasterHoverAnim, -5.0f);
    this->timer = -0x1E;
    this->actionFunc = EnWallmas_TakePlayer;
    this->actor.speedXZ = 0.0f;
    this->actor.velocity.y = 0.0f;

    this->yTarget = this->actor.yDistToPlayer;
    func_8002DF38(play, &this->actor, 0x25);
    OnePointCutscene_Init(play, 9500, 9999, &this->actor, MAIN_CAM);
}

void EnWallmas_ProximityOrSwitchInit(EnWallmas* this) {
    this->timer = 0;
    this->actor.draw = NULL;
    this->actor.flags &= ~ACTOR_FLAG_0;
    if (this->actor.params == WMT_PROXIMITY) {
        this->actionFunc = EnWallmas_WaitForProximity;
    } else {
        this->actionFunc = EnWallmas_WaitForSwitchFlag;
    }
}

void EnWallmas_SetupStun(EnWallmas* this) {
    Animation_Change(&this->skelAnime, &gWallmasterJumpAnim, 1.5f, 0, 20.0f, ANIMMODE_ONCE, -3.0f);

    this->actor.speedXZ = 0.0f;
    if (this->actor.colChkInfo.damageEffect == 4) {
        Actor_SetColorFilter(&this->actor, -0x8000, 0xFF, 0, 0x50);
    } else {
        Actor_SetColorFilter(&this->actor, 0, 0xFF, 0, 0x50);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    }

    this->timer = 0x50;
    this->actionFunc = EnWallmas_Stun;
}

void EnWallmas_WaitToDrop(EnWallmas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f* playerPos = &player->actor.world.pos;

    this->actor.world.pos = *playerPos;
    this->actor.floorHeight = player->actor.floorHeight;
    this->actor.floorPoly = player->actor.floorPoly;

    if (this->timer != 0) {
        this->timer--;
    }

    if ((player->stateFlags1 & 0x100000) || (player->stateFlags1 & 0x8000000) || !(player->actor.bgCheckFlags & 1) ||
        ((this->actor.params == 1) && (320.0f < Math_Vec3f_DistXZ(&this->actor.home.pos, playerPos)))) {
        Audio_StopSfxById(NA_SE_EN_FALL_AIM);
        this->timer = 0x82;
    }

    if (this->timer == 0x50) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FALL_AIM);
    }

    if (this->timer == 0) {
        EnWallmas_SetupDrop(this, play);
    }
}

void EnWallmas_Drop(EnWallmas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!Player_InCsMode(play) && !(player->stateFlags2 & 0x10) && (player->invincibilityTimer >= 0) &&
        (this->actor.xzDistToPlayer < 30.0f) && (this->actor.yDistToPlayer < -5.0f) &&
        (-(f32)(player->cylinder.dim.height + 10) < this->actor.yDistToPlayer)) {
        EnWallmas_SetupTakePlayer(this, play);
    }
}

void EnWallmas_Land(EnWallmas* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime) != 0) {
        EnWallmas_SetupStand(this);
    }
}

void EnWallmas_Stand(EnWallmas* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime) != 0) {
        EnWallmas_SetupWalk(this);
    }

    Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer + 0x8000, 0xB6);
}

void EnWallmas_Walk(EnWallmas* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime) != 0) {
        EnWallmas_SetupJumpToCeiling(this);
    }

    Math_ScaledStepToS(&this->actor.world.rot.y, (s16)((s32)this->actor.yawTowardsPlayer + 0x8000), 0xB6);

    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 12.0f) ||
        Animation_OnFrame(&this->skelAnime, 24.0f) || Animation_OnFrame(&this->skelAnime, 36.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FALL_WALK);
    }
}

void EnWallmas_JumpToCeiling(EnWallmas* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime) != 0) {
        EnWallmas_SetupReturnToCeiling(this);
    }
}

void EnWallmas_ReturnToCeiling(EnWallmas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    SkelAnime_Update(&this->skelAnime);
    if (this->skelAnime.curFrame > 20.0f) {
        this->timer += 9;
        this->actor.world.pos.y = this->actor.world.pos.y + 30.0f;
    }

    if (Animation_OnFrame(&this->skelAnime, 20.0f) != 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FALL_UP);
    }

    if (this->actor.yDistToPlayer < -900.0f) {
        if (this->actor.params == WMT_FLAG) {
            Actor_Kill(&this->actor);
            return;
        }

        if (this->actor.params == WMT_TIMER ||
            Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos) < 200.0f) {
            EnWallmas_TimerInit(this, play);
        } else {
            EnWallmas_ProximityOrSwitchInit(this);
        }
    }
}

void EnWallmas_TakeDamage(EnWallmas* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime) != 0) {
        if (this->actor.colChkInfo.health == 0) {
            EnWallmas_SetupDie(this, play);
        } else {
            EnWallmas_SetupCooldown(this);
        }
    }
    if (Animation_OnFrame(&this->skelAnime, 13.0f) != 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
    }

    Math_StepToF(&this->actor.speedXZ, 0.0f, 0.2f);
}

void EnWallmas_Cooldown(EnWallmas* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime) != 0) {
        EnWallmas_SetupReturnToCeiling(this);
    }
}

void EnWallmas_Die(EnWallmas* this, PlayState* play) {
    if (Math_StepToF(&this->actor.scale.x, 0.0f, 0.0015) != 0) {
        Actor_SetScale(&this->actor, 0.01f);
        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0xC0);
        Actor_Kill(&this->actor);
    }
    this->actor.scale.z = this->actor.scale.x;
    this->actor.scale.y = this->actor.scale.x;
}

void EnWallmas_TakePlayer(EnWallmas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Animation_OnFrame(&this->skelAnime, 1.0f) != 0) {
        if (!LINK_IS_ADULT) {
            func_8002F7DC(&this->actor, NA_SE_VO_LI_DAMAGE_S_KID);
        } else {
            func_8002F7DC(&this->actor, NA_SE_VO_LI_DAMAGE_S);
        }

        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FALL_CATCH);
    }
    if (SkelAnime_Update(&this->skelAnime) != 0) {
        player->actor.world.pos.x = this->actor.world.pos.x;
        player->actor.world.pos.z = this->actor.world.pos.z;

        if (this->timer < 0) {
            this->actor.world.pos.y = this->actor.world.pos.y + 2.0f;
        } else {
            this->actor.world.pos.y = this->actor.world.pos.y + 10.0f;
        }

        if (!LINK_IS_ADULT) {
            player->actor.world.pos.y = this->actor.world.pos.y - 30.0f;
        } else {
            player->actor.world.pos.y = this->actor.world.pos.y - 50.0f;
        }

        if (this->timer == -0x1E) {
            if (!LINK_IS_ADULT) {
                func_8002F7DC(&this->actor, NA_SE_VO_LI_TAKEN_AWAY_KID);
            } else {
                func_8002F7DC(&this->actor, NA_SE_VO_LI_TAKEN_AWAY);
            }
        }
        if (this->timer == 0) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_FALL_UP);
        }

        this->timer = this->timer + 2;
    } else {
        Math_StepToF(&this->actor.world.pos.y, player->actor.world.pos.y + (!LINK_IS_ADULT ? 30.0f : 50.0f), 5.0f);
    }

    Math_StepToF(&this->actor.world.pos.x, player->actor.world.pos.x, 3.0f);
    Math_StepToF(&this->actor.world.pos.z, player->actor.world.pos.z, 3.0f);

    if (this->timer == 0x1E) {
        func_80078884(NA_SE_OC_ABYSS);
        Play_TriggerRespawn(play);
    }
}

void EnWallmas_WaitForProximity(EnWallmas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    if (Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos) < 200.0f) {
        EnWallmas_TimerInit(this, play);
    }
}

void EnWallmas_WaitForSwitchFlag(EnWallmas* this, PlayState* play) {
    if (Flags_GetSwitch(play, this->switchFlag) != 0) {
        EnWallmas_TimerInit(this, play);
        this->timer = 0x51;
    }
}

void EnWallmas_Stun(EnWallmas* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        if (this->actor.colChkInfo.health == 0) {
            EnWallmas_SetupDie(this, play);
        } else {
            EnWallmas_SetupReturnToCeiling(this);
        }
    }
}

void EnWallmas_ColUpdate(EnWallmas* this, PlayState* play) {
    if ((this->collider.base.acFlags & AC_HIT) != 0) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&this->actor, &this->collider.info, 1);
        if ((this->actor.colChkInfo.damageEffect != 0) || (this->actor.colChkInfo.damage != 0)) {
            if (Actor_ApplyDamage(&this->actor) == 0) {
                Enemy_StartFinishingBlow(play, &this->actor);
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_FALL_DEAD);
                this->actor.flags &= ~ACTOR_FLAG_0;
            } else {
                if (this->actor.colChkInfo.damage != 0) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_FALL_DAMAGE);
                }
            }

            if ((this->actor.colChkInfo.damageEffect == DAMAGE_EFFECT_STUN_WHITE) ||
                (this->actor.colChkInfo.damageEffect == DAMAGE_EFFECT_STUN_BLUE)) {
                if (this->actionFunc != EnWallmas_Stun) {
                    EnWallmas_SetupStun(this);
                }
            } else {
                if (this->actor.colChkInfo.damageEffect == DAMAGE_EFFECT_BURN) {
                    EffectSsFCircle_Spawn(play, &this->actor, &this->actor.world.pos, 40, 40);
                }

                EnWallmas_SetupTakeDamage(this);
            }
        }
    }
}

void EnWallmas_Update(Actor* thisx, PlayState* play) {
    EnWallmas* this = (EnWallmas*)thisx;
    char pad[4];

    EnWallmas_ColUpdate(this, play);
    this->actionFunc(this, play);

    if ((this->actionFunc == EnWallmas_WaitToDrop) || (this->actionFunc == EnWallmas_WaitForProximity) ||
        (this->actionFunc == EnWallmas_TakePlayer) || (this->actionFunc == EnWallmas_WaitForSwitchFlag)) {
        return;
    }

    if ((this->actionFunc != EnWallmas_ReturnToCeiling) && (this->actionFunc != EnWallmas_TakePlayer)) {
        Actor_MoveForward(&this->actor);
    }

    if (this->actionFunc != EnWallmas_Drop) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 25.0f, 0.0f, 0x1D);
    } else if (this->actor.world.pos.y <= this->yTarget) {
        this->actor.world.pos.y = this->yTarget;
        this->actor.velocity.y = 0.0f;
        EnWallmas_SetupLand(this, play);
    }

    if ((this->actionFunc != EnWallmas_Die) && (this->actionFunc != EnWallmas_Drop)) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

        if ((this->actionFunc != EnWallmas_TakeDamage) && (this->actor.bgCheckFlags & 1) != 0 &&
            (this->actor.freezeTimer == 0)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
    }

    Actor_SetFocus(&this->actor, 25.0f);

    if (this->actionFunc == EnWallmas_TakeDamage) {
        return;
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void EnWallmas_DrawXlu(EnWallmas* this, PlayState* play) {
    s32 pad;
    f32 xzScale;
    MtxF mf;

    if ((this->actor.floorPoly == NULL) || ((this->timer >= 0x51) && (this->actionFunc != EnWallmas_Stun))) {
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);

    func_80094044(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 255);

    func_80038A28(this->actor.floorPoly, this->actor.world.pos.x, this->actor.floorHeight, this->actor.world.pos.z,
                  &mf);
    Matrix_Mult(&mf, MTXMODE_NEW);

    if ((this->actionFunc != EnWallmas_WaitToDrop) && (this->actionFunc != EnWallmas_ReturnToCeiling) &&
        (this->actionFunc != EnWallmas_TakePlayer) && (this->actionFunc != EnWallmas_WaitForSwitchFlag)) {
        xzScale = this->actor.scale.x * 50.0f;
    } else {
        xzScale = ((0x50 - this->timer) >= 0x51 ? 0x50 : (0x50 - this->timer)) * TIMER_SCALE;
    }

    Matrix_Scale(xzScale, 1.0f, xzScale, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_LOAD);
    gSPDisplayList(POLY_XLU_DISP++, gCircleShadowDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnWallMas_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                               void* thisx) {
    EnWallmas* this = (EnWallmas*)thisx;

    if (limbIndex == 1) {
        if (this->actionFunc != EnWallmas_TakePlayer) {
            pos->z -= 1600.0f;
        } else {
            pos->z -= ((1600.0f * (this->skelAnime.endFrame - this->skelAnime.curFrame)) / this->skelAnime.endFrame);
        }
    }
    return false;
}

void EnWallMas_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    if (limbIndex == 2) {
        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Push();
        Matrix_Translate(1600.0f, -700.0f, -1700.0f, MTXMODE_APPLY);
        Matrix_RotateY(DEGREE_60_RAD, MTXMODE_APPLY);
        Matrix_RotateZ(DEGREE_15_RAD, MTXMODE_APPLY);
        Matrix_Scale(2.0f, 2.0f, 2.0f, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_LOAD);
        gSPDisplayList(POLY_OPA_DISP++, gWallmasterFingerDL);

        Matrix_Pop();

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnWallmas_Draw(Actor* thisx, PlayState* play) {
    EnWallmas* this = (EnWallmas*)thisx;

    if (this->actionFunc != EnWallmas_WaitToDrop) {
        func_80093D18(play->state.gfxCtx);
        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                              this->skelAnime.dListCount, EnWallMas_OverrideLimbDraw, EnWallMas_PostLimbDraw, this);
    }

    EnWallmas_DrawXlu(this, play);
}
