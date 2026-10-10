/*
 * File: z_en_pp.c
 * Overlay: ovl_En_Pp
 * Description: Hiploop
 */

#include "z_en_pp.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void EnPp_Init(Actor* thisx, PlayState* play);
void EnPp_Destroy(Actor* thisx, PlayState* play);
void EnPp_Update(Actor* thisx, PlayState* play);
void EnPp_Draw(Actor* thisx, PlayState* play);

void EnPp_SetupIdle(EnPp* this);
void EnPp_Idle(EnPp* this, PlayState* play);
void EnPp_SetupCharge(EnPp* this);
void EnPp_Charge(EnPp* this, PlayState* play);
void EnPp_SetupAttack(EnPp* this);
void EnPp_Attack(EnPp* this, PlayState* play);
void EnPp_SetupBounced(EnPp* this);
void EnPp_Bounced(EnPp* this, PlayState* play);
void EnPp_SetupRoar(EnPp* this);
void EnPp_Roar(EnPp* this, PlayState* play);
void EnPp_SetupJump(EnPp* this);
void EnPp_Jump(EnPp* this, PlayState* play);
void EnPp_StunnedOrFrozen(EnPp* this, PlayState* play);
void EnPp_Damaged(EnPp* this, PlayState* play);
void EnPp_SetupDead(EnPp* this, PlayState* play);
void EnPp_Dead(EnPp* this, PlayState* play);
void EnPp_Mask_Detach(EnPp* this, PlayState* play);
void EnPp_Fragment_SetupMove(EnPp* this);
void EnPp_Fragment_Move(EnPp* this, PlayState* play);

typedef enum {
    /* 0 */ EN_PP_COLLISION_RESULT_OK,
    /* 1 */ EN_PP_COLLISION_RESULT_ABOUT_TO_RUN_INTO_WALL,
    /* 2 */ EN_PP_COLLISION_RESULT_ABOUT_TO_RUN_OFF_LEDGE
} EnPpCollisionResult;

typedef enum {
    /*  0 */ EN_PP_ACTION_IDLE,
    /*  1 */ EN_PP_ACTION_CHARGE,
    /*  2 */ EN_PP_ACTION_ATTACK,
    /*  3 */ EN_PP_ACTION_BOUNCED,
    /*  4 */ EN_PP_ACTION_ROAR,
    /*  5 */ EN_PP_ACTION_JUMP,
    /*  6 */ EN_PP_ACTION_STUNNED_OR_FROZEN,
    /*  7 */ EN_PP_ACTION_MASK_DETACH,
    /*  8 */ EN_PP_ACTION_DAMAGED,
    /*  9 */ EN_PP_ACTION_DEAD,
    /* 10 */ EN_PP_ACTION_MASK_DEAD,
    /* 11 */ EN_PP_ACTION_SPAWN_BODY_PARTS,
    /* 12 */ EN_PP_ACTION_DONE_SPAWNING_BODY_PARTS,
    /* 13 */ EN_PP_ACTION_BODY_PART_MOVE
} EnPpAction;

typedef enum {
    /* 0 */ EN_PP_MASK_DETACH_STATE_START,
    /* 1 */ EN_PP_MASK_DETACH_STATE_FALL,
    /* 2 */ EN_PP_MASK_DETACH_STATE_DIE
} EnPpMaskDetachState;

static s32 sCurrentFragmentIndex = 0;

typedef enum {
    /* 0x0 */ EN_PP_DMGEFF_JUMP,                 // Forces the Hiploop to jump
    /* 0x1 */ EN_PP_DMGEFF_STUN,                 // Stuns the Hiploop
    /* 0x2 */ EN_PP_DMGEFF_FIRE,                 // Damages and sets the Hiploop on fire
    /* 0x3 */ EN_PP_DMGEFF_FREEZE,               // Damages and freezes the Hiploop in ice
    /* 0x4 */ EN_PP_DMGEFF_LIGHT_ORB,            // Damages and surrounds the Hiploop with light orbs
    /* 0x5 */ EN_PP_DMGEFF_ELECTRIC_STUN,        // Stuns and surrounds the Hiploop with electric sparks
    /* 0xC */ EN_PP_DMGEFF_KNOCK_OFF_MASK = 0xC, // Knocks off the Hiploop's mask or deals regular damage
    /* 0xD */ EN_PP_DMGEFF_HOOKSHOT,             // Pulls the Hiploop's mask to the player or deals regular damage
    /* 0xE */ EN_PP_DMGEFF_GORON_POUND,          // Knocks off the Hiploop's mask or forces it to jump
    /* 0xF */ EN_PP_DMGEFF_DAMAGE                // Deals regular damage
} EnPpDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, EN_PP_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, EN_PP_DMGEFF_DAMAGE),
    /* Horse trample  */ DMG_ENTRY(0, EN_PP_DMGEFF_JUMP),
    /* Explosives     */ DMG_ENTRY(1, EN_PP_DMGEFF_KNOCK_OFF_MASK),
    /* Zora boomerang */ DMG_ENTRY(1, EN_PP_DMGEFF_DAMAGE),
    /* Normal arrow   */ DMG_ENTRY(1, EN_PP_DMGEFF_DAMAGE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, EN_PP_DMGEFF_JUMP),
    /* Hookshot       */ DMG_ENTRY(1, EN_PP_DMGEFF_HOOKSHOT),
    /* Goron punch    */ DMG_ENTRY(1, EN_PP_DMGEFF_KNOCK_OFF_MASK),
    /* Sword          */ DMG_ENTRY(1, EN_PP_DMGEFF_DAMAGE),
    /* Goron pound    */ DMG_ENTRY(0, EN_PP_DMGEFF_GORON_POUND),
    /* Fire arrow     */ DMG_ENTRY(1, EN_PP_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(1, EN_PP_DMGEFF_FREEZE),
    /* Light arrow    */ DMG_ENTRY(2, EN_PP_DMGEFF_LIGHT_ORB),
    /* Goron spikes   */ DMG_ENTRY(1, EN_PP_DMGEFF_DAMAGE),
    /* Deku spin      */ DMG_ENTRY(0, EN_PP_DMGEFF_STUN),
    /* Deku bubble    */ DMG_ENTRY(1, EN_PP_DMGEFF_DAMAGE),
    /* Deku launch    */ DMG_ENTRY(2, EN_PP_DMGEFF_KNOCK_OFF_MASK),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, EN_PP_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(0, EN_PP_DMGEFF_ELECTRIC_STUN),
    /* Normal shield  */ DMG_ENTRY(0, EN_PP_DMGEFF_JUMP),
    /* Light ray      */ DMG_ENTRY(0, EN_PP_DMGEFF_JUMP),
    /* Thrown object  */ DMG_ENTRY(1, EN_PP_DMGEFF_DAMAGE),
    /* Zora punch     */ DMG_ENTRY(1, EN_PP_DMGEFF_DAMAGE),
    /* Spin attack    */ DMG_ENTRY(1, EN_PP_DMGEFF_DAMAGE),
    /* Sword beam     */ DMG_ENTRY(0, EN_PP_DMGEFF_JUMP),
    /* Normal Roll    */ DMG_ENTRY(0, EN_PP_DMGEFF_JUMP),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, EN_PP_DMGEFF_JUMP),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, EN_PP_DMGEFF_JUMP),
    /* Unblockable    */ DMG_ENTRY(0, EN_PP_DMGEFF_JUMP),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, EN_PP_DMGEFF_JUMP),
    /* Powder Keg     */ DMG_ENTRY(1, EN_PP_DMGEFF_KNOCK_OFF_MASK),
};

ActorProfile En_Pp_Profile = {
    /**/ ACTOR_EN_PP,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_PP,
    /**/ sizeof(EnPp),
    /**/ EnPp_Init,
    /**/ EnPp_Destroy,
    /**/ EnPp_Update,
    /**/ EnPp_Draw,
};

static ColliderJntSphElementInit sMaskColliderJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 0 }, 1 },
    },
};

static ColliderJntSphInit sMaskColliderJntSphInit = {
    {
        COL_MATERIAL_HARD,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sMaskColliderJntSphElementsInit),
    sMaskColliderJntSphElementsInit,
};

static ColliderJntSphElementInit sBodyColliderJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x04, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 0 }, 1 },
    },
};

static ColliderJntSphInit sBodyColliderJntSphInit = {
    {
        COL_MATERIAL_HARD,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sBodyColliderJntSphElementsInit),
    sBodyColliderJntSphElementsInit,
};

static ColliderQuadInit sQuadInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x04, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL | ATELEM_UNK7,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static Color_RGBA8 sDustPrimColor = { 60, 50, 20, 255 };

static Color_RGBA8 sDustEnvColor = { 40, 30, 30, 255 };

