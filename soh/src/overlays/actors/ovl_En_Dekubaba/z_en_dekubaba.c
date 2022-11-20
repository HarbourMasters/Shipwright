#include "z_en_dekubaba.h"
#include "objects/object_dekubaba/object_dekubaba.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2)

void EnDekubaba_Init(Actor* thisx, PlayState* play);
void EnDekubaba_Destroy(Actor* thisx, PlayState* play);
void EnDekubaba_Update(Actor* thisx, PlayState* play);
void EnDekubaba_Draw(Actor* thisx, PlayState* play);
void EnDekuBaba_Reset(void);

void EnDekubaba_SetupWait(EnDekubaba* this);
void EnDekubaba_SetupGrow(EnDekubaba* this);
void EnDekubaba_Wait(EnDekubaba* this, PlayState* play);
void EnDekubaba_Grow(EnDekubaba* this, PlayState* play);
void EnDekubaba_Retract(EnDekubaba* this, PlayState* play);
void EnDekubaba_DecideLunge(EnDekubaba* this, PlayState* play);
void EnDekubaba_Lunge(EnDekubaba* this, PlayState* play);
void EnDekubaba_PrepareLunge(EnDekubaba* this, PlayState* play);
void EnDekubaba_PullBack(EnDekubaba* this, PlayState* play);
void EnDekubaba_Recover(EnDekubaba* this, PlayState* play);
void EnDekubaba_Hit(EnDekubaba* this, PlayState* play);
void EnDekubaba_StunnedVertical(EnDekubaba* this, PlayState* play);
void EnDekubaba_Sway(EnDekubaba* this, PlayState* play);
void EnDekubaba_PrunedSomersault(EnDekubaba* this, PlayState* play);
void EnDekubaba_ShrinkDie(EnDekubaba* this, PlayState* play);
void EnDekubaba_DeadStickDrop(EnDekubaba* this, PlayState* play);

static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };

const ActorInit En_Dekubaba_InitVars = {
    ACTOR_EN_DEKUBABA,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_DEKUBABA,
    sizeof(EnDekubaba),
    (ActorFunc)EnDekubaba_Init,
    (ActorFunc)EnDekubaba_Destroy,
    (ActorFunc)EnDekubaba_Update,
    (ActorFunc)EnDekubaba_Draw,
    (ActorResetFunc)EnDekuBaba_Reset,
};

static ColliderJntSphElementInit sJntSphElementsInit[7] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_HARD,
            BUMP_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 100, 1000 }, 15 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 51, { { 0, 0, 1500 }, 8 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 52, { { 0, 0, 500 }, 8 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 53, { { 0, 0, 1500 }, 8 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 54, { { 0, 0, 500 }, 8 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 55, { { 0, 0, 1500 }, 8 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 56, { { 0, 0, 500 }, 8 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    7,
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 2, 25, 25, MASS_IMMOVABLE };

typedef enum {
    /* 0x0 */ DEKUBABA_DMGEFF_NONE,
    /* 0x1 */ DEKUBABA_DMGEFF_DEKUNUT,
    /* 0x2 */ DEKUBABA_DMGEFF_FIRE,
    /* 0xE */ DEKUBABA_DMGEFF_BOOMERANG = 14,
    /* 0xF */ DEKUBABA_DMGEFF_SWORD
} DekuBabaDamageEffect;

static DamageTable sDekuBabaDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, DEKUBABA_DMGEFF_DEKUNUT),
    /* Deku stick    */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Slingshot     */ DMG_ENTRY(1, DEKUBABA_DMGEFF_NONE),
    /* Explosive     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_BOOMERANG),
    /* Normal arrow  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Hookshot      */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Kokiri sword  */ DMG_ENTRY(1, DEKUBABA_DMGEFF_SWORD),
    /* Master sword  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Giant's Knife */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Fire arrow    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Light arrow   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 1   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Light magic   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Shield        */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, DEKUBABA_DMGEFF_SWORD),
    /* Giant spin    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Master spin   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Kokiri jump   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Giant jump    */ DMG_ENTRY(8, DEKUBABA_DMGEFF_SWORD),
    /* Master jump   */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Unknown 1     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, DEKUBABA_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
};

// The only difference is that for Big Deku Babas, Hookshot will act the same as Deku Nuts: i.e. it will stun, but
// cannot kill.
static DamageTable sBigDekuBabaDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, DEKUBABA_DMGEFF_DEKUNUT),
    /* Deku stick    */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Slingshot     */ DMG_ENTRY(1, DEKUBABA_DMGEFF_NONE),
    /* Explosive     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_BOOMERANG),
    /* Normal arrow  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Hookshot      */ DMG_ENTRY(0, DEKUBABA_DMGEFF_DEKUNUT),
    /* Kokiri sword  */ DMG_ENTRY(1, DEKUBABA_DMGEFF_SWORD),
    /* Master sword  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Giant's Knife */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Fire arrow    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Light arrow   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 1   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Light magic   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Shield        */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, DEKUBABA_DMGEFF_SWORD),
    /* Giant spin    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Master spin   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Kokiri jump   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Giant jump    */ DMG_ENTRY(8, DEKUBABA_DMGEFF_SWORD),
    /* Master jump   */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Unknown 1     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, DEKUBABA_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 1500, ICHAIN_STOP),
};

