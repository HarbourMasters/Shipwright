/*
 * File: z_en_jg.c
 * Overlay: ovl_En_Jg
 * Description: Goron Elder
 */

#include "z_en_jg.h"
#include "overlays/actors/ovl_En_S_Goro/z_en_s_goro.h"
#include "overlays/actors/ovl_Obj_Ice_Poly/z_obj_ice_poly.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

#define FLAG_SHRINE_GORON_ARMS_RAISED (1 << 0)
#define FLAG_LOOKING_AT_PLAYER (1 << 2)
#define FLAG_DRUM_SPAWNED (1 << 3)

void EnJg_Init(Actor* thisx, PlayState* play);
void EnJg_Destroy(Actor* thisx, PlayState* play);
void EnJg_Update(Actor* thisx, PlayState* play);
void EnJg_Draw(Actor* thisx, PlayState* play);

void EnJg_GoronShrineTalk(EnJg* this, PlayState* play);
void EnJg_GoronShrineCheer(EnJg* this, PlayState* play);
void EnJg_AlternateTalkOrWalkInPlace(EnJg* this, PlayState* play);
void EnJg_Walk(EnJg* this, PlayState* play);
void EnJg_Talk(EnJg* this, PlayState* play);
void EnJg_SetupWalk(EnJg* this, PlayState* play);
void EnJg_FrozenIdle(EnJg* this, PlayState* play);
void EnJg_EndFrozenInteraction(EnJg* this, PlayState* play);
void EnJg_TeachLullabyIntro(EnJg* this, PlayState* play);
void EnJg_LullabyIntroCutsceneAction(EnJg* this, PlayState* play);
s32 EnJg_GetNextTextId(EnJg* this);
s32 EnJg_GetStartingConversationTextId(EnJg* this, PlayState* play);
void EnJg_CheckIfTalkingToPlayerAndHandleFreezeTimer(EnJg* this, PlayState* play);

typedef enum {
    /* 0 */ EN_JG_ACTION_FIRST_THAW,
    /* 1 */ EN_JG_ACTION_SPAWNING,
    /* 2 */ EN_JG_ACTION_FROZEN_OR_NON_FIRST_THAW,
    /* 3 */ EN_JG_ACTION_LULLABY_INTRO_CS
} EnJgAction;

ActorProfile En_Jg_Profile = {
    /**/ ACTOR_EN_JG,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_JG,
    /**/ sizeof(EnJg),
    /**/ EnJg_Init,
    /**/ EnJg_Destroy,
    /**/ EnJg_Update,
    /**/ EnJg_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 60, 80, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 50, 80, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(0, 0x0),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(0, 0x0),
    /* Zora boomerang */ DMG_ENTRY(0, 0x0),
    /* Normal arrow   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0x0),
    /* Goron punch    */ DMG_ENTRY(0, 0x0),
    /* Sword          */ DMG_ENTRY(0, 0x0),
    /* Goron pound    */ DMG_ENTRY(0, 0x0),
    /* Fire arrow     */ DMG_ENTRY(0, 0x0),
    /* Ice arrow      */ DMG_ENTRY(0, 0x0),
    /* Light arrow    */ DMG_ENTRY(0, 0x0),
    /* Goron spikes   */ DMG_ENTRY(0, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x0),
    /* Deku bubble    */ DMG_ENTRY(0, 0x0),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0x0),
    /* Zora punch     */ DMG_ENTRY(0, 0x0),
    /* Spin attack    */ DMG_ENTRY(0, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(0, 0x0),
};

typedef enum EnJgAnimation {
    /*  0 */ EN_JG_ANIM_IDLE,
    /*  1 */ EN_JG_ANIM_WALK,
    /*  2 */ EN_JG_ANIM_WAVING,
    /*  3 */ EN_JG_ANIM_SHAKING_HEAD,
    /*  4 */ EN_JG_ANIM_SURPRISE_START,
    /*  5 */ EN_JG_ANIM_SURPRISE_LOOP,
    /*  6 */ EN_JG_ANIM_ANGRY,
    /*  7 */ EN_JG_ANIM_FROZEN_START,
    /*  8 */ EN_JG_ANIM_FROZEN_LOOP,
    /*  9 */ EN_JG_ANIM_WALK_2,
    /* 10 */ EN_JG_ANIM_TAKING_OUT_DRUM,
    /* 11 */ EN_JG_ANIM_DRUM_IDLE,
    /* 12 */ EN_JG_ANIM_PLAYING_DRUM,
    /* 13 */ EN_JG_ANIM_THINKING,
    /* 14 */ EN_JG_ANIM_REMEMBERING,
    /* 15 */ EN_JG_ANIM_STRONG_REMEMBERING,
    /* 16 */ EN_JG_ANIM_DEPRESSED,
    /* 17 */ EN_JG_ANIM_CUTSCENE_IDLE,
    /* 18 */ EN_JG_ANIM_CRADLE,
    /* 19 */ EN_JG_ANIM_MAX
} EnJgAnimation;

