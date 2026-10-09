/*
 * File: z_en_nnh.c
 * Overlay: ovl_En_Nnh
 * Description: Twisted Corpse of Deku Butler's Son
 */

#include "z_en_nnh.h"
#include "objects/object_nnh/object_nnh.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnNnh_Init(Actor* thisx, PlayState* play);
void EnNnh_Destroy(Actor* thisx, PlayState* play);
void EnNnh_Update(Actor* thisx, PlayState* play);
void EnNnh_Draw(Actor* thisx, PlayState* play);

void EnNnh_SetupWaitForDialogue(EnNnh* this);
void EnNnh_WaitForDialogue(EnNnh* this, PlayState* play);
void EnNnh_SetupDialogue(EnNnh* this);
void EnNnh_Dialogue(EnNnh* this, PlayState* play);

ActorProfile En_Nnh_Profile = {
    /**/ ACTOR_EN_NNH,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_NNH,
    /**/ sizeof(EnNnh),
    /**/ EnNnh_Init,
    /**/ EnNnh_Destroy,
    /**/ EnNnh_Update,
    /**/ EnNnh_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_TREE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 50, 0, { 0, 0, 0 } },
};

void EnNnh_Init(Actor* thisx, PlayState* play) {
    EnNnh* this = (EnNnh*)thisx;

    Actor_SetScale(&this->actor, 0.01f);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.attentionRangeType = ATTENTION_RANGE_1;
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 30.0f;
    EnNnh_SetupWaitForDialogue(this);
}

void EnNnh_Destroy(Actor* thisx, PlayState* play) {
    EnNnh* this = (EnNnh*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnNnh_SetupWaitForDialogue(EnNnh* this) {
    this->actionFunc = EnNnh_WaitForDialogue;
}

void EnNnh_WaitForDialogue(EnNnh* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_StartTextbox(play, 0x228, &this->actor);
        EnNnh_SetupDialogue(this);
    } else {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }
}

void EnNnh_SetupDialogue(EnNnh* this) {
    this->actionFunc = EnNnh_Dialogue;
}

void EnNnh_Dialogue(EnNnh* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        EnNnh_SetupWaitForDialogue(this);
    }
}

void EnNnh_Update(Actor* thisx, PlayState* play) {
    EnNnh* this = (EnNnh*)thisx;
    s32 pad;

    this->actionFunc(this, play);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnNnh_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gButlerSonMainBodyDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
