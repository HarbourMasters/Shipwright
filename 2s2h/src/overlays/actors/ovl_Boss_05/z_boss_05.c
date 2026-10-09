/*
 * File: z_boss_05.c
 * Overlay: ovl_Boss_05
 * Description: Bio Deku Baba and its lily pad
 *
 * This actor has five main types which can be further split into many different subtypes:
 * - LilyPadWithHead: This variant of the actor consists of a Deku Baba head dangling from a lily pad. It can attack the
 *   player if they get close, and if the player attempts to stand on the lily pad, it will eventually flip the lily pad
 *   over and attack. The Deku Baba head can be detached by attacking the stem that connects it to the lily pad, or it
 *   can be killed by attacking the head directly. This type has a subtype which hides the lily pad leaf; this is used
 *   exclusively by Bio Deku Babas that hang from the ceiling.
 *
 * - LilyPad: If the head attached to a lily pad is detached or killed, then this harmless variant of the actor is left
 *   behind. This is a simple lily pad that the player can stand on. It sinks slightly in the water when the player
 *   stands on it, but it otherwise doesn't do much.
 *
 * - FallingHead: If the head attached to a lily pad with detached by hitting its connected stem, then this variant of
 *   the actor is spawned. It falls until it hits the ground, at which point it will grow legs and eyestalks and
 *   transform into a walking head.
 *
 * - WalkingHead: This is the "transformed" version of the Bio Deku Baba that spawns when the head is detached from a
 *   lily pad and touches the ground. It walks around on three legs and can attack the player by charging at them and
 *   biting them.
 *
 * - Fragment: When a Bio Deku Baba head is defeated (whether it's still attached to a lily pad or it's walking around
 *   freely), it will spawn multiple variants of the actor. This type can be divided into many different subtypes, one
 *   for each limb on the head.
 */

#include "z_boss_05.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

// This actor has an array of timers in its instance, but for the most part, it only uses the first entry
#define TIMER_CURRENT_ACTION 0
#define TIMER_FALLING_HEAD_FALL 2

void Boss05_Init(Actor* thisx, PlayState* play);
void Boss05_Destroy(Actor* thisx, PlayState* play);
void Boss05_Update(Actor* thisx, PlayState* play);
void Boss05_Draw(Actor* thisx, PlayState* play);

void Boss05_LilyPadWithHead_SetupMove(Boss05* this, PlayState* play);
void Boss05_LilyPadWithHead_Move(Boss05* this, PlayState* play);
void Boss05_LilyPad_Idle(Boss05* this, PlayState* play);
void Boss05_FallingHead_Fall(Boss05* this, PlayState* play);
void Boss05_WalkingHead_SetupTransform(Boss05* this, PlayState* play);
void Boss05_WalkingHead_Transform(Boss05* this, PlayState* play);
void Boss05_WalkingHead_SetupIdle(Boss05* this, PlayState* play);
void Boss05_WalkingHead_Idle(Boss05* this, PlayState* play);
void Boss05_WalkingHead_SetupWalk(Boss05* this, PlayState* play);
void Boss05_WalkingHead_Walk(Boss05* this, PlayState* play);
void Boss05_WalkingHead_SetupSpottedPlayer(Boss05* this, PlayState* play);
void Boss05_WalkingHead_SpottedPlayer(Boss05* this, PlayState* play);
void Boss05_WalkingHead_SetupCharge(Boss05* this, PlayState* arg1);
void Boss05_WalkingHead_Charge(Boss05* this, PlayState* play);
void Boss05_WalkingHead_SetupAttack(Boss05* this, PlayState* arg1);
void Boss05_WalkingHead_Attack(Boss05* this, PlayState* play);
void Boss05_WalkingHead_SetupDamaged(Boss05* this, PlayState* play);
void Boss05_WalkingHead_Damaged(Boss05* this, PlayState* play);
void Boss05_WalkingHead_SetupStunned(Boss05* this, PlayState* play);
void Boss05_WalkingHead_SetupFreeze(Boss05* this, PlayState* play);
void Boss05_WalkingHead_Stunned(Boss05* this, PlayState* play);
void Boss05_Fragment_Move(Boss05* this, PlayState* play);

typedef enum BioDekuBabaDrawDmgEffState {
    /*  0 */ BIO_BABA_DRAW_DMGEFF_STATE_NONE,
    /*  1 */ BIO_BABA_DRAW_DMGEFF_STATE_FIRE_INIT,
    /*  2 */ BIO_BABA_DRAW_DMGEFF_STATE_FIRE_ACTIVE,
    /* 10 */ BIO_BABA_DRAW_DMGEFF_STATE_FROZEN_INIT = 10,
    /* 11 */ BIO_BABA_DRAW_DMGEFF_STATE_FROZEN_ACTIVE,
    /* 20 */ BIO_BABA_DRAW_DMGEFF_STATE_LIGHT_ORB_INIT = 20,
    /* 21 */ BIO_BABA_DRAW_DMGEFF_STATE_LIGHT_ORB_ACTIVE
} BioDekuBabaDrawDmgEffState;

typedef enum BioDekuBabaFragmentState {
    /* 0 */ BIO_BABA_FRAGMENT_STATE_SPAWNED,
    /* 1 */ BIO_BABA_FRAGMENT_STATE_UNDERWATER,
    /* 2 */ BIO_BABA_FRAGMENT_STATE_ABOVE_WATER,
    /* 3 */ BIO_BABA_FRAGMENT_STATE_FLYING_THROUGH_AIR
} BioDekuBabaFragmentState;

typedef enum BioDekuBabaHeadHitReaction {
    /*  0 */ BIO_BABA_HEAD_HIT_REACTION_NONE_OR_DAMAGED,
    /*  2 */ BIO_BABA_HEAD_HIT_REACTION_DEAD = 2,
    /* 10 */ BIO_BABA_HEAD_HIT_REACTION_DEATCH = 10
} BioDekuBabaHeadHitReaction;

typedef enum BioDekuBabaLilyPadWithHeadAttackState {
    /*  0 */ BIO_BABA_LILY_PAD_WITH_HEAD_ATTACK_STATE_IDLE,
    /*  1 */ BIO_BABA_LILY_PAD_WITH_HEAD_ATTACK_STATE_WIND_UP,
    /*  2 */ BIO_BABA_LILY_PAD_WITH_HEAD_ATTACK_STATE_LUNGE_ATTACK,
    /* 10 */ BIO_BABA_LILY_PAD_WITH_HEAD_ATTACK_STATE_FLIP_ATTACK = 10
} BioDekuBabaLilyPadWithHeadAttackState;

typedef enum BioDekuBabaLilyPadWithHeadMovementState {
    /* 0 */ BIO_BABA_LILY_PAD_WITH_HEAD_MOVEMENT_STATE_IDLE,
    /* 1 */ BIO_BABA_LILY_PAD_WITH_HEAD_MOVEMENT_STATE_WIND_UP,
    /* 2 */ BIO_BABA_LILY_PAD_WITH_HEAD_MOVEMENT_STATE_LUNGE_ATTACK,
    /* 3 */ BIO_BABA_LILY_PAD_WITH_HEAD_MOVEMENT_STATE_FLIP_ATTACK
} BioDekuBabaLilyPadWithHeadMovementState;

#include "assets/overlays/ovl_Boss_05/ovl_Boss_05.h"

// The limbs referenced here are not used. The spheres are positioned manually by Boss05_LilyPad_PostLimbDraw.
static ColliderJntSphElementInit sLilyPadJntSphElementsInit[] = {
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON,
            OCELEM_ON,
        },
        { BIO_DEKU_BABA_LILY_PAD_LIMB_NONE, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON,
            OCELEM_ON,
        },
        { BIO_DEKU_BABA_LILY_PAD_LIMB_NONE, { { 0, 0, 0 }, 15 }, 100 },
    },
};

static ColliderJntSphInit sLilyPadJntSphInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sLilyPadJntSphElementsInit),
    sLilyPadJntSphElementsInit,
};

// The limb referenced here is not used. The sphere is positioned manually by Boss05_Head_PostLimbDraw.
static ColliderJntSphElementInit sHeadJntSphElementsInit[] = {
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON,
            OCELEM_ON,
        },
        { BIO_DEKU_BABA_HEAD_LIMB_NONE, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sHeadJntSphInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sHeadJntSphElementsInit),
    sHeadJntSphElementsInit,
};

// The limb referenced here is not used. The sphere is positioned manually by Boss05_Head_PostLimbDraw.
static ColliderJntSphElementInit sWalkingHeadJntSphElementsInit[] = {
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7FFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON,
            OCELEM_ON,
        },
        { BIO_DEKU_BABA_HEAD_LIMB_NONE, { { 0, 0, 0 }, 15 }, 100 },
    },
};

static ColliderJntSphInit sWalkingHeadJntSphInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sWalkingHeadJntSphElementsInit),
    sWalkingHeadJntSphElementsInit,
};

/**
 * Spawns eight ice shards on the Bio Deku Baba's head that fly off in random directions.
 */
void Boss05_WalkingHead_Thaw(Boss05* this, PlayState* play) {
    static Color_RGBA8 sIcePrimColor = { 170, 255, 255, 255 };
    static Color_RGBA8 sIceEnvColor = { 200, 200, 255, 255 };
    static Vec3f sIceAccel = { 0.0f, -1.0f, 0.0f };
    Vec3f pos;
    Vec3f velocity;
    s32 i;

    SoundSource_PlaySfxAtFixedWorldPos(play, &this->bodyPartsPos[BIO_BABA_BODYPART_HEAD], 30, NA_SE_EV_ICE_BROKEN);
    for (i = 0; i < 8; i++) {
        velocity.x = Rand_CenteredFloat(7.0f);
        velocity.z = Rand_CenteredFloat(7.0f);
        velocity.y = Rand_ZeroFloat(6.0f) + 4.0f;
        pos.x = this->bodyPartsPos[BIO_BABA_BODYPART_HEAD].x + velocity.x;
        pos.y = this->bodyPartsPos[BIO_BABA_BODYPART_HEAD].y + velocity.y;
        pos.z = this->bodyPartsPos[BIO_BABA_BODYPART_HEAD].z + velocity.z;
        EffectSsEnIce_Spawn(play, &pos, Rand_ZeroFloat(0.5f) + 0.7f, &velocity, &sIceAccel, &sIcePrimColor,
                            &sIceEnvColor, 30);
    }
}

/**
 * Manually sets the position of a sphere collider to a specific position.
 */
void Boss05_SetColliderSphere(s32 index, ColliderJntSph* collider, Vec3f* sphereCenter) {
    collider->elements[index].dim.worldSphere.center.x = sphereCenter->x;
    collider->elements[index].dim.worldSphere.center.y = sphereCenter->y;
    collider->elements[index].dim.worldSphere.center.z = sphereCenter->z;
    collider->elements[index].dim.worldSphere.radius =
        collider->elements[index].dim.modelSphere.radius * collider->elements[index].dim.scale;
}

