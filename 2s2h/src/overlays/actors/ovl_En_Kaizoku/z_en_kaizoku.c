/*
 * File: z_en_kaizoku.c
 * Overlay: ovl_En_Kaizoku
 * Description: Fighter pirate, the miniboss guarding the eggs inside pirates fortress
 */

#include "z_en_kaizoku.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "overlays/effects/ovl_Effect_Ss_Hitmark/z_eff_ss_hitmark.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_FREEZE_EXCEPTION)

void EnKaizoku_Init(Actor* thisx, PlayState* play);
void EnKaizoku_Destroy(Actor* thisx, PlayState* play);
void EnKaizoku_Update(Actor* thisx, PlayState* play2);
void EnKaizoku_Draw(Actor* thisx, PlayState* play);

s32 EnKaizoku_ValidatePictograph(PlayState* play, Actor* actor);
void EnKaizoku_SetupWaitForApproach(EnKaizoku* this);
void EnKaizoku_WaitForApproach(EnKaizoku* this, PlayState* play);
void EnKaizoku_PlayerLoss(EnKaizoku* this, PlayState* play);
void EnKaizoku_SetupPlayerWinCutscene(EnKaizoku* this);
void EnKaizoku_PlayerWinCutscene(EnKaizoku* this, PlayState* play);
void EnKaizoku_SetupReady(EnKaizoku* this);
void EnKaizoku_Ready(EnKaizoku* this, PlayState* play);
void EnKaizoku_SetupSpinDodge(EnKaizoku* this, PlayState* play);
void EnKaizoku_SpinDodge(EnKaizoku* this, PlayState* play);
void EnKaizoku_SetupBlock(EnKaizoku* this);
void EnKaizoku_Block(EnKaizoku* this, PlayState* play);
void EnKaizoku_SetupJump(EnKaizoku* this);
void EnKaizoku_SpawnVerticalFootDust(PlayState* play, Vec3f* pos);
void EnKaizoku_Jump(EnKaizoku* this, PlayState* play);
void EnKaizoku_SetupRollBack(EnKaizoku* this);
void EnKaizoku_RollBack(EnKaizoku* this, PlayState* play);
void EnKaizoku_SetupSlash(EnKaizoku* this);
void EnKaizoku_Slash(EnKaizoku* this, PlayState* play2);
void EnKaizoku_SetupRollForward(EnKaizoku* this);
void EnKaizoku_RollForward(EnKaizoku* this, PlayState* play);
void EnKaizoku_SetupAdvance(EnKaizoku* this);
void EnKaizoku_Advance(EnKaizoku* this, PlayState* play);
void EnKaizoku_SetupPivot(EnKaizoku* this);
void EnKaizoku_Pivot(EnKaizoku* this, PlayState* play);
void EnKaizoku_SetupSpinAttack(EnKaizoku* this);
void EnKaizoku_SpinAttack(EnKaizoku* this, PlayState* play);
void EnKaizoku_SetupCircle(EnKaizoku* this);
void EnKaizoku_Circle(EnKaizoku* this, PlayState* play);
void EnKaizoku_Stunned(EnKaizoku* this, PlayState* play);
void EnKaizoku_Damaged(EnKaizoku* this, PlayState* play);
void EnKaizoku_DefeatKnockdown(EnKaizoku* this, PlayState* play);

typedef enum KaizokuAction {
    /*  0 */ KAIZOKU_ACTION_HIDDEN,
    /*  1 */ KAIZOKU_ACTION_READY,
    /*  2 */ KAIZOKU_ACTION_SPIN_DODGE,
    /*  3 */ KAIZOKU_ACTION_CIRCLE,
    /*  4 */ KAIZOKU_ACTION_ADVANCE,
    /*  5 */ KAIZOKU_ACTION_ROLL_FORWARD,
    /*  6 */ KAIZOKU_ACTION_JUMP,
    /*  7 */ KAIZOKU_ACTION_ROLL_BACK,
    /*  8 */ KAIZOKU_ACTION_UNUSED_8,
    /*  9 */ KAIZOKU_ACTION_SLASH,
    /* 10 */ KAIZOKU_ACTION_PIVOT,
    /* 11 */ KAIZOKU_ACTION_SPIN_ATTACK,
    /* 12 */ KAIZOKU_ACTION_BLOCK,
    /* 13 */ KAIZOKU_ACTION_STUNNED,
    /* 14 */ KAIZOKU_ACTION_DAMAGED,
    /* 15 */ KAIZOKU_ACTION_KNOCK_DOWN,
    /* 16 */ KAIZOKU_ACTION_SCENE_FADE
} KaizokuAction;

// text Ids, grouped into two batches of 4, and two mask values
static u16 sKaizokuTextIds[] = {
    0x11A4, // (shout) halt
    0x11A5, // (intro) you must have courage, going to love doing this to you
    0x11A6, // (win) ouch, dont think this is the end
    0x11A7, // (loss) you're nothing to talk about
    0x11A8, // (shout) go no further
    0x11A9, // (intro) you wont get past here
    0x11AA, // (win) not bad, but this isnt the end
    0x11AB, // (loss) hmph, we're not fools
    0x11AC, // (stone mask) we aren't fooled by that mask
    0x11AD, // (stone mask) did you think that mask would work?
};

static Vec3f sCutsceneCameraPositions[] = {
    { -70.0f, 9.0f, 30.0f },   { -70.0f, 9.0f, 30.0f },   { -70.0f, 9.0f, 30.0f },   { 41.0f, 55.0f, 11.0f },
    { 41.0f, 55.0f, 11.0f },   { 70.0f, 45.0f, 35.0f },   { 80.0f, 45.0f, 25.0f },   { 0.0f, 0.0f, 0.0f },
    { -70.0f, 9.0f, 30.0f },   { -70.0f, 9.0f, 30.0f },   { -70.0f, 9.0f, 30.0f },   { 41.0f, 55.0f, 11.0f },
    { 41.0f, 55.0f, 11.0f },   { 70.0f, 45.0f, 25.0f },   { 80.0f, 45.0f, 25.0f },   { 0.0f, 0.0f, 0.0f },
    { 90.0f, 9.0f, -30.0f },   { 90.0f, 9.0f, -30.0f },   { 90.0f, 9.0f, -30.0f },   { -41.0f, 55.0f, -11.0f },
    { -41.0f, 55.0f, -11.0f }, { -70.0f, 45.0f, -25.0f }, { -80.0f, 45.0f, -25.0f }, { 0.0f, 0.0f, 0.0f },
};

Vec3f sCutsceneCameraTargetPositions[] = {
    { 120.0f, 60.0f, -20.0f },   { 120.0f, 60.0f, -20.0f },   { 120.0f, 60.0f, -20.0f },   { -470.0f, 30.0f, -140.0f },
    { -470.0f, 30.0f, -140.0f }, { -410.0f, 80.0f, -130.0f }, { -410.0f, 80.0f, -130.0f }, { 0.0f, 0.0f, 0.0f },
    { 120.0f, 60.0f, -20.0f },   { 120.0f, 60.0f, -20.0f },   { 120.0f, 60.0f, -20.0f },   { -470.0f, 30.0f, -140.0f },
    { -470.0f, 30.0f, -140.0f }, { -410.0f, 80.0f, -130.0f }, { -410.0f, 80.0f, -130.0f }, { 0.0f, 0.0f, 0.0f },
    { -120.0f, 60.0f, 20.0f },   { -120.0f, 60.0f, 20.0f },   { -120.0f, 60.0f, 20.0f },   { 470.0f, 30.0f, 140.0f },
    { 470.0f, 30.0f, 140.0f },   { 410.0f, 80.0f, 130.0f },   { 410.0f, 80.0f, 130.0f },   { 0.0f, 0.0f, 0.0f },
};

typedef enum KaizokuLipstickColors {
    /* 0x0 */ KAIZOKU_LIPSTICK_YELLOW,
    /* 0x1 */ KAIZOKU_LIPSTICK_PURPLE,
    /* 0x2 */ KAIZOKU_LIPSTICK_SEAGREEN, // (between aqua and green)
} KaizokuLipstickColors;

Color_RGBA8 sKaizokuLipstickColors[] = {
    { 255, 255, 90, 255 }, // KAIZOKU_LIPSTICK_YELLOW
    { 55, 25, 80, 255 },   // KAIZOKU_LIPSTICK_PURPLE
    { 5, 195, 110, 255 },  // KAIZOKU_LIPSTICK_SEAGREEN
};

typedef enum KaizokuOutfitColors {
    /* 0x0 */ KAIZOKU_OUTFIT_ORANGE,
    /* 0x1 */ KAIZOKU_OUTFIT_LAVENDER,
    /* 0x2 */ KAIZOKU_OUTFIT_GREEN,
} KaizokuOutfitColors;

static Color_RGBA8 sKaizokuOutfitColors[] = {
    { 255, 130, 10, 255 },  // KAIZOKU_OUTFIT_ORANGE
    { 185, 130, 210, 255 }, // KAIZOKU_OUTFIT_LAVENDER
    { 135, 195, 80, 255 },  // KAIZOKU_OUTFIT_GREEN
};

typedef enum KaizokuDamageEffect {
    /* 0x0 */ KAIZOKU_DMGEFF_NONE,
    /* 0x1 */ KAIZOKU_DMGEFF_STUN,
    /* 0x2 */ KAIZOKU_DMGEFF_FIRE_ARROW,
    /* 0x3 */ KAIZOKU_DMGEFF_FREEZE,
    /* 0x4 */ KAIZOKU_DMGEFF_LIGHT_ARROW,
    /* 0x5 */ KAIZOKU_DMGEFF_ZORA_SHIELD,
    /* 0xD */ KAIZOKU_DMGEFF_STUNNED_ONLY = 0xD, // deals damage only if the pirate is stunned; she jumps away otherwise
    /* 0xE */ KAIZOKU_DMGEFF_ALWAYS_HIT,
    /* 0xF */ KAIZOKU_DMGEFF_IFRAME_PROTECTED // can only hit while kaizoku has no iframe from rolling
} KaizokuDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, KAIZOKU_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, KAIZOKU_DMGEFF_IFRAME_PROTECTED),
    /* Horse trample  */ DMG_ENTRY(0, KAIZOKU_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, KAIZOKU_DMGEFF_IFRAME_PROTECTED),
    /* Zora boomerang */ DMG_ENTRY(0, KAIZOKU_DMGEFF_STUN),
    /* Normal arrow   */ DMG_ENTRY(1, KAIZOKU_DMGEFF_IFRAME_PROTECTED),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, KAIZOKU_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(0, KAIZOKU_DMGEFF_STUN),
    /* Goron punch    */ DMG_ENTRY(1, KAIZOKU_DMGEFF_ALWAYS_HIT),
    /* Sword          */ DMG_ENTRY(1, KAIZOKU_DMGEFF_ALWAYS_HIT),
    /* Goron pound    */ DMG_ENTRY(0, KAIZOKU_DMGEFF_STUNNED_ONLY),
    /* Fire arrow     */ DMG_ENTRY(2, KAIZOKU_DMGEFF_FIRE_ARROW),
    /* Ice arrow      */ DMG_ENTRY(2, KAIZOKU_DMGEFF_FREEZE),
    /* Light arrow    */ DMG_ENTRY(2, KAIZOKU_DMGEFF_LIGHT_ARROW),
    /* Goron spikes   */ DMG_ENTRY(1, KAIZOKU_DMGEFF_ALWAYS_HIT),
    /* Deku spin      */ DMG_ENTRY(0, KAIZOKU_DMGEFF_STUN),
    /* Deku bubble    */ DMG_ENTRY(1, KAIZOKU_DMGEFF_ALWAYS_HIT),
    /* Deku launch    */ DMG_ENTRY(2, KAIZOKU_DMGEFF_ALWAYS_HIT),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, KAIZOKU_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(0, KAIZOKU_DMGEFF_ZORA_SHIELD),
    /* Normal shield  */ DMG_ENTRY(0, KAIZOKU_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, KAIZOKU_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, KAIZOKU_DMGEFF_ALWAYS_HIT),
    /* Zora punch     */ DMG_ENTRY(1, KAIZOKU_DMGEFF_ALWAYS_HIT),
    /* Spin attack    */ DMG_ENTRY(1, KAIZOKU_DMGEFF_STUNNED_ONLY),
    /* Sword beam     */ DMG_ENTRY(0, KAIZOKU_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, KAIZOKU_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, KAIZOKU_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, KAIZOKU_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, KAIZOKU_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, KAIZOKU_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, KAIZOKU_DMGEFF_ALWAYS_HIT),
};

ActorProfile En_Kaizoku_Profile = {
    /**/ ACTOR_EN_KAIZOKU,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_KZ,
    /**/ sizeof(EnKaizoku),
    /**/ EnKaizoku_Init,
    /**/ EnKaizoku_Destroy,
    /**/ EnKaizoku_Update,
    /**/ NULL,
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
        ELEM_MATERIAL_UNK4,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 50, 0, { 0, 0, 0 } },
};

