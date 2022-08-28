/*
 * File: z_en_ms.c
 * Overlay: ovl_En_Ms
 * Description: Magic Bean Salesman
 */

#include "z_en_ms.h"
#include "objects/object_ms/object_ms.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnMs_Init(Actor* thisx, GlobalContext* globalCtx);
void EnMs_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnMs_Update(Actor* thisx, GlobalContext* globalCtx);
void EnMs_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnMs_SetOfferText(EnMs* this, GlobalContext* globalCtx);
void EnMs_Wait(EnMs* this, GlobalContext* globalCtx);
void EnMs_Talk(EnMs* this, GlobalContext* globalCtx);
void EnMs_Sell(EnMs* this, GlobalContext* globalCtx);
void EnMs_TalkAfterPurchase(EnMs* this, GlobalContext* globalCtx);

const ActorInit En_Ms_InitVars = {
    ACTOR_EN_MS,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_MS,
    sizeof(EnMs),
    (ActorFunc)EnMs_Init,
    (ActorFunc)EnMs_Destroy,
    (ActorFunc)EnMs_Update,
    (ActorFunc)EnMs_Draw,
    NULL,
};

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0x00000000, 0x00, 0x00 }, { 0xFFCFFFFF, 0x00, 0x00 }, 0x00, 0x01, 0x01 },
    { 22, 37, 0, { 0 } },
};

static s16 sPrices[] = {
    10, 20, 30, 40, 50, 60, 70, 80, 90, 100,
};

static u16 sOfferTextIDs[] = {
    0x405E, 0x405F, 0x4060, 0x4061, 0x4062, 0x4063, 0x4064, 0x4065, 0x4066, 0x4067,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 500, ICHAIN_STOP),
};

void EnMs_SetOfferText(EnMs* this, GlobalContext* globalCtx) {
    this->actor.textId = Text_GetFaceReaction(globalCtx, 0x1B);
    if (this->actor.textId == 0) {
        if (BEANS_BOUGHT >= 10) {
            this->actor.textId = 0x406B;
        } else {
            this->actor.textId = sOfferTextIDs[BEANS_BOUGHT];
        }
    }
}

void EnMs_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnMs* this = (EnMs*)thisx;
    s32 pad;

    if (LINK_AGE_IN_YEARS != YEARS_CHILD) {
        Actor_Kill(&this->actor);
        return;
    }
    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_InitFlex(globalCtx, &this->skelAnime, &gBeanSalesmanSkel, &gBeanSalesmanEatingAnim, this->jointTable,
                       this->morphTable, 9);
    Collider_InitCylinder(globalCtx, &this->collider);
    Collider_SetCylinderType1(globalCtx, &this->collider, &this->actor, &sCylinderInit);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
    Actor_SetScale(&this->actor, 0.015f);

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.speedXZ = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -1.0f;

    EnMs_SetOfferText(this, globalCtx);

    this->actionFunc = EnMs_Wait;
}

void EnMs_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnMs* this = (EnMs*)thisx;

    Collider_DestroyCylinder(globalCtx, &this->collider);
}

void EnMs_Wait(EnMs* this, GlobalContext* globalCtx) {
    s16 yawDiff;

    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    EnMs_SetOfferText(this, globalCtx);

    if (Actor_ProcessTalkRequest(&this->actor, globalCtx)) { // if talk is initiated
        this->actionFunc = EnMs_Talk;
    } else if ((this->actor.xzDistToPlayer < 90.0f) && (ABS(yawDiff) < 0x2000)) { // talk range
        func_8002F2CC(&this->actor, globalCtx, 90.0f);
    }
}

void EnMs_Talk(EnMs* this, GlobalContext* globalCtx) {
    u8 dialogState;

    dialogState = Message_GetState(&globalCtx->msgCtx);
    if (dialogState != TEXT_STATE_CHOICE) {
        if ((dialogState == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) { // advanced final textbox
            this->actionFunc = EnMs_Wait;
        }
    } else if (Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0: // yes
                if (gSaveContext.rupees <
                    ((gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_MAGIC_BEANS))
                         ? 60
                         : sPrices[BEANS_BOUGHT])) {
                    Message_ContinueTextbox(globalCtx, 0x4069); // not enough rupees text
                    return;
                }
                if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_MAGIC_BEANS)) {
                    GiveItemEntryFromActor(&this->actor, globalCtx, 
                        Randomizer_GetItemFromKnownCheck(RC_ZR_MAGIC_BEAN_SALESMAN, GI_BEAN), 90.0f, 10.0f);
                } else {
                    func_8002F434(&this->actor, globalCtx, GI_BEAN, 90.0f, 10.0f);
                }
                this->actionFunc = EnMs_Sell;
                return;
            case 1: // no
                Message_ContinueTextbox(globalCtx, 0x4068);
            default:
                return;
        }
    }
}

void EnMs_Sell(EnMs* this, GlobalContext* globalCtx) {
    if (Actor_HasParent(&this->actor, globalCtx)) {
        Rupees_ChangeBy((gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_MAGIC_BEANS)) ? -60 : -sPrices[BEANS_BOUGHT]);
        this->actor.parent = NULL;
        this->actionFunc =
            (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_MAGIC_BEANS)) ? EnMs_Wait : EnMs_TalkAfterPurchase;
    } else {
        if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_MAGIC_BEANS)) {
            GiveItemEntryFromActor(&this->actor, globalCtx,
                                   Randomizer_GetItemFromKnownCheck(RC_ZR_MAGIC_BEAN_SALESMAN, GI_BEAN), 90.0f, 10.0f);
            BEANS_BOUGHT = 10;
        } else {
            func_8002F434(&this->actor, globalCtx, GI_BEAN, 90.0f, 10.0f);
        }
    }
}

void EnMs_TalkAfterPurchase(EnMs* this, GlobalContext* globalCtx) {
    // if dialog state is 6 and player responded to textbox
    if ((Message_GetState(&globalCtx->msgCtx)) == TEXT_STATE_DONE && Message_ShouldAdvance(globalCtx)) {
        Message_ContinueTextbox(globalCtx, 0x406C);
        this->actionFunc = EnMs_Talk;
    }
}

void EnMs_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnMs* this = (EnMs*)thisx;
    s32 pad;

    this->activeTimer += 1;
    Actor_SetFocus(&this->actor, 20.0f);
    this->actor.targetArrowOffset = 500.0f;
    Actor_SetScale(&this->actor, 0.015f);
    SkelAnime_Update(&this->skelAnime);
    this->actionFunc(this, globalCtx);

    if (gSaveContext.entranceIndex == 0x157 && gSaveContext.sceneSetupIndex == 8) { // ride carpet if in credits
        Actor_MoveForward(&this->actor);
        osSyncPrintf("OOOHHHHHH %f\n", this->actor.velocity.y);
        Actor_UpdateBgCheckInfo(globalCtx, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    }
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
}

void EnMs_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnMs* this = (EnMs*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          NULL, NULL, this);
}
