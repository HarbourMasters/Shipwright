/*
 * File: z_en_rr.c
 * Overlay: ovl_En_Rr
 * Description: Like Like
 */

#include "z_en_rr.h"
#include "objects/object_rr/object_rr.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_10)

#define RR_MESSAGE_SHIELD (1 << 0)
#define RR_MESSAGE_TUNIC (1 << 1)
#define RR_MOUTH 4
#define RR_BASE 0

typedef enum {
    /* 0 */ REACH_NONE,
    /* 1 */ REACH_EXTEND,
    /* 2 */ REACH_STOP,
    /* 3 */ REACH_OPEN,
    /* 4 */ REACH_GAPE,
    /* 5 */ REACH_CLOSE
} EnRrReachState;

typedef enum {
    /* 0x0 */ RR_DMG_NONE,
    /* 0x1 */ RR_DMG_STUN,
    /* 0x2 */ RR_DMG_FIRE,
    /* 0x3 */ RR_DMG_ICE,
    /* 0x4 */ RR_DMG_LIGHT_MAGIC,
    /* 0xB */ RR_DMG_LIGHT_ARROW = 11,
    /* 0xC */ RR_DMG_SHDW_ARROW,
    /* 0xD */ RR_DMG_WIND_ARROW,
    /* 0xE */ RR_DMG_SPRT_ARROW,
    /* 0xF */ RR_DMG_NORMAL
} EnRrDamageEffect;

typedef enum {
    /* 0 */ RR_DROP_RANDOM_RUPEE,
    /* 1 */ RR_DROP_MAGIC,
    /* 2 */ RR_DROP_ARROW,
    /* 3 */ RR_DROP_FLEXIBLE,
    /* 4 */ RR_DROP_RUPEE_PURPLE,
    /* 5 */ RR_DROP_RUPEE_RED
} EnRrDropType;

void EnRr_Init(Actor* thisx, PlayState* play);
void EnRr_Destroy(Actor* thisx, PlayState* play);
void EnRr_Update(Actor* thisx, PlayState* play);
void EnRr_Draw(Actor* thisx, PlayState* play);

void EnRr_InitBodySegments(EnRr* this, PlayState* play);

void EnRr_SetupDamage(EnRr* this);
void EnRr_SetupDeath(EnRr* this);

void EnRr_Approach(EnRr* this, PlayState* play);
void EnRr_Reach(EnRr* this, PlayState* play);
void EnRr_GrabPlayer(EnRr* this, PlayState* play);
void EnRr_Damage(EnRr* this, PlayState* play);
void EnRr_Death(EnRr* this, PlayState* play);
void EnRr_Retreat(EnRr* this, PlayState* play);
void EnRr_Stunned(EnRr* this, PlayState* play);

const ActorInit En_Rr_InitVars = {
    ACTOR_EN_RR,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_RR,
    sizeof(EnRr),
    (ActorFunc)EnRr_Init,
    (ActorFunc)EnRr_Destroy,
    (ActorFunc)EnRr_Update,
    (ActorFunc)EnRr_Draw,
    NULL,
};

static char* sDropNames[] = {
    // "type 7", "small magic jar", "arrow", "fairy", "20 rupees", "50 rupees"
    "タイプ７  ", "魔法の壷小", "矢        ", "妖精      ", "20ルピー  ", "50ルピー  ",
};

static ColliderCylinderInitType1 sCylinderInit1 = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 30, 55, 0, { 0, 0, 0 } },
};

static ColliderCylinderInitType1 sCylinderInit2 = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 20, -10, { 0, 0, 0 } },
};

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, RR_DMG_NONE),
    /* Deku stick    */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Slingshot     */ DMG_ENTRY(1, RR_DMG_NORMAL),
    /* Explosive     */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Boomerang     */ DMG_ENTRY(0, RR_DMG_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Hammer swing  */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Hookshot      */ DMG_ENTRY(0, RR_DMG_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, RR_DMG_NORMAL),
    /* Master sword  */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Giant's Knife */ DMG_ENTRY(4, RR_DMG_NORMAL),
    /* Fire arrow    */ DMG_ENTRY(4, RR_DMG_FIRE),
    /* Ice arrow     */ DMG_ENTRY(4, RR_DMG_ICE),
    /* Light arrow   */ DMG_ENTRY(15, RR_DMG_LIGHT_ARROW),
    /* Unk arrow 1   */ DMG_ENTRY(4, RR_DMG_WIND_ARROW),
    /* Unk arrow 2   */ DMG_ENTRY(15, RR_DMG_SHDW_ARROW),
    /* Unk arrow 3   */ DMG_ENTRY(15, RR_DMG_SPRT_ARROW),
    /* Fire magic    */ DMG_ENTRY(4, RR_DMG_FIRE),
    /* Ice magic     */ DMG_ENTRY(3, RR_DMG_ICE),
    /* Light magic   */ DMG_ENTRY(10, RR_DMG_LIGHT_MAGIC),
    /* Shield        */ DMG_ENTRY(0, RR_DMG_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, RR_DMG_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, RR_DMG_NORMAL),
    /* Giant spin    */ DMG_ENTRY(4, RR_DMG_NORMAL),
    /* Master spin   */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Kokiri jump   */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Giant jump    */ DMG_ENTRY(8, RR_DMG_NORMAL),
    /* Master jump   */ DMG_ENTRY(4, RR_DMG_NORMAL),
    /* Unknown 1     */ DMG_ENTRY(10, RR_DMG_SPRT_ARROW),
    /* Unblockable   */ DMG_ENTRY(0, RR_DMG_NONE),
    /* Hammer jump   */ DMG_ENTRY(0, RR_DMG_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, RR_DMG_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x37, ICHAIN_CONTINUE),
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 30, ICHAIN_STOP),
};