typedef enum BioDekuBabaDamageEffect {
    // Named based on the fact that everything with this damage effect deals zero damage. If this effect is given to an
    // attack that deals non-zero damage, it will behave exactly like BIO_BABA_DMGEFF_DAMAGE.
    /* 0x0 */ BIO_BABA_DMGEFF_IMMUNE,

    // Deals no damage, but turns the Bio Deku Baba blue, stops all animations, and makes it wait for 40 frames.
    /* 0x1 */ BIO_BABA_DMGEFF_STUN,

    // Deals damage and surrounds the Bio Deku Baba with fire.
    /* 0x2 */ BIO_BABA_DMGEFF_FIRE,

    // Behaves exactly like BIO_BABA_DMGEFF_STUN, but also surrounds it with ice and lasts for 80 frames.
    /* 0x3 */ BIO_BABA_DMGEFF_FREEZE,

    // Deals damage and surrounds the Bio Deku Baba with yellow light orbs.
    /* 0x4 */ BIO_BABA_DMGEFF_LIGHT_ORB,

    // Behaves exactly like BIO_BABA_DMGEFF_DAMAGE, so its purpose is unknown. Only used for the lily pad with head
    // variant of the Bio Deku Baba.
    /* 0xE */ BIO_BABA_DMGEFF_SWORD = 0xE,

    // Deals damage and has no special effect.
    /* 0xF */ BIO_BABA_DMGEFF_DAMAGE
} BioDekuBabaDamageEffect;