static ColliderQuadInit sQuadInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK4,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL | ATELEM_UNK7,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static AnimationHeader* sAnimations[KAIZOKU_ANIM_MAX] = {
    &gFighterPirateFightingIdleAnim,            // KAIZOKU_ANIM_FIGHTING_IDLE
    &gFighterPirateUnusedTalkAnim,              // KAIZOKU_ANIM_UNUSED_CONVERSATION
    &gFighterPirateUnusedJumpAnim,              // KAIZOKU_ANIM_UNUSED_JUMP
    &gFighterPirateSidestepAnim,                // KAIZOKU_ANIM_SIDESTEP
    &gFighterPirateWalkAnim,                    // KAIZOKU_ANIM_WALK
    &gFighterPirateDamageAnim,                  // KAIZOKU_ANIM_DAMAGE
    &gFighterPirateUnusedDefeatAnim,            // KAIZOKU_ANIM_UNUSED_DEFEAT
    &gFighterPirateBlockAnim,                   // KAIZOKU_ANIM_BLOCK
    &gFighterPirateFlipAnim,                    // KAIZOKU_ANIM_FLIP
    &gFighterPirateSlashAnim,                   // KAIZOKU_ANIM_SLASH_ATTCK
    &gFighterPirateSpinAttackAnim,              // KAIZOKU_ANIM_SPIN_ATTACK
    &gFighterPirateLandAnim,                    // KAIZOKU_ANIM_LAND
    &gFighterPirateLandTalkAnim,                // KAIZOKU_ANIM_CHALLENGE
    &gFighterPirateUnsheatheAnim,               // KAIZOKU_ANIM_UNSHEATHE
    &gFighterPirateLowerWeaponsAnim,            // KAIZOKU_ANIM_LOWER_WEAPONS
    &gFighterPirateDemonstrativeSwordSwingAnim, // KAIZOKU_ANIM_DEMONSTRATIVE_SWORD_SWING
    &gFighterPirateDefeatAnim,                  // KAIZOKU_ANIM_DEFEAT
    &gFighterPirateDefeatIdleAnim,              // KAIZOKU_ANIM_DEFEAT_IDLE
    &gFighterPirateThrowFlashAnim,              // KAIZOKU_ANIM_THROW_FLASH
};

static u8 sAnimationModes[KAIZOKU_ANIM_MAX] = {
    ANIMMODE_LOOP, // KAIZOKU_ANIM_FIGHTING_IDLE
    ANIMMODE_LOOP, // KAIZOKU_ANIM_UNUSED_CONVERSATION
    ANIMMODE_ONCE, // KAIZOKU_ANIM_UNUSED_JUMP
    ANIMMODE_LOOP, // KAIZOKU_ANIM_SIDESTEP
    ANIMMODE_LOOP, // KAIZOKU_ANIM_WALK
    ANIMMODE_ONCE, // KAIZOKU_ANIM_DAMAGE
    ANIMMODE_ONCE, // KAIZOKU_ANIM_UNUSED_DEFEAT
    ANIMMODE_ONCE, // KAIZOKU_ANIM_BLOCK
    ANIMMODE_ONCE, // KAIZOKU_ANIM_FLIP
    ANIMMODE_ONCE, // KAIZOKU_ANIM_SLASH_ATTCK
    ANIMMODE_ONCE, // KAIZOKU_ANIM_SPIN_ATTACK
    ANIMMODE_ONCE, // KAIZOKU_ANIM_LAND
    ANIMMODE_LOOP, // KAIZOKU_ANIM_CHALLENGE
    ANIMMODE_ONCE, // KAIZOKU_ANIM_UNSHEATHE
    ANIMMODE_ONCE, // KAIZOKU_ANIM_LOWER_WEAPONS
    ANIMMODE_ONCE, // KAIZOKU_ANIM_DEMONSTRATIVE_SWORD_SWING
    ANIMMODE_ONCE, // KAIZOKU_ANIM_DEFEAT
    ANIMMODE_LOOP, // KAIZOKU_ANIM_DEFEAT_IDLE
    ANIMMODE_ONCE, // KAIZOKU_ANIM_THROW_FLASH
};

void EnKaizoku_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnKaizoku* this = (EnKaizoku*)thisx;
    Player* player = GET_PLAYER(play);
    EffectBlureInit1 blureInit;

    this->picto.actor.hintId = TATL_HINT_ID_PIRATE;
    this->picto.actor.attentionRangeType = ATTENTION_RANGE_3;
    this->picto.actor.colChkInfo.mass = 80;
    ActorShape_Init(&this->picto.actor.shape, 0.0f, ActorShadow_DrawFeet, 0.0f);
    this->textType = KAIZOKU_GET_TYPE(this);
    this->picto.actor.colChkInfo.health = 8;
    this->exitIndex = KAIZOKU_GET_EXIT_INDEX(&this->picto.actor);
    this->switchFlag = KAIZOKU_GET_SWITCH_FLAG(&this->picto.actor);

    // There are three spawns (0,1,2)
    // they decided to re-align 2 into 0 instead of making a third text combination
    if (this->textType >= 2) {
        this->textType = 0;
    } else if (this->textType == 2) { //! @Bug: '== 2' also matches '>= 2'
        this->textType = 0;
    }

    this->colorType = KAIZOKU_GET_TYPE(this);
    this->picto.actor.world.rot.z = 0; // clear TYPE param, which was rot.z, as we dont want skew
    this->picto.actor.colChkInfo.damageTable = &sDamageTable;
    SkelAnime_InitFlex(play, &this->skelAnime, &gFighterPirateSkel, &gFighterPirateWalkAnim, this->jointTable,
                       this->morphTable, KAIZOKU_LIMB_MAX);
    Collider_InitAndSetCylinder(play, &this->bodyCollider, &this->picto.actor, &sCylinderInit);
    Collider_InitAndSetQuad(play, &this->swordCollider, &this->picto.actor, &sQuadInit);
    blureInit.p1StartColor[0] = blureInit.p1StartColor[1] = blureInit.p1StartColor[2] = blureInit.p1StartColor[3] =
        blureInit.p2StartColor[0] = blureInit.p2StartColor[1] = blureInit.p2StartColor[2] = blureInit.p1EndColor[0] =
            blureInit.p1EndColor[1] = blureInit.p1EndColor[2] = blureInit.p2EndColor[0] = blureInit.p2EndColor[1] =
                blureInit.p2EndColor[2] = 255;
    blureInit.p2StartColor[3] = 64;
    blureInit.p1EndColor[3] = blureInit.p2EndColor[3] = 0;
    blureInit.elemDuration = 8;
    blureInit.unkFlag = 0;
    blureInit.calcMode = 2;
    Effect_Add(play, &this->blureIndex, EFFECT_BLURE1, 0, 0, &blureInit);
    Actor_SetScale(&this->picto.actor, 0.0125f);
    this->picto.actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->picto.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    if (this->switchFlag == KAIZOKU_SWITCH_FLAG_NONE) {
        this->switchFlag = SWITCH_FLAG_NONE;
    }

    if ((this->switchFlag > SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        Actor_Kill(&this->picto.actor);
        return;
    }

    this->bodyCollider.dim.radius = 20;
    this->bodyCollider.dim.height = 65;
    this->bodyCollider.dim.yShift = 0;
    this->csId = this->picto.actor.csId;
    this->picto.actor.world.pos.y = player->actor.world.pos.y + 160.0f;
    this->picto.validationFunc = EnKaizoku_ValidatePictograph;
    this->picto.actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    EnKaizoku_SetupWaitForApproach(this);
}

void EnKaizoku_Destroy(Actor* thisx, PlayState* play) {
    EnKaizoku* this = (EnKaizoku*)thisx;

    Effect_Destroy(play, this->blureIndex);
    Collider_DestroyCylinder(play, &this->bodyCollider);
    Collider_DestroyQuad(play, &this->swordCollider);
    Audio_RestorePrevBgm();
}

s32 EnKaizoku_DodgeRanged(EnKaizoku* this, PlayState* play) {
    Actor* itemAction;
    Player* player = GET_PLAYER(play);
    s16 prevAction = this->action;

    for (itemAction = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].first; itemAction != NULL;
         itemAction = itemAction->next) {
        if (((itemAction->id != ACTOR_ARMS_HOOK) && (itemAction->id != ACTOR_EN_ARROW)) || (player->unk_D57 == 0)) {
            continue;
        }

        if (((itemAction->id != ACTOR_ARMS_HOOK) || (this->action != KAIZOKU_ACTION_STUNNED)) &&
            (this->action != KAIZOKU_ACTION_JUMP)) {
            EnKaizoku_SetupJump(this);
        }
        break;
    }

    if (prevAction != this->action) {
        return true;
    } else {
        return false;
    }
}

void EnKaizoku_TurnHead(EnKaizoku* this) {
    if (this->action == KAIZOKU_ACTION_READY) {
        this->headRot.y = Math_SinS(this->lookTimer * 4200) * 8920;
    } else if (this->action != KAIZOKU_ACTION_STUNNED) {
        if ((this->action == KAIZOKU_ACTION_SLASH) || (this->action == KAIZOKU_ACTION_SPIN_ATTACK)) {
            Math_SmoothStepToS(&this->headRot.y, this->picto.actor.yawTowardsPlayer - this->picto.actor.shape.rot.y, 1,
                               500, 0);
            this->headRot.y = CLAMP(this->headRot.y, -0x256F, 0x256F);
        } else {
            this->headRot.y = 0;
        }
    }
}

s32 EnKaizoku_ReactToPlayer(EnKaizoku* this, PlayState* play, s16 arg2) {
    Player* player = GET_PLAYER(play);
    s16 angleToWall = ABS_ALT(this->picto.actor.wallYaw - this->picto.actor.shape.rot.y);
    s16 angleToPlayer = ABS_ALT(this->picto.actor.yawTowardsPlayer - this->picto.actor.shape.rot.y);
    Actor* explosiveActor;

    if (func_800BE184(play, &this->picto.actor, 100.0f, 0x2710, 0x4000, this->picto.actor.shape.rot.y)) {
        if (player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_START) {
            if (this->action != KAIZOKU_ACTION_SPIN_DODGE) {
                EnKaizoku_SetupSpinDodge(this, play);
            }
            return true;
        } else {
            EnKaizoku_SetupBlock(this);
            return true;
        }
    } else if (func_800BE184(play, &this->picto.actor, 100.0f, 0x5DC0, 0x2AA8, this->picto.actor.shape.rot.y)) {
        this->picto.actor.shape.rot.y = this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;
        if ((this->picto.actor.bgCheckFlags & BGCHECKFLAG_WALL) && (ABS_ALT(angleToWall) < 0x2EE0) &&
            (this->picto.actor.xzDistToPlayer < 90.0f)) {
            if (this->action != KAIZOKU_ACTION_JUMP) {
                EnKaizoku_SetupJump(this);
            }
            return true;
        } else if (player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_START) {
            if (this->action != KAIZOKU_ACTION_SPIN_DODGE) {
                EnKaizoku_SetupSpinDodge(this, play);
            }
        } else if (this->picto.actor.xzDistToPlayer < BREG(11) + 180.0f) {
            EnKaizoku_SetupBlock(this);
        } else if (this->action != KAIZOKU_ACTION_ROLL_BACK) {
            EnKaizoku_SetupRollBack(this);
        }
        return true;
    }

    explosiveActor = Actor_FindNearby(play, &this->picto.actor, -1, ACTORCAT_EXPLOSIVES, 80.0f);
    if (explosiveActor != NULL) {
        this->picto.actor.shape.rot.y = this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;

        if (((this->picto.actor.bgCheckFlags & BGCHECKFLAG_WALL) && (angleToWall < 0x2EE0)) ||
            (explosiveActor->id == ACTOR_EN_BOM_CHU)) {
            if ((explosiveActor->id == ACTOR_EN_BOM_CHU) &&
                (Actor_WorldDistXYZToActor(&this->picto.actor, explosiveActor) < 80.0f) &&
                (BINANG_ADD(this->picto.actor.shape.rot.y - explosiveActor->world.rot.y, 0x8000) < 0x4000)) {
                if (this->action != KAIZOKU_ACTION_JUMP) {
                    EnKaizoku_SetupJump(this);
                }
            } else {
                EnKaizoku_SetupBlock(this);
            }

            return true;
        }

        if (this->action != KAIZOKU_ACTION_ROLL_BACK) {
            EnKaizoku_SetupRollBack(this);
        }
        return true;
    }

    if (arg2) {
        s16 yawDiff;

    //! FAKE:
    label:;
        if (angleToPlayer >= 0x2710) {
            // in OOT this was sidestep instead of block
            EnKaizoku_SetupBlock(this);
        } else {
            yawDiff = player->actor.shape.rot.y - this->picto.actor.shape.rot.y;
            if ((this->picto.actor.xzDistToPlayer <= 65.0f) && !Actor_OtherIsTargeted(play, &this->picto.actor) &&
                (ABS_ALT(yawDiff) < 0x5000)) {
                if (this->action != KAIZOKU_ACTION_SLASH) {
                    EnKaizoku_SetupSlash(this);
                    return true;
                }
            } else if (this->action != KAIZOKU_ACTION_CIRCLE) {
                EnKaizoku_SetupCircle(this);
            }
        }
        return true;
    }

    return false;
}

