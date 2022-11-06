/*
 * File: z_en_ge2.c
 * Overlay: ovl_En_Ge2
 * Description: Purple-clothed Gerudo
 */

#include "z_en_ge2.h"
#include "vt.h"
#include "objects/object_gla/object_gla.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

#define GE2_STATE_ANIMCOMPLETE (1 << 1)
#define GE2_STATE_KO (1 << 2)
#define GE2_STATE_CAPTURING (1 << 3)
#define GE2_STATE_TALKED (1 << 4)

typedef enum {
    /* 0 */ GE2_TYPE_PATROLLING,
    /* 1 */ GE2_TYPE_STATIONARY,
    /* 2 */ GE2_TYPE_GERUDO_CARD_GIVER
} EnGe2Type;

typedef enum {
    /* 0 */ GE2_ACTION_WALK,
    /* 1 */ GE2_ACTION_ABOUTTURN,
    /* 2 */ GE2_ACTION_TURNPLAYERSPOTTED,
    /* 3 */ GE2_ACTION_KNOCKEDOUT,
    /* 4 */ GE2_ACTION_CAPTURETURN,
    /* 5 */ GE2_ACTION_CAPTURECHARGE,
    /* 6 */ GE2_ACTION_CAPTURECLOSE,
    /* 7 */ GE2_ACTION_STAND,
    /* 8 */ GE2_ACTION_WAITLOOKATPLAYER
} EnGe2Action;

void EnGe2_Init(Actor* thisx, PlayState* play);
void EnGe2_Destroy(Actor* thisx, PlayState* play);
void EnGe2_Update(Actor* thisx, PlayState* play);
void EnGe2_Draw(Actor* thisx, PlayState* play);

s32 EnGe2_CheckCarpentersFreed(void);
void EnGe2_CaptureClose(EnGe2* this, PlayState* play);
void EnGe2_CaptureCharge(EnGe2* this, PlayState* play);
void EnGe2_CaptureTurn(EnGe2* this, PlayState* play);
void EnGe2_KnockedOut(EnGe2* this, PlayState* play);
void EnGe2_TurnPlayerSpotted(EnGe2* this, PlayState* play);
void EnGe2_AboutTurn(EnGe2* this, PlayState* play);
void EnGe2_Walk(EnGe2* this, PlayState* play);
void EnGe2_Stand(EnGe2* this, PlayState* play);
void EnGe2_WaitLookAtPlayer(EnGe2* this, PlayState* play);
void EnGe2_ForceTalk(EnGe2* this, PlayState* play);

// Update functions
void EnGe2_UpdateFriendly(Actor* thisx, PlayState* play);
void EnGe2_UpdateAfterTalk(Actor* thisx, PlayState* play);
void EnGe2_UpdateStunned(Actor* thisx, PlayState* play);

const ActorInit En_Ge2_InitVars = {
    ACTOR_EN_GE2,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GLA,
    sizeof(EnGe2),
    (ActorFunc)EnGe2_Init,
    (ActorFunc)EnGe2_Destroy,
    (ActorFunc)EnGe2_Update,
    (ActorFunc)EnGe2_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x000007A2, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 60, 0, { 0, 0, 0 } },
};

static EnGe2ActionFunc sActionFuncs[] = {
    EnGe2_Walk,         EnGe2_AboutTurn,   EnGe2_TurnPlayerSpotted,
    EnGe2_KnockedOut,   EnGe2_CaptureTurn, EnGe2_CaptureCharge,
    EnGe2_CaptureClose, EnGe2_Stand,       EnGe2_WaitLookAtPlayer,
};

static AnimationHeader* sAnimations[] = {
    &gGerudoPurpleWalkingAnim,         &gGerudoPurpleLookingAboutAnim, &gGerudoPurpleLookingAboutAnim,
    &gGerudoPurpleFallingToGroundAnim, &gGerudoPurpleLookingAboutAnim, &gGerudoPurpleChargingAnim,
    &gGerudoPurpleLookingAboutAnim,    &gGerudoPurpleLookingAboutAnim, &gGerudoPurpleLookingAboutAnim,
};

