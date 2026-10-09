/*
 * File: z_en_bal.c
 * Overlay: ovl_En_Bal
 * Description: Tingle with Balloon
 */

#include "z_en_bal.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnBal_Init(Actor* thisx, PlayState* play);
void EnBal_Destroy(Actor* thisx, PlayState* play);
void EnBal_Update(Actor* thisx, PlayState* play);
void EnBal_Draw(Actor* thisx, PlayState* play);

typedef enum {
    /* 0 */ TINGLE_EYETEX_OPEN,
    /* 1 */ TINGLE_EYETEX_CLOSED
} TingleEyeTexture;

typedef enum {
    /* -1 */ TINGLE_ANIM_NONE = -1,
    /*  0 */ TINGLE_ANIM_FLOAT_IDLE,
    /*  1 */ TINGLE_ANIM_FALL_LOOP,
    /*  2 */ TINGLE_ANIM_FALL_ONCE,
    /*  3 */ TINGLE_ANIM_LAND,
    /*  4 */ TINGLE_ANIM_TWIST,
    /*  5 */ TINGLE_ANIM_TALK,
    /*  6 */ TINGLE_ANIM_MAGIC,
    /*  7 */ TINGLE_ANIM_HAPPY_DANCE_LOOP,
    /*  8 */ TINGLE_ANIM_HAPPY_DANCE_ONCE,
    /*  9 */ TINGLE_ANIM_MAGIC_REVERSE,
    /* 10 */ TINGLE_ANIM_IDLE,
    /* 11 */ TINGLE_ANIM_SPIN,
    /* 12 */ TINGLE_ANIM_HIDE_FACE,
    /* 13 */ TINGLE_ANIM_CONFETTI,
    /* 14 */ TINGLE_ANIM_MAX
} TingleAnimation;

typedef enum {
    /* 0 */ TINGLE_MAPCHOICE_PROXIMAL,
    /* 1 */ TINGLE_MAPCHOICE_DISTAL,
    /* 2 */ TINGLE_MAPCHOICE_CANCEL
} TingleBuyMapChoice;

typedef enum {
    /* 0 */ TINGLE_WATCH_TARGET_NONE,
    /* 1 */ TINGLE_WATCH_TARGET_PLAYER,
    /* 2 */ TINGLE_WATCH_TARGET_FAIRY
} TingleWatchTarget;

typedef enum {
    /* 0 */ TINGLE_BALLOON_ACTION_NONE,
    /* 1 */ TINGLE_BALLOON_ACTION_POP,
    /* 2 */ TINGLE_BALLOON_ACTION_FALL,
    /* 4 */ TINGLE_BALLOON_ACTION_INFLATE = 4,
    /* 5 */ TINGLE_BALLOON_ACTION_RISE
} TingleBalloonAction;

typedef enum {
    /* 0 */ TINGLE_IDLESTAGE_ACTIVITY,
    /* 2 */ TINGLE_IDLESTAGE_PREP_WAIT = 2,
    /* 3 */ TINGLE_IDLESTAGE_WAIT
} TingleIdleAnimStage;

void EnBal_SetMainColliderToHead(EnBal* this);
s32 EnBal_ValidatePictograph(PlayState* play, Actor* thisx);
void EnBal_SetupFloatIdle(EnBal* this);
void EnBal_FloatIdle(EnBal* this, PlayState* play);
void EnBal_PopBalloon(EnBal* this, PlayState* play);
void EnBal_SetupFall(EnBal* this);
void EnBal_Fall(EnBal* this, PlayState* play);
void EnBal_InflateBalloon(EnBal* this, PlayState* play);
void EnBal_SetupFloatUp(EnBal* this);
void EnBal_FloatUp(EnBal* this, PlayState* play);
void EnBal_SetupGroundIdle(EnBal* this);
void EnBal_GroundIdle(EnBal* this, PlayState* play);
void EnBal_SetupTalk(EnBal* this);
void EnBal_Talk(EnBal* this, PlayState* play);
PlayerTransformation EnBal_GetRecognizedPlayerForm(void);
void EnBal_ThrowMagicSparkles(EnBal* this, PlayState* play);
void EnBal_EmitDustPuff(EnBal* this, PlayState* play);
void EnBal_TryPurchaseMap(EnBal* this, PlayState* play);
void EnBal_HandleConversation(EnBal* this, PlayState* play);
void EnBal_SetupOfferGetItem(EnBal* this);
void EnBal_OfferGetItem(EnBal* this, PlayState* play);
void EnBal_SetupThankYou(EnBal* this);
void EnBal_ThankYou(EnBal* this, PlayState* play);

