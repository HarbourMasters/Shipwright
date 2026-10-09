/*
 * File: z_en_raf.c
 * Overlay: ovl_En_Raf
 * Description: Carnivorous Lily Pad
 */

#include "z_en_raf.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "BenPort.h"

#define FLAGS (ACTOR_FLAG_LOCK_ON_DISABLED)

void EnRaf_Init(Actor* thisx, PlayState* play);
void EnRaf_Destroy(Actor* thisx, PlayState* play);
void EnRaf_Update(Actor* thisx, PlayState* play);
void EnRaf_Draw(Actor* thisx, PlayState* play);
void EnRaf_Reset(void);

void EnRaf_SetupIdle(EnRaf* this);
void EnRaf_Idle(EnRaf* this, PlayState* play);
void EnRaf_SetupGrab(EnRaf* this);
void EnRaf_Grab(EnRaf* this, PlayState* play);
void EnRaf_SetupChew(EnRaf* this);
void EnRaf_Chew(EnRaf* this, PlayState* play);
void EnRaf_SetupThrow(EnRaf* this, PlayState* play);
void EnRaf_Throw(EnRaf* this, PlayState* play);
void EnRaf_Explode(EnRaf* this, PlayState* play);
void EnRaf_PostDetonation(EnRaf* this, PlayState* play);
void EnRaf_Convulse(EnRaf* this, PlayState* play);
void EnRaf_SetupDissolve(EnRaf* this);
void EnRaf_Dissolve(EnRaf* this, PlayState* play);
void EnRaf_SetupDormant(EnRaf* this);
void EnRaf_Dormant(EnRaf* this, PlayState* play);
void EnRaf_InitializeEffect(EnRaf* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale, s16 timer);
void EnRaf_UpdateEffects(EnRaf* this, PlayState* play);
void EnRaf_DrawEffects(EnRaf* this, PlayState* play);

typedef enum CarnivorousLilyAction {
    /* 0 */ CARNIVOROUS_LILY_ACTION_IDLE,
    /* 1 */ CARNIVOROUS_LILY_ACTION_GRAB,
    /* 2 */ CARNIVOROUS_LILY_ACTION_CHEW,
    /* 3 */ CARNIVOROUS_LILY_ACTION_THROW,
    /* 4 */ CARNIVOROUS_LILY_ACTION_EXPLODE,
    /* 5 */ CARNIVOROUS_LILY_ACTION_CONVULSE,
    /* 6 */ CARNIVOROUS_LILY_ACTION_DISSOLVE,
    /* 7 */ CARNIVOROUS_LILY_ACTION_DORMANT
} CarnivorousLilyAction;

typedef enum CarnivorousLilyGrabTarget {
    /* 0 */ CARNIVOROUS_LILY_GRAB_TARGET_PLAYER,
    /* 1 */ CARNIVOROUS_LILY_GRAB_TARGET_EXPLOSIVE,
    /* 2 */ CARNIVOROUS_LILY_GRAB_TARGET_GORON_PLAYER
} CarnivorousLilyGrabTarget;

typedef enum CarnivorousLilyPetalScaleType {
    /* 0 */ CARNIVOROUS_LILY_PETAL_SCALE_TYPE_DEAD,
    /* 1 */ CARNIVOROUS_LILY_PETAL_SCALE_TYPE_GRAB,
    /* 2 */ CARNIVOROUS_LILY_PETAL_SCALE_TYPE_CHEW,
    /* 3 */ CARNIVOROUS_LILY_PETAL_SCALE_TYPE_IDLE_OR_THROW
} CarnivorousLilyPetalScaleType;

ActorProfile En_Raf_Profile = {
    /**/ ACTOR_EN_RAF,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_RAF,
    /**/ sizeof(EnRaf),
    /**/ EnRaf_Init,
    /**/ EnRaf_Destroy,
    /**/ EnRaf_Update,
    /**/ EnRaf_Draw,
    /**/ EnRaf_Reset,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x04, 0x10 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 50, 10, -10, { 0, 0, 0 } },
};

static u8 sTeethClearPixelTableFirstPass[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1,
    1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0,
};

static u8 sTeethClearPixelTableSecondPass[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

static u8 sPetalClearPixelTableFirstPass[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1,
    1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0,
    1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1,
    1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1,
    1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0,
    0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0,
    0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0,
    1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0,
    0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1,
};

static u8 sPetalClearPixelTableSecondPass[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

static u8 sCurTeethMask[ARRAY_COUNTU(sTeethClearPixelTableFirstPass)];
static u8 sCurPetalMask[ARRAY_COUNTU(sPetalClearPixelTableFirstPass)];

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0xF),
    /* Deku Stick     */ DMG_ENTRY(0, 0xF),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0xE),
    /* Zora boomerang */ DMG_ENTRY(0, 0xF),
    /* Normal arrow   */ DMG_ENTRY(0, 0xF),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0xF),
    /* Goron punch    */ DMG_ENTRY(0, 0xF),
    /* Sword          */ DMG_ENTRY(0, 0xF),
    /* Goron pound    */ DMG_ENTRY(0, 0xF),
    /* Fire arrow     */ DMG_ENTRY(0, 0xF),
    /* Ice arrow      */ DMG_ENTRY(0, 0xF),
    /* Light arrow    */ DMG_ENTRY(0, 0xF),
    /* Goron spikes   */ DMG_ENTRY(0, 0xF),
    /* Deku spin      */ DMG_ENTRY(0, 0xF),
    /* Deku bubble    */ DMG_ENTRY(0, 0xF),
    /* Deku launch    */ DMG_ENTRY(0, 0xF),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0xF),
    /* Zora barrier   */ DMG_ENTRY(0, 0xF),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0xF),
    /* Zora punch     */ DMG_ENTRY(0, 0xF),
    /* Spin attack    */ DMG_ENTRY(0, 0xF),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0xF),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(0, 0xF),
};