void EnPp_Init(Actor* thisx, PlayState* play) {
    EnPp* this = (EnPp*)thisx;
    EffectBlureInit1 blureInit;

    this->actor.attentionRangeType = ATTENTION_RANGE_4;
    this->actor.colChkInfo.mass = 60;
    this->actor.colChkInfo.health = 3;
    this->actor.colChkInfo.damageTable = &sDamageTable;
    SkelAnime_InitFlex(play, &this->skelAnime, &gHiploopSkel, &gHiploopIdleAnim, this->jointTable, this->morphTable,
                       HIPLOOP_LIMB_MAX);

    if (EN_PP_GET_ATTACK_RANGE(&this->actor) == 0) {
        this->attackRange = 200.0f;
    } else {
        this->attackRange = EN_PP_GET_ATTACK_RANGE(&this->actor) * 40.0f;
    }

    this->chargesInStraightLines = EN_PP_DOES_CHARGE_IN_STRAIGHT_LINES(&this->actor);
    if (this->chargesInStraightLines) {
        this->actor.params &= 1;
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    }

    if (this->actor.params < 0) {
        this->actor.params = EN_PP_TYPE_MASKED;
    }

    if (EN_PP_GET_TYPE(&this->actor) >= EN_PP_TYPE_FRAGMENT_BASE) {
        this->fragmentIndex = sCurrentFragmentIndex;
        sCurrentFragmentIndex++;
        this->actor.shape.rot.y = this->actor.world.rot.y;
        Actor_SetScale(&this->actor, 0.03f);
        EnPp_Fragment_SetupMove(this);
    } else {
        Collider_InitAndSetJntSph(play, &this->maskCollider, &this->actor, &sMaskColliderJntSphInit,
                                  this->maskColliderElements);
        Collider_InitAndSetJntSph(play, &this->bodyCollider, &this->actor, &sBodyColliderJntSphInit,
                                  this->bodyColliderElements);

        this->bodyCollider.elements[0].dim.scale = 1.0f;
        if (EN_PP_GET_TYPE(&this->actor) > EN_PP_TYPE_MASKED) {
            this->actor.hintId = TATL_HINT_ID_HIPLOOP;
            this->maskColliderElements[0].base.atElemFlags &= ~ATELEM_ON;
            this->maskColliderElements[0].base.acElemFlags &= ~ACELEM_ON;
            this->maskColliderElements[0].base.ocElemFlags &= ~OCELEM_ON;
            this->maskCollider.base.colMaterial = COL_MATERIAL_HIT2;
            this->maskCollider.elements[0].dim.modelSphere.radius = 42;
            this->maskCollider.elements[0].dim.scale = 1.0f;
            this->maskCollider.elements[0].dim.modelSphere.center.x = 400;
            this->maskCollider.elements[0].dim.modelSphere.center.y = -400;
            this->bodyCollider.elements[0].dim.modelSphere.radius = 20;
            this->bodyCollider.elements[0].dim.scale = 1.0f;
            this->bodyCollider.elements[0].dim.modelSphere.center.x = 400;
            this->bodyCollider.elements[0].dim.modelSphere.center.y = -400;
        } else {
            this->actor.hintId = TATL_HINT_ID_MASKED_HIPLOOP;
            this->maskCollider.elements[0].dim.modelSphere.radius = 10;
            this->maskCollider.elements[0].dim.scale = 1.0f;
            this->maskCollider.elements[0].dim.modelSphere.center.x = 1000;
            this->maskCollider.elements[0].dim.modelSphere.center.y = -500;
            this->bodyCollider.elements[0].dim.modelSphere.radius = 25;
            this->bodyCollider.elements[0].dim.scale = 1.0f;
            this->bodyCollider.elements[0].dim.modelSphere.center.x = 400;
            this->bodyCollider.elements[0].dim.modelSphere.center.y = -400;
            this->bodyColliderElements[0].base.acElemFlags |= ACELEM_HOOKABLE;
            this->maskCollider.elements[0].base.atDmgInfo.damage = 0x10;
        }

        Collider_InitQuad(play, &this->hornCollider);
        Collider_SetQuad(play, &this->hornCollider, &this->actor, &sQuadInit);

        blureInit.p1StartColor[0] = blureInit.p1StartColor[1] = blureInit.p1StartColor[2] = blureInit.p1StartColor[3] =
            blureInit.p2StartColor[0] = blureInit.p2StartColor[1] = blureInit.p2StartColor[2] =
                blureInit.p1EndColor[0] = blureInit.p1EndColor[1] = blureInit.p1EndColor[2] = blureInit.p2EndColor[0] =
                    blureInit.p2EndColor[1] = blureInit.p2EndColor[2] = 255;
        blureInit.p2StartColor[3] = 64;
        blureInit.p1EndColor[3] = blureInit.p2EndColor[3] = 0;
        blureInit.elemDuration = 8;
        blureInit.unkFlag = false;
        blureInit.calcMode = 2;
        Effect_Add(play, &this->blureIndex, EFFECT_BLURE1, 0, 0, &blureInit);

        Actor_SetScale(&this->actor, 0.03f);
        this->floorPolyForCircleShadow = NULL;
        this->actor.gravity = -3.0f;

        if ((EN_PP_GET_TYPE(&this->actor) == EN_PP_TYPE_MASKED) ||
            (EN_PP_GET_TYPE(&this->actor) == EN_PP_TYPE_NO_MASK)) {
            EnPp_SetupIdle(this);
        } else {
            EnPp_SetupJump(this);
        }
    }
}

void EnPp_Destroy(Actor* thisx, PlayState* play) {
    EnPp* this = (EnPp*)thisx;

    if (EN_PP_GET_TYPE(&this->actor) < EN_PP_TYPE_FRAGMENT_BASE) {
        Collider_DestroyJntSph(play, &this->maskCollider);
        Collider_DestroyJntSph(play, &this->bodyCollider);
        Collider_DestroyQuad(play, &this->hornCollider);
    }

    Effect_Destroy(play, this->blureIndex);
}

/**
 * This function converts the Hiploop's world position to polar coordinates,
 * converts those polar coordinates back into Cartesian coordinates, and then
 * checks to see if this doubly-converted position is similar to the original
 * world position. This seemingly always returns true, and logically it should
 * always return true unless there are errors in how math is implemented.
 */
s32 EnPp_PointlessPosCheck(EnPp* this) {
    f32 diffX = this->actor.home.pos.x - this->actor.world.pos.x;
    f32 diffZ = this->actor.home.pos.z - this->actor.world.pos.z;
    f32 radius;
    s16 angle;
    Vec3f nearDuplicateWorldPos;

    // This converts the vector between the Hiploop's home and world positions into polar
    // coordinates. In other words, if the home position is at the origin, then the world
    // position can be found at (radius, angle).
    radius = sqrtf(SQ(diffX) + SQ(diffZ));
    angle = Math_Vec3f_Yaw(&this->actor.home.pos, &this->actor.world.pos);

    // This takes the polar coordinates we found earlier and converts them *back* into
    // Cartesian coordinates by adding them back to the home position. This ends up creating
    // a near-duplicate of the Hiploop's original world position.
    Math_Vec3f_Copy(&nearDuplicateWorldPos, &this->actor.home.pos);
    nearDuplicateWorldPos.x += Math_SinS(angle) * radius;
    nearDuplicateWorldPos.z += Math_CosS(angle) * radius;

    Math_ApproachF(&this->actor.world.pos.x, nearDuplicateWorldPos.x, 0.3f, 2.0f);
    Math_ApproachF(&this->actor.world.pos.z, nearDuplicateWorldPos.z, 0.3f, 2.0f);

    // This computes the difference between the Hiploop's original world position and the
    // near-duplicate position created earlier. Since the two positions are almost identical,
    // the differences are tiny.
    diffX = this->actor.world.pos.x - nearDuplicateWorldPos.x;
    diffZ = this->actor.world.pos.z - nearDuplicateWorldPos.z;

    // Since the two differences are always tiny, this is always true.
    if (sqrtf(SQ(diffX) + SQ(diffZ)) < 2.0f) {
        this->actor.world.pos.x = nearDuplicateWorldPos.x;
        this->actor.world.pos.z = nearDuplicateWorldPos.z;
        return true;
    }

    return false;
}

/**
 * Spawns dust behind the Hiploop's back feet.
 */
void EnPp_SpawnDust(EnPp* this, PlayState* play) {
    s32 i;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    for (i = 0; i < ARRAY_COUNT(this->backFootPos); i++) {
        velocity.x = Rand_CenteredFloat(2.0f);
        velocity.y = Rand_ZeroFloat(2.0f) + 1.0f;
        velocity.z = Rand_CenteredFloat(2.0f);

        accel.y = -0.1f;
        accel.z = 0.0f;
        accel.x = 0.0f;

        pos.x = this->backFootPos[i].x + Rand_CenteredFloat(10.0f);
        pos.y = Rand_ZeroFloat(3.0f) + this->actor.floorHeight + 1.0f;
        pos.z = this->backFootPos[i].z + Rand_CenteredFloat(10.0f);

        func_800B0EB0(play, &pos, &velocity, &accel, &sDustPrimColor, &sDustEnvColor, (Rand_ZeroFloat(50.0f) + 60.0f),
                      30, (Rand_ZeroFloat(5.0f) + 20.0f));
    }
}

