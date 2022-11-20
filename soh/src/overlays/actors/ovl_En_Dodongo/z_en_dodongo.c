#include "z_en_dodongo.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/actors/ovl_En_Bombf/z_en_bombf.h"
#include "objects/object_dodongo/object_dodongo.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

typedef enum {
    DODONGO_SWEEP_TAIL,
    DODONGO_SWALLOW_BOMB,
    DODONGO_DEATH,
    DODONGO_BREATHE_FIRE,
    DODONGO_IDLE,
    DODONGO_END_BREATHE_FIRE,
    DODONGO_UNUSED,
    DODONGO_STUNNED,
    DODONGO_WALK
} EnDodongoActionState;

void EnDodongo_Init(Actor* thisx, PlayState* play);
void EnDodongo_Destroy(Actor* thisx, PlayState* play);
void EnDodongo_Update(Actor* thisx, PlayState* play);
void EnDodongo_Draw(Actor* thisx, PlayState* play);

void EnDodongo_SetupDeath(EnDodongo* this, PlayState* play);
void EnDodongo_ShiftVecRadial(s16 yaw, f32 radius, Vec3f* vec);
s32 EnDodongo_AteBomb(EnDodongo* this, PlayState* play);
void EnDodongo_SetupIdle(EnDodongo* this);

void EnDodongo_Idle(EnDodongo* this, PlayState* play);
void EnDodongo_EndBreatheFire(EnDodongo* this, PlayState* play);
void EnDodongo_BreatheFire(EnDodongo* this, PlayState* play);
void EnDodongo_SwallowBomb(EnDodongo* this, PlayState* play);
void EnDodongo_Walk(EnDodongo* this, PlayState* play);
void EnDodongo_Stunned(EnDodongo* this, PlayState* play);
void EnDodongo_Death(EnDodongo* this, PlayState* play);
void EnDodongo_SweepTail(EnDodongo* this, PlayState* play);

const ActorInit En_Dodongo_InitVars = {
    ACTOR_EN_DODONGO,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_DODONGO,
    sizeof(EnDodongo),
    (ActorFunc)EnDodongo_Init,
    (ActorFunc)EnDodongo_Destroy,
    (ActorFunc)EnDodongo_Update,
    (ActorFunc)EnDodongo_Draw,
    NULL,
};

static ColliderJntSphElementInit sBodyElementsInit[6] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 15, { { 0, 0, 0 }, 17 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 14, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 13, { { 0, 0, 0 }, 10 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 21, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 28, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x0D800691, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON | OCELEM_UNK3,
        },
        { 6, { { 0, 0, 0 }, 35 }, 100 },
    },
};