void EnRr_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnRr* this = (EnRr*)thisx;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actor.colChkInfo.damageTable = &sDamageTable;
    this->actor.colChkInfo.health = 4;
    Collider_InitCylinder(play, &this->collider1);
    Collider_SetCylinderType1(play, &this->collider1, &this->actor, &sCylinderInit1);
    Collider_InitCylinder(play, &this->collider2);
    Collider_SetCylinderType1(play, &this->collider2, &this->actor, &sCylinderInit2);
    Actor_SetFocus(&this->actor, 30.0f);
    this->actor.scale.y = 0.013f;
    this->actor.scale.x = this->actor.scale.z = 0.014f;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.velocity.y = this->actor.speedXZ = 0.0f;
    this->actor.gravity = -0.4f;
    this->actionTimer = 0;
    this->eatenShield = 0;
    this->eatenTunic = 0;
    this->retreat = false;
    this->grabTimer = 0;
    this->invincibilityTimer = 0;
    this->effectTimer = 0;
    this->hasPlayer = false;
    this->stopScroll = false;
    this->ocTimer = 0;
    this->reachState = this->isDead = false;
    this->actionFunc = EnRr_Approach;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].height = this->bodySegs[i].heightTarget = this->bodySegs[i].scaleMod.x =
            this->bodySegs[i].scaleMod.y = this->bodySegs[i].scaleMod.z = 0.0f;
    }
    EnRr_InitBodySegments(this, play);
}

void EnRr_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnRr* this = (EnRr*)thisx;

    Collider_DestroyCylinder(play, &this->collider1);
    Collider_DestroyCylinder(play, &this->collider2);
}

void EnRr_SetSpeed(EnRr* this, f32 speed) {
    this->actor.speedXZ = speed;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_LIKE_WALK);
}

void EnRr_SetupReach(EnRr* this) {
    static f32 segmentHeights[] = { 0.0f, 500.0f, 750.0f, 1000.0f, 1000.0f };
    s32 i;

    this->reachState = 1;
    this->actionTimer = 20;
    this->segPhaseVelTarget = 2500.0f;
    this->segMoveRate = 0.0f;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].heightTarget = segmentHeights[i];
        this->bodySegs[i].scaleTarget.x = this->bodySegs[i].scaleTarget.z = 0.8f;
        this->bodySegs[i].rotTarget.x = 6000.0f;
        this->bodySegs[i].rotTarget.z = 0.0f;
    }
    this->actionFunc = EnRr_Reach;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_LIKE_UNARI);
}

void EnRr_SetupNeutral(EnRr* this) {
    s32 i;

    this->reachState = 0;
    this->segMoveRate = 0.0f;
    this->segPhaseVelTarget = 2500.0f;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].heightTarget = 0.0f;
        this->bodySegs[i].rotTarget.x = this->bodySegs[i].rotTarget.z = 0.0f;
        this->bodySegs[i].scaleTarget.x = this->bodySegs[i].scaleTarget.z = 1.0f;
    }
    if (this->retreat) {
        this->actionTimer = 100;
        this->actionFunc = EnRr_Retreat;
    } else {
        this->actionTimer = 60;
        this->actionFunc = EnRr_Approach;
    }
}

void EnRr_SetupGrabPlayer(EnRr* this, Player* player) {
    s32 i;

    this->grabTimer = 100;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->ocTimer = 8;
    this->hasPlayer = true;
    this->reachState = 0;
    this->segMoveRate = this->swallowOffset = this->actor.speedXZ = 0.0f;
    this->pulseSizeTarget = 0.15f;
    this->segPhaseVelTarget = 5000.0f;
    this->wobbleSizeTarget = 512.0f;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].heightTarget = 0.0f;
        this->bodySegs[i].rotTarget.x = this->bodySegs[i].rotTarget.z = 0.0f;
        this->bodySegs[i].scaleTarget.x = this->bodySegs[i].scaleTarget.z = 1.0f;
    }
    this->actionFunc = EnRr_GrabPlayer;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_LIKE_DRINK);
}