void EnKaizoku_ChangeAnim(EnKaizoku* this, EnKaizokuAnimation animIndex) {
    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[this->animIndex]);
    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], 1.0f, 0.0f, this->animEndFrame,
                     sAnimationModes[this->animIndex], 0.0f);
}

s32 EnKaizoku_ValidatePictograph(PlayState* play, Actor* actor) {
    s32 ret = Snap_ValidatePictograph(play, actor, PICTO_VALID_PIRATE_GOOD, &actor->focus.pos, &actor->shape.rot, 10.0f,
                                      400.0f, -1);

    ret |= Snap_ValidatePictograph(play, actor, PICTO_VALID_PIRATE_TOO_FAR, &actor->focus.pos, &actor->shape.rot, 10.0f,
                                   1200.0f, -1);
    return ret;
}

void EnKaizoku_SetupWaitForApproach(EnKaizoku* this) {
    Math_Vec3f_Copy(&this->swordScaleRight, &gZeroVec3f);
    Math_Vec3f_Copy(&this->swordScaleLeft, &gZeroVec3f);
    this->animationsDisabled = true;
    this->action = KAIZOKU_ACTION_HIDDEN;
    this->actionFunc = EnKaizoku_WaitForApproach;
}

void EnKaizoku_WaitForApproach(EnKaizoku* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame = this->skelAnime.curFrame;
    s32 nextTextId;

    switch (this->cutsceneState) {
        case 0: // waiting for proximity
            if (!(this->picto.actor.xzDistToPlayer < 200.0f)) {
                break;
            }

            if (!CutsceneManager_IsNext(this->csId)) {
                CutsceneManager_Queue(this->csId);
                return;
            }

            CutsceneManager_StartWithPlayerCs(this->csId, &this->picto.actor);
            Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_21);
            this->subCamId = CutsceneManager_GetCurrentSubCamId(this->picto.actor.csId);
            this->picto.actor.shape.rot.y = this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;

            nextTextId = (this->textType * 4) + this->textIdOffset;
            if (this->colorType != 2) {
                player->actor.world.pos.x = this->picto.actor.home.pos.x + 90.0f;
                player->actor.world.pos.z = this->picto.actor.home.pos.z + 30.0f;
            } else {
                player->actor.world.pos.x = this->picto.actor.home.pos.x - 90.0f;
                player->actor.world.pos.z = this->picto.actor.home.pos.z - 30.0f;
            }

            player->actor.speed = 0.0f;
            this->picto.actor.world.pos.x = this->picto.actor.home.pos.x;
            this->picto.actor.world.pos.z = this->picto.actor.home.pos.z;
            Message_StartTextbox(play, sKaizokuTextIds[nextTextId], &this->picto.actor);
            this->textIdOffset++; // KAIZOKU_TALK_INTRO, after landing
            this->picto.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            player->actor.shape.rot.y = player->actor.world.rot.y =
                Math_Vec3f_Yaw(&player->actor.world.pos, &this->picto.actor.world.pos);
            Math_Vec3f_Copy(&this->subCamEye, &this->subCamEyeTarget);
            Math_Vec3f_Copy(&this->subCamAt, &this->subCamAtTarget);
            this->subCamUp.x = -0.11f;
            this->picto.actor.draw = EnKaizoku_Draw;
            this->cutsceneTimer = 0;
            Audio_SetMainBgmVolume(0, 0xA);
            this->cutsceneState++;
            FALLTHROUGH;
        case 1: // waiting for (intro1) text advance
            player->actor.shape.rot.y = player->actor.world.rot.y =
                Math_Vec3f_Yaw(&player->actor.world.pos, &this->picto.actor.world.pos);
            this->picto.actor.shape.rot.y = this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;

            if (this->colorType != 2) {
                player->actor.world.pos.x = this->picto.actor.home.pos.x + 90.0f;
                player->actor.world.pos.z = this->picto.actor.home.pos.z + 30.0f;
            } else {
                player->actor.world.pos.x = this->picto.actor.home.pos.x - 90.0f;
                player->actor.world.pos.z = this->picto.actor.home.pos.z - 30.0f;
            }

            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_LAND);
                this->cutsceneTimer = 0;
                this->cutsceneState++;
                this->picto.actor.gravity = -2.0f;
            }
            break;

        case 2: // waiting for fall to land
            if (this->picto.actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                if (this->animationsDisabled) {
                    this->animationsDisabled = false;
                    this->picto.actor.world.pos.y = this->picto.actor.floorHeight;
                    this->picto.actor.velocity.y = 0.0f;
                    Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_4);
                    Actor_SpawnFloorDustRing(play, &this->picto.actor, &this->leftFootPos, 3.0f, 2, 2.0f, 0, 0, false);
                    Actor_SpawnFloorDustRing(play, &this->picto.actor, &this->rightFootPos, 3.0f, 2, 2.0f, 0, 0, false);
                    Actor_PlaySfx(&this->picto.actor, NA_SE_EN_PIRATE_ONGND);
                }

                if (curFrame >= 11.0f) {
                    this->cutsceneState++;
                    this->cutsceneTimer = 0;
                    this->subCamUp.x = 0.0f;
                }
            }
            break;

        case 3: // talking after landing
            if (curFrame >= this->animEndFrame) {
                nextTextId = this->textType * 4 + this->textIdOffset;
                if (Player_GetMask(play) == PLAYER_MASK_STONE) {
                    // adjust textIds to mention the mask
                    if (sKaizokuTextIds[nextTextId] == 0x11A5) {
                        nextTextId = 8;
                    } else if (sKaizokuTextIds[nextTextId] == 0x11A9) {
                        nextTextId = 9;
                    }
                }

                Message_StartTextbox(play, sKaizokuTextIds[nextTextId], &this->picto.actor);
                EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_CHALLENGE);
                this->textIdOffset++; // KAIZOKU_TALK_PLAYER_VICTORY, although that gets set directly later
                this->cutsceneTimer = 0;
                this->cutsceneState++;
            }
            break;

        case 4: // intro 2 dialogue
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                this->cutsceneTimer = 0;
                this->cutsceneState++;
                Audio_SetMainBgmVolume(0x7F, 0);
                Audio_PlayBgm_StorePrevBgm(NA_BGM_MINI_BOSS);
                EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_UNSHEATHE);
            }
            break;

        case 5: // start pulling out swords
            Math_ApproachF(&this->subCamVelocity, 2.0f, 0.2f, 1.0f);
            if (Animation_OnFrame(&this->skelAnime, 20.0f)) {
                Actor_PlaySfx(&this->picto.actor, NA_SE_EN_BOSU_SWORD);
            }
            if (curFrame >= 30.0f) {
                this->cutsceneTimer = 0;
                this->cutsceneState++;
            }
            break;

        case 6: // finish pulling swords
            Math_ApproachF(&this->subCamVelocity, 5.0f, 0.3f, 1.0f);
            if (curFrame >= this->animEndFrame) {
                this->cutsceneTimer = 7;
                this->cutsceneState++;
                this->swordScaleRight.x = 1.0f;
                this->swordScaleRight.y = 1.0f;
                this->swordScaleRight.z = 1.0f;
                this->swordScaleLeft.x = 1.0f;
                this->swordScaleLeft.y = 1.0f;
                this->swordScaleLeft.z = 1.0f;
            }
            break;

        case 7: // wait for cutscene timer, then start fight
            if (this->cutsceneTimer == 0) {
                Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_END);
                CutsceneManager_Stop(this->csId);
                this->cutsceneState = 0;
                this->subCamId = SUB_CAM_ID_DONE;
                this->picto.actor.flags &= ~ACTOR_FLAG_FREEZE_EXCEPTION;
                this->picto.actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
                this->picto.actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                EnKaizoku_SetupReady(this);
            }
            break;
    }

    if (this->cutsceneState < 7) {
        s32 cameraIndex = this->colorType * 8;

        cameraIndex += this->cutsceneState;
        this->subCamEyeTarget.x = sCutsceneCameraPositions[cameraIndex].x + this->picto.actor.home.pos.x;
        this->subCamEyeTarget.y = sCutsceneCameraPositions[cameraIndex].y + player->actor.world.pos.y;
        this->subCamEyeTarget.z = sCutsceneCameraPositions[cameraIndex].z + this->picto.actor.home.pos.z;

        this->subCamAtTarget.x = sCutsceneCameraTargetPositions[cameraIndex].x + this->picto.actor.home.pos.x;
        this->subCamAtTarget.y = sCutsceneCameraTargetPositions[cameraIndex].y + player->actor.world.pos.y;
        this->subCamAtTarget.z = sCutsceneCameraTargetPositions[cameraIndex].z + this->picto.actor.home.pos.z;
    }

    if (this->cutsceneState >= 5) {
        if (curFrame >= 6.0f) {
            Math_ApproachF(&this->swordScaleRight.x, 1.0f, 0.3f, 0.3f);
            this->swordScaleRight.z = this->swordScaleRight.y = this->swordScaleRight.x;
        }

        if (curFrame >= 22.0f) {
            Math_ApproachF(&this->swordScaleLeft.x, 1.0f, 0.3f, 0.3f);
            this->swordScaleLeft.z = this->swordScaleLeft.y = this->swordScaleLeft.x;
        }
    }

    if (this->cutsceneState >= 5) {
        Math_ApproachF(&this->subCamEye.x, this->subCamEyeTarget.x, 0.5f, this->subCamVelocity);
        Math_ApproachF(&this->subCamEye.y, this->subCamEyeTarget.y, 0.5f, this->subCamVelocity);
        Math_ApproachF(&this->subCamEye.z, this->subCamEyeTarget.z, 0.5f, this->subCamVelocity);
        Math_ApproachF(&this->subCamAt.x, this->subCamAtTarget.x, 0.5f, this->subCamVelocity);
        Math_ApproachF(&this->subCamAt.y, this->subCamAtTarget.y, 0.5f, this->subCamVelocity);
        Math_ApproachF(&this->subCamAt.z, this->subCamAtTarget.z, 0.5f, this->subCamVelocity);
    } else {
        Math_Vec3f_Copy(&this->subCamEye, &this->subCamEyeTarget);
        Math_Vec3f_Copy(&this->subCamAt, &this->subCamAtTarget);
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        this->subCamUp.y = 1.0f;
        this->subCamUp.z = 0.0f;
        Play_SetCameraAtEyeUp(play, this->subCamId, &this->subCamAt, &this->subCamEye, &this->subCamUp);
    }
}

// Player has lost the fight
void EnKaizoku_SetupPlayerLoss(EnKaizoku* this, PlayState* play) {
    if (this->subCamId == SUB_CAM_ID_DONE) {
        if (!CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_Queue(this->csId);
            return;
        }
        CutsceneManager_StartWithPlayerCs(this->csId, &this->picto.actor);
    }

    Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_96);
    this->subCamId = CutsceneManager_GetCurrentSubCamId(this->picto.actor.csId);
    this->combatTimer = 30;
    this->picto.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->cutsceneTimer = 0;
    this->cutsceneState = 0;
    this->animationsDisabled = false;
    this->action = KAIZOKU_ACTION_HIDDEN;
    this->actionFunc = EnKaizoku_PlayerLoss;
    this->picto.actor.speed = 0.0f;
}

void EnKaizoku_PlayerLoss(EnKaizoku* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 1, 0xFA0, 1);
    this->subCamEye.x = Math_SinS(this->picto.actor.yawTowardsPlayer) * 155.0f + this->picto.actor.world.pos.x;
    this->subCamEye.y = this->picto.actor.world.pos.y + 10.0f;
    this->subCamEye.z = Math_CosS(this->picto.actor.yawTowardsPlayer) * 115.0f + this->picto.actor.world.pos.z;

    this->subCamAt.x = Math_SinS(this->picto.actor.yawTowardsPlayer) + this->picto.actor.world.pos.x;
    this->subCamAt.y = this->picto.actor.world.pos.y + 30.0f;
    this->subCamAt.z = Math_CosS(this->picto.actor.yawTowardsPlayer) * 11.0f + this->picto.actor.world.pos.z;

    switch (this->cutsceneState) {
        case 0: // waiting for animation to change
            if (curFrame >= this->animEndFrame) {
                EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_LOWER_WEAPONS);
                this->textIdOffset = KAIZOKU_TALK_PLAYER_DEFEAT;
                this->cutsceneTimer = 0;
                this->cutsceneState++;
            }
            break;

        case 1: // wait for weapons to finish lowering
            if (curFrame >= this->animEndFrame) {
                s32 textId;

                EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_DEMONSTRATIVE_SWORD_SWING);
                textId = this->textType * 4 + this->textIdOffset;
                Message_StartTextbox(play, sKaizokuTextIds[textId], &this->picto.actor);
                Actor_PlaySfx(&this->picto.actor, NA_SE_EN_LAST2_SHOUT);
                this->cutsceneTimer = 0;
                this->cutsceneState++;
            }
            break;

        case 2: // wait for text to finish
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_END);
                CutsceneManager_Stop(this->csId);
                this->subCamId = SUB_CAM_ID_DONE;
                play->nextEntrance = play->setupExitList[this->exitIndex];
                gSaveContext.nextCutsceneIndex = 0;
                Scene_SetExitFade(play);
                play->transitionTrigger = TRANS_TRIGGER_START;
                this->cutsceneState++;
                this->action = KAIZOKU_ACTION_SCENE_FADE;
            }
            break;

        default:
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        this->subCamUp.x = 0.0f;
        this->subCamUp.y = 1.0f;
        this->subCamUp.z = 0.0f;
        Play_SetCameraAtEyeUp(play, this->subCamId, &this->subCamAt, &this->subCamEye, &this->subCamUp);
    }
}