typedef enum {
    /*  0 */ EN_PP_ANIM_IDLE,
    /*  1 */ EN_PP_ANIM_WALK,
    /*  2 */ EN_PP_ANIM_WIND_UP,
    /*  3 */ EN_PP_ANIM_CHARGE,
    /*  4 */ EN_PP_ANIM_ATTACK,
    /*  5 */ EN_PP_ANIM_DAMAGE,
    /*  6 */ EN_PP_ANIM_ROAR,
    /*  7 */ EN_PP_ANIM_TURN_TO_FACE_PLAYER,
    /*  8 */ EN_PP_ANIM_JUMP,
    /*  9 */ EN_PP_ANIM_LAND,
    /* 10 */ EN_PP_ANIM_MAX
} EnPpAnimation;

static AnimationHeader* sAnimations[EN_PP_ANIM_MAX] = {
    &gHiploopIdleAnim,   // EN_PP_ANIM_IDLE
    &gHiploopWalkAnim,   // EN_PP_ANIM_WALK
    &gHiploopWindUpAnim, // EN_PP_ANIM_WIND_UP
    &gHiploopChargeAnim, // EN_PP_ANIM_CHARGE
    &gHiploopAttackAnim, // EN_PP_ANIM_ATTACK
    &gHiploopDamageAnim, // EN_PP_ANIM_DAMAGE
    &gHiploopRoarAnim,   // EN_PP_ANIM_ROAR
    &gHiploopWalkAnim,   // EN_PP_ANIM_TURN_TO_FACE_PLAYER
    &gHiploopWindUpAnim, // EN_PP_ANIM_JUMP
    &gHiploopWalkAnim,   // EN_PP_ANIM_LAND
};

static u8 sAnimationModes[EN_PP_ANIM_MAX] = {
    ANIMMODE_LOOP, // EN_PP_ANIM_IDLE
    ANIMMODE_LOOP, // EN_PP_ANIM_WALK
    ANIMMODE_ONCE, // EN_PP_ANIM_WIND_UP
    ANIMMODE_LOOP, // EN_PP_ANIM_CHARGE
    ANIMMODE_ONCE, // EN_PP_ANIM_ATTACK
    ANIMMODE_LOOP, // EN_PP_ANIM_DAMAGE
    ANIMMODE_LOOP, // EN_PP_ANIM_ROAR
    ANIMMODE_LOOP, // EN_PP_ANIM_TURN_TO_FACE_PLAYER
    ANIMMODE_ONCE, // EN_PP_ANIM_JUMP
    ANIMMODE_ONCE, // EN_PP_ANIM_LAND
};

void EnPp_ChangeAnim(EnPp* this, s32 animIndex) {
    f32 morphFrames = -10.0f;
    f32 playSpeed;
    f32 startFrame;

    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[this->animIndex]);

    if (this->animIndex >= EN_PP_ANIM_WIND_UP) {
        morphFrames = 0.0f;
    }

    playSpeed = 1.0f;
    if (this->animIndex == EN_PP_ANIM_TURN_TO_FACE_PLAYER) {
        playSpeed = 2.0f;
    }

    startFrame = 0.0f;
    if (this->action == EN_PP_ACTION_BODY_PART_MOVE) {
        startFrame = this->animEndFrame;
    }

    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], playSpeed, startFrame, this->animEndFrame,
                     sAnimationModes[this->animIndex], morphFrames);
}

void EnPp_PlaySfxForAnimation(EnPp* this) {
    if (this->action != EN_PP_ACTION_STUNNED_OR_FROZEN) {
        switch (this->animIndex) {
            case EN_PP_ANIM_WALK:
            case EN_PP_ANIM_TURN_TO_FACE_PLAYER:
                if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_HIPLOOP_FOOTSTEP);
                }
                break;

            case EN_PP_ANIM_WIND_UP:
                Actor_PlaySfx(&this->actor, NA_SE_EN_HIPLOOP_FOOT - SFX_FLAG);
                break;

            case EN_PP_ANIM_CHARGE:
                Actor_PlaySfx(&this->actor, NA_SE_EN_HIPLOOP_RUN - SFX_FLAG);
                break;

            case EN_PP_ANIM_ATTACK:
                if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_HIPLOOP_PAUSE);
                }
                break;

            case EN_PP_ANIM_ROAR:
                if (Animation_OnFrame(&this->skelAnime, 6.0f)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_HIPLOOP_PAUSE);
                }
                break;

            default:
                break;
        }
    }
}

/**
 * Checks to see if the Hiploop is about to walk off a ledge or into a wall.
 */
s32 EnPp_CheckCollision(EnPp* this, PlayState* play) {
    static s16 sLedgeCheckAngles[] = { 0x0000, 0x1000, 0xF000 };
    s16 angle;
    s32 i;

    angle = this->actor.world.rot.y;
    if (this->action == EN_PP_ACTION_DAMAGED) {
        angle = this->targetRotY;
    }

    Math_Vec3f_Copy(&this->wallCheckPos, &this->actor.world.pos);
    for (i = 0; i < ARRAY_COUNT(this->ledgeCheckPos); i++) {
        Math_Vec3f_Copy(&this->ledgeCheckPos[i], &this->actor.world.pos);
        this->ledgeCheckPos[i].x += Math_SinS(sLedgeCheckAngles[i] + angle) * 70.0f;
        this->ledgeCheckPos[i].y = this->actor.floorHeight - 10.0f;
        this->ledgeCheckPos[i].z += Math_CosS(sLedgeCheckAngles[i] + angle) * 70.0f;
        if (!BgCheck_SphVsFirstPoly(&play->colCtx, &this->ledgeCheckPos[i], 20.0f)) {
            return EN_PP_COLLISION_RESULT_ABOUT_TO_RUN_OFF_LEDGE;
        }
    }

    this->wallCheckPos.x += Math_SinS(angle) * 40.0f;
    this->wallCheckPos.y = this->actor.world.pos.y + 20.0f;
    if (this->chargesInStraightLines) {
        // This prevents the Hiploops in Woodfall from acting strangely when they move from
        // a wooden platform onto one of the ramps; without this, they'll suddenly slow down
        // and start roaring when they try to climb the small lip.
        this->wallCheckPos.y += 20.0f;
    }

    this->wallCheckPos.z += Math_CosS(angle) * 40.0f;
    if (BgCheck_SphVsFirstWall(&play->colCtx, &this->wallCheckPos, 10.0f)) {
        return EN_PP_COLLISION_RESULT_ABOUT_TO_RUN_INTO_WALL;
    }

    return EN_PP_COLLISION_RESULT_OK;
}

void EnPp_SetupIdle(EnPp* this) {
    this->action = EN_PP_ACTION_IDLE;
    this->actionFunc = EnPp_Idle;
}

/**
 * Alternates between stopping in place, spinning around in random directions, and walking forward.
 */
void EnPp_Idle(EnPp* this, PlayState* play) {
    Vec3f posToLookAt;

    SkelAnime_Update(&this->skelAnime);
    if (this->hasBeenDamaged || (this->actor.xzDistToPlayer < this->attackRange)) {
        EnPp_SetupCharge(this);
    } else {
        if ((this->timer == 0) && (this->secondaryTimer == 0)) {
            this->secondaryTimer = Rand_ZeroFloat(20.0f) + 20.0f;
            Math_Vec3f_Copy(&posToLookAt, &this->actor.home.pos);
            posToLookAt.x += Rand_CenteredFloat(50.0f);
            posToLookAt.z += Rand_CenteredFloat(50.0f);
            this->targetRotY = Math_Vec3f_Yaw(&this->actor.world.pos, &posToLookAt);
            this->actor.speed = 0.0f;
            if (this->animIndex != EN_PP_ANIM_IDLE) {
                EnPp_ChangeAnim(this, EN_PP_ANIM_IDLE);
            }
        }

        if (this->secondaryTimer == 1) {
            this->timer = Rand_ZeroFloat(40.0f) + 40.0f;
        }

        if (EnPp_CheckCollision(this, play) != EN_PP_COLLISION_RESULT_OK) {
            this->timer = 0;
            if (this->animIndex != EN_PP_ANIM_WALK) {
                EnPp_ChangeAnim(this, EN_PP_ANIM_WALK);
            }

            Math_SmoothStepToS(&this->actor.world.rot.y, this->targetRotY, 1, 0x258, 0);
        } else if ((this->secondaryTimer == 0) && (this->timer != 0)) {
            if (this->animIndex == EN_PP_ANIM_IDLE) {
                EnPp_ChangeAnim(this, EN_PP_ANIM_WALK);
            }

            if ((this->maskBounceAngularVelocity < 0x64) &&
                (fabsf(this->actor.world.rot.y - this->targetRotY) < 100.0f)) {
                Math_ApproachF(&this->actor.speed, 1.0f, 0.3f, 1.0f);
            }

            Math_SmoothStepToS(&this->actor.world.rot.y, this->targetRotY, 1, this->maskBounceAngularVelocity + 0x258,
                               0);
            Math_SmoothStepToS(&this->maskBounceAngularVelocity, 0, 1, 0x1F4, 0);
        }
    }
}

void EnPp_SetupCharge(EnPp* this) {
    this->secondaryTimer = this->timer = 0;
    this->actionVar.isCharging = false;
    EnPp_ChangeAnim(this, EN_PP_ANIM_TURN_TO_FACE_PLAYER);
    this->action = EN_PP_ACTION_CHARGE;
    this->actionFunc = EnPp_Charge;
}

/**
 * Charges either directly at the player or in a straight line, depending on the type of Hiploop.
 */