u8 EnRr_GetMessage(u8 shield, u8 tunic) {
    u8 messageIndex = 0;

    if ((shield == 1 /* Deku shield */) || (shield == 2 /* Hylian shield */)) {
        messageIndex = RR_MESSAGE_SHIELD;
    }
    if ((tunic == 2 /* Goron tunic */) || (tunic == 3 /* Zora tunic */)) {
        messageIndex |= RR_MESSAGE_TUNIC;
    }

    return messageIndex;
}

void EnRr_SetupReleasePlayer(EnRr* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u8 shield;
    u8 tunic;

    this->actor.flags |= ACTOR_FLAG_0;
    this->hasPlayer = false;
    this->ocTimer = 110;
    this->segMoveRate = 0.0f;
    this->segPhaseVelTarget = 2500.0f;
    this->wobbleSizeTarget = 2048.0f;
    tunic = 0;
    shield = 0;
    if (CUR_EQUIP_VALUE(EQUIP_SHIELD) != 3 /* Mirror shield */) {
        shield = Inventory_DeleteEquipment(play, EQUIP_SHIELD);
        if (shield != 0) {
            this->eatenShield = shield;
            this->retreat = true;
        }
    }
    if (CUR_EQUIP_VALUE(EQUIP_TUNIC) != 1 /* Kokiri tunic */) {
        tunic = Inventory_DeleteEquipment(play, EQUIP_TUNIC);
        if (tunic != 0) {
            this->eatenTunic = tunic;
            this->retreat = true;
        }
    }
    player->actor.parent = NULL;
    switch (EnRr_GetMessage(shield, tunic)) {
        case RR_MESSAGE_SHIELD:
            Message_StartTextbox(play, 0x305F, NULL);
            break;
        case RR_MESSAGE_TUNIC:
            Message_StartTextbox(play, 0x3060, NULL);
            break;
        case RR_MESSAGE_TUNIC | RR_MESSAGE_SHIELD:
            Message_StartTextbox(play, 0x3061, NULL);
            break;
    }
    osSyncPrintf(VT_FGCOL(YELLOW) "%s[%d] : Rr_Catch_Cancel" VT_RST "\n", __FILE__, __LINE__);
    func_8002F6D4(play, &this->actor, 4.0f, this->actor.shape.rot.y, 12.0f, 8);
    if (this->actor.colorFilterTimer == 0) {
        this->actionFunc = EnRr_Approach;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_LIKE_THROW);
    } else if (this->actor.colChkInfo.health != 0) {
        EnRr_SetupDamage(this);
    } else {
        EnRr_SetupDeath(this);
    }
}

void EnRr_SetupDamage(EnRr* this) {
    s32 i;

    this->reachState = 0;
    this->actionTimer = 20;
    this->segMoveRate = 0.0f;
    this->segPhaseVelTarget = 2500.0f;
    this->pulseSizeTarget = 0.0f;
    this->wobbleSizeTarget = 0.0f;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].heightTarget = 0.0f;
        this->bodySegs[i].rotTarget.x = this->bodySegs[i].rotTarget.z = 0.0f;
        this->bodySegs[i].scaleTarget.x = this->bodySegs[i].scaleTarget.z = 1.0f;
    }
    this->actionFunc = EnRr_Damage;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_LIKE_DAMAGE);
}

void EnRr_SetupApproach(EnRr* this) {
    s32 i;

    this->segMoveRate = 0.0f;
    this->pulseSizeTarget = 0.15f;
    this->segPhaseVelTarget = 2500.0f;
    this->wobbleSizeTarget = 2048.0f;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].heightTarget = 0.0f;
        this->bodySegs[i].rotTarget.x = this->bodySegs[i].rotTarget.z = 0.0f;
        this->bodySegs[i].scaleTarget.x = this->bodySegs[i].scaleTarget.z = 1.0f;
    }
    this->actionFunc = EnRr_Approach;
}

void EnRr_SetupDeath(EnRr* this) {
    s32 i;

    this->isDead = true;
    this->frameCount = 0;
    this->shrinkRate = 0.0f;
    this->segMoveRate = 0.0f;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].heightTarget = 0.0f;
        this->bodySegs[i].rotTarget.x = this->bodySegs[i].rotTarget.z = 0.0f;
    }
    this->actionFunc = EnRr_Death;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_LIKE_DEAD);
    this->actor.flags &= ~ACTOR_FLAG_0;
    gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_LIKE_LIKE]++;
}