// Player has won the fight
void EnKaizoku_SetupPlayerWinCutscene(EnKaizoku* this) {
    this->animationsDisabled = false;
    this->action = KAIZOKU_ACTION_HIDDEN;
    this->actionFunc = EnKaizoku_PlayerWinCutscene;
}

void EnKaizoku_PlayerWinCutscene(EnKaizoku* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 textId;
    f32 curFrame = this->skelAnime.curFrame;

    if (this->cutsceneState < 2) {
        Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 1, 0xFA0, 1);
        player->actor.world.pos.x = this->picto.actor.home.pos.x + 90.0f;
        player->actor.world.pos.z = this->picto.actor.home.pos.z + 30.0f;
        this->picto.actor.world.pos.x = this->picto.actor.home.pos.x;
        this->picto.actor.world.pos.z = this->picto.actor.home.pos.z;
        this->subCamEyeTarget.x = player->actor.world.pos.x + 39.0f;
        this->subCamEyeTarget.y = player->actor.world.pos.y + 4.0f;
        this->subCamEyeTarget.z = player->actor.world.pos.z - 41.0f;
        this->subCamAtTarget.x = player->actor.world.pos.x - 150.0f;
        this->subCamAtTarget.y = player->actor.world.pos.y + 60.0f;
        this->subCamAtTarget.z = player->actor.world.pos.z + 50.0f;
    }

    player->actor.shape.rot.y = player->actor.world.rot.y =
        Math_Vec3f_Yaw(&player->actor.world.pos, &this->picto.actor.world.pos);
    switch (this->cutsceneState) {
        case 0: // start: change animation and start talking
            EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_DEFEAT_IDLE);
            this->textIdOffset = KAIZOKU_TALK_PLAYER_VICTORY;
            textId = this->textType * 4 + this->textIdOffset;
            Message_StartTextbox(play, sKaizokuTextIds[textId], &this->picto.actor);
            this->defeatBreathingStarted = false;
            this->cutsceneTimer = 0;
            this->cutsceneState++;
            break;

        case 1: // talking
            if (curFrame >= this->animEndFrame) {
                if (this->defeatBreathingStarted == false) {
                    Actor_PlaySfx(&this->picto.actor, NA_SE_EN_PIRATE_DAMM_BREATH);
                    this->defeatBreathingStarted = true;
                } else {
                    Actor_PlaySfx(&this->picto.actor, NA_SE_EN_PIRATE_BREATH);
                }
            }

            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_THROW_FLASH);
                Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_133);
                this->flashTimer = 0;
                this->cutsceneTimer = 0;
                this->cutsceneState++;
            }
            break;

        case 2: // throwing a nut to escape
            this->subCamEyeTarget.x = player->actor.world.pos.x + 77.0f;
            this->subCamEyeTarget.y = player->actor.world.pos.y + 84.0f;
            this->subCamEyeTarget.z = player->actor.world.pos.z - 71.0f;

            this->subCamAtTarget.x = player->actor.world.pos.x - 110.0f;
            this->subCamAtTarget.y = player->actor.world.pos.y;
            this->subCamAtTarget.z = player->actor.world.pos.z + 30.0f;
            this->flashTimer++;
            if (curFrame >= 1.0f) {
                Math_ApproachZeroF(&this->swordScaleRight.x, 0.3f, 0.3f);
                Math_ApproachZeroF(&this->swordScaleLeft.x, 0.3f, 0.3f);
                this->swordScaleRight.z = this->swordScaleRight.y = this->swordScaleRight.x;
                this->swordScaleLeft.z = this->swordScaleLeft.y = this->swordScaleLeft.x;
            }
            if (this->flashTimer == 18) {
                Actor* dekuNut;

                Actor_PlaySfx(&this->picto.actor, NA_SE_EN_PIRATE_SHOUT);
                // oddly, instead of just playing the sfx and spawning an effect
                //  they spawn a real nut
                dekuNut = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ARROW, this->picto.actor.world.pos.x,
                                      this->picto.actor.world.pos.y + 10.0f, this->picto.actor.world.pos.z,
                                      this->picto.actor.shape.rot.x, this->picto.actor.shape.rot.y,
                                      this->picto.actor.shape.rot.z, -ARROW_TYPE_DEKU_NUT);

                if (dekuNut != NULL) {
                    dekuNut->gravity = -10.0f;
                    play->envCtx.fillScreen = true;
                }
            }

            if (this->flashTimer >= 18) {
                Math_ApproachF(&this->flashScreenAlphaTarget, 60.0f, 1.0f, 20.0f);
                this->flashScreenAlpha = this->flashScreenAlphaTarget / 60.0f;
                play->envCtx.screenFillColor[3] = this->flashScreenAlpha * 255.0f;
                play->envCtx.screenFillColor[0] = play->envCtx.screenFillColor[1] = play->envCtx.screenFillColor[2] =
                    255;
            }

            if ((curFrame >= this->animEndFrame) && (this->flashTimer >= 40)) {
                this->picto.actor.draw = NULL;
                this->cutsceneTimer = 10;
                Math_Vec3f_Copy(&this->swordScaleRight, &gZeroVec3f);
                Math_Vec3f_Copy(&this->swordScaleLeft, &gZeroVec3f);
                this->cutsceneState++;
            }
            break;

        case 3: // nut flash is dissapearing
            if (this->cutsceneTimer == 0) {
                Math_ApproachZeroF(&this->flashScreenAlphaTarget, 0.5f, 10.0f);
                this->flashScreenAlpha = this->flashScreenAlphaTarget / 60.0f;
                play->envCtx.screenFillColor[3] = this->flashScreenAlpha * 255.0f;
                if (play->envCtx.screenFillColor[3] < 10) {
                    play->envCtx.screenFillColor[0] = 0;
                    play->envCtx.screenFillColor[1] = 0;
                    play->envCtx.screenFillColor[2] = 0;
                    play->envCtx.screenFillColor[3] = 0;
                    play->envCtx.fillScreen = false;
                    this->cutsceneState = 0;
                    this->subCamId = SUB_CAM_ID_DONE;
                    Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_END);
                    CutsceneManager_Stop(this->csId);
                    if (this->switchFlag > SWITCH_FLAG_NONE) {
                        Flags_SetSwitch(play, this->switchFlag);
                    }
                    Actor_Kill(&this->picto.actor);
                }
            }
            break;

        default:
            break;
    }
    Math_ApproachF(&this->subCamEye.x, this->subCamEyeTarget.x, 0.5f, this->subCamVelocity);
    Math_ApproachF(&this->subCamEye.y, this->subCamEyeTarget.y, 0.5f, this->subCamVelocity);
    Math_ApproachF(&this->subCamEye.z, this->subCamEyeTarget.z, 0.5f, this->subCamVelocity);
    Math_ApproachF(&this->subCamAt.x, this->subCamAtTarget.x, 0.5f, this->subCamVelocity);
    Math_ApproachF(&this->subCamAt.y, this->subCamAtTarget.y, 0.5f, this->subCamVelocity);
    Math_ApproachF(&this->subCamAt.z, this->subCamAtTarget.z, 0.5f, this->subCamVelocity);
    Math_ApproachF(&this->subCamVelocity, 10.0f, 0.5f, 100.0f);

    if (this->subCamId != SUB_CAM_ID_DONE) {
        this->subCamUp.x = 0.0f;
        this->subCamUp.z = 0.0f;
        this->subCamUp.y = 1.0f;
        Play_SetCameraAtEyeUp(play, this->subCamId, &this->subCamAt, &this->subCamEye, &this->subCamUp);
    }
}

// Combat idle
void EnKaizoku_SetupReady(EnKaizoku* this) {
    this->picto.actor.speed = 0.0f;
    EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_FIGHTING_IDLE);
    this->action = KAIZOKU_ACTION_READY;
    this->actionFunc = EnKaizoku_Ready;
    this->picto.actor.shape.shadowScale = 90.0f;
}

void EnKaizoku_Ready(EnKaizoku* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((this->lookTimer == 0) && !EnKaizoku_DodgeRanged(this, play) && !EnKaizoku_ReactToPlayer(this, play, false)) {
        s16 angleToPlayer = this->picto.actor.yawTowardsPlayer - this->picto.actor.shape.rot.y;

        if ((this->picto.actor.xzDistToPlayer < 100.0f) && (player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) &&
            (angleToPlayer >= 0x1F40)) {
            this->picto.actor.shape.rot.y = this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;
            EnKaizoku_SetupCircle(this);
        } else if (Actor_IsFacingPlayer(&this->picto.actor, 0xBB8)) {
            if ((this->picto.actor.xzDistToPlayer < 400.0f) && (this->picto.actor.xzDistToPlayer > 150.0f) &&
                (Rand_ZeroOne() < 0.7f)) {
                if ((Rand_ZeroOne() > 0.5f) || (ABS_ALT(angleToPlayer) < 0x3000)) {
                    EnKaizoku_SetupRollForward(this);
                } else {
                    EnKaizoku_SetupSpinAttack(this);
                }
            } else if (Rand_ZeroOne() > 0.1f) {
                EnKaizoku_SetupAdvance(this);
            } else {
                EnKaizoku_SetupCircle(this);
            }
        } else {
            EnKaizoku_SetupPivot(this);
        }
    }
}

void EnKaizoku_SetupSpinDodge(EnKaizoku* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_SIDESTEP);
    if (Math_SinS(player->actor.shape.rot.y - this->picto.actor.shape.rot.y) > 0.0f) {
        this->picto.actor.speed = -10.0f;
    } else if (Math_SinS(player->actor.shape.rot.y - this->picto.actor.shape.rot.y) < 0.0f) {
        this->picto.actor.speed = 10.0f;
    } else if (Rand_ZeroOne() > 0.5f) {
        this->picto.actor.speed = 10.0f;
    } else {
        this->picto.actor.speed = -10.0f;
    }

    this->skelAnime.playSpeed = 1.0f;
    this->picto.actor.world.rot.y = this->picto.actor.shape.rot.y;
    this->combatTimer = 6;
    this->action = KAIZOKU_ACTION_SPIN_DODGE;
    this->circlingRate = 0.0f;
    this->actionFunc = EnKaizoku_SpinDodge;
}

void EnKaizoku_SpinDodge(EnKaizoku* this, PlayState* play) {
    s16 wallAngle;
    s32 beforeCurFrame;
    s32 afterCurFrame;

    this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer + 0x3A98;
    if ((this->picto.actor.bgCheckFlags & BGCHECKFLAG_WALL) ||
        !Actor_TestFloorInDirection(&this->picto.actor, play, this->picto.actor.speed,
                                    this->picto.actor.shape.rot.y + 0x4000)) {
        if (this->picto.actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            if (this->picto.actor.speed >= 0.0f) {
                wallAngle = this->picto.actor.shape.rot.y + 0x4000;
            } else {
                wallAngle = this->picto.actor.shape.rot.y - 0x4000;
            }
            wallAngle = this->picto.actor.wallYaw - wallAngle;
        } else {
            this->picto.actor.speed *= -0.8f;
            wallAngle = 0;
        }

        if (ABS_ALT(wallAngle) > 0x4000) {
            EnKaizoku_SetupJump(this);
            return;
        }
    }

    if (this->picto.actor.xzDistToPlayer <= 65.0f) {
        Math_ApproachF(&this->circlingRate, -4.0f, 1.0f, 1.5f);
    } else if (this->picto.actor.xzDistToPlayer > 40.0f) {
        Math_ApproachF(&this->circlingRate, 4.0f, 1.0f, 1.5f);
    } else {
        Math_ApproachZeroF(&this->circlingRate, 1.0f, 6.65f);
    }

    if (this->circlingRate != 0.0f) {
        this->picto.actor.world.pos.x += Math_SinS(this->picto.actor.yawTowardsPlayer) * this->circlingRate;
        this->picto.actor.world.pos.z += Math_CosS(this->picto.actor.yawTowardsPlayer) * this->circlingRate;
    }

    this->skelAnime.playSpeed = 1.0f;
    beforeCurFrame = this->skelAnime.curFrame - this->skelAnime.playSpeed;
    afterCurFrame = this->skelAnime.curFrame + this->skelAnime.playSpeed;
    if ((this->skelAnime.curFrame != 0.0f) &&
        (((beforeCurFrame < 0) && (afterCurFrame > 0)) || ((beforeCurFrame < 5) && (afterCurFrame >= 6)))) {
        Actor_PlaySfx(&this->picto.actor, NA_SE_EN_GERUDOFT_WALK);
    }

    if (!(play->gameplayFrames & 0x5F)) {
        Actor_PlaySfx(&this->picto.actor, NA_SE_EN_PIRATE_BREATH);
    }

    if (this->combatTimer == 0) {
        this->picto.actor.shape.rot.y = this->picto.actor.yawTowardsPlayer;
        if (!EnKaizoku_DodgeRanged(this, play)) {
            if (this->picto.actor.xzDistToPlayer <= 70.0f) {
                EnKaizoku_SetupSlash(this);
            } else {
                EnKaizoku_SetupRollBack(this);
            }
        }
    } else if (this->picto.actor.speed >= 0.0f) {
        this->picto.actor.shape.rot.y += 0x4000;
    } else {
        this->picto.actor.shape.rot.y -= 0x4000;
    }
}

