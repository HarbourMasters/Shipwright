/*
 * File: z_en_dragon.c
 * Overlay: ovl_En_Dragon
 * Description: Deep Python
 */

#include "z_en_dragon.h"
#include "overlays/actors/ovl_En_Ot/z_en_ot.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnDragon_Init(Actor* thisx, PlayState* play);
void EnDragon_Destroy(Actor* thisx, PlayState* play);
void EnDragon_Update(Actor* thisx, PlayState* play);
void EnDragon_Draw(Actor* thisx, PlayState* play);
void EnDragon_Reset(void);

void EnDragon_SetupRetreatOrIdle(EnDragon* this);
void EnDragon_RetreatOrIdle(EnDragon* this, PlayState* play);
void EnDragon_SetupExtend(EnDragon* this);
void EnDragon_Extend(EnDragon* this, PlayState* play);
void EnDragon_Grab(EnDragon* this, PlayState* play);
void EnDragon_SetupAttack(EnDragon* this);
void EnDragon_Attack(EnDragon* this, PlayState* play);
void EnDragon_Dead(EnDragon* this, PlayState* play);

typedef enum {
    /* 0 */ DEEP_PYTHON_EXTEND_STATE_NOT_FULLY_EXTENDED,
    /* 1 */ DEEP_PYTHON_EXTEND_STATE_FULLY_EXTENDED,
    /* 2 */ DEEP_PYTHON_EXTEND_STATE_REPEAT_LARGE_SWAY,
    /* 3 */ DEEP_PYTHON_EXTEND_STATE_REPEAT_SMALL_SWAY
} DeepPythonExtendState;

typedef enum {
    /* 0 */ DEEP_PYTHON_GRAB_STATE_START,
    /* 1 */ DEEP_PYTHON_GRAB_STATE_GRABBED
} DeepPythonGrabState;

typedef enum {
    /* 0 */ DEEP_PYTHON_ATTACK_STATE_START,
    /* 1 */ DEEP_PYTHON_ATTACK_STATE_RELEASED
} DeepPythonAttackState;

static s32 sNumPythonsDead = 0;

ActorProfile En_Dragon_Profile = {
    /**/ ACTOR_EN_DRAGON,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_UTUBO,
    /**/ sizeof(EnDragon),
    /**/ EnDragon_Init,
    /**/ EnDragon_Destroy,
    /**/ EnDragon_Update,
    /**/ EnDragon_Draw,
    /**/ EnDragon_Reset,
};

typedef enum {
    /* 0x0 */ DEEP_PYTHON_DMGEFF_NONE,
    /* 0xF */ DEEP_PYTHON_DMGEFF_DAMAGE = 0xF
} DeepPythonDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Deku Stick     */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, DEEP_PYTHON_DMGEFF_DAMAGE),
    /* Zora boomerang */ DMG_ENTRY(1, DEEP_PYTHON_DMGEFF_DAMAGE),
    /* Normal arrow   */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Goron punch    */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Ice arrow      */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Light arrow    */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Goron spikes   */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Zora barrier   */ DMG_ENTRY(1, DEEP_PYTHON_DMGEFF_DAMAGE),
    /* Normal shield  */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, DEEP_PYTHON_DMGEFF_DAMAGE),
    /* Spin attack    */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, DEEP_PYTHON_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, DEEP_PYTHON_DMGEFF_DAMAGE),
};