static DamageTable sLilyPadWithHeadDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* Deku Stick     */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Horse trample  */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* Explosives     */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Zora boomerang */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Normal arrow   */ DMG_ENTRY(3, BIO_BABA_DMGEFF_DAMAGE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* Hookshot       */ DMG_ENTRY(3, BIO_BABA_DMGEFF_IMMUNE),
    /* Goron punch    */ DMG_ENTRY(2, BIO_BABA_DMGEFF_DAMAGE),
    /* Sword          */ DMG_ENTRY(1, BIO_BABA_DMGEFF_SWORD),
    /* Goron pound    */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Fire arrow     */ DMG_ENTRY(1, BIO_BABA_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(1, BIO_BABA_DMGEFF_FREEZE),
    /* Light arrow    */ DMG_ENTRY(3, BIO_BABA_DMGEFF_LIGHT_ORB),
    /* Goron spikes   */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Deku spin      */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Deku bubble    */ DMG_ENTRY(3, BIO_BABA_DMGEFF_DAMAGE),
    /* Deku launch    */ DMG_ENTRY(2, BIO_BABA_DMGEFF_DAMAGE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* Zora barrier   */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Normal shield  */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* Light ray      */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* Thrown object  */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Zora punch     */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Spin attack    */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Sword beam     */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* Normal Roll    */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* Unblockable    */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* Powder Keg     */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
};

static DamageTable sWalkingHeadDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, BIO_BABA_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(3, BIO_BABA_DMGEFF_DAMAGE),
    /* Horse trample  */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* Explosives     */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Zora boomerang */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Normal arrow   */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* Hookshot       */ DMG_ENTRY(0, BIO_BABA_DMGEFF_STUN),
    /* Goron punch    */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Sword          */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Goron pound    */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Fire arrow     */ DMG_ENTRY(2, BIO_BABA_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(2, BIO_BABA_DMGEFF_FREEZE),
    /* Light arrow    */ DMG_ENTRY(2, BIO_BABA_DMGEFF_LIGHT_ORB),
    /* Goron spikes   */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Deku spin      */ DMG_ENTRY(0, BIO_BABA_DMGEFF_STUN),
    /* Deku bubble    */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Deku launch    */ DMG_ENTRY(2, BIO_BABA_DMGEFF_DAMAGE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, BIO_BABA_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Normal shield  */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* Light ray      */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* Thrown object  */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Zora punch     */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Spin attack    */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
    /* Sword beam     */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* Normal Roll    */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(1, BIO_BABA_DMGEFF_FIRE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* Unblockable    */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, BIO_BABA_DMGEFF_IMMUNE),
    /* Powder Keg     */ DMG_ENTRY(1, BIO_BABA_DMGEFF_DAMAGE),
};

ActorProfile Boss_05_Profile = {
    /**/ ACTOR_BOSS_05,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_BOSS05,
    /**/ sizeof(Boss05),
    /**/ Boss05_Init,
    /**/ Boss05_Destroy,
    /**/ Boss05_Update,
    /**/ Boss05_Draw,
};

void Boss05_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    Boss05* this = (Boss05*)thisx;
    CollisionHeader* colHeader = NULL;

    this->dyna.actor.attentionRangeType = ATTENTION_RANGE_3;
    this->dyna.actor.colChkInfo.mass = MASS_HEAVY;
    this->dyna.actor.colChkInfo.health = 2;
    this->frameCounter = Rand_ZeroFloat(1000.0f);
    this->lowerJawScaleXZ = 1.0f;
    this->dyna.actor.gravity = -0.3f;

    Actor_SetScale(&this->dyna.actor, 0.01f);

    if ((BIO_BABA_GET_TYPE(&this->dyna.actor) == BIO_BABA_TYPE_LILY_PAD_WITH_HEAD) ||
        (BIO_BABA_GET_TYPE(&this->dyna.actor) == BIO_BABA_TYPE_NO_LEAF_LILY_PAD_WITH_HEAD)) {
        if (BIO_BABA_FORCE_DETACH_TIMER(&this->dyna.actor) == 0) {
            // This assignment is redundant; the timer already needs to be 0 to enter this block.
            BIO_BABA_FORCE_DETACH_TIMER(&this->dyna.actor) = 0;
        }

        this->dyna.actor.shape.rot.z = 0;
        this->forceDetachTimer = BIO_BABA_FORCE_DETACH_TIMER(&this->dyna.actor);
        // BIO_BABA_FORCE_DETACH_TIMER uses world.rot.z, so once we've stored the value, reset the rotation to 0 here.
        this->dyna.actor.world.rot.z = this->dyna.actor.shape.rot.z;
        this->dyna.actor.colChkInfo.damageTable = &sLilyPadWithHeadDamageTable;

        DynaPolyActor_Init(&this->dyna, 0);
        CollisionHeader_GetVirtual(&sBioBabaLilypadCol, &colHeader);
        this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
        Boss05_LilyPadWithHead_SetupMove(this, play);

        SkelAnime_InitFlex(play, &this->lilyPadSkelAnime, &gBioDekuBabaLilyPadSkel, &gBioDekuBabaLilyPadIdleAnim,
                           this->lilyPadJointTable, this->lilyPadMorphTable, BIO_DEKU_BABA_LILY_PAD_LIMB_MAX);
        SkelAnime_InitFlex(play, &this->headSkelAnime, &gBioDekuBabaHeadSkel, &gBioDekuBabaHeadChompAnim,
                           this->headJointTable, this->headMorphTable, BIO_DEKU_BABA_HEAD_LIMB_MAX);
        this->animEndFrame = Animation_GetLastFrame(&gBioDekuBabaHeadChompAnim);

        Collider_InitAndSetJntSph(play, &this->lilyPadCollider, &this->dyna.actor, &sLilyPadJntSphInit,
                                  this->lilyPadColliderElements);
        Collider_InitAndSetJntSph(play, &this->headCollider, &this->dyna.actor, &sHeadJntSphInit,
                                  this->headColliderElements);

        if (Flags_GetClear(play, play->roomCtx.curRoom.num)) {
            this->dyna.actor.params = BIO_BABA_TYPE_LILY_PAD;
            this->actionFunc = Boss05_LilyPad_Idle;
            this->dyna.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            Actor_ChangeCategory(play, &play->actorCtx, &this->dyna.actor, ACTORCAT_BG);
        }
    } else if (BIO_BABA_GET_TYPE(&this->dyna.actor) == BIO_BABA_TYPE_LILY_PAD) {
        this->actionFunc = Boss05_LilyPad_Idle;

        CollisionHeader_GetVirtual(&sBioBabaLilypadCol, &colHeader);
        this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

        SkelAnime_InitFlex(play, &this->lilyPadSkelAnime, &gBioDekuBabaLilyPadSkel, &gBioDekuBabaLilyPadIdleAnim,
                           this->lilyPadJointTable, this->lilyPadMorphTable, BIO_DEKU_BABA_LILY_PAD_LIMB_MAX);
        this->dyna.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        Actor_ChangeCategory(play, &play->actorCtx, &this->dyna.actor, ACTORCAT_BG);
    } else if (BIO_BABA_GET_TYPE(&this->dyna.actor) == BIO_BABA_TYPE_FALLING_HEAD) {
        this->actionFunc = Boss05_FallingHead_Fall;
        this->fallingHeadLilyPadLimbScale = 1.0f;

        SkelAnime_InitFlex(play, &this->lilyPadSkelAnime, &gBioDekuBabaLilyPadSkel, &gBioDekuBabaLilyPadIdleAnim,
                           this->lilyPadJointTable, this->lilyPadMorphTable, BIO_DEKU_BABA_LILY_PAD_LIMB_MAX);
        SkelAnime_InitFlex(play, &this->headSkelAnime, &gBioDekuBabaHeadSkel, &gBioDekuBabaHeadChompAnim,
                           this->headJointTable, this->headMorphTable, BIO_DEKU_BABA_HEAD_LIMB_MAX);
        this->animEndFrame = Animation_GetLastFrame(&gBioDekuBabaHeadChompAnim);

        Collider_InitAndSetJntSph(play, &this->lilyPadCollider, &this->dyna.actor, &sLilyPadJntSphInit,
                                  this->lilyPadColliderElements);
        Collider_InitAndSetJntSph(play, &this->headCollider, &this->dyna.actor, &sHeadJntSphInit,
                                  this->headColliderElements);

        ActorShape_Init(&this->dyna.actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
        this->dyna.actor.colChkInfo.damageTable = &sLilyPadWithHeadDamageTable;
    } else if (BIO_BABA_GET_TYPE(&this->dyna.actor) == BIO_BABA_TYPE_WALKING_HEAD) {
        Boss05_WalkingHead_SetupTransform(this, play);
        this->dyna.actor.colChkInfo.mass = 90;

        SkelAnime_InitFlex(play, &this->headSkelAnime, &gBioDekuBabaHeadSkel, &gBioDekuBabaHeadChompAnim,
                           this->headJointTable, this->headMorphTable, BIO_DEKU_BABA_HEAD_LIMB_MAX);

        Collider_InitAndSetJntSph(play, &this->headCollider, &this->dyna.actor, &sWalkingHeadJntSphInit,
                                  this->headColliderElements);

        ActorShape_Init(&this->dyna.actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
        this->dyna.actor.colChkInfo.damageTable = &sWalkingHeadDamageTable;
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED;
    } else if (BIO_BABA_GET_TYPE(&this->dyna.actor) >= BIO_BABA_TYPE_FRAGMENT_BASE) {
        SkelAnime_InitFlex(play, &this->headSkelAnime, &gBioDekuBabaHeadSkel, &gBioDekuBabaHeadChompAnim,
                           this->headJointTable, this->headMorphTable, BIO_DEKU_BABA_HEAD_LIMB_MAX);

        this->dyna.actor.gravity = 0.0f;
        this->dyna.actor.world.rot.y = Rand_ZeroFloat(0x8000);
        this->dyna.actor.speed = Rand_ZeroFloat(3.0f) + 3.0f;
        this->dyna.actor.velocity.y = Rand_ZeroFloat(1.5f) + 1.5f;

        this->fragmentAngularVelocity.x = Rand_CenteredFloat(700.0f);
        this->fragmentAngularVelocity.y = Rand_CenteredFloat(1500.0f);
        this->timers[TIMER_CURRENT_ACTION] = Rand_ZeroFloat(30.0f) + 50.0f;

        this->dyna.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->actionFunc = Boss05_Fragment_Move;
    }
}

void Boss05_Destroy(Actor* thisx, PlayState* play) {
    Boss05* this = (Boss05*)thisx;

    if ((BIO_BABA_GET_TYPE(&this->dyna.actor) == BIO_BABA_TYPE_LILY_PAD) ||
        (BIO_BABA_GET_TYPE(&this->dyna.actor) == BIO_BABA_TYPE_LILY_PAD_WITH_HEAD) ||
        (BIO_BABA_GET_TYPE(&this->dyna.actor) == BIO_BABA_TYPE_NO_LEAF_LILY_PAD_WITH_HEAD)) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

/**
 * Checks to see if the lily pad stem or the head was hit by an attack. If either of them were hit, this function will
 * return a value indicating what the head should do; if the head should detach from the lily pad, then the draw damage
 * effect to apply to the falling head will be included in the return value.
 */
s32 Boss05_LilyPadWithHead_UpdateDamage(Boss05* this, PlayState* play) {
    if (this->damagedFlashTimer == 0) {
        s32 i = 0;

        while (true) {
            if (this->lilyPadCollider.elements[i].base.acElemFlags & ACELEM_HIT) {
                switch (this->dyna.actor.colChkInfo.damageEffect) {
                    case BIO_BABA_DMGEFF_FIRE:
                        return BIO_BABA_HEAD_HIT_REACTION_DEATCH + BIO_BABA_DRAW_DMGEFF_STATE_FIRE_INIT;

                    case BIO_BABA_DMGEFF_FREEZE:
                        return BIO_BABA_HEAD_HIT_REACTION_DEATCH + BIO_BABA_DRAW_DMGEFF_STATE_FROZEN_INIT;

                    case BIO_BABA_DMGEFF_LIGHT_ORB:
                        return BIO_BABA_HEAD_HIT_REACTION_DEATCH + BIO_BABA_DRAW_DMGEFF_STATE_LIGHT_ORB_INIT;

                    default:
                        return BIO_BABA_HEAD_HIT_REACTION_DEATCH + BIO_BABA_DRAW_DMGEFF_STATE_NONE;
                }
            }

            i++;
            if (i == BIO_BABA_LILY_PAD_COLLIDER_MAX) {
                if (this->headCollider.elements[BIO_BABA_HEAD_COLLIDER_HEAD].base.acElemFlags & ACELEM_HIT) {
                    u8 damage = this->dyna.actor.colChkInfo.damage;

                    this->dyna.actor.colChkInfo.health -= damage;
                    if ((s8)this->dyna.actor.colChkInfo.health <= 0) {
                        Enemy_StartFinishingBlow(play, &this->dyna.actor);
                        return BIO_BABA_HEAD_HIT_REACTION_DEAD;
                    }

                    Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_MIZUBABA2_DAMAGE);
                    this->damagedFlashTimer = 15;
                    this->lilyPadWithHeadAttackState = BIO_BABA_LILY_PAD_WITH_HEAD_ATTACK_STATE_IDLE;
                    this->timers[TIMER_CURRENT_ACTION] = 30;
                    return BIO_BABA_HEAD_HIT_REACTION_NONE_OR_DAMAGED;
                }

                break;
            }
        }
    }

    return BIO_BABA_HEAD_HIT_REACTION_NONE_OR_DAMAGED;
}

void Boss05_LilyPadWithHead_SetupMove(Boss05* this, PlayState* play) {
    this->actionFunc = Boss05_LilyPadWithHead_Move;
}

static Vec3s sWindUpLimbRot[BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_MAX] = {
    { 0x3200, 0, 0 },  // BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_UPPER_STEM
    { -0x1E00, 0, 0 }, // BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_MIDDLE_STEM
    { -0x1400, 0, 0 }, // BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_LOWER_STEM
    { 0, 0, 0 },       // BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_LEFT_UPPER_ARM
    { 0, 0, 0 },       // BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_LEFT_LOWER_ARM
    { 0, 0, 0 },       // BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_RIGHT_UPPER_ARM
    { 0, 0, 0 },       // BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_RIGHT_LOWER_ARM
};

static Vec3s sLungeAttackLimbRot[BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_MAX] = {
    { -0x3200, 0, 0 }, // BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_UPPER_STEM
    { 0, 0, 0 },       // BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_MIDDLE_STEM
    { 0x1E00, 0, 0 },  // BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_LOWER_STEM
    { 0, 0, 0 },       // BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_LEFT_UPPER_ARM
    { 0, 0, 0 },       // BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_LEFT_LOWER_ARM
    { 0, 0, 0 },       // BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_RIGHT_UPPER_ARM
    { 0, 0, 0 },       // BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_RIGHT_LOWER_ARM
};

/**
 * Controls everything about how the lily pad and head should move and act, including manually rotating the limbs of the
 * lily pad, changing the speed of the head animations, enabling collisions, spawning child actors when the head is
 * detached or destroyed, etc.
 */
void Boss05_LilyPadWithHead_Move(Boss05* this, PlayState* play) {
    s32 i;
    Player* player = GET_PLAYER(play);
    s32 j;
    u8 disableATCollisions = false;
    Vec3s targetLimbRot[BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_MAX];
    s32 frequencyX;
    s32 shiftX;
    s32 amplitudeX;
    s32 frequencyY;
    s32 shiftY;
    s32 amplitudeY;
    s32 frequencyZ;
    s32 shiftZ;
    s32 amplitudeZ;
    s32 limbRotMaxAngularVelocityFrac;
    s32 limbRotAngularVelocity;
    s16 rotAngularVelocity;
    s16 rotMaxAngularVelocityFrac;
    f32 diffY;
    f32 attackRange;
    Vec3f splashPos;
    s32 hitReaction;

    this->dyna.actor.hintId = TATL_HINT_ID_BIO_DEKU_BABA;

    if (this->lilyPadWithHeadMovementState == BIO_BABA_LILY_PAD_WITH_HEAD_MOVEMENT_STATE_IDLE) {
        frequencyX = 0x3E8;
        shiftX = 0x3E80;
        amplitudeX = 0x7D0;

        frequencyY = 0x5DC;
        shiftY = 0x1770;
        amplitudeY = 0xBB8;

        frequencyZ = 0x514;
        shiftZ = 0x5208;
        amplitudeZ = 0xDAC;

        limbRotMaxAngularVelocityFrac = 0xA;
        limbRotAngularVelocity = 0x300;

        if (this->damagedFlashTimer != 0) {
            frequencyX = 0x1B58;
            amplitudeX = 0x1770;

            amplitudeY = 0x1770;

            frequencyZ = 0x1C84;
            amplitudeZ = 0x1964;

            limbRotMaxAngularVelocityFrac = 1;
            limbRotAngularVelocity = 0x1000;
        }
    } else if (this->lilyPadWithHeadMovementState == BIO_BABA_LILY_PAD_WITH_HEAD_MOVEMENT_STATE_FLIP_ATTACK) {
        frequencyX = 0x1B58;
        shiftX = 0x3E80;
        amplitudeX = 0x1388;

        frequencyY = 0x5DC;
        shiftY = 0x2328;
        amplitudeY = 0x1388;

        frequencyZ = 0x1C84;
        shiftZ = 0x5208;
        amplitudeZ = 0x157C;

        limbRotMaxAngularVelocityFrac = 1;
        limbRotAngularVelocity = 0x1000;
    } else {
        frequencyX = shiftX = amplitudeX = frequencyY = shiftY = amplitudeY = frequencyZ = shiftZ = amplitudeZ =
            limbRotMaxAngularVelocityFrac = limbRotAngularVelocity = 0;
    }

    for (i = 0; i < BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_MAX; i++) {
        if ((this->lilyPadWithHeadMovementState == BIO_BABA_LILY_PAD_WITH_HEAD_MOVEMENT_STATE_IDLE) ||
            (this->lilyPadWithHeadMovementState == BIO_BABA_LILY_PAD_WITH_HEAD_MOVEMENT_STATE_FLIP_ATTACK)) {
            if (i <= BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_LOWER_STEM) {
                targetLimbRot[i].y = 0;
                targetLimbRot[i].x = Math_SinS((this->frameCounter * frequencyX) + (i * shiftX)) * amplitudeX;
            } else {
                targetLimbRot[i].x = 0;
                targetLimbRot[i].y = Math_SinS((this->frameCounter * frequencyY) + (i * shiftY)) * amplitudeY;
            }

            if ((i == BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_LEFT_LOWER_ARM) ||
                (i == BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_RIGHT_LOWER_ARM)) {
                targetLimbRot[i].z = Math_SinS((this->frameCounter * frequencyZ) + (i * shiftZ)) * amplitudeZ * 2.0f;
            } else {
                targetLimbRot[i].z = Math_SinS((this->frameCounter * frequencyZ) + (i * shiftZ)) * amplitudeZ;
            }

            rotMaxAngularVelocityFrac = limbRotMaxAngularVelocityFrac;
            rotAngularVelocity = limbRotAngularVelocity;
        } else {
            if (this->lilyPadWithHeadMovementState == BIO_BABA_LILY_PAD_WITH_HEAD_MOVEMENT_STATE_WIND_UP) {
                limbRotMaxAngularVelocityFrac = 5;
                limbRotAngularVelocity = 0x1000;

                targetLimbRot[i].x = sWindUpLimbRot[i].x;
                targetLimbRot[i].y = sWindUpLimbRot[i].y;
                targetLimbRot[i].z = sWindUpLimbRot[i].z;
            } else if (this->lilyPadWithHeadMovementState == BIO_BABA_LILY_PAD_WITH_HEAD_MOVEMENT_STATE_LUNGE_ATTACK) {
                limbRotMaxAngularVelocityFrac = 1;
                limbRotAngularVelocity = 0x2000;

                targetLimbRot[i].x = sLungeAttackLimbRot[i].x;
                targetLimbRot[i].y = sLungeAttackLimbRot[i].y;
                targetLimbRot[i].z = sLungeAttackLimbRot[i].z;
            }

            rotMaxAngularVelocityFrac = limbRotMaxAngularVelocityFrac;
            rotAngularVelocity = limbRotAngularVelocity;
        }

        Math_ApproachS(&this->lilyPadWithHeadLimbRot[i].x, targetLimbRot[i].x, limbRotMaxAngularVelocityFrac,
                       limbRotAngularVelocity);
        Math_ApproachS(&this->lilyPadWithHeadLimbRot[i].y, targetLimbRot[i].y, limbRotMaxAngularVelocityFrac,
                       limbRotAngularVelocity);
        Math_ApproachS(&this->lilyPadWithHeadLimbRot[i].z, targetLimbRot[i].z, limbRotMaxAngularVelocityFrac,
                       limbRotAngularVelocity);
    }

    diffY = (player->actor.world.pos.y - this->dyna.actor.world.pos.y) + 10.0f;
    if (diffY < 0.0f) {
        Math_ApproachS(&this->dyna.actor.shape.rot.y, this->dyna.actor.yawTowardsPlayer, rotMaxAngularVelocityFrac,
                       rotAngularVelocity);
    }

    SkelAnime_Update(&this->headSkelAnime);
    Math_ApproachF(&this->lowerJawScaleXZ, 1.0f, 0.1f, 0.1f);

    if (this->lilyPadWithHeadAttackState != BIO_BABA_LILY_PAD_WITH_HEAD_ATTACK_STATE_FLIP_ATTACK) {
        Math_ApproachS(&this->dyna.actor.shape.rot.x, 0, 0x14, 0x800);
    }

    switch (this->lilyPadWithHeadAttackState) {
        case BIO_BABA_LILY_PAD_WITH_HEAD_ATTACK_STATE_IDLE:
            this->lilyPadWithHeadMovementState = BIO_BABA_LILY_PAD_WITH_HEAD_MOVEMENT_STATE_IDLE;
            this->headSkelAnime.playSpeed = 1.0f;

            if (diffY < 0.0f) {
                if (player->actor.speed > 10.0f) {
                    attackRange = 220.0f;
                } else {
                    attackRange = 150.0f;
                }

                if ((this->timers[TIMER_CURRENT_ACTION] == 0) &&
                    (sqrtf(this->dyna.actor.xyzDistToPlayerSq) <= attackRange)) {
                    this->lilyPadWithHeadAttackState = BIO_BABA_LILY_PAD_WITH_HEAD_ATTACK_STATE_WIND_UP;
                    this->timers[TIMER_CURRENT_ACTION] = 10;
                }

                this->flipAttackFrameCounter = 0;
            } else {
                disableATCollisions = true;

                if (sqrtf(this->dyna.actor.xyzDistToPlayerSq) <= 40.0f) {
                    this->flipAttackFrameCounter++;

                    if (this->flipAttackFrameCounter > 30) {
                        this->lilyPadWithHeadAttackState = 10;
                        this->timers[TIMER_CURRENT_ACTION] = 30;
                        Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_MIZUBABA1_ATTACK);
                    }
                } else {
                    this->flipAttackFrameCounter = 0;
                }
            }

            if (((this->frameCounter % 4) == 0) && (Rand_ZeroOne() < 0.5f)) {
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_MIZUBABA1_MOUTH);
            }
            break;

        case BIO_BABA_LILY_PAD_WITH_HEAD_ATTACK_STATE_FLIP_ATTACK:
            if ((this->frameCounter % 2) == 0) {
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_MIZUBABA1_MOUTH);
            }

            this->lilyPadWithHeadStemRotX = (this->timers[TIMER_CURRENT_ACTION] & 1) * 0x200;
            this->lilyPadWithHeadMovementState = BIO_BABA_LILY_PAD_WITH_HEAD_MOVEMENT_STATE_FLIP_ATTACK;
            this->headSkelAnime.playSpeed = 4.0f;
            Math_ApproachS(&this->dyna.actor.shape.rot.x, -0x8000, 2, 0x2000);
            Math_ApproachS(&this->dyna.actor.shape.rot.y, this->dyna.actor.yawTowardsPlayer, 2, 0x2000);

            if (this->timers[TIMER_CURRENT_ACTION] == 0) {
                this->flipAttackFrameCounter = 0;
                this->lilyPadWithHeadAttackState = BIO_BABA_LILY_PAD_WITH_HEAD_ATTACK_STATE_IDLE;
                this->timers[TIMER_CURRENT_ACTION] = 100;
            }

            if (this->timers[TIMER_CURRENT_ACTION] == 27) {
                Math_Vec3f_Copy(&splashPos, &this->dyna.actor.world.pos);
                splashPos.y += 40.0f;
                EffectSsGSplash_Spawn(play, &splashPos, NULL, NULL, 1, 2000);
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_OUT_OF_WATER);
            }
            break;

        case BIO_BABA_LILY_PAD_WITH_HEAD_ATTACK_STATE_WIND_UP:
            if (Animation_OnFrame(&this->headSkelAnime, this->animEndFrame)) {
                this->headSkelAnime.playSpeed = 0.0f;
            }

            this->lilyPadWithHeadMovementState = BIO_BABA_LILY_PAD_WITH_HEAD_MOVEMENT_STATE_WIND_UP;

            if (this->timers[TIMER_CURRENT_ACTION] == 0) {
                this->lilyPadWithHeadAttackState = BIO_BABA_LILY_PAD_WITH_HEAD_ATTACK_STATE_LUNGE_ATTACK;
                this->timers[TIMER_CURRENT_ACTION] = 20;
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_MIZUBABA1_ATTACK);
            }
            break;

        case BIO_BABA_LILY_PAD_WITH_HEAD_ATTACK_STATE_LUNGE_ATTACK:
            if ((this->frameCounter % 2) == 0) {
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_MIZUBABA1_MOUTH);
            }

            this->lilyPadWithHeadStemRotX = (this->timers[TIMER_CURRENT_ACTION] & 1) * 0x200;
            this->headSkelAnime.playSpeed = 4.0f;
            Math_ApproachF(&this->lowerJawScaleXZ, 1.5f, 1.0f, 0.7f);
            this->lilyPadWithHeadMovementState = BIO_BABA_LILY_PAD_WITH_HEAD_MOVEMENT_STATE_LUNGE_ATTACK;

            if (this->timers[TIMER_CURRENT_ACTION] == 0) {
                this->lilyPadWithHeadAttackState = BIO_BABA_LILY_PAD_WITH_HEAD_ATTACK_STATE_IDLE;
                this->timers[TIMER_CURRENT_ACTION] = 30;
            }
            break;

        default:
            break;
    }

    hitReaction = Boss05_LilyPadWithHead_UpdateDamage(this, play);
    if ((hitReaction != BIO_BABA_HEAD_HIT_REACTION_NONE_OR_DAMAGED) || (this->forceDetachTimer == 1)) {
        Boss05* child;

        this->dyna.actor.params = BIO_BABA_TYPE_LILY_PAD;
        this->actionFunc = Boss05_LilyPad_Idle;
        this->dyna.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        Actor_ChangeCategory(play, &play->actorCtx, &this->dyna.actor, ACTORCAT_BG);
        if (this->forceDetachTimer != 0) {
            hitReaction = BIO_BABA_HEAD_HIT_REACTION_DEATCH;
        }

        if (hitReaction >= BIO_BABA_HEAD_HIT_REACTION_DEATCH) {
            child = (Boss05*)Actor_SpawnAsChild(
                &play->actorCtx, &this->dyna.actor, play, ACTOR_BOSS_05, this->dyna.actor.world.pos.x,
                this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z, this->dyna.actor.shape.rot.x,
                this->dyna.actor.shape.rot.y, this->dyna.actor.shape.rot.z, BIO_BABA_TYPE_FALLING_HEAD);

            if (child != NULL) {
                Player* player2 = GET_PLAYER(play);

                if (player2->focusActor == &this->dyna.actor) {
                    player2->focusActor = &child->dyna.actor;
                    play->actorCtx.attention.tatlHoverActor = &child->dyna.actor;
                    play->actorCtx.attention.reticleActor = &child->dyna.actor;
                }

                for (i = 0; i < BIO_DEKU_BABA_LILY_PAD_LIMB_MAX; i++) {
                    child->lilyPadSkelAnime.jointTable[i] = this->lilyPadSkelAnime.jointTable[i];
                }

                for (i = 0; i < BIO_DEKU_BABA_HEAD_LIMB_MAX; i++) {
                    child->headSkelAnime.jointTable[i] = this->headSkelAnime.jointTable[i];
                }

                Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_MIZUBABA1_DAMAGE);

                if (hitReaction > BIO_BABA_HEAD_HIT_REACTION_DEATCH) {
                    child->drawDmgEffState = hitReaction - BIO_BABA_HEAD_HIT_REACTION_DEATCH;
                }
            }
        } else if (hitReaction == BIO_BABA_HEAD_HIT_REACTION_DEAD) {
            for (i = 0; i < 2; i++) {
                child = (Boss05*)Actor_SpawnAsChild(&play->actorCtx, &this->dyna.actor, play, ACTOR_BOSS_05,
                                                    this->headPos.x, this->headPos.y, this->headPos.z, this->headRot.x,
                                                    this->headRot.y, this->headRot.z, BIO_BABA_TYPE_FRAGMENT_BASE + i);

                if (child != NULL) {
                    for (j = 0; j < BIO_DEKU_BABA_HEAD_LIMB_MAX; j++) {
                        child->headSkelAnime.jointTable[j] = this->headSkelAnime.jointTable[j];
                    }

                    child->timers[TIMER_CURRENT_ACTION] = Rand_ZeroFloat(20.0f) + 20.0f;
                }
            }

            Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_MIZUBABA_DEAD);
        }
    }

    if (!disableATCollisions) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->lilyPadCollider.base);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->headCollider.base);
    }

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->lilyPadCollider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->headCollider.base);
}