static u8 sAnimModes[] = {
    ANIMMODE_LOOP, ANIMMODE_ONCE, ANIMMODE_LOOP, ANIMMODE_ONCE, ANIMMODE_LOOP,
    ANIMMODE_LOOP, ANIMMODE_LOOP, ANIMMODE_LOOP, ANIMMODE_ONCE,
};

void EnGe2_ChangeAction(EnGe2* this, s32 i) {
    this->actionFunc = sActionFuncs[i];
    Animation_Change(&this->skelAnime, sAnimations[i], 1.0f, 0.0f, Animation_GetLastFrame(sAnimations[i]),
                     sAnimModes[i], -8.0f);
    this->stateFlags &= ~GE2_STATE_ANIMCOMPLETE;
}

void EnGe2_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGe2* this = (EnGe2*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGerudoPurpleSkel, NULL, this->jointTable, this->morphTable, 22);
    Animation_PlayLoop(&this->skelAnime, &gGerudoPurpleWalkingAnim);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->actor, 0.01f);

    if (play->sceneNum == SCENE_SPOT09) {
        this->actor.uncullZoneForward = 1000.0f;
    } else {
        this->actor.uncullZoneForward = 1200.0f;
    }

    this->yDetectRange = (this->actor.world.rot.z + 1) * 40.0f;
    this->actor.world.rot.z = 0;
    this->actor.shape.rot.z = 0;

    switch (this->actor.params & 0xFF) {
        case GE2_TYPE_PATROLLING:
            EnGe2_ChangeAction(this, GE2_ACTION_WALK);
            if (EnGe2_CheckCarpentersFreed()) {
                this->actor.update = EnGe2_UpdateFriendly;
                this->actor.targetMode = 6;
            }
            break;
        case GE2_TYPE_STATIONARY:
            EnGe2_ChangeAction(this, GE2_ACTION_STAND);
            if (EnGe2_CheckCarpentersFreed()) {
                this->actor.update = EnGe2_UpdateFriendly;
                this->actor.targetMode = 6;
            }
            break;
        case GE2_TYPE_GERUDO_CARD_GIVER:
            EnGe2_ChangeAction(this, GE2_ACTION_WAITLOOKATPLAYER);
            this->actor.update = EnGe2_UpdateAfterTalk;
            this->actionFunc = EnGe2_ForceTalk;
            this->actor.targetMode = 6;
            break;
        default:
            ASSERT(0);
    }

    this->stateFlags = 0;
    this->unk_304 = 0; // Set and not used
    this->walkTimer = 0;
    this->playerSpottedParam = 0;
    this->actor.minVelocityY = -4.0f;
    this->actor.gravity = -1.0f;
    this->walkDirection = this->actor.world.rot.y;
    this->walkDuration = ((this->actor.params & 0xFF00) >> 8) * 10;
}