// #region 2S2H [Port] The original game set the texture pixel to zero to make it transparent.
// We need to set a mask to 1 to tell the shader to mask that part of the texture.
// This applies to both functions.

/**
 * Sets the `index`th pixel of the trap teeth texture mask to 1
 * according to the `clearPixelTable`. Only works if the texture uses 16-bit pixels.
 */
void EnRaf_ClearPixelTeeth(u8* mask, u8* clearPixelTable, s32 index) {
    if ((index < (8 * 8)) && (clearPixelTable[index] != 0)) {
        mask[index] = 1;
    }
}

/**
 * Sets the `index`th pixel of the trap petal texture mask to 1
 * according to the `clearPixelTable`. Only works if the texture uses 16-bit pixels.
 */
void EnRaf_ClearPixelPetal(u8* mask, u8* clearPixelTable, s32 index) {
    if (clearPixelTable[index] != 0) {
        mask[index] = 1;
    }
}

void EnRaf_Init(Actor* thisx, PlayState* play) {
    EnRaf* this = (EnRaf*)thisx;
    static Vec3f limbScale = { 1.0f, 1.0f, 1.0f };
    s32 pad;
    s32 i;
    CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&this->dyna, 0);
    CollisionHeader_GetVirtual(&gCarnivorousLilyPadCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    Collider_InitAndSetCylinder(play, &this->collider, &this->dyna.actor, &sCylinderInit);
    this->dyna.actor.attentionRangeType = ATTENTION_RANGE_3;
    this->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;
    SkelAnime_InitFlex(play, &this->skelAnime, &gCarnivorousLilyPadSkel, &gCarnivorousLilyPadSpitAnim, this->jointTable,
                       this->morphTable, CARNIVOROUS_LILY_PAD_LIMB_MAX);

    for (i = 0; i < ARRAY_COUNT(this->limbScale); i++) {
        Math_Vec3f_Copy(&this->targetLimbScale[i], &limbScale);
        Math_Vec3f_Copy(&this->limbScale[i], &limbScale);
    }

    this->dyna.actor.colChkInfo.damageTable = &sDamageTable;
    this->dyna.actor.colChkInfo.health = BREG(1) + 2;
    this->mainType = CARNIVOROUS_LILY_GET_TYPE(&this->dyna.actor);
    this->reviveTimer = CARNIVOROUS_LILY_GET_REVIVE_TIMER(&this->dyna.actor);
    this->switchFlag = CARNIVOROUS_LILY_GET_SWITCH_FLAG(&this->dyna.actor);
    if (this->switchFlag == CARNIVOROUS_LILY_SWITCH_FLAG_NONE) {
        this->switchFlag = SWITCH_FLAG_NONE;
    }

    if (this->reviveTimer == 31) {
        this->reviveTimer = -1;
    } else {
        this->reviveTimer = 30;
    }

    if (((this->switchFlag > SWITCH_FLAG_NONE) || (this->mainType == CARNIVOROUS_LILY_TYPE_DORMANT) ||
         CHECK_WEEKEVENTREG(WEEKEVENTREG_12_01)) &&
        (Flags_GetSwitch(play, this->switchFlag) || (this->mainType == CARNIVOROUS_LILY_TYPE_DORMANT))) {
        s32 i;

        for (i = CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_LOWER_SEGMENT; i <= CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_UPPER_SEGMENT;
             i++) {
            Math_Vec3f_Copy(&this->limbScale[i], &gZeroVec3f);
            Math_Vec3f_Copy(&this->targetLimbScale[i], &gZeroVec3f);
        }

        EnRaf_SetupDormant(this);
    } else {
        this->bobPhase = Rand_ZeroFloat(1.0f) * 20000.0f;
        Actor_SetScale(&this->dyna.actor, 0.01f);
        EnRaf_SetupIdle(this);
    }

    Gfx_RegisterBlendedTexture(gCarnivorousLilyPadTrapPetalTex, sCurPetalMask, NULL);
    Gfx_RegisterBlendedTexture(gCarnivorousLilyPadTrapTeethTex, sCurTeethMask, NULL);
}

