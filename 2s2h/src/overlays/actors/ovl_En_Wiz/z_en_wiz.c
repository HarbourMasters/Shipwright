/*
 * File: z_en_wiz.c
 * Overlay: ovl_En_Wiz
 * Description: Wizrobe
 */

#include "z_en_wiz.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/actors/ovl_En_Wiz_Brock/z_en_wiz_brock.h"

#define FLAGS                                                                                   \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED |   \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_IGNORE_QUAKE | ACTOR_FLAG_FREEZE_EXCEPTION | \
     ACTOR_FLAG_LOCK_ON_DISABLED | ACTOR_FLAG_MINIMAP_ICON_ENABLED)

void EnWiz_Init(Actor* thisx, PlayState* play);
void EnWiz_Destroy(Actor* thisx, PlayState* play);
void EnWiz_Update(Actor* thisx, PlayState* play);
void EnWiz_Draw(Actor* thisx, PlayState* play);

void EnWiz_StartIntroCutscene(EnWiz* this, PlayState* play);
void EnWiz_SetupAppear(EnWiz* this, PlayState* play);
void EnWiz_Appear(EnWiz* this, PlayState* play);
void EnWiz_SetupDance(EnWiz* this);
void EnWiz_Dance(EnWiz* this, PlayState* play);
void EnWiz_SetupSecondPhaseCutscene(EnWiz* this, PlayState* play);
void EnWiz_SecondPhaseCutscene(EnWiz* this, PlayState* play);
void EnWiz_SetupWindUp(EnWiz* this);
void EnWiz_WindUp(EnWiz* this, PlayState* play);
void EnWiz_SetupAttack(EnWiz* this);
void EnWiz_Attack(EnWiz* this, PlayState* play);
void EnWiz_SetupDisappear(EnWiz* this);
void EnWiz_Disappear(EnWiz* this, PlayState* play);
void EnWiz_Damaged(EnWiz* this, PlayState* play);
void EnWiz_SetupDead(EnWiz* this);
void EnWiz_Dead(EnWiz* this, PlayState* play);

// This number is almost-entirely arbirary, with the only requirement being
// that it cannot be a valid curPlatformIndex. Any negative number, or any
// number larger than 10, would work just as well.
#define INITIAL_PLATFORM_INDEX 777

typedef enum {
    /* 1 */ EN_WIZ_ACTION_APPEAR = 1,
    /* 2 */ EN_WIZ_ACTION_RUN_BETWEEN_PLATFORMS,
    /* 3 */ EN_WIZ_ACTION_DISAPPEAR,
    /* 4 */ EN_WIZ_ACTION_DAMAGED,
    /* 5 */ EN_WIZ_ACTION_DEAD,
    /* 6 */ EN_WIZ_ACTION_BURST_INTO_FLAMES,
    /* 7 */ EN_WIZ_ACTION_RUN_IN_CIRCLES,
    /* 8 */ EN_WIZ_ACTION_ATTACK,
    /* 9 */ EN_WIZ_ACTION_DANCE
} EnWizAction;

typedef enum {
    /* 0 */ EN_WIZ_INTRO_CS_NOT_STARTED,
    /* 1 */ EN_WIZ_INTRO_CS_CAMERA_MOVE_TO_PLATFORM,
    /* 2 */ EN_WIZ_INTRO_CS_APPEAR,
    /* 3 */ EN_WIZ_INTRO_CS_CAMERA_SPIN_TO_FACE_WIZROBE,
    /* 4 */ EN_WIZ_INTRO_CS_WAIT_BEFORE_RUN,
    /* 5 */ EN_WIZ_INTRO_CS_RUN_IN_CIRCLES,
    /* 6 */ EN_WIZ_INTRO_CS_DISAPPEAR,
    /* 7 */ EN_WIZ_INTRO_CS_END
} EnWizIntroCutsceneState;

typedef enum {
    /* 0 */ EN_WIZ_FIGHT_STATE_FIRST_PHASE,
    /* 1 */ EN_WIZ_FIGHT_STATE_SECOND_PHASE_CUTSCENE,
    /* 2 */ EN_WIZ_FIGHT_STATE_SECOND_PHASE_GHOSTS_COPY_WIZROBE,
    /* 3 */ EN_WIZ_FIGHT_STATE_SECOND_PHASE_GHOSTS_RUN_AROUND
} EnWizFightState;