static AnimationInfoS sAnimationInfo[EN_JG_ANIM_MAX] = {
    { &gGoronElderIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -10 },            // EN_JG_ANIM_IDLE
    { &gGoronElderWalkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -10 },            // EN_JG_ANIM_WALK
    { &gGoronElderWavingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -10 },          // EN_JG_ANIM_WAVING
    { &gGoronElderHeadShakeAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -10 },       // EN_JG_ANIM_SHAKING_HEAD
    { &gGoronElderSurpriseStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -10 },   // EN_JG_ANIM_SURPRISE_START
    { &gGoronElderSurpriseLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -10 },    // EN_JG_ANIM_SURPRISE_LOOP
    { &gGoronElderAngryAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -10 },           // EN_JG_ANIM_ANGRY
    { &gGoronElderSurpriseStartAnim, 2.0f, 0, -1, ANIMMODE_ONCE, 0 },     // EN_JG_ANIM_FROZEN_START
    { &gGoronElderSurpriseStartAnim, -2.0f, 0, -1, ANIMMODE_ONCE, 0 },    // EN_JG_ANIM_FROZEN_LOOP
    { &gGoronElderWalkAnim, -1.0f, 0, -1, ANIMMODE_LOOP, -10 },           // EN_JG_ANIM_WALK_2
    { &gGoronElderTakeOutDrumAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },       // EN_JG_ANIM_TAKING_OUT_DRUM
    { &gGoronElderDrumIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },          // EN_JG_ANIM_DRUM_IDLE
    { &gGoronElderPlayingDrumAnim, 1.0f, 1, 44, ANIMMODE_ONCE, 0 },       // EN_JG_ANIM_PLAYING_DRUM
    { &gGoronElderThinkingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },          // EN_JG_ANIM_THINKING
    { &gGoronElderRememberingAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },       // EN_JG_ANIM_REMEMBERING
    { &gGoronElderStrongRememberingAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 }, // EN_JG_ANIM_STRONG_REMEMBERING
    { &gGoronElderDepressedAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },         // EN_JG_ANIM_DEPRESSED
    { &gGoronElderIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },              // EN_JG_ANIM_CUTSCENE_IDLE
    { &gGoronElderCradleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },            // EN_JG_ANIM_CRADLE
};

static Vec3f sSfxPos = { 0.0f, 0.0f, 0.0f };

static Vec3f sFocusOffset = { 0.0f, 0.0f, 0.0f };

static Vec3f sBreathPosOffset = { 1000.0f, -500.0f, 0.0f };

static Vec3f sBreathVelOffset = { 0.0f, 0.0f, 0.75f };

static Vec3f sBreathAccelOffset = { 0.0f, 0.0f, -0.070000000298f };

/**
 * When the elder and the Gorons in the shrine are doing a cheer
 * for Darmani, the camera will focus on specific Gorons for
 * certain parts of the cheer. This function is responsible for
 * returning a pointer to the specified Goron so the camera
 * can focus on them.
 */
Actor* EnJg_GetShrineGoronToFocusOn(PlayState* play, u8 focusedShrineGoronParam) {
    Actor* actorIterator = play->actorCtx.actorLists[ACTORCAT_NPC].first;

    while (actorIterator != NULL) {
        if ((actorIterator->id == ACTOR_EN_S_GORO) &&
            (EN_S_GORO_GET_MAIN_TYPE(actorIterator) == focusedShrineGoronParam)) {
            return actorIterator;
        }

        actorIterator = actorIterator->next;
    }

    return NULL;
}

void EnJg_UpdateCollision(EnJg* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 30.0f, 30.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4);
}

s16 EnJg_GetWalkingYRotation(Path* path, s32 pointIndex, Vec3f* pos, f32* distSQ) {
    Vec3s* points;
    f32 diffX;
    f32 diffZ;

    if (path != NULL) {
        points = Lib_SegmentedToVirtual(path->points);
        points = &points[pointIndex];
        diffX = points[0].x - pos->x;
        diffZ = points[0].z - pos->z;
    } else {
        diffX = 0.0f;
        diffZ = 0.0f;
    }

    *distSQ = SQ(diffX) + SQ(diffZ);

    return RAD_TO_BINANG(Math_Atan2F_XY(diffZ, diffX));
}

s32 EnJg_HasReachedPoint(EnJg* this, Path* path, s32 pointIndex) {
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    s32 count = path->count;
    s32 index = pointIndex;
    s32 reached = false;
    f32 diffX;
    f32 diffZ;
    f32 px;
    f32 pz;
    f32 d;
    Vec3f point;

    Math_Vec3s_ToVec3f(&point, &points[index]);

    if (index == 0) {
        diffX = points[1].x - points[0].x;
        diffZ = points[1].z - points[0].z;
    } else if (index == (count - 1)) {
        diffX = points[count - 1].x - points[count - 2].x;
        diffZ = points[count - 1].z - points[count - 2].z;
    } else {
        diffX = points[index + 1].x - points[index - 1].x;
        diffZ = points[index + 1].z - points[index - 1].z;
    }

    Math3D_RotateXZPlane(&point, RAD_TO_BINANG(Math_FAtan2F(diffX, diffZ)), &px, &pz, &d);

    if (((px * this->actor.world.pos.x) + (pz * this->actor.world.pos.z) + d) > 0.0f) {
        reached = true;
    }

    return reached;
}