/**
 * Makes the lily pad sink slightly and spawn some ripples when the player stands on it.
 */
void Boss05_LilyPad_Idle(Boss05* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 distToPlayer = sqrtf(this->dyna.actor.xyzDistToPlayerSq);
    s16 targetRotX;

    if ((distToPlayer < 50.0f) && (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        targetRotX = distToPlayer * 80.0f;
        targetRotX = CLAMP_MAX(targetRotX, 0x7D0);

        Math_ApproachS(&this->lilyPadRotX, targetRotX, 0x14, 0x7D0);
        Math_ApproachS(&this->lilyPadRotY, this->dyna.actor.yawTowardsPlayer, 0x14, 0xFA0);
        if ((this->frameCounter % 16) == 0) {
            EffectSsGRipple_Spawn(play, &this->dyna.actor.world.pos, 500, 1000, 0);
        }
    } else {
        Math_ApproachS(&this->lilyPadRotX, 0, 0x14, 0x100);
    }
}

/**
 * Controls everything about how the falling head should move and act, including manually adjusting its gravity,
 * spawning a splash if the head touches the water, spawning bubbles as it sinks through the water, spawning a walking
 * head when it touches the ground, etc.
 */
void Boss05_FallingHead_Fall(Boss05* this, PlayState* play) {
    s32 i;
    Vec3f unusedPos;
    Vec3f bubblePos;
    Boss05* walkingHead;

    Actor_MoveWithGravity(&this->dyna.actor);
    this->dyna.actor.world.pos.y -= 85.0f;
    this->dyna.actor.prevPos.y -= 85.0f;
    Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 20.0f, 50.0f, 40.0f,
                            UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_40);
    this->dyna.actor.world.pos.y += 85.0f;
    this->dyna.actor.prevPos.y += 85.0f;

    if (this->dyna.actor.bgCheckFlags & BGCHECKFLAG_WATER) {
        this->dyna.actor.gravity = -0.3f;
        this->dyna.actor.terminalVelocity = -5.0f;
    } else {
        this->dyna.actor.gravity = -2.0f;
        this->dyna.actor.terminalVelocity = -20.0f;
    }

    if (this->dyna.actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH) {
        this->dyna.actor.velocity.y = -3.0f;
        this->dyna.actor.gravity = -0.3f;
        this->dyna.actor.terminalVelocity = -5.0f;
        Math_Vec3f_Copy(&unusedPos, &this->dyna.actor.world.pos);
        unusedPos.y += 20.0f;
        EffectSsGSplash_Spawn(play, &this->dyna.actor.world.pos, NULL, NULL, 1, 1000);
        this->timers[TIMER_FALLING_HEAD_FALL] = 20;
    }

    if (this->timers[TIMER_FALLING_HEAD_FALL] != 0) {
        for (i = 0; i < 3; i++) {
            bubblePos.x = Rand_CenteredFloat(40.0f) + this->dyna.actor.world.pos.x;
            bubblePos.y = Rand_CenteredFloat(40.0f) + this->dyna.actor.world.pos.y;
            bubblePos.z = Rand_CenteredFloat(40.0f) + this->dyna.actor.world.pos.z;
            EffectSsBubble_Spawn(play, &bubblePos, 20.0f, 10.0f, 20.0f, 0.13f);
        }
    }

    if (this->dyna.actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) && (this->drawDmgEffTimer != 0)) {
            this->drawDmgEffTimer = 0;
        }

        Math_ApproachZeroF(&this->fallingHeadLilyPadLimbScale, 1.0f, 0.05f);

        if (this->fallingHeadLilyPadLimbScale == 0.0f) {
            walkingHead = (Boss05*)Actor_SpawnAsChild(
                &play->actorCtx, &this->dyna.actor, play, ACTOR_BOSS_05, this->headPos.x, this->headPos.y,
                this->headPos.z, this->headRot.x, this->headRot.y, this->headRot.z, BIO_BABA_TYPE_WALKING_HEAD);

            if (walkingHead != NULL) {
                Player* player = GET_PLAYER(play);

                if (player->focusActor == &this->dyna.actor) {
                    player->focusActor = &walkingHead->dyna.actor;
                    play->actorCtx.attention.tatlHoverActor = &walkingHead->dyna.actor;
                    play->actorCtx.attention.reticleActor = &walkingHead->dyna.actor;
                }

                for (i = 0; i < BIO_DEKU_BABA_HEAD_LIMB_MAX; i++) {
                    walkingHead->headSkelAnime.jointTable[i] = this->headSkelAnime.jointTable[i];
                }

                Actor_PlaySfx(&walkingHead->dyna.actor, NA_SE_EN_MIZUBABA_TRANSFORM);
            }

            Actor_Kill(&this->dyna.actor);
        }
    }
}

