/*
 * File: z_en_jso.c
 * Overlay: ovl_En_Jso
 * Description: Garo
 */

#include "z_en_jso.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/actors/ovl_En_Encount3/z_en_encount3.h"
#include "overlays/actors/ovl_En_Part/z_en_part.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnJso_Init(Actor* thisx, PlayState* play);
void EnJso_Destroy(Actor* thisx, PlayState* play);
void EnJso_Update(Actor* thisx, PlayState* play);
void EnJso_Draw(Actor* thisx, PlayState* play);

void EnJso_SetupIntroCutscene(EnJso* this);
void EnJso_IntroCutscene(EnJso* this, PlayState* play);
void EnJso_Reappear(EnJso* this, PlayState* play);
void EnJso_SetupCirclePlayer(EnJso* this, PlayState* play);
void EnJso_CirclePlayer(EnJso* this, PlayState* play);
void EnJso_Guard(EnJso* this, PlayState* play);
void EnJso_SetupSpinBeforeAttack(EnJso* this);
void EnJso_SpinBeforeAttack(EnJso* this, PlayState* play);
void EnJso_SetupDashAttack(EnJso* this);
void EnJso_DashAttack(EnJso* this, PlayState* play);
void EnJso_SetupSlash(EnJso* this, PlayState* play);
void EnJso_Slash(EnJso* this, PlayState* play);
void EnJso_SetupWaitAfterSlash(EnJso* this);
void EnJso_WaitAfterSlash(EnJso* this, PlayState* play);
void EnJso_SetupKnockedBack(EnJso* this);
void EnJso_KnockedBack(EnJso* this, PlayState* play);
void EnJso_SetupCower(EnJso* this);
void EnJso_Cower(EnJso* this, PlayState* play);
void EnJso_Stunned(EnJso* this, PlayState* play);
void EnJso_Damaged(EnJso* this, PlayState* play);
void EnJso_SetupJumpBack(EnJso* this);
void EnJso_JumpBack(EnJso* this, PlayState* play);
void EnJso_Dead(EnJso* this, PlayState* play);
void EnJso_SetupFallDownAndTalk(EnJso* this, PlayState* play);
void EnJso_FallDownAndTalk(EnJso* this, PlayState* play);
void EnJso_TellHint(EnJso* this, PlayState* play);
void EnJso_BurstIntoFlames(EnJso* this, PlayState* play);

typedef enum EnJsoAction {
    /*  0 */ EN_JSO_ACTION_INTRO_CUTSCENE,
    /*  1 */ EN_JSO_ACTION_REAPPEAR,
    /*  2 */ EN_JSO_ACTION_CIRCLE_PLAYER,
    /*  3 */ EN_JSO_ACTION_GUARD,
    /*  4 */ EN_JSO_ACTION_SPIN_BEFORE_ATTACK,
    /*  5 */ EN_JSO_ACTION_DASH_ATTACK,
    /*  6 */ EN_JSO_ACTION_SLASH,
    /*  7 */ EN_JSO_ACTION_WAIT_AFTER_SLASH,
    /*  8 */ EN_JSO_ACTION_STUNNED,
    /*  9 */ EN_JSO_ACTION_KNOCKED_BACK,
    /* 10 */ EN_JSO_ACTION_COWER,
    /* 11 */ EN_JSO_ACTION_DAMAGED,
    /* 12 */ EN_JSO_ACTION_JUMP_BACK,
    /* 13 */ EN_JSO_ACTION_DEAD,
    /* 14 */ EN_JSO_ACTION_FALL_DOWN_AND_TALK,
    /* 15 */ EN_JSO_ACTION_UNK_15 // Checked in EnJso_Update, but never actually used
} EnJsoAction;

typedef enum EnJsoIntroType {
    /* 0 */ EN_JSO_INTRO_SPIN_UP_FROM_GROUND,
    /* 1 */ EN_JSO_INTRO_JUMP_OUT_FROM_GROUND,
    /* 2 */ EN_JSO_INTRO_LAND_FROM_ABOVE,
    /* 3 */ EN_JSO_INTRO_SCALE_UP
} EnJsoIntroType;

typedef enum EnJsoIntroCsState {
    // Either the cutscene started (in which case, we'll transition to the next state on the next frame) or it's done.
    /* 0 */ EN_JSO_INTRO_CS_STATE_DONE_OR_STARTED,

    // Waits for the Garo to finish their intro animation and open the first textbox.
    /* 1 */ EN_JSO_INTRO_CS_STATE_WAITING_FOR_TEXTBOX_TO_APPEAR,

    // Waits for the player to press a button to continue on from the Garo's first textbox.
    /* 2 */ EN_JSO_INTRO_CS_STATE_WAITING_FOR_TEXTBOX_TO_CONTINUE,

    // Lifts the Garo's right arm such that, when they draw their sword, it's pointed at the player. Waits until the
    // player has finished reading all of the Garo's dialogue before continuing.
    /* 3 */ EN_JSO_INTRO_CS_STATE_RAISE_ARM_AND_DRAW_RIGHT_SWORD,

    // Waits 10 frames, then the Garo draws its left sword and continues to the next state.
    /* 4 */ EN_JSO_INTRO_CS_STATE_DRAW_LEFT_SWORD,

    // Waits 10 frames, then the Garo jumps back and the cutscene ends.
    /* 5 */ EN_JSO_INTRO_CS_STATE_ENDING
} EnJsoIntroCsState;

typedef enum EnJsoSwordState {
    /* 0 */ EN_JSO_SWORD_STATE_BOTH_DRAWN,
    /* 1 */ EN_JSO_SWORD_STATE_KNOCKED_OUT_OF_HANDS,
    /* 2 */ EN_JSO_SWORD_STATE_RIGHT_DRAWN,
    /* 3 */ EN_JSO_SWORD_STATE_LEFT_DRAWN,
    /* 5 */ EN_JSO_SWORD_STATE_NONE_DRAWN = 5
} EnJsoSwordState;

// Seemingly a duplicate of the isAttacking instance variable. Its purpose is unknown.
static s32 sIsAttacking = false;

// Seemingly a duplicate of the isPlayerLockedOn instance variable. Its purpose is unknown.
static s32 sIsPlayerLockedOn = false;