void EnRaf_Destroy(Actor* thisx, PlayState* play) {
    EnRaf* this = (EnRaf*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyCylinder(play, &this->collider);
}

typedef enum CarnivorousLilyAnimation {
    /* 0 */ CARNIVOROUS_LILY_ANIM_IDLE,
    /* 1 */ CARNIVOROUS_LILY_ANIM_CLOSE,
    /* 2 */ CARNIVOROUS_LILY_ANIM_CHEW,
    /* 3 */ CARNIVOROUS_LILY_ANIM_SPIT,
    /* 4 */ CARNIVOROUS_LILY_ANIM_CONVULSE,
    /* 5 */ CARNIVOROUS_LILY_ANIM_DEATH,
    /* 6 */ CARNIVOROUS_LILY_ANIM_MAX
} CarnivorousLilyAnimation;

static AnimationHeader* sAnimations[CARNIVOROUS_LILY_ANIM_MAX] = {
    &gCarnivorousLilyPadSpitAnim,     // CARNIVOROUS_LILY_ANIM_IDLE
    &gCarnivorousLilyPadCloseAnim,    // CARNIVOROUS_LILY_ANIM_CLOSE
    &gCarnivorousLilyPadChewAnim,     // CARNIVOROUS_LILY_ANIM_CHEW
    &gCarnivorousLilyPadSpitAnim,     // CARNIVOROUS_LILY_ANIM_SPIT
    &gCarnivorousLilyPadConvulseAnim, // CARNIVOROUS_LILY_ANIM_CONVULSE
    &gCarnivorousLilyPadDeathAnim,    // CARNIVOROUS_LILY_ANIM_DEATH
};

static u8 sAnimationModes[CARNIVOROUS_LILY_ANIM_MAX] = {
    ANIMMODE_ONCE, // CARNIVOROUS_LILY_ANIM_IDLE
    ANIMMODE_ONCE, // CARNIVOROUS_LILY_ANIM_CLOSE
    ANIMMODE_LOOP, // CARNIVOROUS_LILY_ANIM_CHEW
    ANIMMODE_ONCE, // CARNIVOROUS_LILY_ANIM_SPIT
    ANIMMODE_LOOP, // CARNIVOROUS_LILY_ANIM_CONVULSE
    ANIMMODE_ONCE, // CARNIVOROUS_LILY_ANIM_DEATH
};

void EnRaf_ChangeAnim(EnRaf* this, s32 animIndex) {
    f32 startFrame = 0.0f;
    f32 playSpeed = 1.0f;

    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);
    if (animIndex == CARNIVOROUS_LILY_ANIM_IDLE) {
        startFrame = this->animEndFrame;
    } else if (animIndex == CARNIVOROUS_LILY_ANIM_CLOSE) {
        playSpeed = 2.0f;
    }

    Animation_Change(&this->skelAnime, sAnimations[animIndex], playSpeed, startFrame, this->animEndFrame,
                     sAnimationModes[animIndex], -4.0f);
}

void EnRaf_SetupIdle(EnRaf* this) {
    Vec3f targetLimbScale = { 1.0f, 1.0f, 1.0f };
    s32 i;

    EnRaf_ChangeAnim(this, CARNIVOROUS_LILY_ANIM_IDLE);

    for (i = CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_LOWER_SEGMENT; i <= CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_UPPER_SEGMENT; i++) {
        Math_Vec3f_Copy(&this->targetLimbScale[i], &targetLimbScale);
    }

    this->petalScaleType = CARNIVOROUS_LILY_PETAL_SCALE_TYPE_IDLE_OR_THROW;
    this->action = CARNIVOROUS_LILY_ACTION_IDLE;
    this->actionFunc = EnRaf_Idle;
}

/**
 * Sits around waiting for the player or an explosive to get near in order to grab them.
 */
void EnRaf_Idle(EnRaf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* explosive;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;

    if (this->timer == 0) {
        if ((player->transformation != PLAYER_FORM_DEKU) &&
            (this->dyna.actor.xzDistToPlayer < (BREG(48) + 80.0f) && (player->invincibilityTimer == 0) &&
             DynaPolyActor_IsPlayerOnTop(&this->dyna) && !(player->stateFlags1 & PLAYER_STATE1_8000000) &&
             play->grabPlayer(play, player))) {
            player->actor.parent = &this->dyna.actor;
            this->grabTarget = CARNIVOROUS_LILY_GRAB_TARGET_PLAYER;

            if (player->transformation == PLAYER_FORM_GORON) {
                this->grabTarget = CARNIVOROUS_LILY_GRAB_TARGET_GORON_PLAYER;
            } else {
                player->av2.actionVar2 = 50;
            }

            this->playerRotYWhenGrabbed = player->actor.world.rot.y;
            EnRaf_SetupGrab(this);
            return;
        }

        if ((play->gameplayFrames % 2) == 0) {
            return;
        }

        explosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first;
        while (explosive != NULL) {
            // This check is pointless, since EnRaf is never in the explosive category.
            if ((EnRaf*)explosive == this) {
                explosive = explosive->next;
                continue;
            }

            xDiff = explosive->world.pos.x - this->dyna.actor.world.pos.x;
            yDiff = explosive->world.pos.y - this->dyna.actor.world.pos.y;
            zDiff = explosive->world.pos.z - this->dyna.actor.world.pos.z;
            if ((fabsf(xDiff) < 80.0f) && (fabsf(yDiff) < 30.0f) && (fabsf(zDiff) < 80.0f) &&
                (explosive->update != NULL) && (explosive->velocity.y != 0.0f)) {
                Actor_Kill(explosive);
                this->grabTarget = CARNIVOROUS_LILY_GRAB_TARGET_EXPLOSIVE;
                this->collider.dim.radius = 30;
                this->collider.dim.height = 90;
                this->collider.dim.yShift = -10;
                EnRaf_SetupGrab(this);
                return;
            }

            explosive = explosive->next;
        }
    }
}

void EnRaf_SetupGrab(EnRaf* this) {
    EnRaf_ChangeAnim(this, CARNIVOROUS_LILY_ANIM_CLOSE);
    this->petalScaleType = CARNIVOROUS_LILY_PETAL_SCALE_TYPE_GRAB;
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_SUISEN_DRINK);
    this->action = CARNIVOROUS_LILY_ACTION_GRAB;
    this->actionFunc = EnRaf_Grab;
}

/**
 * Grabs the player or explosive that entered its range.
 */