void EnGe2_Destroy(Actor* thisx, PlayState* play) {
    EnGe2* this = (EnGe2*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

// Detection/check functions

s32 Ge2_DetectPlayerInAction(PlayState* play, EnGe2* this) {
    f32 visionScale;

    visionScale = (!IS_DAY ? 0.75f : 1.5f);

    if ((250.0f * visionScale) < this->actor.xzDistToPlayer) {
        return 0;
    }

    if (this->actor.xzDistToPlayer < 50.0f) {
        return 2;
    }

    if (func_8002DDE4(play)) {
        return 1;
    }
    return 0;
}

s32 Ge2_DetectPlayerInUpdate(PlayState* play, EnGe2* this, Vec3f* pos, s16 yRot, f32 yDetectRange) {
    Player* player = GET_PLAYER(play);
    Vec3f posResult;
    CollisionPoly* outPoly;
    f32 visionScale;

    visionScale = (!IS_DAY ? 0.75f : 1.5f);

    if ((250.0f * visionScale) < this->actor.xzDistToPlayer) {
        return 0;
    }

    if (yDetectRange < ABS(this->actor.yDistToPlayer)) {
        return 0;
    }

    if (ABS((s16)(this->actor.yawTowardsPlayer - yRot)) > 0x2000) {
        return 0;
    }

    if (BgCheck_AnyLineTest1(&play->colCtx, pos, &player->bodyPartsPos[7], &posResult, &outPoly, 0)) {
        return 0;
    }
    return 1;
}

s32 EnGe2_CheckCarpentersFreed(void) {
    if (gSaveContext.n64ddFlag) {
        if (CHECK_QUEST_ITEM(QUEST_GERUDO_CARD)) {
            return 1;
        } else {
            return 0;
        }
    } 

    if ((u8)(gSaveContext.eventChkInf[9] & 0xF) == 0xF) {
        return 1;
    }
    return 0;
}

// Actions

void EnGe2_CaptureClose(EnGe2* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
    } else {
        func_8006D074(play);

        if ((INV_CONTENT(ITEM_HOOKSHOT) == ITEM_NONE) || (INV_CONTENT(ITEM_LONGSHOT) == ITEM_NONE)) {
            play->nextEntranceIndex = 0x1A5;
        } else if (gSaveContext.eventChkInf[12] & 0x80) {
            play->nextEntranceIndex = 0x5F8;
        } else {
            play->nextEntranceIndex = 0x3B4;
        }

        play->fadeTransition = 0x26;
        play->sceneLoadFlag = 0x14;
    }
}

void EnGe2_CaptureCharge(EnGe2* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 2, 0x400, 0x100);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if (this->actor.xzDistToPlayer < 50.0f) {
        EnGe2_ChangeAction(this, GE2_ACTION_CAPTURECLOSE);
        this->actor.speedXZ = 0.0f;
    }

    if (this->timer > 0) {
        this->timer--;
    } else {
        func_8006D074(play);

        if ((INV_CONTENT(ITEM_HOOKSHOT) == ITEM_NONE) || (INV_CONTENT(ITEM_LONGSHOT) == ITEM_NONE)) {
            play->nextEntranceIndex = 0x1A5;
        } else if (gSaveContext.eventChkInf[12] & 0x80) {
            play->nextEntranceIndex = 0x5F8;
        } else {
            play->nextEntranceIndex = 0x3B4;
        }

        play->fadeTransition = 0x26;
        play->sceneLoadFlag = 0x14;
    }
}

void EnGe2_CaptureTurn(EnGe2* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 2, 0x400, 0x100);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->actor.world.rot.y == this->actor.yawTowardsPlayer) {
        EnGe2_ChangeAction(this, GE2_ACTION_CAPTURECHARGE);
        this->timer = 50;
        this->actor.speedXZ = 4.0f;
    }
}

void EnGe2_KnockedOut(EnGe2* this, PlayState* play) {
    static Vec3f effectVelocity = { 0.0f, -0.05f, 0.0f };
    static Vec3f effectAccel = { 0.0f, -0.025f, 0.0f };
    static Color_RGBA8 effectPrimColor = { 255, 255, 255, 0 };
    static Color_RGBA8 effectEnvColor = { 255, 150, 0, 0 };
    s32 effectAngle;
    Vec3f effectPos;

    this->actor.flags &= ~ACTOR_FLAG_0;
    if (this->stateFlags & GE2_STATE_ANIMCOMPLETE) {
        effectAngle = (play->state.frames) * 0x2800;
        effectPos.x = this->actor.focus.pos.x + (Math_CosS(effectAngle) * 5.0f);
        effectPos.y = this->actor.focus.pos.y + 10.0f;
        effectPos.z = this->actor.focus.pos.z + (Math_SinS(effectAngle) * 5.0f);
        EffectSsKiraKira_SpawnDispersed(play, &effectPos, &effectVelocity, &effectAccel, &effectPrimColor,
                                        &effectEnvColor, 1000, 16);
    }
}