void EnKaizoku_SetupBlock(EnKaizoku* this) {
    EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_BLOCK);

    if (this->swordState != 0) {
        this->swordState = -1;
    }
    this->animationsDisabled = false;
    this->picto.actor.speed = 0.0f;
    this->combatTimer = Rand_S16Offset(10, 10);
    this->bodyCollider.base.acFlags |= AC_HARD;
    this->lookTimer = 12;
    this->action = KAIZOKU_ACTION_BLOCK;
    this->actionFunc = EnKaizoku_Block;
}

void EnKaizoku_Block(EnKaizoku* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 angleToPlayer;

    this->animationsDisabled = true;
    if (this->combatTimer == 0) {
        this->skelAnime.playSpeed = 1.0f;
    }
    if (this->lookTimer == 0) {
        this->animationsDisabled = false;
        angleToPlayer = this->picto.actor.yawTowardsPlayer - this->picto.actor.shape.rot.y;
        if ((ABS_ALT(angleToPlayer) <= 0x4000) && (this->picto.actor.xzDistToPlayer < 40.0f) &&
            (fabsf(this->picto.actor.playerHeightRel) < 50.0f)) {
            if (func_800BE184(play, &this->picto.actor, 100.0f, 10000, 0x4000, this->picto.actor.shape.rot.y)) {
                if (player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_START) {
                    this->bodyCollider.base.acFlags &= ~AC_HARD;
                    EnKaizoku_SetupSpinDodge(this, play);
                } else if ((play->gameplayFrames % 2) == 0) {
                    EnKaizoku_SetupBlock(this);
                } else {
                    this->bodyCollider.base.acFlags &= ~AC_HARD;
                    EnKaizoku_SetupRollBack(this);
                }
            } else {
                angleToPlayer = player->actor.shape.rot.y - this->picto.actor.shape.rot.y;
                if (ABS_ALT(angleToPlayer) < 0x4000) {
                    this->bodyCollider.base.acFlags &= ~AC_HARD;
                    EnKaizoku_SetupSlash(this);
                } else {
                    this->bodyCollider.base.acFlags &= ~AC_HARD;
                    EnKaizoku_SetupCircle(this);
                }
            }
        } else {
            this->bodyCollider.base.acFlags &= ~AC_HARD;
            EnKaizoku_SetupCircle(this);
        }
    } else if ((this->combatTimer == 0) &&
               func_800BE184(play, &this->picto.actor, 100.0f, 10000, 0x4000, this->picto.actor.shape.rot.y)) {
        if (player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_START) {
            this->bodyCollider.base.acFlags &= ~AC_HARD;
            EnKaizoku_SetupSpinDodge(this, play);
        } else if (!EnKaizoku_DodgeRanged(this, play)) {
            if (!(play->gameplayFrames & 1)) {
                if ((this->picto.actor.xzDistToPlayer < 100.0f) && (Rand_ZeroOne() > 0.7f)) {
                    this->bodyCollider.base.acFlags &= ~AC_HARD;
                    EnKaizoku_SetupJump(this);
                } else {
                    this->bodyCollider.base.acFlags &= ~AC_HARD;
                    EnKaizoku_SetupRollBack(this);
                }
            } else {
                EnKaizoku_SetupBlock(this);
            }
        }
    }
}

void EnKaizoku_SetupJump(EnKaizoku* this) {
    EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_FLIP);
    this->picto.actor.speed = 6.5f;
    this->picto.actor.velocity.y = 15.0f;
    Actor_PlaySfx(&this->picto.actor, NA_SE_EN_TEKU_JUMP);
    this->picto.actor.world.rot.y = this->picto.actor.shape.rot.y;
    this->bodyCollider.elem.elemMaterial = ELEM_MATERIAL_UNK4;
    this->bodyCollider.base.colMaterial = COL_MATERIAL_NONE;
    this->swordCollider.elem.elemMaterial = ELEM_MATERIAL_UNK4;
    this->action = KAIZOKU_ACTION_JUMP;
    this->actionFunc = EnKaizoku_Jump;
}

static Color_RGBA8 sKaizokuDustPrimColor = { 200, 160, 120, 255 }; // light brown
static Color_RGBA8 sKaizokuDustEnvColor = { 130, 90, 50, 255 };    // darker brown

static Vec3f sKaizokuDustVelocity = { 0.0f, -1.5f, 0.0f };
static Vec3f sKaizokuDustAccel = { 0.0f, -0.2f, 0.0f };

// Used for backflip
void EnKaizoku_SpawnVerticalFootDust(PlayState* play, Vec3f* pos) {
    EffectSsKirakira_SpawnSmall(play, pos, &sKaizokuDustVelocity, &sKaizokuDustAccel, &sKaizokuDustPrimColor,
                                &sKaizokuDustEnvColor);
}

void EnKaizoku_Jump(EnKaizoku* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 1, 0xFA0, 1);
    if (this->picto.actor.velocity.y >= 5.0f) {
        EnKaizoku_SpawnVerticalFootDust(play, &this->leftFootPos);
        EnKaizoku_SpawnVerticalFootDust(play, &this->rightFootPos);
    }

    this->animationsDisabled = false;
    if ((curFrame >= this->animEndFrame) &&
        (this->picto.actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH))) {
        this->bodyCollider.elem.elemMaterial = ELEM_MATERIAL_UNK1;
        this->bodyCollider.base.colMaterial = COL_MATERIAL_HIT3;
        this->swordCollider.elem.elemMaterial = ELEM_MATERIAL_UNK2;
        this->picto.actor.shape.rot.x = 0;
        this->picto.actor.world.rot.y = this->picto.actor.shape.rot.y = this->picto.actor.yawTowardsPlayer;
        this->picto.actor.speed = 0.0f;
        this->picto.actor.velocity.y = 0.0f;
        this->picto.actor.world.pos.y = this->picto.actor.floorHeight;
        EnKaizoku_SetupSlash(this);
    }
}

void EnKaizoku_SetupRollBack(EnKaizoku* this) {
    EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_FLIP);
    this->picto.actor.speed = -8.0f;
    Actor_PlaySfx(&this->picto.actor, NA_SE_EN_TEKU_JUMP);
    this->bodyCollider.elem.elemMaterial = ELEM_MATERIAL_UNK4;
    this->bodyCollider.base.colMaterial = COL_MATERIAL_NONE;
    this->swordCollider.elem.elemMaterial = ELEM_MATERIAL_UNK4;
    this->action = KAIZOKU_ACTION_ROLL_BACK;
    this->actionFunc = EnKaizoku_RollBack;
    this->picto.actor.shape.rot.y = this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;
}

void EnKaizoku_RollBack(EnKaizoku* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    this->animationsDisabled = false;
    if (curFrame >= this->animEndFrame) {
        if ((this->picto.actor.xzDistToPlayer < 170.0f) && (this->picto.actor.xzDistToPlayer > 140.0f) &&
            (Rand_ZeroOne() < 0.2f)) {
            EnKaizoku_SetupSpinAttack(this);
        } else {
            EnKaizoku_SetupBlock(this);
        }
    }
    if (!(play->state.frames & 0x5F)) {
        Actor_PlaySfx(&this->picto.actor, NA_SE_EN_PIRATE_BREATH);
    }
}

void EnKaizoku_SetupSlash(EnKaizoku* this) {
    EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_SLASH_ATTCK);
    this->spinAttackState = 0;
    this->swordCollider.base.atFlags &= ~AT_BOUNCED;
    this->picto.actor.speed = 0.0f;
    AudioSfx_StopByPosAndId(&this->picto.actor.projectedPos, NA_SE_EN_PIRATE_BREATH);
    this->action = KAIZOKU_ACTION_SLASH;
    this->actionFunc = EnKaizoku_Slash;
}

void EnKaizoku_Slash(EnKaizoku* this, PlayState* play2) {
    PlayState* play = play2;
    f32 curFrame;
    s16 rotYDelta;
    s16 rotYDelta2;
    Player* player = GET_PLAYER(play2);

    curFrame = this->skelAnime.curFrame;
    // one is left and one is right, but y rotation is counter clockwise, this hurts my head
    rotYDelta = ABS_ALT(player->actor.shape.rot.y - this->picto.actor.shape.rot.y);
    rotYDelta2 = ABS_ALT(this->picto.actor.yawTowardsPlayer - this->picto.actor.shape.rot.y);
    this->picto.actor.speed = 0.0f;

    if (Animation_OnFrame(&this->skelAnime, 1.0f)) {
        Actor_PlaySfx(&this->picto.actor, NA_SE_EN_PIRATE_ATTACK);
        this->swordState = 1;
    } else if (Animation_OnFrame(&this->skelAnime, 6.0f)) {
        this->swordState = -1;
    }

    if (this->swordCollider.base.atFlags & AT_BOUNCED) {
        this->swordState = -1;
        this->swordCollider.base.atFlags &= ~(AT_BOUNCED | AT_HIT);
        EnKaizoku_SetupRollBack(this);
        return;
    }

    this->animationsDisabled = false;
    if (curFrame >= this->animEndFrame) {
        if (!Actor_IsFacingPlayer(&this->picto.actor, 0x1554)) {
            EnKaizoku_SetupReady(this);

            if (rotYDelta2 > 0x4000) {
                this->lookTimer = 20;
            }
        } else {
            if ((Rand_ZeroOne() > 0.7f) || (this->picto.actor.xzDistToPlayer >= 120.0f)) {
                EnKaizoku_SetupReady(this);
                return;
            }

            this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;

            if (BREG(12) * 0.1f + 0.01f * 40.0f < Rand_ZeroOne()) {
                EnKaizoku_SetupBlock(this);
            } else if (rotYDelta <= 0x2710) {
                if (rotYDelta2 > 0x4000) {
                    this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;
                    EnKaizoku_SetupCircle(this);
                } else {
                    EnKaizoku_ReactToPlayer(this, play, true);
                }
            } else {
                EnKaizoku_SetupCircle(this);
            }
        }
    }
}

void EnKaizoku_SetupRollForward(EnKaizoku* this) {
    EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_FLIP);
    this->combatTimer = 0;
    this->picto.actor.speed = 10.0f;
    this->picto.actor.world.rot.y = this->picto.actor.shape.rot.y = this->picto.actor.yawTowardsPlayer;
    Actor_PlaySfx(&this->picto.actor, NA_SE_EN_TEKU_JUMP);
    this->action = KAIZOKU_ACTION_ROLL_FORWARD;
    this->actionFunc = EnKaizoku_RollForward;
}

void EnKaizoku_RollForward(EnKaizoku* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    this->animationsDisabled = false;
    if (curFrame >= this->animEndFrame) {
        this->picto.actor.speed = 0.0f;
        if (!Actor_IsFacingPlayer(&this->picto.actor, 0x1554)) {
            EnKaizoku_SetupReady(this);
            this->combatTimer = Rand_ZeroOne() * 5.0f + 5.0f;
        } else {
            EnKaizoku_SetupSlash(this);
        }
    }
    if (!(play->gameplayFrames & 0x5F)) {
        Actor_PlaySfx(&this->picto.actor, NA_SE_EN_PIRATE_BREATH);
    }
}

void EnKaizoku_SetupAdvance(EnKaizoku* this) {
    EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_WALK);
    this->action = KAIZOKU_ACTION_ADVANCE;
    this->actionFunc = EnKaizoku_Advance;
}

