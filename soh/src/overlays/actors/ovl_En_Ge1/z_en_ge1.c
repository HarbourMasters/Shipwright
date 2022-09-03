/*
 * File: z_en_ge1.c
 * Overlay: ovl_En_Ge1
 * Description: White-clothed Gerudo
 */

#include "z_en_ge1.h"
#include "vt.h"
#include "objects/object_ge1/object_ge1.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

#define GE1_STATE_TALKING (1 << 0)
#define GE1_STATE_GIVE_QUIVER (1 << 1)
#define GE1_STATE_IDLE_ANIM (1 << 2)
#define GE1_STATE_STOP_FIDGET (1 << 3)

typedef enum {
    /* 00 */ GE1_HAIR_BOB,
    /* 01 */ GE1_HAIR_STRAIGHT,
    /* 02 */ GE1_HAIR_SPIKY
} EnGe1Hairstyle;

void EnGe1_Init(Actor* thisx, GlobalContext* globalCtx);
void EnGe1_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnGe1_Update(Actor* thisx, GlobalContext* globalCtx);
void EnGe1_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnGe1_WaitTillItemGiven_Archery(EnGe1* this, GlobalContext* globalCtx);
void EnGe1_BeginGiveItem_Archery(EnGe1* this, GlobalContext* globalCtx);

s32 EnGe1_CheckCarpentersFreed(void);
void EnGe1_WatchForPlayerFrontOnly(EnGe1* this, GlobalContext* globalCtx);
void EnGe1_SetNormalText(EnGe1* this, GlobalContext* globalCtx);
void EnGe1_WatchForAndSensePlayer(EnGe1* this, GlobalContext* globalCtx);
void EnGe1_GetReaction_ValleyFloor(EnGe1* this, GlobalContext* globalCtx);
void EnGe1_CheckForCard_GTGGuard(EnGe1* this, GlobalContext* globalCtx);
void EnGe1_CheckGate_GateOp(EnGe1* this, GlobalContext* globalCtx);
void EnGe1_GetReaction_GateGuard(EnGe1* this, GlobalContext* globalCtx);
void EnGe1_TalkAfterGame_Archery(EnGe1* this, GlobalContext* globalCtx);
void EnGe1_Wait_Archery(EnGe1* this, GlobalContext* globalCtx);
void EnGe1_CueUpAnimation(EnGe1* this);
void EnGe1_StopFidget(EnGe1* this);
void EnGe1_MoveForRandomizer(EnGe1* this, GlobalContext* globalCtx);

const ActorInit En_Ge1_InitVars = {
    ACTOR_EN_GE1,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GE1,
    sizeof(EnGe1),
    (ActorFunc)EnGe1_Init,
    (ActorFunc)EnGe1_Destroy,
    (ActorFunc)EnGe1_Update,
    (ActorFunc)EnGe1_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000702, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 40, 0, { 0, 0, 0 } },
};

static Gfx* sHairstyleDLists[] = {
    gGerudoWhiteHairstyleBobDL,
    gGerudoWhiteHairstyleStraightFringeDL,
    gGerudoWhiteHairstyleSpikyDL,
};

static Vec3f D_80A327A8 = { 600.0f, 700.0f, 0.0f };

static void* sEyeTextures[] = {
    gGerudoWhiteEyeOpenTex,
    gGerudoWhiteEyeHalfTex,
    gGerudoWhiteEyeClosedTex,
};