ActorProfile En_Bal_Profile = {
    /**/ ACTOR_EN_BAL,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_BAL,
    /**/ sizeof(EnBal),
    /**/ EnBal_Init,
    /**/ EnBal_Destroy,
    /**/ EnBal_Update,
    /**/ EnBal_Draw,
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(0, 0x0),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(0, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0xE),
    /* Normal arrow   */ DMG_ENTRY(1, 0xE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, 0xF),
    /* Hookshot       */ DMG_ENTRY(1, 0xF),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(1, 0x0),
    /* Fire arrow     */ DMG_ENTRY(2, 0x2),
    /* Ice arrow      */ DMG_ENTRY(2, 0x3),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(1, 0xF),
    /* Deku spin      */ DMG_ENTRY(0, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0xE),
    /* Deku launch    */ DMG_ENTRY(2, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0xF),
    /* Zora punch     */ DMG_ENTRY(1, 0xF),
    /* Spin attack    */ DMG_ENTRY(1, 0xF),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(0, 0x0),
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 1, 0, 0, 0, MASS_IMMOVABLE };

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { TINGLE_LIMB_BALLOON, { { 2400, 0, 0 }, 50 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static s16 sBuyMapOptions[TINGLE_MAP_MAX][2] = {
    { TINGLE_MAP_CLOCK_TOWN, TINGLE_MAP_WOODFALL },    // TINGLE_MAP_CLOCK_TOWN
    { TINGLE_MAP_WOODFALL, TINGLE_MAP_SNOWHEAD },      // TINGLE_MAP_WOODFALL
    { TINGLE_MAP_SNOWHEAD, TINGLE_MAP_ROMANI_RANCH },  // TINGLE_MAP_SNOWHEAD
    { TINGLE_MAP_ROMANI_RANCH, TINGLE_MAP_GREAT_BAY }, // TINGLE_MAP_ROMANI_RANCH
    { TINGLE_MAP_GREAT_BAY, TINGLE_MAP_STONE_TOWER },  // TINGLE_MAP_GREAT_BAY
    { TINGLE_MAP_STONE_TOWER, TINGLE_MAP_CLOCK_TOWN }, // TINGLE_MAP_STONE_TOWER
};

static AnimationInfo sAnimationInfo[TINGLE_ANIM_MAX] = {
    { &gTingleFloatIdleAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },          // TINGLE_ANIM_FLOAT_IDLE
    { &gTingleFallAnim, 1.5f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },               // TINGLE_ANIM_FALL_LOOP
    { &gTingleFallAnim, 1.5f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0f },               // TINGLE_ANIM_FALL_ONCE
    { &gTingleLandAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0f },               // TINGLE_ANIM_LAND
    { &gTingleTwistAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },              // TINGLE_ANIM_TWIST
    { &gTingleTalkAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },               // TINGLE_ANIM_TALK
    { &gTingleThrowConfettiAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -8.0f },      // TINGLE_ANIM_MAGIC
    { &gTingleHappyDanceAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },         // TINGLE_ANIM_HAPPY_DANCE_LOOP
    { &gTingleHappyDanceAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -8.0f },         // TINGLE_ANIM_HAPPY_DANCE_ONCE
    { &gTingleThrowConfettiAnim, 1.0f, 23.0f, 0.0f, ANIMMODE_ONCE, -8.0f },     // TINGLE_ANIM_MAGIC_REVERSE
    { &gTingleIdleAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },               // TINGLE_ANIM_IDLE
    { &gTingleSpinAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -2.0f },               // TINGLE_ANIM_SPIN
    { &gTingleFloatHideFaceAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -2.0f },      // TINGLE_ANIM_HIDE_FACE
    { &gTingleFloatThrowConfettiAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -2.0f }, // TINGLE_ANIM_CONFETTI
};

void EnBal_Init(Actor* thisx, PlayState* play) {
    EnBal* this = (EnBal*)thisx;
    s32 pad;
    f32 endFrame = Animation_GetLastFrame(&gTingleFloatIdleAnim);

    this->locationMapId = TINGLE_GET_MAP_ID(&this->picto.actor);
    this->picto.actor.attentionRangeType = 1;
    this->picto.actor.cullingVolumeDistance = 3000.0f;
    Actor_SetScale(&this->picto.actor, 0.02f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gTingleSkel, &gTingleFloatIdleAnim, this->jointTable, this->morphTable,
                       TINGLE_LIMB_MAX);
    if (GameInteractor_Should(VB_HAVE_MAGIC_FOR_TINGLE, gSaveContext.save.saveInfo.playerData.isMagicAcquired)) {
        Animation_Change(&this->skelAnime, &gTingleTalkAnim, 1.0f, 0.0f, endFrame, ANIMMODE_LOOP, -10.0f);
    }
    ActorShape_Init(&this->picto.actor.shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
    this->skyFloatPhase = 0;
    this->textId = 0;
    this->balloonAction = TINGLE_BALLOON_ACTION_NONE;
    this->timer = 0;
    this->watchTarget = TINGLE_WATCH_TARGET_NONE;
    this->inflateEarly = false;
    this->isTalking = false;
    this->eyeTexIndex = TINGLE_EYETEX_OPEN;
    this->eyeTimer = 60;
    this->forceEyesShut = false;
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->picto.actor, &sJntSphInit, this->colliderElements);
    CollisionCheck_SetInfo2(&this->picto.actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    Actor_UpdateBgCheckInfo(play, &this->picto.actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    this->picto.validationFunc = EnBal_ValidatePictograph;
    if (!GameInteractor_Should(VB_HAVE_MAGIC_FOR_TINGLE, gSaveContext.save.saveInfo.playerData.isMagicAcquired)) {
        this->picto.actor.world.pos.y = this->picto.actor.floorHeight;
        EnBal_SetMainColliderToHead(this);
        EnBal_SetupGroundIdle(this);
    } else {
        EnBal_SetupFloatIdle(this);
    }
}

void EnBal_Destroy(Actor* thisx, PlayState* play) {
    EnBal* this = (EnBal*)thisx;

    Collider_InitJntSph(play, &this->collider);
}

void EnBal_SetMainColliderToBalloon(EnBal* this) {
    this->collider.elements[0].dim.limb = TINGLE_LIMB_BALLOON;
    this->collider.elements[0].dim.modelSphere.radius = 40;
    this->collider.elements[0].dim.modelSphere.center.x = 2200;
}

void EnBal_SetMainColliderToHead(EnBal* this) {
    this->collider.elements[0].dim.limb = TINGLE_LIMB_HEAD;
    this->collider.elements[0].dim.modelSphere.radius = 25;
    this->collider.elements[0].dim.modelSphere.center.x = 0;
}

s32 EnBal_ValidatePictograph(PlayState* play, Actor* thisx) {
    s32 pictoValid;
    EnBal* this = (EnBal*)thisx;

    pictoValid = Snap_ValidatePictograph(play, &this->picto.actor, PICTO_VALID_TINGLE, &this->picto.actor.focus.pos,
                                         &this->picto.actor.shape.rot, 10.0f, 400.0f, 0x4000);
    if (!pictoValid) {
        this->forceEyesShut = true;
        this->idleAnimStage = TINGLE_IDLESTAGE_WAIT;
    }
    return pictoValid;
}

void EnBal_SetupFloatIdle(EnBal* this) {
    this->timer = 0;
    EnBal_SetMainColliderToBalloon(this);
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_FLOAT_IDLE);

    this->picto.actor.focus.pos = this->picto.actor.world.pos;
    this->picto.actor.focus.pos.y = this->picto.actor.world.pos.y + 100.0f;

    this->actionFunc = EnBal_FloatIdle;
}

void EnBal_FloatIdle(EnBal* this, PlayState* play) {
    f32 scaleFactor;

    this->skyFloatPhase += 0x320;
    this->picto.actor.focus.pos.y = this->picto.actor.world.pos.y + 100.0f;
    this->timer += 1000;

    scaleFactor = (Math_CosS(this->skyFloatPhase) * 0.1f) + 1.0f;
    this->balloonScale.z = scaleFactor;
    this->balloonScale.y = scaleFactor;

    scaleFactor = (Math_SinS(this->skyFloatPhase) * 0.1f) + 1.0f;
    this->balloonScale.x = SQ(scaleFactor);

    this->picto.actor.world.pos.y = (Math_SinS(this->skyFloatPhase) * 50.0f) + this->picto.actor.home.pos.y;
    if (this->forceEyesShut == true) {
        if (this->skelAnime.animation != &gTingleFloatHideFaceAnim) {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_HIDE_FACE);
        } else if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            if (this->idleAnimStage != TINGLE_IDLESTAGE_ACTIVITY) {
                this->idleAnimStage--;
            } else {
                this->forceEyesShut = false;
                this->eyeTexIndex = TINGLE_EYETEX_OPEN;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_FLOAT_IDLE);
            }
        }
    }
}