s16 EnJg_GetCsIdForTeachingLullabyIntro(EnJg* this) {
    s16 temp = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (temp > 0) {
        return this->actor.csId;
    }

    return CutsceneManager_GetAdditionalCsId(this->actor.csId);
}

void EnJg_SetupGoronShrineCheer(EnJg* this) {
    CutsceneManager_Stop(this->csId);
    if (this->focusedShrineGoronParam == 10) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            this->actionFunc = EnJg_GoronShrineTalk;
        } else {
            this->csId = CS_ID_GLOBAL_TALK;
        }
    } else {
        this->csId = CutsceneManager_GetAdditionalCsId(this->csId);
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
    }

    CutsceneManager_Queue(this->csId);
    this->actionFunc = EnJg_GoronShrineCheer;

    switch (this->textId) {
        case 0xDD0: // The greatest Goron hero of all?
        case 0xDD2: // The immortal Goron?
        case 0xDD3: // The star we wish upon?
        case 0xDD4: // "Darmani, greatest of Gorons!"
        case 0xDD6: // Darmani, greatest in the world!
            this->flags |= FLAG_SHRINE_GORON_ARMS_RAISED;
            break;

        default:
            this->flags &= ~FLAG_SHRINE_GORON_ARMS_RAISED;
            break;
    }
}

void EnJg_SetupTalk(EnJg* this, PlayState* play) {
    switch (this->textId) {
        case 0xDAC: // What was I doing?
            this->animIndex = EN_JG_ANIM_SHAKING_HEAD;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnJg_Talk;
            break;

        case 0xDAD: // I must hurry!
            this->animIndex = EN_JG_ANIM_SURPRISE_START;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnJg_AlternateTalkOrWalkInPlace;
            break;

        case 0xDB7: // You're Darmani!
            this->animIndex = EN_JG_ANIM_SURPRISE_START;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnJg_Talk;
            break;

        case 0xDAE: // Do you have business with the Elder?
        case 0xDB3: // As Elder, I must do something
        case 0xDB6: // "Hunh???"
        case 0xDBA: // I've been made a fool of!
        case 0xDBD: // "...What?"
        case 0xDC4: // It's so cold I can't play
            this->animIndex = EN_JG_ANIM_IDLE;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnJg_Talk;
            break;

        case 0xDB0: // It's this cold snap
        case 0xDBB: // If I can see past the illusion, you'll vanish
        case 0xDBC: // Following me won't do you any good
        case 0xDC6: // I am counting on you
            this->animIndex = EN_JG_ANIM_ANGRY;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnJg_Talk;
            break;

        case 0xDB4: // This is our problem (first)
        case 0xDB5: // This is our problem (repeat)
            this->animIndex = EN_JG_ANIM_WAVING;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnJg_Talk;
            break;

        default:
            break;
    }
}

void EnJg_Idle(EnJg* this, PlayState* play) {
    EnJg_CheckIfTalkingToPlayerAndHandleFreezeTimer(this, play);
}

void EnJg_GoronShrineIdle(EnJg* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->flags |= FLAG_LOOKING_AT_PLAYER;
        Message_StartTextbox(play, this->textId, &this->actor);
        this->actionFunc = EnJg_GoronShrineTalk;
    } else if ((this->actor.xzDistToPlayer < 100.0f) || this->actor.isLockedOn) {
        Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
        this->textId = EnJg_GetStartingConversationTextId(this, play);
    }
}

void EnJg_GoronShrineTalk(EnJg* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if ((this->textId == 0xDCC) || (this->textId == 0xDDD) || (this->textId == 0xDE0)) {
            // There is nothing more to say after these lines, so end the current conversation.
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            this->flags &= ~FLAG_LOOKING_AT_PLAYER;
            this->actionFunc = EnJg_GoronShrineIdle;
        } else {
            this->textId = EnJg_GetNextTextId(this);
            Message_StartTextbox(play, this->textId, &this->actor);
        }
    }
}