void EnGe1_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnGe1* this = (EnGe1*)thisx;

    if (gSaveContext.n64ddFlag) {
        EnGe1_MoveForRandomizer(thisx, globalCtx);
    }

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    SkelAnime_InitFlex(globalCtx, &this->skelAnime, &gGerudoWhiteSkel, &gGerudoWhiteIdleAnim, this->jointTable,
                       this->morphTable, GE1_LIMB_MAX);
    Animation_PlayOnce(&this->skelAnime, &gGerudoWhiteIdleAnim);
    Collider_InitCylinder(globalCtx, &this->collider);
    Collider_SetCylinder(globalCtx, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->animation = &gGerudoWhiteIdleAnim;
    this->animFunc = EnGe1_CueUpAnimation;
    this->actor.targetMode = 6;
    Actor_SetScale(&this->actor, 0.01f);

    // In Gerudo Valley
    this->actor.uncullZoneForward = ((globalCtx->sceneNum == SCENE_SPOT09) ? 1000.0f : 1200.0f);

    switch (this->actor.params & 0xFF) {

        case GE1_TYPE_GATE_GUARD:
            this->hairstyle = GE1_HAIR_SPIKY;
            if (!gSaveContext.n64ddFlag) { //todo make entrance specific
                this->actionFunc = EnGe1_GetReaction_GateGuard;
                break;
            }

            else if (gSaveContext.n64ddFlag) {

                if (EnGe1_CheckCarpentersFreed()) {
                    this->actionFunc = EnGe1_CheckGate_GateOp;
                } else {
                    this->actionFunc = EnGe1_WatchForPlayerFrontOnly;
                }
                break;
            }
        case GE1_TYPE_GATE_OPERATOR:
            this->hairstyle = GE1_HAIR_STRAIGHT;

            if (EnGe1_CheckCarpentersFreed()) {
                this->actionFunc = EnGe1_CheckGate_GateOp;
            } else {
                this->actionFunc = EnGe1_WatchForPlayerFrontOnly;
            }
            break;

        case GE1_TYPE_NORMAL:
            this->hairstyle = GE1_HAIR_STRAIGHT;
            if (gSaveContext.n64ddFlag) {
                if (EnGe1_CheckCarpentersFreed()) {
                    this->actionFunc = EnGe1_CheckGate_GateOp;
                } else {
                    this->actionFunc = EnGe1_WatchForPlayerFrontOnly;
                }
                break;
            } else {

                if (EnGe1_CheckCarpentersFreed()) {
                    this->actionFunc = EnGe1_SetNormalText;
                } else {
                    this->actionFunc = EnGe1_WatchForAndSensePlayer;
                }
                break;
            }

        case GE1_TYPE_VALLEY_FLOOR:
            if (LINK_IS_ADULT) {
                // "Valley floor Gerudo withdrawal"
                osSyncPrintf(VT_FGCOL(CYAN) "谷底 ゲルド 撤退 \n" VT_RST);
                Actor_Kill(&this->actor);
                return;
            }
            this->hairstyle = GE1_HAIR_BOB;
            this->actionFunc = EnGe1_GetReaction_ValleyFloor;
            break;

        case GE1_TYPE_HORSEBACK_ARCHERY:
            if (INV_CONTENT(SLOT_BOW) == ITEM_NONE) {
                Actor_Kill(&this->actor);
                return;
            }
            this->actor.targetMode = 3;
            this->hairstyle = GE1_HAIR_BOB;
            // "Horseback archery Gerudo EVENT_INF(0) ="
            osSyncPrintf(VT_FGCOL(CYAN) "やぶさめ ゲルド EVENT_INF(0) = %x\n" VT_RST, gSaveContext.eventInf[0]);

            if (gSaveContext.eventInf[0] & 0x100) {
                this->actionFunc = EnGe1_TalkAfterGame_Archery;
            } else if (EnGe1_CheckCarpentersFreed()) {
                this->actionFunc = EnGe1_Wait_Archery;
            } else {
                this->actionFunc = EnGe1_WatchForPlayerFrontOnly;
            }
            break;

        case GE1_TYPE_TRAINING_GROUNDS_GUARD:
            this->hairstyle = GE1_HAIR_STRAIGHT;

            if (EnGe1_CheckCarpentersFreed()) {
                this->actionFunc = EnGe1_CheckForCard_GTGGuard;
            } else {
                this->actionFunc = EnGe1_WatchForPlayerFrontOnly;
            }
            break;
    }

    this->stateFlags = 0;
}

void EnGe1_MoveForRandomizer(EnGe1* this, GlobalContext* globalCtx) {

    // Move left cow in lon lon tower
    if (globalCtx->sceneNum == 0x5D && this->actor.world.pos.x == -857 && this->actor.world.pos.z == -3123) {
        this->actor.world.pos.x = -1224.0f;
        this->actor.world.pos.z = -3160.0f;
        this->actor.world.pos.y = 93.291f;
        this->actor.shape.rot.y = -23120.0f;
        
    }
}


void EnGe1_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnGe1* this = (EnGe1*)thisx;

    Collider_DestroyCylinder(globalCtx, &this->collider);
}