void EnRr_SetupStunned(EnRr* this) {
    s32 i;

    this->stopScroll = true;
    this->segMovePhase = 0;
    this->segPhaseVel = 0.0f;
    this->segPhaseVelTarget = 2500.0f;
    this->segPulsePhaseDiff = 0.0f;
    this->segWobblePhaseDiffX = 0.0f;
    this->segWobbleXTarget = 3.0f;
    this->segWobblePhaseDiffZ = 0.0f;
    this->segWobbleZTarget = 1.0f;
    this->pulseSize = 0.0f;
    this->pulseSizeTarget = 0.15f;
    this->wobbleSize = 0.0f;
    this->wobbleSizeTarget = 2048.0f;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].scaleMod.y = 0.0f;
        this->bodySegs[i].rotTarget.x = 0.0f;
        this->bodySegs[i].rotTarget.y = 0.0f;
        this->bodySegs[i].rotTarget.z = 0.0f;
        this->bodySegs[i].scale.x = this->bodySegs[i].scale.y = this->bodySegs[i].scale.z =
            this->bodySegs[i].scaleTarget.x = this->bodySegs[i].scaleTarget.y = this->bodySegs[i].scaleTarget.z = 1.0f;
    }
    this->actionFunc = EnRr_Stunned;
}

void EnRr_CollisionCheck(EnRr* this, PlayState* play) {
    Vec3f hitPos;
    Player* player = GET_PLAYER(play);

    if (this->collider2.base.acFlags & AC_HIT) {
        this->collider2.base.acFlags &= ~AC_HIT;
        // "Kakin" (not sure what this means)
        osSyncPrintf(VT_FGCOL(GREEN) "カキン(%d)！！" VT_RST "\n", this->frameCount);
        hitPos.x = this->collider2.info.bumper.hitPos.x;
        hitPos.y = this->collider2.info.bumper.hitPos.y;
        hitPos.z = this->collider2.info.bumper.hitPos.z;
        CollisionCheck_SpawnShieldParticlesMetal2(play, &hitPos);
    } else {
        if (this->collider1.base.acFlags & AC_HIT) {
            u8 dropType = RR_DROP_RANDOM_RUPEE;

            this->collider1.base.acFlags &= ~AC_HIT;
            if (this->actor.colChkInfo.damageEffect != 0) {
                hitPos.x = this->collider1.info.bumper.hitPos.x;
                hitPos.y = this->collider1.info.bumper.hitPos.y;
                hitPos.z = this->collider1.info.bumper.hitPos.z;
                CollisionCheck_BlueBlood(play, NULL, &hitPos);
            }
            switch (this->actor.colChkInfo.damageEffect) {
                case RR_DMG_LIGHT_ARROW:
                    dropType++; // purple rupee
                case RR_DMG_SHDW_ARROW:
                    dropType++; // flexible
                case RR_DMG_WIND_ARROW:
                    dropType++; // arrow
                case RR_DMG_SPRT_ARROW:
                    dropType++; // magic jar
                case RR_DMG_NORMAL:
                    // "ouch"
                    osSyncPrintf(VT_FGCOL(RED) "いてっ( %d : LIFE %d : DAMAGE %d : %x )！！" VT_RST "\n",
                                 this->frameCount, this->actor.colChkInfo.health, this->actor.colChkInfo.damage,
                                 this->actor.colChkInfo.damageEffect);
                    this->stopScroll = false;
                    Actor_ApplyDamage(&this->actor);
                    this->invincibilityTimer = 40;
                    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0x2000, this->invincibilityTimer);
                    if (this->hasPlayer) {
                        EnRr_SetupReleasePlayer(this, play);
                    } else if (this->actor.colChkInfo.health != 0) {
                        EnRr_SetupDamage(this);
                    } else {
                        this->dropType = dropType;
                        EnRr_SetupDeath(this);
                    }
                    return;
                case RR_DMG_FIRE: // Fire Arrow and Din's Fire
                    Actor_ApplyDamage(&this->actor);
                    if (this->actor.colChkInfo.health == 0) {
                        this->dropType = RR_DROP_RANDOM_RUPEE;
                    }
                    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0x2000, 0x50);
                    this->effectTimer = 20;
                    EnRr_SetupStunned(this);
                    return;
                case RR_DMG_ICE: // Ice Arrow and unused ice magic
                    Actor_ApplyDamage(&this->actor);
                    if (this->actor.colChkInfo.health == 0) {
                        this->dropType = RR_DROP_RANDOM_RUPEE;
                    }
                    if (this->actor.colorFilterTimer == 0) {
                        this->effectTimer = 20;
                        Actor_SetColorFilter(&this->actor, 0, 0xFF, 0x2000, 0x50);
                    }
                    EnRr_SetupStunned(this);
                    return;
                case RR_DMG_LIGHT_MAGIC: // Unused light magic
                    Actor_ApplyDamage(&this->actor);
                    if (this->actor.colChkInfo.health == 0) {
                        this->dropType = RR_DROP_RUPEE_RED;
                    }
                    Actor_SetColorFilter(&this->actor, -0x8000, 0xFF, 0x2000, 0x50);
                    EnRr_SetupStunned(this);
                    return;
                case RR_DMG_STUN: // Boomerang and Hookshot
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
                    Actor_SetColorFilter(&this->actor, 0, 0xFF, 0x2000, 0x50);
                    EnRr_SetupStunned(this);
                    return;
            }
        }
        if ((this->ocTimer == 0) && (this->actor.colorFilterTimer == 0) && (player->invincibilityTimer == 0) &&
            !(player->stateFlags2 & 0x80) &&
            ((this->collider1.base.ocFlags1 & OC1_HIT) || (this->collider2.base.ocFlags1 & OC1_HIT))) {
            this->collider1.base.ocFlags1 &= ~OC1_HIT;
            this->collider2.base.ocFlags1 &= ~OC1_HIT;
            // "catch"
            osSyncPrintf(VT_FGCOL(GREEN) "キャッチ(%d)！！" VT_RST "\n", this->frameCount);
            if (play->grabPlayer(play, player)) {
                player->actor.parent = &this->actor;
                this->stopScroll = false;
                EnRr_SetupGrabPlayer(this, player);
            }
        }
    }
}