void EnDekubaba_Init(Actor* thisx, PlayState* play) {
    EnDekubaba* this = (EnDekubaba*)thisx;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 22.0f);
    SkelAnime_Init(play, &this->skelAnime, &gDekuBabaSkel, &gDekuBabaFastChompAnim, this->jointTable,
                   this->morphTable, 8);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElements);

    if (this->actor.params == DEKUBABA_BIG) {
        this->size = 2.5f;

        for (i = 0; i < sJntSphInit.count; i++) {
            this->collider.elements[i].dim.worldSphere.radius = this->collider.elements[i].dim.modelSphere.radius =
                (sJntSphElementsInit[i].dim.modelSphere.radius * 2.50f);
        }

        // This and its counterpart below mean that a Deku Stick jumpslash will not trigger the Deku Stick drop route.
        // (Of course they reckoned without each age being able to use the other's items, so Stick and Master Sword
        // jumpslash can give the Stick drop as adult, and neither will as child.)
        if (!LINK_IS_ADULT) {
            sBigDekuBabaDamageTable.table[0x1B] = DMG_ENTRY(4, DEKUBABA_DMGEFF_NONE); // DMG_JUMP_MASTER
        }

        CollisionCheck_SetInfo(&this->actor.colChkInfo, &sBigDekuBabaDamageTable, &sColChkInfoInit);
        this->actor.colChkInfo.health = 4;
        this->actor.naviEnemyId = 0x08; // Big Deku Baba
        this->actor.targetMode = 2;
    } else {
        this->size = 1.0f;

        for (i = 0; i < sJntSphInit.count; i++) {
            this->collider.elements[i].dim.worldSphere.radius = this->collider.elements[i].dim.modelSphere.radius;
        }

        if (!LINK_IS_ADULT) {
            sDekuBabaDamageTable.table[0x1B] = DMG_ENTRY(4, DEKUBABA_DMGEFF_NONE); // DMG_JUMP_MASTER
        }

        CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDekuBabaDamageTable, &sColChkInfoInit);
        this->actor.naviEnemyId = 0x07; // Deku Baba
        this->actor.targetMode = 1;
    }

    EnDekubaba_SetupWait(this);
    this->timer = 0;
    this->boundFloor = NULL;
    this->bodyPartsPos[3] = this->actor.home.pos;
}

void EnDekubaba_Destroy(Actor* thisx, PlayState* play) {
    EnDekubaba* this = (EnDekubaba*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void EnDekubaba_DisableHitboxes(EnDekubaba* this) {
    s32 i;

    for (i = 1; i < ARRAY_COUNT(this->colliderElements); i++) {
        this->collider.elements[i].info.bumperFlags &= ~BUMP_ON;
    }
}

void EnDekubaba_SetupWait(EnDekubaba* this) {
    s32 i;
    ColliderJntSphElement* element;

    this->actor.shape.rot.x = -0x4000;
    this->stemSectionAngle[0] = this->stemSectionAngle[1] = this->stemSectionAngle[2] = this->actor.shape.rot.x;

    this->actor.world.pos.x = this->actor.home.pos.x;
    this->actor.world.pos.z = this->actor.home.pos.z;
    this->actor.world.pos.y = this->actor.home.pos.y + 14.0f * this->size;

    Actor_SetScale(&this->actor, this->size * 0.01f * 0.5f);

    this->collider.base.colType = COLTYPE_HARD;
    this->collider.base.acFlags |= AC_HARD;
    this->timer = 45;

    for (i = 1; i < ARRAY_COUNT(this->colliderElements); i++) {
        element = &this->collider.elements[i];
        element->dim.worldSphere.center.x = this->actor.world.pos.x;
        element->dim.worldSphere.center.y = (s16)this->actor.world.pos.y - 7;
        element->dim.worldSphere.center.z = this->actor.world.pos.z;
    }

    this->actionFunc = EnDekubaba_Wait;
}

void EnDekubaba_SetupGrow(EnDekubaba* this) {
    s32 i;

    Animation_Change(&this->skelAnime, &gDekuBabaFastChompAnim,
                     Animation_GetLastFrame(&gDekuBabaFastChompAnim) * (1.0f / 15), 0.0f,
                     Animation_GetLastFrame(&gDekuBabaFastChompAnim), ANIMMODE_ONCE, 0.0f);

    this->timer = 15;

    for (i = 2; i < ARRAY_COUNT(this->colliderElements); i++) {
        this->collider.elements[i].info.ocElemFlags |= OCELEM_ON;
    }

    this->collider.base.colType = COLTYPE_HIT6;
    this->collider.base.acFlags &= ~AC_HARD;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DUMMY482);
    this->actionFunc = EnDekubaba_Grow;
}

void EnDekubaba_SetupRetract(EnDekubaba* this) {
    s32 i;

    Animation_Change(&this->skelAnime, &gDekuBabaFastChompAnim, -1.5f, Animation_GetLastFrame(&gDekuBabaFastChompAnim),
                     0.0f, ANIMMODE_ONCE, -3.0f);

    this->timer = 15;

    for (i = 2; i < ARRAY_COUNT(this->colliderElements); i++) {
        this->collider.elements[i].info.ocElemFlags &= ~OCELEM_ON;
    }

    this->actionFunc = EnDekubaba_Retract;
}

void EnDekubaba_SetupDecideLunge(EnDekubaba* this) {
    this->timer = Animation_GetLastFrame(&gDekuBabaFastChompAnim) * 2;
    Animation_MorphToLoop(&this->skelAnime, &gDekuBabaFastChompAnim, -3.0f);
    this->actionFunc = EnDekubaba_DecideLunge;
}

void EnDekubaba_SetupPrepareLunge(EnDekubaba* this) {
    this->timer = 8;
    this->skelAnime.playSpeed = 0.0f;
    this->actionFunc = EnDekubaba_PrepareLunge;
}

void EnDekubaba_SetupLunge(EnDekubaba* this) {
    Animation_PlayOnce(&this->skelAnime, &gDekuBabaPauseChompAnim);
    this->timer = 0;
    this->actionFunc = EnDekubaba_Lunge;
}

void EnDekubaba_SetupPullBack(EnDekubaba* this) {
    Animation_Change(&this->skelAnime, &gDekuBabaPauseChompAnim, 1.0f, 15.0f,
                     Animation_GetLastFrame(&gDekuBabaPauseChompAnim), ANIMMODE_ONCE, -3.0f);
    this->timer = 0;
    this->actionFunc = EnDekubaba_PullBack;
}

void EnDekubaba_SetupRecover(EnDekubaba* this) {
    this->timer = 9;
    this->collider.base.acFlags |= AC_ON;
    this->skelAnime.playSpeed = -1.0f;
    this->actionFunc = EnDekubaba_Recover;
}

void EnDekubaba_SetupHit(EnDekubaba* this, s32 arg1) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gDekuBabaPauseChompAnim, -5.0f);
    this->timer = arg1;
    this->collider.base.acFlags &= ~AC_ON;
    Actor_SetScale(&this->actor, this->size * 0.01f);

    if (arg1 == 2) {
        Actor_SetColorFilter(&this->actor, 0, 155, 0, 62);
    } else {
        Actor_SetColorFilter(&this->actor, 0x4000, 255, 0, 42);
    }

    this->actionFunc = EnDekubaba_Hit;
}