s32 EnGe1_SetTalkAction(EnGe1* this, GlobalContext* globalCtx, u16 textId, f32 arg3, EnGe1ActionFunc actionFunc) {
    if (Actor_ProcessTalkRequest(&this->actor, globalCtx)) {
        this->actionFunc = actionFunc;
        this->animFunc = EnGe1_StopFidget;
        this->stateFlags &= ~GE1_STATE_IDLE_ANIM;
        this->animation = &gGerudoWhiteIdleAnim;
        Animation_Change(&this->skelAnime, &gGerudoWhiteIdleAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gGerudoWhiteIdleAnim), ANIMMODE_ONCE, -8.0f);
        return true;
    }

    this->actor.textId = textId;

    if (this->actor.xzDistToPlayer < arg3) {
        func_8002F2CC(&this->actor, globalCtx, arg3);
    }

    return false;
}

void EnGe1_SetAnimationIdle(EnGe1* this) {
    Animation_Change(&this->skelAnime, &gGerudoWhiteIdleAnim, -1.0f, Animation_GetLastFrame(&gGerudoWhiteIdleAnim),
                     0.0f, ANIMMODE_ONCE, 8.0f);
    this->animation = &gGerudoWhiteIdleAnim;
    this->animFunc = EnGe1_CueUpAnimation;
}

s32 EnGe1_CheckCarpentersFreed(void) {
    if (gSaveContext.n64ddFlag) {
        if (CHECK_QUEST_ITEM(QUEST_GERUDO_CARD)) {
            return 1;
        } else {
            return 0;
        }
    }

    u16 carpenterFlags = gSaveContext.eventChkInf[9];
    if (!((carpenterFlags & 1) && (carpenterFlags & 2) && (carpenterFlags & 4) && (carpenterFlags & 8))) {
        return 0;
    }
    return 1;
}

/**
 * Sends player to different places depending on if has hookshot, and if this is the first time captured
 */
void EnGe1_KickPlayer(EnGe1* this, GlobalContext* globalCtx) {
    this->stateFlags |= GE1_STATE_TALKING;

    if (this->cutsceneTimer > 0) {
        this->cutsceneTimer--;
    } else {
        func_8006D074(globalCtx);

        if ((INV_CONTENT(ITEM_HOOKSHOT) == ITEM_NONE) || (INV_CONTENT(ITEM_LONGSHOT) == ITEM_NONE)) {
            globalCtx->nextEntranceIndex = 0x1A5;
        } else if (gSaveContext.eventChkInf[12] & 0x80) { // Caught previously
            globalCtx->nextEntranceIndex = 0x5F8;
        } else {
            globalCtx->nextEntranceIndex = 0x3B4;
        }

        globalCtx->fadeTransition = 0x26;
        globalCtx->sceneLoadFlag = 0x14;
    }
}

void EnGe1_SpotPlayer(EnGe1* this, GlobalContext* globalCtx) {
    this->cutsceneTimer = 30;
    this->actionFunc = EnGe1_KickPlayer;
    func_8002DF54(globalCtx, &this->actor, 0x5F);
    func_80078884(NA_SE_SY_FOUND);
    Message_StartTextbox(globalCtx, 0x6000, &this->actor);
}

void EnGe1_WatchForPlayerFrontOnly(EnGe1* this, GlobalContext* globalCtx) {
    s16 angleDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if ((ABS(angleDiff) <= 0x4300) && (this->actor.xzDistToPlayer < 100.0f)) {
        EnGe1_SpotPlayer(this, globalCtx);
    }

    if (this->collider.base.acFlags & AC_HIT) {
        EnGe1_SpotPlayer(this, globalCtx);
    }

    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
}

void EnGe1_ChooseActionFromTextId(EnGe1* this, GlobalContext* globalCtx) {
    this->stateFlags |= GE1_STATE_TALKING;

    if (Actor_TextboxIsClosing(&this->actor, globalCtx)) {
        switch (this->actor.textId) {
            case 0x6001:
                this->actionFunc = EnGe1_SetNormalText;
                break;

            case 0x601A:
            case 0x6019:
                this->actionFunc = EnGe1_GetReaction_ValleyFloor;
                break;

            case 0x6018:
                this->actionFunc = EnGe1_CheckGate_GateOp;
                break;

            default:
                this->actionFunc = EnGe1_GetReaction_ValleyFloor;
                break;
        }
    }
}

void EnGe1_SetNormalText(EnGe1* this, GlobalContext* globalCtx) {
    EnGe1_SetTalkAction(this, globalCtx, 0x6001, 100.0f, EnGe1_ChooseActionFromTextId);
}