void EnRr_InitBodySegments(EnRr* this, PlayState* play) {
    s32 i;

    this->segMovePhase = 0;
    this->segPhaseVel = 0.0f;
    this->segPhaseVelTarget = 2500.0f;
    this->segPulsePhaseDiff = 0.0f;
    this->segWobblePhaseDiffX = 0.0f;
    this->segWobbleXTarget = 3.0f;
    this->segWobblePhaseDiffZ = 0.0f;
    this->segWobbleZTarget = 1.0f;
    this->pulseSize = 0.0f;
    this->pulseSizeTarget = 0.15f;
    this->wobbleSize = 0.0f;
    this->wobbleSizeTarget = 2048.0f;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].scaleMod.y = 0.0f;
        this->bodySegs[i].rotTarget.x = 0.0f;
        this->bodySegs[i].rotTarget.y = 0.0f;
        this->bodySegs[i].rotTarget.z = 0.0f;
        this->bodySegs[i].scale.x = this->bodySegs[i].scale.y = this->bodySegs[i].scale.z =
            this->bodySegs[i].scaleTarget.x = this->bodySegs[i].scaleTarget.y = this->bodySegs[i].scaleTarget.z = 1.0f;
    }
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].scaleMod.x = this->bodySegs[i].scaleMod.z =
            Math_CosS(i * (u32)(s16)this->segPulsePhaseDiff * 0x1000) * this->pulseSize;
    }
    for (i = 1; i < 5; i++) {
        this->bodySegs[i].rotTarget.x = Math_CosS(i * (u32)(s16)this->segWobblePhaseDiffX * 0x1000) * this->wobbleSize;
        this->bodySegs[i].rotTarget.z = Math_SinS(i * (u32)(s16)this->segWobblePhaseDiffZ * 0x1000) * this->wobbleSize;
    }
}

void EnRr_UpdateBodySegments(EnRr* this, PlayState* play) {
    s32 i;
    s16 phase = this->segMovePhase;

    if (!this->isDead) {
        for (i = 0; i < 5; i++) {
            this->bodySegs[i].scaleMod.x = this->bodySegs[i].scaleMod.z =
                Math_CosS(phase + i * (s16)this->segPulsePhaseDiff * 0x1000) * this->pulseSize;
        }
        phase = this->segMovePhase;
        if (!this->isDead && (this->reachState == 0)) {
            for (i = 1; i < 5; i++) {
                this->bodySegs[i].rotTarget.x =
                    Math_CosS(phase + i * (s16)this->segWobblePhaseDiffX * 0x1000) * this->wobbleSize;
                this->bodySegs[i].rotTarget.z =
                    Math_SinS(phase + i * (s16)this->segWobblePhaseDiffZ * 0x1000) * this->wobbleSize;
            }
        }
    }
    if (!this->stopScroll) {
        this->segMovePhase += (s16)this->segPhaseVel;
    }
}

void EnRr_Approach(EnRr* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x1F4, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if ((this->actionTimer == 0) && (this->actor.xzDistToPlayer < 160.0f)) {
        EnRr_SetupReach(this);
    } else if ((this->actor.xzDistToPlayer < 400.0f) && (this->actor.speedXZ == 0.0f)) {
        EnRr_SetSpeed(this, 2.0f);
    }
}