void Boss05_WalkingHead_UpdateDamage(Boss05* this, PlayState* play) {
    s32 pad[2];
    u8 attackDealsDamage = false;
    ColliderElement* acHitElem;

    if ((this->damagedTimer == 0) &&
        (this->headCollider.elements[BIO_BABA_HEAD_COLLIDER_HEAD].base.acElemFlags & ACELEM_HIT)) {
        this->headCollider.elements[BIO_BABA_HEAD_COLLIDER_HEAD].base.acElemFlags &= ~ACELEM_HIT;
        acHitElem = this->headCollider.elements[BIO_BABA_HEAD_COLLIDER_HEAD].base.acHitElem;
        if (acHitElem->atDmgInfo.dmgFlags & 0x300000) { // (DMG_NORMAL_SHIELD | DMG_LIGHT_RAY)
            this->knockbackMagnitude = -12.0f;
            this->knockbackAngle = this->dyna.actor.yawTowardsPlayer;
            this->damagedTimer = 6;
            return;
        }

        this->damagedTimer = 10;
        this->dyna.actor.speed = 0.0f;
        this->knockbackMagnitude = -20.0f;
        this->knockbackAngle = this->dyna.actor.yawTowardsPlayer;

        Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_MIZUBABA2_DAMAGE);

        switch (this->dyna.actor.colChkInfo.damageEffect) {
            case BIO_BABA_DMGEFF_STUN:
                Boss05_WalkingHead_SetupStunned(this, play);
                break;

            case BIO_BABA_DMGEFF_FIRE:
                this->drawDmgEffState = BIO_BABA_DRAW_DMGEFF_STATE_FIRE_INIT;
                attackDealsDamage = true;
                break;

            case BIO_BABA_DMGEFF_FREEZE:
                Boss05_WalkingHead_SetupFreeze(this, play);
                this->drawDmgEffState = BIO_BABA_DRAW_DMGEFF_STATE_FROZEN_INIT;
                break;

            case BIO_BABA_DMGEFF_LIGHT_ORB:
                this->drawDmgEffState = BIO_BABA_DRAW_DMGEFF_STATE_LIGHT_ORB_INIT;
                attackDealsDamage = true;
                break;

            default:
                attackDealsDamage = true;
                break;
        }

        if (attackDealsDamage) {
            u8 damage;

            if ((this->actionFunc == Boss05_WalkingHead_Stunned) &&
                (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) && (this->drawDmgEffTimer != 0)) {
                Boss05_WalkingHead_Thaw(this, play);
                this->drawDmgEffState = BIO_BABA_DRAW_DMGEFF_STATE_NONE;
            }

            damage = this->dyna.actor.colChkInfo.damage;
            this->dyna.actor.colChkInfo.health -= damage;
            if ((s8)this->dyna.actor.colChkInfo.health <= 0) {
                Enemy_StartFinishingBlow(play, &this->dyna.actor);
            }

            Boss05_WalkingHead_SetupDamaged(this, play);
            this->damagedFlashTimer = 15;
        }
    }
}

/**
 * Returns true if the Bio Deku Baba's model is rotated such that it is roughly facing the player, false otherwise.
 */
s32 Boss05_WalkingHead_IsLookingAtPlayer(Boss05* this, PlayState* play) {
    s16 yawDiff = this->dyna.actor.yawTowardsPlayer - this->dyna.actor.shape.rot.y;

    if (ABS_ALT(yawDiff) < 0x3000) {
        return true;
    } else {
        return false;
    }
}

/**
 * Checks to see if the Bio Deku Baba is looking at the player, if it's within 200 units of the player, and if the
 * player is not too far above or below the Bio Deku Baba. If all of these conditions are met, it will setup the Bio
 * Deku Baba to play its spotted animation and start charging towards the player.
 */
void Boss05_WalkingHead_TrySpottingPlayer(Boss05* this, PlayState* play) {
    if (Boss05_WalkingHead_IsLookingAtPlayer(this, play) && (this->dyna.actor.xyzDistToPlayerSq <= SQ(200.0f)) &&
        (fabsf(this->dyna.actor.playerHeightRel) < 70.0f)) {
        Boss05_WalkingHead_SetupSpottedPlayer(this, play);
    }
}

void Boss05_WalkingHead_SetupTransform(Boss05* this, PlayState* play) {
    this->actionFunc = Boss05_WalkingHead_Transform;
    Animation_MorphToPlayOnce(&this->headSkelAnime, &gBioDekuBabaHeadTransformAnim, -5.0f);
}

/**
 * Handles the "transformation" the Bio Deku Baba undergoes where its head grows legs and eyestalks. This function will
 * gradually scale up the Bio Deku Baba's various limbs, and once they reach their full size, it will put the Bio Deku
 * Baba into an idle state.
 */
void Boss05_WalkingHead_Transform(Boss05* this, PlayState* play) {
    SkelAnime_Update(&this->headSkelAnime);
    Math_ApproachS(&this->dyna.actor.shape.rot.x, 0, 2, 0x400);
    Math_ApproachS(&this->dyna.actor.shape.rot.z, 0, 2, 0x400);
    Math_ApproachF(&this->headBodyScale, 1.0f, 1.0f, 0.5f);
    Math_ApproachF(&this->headLimbScale, 1.0f, 1.0f, 0.14f);

    if (this->headLimbScale == 1.0f) {
        Boss05_WalkingHead_SetupIdle(this, play);
    }
}

void Boss05_WalkingHead_SetupIdle(Boss05* this, PlayState* play) {
    this->actionFunc = Boss05_WalkingHead_Idle;
    Animation_MorphToLoop(&this->headSkelAnime, &gBioDekuBabaHeadIdleAnim, -10.0f);
    this->timers[TIMER_CURRENT_ACTION] = Rand_ZeroFloat(25.0f) + 25.0f;
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_MIZUBABA1_MOUTH);
}

/**
 * Waits in place for 25-50 frames, then starts walking.
 */
void Boss05_WalkingHead_Idle(Boss05* this, PlayState* play) {
    SkelAnime_Update(&this->headSkelAnime);
    Math_ApproachZeroF(&this->dyna.actor.speed, 1.0f, 2.5f);
    Math_ApproachS(&this->dyna.actor.shape.rot.x, 0, 2, 0x400);
    Math_ApproachS(&this->dyna.actor.shape.rot.z, 0, 2, 0x400);

    if (this->timers[TIMER_CURRENT_ACTION] == 0) {
        Boss05_WalkingHead_SetupWalk(this, play);
    }

    Boss05_WalkingHead_TrySpottingPlayer(this, play);
}

void Boss05_WalkingHead_SetupWalk(Boss05* this, PlayState* play) {
    this->actionFunc = Boss05_WalkingHead_Walk;
    Animation_MorphToLoop(&this->headSkelAnime, &gBioDekuBabaHeadWalkAnim, 0.0f);
    this->timers[TIMER_CURRENT_ACTION] = Rand_ZeroFloat(80.0f) + 60.0f;
    this->walkTargetPos.x = Rand_CenteredFloat(400.0f) + this->dyna.actor.world.pos.x;
    this->walkTargetPos.z = Rand_CenteredFloat(400.0f) + this->dyna.actor.world.pos.z;
    this->walkAngularVelocityY = 0.0f;
}