void EnRaf_Grab(EnRaf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame = this->skelAnime.curFrame;

    if ((this->grabTarget != CARNIVOROUS_LILY_GRAB_TARGET_EXPLOSIVE) && (player->stateFlags2 & PLAYER_STATE2_80) &&
        (&this->dyna.actor == player->actor.parent)) {
        Math_ApproachF(&player->actor.world.pos.x, this->dyna.actor.world.pos.x, 0.3f, 10.0f);
        Math_ApproachF(&player->actor.world.pos.y, this->dyna.actor.world.pos.y, 0.3f, 10.0f);
        Math_ApproachF(&player->actor.world.pos.z, this->dyna.actor.world.pos.z, 0.3f, 10.0f);
    }

    if (curFrame >= this->animEndFrame) {
        EnRaf_SetupChew(this);
    }
}

void EnRaf_SetupChew(EnRaf* this) {
    s32 i;

    EnRaf_ChangeAnim(this, CARNIVOROUS_LILY_ANIM_CHEW);
    this->chewCount = 0;
    for (i = 0; i < ARRAY_COUNT(this->chewLimbRot); i++) {
        this->chewLimbRot[i].x = Rand_S16Offset(8, 8) << 8;
        this->chewLimbRot[i].y = Rand_S16Offset(8, 8) << 8;
        this->chewLimbRot[i].z = Rand_S16Offset(8, 8) << 8;
    }

    this->petalScaleType = CARNIVOROUS_LILY_PETAL_SCALE_TYPE_CHEW;
    this->action = CARNIVOROUS_LILY_ACTION_CHEW;
    this->actionFunc = EnRaf_Chew;
}

/**
 * Chews the player or explosive that was grabbed. If it grabbed a non-Goron player, it will deal
 * damage to them and eventually throw them. If it grabbed a Goron player or explosive, it will
 * instead explode after chewing them a bit.
 */
void EnRaf_Chew(EnRaf* this, PlayState* play) {
    f32 targetChewScale;
    f32 curFrame;
    Player* player = GET_PLAYER(play);

    curFrame = this->skelAnime.curFrame;
    targetChewScale = (BREG(51) / 100.0f) + 0.2f;
    Math_ApproachF(&this->chewScale, targetChewScale, 0.2f, 0.03f);

    if ((player->stateFlags2 & PLAYER_STATE2_80) && (this->grabTarget != CARNIVOROUS_LILY_GRAB_TARGET_EXPLOSIVE) &&
        (&this->dyna.actor == player->actor.parent)) {
        Math_ApproachF(&player->actor.world.pos.x, this->dyna.actor.world.pos.x, 0.3f, 10.0f);
        Math_ApproachF(&player->actor.world.pos.y, this->dyna.actor.world.pos.y, 0.3f, 10.0f);
        Math_ApproachF(&player->actor.world.pos.z, this->dyna.actor.world.pos.z, 0.3f, 10.0f);
    }

    if (curFrame >= this->animEndFrame) {
        if (BREG(52) == 0) {
            this->chewCount++;
        }

        Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_SUISEN_EAT);
        switch (this->grabTarget) {
            case CARNIVOROUS_LILY_GRAB_TARGET_PLAYER:
                play->damagePlayer(play, -2);

                //! @bug: This function should only pass Player*: it uses *(this + 0x153), which is meant to be
                //! player->currentMask, but in this case is padding in `DynaPolyActor`
                Player_PlaySfx((Player*)&this->dyna.actor,
                               player->ageProperties->voiceSfxIdOffset + NA_SE_VO_LI_DAMAGE_S);
                CollisionCheck_GreenBlood(play, NULL, &player->actor.world.pos);
                if ((this->chewCount > (BREG(53) + 5)) || !(player->stateFlags2 & PLAYER_STATE2_80)) {
                    player->actor.freezeTimer = 10;
                    EnRaf_SetupThrow(this, play);
                    return;
                }
                break;

            case CARNIVOROUS_LILY_GRAB_TARGET_EXPLOSIVE:
                Actor_ApplyDamage(&this->dyna.actor);
                if (this->chewCount > (BREG(54) + 4)) {
                    EnRaf_Explode(this, play);
                    return;
                }
                break;

            case CARNIVOROUS_LILY_GRAB_TARGET_GORON_PLAYER:
                if (this->chewCount > (BREG(54) + 4)) {
                    player->actor.parent = NULL;
                    player->av2.actionVar2 = 1000;
                    EnRaf_Explode(this, play);
                }
                break;

            default:
                break;
        }
    }
}

void EnRaf_SetupThrow(EnRaf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    EnRaf_ChangeAnim(this, CARNIVOROUS_LILY_ANIM_SPIT);
    player->actor.freezeTimer = 10;
    this->petalScaleType = CARNIVOROUS_LILY_PETAL_SCALE_TYPE_IDLE_OR_THROW;
    this->action = CARNIVOROUS_LILY_ACTION_THROW;
    this->actionFunc = EnRaf_Throw;
}

/**
 * Spits out the grabbed player.
 */
void EnRaf_Throw(EnRaf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame = this->skelAnime.curFrame;

    if (Animation_OnFrame(&this->skelAnime, 10.0f)) {
        player->actor.freezeTimer = 0;
        player->actor.parent = NULL;
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_SUISEN_THROW);
        func_800B8D50(play, &this->dyna.actor, BREG(55) + 3.0f, this->playerRotYWhenGrabbed + 0x8000, BREG(56) + 10.0f,
                      0);
    } else if (curFrame < 10.0f) {
        player->actor.freezeTimer = 10;
    }

    if (curFrame >= this->animEndFrame) {
        this->petalScaleType = CARNIVOROUS_LILY_PETAL_SCALE_TYPE_IDLE_OR_THROW;
        this->action = CARNIVOROUS_LILY_ACTION_IDLE;
        this->timer = 20;
        this->actionFunc = EnRaf_Idle;
    }
}