void EnJg_GoronShrineCheer(EnJg* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->csId)) {
        switch (this->textId) {
            case 0xDD0: // The greatest Goron hero of all?
            case 0xDD2: // The immortal Goron?
            case 0xDD3: // The star we wish upon?
            case 0xDD4: // "Darmani, greatest of Gorons!"
            case 0xDD6: // Darmani, greatest in the world!
                // Focus on a specifc Goron for these lines
                this->shrineGoron = EnJg_GetShrineGoronToFocusOn(play, this->focusedShrineGoronParam);
                CutsceneManager_Start(this->csId, this->shrineGoron);
                Camera_SetTargetActor(play->cameraPtrs[CAM_ID_MAIN], this->shrineGoron);
                break;

            default:
                // Focus on the whole group for these lines
                CutsceneManager_Start(this->csId, &this->actor);
                Camera_SetTargetActor(play->cameraPtrs[CAM_ID_MAIN], this->shrineGoron);
                break;
        }
        this->actionFunc = EnJg_GoronShrineTalk;
    } else {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            if (this->focusedShrineGoronParam == 10) {
                this->actionFunc = EnJg_GoronShrineTalk;
            } else {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
        }
        CutsceneManager_Queue(this->csId);
    }
}

/**
 * You need to reach a certain point in the conversation when talking with the elder before
 * he starts walking for the first time. This function specifically handles this scenario; the
 * reason they used this instead of EnJg_Talk is seemingly to bypass EnJg_SetupWalk resetting
 * the freeze timer.
 *
 * Additionally, when the elder has reached the end of his specified path, this function will
 * set his speed to 0, causing him to walk in place.
 */
void EnJg_AlternateTalkOrWalkInPlace(EnJg* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (this->animIndex == EN_JG_ANIM_SURPRISE_START) {
        if (curFrame == endFrame) {
            this->animIndex = EN_JG_ANIM_SURPRISE_LOOP;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        }
    } else if (this->animIndex == EN_JG_ANIM_SURPRISE_LOOP) {
        if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            this->flags &= ~FLAG_LOOKING_AT_PLAYER;
            this->animIndex = EN_JG_ANIM_WALK;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnJg_Walk;
        }
    } else if (this->animIndex == EN_JG_ANIM_WALK) {
        Math_ApproachF(&this->actor.speed, 0.0f, 0.2f, 1.0f);
        EnJg_CheckIfTalkingToPlayerAndHandleFreezeTimer(this, play);
    }
}

void EnJg_Walk(EnJg* this, PlayState* play) {
    s16 yRotation;
    f32 distSQ;

    if (this->path != NULL) {
        yRotation = EnJg_GetWalkingYRotation(this->path, this->currentPoint, &this->actor.world.pos, &distSQ);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            yRotation = this->actor.wallYaw;
        }

        Math_SmoothStepToS(&this->actor.world.rot.y, yRotation, 4, 0x3E8, 1);
        this->actor.shape.rot.y = this->actor.world.rot.y;

        if (EnJg_HasReachedPoint(this, this->path, this->currentPoint)) {
            if (this->currentPoint >= (this->path->count - 1)) {
                // Force the elder to walk in place
                this->animIndex = EN_JG_ANIM_WALK;
                this->actionFunc = EnJg_AlternateTalkOrWalkInPlace;
            } else {
                this->currentPoint++;
                Math_ApproachF(&this->actor.speed, 0.5f, 0.2f, 1.0f);
            }
        } else {
            Math_ApproachF(&this->actor.speed, 0.5f, 0.2f, 1.0f);
        }
    }

    EnJg_CheckIfTalkingToPlayerAndHandleFreezeTimer(this, play);
}

void EnJg_Talk(EnJg* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);
    u16 temp;

    if ((this->animIndex == EN_JG_ANIM_SURPRISE_START) && (curFrame == endFrame)) {
        this->animIndex = EN_JG_ANIM_SURPRISE_LOOP;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
    }

    if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        temp = this->textId;
        if ((temp == 0xDB4) || (temp == 0xDB5) || (temp == 0xDC4) || (temp == 0xDC6)) {
            // There is nothing more to say after these lines, so end the current conversation.
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            this->flags &= ~FLAG_LOOKING_AT_PLAYER;
            this->actionFunc = EnJg_SetupWalk;
            return;
        }

        temp = this->textId;
        if ((temp == 0xDBB) || (temp == 0xDBC)) {
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_24_80)) {
                // The player hasn't talked to the Goron Child at least once, so they can't learn
                // the Lullaby Intro. End the current conversation with the player.
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                this->flags &= ~FLAG_LOOKING_AT_PLAYER;
                this->actionFunc = EnJg_SetupWalk;
            } else if (GameInteractor_Should(VB_JG_THINK_YOU_KNOW_LULLABY,
                                             CHECK_WEEKEVENTREG(WEEKEVENTREG_24_40) ||
                                                 CHECK_QUEST_ITEM(QUEST_SONG_LULLABY) ||
                                                 CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO),
                                             this)) {
                // The player already has the Lullaby or Lullaby Intro, so say "I'm counting on you"
                this->textId = EnJg_GetNextTextId(this);
                Message_StartTextbox(play, this->textId, &this->actor);
                this->actionFunc = EnJg_SetupTalk;
            } else {
                // To get to this point, the player *has* talked to the Goron Child, but doesn't
                // already have the Lullaby or Lullaby Intro. End the current conversation and
                // start the cutscene that teaches the Lullaby Intro.
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                this->flags &= ~FLAG_LOOKING_AT_PLAYER;
                this->csId = EnJg_GetCsIdForTeachingLullabyIntro(this);
                if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                    CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
                }
                CutsceneManager_Queue(this->csId);
                this->actionFunc = EnJg_TeachLullabyIntro;
            }
        } else {
            this->textId = EnJg_GetNextTextId(this);
            Message_StartTextbox(play, this->textId, &this->actor);
            this->actionFunc = EnJg_SetupTalk;
        }
    }
}