static ColliderJntSphElementInit sJntSphElementsInit[8] = {
    {
        {
            ELEM_MATERIAL_UNK1,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { DEEP_PYTHON_LIMB_HEAD, { { 0, 0, 0 }, 0 }, 1 },
    },
    {
        {
            ELEM_MATERIAL_UNK1,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { DEEP_PYTHON_LIMB_COLLAR, { { 0, 0, 0 }, 0 }, 1 },
    },
    {
        {
            ELEM_MATERIAL_UNK1,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { DEEP_PYTHON_LIMB_COLLAR, { { 0, 0, 0 }, 0 }, 1 },
    },
    {
        {
            ELEM_MATERIAL_UNK1,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { DEEP_PYTHON_LIMB_BODY_SEGMENT_1, { { 0, 0, 0 }, 0 }, 1 },
    },
    {
        {
            ELEM_MATERIAL_UNK1,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { DEEP_PYTHON_LIMB_BODY_SEGMENT_1, { { 0, 0, 0 }, 0 }, 1 },
    },
    {
        {
            ELEM_MATERIAL_UNK1,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { DEEP_PYTHON_LIMB_BODY_SEGMENT_2, { { 0, 0, 0 }, 0 }, 1 },
    },
    {
        {
            ELEM_MATERIAL_UNK1,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { DEEP_PYTHON_LIMB_BODY_SEGMENT_2, { { 0, 0, 0 }, 0 }, 1 },
    },
    {
        {
            ELEM_MATERIAL_UNK1,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { DEEP_PYTHON_LIMB_BODY_SEGMENT_2, { { 0, 0, 0 }, 0 }, 1 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

void EnDragon_Init(Actor* thisx, PlayState* play) {
    EnDragon* this = (EnDragon*)thisx;

    SkelAnime_InitFlex(play, &this->skelAnime, &gDeepPythonSkel, &gDeepPythonSmallSideSwayAnim, this->jointTable,
                       this->morphTable, DEEP_PYTHON_LIMB_MAX);

    this->actor.colChkInfo.health = 4;
    this->actor.colChkInfo.damageTable = &sDamageTable;
    this->actor.attentionRangeType = ATTENTION_RANGE_10;
    Collider_InitAndSetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElements);

    this->collider.elements[0].dim.scale = this->collider.elements[1].dim.scale = this->collider.elements[2].dim.scale =
        this->collider.elements[3].dim.scale = this->collider.elements[4].dim.scale =
            this->collider.elements[5].dim.scale = this->collider.elements[6].dim.scale =
                this->collider.elements[7].dim.scale = 1.0f;

    this->collider.elements[0].dim.modelSphere.radius = 150;
    this->collider.elements[0].dim.modelSphere.center.x = 420;
    this->collider.elements[1].dim.modelSphere.radius = 160;
    this->collider.elements[1].dim.modelSphere.center.x = 630;
    this->collider.elements[2].dim.modelSphere.radius = 130;
    this->collider.elements[2].dim.modelSphere.center.x = 630;
    this->collider.elements[3].dim.modelSphere.radius = 170;
    this->collider.elements[3].dim.modelSphere.center.x = 920;
    this->collider.elements[4].dim.modelSphere.radius = 150;
    this->collider.elements[4].dim.modelSphere.center.x = 530;
    this->collider.elements[5].dim.modelSphere.radius = 140;
    this->collider.elements[5].dim.modelSphere.center.x = 730;
    this->collider.elements[6].dim.modelSphere.radius = 120;
    this->collider.elements[6].dim.modelSphere.center.x = 430;
    this->collider.elements[7].dim.modelSphere.radius = 110;
    this->collider.elements[7].dim.modelSphere.center.x = 160;

    this->pythonIndex = EN_DRAGON_GET_PYTHON_INDEX(&this->actor);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->action = DEEP_PYTHON_ACTION_IDLE;
    this->actor.hintId = TATL_HINT_ID_DEEP_PYTHON;
    this->scale = 0.5f;
    this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;

    EnDragon_SetupRetreatOrIdle(this);
}

void EnDragon_Destroy(Actor* thisx, PlayState* play) {
    EnDragon* this = (EnDragon*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

typedef enum {
    /* 0 */ DEEP_PYTHON_ANIM_SMALL_SIDE_SWAY,
    /* 1 */ DEEP_PYTHON_ANIM_LARGE_SIDE_SWAY,
    /* 2 */ DEEP_PYTHON_ANIM_VERTICAL_SWAY,
    /* 3 */ DEEP_PYTHON_ANIM_IDLE,
    /* 4 */ DEEP_PYTHON_ANIM_MAX
} DeepPythonAnimation;

static AnimationHeader* sAnimations[DEEP_PYTHON_ANIM_MAX] = {
    &gDeepPythonSmallSideSwayAnim, // DEEP_PYTHON_ANIM_SMALL_SIDE_SWAY
    &gDeepPythonLargeSideSwayAnim, // DEEP_PYTHON_ANIM_LARGE_SIDE_SWAY
    &gDeepPythonVerticalSwayAnim,  // DEEP_PYTHON_ANIM_VERTICAL_SWAY
    &gDeepPythonSmallSideSwayAnim, // DEEP_PYTHON_ANIM_IDLE
};

static u8 sAnimationModes[DEEP_PYTHON_ANIM_MAX] = {
    ANIMMODE_LOOP, // DEEP_PYTHON_ANIM_SMALL_SIDE_SWAY
    ANIMMODE_LOOP, // DEEP_PYTHON_ANIM_LARGE_SIDE_SWAY
    ANIMMODE_ONCE, // DEEP_PYTHON_ANIM_VERTICAL_SWAY
    ANIMMODE_ONCE, // DEEP_PYTHON_ANIM_IDLE
};

void EnDragon_ChangeAnim(EnDragon* this, s32 animIndex) {
    f32 startFrame;

    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);
    startFrame = 0.0f;
    if (this->animIndex == DEEP_PYTHON_ANIM_IDLE) {
        startFrame = this->animEndFrame;
    }

    Animation_Change(&this->skelAnime, sAnimations[animIndex], 1.0f, startFrame, this->animEndFrame,
                     sAnimationModes[this->animIndex], -4.0f);
}

static Color_RGBA8 sBubblePrimColors[] = {
    { 255, 255, 255, 255 },
    { 150, 255, 255, 255 },
    { 100, 255, 255, 255 },
};

static Color_RGBA8 sBubbleEnvColors[] = {
    { 150, 150, 150, 0 },
    { 0, 100, 0, 255 },
    { 0, 0, 255, 255 },
};

void EnDragon_SpawnBubbles(EnDragon* this, PlayState* play, Vec3f basePos) {
    static Vec3f sBubbleVelocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f sBubbleAccel = { 0.0f, 0.1f, 0.0f };
    s32 bubbleCount;
    s16 colorIndex;
    s16 scale;
    Vec3f bubblePos;
    s32 i;

    bubbleCount = (s32)Rand_CenteredFloat(5.0f) + 10;
    colorIndex = 0;
    if (this->action == DEEP_PYTHON_ACTION_DEAD) {
        colorIndex = 1;
        bubbleCount = (s32)Rand_CenteredFloat(5.0f) + 10;
    }

    for (i = 0; i < bubbleCount; i++) {
        Math_Vec3f_Copy(&bubblePos, &basePos);
        sBubbleVelocity.x = Rand_ZeroFloat(1.0f) * 23.0f;
        sBubbleVelocity.y = Rand_ZeroFloat(1.0f) * 10.0f;
        sBubbleVelocity.z = Rand_ZeroFloat(1.0f) * 23.0f;
        bubblePos.x += Rand_CenteredFloat(i * 30.0f);
        bubblePos.y += Rand_CenteredFloat(5.0f);
        bubblePos.z += Rand_CenteredFloat(i * 30.0f);
        sBubbleAccel.y = Rand_ZeroFloat(1.0f) * 20.0f * 3.0f;
        scale = Rand_S16Offset(380, 240);
        EffectSsDtBubble_SpawnCustomColor(play, &bubblePos, &sBubbleVelocity, &sBubbleAccel,
                                          &sBubblePrimColors[colorIndex], &sBubbleEnvColors[colorIndex], scale, 30,
                                          false);
    }
}

void EnDragon_RetreatOnceTimerEnds(EnDragon* this, PlayState* play) {
    if (this->timer == 0) {
        func_800B8D50(play, &this->actor, 10.0f, this->actor.world.rot.y, 10.0f, 8);
        EnDragon_SetupRetreatOrIdle(this);
    }
}

void EnDragon_SetupRetreatOrIdle(EnDragon* this) {
    EnDragon_ChangeAnim(this, DEEP_PYTHON_ANIM_IDLE);
    this->state = 0;
    this->unk_2CC = 0;
    this->hasGrabbedPlayer = false;
    this->grabTimer = 0;
    this->timer = 30;
    this->actionFunc = EnDragon_RetreatOrIdle;
}

void EnDragon_RetreatOrIdle(EnDragon* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->action == DEEP_PYTHON_ACTION_EXTEND) {
        EnDragon_SetupExtend(this);
    } else if ((this->timer != 0) && (fabsf(this->actor.world.pos.x - this->actor.home.pos.x) > 101.0f) &&
               (fabsf(this->actor.world.pos.z - this->actor.home.pos.z) > 101.0f)) {
        this->actor.speed = -100.0f;
    } else {
        this->actor.speed = 0.0f;

        if ((fabsf(this->actor.world.pos.x - this->actor.home.pos.x) > 4.0f) &&
            (fabsf(this->actor.world.pos.z - this->actor.home.pos.z) > 4.0f)) {
            Math_ApproachF(&this->actor.world.pos.x, this->actor.home.pos.x, 0.3f, 200.0f);
            Math_ApproachF(&this->actor.world.pos.z, this->actor.home.pos.z, 0.3f, 200.0f);
        } else if (this->action != DEEP_PYTHON_ACTION_IDLE) {
            this->action = DEEP_PYTHON_ACTION_IDLE;
        }
    }
}

void EnDragon_SetupExtend(EnDragon* this) {
    this->state = 0;
    this->behindJawRetreatTimer = this->state;
    Actor_PlaySfx(&this->actor, NA_SE_EN_UTSUBO_APPEAR_TRG);
    this->retreatTimer = 250;
    this->actionFunc = EnDragon_Extend;
}

void EnDragon_Extend(EnDragon* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame = this->skelAnime.curFrame;
    s16 yaw;

    EnDragon_SpawnBubbles(this, play, this->jawPos);

    if (this->action >= DEEP_PYTHON_ACTION_DAMAGE) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_UTSUBO_BACK);
        EnDragon_SetupRetreatOrIdle(this);
    } else if (this->retreatTimer == 0) {
        this->action = DEEP_PYTHON_ACTION_RETREAT;
        Actor_PlaySfx(&this->actor, NA_SE_EN_UTSUBO_BACK);
        EnDragon_SetupRetreatOrIdle(this);
    } else if (this->state == DEEP_PYTHON_EXTEND_STATE_NOT_FULLY_EXTENDED) {
        Vec3f extendedPos;

        Math_Vec3f_Copy(&extendedPos, &this->burrowEntrancePos);
        Actor_PlaySfx(&this->actor, NA_SE_EN_UTSUBO_APPEAR - SFX_FLAG);
        extendedPos.x += Math_SinS(this->actor.world.rot.y) * -530.0f;
        extendedPos.z += Math_CosS(this->actor.world.rot.y) * -530.0f;
        this->actor.speed = 40.0f;
        Math_SmoothStepToS(&this->jawZRotation, 0xFA0, 5, 0xBB8, 0x14);

        if ((fabsf(this->actor.world.pos.x - extendedPos.x) < 51.0f) &&
            (fabsf(this->actor.world.pos.z - extendedPos.z) < 51.0f)) {
            this->actor.speed = 0.0f;
            Math_ApproachF(&this->actor.world.pos.x, extendedPos.x, 0.3f, 50.0f);
            Math_ApproachF(&this->actor.world.pos.z, extendedPos.z, 0.3f, 50.0f);
            if ((fabsf(this->actor.world.pos.x - extendedPos.x) < 4.0f) &&
                (fabsf(this->actor.world.pos.z - extendedPos.z) < 4.0f)) {
                if (this->animIndex != DEEP_PYTHON_ANIM_LARGE_SIDE_SWAY) {
                    EnDragon_ChangeAnim(this, DEEP_PYTHON_ANIM_LARGE_SIDE_SWAY);
                }

                this->state = DEEP_PYTHON_EXTEND_STATE_FULLY_EXTENDED;
            }
        }
    } else {
        Math_SmoothStepToS(&this->jawZRotation, 0, 5, 0xBB8, 0x14);
        SkelAnime_Update(&this->skelAnime);
        if (this->state == DEEP_PYTHON_EXTEND_STATE_FULLY_EXTENDED) {
            if (curFrame < this->animEndFrame) {
                return;
            }

            this->state = DEEP_PYTHON_EXTEND_STATE_REPEAT_LARGE_SWAY;
        }

        yaw = ABS_ALT(BINANG_SUB(Math_Vec3f_Yaw(&this->jawPos, &player->actor.world.pos), this->actor.shape.rot.y));
        if (yaw < 0x5000) {
            // Player is in front of the jaw
            if ((curFrame >= this->animEndFrame) && (this->largeSwayWaitTimer == 0)) {
                if (this->animIndex != DEEP_PYTHON_ANIM_LARGE_SIDE_SWAY) {
                    EnDragon_ChangeAnim(this, DEEP_PYTHON_ANIM_LARGE_SIDE_SWAY);
                }

                this->state = DEEP_PYTHON_EXTEND_STATE_REPEAT_LARGE_SWAY;
            }

            this->behindJawRetreatTimer = 0;
        } else {
            // Player is in behind the jaw
            if (this->state == DEEP_PYTHON_EXTEND_STATE_REPEAT_LARGE_SWAY) {
                EnDragon_ChangeAnim(this, DEEP_PYTHON_ANIM_SMALL_SIDE_SWAY);
                this->largeSwayWaitTimer = Rand_ZeroFloat(20.0f) + this->animEndFrame;
                this->state = DEEP_PYTHON_EXTEND_STATE_REPEAT_SMALL_SWAY;
            }

            this->behindJawRetreatTimer++;
            if (this->behindJawRetreatTimer > 60) {
                this->action = DEEP_PYTHON_ACTION_RETREAT;
                Actor_PlaySfx(&this->actor, NA_SE_EN_UTSUBO_BACK);
                EnDragon_SetupRetreatOrIdle(this);
            }
        }
    }
}

void EnDragon_SetSubCamEyeAt(EnDragon* this, PlayState* play, Vec3f subCamEye, Vec3f subCamAt) {
    this->subCamId = CutsceneManager_GetCurrentSubCamId(this->actor.csId);
    Math_Vec3f_Copy(&this->subCamEye, &subCamEye);
    Math_Vec3f_Copy(&this->subCamAt, &subCamAt);
    Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
}

void EnDragon_SetupGrab(EnDragon* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f extendedPos;
    s16 yaw;

    if (!CutsceneManager_IsNext(this->grabCsId)) {
        CutsceneManager_Queue(this->grabCsId);
    } else {
        CutsceneManager_StartWithPlayerCs(this->grabCsId, &this->actor);
        Math_Vec3f_Copy(&extendedPos, &this->burrowEntrancePos);
        extendedPos.x += Math_SinS(this->actor.world.rot.y) * -530.0f;
        extendedPos.z += Math_CosS(this->actor.world.rot.y) * -530.0f;
        Math_Vec3f_Copy(&this->actor.world.pos, &extendedPos);
        yaw = Math_Vec3f_Yaw(&player->actor.world.pos, &this->jawPos);
        player->actor.shape.rot.y = yaw;
        player->actor.world.rot.y = yaw;
        this->state = DEEP_PYTHON_GRAB_STATE_START;
        this->grabTimer = 0;
        this->hasGrabbedPlayer = false;
        EnDragon_ChangeAnim(this, DEEP_PYTHON_ANIM_IDLE);
        this->actionFunc = EnDragon_Grab;
    }
}

static Vec3f sSubCamEyePerPython[] = {
    { 1600.0f, 0.0f, 1400.0f }, { 1400.0f, 0.0f, 400.0f },  { 1800.0f, 0.0f, 1400.f },  { 1100.0f, -200.0f, 1500.0f },
    { 2000.0f, 0.0f, 1500.0f }, { 1900.0f, 0.0f, 1800.0f }, { 1700.0f, 0.0f, 1100.0f }, { 1700.0f, 0.0f, 1100.0f },
};

static Vec3f sSubCamAtPerPython[] = {
    { 300.0f, -100.0f, 1300.0f }, { 1500.0f, 0.0f, 2400.0f }, { 300.0f, -100.0f, 1300.0f }, { 1900.0f, 500.0f, 600.0f },
    { -1000.0f, 0.0f, 1000.0f },  { 1200.0f, 0.0f, 1500.0f }, { 1100.0f, 0.0f, 2000.0f },   { 1100.0f, 0.0f, 2000.0f },
};

static s16 sZRotPerPython[] = { 0x07D0, 0x07D0, 0x07D0, 0x07D0, 0x07D0, 0x0BB8, 0x0000, 0x0000 };

static s32 sMaxGrabTimerPerPython[] = { 5, 5, 5, 4, 5, 8, 5, 5 };

void EnDragon_Grab(EnDragon* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f pos; // used as both the extended position and the camera eye
    Vec3f subCamAt;

    this->subCamId = CutsceneManager_GetCurrentSubCamId(this->actor.csId);
    SkelAnime_Update(&this->skelAnime);

    if (this->grabTimer == 0) {
        if (!this->hasGrabbedPlayer) {
            Math_Vec3f_Copy(&player->actor.world.pos, &this->playerGrabPosition);
            Math_Vec3f_Copy(&this->playerGrabPositionTemp, &this->playerGrabPosition);
            this->hasGrabbedPlayer = true;
        } else {
            Math_Vec3f_Copy(&player->actor.world.pos, &this->playerGrabPositionTemp);
        }

        Math_Vec3f_Copy(&pos, &this->burrowEntrancePos);
        pos.x += Math_SinS(this->actor.world.rot.y) * -930.0f;
        pos.z += Math_CosS(this->actor.world.rot.y) * -930.0f;
        Math_Vec3f_Copy(&this->actor.world.pos, &pos);
        this->jawZRotation = 0x1450;
        this->actor.speed = 60.0f;
    }

    this->grabTimer++;
    Math_SmoothStepToS(&this->actor.shape.rot.z, sZRotPerPython[this->pythonIndex], 0xA, 0x1F4, 0x14);
    EnDragon_SpawnBubbles(this, play, this->jawPos);

    Math_Vec3f_Copy(&pos, &this->burrowEntrancePos);

    pos.x += Math_SinS(this->actor.world.rot.y) * sSubCamEyePerPython[this->pythonIndex].x;
    pos.y += sSubCamEyePerPython[this->pythonIndex].y;
    pos.z += Math_CosS(this->actor.world.rot.y) * sSubCamEyePerPython[this->pythonIndex].z;

    Math_Vec3f_Copy(&subCamAt, &this->actor.world.pos);
    subCamAt.x += Math_SinS(this->actor.world.rot.y) * sSubCamAtPerPython[this->pythonIndex].x;
    subCamAt.y += sSubCamAtPerPython[this->pythonIndex].y;
    subCamAt.z += Math_CosS(this->actor.world.rot.y) * sSubCamAtPerPython[this->pythonIndex].z;

    EnDragon_SetSubCamEyeAt(this, play, pos, subCamAt);

    if (this->grabTimer > sMaxGrabTimerPerPython[this->pythonIndex]) {
        if (this->state == DEEP_PYTHON_GRAB_STATE_START) {
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
            this->state = DEEP_PYTHON_GRAB_STATE_GRABBED;
        }

        play->unk_18770(play, player);
        player->actor.parent = &this->actor;
        player->av2.actionVar2 = 50;
        this->action = DEEP_PYTHON_ACTION_GRAB;
        Actor_PlaySfx(&this->actor, NA_SE_EN_UTSUBO_EAT);
        EnDragon_SetupAttack(this);
    }
}

void EnDragon_SetupAttack(EnDragon* this) {
    if (this->animIndex != DEEP_PYTHON_ANIM_LARGE_SIDE_SWAY) {
        EnDragon_ChangeAnim(this, DEEP_PYTHON_ANIM_LARGE_SIDE_SWAY);
    }

    this->behindJawRetreatTimer = 0;
    this->grabTimer = 0;
    this->state = DEEP_PYTHON_ATTACK_STATE_START;
    this->actionFunc = EnDragon_Attack;
}

void EnDragon_Attack(EnDragon* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame = this->skelAnime.curFrame;
    Vec3f pos; // used as both the extended position and the camera eye
    Vec3f subCamAt;

    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->actor.shape.rot.z, 0, 0xA, 0x1388, 0);
    if ((play->gameplayFrames % 16) == 0) {
        play->damagePlayer(play, -2);

        //! @bug: This function should only pass Player*: it uses *(this + 0x153), which is meant to be
        //! player->currentMask, but in this case is garbage in the skelAnime
        Player_PlaySfx((Player*)&this->actor, player->ageProperties->voiceSfxIdOffset + NA_SE_VO_LI_DAMAGE_S);
        Actor_PlaySfx(&this->actor, NA_SE_EN_UTSUBO_BITE);
        CollisionCheck_GreenBlood(play, NULL, &player->actor.world.pos);
    }

    Math_Vec3f_Copy(&pos, &this->actor.world.pos);
    pos.x += Math_SinS(this->actor.world.rot.y) * 3000.0f;
    pos.y += 600.0f;
    pos.z += Math_CosS(this->actor.world.rot.y) * 3000.0f;

    Math_Vec3f_Copy(&subCamAt, &this->actor.world.pos);
    subCamAt.x += Math_SinS(this->actor.world.rot.y) * 1200.0f;
    subCamAt.y += -100.0f;
    subCamAt.z += Math_CosS(this->actor.world.rot.y) * 1200.0f;

    EnDragon_SetSubCamEyeAt(this, play, pos, subCamAt);

    player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y;
    player->actor.world.rot.x = player->actor.shape.rot.x = this->actor.world.rot.x;
    player->actor.world.rot.z = player->actor.shape.rot.z = this->actor.world.rot.z - 0x36B0;
    Math_Vec3f_Copy(&player->actor.world.pos, &this->playerGrabPosition);
    this->jawZRotation = 0xC8;

    Math_Vec3f_Copy(&pos, &this->burrowEntrancePos);
    pos.x += Math_SinS(this->actor.world.rot.y) * -530.0f;
    pos.z += Math_CosS(this->actor.world.rot.y) * -530.0f;
    Math_ApproachF(&this->actor.world.pos.x, pos.x, 0.3f, 200.0f);
    Math_ApproachF(&this->actor.world.pos.y, pos.y, 0.3f, 200.0f);
    Math_ApproachF(&this->actor.world.pos.z, pos.z, 0.3f, 200.0f);

    if ((this->state <= DEEP_PYTHON_ATTACK_STATE_START) && (curFrame >= this->animEndFrame)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_UTSUBO_BITE);
        if (this->animIndex != DEEP_PYTHON_ANIM_LARGE_SIDE_SWAY) {
            EnDragon_ChangeAnim(this, DEEP_PYTHON_ANIM_LARGE_SIDE_SWAY);
        }

        this->state++;
    }

    if (((this->state != DEEP_PYTHON_ATTACK_STATE_START) && (curFrame >= this->animEndFrame)) ||
        !(player->stateFlags2 & PLAYER_STATE2_80) || (this->collider.elements[0].base.acElemFlags & ACELEM_HIT) ||
        (this->collider.elements[1].base.acElemFlags & ACELEM_HIT) ||
        (this->collider.elements[2].base.acElemFlags & ACELEM_HIT)) {
        player->actor.parent = NULL;
        this->grabWaitTimer = 30;
        CutsceneManager_Stop(this->grabCsId);
        if (player->stateFlags2 & PLAYER_STATE2_80) {
            player->av2.actionVar2 = 100;
        }

        this->actor.flags &= ~ACTOR_FLAG_FREEZE_EXCEPTION;

        if ((this->state != DEEP_PYTHON_ATTACK_STATE_START) && (curFrame >= this->animEndFrame)) {
            this->timer = 3;
            this->actionFunc = EnDragon_RetreatOnceTimerEnds;
        } else {
            EnDragon_SetupRetreatOrIdle(this);
        }
    }
}

void EnDragon_SetupDead(EnDragon* this, PlayState* play) {
    if (!CutsceneManager_IsNext(this->deathCsId)) {
        CutsceneManager_Queue(this->deathCsId);
    } else {
        CutsceneManager_StartWithPlayerCs(this->deathCsId, &this->actor);
        this->animEndFrame = Animation_GetLastFrame(&gDeepPythonSmallSideSwayAnim);
        Animation_Change(&this->skelAnime, &gDeepPythonSmallSideSwayAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f);
        this->timer = 20;
        this->actionFunc = EnDragon_Dead;
    }
}

void EnDragon_Dead(EnDragon* this, PlayState* play) {
    Vec3f seahorsePos;

    SkelAnime_Update(&this->skelAnime);
    this->actor.shape.rot.z += 0x1000;
    this->jawZRotation = 0xFA0;
    EnDragon_SpawnBubbles(this, play, this->jawPos);

    if ((this->timer != 0) && (fabsf(this->actor.world.pos.x - this->actor.home.pos.x) > 121.0f) &&
        (fabsf(this->actor.world.pos.z - this->actor.home.pos.z) > 121.0f)) {
        this->actor.speed = -120.0f;
        if (((this->pythonIndex & 1) == 0) && (Rand_ZeroOne() < 0.5f)) {
            //! @bug: !play->gameplayFrames is 0 essentially all the time, so this code never runs.
            if (((!play->gameplayFrames) & 0x1F)) {
                Item_DropCollectibleRandom(play, NULL, &this->jawPos, 0x90);
            }
        }

        this->action = DEEP_PYTHON_ACTION_DEAD;
        return;
    }

    this->actor.speed = 0.0f;
    if ((fabsf(this->actor.world.pos.x - this->actor.home.pos.x) > 20.0f) &&
        (fabsf(this->actor.world.pos.z - this->actor.home.pos.z) > 20.0f)) {
        Math_ApproachF(&this->actor.world.pos.x, this->actor.home.pos.x, 0.3f, 300.0f);
        Math_ApproachF(&this->actor.world.pos.z, this->actor.home.pos.z, 0.3f, 300.0f);
        return;
    }

    sNumPythonsDead++;
    if (sNumPythonsDead >= (BREG(39) + 8)) {
        Math_Vec3f_Copy(&seahorsePos, &this->actor.parent->world.pos);
        seahorsePos.x += Math_SinS(this->actor.parent->world.rot.y + 0x8000) * (500.0f + BREG(38));
        seahorsePos.y += -100.0f + BREG(33);
        seahorsePos.z += Math_CosS(this->actor.parent->world.rot.y + 0x8000) * (500.0f + BREG(38));
        if (Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, ACTOR_EN_OT, seahorsePos.x, seahorsePos.y,
                                          seahorsePos.z, 0, this->actor.shape.rot.y, 0,
                                          SEAHORSE_PARAMS(SEAHORSE_TYPE_1, 0, 0), this->actor.csId,
                                          this->actor.halfDaysBits, NULL)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_13_01);
            switch (this->pythonIndex) {
                case 0:
                    SET_WEEKEVENTREG(WEEKEVENTREG_83_10);
                    break;

                case 1:
                    SET_WEEKEVENTREG(WEEKEVENTREG_83_20);
                    break;

                case 2:
                    SET_WEEKEVENTREG(WEEKEVENTREG_83_40);
                    break;

                case 3:
                    SET_WEEKEVENTREG(WEEKEVENTREG_83_80);
                    break;
                case 4:
                    SET_WEEKEVENTREG(WEEKEVENTREG_84_01);
                    break;

                case 5:
                    SET_WEEKEVENTREG(WEEKEVENTREG_84_02);
                    break;

                case 6:
                    SET_WEEKEVENTREG(WEEKEVENTREG_84_04);
                    break;

                case 7:
                    SET_WEEKEVENTREG(WEEKEVENTREG_84_08);
                    break;
            }
        }
    }

    Actor_Kill(&this->actor);
}

void EnDragon_UpdateDamage(EnDragon* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    PlayerImpactType playerImpactType;

    if (this->action == DEEP_PYTHON_ACTION_EXTEND) {
        if ((this->collider.elements[2].base.acElemFlags & ACELEM_HIT) ||
            (this->collider.elements[3].base.acElemFlags & ACELEM_HIT) ||
            (this->collider.elements[4].base.acElemFlags & ACELEM_HIT) ||
            (this->collider.elements[5].base.acElemFlags & ACELEM_HIT) ||
            (this->collider.elements[6].base.acElemFlags & ACELEM_HIT) ||
            (this->collider.elements[7].base.acElemFlags & ACELEM_HIT)) {
            Actor_ApplyDamage(&this->actor);
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 25);
            if (this->actor.colChkInfo.health > 0) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_UTSUBO_DAMAGE);
                this->action = DEEP_PYTHON_ACTION_DAMAGE;
            } else {
                Enemy_StartFinishingBlow(play, &this->actor);
                Actor_PlaySfx(&this->actor, NA_SE_EN_UTSUBO_DEAD);
                this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                this->actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;
                this->action = DEEP_PYTHON_ACTION_SETUP_DEAD;
                this->actionFunc = EnDragon_SetupDead;
            }
        }
    }

    if ((this->action == DEEP_PYTHON_ACTION_EXTEND) && (this->grabWaitTimer == 0) &&
        (player->invincibilityTimer == 0) && (this->collider.elements[0].base.ocElemFlags & OCELEM_HIT) &&
        !((Actor_GetPlayerImpact(play, 1000.0f, &this->actor.world.pos, &playerImpactType) >= 0.0f) &&
          (playerImpactType == PLAYER_IMPACT_ZORA_BARRIER))) {
        this->actor.speed = 0.0f;
        this->action = DEEP_PYTHON_ACTION_GRAB;
        this->actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;
        this->actionFunc = EnDragon_SetupGrab;
    }
}

void EnDragon_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDragon* this = (EnDragon*)thisx;

    if (this->retreatTimer != 0) {
        this->retreatTimer--;
    }

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->largeSwayWaitTimer != 0) {
        this->largeSwayWaitTimer--;
    }

    if (this->grabWaitTimer != 0) {
        this->grabWaitTimer--;
    }

    EnDragon_UpdateDamage(this, play);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    Math_Vec3f_Copy(&this->actor.focus.pos, &this->focusPos);
    Math_Vec3s_Copy(&this->actor.focus.rot, &this->actor.world.rot);
    Actor_SetScale(&this->actor, this->scale);

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);

    if (this->action != DEEP_PYTHON_ACTION_GRAB) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->action < DEEP_PYTHON_ACTION_DAMAGE) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }
}