/**
 * Walks toward a randomized target position until either 60-140 frames pass or until it gets close enough to its
 * target. If either condition is met, the Bio Deku Baba will go back to being idle.
 */
void Boss05_WalkingHead_Walk(Boss05* this, PlayState* play) {
    f32 diffX;
    f32 diffZ;

    Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_MIZUBABA2_WALK - SFX_FLAG);
    SkelAnime_Update(&this->headSkelAnime);
    Math_ApproachF(&this->dyna.actor.speed, 5.0f, 1.0f, 2.0f);
    diffX = this->walkTargetPos.x - this->dyna.actor.world.pos.x;
    diffZ = this->walkTargetPos.z - this->dyna.actor.world.pos.z;
    Math_ApproachS(&this->dyna.actor.world.rot.y, Math_Atan2S(diffX, diffZ), 5, this->walkAngularVelocityY);
    Math_ApproachF(&this->walkAngularVelocityY, 2000.0f, 1.0f, 100.0f);

    if ((this->timers[TIMER_CURRENT_ACTION] == 0) || ((SQ(diffX) + SQ(diffZ)) < SQ(50.0f))) {
        Boss05_WalkingHead_SetupIdle(this, play);
    }

    Boss05_WalkingHead_TrySpottingPlayer(this, play);
}

void Boss05_WalkingHead_SetupSpottedPlayer(Boss05* this, PlayState* play) {
    this->actionFunc = Boss05_WalkingHead_SpottedPlayer;
    Animation_MorphToPlayOnce(&this->headSkelAnime, &gBioDekuBabaHeadSpotAnim, 0.0f);
    this->timers[TIMER_CURRENT_ACTION] = 20;
}

/**
 * Play the spotted animation and turn to face the player for 20 frames, then sets the Bio Deku Baba up to charge.
 */
void Boss05_WalkingHead_SpottedPlayer(Boss05* this, PlayState* play) {
    SkelAnime_Update(&this->headSkelAnime);
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_MIZUBABA2_VOICE - SFX_FLAG);
    Math_ApproachZeroF(&this->dyna.actor.speed, 1.0f, 2.5f);
    Math_ApproachS(&this->dyna.actor.world.rot.y, this->dyna.actor.yawTowardsPlayer, 5, 0x1000);

    if (this->timers[TIMER_CURRENT_ACTION] == 0) {
        Boss05_WalkingHead_SetupCharge(this, play);
    }
}

void Boss05_WalkingHead_SetupCharge(Boss05* this, PlayState* arg1) {
    this->actionFunc = Boss05_WalkingHead_Charge;
    Animation_MorphToLoop(&this->headSkelAnime, &gBioDekuBabaHeadChargeAnim, 0.0f);
    this->timers[TIMER_CURRENT_ACTION] = 60;
    this->walkAngularVelocityY = 0.0f;
}

/**
 * Charge toward the player until either 60 frames pass or the Bio Deku Baba gets close enough to the player. If either
 * condition is met, the Bio Deku Baba will switch to attacking the player.
 */
void Boss05_WalkingHead_Charge(Boss05* this, PlayState* play) {
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_MIZUBABA2_VOICE - SFX_FLAG);
    SkelAnime_Update(&this->headSkelAnime);
    Math_ApproachF(&this->dyna.actor.speed, 8.0f, 1.0f, 4.0f);
    Math_ApproachS(&this->dyna.actor.world.rot.y, this->dyna.actor.yawTowardsPlayer, 5, this->walkAngularVelocityY);
    Math_ApproachF(&this->walkAngularVelocityY, 4000.0f, 1.0f, 400.0f);

    if ((this->timers[TIMER_CURRENT_ACTION] == 0) || (this->dyna.actor.xyzDistToPlayerSq <= SQ(150.0f))) {
        Boss05_WalkingHead_SetupAttack(this, play);
    }
}

void Boss05_WalkingHead_SetupAttack(Boss05* this, PlayState* arg1) {
    this->actionFunc = Boss05_WalkingHead_Attack;
    Animation_MorphToPlayOnce(&this->headSkelAnime, &gBioDekuBabaHeadAttackAnim, 0.0f);
    this->animEndFrame = Animation_GetLastFrame(&gBioDekuBabaHeadAttackAnim);
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_MIZUBABA2_ATTACK);
}

/**
 * Play the attack animation to completion, then go back to being idle.
 */
void Boss05_WalkingHead_Attack(Boss05* this, PlayState* play) {
    SkelAnime_Update(&this->headSkelAnime);

    if (Animation_OnFrame(&this->headSkelAnime, this->animEndFrame)) {
        Boss05_WalkingHead_SetupIdle(this, play);
    }
}

void Boss05_WalkingHead_SetupDamaged(Boss05* this, PlayState* play) {
    this->actionFunc = Boss05_WalkingHead_Damaged;
    Animation_MorphToPlayOnce(&this->headSkelAnime, &gBioDekuBabaHeadDamagedAnim, 0.0f);
    this->animEndFrame = Animation_GetLastFrame(&gBioDekuBabaHeadAttackAnim);
    Actor_SetColorFilter(&this->dyna.actor, COLORFILTER_COLORFLAG_RED, 120, COLORFILTER_BUFFLAG_OPA, 30);
}

/**
 * If the Bio Deku Baba was killed by the most recent attack, this function will play its damaged animation for 22
 * frames, then spawn fragments for each of the walking head's limbs along with many bubble effects. Otherwise, it will
 * play the damaged animation to completion and then make the Bio Deku Baba spot the player.
 */
void Boss05_WalkingHead_Damaged(Boss05* this, PlayState* play) {
    s32 i;
    s32 j;
    Vec3f bubblePos;
    Boss05* fragment;

    SkelAnime_Update(&this->headSkelAnime);

    if ((s8)this->dyna.actor.colChkInfo.health <= 0) {
        if (Animation_OnFrame(&this->headSkelAnime, 22.0f)) {
            for (i = 0; i < BIO_BABA_TYPE_MAX - BIO_BABA_TYPE_FRAGMENT_BASE; i++) {
                fragment = (Boss05*)Actor_SpawnAsChild(
                    &play->actorCtx, &this->dyna.actor, play, ACTOR_BOSS_05, this->dyna.actor.world.pos.x,
                    this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z, this->dyna.actor.shape.rot.x,
                    this->dyna.actor.shape.rot.y, this->dyna.actor.shape.rot.z, BIO_BABA_TYPE_FRAGMENT_BASE + i);

                if (fragment != NULL) {
                    for (j = 0; j < BIO_DEKU_BABA_HEAD_LIMB_MAX; j++) {
                        fragment->headSkelAnime.jointTable[j] = this->headSkelAnime.jointTable[j];
                    }
                }
            }

            for (i = 0; i < 20; i++) {
                bubblePos.x = Rand_CenteredFloat(40.0f) + this->dyna.actor.world.pos.x;
                bubblePos.y = Rand_CenteredFloat(40.0f) + this->dyna.actor.world.pos.y;
                bubblePos.z = Rand_CenteredFloat(40.0f) + this->dyna.actor.world.pos.z;
                EffectSsBubble_Spawn(play, &bubblePos, 20.0f, 10.0f, 20.0f, 0.13f);
            }

            SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.world.pos, 40, NA_SE_EN_MIZUBABA_DEAD);
            Actor_Kill(&this->dyna.actor);
            Item_DropCollectibleRandom(play, NULL, &this->dyna.actor.world.pos, 0xE0);
        }
    } else if (Animation_OnFrame(&this->headSkelAnime, this->animEndFrame)) {
        Boss05_WalkingHead_SetupSpottedPlayer(this, play);
    }
}

void Boss05_WalkingHead_SetupStunned(Boss05* this, PlayState* play) {
    this->actionFunc = Boss05_WalkingHead_Stunned;
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_MIZUBABA2_DAMAGE);
    this->timers[TIMER_CURRENT_ACTION] = 40;
    Actor_SetColorFilter(&this->dyna.actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 40);
}

void Boss05_WalkingHead_SetupFreeze(Boss05* this, PlayState* play) {
    this->actionFunc = Boss05_WalkingHead_Stunned;
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_MIZUBABA2_DAMAGE);
    this->timers[TIMER_CURRENT_ACTION] = 80;
    Actor_SetColorFilter(&this->dyna.actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 80);
}

/**
 * Turns the Bio Deku Baba blue, stops all animations and locks the Bio Deku Baba in place for either 40 or 80 frames
 * (depending on whether or not it was stunned via being frozen from an ice attack). Once the timer runs out, the Bio
 * Deku Baba will go back to being idle.
 */
void Boss05_WalkingHead_Stunned(Boss05* this, PlayState* play) {
    Math_ApproachZeroF(&this->dyna.actor.speed, 1.0f, 2.5f);

    if (this->timers[TIMER_CURRENT_ACTION] == 0) {
        Boss05_WalkingHead_SetupIdle(this, play);
    }
}

/**
 * Controls how fragments of a dead Bio Deku Baba should move and act. If the fragments are underwater, then they slowly
 * drift upwards for 50-80 frames before despawning. If the fragments are above water, they instead fly through the air
 * until they touch the ground.
 */
void Boss05_Fragment_Move(Boss05* this, PlayState* play) {
    Actor_MoveWithGravity(&this->dyna.actor);

    if (this->fragmentState == BIO_BABA_FRAGMENT_STATE_SPAWNED) {
        Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 20.0f, 50.0f, 40.0f,
                                UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_40);
        if (this->dyna.actor.bgCheckFlags & BGCHECKFLAG_WATER) {
            this->fragmentState = BIO_BABA_FRAGMENT_STATE_UNDERWATER;
        } else {
            this->fragmentState = BIO_BABA_FRAGMENT_STATE_ABOVE_WATER;
        }
    }

    if (this->fragmentState == BIO_BABA_FRAGMENT_STATE_UNDERWATER) {
        Math_ApproachF(&this->dyna.actor.velocity.y, 1.0f, 1.0f, 0.1f);
        Math_ApproachZeroF(&this->dyna.actor.speed, 0.5f, 0.5f);
        this->dyna.actor.shape.rot.x += this->fragmentAngularVelocity.x;
        this->dyna.actor.shape.rot.y += this->fragmentAngularVelocity.y;

        if (this->timers[TIMER_CURRENT_ACTION] == 0) {
            Actor_Kill(&this->dyna.actor);
        }
    } else {
        switch (this->fragmentState) {
            case BIO_BABA_FRAGMENT_STATE_ABOVE_WATER:
                this->dyna.actor.velocity.y = Rand_ZeroFloat(3.0f) + 3.0f;
                this->dyna.actor.speed = Rand_CenteredFloat(5.0f) + 5.0f;
                this->dyna.actor.world.rot.y = Rand_ZeroFloat(0x10000);
                this->dyna.actor.gravity = -1.0f;
                this->fragmentState = BIO_BABA_FRAGMENT_STATE_FLYING_THROUGH_AIR;
                break;

            case BIO_BABA_FRAGMENT_STATE_FLYING_THROUGH_AIR:
                Actor_MoveWithGravity(&this->dyna.actor);

                if (this->fragmentPos.y < (this->dyna.actor.floorHeight - 30.0f)) {
                    Actor_Kill(&this->dyna.actor);
                }
                break;

            default:
                break;
        }
    }
}