void EnJg_SetupWalk(EnJg* this, PlayState* play) {
    if (this->animIndex != EN_JG_ANIM_WALK) {
        this->animIndex = EN_JG_ANIM_WALK;
        this->freezeTimer = 1000;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        this->actionFunc = EnJg_Walk;
    } else {
        this->freezeTimer = 1000;
        this->actionFunc = EnJg_Walk;
    }
}

void EnJg_Freeze(EnJg* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (this->action == EN_JG_ACTION_SPAWNING) {
        this->action = EN_JG_ACTION_FROZEN_OR_NON_FIRST_THAW;
        this->freezeTimer = 1000;
        this->skelAnime.curFrame = endFrame;
        this->icePoly =
            Actor_Spawn(&play->actorCtx, play, ACTOR_OBJ_ICE_POLY, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, this->actor.world.rot.x, this->actor.world.rot.y,
                        this->actor.world.rot.z, OBJICEPOLY_PARAMS(80, OBJICEPOLY_SWITCH_FLAG_NONE));
        this->animIndex = EN_JG_ANIM_FROZEN_LOOP;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        this->actionFunc = EnJg_FrozenIdle;
    } else if (this->animIndex == EN_JG_ANIM_FROZEN_START) {
        this->action = EN_JG_ACTION_FROZEN_OR_NON_FIRST_THAW;
        if (curFrame == endFrame) {
            this->freezeTimer = 1000;
            this->icePoly =
                Actor_Spawn(&play->actorCtx, play, ACTOR_OBJ_ICE_POLY, this->actor.world.pos.x, this->actor.world.pos.y,
                            this->actor.world.pos.z, this->actor.world.rot.x, this->actor.world.rot.y,
                            this->actor.world.rot.z, OBJICEPOLY_PARAMS(80, OBJICEPOLY_SWITCH_FLAG_NONE));
            this->animIndex = EN_JG_ANIM_FROZEN_LOOP;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnJg_FrozenIdle;
        }
    }
}

void EnJg_FrozenIdle(EnJg* this, PlayState* play) {
    if (this->icePoly->update == NULL) {
        this->icePoly = NULL;
        if (this->animIndex == EN_JG_ANIM_FROZEN_LOOP) {
            if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
                this->animIndex = EN_JG_ANIM_IDLE;

                // If you've never talked to the elder before, he stands
                // around idle until you've talked to him at least once.
                // Otherwise, he immediately begins walking after being
                // thawed out.
                if (this->textId == 0xDAC) {
                    SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
                    this->actionFunc = EnJg_Idle;
                } else {
                    this->freezeTimer = 1000;
                    SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
                    this->actionFunc = EnJg_Walk;
                }
            }
        }
    } else {
        if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
            Message_StartTextbox(play, 0x236, &this->actor); // The old Goron is frozen solid!
            this->actionFunc = EnJg_EndFrozenInteraction;
        } else if (this->actor.isLockedOn) {
            Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
        }
    }
}

void EnJg_EndFrozenInteraction(EnJg* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        play->msgCtx.stateTimer = 4;
        this->actionFunc = EnJg_FrozenIdle;
    }
}

void EnJg_TeachLullabyIntro(EnJg* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->csId)) {
        CutsceneManager_Start(this->csId, &this->actor);
        this->actionFunc = EnJg_LullabyIntroCutsceneAction;
    } else {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        CutsceneManager_Queue(this->csId);
    }
}