s32 EnDragon_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnDragon* this = (EnDragon*)thisx;

    if (limbIndex == DEEP_PYTHON_LIMB_JAW) {
        rot->x += this->jawXRotation;
        rot->y += this->jawYRotation;
        rot->z += this->jawZRotation;
    }

    return false;
}

void EnDragon_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnDragon* this = (EnDragon*)thisx;
    Vec3f playerGrabOffsetFromJawPos = { 350.0f, -120.0f, -60.0f };

    if (limbIndex == DEEP_PYTHON_LIMB_JAW) {
        Matrix_MultVec3f(&gZeroVec3f, &this->jawPos);
        playerGrabOffsetFromJawPos.x = 350.0f;
        playerGrabOffsetFromJawPos.y = -120.0f;
        playerGrabOffsetFromJawPos.z = -60.0f;
        Matrix_MultVec3f(&playerGrabOffsetFromJawPos, &this->playerGrabPosition);
    }

    if (limbIndex == DEEP_PYTHON_LIMB_HEAD_AND_COLLAR_ROOT) {
        Matrix_MultVec3f(&gZeroVec3f, &this->focusPos);
    }

    Collider_UpdateSpheres(limbIndex, &this->collider);
}

void EnDragon_Draw(Actor* thisx, PlayState* play) {
    EnDragon* this = (EnDragon*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnDragon_OverrideLimbDraw, EnDragon_PostLimbDraw, &this->actor);
}

void EnDragon_Reset(void) {
    sNumPythonsDead = 0;
}