void EnDekubaba_SetupPrunedSomersault(EnDekubaba* this) {
    this->timer = 0;
    this->skelAnime.playSpeed = 0.0f;
    this->actor.gravity = -0.8f;
    this->actor.velocity.y = 4.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y + 0x8000;
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.speedXZ = this->size * 3.0f;
    this->actor.flags |= ACTOR_FLAG_4 | ACTOR_FLAG_5;
    this->actionFunc = EnDekubaba_PrunedSomersault;

    if (this->actor.params == DEKUBABA_BIG) {
        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DEKU_BABA_BIG]++;
    } else {
        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DEKU_BABA]++;
    }
}

void EnDekubaba_SetupShrinkDie(EnDekubaba* this) {
    Animation_Change(&this->skelAnime, &gDekuBabaFastChompAnim, -1.5f, Animation_GetLastFrame(&gDekuBabaFastChompAnim),
                     0.0f, ANIMMODE_ONCE, -3.0f);
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = EnDekubaba_ShrinkDie;

    if (this->actor.params == DEKUBABA_BIG) {
        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DEKU_BABA_BIG]++;
    } else {
        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_DEKU_BABA]++;
    }
}

void EnDekubaba_SetupStunnedVertical(EnDekubaba* this) {
    s32 i;

    for (i = 1; i < ARRAY_COUNT(this->colliderElements); i++) {
        this->collider.elements[i].info.bumperFlags |= BUMP_ON;
    }

    if (this->timer == 1) {
        Animation_Change(&this->skelAnime, &gDekuBabaFastChompAnim, 4.0f, 0.0f,
                         Animation_GetLastFrame(&gDekuBabaFastChompAnim), ANIMMODE_LOOP, -3.0f);
        this->timer = 40;
    } else {
        Animation_Change(&this->skelAnime, &gDekuBabaFastChompAnim, 0.0f, 0.0f,
                         Animation_GetLastFrame(&gDekuBabaFastChompAnim), ANIMMODE_LOOP, -3.0f);
        this->timer = 60;
    }

    this->actor.world.pos.x = this->actor.home.pos.x;
    this->actor.world.pos.y = this->actor.home.pos.y + (60.0f * this->size);
    this->actor.world.pos.z = this->actor.home.pos.z;
    this->actionFunc = EnDekubaba_StunnedVertical;
}

void EnDekubaba_SetupSway(EnDekubaba* this) {
    this->targetSwayAngle = -0x6000;
    this->stemSectionAngle[2] = -0x5000;
    this->stemSectionAngle[1] = -0x4800;

    EnDekubaba_DisableHitboxes(this);
    Actor_SetColorFilter(&this->actor, 0x4000, 255, 0, 35);
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = EnDekubaba_Sway;
}

void EnDekubaba_SetupDeadStickDrop(EnDekubaba* this, PlayState* play) {
    Actor_SetScale(&this->actor, 0.03f);
    this->actor.shape.rot.x -= 0x4000;
    this->actor.shape.yOffset = 1000.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.shape.shadowScale = 3.0f;
    Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_MISC);
    this->actor.flags &= ~ACTOR_FLAG_5;
    this->timer = 200;
    this->actionFunc = EnDekubaba_DeadStickDrop;
}

// Action functions

void EnDekubaba_Wait(EnDekubaba* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.world.pos.x = this->actor.home.pos.x;
    this->actor.world.pos.z = this->actor.home.pos.z;
    this->actor.world.pos.y = this->actor.home.pos.y + 14.0f * this->size;

    if ((this->timer == 0) && (this->actor.xzDistToPlayer < 200.0f * this->size) &&
        (fabsf(this->actor.yDistToPlayer) < 30.0f * this->size)) {
        EnDekubaba_SetupGrow(this);
    }
}

void EnDekubaba_Grow(EnDekubaba* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 headDistHorizontal;
    f32 headDistVertical;
    f32 headShiftX;
    f32 headShiftZ;

    if (this->timer != 0) {
        this->timer--;
    }

    SkelAnime_Update(&this->skelAnime);

    this->actor.scale.x = this->actor.scale.y = this->actor.scale.z =
        this->size * 0.01f * (0.5f + (15 - this->timer) * 0.5f / 15.0f);
    Math_ScaledStepToS(&this->actor.shape.rot.x, 0x1800, 0x800);

    headDistVertical = sinf(CLAMP_MAX((15 - this->timer) * (1.0f / 15), 0.7f) * M_PI) * 32.0f + 14.0f;

    if (this->actor.shape.rot.x < -0x38E3) {
        headDistHorizontal = 0.0f;
    } else if (this->actor.shape.rot.x < -0x238E) {
        Math_ScaledStepToS(&this->stemSectionAngle[0], -0x5555, 0x38E);
        headDistHorizontal = Math_CosS(this->stemSectionAngle[0]) * 20.0f;
    } else if (this->actor.shape.rot.x < -0xE38) {
        Math_ScaledStepToS(&this->stemSectionAngle[0], -0xAAA, 0x38E);
        Math_ScaledStepToS(&this->stemSectionAngle[1], -0x5555, 0x38E);
        Math_ScaledStepToS(&this->stemSectionAngle[2], -0x5555, 0x222);

        headDistHorizontal = 20.0f * (Math_CosS(this->stemSectionAngle[0]) + Math_CosS(this->stemSectionAngle[1])) +
                             (headDistVertical -
                              20.0f * (-Math_SinS(this->stemSectionAngle[0]) - Math_SinS(this->stemSectionAngle[1]))) *
                                 Math_CosS(this->stemSectionAngle[2]) / -Math_SinS(this->stemSectionAngle[2]);
    } else {
        Math_ScaledStepToS(&this->stemSectionAngle[0], -0xAAA, 0x38E);
        Math_ScaledStepToS(&this->stemSectionAngle[1], -0x31C7, 0x222);
        Math_ScaledStepToS(&this->stemSectionAngle[2], -0x5555, 0x222);

        headDistHorizontal = 20.0f * (Math_CosS(this->stemSectionAngle[0]) + Math_CosS(this->stemSectionAngle[1])) +
                             (headDistVertical -
                              20.0f * (-Math_SinS(this->stemSectionAngle[0]) - Math_SinS(this->stemSectionAngle[1]))) *
                                 Math_CosS(this->stemSectionAngle[2]) / -Math_SinS(this->stemSectionAngle[2]);
    }

    if (this->timer < 10) {
        Math_ApproachS(&this->actor.shape.rot.y, Math_Vec3f_Yaw(&this->actor.home.pos, &player->actor.world.pos), 2,
                       0xE38);
        if (headShiftZ) {} // One way of fake-matching
    }

    this->actor.world.pos.y = this->actor.home.pos.y + (headDistVertical * this->size);
    headShiftX = headDistHorizontal * this->size * Math_SinS(this->actor.shape.rot.y);
    headShiftZ = headDistHorizontal * this->size * Math_CosS(this->actor.shape.rot.y);
    this->actor.world.pos.x = this->actor.home.pos.x + headShiftX;
    this->actor.world.pos.z = this->actor.home.pos.z + headShiftZ;

    EffectSsHahen_SpawnBurst(play, &this->actor.home.pos, this->size * 3.0f, 0, this->size * 12.0f,
                             this->size * 5.0f, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);

    if (this->timer == 0) {
        if (Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos) < 240.0f * this->size) {
            EnDekubaba_SetupPrepareLunge(this);
        } else {
            EnDekubaba_SetupRetract(this);
        }
    }
}