void EnBal_SetupPopBalloon(EnBal* this) {
    EnBal_SetMainColliderToHead(this);
    this->timer = 0;
    Actor_PlaySfx(&this->picto.actor, NA_SE_EV_MUJURA_BALLOON_BROKEN);
    this->balloonAction = TINGLE_BALLOON_ACTION_POP;
    this->actionFunc = EnBal_PopBalloon;
}

void EnBal_PopBalloon(EnBal* this, PlayState* play) {
    this->picto.actor.focus.pos.x = this->picto.actor.world.pos.x;
    this->picto.actor.focus.pos.y = this->picto.actor.world.pos.y + 20.0f;
    this->picto.actor.focus.pos.z = this->picto.actor.world.pos.z + 30.0f;
    if (this->picto.actor.csId != CS_ID_NONE) {
        if (CutsceneManager_IsNext(this->picto.actor.csId)) {
            CutsceneManager_StartWithPlayerCs(this->picto.actor.csId, &this->picto.actor);
            Camera_SetFocalActor(Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(this->picto.actor.csId)),
                                 &this->picto.actor);
        } else {
            CutsceneManager_Queue(this->picto.actor.csId);
        }
    }

    if (this->timer > 30) {
        this->timer = 0;
        EnBal_SetupFall(this);
    } else if (this->timer > 10) {
        // Freeze animation and induce blink
        this->skelAnime.playSpeed = 0.0f;
        if (this->eyeTimer > 6) {
            this->eyeTimer = 6;
        }
        this->timer++;
    } else {
        this->timer++;
    }
}

void EnBal_SetupFall(EnBal* this) {
    Actor_PlaySfx(&this->picto.actor, NA_SE_VO_TIVO00);
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_FALL_LOOP);
    this->forceEyesShut = true;
    this->balloonAction = TINGLE_BALLOON_ACTION_FALL;
    this->actionFunc = EnBal_Fall;
}

void EnBal_Fall(EnBal* this, PlayState* play) {
    Vec3f worldPos = this->picto.actor.world.pos;

    this->picto.actor.focus.pos.x = this->picto.actor.world.pos.x;
    this->picto.actor.focus.pos.y = this->picto.actor.world.pos.y + 20.0f;
    this->picto.actor.focus.pos.z = this->picto.actor.world.pos.z + 30.0f;
    if (this->timer == 30) {
        this->picto.actor.gravity = -1.5f;
        Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.world.rot.y, 5, 0x3000, 0x100);
        Math_SmoothStepToS(&this->picto.actor.shape.rot.z, this->picto.actor.world.rot.z, 5, 0x3000, 0x100);
    } else if (this->timer < 30) {
        this->timer++;
        this->picto.actor.shape.rot.y = (s32)(Math_CosS(this->skyFloatPhase) * 2500.0f) + this->picto.actor.world.rot.y;
        this->picto.actor.shape.rot.z = (s32)(Math_SinS(this->skyFloatPhase) * 2500.0f) + this->picto.actor.world.rot.z;
        if (this->timer == 25) {
            this->forceEyesShut = false;
            this->eyeTexIndex = TINGLE_EYETEX_OPEN;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_FALL_ONCE);
        } else if (this->timer == 29) {
            this->picto.actor.velocity.y = -12.0f;
        }
    }

    if (this->picto.actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        Actor_PlaySfx(&this->picto.actor, NA_SE_VO_TIVO01);
        Actor_PlaySfx(&this->picto.actor, NA_SE_EN_GERUDOFT_DOWN);
    }

    if (this->picto.actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->picto.actor.colChkInfo.health = 0;
        if (this->timer > 80) {
            this->picto.actor.shape.yOffset = 0.0f;
            EnBal_SetupGroundIdle(this);
        } else if (this->timer == 30) {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_LAND);
            this->picto.actor.shape.rot = this->picto.actor.world.rot;
            Actor_SpawnFloorDustRing(play, &this->picto.actor, &worldPos, 10.0f, 30, 5.0f, 0, 0, false);
            this->timer++;
        } else {
            if ((play->gameplayFrames % 2) != 0) {
                this->picto.actor.shape.yOffset = 20.0f;
            } else {
                this->picto.actor.shape.yOffset = 0.0f;
            }
            this->timer++;
        }
    } else if (this->picto.actor.velocity.y < 0.0f) {
        Actor_PlaySfx_Flagged(&this->picto.actor, NA_SE_EV_HONEYCOMB_FALL - SFX_FLAG);
    }

    this->skyFloatPhase += 0xBB8;
}