void Boss05_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    Boss05* this = (Boss05*)thisx;
    s16 i;

    this->frameCounter++;

    for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
        DECR(this->timers[i]);
    }

    DECR(this->damagedTimer);
    DECR(this->damagedFlashTimer);
    DECR(this->forceDetachTimer);
    DECR(this->drawDmgEffTimer);

    this->actionFunc(this, play);

    if (BIO_BABA_GET_TYPE(&this->dyna.actor) == BIO_BABA_TYPE_WALKING_HEAD) {
        Actor_MoveWithGravity(&this->dyna.actor);
        Matrix_RotateYS(this->knockbackAngle, MTXMODE_NEW);
        Matrix_MultVecZ(this->knockbackMagnitude, &this->knockbackVelocity);
        this->dyna.actor.world.pos.x += this->knockbackVelocity.x;
        this->dyna.actor.world.pos.z += this->knockbackVelocity.z;
        Math_ApproachZeroF(&this->knockbackMagnitude, 1.0f, 1.0f);
        Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 20.0f, 50.0f, 40.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_40);
        Boss05_WalkingHead_UpdateDamage(this, play);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->headCollider.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->headCollider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->headCollider.base);
        this->dyna.actor.shape.rot.y = this->dyna.actor.world.rot.y;
    }

    switch (this->drawDmgEffState) {
        case BIO_BABA_DRAW_DMGEFF_STATE_NONE:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
            this->drawDmgEffTimer = 0;
            this->drawDmgEffAlpha = 0.0f;
            break;

        case BIO_BABA_DRAW_DMGEFF_STATE_FIRE_INIT:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
            this->drawDmgEffTimer = 80;
            this->drawDmgEffAlpha = 1.0f;
            this->drawDmgEffState++;
            this->drawDmgEffScale = 0.0f;
            // fallthrough
        case BIO_BABA_DRAW_DMGEFF_STATE_FIRE_ACTIVE:
            if (this->drawDmgEffTimer == 0) {
                Math_ApproachZeroF(&this->drawDmgEffAlpha, 1.0f, 0.02f);
                if (this->drawDmgEffAlpha == 0.0f) {
                    this->drawDmgEffState = BIO_BABA_DRAW_DMGEFF_STATE_NONE;
                }
            } else {
                Math_ApproachF(&this->drawDmgEffScale, 1.0f, 0.1f, 0.5f);
            }
            break;

        case BIO_BABA_DRAW_DMGEFF_STATE_FROZEN_INIT:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
            this->drawDmgEffTimer = 80;
            this->drawDmgEffAlpha = 1.0f;
            this->drawDmgEffState++;
            this->drawDmgEffScale = 0.0f;
            this->drawDmgEffFrozenSteamScale = 2.0f;
            // fallthrough
        case BIO_BABA_DRAW_DMGEFF_STATE_FROZEN_ACTIVE:
            if (this->drawDmgEffTimer == 0) {
                Boss05_WalkingHead_Thaw(this, play);
                this->drawDmgEffState = BIO_BABA_DRAW_DMGEFF_STATE_NONE;
            } else {
                Math_ApproachF(&this->drawDmgEffScale, 1.0f, 1.0f, 0.25f);
                Math_ApproachF(&this->drawDmgEffFrozenSteamScale, 1.0f, 0.1f, 0.1f);
            }
            break;

        case BIO_BABA_DRAW_DMGEFF_STATE_LIGHT_ORB_INIT:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
            this->drawDmgEffTimer = 80;
            this->drawDmgEffAlpha = 1.0f;
            this->drawDmgEffState++;
            this->drawDmgEffScale = 0.0f;
            break;

        case BIO_BABA_DRAW_DMGEFF_STATE_LIGHT_ORB_ACTIVE:
            if (this->drawDmgEffTimer == 0) {
                Math_ApproachZeroF(&this->drawDmgEffScale, 1.0f, 0.03f);

                if (this->drawDmgEffScale == 0.0f) {
                    this->drawDmgEffState = BIO_BABA_DRAW_DMGEFF_STATE_NONE;
                    this->drawDmgEffAlpha = 0.0f;
                }
            } else {
                Math_ApproachF(&this->drawDmgEffScale, 1.0f, 0.5f, 0.5f);
            }
            break;

        default:
            break;
    }
}

static s8 sLimbIndexToLimbRotIndex[] = {
    BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_NONE,            // BIO_DEKU_BABA_LILY_PAD_LIMB_NONE
    BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_NONE,            // BIO_DEKU_BABA_LILY_PAD_LIMB_ROOTS
    BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_UPPER_STEM,      // BIO_DEKU_BABA_LILY_PAD_LIMB_UPPER_STEM
    BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_MIDDLE_STEM,     // BIO_DEKU_BABA_LILY_PAD_LIMB_MIDDLE_STEM
    BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_LOWER_STEM,      // BIO_DEKU_BABA_LILY_PAD_LIMB_LOWER_STEM
    BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_LEFT_UPPER_ARM,  // BIO_DEKU_BABA_LILY_PAD_LIMB_LEFT_UPPER_ARM
    BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_LEFT_LOWER_ARM,  // BIO_DEKU_BABA_LILY_PAD_LIMB_LEFT_LOWER_ARM
    BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_RIGHT_UPPER_ARM, // BIO_DEKU_BABA_LILY_PAD_LIMB_RIGHT_UPPER_ARM
    BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_RIGHT_LOWER_ARM, // BIO_DEKU_BABA_LILY_PAD_LIMB_RIGHT_LOWER_ARM
    BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_NONE,            // BIO_DEKU_BABA_LILY_PAD_LIMB_LEAF
    BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_NONE,            // Doesn't correspond to a real limb on the lily pad
};

s32 Boss05_LilyPadWithHead_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                            Actor* thisx) {
    Boss05* this = (Boss05*)thisx;
    s8 limbRotIndex;

    if (limbIndex == KREG(32)) {
        if ((this->frameCounter % 4) == 0) {
            *dList = NULL;
        }

        rot->x += KREG(33) * 0x100;
        rot->y += KREG(34) * 0x100;
        rot->z += KREG(35) * 0x100;
    }

    if ((BIO_BABA_GET_TYPE(&this->dyna.actor) == BIO_BABA_TYPE_NO_LEAF_LILY_PAD_WITH_HEAD) &&
        (limbIndex == BIO_DEKU_BABA_LILY_PAD_LIMB_LEAF)) {
        *dList = NULL;
    }

    if (limbIndex == BIO_DEKU_BABA_LILY_PAD_LIMB_UPPER_STEM) {
        rot->x += this->lilyPadWithHeadStemRotX;
    }

    limbRotIndex = sLimbIndexToLimbRotIndex[limbIndex];
    if (limbRotIndex > BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_NONE) {
        rot->x = rot->x + this->lilyPadWithHeadLimbRot[limbRotIndex].x;
        rot->y = rot->y + this->lilyPadWithHeadLimbRot[limbRotIndex].y;
        rot->z = rot->z + this->lilyPadWithHeadLimbRot[limbRotIndex].z;
    }

    return false;
}

void Boss05_LilyPad_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f sHeadOffset = { 0.0f, -1400.0f, 600.0f };
    Boss05* this = (Boss05*)thisx;
    MtxF mf;
    Vec3f upperStemPos;
    Vec3f lowerStemPos;

    if (limbIndex == BIO_DEKU_BABA_LILY_PAD_LIMB_LOWER_STEM) {
        Matrix_MultVec3f(&sHeadOffset, &this->headPos);

        if (this->actionFunc == Boss05_FallingHead_Fall) {
            Matrix_MultVec3f(&sHeadOffset, &this->dyna.actor.focus.pos);
        }

        Matrix_Get(&mf);
        Matrix_MtxFToYXZRot(&mf, &this->headRot, false);
        this->headRot.x += 0xF00;
    }

    if (limbIndex == BIO_DEKU_BABA_LILY_PAD_LIMB_UPPER_STEM) {
        Matrix_MultZero(&upperStemPos);
        Boss05_SetColliderSphere(BIO_BABA_LILY_PAD_COLLIDER_UPPER_STEM, &this->lilyPadCollider, &upperStemPos);
    }

    if (limbIndex == BIO_DEKU_BABA_LILY_PAD_LIMB_MIDDLE_STEM) {
        Matrix_MultVecY(-500.0f, &lowerStemPos);
        Boss05_SetColliderSphere(BIO_BABA_LILY_PAD_COLLIDER_MIDDLE_STEM, &this->lilyPadCollider, &lowerStemPos);

        if (this->actionFunc == Boss05_LilyPadWithHead_Move) {
            Matrix_MultVecY(1500.0f, &this->dyna.actor.focus.pos);
        }
    }
}

s32 Boss05_Head_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    return false;
}

Vec3f sBioDekuBabaHeadColliderPos;

void Boss05_Head_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f sHeadColliderOffset = { 1600.0f, -300.0f, 0.0f };
    static Vec3f sHeadOffset = { 700.0f, 0.0f, 0.0f };
    Boss05* this = (Boss05*)thisx;

    if (limbIndex == BIO_DEKU_BABA_HEAD_LIMB_BODY) {
        Matrix_MultVec3f(&sHeadColliderOffset, &sBioDekuBabaHeadColliderPos);
        Boss05_SetColliderSphere(BIO_BABA_HEAD_COLLIDER_HEAD, &this->headCollider, &sBioDekuBabaHeadColliderPos);

        if (BIO_BABA_GET_TYPE(&this->dyna.actor) == BIO_BABA_TYPE_WALKING_HEAD) {
            Matrix_MultVec3f(&sHeadOffset, &this->dyna.actor.focus.pos);
        }

        if (this->drawDmgEffState != BIO_BABA_DRAW_DMGEFF_STATE_NONE) {
            Matrix_MultVec3f(&sHeadOffset, &this->bodyPartsPos[BIO_BABA_BODYPART_HEAD]);
            if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FIRE) {
                this->bodyPartsPos[BIO_BABA_BODYPART_HEAD].y -= 15.0f;
            }
        }
    }
}

void Boss05_Head_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    Boss05* this = (Boss05*)thisx;

    if ((limbIndex == BIO_DEKU_BABA_HEAD_LIMB_LOWER_JAW) || (limbIndex == BIO_DEKU_BABA_HEAD_LIMB_UPPER_JAW)) {
        Matrix_Scale(this->lowerJawScaleXZ, 1.0f, this->lowerJawScaleXZ, MTXMODE_APPLY);
    }

    if (limbIndex == BIO_DEKU_BABA_HEAD_LIMB_BODY) {
        Matrix_Scale(this->headBodyScale, this->headBodyScale, this->headBodyScale, MTXMODE_APPLY);
    }

    if ((limbIndex >= BIO_DEKU_BABA_HEAD_LIMB_BACK_UPPER_LEG) && (limbIndex <= BIO_DEKU_BABA_HEAD_LIMB_LEAVES)) {
        Matrix_Scale(this->headLimbScale, this->headLimbScale, this->headLimbScale, MTXMODE_APPLY);
    }
}