void EnGe2_TurnPlayerSpotted(EnGe2* this, PlayState* play) {
    s32 playerSpotted;

    this->actor.speedXZ = 0.0f;

    if (this->stateFlags & GE2_STATE_TALKED) {
        this->stateFlags &= ~GE2_STATE_TALKED;
    } else {
        playerSpotted = Ge2_DetectPlayerInAction(play, this);

        if (playerSpotted != 0) {
            this->timer = 100;
            this->yawTowardsPlayer = this->actor.yawTowardsPlayer;

            if (this->playerSpottedParam < playerSpotted) {
                this->playerSpottedParam = playerSpotted;
            }
        } else if (this->actor.world.rot.y == this->yawTowardsPlayer) {
            this->playerSpottedParam = 0;
            EnGe2_ChangeAction(this, GE2_ACTION_ABOUTTURN);
            return;
        }
    }

    switch (this->playerSpottedParam) {
        case 1:
            Math_SmoothStepToS(&this->actor.world.rot.y, this->yawTowardsPlayer, 2, 0x200, 0x100);
            break;
        case 2:
            Math_SmoothStepToS(&this->actor.world.rot.y, this->yawTowardsPlayer, 2, 0x600, 0x180);
            break;
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void EnGe2_AboutTurn(EnGe2* this, PlayState* play) {
    s32 playerSpotted;

    this->actor.speedXZ = 0.0f;
    playerSpotted = Ge2_DetectPlayerInAction(play, this);

    if (playerSpotted != 0) {
        EnGe2_ChangeAction(this, GE2_ACTION_TURNPLAYERSPOTTED);
        this->timer = 100;
        this->playerSpottedParam = playerSpotted;
        this->yawTowardsPlayer = this->actor.yawTowardsPlayer;
    } else if (this->stateFlags & GE2_STATE_ANIMCOMPLETE) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->walkDirection, 2, 0x400, 0x200);
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    if (this->actor.shape.rot.y == this->walkDirection) {
        EnGe2_ChangeAction(this, GE2_ACTION_WALK);
    }
}

void EnGe2_Walk(EnGe2* this, PlayState* play) {
    u8 playerSpotted;

    playerSpotted = Ge2_DetectPlayerInAction(play, this);
    if (playerSpotted != 0) {
        this->actor.speedXZ = 0.0f;
        EnGe2_ChangeAction(this, GE2_ACTION_TURNPLAYERSPOTTED);
        this->timer = 100;
        this->playerSpottedParam = playerSpotted;
        this->yawTowardsPlayer = this->actor.yawTowardsPlayer;
    } else if (this->walkTimer >= this->walkDuration) {
        this->walkTimer = 0;
        this->walkDirection += 0x8000;
        EnGe2_ChangeAction(this, GE2_ACTION_ABOUTTURN);
        this->actor.speedXZ = 0.0f;
    } else {
        this->walkTimer++;
        this->actor.speedXZ = 2.0f;
    }
}

void EnGe2_Stand(EnGe2* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.world.rot.y, this->walkDirection, 2, 0x400, 0x200);
}

void EnGe2_TurnToFacePlayer(EnGe2* this, PlayState* play) {
    s32 pad;
    s16 angleDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (ABS(angleDiff) <= 0x4000) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 6, 4000, 100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        func_80038290(play, &this->actor, &this->headRot, &this->unk_2EE, this->actor.focus.pos);
    } else {
        if (angleDiff < 0) {
            Math_SmoothStepToS(&this->headRot.y, -0x2000, 6, 6200, 0x100);
        } else {
            Math_SmoothStepToS(&this->headRot.y, 0x2000, 6, 6200, 0x100);
        }

        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 12, 1000, 100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
}

void EnGe2_LookAtPlayer(EnGe2* this, PlayState* play) {
    if ((ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) <= 0x4300) &&
        (this->actor.xzDistToPlayer < 200.0f)) {
        func_80038290(play, &this->actor, &this->headRot, &this->unk_2EE, this->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&this->headRot.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->headRot.y, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->unk_2EE.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->unk_2EE.y, 0, 6, 6200, 100);
    }
}

void EnGe2_SetActionAfterTalk(EnGe2* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {

        switch (this->actor.params & 0xFF) {
            case GE2_TYPE_PATROLLING:
                EnGe2_ChangeAction(this, GE2_ACTION_ABOUTTURN);
                break;
            case GE2_TYPE_STATIONARY:
                EnGe2_ChangeAction(this, GE2_ACTION_STAND);
                break;
            case GE2_TYPE_GERUDO_CARD_GIVER:
                this->actionFunc = EnGe2_WaitLookAtPlayer;
                break;
        }
        this->actor.update = EnGe2_UpdateFriendly;
        this->actor.flags &= ~ACTOR_FLAG_16;
    }
    EnGe2_TurnToFacePlayer(this, play);
}

void EnGe2_WaitLookAtPlayer(EnGe2* this, PlayState* play) {
    EnGe2_LookAtPlayer(this, play);
}