void EnJg_LullabyIntroCutsceneAction(EnJg* this, PlayState* play) {
    s32 pad;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_470)) {
        s32 cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_470);

        if (this->cueId != play->csCtx.actorCues[cueChannel]->id) {
            this->cueId = play->csCtx.actorCues[cueChannel]->id;

            switch (play->csCtx.actorCues[cueChannel]->id) {
                case 1:
                    this->csAnimIndex = EN_JG_ANIM_CUTSCENE_IDLE;
                    if (this->drum != NULL) {
                        Actor_Kill(this->drum);
                        this->drum = NULL;
                    }
                    break;

                case 2:
                    this->csAnimIndex = EN_JG_ANIM_TAKING_OUT_DRUM;
                    break;

                case 3:
                    this->csAnimIndex = EN_JG_ANIM_DRUM_IDLE;
                    break;

                case 4:
                    this->csAnimIndex = EN_JG_ANIM_PLAYING_DRUM;
                    break;

                case 5:
                    this->csAnimIndex = EN_JG_ANIM_THINKING;
                    break;

                case 6:
                    this->csAnimIndex = EN_JG_ANIM_REMEMBERING;
                    break;

                case 7:
                    this->csAnimIndex = EN_JG_ANIM_STRONG_REMEMBERING;
                    break;

                case 8:
                    this->csAnimIndex = EN_JG_ANIM_DEPRESSED;
                    break;

                case 9:
                    this->csAnimIndex = EN_JG_ANIM_CRADLE;
                    break;

                default:
                    this->csAnimIndex = EN_JG_ANIM_IDLE;
                    break;
            }

            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->csAnimIndex);
        }

        if (!(this->flags & FLAG_DRUM_SPAWNED) &&
            (((this->csAnimIndex == EN_JG_ANIM_TAKING_OUT_DRUM) && Animation_OnFrame(&this->skelAnime, 14.0f) &&
              (this->action != EN_JG_ACTION_LULLABY_INTRO_CS)) ||
             (((this->csAnimIndex == EN_JG_ANIM_DRUM_IDLE) || (this->csAnimIndex == EN_JG_ANIM_PLAYING_DRUM)) &&
              (this->action == EN_JG_ACTION_LULLABY_INTRO_CS)))) {
            this->flags |= FLAG_DRUM_SPAWNED;
            this->drum = Actor_SpawnAsChildAndCutscene(
                &play->actorCtx, play, ACTOR_OBJ_JG_GAKKI, this->actor.world.pos.x, this->actor.world.pos.y,
                this->actor.world.pos.z, this->actor.shape.rot.x, this->actor.shape.rot.y, this->actor.shape.rot.z,
                this->actor.params, this->actor.csId, this->actor.halfDaysBits, NULL);
        }

        if (this->csAnimIndex == EN_JG_ANIM_TAKING_OUT_DRUM) {
            if (Animation_OnFrame(&this->skelAnime, 23.0f)) {
                Audio_PlaySfx_AtPos(&sSfxPos, NA_SE_EV_WOOD_BOUND_S);
            } else if (Animation_OnFrame(&this->skelAnime, 38.0f)) {
                Audio_PlaySfx_AtPos(&sSfxPos, NA_SE_EV_OBJECT_SLIDE);
            }
        }
    } else {
        this->cueId = 99;
        this->freezeTimer = 1000;
        SET_WEEKEVENTREG(WEEKEVENTREG_24_40);
        this->actionFunc = EnJg_Idle;
    }
}

/**
 * Returns the "next" text ID that the elder should use based on his current text ID.
 * Note that the results here can be counterintuitive, because his current text ID can
 * cause multiple text boxes to display in sequence; this function will return the text
 * ID that starts the *next* sequence.
 *
 * Sequences generally seem to be tied to text IDs that should all share the same animation.
 */