ActorProfile En_Wiz_Profile = {
    /**/ ACTOR_EN_WIZ,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_WIZ,
    /**/ sizeof(EnWiz),
    /**/ EnWiz_Init,
    /**/ EnWiz_Destroy,
    /**/ EnWiz_Update,
    /**/ EnWiz_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[10] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0x01000202, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { WIZROBE_LIMB_PELVIS, { { 0, 0, 0 }, 0 }, 1 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { WIZROBE_LIMB_PELVIS, { { 0, 0, 0 }, 0 }, 0 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { WIZROBE_LIMB_PELVIS, { { 0, 0, 0 }, 0 }, 0 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { WIZROBE_LIMB_PELVIS, { { 0, 0, 0 }, 0 }, 0 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { WIZROBE_LIMB_PELVIS, { { 0, 0, 0 }, 0 }, 0 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { WIZROBE_LIMB_PELVIS, { { 0, 0, 0 }, 0 }, 0 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { WIZROBE_LIMB_PELVIS, { { 0, 0, 0 }, 0 }, 0 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { WIZROBE_LIMB_PELVIS, { { 0, 0, 0 }, 0 }, 0 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { WIZROBE_LIMB_PELVIS, { { 0, 0, 0 }, 0 }, 0 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { WIZROBE_LIMB_PELVIS, { { 0, 0, 0 }, 0 }, 0 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_HIT2,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0xF7CFFFFF, 0x08, 0x04 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 35, 130, 0, { 0, 0, 0 } },
};

typedef enum {
    /* 0x0 */ EN_WIZ_DMGEFF_IMMUNE,    // Deals no damage
    /* 0x1 */ EN_WIZ_DMGEFF_UNK1,      // Deals no damage. Was probably originally intended for destroying ghosts.
    /* 0x2 */ EN_WIZ_DMGEFF_FIRE,      // Damages and sets Ice Wizrobes on fire
    /* 0x3 */ EN_WIZ_DMGEFF_FREEZE,    // Damages and surrounds Fire Wizrobes with ice
    /* 0x4 */ EN_WIZ_DMGEFF_LIGHT_ORB, // Damages and surrounds the Wizrobe with light orbs
    /* 0xF */ EN_WIZ_DMGEFF_NONE = 0xF // Deals regular damage and has no special effect
} EnWizDamageEffect;

static DamageTable sFireWizrobeDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, EN_WIZ_DMGEFF_UNK1),
    /* Deku Stick     */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Explosives     */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* UNK_DMG_D_06   */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Hookshot       */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Goron punch    */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(3, EN_WIZ_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(1, EN_WIZ_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(2, EN_WIZ_DMGEFF_FREEZE),
    /* Light arrow    */ DMG_ENTRY(2, EN_WIZ_DMGEFF_LIGHT_ORB),
    /* Goron spikes   */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Deku spin      */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, EN_WIZ_DMGEFF_UNK1),
    /* Zora barrier   */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Normal shield  */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Light ray      */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Thrown object  */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Normal Roll    */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Unblockable    */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Powder Keg     */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
};

static DamageTable sIceWizrobeDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, EN_WIZ_DMGEFF_UNK1),
    /* Deku Stick     */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Explosives     */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* UNK_DMG_D_06   */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Hookshot       */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Goron punch    */ DMG_ENTRY(2, EN_WIZ_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(3, EN_WIZ_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(2, EN_WIZ_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(1, EN_WIZ_DMGEFF_FREEZE),
    /* Light arrow    */ DMG_ENTRY(2, EN_WIZ_DMGEFF_LIGHT_ORB),
    /* Goron spikes   */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Deku spin      */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, EN_WIZ_DMGEFF_UNK1),
    /* Zora barrier   */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Normal shield  */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Light ray      */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Thrown object  */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Normal Roll    */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Unblockable    */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, EN_WIZ_DMGEFF_IMMUNE),
    /* Powder Keg     */ DMG_ENTRY(1, EN_WIZ_DMGEFF_NONE),
};

void EnWiz_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnWiz* this = (EnWiz*)thisx;

    SkelAnime_InitFlex(play, &this->skelAnime, &gWizrobeSkel, &gWizrobeIdleAnim, this->jointTable, this->morphTable,
                       WIZROBE_LIMB_MAX);
    SkelAnime_InitFlex(play, &this->ghostSkelAnime, &gWizrobeSkel, &gWizrobeIdleAnim, this->ghostBaseJointTable,
                       this->ghostMorphTable, WIZROBE_LIMB_MAX);
    Actor_SetScale(&this->actor, 0.0f);
    this->platformLightAlpha = 0;
    this->alpha = 255;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.attentionRangeType = ATTENTION_RANGE_3;
    this->unk_450 = 1.0f;
    this->actor.shape.yOffset = 700.0f;
    Collider_InitAndSetJntSph(play, &this->ghostColliders, &this->actor, &sJntSphInit, this->ghostColliderElements);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->staffFlameScroll = Rand_S16Offset(0, 7);
    this->switchFlag = EN_WIZ_GET_SWITCH_FLAG(&this->actor);
    this->type = EN_WIZ_GET_TYPE(&this->actor);

    if (this->switchFlag == EN_WIZ_SWITCH_FLAG_NONE) {
        this->switchFlag = SWITCH_FLAG_NONE;
    }

    if ((this->type == EN_WIZ_TYPE_FIRE) || (this->type == EN_WIZ_TYPE_FIRE_NO_BGM)) {
        this->actor.colChkInfo.damageTable = &sFireWizrobeDamageTable;
        this->actor.colChkInfo.health = 8;
        this->actor.flags &= ~ACTOR_FLAG_FREEZE_EXCEPTION;
    } else {
        this->actor.colChkInfo.damageTable = &sIceWizrobeDamageTable;
        this->actor.colChkInfo.health = 6;
    }

    if ((this->switchFlag > SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.hintId = TATL_HINT_ID_WIZROBE;
    this->curPlatformIndex = INITIAL_PLATFORM_INDEX;

    // Setting the radius and scale to zero here effectively disables all of the ghost colliders.
    this->ghostColliders.elements[0].dim.modelSphere.radius = 0;
    this->ghostColliders.elements[0].dim.scale = 0.0f;
    this->ghostColliders.elements[0].dim.modelSphere.center.x = 0;
    this->ghostColliders.elements[0].dim.modelSphere.center.y = 0;
    this->ghostColliders.elements[0].dim.modelSphere.center.z = 0;

    this->actionFunc = EnWiz_StartIntroCutscene;
}

void EnWiz_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnWiz* this = (EnWiz*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
    Collider_DestroyJntSph(play, &this->ghostColliders);
    if (this->type != EN_WIZ_TYPE_FIRE_NO_BGM) {
        Audio_RestorePrevBgm();
    }
}

typedef enum EnWizAnimation {
    /* 0 */ EN_WIZ_ANIM_IDLE,
    /* 1 */ EN_WIZ_ANIM_RUN,
    /* 2 */ EN_WIZ_ANIM_DANCE,
    /* 3 */ EN_WIZ_ANIM_WIND_UP,
    /* 4 */ EN_WIZ_ANIM_ATTACK,
    /* 5 */ EN_WIZ_ANIM_DAMAGE,
    /* 6 */ EN_WIZ_ANIM_MAX
} EnWizAnimation;

static AnimationHeader* sAnimations[EN_WIZ_ANIM_MAX] = {
    &gWizrobeIdleAnim,   // EN_WIZ_ANIM_IDLE
    &gWizrobeRunAnim,    // EN_WIZ_ANIM_RUN
    &gWizrobeDanceAnim,  // EN_WIZ_ANIM_DANCE
    &gWizrobeWindUpAnim, // EN_WIZ_ANIM_WIND_UP
    &gWizrobeAttackAnim, // EN_WIZ_ANIM_ATTACK
    &gWizrobeDamageAnim, // EN_WIZ_ANIM_DAMAGE
};

static u8 sAnimationModes[EN_WIZ_ANIM_MAX] = {
    ANIMMODE_LOOP, // EN_WIZ_ANIM_IDLE
    ANIMMODE_LOOP, // EN_WIZ_ANIM_RUN
    ANIMMODE_LOOP, // EN_WIZ_ANIM_DANCE
    ANIMMODE_LOOP, // EN_WIZ_ANIM_WIND_UP
    ANIMMODE_LOOP, // EN_WIZ_ANIM_ATTACK
    ANIMMODE_ONCE, // EN_WIZ_ANIM_DAMAGE
};

void EnWiz_ChangeAnim(EnWiz* this, s32 animIndex, s32 updateGhostAnim) {
    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);
    Animation_Change(&this->skelAnime, sAnimations[animIndex], 1.0f, 0.0f, this->animEndFrame,
                     sAnimationModes[animIndex], -2.0f);
    if (updateGhostAnim) {
        Animation_Change(&this->ghostSkelAnime, sAnimations[animIndex], 1.0f, 0.0f, this->animEndFrame,
                         sAnimationModes[animIndex], -2.0f);
    }
}

/**
 * Responsible for moving the camera around and making the Wizrobe run in circles during the intro cutscene.
 */
void EnWiz_HandleIntroCutscene(EnWiz* this, PlayState* play) {
    Camera* subCam;
    Vec3f eyeNext;
    Vec3f atNext;

    if (this->introCutsceneState < EN_WIZ_INTRO_CS_DISAPPEAR) {
        subCam = Play_GetCamera(play, this->subCamId);
        switch (this->introCutsceneState) {
            case EN_WIZ_INTRO_CS_NOT_STARTED:
                this->introCutsceneTimer = 100;
                this->introCutsceneCameraAngle = this->actor.world.rot.y;
                this->introCutsceneState++;
                break;

            case EN_WIZ_INTRO_CS_CAMERA_MOVE_TO_PLATFORM:
                Math_Vec3f_Copy(&eyeNext, &this->actor.world.pos);
                Math_Vec3f_Copy(&atNext, &this->actor.world.pos);
                eyeNext.x += Math_SinS(this->introCutsceneCameraAngle) * 200.0f;
                eyeNext.y += 100.0f;
                eyeNext.z += Math_CosS(this->introCutsceneCameraAngle) * 200.0f;
                atNext.y += 80.0f;
                Math_ApproachF(&subCam->eye.x, eyeNext.x, 0.3f, 30.0f);
                Math_ApproachF(&subCam->eye.z, eyeNext.z, 0.3f, 30.0f);
                Math_ApproachF(&subCam->at.x, atNext.x, 0.3f, 30.0f);
                Math_ApproachF(&subCam->at.z, atNext.z, 0.3f, 30.0f);
                subCam->eye.y = eyeNext.y;
                subCam->at.y = atNext.y;
                if ((fabsf(subCam->eye.x - eyeNext.x) < 2.0f) && (fabsf(subCam->eye.y - eyeNext.y) < 2.0f) &&
                    (fabsf(subCam->eye.z - eyeNext.z) < 2.0f) && (fabsf(subCam->at.x - atNext.x) < 2.0f) &&
                    (fabsf(subCam->at.y - atNext.y) < 2.0f) && (fabsf(subCam->at.z - atNext.z) < 2.0f)) {
                    Player* player = GET_PLAYER(play);
                    s32 i;

                    this->actor.world.rot.y = this->actor.shape.rot.y =
                        Math_Vec3f_Yaw(&this->actor.world.pos, &player->actor.world.pos);

                    for (i = 0; i < this->platformCount; i++) {
                        this->ghostRot[i].y = Math_Vec3f_Yaw(&this->ghostPos[i], &player->actor.world.pos);
                    }

                    EnWiz_ChangeAnim(this, EN_WIZ_ANIM_IDLE, true);
                    this->shouldStartTimer = false;
                    this->targetPlatformLightAlpha = 255;
                    Math_Vec3f_Copy(&this->platformLightPos, &this->actor.world.pos);
                    if (this->fightState == EN_WIZ_FIGHT_STATE_FIRST_PHASE) {
                        Actor_PlaySfx(&this->actor, NA_SE_EN_WIZ_UNARI);
                    } else {
                        Actor_PlaySfx(&this->actor, NA_SE_EN_WIZ_VOICE - SFX_FLAG);
                    }

                    this->introCutsceneTimer = 40;
                    this->introCutsceneState++;
                }
                break;

            case EN_WIZ_INTRO_CS_APPEAR:
                if (this->introCutsceneTimer == 0) {
                    this->introCutsceneTimer = 20;
                    this->introCutsceneState++;
                }
                break;

            case EN_WIZ_INTRO_CS_CAMERA_SPIN_TO_FACE_WIZROBE:
                Math_Vec3f_Copy(&eyeNext, &this->actor.world.pos);
                Math_Vec3f_Copy(&atNext, &this->actor.world.pos);
                eyeNext.x += Math_SinS(this->actor.world.rot.y) * 160.0f;
                eyeNext.y += 70.0f;
                eyeNext.z += Math_CosS(this->actor.world.rot.y) * 140.0f;
                atNext.x += -10.0f;
                atNext.y += 100.0f;
                Math_ApproachF(&subCam->eye.x, eyeNext.x, 0.3f, 30.0f);
                Math_ApproachF(&subCam->eye.z, eyeNext.z, 0.3f, 30.0f);
                Math_ApproachF(&subCam->at.x, atNext.x, 0.3f, 30.0f);
                Math_ApproachF(&subCam->at.z, atNext.z, 0.3f, 30.0f);
                subCam->eye.y = eyeNext.y;
                subCam->at.y = atNext.y;
                if (this->introCutsceneTimer == 0) {
                    this->introCutsceneTimer = 10;
                    this->introCutsceneState++;
                    this->introCutsceneCameraAngle = this->actor.world.rot.y;
                }
                break;

            case EN_WIZ_INTRO_CS_WAIT_BEFORE_RUN:
                if (this->introCutsceneTimer == 0) {
                    EnWiz_ChangeAnim(this, EN_WIZ_ANIM_RUN, false);
                    this->angularVelocity = 0;
                    this->introCutsceneTimer = 34;
                    this->introCutsceneState++;
                }
                break;

            case EN_WIZ_INTRO_CS_RUN_IN_CIRCLES:
                Actor_PlaySfx(&this->actor, NA_SE_EN_WIZ_RUN - SFX_FLAG);
                if (this->introCutsceneTimer == 0) {
                    this->animLoopCounter = this->introCutsceneCameraAngle = 0;
                    this->introCutsceneState = EN_WIZ_INTRO_CS_DISAPPEAR;
                } else {
                    Math_SmoothStepToS(&this->angularVelocity, 0x1388, 0x64, 0x3E8, 0x3E8);
                    this->actor.world.rot.y += this->angularVelocity;
                }

                Math_Vec3f_Copy(&eyeNext, &this->actor.world.pos);
                Math_Vec3f_Copy(&atNext, &this->actor.world.pos);
                eyeNext.x += Math_SinS(this->introCutsceneCameraAngle) * 200.0f;
                eyeNext.y += 100.0f;
                eyeNext.z += Math_CosS(this->introCutsceneCameraAngle) * 200.0f;
                atNext.y += 80.0f;
                Math_ApproachF(&subCam->eye.x, eyeNext.x, 0.3f, 30.0f);
                Math_ApproachF(&subCam->eye.z, eyeNext.z, 0.3f, 30.0f);
                Math_ApproachF(&subCam->at.x, atNext.x, 0.3f, 30.0f);
                Math_ApproachF(&subCam->at.z, atNext.z, 0.3f, 30.0f);
                subCam->eye.y = eyeNext.y;
                subCam->at.y = atNext.y;
                break;

            default:
                break;
        }

        if (this->musicStartTimer < 11) {
            this->musicStartTimer++;
            if ((this->type != EN_WIZ_TYPE_FIRE_NO_BGM) && (this->musicStartTimer == 11)) {
                Audio_PlayBgm_StorePrevBgm(NA_BGM_MINI_BOSS);
            }
        }
    }
}

/**
 * Chooses which platform the Wizrobe should appear at, and updates its position accordingly.
 * It also updates the position and alpha of all the ghosts.
 */
void EnWiz_SelectPlatform(EnWiz* this, PlayState* play) {
    Actor* prop;
    s32 i;
    s32 j;
    s16 ghostAlpha;
    s16 type;
    s16 curPlatformIndex;

    for (i = 0; i < ARRAY_COUNT(this->platforms); i++) {
        this->platforms[i] = NULL;
    }

    prop = play->actorCtx.actorLists[ACTORCAT_PROP].first;
    i = 0;
    while (prop != NULL) {
        if (prop->id != ACTOR_EN_WIZ_BROCK) {
            prop = prop->next;
            continue;
        }

        this->platforms[i] = prop;
        i++;
        if (this->action != EN_WIZ_ACTION_BURST_INTO_FLAMES) {
            type = this->type;
            if (type == EN_WIZ_TYPE_FIRE_NO_BGM) {
                type = EN_WIZ_TYPE_FIRE;
            }

            prop->colChkInfo.health = type + EN_WIZ_BROCK_PLATFORM_TYPE_FIRE;
            prop = prop->next;
        } else {
            prop->colChkInfo.health = EN_WIZ_BROCK_PLATFORM_TYPE_INACTIVE;
            prop = prop->next;
        }
    }

    if (this->action != EN_WIZ_ACTION_DEAD) {
        this->platformCount = i;
        if (i < 0) {
            i = 0;
        } else if (this->platformCount > 10) {
            this->platformCount = 10;
        }

        curPlatformIndex = Rand_ZeroFloat(i);
        while ((this->curPlatformIndex == curPlatformIndex) || ((s16)i == curPlatformIndex)) {
            curPlatformIndex = Rand_ZeroFloat(i);
            // FAKE:
            if (1) {}
        }

        this->curPlatformIndex = curPlatformIndex;
        switch (this->fightState) {
            case EN_WIZ_FIGHT_STATE_FIRST_PHASE:
                Math_Vec3f_Copy(&this->actor.world.pos, &this->platforms[curPlatformIndex]->world.pos);
                break;

            case EN_WIZ_FIGHT_STATE_SECOND_PHASE_CUTSCENE:
                //! @bug: Setting the Wizrobe's position to the first platform *without* updating
                //! this->curPlatformIndex can cause a bug later in EnWiz_Damaged. One way to fix
                //! this is to set this->curPlatformIndex to 0 here.
                Math_Vec3f_Copy(&this->actor.world.pos, &this->platforms[0]->world.pos);
                for (i = 0, ghostAlpha = 128; i < this->platformCount; i++, ghostAlpha -= 10) {
                    Math_Vec3f_Copy(&this->ghostPos[i], &this->actor.world.pos);
                    this->ghostAlpha[i] = ghostAlpha;
                }
                break;

            default:
                Math_Vec3f_Copy(&this->actor.world.pos, &this->platforms[curPlatformIndex]->world.pos);
                for (i--; i >= 0; i--) {
                    if (curPlatformIndex != i) {
                        Math_Vec3f_Copy(&this->ghostPos[i], &this->platforms[i]->world.pos);
                        this->ghostRot[i] = this->actor.world.rot;
                        this->ghostAlpha[i] = 100;
                        this->ghostNextPlatformIndex[i] = i;
                        for (j = 0; j < ARRAY_COUNT(this->jointTable); j++) {
                            this->ghostJointTables[i][j] = this->jointTable[j];
                        }
                    } else {
                        Math_Vec3f_Copy(&this->ghostPos[i], &gZeroVec3f);
                    }
                }
                break;
        }
    }
}

/**
 * Makes the ghosts run around the room from platform to platform.
 */
void EnWiz_MoveGhosts(EnWiz* this) {
    s32 i;
    s32 ghostNextPlatformIndex;
    s32 playSfx = false;

    for (i = 0; i < this->platformCount; i++) {
        if (this->ghostPos[i].x != 0.0f && this->ghostPos[i].z != 0.0f) {
            f32 diffX;
            f32 diffZ;

            ghostNextPlatformIndex = this->ghostNextPlatformIndex[i];
            diffX = this->platforms[ghostNextPlatformIndex]->world.pos.x - this->ghostPos[i].x;
            diffZ = this->platforms[ghostNextPlatformIndex]->world.pos.z - this->ghostPos[i].z;
            playSfx++;

            if (sqrtf(SQ(diffX) + SQ(diffZ)) < 30.0f) {
                this->ghostNextPlatformIndex[i]--;
                if (this->ghostNextPlatformIndex[i] < 0) {
                    this->ghostNextPlatformIndex[i] = this->platformCount - 1;
                }
            }

            ghostNextPlatformIndex = this->ghostNextPlatformIndex[i];
            Math_ApproachF(&this->ghostPos[i].x, this->platforms[ghostNextPlatformIndex]->world.pos.x, 0.3f, 30.0f);
            Math_ApproachF(&this->ghostPos[i].y, this->platforms[ghostNextPlatformIndex]->world.pos.y, 0.3f, 30.0f);
            Math_ApproachF(&this->ghostPos[i].z, this->platforms[ghostNextPlatformIndex]->world.pos.z, 0.3f, 30.0f);
            this->ghostRot[i].y =
                Math_Vec3f_Yaw(&this->ghostPos[i], &this->platforms[ghostNextPlatformIndex]->world.pos);
        }
    }

    if (playSfx) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_WIZ_RUN - SFX_FLAG);
    }
}

void EnWiz_StartIntroCutscene(EnWiz* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_StartWithPlayerCsAndSetFlag(this->actor.csId, &this->actor);
        this->subCamId = CutsceneManager_GetCurrentSubCamId(this->actor.csId);
        this->actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;
        EnWiz_SetupAppear(this, play);
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void EnWiz_SetupAppear(EnWiz* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 i;
    s16 angle;

    this->action = EN_WIZ_ACTION_APPEAR;

    if (this->timer == 0) {
        EnWiz_SelectPlatform(this, play);

        if (this->introCutsceneState != EN_WIZ_INTRO_CS_NOT_STARTED) {
            angle = Math_Vec3f_Yaw(&this->actor.world.pos, &player->actor.world.pos);
            this->actor.shape.rot.y = angle;
            this->actor.world.rot.y = angle;
            for (i = 0; i < this->platformCount; i++) {
                this->ghostRot[i].y = Math_Vec3f_Yaw(&this->ghostPos[i], &player->actor.world.pos);
            }

            EnWiz_ChangeAnim(this, EN_WIZ_ANIM_IDLE, true);
            this->shouldStartTimer = false;
            this->targetPlatformLightAlpha = 255;
            Math_Vec3f_Copy(&this->platformLightPos, &this->actor.world.pos);

            if (this->fightState == EN_WIZ_FIGHT_STATE_FIRST_PHASE) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_WIZ_UNARI);
            } else {
                Actor_PlaySfx(&this->actor, NA_SE_EN_WIZ_VOICE - SFX_FLAG);
            }
        }

        this->actionFunc = EnWiz_Appear;
    }
}

/**
 * Makes the Wizrobe grow in scale and become more opaque.
 * During the first phase, it also increases the alpha for the platform's light.
 * If the player gets too close during the first phase, this will set up the Wizrobe to disappear.
 */
void EnWiz_Appear(EnWiz* this, PlayState* play) {
    Vec3f staffTargetFlameScale = { 0.006f, 0.006f, 0.006f };
    Player* player = GET_PLAYER(play);

    EnWiz_HandleIntroCutscene(this, play);

    if (this->introCutsceneState >= EN_WIZ_INTRO_CS_APPEAR) {
        SkelAnime_Update(&this->skelAnime);

        if ((this->fightState == EN_WIZ_FIGHT_STATE_FIRST_PHASE) &&
            (this->introCutsceneState >= EN_WIZ_INTRO_CS_DISAPPEAR) &&
            ((this->actor.xzDistToPlayer < 200.0f) ||
             ((player->unk_D57 != 0) &&
              ((ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.shape.rot.y)) < 0x7D0)) &&
              (ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, BINANG_ADD(player->actor.shape.rot.y, 0x8000))) <
               0x7D0)))) {
            EnWiz_SetupDisappear(this);
        } else {
            Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0xA, 0xBB8, 0);

            if (this->fightState == EN_WIZ_FIGHT_STATE_FIRST_PHASE) {
                Math_SmoothStepToS(&this->platformLightAlpha, this->targetPlatformLightAlpha, 10, 10, 10);
                if (!this->shouldStartTimer) {
                    this->timer = 20;
                    this->shouldStartTimer = true;
                }
            } else {
                Actor_PlaySfx(&this->actor, NA_SE_EN_WIZ_VOICE - SFX_FLAG);
            }

            if (this->timer == 0) {
                Math_ApproachF(&this->scale, 0.015f, 0.05f, 0.01f);
                Math_SmoothStepToS(&this->alpha, 255, 1, 5, 0);
            }

            if (this->scale < 0.0138f) {
                return;
            } else {
                this->action = EN_WIZ_ACTION_RUN_IN_CIRCLES;
                this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
                this->ghostColliders.elements[0].base.acDmgInfo.dmgFlags = 0x1013A22;
                Math_Vec3f_Copy(&this->staffTargetFlameScale, &staffTargetFlameScale);
                this->targetPlatformLightAlpha = 0;

                if (this->introCutsceneState == EN_WIZ_INTRO_CS_DISAPPEAR) {
                    this->timer = 0;
                    this->introCutsceneTimer = 20;
                    EnWiz_SetupDisappear(this);
                } else if (this->introCutsceneState >= EN_WIZ_INTRO_CS_END) {
                    if (this->fightState == EN_WIZ_FIGHT_STATE_SECOND_PHASE_CUTSCENE) {
                        this->actionFunc = EnWiz_SetupSecondPhaseCutscene;
                    } else {
                        EnWiz_SetupDance(this);
                    }
                }
            }
        }
    }
}