typedef enum EnJsoDamageEffect {
    /* 0x0 */ EN_JSO_DMGEFF_IMMUNE,        // Deals no damage and has no special effect
    /* 0x1 */ EN_JSO_DMGEFF_STUN,          // Deals no damage but stuns the Garo
    /* 0x2 */ EN_JSO_DMGEFF_FIRE,          // Damages and sets the Garo on fire
    /* 0x3 */ EN_JSO_DMGEFF_FREEZE,        // Damages and surrounds the Garo with ice
    /* 0x4 */ EN_JSO_DMGEFF_LIGHT_ORB,     // Damages and surrounds the Garo with light orbs
    /* 0x5 */ EN_JSO_DMGEFF_ELECTRIC_STUN, // Deals no damage but stuns the Garo and surrounds them with electric sparks
    /* 0xF */ EN_JSO_DMGEFF_NONE = 0xF     // Damages the Garo and has no special effect
} EnJsoDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, EN_JSO_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, EN_JSO_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(0, EN_JSO_DMGEFF_IMMUNE),
    /* Explosives     */ DMG_ENTRY(1, EN_JSO_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, EN_JSO_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, EN_JSO_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, EN_JSO_DMGEFF_IMMUNE),
    /* Hookshot       */ DMG_ENTRY(0, EN_JSO_DMGEFF_STUN),
    /* Goron punch    */ DMG_ENTRY(1, EN_JSO_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(1, EN_JSO_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(1, EN_JSO_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(2, EN_JSO_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(2, EN_JSO_DMGEFF_FREEZE),
    /* Light arrow    */ DMG_ENTRY(2, EN_JSO_DMGEFF_LIGHT_ORB),
    /* Goron spikes   */ DMG_ENTRY(1, EN_JSO_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(0, EN_JSO_DMGEFF_STUN),
    /* Deku bubble    */ DMG_ENTRY(1, EN_JSO_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(2, EN_JSO_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, EN_JSO_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(0, EN_JSO_DMGEFF_ELECTRIC_STUN),
    /* Normal shield  */ DMG_ENTRY(0, EN_JSO_DMGEFF_IMMUNE),
    /* Light ray      */ DMG_ENTRY(0, EN_JSO_DMGEFF_IMMUNE),
    /* Thrown object  */ DMG_ENTRY(1, EN_JSO_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, EN_JSO_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, EN_JSO_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, EN_JSO_DMGEFF_IMMUNE),
    /* Normal Roll    */ DMG_ENTRY(0, EN_JSO_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, EN_JSO_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, EN_JSO_DMGEFF_IMMUNE),
    /* Unblockable    */ DMG_ENTRY(0, EN_JSO_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, EN_JSO_DMGEFF_IMMUNE),
    /* Powder Keg     */ DMG_ENTRY(1, EN_JSO_DMGEFF_NONE),
};

ActorProfile En_Jso_Profile = {
    /**/ ACTOR_EN_JSO,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_JSO,
    /**/ sizeof(EnJso),
    /**/ EnJso_Init,
    /**/ EnJso_Destroy,
    /**/ EnJso_Update,
    /**/ NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x08, 0x04 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 22, 55, 0, { 0, 0, 0 } },
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

// These text IDs are the ones common to all Garos, regardless of which hint they provide. This includes their dialogue
// during their intro cutscene, as well as most of their dialogue when they are defeated.
static u16 sTextIds[] = {
    0x1396,
    0x1397,
    0x1398,
    0x1399,
};

typedef enum EnJsoAnimation {
    /*  0 */ EN_JSO_ANIM_APPEAR,
    /*  1 */ EN_JSO_ANIM_IDLE,
    /*  2 */ EN_JSO_ANIM_BOUNCE,
    /*  3 */ EN_JSO_ANIM_GUARD,
    /*  4 */ EN_JSO_ANIM_DASH_ATTACK,
    /*  5 */ EN_JSO_ANIM_SLASH_START,
    /*  6 */ EN_JSO_ANIM_SLASH_LOOP,
    /*  7 */ EN_JSO_ANIM_KNOCKED_BACK,
    /*  8 */ EN_JSO_ANIM_COWER,
    /*  9 */ EN_JSO_ANIM_DAMAGED,
    /* 10 */ EN_JSO_ANIM_JUMP_BACK,
    /* 11 */ EN_JSO_ANIM_FALL_DOWN,
    /* 12 */ EN_JSO_ANIM_MAX
} EnJsoAnimation;

static AnimationHeader* sAnimations[EN_JSO_ANIM_MAX] = {
    &gGaroAppearAnim,      // EN_JSO_ANIM_APPEAR
    &gGaroIdleAnim,        // EN_JSO_ANIM_IDLE
    &gGaroBounceAnim,      // EN_JSO_ANIM_BOUNCE
    &gGaroGuardAnim,       // EN_JSO_ANIM_GUARD
    &gGaroDashAttackAnim,  // EN_JSO_ANIM_DASH_ATTACK
    &gGaroSlashStartAnim,  // EN_JSO_ANIM_SLASH_START
    &gGaroSlashLoopAnim,   // EN_JSO_ANIM_SLASH_LOOP
    &gGaroKnockedBackAnim, // EN_JSO_ANIM_KNOCKED_BACK
    &gGaroCowerAnim,       // EN_JSO_ANIM_COWER
    &gGaroDamagedAnim,     // EN_JSO_ANIM_DAMAGED
    &gGaroJumpBackAnim,    // EN_JSO_ANIM_JUMP_BACK
    &gGaroFallDownAnim,    // EN_JSO_ANIM_FALL_DOWN
};

static u8 sAnimationModes[EN_JSO_ANIM_MAX] = {
    ANIMMODE_ONCE, // EN_JSO_ANIM_APPEAR
    ANIMMODE_LOOP, // EN_JSO_ANIM_IDLE
    ANIMMODE_LOOP, // EN_JSO_ANIM_BOUNCE
    ANIMMODE_ONCE, // EN_JSO_ANIM_GUARD
    ANIMMODE_ONCE, // EN_JSO_ANIM_DASH_ATTACK
    ANIMMODE_ONCE, // EN_JSO_ANIM_SLASH_START
    ANIMMODE_LOOP, // EN_JSO_ANIM_SLASH_LOOP
    ANIMMODE_ONCE, // EN_JSO_ANIM_KNOCKED_BACK
    ANIMMODE_LOOP, // EN_JSO_ANIM_COWER
    ANIMMODE_LOOP, // EN_JSO_ANIM_DAMAGED
    ANIMMODE_ONCE, // EN_JSO_ANIM_JUMP_BACK
    ANIMMODE_ONCE, // EN_JSO_ANIM_FALL_DOWN
};

void EnJso_Init(Actor* thisx, PlayState* play) {
    EnJso* this = (EnJso*)thisx;
    EffectBlureInit1 rightSwordBlureInit;
    EffectBlureInit1 leftSwordBlureInit;

    this->actor.hintId = TATL_HINT_ID_GARO;
    this->actor.attentionRangeType = ATTENTION_RANGE_5;
    this->actor.colChkInfo.mass = 80;
    this->actor.colChkInfo.health = 3;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
    this->actor.colChkInfo.damageTable = &sDamageTable;
    this->actor.shape.shadowScale = 0.0f;
    SkelAnime_InitFlex(play, &this->skelAnime, &gGaroSkel, &gGaroAppearAnim, this->jointTable, this->morphTable,
                       GARO_LIMB_MAX);
    this->actor.focus.pos = this->actor.world.pos;
    Collider_InitAndSetCylinder(play, &this->bodyCollider, &this->actor, &sCylinderInit);
    Collider_InitAndSetQuad(play, &this->rightSwordCollider, &this->actor, &sQuadInit);
    Collider_InitAndSetQuad(play, &this->leftSwordCollider, &this->actor, &sQuadInit);

    rightSwordBlureInit.p1StartColor[0] = leftSwordBlureInit.p1StartColor[0] = rightSwordBlureInit.p1StartColor[1] =
        leftSwordBlureInit.p1StartColor[1] = rightSwordBlureInit.p1StartColor[2] = leftSwordBlureInit.p1StartColor[2] =
            rightSwordBlureInit.p1StartColor[3] = leftSwordBlureInit.p1StartColor[3] =
                rightSwordBlureInit.p2StartColor[0] = leftSwordBlureInit.p2StartColor[0] =
                    rightSwordBlureInit.p2StartColor[1] = leftSwordBlureInit.p2StartColor[1] =
                        rightSwordBlureInit.p2StartColor[2] = leftSwordBlureInit.p2StartColor[2] =
                            rightSwordBlureInit.p1EndColor[0] = leftSwordBlureInit.p1EndColor[0] =
                                rightSwordBlureInit.p1EndColor[1] = leftSwordBlureInit.p1EndColor[1] =
                                    rightSwordBlureInit.p1EndColor[2] = leftSwordBlureInit.p1EndColor[2] =
                                        rightSwordBlureInit.p2EndColor[0] = leftSwordBlureInit.p2EndColor[0] =
                                            rightSwordBlureInit.p2EndColor[1] = leftSwordBlureInit.p2EndColor[1] =
                                                rightSwordBlureInit.p2EndColor[2] = leftSwordBlureInit.p2EndColor[2] =
                                                    255;
    leftSwordBlureInit.p2StartColor[3] = rightSwordBlureInit.p2StartColor[3] = 64;
    rightSwordBlureInit.p1EndColor[3] = leftSwordBlureInit.p1EndColor[3] = rightSwordBlureInit.p2EndColor[3] =
        leftSwordBlureInit.p2EndColor[3] = 0;
    rightSwordBlureInit.elemDuration = leftSwordBlureInit.elemDuration = 8;
    rightSwordBlureInit.unkFlag = leftSwordBlureInit.unkFlag = 0;
    rightSwordBlureInit.calcMode = leftSwordBlureInit.calcMode = 2;

    Effect_Add(play, &this->rightSwordBlureIndex, EFFECT_BLURE1, 0, 0, &rightSwordBlureInit);
    Effect_Add(play, &this->leftSwordBlureIndex, EFFECT_BLURE1, 0, 0, &leftSwordBlureInit);

    this->actor.gravity = -3.0f;
    this->scale = 0.035f;
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->hintType = EN_JSO_GET_HINT_TYPE(&this->actor);
    this->introCsType = this->hintType & EN_JSO_INTRO_SCALE_UP;
    EnJso_SetupIntroCutscene(this);
}

void EnJso_Destroy(Actor* thisx, PlayState* play) {
    EnJso* this = (EnJso*)thisx;
    EnEncount3* parent;

    Collider_DestroyCylinder(play, &this->bodyCollider);
    Collider_DestroyQuad(play, &this->rightSwordCollider);
    Collider_DestroyQuad(play, &this->leftSwordCollider);

    if ((this->actor.parent != NULL) && (this->actor.parent->update != NULL)) {
        parent = (EnEncount3*)this->actor.parent;
        if (parent->actor.update != NULL) {
            parent->child = NULL;
            if (parent->unk14E > 0) {
                parent->unk14E--;
            }
        }
    }

    Effect_Destroy(play, this->rightSwordBlureIndex);
    Effect_Destroy(play, this->leftSwordBlureIndex);
}

void EnJso_ChangeAnim(EnJso* this, s32 animIndex) {
    f32 morphFrames = -4.0f;

    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);
    if ((animIndex == EN_JSO_ANIM_GUARD) || (animIndex == EN_JSO_ANIM_DAMAGED)) {
        morphFrames = 0.0f;
    }

    Animation_Change(&this->skelAnime, sAnimations[animIndex], 1.0f, 0.0f, this->animEndFrame,
                     sAnimationModes[animIndex], morphFrames);
}

void EnJso_SetupIntroCutscene(EnJso* this) {
    EnEncount3* parent = (EnEncount3*)this->actor.parent;

    this->csId = parent->csId;
    this->swordState = EN_JSO_SWORD_STATE_NONE_DRAWN;
    this->action = EN_JSO_ACTION_INTRO_CUTSCENE;
    this->actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;
    this->actionFunc = EnJso_IntroCutscene;
}

/**
 * Responsible for handling all aspects of the intro cutscene, including manipulating the sub-camera, making the Garo
 * appear in one of four different ways, activating the ring of fire, etc. When the cutscene is over, the Garo will
 * jump back to start the fight.
 */
void EnJso_IntroCutscene(EnJso* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame = this->skelAnime.curFrame;
    s16 showTextbox;
    s16 diffToTargetRobeRightRot;

    switch (this->cutsceneState) {
        case EN_JSO_INTRO_CS_STATE_DONE_OR_STARTED:
            if (!CutsceneManager_IsNext(this->csId)) {
                CutsceneManager_Queue(this->csId);
                return;
            }

            CutsceneManager_StartWithPlayerCs(this->csId, &this->actor);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_21);
            this->subCamId = CutsceneManager_GetCurrentSubCamId(this->actor.csId);
            player->actor.world.pos.x = this->actor.home.pos.x + 30.0f;
            player->actor.world.pos.z = this->actor.home.pos.z + 30.0f;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->subCamEyeNext.x = player->actor.world.pos.x;
            this->subCamEyeNext.y = player->actor.world.pos.y;
            this->subCamEyeNext.z = player->actor.world.pos.z;
            this->subCamAtNext.x = player->actor.world.pos.x;
            this->subCamAtNext.y = player->actor.world.pos.y;
            this->subCamAtNext.z = player->actor.world.pos.z;
            player->actor.shape.rot.y = player->actor.world.rot.y =
                Math_Vec3f_Yaw(&player->actor.world.pos, &this->actor.world.pos);
            Math_Vec3f_Copy(&this->subCamEye, &this->subCamEyeNext);
            Math_Vec3f_Copy(&this->subCamAt, &this->subCamAtNext);
            EnJso_ChangeAnim(this, EN_JSO_ANIM_IDLE);
            this->actor.draw = EnJso_Draw;
            Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_ENTRY);
            this->actor.shape.yOffset = 970.0f;

            switch (this->introCsType) {
                case EN_JSO_INTRO_SPIN_UP_FROM_GROUND:
                    this->actor.shape.yOffset = -300.0f;
                    break;

                case EN_JSO_INTRO_JUMP_OUT_FROM_GROUND:
                    EnJso_ChangeAnim(this, EN_JSO_ANIM_APPEAR);
                    Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale,
                                             1, 8.0f, 500, 10, true);
                    this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                    break;

                case EN_JSO_INTRO_LAND_FROM_ABOVE:
                    this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                    this->actor.world.pos.y = this->actor.home.pos.y + 200.0f;
                    break;

                case EN_JSO_INTRO_SCALE_UP:
                    this->scale = 0.0f;
                    break;

                default:
                    break;
            }

            this->cutsceneTimer = 0;
            this->cutsceneState++;
            break;

        case EN_JSO_INTRO_CS_STATE_WAITING_FOR_TEXTBOX_TO_APPEAR:
            player->actor.world.pos.x = this->actor.home.pos.x + 30.0f;
            player->actor.world.pos.z = this->actor.home.pos.z + 90.0f;
            this->subCamEyeNext.x = player->actor.world.pos.x - 170.0f;
            this->subCamEyeNext.y = player->actor.world.pos.y + 30.0f;
            this->subCamEyeNext.z = player->actor.world.pos.z - 80.0f;
            this->subCamAtNext.x = player->actor.world.pos.x + 200.0f;
            this->subCamAtNext.y = player->actor.world.pos.y + 10.0f;
            this->subCamAtNext.z = player->actor.world.pos.z;
            Math_ApproachF(&this->actor.shape.shadowScale, 16.0f, 0.4f, 4.0f);
            showTextbox = false;

            switch (this->introCsType) {
                case EN_JSO_INTRO_SPIN_UP_FROM_GROUND:
                    Math_ApproachF(&this->actor.shape.yOffset, 970.0f, 0.5f, 100.0f);
                    this->actor.shape.rot.y += 0x1770;

                    if (this->actor.shape.yOffset >= 969.0f) {
                        showTextbox = true;
                    }

                    if ((play->gameplayFrames % 4) == 0) {
                        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos,
                                                 this->actor.shape.shadowScale, 1, 8.0f, 500, 10, true);
                    }
                    break;

                case EN_JSO_INTRO_JUMP_OUT_FROM_GROUND:
                    if (curFrame >= this->animEndFrame) {
                        showTextbox = true;
                        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos,
                                                 this->actor.shape.shadowScale, 1, 8.0f, 500, 10, true);
                    }
                    break;

                case EN_JSO_INTRO_LAND_FROM_ABOVE:
                    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                        showTextbox = true;
                        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos,
                                                 this->actor.shape.shadowScale, 1, 8.0f, 500, 10, true);
                    }
                    break;

                case EN_JSO_INTRO_SCALE_UP:
                    Math_ApproachF(&this->scale, 0.035f, 0.2f, 0.03f);
                    if (this->scale >= 0.034f) {
                        this->scale = 0.035f;
                        showTextbox = true;
                    }
                    break;

                default:
                    break;
            }

            if (showTextbox) {
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_4);
                Message_StartTextbox(play, sTextIds[this->textIndex], &this->actor);
                this->textIndex++;
                this->actor.shape.yOffset = 970.0f;
                this->cutsceneTimer = 0;
                this->cutsceneState++;
            }
            break;

        case EN_JSO_INTRO_CS_STATE_WAITING_FOR_TEXTBOX_TO_CONTINUE:
            this->subCamEyeNext.x = player->actor.world.pos.x - 30.0f;
            this->subCamEyeNext.y = player->actor.world.pos.y + 30.0f;
            this->subCamEyeNext.z = player->actor.world.pos.z + 70.0f;
            this->subCamAtNext.x = player->actor.world.pos.x - 20.0f;
            this->subCamAtNext.y = player->actor.world.pos.y + 20.0f;
            this->subCamAtNext.z = player->actor.world.pos.z;
            Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xBB8, 0);

            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                Message_ContinueTextbox(play, sTextIds[this->textIndex]);
                this->cutsceneTimer = 0;
                this->textIndex++;
                this->cutsceneState++;
            }
            break;

        case EN_JSO_INTRO_CS_STATE_RAISE_ARM_AND_DRAW_RIGHT_SWORD:
            Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xBB8, 0);

            if (Message_GetState(&play->msgCtx) == TEXT_STATE_AWAITING_NEXT) {
                this->targetRobeRightRot.x = -0x2710;
                this->targetRobeRightRot.z = -0x2710;
            }

            diffToTargetRobeRightRot = this->targetRobeRightRot.x - this->robeRightRot.x;
            diffToTargetRobeRightRot = ABS_ALT(diffToTargetRobeRightRot);

            if (this->targetRobeRightRot.x != 0) {
                if ((ABS_ALT(diffToTargetRobeRightRot) < 0x64) && (this->swordState == EN_JSO_SWORD_STATE_NONE_DRAWN)) {
                    this->swordState = EN_JSO_SWORD_STATE_RIGHT_DRAWN;
                    this->targetRightArmRot.x = -0x32C8;
                    this->targetRightArmRot.y = -0xBB8;
                    this->rightArmRot.z = this->targetRightArmRot.z = -0x32C8;
                    Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_SWORD);
                }
            }

            this->subCamEyeNext.x = player->actor.world.pos.x + 100.0f;
            this->subCamEyeNext.y = player->actor.world.pos.y + 60.0f;
            this->subCamEyeNext.z = player->actor.world.pos.z - 30.0f;
            this->subCamAtNext.x = player->actor.world.pos.x - 35.0f;
            this->subCamAtNext.y = player->actor.world.pos.y + 30.0f;
            this->subCamAtNext.z = player->actor.world.pos.z - 40.0f;

            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                this->targetRightArmRot.x = this->targetRightArmRot.y = this->targetRightArmRot.z =
                    this->targetRobeRightRot.x = this->targetRobeRightRot.y = this->targetRobeRightRot.z = 0;
                this->cutsceneTimer = 0;
                this->cutsceneState++;
            }
            break;

        case EN_JSO_INTRO_CS_STATE_DRAW_LEFT_SWORD:
            this->subCamEyeNext.x = player->actor.world.pos.x - 40.0f;
            this->subCamEyeNext.y = player->actor.world.pos.y + 20.0f;
            this->subCamEyeNext.z = player->actor.world.pos.z;
            this->subCamAtNext.x = player->actor.world.pos.x - 30.0f;
            this->subCamAtNext.y = player->actor.world.pos.y + 40.0f;
            this->subCamAtNext.z = player->actor.world.pos.z - 130.0f;
            this->cutsceneTimer++;

            if (this->cutsceneTimer >= 10) {
                this->swordState = EN_JSO_SWORD_STATE_BOTH_DRAWN;
                Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_SWORD);
                if ((this->actor.parent != NULL) && (this->actor.parent->update != NULL)) {
                    EnEncount3* parent = (EnEncount3*)this->actor.parent;

                    if (parent->actor.update != NULL) {
                        parent->unk148 = 1;
                        play->unk_18880 = true;
                    }
                }

                EnJso_ChangeAnim(this, EN_JSO_ANIM_IDLE);
                this->cutsceneTimer = 0;
                this->cutsceneState++;
            }
            break;

        case EN_JSO_INTRO_CS_STATE_ENDING:
            this->subCamEyeNext.x = player->actor.world.pos.x - 40.0f;
            this->subCamEyeNext.y = player->actor.world.pos.y + 20.0f;
            this->subCamEyeNext.z = player->actor.world.pos.z;
            this->subCamAtNext.x = player->actor.world.pos.x - 30.0f;
            this->subCamAtNext.y = player->actor.world.pos.y + 40.0f;
            this->subCamAtNext.z = player->actor.world.pos.z - 130.0f;
            this->cutsceneTimer++;

            if (this->cutsceneTimer >= 10) {
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                CutsceneManager_Stop(this->csId);
                this->rightArmRot.x = this->rightArmRot.y = this->rightArmRot.z = this->robeRightRot.x =
                    this->robeRightRot.y = this->robeRightRot.z = 0;
                this->cutsceneState = EN_JSO_INTRO_CS_STATE_DONE_OR_STARTED;
                this->subCamId = SUB_CAM_ID_DONE;
                this->actor.flags &= ~ACTOR_FLAG_FREEZE_EXCEPTION;
                this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                EnJso_SetupJumpBack(this);
            }
            break;
    }

    Math_SmoothStepToS(&this->rightArmRot.x, this->targetRightArmRot.x, 1, 0xBB8, 0);
    Math_SmoothStepToS(&this->rightArmRot.y, this->targetRightArmRot.y, 1, 0xBB8, 0);
    Math_SmoothStepToS(&this->rightArmRot.z, this->targetRightArmRot.z, 1, 0xBB8, 0);
    Math_SmoothStepToS(&this->robeRightRot.x, this->targetRobeRightRot.x, 1, 0xBB8, 0);
    Math_SmoothStepToS(&this->robeRightRot.y, this->targetRobeRightRot.y, 1, 0xBB8, 0);
    Math_SmoothStepToS(&this->robeRightRot.z, this->targetRobeRightRot.z, 1, 0xBB8, 0);
    Math_Vec3f_Copy(&this->subCamEye, &this->subCamEyeNext);
    Math_Vec3f_Copy(&this->subCamAt, &this->subCamAtNext);

    if (this->subCamId != SUB_CAM_ID_DONE) {
        this->subCamUp.x = 0.0f;
        this->subCamUp.y = 1.0f;
        this->subCamUp.z = 0.0f;
        Play_SetCameraAtEyeUp(play, this->subCamId, &this->subCamAt, &this->subCamEye, &this->subCamUp);
    }
}