static ColliderJntSphInit sBodyJntSphInit = {
    {
        COLTYPE_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    6,
    sBodyElementsInit,
};

static ColliderTrisElementInit sHardElementsInit[3] = {
    {
        {
            ELEMTYPE_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xF24BF96E, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_HOOKABLE | BUMP_NO_AT_INFO,
            OCELEM_NONE,
        },
        { { { -10.0f, 14.0f, 2.0f }, { -10.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCBF96E, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_HOOKABLE | BUMP_NO_AT_INFO,
            OCELEM_NONE,
        },
        { { { -10.0f, -6.0f, 2.0f }, { 9.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCBF96E, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_HOOKABLE | BUMP_NO_AT_INFO,
            OCELEM_NONE,
        },
        { { { -10.0f, -6.0f, 2.0f }, { 9.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
};

static ColliderTrisInit sHardTrisInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    3,
    sHardElementsInit,
};

static ColliderQuadInit sAttackQuadInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0x20000000, 0x01, 0x10 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL | TOUCH_UNK7,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(1, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(4, 0xF),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x6),
    /* Ice magic     */ DMG_ENTRY(3, 0xF),
    /* Light magic   */ DMG_ENTRY(0, 0x6),
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

void EnDodongo_SetupAction(EnDodongo* this, EnDodongoActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnDodongo_SpawnBombSmoke(EnDodongo* this, PlayState* play) {
    s32 pad;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    Vec3f pos = this->headPos;
    s16 randAngle = Rand_CenteredFloat(0x4000);
    f32 randCos;
    f32 randSin;

    randCos = Math_CosS(this->actor.shape.rot.y + randAngle);
    randSin = Math_SinS(this->actor.shape.rot.y + randAngle);
    if (this->bombSmokePrimColor.r > 30) {
        this->bombSmokePrimColor.r -= 16;
        this->bombSmokePrimColor.g -= 16;
    }

    if (this->bombSmokePrimColor.b < 30) {
        this->bombSmokePrimColor.b += 5;
        this->bombSmokePrimColor.a += 8;
        this->bombSmokeEnvColor.a += 8;
    }
    if (this->bombSmokeEnvColor.r != 0) {
        this->bombSmokeEnvColor.r -= 15;
    }
    if (this->bombSmokeEnvColor.g != 0) {
        this->bombSmokeEnvColor.g--;
    }
    velocity.x = randSin * 3.5f;
    velocity.y = this->bombSmokeEnvColor.r * 0.02f;
    velocity.z = randCos * 3.5f;
    accel.x = ((Rand_ZeroOne() * 0.1f) + 0.15f) * -randSin;
    accel.z = ((Rand_ZeroOne() * 0.1f) + 0.15f) * -randCos;
    func_8002836C(play, &pos, &velocity, &accel, &this->bombSmokePrimColor, &this->bombSmokeEnvColor, 100, 25, 20);

    randAngle = Rand_ZeroOne() * 0x2000;
    randCos = Math_CosS(this->actor.shape.rot.y + randAngle);
    randSin = Math_SinS(this->actor.shape.rot.y + randAngle);
    pos.x -= randCos * 6.0f;
    pos.z += randSin * 6.0f;
    velocity.x = -randCos * 3.5f;
    velocity.y = this->bombSmokeEnvColor.r * 0.02f;
    velocity.z = randSin * 3.5f;
    accel.x = ((Rand_ZeroOne() * 0.1f) + 0.15f) * randCos;
    accel.z = ((Rand_ZeroOne() * 0.1f) + 0.15f) * -randSin;
    func_8002836C(play, &pos, &velocity, &accel, &this->bombSmokePrimColor, &this->bombSmokeEnvColor, 100, 25, 20);

    randAngle = Rand_ZeroOne() * 0x2000;
    randCos = Math_CosS(this->actor.shape.rot.y + randAngle);
    randSin = Math_SinS(this->actor.shape.rot.y + randAngle);

    pos.x = this->headPos.x + (randCos * 6.0f);
    pos.z = this->headPos.z - (randSin * 6.0f);
    velocity.x = randCos * 3.5f;
    velocity.y = this->bombSmokeEnvColor.r * 0.02f;
    velocity.z = -randSin * 3.5f;
    accel.x = ((Rand_ZeroOne() * 0.1f) + 0.15f) * -randCos;
    accel.z = ((Rand_ZeroOne() * 0.1f) + 0.15f) * randSin;
    func_8002836C(play, &pos, &velocity, &accel, &this->bombSmokePrimColor, &this->bombSmokeEnvColor, 100, 25, 20);
}

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x0D, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2800, ICHAIN_STOP),
};

void EnDodongo_Init(Actor* thisx, PlayState* play) {
    EnDodongo* this = (EnDodongo*)thisx;
    EffectBlureInit1 blureInit;

    this->actor.targetMode = 3;
    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->bombSmokePrimColor.r = this->bombSmokePrimColor.g = this->bombSmokeEnvColor.r = 255;
    this->bombSmokePrimColor.a = this->bombSmokeEnvColor.a = 200;
    this->bombSmokeEnvColor.g = 10;
    this->bodyScale.x = this->bodyScale.y = this->bodyScale.z = 1.0f;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 48.0f);
    Actor_SetScale(&this->actor, 0.01875f);
    SkelAnime_Init(play, &this->skelAnime, &gDodongoSkel, &gDodongoWaitAnim, this->jointTable, this->morphTable,
                   31);
    this->actor.colChkInfo.health = 4;
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.colChkInfo.damageTable = &sDamageTable;
    Collider_InitQuad(play, &this->colliderAT);
    Collider_InitTris(play, &this->colliderHard);
    Collider_InitJntSph(play, &this->colliderBody);
    Collider_SetQuad(play, &this->colliderAT, &this->actor, &sAttackQuadInit);
    Collider_SetTris(play, &this->colliderHard, &this->actor, &sHardTrisInit, this->trisElements);
    Collider_SetJntSph(play, &this->colliderBody, &this->actor, &sBodyJntSphInit, this->sphElements);

    blureInit.p1StartColor[0] = blureInit.p1StartColor[1] = blureInit.p1StartColor[2] = blureInit.p1StartColor[3] =
        blureInit.p2StartColor[0] = blureInit.p2StartColor[1] = blureInit.p2StartColor[2] = blureInit.p1EndColor[0] =
            blureInit.p1EndColor[1] = blureInit.p1EndColor[2] = blureInit.p2EndColor[0] = blureInit.p2EndColor[1] =
                blureInit.p2EndColor[2] = 255;

    blureInit.p1EndColor[3] = blureInit.p2EndColor[3] = 0;
    blureInit.p2StartColor[3] = 64;
    blureInit.elemDuration = 8;
    blureInit.unkFlag = false;
    blureInit.calcMode = 2;

    Effect_Add(play, &this->blureIdx, EFFECT_BLURE1, 0, 0, &blureInit);
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 60.0f, 70.0f, 0x1D);
    EnDodongo_SetupIdle(this);
}

void EnDodongo_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDodongo* this = (EnDodongo*)thisx;

    Effect_Delete(play, this->blureIdx);
    Collider_DestroyTris(play, &this->colliderHard);
    Collider_DestroyJntSph(play, &this->colliderBody);
    Collider_DestroyQuad(play, &this->colliderAT);
}

void EnDodongo_SetupIdle(EnDodongo* this) {
    Animation_MorphToLoop(&this->skelAnime, &gDodongoWaitAnim, -4.0f);
    this->actor.speedXZ = 0.0f;
    this->timer = Rand_S16Offset(30, 50);
    this->actionState = DODONGO_IDLE;
    EnDodongo_SetupAction(this, EnDodongo_Idle);
}

void EnDodongo_SetupWalk(EnDodongo* this) {
    f32 frames = Animation_GetLastFrame(&gDodongoWalkAnim);

    Animation_Change(&this->skelAnime, &gDodongoWalkAnim, 0.0f, 0.0f, frames, ANIMMODE_LOOP, -4.0f);
    this->actor.speedXZ = 1.5f;
    this->timer = Rand_S16Offset(50, 70);
    this->rightFootStep = true;
    this->actionState = DODONGO_WALK;
    EnDodongo_SetupAction(this, EnDodongo_Walk);
}

void EnDodongo_SetupBreatheFire(EnDodongo* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gDodongoBreatheFireAnim, -4.0f);
    this->actionState = DODONGO_BREATHE_FIRE;
    this->actor.speedXZ = 0.0f;
    EnDodongo_SetupAction(this, EnDodongo_BreatheFire);
}

void EnDodongo_SetupEndBreatheFire(EnDodongo* this) {
    Animation_PlayOnce(&this->skelAnime, &gDodongoAfterBreatheFireAnim);
    this->actionState = DODONGO_END_BREATHE_FIRE;
    this->actor.speedXZ = 0.0f;
    EnDodongo_SetupAction(this, EnDodongo_EndBreatheFire);
}

void EnDodongo_SetupSwallowBomb(EnDodongo* this) {
    Animation_Change(&this->skelAnime, &gDodongoBreatheFireAnim, -1.0f, 35.0f, 0.0f, ANIMMODE_ONCE, -4.0f);
    this->actionState = DODONGO_SWALLOW_BOMB;
    this->timer = 25;
    this->actor.speedXZ = 0.0f;
    EnDodongo_SetupAction(this, EnDodongo_SwallowBomb);
}

void EnDodongo_SetupStunned(EnDodongo* this) {
    Animation_Change(&this->skelAnime, &gDodongoBreatheFireAnim, 0.0f, 25.0f, 0.0f, ANIMMODE_ONCE, -4.0f);
    this->actionState = DODONGO_STUNNED;
    this->actor.speedXZ = 0.0f;
    if (this->damageEffect == 0xF) {
        this->iceTimer = 36;
    }
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    EnDodongo_SetupAction(this, EnDodongo_Stunned);
}

void EnDodongo_Idle(EnDodongo* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if ((DECR(this->timer) == 0) && Animation_OnFrame(&this->skelAnime, 0.0f)) {
        EnDodongo_SetupWalk(this);
    }
}

void EnDodongo_EndBreatheFire(EnDodongo* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        EnDodongo_SetupIdle(this);
        this->timer = Rand_S16Offset(10, 20);
    }
}

void EnDodongo_BreatheFire(EnDodongo* this, PlayState* play) {
    s32 pad;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;
    s16 pad2;
    s16 fireFrame;

    if ((s32)this->skelAnime.curFrame == 24) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_J_CRY);
    }
    if ((29.0f <= this->skelAnime.curFrame) && (this->skelAnime.curFrame <= 43.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_J_FIRE - SFX_FLAG);
        fireFrame = this->skelAnime.curFrame - 29.0f;
        pos = this->actor.world.pos;
        pos.y += 35.0f;
        EnDodongo_ShiftVecRadial(this->actor.world.rot.y, 30.0f, &pos);
        EnDodongo_ShiftVecRadial(this->actor.world.rot.y, 2.5f, &accel);
        EffectSsDFire_SpawnFixedScale(play, &pos, &velocity, &accel, 255 - (fireFrame * 10), fireFrame + 3);
    } else if ((2.0f <= this->skelAnime.curFrame) && (this->skelAnime.curFrame <= 20.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_J_BREATH - SFX_FLAG);
    }
    if (SkelAnime_Update(&this->skelAnime)) {
        EnDodongo_SetupEndBreatheFire(this);
    }
}