void EnWiz_SetupDance(EnWiz* this) {
    EnWiz_ChangeAnim(this, EN_WIZ_ANIM_DANCE, false);
    Math_ApproachF(&this->scale, 0.015f, 0.05f, 0.001f);
    this->angularVelocity = 0;
    this->animLoopCounter = 0;
    this->action = EN_WIZ_ACTION_DANCE;
    if (this->fightState >= EN_WIZ_FIGHT_STATE_SECOND_PHASE_GHOSTS_COPY_WIZROBE) {
        Animation_Change(&this->ghostSkelAnime, &gWizrobeRunAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gWizrobeRunAnim),
                         ANIMMODE_LOOP, 0.0f);
        this->fightState = EN_WIZ_FIGHT_STATE_SECOND_PHASE_GHOSTS_RUN_AROUND;
    }

    Math_SmoothStepToS(&this->alpha, 255, 1, 5, 0);
    this->actionFunc = EnWiz_Dance;
}

/**
 * Makes the Wizrobe spin around and dance until there are no active projectiles
 * and it completes at least three loops of its dancing animation.
 */
void EnWiz_Dance(EnWiz* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    s32 i;

    Math_SmoothStepToS(&this->alpha, 255, 1, 5, 0);
    Math_ApproachF(&this->scale, 0.015f, 0.05f, 0.001f);
    Actor_PlaySfx(&this->actor, NA_SE_EN_WIZ_RUN - SFX_FLAG);
    this->actor.world.rot.y += this->angularVelocity;
    if (this->fightState >= EN_WIZ_FIGHT_STATE_SECOND_PHASE_GHOSTS_RUN_AROUND) {
        EnWiz_MoveGhosts(this);
    } else {
        for (i = 0; i < this->platformCount; i++) {
            this->ghostRot[i].y += this->angularVelocity;
        }
    }

    Math_SmoothStepToS(&this->angularVelocity, 0x1388, 0x64, 0x3E8, 0x3E8);
    Math_SmoothStepToS(&this->platformLightAlpha, this->targetPlatformLightAlpha, 20, 50, 10);
    if (curFrame >= this->animEndFrame) {
        if (this->animLoopCounter < 10) {
            this->animLoopCounter++;
        }
    }

    if ((this->animLoopCounter >= 3) && !this->hasActiveProjectile) {
        this->targetPlatformLightAlpha = 0;
        EnWiz_SetupWindUp(this);
    }
}