void EnGe1_WatchForAndSensePlayer(EnGe1* this, GlobalContext* globalCtx) {
    s16 angleDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if ((this->actor.xzDistToPlayer < 50.0f) || ((ABS(angleDiff) <= 0x4300) && (this->actor.xzDistToPlayer < 400.0f))) {
        EnGe1_SpotPlayer(this, globalCtx);
    }

    if (this->collider.base.acFlags & AC_HIT) {
        EnGe1_SpotPlayer(this, globalCtx);
    }
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
}

void EnGe1_GetReaction_ValleyFloor(EnGe1* this, GlobalContext* globalCtx) {
    u16 reactionText = Text_GetFaceReaction(globalCtx, 0x22);

    if (reactionText == 0) {
        reactionText = 0x6019;
    }

    EnGe1_SetTalkAction(this, globalCtx, reactionText, 100.0f, EnGe1_ChooseActionFromTextId);
}

// Gerudo Training Ground Guard functions

void EnGe1_WaitTillOpened_GTGGuard(EnGe1* this, GlobalContext* globalCtx) {
    if (this->cutsceneTimer > 0) {
        this->cutsceneTimer--;
    } else {
        EnGe1_SetAnimationIdle(this);
        this->actionFunc = EnGe1_SetNormalText;
    }

    this->stateFlags |= GE1_STATE_STOP_FIDGET;
}

void EnGe1_Open_GTGGuard(EnGe1* this, GlobalContext* globalCtx) {
    if (this->stateFlags & GE1_STATE_IDLE_ANIM) {
        this->actionFunc = EnGe1_WaitTillOpened_GTGGuard;
        Flags_SetSwitch(globalCtx, (this->actor.params >> 8) & 0x3F);
        this->cutsceneTimer = 50;
        Message_CloseTextbox(globalCtx);
    } else if ((this->skelAnime.curFrame == 15.0f) || (this->skelAnime.curFrame == 19.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_IT_HAND_CLAP);
    }
}

void EnGe1_SetupOpen_GTGGuard(EnGe1* this, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        this->actionFunc = EnGe1_Open_GTGGuard;
        Animation_Change(&this->skelAnime, &gGerudoWhiteClapAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gGerudoWhiteClapAnim), ANIMMODE_ONCE, -3.0f);
        this->animation = &gGerudoWhiteClapAnim;
        this->animFunc = EnGe1_StopFidget;
        this->stateFlags &= ~GE1_STATE_IDLE_ANIM;
    }
}

void EnGe1_RefuseEntryTooPoor_GTGGuard(EnGe1* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        this->actionFunc = EnGe1_CheckForCard_GTGGuard;
        EnGe1_SetAnimationIdle(this);
    }
}

void EnGe1_OfferOpen_GTGGuard(EnGe1* this, GlobalContext* globalCtx) {
    this->stateFlags |= GE1_STATE_TALKING;
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);

        switch (globalCtx->msgCtx.choiceIndex) {
            case 0:
                if (gSaveContext.rupees < 10) {
                    Message_ContinueTextbox(globalCtx, 0x6016);
                    this->actionFunc = EnGe1_RefuseEntryTooPoor_GTGGuard;
                } else {
                    Rupees_ChangeBy(-10);
                    Message_ContinueTextbox(globalCtx, 0x6015);
                    this->actionFunc = EnGe1_SetupOpen_GTGGuard;
                }
                break;
            case 1:
                this->actionFunc = EnGe1_CheckForCard_GTGGuard;
                EnGe1_SetAnimationIdle(this);
                break;
        }
    }
}

void EnGe1_RefuseOpenNoCard_GTGGuard(EnGe1* this, GlobalContext* globalCtx) {
    this->stateFlags |= GE1_STATE_TALKING;
    if (Actor_TextboxIsClosing(&this->actor, globalCtx)) {
        this->actionFunc = EnGe1_CheckForCard_GTGGuard;
        EnGe1_SetAnimationIdle(this);
    }
}

void EnGe1_CheckForCard_GTGGuard(EnGe1* this, GlobalContext* globalCtx) {
    if (CHECK_QUEST_ITEM(QUEST_GERUDO_CARD)) {
        EnGe1_SetTalkAction(this, globalCtx, 0x6014, 100.0f, EnGe1_OfferOpen_GTGGuard);
    } else {
        //! @bug This outcome is inaccessible in normal gameplay since this function it is unreachable without
        //! obtaining the card in the first place.
        EnGe1_SetTalkAction(this, globalCtx, 0x6013, 100.0f, EnGe1_RefuseOpenNoCard_GTGGuard);
    }
}