void EnBal_SetupInflateBalloon(EnBal* this) {
    Vec3f scale = { 0.0f, 0.0f, 0.0f };

    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_FLOAT_IDLE);
    this->balloonAction = TINGLE_BALLOON_ACTION_INFLATE;
    this->watchTarget = TINGLE_WATCH_TARGET_NONE;
    this->forceEyesShut = false;
    this->eyeTexIndex = TINGLE_EYETEX_OPEN;
    this->balloonScale = scale;
    this->skyFloatPhase = 0;
    this->timer = 0;
    this->actionFunc = EnBal_InflateBalloon;
}

void EnBal_InflateBalloon(EnBal* this, PlayState* play) {
    f32 scale;

    if (Math_SmoothStepToF(&this->balloonScale.x, 1.0f, 0.05f, 0.03f, 0.01f) == 0.0f) {
        EnBal_SetupFloatUp(this);
    }
    EnBal_EmitDustPuff(this, play);
    Actor_PlaySfx_Flagged(&this->picto.actor, NA_SE_EV_BALLOON_SWELL - SFX_FLAG);
    scale = this->balloonScale.x * 1.1f;
    this->balloonScale.z = scale;
    this->balloonScale.y = scale;
}

void EnBal_SetupFloatUp(EnBal* this) {
    this->balloonAction = TINGLE_BALLOON_ACTION_RISE;
    EnBal_SetMainColliderToBalloon(this);
    this->actionFunc = EnBal_FloatUp;
    this->picto.actor.gravity = 0.0f;
}

void EnBal_FloatUp(EnBal* this, PlayState* play) {
    f32 cosFactor;
    f32 sinFactor;

    if (this->timer < 500) {
        cosFactor = (Math_CosS(this->skyFloatPhase) * 0.1f) + 1.0f;
        this->balloonScale.z = cosFactor;
        this->balloonScale.y = cosFactor;
        sinFactor = (Math_SinS(this->skyFloatPhase) * 0.1f) + 1.0f;
        this->balloonScale.x = SQ(sinFactor);
        this->picto.actor.world.pos.y *= 500 - this->timer;
        this->picto.actor.world.pos.y +=
            this->timer * (this->picto.actor.home.pos.y + (50.0f * Math_SinS(this->skyFloatPhase)));
        this->picto.actor.world.pos.y *= 0.002f;

        if (this->timer < 100) {
            this->timer++;
        } else {
            this->timer += 10;
        }
        this->skyFloatPhase += 0x320;
    } else {
        EnBal_SetupFloatIdle(this);
    }
}

void EnBal_SetupGroundIdle(EnBal* this) {
    if (this->actionFunc == EnBal_Fall) {
        this->forceEyesShut = false;
        this->eyeTexIndex = TINGLE_EYETEX_OPEN;
        this->idleAnimStage = TINGLE_IDLESTAGE_WAIT;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_IDLE);
    } else if (Rand_Next() & 1) {
        this->idleAnimStage = TINGLE_IDLESTAGE_ACTIVITY;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_TALK);
    } else {
        this->idleAnimStage = TINGLE_IDLESTAGE_ACTIVITY;
        this->forceEyesShut = true;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_SPIN);
    }

    this->watchTarget = TINGLE_WATCH_TARGET_NONE;
    if (this->locationMapId == TINGLE_MAP_CLOCK_TOWN) {
        if (!GameInteractor_Should(VB_HAVE_MAGIC_FOR_TINGLE, gSaveContext.save.saveInfo.playerData.isMagicAcquired)) {
            // Effectively turn off reinflation timer by setting above 300
            this->timer = 301;
        } else if (this->inflateEarly == true) {
            // If this field is set, reduce time to balloon reinflation.
            this->inflateEarly = false;
            this->timer = 290;
        } else {
            this->timer = 0;
        }
    } else {
        this->timer = 0;
    }

    this->actionFunc = EnBal_GroundIdle;
}