void EnDodongo_SwallowBomb(EnDodongo* this, PlayState* play) {
    Vec3f smokeVel = { 0.0f, 0.0f, 0.0f };
    Vec3f smokeAccel = { 0.0f, 0.6f, 0.0f };
    Color_RGBA8 white = { 255, 255, 255, 255 };
    Vec3f deathFireVel = { 0.0f, 0.0f, 0.0f };
    Vec3f deathFireAccel = { 0.0f, 1.0f, 0.0f };
    s16 i;
    Vec3f pos;
    s32 pad;

    if (this->actor.child != NULL) {
        this->actor.child->world.pos = this->mouthPos;
        ((EnBom*)this->actor.child)->timer++;
    } else if (this->actor.parent != NULL) {
        this->actor.parent->world.pos = this->mouthPos;
        ((EnBombf*)this->actor.parent)->timer++;
        //! @bug An explosive can also be a bombchu, not always a bomb, which leads to a serious bug. ->timer (0x1F8) is
        //! outside the bounds of the bombchu actor, and the memory it writes to happens to be one of the pointers in
        //! the next arena node. When this value is written to, massive memory corruption occurs.
    }

    if ((s32)this->skelAnime.curFrame == 28) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_J_EAT);
        if (this->actor.child != NULL) {
            Actor_Kill(this->actor.child);
            this->actor.child = NULL;
        } else if (this->actor.parent != NULL) {
            Actor_Kill(this->actor.parent);
            this->actor.parent = NULL;
        }
    } else if ((s32)this->skelAnime.curFrame == 24) {
        this->timer--;
        if (this->timer != 0) {
            this->skelAnime.curFrame++;
            if (this->timer == 10) {
                for (i = 10; i >= 0; i--) {
                    deathFireVel.x = Rand_CenteredFloat(10.0f);
                    deathFireVel.y = Rand_CenteredFloat(10.0f);
                    deathFireVel.z = Rand_CenteredFloat(10.0f);
                    deathFireAccel.x = deathFireVel.x * -0.1f;
                    deathFireAccel.y = deathFireVel.y * -0.1f;
                    deathFireAccel.z = deathFireVel.z * -0.1f;
                    pos.x = this->sphElements[0].dim.worldSphere.center.x + deathFireVel.x;
                    pos.y = this->sphElements[0].dim.worldSphere.center.y + deathFireVel.y;
                    pos.z = this->sphElements[0].dim.worldSphere.center.z + deathFireVel.z;
                    func_8002836C(play, &pos, &deathFireVel, &deathFireAccel, &this->bombSmokePrimColor,
                                  &this->bombSmokeEnvColor, 400, 10, 10);
                }
                Audio_PlayActorSound2(&this->actor, NA_SE_IT_BOMB_EXPLOSION);
                Actor_SetColorFilter(&this->actor, 0x4000, 0x78, 0, 8);
            }
        }
    }
    if ((s32)this->skelAnime.curFrame < 28) {
        if (((s32)this->skelAnime.curFrame < 26) && (this->timer <= 10)) {
            EnDodongo_SpawnBombSmoke(this, play);
        } else {
            pos = this->headPos;
            func_8002829C(play, &pos, &smokeVel, &smokeAccel, &white, &white, 50, 5);
            pos.x -= (Math_CosS(this->actor.shape.rot.y) * 6.0f);
            pos.z += (Math_SinS(this->actor.shape.rot.y) * 6.0f);
            func_8002829C(play, &pos, &smokeVel, &smokeAccel, &white, &white, 50, 5);
            pos.x = this->headPos.x + (Math_CosS(this->actor.shape.rot.y) * 6.0f);
            pos.z = this->headPos.z - (Math_SinS(this->actor.shape.rot.y) * 6.0f);
            func_8002829C(play, &pos, &smokeVel, &smokeAccel, &white, &white, 50, 5);
        }
    }
    this->bodyScale.y = this->bodyScale.z = (Math_SinS(this->actor.colorFilterTimer * 0x1000) * 0.5f) + 1.0f;
    this->bodyScale.x = Math_SinS(this->actor.colorFilterTimer * 0x1000) + 1.0f;

    SkelAnime_Update(&this->skelAnime);
    if (this->timer == 0) {
        EnDodongo_SetupDeath(this, play);
    }
}