/**
 * Creates an explosion effect/sound and spawns effects.
 */
void EnRaf_Explode(EnRaf* this, PlayState* play) {
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f explosionPos;
    s32 i;
    s32 pad;

    this->action = CARNIVOROUS_LILY_ACTION_EXPLODE;
    Math_Vec3f_Copy(&explosionPos, &this->dyna.actor.world.pos);
    explosionPos.y += 10.0f;
    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, explosionPos.x, explosionPos.y, explosionPos.z, 0, 0, 0,
                CLEAR_TAG_PARAMS(CLEAR_TAG_SMALL_EXPLOSION));
    Actor_PlaySfx(&this->dyna.actor, NA_SE_IT_BOMB_EXPLOSION);
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_SUISEN_DEAD);
    if (this->switchFlag > SWITCH_FLAG_NONE) {
        Flags_SetSwitch(play, this->switchFlag);
    }

    this->petalScaleType = CARNIVOROUS_LILY_PETAL_SCALE_TYPE_DEAD;
    for (i = 0; i < BREG(57) + 30; i++) {
        accel.x = (Rand_ZeroOne() - 0.5f) * 0.5f;
        accel.y = -0.3f;
        accel.z = (Rand_ZeroOne() - 0.5f) * 0.5f;
        velocity.x = Rand_ZeroOne() - 0.5f;
        velocity.y = Rand_ZeroOne() * 10.0f;
        velocity.z = Rand_ZeroOne() - 0.5f;
        EnRaf_InitializeEffect(this, &this->dyna.actor.world.pos, &velocity, &accel,
                               (Rand_ZeroFloat(1.0f) / 500.0f) + 0.002f, 90);
    }

    for (i = CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_LOWER_SEGMENT; i <= CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_UPPER_SEGMENT; i++) {
        Math_Vec3f_Copy(&this->targetLimbScale[i], &gZeroVec3f);
    }

    this->timer = 5;
    if (this->grabTarget == CARNIVOROUS_LILY_GRAB_TARGET_EXPLOSIVE) {
        Actor_ChangeCategory(play, &play->actorCtx, &this->dyna.actor, ACTORCAT_ENEMY);
        this->dyna.actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
    }

    this->actionFunc = EnRaf_PostDetonation;
}

/**
 * Switches the lily pad to the "dead" state once the timer reaches 0.
 */