void EnPp_Charge(EnPp* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    s32 yawDiff;
    Vec3f distanceFromWorldPos;

    SkelAnime_Update(&this->skelAnime);

    if (!this->actionVar.isCharging || (this->animIndex == EN_PP_ANIM_WIND_UP)) {
        this->targetRotY = this->actor.yawTowardsPlayer;
        if (this->chargesInStraightLines) {
            yawDiff = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.home.rot.y));
            this->targetRotY = this->actor.home.rot.y + 0x8000;
            if (yawDiff < 0x3000) {
                this->targetRotY = this->actor.home.rot.y;
            }

            Math_Vec3f_Copy(&this->targetPos, &this->actor.world.pos);
            Matrix_RotateYS(this->targetRotY, MTXMODE_NEW);
            Matrix_MultVecZ(300.0f, &distanceFromWorldPos);
            this->targetPos.x += distanceFromWorldPos.x;
            this->targetPos.z += distanceFromWorldPos.z;
        }

        Math_SmoothStepToS(&this->actor.world.rot.y, this->targetRotY, 1, this->maskBounceAngularVelocity + 0x7D0, 0);
    }

    Math_SmoothStepToS(&this->maskBounceAngularVelocity, 0, 1, 0x1F4, 0);
    if (!this->actionVar.isCharging) {
        Math_ApproachZeroF(&this->actor.speed, 0.5f, 1.0f);
        if (fabsf(this->actor.world.rot.y - this->targetRotY) < 100.0f) {
            if (this->chargesInStraightLines) {
                this->actor.world.rot.y = this->targetRotY;
            }

            this->actionVar.isCharging = true;
        }
    } else if (this->animIndex == EN_PP_ANIM_TURN_TO_FACE_PLAYER) {
        EnPp_ChangeAnim(this, EN_PP_ANIM_WIND_UP);
    } else if (this->animIndex == EN_PP_ANIM_WIND_UP) {
        if (curFrame >= this->animEndFrame) {
            this->chargeAndBounceSpeed = 14.0f;
            this->timer = 20;
            EnPp_ChangeAnim(this, EN_PP_ANIM_CHARGE);
        }
    } else if (this->animIndex == EN_PP_ANIM_CHARGE) {
        if (EnPp_CheckCollision(this, play) != EN_PP_COLLISION_RESULT_OK) {
            this->actor.speed = 0.0f;
            EnPp_SetupRoar(this);
            return;
        }

        if (!this->chargesInStraightLines) {
            Math_ApproachF(&this->actor.speed, 10.0f, 0.3f, 1.0f);
        } else {
            Math_ApproachF(&this->actor.world.pos.x, this->targetPos.x, 0.5f,
                           fabsf(Math_SinS(this->targetRotY) * this->chargeAndBounceSpeed));
            Math_ApproachF(&this->actor.world.pos.z, this->targetPos.z, 0.5f,
                           fabsf(Math_CosS(this->targetRotY) * this->chargeAndBounceSpeed));
            if (this->timer < 10) {
                Math_ApproachZeroF(&this->chargeAndBounceSpeed, 0.3f, 0.2f);
            }
        }

        if (this->timer == 0) {
            EnPp_SetupRoar(this);
            return;
        }

        if (!(this->maskCollider.base.atFlags & AT_BOUNCED) && !(this->bodyCollider.base.atFlags & AT_BOUNCED)) {
            if ((this->maskCollider.base.atFlags & AT_HIT) || (this->bodyCollider.base.atFlags & AT_HIT)) {
                EnPp_SetupAttack(this);
                return;
            }
        } else if (this->chargesInStraightLines) {
            EnPp_SetupBounced(this);
            return;
        } else {
            if (EN_PP_GET_TYPE(&this->actor) != EN_PP_TYPE_MASKED) {
                this->actor.speed *= -1.0f;
            } else {
                this->actor.speed *= -0.5f;
            }

            EnPp_SetupRoar(this);
            return;
        }
    }

    if (((this->animIndex == EN_PP_ANIM_WIND_UP) || (this->animIndex == EN_PP_ANIM_CHARGE)) &&
        (this->secondaryTimer == 0)) {
        EnPp_SpawnDust(this, play);
        this->secondaryTimer = 3;
    }
}

void EnPp_SetupAttack(EnPp* this) {
    EnPp_ChangeAnim(this, EN_PP_ANIM_ATTACK);
    this->hornColliderOn = true;
    this->action = EN_PP_ACTION_ATTACK;
    this->actor.speed = 0.0f;
    this->actionFunc = EnPp_Attack;
}

void EnPp_Attack(EnPp* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    SkelAnime_Update(&this->skelAnime);
    if (curFrame >= this->animEndFrame) {
        EnPp_SetupCharge(this);
    }
}

void EnPp_SetupBounced(EnPp* this) {
    Vec3f distanceFromWorldPos;
    s32 yawDiff = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.home.rot.y));

    this->targetRotY = this->actor.home.rot.y + 0x8000;
    if (yawDiff < 0x3000) {
        this->targetRotY = this->actor.home.rot.y;
    }

    Math_Vec3f_Copy(&this->targetPos, &this->actor.world.pos);
    Matrix_RotateYS(this->targetRotY, MTXMODE_NEW);
    Matrix_MultVecZ(-200.0f, &distanceFromWorldPos);
    this->timer = 10;
    this->targetPos.x += distanceFromWorldPos.x;
    this->targetPos.z += distanceFromWorldPos.z;
    EnPp_ChangeAnim(this, EN_PP_ANIM_DAMAGE);
    this->actor.speed = 0.0f;
    this->action = EN_PP_ACTION_BOUNCED;
    this->chargeAndBounceSpeed = 14.0f;
    this->actionFunc = EnPp_Bounced;
}

/**
 * Pushes the Hiploop back for a half-second, then forces it to charge again.
 */
void EnPp_Bounced(EnPp* this, PlayState* play) {
    Math_ApproachF(&this->actor.world.pos.x, this->targetPos.x, 0.5f,
                   fabsf(Math_SinS(this->targetRotY) * this->chargeAndBounceSpeed));
    Math_ApproachF(&this->actor.world.pos.z, this->targetPos.z, 0.5f,
                   fabsf(Math_CosS(this->targetRotY) * this->chargeAndBounceSpeed));
    Math_ApproachZeroF(&this->chargeAndBounceSpeed, 0.3f, 0.2f);
    if ((this->timer == 0) && (EnPp_PointlessPosCheck(this))) {
        EnPp_SetupCharge(this);
    }
}

void EnPp_SetupRoar(EnPp* this) {
    this->actionVar.hasDoneFirstRoar = false;
    this->timer = 20;
    this->secondaryTimer = 0;
    this->chargeAndBounceSpeed = 14.0f;
    EnPp_ChangeAnim(this, EN_PP_ANIM_ROAR);
    this->action = EN_PP_ACTION_ROAR;
    this->actionFunc = EnPp_Roar;
}

/**
 * Slows the Hiploop down and makes it do two roars. Afterwards, it will make the Hiploop charge again.
 */
void EnPp_Roar(EnPp* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame = this->skelAnime.curFrame;

    SkelAnime_Update(&this->skelAnime);

    if (EnPp_CheckCollision(this, play) == EN_PP_COLLISION_RESULT_ABOUT_TO_RUN_OFF_LEDGE) {
        this->actor.speed = 0.0f;
        this->chargeAndBounceSpeed = 0.0f;
    }

    if (!this->chargesInStraightLines) {
        if ((fabsf(this->actor.home.pos.y - this->actor.world.pos.y) > 100.0f) &&
            (fabsf(this->actor.floorHeight - player->actor.floorHeight) > 100.0f)) {
            EnPp_SetupDead(this, play);
            return;
        }
    } else if (this->chargeAndBounceSpeed) {
        Math_ApproachF(&this->actor.world.pos.x, this->targetPos.x, 0.5f,
                       fabsf(Math_SinS(this->targetRotY) * this->chargeAndBounceSpeed));
        Math_ApproachF(&this->actor.world.pos.z, this->targetPos.z, 0.5f,
                       fabsf(Math_CosS(this->targetRotY) * this->chargeAndBounceSpeed));
        if (this->timer < 10) {
            Math_ApproachZeroF(&this->chargeAndBounceSpeed, 0.3f, 0.2f);
        }
    }

    Math_ApproachZeroF(&this->actor.speed, 0.5f, 1.0f);
    if ((this->actor.speed > 0.3f) && (this->secondaryTimer == 0)) {
        EnPp_SpawnDust(this, play);
        this->secondaryTimer = 3;
    }

    if (!this->actionVar.hasDoneFirstRoar && (curFrame >= this->animEndFrame)) {
        this->skelAnime.startFrame = 6.0f;
        this->actionVar.hasDoneFirstRoar = true;
    }

    if (this->timer == 0) {
        EnPp_SetupCharge(this);
    }
}