// Gate Operator functions

void EnGe1_WaitGateOpen_GateOp(EnGe1* this, GlobalContext* globalCtx) {
    this->stateFlags |= GE1_STATE_TALKING;

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        this->actionFunc = EnGe1_CheckGate_GateOp;
        EnGe1_SetAnimationIdle(this);
    }
}

void EnGe1_WaitUntilGateOpened_GateOp(EnGe1* this, GlobalContext* globalCtx) {
    if (this->cutsceneTimer > 0) {
        this->cutsceneTimer--;
    } else {
        EnGe1_SetAnimationIdle(this);
        this->actionFunc = EnGe1_CheckGate_GateOp;
    }
    this->stateFlags |= GE1_STATE_STOP_FIDGET;
}

void EnGe1_OpenGate_GateOp(EnGe1* this, GlobalContext* globalCtx) {
    if (this->stateFlags & GE1_STATE_IDLE_ANIM) {
        this->actionFunc = EnGe1_WaitUntilGateOpened_GateOp;
        if (gSaveContext.n64ddFlag) {
            Flags_SetSwitch(globalCtx, (769 >> 8) & 0x3F);
        } else {
            Flags_SetSwitch(globalCtx, (this->actor.params >> 8) & 0x3F);
        }
        this->cutsceneTimer = 50;
        Message_CloseTextbox(globalCtx);
    } else if ((this->skelAnime.curFrame == 15.0f) || (this->skelAnime.curFrame == 19.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_IT_HAND_CLAP);
    }
}

void EnGe1_SetupOpenGate_GateOp(EnGe1* this, GlobalContext* globalCtx) {
    this->stateFlags |= GE1_STATE_TALKING;

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        this->actionFunc = EnGe1_OpenGate_GateOp;
        Animation_Change(&this->skelAnime, &gGerudoWhiteClapAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gGerudoWhiteClapAnim), ANIMMODE_ONCE, -3.0f);
        this->animation = &gGerudoWhiteClapAnim;
        this->animFunc = EnGe1_StopFidget;
        this->stateFlags &= ~GE1_STATE_IDLE_ANIM;
    }
}

void EnGe1_CheckGate_GateOp(EnGe1* this, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, (this->actor.params >> 8) & 0x3F)) {
        EnGe1_SetTalkAction(this, globalCtx, 0x6018, 100.0f, EnGe1_WaitGateOpen_GateOp);
    } else {
        EnGe1_SetTalkAction(this, globalCtx, 0x6017, 100.0f, EnGe1_SetupOpenGate_GateOp);
    }
}

// Gate guard functions

void EnGe1_Talk_GateGuard(EnGe1* this, GlobalContext* globalCtx) {
    this->stateFlags |= GE1_STATE_TALKING;

    if (Actor_TextboxIsClosing(&this->actor, globalCtx)) {
        this->actionFunc = EnGe1_GetReaction_GateGuard;
        EnGe1_SetAnimationIdle(this);
    }
}

void EnGe1_GetReaction_GateGuard(EnGe1* this, GlobalContext* globalCtx) {
    u16 reactionText;

    reactionText = Text_GetFaceReaction(globalCtx, 0x22);

    if (reactionText == 0) {
        reactionText = 0x6069;
    }

    if (EnGe1_SetTalkAction(this, globalCtx, reactionText, 100.0f, EnGe1_Talk_GateGuard)) {
        this->animFunc = EnGe1_CueUpAnimation;
        this->animation = &gGerudoWhiteDismissiveAnim;
        Animation_Change(&this->skelAnime, &gGerudoWhiteDismissiveAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gGerudoWhiteDismissiveAnim), ANIMMODE_ONCE, -8.0f);
    }
}

// Archery functions

void EnGe1_SetupWait_Archery(EnGe1* this, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&this->actor, globalCtx)) {
        this->actionFunc = EnGe1_Wait_Archery;
        EnGe1_SetAnimationIdle(this);
    }
}