void EnBal_GroundIdle(EnBal* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->timer == 300) {
        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            EnBal_SetupInflateBalloon(this);
            return;
        }
    } else if (this->timer < 300) {
        this->timer++;
    }

    if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        this->forceEyesShut = false;
        this->eyeTexIndex = TINGLE_EYETEX_OPEN;
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_TINGLE)) {
            if (GET_PLAYER_FORM == EnBal_GetRecognizedPlayerForm()) {
                this->watchTarget = TINGLE_WATCH_TARGET_PLAYER;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_TALK);
                Message_StartTextbox(play, 0x1D0C, &this->picto.actor);
                this->textId = 0x1D0C;
            } else {
                this->watchTarget = TINGLE_WATCH_TARGET_PLAYER;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_TALK);
                Message_StartTextbox(play, 0x1D05, &this->picto.actor);
                this->textId = 0x1D05;
            }

            if (!GameInteractor_Should(VB_HAVE_MAGIC_FOR_TINGLE,
                                       gSaveContext.save.saveInfo.playerData.isMagicAcquired)) {
                // Reinflation should be unreachable while player does not have magic
                this->inflateEarly = true;
            }
        } else {
            this->watchTarget = TINGLE_WATCH_TARGET_FAIRY;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_LAND);
            Message_StartTextbox(play, 0x1D00, &this->picto.actor);
            this->textId = 0x1D00;

            if (this->locationMapId == TINGLE_MAP_CLOCK_TOWN) {
                this->inflateEarly = true;
            }
        }
        EnBal_SetupTalk(this);
    } else {
        if ((this->picto.actor.xzDistToPlayer < 100.0f) && (this->actionFunc != EnBal_InflateBalloon)) {
            if (this->idleAnimStage != TINGLE_IDLESTAGE_WAIT) {
                this->watchTarget = TINGLE_WATCH_TARGET_PLAYER;
            } else {
                this->watchTarget = TINGLE_WATCH_TARGET_NONE;
            }

            if (!(player->stateFlags1 & PLAYER_STATE1_800000) && !(player->actor.bgCheckFlags & BGCHECKFLAG_WATER) &&
                ((this->timer < 300) || (this->timer == 301))) {
                Actor_OfferTalk(&this->picto.actor, play, 100.0f);
            }
        } else {
            this->watchTarget = TINGLE_WATCH_TARGET_NONE;
        }

        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            if (this->idleAnimStage == TINGLE_IDLESTAGE_PREP_WAIT) {
                this->forceEyesShut = false;
                this->eyeTexIndex = TINGLE_EYETEX_OPEN;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_IDLE);
                this->idleAnimStage++;
            } else if (this->idleAnimStage == TINGLE_IDLESTAGE_WAIT) {
                if (Rand_Next() & 1) {
                    this->forceEyesShut = false;
                    this->eyeTexIndex = TINGLE_EYETEX_OPEN;
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_TALK);
                } else {
                    this->forceEyesShut = true;
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_SPIN);
                }
                this->idleAnimStage = TINGLE_IDLESTAGE_ACTIVITY;
            } else {
                this->idleAnimStage++;
            }
        } else if ((this->idleAnimStage == TINGLE_IDLESTAGE_WAIT) && Animation_OnFrame(&this->skelAnime, 20.0f)) {
            this->forceEyesShut = true;
        }
    }
}

void EnBal_SetupTalk(EnBal* this) {
    this->timer = 0;
    this->actionFunc = EnBal_Talk;
}

void EnBal_Talk(EnBal* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->stateFlags2 |= PLAYER_STATE2_100000;
    this->isTalking = false;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_NEXT:
            break;

        case TEXT_STATE_CLOSING:
            break;

        case TEXT_STATE_FADING:
            if (this->textId != 0x1D10) {
                this->isTalking = true;
            }
            break;

        case TEXT_STATE_CHOICE:
            EnBal_TryPurchaseMap(this, play);
            break;

        case TEXT_STATE_EVENT:
            EnBal_HandleConversation(this, play);
            break;

        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                EnBal_SetupGroundIdle(this);
            }
            break;

        case TEXT_STATE_AWAITING_NEXT:
            if (Message_ShouldAdvance(play) && (this->textId == 0x1D08)) {
                this->forceEyesShut = false;
                this->eyeTexIndex = TINGLE_EYETEX_OPEN;
            }
            break;

        default:
            break;
    }

    if (this->textId == 0x1D07) {
        if ((this->skelAnime.curFrame > 29.0f) && (this->skelAnime.curFrame < 33.0f)) {
            if (Animation_OnFrame(&this->skelAnime, 30.0f)) {
                Actor_PlaySfx(&this->picto.actor, NA_SE_EV_CHINCLE_SPELL_EFFECT);
            }
            EnBal_ThrowMagicSparkles(this, play);
        }
        if (Animation_OnFrame(&this->skelAnime, 35.0f)) {
            this->forceEyesShut = false;
            this->eyeTexIndex = TINGLE_EYETEX_OPEN;
        }
    }
}

void EnBal_UpdateShadow(EnBal* this) {
    f32 homeY;
    f32 worldY;
    f32 shadowScale;

    if ((this->actionFunc != EnBal_GroundIdle) && (this->actionFunc != EnBal_FloatIdle)) {
        homeY = this->picto.actor.home.pos.y - this->picto.actor.floorHeight;
        worldY = this->picto.actor.world.pos.y - this->picto.actor.floorHeight;
        shadowScale = (homeY - worldY) / homeY;
        shadowScale = CLAMP_MIN(shadowScale, 0.0f);
        this->picto.actor.shape.shadowScale = 12.0f * shadowScale;
        this->picto.actor.shape.shadowAlpha = 255.0f * shadowScale;
    }
}

void EnBal_SetRecognizedPlayerForm(void) {
    switch (gSaveContext.save.playerForm) {
        case PLAYER_FORM_HUMAN:
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_RECOGNIZED_PLAYER_FORM_LOW_BIT);
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_RECOGNIZED_PLAYER_FORM_HIGH_BIT);
            break;

        case PLAYER_FORM_DEKU:
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_RECOGNIZED_PLAYER_FORM_LOW_BIT);
            break;

        case PLAYER_FORM_GORON:
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_RECOGNIZED_PLAYER_FORM_HIGH_BIT);
            break;

        case PLAYER_FORM_ZORA:
            break;

        default:
            break;
    }
}

PlayerTransformation EnBal_GetRecognizedPlayerForm(void) {
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_RECOGNIZED_PLAYER_FORM_LOW_BIT)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_RECOGNIZED_PLAYER_FORM_HIGH_BIT)) {
            return PLAYER_FORM_HUMAN;
        }

        return PLAYER_FORM_DEKU;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_RECOGNIZED_PLAYER_FORM_HIGH_BIT)) {
        return PLAYER_FORM_GORON;
    }

    return PLAYER_FORM_ZORA;
}