void EnWiz_SetupSecondPhaseCutscene(EnWiz* this, PlayState* play) {
    s16 secondPhaseCsId = CutsceneManager_GetAdditionalCsId(this->actor.csId);

    if (!CutsceneManager_IsNext(secondPhaseCsId)) {
        CutsceneManager_Queue(secondPhaseCsId);
    } else {
        CutsceneManager_StartWithPlayerCsAndSetFlag(secondPhaseCsId, &this->actor);
        this->subCamId = CutsceneManager_GetCurrentSubCamId(secondPhaseCsId);
        this->actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;
        EnWiz_ChangeAnim(this, EN_WIZ_ANIM_DANCE, false);
        this->action = EN_WIZ_ACTION_RUN_BETWEEN_PLATFORMS;
        this->nextPlatformIndex = 1;
        this->hasRunToEveryPlatform = false;
        Math_SmoothStepToS(&this->alpha, 255, 1, 5, 0);
        this->actionFunc = EnWiz_SecondPhaseCutscene;
    }
}

/**
 * Makes the Wizrobe run between every platform in the room once before returning to its
 * original platform and disappearing. Ghosts trail behind the Wizrobe as it runs.
 */
void EnWiz_SecondPhaseCutscene(EnWiz* this, PlayState* play) {
    Camera* subCam;
    s32 i;

    Math_SmoothStepToS(&this->alpha, 255, 1, 5, 0);
    subCam = Play_GetCamera(play, this->subCamId);
    Math_Vec3f_Copy(&subCam->at, &this->actor.focus.pos);
    Actor_PlaySfx(&this->actor, NA_SE_EN_WIZ_RUN - SFX_FLAG);
    if (this->platforms[this->nextPlatformIndex] != NULL) {
        f32 diffX = this->actor.world.pos.x - this->platforms[this->nextPlatformIndex]->world.pos.x;
        f32 diffZ = this->actor.world.pos.z - this->platforms[this->nextPlatformIndex]->world.pos.z;
        s32 pad;

        if (sqrtf(SQ(diffX) + SQ(diffZ)) < 30.0f) {
            if (!this->hasRunToEveryPlatform) {
                this->nextPlatformIndex++;
                if (this->nextPlatformIndex >= this->platformCount) {
                    this->hasRunToEveryPlatform = true;
                    this->nextPlatformIndex = 0;
                }
            } else {
                f32 diffX = this->actor.world.pos.x - this->ghostPos[this->platformCount].x;
                f32 diffZ = this->actor.world.pos.z - this->ghostPos[this->platformCount].z;
                s32 pad;
                s32 i;

                this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
                if (sqrtf(SQ(diffX) + SQ(diffZ)) < 20.0f) {
                    for (i = 0; i < this->platformCount; i++) {
                        Math_Vec3f_Copy(&this->ghostPos[i], &gZeroVec3f);
                    }

                    this->nextPlatformIndex = 0;
                    this->platformCount = 0;
                    this->fightState = EN_WIZ_FIGHT_STATE_SECOND_PHASE_GHOSTS_COPY_WIZROBE;
                    this->timer = 0;
                    CutsceneManager_Stop(CutsceneManager_GetAdditionalCsId(this->actor.csId));
                    this->actor.flags &= ~ACTOR_FLAG_FREEZE_EXCEPTION;
                    EnWiz_SetupDisappear(this);
                    return;
                }
            }
        }
    }

    Math_Vec3f_Copy(this->ghostPos, &this->actor.world.pos);
    this->ghostRot[0].y = this->actor.world.rot.y;
    Math_ApproachF(&this->actor.world.pos.x, this->platforms[this->nextPlatformIndex]->world.pos.x, 0.3f, 30.0f);
    Math_ApproachF(&this->actor.world.pos.y, this->platforms[this->nextPlatformIndex]->world.pos.y, 0.3f, 30.0f);
    Math_ApproachF(&this->actor.world.pos.z, this->platforms[this->nextPlatformIndex]->world.pos.z, 0.3f, 30.0f);
    for (i = this->platformCount; i > 0; i--) {
        Math_Vec3f_Copy(&this->ghostPos[i], &this->ghostPos[i - 1]);
        this->ghostRot[i].y = this->ghostRot[i - 1].y;
    }

    this->actor.world.rot.y =
        Math_Vec3f_Yaw(&this->actor.world.pos, &this->platforms[this->nextPlatformIndex]->world.pos);
}