void EnRr_Reach(EnRr* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x1F4, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    switch (this->reachState) {
        case REACH_EXTEND:
            if (this->actionTimer == 0) {
                this->reachState = REACH_STOP;
            }
            break;
        case REACH_STOP:
            if (this->actionTimer == 0) {
                this->actionTimer = 5;
                this->bodySegs[RR_MOUTH].scaleTarget.x = this->bodySegs[RR_MOUTH].scaleTarget.z = 1.5f;
                this->reachState = REACH_OPEN;
            }
            break;
        case REACH_OPEN:
            if (this->actionTimer == 0) {
                this->actionTimer = 2;
                this->bodySegs[RR_MOUTH].heightTarget = 2000.0f;
                this->reachState = REACH_GAPE;
            }
            break;
        case REACH_GAPE:
            if (this->actionTimer == 0) {
                this->actionTimer = 20;
                this->bodySegs[RR_MOUTH].scaleTarget.x = this->bodySegs[RR_MOUTH].scaleTarget.z = 0.8f;
                this->reachState = REACH_CLOSE;
            }
            break;
        case REACH_CLOSE:
            if (this->actionTimer == 0) {
                EnRr_SetupNeutral(this);
            }
            break;
    }
}

void EnRr_GrabPlayer(EnRr* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    func_800AA000(this->actor.xyzDistToPlayerSq, 120, 2, 120);
    if ((this->frameCount % 8) == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_LIKE_EAT);
    }
    this->ocTimer = 8;
    if ((this->grabTimer == 0) || !(player->stateFlags2 & 0x80)) {
        EnRr_SetupReleasePlayer(this, play);
    } else {
        Math_ApproachF(&player->actor.world.pos.x, this->mouthPos.x, 1.0f, 30.0f);
        Math_ApproachF(&player->actor.world.pos.y, this->mouthPos.y + this->swallowOffset, 1.0f, 30.0f);
        Math_ApproachF(&player->actor.world.pos.z, this->mouthPos.z, 1.0f, 30.0f);
        Math_ApproachF(&this->swallowOffset, -55.0f, 1.0f, 5.0f);
    }
}

void EnRr_Damage(EnRr* this, PlayState* play) {
    s32 i;

    if (this->actor.colorFilterTimer == 0) {
        EnRr_SetupApproach(this);
    } else if ((this->actor.colorFilterTimer & 8) != 0) {
        for (i = 1; i < 5; i++) {
            this->bodySegs[i].rotTarget.z = 5000.0f;
        }
    } else {
        for (i = 1; i < 5; i++) {
            this->bodySegs[i].rotTarget.z = -5000.0f;
        }
    }
}

void EnRr_Death(EnRr* this, PlayState* play) {
    s32 pad;
    s32 i;

    if (this->frameCount < 40) {
        for (i = 0; i < 5; i++) {
            Math_ApproachF(&this->bodySegs[i].heightTarget, i + 59 - (this->frameCount * 25.0f), 1.0f, 50.0f);
            this->bodySegs[i].scaleTarget.x = this->bodySegs[i].scaleTarget.z =
                (SQ(4 - i) * (f32)this->frameCount * 0.003f) + 1.0f;
        }
    } else if (this->frameCount >= 95) {
        Vec3f dropPos;

        dropPos.x = this->actor.world.pos.x;
        dropPos.y = this->actor.world.pos.y;
        dropPos.z = this->actor.world.pos.z;
        switch (this->eatenShield) {
            case 1:
                Item_DropCollectible(play, &dropPos, ITEM00_SHIELD_DEKU);
                break;
            case 2:
                Item_DropCollectible(play, &dropPos, ITEM00_SHIELD_HYLIAN);
                break;
        }
        switch (this->eatenTunic) {
            case 2:
                Item_DropCollectible(play, &dropPos, ITEM00_TUNIC_GORON);
                break;
            case 3:
                Item_DropCollectible(play, &dropPos, ITEM00_TUNIC_ZORA);
                break;
        }
        // "dropped"
        osSyncPrintf(VT_FGCOL(GREEN) "「%s」が出た！！" VT_RST "\n", sDropNames[this->dropType]);
        switch (this->dropType) {
            case RR_DROP_MAGIC:
                Item_DropCollectible(play, &dropPos, ITEM00_MAGIC_SMALL);
                break;
            case RR_DROP_ARROW:
                Item_DropCollectible(play, &dropPos, ITEM00_ARROWS_SINGLE);
                break;
            case RR_DROP_FLEXIBLE:
                Item_DropCollectible(play, &dropPos, ITEM00_FLEXIBLE);
                break;
            case RR_DROP_RUPEE_PURPLE:
                Item_DropCollectible(play, &dropPos, ITEM00_RUPEE_PURPLE);
                break;
            case RR_DROP_RUPEE_RED:
                Item_DropCollectible(play, &dropPos, ITEM00_RUPEE_RED);
                break;
            case RR_DROP_RANDOM_RUPEE:
            default:
                Item_DropCollectibleRandom(play, &this->actor, &dropPos, 12 << 4);
                break;
        }
        Actor_Kill(&this->actor);
    } else if (this->frameCount == 88) {
        Vec3f pos;
        Vec3f vel;
        Vec3f accel;

        pos.x = this->actor.world.pos.x;
        pos.y = this->actor.world.pos.y + 20.0f;
        pos.z = this->actor.world.pos.z;
        vel.x = 0.0f;
        vel.y = 0.0f;
        vel.z = 0.0f;
        accel.x = 0.0f;
        accel.y = 0.0f;
        accel.z = 0.0f;

        EffectSsDeadDb_Spawn(play, &pos, &vel, &accel, 100, 0, 255, 255, 255, 255, 255, 0, 0, 1, 9, true);
    } else {
        Math_ApproachF(&this->actor.scale.x, 0.0f, 1.0f, this->shrinkRate);
        Math_ApproachF(&this->shrinkRate, 0.001f, 1.0f, 0.00001f);
        this->actor.scale.z = this->actor.scale.x;
    }
}