void EnGe1_WaitTillItemGiven_Archery(EnGe1* this, GlobalContext* globalCtx) {
    GetItemEntry getItemEntry = (GetItemEntry)GET_ITEM_NONE;
    s32 getItemId;

    if (Actor_HasParent(&this->actor, globalCtx)) {
        if (gSaveContext.n64ddFlag && gSaveContext.minigameScore >= 1500 && !(gSaveContext.infTable[25] & 1)) {
            gSaveContext.itemGetInf[0] |= 0x8000;
            gSaveContext.infTable[25] |= 1;
            this->stateFlags |= GE1_STATE_GIVE_QUIVER;
            this->actor.parent = NULL;
            return;
        } else {
            this->actionFunc = EnGe1_SetupWait_Archery;
        }

        if (this->stateFlags & GE1_STATE_GIVE_QUIVER) {
            gSaveContext.itemGetInf[0] |= 0x8000;
        } else {
            gSaveContext.infTable[25] |= 1;
        }
    } else {
        if (this->stateFlags & GE1_STATE_GIVE_QUIVER) {
            if (!gSaveContext.n64ddFlag) {
                switch (CUR_UPG_VALUE(UPG_QUIVER)) {
                    //! @bug Asschest. See next function for details
                    case 1:
                        getItemId = GI_QUIVER_40;
                        break;
                    case 2:
                        getItemId = GI_QUIVER_50;
                        break;
                }
            } else {
                getItemEntry = Randomizer_GetItemFromKnownCheck(RC_GF_HBA_1500_POINTS, CUR_UPG_VALUE(UPG_QUIVER) == 1 ? GI_QUIVER_40 : GI_QUIVER_50);
                getItemId = getItemEntry.getItemId;
            }
        } else {
            if (!gSaveContext.n64ddFlag) {
                getItemId = GI_HEART_PIECE;
            } else {
                getItemEntry = Randomizer_GetItemFromKnownCheck(RC_GF_HBA_1000_POINTS, GI_HEART_PIECE);
                getItemId = getItemEntry.getItemId;
            }
        }

        if (!gSaveContext.n64ddFlag || getItemEntry.getItemId == GI_NONE) {
            func_8002F434(&this->actor, globalCtx, getItemId, 10000.0f, 50.0f);
        } else {
            GiveItemEntryFromActor(&this->actor, globalCtx, getItemEntry, 10000.0f, 50.0f);
        }
    }
}

void EnGe1_BeginGiveItem_Archery(EnGe1* this, GlobalContext* globalCtx) {
    GetItemEntry getItemEntry = (GetItemEntry)GET_ITEM_NONE;
    s32 getItemId;

    if (Actor_TextboxIsClosing(&this->actor, globalCtx)) {
        this->actor.flags &= ~ACTOR_FLAG_16;
        this->actionFunc = EnGe1_WaitTillItemGiven_Archery;
    }

    if (this->stateFlags & GE1_STATE_GIVE_QUIVER) {
        if (!gSaveContext.n64ddFlag) {
            switch (CUR_UPG_VALUE(UPG_QUIVER)) {
                //! @bug Asschest. See next function for details
                case 1:
                    getItemId = GI_QUIVER_40;
                    break;
                case 2:
                    getItemId = GI_QUIVER_50;
                    break;
            }
        } else {
            getItemEntry = Randomizer_GetItemFromKnownCheck(RC_GF_HBA_1500_POINTS, CUR_UPG_VALUE(UPG_QUIVER) == 1 ? GI_QUIVER_40 : GI_QUIVER_50);
            getItemId = getItemEntry.getItemId;
        }
    } else {
        if (!gSaveContext.n64ddFlag) {
            getItemId = GI_HEART_PIECE;
        } else {
            getItemEntry = Randomizer_GetItemFromKnownCheck(RC_GF_HBA_1000_POINTS, GI_HEART_PIECE);
            getItemId = getItemEntry.getItemId;
        }
    }

    if (!gSaveContext.n64ddFlag || getItemEntry.getItemId == GI_NONE) {
        func_8002F434(&this->actor, globalCtx, getItemId, 10000.0f, 50.0f);
    } else {
        GiveItemEntryFromActor(&this->actor, globalCtx, getItemEntry, 10000.0f, 50.0f);
    }
}

void EnGe1_TalkWinPrize_Archery(EnGe1* this, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&this->actor, globalCtx)) {
        this->actionFunc = EnGe1_BeginGiveItem_Archery;
        this->actor.flags &= ~ACTOR_FLAG_16;
    } else {
        func_8002F2CC(&this->actor, globalCtx, 200.0f);
    }
}