void EnKaizoku_Advance(EnKaizoku* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 beforeCurFrame;
    s32 afterCurFrame;
    s16 yawDiff;

    if (!EnKaizoku_DodgeRanged(this, play)) {
        Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 1, 0x2EE, 0);
        this->picto.actor.world.rot.y = this->picto.actor.shape.rot.y;
        if (this->picto.actor.xzDistToPlayer <= 40.0f) {
            Math_ApproachF(&this->picto.actor.speed, -8.0f, 1.0f, 1.5f);
        } else if (this->picto.actor.xzDistToPlayer > 55.0f) {
            Math_ApproachF(&this->picto.actor.speed, 8.0f, 1.0f, 1.5f);
        } else {
            Math_ApproachZeroF(&this->picto.actor.speed, 2.0f, 6.65f);
        }

        this->skelAnime.playSpeed = 1.0f;
        yawDiff = ABS_ALT(player->actor.shape.rot.y - this->picto.actor.shape.rot.y);
        if ((this->picto.actor.xzDistToPlayer < 150.0f) && (player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) &&
            (yawDiff >= 0x2000)) {
            this->picto.actor.shape.rot.y = this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;
            if (Rand_ZeroOne() > 0.7f) {
                EnKaizoku_SetupCircle(this);
                return;
            }
        }

        beforeCurFrame = this->skelAnime.curFrame - this->skelAnime.playSpeed;
        afterCurFrame = this->skelAnime.curFrame + this->skelAnime.playSpeed;
        if (!Actor_IsFacingPlayer(&this->picto.actor, 0x11C7)) {
            if (Rand_ZeroOne() > 0.5f) {
                EnKaizoku_SetupCircle(this);
            } else {
                EnKaizoku_SetupReady(this);
            }
        } else if (this->picto.actor.xzDistToPlayer < 90.0f) {
            if ((Rand_ZeroOne() > 0.03f) || ((this->picto.actor.xzDistToPlayer <= 65.0f) && (yawDiff < 0x4000))) {
                EnKaizoku_SetupSlash(this);
            } else if (Actor_OtherIsTargeted(play, &this->picto.actor) && (Rand_ZeroOne() > 0.5f)) {
                EnKaizoku_SetupRollBack(this);
            } else {
                EnKaizoku_SetupCircle(this);
            }
        }

        if (!EnKaizoku_ReactToPlayer(this, play, false)) {
            if ((this->picto.actor.xzDistToPlayer < 210.0f) && (this->picto.actor.xzDistToPlayer > 150.0f) &&
                Actor_IsFacingPlayer(&this->picto.actor, 0x1388)) {
                if (Actor_IsTargeted(play, &this->picto.actor)) {
                    if (Rand_ZeroOne() > 0.5f) {
                        EnKaizoku_SetupRollForward(this);
                    } else {
                        EnKaizoku_SetupSpinAttack(this);
                    }
                } else {
                    EnKaizoku_SetupCircle(this);
                    return;
                }
            }

            if (!(play->gameplayFrames & 0x5F)) {
                Actor_PlaySfx(&this->picto.actor, NA_SE_EN_PIRATE_BREATH);
            }

            if ((this->skelAnime.curFrame != 0.0f) &&
                (((beforeCurFrame < 0) && (afterCurFrame > 0)) || ((beforeCurFrame < 4) && (afterCurFrame >= 5)))) {
                Actor_PlaySfx(&this->picto.actor, NA_SE_EN_GERUDOFT_WALK);
            }
        }
    }
}

void EnKaizoku_SetupPivot(EnKaizoku* this) {
    EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_SIDESTEP);
    this->action = KAIZOKU_ACTION_PIVOT;
    this->actionFunc = EnKaizoku_Pivot;
}

void EnKaizoku_Pivot(EnKaizoku* this, PlayState* play) {
    s16 angleToPlayer;
    s16 turnRate;

    if (!EnKaizoku_DodgeRanged(this, play) && !EnKaizoku_ReactToPlayer(this, play, false)) {
        angleToPlayer = this->picto.actor.yawTowardsPlayer - this->picto.actor.shape.rot.y;
        if (angleToPlayer > 0) {
            turnRate = TRUNCF_BINANG(angleToPlayer * 0.25f) + 0x7D0;
        } else {
            turnRate = TRUNCF_BINANG(angleToPlayer * 0.25f) - 0x7D0;
        }
        this->picto.actor.shape.rot.y += turnRate;
        this->picto.actor.world.rot.y = this->picto.actor.shape.rot.y;
        this->skelAnime.playSpeed = 1.0f;

        if (Actor_IsFacingPlayer(&this->picto.actor, 0x1388)) {
            if (Rand_ZeroOne() > 0.8f) {
                EnKaizoku_SetupCircle(this);
            } else {
                EnKaizoku_SetupAdvance(this);
            }
        }
        if (!(play->gameplayFrames & 0x5F)) {
            Actor_PlaySfx(&this->picto.actor, NA_SE_EN_PIRATE_BREATH);
        }
    }
}

void EnKaizoku_SetupSpinAttack(EnKaizoku* this) {
    EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_SPIN_ATTACK);
    this->swordCollider.base.atFlags &= ~(AT_BOUNCED | AT_HIT);
    this->spinAttackState = 0;
    this->action = KAIZOKU_ACTION_SPIN_ATTACK;
    this->actionFunc = EnKaizoku_SpinAttack;
    this->picto.actor.speed = 0.0f;
}

void EnKaizoku_SpinAttack(EnKaizoku* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame = this->skelAnime.curFrame;
    s16 yawDiff;
    s16 angleToPlayer;
    s32 pad;

    if (this->spinAttackState < 2) {
        if (this->swordCollider.base.atFlags & AT_BOUNCED) {
            this->swordCollider.base.atFlags &= ~(AT_BOUNCED | AT_HIT);
            this->spinAttackState = 1;
            this->skelAnime.playSpeed = 1.0f;
        }
        // in OOT, player-getting-caught, laughing, cutscene code was here
    }

    if (this->skelAnime.curFrame <= 8.0f) {
        this->picto.actor.shape.rot.y = this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;
    } else if (Animation_OnFrame(&this->skelAnime, 13.0f)) {
        Actor_SpawnFloorDustRing(play, &this->picto.actor, &this->leftFootPos, 3.0f, 2, 2.0f, 0, 0, false);
        Actor_SpawnFloorDustRing(play, &this->picto.actor, &this->rightFootPos, 3.0f, 2, 2.0f, 0, 0, false);
        this->swordState = 1;
        this->picto.actor.speed = 10.0f;
        Actor_PlaySfx(&this->picto.actor, NA_SE_EN_PIRATE_ATTACK);
    } else if (Animation_OnFrame(&this->skelAnime, 21.0f)) {
        this->picto.actor.speed = 0.0f;
    } else if (Animation_OnFrame(&this->skelAnime, 24.0f)) {
        this->swordState = -1;
    }

    this->animationsDisabled = false;
    if ((curFrame >= this->animEndFrame) && (this->spinAttackState < 2)) {
        if (!Actor_IsFacingPlayer(&this->picto.actor, 0x1554)) {
            EnKaizoku_SetupReady(this);
            this->combatTimer = Rand_ZeroOne() * 5.0f + 5.0f;
            this->lookTimer = 46;
            return;
        }

        if (this->spinAttackState != 0) {
            EnKaizoku_SetupRollBack(this);
            return;
        }

        if ((Rand_ZeroOne() > 0.7f) || (this->picto.actor.xzDistToPlayer >= 120.0f)) {
            EnKaizoku_SetupReady(this);
            this->combatTimer = Rand_ZeroOne() * 5.0f + 5.0f;
            return;
        }

        this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;
        if (BREG(12) * 0.1f + 0.01f * 40.0f < Rand_ZeroOne()) {
            EnKaizoku_SetupBlock(this); // in OOT this was sidestep
            return;
        }

        angleToPlayer = ABS_ALT(player->actor.shape.rot.y - this->picto.actor.shape.rot.y);
        if (angleToPlayer <= 0x2710) {
            yawDiff = ABS_ALT(this->picto.actor.yawTowardsPlayer - this->picto.actor.shape.rot.y);
            if (yawDiff > 0x4000) {
                this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;
                EnKaizoku_SetupCircle(this);
            } else {
                EnKaizoku_ReactToPlayer(this, play, true);
            }
        } else {
            EnKaizoku_SetupCircle(this);
        }
    }
}

void EnKaizoku_SetupCircle(EnKaizoku* this) {
    EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_SIDESTEP);
    this->picto.actor.speed = Rand_CenteredFloat(12.0f);
    this->skelAnime.playSpeed = 1.0f;
    this->picto.actor.world.rot.y = this->picto.actor.shape.rot.y;
    this->combatTimer = Rand_ZeroOne() * 30.0f + 30.0f;
    this->action = KAIZOKU_ACTION_CIRCLE;
    this->actionFunc = EnKaizoku_Circle;
    this->circlingRate = 0.0f;
}

void EnKaizoku_Circle(EnKaizoku* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 angleBehindPlayer;
    s32 beforeCurFrame;
    s32 afterCurFrame;
    s16 yaw;

    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 1, 0xFA0, 1);
    if (!EnKaizoku_DodgeRanged(this, play) && !EnKaizoku_ReactToPlayer(this, play, false)) {
        this->picto.actor.world.rot.y = this->picto.actor.shape.rot.y + 0x4000;
        angleBehindPlayer = player->actor.shape.rot.y + 0x8000;
        if (Math_SinS(angleBehindPlayer - this->picto.actor.shape.rot.y) >= 0.0f) {
            this->picto.actor.speed -= 0.25f;
            if (this->picto.actor.speed < -8.0f) {
                this->picto.actor.speed = -8.0f;
            }
        } else if (Math_SinS(angleBehindPlayer - this->picto.actor.shape.rot.y) < 0.0f) {
            this->picto.actor.speed += 0.25f;
            if (this->picto.actor.speed > 8.0f) {
                this->picto.actor.speed = 8.0f;
            }
        }

        if ((this->picto.actor.bgCheckFlags & BGCHECKFLAG_WALL) ||
            !Actor_TestFloorInDirection(&this->picto.actor, play, this->picto.actor.speed,
                                        this->picto.actor.shape.rot.y + 0x4000)) {

            if (this->picto.actor.bgCheckFlags & BGCHECKFLAG_WALL) {
                if (this->picto.actor.speed >= 0.0f) {
                    yaw = this->picto.actor.shape.rot.y + 0x4000;
                } else {
                    yaw = this->picto.actor.shape.rot.y - 0x4000;
                }
                yaw = this->picto.actor.wallYaw - yaw;
            } else {
                this->picto.actor.speed *= -0.8f;
                yaw = 0;
            }

            if (ABS_ALT(yaw) > 0x4000) {
                this->picto.actor.speed *= -0.8f;
                if (this->picto.actor.speed < 0.0f) {
                    this->picto.actor.speed -= 0.5f;
                } else {
                    this->picto.actor.speed += 0.5f;
                }
            }
        }

        if (this->picto.actor.xzDistToPlayer <= 65.0f) {
            Math_ApproachF(&this->circlingRate, -4.0f, 1.0f, 1.5f);
        } else if (this->picto.actor.xzDistToPlayer > 40.0f) {
            Math_ApproachF(&this->circlingRate, 4.0f, 1.0f, 1.5f);
        } else {
            Math_ApproachZeroF(&this->circlingRate, 1.0f, 6.65f);
        }

        if (this->circlingRate != 0.0f) {
            this->picto.actor.world.pos.x += Math_SinS(this->picto.actor.shape.rot.y) * this->circlingRate;
            this->picto.actor.world.pos.z += Math_CosS(this->picto.actor.shape.rot.y) * this->circlingRate;
        }

        beforeCurFrame = this->skelAnime.curFrame - this->skelAnime.playSpeed;
        afterCurFrame = this->skelAnime.curFrame + this->skelAnime.playSpeed;
        if (this->skelAnime.curFrame != 0.0f) {
            if ((((beforeCurFrame < 0) && (afterCurFrame > 0)) || ((beforeCurFrame < 5) && (afterCurFrame > 5)))) {
                Actor_PlaySfx(&this->picto.actor, NA_SE_EN_GERUDOFT_WALK);
            }
        }

        if (!(play->gameplayFrames & 0x5F)) {
            Actor_PlaySfx(&this->picto.actor, NA_SE_EN_PIRATE_BREATH);
        }

        if (this->picto.actor.xzDistToPlayer <= 65.0f) {
            EnKaizoku_SetupSlash(this);
        } else if (this->combatTimer == 0) {
            if (Actor_OtherIsTargeted(play, &this->picto.actor) && (Rand_ZeroOne() > 0.5f)) {
                EnKaizoku_SetupRollBack(this);
            } else {
                EnKaizoku_SetupReady(this);
            }
        }
    }
}