void EnPp_SetupJump(EnPp* this) {
    s32 yawDiff;

    this->secondaryTimer = 0;
    this->actionVar.hasLandedFromJump = false;
    this->timer = this->secondaryTimer;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 20.0f;
    this->actor.gravity = -3.0f;
    EnPp_ChangeAnim(this, EN_PP_ANIM_JUMP);

    if (this->chargesInStraightLines) {
        yawDiff = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.home.rot.y));
        this->actor.world.rot.y = this->actor.home.rot.y + 0x8000;
        if (yawDiff < 0x3000) {
            this->actor.world.rot.y = this->actor.home.rot.y;
        }

        this->targetRotY = this->actor.world.rot.y;
    }

    this->actionVar.hasLandedFromJump = false;
    this->action = EN_PP_ACTION_JUMP;
    this->actionFunc = EnPp_Jump;
}

/**
 * Makes the Hiploop jump straight up. Once it lands, the Hiploop will be forced to be idle.
 */
void EnPp_Jump(EnPp* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    s32 yawDiff;

    SkelAnime_Update(&this->skelAnime);

    if (this->chargesInStraightLines) {
        yawDiff = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.home.rot.y));
        this->targetRotY = this->actor.home.rot.y + 0x8000;
        if (yawDiff < 0x3000) {
            this->targetRotY = this->actor.home.rot.y;
        }

        EnPp_PointlessPosCheck(this);
    } else {
        this->targetRotY = this->actor.yawTowardsPlayer;
    }

    Math_SmoothStepToS(&this->actor.world.rot.y, this->targetRotY, 1, 0x1388, 0);
    if (!this->actionVar.hasLandedFromJump) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->actionVar.hasLandedFromJump = true;
            EnPp_ChangeAnim(this, EN_PP_ANIM_LAND);
        }
    } else if (curFrame >= this->animEndFrame) {
        EnPp_SetupIdle(this);
    }
}

void EnPp_SetupStunnedOrFrozen(EnPp* this) {
    if (((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
         (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) &&
        (this->drawDmgEffTimer == 0)) {
        this->drawDmgEffTimer = 0;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    }

    this->actor.speed = 0.0f;
    this->action = EN_PP_ACTION_STUNNED_OR_FROZEN;
    this->actionFunc = EnPp_StunnedOrFrozen;
}

/**
 * Locks the Hiploop in place until the draw damage effect timer or the stun timer reaches 0.
 * When the stun wears off, this forces the Hiploop to charge.
 */
void EnPp_StunnedOrFrozen(EnPp* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) {
        if ((this->drawDmgEffTimer != 0) && (this->drawDmgEffTimer < 60)) {
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
        }
    }

    if ((this->secondaryTimer == 0) && (this->drawDmgEffTimer == 0)) {
        if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
            (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) {
            Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, EN_PP_BODYPART_MAX, 2, 0.7f, 0.4f);
            this->drawDmgEffTimer = 0;
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        }

        EnPp_SetupCharge(this);
    }
}

void EnPp_SetupDamaged(EnPp* this, PlayState* play) {
    Vec3f temp;
    s32 pad;
    s32 yawDiff;

    if (!this->chargesInStraightLines) {
        Matrix_RotateYS(this->actor.yawTowardsPlayer, MTXMODE_NEW);
        Matrix_MultVecZ(-30.0f, &temp);
        Math_Vec3f_Copy(&this->damagedVelocity, &temp);
    } else {
        yawDiff = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.home.rot.y));
        this->targetRotY = this->actor.home.rot.y + 0x8000;
        if (yawDiff < 0x3000) {
            this->targetRotY = this->actor.home.rot.y;
        }

        Math_Vec3f_Copy(&this->targetPos, &this->actor.world.pos);
        Matrix_RotateYS(this->targetRotY, MTXMODE_NEW);
        Matrix_MultVecZ(-200.0f, &temp);
        this->timer = 10;
        this->targetPos.x += temp.x;
        this->targetPos.z += temp.z;
    }

    if (((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
         (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) &&
        (this->drawDmgEffTimer != 0)) {
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, EN_PP_BODYPART_MAX, 2, 0.7f, 0.4f);
        this->drawDmgEffTimer = 0;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    }

    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
    this->secondaryTimer = 0;
    EnPp_ChangeAnim(this, EN_PP_ANIM_DAMAGE);
    this->targetRotY = this->actor.yawTowardsPlayer + 0x8000;
    this->action = EN_PP_ACTION_DAMAGED;
    if (EnPp_CheckCollision(this, play) == EN_PP_COLLISION_RESULT_ABOUT_TO_RUN_OFF_LEDGE) {
        this->damagedVelocity.z = 0.0f;
        this->damagedVelocity.x = 0.0f;
        this->actor.speed = 0.0f;
    }

    Actor_PlaySfx(&this->actor, NA_SE_EN_HIPLOOP_DAMAGE);
    this->actionFunc = EnPp_Damaged;
}

/**
 * Makes the Hiploop move backwards and spin quickly.
 */
void EnPp_Damaged(EnPp* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    this->actor.world.rot.y += 0x1000;
    if (this->secondaryTimer == 0) {
        EnPp_SpawnDust(this, play);
        this->secondaryTimer = 3;
    }

    if (this->chargesInStraightLines) {
        Math_ApproachF(&this->actor.world.pos.x, this->targetPos.x, 0.5f, 25.0f);
        Math_ApproachF(&this->actor.world.pos.z, this->targetPos.z, 0.5f, 25.0f);
        this->actor.world.rot.y += 0x1000;
        if ((this->timer == 0) && (EnPp_PointlessPosCheck(this))) {
            EnPp_SetupCharge(this);
        }
    } else if ((this->actor.colorFilterTimer == 0) && (this->damagedVelocity.x < 1.0f) &&
               (this->damagedVelocity.z < 1.0f)) {
        EnPp_SetupCharge(this);
    }

    if (EnPp_CheckCollision(this, play) == EN_PP_COLLISION_RESULT_ABOUT_TO_RUN_OFF_LEDGE) {
        this->damagedVelocity.z = 0.0f;
        this->damagedVelocity.x = 0.0f;
        this->actor.speed = 0.0f;
    }

    if ((fabsf(this->actor.home.pos.y - this->actor.world.pos.y) > 100.0f) &&
        (fabsf(this->actor.floorHeight - player->actor.floorHeight) > 100.0f)) {
        EnPp_SetupDead(this, play);
    }
}

void EnPp_SetupDead(EnPp* this, PlayState* play) {
    Vec3f deadVelocity;
    s32 yawDiff;

    this->secondaryTimer = this->timer = 0;
    if (!this->chargesInStraightLines) {
        Matrix_RotateYS(this->actor.yawTowardsPlayer, MTXMODE_NEW);
        Matrix_MultVecZ(-30.0f, &deadVelocity);
        Math_Vec3f_Copy(&this->damagedVelocity, &deadVelocity);
    } else {
        yawDiff = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.home.rot.y));
        this->targetRotY = this->actor.home.rot.y + 0x8000;
        if (yawDiff < 0x3000) {
            this->targetRotY = this->actor.home.rot.y;
        }

        Math_Vec3f_Copy(&this->targetPos, &this->actor.world.pos);
        Matrix_RotateYS(this->targetRotY, MTXMODE_NEW);
        Matrix_MultVecZ(-200.0f, &deadVelocity);
        this->targetPos.x += deadVelocity.x;
        this->targetPos.z += deadVelocity.z;
    }

    this->maskBounceAngularVelocity = this->actionVar.isCharging = 0;
    EnPp_ChangeAnim(this, EN_PP_ANIM_DAMAGE);
    this->timer = 15;
    if (((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
         (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) &&
        (this->drawDmgEffTimer == 0)) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    }

    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 25);
    Enemy_StartFinishingBlow(play, &this->actor);
    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 30, NA_SE_EN_HIPLOOP_DEAD);
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->action = EN_PP_ACTION_DEAD;
    this->actionFunc = EnPp_Dead;
}

/**
 * Makes the Hiploop move backwards and spin even quicker than when it's damaged.
 * If the Hiploop ends up in the water while it's moving backwards, this function
 * will make some splashes too. After a short amount of time, this will spawn body
 * parts that fly away from the Hiploop.
 */
void EnPp_Dead(EnPp* this, PlayState* play) {
    Vec3f splashPos;
    WaterBox* waterBox;
    f32 waterSurface;
    s32 isUnderWater;
    s32 i;

    SkelAnime_Update(&this->skelAnime);
    this->actor.world.rot.y += 0x3000;
    waterSurface = this->actor.world.pos.y - 30.0f;
    if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
        (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) {
        if (this->drawDmgEffTimer == 0) {
            return;
        }

        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, EN_PP_BODYPART_MAX, 2, 0.7f, 0.4f);
        this->drawDmgEffTimer = 0;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    }

    if (this->secondaryTimer == 0) {
        EnPp_SpawnDust(this, play);
        this->secondaryTimer = 3;
    }

    isUnderWater = false;
    if ((this->action == EN_PP_ACTION_DEAD) &&
        (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &waterSurface,
                              &waterBox))) {
        if (this->actor.world.pos.y < waterSurface) {
            for (i = 0; i < 5; i++) {
                Math_Vec3f_Copy(&splashPos, &this->actor.world.pos);
                splashPos.x += Rand_CenteredFloat(10 + (5 * i));
                splashPos.z += Rand_CenteredFloat(40 + (5 * i));
                EffectSsGSplash_Spawn(play, &splashPos, NULL, NULL, 0, (Rand_ZeroOne() * 100.0f) + 400.0f);
            }

            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 50, NA_SE_EV_BOMB_DROP_WATER);
            isUnderWater = true;
        }
    }

    if (this->chargesInStraightLines) {
        Math_ApproachF(&this->actor.world.pos.x, this->targetPos.x, 0.5f, 25.0f);
        Math_ApproachF(&this->actor.world.pos.z, this->targetPos.z, 0.5f, 25.0f);
    }

    if ((this->action == EN_PP_ACTION_DEAD) && (isUnderWater || (this->timer == 1))) {
        this->action = EN_PP_ACTION_SPAWN_BODY_PARTS;
    }

    if (this->action == EN_PP_ACTION_DONE_SPAWNING_BODY_PARTS) {
        if (EN_PP_GET_TYPE(&this->actor) == EN_PP_TYPE_UNMASKED) {
            Item_DropCollectibleRandom(play, NULL, &this->actor.world.pos, 0x70);
        } else {
            Item_DropCollectibleRandom(play, NULL, &this->actor.world.pos, 0xE0);
        }

        Actor_Kill(&this->actor);
    }
}