void EnDodongo_Walk(EnDodongo* this, PlayState* play) {
    s32 pad;
    f32 playbackSpeed;
    Player* player = GET_PLAYER(play);
    s16 yawDiff = (s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y);

    yawDiff = ABS(yawDiff);

    Math_SmoothStepToF(&this->actor.speedXZ, 1.5f, 0.1f, 1.0f, 0.0f);

    playbackSpeed = this->actor.speedXZ * 0.75f;
    if (this->actor.speedXZ >= 0.0f) {
        if (playbackSpeed > 3.0f / 2) {
            playbackSpeed = 3.0f / 2;
        }
    } else {
        if (playbackSpeed < -3.0f / 2) {
            playbackSpeed = -3.0f / 2;
        }
    }
    this->skelAnime.playSpeed = playbackSpeed;

    SkelAnime_Update(&this->skelAnime);
    if ((s32)this->skelAnime.curFrame < 21) {
        if (!this->rightFootStep) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_J_WALK);
            Actor_SpawnFloorDustRing(play, &this->actor, &this->leftFootPos, 10.0f, 3, 2.0f, 200, 15, false);
            this->rightFootStep = true;
        }
    } else {
        if (this->rightFootStep) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_J_WALK);
            Actor_SpawnFloorDustRing(play, &this->actor, &this->rightFootPos, 10.0f, 3, 2.0f, 200, 15, false);
            this->rightFootStep = false;
        }
    }

    if (Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos) < 400.0f) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0x1F4, 0);
        this->actor.flags |= ACTOR_FLAG_0;
        if ((this->actor.xzDistToPlayer < 100.0f) && (yawDiff < 0x1388) && (this->actor.yDistToPlayer < 60.0f)) {
            EnDodongo_SetupBreatheFire(this);
        }
    } else {
        this->actor.flags &= ~ACTOR_FLAG_0;
        if ((Math_Vec3f_DistXZ(&this->actor.world.pos, &this->actor.home.pos) > 150.0f) || (this->retreatTimer != 0)) {
            s16 yawToHome = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos);

            Math_SmoothStepToS(&this->actor.world.rot.y, yawToHome, 1, 0x1F4, 0);
        }
        if (this->retreatTimer != 0) {
            this->retreatTimer--;
        }
        this->timer--;
        if (this->timer == 0) {
            if (Rand_ZeroOne() > 0.7f) {
                this->timer = Rand_S16Offset(50, 70);
                this->retreatTimer = Rand_S16Offset(15, 40);
            } else {
                EnDodongo_SetupIdle(this);
            }
        }
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void EnDodongo_SetupSweepTail(EnDodongo* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gDodongoDamageAnim, -4.0f);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_J_DAMAGE);
    this->actionState = DODONGO_SWEEP_TAIL;
    this->timer = 0;
    this->actor.speedXZ = 0.0f;
    EnDodongo_SetupAction(this, EnDodongo_SweepTail);
}