void EnBal_ThrowMagicSparkles(EnBal* this, PlayState* play) {
    static Vec3f sSparkleVelocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f sSparkleAccel = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 sSparklePrimColor = { 255, 255, 255, 255 };
    static Color_RGBA8 sSparkleEnvColor = { 100, 200, 0, 255 };
    s32 i;
    Vec3f pos = this->picto.actor.world.pos;

    pos.y += 30.0f;
    pos.x += 20.0f * Math_SinS(this->picto.actor.shape.rot.y);
    pos.z += 20.0f * Math_CosS(this->picto.actor.shape.rot.y);

    sSparkleAccel.y = -0.4f;
    sSparkleVelocity.y = 5.5f;

    for (i = 0; i < 20; i++) {
        sSparkleVelocity.x = Rand_Centered() * 3.0f;
        sSparkleVelocity.z = Rand_Centered() * 3.0f;
        EffectSsKirakira_SpawnDispersed(play, &pos, &sSparkleVelocity, &sSparkleAccel, &sSparklePrimColor,
                                        &sSparkleEnvColor, 2500, 40);
    }
}

void EnBal_EmitDustPuff(EnBal* this, PlayState* play) {
    static Vec3f sDustVelocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f sDustAccel = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 sDustPrimColor = { 255, 255, 255, 255 };
    static Color_RGBA8 sDustEnvColor = { 198, 198, 198, 255 };
    Vec3f pos = this->picto.actor.world.pos;

    pos.y += 40.0f;
    sDustVelocity.x = Rand_CenteredFloat(10.0f);
    sDustVelocity.z = Rand_CenteredFloat(10.0f);
    sDustVelocity.y = Rand_ZeroFloat(5.0f);
    func_800B0EB0(play, &pos, &sDustVelocity, &sDustAccel, &sDustPrimColor, &sDustEnvColor, 150, 40, 15);
}

static s32 sGetItemPending = false;
static Vec3f sFocusPosMultiplier = { 1000.0f, 0.0f, 0.0f };
static TexturePtr sEyeTextures[] = { gTingleEyeOpenTex, gTingleEyeClosedTex };

s32 EnBal_CheckIfMapUnlocked(EnBal* this, PlayState* play) {
    this->purchaseMapId = sBuyMapOptions[this->locationMapId][play->msgCtx.choiceIndex];
    switch (this->purchaseMapId) {
        case TINGLE_MAP_CLOCK_TOWN:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_CLOCK_TOWN)) {
                return true;
            }
            break;

        case TINGLE_MAP_WOODFALL:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_WOODFALL)) {
                return true;
            }
            break;

        case TINGLE_MAP_SNOWHEAD:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_SNOWHEAD)) {
                return true;
            }
            break;

        case TINGLE_MAP_ROMANI_RANCH:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_ROMANI_RANCH)) {
                return true;
            }
            break;

        case TINGLE_MAP_GREAT_BAY:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_GREAT_BAY)) {
                return true;
            }
            break;

        case TINGLE_MAP_STONE_TOWER:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_STONE_TOWER)) {
                return true;
            }
            break;

        default:
            break;
    }
    return false;
}

void EnBal_UnlockSelectedAreaMap(EnBal* this) {
    Inventory_SetWorldMapCloudVisibility(this->purchaseMapId);
    switch (this->purchaseMapId) {
        case TINGLE_MAP_CLOCK_TOWN:
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_CLOCK_TOWN);
            break;

        case TINGLE_MAP_WOODFALL:
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_WOODFALL);
            break;

        case TINGLE_MAP_SNOWHEAD:
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_SNOWHEAD);
            break;

        case TINGLE_MAP_ROMANI_RANCH:
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_ROMANI_RANCH);
            break;

        case TINGLE_MAP_GREAT_BAY:
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_GREAT_BAY);
            break;

        case TINGLE_MAP_STONE_TOWER:
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_STONE_TOWER);
            break;

        default:
            break;
    }
}

void EnBal_TryPurchaseMap(EnBal* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 price;

    if (Message_ShouldAdvance(play)) {
        if (play->msgCtx.choiceIndex != TINGLE_MAPCHOICE_CANCEL) {
            // Get price depending on which map player wants to buy
            if (play->msgCtx.choiceIndex == TINGLE_MAPCHOICE_PROXIMAL) {
                price = play->msgCtx.unk1206C;
            } else {
                price = play->msgCtx.unk12070;
            }

            if (GameInteractor_Should(VB_NOT_AFFORD_TINGLE_MAP, gSaveContext.save.saveInfo.playerData.rupees < price,
                                      this, &price)) {
                // Can't buy map because player doesn't have the money
                Audio_PlaySfx(NA_SE_SY_ERROR);
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_TALK);
                Message_StartTextbox(play, 0x1D0A, &this->picto.actor);
                this->textId = 0x1D0A;
            } else if (GameInteractor_Should(VB_ALREADY_HAVE_TINGLE_MAP, EnBal_CheckIfMapUnlocked(this, play), this)) {
                // Can't buy map because player already has it
                Audio_PlaySfx(NA_SE_SY_ERROR);
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_TALK);
                Message_StartTextbox(play, 0x1D09, &this->picto.actor);
                this->textId = 0x1D09;
            } else {
                // Proceed with map purchase
                Audio_PlaySfx_MessageDecide();
                Rupees_ChangeBy(-price);
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_MAGIC_REVERSE);
                this->forceEyesShut = true;
                if (GameInteractor_Should(VB_TINGLE_GIVE_MAP_UNLOCK, true, this)) {
                    Message_StartTextbox(play, 0x1D0B, &this->picto.actor);
                    this->textId = 0x1D0B;
                    EnBal_UnlockSelectedAreaMap(this);
                    player->stateFlags1 |= PLAYER_STATE1_20;
                    EnBal_SetupOfferGetItem(this);
                }
            }
        } else {
            // Cancel
            Audio_PlaySfx_MessageCancel();
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_TALK);
            Message_StartTextbox(play, 0x1D06, &this->picto.actor);
            this->textId = 0x1D06;
        }
    }
}

