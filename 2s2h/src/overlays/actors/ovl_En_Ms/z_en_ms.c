/*
 * File: z_en_ms.c
 * Overlay: ovl_En_Ms
 * Description: Bean Seller
 */

#include "z_en_ms.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnMs_Init(Actor* thisx, PlayState* play);
void EnMs_Destroy(Actor* thisx, PlayState* play);
void EnMs_Update(Actor* thisx, PlayState* play);
void EnMs_Draw(Actor* thisx, PlayState* play);

void EnMs_Wait(EnMs* this, PlayState* play);
void EnMs_Talk(EnMs* this, PlayState* play);
void EnMs_Sell(EnMs* this, PlayState* play);
void EnMs_TalkAfterPurchase(EnMs* this, PlayState* play);

ActorProfile En_Ms_Profile = {
    /**/ ACTOR_EN_MS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_MS,
    /**/ sizeof(EnMs),
    /**/ EnMs_Init,
    /**/ EnMs_Destroy,
    /**/ EnMs_Update,
    /**/ EnMs_Draw,
};

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
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
    { 22, 37, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 500, ICHAIN_STOP),
};

void EnMs_Init(Actor* thisx, PlayState* play) {
    EnMs* this = (EnMs*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    SkelAnime_InitFlex(play, &this->skelAnime, &gBeanSalesmanSkel, &gBeanSalesmanEatingAnim, this->jointTable,
                       this->morphTable, BEAN_SALESMAN_LIMB_MAX);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinderType1(play, &this->collider, &this->actor, &sCylinderInit);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
    Actor_SetScale(&this->actor, 0.015f);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE; // Eating Magic Beans all day will do that to you
    this->actionFunc = EnMs_Wait;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -1.0f;
}

void EnMs_Destroy(Actor* thisx, PlayState* play) {
    EnMs* this = (EnMs*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnMs_Wait(EnMs* this, PlayState* play) {
    s16 yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (gSaveContext.save.saveInfo.inventory.items[SLOT_MAGIC_BEANS] == ITEM_NONE) {
        this->actor.textId = 0x92E;
    } else {
        this->actor.textId = 0x932;
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = EnMs_Talk;
    } else if ((this->actor.xzDistToPlayer < 90.0f) && (ABS_ALT(yawDiff) < 0x2000)) {
        Actor_OfferTalk(&this->actor, play, 90.0f);
    }
}

void EnMs_Talk(EnMs* this, PlayState* play) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                this->actionFunc = EnMs_Wait;
            }
            break;

        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                Actor_OfferGetItem(&this->actor, play, GI_MAGIC_BEANS, this->actor.xzDistToPlayer,
                                   this->actor.playerHeightRel);
                this->actionFunc = EnMs_Sell;
            }
            break;

        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.choiceIndex) {
                    case 0: // yes
                        Message_CloseTextbox(play);
                        if (gSaveContext.save.saveInfo.playerData.rupees < 10) {
                            Audio_PlaySfx(NA_SE_SY_ERROR);
                            Message_ContinueTextbox(play, 0x935);
                        } else if (AMMO(ITEM_MAGIC_BEANS) >= 20) {
                            Audio_PlaySfx(NA_SE_SY_ERROR);
                            Message_ContinueTextbox(play, 0x937);
                        } else {
                            Audio_PlaySfx_MessageDecide();
                            Actor_OfferGetItem(&this->actor, play, GI_MAGIC_BEANS, 90.0f, 10.0f);
                            Rupees_ChangeBy(-10);
                            this->actionFunc = EnMs_Sell;
                        }
                        break;

                    case 1: // no
                    default:
                        Audio_PlaySfx_MessageCancel();
                        Message_ContinueTextbox(play, 0x934);
                        break;
                }
            }
            break;

        default:
            break;
    }
}

void EnMs_Sell(EnMs* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.textId = 0;
        Actor_OfferTalkExchange(&this->actor, play, this->actor.xzDistToPlayer, this->actor.playerHeightRel,
                                PLAYER_IA_NONE);
        this->actionFunc = EnMs_TalkAfterPurchase;
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_MAGIC_BEANS, this->actor.xzDistToPlayer, this->actor.playerHeightRel);
    }
}

void EnMs_TalkAfterPurchase(EnMs* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_ContinueTextbox(play, 0x936);
        this->actionFunc = EnMs_Talk;
    } else {
        Actor_OfferTalkExchange(&this->actor, play, this->actor.xzDistToPlayer, this->actor.playerHeightRel,
                                PLAYER_IA_MINUS1);
    }
}

void EnMs_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnMs* this = (EnMs*)thisx;

    Actor_SetFocus(&this->actor, 20.0f);
    this->actor.lockOnArrowOffset = 500.0f;
    Actor_SetScale(&this->actor, 0.015f);
    SkelAnime_Update(&this->skelAnime);
    this->actionFunc(this, play);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnMs_Draw(Actor* thisx, PlayState* play) {
    EnMs* this = (EnMs*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, &this->actor);
}