void EnRaf_PostDetonation(EnRaf* this, PlayState* play) {
    if (this->timer == 0) {
        this->collider.dim.radius = 50;
        this->collider.dim.height = 10;
        Actor_ChangeCategory(play, &play->actorCtx, &this->dyna.actor, ACTORCAT_PROP);
        this->dyna.actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
        EnRaf_SetupDormant(this);
    } else if (this->grabTarget == CARNIVOROUS_LILY_GRAB_TARGET_EXPLOSIVE) {
        this->collider.dim.radius = 80;
        this->collider.dim.height = 50;
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnRaf_SetupConvulse(EnRaf* this) {
    EnRaf_ChangeAnim(this, CARNIVOROUS_LILY_ANIM_CONVULSE);
    this->chewCount = 0;
    this->action = CARNIVOROUS_LILY_ACTION_CONVULSE;
    this->actionFunc = EnRaf_Convulse;
}

/**
 * Plays the convulsing animation and sets the lily pad's switch flag to prevent it from
 * ever coming back to life. When the water in Woodfall Temple is purified, this function
 * and EnRaf_Dissolve are jointly responsible for controlling the lily pad's death.
 */
void EnRaf_Convulse(EnRaf* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (curFrame >= this->animEndFrame) {
        this->chewCount++;
        if (this->chewCount > (BREG(2) + 2)) {
            if (this->switchFlag > SWITCH_FLAG_NONE) {
                Flags_SetSwitch(play, this->switchFlag);
            }

            EnRaf_SetupDissolve(this);
        }
    }
}

void EnRaf_SetupDissolve(EnRaf* this) {
    EnRaf_ChangeAnim(this, CARNIVOROUS_LILY_ANIM_DEATH);
    this->action = CARNIVOROUS_LILY_ACTION_DISSOLVE;
    this->dissolveTimer = 0;
    this->actionFunc = EnRaf_Dissolve;
}

/**
 * Makes the trap petals on the lily pad dissolve and switches the lily pad to the "dead" state.
 * When the water in Woodfall Temple is purified, this function and EnRaf_Convulse are jointly
 * responsible for controlling the lily pad's death.
 */
void EnRaf_Dissolve(EnRaf* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    s32 i;

    if (curFrame >= this->animEndFrame) {
        this->dissolveTimer++;
        if (this->dissolveTimer < (BREG(3) + 105)) {
            for (i = 0; i < (BREG(4) + 5); i++) {
                //#region 2S2H [Port] Instead of directly zeroing the texture pixels, set a texture mask.
                // Applies to both loops.
                EnRaf_ClearPixelPetal(sCurPetalMask, sPetalClearPixelTableFirstPass,
                                      this->petalClearPixelFirstPassIndex);
                EnRaf_ClearPixelTeeth(sCurTeethMask, sTeethClearPixelTableFirstPass,
                                      this->teethClearPixelFirstPassIndex);
                if (this->petalClearPixelFirstPassIndex < (16 * 32)) {
                    this->petalClearPixelFirstPassIndex++;
                }

                if (this->teethClearPixelFirstPassIndex < (8 * 8)) {
                    this->teethClearPixelFirstPassIndex++;
                }
            }
        }
    }

    if (this->dissolveTimer > (BREG(5) + 50)) {
        for (i = 0; i < (BREG(6) + 5); i++) {
            EnRaf_ClearPixelPetal(sCurPetalMask, sPetalClearPixelTableSecondPass, this->petalClearPixelSecondPassIndex);
            EnRaf_ClearPixelTeeth(sCurTeethMask, sTeethClearPixelTableSecondPass, this->teethClearPixelSecondPassIndex);
            if (this->petalClearPixelSecondPassIndex < (16 * 32)) {
                this->petalClearPixelSecondPassIndex++;
            }

            if (this->teethClearPixelSecondPassIndex < (8 * 8)) {
                this->teethClearPixelSecondPassIndex++;
            }
        }
    }

    if (this->dissolveTimer > (BREG(7) + 160)) {
        for (i = CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_LOWER_SEGMENT; i <= CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_UPPER_SEGMENT;
             i++) {
            Math_Vec3f_Copy(&this->limbScale[i], &gZeroVec3f);
            Math_Vec3f_Copy(&this->targetLimbScale[i], &gZeroVec3f);
        }

        EnRaf_SetupDormant(this);
    }
}

void EnRaf_SetupDormant(EnRaf* this) {
    if (this->action == CARNIVOROUS_LILY_ACTION_EXPLODE) {
        this->timer = 90;
    } else {
        this->action = CARNIVOROUS_LILY_ACTION_DORMANT;
    }

    this->actionFunc = EnRaf_Dormant;
}

/**
 * Simply sits around doing nothing. If the revive timer is non-zero, then this function
 * will decrement the revive timer and revive the trap petals once it reaches 0.
 */
void EnRaf_Dormant(EnRaf* this, PlayState* play) {
    Vec3f targetLimbScale = { 1.0f, 1.0f, 1.0f };
    s32 i;

    if (this->timer == 0) {
        this->action = CARNIVOROUS_LILY_ACTION_DORMANT;
    }

    if (this->reviveTimer >= 0) {
        DECR(this->reviveTimer);

        if (this->reviveTimer == 0) {
            EnRaf_ChangeAnim(this, CARNIVOROUS_LILY_ANIM_SPIT);

            for (i = CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_LOWER_SEGMENT;
                 i <= CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_UPPER_SEGMENT; i++) {
                Math_Vec3f_Copy(&this->targetLimbScale[i], &targetLimbScale);
            }

            this->petalScaleType = CARNIVOROUS_LILY_PETAL_SCALE_TYPE_IDLE_OR_THROW;
            this->action = CARNIVOROUS_LILY_ACTION_IDLE;
            this->reviveTimer = CARNIVOROUS_LILY_GET_REVIVE_TIMER(&this->dyna.actor);
            this->reviveTimer += 30;
            this->actionFunc = EnRaf_Idle;
        }
    }
}

void EnRaf_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnRaf* this = (EnRaf*)thisx;
    WaterBox* waterBox;
    f32 ySurface;
    Vec3f ripplePos;
    s32 i;

    SkelAnime_Update(&this->skelAnime);
    DECR(this->rippleTimer);
    DECR(this->timer);
    this->actionFunc(this, play);

    if ((this->action == CARNIVOROUS_LILY_ACTION_IDLE) && CHECK_WEEKEVENTREG(WEEKEVENTREG_12_01)) {
        this->petalScaleType = CARNIVOROUS_LILY_PETAL_SCALE_TYPE_DEAD;
        EnRaf_SetupConvulse(this);
        return;
    }

    if (DynaPolyActor_IsPlayerOnTop(&this->dyna)) {
        if ((this->heightDiffFromPlayer > -0.1f) && !this->isCurrentlyInRidingMovingState) {
            this->heightDiffFromPlayer = -20.0f;
            this->isCurrentlyInRidingMovingState = true;
            Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_COMMON_WATER_MID);
        }
    } else {
        this->isCurrentlyInRidingMovingState = false;
    }

    this->bobPhase += 3000.0f;
    this->bobOffset = 2.0f * Math_SinS(this->bobPhase);

    if (this->mainType != CARNIVOROUS_LILY_TYPE_NO_WATER_INTERACTIONS) {
        ySurface = BREG(60) + (this->dyna.actor.world.pos.y - 60.0f);
        if (WaterBox_GetSurface1(play, &play->colCtx, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.z,
                                 &ySurface, &waterBox)) {
            ySurface -= this->bobOffset + BREG(59);
            Math_ApproachF(&this->dyna.actor.world.pos.y, this->heightDiffFromPlayer + ySurface, 0.5f, 40.0f);
            if (this->rippleTimer == 0) {
                this->rippleTimer = 30;
                if (this->petalScaleType == CARNIVOROUS_LILY_PETAL_SCALE_TYPE_CHEW) {
                    this->rippleTimer = 10;
                }

                Math_Vec3f_Copy(&ripplePos, &this->dyna.actor.world.pos);
                ripplePos.y = ySurface;
                EffectSsGRipple_Spawn(play, &ripplePos, 650, 3150, 0);
            }
        }
    } else {
        Math_ApproachF(&this->dyna.actor.world.pos.y,
                       (this->dyna.actor.home.pos.y + this->heightDiffFromPlayer) - this->bobOffset, 0.5f, 40.0f);
    }

    Math_ApproachZeroF(&this->heightDiffFromPlayer, 0.3f, 2.0f);

    if (this->action == CARNIVOROUS_LILY_ACTION_EXPLODE) {
        EnRaf_UpdateEffects(this, play);
    }

    for (i = 0; i < ARRAY_COUNT(this->limbScale); i++) {
        if (this->action < CARNIVOROUS_LILY_ACTION_EXPLODE) {
            Math_ApproachF(&this->limbScale[i].x, this->targetLimbScale[i].x, 0.4f, 0.5f);
            Math_ApproachF(&this->limbScale[i].y, this->targetLimbScale[i].y, 0.4f, 0.5f);
            Math_ApproachF(&this->limbScale[i].z, this->targetLimbScale[i].z, 0.4f, 0.5f);
        } else {
            Math_ApproachF(&this->limbScale[i].x, this->targetLimbScale[i].x, 1.0f, 1.0f);
            Math_ApproachF(&this->limbScale[i].y, this->targetLimbScale[i].y, 1.0f, 1.0f);
            Math_ApproachF(&this->limbScale[i].z, this->targetLimbScale[i].z, 1.0f, 1.0f);
        }
    }

    Collider_UpdateCylinder(&this->dyna.actor, &this->collider);
    if (this->action < CARNIVOROUS_LILY_ACTION_EXPLODE) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

static s16 sGrabAnimationCheckFrames[] = { 0, 4, 6 };

static Vec3f sMiddleSegmentTargetScaleDuringGrab[] = {
    { 1.0f, 1.0f, 1.0f },
    { 1.0f, 2.0f, 1.0f },
    { 0.0f, 1.5f, 0.7f },
};

static Vec3f sUpperSegmentTargetScaleDuringGrab[] = {
    { 1.0f, 1.0f, 1.0f },
    { 3.0f, 2.0f, 1.5f },
    { 1.5f, 1.2f, 0.8f },
};

static s16 sSpitAnimationCheckFrames[] = { 0, 7, 9, 13, 19 };

static Vec3f sMiddleSegmentTargetScaleDuringSpit[] = {
    { 1.0f, 1.5f, 0.7f }, { 1.0f, 2.0f, 1.5f }, { 1.0f, 2.0f, 0.5f }, { 1.0f, 2.0f, 0.5f }, { 1.0f, 1.0f, 1.0f },
};

static Vec3f sUpperSegmentTargetScaleDuringSpit[] = {
    { 1.5f, 1.5f, 1.7f }, { 1.5f, 1.5f, 1.3f }, { 3.0f, 1.0f, 0.5f }, { 1.0f, 1.0f, 0.5f }, { 1.0f, 1.0f, 1.0f },
};

void EnRaf_TransformLimbDraw(PlayState* play2, s32 limbIndex, Actor* thisx) {
    PlayState* play = play2;
    EnRaf* this = (EnRaf*)thisx;
    s32 i;

    switch (this->petalScaleType) {
        case CARNIVOROUS_LILY_PETAL_SCALE_TYPE_GRAB:
            if ((limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_MIDDLE_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_MIDDLE_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_2_MIDDLE_SEGMENT)) {
                for (i = 0; i < ARRAY_COUNT(sGrabAnimationCheckFrames); i++) {
                    if (TRUNCF_BINANG(this->skelAnime.curFrame) == sGrabAnimationCheckFrames[i]) {
                        Math_Vec3f_Copy(&this->targetLimbScale[limbIndex], &sMiddleSegmentTargetScaleDuringGrab[i]);
                    }
                }
            }

            if ((limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_UPPER_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_UPPER_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_2_UPPER_SEGMENT)) {
                for (i = 0; i < ARRAY_COUNT(sGrabAnimationCheckFrames); i++) {
                    if (TRUNCF_BINANG(this->skelAnime.curFrame) == sGrabAnimationCheckFrames[i]) {
                        Math_Vec3f_Copy(&this->targetLimbScale[limbIndex], &sUpperSegmentTargetScaleDuringGrab[i]);
                    }
                }
            }
            break;

        case CARNIVOROUS_LILY_PETAL_SCALE_TYPE_CHEW:
            if ((limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_MIDDLE_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_MIDDLE_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_2_MIDDLE_SEGMENT)) {
                Math_Vec3f_Copy(&this->targetLimbScale[limbIndex], &sMiddleSegmentTargetScaleDuringGrab[2]);
            } else if ((limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_UPPER_SEGMENT) ||
                       (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_UPPER_SEGMENT) ||
                       (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_2_UPPER_SEGMENT)) {
                Math_Vec3f_Copy(&this->targetLimbScale[limbIndex], &sUpperSegmentTargetScaleDuringGrab[2]);
            }

            // These matrix operations make the trap petals look a bit more "wobbly" as it chews
            // by stretching the limbs in various random directions.
            if ((limbIndex > CARNIVOROUS_LILY_PAD_LIMB_FLOWER) && (limbIndex < CARNIVOROUS_LILY_PAD_LIMB_ROOTS)) {
                Matrix_RotateYS((this->chewLimbRot[limbIndex].y * play->gameplayFrames), MTXMODE_APPLY);
                Matrix_RotateXS((this->chewLimbRot[limbIndex].x * play->gameplayFrames), MTXMODE_APPLY);
                Matrix_RotateZS((this->chewLimbRot[limbIndex].z * play->gameplayFrames), MTXMODE_APPLY);
                Matrix_Scale(this->chewScale + 1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
                Matrix_RotateZS(-(this->chewLimbRot[limbIndex].z * play->gameplayFrames), MTXMODE_APPLY);
                Matrix_RotateXS(-(this->chewLimbRot[limbIndex].x * play->gameplayFrames), MTXMODE_APPLY);
                Matrix_RotateYS(-(this->chewLimbRot[limbIndex].y * play->gameplayFrames), MTXMODE_APPLY);
            }
            break;

        case CARNIVOROUS_LILY_PETAL_SCALE_TYPE_IDLE_OR_THROW:
            if ((limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_MIDDLE_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_MIDDLE_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_2_MIDDLE_SEGMENT)) {
                for (i = 0; i < ARRAY_COUNT(sSpitAnimationCheckFrames); i++) {
                    if (TRUNCF_BINANG(this->skelAnime.curFrame) == sSpitAnimationCheckFrames[i]) {
                        Math_Vec3f_Copy(&this->targetLimbScale[limbIndex], &sMiddleSegmentTargetScaleDuringSpit[i]);
                    }
                }
            }

            if ((limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_1_UPPER_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_3_UPPER_SEGMENT) ||
                (limbIndex == CARNIVOROUS_LILY_PAD_LIMB_TRAP_2_UPPER_SEGMENT)) {
                for (i = 0; i < 4; i++) {
                    if (TRUNCF_BINANG(this->skelAnime.curFrame) == sSpitAnimationCheckFrames[i]) {
                        Math_Vec3f_Copy(&this->targetLimbScale[limbIndex], &sUpperSegmentTargetScaleDuringSpit[i]);
                    }
                }
            }
            break;

        default:
            break;
    }

    Matrix_Scale(this->limbScale[limbIndex].x, this->limbScale[limbIndex].y, this->limbScale[limbIndex].z,
                 MTXMODE_APPLY);
}

void EnRaf_Draw(Actor* thisx, PlayState* play) {
    EnRaf* this = (EnRaf*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    OPEN_DISPS(play->state.gfxCtx);

    // #2S2H [Port] Invalidate the blend masks when they are set in the cutscene
    if (this->dissolveTimer < (BREG(3) + 105) || (this->dissolveTimer > (BREG(5) + 50))) {
        gSPInvalidateTexCache(POLY_OPA_DISP++, sCurTeethMask);
        gSPInvalidateTexCache(POLY_OPA_DISP++, sCurPetalMask);
    }

    CLOSE_DISPS(play->state.gfxCtx);

    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, NULL, NULL, EnRaf_TransformLimbDraw, &this->dyna.actor);

    if (this->action == CARNIVOROUS_LILY_ACTION_EXPLODE) {
        EnRaf_DrawEffects(this, play);
    }
}

void EnRaf_InitializeEffect(EnRaf* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale, s16 timer) {
    s16 i;
    CarnivorousLilyEffect* effect = this->effects;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (!effect->isEnabled) {
            effect->isEnabled = true;
            effect->pos = *pos;
            effect->velocity = *velocity;
            effect->accel = *accel;
            effect->scale = scale;
            effect->timer = timer;
            effect->rot.x = Rand_CenteredFloat(30000.0f);
            effect->rot.y = Rand_CenteredFloat(30000.0f);
            effect->rot.z = Rand_CenteredFloat(30000.0f);
            return;
        }
    }
}

void EnRaf_UpdateEffects(EnRaf* this, PlayState* play) {
    s32 i;
    CarnivorousLilyEffect* effect = this->effects;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->isEnabled) {
            effect->pos.x += effect->velocity.x;
            effect->pos.y += effect->velocity.y;
            effect->pos.z += effect->velocity.z;
            effect->rot.x += 0xBB8;
            effect->rot.y += 0xBB8;
            effect->rot.z += 0xBB8;
            effect->velocity.x += effect->accel.x;
            effect->velocity.y += effect->accel.y;
            effect->velocity.z += effect->accel.z;

            if (this->mainType != CARNIVOROUS_LILY_TYPE_NO_WATER_INTERACTIONS) {
                if (effect->pos.y < (this->dyna.actor.world.pos.y - 10.0f)) {
                    EffectSsGSplash_Spawn(play, &effect->pos, NULL, NULL, 0, effect->scale * 200000.0f);
                    SoundSource_PlaySfxAtFixedWorldPos(play, &effect->pos, 50, NA_SE_EV_BOMB_DROP_WATER);
                    effect->isEnabled = false;
                }
            } else if (effect->pos.y < (this->dyna.actor.world.pos.y - 10.0f)) {
                Math_ApproachZeroF(&effect->scale, 0.2f, 0.001f);
                if (effect->scale <= 0.0001f) {
                    effect->timer = 0;
                }
            }

            if (effect->timer != 0) {
                effect->timer--;
            } else {
                effect->isEnabled = false;
            }
        }
    }
}

void EnRaf_DrawEffects(EnRaf* this, PlayState* play) {
    s16 i;
    CarnivorousLilyEffect* effect = this->effects;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->isEnabled) {
            FrameInterpolation_RecordOpenChild(effect, i);
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            Matrix_RotateXS(effect->rot.x, MTXMODE_APPLY);
            Matrix_RotateYS(effect->rot.y, MTXMODE_APPLY);
            Matrix_RotateZS(effect->rot.z, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, gCarnivorousLilyPadParticleDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(gfxCtx);
}

void EnRaf_Reset(void) {
    Gfx_UnregisterBlendedTexture(gCarnivorousLilyPadTrapPetalTex);
    Gfx_UnregisterBlendedTexture(gCarnivorousLilyPadTrapTeethTex);

    Gfx_TextureCacheDelete(sCurPetalMask);
    Gfx_TextureCacheDelete(sCurTeethMask);

    memset(sCurPetalMask, 0, sizeof(sCurPetalMask));
    memset(sCurTeethMask, 0, sizeof(sCurTeethMask));
}