void EnGe2_WaitTillCardGiven(EnGe2* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = EnGe2_SetActionAfterTalk;
    } else {
        if (!gSaveContext.n64ddFlag) {
            func_8002F434(&this->actor, play, GI_GERUDO_CARD, 10000.0f, 50.0f);
        } else {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_GF_GERUDO_MEMBERSHIP_CARD, GI_GERUDO_CARD);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, 10000.0f, 50.0f);
        }
    }
}

void EnGe2_GiveCard(EnGe2* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        this->actor.flags &= ~ACTOR_FLAG_16;
        this->actionFunc = EnGe2_WaitTillCardGiven;
         if (!gSaveContext.n64ddFlag) {
            func_8002F434(&this->actor, play, GI_GERUDO_CARD, 10000.0f, 50.0f);
        } else {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_GF_GERUDO_MEMBERSHIP_CARD, GI_GERUDO_CARD);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, 10000.0f, 50.0f);
        }
    }
}

void EnGe2_ForceTalk(EnGe2* this, PlayState* play) {

    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->actionFunc = EnGe2_GiveCard;
    } else {
        this->actor.textId = 0x6004;
        this->actor.flags |= ACTOR_FLAG_16;
        func_8002F1C4(&this->actor, play, 300.0f, 300.0f, 0);
    }
    EnGe2_LookAtPlayer(this, play);
}

void EnGe2_SetupCapturePlayer(EnGe2* this, PlayState* play) {
    this->stateFlags |= GE2_STATE_CAPTURING;
    this->actor.speedXZ = 0.0f;
    EnGe2_ChangeAction(this, GE2_ACTION_CAPTURETURN);
    func_8002DF54(play, &this->actor, 95);
    func_80078884(NA_SE_SY_FOUND);
    Message_StartTextbox(play, 0x6000, &this->actor);
}

void EnGe2_MaintainColliderAndSetAnimState(EnGe2* this, PlayState* play) {
    s32 pad;
    s32 pad2;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(play, &this->actor, 40.0f, 25.0f, 40.0f, 5);

    if (!(this->stateFlags & GE2_STATE_ANIMCOMPLETE) && SkelAnime_Update(&this->skelAnime)) {
        this->stateFlags |= GE2_STATE_ANIMCOMPLETE;
    }
}

void EnGe2_MoveAndBlink(EnGe2* this, PlayState* play) {
    Actor_MoveForward(&this->actor);

    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = Rand_S16Offset(60, 60);
    }
    this->eyeIndex = this->blinkTimer;

    if (this->eyeIndex >= 3) {
        this->eyeIndex = 0;
    }
}

// Update functions

void EnGe2_UpdateFriendly(Actor* thisx, PlayState* play) {
    EnGe2* this = (EnGe2*)thisx;

    EnGe2_MaintainColliderAndSetAnimState(this, play);
    this->actionFunc(this, play);

    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        if ((this->actor.params & 0xFF) == GE2_TYPE_PATROLLING) {
            this->actor.speedXZ = 0.0f;
            EnGe2_ChangeAction(this, GE2_ACTION_WAITLOOKATPLAYER);
        }
        this->actionFunc = EnGe2_SetActionAfterTalk;
        this->actor.update = EnGe2_UpdateAfterTalk;
    } else {
        this->actor.textId = 0x6005;

        if (this->actor.xzDistToPlayer < 100.0f) {
            func_8002F2CC(&this->actor, play, 100.0f);
        }
    }
    EnGe2_MoveAndBlink(this, play);
}

void EnGe2_UpdateAfterTalk(Actor* thisx, PlayState* play) {
    EnGe2* this = (EnGe2*)thisx;

    this->stateFlags |= GE2_STATE_TALKED;
    EnGe2_MaintainColliderAndSetAnimState(this, play);
    this->actionFunc(this, play);
    EnGe2_MoveAndBlink(this, play);
}