s32 EnJg_GetNextTextId(EnJg* this) {
    switch (this->textId) {
        case 0xDAC:       // What was I doing?
            return 0xDAD; // I must hurry!

        case 0xDAE:       // Do you have business with the Elder?
            return 0xDB0; // It's this cold snap

        case 0xDB0:       // It's this cold snap
            return 0xDB3; // As Elder, I must do something

        case 0xDB3:       // As Elder, I must do something
            return 0xDB4; // This is our problem (first)

        case 0xDB6:       // "Hunh???"
            return 0xDB7; // You're Darmani!

        case 0xDB7:       // You're Darmani!
            return 0xDBA; // I've been made a fool of!

        case 0xDBA:       // I've been made a fool of!
            return 0xDBB; // If I can see past the illusion, you'll vanish

        case 0xDBB:       // If I can see past the illusion, you'll vanish
        case 0xDBC:       // Following me won't do you any good
            return 0xDC6; // I am counting on you

        case 0xDCB:       // Welcome to spring
            return 0xDCC; // We're holding the Goron Races

        case 0xDDE:       // Come back after entering the race
            return 0xDDF; // My son is waiting for you

        case 0xDDF:       // My son is waiting for you
            return 0xDE0; // Go beyond Twin Islands Cave

        case 0xDCD:       // I've been waiting impatiently for you
            return 0xDCE; // I heard you sealed the blizzard

        case 0xDCE:       // I heard you sealed the blizzard
            return 0xDCF; // Spring has come thanks to you

        case 0xDCF: // Spring has come thanks to you
            this->focusedShrineGoronParam = 3;
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            CutsceneManager_Queue(this->csId);
            this->actionFunc = EnJg_GoronShrineCheer;
            return 0xDD0; // The greatest Goron hero of all?

        case 0xDD0: // The greatest Goron hero of all?
            EnJg_SetupGoronShrineCheer(this);
            return 0xDD1; // "Darmani!"

        case 0xDD1: // "Darmani!"
            switch (this->focusedShrineGoronParam) {
                case 3:
                    this->focusedShrineGoronParam = 4;
                    EnJg_SetupGoronShrineCheer(this);
                    return 0xDD2; // The immortal Goron?

                case 4:
                    this->focusedShrineGoronParam = 5;
                    EnJg_SetupGoronShrineCheer(this);
                    return 0xDD3; // The star we wish upon?

                case 5:
                    this->focusedShrineGoronParam = 6;
                    EnJg_SetupGoronShrineCheer(this);
                    return 0xDD4; // "Darmani, greatest of Gorons!"

                default:
                    return 0xDD4; // "Darmani, greatest of Gorons!"
            }
            break;

        case 0xDD2: // The immortal Goron?
            EnJg_SetupGoronShrineCheer(this);
            return 0xDD1; // "Darmani!"

        case 0xDD3: // The star we wish upon?
            EnJg_SetupGoronShrineCheer(this);
            return 0xDD1; // "Darmani!"

        case 0xDD4: // "Darmani, greatest of Gorons!"
            EnJg_SetupGoronShrineCheer(this);
            return 0xDD5; // "Greatest of Gorons!"

        case 0xDD5: // "Greatest of Gorons!"
            this->focusedShrineGoronParam = 7;
            EnJg_SetupGoronShrineCheer(this);
            return 0xDD6; // Darmani, greatest in the world!

        case 0xDD6: // Darmani, greatest in the world!
            EnJg_SetupGoronShrineCheer(this);
            return 0xDD7; // "Greatest in the world!"

        case 0xDD7: // "Greatest in the world!"
            this->focusedShrineGoronParam = 10;
            EnJg_SetupGoronShrineCheer(this);
            this->flags &= ~FLAG_SHRINE_GORON_ARMS_RAISED;
            return 0xDD8; // My son went to see the races

        case 0xDD8:       // My son went to see the races
            return 0xDD9; // I yield to a younger one

        case 0xDD9:       // I yield to a younger one
            return 0xDDA; // A Goron who takes care of all

        case 0xDDA:       // A Goron who takes care of all
            return 0xDDB; // The chosen one, Darmani!

        case 0xDDB:       // The chosen one, Darmani!
            return 0xDDC; // Everyone would accept you

        case 0xDDC: // Everyone would accept you
            SET_WEEKEVENTREG(WEEKEVENTREG_77_80);
            return 0xDDD; // Think it over slowly

        default:
            return 0;
    }
}

/**
 * Returns the text ID of the first thing the elder should say when you talk to him
 * under most circumstances. Some circumstances (like the very first time the player
 * talks to him after he thaws) bypass this function.
 */
s32 EnJg_GetStartingConversationTextId(EnJg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!EN_JG_IS_IN_GORON_SHRINE(&this->actor)) {
        if (player->transformation == PLAYER_FORM_GORON) {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_24_10) || CHECK_QUEST_ITEM(QUEST_SONG_LULLABY) ||
                CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO)) {
                // The player has already talked as a Goron at least once.
                return 0xDBC; // Following me won't do you any good
            }

            // The player has never talked as a Goron.
            return 0xDB6; // "Hunh???"
        }

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_24_20)) {
            // The player has already talked as a non-Goron at least once.
            return 0xDB5; // This is our problem (repeat)
        }

        // The player has never talked as a non-Goron.
        return 0xDAE; // Do you have business with the Elder?
    }

    if (player->transformation == PLAYER_FORM_GORON) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_77_80)) {
            // The player has heard the Goron Shrine cheer as a Goron at least once.
            return 0xDDE; // Come back after entering the race
        }

        // The player has never heard the Goron Shrine cheer as a Goron.
        return 0xDCD; // I've been waiting impatiently for you
    }

    // The player is talking to the elder in Goron Shrine as a non-Goron.
    return 0xDCB; // Welcome to spring
}

void EnJg_SpawnBreath(EnJg* this, PlayState* play) {
    s16 scale = (Rand_ZeroOne() * 20.0f) + 30.0f;

    if (play->state.frames % 8 == 0) {
        EffectSsIceSmoke_Spawn(play, &this->breathPos, &this->breathVelocity, &this->breathAccel, scale);
    }
}

/**
 * This function has two purposes:
 * - If the player starts talking to the elder, this stops him from moving, makes him face
 *   towards the player, updates certain weekEventRegs, and calls EnJg_SetupTalk.
 * - If the player is *not* talking to the elder, this decrements his freeze timer. If his
 *   freeze timer is 0 or negative, this function starts the process of freezing him before
 *   handing it off to EnJg_Freeze.
 */