void EnRr_Retreat(EnRr* this, PlayState* play) {
    if (this->actionTimer == 0) {
        this->retreat = false;
        this->actionFunc = EnRr_Approach;
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer + 0x8000, 0xA, 0x3E8, 0);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (this->actor.speedXZ == 0.0f) {
            EnRr_SetSpeed(this, 2.0f);
        }
    }
}

void EnRr_Stunned(EnRr* this, PlayState* play) {
    if (this->actor.colorFilterTimer == 0) {
        this->stopScroll = false;
        if (this->hasPlayer) {
            EnRr_SetupReleasePlayer(this, play);
        } else if (this->actor.colChkInfo.health != 0) {
            this->actionFunc = EnRr_Approach;
        } else {
            EnRr_SetupDeath(this);
        }
    }
}

void EnRr_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnRr* this = (EnRr*)thisx;
    s32 i;

    this->frameCount++;
    if (!this->stopScroll) {
        this->scrollTimer++;
    }
    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    if (this->grabTimer != 0) {
        this->grabTimer--;
    }
    if (this->ocTimer != 0) {
        this->ocTimer--;
    }
    if (this->invincibilityTimer != 0) {
        this->invincibilityTimer--;
    }
    if (this->effectTimer != 0) {
        this->effectTimer--;
    }

    Actor_SetFocus(&this->actor, 30.0f);
    EnRr_UpdateBodySegments(this, play);
    if (!this->isDead && ((this->actor.colorFilterTimer == 0) || !(this->actor.colorFilterParams & 0x4000))) {
        EnRr_CollisionCheck(this, play);
    }

    this->actionFunc(this, play);
    if (this->hasPlayer == 0x3F80) { // checks if 1.0f has been stored to hasPlayer's address
        ASSERT(this->hasPlayer == 0x3F80);
    }

    Math_StepToF(&this->actor.speedXZ, 0.0f, 0.1f);
    Actor_MoveForward(&this->actor);
    Collider_UpdateCylinder(&this->actor, &this->collider1);
    this->collider2.dim.pos.x = this->mouthPos.x;
    this->collider2.dim.pos.y = this->mouthPos.y;
    this->collider2.dim.pos.z = this->mouthPos.z;
    if (!this->isDead && (this->invincibilityTimer == 0)) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider1.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider2.base);
        if (this->ocTimer == 0) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider1.base);
        }
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider2.base);
    } else {
        this->collider2.base.ocFlags1 &= ~OC1_HIT;
        this->collider2.base.acFlags &= ~AC_HIT;
        this->collider1.base.ocFlags1 &= ~OC1_HIT;
        this->collider1.base.acFlags &= ~AC_HIT;
    }
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 30.0f, 20.0f, 7);
    if (!this->stopScroll) {
        Math_ApproachF(&this->segPhaseVel, this->segPhaseVelTarget, 1.0f, 50.0f);
        Math_ApproachF(&this->segPulsePhaseDiff, 4.0f, 1.0f, 5.0f);
        Math_ApproachF(&this->segWobblePhaseDiffX, this->segWobbleXTarget, 1.0f, 0.04f);
        Math_ApproachF(&this->segWobblePhaseDiffZ, this->segWobbleZTarget, 1.0f, 0.01f);
        Math_ApproachF(&this->pulseSize, this->pulseSizeTarget, 1.0f, 0.0015f);
        Math_ApproachF(&this->wobbleSize, this->wobbleSizeTarget, 1.0f, 20.0f);
        for (i = 0; i < 5; i++) {
            Math_SmoothStepToS(&this->bodySegs[i].rot.x, this->bodySegs[i].rotTarget.x, 5, this->segMoveRate * 1000.0f,
                               0);
            Math_SmoothStepToS(&this->bodySegs[i].rot.z, this->bodySegs[i].rotTarget.z, 5, this->segMoveRate * 1000.0f,
                               0);
            Math_ApproachF(&this->bodySegs[i].scale.x, this->bodySegs[i].scaleTarget.x, 1.0f, this->segMoveRate * 0.2f);
            this->bodySegs[i].scale.z = this->bodySegs[i].scale.x;
            Math_ApproachF(&this->bodySegs[i].height, this->bodySegs[i].heightTarget, 1.0f, this->segMoveRate * 300.0f);
        }
        Math_ApproachF(&this->segMoveRate, 1.0f, 1.0f, 0.2f);
    }
}