void EnWiz_SetupWindUp(EnWiz* this) {
    EnWiz_ChangeAnim(this, EN_WIZ_ANIM_WIND_UP, false);
    this->animLoopCounter = 0;
    this->actionFunc = EnWiz_WindUp;
}

/**
 * Plays the wind up animation for at least two animation loops before attacking.
 */
void EnWiz_WindUp(EnWiz* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    s32 i;

    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0xC8, 0x1F40, 0x1388);
    if (this->fightState >= EN_WIZ_FIGHT_STATE_SECOND_PHASE_GHOSTS_RUN_AROUND) {
        EnWiz_MoveGhosts(this);
    } else {
        for (i = 0; i < this->platformCount; i++) {
            Math_SmoothStepToS(&this->ghostRot[i].y, this->actor.yawTowardsPlayer, 0xC8, 0x1F40, 0x1388);
        }
    }

    if (curFrame >= this->animEndFrame) {
        this->animLoopCounter++;
        if (this->animLoopCounter >= 2) {
            EnWiz_SetupAttack(this);
        }
    }

    Math_SmoothStepToS(&this->platformLightAlpha, this->targetPlatformLightAlpha, 10, 10, 10);
}

void EnWiz_SetupAttack(EnWiz* this) {
    EnWiz_ChangeAnim(this, EN_WIZ_ANIM_ATTACK, false);
    this->timer = 0;
    this->shouldStartTimer = false;
    this->action = EN_WIZ_ACTION_ATTACK;
    this->actionFunc = EnWiz_Attack;
}

/**
 * Spawns an EnWizFire projectile that is pointed at the player, then disappears.
 */
void EnWiz_Attack(EnWiz* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (this->fightState >= EN_WIZ_FIGHT_STATE_SECOND_PHASE_GHOSTS_RUN_AROUND) {
        EnWiz_MoveGhosts(this);
    }

    if (this->timer == 0) {
        if (Animation_OnFrame(&this->skelAnime, 6.0f) && !this->hasActiveProjectile) {
            Player* player = GET_PLAYER(play);
            Vec3f pos;
            s32 type = this->type;

            Math_Vec3f_Copy(&pos, &this->actor.world.pos);
            pos.x += Math_SinS(this->actor.world.rot.y) * 40.0f;
            pos.y += 60.0f;
            pos.z += Math_CosS(this->actor.world.rot.y) * 40.0f;
            if (type == EN_WIZ_TYPE_FIRE_NO_BGM) {
                type = EN_WIZ_TYPE_FIRE;
            }

            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_WIZ_FIRE, pos.x, pos.y, pos.z,
                               Math_Vec3f_Pitch(&pos, &player->actor.world.pos),
                               Math_Vec3f_Yaw(&pos, &player->actor.world.pos), 0, type * 4);
            this->hasActiveProjectile = true;
            Actor_PlaySfx(&this->actor, NA_SE_EN_WIZ_ATTACK);
            Actor_PlaySfx(&this->actor, NA_SE_PL_MAGIC_FIRE);
        }

        if ((curFrame >= 8.0f) && !this->shouldStartTimer) {
            this->timer = 3;
            this->shouldStartTimer = true;
        }

        if (curFrame >= this->animEndFrame) {
            EnWiz_SetupDisappear(this);
        }
    }
}

void EnWiz_SetupDisappear(EnWiz* this) {
    if (this->action != EN_WIZ_ACTION_DAMAGED) {
        this->angularVelocity = 0x2710;
        this->timer = 0;
        EnWiz_ChangeAnim(this, EN_WIZ_ANIM_IDLE, false);
        this->action = EN_WIZ_ACTION_DISAPPEAR;
    } else {
        this->angularVelocity = 0x2710;
        this->actor.world.rot.y += this->angularVelocity;
    }

    this->targetPlatformLightAlpha = 0;
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    Actor_PlaySfx(&this->actor, NA_SE_EN_WIZ_DISAPPEAR);
    Math_SmoothStepToS(&this->angularVelocity, 0x1388, 0x64, 0x3E8, 0x3E8);
    this->actor.world.rot.y += this->angularVelocity;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = EnWiz_Disappear;
}