void EnDekubaba_Retract(EnDekubaba* this, PlayState* play) {
    f32 headDistHorizontal;
    f32 headDistVertical;
    f32 xShift;
    f32 zShift;

    if (this->timer != 0) {
        this->timer--;
    }

    SkelAnime_Update(&this->skelAnime);

    this->actor.scale.x = this->actor.scale.y = this->actor.scale.z =
        this->size * 0.01f * (0.5f + this->timer * (1.0f / 30));
    Math_ScaledStepToS(&this->actor.shape.rot.x, -0x4000, 0x300);

    headDistVertical = (sinf(CLAMP_MAX(this->timer * 0.033f, 0.7f) * M_PI) * 32.0f) + 14.0f;

    if (this->actor.shape.rot.x < -0x38E3) {
        headDistHorizontal = 0.0f;
    } else if (this->actor.shape.rot.x < -0x238E) {
        Math_ScaledStepToS(&this->stemSectionAngle[0], -0x4000, 0x555);
        headDistHorizontal = Math_CosS(this->stemSectionAngle[0]) * 20.0f;
    } else if (this->actor.shape.rot.x < -0xE38) {
        Math_ScaledStepToS(&this->stemSectionAngle[0], -0x5555, 0x555);
        Math_ScaledStepToS(&this->stemSectionAngle[1], -0x4000, 0x555);
        Math_ScaledStepToS(&this->stemSectionAngle[2], -0x4000, 0x333);

        headDistHorizontal = 20.0f * (Math_CosS(this->stemSectionAngle[0]) + Math_CosS(this->stemSectionAngle[1])) +
                             (headDistVertical -
                              20.0f * (-Math_SinS(this->stemSectionAngle[0]) - Math_SinS(this->stemSectionAngle[1]))) *
                                 Math_CosS(this->stemSectionAngle[2]) / -Math_SinS(this->stemSectionAngle[2]);
    } else {
        Math_ScaledStepToS(&this->stemSectionAngle[0], -0x5555, 0x555);
        Math_ScaledStepToS(&this->stemSectionAngle[1], -0x5555, 0x333);
        Math_ScaledStepToS(&this->stemSectionAngle[2], -0x4000, 0x333);

        headDistHorizontal = 20.0f * (Math_CosS(this->stemSectionAngle[0]) + Math_CosS(this->stemSectionAngle[1])) +
                             (headDistVertical -
                              20.0f * (-Math_SinS(this->stemSectionAngle[0]) - Math_SinS(this->stemSectionAngle[1]))) *
                                 Math_CosS(this->stemSectionAngle[2]) / -Math_SinS(this->stemSectionAngle[2]);
    }

    this->actor.world.pos.y = this->actor.home.pos.y + (headDistVertical * this->size);
    xShift = headDistHorizontal * this->size * Math_SinS(this->actor.shape.rot.y);
    zShift = headDistHorizontal * this->size * Math_CosS(this->actor.shape.rot.y);
    this->actor.world.pos.x = this->actor.home.pos.x + xShift;
    this->actor.world.pos.z = this->actor.home.pos.z + zShift;

    EffectSsHahen_SpawnBurst(play, &this->actor.home.pos, this->size * 3.0f, 0, this->size * 12.0f,
                             this->size * 5.0f, 1, HAHEN_OBJECT_DEFAULT, 0xA, NULL);

    if (this->timer == 0) {
        EnDekubaba_SetupWait(this);
    }
}

void EnDekubaba_UpdateHeadPosition(EnDekubaba* this) {
    f32 horizontalHeadShift = (Math_CosS(this->stemSectionAngle[0]) + Math_CosS(this->stemSectionAngle[1]) +
                               Math_CosS(this->stemSectionAngle[2])) *
                              20.0f;

    this->actor.world.pos.x =
        this->actor.home.pos.x + Math_SinS(this->actor.shape.rot.y) * (horizontalHeadShift * this->size);
    this->actor.world.pos.y =
        this->actor.home.pos.y - (Math_SinS(this->stemSectionAngle[0]) + Math_SinS(this->stemSectionAngle[1]) +
                                  Math_SinS(this->stemSectionAngle[2])) *
                                     20.0f * this->size;
    this->actor.world.pos.z =
        this->actor.home.pos.z + Math_CosS(this->actor.shape.rot.y) * (horizontalHeadShift * this->size);
}