void EnJso_SetupReappear(EnJso* this, PlayState* play) {
    EnJso_ChangeAnim(this, EN_JSO_ANIM_APPEAR);
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;

    if (this->isPlayerLockedOn) {
        this->isPlayerLockedOn = false;
        sIsPlayerLockedOn = false;
    }

    if (this->isAttacking) {
        this->isAttacking = false;
        sIsAttacking = false;
    }

    if (this->swordState != EN_JSO_SWORD_STATE_BOTH_DRAWN) {
        this->swordState = EN_JSO_SWORD_STATE_BOTH_DRAWN;
    }

    this->actor.shape.rot.x = this->actor.world.rot.x = 0;
    this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
    this->actor.floorHeight = this->actor.home.pos.y;
    Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1, 8.0f, 500,
                             10, true);
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_ENTRY);
    this->afterimageCount = 0;
    this->action = EN_JSO_ACTION_REAPPEAR;
    this->actor.gravity = 0.0f;
    this->actionFunc = EnJso_Reappear;
}

/**
 * This function makes the Garo jump out of the ground from its home position. It's intended to be used if the Garo gets
 * too far away from the player (for example, if the Garo's dash attack makes it fall off a ledge); this will ensure the
 * Garo is back inside the ring of fire, so the player can interact with them again.
 */