/**
 * Spin the Wizrobe around and shrink it so that it disappears into its platform.
 * Afterwards, set it up to appear again.
 */
void EnWiz_Disappear(EnWiz* this, PlayState* play) {
    s32 i;

    Math_SmoothStepToS(&this->angularVelocity, 0, 0xA, 0xBB8, 0x14);
    this->actor.world.rot.y += this->angularVelocity;
    if ((this->fightState == EN_WIZ_FIGHT_STATE_FIRST_PHASE) || (this->action == EN_WIZ_ACTION_DAMAGED)) {
        Math_ApproachZeroF(&this->scale, 0.3f, 0.01f);
        Math_SmoothStepToS(&this->platformLightAlpha, this->targetPlatformLightAlpha, 5, 50, 0);
    } else {
        Math_ApproachZeroF(&this->scale, 0.3f, 0.001f);
        Math_SmoothStepToS(&this->platformLightAlpha, this->targetPlatformLightAlpha, 10, 50, 0);
        for (i = 0; i < this->platformCount; i++) {
            this->ghostRot[i].y += this->angularVelocity;
        }
    }

    Math_Vec3f_Copy(&this->staffTargetFlameScale, &gZeroVec3f);
    if (this->scale < 0.001f) {
        this->scale = 0.0f;

        if ((this->introCutsceneState == EN_WIZ_INTRO_CS_DISAPPEAR) && (this->introCutsceneTimer == 0)) {
            this->introCutsceneState = EN_WIZ_INTRO_CS_END;
            CutsceneManager_Stop(this->actor.csId);
            this->actor.flags &= ~ACTOR_FLAG_FREEZE_EXCEPTION;
        }

        if (this->introCutsceneState != EN_WIZ_INTRO_CS_DISAPPEAR) {
            this->alpha = 0;
            if (this->fightState == EN_WIZ_FIGHT_STATE_FIRST_PHASE) {
                this->ghostColliders.elements[0].base.acDmgInfo.dmgFlags = 0x1000202;
            }

            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->actionFunc = EnWiz_SetupAppear;
        }
    }
}

void EnWiz_SetupDamaged(EnWiz* this, PlayState* play) {
    EnWiz_ChangeAnim(this, EN_WIZ_ANIM_DAMAGE, false);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
    this->timer = 20;

    if ((this->fightState != EN_WIZ_FIGHT_STATE_FIRST_PHASE) && (this->actor.colChkInfo.health <= 0)) {
        Enemy_StartFinishingBlow(play, &this->actor);
        Actor_PlaySfx(&this->actor, NA_SE_EN_WIZ_DEAD);
        this->timer = 0;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    } else {
        Actor_PlaySfx(&this->actor, NA_SE_EN_WIZ_DAMAGE);
    }

    this->scale = 0.015f;
    this->platformCount = 0;
    this->targetPlatformLightAlpha = 0;
    if ((this->drawDmgEffTimer != 0) &&
        ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FIRE) || (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_LIGHT_ORBS))) {
        this->timer = 0;
    }

    this->angularVelocity = 0x4E20;
    if ((this->drawDmgEffTimer != 0) && (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX)) {
        this->angularVelocity = 0;
        this->timer = 0;
    }

    this->action = EN_WIZ_ACTION_DAMAGED;
    this->actionFunc = EnWiz_Damaged;
}

/**
 * Spins the Wizrobe around quickly and makes it jump in the air if it was defeated or if
 * it was damaged in a certain way. Afterwards, the Wizrobe either disappears or dies.
 */
void EnWiz_Damaged(EnWiz* this, PlayState* play) {
    s32 i;

    if ((this->drawDmgEffTimer < 50) && (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX)) {
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, EN_WIZ_BODYPART_MAX, 2, 1.0f, 0.7f);
        this->drawDmgEffTimer = 0;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->angularVelocity = 0x4E20;
        this->actor.velocity.y = 30.0f;
        this->actor.gravity = -3.0f;
    }

    if ((this->drawDmgEffTimer != 0) && (this->drawDmgEffTimer < 30) &&
        ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FIRE) || (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_LIGHT_ORBS))) {
        this->actor.velocity.y = 30.0f;
        this->actor.gravity = -3.0f;
        this->drawDmgEffTimer = 0;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    } else if (!this->isDead && (this->fightState != EN_WIZ_FIGHT_STATE_FIRST_PHASE) &&
               (this->actor.colChkInfo.health <= 0)) {
        this->actor.velocity.y = 30.0f;
        this->actor.gravity = -3.0f;
        this->isDead = true;
    }

    this->actor.world.rot.y += this->angularVelocity;
    Math_SmoothStepToS(&this->angularVelocity, 0, 0xA, 0xBB8, 0x14);
    for (i = 0; i < this->platformCount; i++) {
        this->ghostRot[i].y += this->angularVelocity;
    }

    //! @bug: When the Wizrobe is defeated, it is launched into the air by the code above, and the
    //! last check in this conditional is intended to check that the Wizrobe is standing on its
    //! platform before transitioning to a different state. However, when the fight is in the
    //! EN_WIZ_FIGHT_STATE_SECOND_PHASE_CUTSCENE state, the Wizrobe will always appear on top of
    //! the first platform, while its curPlatformIndex is allowed to randomly choose any other
    //! platform in the room. If the Wizrobe is defeated in this state (which is possible with a
    //! well-timed attack before the cutscene starts), and if the first platform is elevated above
    //! other platforms in the room (as it is in the Secret Shrine), then it is possible for
    //! this->platforms[this->curPlatformIndex]->world.pos.y to be under the floor compared to the
    //! Wizrobe's current position, causing it to get stuck here and never actually die. This can
    //! be fixed by addressing the bug in EnWiz_SelectPlatform.
    if ((this->timer == 1) ||
        ((this->actor.velocity.y < 0.0f) &&
         (this->actor.world.pos.y < (this->platforms[this->curPlatformIndex]->world.pos.y + 11.0f)))) {
        this->timer = 0;
        this->actor.velocity.y = 0.0f;
        this->actor.gravity = 0.0f;
        this->drawDmgEffTimer = this->timer;
        this->drawDmgEffType = this->timer;

        if (this->actor.colChkInfo.health <= 0) {
            if (this->fightState == EN_WIZ_FIGHT_STATE_FIRST_PHASE) {
                this->fightState = EN_WIZ_FIGHT_STATE_SECOND_PHASE_CUTSCENE;
                if ((this->type == EN_WIZ_TYPE_FIRE) || (this->type == EN_WIZ_TYPE_FIRE_NO_BGM)) {
                    this->actor.colChkInfo.health = 8;
                } else {
                    this->actor.colChkInfo.health = 6;
                }

                EnWiz_SetupDisappear(this);
            } else {
                EnWiz_SetupDead(this);
            }
        } else {
            EnWiz_SetupDisappear(this);
        }

        this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    }

    Math_SmoothStepToS(&this->platformLightAlpha, this->targetPlatformLightAlpha, 20, 50, 10);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 35.0f, 40.0f, 40.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
}

void EnWiz_SetupDead(EnWiz* this) {
    EnWiz_ChangeAnim(this, EN_WIZ_ANIM_DAMAGE, false);
    this->angularVelocity = 0x2710;
    this->action = EN_WIZ_ACTION_DEAD;
    this->timer = 0;
    this->actionFunc = EnWiz_Dead;
}

/**
 * Slows the Wizrobe's rotation to a stop, then makes it burst into flames.
 * If the Wizrobe has a switch flag, it will set that switch flag when it dies.
 */
void EnWiz_Dead(EnWiz* this, PlayState* play) {
    s32 i;

    this->actor.world.rot.y += this->angularVelocity;
    Math_SmoothStepToS(&this->angularVelocity, 0, 0xA, 0xBB8, 0x14);
    if (this->angularVelocity < 0x1E) {
        Math_SmoothStepToS(&this->alpha, 0, 10, 30, 20);
        for (i = 0; i < this->platformCount; i++) {
            Math_SmoothStepToS(&this->ghostAlpha[i], 0, 10, 30, 20);
        }

        this->action = EN_WIZ_ACTION_BURST_INTO_FLAMES;
    }

    if (this->alpha < 30) {
        EnWiz_SelectPlatform(this, play);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 50, NA_SE_EN_EXTINCT);
        Actor_Kill(&this->actor);
        if (this->switchFlag > SWITCH_FLAG_NONE) {
            Flags_SetSwitch(play, this->switchFlag);
        }
    }
}