void EnGe2_Update(Actor* thisx, PlayState* play) {
    EnGe2* this = (EnGe2*)thisx;
    s32 paramsType;

    EnGe2_MaintainColliderAndSetAnimState(this, play);

    if ((this->stateFlags & GE2_STATE_KO) || (this->stateFlags & GE2_STATE_CAPTURING)) {
        this->actionFunc(this, play);
    } else if (this->collider.base.acFlags & 2) {
        if ((this->collider.info.acHitInfo != NULL) && (this->collider.info.acHitInfo->toucher.dmgFlags & 0x80)) {
            Actor_SetColorFilter(&this->actor, 0, 120, 0, 400);
            this->actor.update = EnGe2_UpdateStunned;
            return;
        }

        EnGe2_ChangeAction(this, GE2_ACTION_KNOCKEDOUT);
        this->timer = 100;
        this->stateFlags |= GE2_STATE_KO;
        this->actor.speedXZ = 0.0f;
        Audio_PlayActorSound2(&this->actor, NA_SE_VO_SK_CRASH);
    } else {
        this->actionFunc(this, play);

        if (Ge2_DetectPlayerInUpdate(play, this, &this->actor.focus.pos, this->actor.shape.rot.y,
                                     this->yDetectRange)) {
            // "Discovered!"
            osSyncPrintf(VT_FGCOL(GREEN) "発見!!!!!!!!!!!!\n" VT_RST);
            EnGe2_SetupCapturePlayer(this, play);
        }

        if (((this->actor.params & 0xFF) == GE2_TYPE_STATIONARY) && (this->actor.xzDistToPlayer < 100.0f)) {
            // "Discovered!"
            osSyncPrintf(VT_FGCOL(GREEN) "発見!!!!!!!!!!!!\n" VT_RST);
            EnGe2_SetupCapturePlayer(this, play);
        }
    }

    if (!(this->stateFlags & GE2_STATE_KO)) {
        paramsType = this->actor.params & 0xFF; // Not necessary, but looks a bit nicer
        if ((paramsType == GE2_TYPE_PATROLLING) || (paramsType == GE2_TYPE_STATIONARY)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
    EnGe2_MoveAndBlink(this, play);

    if (EnGe2_CheckCarpentersFreed() && !(this->stateFlags & GE2_STATE_KO)) {
        this->actor.update = EnGe2_UpdateFriendly;
        this->actor.targetMode = 6;
    }
}

void EnGe2_UpdateStunned(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnGe2* this = (EnGe2*)thisx;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(play, &this->actor, 40.0f, 25.0f, 40.0f, 5);

    if ((this->collider.base.acFlags & 2) &&
        ((this->collider.info.acHitInfo == NULL) || !(this->collider.info.acHitInfo->toucher.dmgFlags & 0x80))) {
        this->actor.colorFilterTimer = 0;
        EnGe2_ChangeAction(this, GE2_ACTION_KNOCKEDOUT);
        this->timer = 100;
        this->stateFlags |= GE2_STATE_KO;
        this->actor.speedXZ = 0.0f;
        Audio_PlayActorSound2(&this->actor, NA_SE_VO_SK_CRASH);
    }
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);

    if (EnGe2_CheckCarpentersFreed()) {
        this->actor.update = EnGe2_UpdateFriendly;
        this->actor.targetMode = 6;
        this->actor.colorFilterTimer = 0;
    } else if (this->actor.colorFilterTimer == 0) {
        this->actor.update = EnGe2_Update;
    }
}

s32 EnGe2_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnGe2* this = (EnGe2*)thisx;

    if (limbIndex == 3) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.x;
    }
    return 0;
}

void EnGe2_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f D_80A343B0 = { 600.0f, 700.0f, 0.0f };
    EnGe2* this = (EnGe2*)thisx;

    if (limbIndex == 6) {
        Matrix_MultVec3f(&D_80A343B0, &this->actor.focus.pos);
    }
}

void EnGe2_Draw(Actor* thisx, PlayState* play) {
    static void* eyeTextures[] = { gGerudoPurpleEyeOpenTex, gGerudoPurpleEyeHalfTex, gGerudoPurpleEyeClosedTex };
    s32 pad;
    EnGe2* this = (EnGe2*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    func_800943C8(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeIndex]));
    func_8002EBCC(&this->actor, play, 0);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnGe2_OverrideLimbDraw, EnGe2_PostLimbDraw, this);

    CLOSE_DISPS(play->state.gfxCtx);
}