void EnJso_Reappear(EnJso* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (this->actor.colChkInfo.health <= 0) {
        Player* player = GET_PLAYER(play);

        player->actor.freezeTimer = 3;
    }

    Math_ApproachF(&this->actor.shape.shadowScale, 16.0f, 0.4f, 4.0f);
    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0xA, 0xBB8, 0x14);

    if ((play->gameplayFrames % 8) == 0) {
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1, 8.0f,
                                 500, 10, true);
    }

    if (curFrame >= this->animEndFrame) {
        this->actor.gravity = -3.0f;
        if (this->actor.colChkInfo.health > 0) {
            this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
            EnJso_SetupCirclePlayer(this, play);
        } else {
            EnJso_SetupFallDownAndTalk(this, play);
        }
    }
}

void EnJso_SetupCirclePlayer(EnJso* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    EnJso_ChangeAnim(this, EN_JSO_ANIM_BOUNCE);
    this->circlingAngularVelocity = 0x258;
    this->action = EN_JSO_ACTION_CIRCLE_PLAYER;
    this->circlingAngle = player->actor.shape.rot.y;
    this->actionFunc = EnJso_CirclePlayer;
}

/**
 * Makes the Garo bounce in a circle around the player, sometimes randomly switching the direction it's traveling, until
 * the attack timer reaches 0. Once it does, then the Garo will prepare to attack.
 */
void EnJso_CirclePlayer(EnJso* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    Player* player = GET_PLAYER(play);
    Vec3f targetPos;
    s32 pad;

    Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_MOVING - SFX_FLAG);

    if ((this->action != EN_JSO_ACTION_REAPPEAR) && (this->action != EN_JSO_ACTION_INTRO_CUTSCENE) &&
        (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) > 60.0f)) {
        EnJso_SetupReappear(this, play);
        return;
    }

    if (curFrame < this->animEndFrame) {
        SkelAnime_Update(&this->skelAnime);
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        SkelAnime_Update(&this->skelAnime);
    }

    if (Animation_OnFrame(&this->skelAnime, 6.0f)) {
        this->actor.velocity.y = 10.0f;
        if ((play->gameplayFrames % 2) == 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_CRYING);
        }
    }

    if (Animation_OnFrame(&this->skelAnime, 12.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_SKIP);
        this->actor.speed = 0.0f;
        if (Rand_ZeroFloat(1.0f) < 0.5f) {
            this->circlingAngularVelocity = -this->circlingAngularVelocity;
        }
    }

    //! @note: Since sIsPlayerLockedOn always mirrors the value of this->isPlayerLockedOn, the last part of this
    //! if-statement is always true and has no impact on the logic. In other words, the Garo doesn't care whether or not
    //! the player is locked on; it will attack regardless as long as the other conditions are met.
    if ((this->attackTimer == 0) && !sIsAttacking && (!sIsPlayerLockedOn || this->isPlayerLockedOn)) {
        sIsAttacking = true;
        this->isAttacking = true;
        this->action = EN_JSO_ACTION_SPIN_BEFORE_ATTACK;
        this->actor.speed = 0.0f;
        EnJso_SetupSpinBeforeAttack(this);
        return;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0xFA0, 0x14);
    Math_ApproachF(&this->actor.speed, 5.0f, 0.3f, 2.0f);
    this->circlingAngle += this->circlingAngularVelocity;
    targetPos.x = (Math_SinS(this->circlingAngle) * 200.0f) + player->actor.world.pos.x;
    targetPos.y = this->actor.world.pos.y;
    targetPos.z = (Math_CosS(this->circlingAngle) * 200.0f) + player->actor.world.pos.z;
    Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &targetPos), 0xA, 0xFA0, 0x14);
}

void EnJso_SetupGuard(EnJso* this) {
    EnJso_ChangeAnim(this, EN_JSO_ANIM_GUARD);
    this->action = EN_JSO_ACTION_GUARD;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    Actor_PlaySfx(&this->actor, NA_SE_IT_SHIELD_BOUND);
    this->bodyCollider.base.acFlags |= AC_HARD;
    this->actionFunc = EnJso_Guard;
}

/**
 * Plays the guard animation to completion, then goes back to circling the player.
 */
void EnJso_Guard(EnJso* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (curFrame >= this->animEndFrame) {
        this->isGuarding = false;

        if (this->isAttacking) {
            this->isAttacking = false;
            sIsAttacking = false;
        }

        EnJso_SetupCirclePlayer(this, play);
    }
}

void EnJso_SetupSpinBeforeAttack(EnJso* this) {
    EnJso_ChangeAnim(this, EN_JSO_ANIM_DASH_ATTACK);
    this->actor.world.rot.y = -this->actor.yawTowardsPlayer;
    this->actor.speed = 10.0f;
    this->actor.velocity.y = 20.0f;
    this->actionFunc = EnJso_SpinBeforeAttack;
}

/**
 * Leaps into the air while spinning forward. Once the Garo touches the ground, it will begin a dash attack.
 */
void EnJso_SpinBeforeAttack(EnJso* this, PlayState* play) {
    this->actor.world.rot.x += 0x1770;
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0xFA0, 0x14);

    if (this->actor.velocity.y > 0.0f) {
        return;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.world.rot.x = 0;
        this->actor.velocity.y = 0.0f;
        this->actor.speed = 0.0f;
        this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
        EnJso_SetupDashAttack(this);
    }
}