/**
 * Spawns an unmasked Hiploop and prepares the mask to fly through the air.
 */
void EnPp_Mask_SetupDetach(EnPp* this, PlayState* play) {
    s32 yawDiff;
    EnPp* pp = (EnPp*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_PP, this->actor.world.pos.x, this->actor.world.pos.y,
                                  this->actor.world.pos.z, this->actor.world.rot.x, this->actor.shape.rot.y,
                                  this->actor.world.rot.z, EN_PP_TYPE_UNMASKED);

    if (pp != NULL) {
        if (this->chargesInStraightLines) {
            yawDiff = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.home.rot.y));
            pp->chargesInStraightLines = true;
            pp->actor.world.rot.y = this->actor.home.rot.y + 0x8000;
            if (yawDiff < 0x3000) {
                pp->actor.world.rot.y = this->actor.home.rot.y;
            }
        }

        this->actor.gravity = 0.0f;
        this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->actionVar.maskDetachState = EN_PP_MASK_DETACH_STATE_START;
        EnPp_ChangeAnim(this, EN_PP_ANIM_IDLE);
        SkelAnime_Update(&this->skelAnime);
        this->action = EN_PP_ACTION_MASK_DETACH;
        this->actionFunc = EnPp_Mask_Detach;
    }
}

/**
 * Moves the mask through the air and eventually makes it burst into flames.
 */
void EnPp_Mask_Detach(EnPp* this, PlayState* play) {
    if (!CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED) || (this->action == EN_PP_ACTION_MASK_DEAD)) {
        switch (this->actionVar.maskDetachState) {
            case EN_PP_MASK_DETACH_STATE_START:
                this->action = EN_PP_ACTION_MASK_DEAD;
                this->maskAccelY = 50.0f;
                this->maskVelocity.y = -230.0f;
                this->maskVelocity.x = -150.0f;
                Actor_PlaySfx(&this->actor, NA_SE_EN_HIPLOOP_MASC_OFF);
                this->actionVar.maskDetachState = EN_PP_MASK_DETACH_STATE_FALL;
                break;

            case EN_PP_MASK_DETACH_STATE_FALL:
                if (this->maskPos.y > 800.0f) {
                    this->maskAccelY = 50.0f;
                    this->actionVar.maskDetachState++;
                    this->maskVelocity.y = -200.0f;
                    this->maskVelocity.x = -150.0f;
                }
                break;

            case EN_PP_MASK_DETACH_STATE_DIE:
                if (this->maskPos.y > 900.0f) {
                    if (this->maskVelocity.y > 0.0f) {
                        Vec3f maskFirePos;
                        Vec3f sMaskFireVelocityAndAccel[] = {
                            { 1.0f, 0.0f, 0.5f },   { 1.0f, 0.0f, -0.5f },  { -1.0f, 0.0f, 0.5f },
                            { -1.0f, 0.0f, -0.5f }, { 0.5f, 0.0f, 1.0f },   { -0.5f, 0.0f, 1.0f },
                            { 0.5f, 0.0f, -1.0f },  { -0.5f, 0.0f, -1.0f }, { 0.0f, 0.0f, 0.0f },
                        };
                        s32 i;

                        for (i = 0; i < ARRAY_COUNT(sMaskFireVelocityAndAccel); i++) {
                            Math_Vec3f_Copy(&maskFirePos, &this->maskFlamesBasePos);
                            maskFirePos.x += Rand_CenteredFloat(20.0f);
                            maskFirePos.y = this->actor.floorHeight;
                            maskFirePos.z += Rand_CenteredFloat(20.0f);
                            func_800B3030(play, &maskFirePos, &sMaskFireVelocityAndAccel[i],
                                          &sMaskFireVelocityAndAccel[i], 70, 0, 2);
                        }

                        Actor_Kill(&this->actor);
                    }
                }
                break;
        }

        this->maskVelocity.y += this->maskAccelY;
        this->maskRot.z += 0x2000;
        this->maskPos.x += this->maskVelocity.x;
        this->maskPos.y += this->maskVelocity.y;
        this->maskPos.z += this->maskVelocity.z;
    }
}

void EnPp_Fragment_SetupMove(EnPp* this) {
    EnPp_ChangeAnim(this, EN_PP_ANIM_DAMAGE);
    this->actor.velocity.y = Rand_ZeroFloat(5.0f) + 13.0f;
    this->actor.gravity = -2.0f;
    this->timer = Rand_S16Offset(30, 30);
    this->fragmentAngularVelocity.x = (this->fragmentIndex * 0x2E) + 0xFF00;
    this->fragmentAngularVelocity.z = (this->fragmentIndex * 0x2E) + 0xFF00;
    if (EN_PP_GET_TYPE(&this->actor) != EN_PP_TYPE_FRAGMENT_BODY) {
        this->actor.speed = Rand_ZeroFloat(4.0f) + 4.0f;
        this->actor.world.rot.y = ((s32)Rand_CenteredFloat(223.0f) + 0x1999) * this->fragmentIndex;
    }

    this->action = EN_PP_ACTION_BODY_PART_MOVE;
    this->actionFunc = EnPp_Fragment_Move;
}

/**
 * Makes the fragment fly through the air. If it touches water, it will make a splash.
 */
void EnPp_Fragment_Move(EnPp* this, PlayState* play) {
    s32 pad;
    Vec3f splashPos;
    WaterBox* waterBox;
    f32 waterSurface;
    s32 i;

    SkelAnime_Update(&this->skelAnime);

    // Updates the positions of the blue flames for this fragment. The body fragment has 10 flames, while all other
    // fragments only have a single flame.
    if (EN_PP_GET_TYPE(&this->actor) == EN_PP_TYPE_FRAGMENT_BODY) {
        this->fragmentFlameCount = ARRAY_COUNT(this->fragmentFlamesPos);
        for (i = 0; i < ARRAY_COUNT(this->fragmentFlamesPos); i++) {
            Math_Vec3f_Copy(&this->fragmentFlamesPos[i], &this->fragmentPos);
            this->fragmentFlamesPos[i].x += Math_SinS(0xCCC * i) * 15.0f;
            this->fragmentFlamesPos[i].y += -5.0f;
            this->fragmentFlamesPos[i].z += Math_CosS(0xCCC * i) * 15.0f;
        }
    } else {
        Math_Vec3f_Copy(&this->fragmentFlamesPos[0], &this->fragmentPos);
        this->fragmentFlameCount = 1;
        this->actor.shape.rot.x += this->fragmentAngularVelocity.x;
        this->actor.shape.rot.z += this->fragmentAngularVelocity.z;
    }

    if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &waterSurface,
                             &waterBox) &&
        (this->actor.world.pos.y < (waterSurface + 5.0f))) {
        this->timer = 0;
        if (EN_PP_GET_TYPE(&this->actor) == EN_PP_TYPE_FRAGMENT_BODY) {
            for (i = 0; i < 6; i++) {
                Math_Vec3f_Copy(&splashPos, &this->actor.world.pos);
                splashPos.x += Rand_CenteredFloat(10 + (5 * i));
                splashPos.z += Rand_CenteredFloat(40 + (5 * i));
                EffectSsGSplash_Spawn(play, &splashPos, NULL, NULL, 0, (Rand_ZeroOne() * 100.0f) + 400.0f);
            }
        } else {
            EffectSsGSplash_Spawn(play, &this->actor.world.pos, NULL, NULL, 0, 400);
        }

        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 50, NA_SE_EV_BOMB_DROP_WATER);
    }

    if ((this->timer == 0) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        Actor_Kill(&this->actor);
    }
}