void EnDodongo_SweepTail(EnDodongo* this, PlayState* play) {
    s16 yawDiff1 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (SkelAnime_Update(&this->skelAnime)) {
        if ((this->timer != 0) || (ABS(yawDiff1) < 0x4000)) {
            this->sphElements[2].info.toucherFlags = TOUCH_NONE;
            this->sphElements[1].info.toucherFlags = TOUCH_NONE;
            this->colliderBody.base.atFlags = AT_NONE;
            this->sphElements[2].info.toucher.dmgFlags = 0;
            this->sphElements[1].info.toucher.dmgFlags = 0;
            this->sphElements[2].info.toucher.damage = 0;
            this->sphElements[1].info.toucher.damage = 0;
            EnDodongo_SetupBreatheFire(this);
            this->timer = Rand_S16Offset(5, 10);
        } else {
            s16 yawDiff2 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
            AnimationHeader* animation;

            this->tailSwipeSpeed = (0xFFFF - ABS(yawDiff2)) / 0xF;
            if ((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y) >= 0) {
                this->tailSwipeSpeed = -this->tailSwipeSpeed;
                animation = &gDodongoSweepTailLeftAnim;
            } else {
                animation = &gDodongoSweepTailRightAnim;
            }
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_J_TAIL);
            Animation_PlayOnceSetSpeed(&this->skelAnime, animation, 2.0f);
            this->timer = 18;
            this->colliderBody.base.atFlags = this->sphElements[1].info.toucherFlags =
                this->sphElements[2].info.toucherFlags = AT_ON | AT_TYPE_ENEMY; // also TOUCH_ON | TOUCH_SFX_WOOD
            this->sphElements[1].info.toucher.dmgFlags = this->sphElements[2].info.toucher.dmgFlags = 0xFFCFFFFF;
            this->sphElements[1].info.toucher.damage = this->sphElements[2].info.toucher.damage = 8;
        }
    } else if (this->timer > 1) {
        Vec3f tailPos;

        this->timer--;
        this->actor.shape.rot.y = this->actor.world.rot.y += this->tailSwipeSpeed;
        tailPos.x = this->sphElements[1].dim.worldSphere.center.x;
        tailPos.y = this->sphElements[1].dim.worldSphere.center.y;
        tailPos.z = this->sphElements[1].dim.worldSphere.center.z;
        Actor_SpawnFloorDustRing(play, &this->actor, &tailPos, 5.0f, 2, 2.0f, 100, 15, false);
        tailPos.x = this->sphElements[2].dim.worldSphere.center.x;
        tailPos.y = this->sphElements[2].dim.worldSphere.center.y;
        tailPos.z = this->sphElements[2].dim.worldSphere.center.z;
        Actor_SpawnFloorDustRing(play, &this->actor, &tailPos, 5.0f, 2, 2.0f, 100, 15, false);

        if (this->colliderBody.base.atFlags & AT_HIT) {
            Player* player = GET_PLAYER(play);

            if (this->colliderBody.base.at == &player->actor) {
                Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
            }
        }
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderBody.base);
    }
}