void EnJso_SetupDashAttack(EnJso* this) {
    this->action = EN_JSO_ACTION_DASH_ATTACK;
    this->attackMovementTimer = 40;
    this->bodyCollider.base.colMaterial = COL_MATERIAL_HIT2;
    this->bodyCollider.base.acFlags &= ~AC_HARD;
    this->actor.speed = 15.0f;
    this->actor.velocity.y = 13.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_ENTRY);
    Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_DASH_2);
    this->actionFunc = EnJso_DashAttack;
}

/**
 * Dashes toward the player with its swords out. If the player shields the attack, the Garo will be knocked back and
 * drop its swords. The Garo will stop dashing and perform a slash if it's close enough to the player, if the difference
 * between its y-rotation and the yaw towards the player is large enough (usually indicating that it has dashed past the
 * player), or if 40 frames have passed, whichever comes first.
 */
void EnJso_DashAttack(EnJso* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    s16 yawDiff;
    s16 absYawDiff;
    Vec3f knockbackVelocity;

    if ((this->rightSwordCollider.base.atFlags & AT_BOUNCED) || (this->leftSwordCollider.base.atFlags & AT_BOUNCED)) {
        this->rightSwordCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
        this->leftSwordCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
        if (this->swordState == EN_JSO_SWORD_STATE_BOTH_DRAWN) {
            Matrix_RotateYS(this->actor.yawTowardsPlayer, MTXMODE_NEW);
            knockbackVelocity.x = 0.0f;
            knockbackVelocity.y = 0.0f;
            knockbackVelocity.z = -10.0f;
            Matrix_MultVec3f(&knockbackVelocity, &this->knockbackVelocity);
            this->swordState = EN_JSO_SWORD_STATE_KNOCKED_OUT_OF_HANDS;
            this->attackMovementTimer = 0;
            this->disableBlure = true;
            AudioSfx_SetChannelIO(&this->actor.projectedPos, NA_SE_EN_ANSATSUSYA_DASH_2, 0);
            EnJso_SetupKnockedBack(this);
            return;
        }
    }

    if ((this->actor.velocity.y < 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->actor.velocity.y = 13.0f;
    }

    if (curFrame < this->animEndFrame) {
        return;
    }

    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    absYawDiff = ABS_ALT(yawDiff);

    if ((this->attackMovementTimer == 0) || (this->actor.xzDistToPlayer < 100.0f) || (absYawDiff > 0x4300)) {
        AudioSfx_SetChannelIO(&this->actor.projectedPos, NA_SE_EN_ANSATSUSYA_DASH_2, 0);
        Math_ApproachZeroF(&this->actor.speed, 0.3f, 3.0f);
        EnJso_SetupSlash(this, play);
    }
}

void EnJso_SetupSlash(EnJso* this, PlayState* play) {
    EnJso_ChangeAnim(this, EN_JSO_ANIM_SLASH_START);
    this->action = EN_JSO_ACTION_SLASH;
    Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1, 8.0f, 500,
                             10, true);
    Math_ApproachZeroF(&this->actor.speed, 0.3f, 3.0f);
    this->slashHitSomething = false;
    Actor_PlaySfx(&this->actor, NA_SE_IT_SWORD_SWING_HARD);
    this->actionFunc = EnJso_Slash;
}

/**
 * Slash in place with both swords. If the player shields the attack, the Garo will be knocked back and drop its swords.
 * Once the slash animation ends, this function will transition the Garo to a waiting state.
 */
void EnJso_Slash(EnJso* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    Math_ApproachZeroF(&this->actor.speed, 0.5f, 5.0f);

    if ((play->gameplayFrames % 8) == 0) {
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1, 8.0f,
                                 500, 10, true);
    }

    if ((this->rightSwordCollider.base.atFlags & AT_HIT) || (this->leftSwordCollider.base.atFlags & AT_HIT)) {
        this->slashHitSomething = true;
        this->rightSwordCollider.base.atFlags &= ~AT_HIT;
        this->leftSwordCollider.base.atFlags &= ~AT_HIT;
    }

    if ((this->rightSwordCollider.base.atFlags & AT_BOUNCED) || (this->leftSwordCollider.base.atFlags & AT_BOUNCED)) {
        this->rightSwordCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
        this->leftSwordCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);

        if (this->swordState == EN_JSO_SWORD_STATE_BOTH_DRAWN) {
            Vec3f knockbackVelocity;

            Matrix_RotateYS(this->actor.yawTowardsPlayer, MTXMODE_NEW);
            knockbackVelocity.x = 0.0f;
            knockbackVelocity.y = 0.0f;
            knockbackVelocity.z = -10.0f;
            Matrix_MultVec3f(&knockbackVelocity, &this->knockbackVelocity);
            this->swordState = EN_JSO_SWORD_STATE_KNOCKED_OUT_OF_HANDS;
            this->disableBlure = true;
            EnJso_SetupKnockedBack(this);
            return;
        }
    }

    if (curFrame >= this->animEndFrame) {
        this->actor.speed = 0.0f;
        this->disableBlure = true;
        EnJso_SetupWaitAfterSlash(this);
    }
}

void EnJso_SetupWaitAfterSlash(EnJso* this) {
    if (this->slashHitSomething) {
        EnJso_ChangeAnim(this, EN_JSO_ANIM_SLASH_LOOP);
        this->timer = 20;
    } else {
        EnJso_ChangeAnim(this, EN_JSO_ANIM_SLASH_LOOP);
        this->timer = 40;
    }

    this->action = EN_JSO_ACTION_WAIT_AFTER_SLASH;
    this->actionFunc = EnJso_WaitAfterSlash;
}

/**
 * Waits either 20 or 40 frames, depending on whether or not the Garo hit the player with its slash attack. Once this
 * waiting period ends, the Garo starts circling the player again.
 */
void EnJso_WaitAfterSlash(EnJso* this, PlayState* play) {
    if (this->timer == 0) {
        this->attackTimer = Rand_S16Offset(30, 30);
        this->bodyCollider.base.colMaterial = COL_MATERIAL_NONE;
        this->bodyCollider.base.acFlags |= AC_HARD;
        this->slashHitSomething = false;
        sIsAttacking = false;
        this->isAttacking = false;
        EnJso_SetupCirclePlayer(this, play);
    }
}

void EnJso_SetupKnockedBack(EnJso* this) {
    EnJso_ChangeAnim(this, EN_JSO_ANIM_KNOCKED_BACK);
    this->bodyCollider.base.acFlags &= ~AC_HARD;
    this->timer = 30;
    this->action = EN_JSO_ACTION_KNOCKED_BACK;
    this->actor.speed = 0.0f;
    this->actionFunc = EnJso_KnockedBack;
}

/**
 * Plays the knocked back animation to completion, then transitions to cowering on the ground.
 */
void EnJso_KnockedBack(EnJso* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (curFrame >= this->animEndFrame) {
        EnJso_SetupCower(this);
    }
}

void EnJso_SetupCower(EnJso* this) {
    EnJso_ChangeAnim(this, EN_JSO_ANIM_COWER);
    this->action = EN_JSO_ACTION_COWER;
    this->timer = 30;
    this->actionFunc = EnJso_Cower;
}

/**
 * Cower on the ground for 30 frames, then jump back.
 */
void EnJso_Cower(EnJso* this, PlayState* play) {
    if (this->timer == 0) {
        EnJso_SetupJumpBack(this);
    }
}

/**
 * Unlike most enemies, this will make the Garo play its damaged animation rather than stopping all animations.
 */
void EnJso_SetupStunned(EnJso* this) {
    Vec3f knockbackVelocity;

    AudioSfx_SetChannelIO(&this->actor.projectedPos, NA_SE_EN_ANSATSUSYA_DASH_2, 0);
    EnJso_ChangeAnim(this, EN_JSO_ANIM_DAMAGED);

    //! @note: This assignment is immediately overridden below. This is probably a leftover from EnJso2's version of
    //! this function, where the Garo Master is stunned for longer if it's frozen. Garos just use the longer stun
    //! unconditionally, resulting in this useless assignment here.
    this->timer = 30;
    this->actor.speed = 0.0f;

    if (((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
         (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) &&
        (this->drawDmgEffAlpha == 0)) {
        this->drawDmgEffAlpha = 0;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    }

    this->timer = 40;
    Matrix_RotateYS(this->actor.yawTowardsPlayer, MTXMODE_NEW);
    Matrix_MultVecZ(-10.0f, &knockbackVelocity);
    Math_Vec3f_Copy(&this->knockbackVelocity, &knockbackVelocity);
    this->action = EN_JSO_ACTION_STUNNED;
    this->bodyCollider.base.acFlags &= ~AC_HARD;
    this->actionFunc = EnJso_Stunned;
}

/**
 * Makes the Garo play its damaged animation and stop moving until 40 frames have passed, then it jumps back.
 */
void EnJso_Stunned(EnJso* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) {
        if ((this->drawDmgEffAlpha != 0) && (this->drawDmgEffAlpha < 60)) {
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
        }
    }

    if ((this->timer == 0) && (this->drawDmgEffAlpha == 0)) {
        if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
            (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) {
            Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, EN_JSO_BODYPART_MAX, 2, 0.3f, 0.2f);
            this->drawDmgEffAlpha = 0;
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        }

        EnJso_SetupJumpBack(this);
    }
}