void EnPp_UpdateDamage(EnPp* this, PlayState* play) {
    s16 yawDiff = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.world.rot.y));
    s32 attackBouncedOffMask = false;
    s32 attackDealsDamage = false;

    if (EN_PP_GET_TYPE(&this->actor) == EN_PP_TYPE_MASKED) {
        if ((yawDiff < (BREG(2) + 0x4A9C)) || (EN_PP_GET_TYPE(&this->actor) > EN_PP_TYPE_MASKED)) {
            this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR;
        } else {
            this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR;
        }
    }

    if ((EN_PP_GET_TYPE(&this->actor) == EN_PP_TYPE_MASKED) && (this->action < EN_PP_ACTION_MASK_DETACH)) {
        if (this->maskCollider.elements[0].base.acElemFlags & ACELEM_HIT) {
            if (yawDiff < (BREG(2) + 0x4A9C)) {
                if (this->actor.colChkInfo.damageEffect == EN_PP_DMGEFF_HOOKSHOT) {
                    EnPp_Mask_SetupDetach(this, play);
                } else if ((this->actor.colChkInfo.damageEffect == EN_PP_DMGEFF_GORON_POUND) ||
                           (this->actor.colChkInfo.damageEffect == EN_PP_DMGEFF_KNOCK_OFF_MASK)) {
                    EnPp_Mask_SetupDetach(this, play);
                    this->action = EN_PP_ACTION_MASK_DEAD;
                }
            } else {
                attackBouncedOffMask = true;
            }
        } else if (this->maskCollider.elements[0].base.acElemFlags & ACELEM_HIT) {
            attackBouncedOffMask = true;
        }
    }

    if (this->bodyCollider.elements[0].base.acElemFlags & ACELEM_HIT) {
        if (EN_PP_GET_TYPE(&this->actor) != EN_PP_TYPE_MASKED) {
            if ((this->action < EN_PP_ACTION_DAMAGED) && (this->action != EN_PP_ACTION_JUMP)) {
                if (this->actor.colChkInfo.damageEffect == EN_PP_DMGEFF_HOOKSHOT) {
                    if (EN_PP_GET_TYPE(&this->actor) == EN_PP_TYPE_MASKED) {
                        return;
                    }
                    attackDealsDamage = true;
                }

                if (this->actor.colChkInfo.damageEffect == EN_PP_DMGEFF_ELECTRIC_STUN) {
                    if (((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
                         (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) ||
                        (this->drawDmgEffTimer == 0)) {
                        this->drawDmgEffTimer = 40;
                        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL;
                        Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA,
                                             40);
                        EnPp_SetupStunnedOrFrozen(this);
                    }
                    return;
                } else if (this->actor.colChkInfo.damageEffect == EN_PP_DMGEFF_STUN) {
                    if (((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
                         (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) ||
                        (this->drawDmgEffTimer == 0)) {
                        Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA,
                                             40);
                        this->secondaryTimer = 40;
                        EnPp_SetupStunnedOrFrozen(this);
                    }
                    return;
                } else if ((this->actor.colChkInfo.damageEffect != EN_PP_DMGEFF_GORON_POUND) &&
                           (this->actor.colChkInfo.damageEffect != EN_PP_DMGEFF_JUMP)) {
                    attackDealsDamage = true;
                    this->hasBeenDamaged = true;
                    this->actor.speed = 0.0f;
                    if (this->actor.colChkInfo.damageEffect == EN_PP_DMGEFF_FIRE) {
                        this->drawDmgEffTimer = 40;
                        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                    } else if (this->actor.colChkInfo.damageEffect == EN_PP_DMGEFF_FREEZE) {
                        if (((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
                             (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) ||
                            (this->drawDmgEffTimer == 0)) {
                            Actor_ApplyDamage(&this->actor);
                            this->drawDmgEffTimer = 80;
                            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_SFX;
                            this->drawDmgEffScale = 0.0f;
                            this->drawDmgEffFrozenSteamScale = 1.5f;
                        }

                        if (this->actor.colChkInfo.health <= 0) {
                            EnPp_SetupDead(this, play);
                        } else {
                            EnPp_SetupStunnedOrFrozen(this);
                        }
                        return;
                    } else if ((this->actor.colChkInfo.damageEffect == EN_PP_DMGEFF_LIGHT_ORB) &&
                               ((((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX)) &&
                                 (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) ||
                                (this->drawDmgEffTimer == 0))) {
                        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.focus.pos.x,
                                    this->actor.focus.pos.y, this->actor.focus.pos.z, 0, 0, 0,
                                    CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_GRAY, 255, COLORFILTER_BUFFLAG_OPA,
                                             25);
                        this->drawDmgEffTimer = 20;
                        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                    }
                } else if (this->action != EN_PP_ACTION_JUMP) {
                    EnPp_SetupJump(this);
                }
            }

            if (attackDealsDamage) {
                Actor_ApplyDamage(&this->actor);
                if (this->actor.colChkInfo.health > 0) {
                    EnPp_SetupDamaged(this, play);
                } else {
                    EnPp_SetupDead(this, play);
                }
            }
        } else {
            attackBouncedOffMask = true;
        }
    }

    if (attackBouncedOffMask && (((this->action == EN_PP_ACTION_IDLE)) || (this->action == EN_PP_ACTION_CHARGE) ||
                                 (this->action == EN_PP_ACTION_ROAR))) {
        this->secondaryTimer = 0;
        this->timer = 10;
        this->maskBounceAngularVelocity = 0x20D0;
        this->actor.speed = 0.0f;
        if (this->action == EN_PP_ACTION_CHARGE) {
            this->actionVar.isCharging = false;
            EnPp_ChangeAnim(this, EN_PP_ANIM_TURN_TO_FACE_PLAYER);
            this->maskBounceAngularVelocity = 0x1B58;
        }

        this->targetRotY = this->actor.yawTowardsPlayer;
        if (EN_PP_GET_TYPE(&this->actor) == EN_PP_TYPE_MASKED) {
            Actor_PlaySfx(&this->actor, NA_SE_IT_SHIELD_BOUND);
        }
    }
}

void EnPp_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnPp* this = (EnPp*)thisx;
    WaterBox* waterBox;
    f32 waterSurface;

    DECR(this->timer);

    if (this->secondaryTimer != 0) {
        this->secondaryTimer--;
    }

    if (this->drawDmgEffTimer != 0) {
        this->drawDmgEffTimer--;
    }

    this->actionFunc(this, play);
    EnPp_PlaySfxForAnimation(this);
    Actor_MoveWithGravity(&this->actor);

    if (this->action != EN_PP_ACTION_BODY_PART_MOVE) {
        if (EN_PP_GET_TYPE(&this->actor) > EN_PP_TYPE_MASKED) {
            this->maskCollider.elements[0].dim.modelSphere.radius = 42;
            this->maskCollider.elements[0].dim.modelSphere.center.x = 400;
            this->maskCollider.elements[0].dim.modelSphere.center.y = -400;
            this->bodyCollider.elements[0].dim.modelSphere.radius = 20;
            this->bodyCollider.elements[0].dim.modelSphere.center.x = 400;
            this->bodyCollider.elements[0].dim.modelSphere.center.y = -400;
        } else {
            this->maskCollider.elements[0].dim.modelSphere.radius = 25;
            this->maskCollider.elements[0].dim.modelSphere.center.x = 780;
            this->maskCollider.elements[0].dim.modelSphere.center.y = -500;
            this->bodyCollider.elements[0].dim.modelSphere.radius = 25;
            this->bodyCollider.elements[0].dim.modelSphere.center.x = 400;
            this->bodyCollider.elements[0].dim.modelSphere.center.y = -400;
        }
    }

    if ((this->action < EN_PP_ACTION_DEAD) &&
        (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &waterSurface,
                              &waterBox)) &&
        (this->actor.world.pos.y < waterSurface)) {
        EnPp_SetupDead(this, play);
        return;
    }

    if (this->action != EN_PP_ACTION_BODY_PART_MOVE) {
        EnPp_UpdateDamage(this, play);
        Actor_SetFocus(&this->actor, 40.0f);
        if ((this->action == EN_PP_ACTION_DEAD) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            this->actor.world.pos.x += this->damagedVelocity.x;
            this->actor.world.pos.z += this->damagedVelocity.z;
            Math_ApproachZeroF(&this->damagedVelocity.x, 1.0f, 2.0f);
            Math_ApproachZeroF(&this->damagedVelocity.z, 1.0f, 2.0f);
        }
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 35.0f, 40.0f, 40.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
    if (this->action != EN_PP_ACTION_BODY_PART_MOVE) {
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    if (this->action != EN_PP_ACTION_BODY_PART_MOVE) {
        if (this->floorPolyForCircleShadow == NULL) {
            this->floorPolyForCircleShadow = this->actor.floorPoly;
        }

        if ((this->action < EN_PP_ACTION_DEAD) || (this->action == EN_PP_ACTION_JUMP)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->maskCollider.base);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyCollider.base);
        }

        if ((this->action < EN_PP_ACTION_MASK_DETACH) && (this->action != EN_PP_ACTION_STUNNED_OR_FROZEN)) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->maskCollider.base);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->bodyCollider.base);
        }

        CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);
        if (this->hornColliderOn) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->hornCollider.base);
        }
    }
}