void EnDekubaba_DecideLunge(EnDekubaba* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 12.0f)) {
        if (this->actor.params == DEKUBABA_BIG) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEKU_MOUTH);
        } else {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEKU_JR_MOUTH);
        }
    }

    if (this->timer != 0) {
        this->timer--;
    }

    Math_ApproachS(&this->actor.shape.rot.y, Math_Vec3f_Yaw(&this->actor.home.pos, &player->actor.world.pos), 2,
                   (this->timer % 5) * 0x222);

    if (this->timer < 10) {
        this->stemSectionAngle[0] += 0x16C;
        this->stemSectionAngle[1] += 0x16C;
        this->stemSectionAngle[2] += 0xB6;
        this->actor.shape.rot.x += 0x222;
    } else if (this->timer < 20) {
        this->stemSectionAngle[0] -= 0x16C;
        this->stemSectionAngle[1] += 0x111;
        this->actor.shape.rot.x += 0x16C;
    } else if (this->timer < 30) {
        this->stemSectionAngle[1] -= 0x111;
        this->actor.shape.rot.x -= 0xB6;
    } else {
        this->stemSectionAngle[1] -= 0xB6;
        this->stemSectionAngle[2] += 0xB6;
        this->actor.shape.rot.x -= 0x16C;
    }

    EnDekubaba_UpdateHeadPosition(this);

    if (240.0f * this->size < Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos)) {
        EnDekubaba_SetupRetract(this);
    } else if ((this->timer == 0) || (this->actor.xzDistToPlayer < 80.0f * this->size)) {
        EnDekubaba_SetupPrepareLunge(this);
    }
}

void EnDekubaba_Lunge(EnDekubaba* this, PlayState* play) {
    static Color_RGBA8 primColor = { 105, 255, 105, 255 };
    static Color_RGBA8 envColor = { 150, 250, 150, 0 };
    s32 allStepsDone;
    s16 curFrame10;
    Vec3f velocity;

    SkelAnime_Update(&this->skelAnime);

    if (this->timer == 0) {
        if (Animation_OnFrame(&this->skelAnime, 1.0f)) {
            if (this->actor.params == DEKUBABA_BIG) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEKU_ATTACK);
            } else {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEKU_JR_ATTACK);
            }
        }

        Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x222);

        curFrame10 = this->skelAnime.curFrame * 10.0f;

        allStepsDone = true;
        allStepsDone &= Math_ScaledStepToS(&this->stemSectionAngle[0], -0xE38, curFrame10 + 0x38E);
        allStepsDone &= Math_ScaledStepToS(&this->stemSectionAngle[1], -0xE38, curFrame10 + 0x71C);
        allStepsDone &= Math_ScaledStepToS(&this->stemSectionAngle[2], -0xE38, curFrame10 + 0xE38);

        if (allStepsDone) {
            Animation_PlayLoopSetSpeed(&this->skelAnime, &gDekuBabaFastChompAnim, 4.0f);
            velocity.x = Math_SinS(this->actor.shape.rot.y) * 5.0f;
            velocity.y = 0.0f;
            velocity.z = Math_CosS(this->actor.shape.rot.y) * 5.0f;

            func_8002829C(play, &this->actor.world.pos, &velocity, &sZeroVec, &primColor, &envColor, 1,
                          this->size * 100.0f);
            this->timer = 1;
            this->collider.base.acFlags |= AC_ON;
        }
    } else if (this->timer > 10) {
        EnDekubaba_SetupPullBack(this);
    } else {
        this->timer++;

        if ((this->timer >= 4) && !Actor_IsFacingPlayer(&this->actor, 0x16C)) {
            Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xF, 0x71C);
        }

        if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 12.0f)) {
            if (this->actor.params == DEKUBABA_BIG) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEKU_MOUTH);
            } else {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEKU_JR_MOUTH);
            }
        }
    }

    EnDekubaba_UpdateHeadPosition(this);
}

void EnDekubaba_PrepareLunge(EnDekubaba* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->timer != 0) {
        this->timer--;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.x, 0x1800, 2, 0xE38, 0x71C);
    Math_ApproachS(&this->actor.shape.rot.y, Math_Vec3f_Yaw(&this->actor.home.pos, &player->actor.world.pos), 2, 0xE38);
    Math_ScaledStepToS(&this->stemSectionAngle[0], 0xAAA, 0x444);
    Math_ScaledStepToS(&this->stemSectionAngle[1], -0x4718, 0x888);
    Math_ScaledStepToS(&this->stemSectionAngle[2], -0x6AA4, 0x888);

    if (this->timer == 0) {
        EnDekubaba_SetupLunge(this);
    }

    EnDekubaba_UpdateHeadPosition(this);
}

void EnDekubaba_PullBack(EnDekubaba* this, PlayState* play) {
    Vec3f dustPos;
    f32 xIncr;
    f32 zIncr;
    s32 i;

    SkelAnime_Update(&this->skelAnime);

    if (this->timer == 0) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, -0x93E, 0x38E);
        Math_ScaledStepToS(&this->stemSectionAngle[0], -0x888, 0x16C);
        Math_ScaledStepToS(&this->stemSectionAngle[1], -0x888, 0x16C);
        if (Math_ScaledStepToS(&this->stemSectionAngle[2], -0x888, 0x16C)) {
            xIncr = Math_SinS(this->actor.shape.rot.y) * 30.0f * this->size;
            zIncr = Math_CosS(this->actor.shape.rot.y) * 30.0f * this->size;
            dustPos = this->actor.home.pos;

            for (i = 0; i < 3; i++) {
                func_800286CC(play, &dustPos, &sZeroVec, &sZeroVec, this->size * 500.0f, this->size * 50.0f);
                dustPos.x += xIncr;
                dustPos.z += zIncr;
            }

            this->timer = 1;
        }
    } else if (this->timer == 11) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, -0x93E, 0x200);
        Math_ScaledStepToS(&this->stemSectionAngle[0], -0xAAA, 0x200);
        Math_ScaledStepToS(&this->stemSectionAngle[2], -0x5C71, 0x200);

        if (Math_ScaledStepToS(&this->stemSectionAngle[1], 0x238C, 0x200)) {
            this->timer = 12;
        }
    } else if (this->timer == 18) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, 0x2AA8, 0xAAA);

        if (Math_ScaledStepToS(&this->stemSectionAngle[0], 0x1554, 0x5B0)) {
            this->timer = 25;
        }

        Math_ScaledStepToS(&this->stemSectionAngle[1], -0x38E3, 0xAAA);
        Math_ScaledStepToS(&this->stemSectionAngle[2], -0x5C71, 0x2D8);
    } else if (this->timer == 25) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, -0x5550, 0xAAA);

        if (Math_ScaledStepToS(&this->stemSectionAngle[0], -0x6388, 0x93E)) {
            this->timer = 26;
        }

        Math_ScaledStepToS(&this->stemSectionAngle[1], -0x3FFC, 0x4FA);
        Math_ScaledStepToS(&this->stemSectionAngle[2], -0x238C, 0x444);
    } else if (this->timer == 26) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, 0x1800, 0x93E);

        if (Math_ScaledStepToS(&this->stemSectionAngle[0], -0x1555, 0x71C)) {
            this->timer = 27;
        }

        Math_ScaledStepToS(&this->stemSectionAngle[1], -0x38E3, 0x2D8);
        Math_ScaledStepToS(&this->stemSectionAngle[2], -0x5C71, 0x5B0);
    } else if (this->timer >= 27) {
        this->timer++;

        if (this->timer > 30) {
            if (this->actor.xzDistToPlayer < 80.0f * this->size) {
                EnDekubaba_SetupPrepareLunge(this);
            } else {
                EnDekubaba_SetupDecideLunge(this);
            }
        }
    } else {
        this->timer++;

        if (this->timer == 10) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEKU_SCRAPE);
        }

        if (this->timer >= 12) {
            Math_ScaledStepToS(&this->stemSectionAngle[2], -0x5C71, 0x88);
        }
    }

    EnDekubaba_UpdateHeadPosition(this);
}