void EnJso_SetupDamaged(EnJso* this, PlayState* play) {
    Vec3f knockbackVelocity;

    AudioSfx_SetChannelIO(&this->actor.projectedPos, NA_SE_EN_ANSATSUSYA_DASH_2, 0);
    EnJso_ChangeAnim(this, EN_JSO_ANIM_DAMAGED);
    this->slashHitSomething = false;
    this->actor.velocity.y = 10.0f;
    this->actor.speed = 0.0f;
    Matrix_RotateYS(this->actor.yawTowardsPlayer, MTXMODE_NEW);
    Matrix_MultVecZ(-20.0f, &knockbackVelocity);
    Math_Vec3f_Copy(&this->knockbackVelocity, &knockbackVelocity);

    if (((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
         (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) &&
        (this->drawDmgEffAlpha != 0)) {
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, EN_JSO_BODYPART_MAX, 2, 0.3f, 0.2f);
        this->drawDmgEffAlpha = 0;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    }

    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
    Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_DAMAGE);
    this->action = EN_JSO_ACTION_DAMAGED;
    this->actionFunc = EnJso_Damaged;
}

/**
 * Plays the damaged animation until the Garo is touching the ground and no longer has its red color filter, at which
 * point it will jump back.
 */
void EnJso_Damaged(EnJso* this, PlayState* play) {
    if (this->actor.velocity.y > 0.0f) {
        return;
    }

    if ((this->actor.colorFilterTimer == 0) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        EnJso_SetupJumpBack(this);
    }
}

void EnJso_SetupJumpBack(EnJso* this) {
    EnJso_ChangeAnim(this, EN_JSO_ANIM_JUMP_BACK);
    this->action = EN_JSO_ACTION_JUMP_BACK;
    this->timer = 30;
    this->actor.speed = 7.0f;
    this->actor.velocity.y = 20.0f;

    if (this->swordState != EN_JSO_SWORD_STATE_BOTH_DRAWN) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_SWORD);
        this->swordState = EN_JSO_SWORD_STATE_BOTH_DRAWN;
    }

    this->actor.world.rot.y += 0x8000;
    this->actionFunc = EnJso_JumpBack;
}

/**
 * Jump backwards away from the player. After the Garo touches the ground, the jump back animation completes, and 30
 * frames have passed, the Garo will transition to circling the player.
 */
void EnJso_JumpBack(EnJso* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0xBB8, 0x14);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = 0.0f;
    }

    if ((this->timer == 0) || ((curFrame >= this->animEndFrame) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND))) {
        this->actor.world.rot.x = 0;
        this->actor.velocity.y = 0.0f;
        this->actor.speed = 0.0f;
        this->timer = 0;
        sIsAttacking = false;
        this->isAttacking = false;
        this->actor.world.rot.y = this->actor.shape.rot.y;
        this->attackTimer = Rand_S16Offset(10, 10);
        EnJso_SetupCirclePlayer(this, play);
    }
}

void EnJso_SetupDead(EnJso* this, PlayState* play) {
    AudioSfx_SetChannelIO(&this->actor.projectedPos, NA_SE_EN_ANSATSUSYA_DASH_2, 0);
    EnJso_ChangeAnim(this, EN_JSO_ANIM_DAMAGED);

    if (((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
         (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) &&
        (this->drawDmgEffAlpha == 0)) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    }

    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.speed = 0.0f;
    this->disableBlure = true;
    this->timer = 30;
    Enemy_StartFinishingBlow(play, &this->actor);
    Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_DEAD);
    this->action = EN_JSO_ACTION_DEAD;
    this->actionFunc = EnJso_Dead;
}

/**
 * Plays the damaged animation for 30 frames, then the Garo falls down and starts talking to the player.
 */
void EnJso_Dead(EnJso* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
        (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) {
        if (this->drawDmgEffAlpha != 0) {
            Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, EN_JSO_BODYPART_MAX, 2, 0.3f, 0.2f);
            this->drawDmgEffAlpha = 0;
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        } else {
            return;
        }
    }

    if ((curFrame >= this->animEndFrame) && (this->timer == 0)) {
        EnJso_SetupFallDownAndTalk(this, play);
    }
}

void EnJso_SetupFallDownAndTalk(EnJso* this, PlayState* play) {
    EnJso_ChangeAnim(this, EN_JSO_ANIM_FALL_DOWN);
    this->textIndex = 2;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_NPC);
    this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.flags &= ~ACTOR_FLAG_FREEZE_EXCEPTION;
    this->action = EN_JSO_ACTION_FALL_DOWN_AND_TALK;
    this->actionFunc = EnJso_FallDownAndTalk;
}

/**
 * Handles the first part of the dialogue the Garo gives when defeated.
 */
void EnJso_FallDownAndTalk(EnJso* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    Player* player = GET_PLAYER(play);

    if (curFrame < this->animEndFrame) {
        return;
    }

    if (!Play_InCsMode(play) && (play->msgCtx.msgLength == 0)) {
        if (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 60.0f) {
            Player* player2 = GET_PLAYER(play);

            this->actor.textId = sTextIds[this->textIndex];
            Message_StartTextbox(play, this->actor.textId, &this->actor);
            player2->stateFlags1 |= PLAYER_STATE1_10000000;
            player2->actor.freezeTimer = 3;
            this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
            this->actionFunc = EnJso_TellHint;
        } else {
            EnJso_SetupReappear(this, play);
        }
    }
}

/**
 * Handles the second part of the dialogue the Garo gives when defeated involving its specific hint. If the player has
 * the Elegy of Emptiness, then the Garo ignore its specific hint and will instead always tell them that the Elegy can
 * be used to climb Stone Tower. Once the Garo has given its hint, this function will have it say one final piece of
 * dialogue and prepare for it to burst into flames.
 */
void EnJso_TellHint(EnJso* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->actor.freezeTimer = 3;

    // Checking for Dawn of the Second Day, Night of the First Day, etc. messages
    if ((play->msgCtx.currentTextId >= 0x1BB2) && (play->msgCtx.currentTextId < 0x1BB7)) {
        this->actor.textId = sTextIds[this->textIndex];
        Message_StartTextbox(play, this->actor.textId, &this->actor);
        return;
    }

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        if (this->textIndex == 2) {
            u16 textId = 0x139C; // Hint about using Elegy to climb Stone Tower

            switch (this->hintType) {
                case EN_JSO_HINT_FREEZE_OCTOROKS:
                    textId = 0x139A; // Hint about freezing the Octoroks
                    break;

                case EN_JSO_HINT_FLATS_LOCATION:
                    textId = 0x139B; // Hint about Flat being in the Graveyard
                    break;

                case EN_JSO_HINT_VARIABLE_1:
                    textId = 0x139D; // Hint about Pamela's family being in the house with the waterwheel
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_14_04)) {
                        textId = 0x13A1; // Hint about using the Song of Healing on Pamela's father
                    }
                    break;

                case EN_JSO_HINT_VARIABLE_2:
                    textId = 0x139E; // Hint about Pamela leaving her house to check something
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_14_04)) {
                        textId = 0x13A2; // Hint about the well being connected to Ikana Castle
                    }
                    break;

                case EN_JSO_HINT_VARIABLE_3:
                    textId = 0x139F; // Hint about restoring the river by going into Sharp's cave
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_14_04)) {
                        textId = 0x13A3; // Hint about the Gibdos in the well wanting items
                    }
                    break;

                case EN_JSO_HINT_VARIABLE_4:
                    textId = 0x13A0; // Hint about needing the Song of Storms to pacify Sharp
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_14_04)) {
                        textId = 0x13A4; // Hint about the items the Gibdos want being in the well
                    }
                    break;

                case EN_JSO_HINT_DANCING_REDEADS:
                    textId = 0x13A5; // Hint about the Redeads being able to dance
                    break;

                case EN_JSO_HINT_DESTRUCTIBLE_CEILING_IN_CASTLE:
                    textId = 0x13A6; // Hint about the destructible ceiling in Ikana Castle
                    break;

                case EN_JSO_HINT_SECOND_ENTRANCE_TO_CASTLE:
                    textId = 0x13A7; // Hint about the second entrance to Ikana Castle
                    break;

                case EN_JSO_HINT_KING_WEAK_TO_MIRROR_SHIELD:
                    textId = 0x13A8; // Hint about using the Mirror Shield on Igos du Ikana
                    break;

                default:
                    break;
            }

            if (CHECK_QUEST_ITEM(QUEST_SONG_ELEGY)) {
                textId = 0x139C; // Hint about using Elegy to climb Stone Tower
            }

            Message_ContinueTextbox(play, textId);
            this->textIndex++;
        } else if (this->textIndex == 3) {
            Message_ContinueTextbox(play, sTextIds[this->textIndex]);
            this->actionFunc = EnJso_BurstIntoFlames;
        }
    }
}

/**
 * Once the player closes the Garo's final textbox, this function will spawn nine green flames to obscure the Garo and
 * kill the actor.
 */
