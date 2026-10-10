/*
 * File: z_en_onpuman.c
 * Overlay: ovl_En_Onpuman
 * Description: Monkey Instrument Prompt (unused)
 */

#include "z_en_onpuman.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnOnpuman_Init(Actor* thisx, PlayState* play);
void EnOnpuman_Destroy(Actor* thisx, PlayState* play);
void EnOnpuman_Update(Actor* thisx, PlayState* play);

void func_80B121D8(EnOnpuman* this, PlayState* play);

ActorProfile En_Onpuman_Profile = {
    /**/ ACTOR_EN_ONPUMAN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnOnpuman),
    /**/ EnOnpuman_Init,
    /**/ EnOnpuman_Destroy,
    /**/ EnOnpuman_Update,
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
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 30, 40, 0, { 0, 0, 0 } },
};

void EnOnpuman_Init(Actor* thisx, PlayState* play) {
    EnOnpuman* this = (EnOnpuman*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    this->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->unk_2A4 = 0;
    this->unk_2A0 = NULL;
    this->actionFunc = func_80B121D8;
    this->actor.velocity.y = -10.0f;
    this->actor.terminalVelocity = -10.0f;
    this->actor.gravity = -5.0f;
}

void EnOnpuman_Destroy(Actor* thisx, PlayState* play) {
    EnOnpuman* this = (EnOnpuman*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

Actor* func_80B11F44(PlayState* play) {
    Actor* itemAction = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].first;

    if (itemAction != NULL) {
        if (itemAction->id != ACTOR_EN_GAKUFU) {
            itemAction = itemAction->next;
        }
        return itemAction;
    }
    return NULL;
}

void func_80B11F78(EnOnpuman* this, PlayState* play) {
    if (play->msgCtx.ocarinaMode == OCARINA_MODE_END) {
        this->actionFunc = func_80B121D8;
        if (this->actor.csId != CS_ID_NONE) {
            CutsceneManager_Stop(this->actor.csId);
        }
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) {
        Audio_PlaySfx(NA_SE_SY_CORRECT_CHIME);
        play->msgCtx.ocarinaMode = OCARINA_MODE_END;
        if (this->actor.csId != CS_ID_NONE) {
            CutsceneManager_Stop(this->actor.csId);
        }
        this->actionFunc = func_80B121D8;
    }
}

void func_80B1202C(EnOnpuman* this, PlayState* play2) {
    PlayState* play = play2;

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x8D4:
                this->unk_2A4 |= 1;
                Message_ContinueTextbox(play, 0x8DA);
                break;

            case 0x8DA:
                Message_ContinueTextbox(play, 0x8D6);
                if (this->unk_2A0 != NULL) {
                    this->unk_2A0->home.rot.x = 0x50;
                }
                break;

            case 0x8D6:
                this->actionFunc = func_80B11F78;
                Message_DisplayOcarinaStaff(play, OCARINA_ACTION_3A);
                if (this->unk_2A0 != NULL) {
                    this->unk_2A0->home.rot.x = 0;
                }
                break;

            default:
                break;
        }
    }
    if (this->unk_2A4 & 1) {
        if (this->actor.csId == CS_ID_NONE) {
            this->unk_2A4 &= ~1;
        } else if (CutsceneManager_IsNext(this->actor.csId)) {
            this->unk_2A4 &= ~1;
            CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        } else {
            CutsceneManager_Queue(this->actor.csId);
        }
    }
}

void func_80B1217C(EnOnpuman* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->actionFunc = func_80B121D8;
        Message_CloseTextbox(play);
    }
}

void func_80B121D8(EnOnpuman* this, PlayState* play) {
    s16 yaw;

    if (Actor_OcarinaInteractionAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80B1202C;
        Message_StartTextbox(play, 0x8D4, NULL);
        this->unk_2A0 = func_80B11F44(play);
    } else if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80B1217C;
    } else {
        yaw = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        if (this->actor.xzDistToPlayer < 200.0f) {
            if (ABS_ALT(yaw) <= 0x4300) {
                this->actor.textId = 0x8D3;
                Actor_OfferTalk(&this->actor, play, 100.0f);
                Actor_OfferOcarinaInteraction(&this->actor, play, 100.0f, 100.0f);
            }
        }
    }
}

void EnOnpuman_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnOnpuman* this = (EnOnpuman*)thisx;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    this->actionFunc(this, play);
}