void EnDekubaba_Recover(EnDekubaba* this, PlayState* play) {
    s32 anyStepsDone;

    SkelAnime_Update(&this->skelAnime);

    if (this->timer > 8) {
        anyStepsDone = Math_SmoothStepToS(&this->actor.shape.rot.x, 0x1800, 1, 0x11C6, 0x71C);
        anyStepsDone |= Math_SmoothStepToS(&this->stemSectionAngle[0], -0x1555, 1, 0xAAA, 0x71C);
        anyStepsDone |= Math_SmoothStepToS(&this->stemSectionAngle[1], -0x38E3, 1, 0xE38, 0x71C);
        anyStepsDone |= Math_SmoothStepToS(&this->stemSectionAngle[2], -0x5C71, 1, 0x11C6, 0x71C);

        if (!anyStepsDone) {
            this->timer = 8;
        }
    } else {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            EnDekubaba_SetupDecideLunge(this);
        }
    }

    EnDekubaba_UpdateHeadPosition(this);
}

/**
 * Hit by a weapon or hit something when lunging.
 */
void EnDekubaba_Hit(EnDekubaba* this, PlayState* play) {
    s32 allStepsDone;

    SkelAnime_Update(&this->skelAnime);

    allStepsDone = true;
    allStepsDone &= Math_ScaledStepToS(&this->actor.shape.rot.x, -0x4000, 0xE38);
    allStepsDone &= Math_ScaledStepToS(&this->stemSectionAngle[0], -0x4000, 0xE38);
    allStepsDone &= Math_ScaledStepToS(&this->stemSectionAngle[1], -0x4000, 0xE38);
    allStepsDone &= Math_ScaledStepToS(&this->stemSectionAngle[2], -0x4000, 0xE38);

    if (allStepsDone) {
        if (this->actor.colChkInfo.health == 0) {
            EnDekubaba_SetupShrinkDie(this);
        } else {
            this->collider.base.acFlags |= AC_ON;
            if (this->timer == 0) {
                if (this->actor.xzDistToPlayer < 80.0f * this->size) {
                    EnDekubaba_SetupPrepareLunge(this);
                } else {
                    EnDekubaba_SetupRecover(this);
                }
            } else {
                EnDekubaba_SetupStunnedVertical(this);
            }
        }
    }

    EnDekubaba_UpdateHeadPosition(this);
}

void EnDekubaba_StunnedVertical(EnDekubaba* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        EnDekubaba_DisableHitboxes(this);

        if (this->actor.xzDistToPlayer < 80.0f * this->size) {
            EnDekubaba_SetupPrepareLunge(this);
        } else {
            EnDekubaba_SetupRecover(this);
        }
    }
}

/**
 * Sway back and forth with decaying amplitude until close enough to vertical.
 */
void EnDekubaba_Sway(EnDekubaba* this, PlayState* play) {
    s16 angleToVertical;

    SkelAnime_Update(&this->skelAnime);
    Math_ScaledStepToS(&this->actor.shape.rot.x, this->stemSectionAngle[0], 0x71C);
    Math_ScaledStepToS(&this->stemSectionAngle[0], this->stemSectionAngle[1], 0x71C);
    Math_ScaledStepToS(&this->stemSectionAngle[1], this->stemSectionAngle[2], 0x71C);

    if (Math_ScaledStepToS(&this->stemSectionAngle[2], this->targetSwayAngle, 0x71C)) {
        this->targetSwayAngle = -0x4000 - (this->targetSwayAngle + 0x4000) * 0.8f;
    }
    angleToVertical = this->targetSwayAngle + 0x4000;

    if (ABS(angleToVertical) < 0x100) {
        this->collider.base.acFlags |= AC_ON;
        if (this->actor.xzDistToPlayer < 80.0f * this->size) {
            EnDekubaba_SetupPrepareLunge(this);
        } else {
            EnDekubaba_SetupRecover(this);
        }
    }

    EnDekubaba_UpdateHeadPosition(this);
}