void EnJso_BurstIntoFlames(EnJso* this, PlayState* play) {
    Vec3f firePos;
    Vec3f fireVelocityAndAccel[] = {
        { 1.0f, 0.0f, 0.5f },   { 1.0f, 0.0f, -0.5f },  { -1.0f, 0.0f, 0.5f },
        { -1.0f, 0.0f, -0.5f }, { 0.5f, 0.0f, 1.0f },   { -0.5f, 0.0f, 1.0f },
        { 0.5f, 0.0f, -1.0f },  { -0.5f, 0.0f, -1.0f }, { 0.0f, 0.0f, 0.0f },
    };
    Player* player = GET_PLAYER(play);
    f32 scale;
    f32 scaleStep;
    s32 i;

    player->actor.freezeTimer = 3;
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Player* player2 = GET_PLAYER(play);

        Message_CloseTextbox(play);
        for (i = 0; i < ARRAY_COUNT(fireVelocityAndAccel); i++) {
            Math_Vec3f_Copy(&firePos, &this->actor.world.pos);
            firePos.x += Rand_CenteredFloat(30.0f);
            firePos.y = this->actor.floorHeight;
            firePos.z += Rand_CenteredFloat(30.0f);
            scale = (Rand_ZeroFloat(1.0f) * 100.0f) + 100.0f;
            scaleStep = 20.0f;
            func_800B3030(play, &firePos, &fireVelocityAndAccel[i], &fireVelocityAndAccel[i], scale, scaleStep, 1);
        }

        SoundSource_PlaySfxEachFrameAtFixedWorldPos(play, &this->actor.world.pos, 10,
                                                    NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG);

        sIsAttacking = false;
        this->isAttacking = false;
        if (this->isPlayerLockedOn) {
            this->isPlayerLockedOn = false;
            sIsPlayerLockedOn = false;
        }

        player2->stateFlags1 &= ~PLAYER_STATE1_10000000;
        Actor_Kill(&this->actor);
    }
}

void EnJso_UpdateDamage(EnJso* this, PlayState* play) {
    s32 attackDealsDamage = false;

    if (this->bodyCollider.base.acFlags & AC_HIT) {
        this->bodyCollider.base.acFlags &= ~AC_HIT;
        if ((this->actor.colChkInfo.damageEffect == EN_JSO_DMGEFF_STUN) ||
            (this->actor.colChkInfo.damageEffect == EN_JSO_DMGEFF_ELECTRIC_STUN)) {
            if (((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
                 (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) ||
                (this->drawDmgEffAlpha == 0)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
                if (this->actor.colChkInfo.damageEffect == EN_JSO_DMGEFF_ELECTRIC_STUN) {
                    this->drawDmgEffAlpha = 40;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL;
                }

                EnJso_SetupStunned(this);
            }
        } else {
            switch (this->action) {
                case EN_JSO_ACTION_CIRCLE_PLAYER:
                case EN_JSO_ACTION_GUARD:
                    this->actor.speed = 0.0f;
                    EnJso_SetupGuard(this);
                    attackDealsDamage = false;
                    break;

                case EN_JSO_ACTION_DASH_ATTACK:
                case EN_JSO_ACTION_SLASH:
                case EN_JSO_ACTION_WAIT_AFTER_SLASH:
                case EN_JSO_ACTION_STUNNED:
                case EN_JSO_ACTION_COWER:
                    switch (this->actor.colChkInfo.damageEffect) {
                        case EN_JSO_DMGEFF_NONE:
                            attackDealsDamage = true;
                            break;

                        case EN_JSO_DMGEFF_FIRE:
                            this->drawDmgEffAlpha = 40;
                            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                            attackDealsDamage = true;
                            break;

                        case EN_JSO_DMGEFF_LIGHT_ORB:
                            if (((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
                                 (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) ||
                                (this->drawDmgEffAlpha == 0)) {
                                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.focus.pos.x,
                                            this->actor.focus.pos.y, this->actor.focus.pos.z, 0, 0, 0,
                                            CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                                this->drawDmgEffAlpha = 20;
                                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                                attackDealsDamage = true;
                            }
                            break;

                        case EN_JSO_DMGEFF_FREEZE:
                            if (((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
                                 (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) ||
                                (this->drawDmgEffAlpha == 0)) {
                                attackDealsDamage = false;
                                Actor_ApplyDamage(&this->actor);
                                this->drawDmgEffAlpha = 80;
                                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_SFX;
                                this->drawDmgEffScale = 0.0f;
                                this->drawDmgEffFrozenSteamScale = 1.5f;
                            }

                            if (this->actor.colChkInfo.health <= 0) {
                                EnJso_SetupDead(this, play);
                                attackDealsDamage = false;
                            } else {
                                EnJso_SetupStunned(this);
                                attackDealsDamage = false;
                            }
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }

            if (attackDealsDamage) {
                Actor_ApplyDamage(&this->actor);
                this->bodyCollider.base.colMaterial = COL_MATERIAL_NONE;
                this->bodyCollider.base.acFlags |= AC_HARD;
                if (this->actor.colChkInfo.health > 0) {
                    EnJso_SetupDamaged(this, play);
                } else {
                    EnJso_SetupDead(this, play);
                }
            }
        }
    }
}

void EnJso_Update(Actor* thisx, PlayState* play) {
    EnJso* this = (EnJso*)thisx;
    s32 pad;

    if ((this->action != EN_JSO_ACTION_CIRCLE_PLAYER) && !this->disableAnimations) {
        SkelAnime_Update(&this->skelAnime);
    }

    EnJso_UpdateDamage(this, play);
    if (this->actor.isLockedOn) {
        sIsPlayerLockedOn = true;
        this->isPlayerLockedOn = true;
    } else if (this->isPlayerLockedOn) {
        this->isPlayerLockedOn = false;
        sIsPlayerLockedOn = false;
    }

    this->actionFunc(this, play);
    DECR(this->attackMovementTimer);
    DECR(this->attackTimer);
    DECR(this->timer);
    DECR(this->drawDmgEffAlpha);

    if (this->action != EN_JSO_ACTION_FALL_DOWN_AND_TALK) {
        Actor_SetFocus(&this->actor, 50.0f);
    } else {
        Actor_SetFocus(&this->actor, 30.0f);
    }

    Actor_SetScale(&this->actor, this->scale);
    Actor_MoveWithGravity(&this->actor);
    this->actor.world.pos.x += this->knockbackVelocity.x;
    this->actor.world.pos.z += this->knockbackVelocity.z;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Math_ApproachZeroF(&this->knockbackVelocity.x, 1.0f, 2.0f);
        Math_ApproachZeroF(&this->knockbackVelocity.z, 1.0f, 2.0f);
    }

    if (this->action != EN_JSO_ACTION_REAPPEAR) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 35.0f, 40.0f, 40.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                    UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
    }

    if ((this->action == EN_JSO_ACTION_SPIN_BEFORE_ATTACK) || (this->action == EN_JSO_ACTION_DASH_ATTACK)) {
        this->afterimageIndex++;
        if (this->afterimageIndex >= EN_JSO_AFTERIMAGE_COUNT) {
            this->afterimageIndex = 0;
        }

        if (this->afterimageCount < EN_JSO_AFTERIMAGE_COUNT - 1) {
            this->afterimageCount++;
        }

        this->afterimagePos[this->afterimageIndex] = this->actor.world.pos;
        this->afterimagePos[this->afterimageIndex].y += 40.0f;
        this->afterimageRot[this->afterimageIndex] = this->actor.world.rot;

        {
            s32 i;

            for (i = 0; i < EN_JSO_AFTERIMAGE_COUNT; i++) {
                this->afterimageJointTable[this->afterimageIndex][i] = this->jointTable[i];
            }
        }
    } else {
        this->afterimageCount = 0;
    }

    if ((this->action != EN_JSO_ACTION_INTRO_CUTSCENE) && (this->action != EN_JSO_ACTION_CIRCLE_PLAYER) &&
        (this->action != EN_JSO_ACTION_SPIN_BEFORE_ATTACK) && (this->action != EN_JSO_ACTION_DAMAGED) &&
        (this->action != EN_JSO_ACTION_JUMP_BACK)) {
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    this->actor.shape.rot.x = this->actor.world.rot.x;
    Collider_UpdateCylinder(&this->actor, &this->bodyCollider);

    if ((this->action != EN_JSO_ACTION_INTRO_CUTSCENE) && (this->action != EN_JSO_ACTION_REAPPEAR) &&
        (this->action != EN_JSO_ACTION_DAMAGED) && (this->action != EN_JSO_ACTION_SPIN_BEFORE_ATTACK) &&
        (this->action != EN_JSO_ACTION_DEAD) && (this->action != EN_JSO_ACTION_FALL_DOWN_AND_TALK) &&
        (this->action != EN_JSO_ACTION_UNK_15)) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyCollider.base);

        if ((this->action == EN_JSO_ACTION_SLASH) && !this->slashHitSomething &&
            (this->swordState == EN_JSO_SWORD_STATE_BOTH_DRAWN)) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->rightSwordCollider.base);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->leftSwordCollider.base);
        }
    }
}

s32 EnJso_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnJso* this = (EnJso*)thisx;

    if (limbIndex == GARO_LIMB_RIGHT_ARM) {
        rot->x += this->rightArmRot.x;
        rot->y += this->rightArmRot.y;
        rot->z += this->rightArmRot.z;
    }

    if (limbIndex == GARO_LIMB_ROBE_RIGHT) {
        rot->x += this->robeRightRot.x;
        rot->y += this->robeRightRot.y;
        rot->z += this->robeRightRot.z;
    }

    if ((limbIndex == GARO_LIMB_LEFT_SWORD) || (limbIndex == GARO_LIMB_RIGHT_SWORD)) {
        if (this->swordState == EN_JSO_SWORD_STATE_NONE_DRAWN) {
            *dList = NULL;
        } else if (limbIndex == GARO_LIMB_LEFT_SWORD) {
            if (this->swordState == EN_JSO_SWORD_STATE_RIGHT_DRAWN) {
                *dList = NULL;
            }
        } else if ((limbIndex == GARO_LIMB_RIGHT_SWORD) && (this->swordState == EN_JSO_SWORD_STATE_LEFT_DRAWN)) {
            *dList = NULL;
        }
    }

    return false;
}