void EnGe1_TalkTooPoor_Archery(EnGe1* this, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        this->actionFunc = EnGe1_Wait_Archery;
        EnGe1_SetAnimationIdle(this);
    }
}

void EnGe1_WaitDoNothing(EnGe1* this, GlobalContext* globalCtx) {
}

void EnGe1_BeginGame_Archery(EnGe1* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Actor* horse;

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
        this->actor.flags &= ~ACTOR_FLAG_16;

        switch (globalCtx->msgCtx.choiceIndex) {
            case 0:
                if (gSaveContext.rupees < 20) {
                    Message_ContinueTextbox(globalCtx, 0x85);
                    this->actionFunc = EnGe1_TalkTooPoor_Archery;
                } else {
                    Rupees_ChangeBy(-20);
                    globalCtx->nextEntranceIndex = 0x129;
                    gSaveContext.nextCutsceneIndex = 0xFFF0;
                    globalCtx->fadeTransition = 0x26;
                    globalCtx->sceneLoadFlag = 0x14;
                    gSaveContext.eventInf[0] |= 0x100;
                    gSaveContext.eventChkInf[6] |= 0x100;

                    if (!(player->stateFlags1 & 0x800000)) {
                        func_8002DF54(globalCtx, &this->actor, 1);
                    } else {
                        horse = Actor_FindNearby(globalCtx, &player->actor, ACTOR_EN_HORSE, ACTORCAT_BG, 1200.0f);
                        player->actor.freezeTimer = 1200;

                        if (horse != NULL) {
                            horse->freezeTimer = 1200;
                        }
                    }

                    this->actionFunc = EnGe1_WaitDoNothing;
                }
                break;

            case 1:
                this->actionFunc = EnGe1_Wait_Archery;
                Message_CloseTextbox(globalCtx);
                break;
        }
    }
}

void EnGe1_TalkOfferPlay_Archery(EnGe1* this, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_ContinueTextbox(globalCtx, 0x6041);
        this->actionFunc = EnGe1_BeginGame_Archery;
    }
}

void EnGe1_TalkNoPrize_Archery(EnGe1* this, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&this->actor, globalCtx)) {
        this->actionFunc = EnGe1_TalkOfferPlay_Archery;
    } else {
        func_8002F2CC(&this->actor, globalCtx, 300.0f);
    }
}

void EnGe1_TalkAfterGame_Archery(EnGe1* this, GlobalContext* globalCtx) {
    gSaveContext.eventInf[0] &= ~0x100;
    LOG_NUM("z_common_data.yabusame_total", gSaveContext.minigameScore);
    LOG_NUM("z_common_data.memory.information.room_inf[127][ 0 ]", HIGH_SCORE(HS_HBA));
    this->actor.flags |= ACTOR_FLAG_16;

    if (HIGH_SCORE(HS_HBA) < gSaveContext.minigameScore) {
        HIGH_SCORE(HS_HBA) = gSaveContext.minigameScore;
    }

    if (gSaveContext.minigameScore < 1000) {
        this->actor.textId = 0x6045;
        this->actionFunc = EnGe1_TalkNoPrize_Archery;
    } else if (!(gSaveContext.infTable[25] & 1)) {
        this->actor.textId = 0x6046;
        this->actionFunc = EnGe1_TalkWinPrize_Archery;
        this->stateFlags &= ~GE1_STATE_GIVE_QUIVER;
    } else if (gSaveContext.minigameScore < 1500) {
        this->actor.textId = 0x6047;
        this->actionFunc = EnGe1_TalkNoPrize_Archery;
    } else if (gSaveContext.itemGetInf[0] & 0x8000) {
        this->actor.textId = 0x6047;
        this->actionFunc = EnGe1_TalkNoPrize_Archery;
    } else {
        this->actor.textId = 0x6044;
        this->actionFunc = EnGe1_TalkWinPrize_Archery;
        this->stateFlags |= GE1_STATE_GIVE_QUIVER;
    }
}

void EnGe1_TalkNoHorse_Archery(EnGe1* this, GlobalContext* globalCtx) {
    this->stateFlags |= GE1_STATE_TALKING;
    if (Actor_TextboxIsClosing(&this->actor, globalCtx)) {
        this->actionFunc = EnGe1_Wait_Archery;
        EnGe1_SetAnimationIdle(this);
    }
}