void EnDekubaba_PrunedSomersault(EnDekubaba* this, PlayState* play) {
    s32 i;
    Vec3f dustPos;
    f32 deltaX;
    f32 deltaZ;
    f32 deltaY;

    Math_StepToF(&this->actor.speedXZ, 0.0f, this->size * 0.1f);

    if (this->timer == 0) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, 0x4800, 0x71C);
        Math_ScaledStepToS(&this->stemSectionAngle[0], 0x4800, 0x71C);
        Math_ScaledStepToS(&this->stemSectionAngle[1], 0x4800, 0x71C);

        EffectSsHahen_SpawnBurst(play, &this->actor.world.pos, this->size * 3.0f, 0, this->size * 12.0f,
                                 this->size * 5.0f, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);

        if ((this->actor.scale.x > 0.005f) && ((this->actor.bgCheckFlags & 2) || (this->actor.bgCheckFlags & 8))) {
            this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = 0.0f;
            this->actor.speedXZ = 0.0f;
            this->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_2);
            EffectSsHahen_SpawnBurst(play, &this->actor.world.pos, this->size * 3.0f, 0, this->size * 12.0f,
                                     this->size * 5.0f, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        }

        if (this->actor.bgCheckFlags & 2) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
            this->timer = 1;
        }
    } else if (this->timer == 1) {
        dustPos = this->actor.world.pos;

        deltaY = 20.0f * Math_SinS(this->actor.shape.rot.x);
        deltaX = -20.0f * Math_CosS(this->actor.shape.rot.x) * Math_SinS(this->actor.shape.rot.y);
        deltaZ = -20.0f * Math_CosS(this->actor.shape.rot.x) * Math_CosS(this->actor.shape.rot.y);

        for (i = 0; i < 4; i++) {
            func_800286CC(play, &dustPos, &sZeroVec, &sZeroVec, 500, 50);
            dustPos.x += deltaX;
            dustPos.y += deltaY;
            dustPos.z += deltaZ;
        }

        func_800286CC(play, &this->actor.home.pos, &sZeroVec, &sZeroVec, this->size * 500.0f, this->size * 100.0f);
        EnDekubaba_SetupDeadStickDrop(this, play);
    }
}

/**
 * Die and drop Deku Nuts (Stick drop is handled elsewhere)
 */
void EnDekubaba_ShrinkDie(EnDekubaba* this, PlayState* play) {
    Math_StepToF(&this->actor.world.pos.y, this->actor.home.pos.y, this->size * 5.0f);

    if (Math_StepToF(&this->actor.scale.x, this->size * 0.1f * 0.01f, this->size * 0.1f * 0.01f)) {
        func_800286CC(play, &this->actor.home.pos, &sZeroVec, &sZeroVec, this->size * 500.0f, this->size * 100.0f);
        if (this->actor.dropFlag == 0) {
            Item_DropCollectible(play, &this->actor.world.pos, ITEM00_NUTS);

            if (this->actor.params == DEKUBABA_BIG) {
                Item_DropCollectible(play, &this->actor.world.pos, ITEM00_NUTS);
                Item_DropCollectible(play, &this->actor.world.pos, ITEM00_NUTS);
            }
        } else {
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x30);
        }
        Actor_Kill(&this->actor);
    }

    this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;
    this->actor.shape.rot.z += 0x1C70;
    EffectSsHahen_SpawnBurst(play, &this->actor.home.pos, this->size * 3.0f, 0, this->size * 12.0f,
                             this->size * 5.0f, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);
}

void EnDekubaba_DeadStickDrop(EnDekubaba* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (Actor_HasParent(&this->actor, play) || (this->timer == 0)) {
        Actor_Kill(&this->actor);
        return;
    }

    func_8002F554(&this->actor, play, GI_STICKS_1);
}

// Update and associated functions

void EnDekubaba_UpdateDamage(EnDekubaba* this, PlayState* play) {
    Vec3f* firePos;
    f32 fireScale;
    s32 phi_s0; // Used for both health and iterator

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlagJntSph(&this->actor, &this->collider, 1);

        if ((this->collider.base.colType != COLTYPE_HARD) &&
            ((this->actor.colChkInfo.damageEffect != DEKUBABA_DMGEFF_NONE) || (this->actor.colChkInfo.damage != 0))) {

            phi_s0 = this->actor.colChkInfo.health - this->actor.colChkInfo.damage;

            if (this->actionFunc != EnDekubaba_StunnedVertical) {
                if ((this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_BOOMERANG) ||
                    (this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_DEKUNUT)) {
                    if (this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_BOOMERANG) {
                        phi_s0 = this->actor.colChkInfo.health;
                    }

                    EnDekubaba_SetupHit(this, 2);
                } else if (this->actionFunc == EnDekubaba_PullBack) {
                    if (phi_s0 <= 0) {
                        phi_s0 = 1;
                    }

                    EnDekubaba_SetupHit(this, 1);
                } else {
                    EnDekubaba_SetupHit(this, 0);
                }
            } else if ((this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_BOOMERANG) ||
                       (this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_SWORD)) {
                if (phi_s0 > 0) {
                    EnDekubaba_SetupSway(this);
                } else {
                    EnDekubaba_SetupPrunedSomersault(this);
                }
            } else if (this->actor.colChkInfo.damageEffect != DEKUBABA_DMGEFF_DEKUNUT) {
                EnDekubaba_SetupHit(this, 0);
            } else {
                return;
            }

            this->actor.colChkInfo.health = CLAMP_MIN(phi_s0, 0);

            if (this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_FIRE) {
                firePos = &this->actor.world.pos;
                fireScale = (this->size * 70.0f);

                for (phi_s0 = 0; phi_s0 < 4; phi_s0++) {
                    EffectSsEnFire_SpawnVec3f(play, &this->actor, firePos, fireScale, 0, 0, phi_s0);
                }
            }
        } else {
            return;
        }
    } else if ((play->actorCtx.unk_02 != 0) && (this->collider.base.colType != COLTYPE_HARD) &&
               (this->actionFunc != EnDekubaba_StunnedVertical) && (this->actionFunc != EnDekubaba_Hit) &&
               (this->actor.colChkInfo.health != 0)) {
        this->actor.colChkInfo.health--;
        this->actor.dropFlag = 0x00;
        EnDekubaba_SetupHit(this, 1);
    } else {
        return;
    }

    if (this->actor.colChkInfo.health != 0) {
        if (this->timer == 2) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
        } else {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEKU_DAMAGE);
        }
    } else {
        Enemy_StartFinishingBlow(play, &this->actor);
        if (this->actor.params == DEKUBABA_BIG) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEKU_DEAD);
        } else {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DEKU_JR_DEAD);
        }
    }
}

void EnDekubaba_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDekubaba* this = (EnDekubaba*)thisx;

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        EnDekubaba_SetupRecover(this);
    }

    EnDekubaba_UpdateDamage(this, play);
    this->actionFunc(this, play);

    if (this->actionFunc == EnDekubaba_PrunedSomersault) {
        Actor_MoveForward(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, this->size * 15.0f, 10.0f, 5);
    } else if (this->actionFunc != EnDekubaba_DeadStickDrop) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
        if (this->boundFloor == NULL) {
            this->boundFloor = this->actor.floorPoly;
        }
    }
    if (this->actionFunc == EnDekubaba_Lunge) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        this->actor.flags |= ACTOR_FLAG_24;
    }

    if (this->collider.base.acFlags & AC_ON) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->actionFunc != EnDekubaba_DeadStickDrop) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