void EnBal_HandleConversation(EnBal* this, PlayState* play) {
    if (((this->textId != 0x1D07) || Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) &&
        Message_ShouldAdvance(play)) {
        switch (this->textId) {
            case 0x1D00:
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_TALK);
                Message_StartTextbox(play, 0x1D01, &this->picto.actor);
                this->textId = 0x1D01;
                break;

            case 0x1D01:
                this->watchTarget = TINGLE_WATCH_TARGET_PLAYER;
                Message_StartTextbox(play, 0x1D02, &this->picto.actor);
                this->textId = 0x1D02;
                break;

            case 0x1D02:
                this->forceEyesShut = true;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_HAPPY_DANCE_LOOP);
                Message_StartTextbox(play, 0x1D03, &this->picto.actor);
                this->textId = 0x1D03;
                break;

            case 0x1D03:
            case 0x1D0D:
                this->forceEyesShut = false;
                this->eyeTexIndex = TINGLE_EYETEX_OPEN;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_TALK);
                Message_StartTextbox(play, 0x1D04, &this->picto.actor);
                this->textId = 0x1D04;
                break;

            case 0x1D04:
                this->watchTarget = TINGLE_WATCH_TARGET_PLAYER;
                switch (this->locationMapId) {
                    case TINGLE_MAP_CLOCK_TOWN:
                        Message_StartTextbox(play, 0x1D11, &this->picto.actor);
                        this->textId = 0x1D11;
                        break;

                    case TINGLE_MAP_WOODFALL:
                        Message_StartTextbox(play, 0x1D12, &this->picto.actor);
                        this->textId = 0x1D12;
                        break;

                    case TINGLE_MAP_SNOWHEAD:
                        Message_StartTextbox(play, 0x1D13, &this->picto.actor);
                        this->textId = 0x1D13;
                        break;

                    case TINGLE_MAP_ROMANI_RANCH:
                        Message_StartTextbox(play, 0x1D14, &this->picto.actor);
                        this->textId = 0x1D14;
                        break;

                    case TINGLE_MAP_GREAT_BAY:
                        Message_StartTextbox(play, 0x1D15, &this->picto.actor);
                        this->textId = 0x1D15;
                        break;

                    case TINGLE_MAP_STONE_TOWER:
                        Message_StartTextbox(play, 0x1D16, &this->picto.actor);
                        this->textId = 0x1D16;
                        break;

                    default:
                        Message_StartTextbox(play, 0x1D11, &this->picto.actor);
                        this->textId = 0x1D11;
                        break;
                }
                break;

            case 0x1D05:
            case 0x1D0C:
                this->watchTarget = TINGLE_WATCH_TARGET_FAIRY;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_HAPPY_DANCE_LOOP);
                Message_StartTextbox(play, 0x1D0D, &this->picto.actor);
                this->textId = 0x1D0D;
                break;

            case 0x1D06:
            case 0x1D17:
                this->watchTarget = TINGLE_WATCH_TARGET_NONE;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_MAGIC);
                this->forceEyesShut = true;
                Message_StartTextbox(play, 0x1D07, &this->picto.actor);
                this->textId = 0x1D07;
                break;

            case 0x1D07:
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_TINGLE)) {
                    Message_CloseTextbox(play);
                    EnBal_SetupGroundIdle(this);
                } else {
                    this->forceEyesShut = true;
                    this->watchTarget = TINGLE_WATCH_TARGET_PLAYER;
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_TALK);
                    Message_StartTextbox(play, 0x1D08, &this->picto.actor);
                    this->textId = 0x1D08;
                    SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_TINGLE);
                    EnBal_SetRecognizedPlayerForm();
                }
                break;

            case 0x1D08:
            case 0x1D09:
            case 0x1D0A:
                Message_CloseTextbox(play);
                EnBal_SetupGroundIdle(this);
                break;

            default:
                break;
        }
    }
}

void EnBal_SetupOfferGetItem(EnBal* this) {
    this->actionFunc = EnBal_OfferGetItem;
}

void EnBal_OfferGetItem(EnBal* this, PlayState* play) {
    GetItemId mapGetItemId;

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        sGetItemPending = true;
    }

    if ((sGetItemPending == true) && Actor_HasParent(&this->picto.actor, play)) {
        this->picto.actor.parent = NULL;
        EnBal_SetupThankYou(this);
        sGetItemPending = false;
    } else {
        switch (this->purchaseMapId) {
            case TINGLE_MAP_CLOCK_TOWN:
                mapGetItemId = GI_TINGLE_MAP_CLOCK_TOWN;
                break;

            case TINGLE_MAP_WOODFALL:
                mapGetItemId = GI_TINGLE_MAP_WOODFALL;
                break;

            case TINGLE_MAP_SNOWHEAD:
                mapGetItemId = GI_TINGLE_MAP_SNOWHEAD;
                break;

            case TINGLE_MAP_ROMANI_RANCH:
                mapGetItemId = GI_TINGLE_MAP_ROMANI_RANCH;
                break;

            case TINGLE_MAP_GREAT_BAY:
                mapGetItemId = GI_TINGLE_MAP_GREAT_BAY;
                break;

            case TINGLE_MAP_STONE_TOWER:
                mapGetItemId = GI_TINGLE_MAP_STONE_TOWER;
                break;

            default:
                mapGetItemId = GI_TINGLE_MAP_CLOCK_TOWN;
                break;
        }
        Actor_OfferGetItem(&this->picto.actor, play, mapGetItemId, 500.0f, 100.0f);
    }
}