void EnJg_CheckIfTalkingToPlayerAndHandleFreezeTimer(EnJg* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->flags |= FLAG_LOOKING_AT_PLAYER;
        this->actor.speed = 0.0f;

        if (this->textId == 0xDAC) {
            this->action = EN_JG_ACTION_FIRST_THAW;
        } else if (this->textId == 0xDAE) {
            SET_WEEKEVENTREG(WEEKEVENTREG_24_20);
        } else if (this->textId == 0xDB6) {
            SET_WEEKEVENTREG(WEEKEVENTREG_24_10);
        }

        Message_StartTextbox(play, this->textId, &this->actor);
        this->actionFunc = EnJg_SetupTalk;
    } else {
        if ((this->actor.xzDistToPlayer < 100.0f) || this->actor.isLockedOn) {
            Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
            if (this->action == EN_JG_ACTION_FIRST_THAW) {
                this->textId = EnJg_GetStartingConversationTextId(this, play);
            }
        }

        this->freezeTimer--;
        if ((this->freezeTimer <= 0) && (curFrame == endFrame)) {
            this->animIndex = EN_JG_ANIM_FROZEN_START;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
            Audio_PlaySfx_AtPos(&sSfxPos, NA_SE_EV_FREEZE_S);
            this->actionFunc = EnJg_Freeze;
        }
    }
}

void EnJg_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnJg* this = (EnJg*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGoronElderSkel, &gGoronElderIdleAnim, this->jointTable,
                       this->morphTable, GORON_ELDER_LIMB_MAX);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    Actor_SetScale(&this->actor, 0.01f);

    if (!EN_JG_IS_IN_GORON_SHRINE(thisx)) {
        if ((play->sceneId == SCENE_SPOT00) && (gSaveContext.sceneLayer == 7) && (play->csCtx.scriptIndex == 0)) {
            // This is the elder that appears in the cutscene for learning the full Goron Lullaby.
            this->animIndex = EN_JG_ANIM_IDLE;
            this->action = EN_JG_ACTION_LULLABY_INTRO_CS;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnJg_LullabyIntroCutsceneAction;
        } else {
            // This is the elder that appears in Mountain Village or the Path to Goron Village in winter.
            this->path = SubS_GetPathByIndex(play, EN_JG_GET_PATH_INDEX(thisx), EN_JG_PATH_INDEX_NONE);
            this->animIndex = EN_JG_ANIM_SURPRISE_START;
            this->action = EN_JG_ACTION_SPAWNING;
            this->freezeTimer = 1000;
            this->textId = 0xDAC; // What was I doing?
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnJg_Freeze;
        }
    } else {
        // This is the elder that appears in Goron Shrine in spring.
        this->animIndex = EN_JG_ANIM_IDLE;
        this->csId = this->actor.csId;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        this->actionFunc = EnJg_GoronShrineIdle;
    }
}

void EnJg_Destroy(Actor* thisx, PlayState* play) {
    EnJg* this = (EnJg*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnJg_Update(Actor* thisx, PlayState* play) {
    EnJg* this = (EnJg*)thisx;

    if ((this->actionFunc != EnJg_FrozenIdle) && (this->actionFunc != EnJg_EndFrozenInteraction)) {
        EnJg_UpdateCollision(this, play);
        Actor_MoveWithGravity(&this->actor);
        SkelAnime_Update(&this->skelAnime);

        if ((this->action != EN_JG_ACTION_LULLABY_INTRO_CS) && (!EN_JG_IS_IN_GORON_SHRINE(&this->actor))) {
            EnJg_SpawnBreath(this, play);
        }

        Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    }
    this->actionFunc(this, play);
}

s32 EnJg_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnJg* this = (EnJg*)thisx;

    if (limbIndex == GORON_ELDER_LIMB_ROOT) {
        if (this->flags & FLAG_LOOKING_AT_PLAYER) {
            Math_SmoothStepToS(&this->rootRotationWhenTalking, this->actor.yawTowardsPlayer - this->actor.shape.rot.y,
                               5, 0x1000, 0x100);
            Matrix_RotateYS(this->rootRotationWhenTalking, MTXMODE_APPLY);
        } else {
            Math_SmoothStepToS(&this->rootRotationWhenTalking, 0, 5, 0x1000, 0x100);
            Matrix_RotateYS(this->rootRotationWhenTalking, MTXMODE_APPLY);
        }
    }

    return false;
}

void EnJg_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnJg* this = (EnJg*)thisx;

    if (limbIndex == GORON_ELDER_LIMB_HEAD) {
        Matrix_MultVec3f(&sFocusOffset, &this->actor.focus.pos);
    }

    if (limbIndex == GORON_ELDER_LIMB_LOWER_LIP) {
        Matrix_MultVec3f(&sBreathPosOffset, &this->breathPos);
        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
        Matrix_MultVec3f(&sBreathVelOffset, &this->breathVelocity);
        Matrix_MultVec3f(&sBreathAccelOffset, &this->breathAccel);
    }
}

void EnJg_Draw(Actor* thisx, PlayState* play) {
    EnJg* this = (EnJg*)thisx;

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnJg_OverrideLimbDraw, EnJg_PostLimbDraw, &this->actor);
}