void EnDodongo_SetupDeath(EnDodongo* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gDodongoDieAnim, -8.0f);
    this->timer = 0;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_J_DEAD);
    this->actionState = DODONGO_DEATH;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->actor.speedXZ = 0.0f;
    EnDodongo_SetupAction(this, EnDodongo_Death);
    gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DODONGO]++;
}

void EnDodongo_Death(EnDodongo* this, PlayState* play) {
    EnBom* bomb;

    if (this->skelAnime.curFrame < 35.0f) {
        if (this->actor.params == EN_DODONGO_SMOKE_DEATH) {
            EnDodongo_SpawnBombSmoke(this, play);
        }
    } else if (this->actor.colorFilterTimer == 0) {
        Actor_SetColorFilter(&this->actor, 0x4000, 0x78, 0, 4);
    }
    if (SkelAnime_Update(&this->skelAnime) != 0) {
        if (this->timer == 0) {
            bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 6, BOMB_BODY);
            if (bomb != NULL) {
                bomb->timer = 0;
                this->timer = 8;
            }
        }
    } else if ((s32)this->skelAnime.curFrame == 52) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_DOWN);
    }
    if (this->timer != 0) {
        this->timer--;
        if (this->timer == 0) {
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x40);
            Actor_Kill(&this->actor);
        }
    }
}

void EnDodongo_Stunned(EnDodongo* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->actor.colorFilterTimer == 0) {
        if (this->actor.colChkInfo.health == 0) {
            EnDodongo_SetupDeath(this, play);
        } else {
            EnDodongo_SetupIdle(this);
        }
    }
}

void EnDodongo_CollisionCheck(EnDodongo* this, PlayState* play) {
    if (this->colliderHard.base.acFlags & AC_BOUNCED) {
        this->colliderHard.base.acFlags &= ~AC_BOUNCED;
        this->colliderBody.base.acFlags &= ~AC_HIT;
    } else if ((this->colliderBody.base.acFlags & AC_HIT) && (this->actionState > DODONGO_DEATH)) {
        this->colliderBody.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlagJntSph(&this->actor, &this->colliderBody, 0);
        if (this->actor.colChkInfo.damageEffect != 0xE) {
            this->damageEffect = this->actor.colChkInfo.damageEffect;
            if ((this->actor.colChkInfo.damageEffect == 1) || (this->actor.colChkInfo.damageEffect == 0xF)) {
                if (this->actionState != DODONGO_STUNNED) {
                    Actor_SetColorFilter(&this->actor, 0, 0x78, 0, 0x50);
                    Actor_ApplyDamage(&this->actor);
                    EnDodongo_SetupStunned(this);
                }
            } else {
                Actor_SetColorFilter(&this->actor, 0x4000, 0x78, 0, 8);
                if (Actor_ApplyDamage(&this->actor) == 0) {
                    EnDodongo_SetupDeath(this, play);
                } else {
                    EnDodongo_SetupSweepTail(this);
                }
            }
        }
    }
}