static Vec3f sEffectOffsets[] = {
    { 25.0f, 0.0f, 0.0f },
    { -25.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 25.0f },
    { 0.0f, 0.0f, -25.0f },
};

void EnRr_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    Vec3f zeroVec;
    EnRr* this = (EnRr*)thisx;
    s32 i;
    Mtx* segMtx = Graph_Alloc(play->state.gfxCtx, 4 * sizeof(Mtx));

    OPEN_DISPS(play->state.gfxCtx);
    func_80093D84(play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x0C, segMtx);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, (this->scrollTimer * 0) & 0x7F,
                                (this->scrollTimer * 0) & 0x3F, 32, 16, 1, (this->scrollTimer * 0) & 0x3F,
                                (this->scrollTimer * -6) & 0x7F, 32, 16));
    Matrix_Push();

    Matrix_Scale((1.0f + this->bodySegs[RR_BASE].scaleMod.x) * this->bodySegs[RR_BASE].scale.x,
                 (1.0f + this->bodySegs[RR_BASE].scaleMod.y) * this->bodySegs[RR_BASE].scale.y,
                 (1.0f + this->bodySegs[RR_BASE].scaleMod.z) * this->bodySegs[RR_BASE].scale.z, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    Matrix_Pop();
    zeroVec.x = 0.0f;
    zeroVec.y = 0.0f;
    zeroVec.z = 0.0f;
    for (i = 1; i < 5; i++) {
        Matrix_Translate(0.0f, this->bodySegs[i].height + 1000.0f, 0.0f, MTXMODE_APPLY);

        Matrix_RotateZYX(this->bodySegs[i].rot.x, this->bodySegs[i].rot.y, this->bodySegs[i].rot.z, MTXMODE_APPLY);
        Matrix_Push();
        Matrix_Scale((1.0f + this->bodySegs[i].scaleMod.x) * this->bodySegs[i].scale.x,
                     (1.0f + this->bodySegs[i].scaleMod.y) * this->bodySegs[i].scale.y,
                     (1.0f + this->bodySegs[i].scaleMod.z) * this->bodySegs[i].scale.z, MTXMODE_APPLY);
        MATRIX_TOMTX(segMtx);
        Matrix_Pop();
        segMtx++;
        Matrix_MultVec3f(&zeroVec, &this->effectPos[i]);
    }
    this->effectPos[0] = this->actor.world.pos;
    Matrix_MultVec3f(&zeroVec, &this->mouthPos);
    gSPDisplayList(POLY_XLU_DISP++, gLikeLikeDL);

    CLOSE_DISPS(play->state.gfxCtx);
    if (this->effectTimer != 0) {
        Vec3f effectPos;
        s16 effectTimer = this->effectTimer - 1;

        this->actor.colorFilterTimer++;
        if ((effectTimer & 1) == 0) {
            s32 segIndex = 4 - (effectTimer >> 2);
            s32 offIndex = (effectTimer >> 1) & 3;

            effectPos.x = this->effectPos[segIndex].x + sEffectOffsets[offIndex].x + Rand_CenteredFloat(10.0f);
            effectPos.y = this->effectPos[segIndex].y + sEffectOffsets[offIndex].y + Rand_CenteredFloat(10.0f);
            effectPos.z = this->effectPos[segIndex].z + sEffectOffsets[offIndex].z + Rand_CenteredFloat(10.0f);
            if (this->actor.colorFilterParams & 0x4000) {
                EffectSsEnFire_SpawnVec3f(play, &this->actor, &effectPos, 100, 0, 0, -1);
            } else {
                EffectSsEnIce_SpawnFlyingVec3f(play, &this->actor, &effectPos, 150, 150, 150, 250, 235, 245, 255,
                                               3.0f);
            }
        }
    }
}