static Color_RGBA8 sDustPrimColor = { 250, 250, 250, 255 };
static Color_RGBA8 sDustEnvTimer = { 180, 180, 180, 255 };

void EnWiz_UpdateDamage(EnWiz* this, PlayState* play) {
    s32 i;
    s32 attackDealsDamage = false;

    if (this->collider.base.acFlags & AC_HIT) {
        this->ghostColliders.base.acFlags &= ~AC_HIT;
        if (this->action < EN_WIZ_ACTION_RUN_IN_CIRCLES) {
            return;
        }

        switch (this->actor.colChkInfo.damageEffect) {
            case EN_WIZ_DMGEFF_NONE:
                attackDealsDamage = true;
                break;

            case EN_WIZ_DMGEFF_FIRE:
                if (this->type == EN_WIZ_TYPE_ICE) {
                    this->drawDmgEffTimer = 40;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                }

                attackDealsDamage = true;
                break;

            case EN_WIZ_DMGEFF_FREEZE:
                if ((this->type == EN_WIZ_TYPE_FIRE) || (this->type == EN_WIZ_TYPE_FIRE_NO_BGM)) {
                    this->drawDmgEffTimer = 80;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_SFX;
                    this->drawDmgEffScale = 0.0f;
                    this->drawDmgEffFrozenSteamScale = 1.5f;
                }

                Actor_ApplyDamage(&this->actor);
                EnWiz_SetupDamaged(this, play);
                break;

            case EN_WIZ_DMGEFF_LIGHT_ORB:
                if (((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
                     (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) ||
                    (this->drawDmgEffTimer == 0)) {
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.focus.pos.x,
                                this->actor.focus.pos.y, this->actor.focus.pos.z, 0, 0, 0,
                                CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                    this->drawDmgEffTimer = 40;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                    attackDealsDamage = true;
                }
                break;
        }

        this->alpha = 255;
        if (attackDealsDamage) {
            Actor_ApplyDamage(&this->actor);
            EnWiz_SetupDamaged(this, play);
            return;
        }
    }

    if ((this->platformCount != 0) && (this->fightState != EN_WIZ_FIGHT_STATE_SECOND_PHASE_CUTSCENE)) {
        for (i = 0; i < this->platformCount; i++) {
            Vec3f accel;
            Vec3f velocity;
            Vec3f pos;
            f32 scaleStep;
            s32 j;

            // If the player throws a Deku Nut or hits a ghost's collider (something that is impossible
            // in the final game, since EnWiz_Init effectively disables them), then the below code will
            // "destroy" the ghost by turning into a cloud of smoke.
            if ((R_TRANS_FADE_FLASH_ALPHA_STEP != 0) ||
                (this->ghostColliders.elements[i + 1].base.acElemFlags & ACELEM_HIT)) {
                //! @bug: If a single ghost is destroyed, then changing the fight state here will cause
                //! strange behavior; the ghosts will stand still and pretend to attack the player like
                //! the real Wizrobe. Since Deku Nuts destroy all ghosts at once, and since the ghost
                //! colliders are effectively disabled, this doesn't cause any problems in the final
                //! game, but it becomes an issue if the ghost colliders are enabled.
                this->fightState = EN_WIZ_FIGHT_STATE_SECOND_PHASE_GHOSTS_COPY_WIZROBE;
                this->ghostColliders.base.acFlags &= ~AC_HIT;
                if ((this->ghostPos[i].x != .0f) || (this->ghostPos[i].z != .0f)) {
                    for (j = 0; j < 9; j++) {
                        accel.x = 0.0f;
                        accel.y = 1.0f;
                        accel.z = 0.0f;
                        velocity.x = 0.0f;
                        velocity.y = 1.0f;
                        velocity.z = 0.0f;
                        scaleStep = Rand_S16Offset(20, 10);
                        Math_Vec3f_Copy(&pos, &this->ghostPos[i]);
                        pos.x += (f32)Rand_S16Offset(20, 20) * ((Rand_ZeroOne() < 0.5f) ? -1 : 1);
                        pos.y += 70.0f + Rand_CenteredFloat(30.0f);
                        pos.z += (f32)Rand_S16Offset(20, 20) * ((Rand_ZeroOne() < 0.5f) ? -1 : 1);
                        func_800B0DE0(play, &pos, &velocity, &accel, &sDustPrimColor, &sDustEnvTimer,
                                      Rand_S16Offset(350, 100), scaleStep);
                    }

                    SoundSource_PlaySfxAtFixedWorldPos(play, &this->ghostPos[i], 50, NA_SE_EN_WIZ_LAUGH);
                    Math_Vec3f_Copy(&this->ghostPos[i], &gZeroVec3f);
                }
            }
        }
    }
}

void EnWiz_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnWiz* this = (EnWiz*)thisx;
    s32 i;
    s32 j;

    if (this->action != EN_WIZ_ACTION_APPEAR) {
        SkelAnime_Update(&this->skelAnime);
        SkelAnime_Update(&this->ghostSkelAnime);
    }

    Actor_SetFocus(&this->actor, 60.0f);
    Actor_SetScale(&this->actor, this->scale);
    EnWiz_UpdateDamage(this, play);
    this->actionFunc(this, play);

    this->actor.shape.rot.y = this->actor.world.rot.y;

    DECR(this->timer);
    DECR(this->introCutsceneTimer);
    DECR(this->drawDmgEffTimer);

    this->collider.dim.radius = 35;
    this->collider.dim.height = 130;
    this->collider.dim.yShift = 0;
    if (this->action >= EN_WIZ_ACTION_RUN_IN_CIRCLES) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->ghostColliders.base);
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }

    Math_ApproachF(&this->staffFlameScale.x, this->staffTargetFlameScale.x, 0.3f, 0.002f);
    Math_ApproachF(&this->staffFlameScale.y, this->staffTargetFlameScale.y, 0.3f, 0.002f);
    Math_ApproachF(&this->staffFlameScale.z, this->staffTargetFlameScale.z, 0.3f, 0.002f);

    if (this->fightState == EN_WIZ_FIGHT_STATE_FIRST_PHASE) {
        this->platformCount = 0;
    } else if (this->fightState == EN_WIZ_FIGHT_STATE_SECOND_PHASE_GHOSTS_RUN_AROUND) {
        for (i = 0; i < this->platformCount; i++) {
            for (j = 0; j < ARRAY_COUNT(this->ghostBaseJointTable); j++) {
                this->ghostJointTables[i][j] = this->ghostBaseJointTable[j];
            }
        }
    } else {
        for (i = 0; i < this->platformCount; i++) {
            for (j = 0; j < ARRAY_COUNT(this->jointTable); j++) {
                this->ghostJointTables[i][j] = this->jointTable[j];
            }
        }
    }
}

void EnWiz_PostLimbDrawOpa(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    Vec3f staffFlamePos = { 0.0f, 0.0f, 0.0f };
    EnWiz* this = (EnWiz*)thisx;

    if (limbIndex == WIZROBE_LIMB_STAFF) {
        staffFlamePos.x = 7300.0f;
        staffFlamePos.y = -1500.0f;
        if (this->action != EN_WIZ_ACTION_DANCE) {
            staffFlamePos.y = 0.0f;
            staffFlamePos.x = 5300.0f;
        }

        Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_MultVec3f(&staffFlamePos, &this->staffFlamePos);
    }

    Collider_UpdateSpheres(limbIndex, &this->ghostColliders);

    if ((limbIndex == WIZROBE_LIMB_PELVIS) || (limbIndex == WIZROBE_LIMB_TORSO) ||
        (limbIndex == WIZROBE_LIMB_LEFT_UPPER_ARM) || (limbIndex == WIZROBE_LIMB_LEFT_FOREARM) ||
        (limbIndex == WIZROBE_LIMB_RIGHT_UPPER_ARM) || (limbIndex == WIZROBE_LIMB_RIGHT_FOREARM) ||
        (limbIndex == WIZROBE_LIMB_NECK) || (limbIndex == WIZROBE_LIMB_HEAD) || (limbIndex == WIZROBE_LIMB_JAW) ||
        (limbIndex == WIZROBE_LIMB_LEFT_SHIN) || (limbIndex == WIZROBE_LIMB_RIGHT_SHIN) ||
        (limbIndex == WIZROBE_LIMB_LOINCLOTH)) {
        Matrix_MultZero(&this->bodyPartsPos[this->bodyPartIndex]);
        this->bodyPartIndex++;
        if (this->bodyPartIndex >= EN_WIZ_BODYPART_MAX) {
            this->bodyPartIndex = 0;
        }
    }
}