void EnGe1_Wait_Archery(EnGe1* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    u16 textId;

    if (!(player->stateFlags1 & 0x800000)) {
        EnGe1_SetTalkAction(this, globalCtx, 0x603F, 100.0f, EnGe1_TalkNoHorse_Archery);
    } else {
        if (gSaveContext.eventChkInf[6] & 0x100) {
            if (gSaveContext.infTable[25] & 1) {
                textId = 0x6042;
            } else {
                textId = 0x6043;
            }
        } else {
            textId = 0x6040;
        }
        EnGe1_SetTalkAction(this, globalCtx, textId, 200.0f, EnGe1_TalkOfferPlay_Archery);
    }
}

// General functions

void EnGe1_TurnToFacePlayer(EnGe1* this, GlobalContext* globalCtx) {
    s32 pad;
    s16 angleDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (ABS(angleDiff) <= 0x4000) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 6, 4000, 100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        func_80038290(globalCtx, &this->actor, &this->headRot, &this->unk_2A2, this->actor.focus.pos);
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

void EnGe1_LookAtPlayer(EnGe1* this, GlobalContext* globalCtx) {
    s16 angleDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if ((ABS(angleDiff) <= 0x4300) && (this->actor.xzDistToPlayer < 100.0f)) {
        func_80038290(globalCtx, &this->actor, &this->headRot, &this->unk_2A2, this->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&this->headRot.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&this->headRot.y, 0, 6, 6200, 100);
    }
}

void EnGe1_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnGe1* this = (EnGe1*)thisx;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &this->actor, 40.0f, 25.0f, 40.0f, 5);
    this->animFunc(this);
    this->actionFunc(this, globalCtx);

    if (this->stateFlags & GE1_STATE_TALKING) {
        EnGe1_TurnToFacePlayer(this, globalCtx);
        this->stateFlags &= ~GE1_STATE_TALKING;
    } else {
        EnGe1_LookAtPlayer(this, globalCtx);
    }
    this->unk_2A2.x = this->unk_2A2.y = this->unk_2A2.z = 0;

    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = Rand_S16Offset(60, 60);
    }
    this->eyeIndex = this->blinkTimer;

    if (this->eyeIndex >= 3) {
        this->eyeIndex = 0;
    }
}

// Animation functions

void EnGe1_CueUpAnimation(EnGe1* this) {
    if (SkelAnime_Update(&this->skelAnime)) {
        Animation_PlayOnce(&this->skelAnime, this->animation);
    }
}

void EnGe1_StopFidget(EnGe1* this) {
    if (!(this->stateFlags & GE1_STATE_IDLE_ANIM)) {
        if (SkelAnime_Update(&this->skelAnime)) {
            this->stateFlags |= GE1_STATE_IDLE_ANIM;
        }
        this->stateFlags |= GE1_STATE_STOP_FIDGET;
    }
}

s32 EnGe1_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    s32 pad;
    EnGe1* this = (EnGe1*)thisx;

    if (limbIndex == GE1_LIMB_HEAD) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.x;
    }

    if (this->stateFlags & GE1_STATE_STOP_FIDGET) {
        this->stateFlags &= ~GE1_STATE_STOP_FIDGET;
        return 0;
    }

    // The purpose of the state flag GE1_STATE_STOP_FIDGET is to skip this code, which this actor has in lieu of an idle
    // animation.
    if ((limbIndex == GE1_LIMB_TORSO) || (limbIndex == GE1_LIMB_L_FOREARM) || (limbIndex == GE1_LIMB_R_FOREARM)) {
        rot->y += Math_SinS(globalCtx->state.frames * (limbIndex * 50 + 0x814)) * 200.0f;
        rot->z += Math_CosS(globalCtx->state.frames * (limbIndex * 50 + 0x940)) * 200.0f;
    }
    return 0;
}

void EnGe1_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnGe1* this = (EnGe1*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    if (limbIndex == GE1_LIMB_HEAD) {
        gSPDisplayList(POLY_OPA_DISP++, sHairstyleDLists[this->hairstyle]);
        Matrix_MultVec3f(&D_80A327A8, &this->actor.focus.pos);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void EnGe1_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnGe1* this = (EnGe1*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_800943C8(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[this->eyeIndex]));
    SkelAnime_DrawFlexOpa(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnGe1_OverrideLimbDraw, EnGe1_PostLimbDraw, this);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