void EnJso_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f sSwordTipOffset = { 1600.0f, 0.0f, 0.0f };
    static Vec3f sSwordBaseOffset = { 0.0f, 0.0f, 0.0f };
    static Vec3f sSwordTipQuadOffset = { 1700.0f, 0.0f, 0.0f };
    static Vec3f sSwordBaseQuadOffset = { 0.0f, 0.0f, 0.0f };
    EnJso* this = (EnJso*)thisx;
    Vec3f swordTipPos;
    Vec3f swordBasePos;

    Matrix_Push();

    if (limbIndex == GARO_LIMB_LEFT_SWORD) {
        if (this->swordState == EN_JSO_SWORD_STATE_KNOCKED_OUT_OF_HANDS) {
            Actor_SpawnBodyParts(&this->actor, play, ENPART_PARAMS(ENPART_TYPE_15), dList);
        }

        Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Math_Vec3f_Copy(&this->leftSwordCollider.dim.quad[3], &this->leftSwordCollider.dim.quad[1]);
        Math_Vec3f_Copy(&this->leftSwordCollider.dim.quad[2], &this->leftSwordCollider.dim.quad[0]);
        Matrix_MultVec3f(&sSwordTipQuadOffset, &this->leftSwordCollider.dim.quad[1]);
        Matrix_MultVec3f(&sSwordBaseQuadOffset, &this->leftSwordCollider.dim.quad[0]);
        Collider_SetQuadVertices(&this->leftSwordCollider, &this->leftSwordCollider.dim.quad[0],
                                 &this->leftSwordCollider.dim.quad[1], &this->leftSwordCollider.dim.quad[2],
                                 &this->leftSwordCollider.dim.quad[3]);
        Matrix_MultVec3f(&sSwordTipOffset, &swordTipPos);
        Matrix_MultVec3f(&sSwordBaseOffset, &swordBasePos);

        if (((this->action == EN_JSO_ACTION_SLASH) || (this->action == EN_JSO_ACTION_DASH_ATTACK)) &&
            !this->disableBlure) {
            EffectBlure_AddVertex(Effect_GetByIndex(this->leftSwordBlureIndex), &swordTipPos, &swordBasePos);
        } else if (this->disableBlure == true) {
            EffectBlure_AddSpace(Effect_GetByIndex(this->leftSwordBlureIndex));
        }
    }

    if (limbIndex == GARO_LIMB_RIGHT_SWORD) {
        if (this->swordState == EN_JSO_SWORD_STATE_KNOCKED_OUT_OF_HANDS) {
            Actor_SpawnBodyParts(&this->actor, play, ENPART_PARAMS(ENPART_TYPE_15), dList);
            this->swordState = EN_JSO_SWORD_STATE_NONE_DRAWN;
        }

        Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Math_Vec3f_Copy(&this->rightSwordCollider.dim.quad[3], &this->rightSwordCollider.dim.quad[1]);
        Math_Vec3f_Copy(&this->rightSwordCollider.dim.quad[2], &this->rightSwordCollider.dim.quad[0]);
        Matrix_MultVec3f(&sSwordTipQuadOffset, &this->rightSwordCollider.dim.quad[1]);
        Matrix_MultVec3f(&sSwordBaseQuadOffset, &this->rightSwordCollider.dim.quad[0]);
        Collider_SetQuadVertices(&this->rightSwordCollider, &this->rightSwordCollider.dim.quad[0],
                                 &this->rightSwordCollider.dim.quad[1], &this->rightSwordCollider.dim.quad[2],
                                 &this->rightSwordCollider.dim.quad[3]);
        Matrix_MultVec3f(&sSwordTipOffset, &swordTipPos);
        Matrix_MultVec3f(&sSwordBaseOffset, &swordBasePos);

        if (((this->action == EN_JSO_ACTION_SLASH) || (this->action == EN_JSO_ACTION_DASH_ATTACK)) &&
            !this->disableBlure) {
            EffectBlure_AddVertex(Effect_GetByIndex(this->rightSwordBlureIndex), &swordTipPos, &swordBasePos);
        } else if (this->disableBlure == true) {
            EffectBlure_AddSpace(Effect_GetByIndex(this->rightSwordBlureIndex));
            this->disableBlure = false;
        }
    }

    if ((limbIndex == GARO_LIMB_LEFT_SWORD) || (limbIndex == GARO_LIMB_RIGHT_SWORD) ||
        (limbIndex == GARO_LIMB_ROBE_TOP) || (limbIndex == GARO_LIMB_ROBE_BACK) || (limbIndex == GARO_LIMB_ROBE_LEFT) ||
        (limbIndex == GARO_LIMB_ROBE_RIGHT) || (limbIndex == GARO_LIMB_ROBE_FRONT) || (limbIndex == GARO_LIMB_HEAD) ||
        (limbIndex == GARO_LIMB_RIGHT_THIGH) || (limbIndex == GARO_LIMB_RIGHT_FOOT) ||
        (limbIndex == GARO_LIMB_LEFT_THIGH) || (limbIndex == GARO_LIMB_LEFT_FOOT)) {
        Matrix_MultZero(&this->bodyPartsPos[this->bodyPartIndex]);
        this->bodyPartIndex++;
        if (this->bodyPartIndex >= EN_JSO_BODYPART_MAX) {
            this->bodyPartIndex = 0;
        }
    }

    Matrix_Pop();
}

Gfx* EnJso_SetAfterimageRenderMode(GraphicsContext* gfxCtx) {
    Gfx* gfxHead = GRAPH_ALLOC(gfxCtx, 2 * sizeof(Gfx));
    Gfx* gfx = gfxHead;

    gDPSetRenderMode(
        gfx++, AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL | G_RM_FOG_SHADE_A,
        AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
            GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA));
    gSPEndDisplayList(gfx++);

    return gfxHead;
}

void EnJso_Draw(Actor* thisx, PlayState* play) {
    static s16 sAfterimageAlpha[EN_JSO_AFTERIMAGE_COUNT] = {
        128, 0, 0, 0, 0, 128, 0, 0, 0, 0, 128, 0, 0, 0, 0, 128, 0, 0, 0, 0,
    };
    EnJso* this = (EnJso*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x0C, D_801AEFA0);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnJso_OverrideLimbDraw, EnJso_PostLimbDraw, &this->actor);

    if (this->afterimageCount > 0) {
        s32 i;
        s32 index = this->afterimageIndex;

        for (i = 0; i < this->afterimageCount; i++) {
            if (sAfterimageAlpha[i] == 0) {
                continue;
            }

            Matrix_Translate(this->afterimagePos[index].x, this->afterimagePos[index].y, this->afterimagePos[index].z,
                             MTXMODE_NEW);
            Matrix_Scale(this->scale, this->scale, this->scale, MTXMODE_APPLY);
            //! @bug: These matrix rotation functions are supposed to be passed binangs, so using BINANG_TO_RAD here
            //! is incorrect. Specifically, it results in rotation values far smaller than expected, since
            //! BINANG_TO_RAD divides the angle by 0x8000. In-game, this bug manifests as the Garo's afterimages
            //! looking the "wrong way", as they have a rotation close to (0, 0, 0) instead of matching the Garo's
            //! rotation as intended. This bug can be fixed by using the correct matrix rotation functions (e.g.,
            //! Matrix_RotateYF instead of Matrix_RotateYS) or by removing BINANG_TO_RAD entirely.
            Matrix_RotateYS(BINANG_TO_RAD(this->afterimageRot[index].y), MTXMODE_APPLY);
            Matrix_RotateXS(BINANG_TO_RAD(this->afterimageRot[index].x), MTXMODE_APPLY);
            Matrix_RotateZS(BINANG_TO_RAD(this->afterimageRot[index].z), MTXMODE_APPLY);

            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, sAfterimageAlpha[i]);
            gSPSegment(POLY_XLU_DISP++, 0x0C, EnJso_SetAfterimageRenderMode(play->state.gfxCtx));
            POLY_XLU_DISP = SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->afterimageJointTable[index],
                                               this->skelAnime.dListCount, NULL, NULL, &this->actor, POLY_XLU_DISP);

            index--;
            if (index < 0) {
                index = EN_JSO_AFTERIMAGE_COUNT - 1;
            }
        }
    }

    if (this->drawDmgEffAlpha != 0) {
        f32 drawDmgEffAlpha = this->drawDmgEffAlpha * 0.05f;

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

        Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, EN_JSO_BODYPART_MAX, this->drawDmgEffScale,
                                this->drawDmgEffFrozenSteamScale, drawDmgEffAlpha, this->drawDmgEffType);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