void EnWiz_PostLimbDrawXlu(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    Vec3f staffFlamePos = { 0.0f, 0.0f, 0.0f };
    s32 pad;
    EnWiz* this = (EnWiz*)thisx;

    if (this->action != EN_WIZ_ACTION_BURST_INTO_FLAMES) {
        if (limbIndex == WIZROBE_LIMB_STAFF) {
            staffFlamePos.x = 7300.0f;
            staffFlamePos.y = -1500.0f;
            if (this->action != EN_WIZ_ACTION_DANCE) {
                staffFlamePos.y = 0.0f;
                staffFlamePos.x = 5300.0f;
            }

            Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            Matrix_MultVec3f(&staffFlamePos, &this->staffFlamePos);
        }
    } else {
        if (this->timer == 0) {
            Vec3f flamePos;

            Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            Matrix_MultVec3f(&staffFlamePos, &flamePos);
            flamePos.x += Rand_CenteredFloat(4.0f);
            flamePos.y += Rand_CenteredFloat(7.0f);
            flamePos.z += Rand_CenteredFloat(5.0f);
            func_800B3030(play, &flamePos, &gZeroVec3f, &gZeroVec3f, ((Rand_ZeroFloat(1.0f) * 50.0f) + 70.0f), 10, 1);
            SoundSource_PlaySfxAtFixedWorldPos(play, &flamePos, 10, NA_SE_EN_EXTINCT);
        }

        if ((limbIndex >= WIZROBE_LIMB_RIGHT_FOOT) && (this->timer == 0)) {
            this->timer = 4;
        }
    }

    if ((limbIndex == WIZROBE_LIMB_PELVIS) || (limbIndex == WIZROBE_LIMB_TORSO) ||
        (limbIndex == WIZROBE_LIMB_LEFT_UPPER_ARM) || (limbIndex == WIZROBE_LIMB_LEFT_FOREARM) ||
        (limbIndex == WIZROBE_LIMB_RIGHT_UPPER_ARM) || (limbIndex == WIZROBE_LIMB_RIGHT_FOREARM) ||
        (limbIndex == WIZROBE_LIMB_NECK) || (limbIndex == WIZROBE_LIMB_HEAD) || (limbIndex == WIZROBE_LIMB_JAW) ||
        (limbIndex == WIZROBE_LIMB_LEFT_SHIN) || (limbIndex == WIZROBE_LIMB_RIGHT_SHIN) ||
        (limbIndex == WIZROBE_LIMB_LOINCLOTH)) {
        Matrix_MultZero(&this->bodyPartsPos[this->bodyPartIndex]);
        this->bodyPartIndex++;
        if (this->bodyPartIndex >= EN_WIZ_BODYPART_MAX) {
            this->bodyPartIndex = 0;
        }
    }
}

void EnWiz_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnWiz* this = (EnWiz*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    if ((this->action == EN_WIZ_ACTION_BURST_INTO_FLAMES) || (this->alpha != 255)) {
        Scene_SetRenderModeXlu(play, 1, 2);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
        POLY_XLU_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               NULL, EnWiz_PostLimbDrawXlu, &this->actor, POLY_XLU_DISP);
    } else {
        Scene_SetRenderModeXlu(play, 0, 1);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, this->alpha);
        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              NULL, EnWiz_PostLimbDrawOpa, &this->actor);
    }

    if (this->drawDmgEffTimer != 0) {
        f32 drawDmgEffAlpha = this->drawDmgEffTimer * 0.05f;

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

        Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, EN_WIZ_BODYPART_MAX, this->drawDmgEffScale,
                                this->drawDmgEffFrozenSteamScale, drawDmgEffAlpha, this->drawDmgEffType);
    }

    if (this->platformCount > 0) {
        s32 i;
        s16 platformCount;

        Matrix_Push();

        platformCount = this->platformCount;
        if (this->fightState == EN_WIZ_FIGHT_STATE_SECOND_PHASE_CUTSCENE) {
            platformCount = 10;
        }

        for (i = 0; i < platformCount; i++) {
            Gfx_SetupDL25_Opa(play->state.gfxCtx);
            Gfx_SetupDL25_Xlu(play->state.gfxCtx);

            if ((this->ghostPos[i].x != 0.0f) && (this->ghostPos[i].z != 0.0f)) {
                Matrix_Translate(this->ghostPos[i].x, this->ghostPos[i].y + 10.0f, this->ghostPos[i].z, MTXMODE_NEW);
                Matrix_Scale(this->scale, this->scale, this->scale, MTXMODE_APPLY);
                Matrix_RotateYS(this->ghostRot[i].y, MTXMODE_APPLY);
                Matrix_RotateXS(this->ghostRot[i].x, MTXMODE_APPLY);
                Matrix_RotateZS(this->ghostRot[i].z, MTXMODE_APPLY);
                Scene_SetRenderModeXlu(play, 1, 2);
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->ghostAlpha[i]);
                POLY_XLU_DISP =
                    SkelAnime_DrawFlex(play, this->ghostSkelAnime.skeleton, this->ghostJointTables[i],
                                       this->ghostSkelAnime.dListCount, NULL, NULL, &this->actor, POLY_XLU_DISP);
                this->ghostColliders.elements[i + 1].dim.worldSphere.center.x = this->ghostPos[i].x;
                this->ghostColliders.elements[i + 1].dim.worldSphere.center.y = this->ghostPos[i].y + 50.0f;
                this->ghostColliders.elements[i + 1].dim.worldSphere.center.z = this->ghostPos[i].z;
                this->ghostColliders.elements[i + 1].dim.worldSphere.radius =
                    this->ghostColliders.elements[0].dim.modelSphere.radius;
                this->ghostColliders.elements[i + 1].dim.scale = this->ghostColliders.elements[0].dim.scale;
            }
        }

        Matrix_Pop();
    }

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    // Draw the light emanating from the Wizrobe's platform
    if (this->fightState == EN_WIZ_FIGHT_STATE_FIRST_PHASE) {
        Matrix_Push();

        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gWizrobePlatformLightTexAnim));
        Matrix_Translate(this->platformLightPos.x, this->platformLightPos.y, this->platformLightPos.z, MTXMODE_NEW);
        Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, this->platformLightAlpha);

        if ((this->type == EN_WIZ_TYPE_FIRE) || (this->type == EN_WIZ_TYPE_FIRE_NO_BGM)) {
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 100, 255);
        } else {
            gDPSetEnvColor(POLY_XLU_DISP++, 50, 0, 255, 255);
        }

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gWizrobePlatformLightDL);

        Matrix_Pop();
    }

    // Draw the flame at the tip of the Wizrobe's staff
    Matrix_Translate(this->staffFlamePos.x, this->staffFlamePos.y, this->staffFlamePos.z, MTXMODE_NEW);
    Matrix_Scale(this->staffFlameScale.x, this->staffFlameScale.y, this->staffFlameScale.z, MTXMODE_APPLY);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0,
                                  ((this->staffFlameScroll * 10) - (play->state.frames * 20)) % 512, 32, 128, 0, 0, 0,
                                  -20));
    gDPPipeSync(POLY_XLU_DISP++);

    if ((this->type == EN_WIZ_TYPE_FIRE) || (this->type == EN_WIZ_TYPE_FIRE_NO_BGM)) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 170, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 50, 0, 255);
    } else {
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 170, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 255, 255);
    }

    Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    CLOSE_DISPS(play->state.gfxCtx);
}