s32 Boss05_LilyPad_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    if ((limbIndex >= BIO_DEKU_BABA_LILY_PAD_LIMB_MIDDLE_STEM) &&
        (limbIndex <= BIO_DEKU_BABA_LILY_PAD_LIMB_RIGHT_LOWER_ARM)) {
        *dList = NULL;
    }

    return false;
}

s32 Boss05_FallingHeadLilyPad_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                               Actor* thisx) {
    if ((limbIndex == BIO_DEKU_BABA_LILY_PAD_LIMB_ROOTS) || (limbIndex == BIO_DEKU_BABA_LILY_PAD_LIMB_UPPER_STEM) ||
        (limbIndex == BIO_DEKU_BABA_LILY_PAD_LIMB_LEAF)) {
        *dList = NULL;
    }

    return false;
}

void Boss05_FallingHeadLilyPad_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    Boss05* this = (Boss05*)thisx;

    if ((limbIndex >= BIO_DEKU_BABA_LILY_PAD_LIMB_MIDDLE_STEM) &&
        (limbIndex <= BIO_DEKU_BABA_LILY_PAD_LIMB_RIGHT_LOWER_ARM)) {
        Matrix_Scale(this->fallingHeadLilyPadLimbScale, this->fallingHeadLilyPadLimbScale,
                     this->fallingHeadLilyPadLimbScale, MTXMODE_APPLY);
    }
}

static BioDekuBabaHeadLimb sFragmentIndexToLimbIndex[BIO_BABA_TYPE_MAX - BIO_BABA_TYPE_FRAGMENT_BASE] = {
    BIO_DEKU_BABA_HEAD_LIMB_LOWER_JAW,            // BIO_BABA_TYPE_FRAGMENT_LOWER_JAW
    BIO_DEKU_BABA_HEAD_LIMB_UPPER_JAW,            // BIO_BABA_TYPE_FRAGMENT_UPPER_JAW
    BIO_DEKU_BABA_HEAD_LIMB_BODY,                 // BIO_BABA_TYPE_FRAGMENT_BODY
    BIO_DEKU_BABA_HEAD_LIMB_BACK_UPPER_LEG,       // BIO_BABA_TYPE_FRAGMENT_BACK_UPPER_LEG
    BIO_DEKU_BABA_HEAD_LIMB_BACK_LOWER_LEG,       // BIO_BABA_TYPE_FRAGMENT_BACK_LOWER_LEG
    BIO_DEKU_BABA_HEAD_LIMB_LEFT_UPPER_LEG,       // BIO_BABA_TYPE_FRAGMENT_LEFT_UPPER_LEG
    BIO_DEKU_BABA_HEAD_LIMB_LEFT_LOWER_LEG,       // BIO_BABA_TYPE_FRAGMENT_LEFT_LOWER_LEG
    BIO_DEKU_BABA_HEAD_LIMB_LEFT_LOWER_EYESTALK,  // BIO_BABA_TYPE_FRAGMENT_LEFT_LOWER_EYESTALK
    BIO_DEKU_BABA_HEAD_LIMB_LEFT_UPPER_EYESTALK,  // BIO_BABA_TYPE_FRAGMENT_LEFT_UPPER_EYESTALK
    BIO_DEKU_BABA_HEAD_LIMB_RIGHT_LOWER_EYESTALK, // BIO_BABA_TYPE_FRAGMENT_RIGHT_LOWER_EYESTALK
    BIO_DEKU_BABA_HEAD_LIMB_RIGHT_UPPER_EYESTALK, // BIO_BABA_TYPE_FRAGMENT_RIGHT_UPPER_EYESTALK
    BIO_DEKU_BABA_HEAD_LIMB_RIGHT_UPPER_LEG,      // BIO_BABA_TYPE_FRAGMENT_RIGHT_UPPER_LEG
    BIO_DEKU_BABA_HEAD_LIMB_RIGHT_LOWER_LEG,      // BIO_BABA_TYPE_FRAGMENT_RIGHT_LOWER_LEG
    BIO_DEKU_BABA_HEAD_LIMB_LEAVES,               // BIO_BABA_TYPE_FRAGMENT_LEAVES
};

s32 Boss05_Fragment_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                     Actor* thisx) {
    Boss05* this = (Boss05*)thisx;

    if (limbIndex != sFragmentIndexToLimbIndex[BIO_BABA_GET_FRAGMENT_INDEX(&this->dyna.actor)]) {
        *dList = NULL;
    } else if (this->fragmentState >= BIO_BABA_FRAGMENT_STATE_ABOVE_WATER) {
        rot->x += this->frameCounter * 0x3000;
        rot->y += this->frameCounter * 0x1A00;
        rot->z += this->frameCounter * 0x2000;
    }

    return false;
}

void Boss05_Fragment_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    Boss05* this = (Boss05*)thisx;

    if (limbIndex != sFragmentIndexToLimbIndex[BIO_BABA_GET_FRAGMENT_INDEX(&this->dyna.actor)]) {
        Matrix_MultZero(&this->fragmentPos);
    }
}

void Boss05_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    Boss05* this = (Boss05*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (this->actionFunc == Boss05_LilyPadWithHead_Move) {
        SkelAnime_DrawFlexOpa(play, this->lilyPadSkelAnime.skeleton, this->lilyPadSkelAnime.jointTable,
                              this->lilyPadSkelAnime.dListCount, Boss05_LilyPadWithHead_OverrideLimbDraw,
                              Boss05_LilyPad_PostLimbDraw, &this->dyna.actor);

        if ((this->damagedFlashTimer % 2) != 0) {
            POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 0, 0, 255, 900, 1099);
        }

        Matrix_Translate(this->headPos.x, this->headPos.y, this->headPos.z, MTXMODE_NEW);
        Matrix_RotateYS(this->headRot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->headRot.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->headRot.z, MTXMODE_APPLY);
        Matrix_Scale(this->dyna.actor.scale.x, this->dyna.actor.scale.y, this->dyna.actor.scale.z, MTXMODE_APPLY);

        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gBioDekuBabaHeadEyeFlashTexAnim));

        SkelAnime_DrawTransformFlexOpa(play, this->headSkelAnime.skeleton, this->headSkelAnime.jointTable,
                                       this->headSkelAnime.dListCount, Boss05_Head_OverrideLimbDraw,
                                       Boss05_Head_PostLimbDraw, Boss05_Head_TransformLimbDraw, &this->dyna.actor);
    } else if (BIO_BABA_GET_TYPE(&this->dyna.actor) == BIO_BABA_TYPE_LILY_PAD) {
        Matrix_Translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                         MTXMODE_NEW);
        Matrix_RotateYS(this->lilyPadRotY, MTXMODE_APPLY);
        Matrix_RotateXS(this->lilyPadRotX, MTXMODE_APPLY);
        Matrix_RotateYS(-this->lilyPadRotY, MTXMODE_APPLY);
        Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->dyna.actor.shape.rot.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->dyna.actor.shape.rot.z, MTXMODE_APPLY);
        Matrix_Scale(this->dyna.actor.scale.x, this->dyna.actor.scale.y, this->dyna.actor.scale.z, MTXMODE_APPLY);

        SkelAnime_DrawFlexOpa(play, this->lilyPadSkelAnime.skeleton, this->lilyPadSkelAnime.jointTable,
                              this->lilyPadSkelAnime.dListCount, Boss05_LilyPad_OverrideLimbDraw, NULL,
                              &this->dyna.actor);
    } else if (BIO_BABA_GET_TYPE(&this->dyna.actor) == BIO_BABA_TYPE_FALLING_HEAD) {
        SkelAnime_DrawTransformFlexOpa(play, this->lilyPadSkelAnime.skeleton, this->lilyPadSkelAnime.jointTable,
                                       this->lilyPadSkelAnime.dListCount, Boss05_FallingHeadLilyPad_OverrideLimbDraw,
                                       Boss05_LilyPad_PostLimbDraw, Boss05_FallingHeadLilyPad_TransformLimbDraw,
                                       &this->dyna.actor);

        Matrix_Translate(this->headPos.x, this->headPos.y, this->headPos.z, MTXMODE_NEW);
        Matrix_RotateYS(this->headRot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->headRot.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->headRot.z, MTXMODE_APPLY);
        Matrix_Scale(this->dyna.actor.scale.x, this->dyna.actor.scale.y, this->dyna.actor.scale.z, MTXMODE_APPLY);

        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gBioDekuBabaHeadEyeFlashTexAnim));

        SkelAnime_DrawTransformFlexOpa(play, this->headSkelAnime.skeleton, this->headSkelAnime.jointTable,
                                       this->headSkelAnime.dListCount, Boss05_Head_OverrideLimbDraw,
                                       Boss05_Head_PostLimbDraw, Boss05_Head_TransformLimbDraw, &this->dyna.actor);

        Actor_DrawDamageEffects(play, &this->dyna.actor, this->bodyPartsPos, BIO_BABA_BODYPART_MAX,
                                this->drawDmgEffScale, this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha,
                                this->drawDmgEffType);
    } else if (BIO_BABA_GET_TYPE(&this->dyna.actor) == BIO_BABA_TYPE_WALKING_HEAD) {
        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gBioDekuBabaHeadEyeFlashTexAnim));

        if ((this->damagedFlashTimer % 2) != 0) {
            POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 0, 0, 255, 900, 1099);
        }

        SkelAnime_DrawTransformFlexOpa(play, this->headSkelAnime.skeleton, this->headSkelAnime.jointTable,
                                       this->headSkelAnime.dListCount, Boss05_Head_OverrideLimbDraw,
                                       Boss05_Head_PostLimbDraw, Boss05_Head_TransformLimbDraw, &this->dyna.actor);

        Actor_DrawDamageEffects(play, &this->dyna.actor, this->bodyPartsPos, BIO_BABA_BODYPART_MAX,
                                this->drawDmgEffScale, this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha,
                                this->drawDmgEffType);
    } else if (BIO_BABA_GET_TYPE(&this->dyna.actor) >= BIO_BABA_TYPE_FRAGMENT_LOWER_JAW) {
        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gBioDekuBabaHeadEyeFlashTexAnim));

        SkelAnime_DrawFlexOpa(play, this->headSkelAnime.skeleton, this->headSkelAnime.jointTable,
                              this->headSkelAnime.dListCount, Boss05_Fragment_OverrideLimbDraw,
                              Boss05_Fragment_PostLimbDraw, &this->dyna.actor);
    }

    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}