// Draw functions

void EnDekubaba_DrawStemRetracted(EnDekubaba* this, PlayState* play) {
    f32 horizontalScale;

    OPEN_DISPS(play->state.gfxCtx);

    horizontalScale = this->size * 0.01f;

    Matrix_Translate(this->actor.home.pos.x, this->actor.home.pos.y + (-6.0f * this->size), this->actor.home.pos.z,
                     MTXMODE_NEW);
    Matrix_RotateZYX(this->stemSectionAngle[0], this->actor.shape.rot.y, 0, MTXMODE_APPLY);
    Matrix_Scale(horizontalScale, horizontalScale, horizontalScale, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStemTopDL);

    Actor_SetFocus(&this->actor, 0.0f);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnDekubaba_DrawStemExtended(EnDekubaba* this, PlayState* play) {
    static Gfx* stemDLists[] = { gDekuBabaStemTopDL, gDekuBabaStemMiddleDL, gDekuBabaStemBaseDL };
    MtxF mtx;
    s32 i;
    f32 horizontalStepSize;
    f32 spA4;
    f32 scale;
    s32 stemSections;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->actionFunc == EnDekubaba_PrunedSomersault) {
        stemSections = 2;
    } else {
        stemSections = 3;
    }

    scale = this->size * 0.01f;
    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    Matrix_Get(&mtx);
    if (this->actor.colorFilterTimer != 0) {
        spA4 = this->size * 20.0f;
        this->bodyPartsPos[2].x = this->actor.world.pos.x;
        this->bodyPartsPos[2].y = this->actor.world.pos.y - spA4;
        this->bodyPartsPos[2].z = this->actor.world.pos.z;
    }

    for (i = 0; i < stemSections; i++) {
        mtx.yw += 20.0f * Math_SinS(this->stemSectionAngle[i]) * this->size;
        horizontalStepSize = 20.0f * Math_CosS(this->stemSectionAngle[i]) * this->size;
        mtx.xw -= horizontalStepSize * Math_SinS(this->actor.shape.rot.y);
        mtx.zw -= horizontalStepSize * Math_CosS(this->actor.shape.rot.y);

        Matrix_Put(&mtx);
        Matrix_RotateZYX(this->stemSectionAngle[i], this->actor.shape.rot.y, 0, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPDisplayList(POLY_OPA_DISP++, stemDLists[i]);

        Collider_UpdateSpheres(51 + 2 * i, &this->collider);
        Collider_UpdateSpheres(52 + 2 * i, &this->collider);

        if (i == 0) {
            if (this->actionFunc != EnDekubaba_Sway) {
                this->actor.focus.pos.x = mtx.xw;
                this->actor.focus.pos.y = mtx.yw;
                this->actor.focus.pos.z = mtx.zw;
            } else {
                this->actor.focus.pos.x = this->actor.home.pos.x;
                this->actor.focus.pos.y = this->actor.home.pos.y + (40.0f * this->size);
                this->actor.focus.pos.z = this->actor.home.pos.z;
            }
        }

        if ((i < 2) && (this->actor.colorFilterTimer != 0)) {
            // checking colorFilterTimer ensures that spA4 has been initialized earlier, so not a bug
            this->bodyPartsPos[i].x = mtx.xw;
            this->bodyPartsPos[i].y = mtx.yw - spA4;
            this->bodyPartsPos[i].z = mtx.zw;
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnDekubaba_DrawStemBasePruned(EnDekubaba* this, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Matrix_RotateZYX(this->stemSectionAngle[2], this->actor.shape.rot.y, 0, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStemBaseDL);

    Collider_UpdateSpheres(55, &this->collider);
    Collider_UpdateSpheres(56, &this->collider);
    CLOSE_DISPS(play->state.gfxCtx);
}

void EnDekubaba_DrawBaseShadow(EnDekubaba* this, PlayState* play) {
    MtxF mtx;
    f32 horizontalScale;

    OPEN_DISPS(play->state.gfxCtx);
    func_80094044(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 255);

    func_80038A28(this->boundFloor, this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z, &mtx);
    Matrix_Mult(&mtx, MTXMODE_NEW);

    horizontalScale = this->size * 0.15f;
    Matrix_Scale(horizontalScale, 1.0f, horizontalScale, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gCircleShadowDL);
    CLOSE_DISPS(play->state.gfxCtx);
}

void EnDekubaba_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnDekubaba* this = (EnDekubaba*)thisx;

    if (limbIndex == 1) {
        Collider_UpdateSpheres(limbIndex, &this->collider);
    }
}

void EnDekubaba_Draw(Actor* thisx, PlayState* play) {
    EnDekubaba* this = (EnDekubaba*)thisx;
    f32 scale;

    OPEN_DISPS(play->state.gfxCtx);
    func_80093D18(play->state.gfxCtx);

    if (this->actionFunc != EnDekubaba_DeadStickDrop) {
        SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL,
                          EnDekubaba_PostLimbDraw, this);

        if (this->actionFunc == EnDekubaba_Wait) {
            EnDekubaba_DrawStemRetracted(this, play);
        } else {
            EnDekubaba_DrawStemExtended(this, play);
        }

        scale = this->size * 0.01f;
        Matrix_Translate(this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z, MTXMODE_NEW);
        Matrix_RotateY(this->actor.home.rot.y * (M_PI / 0x8000), MTXMODE_APPLY);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gDekuBabaBaseLeavesDL);

        if (this->actionFunc == EnDekubaba_PrunedSomersault) {
            EnDekubaba_DrawStemBasePruned(this, play);
        }

        if (this->boundFloor != NULL) {
            EnDekubaba_DrawBaseShadow(this, play);
        }

        // Display solid until 40 frames left, then blink until killed.
    } else if ((this->timer > 40) || ((this->timer % 2) != 0)) {
        Matrix_Translate(0.0f, 0.0f, 200.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStickDropDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

//OTRTODO fix this one
void EnDekuBaba_Reset(void) {
    // DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD)
    
}