void EnDodongo_UpdateQuad(EnDodongo* this, PlayState* play) {
    Vec3f sp94 = { -1000.0f, -1500.0f, 0.0f };
    Vec3f sp88 = { -1000.0f, -200.0f, 1500.0f };
    Vec3f sp7C = { -1000.0f, -200.0f, -1500.0f };
    Vec3f sp70 = { 0.0f, 0.0f, 0.0f };
    s32 pad4C[9]; // Possibly 3 more Vec3fs?
    s32 a = 0;
    s32 b = 1; // These indices are needed to match.
    s32 c = 2; // Might be a way to quickly test vertex arrangements
    s32 d = 3;
    f32 xMod = Math_SinF((this->skelAnime.curFrame - 28.0f) * 0.08f) * 5500.0f;

    sp7C.x -= xMod;
    sp94.x -= xMod;
    sp88.x -= xMod;

    Matrix_MultVec3f(&sp94, &this->colliderAT.dim.quad[b]);
    Matrix_MultVec3f(&sp88, &this->colliderAT.dim.quad[a]);
    Matrix_MultVec3f(&sp7C, &this->colliderAT.dim.quad[d]);
    Matrix_MultVec3f(&sp70, &this->colliderAT.dim.quad[c]);

    Collider_SetQuadVertices(&this->colliderAT, &this->colliderAT.dim.quad[a], &this->colliderAT.dim.quad[b],
                             &this->colliderAT.dim.quad[c], &this->colliderAT.dim.quad[d]);
}

void EnDodongo_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDodongo* this = (EnDodongo*)thisx;

    EnDodongo_CollisionCheck(this, play);
    if (this->actor.colChkInfo.damageEffect != 0xE) {
        this->actionFunc(this, play);
        Actor_MoveForward(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 60.0f, 70.0f, 0x1D);
        if (this->actor.bgCheckFlags & 2) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_DOWN);
        }
    }
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderBody.base);
    if (this->actionState != DODONGO_DEATH) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderHard.base);
    }
    if (this->actionState > DODONGO_DEATH) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderBody.base);
    }
    if ((this->actionState >= DODONGO_IDLE) && EnDodongo_AteBomb(this, play)) {
        EnDodongo_SetupSwallowBomb(this);
    }
    if (this->actionState == DODONGO_BREATHE_FIRE) {
        if ((29.0f < this->skelAnime.curFrame) && (this->skelAnime.curFrame < 43.0f)) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderAT.base);
        }
    }
    this->actor.focus.pos.x = this->actor.world.pos.x + Math_SinS(this->actor.shape.rot.y) * -30.0f;
    this->actor.focus.pos.y = this->actor.world.pos.y + 20.0f;
    this->actor.focus.pos.z = this->actor.world.pos.z + Math_CosS(this->actor.shape.rot.y) * -30.0f;
}

s32 EnDodongo_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                               void* thisx) {
    EnDodongo* this = (EnDodongo*)thisx;

    if ((limbIndex == 15) || (limbIndex == 16)) {
        Matrix_Scale(this->bodyScale.x, this->bodyScale.y, this->bodyScale.z, MTXMODE_APPLY);
    }
    return false;
}