void EnBal_SetupThankYou(EnBal* this) {
    this->actionFunc = EnBal_ThankYou;
}

void EnBal_ThankYou(EnBal* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        player->stateFlags1 &= ~PLAYER_STATE1_20;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_ANIM_TWIST);
        this->forceEyesShut = false;
        this->eyeTexIndex = TINGLE_EYETEX_OPEN;
        Message_StartTextbox(play, 0x1D17, &this->picto.actor);
        this->textId = 0x1D17;
        this->picto.actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        EnBal_SetupTalk(this);
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->picto.actor, play, 200.0f, PLAYER_IA_MINUS1);
    }
}

void EnBal_UpdateHead(EnBal* this, PlayState* play) {
    Player* player;

    if (this->watchTarget == TINGLE_WATCH_TARGET_PLAYER) {
        if ((this->skelAnime.animation != &gTingleIdleAnim) && (this->skelAnime.animation != &gTingleSpinAnim)) {
            Actor_TrackPlayer(play, &this->picto.actor, &this->headRot, &this->torsoRot, this->picto.actor.focus.pos);
        } else {
            Actor_TrackNone(&this->headRot, &this->torsoRot);
        }
    } else if (this->watchTarget == TINGLE_WATCH_TARGET_FAIRY) {
        Vec3f tatlPos;

        player = GET_PLAYER(play);
        if (player->tatlActor != NULL) {
            tatlPos = player->tatlActor->world.pos;
            Actor_TrackPoint(&this->picto.actor, &tatlPos, &this->headRot, &this->torsoRot);
        }
    } else {
        Actor_TrackNone(&this->headRot, &this->torsoRot);
    }

    if (this->forceEyesShut == true) {
        this->eyeTexIndex = TINGLE_EYETEX_CLOSED;
    } else if (this->eyeTimer > 3) {
        this->eyeTimer--;
    } else if (this->eyeTimer != 0) {
        this->eyeTexIndex = TINGLE_EYETEX_CLOSED;
        this->eyeTimer--;
    } else {
        this->eyeTexIndex = TINGLE_EYETEX_OPEN;
        this->eyeTimer = 60;
    }
}

void EnBal_UpdateCollision(EnBal* this, PlayState* play) {
    if ((this->actionFunc == EnBal_FloatIdle) || ((this->actionFunc == EnBal_FloatUp) && (this->timer > 50))) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnBal_TryBalloonPopped(EnBal* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->picto.actor.world.pos.x,
                    this->picto.actor.world.pos.y + 100.0f, this->picto.actor.world.pos.z, 0xFF, 0xFF, 0xC8,
                    CLEAR_TAG_PARAMS(CLEAR_TAG_POP));
        this->collider.base.acFlags &= ~AC_HIT;
        EnBal_SetupPopBalloon(this);
    }
}

void EnBal_Update(Actor* thisx, PlayState* play) {
    EnBal* this = (EnBal*)thisx;

    this->actionFunc(this, play);
    EnBal_TryBalloonPopped(this, play);
    EnBal_UpdateCollision(this, play);
    SkelAnime_Update(&this->skelAnime);
    if (this->actionFunc != EnBal_FloatIdle) {
        Actor_MoveWithGravity(&this->picto.actor);
        Actor_UpdateBgCheckInfo(play, &this->picto.actor, 32.0f, 30.0f, 60.0f,
                                UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4);
    }
    EnBal_UpdateHead(this, play);
    if ((this->actionFunc == EnBal_Talk) && (this->textId != 0x1D10)) {
        Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 5, 0x1000, 0x100);
        this->picto.actor.world.rot.y = this->picto.actor.shape.rot.y;
    }
    EnBal_UpdateShadow(this);
}

s32 EnBal_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnBal* this = (EnBal*)thisx;
    Vec3s balloonRot;

    if (limbIndex == TINGLE_LIMB_BALLOON) {
        if ((this->balloonAction == TINGLE_BALLOON_ACTION_NONE) ||
            (this->balloonAction == TINGLE_BALLOON_ACTION_INFLATE) ||
            (this->balloonAction == TINGLE_BALLOON_ACTION_RISE)) {
            balloonRot.x = Math_SinS(this->timer) * 0xE38;
            balloonRot.z = Math_CosS(this->timer) * 0xE38;
            Matrix_RotateZYX(balloonRot.x, 0, balloonRot.z, MTXMODE_APPLY);
            Matrix_Scale(this->balloonScale.x, this->balloonScale.y, this->balloonScale.z, MTXMODE_APPLY);
            Matrix_RotateZS(-balloonRot.z, MTXMODE_APPLY);
            Matrix_RotateXS(-balloonRot.x, MTXMODE_APPLY);
        } else {
            *dList = NULL;
        }
    }

    if (limbIndex == TINGLE_LIMB_HEAD) {
        rot->x += this->headRot.y;
        rot->y -= this->headRot.x;
        if ((this->isTalking == true) && ((play->state.frames & 2) == 0)) {
            Matrix_Translate(20.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        }
    }

    return false;
}

void EnBal_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnBal* this = (EnBal*)thisx;

    Collider_UpdateSpheres(limbIndex, &this->collider);
    if (limbIndex == TINGLE_LIMB_HEAD) {
        Matrix_MultVec3f(&sFocusPosMultiplier, &this->picto.actor.focus.pos);
    }
}

void EnBal_Draw(Actor* thisx, PlayState* play) {
    EnBal* this = (EnBal*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_K0(sEyeTextures[this->eyeTexIndex]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnBal_OverrideLimbDraw, EnBal_PostLimbDraw, &this->picto.actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