void EnKaizoku_SetupStunned(EnKaizoku* this) {
    if (this->picto.actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->picto.actor.speed = 0.0f;
    }

    if (this->action == KAIZOKU_ACTION_SPIN_ATTACK) {
        EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_DAMAGE);
    }

    if (((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
         (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) &&
        (this->drawDmgEffTimer == 0)) {
        this->drawDmgEffTimer = 0;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    }

    Actor_PlaySfx(&this->picto.actor, NA_SE_EN_COMMON_FREEZE);
    this->animationsDisabled = true;

    if ((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
        (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) {
        this->iceTimer = 40;
    }

    this->action = KAIZOKU_ACTION_STUNNED;
    this->actionFunc = EnKaizoku_Stunned;
}

void EnKaizoku_Stunned(EnKaizoku* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) {
        if ((this->drawDmgEffTimer != 0) && (this->drawDmgEffTimer < 60)) {
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
        }
    }

    if (this->picto.actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->picto.actor.speed = 0.0f;
    }

    if (this->picto.actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->picto.actor.speed < 0.0f) {
            this->picto.actor.speed += 0.05f;
        }
    }

    if ((this->iceTimer == 0) && (this->drawDmgEffTimer == 0) &&
        !CHECK_FLAG_ALL(this->picto.actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED) &&
        (this->picto.actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->animationsDisabled = false;
        EnKaizoku_ReactToPlayer(this, play, true);

        if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
            (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) {
            Actor_SpawnIceEffects(play, &this->picto.actor, this->bodyPartsPos, KAIZOKU_BODYPART_MAX, 2, 0.7f, 0.4f);
            this->drawDmgEffTimer = 0;
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
            this->picto.actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
        }
    }
}

void EnKaizoku_SetupDamaged(EnKaizoku* this, PlayState* play) {
    Vec3f velocity;

    Matrix_RotateYS(this->picto.actor.yawTowardsPlayer, MTXMODE_NEW);
    Matrix_MultVecZ(-10.0f, &velocity);
    Math_Vec3f_Copy(&this->velocity, &velocity);
    this->lookTimer = 0;
    this->animationsDisabled = false;
    this->picto.actor.speed = 0.0f;
    EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_DAMAGE);

    if (((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
         (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) &&
        (this->drawDmgEffTimer != 0)) {
        Actor_SpawnIceEffects(play, &this->picto.actor, this->bodyPartsPos, KAIZOKU_BODYPART_MAX, 2, 0.7f, 0.4f);
        this->drawDmgEffTimer = 0;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->picto.actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    }

    Actor_PlaySfx(&this->picto.actor, NA_SE_EN_PIRATE_DAMAGE);
    this->action = KAIZOKU_ACTION_DAMAGED;
    this->actionFunc = EnKaizoku_Damaged;
}

void EnKaizoku_Damaged(EnKaizoku* this, PlayState* play) {
    s16 angleToWall;

    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 1, 0x1194, 0);
    if (!EnKaizoku_DodgeRanged(this, play) && !EnKaizoku_ReactToPlayer(this, play, false) &&
        (this->velocity.x < 1.0f) && (this->velocity.z < 1.0f)) {
        angleToWall = this->picto.actor.wallYaw - this->picto.actor.shape.rot.y;
        if (this->picto.actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            if (ABS_ALT(angleToWall) < 0x3000 && (this->picto.actor.xzDistToPlayer < 90.0f)) {
                EnKaizoku_SetupJump(this);
                return;
            }
        }

        if ((this->picto.actor.xzDistToPlayer <= 65.0f) && ((play->gameplayFrames % 8) != 0)) {
            this->swordCollider.elem.elemMaterial = ELEM_MATERIAL_UNK2;
            EnKaizoku_SetupSlash(this);
        } else {
            EnKaizoku_SetupRollBack(this);
        }
    }
}

void EnKaizoku_SetupDefeatKnockdown(EnKaizoku* this, PlayState* play) {
    Vec3f sp24;

    this->picto.actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;
    Matrix_RotateYS(this->picto.actor.yawTowardsPlayer, MTXMODE_NEW);
    Matrix_MultVecZ(-10.0f, &sp24);
    Math_Vec3f_Copy(&this->velocity, &sp24);
    EnKaizoku_ChangeAnim(this, KAIZOKU_ANIM_DEFEAT);

    if (((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
         (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) &&
        (this->drawDmgEffTimer == 0)) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    }
    this->picto.actor.speed = 0.0f;
    this->animationsDisabled = true;
    Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_123);
    Enemy_StartFinishingBlow(play, &this->picto.actor);
    Actor_PlaySfx(&this->picto.actor, NA_SE_EN_PIRATE_DEAD);
    this->picto.actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->picto.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->picto.actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    this->cutsceneTimer = 0;
    this->cutsceneState = 0;
    this->action = KAIZOKU_ACTION_KNOCK_DOWN;
    this->actionFunc = EnKaizoku_DefeatKnockdown;
}

void EnKaizoku_DefeatKnockdown(EnKaizoku* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    Player* player;

    if (this->picto.actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->picto.actor.speed = 0.0f;
    }

    if (this->picto.actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Math_SmoothStepToF(&this->picto.actor.speed, 0.0f, 1.0f, 0.5f, 0.0f);
    }

    if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
        (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) {
        if (this->drawDmgEffTimer == 0) {
            return;
        }
        Actor_SpawnIceEffects(play, &this->picto.actor, this->bodyPartsPos, KAIZOKU_BODYPART_MAX, 2, 0.7f, 0.4f);
        this->drawDmgEffTimer = 0;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    }

    if (curFrame >= 25.0f) {
        player = GET_PLAYER(play);
        if (this->subCamId == SUB_CAM_ID_DONE) {
            if (!CutsceneManager_IsNext(this->csId)) {
                CutsceneManager_Queue(this->csId);
                return;
            }
            CutsceneManager_StartWithPlayerCs(this->csId, &this->picto.actor);
            this->subCamId = CutsceneManager_GetCurrentSubCamId(this->picto.actor.csId);
        }

        Math_Vec3f_Copy(&this->velocity, &gZeroVec3f);
        player->actor.world.pos.x = this->picto.actor.home.pos.x + 90.0f;
        player->actor.world.pos.z = this->picto.actor.home.pos.z + 30.0f;
        this->picto.actor.world.pos.x = this->picto.actor.home.pos.x;
        this->picto.actor.world.pos.z = this->picto.actor.home.pos.z;

        this->subCamEye.x = this->subCamEyeTarget.x = player->actor.world.pos.x + 39.0f;
        this->subCamEye.y = this->subCamEyeTarget.y = player->actor.world.pos.y + 4.0f;
        this->subCamEye.z = this->subCamEyeTarget.z = player->actor.world.pos.z - 41.0f;

        this->subCamAt.x = this->subCamAtTarget.x = player->actor.world.pos.x - 150.0f;
        this->subCamAt.y = this->subCamAtTarget.y = player->actor.world.pos.y + 60.0f;
        this->subCamAt.z = this->subCamAtTarget.z = player->actor.world.pos.z + 50.0f;

        player->actor.shape.rot.y = player->actor.world.rot.y =
            Math_Vec3f_Yaw(&player->actor.world.pos, &this->picto.actor.world.pos);

        if (this->subCamId != SUB_CAM_ID_DONE) {
            this->subCamUp.x = 0.0f;
            this->subCamUp.y = 1.0f;
            this->subCamUp.z = 0.0f;
            Play_SetCameraAtEyeUp(play, this->subCamId, &this->subCamAt, &this->subCamEye, &this->subCamUp);
        }
    }

    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 1, 0xFA0, 1);
    SkelAnime_Update(&this->skelAnime);
    if (curFrame >= this->animEndFrame) {
        this->animationsDisabled = false;
        EnKaizoku_SetupPlayerWinCutscene(this);
    } else if (Animation_OnFrame(&this->skelAnime, 10.0f)) {
        Actor_PlaySfx(&this->picto.actor, NA_SE_EN_GERUDOFT_DOWN);
    }
}

// Handle damage to player and from player
void EnKaizoku_UpdateDamage(EnKaizoku* this, PlayState* play) {
    s32 wasHit = false;

    if (gSaveContext.save.saveInfo.playerData.health <= 0x10) {
        this->swordCollider.elem.atDmgInfo.damage = 0;
    } else {
        this->swordCollider.elem.atDmgInfo.damage = 4;
    }

    if (!(this->swordCollider.base.atFlags & AT_BOUNCED) && (this->swordCollider.base.atFlags & AT_HIT)) {
        if ((gSaveContext.save.saveInfo.playerData.health <= 0x10) && (this->action != KAIZOKU_ACTION_SCENE_FADE)) {
            this->spinAttackState = 2;
            this->subCamId = SUB_CAM_ID_DONE;
            this->picto.actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;

            if (!CutsceneManager_IsNext(this->csId)) {
                CutsceneManager_Queue(this->csId);
                this->actionFunc = EnKaizoku_SetupPlayerLoss;
            } else {
                CutsceneManager_StartWithPlayerCs(this->csId, &this->picto.actor);
                this->subCamId = CutsceneManager_GetCurrentSubCamId(this->picto.actor.csId);
                this->actionFunc = EnKaizoku_SetupPlayerLoss;
            }
            return;
        } else if ((this->action == KAIZOKU_ACTION_SPIN_ATTACK) &&
                   (this->swordCollider.base.at == &GET_PLAYER(play)->actor)) {
            func_800B8D98(play, &this->picto.actor, 3.0f, this->picto.actor.yawTowardsPlayer, 1.0f);
            Health_ChangeBy(play, -0xC);
            if ((gSaveContext.save.saveInfo.playerData.health <= 0x10) && (this->action != KAIZOKU_ACTION_SCENE_FADE)) {
                Health_ChangeBy(play, 0x10);
                this->spinAttackState = 2;
                this->subCamId = SUB_CAM_ID_DONE;
                this->picto.actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;

                if (!CutsceneManager_IsNext(this->csId)) {
                    CutsceneManager_Queue(this->csId);
                    this->actionFunc = EnKaizoku_SetupPlayerLoss;
                } else {
                    CutsceneManager_StartWithPlayerCs(this->csId, &this->picto.actor);
                    this->subCamId = CutsceneManager_GetCurrentSubCamId(this->picto.actor.csId);
                    this->actionFunc = EnKaizoku_SetupPlayerLoss;
                }
                return;
            }
        }

        // Unused: all three rooms with all three of them only contain themselves and 2 torches for actors
        // they are never in a room with a barrel or a wooden plank
        //   did they plan a special fight mechanic?
        if (this->swordCollider.base.at->id == ACTOR_OBJ_TARU) {
            this->swordCollider.base.at->home.rot.z = 1;
        }
    }

    if ((this->picto.actor.bgCheckFlags & BGCHECKFLAG_WALL) && (this->picto.actor.wallBgId != BG_ACTOR_MAX) &&
        ((this->action == KAIZOKU_ACTION_SPIN_DODGE) || (this->action == KAIZOKU_ACTION_CIRCLE) ||
         (this->action == KAIZOKU_ACTION_ADVANCE) || (this->action == KAIZOKU_ACTION_READY))) {
        EnKaizoku_SetupSlash(this);
    }

    if ((this->bodyCollider.base.acFlags & AC_HIT) && (this->action > KAIZOKU_ACTION_HIDDEN) &&
        (this->spinAttackState < 2) && (this->action != KAIZOKU_ACTION_JUMP) &&
        (this->action != KAIZOKU_ACTION_BLOCK) && (this->action != KAIZOKU_ACTION_DAMAGED) &&
        (this->action != KAIZOKU_ACTION_KNOCK_DOWN)) {
        Actor_SetDropFlag(&this->picto.actor, &this->bodyCollider.elem);
        AudioSfx_StopByPosAndId(&this->picto.actor.projectedPos, NA_SE_EN_PIRATE_BREATH);

        switch (this->picto.actor.colChkInfo.damageEffect) {
            case KAIZOKU_DMGEFF_ZORA_SHIELD:
                if (((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
                     (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) ||
                    (this->drawDmgEffTimer == 0)) {
                    this->drawDmgEffTimer = 40;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_MEDIUM;
                }
                FALLTHROUGH;
            case KAIZOKU_DMGEFF_STUN:
                if (((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
                     (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) ||
                    this->drawDmgEffTimer == 0) {
                    Actor_SetColorFilter(&this->picto.actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA,
                                         40);
                    this->bodyCollider.elem.elemMaterial = ELEM_MATERIAL_UNK1;
                    this->bodyCollider.base.colMaterial = COL_MATERIAL_HIT3;
                    this->swordCollider.elem.elemMaterial = ELEM_MATERIAL_UNK2;
                    EnKaizoku_SetupStunned(this);
                }
                break;

            case KAIZOKU_DMGEFF_IFRAME_PROTECTED:
                if ((this->action != KAIZOKU_ACTION_ROLL_FORWARD) && (this->action != KAIZOKU_ACTION_ROLL_BACK) &&
                    (this->action < KAIZOKU_ACTION_DAMAGED)) {
                    wasHit = true;
                }
                break;

            case KAIZOKU_DMGEFF_ALWAYS_HIT:
                wasHit = true;
                break;

            case KAIZOKU_DMGEFF_STUNNED_ONLY:
                if (this->action == KAIZOKU_ACTION_STUNNED) {
                    wasHit = true;
                } else if (this->action != KAIZOKU_ACTION_JUMP) {
                    EnKaizoku_SetupJump(this);
                }
                break;

            case KAIZOKU_DMGEFF_FIRE_ARROW:
                if ((this->action != KAIZOKU_ACTION_ROLL_FORWARD) && (this->action != KAIZOKU_ACTION_ROLL_BACK) &&
                    (this->action < KAIZOKU_ACTION_DAMAGED)) {
                    this->drawDmgEffTimer = 40;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                    wasHit = true;
                }
                break;

            case KAIZOKU_DMGEFF_FREEZE:
                if (((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
                     (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) ||
                    (this->drawDmgEffTimer == 0)) {
                    Actor_ApplyDamage(&this->picto.actor);
                    this->bodyCollider.elem.elemMaterial = ELEM_MATERIAL_UNK1;
                    this->bodyCollider.base.colMaterial = COL_MATERIAL_HIT3;
                    this->swordCollider.elem.elemMaterial = ELEM_MATERIAL_UNK4;
                    this->drawDmgEffTimer = 80;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_SFX;
                    this->drawDmgEffScale = 0.0f;
                    this->drawDmgEffFrozenSteamScale = 1.5f;
                    this->picto.actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
                    if (this->picto.actor.colChkInfo.health <= 0) {
                        EnKaizoku_SetupDefeatKnockdown(this, play);
                    } else {
                        EnKaizoku_SetupStunned(this);
                    }
                }
                break;

            case KAIZOKU_DMGEFF_LIGHT_ARROW:
                if (((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
                     (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) ||
                    (this->drawDmgEffTimer == 0)) {
                    this->drawDmgEffTimer = 20;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->picto.actor.focus.pos.x,
                                this->picto.actor.focus.pos.y, this->picto.actor.focus.pos.z, 0, 0, 0,
                                CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                    wasHit = true;
                }
                break;

            default:
                break;
        }

        if (wasHit) {
            Vec3f bloodPos;
            s32 i;

            Actor_SetColorFilter(&this->picto.actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
            Actor_ApplyDamage(&this->picto.actor);
            if (this->picto.actor.colChkInfo.health <= 0) {
                EnKaizoku_SetupDefeatKnockdown(this, play);
                return;
            }

            this->bodyCollider.elem.elemMaterial = ELEM_MATERIAL_UNK1;
            this->bodyCollider.base.colMaterial = COL_MATERIAL_HIT3;
            this->swordCollider.elem.elemMaterial = ELEM_MATERIAL_UNK4;
            Math_Vec3f_Copy(&bloodPos, &this->picto.actor.focus.pos);
            for (i = 0; i < 3; i++) {
                bloodPos.y += Rand_ZeroFloat(20.0f);
                CollisionCheck_BlueBlood(play, NULL, &bloodPos);
            }

            EnKaizoku_SetupDamaged(this, play);
        }
    } else if (this->bodyCollider.base.acFlags & AC_HIT) {
        if ((this->action != KAIZOKU_ACTION_JUMP) && (this->action != KAIZOKU_ACTION_DAMAGED) &&
            (this->action != KAIZOKU_ACTION_KNOCK_DOWN)) {
            Vec3f pos;
            Player* player = GET_PLAYER(play);

            pos.x = this->bodyCollider.elem.acDmgInfo.hitPos.x;
            pos.y = this->bodyCollider.elem.acDmgInfo.hitPos.y;
            pos.z = this->bodyCollider.elem.acDmgInfo.hitPos.z;

            if (player->transformation != PLAYER_FORM_HUMAN) {
                player->pushedYaw = this->picto.actor.yawTowardsPlayer;
                player->pushedSpeed = 15.0f;
            }

            this->bodyCollider.base.acFlags &= ~AC_HIT;
            Actor_PlaySfx(&this->picto.actor, NA_SE_IT_SHIELD_BOUND);
            EffectSsHitmark_SpawnFixedScale(play, EFFECT_HITMARK_METAL, &pos);
            CollisionCheck_SpawnShieldParticlesMetal(play, &pos);
        }
    }
}

static Vec3f sFootOffset = { 300.0f, 0.0f, 0.0f };

static Vec3f sSwordTipOffset = { 0.0f, -3000.0f, 0.0f };
static Vec3f sSwordHiltOffset = { 400.0f, 0.0f, 0.0f };

static Vec3f sSwordQuadOffset1 = { 1600.0f, -4000.0f, 0.0f };
static Vec3f sSwordQuadOffset0 = { -3000.0f, -2000.0f, 1300.0f };
static Vec3f sSwordQuadOffset3 = { -3000.0f, -2000.0f, -1300.0f };
static Vec3f sSwordQuadOffset2 = { 1000.0f, 1000.0f, 0.0f };

static TexturePtr sEyeTextures[] = {
    gFighterPirateEyeOpenTex,
    gFighterPirateEyeHalfTex,
    gFighterPirateEyeClosedTex,
    gFighterPirateEyeHalfTex,
};

void EnKaizoku_Update(Actor* thisx, PlayState* play2) {
    EnKaizoku* this = (EnKaizoku*)thisx;
    PlayState* play = play2;

    if (!this->animationsDisabled) {
        SkelAnime_Update(&this->skelAnime);
    }

    if (this->action != KAIZOKU_ACTION_HIDDEN) {
        EnKaizoku_TurnHead(this);
    }

    DECR(this->blinkTimer);
    DECR(this->combatTimer);
    DECR(this->lookTimer);
    DECR(this->iceTimer);
    DECR(this->cutsceneTimer);
    DECR(this->drawDmgEffTimer);

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->picto.actor);
    if (this->action != KAIZOKU_ACTION_HIDDEN) {
        EnKaizoku_UpdateDamage(this, play);
    }
    if (this->action != KAIZOKU_ACTION_HIDDEN) {
        Actor_SetFocus(&this->picto.actor, 60.0f);
    }

    Actor_SetScale(&this->picto.actor, 0.0125f);
    if (this->blinkTimer == 0) {
        this->eyeIndex++;
        if (this->eyeIndex >= ARRAY_COUNT(sEyeTextures)) {
            this->eyeIndex = 0;
            this->blinkTimer = Rand_S16Offset(20, 60);
        }
    }

    if (this->picto.actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Vec3f bounceVelocity;
        s32 pad;

        Matrix_RotateYS(this->picto.actor.shape.rot.y + this->boyoBounceAngle, MTXMODE_NEW);
        Matrix_MultVecZ(this->boyoBounceVelocity, &bounceVelocity);
        this->picto.actor.world.pos.x += this->velocity.x + bounceVelocity.x;
        this->picto.actor.world.pos.z += this->velocity.z + bounceVelocity.z;
        Math_ApproachZeroF(&this->velocity.x, 1.0f, 2.0f);
        Math_ApproachZeroF(&this->velocity.z, 1.0f, 2.0f);
        Math_ApproachZeroF(&this->boyoBounceVelocity, 1.0f, 5.0f);
    }

    Actor_UpdateBgCheckInfo(play, &this->picto.actor, 35.0f, 40.0f, 35.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
    Collider_UpdateCylinder(&this->picto.actor, &this->bodyCollider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);
    if ((this->spinAttackState < 2) && (this->action != KAIZOKU_ACTION_HIDDEN)) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyCollider.base);
    }
    if (this->swordState > 0) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->swordCollider.base);
    }
}

s32 EnKaizoku_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnKaizoku* this = (EnKaizoku*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    // disabled because left over from OOT, and dev wanted us to see lipstick
    if (limbIndex == KAIZOKU_LIMB_VEIL) {
        *dList = NULL;
    }

    if (limbIndex == KAIZOKU_LIMB_NECK) {
        rot->z += this->headRot.x;
        rot->x += this->headRot.y;
        rot->y += this->headRot.z;
    } else if (limbIndex == KAIZOKU_LIMB_HEAD) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, sKaizokuLipstickColors[this->colorType].r,
                       sKaizokuLipstickColors[this->colorType].g, sKaizokuLipstickColors[this->colorType].b, 255);
    } else {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, sKaizokuOutfitColors[this->colorType].r,
                       sKaizokuOutfitColors[this->colorType].g, sKaizokuOutfitColors[this->colorType].b, 255);
    }

    CLOSE_DISPS(play->state.gfxCtx);

    return false;
}

void EnKaizoku_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    Vec3f swordTip;
    Vec3f swordHilt;
    EnKaizoku* this = (EnKaizoku*)thisx;

    if (limbIndex == KAIZOKU_LIMB_RIGHT_SWORD) {
        Matrix_MultVec3f(&sSwordQuadOffset1, &this->swordCollider.dim.quad[1]);
        Matrix_MultVec3f(&sSwordQuadOffset0, &this->swordCollider.dim.quad[0]);
        Matrix_MultVec3f(&sSwordQuadOffset3, &this->swordCollider.dim.quad[3]);
        Matrix_MultVec3f(&sSwordQuadOffset2, &this->swordCollider.dim.quad[2]);
        Collider_SetQuadVertices(&this->swordCollider, &this->swordCollider.dim.quad[0],
                                 &this->swordCollider.dim.quad[1], &this->swordCollider.dim.quad[2],
                                 &this->swordCollider.dim.quad[3]);
        Matrix_MultVec3f(&sSwordTipOffset, &swordTip);
        Matrix_MultVec3f(&sSwordHiltOffset, &swordHilt);

        if ((this->swordState == 1) &&
            ((this->action == KAIZOKU_ACTION_SLASH) || (this->action == KAIZOKU_ACTION_SPIN_ATTACK))) {
            EffectBlure_AddVertex(Effect_GetByIndex(this->blureIndex), &swordTip, &swordHilt);
        } else if (this->swordState != 0) {
            EffectBlure_AddSpace(Effect_GetByIndex(this->blureIndex));
            this->swordState = 0;
        }
    } else {
        Actor_SetFeetPos(&this->picto.actor, limbIndex, KAIZOKU_LIMB_LEFT_FOOT, &sFootOffset, KAIZOKU_LIMB_RIGHT_FOOT,
                         &sFootOffset);
    }

    if (limbIndex == KAIZOKU_LIMB_LEFT_FOOT) {
        Matrix_MultVec3f(&sFootOffset, &this->leftFootPos);
    } else if (limbIndex == KAIZOKU_LIMB_RIGHT_FOOT) {
        Matrix_MultVec3f(&sFootOffset, &this->rightFootPos);
    }

    if ((limbIndex == KAIZOKU_LIMB_TORSO) || (limbIndex == KAIZOKU_LIMB_HEAD) ||
        (limbIndex == KAIZOKU_LIMB_RIGHT_UPPER_ARM) || (limbIndex == KAIZOKU_LIMB_RIGHT_FOREARM) ||
        (limbIndex == KAIZOKU_LIMB_RIGHT_HAND) || (limbIndex == KAIZOKU_LIMB_LEFT_UPPER_ARM) ||
        (limbIndex == KAIZOKU_LIMB_LEFT_FOREARM) || (limbIndex == KAIZOKU_LIMB_LEFT_HAND) ||
        (limbIndex == KAIZOKU_LIMB_LEFT_THIGH) || (limbIndex == KAIZOKU_LIMB_LEFT_SHIN) ||
        (limbIndex == KAIZOKU_LIMB_LEFT_FOOT) || (limbIndex == KAIZOKU_LIMB_RIGHT_THIGH) ||
        (limbIndex == KAIZOKU_LIMB_RIGHT_SHIN) || (limbIndex == KAIZOKU_LIMB_RIGHT_FOOT) ||
        (limbIndex == KAIZOKU_LIMB_WAIST)) {

        Matrix_MultZero(&this->bodyPartsPos[this->bodyPartIndex]);
        this->bodyPartIndex++;
        if (this->bodyPartIndex >= KAIZOKU_BODYPART_MAX) {
            this->bodyPartIndex = 0;
        }
    }
}

void EnKaizoku_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    EnKaizoku* this = (EnKaizoku*)thisx;

    if (limbIndex == KAIZOKU_LIMB_RIGHT_SWORD) {
        Matrix_Scale(this->swordScaleRight.x, this->swordScaleRight.y, this->swordScaleRight.z, MTXMODE_APPLY);
    }
    if (limbIndex == KAIZOKU_LIMB_LEFT_SWORD) {
        Matrix_Scale(this->swordScaleLeft.x, this->swordScaleLeft.y, this->swordScaleLeft.z, MTXMODE_APPLY);
    }
}

void EnKaizoku_Draw(Actor* thisx, PlayState* play) {
    f32 pad[4];
    f32 drawDmgEffAlpha;
    EnKaizoku* this = (EnKaizoku*)thisx;

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    OPEN_DISPS(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_K0(sEyeTextures[this->eyeIndex]));
    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, EnKaizoku_OverrideLimbDraw, EnKaizoku_PostLimbDraw,
                                   EnKaizoku_TransformLimbDraw, &this->picto.actor);

    if (this->drawDmgEffTimer != 0) {
        drawDmgEffAlpha = this->drawDmgEffTimer * 0.05f;

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

        Actor_DrawDamageEffects(play, &this->picto.actor, this->bodyPartsPos, KAIZOKU_BODYPART_MAX,
                                this->drawDmgEffScale, this->drawDmgEffFrozenSteamScale, drawDmgEffAlpha,
                                this->drawDmgEffType);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