void EnDodongo_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f legOffsets[3] = {
        { 1100.0f, -700.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { 2190.0f, 0.0f, 0.0f },
    };
    Vec3f tailTipOffset = { 3000.0f, 0.0f, 0.0f };
    Vec3f baseOffset = { 0.0f, 0.0f, 0.0f };
    s32 i;
    Vec3f hardTris0Vtx[3];
    Vec3f hardTris1Vtx[3];
    Vec3f hardTris2Vtx[3];
    Vec3f tailTip;
    Vec3f tailBase;
    EnDodongo* this = (EnDodongo*)thisx;
    Vec3f hardTris0VtxOffset[] = {
        { -300.0f, -2500.0f, 0.0f },
        { -300.0f, 1200.0f, -2700.0f },
        { 3000.0f, 1200.0f, 0.0f },
    };
    Vec3f hardTris1VtxOffset[] = {
        { -300.0f, -2500.0f, 0.0f },
        { -300.0f, 1200.0f, 2700.0f },
        { 3000.0f, 1200.0f, 0.0f },
    };
    Vec3f hardTris2VtxOffset[] = {
        { -600.0f, 1200.0f, -2800.0f },
        { -600.0f, 1200.0f, 2800.0f },
        { 3000.0f, 1200.0f, 0.0f },
    };
    Vec3f mouthOffset = { 1800.0f, 1200.0f, 0.0f };
    Vec3f headOffset = { 1500.0f, 300.0f, 0.0f };

    Collider_UpdateSpheres(limbIndex, &this->colliderBody);

    switch (limbIndex) {
        case 2:
            if ((this->actionState == DODONGO_BREATHE_FIRE) && (29.0f < this->skelAnime.curFrame) &&
                (this->skelAnime.curFrame < 43.0f)) {
                EnDodongo_UpdateQuad(this, play);
            }
            break;
        case 7:
            for (i = 0; i < 3; i++) {
                Matrix_MultVec3f(&hardTris0VtxOffset[i], &hardTris0Vtx[i]);
                Matrix_MultVec3f(&hardTris1VtxOffset[i], &hardTris1Vtx[i]);
                Matrix_MultVec3f(&hardTris2VtxOffset[i], &hardTris2Vtx[i]);
            }
            Collider_SetTrisVertices(&this->colliderHard, 0, &hardTris0Vtx[0], &hardTris0Vtx[1], &hardTris0Vtx[2]);
            Collider_SetTrisVertices(&this->colliderHard, 1, &hardTris1Vtx[0], &hardTris1Vtx[1], &hardTris1Vtx[2]);
            Collider_SetTrisVertices(&this->colliderHard, 2, &hardTris2Vtx[0], &hardTris2Vtx[1], &hardTris2Vtx[2]);
            Matrix_MultVec3f(&mouthOffset, &this->mouthPos);
            Matrix_MultVec3f(&headOffset, &this->headPos);
            break;
        case 15:
            if ((this->actionState == DODONGO_SWEEP_TAIL) && (this->timer >= 2)) {
                Matrix_MultVec3f(&tailTipOffset, &tailTip);
                Matrix_MultVec3f(&baseOffset, &tailBase);
                EffectBlure_AddVertex(Effect_GetByIndex(this->blureIdx), &tailTip, &tailBase);
            } else if ((this->actionState == DODONGO_SWEEP_TAIL) && (this->timer != 0)) {
                EffectBlure_AddSpace(Effect_GetByIndex(this->blureIdx));
            }
            break;
        case 21:
            Matrix_MultVec3f(&legOffsets[1], &this->leftFootPos);
            break;
        case 28:
            Matrix_MultVec3f(&legOffsets[1], &this->rightFootPos);
            break;
    }
    if (this->iceTimer != 0) {
        i = -1;
        switch (limbIndex) {
            case 7:
                baseOffset.x = 1200.0f;
                i = 0;
                break;
            case 13:
                i = 1;
                break;
            case 14:
                i = 2;
                break;
            case 15:
                i = 3;
                break;
            case 16:
                i = 4;
                break;
            case 22:
                i = 5;
                break;
            case 29:
                i = 6;
                break;
            case 21:
                i = 7;
                break;
            case 28:
                i = 8;
                break;
        }
        if (i >= 0) {
            Matrix_MultVec3f(&baseOffset, &this->icePos[i]);
        }
    }
}

void EnDodongo_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnDodongo* this = (EnDodongo*)thisx;
    s32 index;

    func_80093D18(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnDodongo_OverrideLimbDraw,
                      EnDodongo_PostLimbDraw, this);

    if (this->iceTimer != 0) {
        this->actor.colorFilterTimer++;
        this->iceTimer--;
        if ((this->iceTimer % 4) == 0) {
            index = this->iceTimer >> 2;
            EffectSsEnIce_SpawnFlyingVec3f(play, &this->actor, &this->icePos[index], 150, 150, 150, 250, 235, 245,
                                           255, 1.8f);
        }
    }
}

void EnDodongo_ShiftVecRadial(s16 yaw, f32 radius, Vec3f* vec) {
    vec->x += Math_SinS(yaw) * radius;
    vec->z += Math_CosS(yaw) * radius;
}

s32 EnDodongo_AteBomb(EnDodongo* this, PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;
    f32 dx;
    f32 dy;
    f32 dz;

    while (actor != NULL) {
        if ((actor->params != 0) || (actor->parent != NULL)) {
            actor = actor->next;
            continue;
        }
        dx = actor->world.pos.x - this->mouthPos.x;
        dy = actor->world.pos.y - this->mouthPos.y;
        dz = actor->world.pos.z - this->mouthPos.z;
        if ((fabsf(dx) < 20.0f) && (fabsf(dy) < 10.0f) && (fabsf(dz) < 20.0f)) {
            if (actor->id == ACTOR_EN_BOM) {
                this->actor.child = actor;
            } else {
                this->actor.parent = actor;
            }
            actor->parent = &this->actor;
            return true;
        }
        actor = actor->next;
    }
    return false;
}