s32 EnPp_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnPp* this = (EnPp*)thisx;

    if (this->action != EN_PP_ACTION_BODY_PART_MOVE) {
        if ((limbIndex != HIPLOOP_LIMB_MASK) &&
            ((this->action == EN_PP_ACTION_MASK_DETACH) || (this->action == EN_PP_ACTION_MASK_DEAD))) {
            *dList = NULL;
        }

        if ((limbIndex == HIPLOOP_LIMB_MASK) && (EN_PP_GET_TYPE(&this->actor) != EN_PP_TYPE_MASKED)) {
            *dList = NULL;
        }

        if (limbIndex == HIPLOOP_LIMB_MASK) {
            rot->x += TRUNCF_BINANG(this->maskRot.x);
            rot->y += TRUNCF_BINANG(this->maskRot.y);
            rot->z += TRUNCF_BINANG(this->maskRot.z);
            pos->x += this->maskPos.x;
            pos->y += this->maskPos.y;
            pos->z += this->maskPos.z;
        }
    } else if ((limbIndex + EN_PP_TYPE_FRAGMENT_BASE) != EN_PP_GET_TYPE(&this->actor)) {
        *dList = NULL;
    }

    return false;
}

void EnPp_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f sVertexOffset1 = { 0.0f, 0.0f, 0.0f };
    static Vec3f sVertexOffset2 = { 0.0f, 0.0f, 0.0f };
    EnPp* this = (EnPp*)thisx;
    Vec3f blureVertex1;
    Vec3f blureVertex2;

    Matrix_Push();

    if (this->action != EN_PP_ACTION_BODY_PART_MOVE) {
        if (limbIndex == HIPLOOP_LIMB_BACK_RIGHT_FOOT) {
            Matrix_MultVec3f(&gZeroVec3f, &this->backFootPos[0]);
        }

        if (limbIndex == HIPLOOP_LIMB_BACK_LEFT_FOOT) {
            Matrix_MultVec3f(&gZeroVec3f, &this->backFootPos[1]);
        }

        if ((limbIndex == HIPLOOP_LIMB_MASK) && (EN_PP_GET_TYPE(&this->actor) == EN_PP_TYPE_MASKED)) {
            Matrix_MultVec3f(&gZeroVec3f, &this->maskFlamesBasePos);
            if (this->hornColliderOn) {
                if ((this->skelAnime.curFrame > 26.0f) || (this->action >= EN_PP_ACTION_MASK_DETACH)) {
                    this->hornColliderOn = false;
                    EffectBlure_AddSpace(Effect_GetByIndex(this->blureIndex));
                } else if ((this->skelAnime.curFrame > 0.0f) && (this->skelAnime.curFrame < 10.0f)) {
                    Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
                    sVertexOffset1.x = 1160.0f;
                    sVertexOffset1.y = -900.0f;
                    sVertexOffset1.z = 0.0f;
                    sVertexOffset2.x = 100.0f;
                    sVertexOffset2.y = 300.0f;
                    sVertexOffset2.z = 0.0f;
                    Math_Vec3f_Copy(&this->hornCollider.dim.quad[3], &this->hornCollider.dim.quad[1]);
                    Math_Vec3f_Copy(&this->hornCollider.dim.quad[2], &this->hornCollider.dim.quad[0]);
                    Matrix_MultVec3f(&sVertexOffset1, &this->hornCollider.dim.quad[1]);
                    Matrix_MultVec3f(&sVertexOffset2, &this->hornCollider.dim.quad[0]);
                    Collider_SetQuadVertices(&this->hornCollider, &this->hornCollider.dim.quad[0],
                                             &this->hornCollider.dim.quad[1], &this->hornCollider.dim.quad[2],
                                             &this->hornCollider.dim.quad[3]);
                    Matrix_MultVec3f(&sVertexOffset1, &blureVertex1);
                    Matrix_MultVec3f(&sVertexOffset2, &blureVertex2);
                    EffectBlure_AddVertex(Effect_GetByIndex(this->blureIndex), &blureVertex1, &blureVertex2);
                }
            }
        }
    } else {
        if ((EN_PP_GET_TYPE(&this->actor) >= EN_PP_TYPE_FRAGMENT_BASE) &&
            ((limbIndex + EN_PP_TYPE_FRAGMENT_BASE) == EN_PP_GET_TYPE(&this->actor))) {
            Matrix_MultVec3f(&gZeroVec3f, &this->fragmentPos);
        }
    }

    Matrix_Pop();

    if (this->action != EN_PP_ACTION_BODY_PART_MOVE) {
        Collider_UpdateSpheres(limbIndex, &this->maskCollider);
        Collider_UpdateSpheres(limbIndex, &this->bodyCollider);
        if ((limbIndex == HIPLOOP_LIMB_BODY) || (limbIndex == HIPLOOP_LIMB_FRONT_LEFT_LOWER_LEG) ||
            (limbIndex == HIPLOOP_LIMB_FRONT_RIGHT_LOWER_LEG) || (limbIndex == HIPLOOP_LIMB_LEFT_WING_MIDDLE) ||
            (limbIndex == HIPLOOP_LIMB_BACK_RIGHT_LOWER_LEG) || (limbIndex == HIPLOOP_LIMB_RIGHT_WING_MIDDLE) ||
            (limbIndex == HIPLOOP_LIMB_CENTER_WING_BASE) || (limbIndex == HIPLOOP_LIMB_CENTER_WING_MIDDLE) ||
            (limbIndex == HIPLOOP_LIMB_BACK_LEFT_LOWER_LEG) || (limbIndex == HIPLOOP_LIMB_RIGHT_EYE) ||
            (limbIndex == HIPLOOP_LIMB_LEFT_EYE)) {
            Matrix_MultZero(&this->bodyPartsPos[this->bodyPartIndex]);
            this->bodyPartIndex++;
            if (this->bodyPartIndex >= EN_PP_BODYPART_MAX) {
                this->bodyPartIndex = 0;
            }

            if ((this->action == EN_PP_ACTION_SPAWN_BODY_PARTS) && (this->deadBodyPartsSpawnedCount < 6) &&
                ((limbIndex == HIPLOOP_LIMB_BODY) || (limbIndex == HIPLOOP_LIMB_FRONT_LEFT_LOWER_LEG) ||
                 (limbIndex == HIPLOOP_LIMB_FRONT_RIGHT_LOWER_LEG) || (limbIndex == HIPLOOP_LIMB_LEFT_WING_MIDDLE) ||
                 (limbIndex == HIPLOOP_LIMB_RIGHT_WING_MIDDLE) || (limbIndex == HIPLOOP_LIMB_CENTER_WING_MIDDLE))) {
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_PP, this->actor.world.pos.x, this->actor.world.pos.y,
                            this->actor.world.pos.z, this->actor.world.rot.x, this->actor.world.rot.y,
                            this->actor.world.rot.z, limbIndex + EN_PP_TYPE_FRAGMENT_BASE);

                this->deadBodyPartsSpawnedCount++;
                if (this->deadBodyPartsSpawnedCount >= 6) {
                    this->action = EN_PP_ACTION_DONE_SPAWNING_BODY_PARTS;
                }
            }
        }
    }
}

void EnPp_Draw(Actor* thisx, PlayState* play) {
    EnPp* this = (EnPp*)thisx;
    MtxF mtxF;
    Vec3f pos;
    f32 scale;
    f32 alpha;

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnPp_OverrideLimbDraw, EnPp_PostLimbDraw, &this->actor);

    if (this->fragmentFlameCount != 0) {
        scale = 0.4f;
        if (EN_PP_GET_TYPE(&this->actor) == EN_PP_TYPE_FRAGMENT_BODY) {
            scale = 0.6f;
        }

        Actor_DrawDamageEffects(play, &this->actor, this->fragmentFlamesPos, this->fragmentFlameCount, scale, scale,
                                1.0f, ACTOR_DRAW_DMGEFF_BLUE_FIRE);
    }

    if (this->drawDmgEffTimer != 0) {
        alpha = this->drawDmgEffTimer * 0.05f;
        if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
            (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) {
            this->drawDmgEffScale += 0.3f;
            if (this->drawDmgEffScale > 0.5f) {
                this->drawDmgEffScale = 0.5f;
            }

            Math_ApproachF(&this->drawDmgEffFrozenSteamScale, this->drawDmgEffScale, 0.1f, 0.04f);
        } else {
            this->drawDmgEffScale = 0.8f;
            this->drawDmgEffFrozenSteamScale = 0.8f;
        }

        Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, EN_PP_BODYPART_MAX, this->drawDmgEffScale,
                                this->drawDmgEffFrozenSteamScale, alpha, this->drawDmgEffType);
    }

    if (this->floorPolyForCircleShadow != NULL) {
        if ((this->action != EN_PP_ACTION_MASK_DETACH) && (this->action < EN_PP_ACTION_DEAD)) {
            OPEN_DISPS(play->state.gfxCtx);

            Gfx_SetupDL44_Xlu(play->state.gfxCtx);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 255);

            Math_Vec3f_Copy(&pos, &this->actor.world.pos);
            pos.x += (Math_SinS(this->actor.world.rot.y) * -13.0f);
            pos.y = this->actor.floorHeight;
            pos.z += (Math_CosS(this->actor.world.rot.y) * -13.0f);
            func_800C0094(this->actor.floorPoly, pos.x, pos.y, pos.z, &mtxF);

            Matrix_Mult(&mtxF, MTXMODE_NEW);
            Matrix_Scale(0.5f, 1.0f, 0.5f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gCircleShadowDL);

            CLOSE_DISPS(play->state.gfxCtx);
        }
    }
}
