/*
 * File: z_obj_ghaka.c
 * Overlay: ovl_Obj_Ghaka
 * Description: Darmani's Gravestone
 */

#include "z_obj_ghaka.h"
#include "objects/object_ghaka/object_ghaka.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void ObjGhaka_Init(Actor* thisx, PlayState* play);
void ObjGhaka_Destroy(Actor* thisx, PlayState* play);
void ObjGhaka_Update(Actor* thisx, PlayState* play);
void ObjGhaka_Draw(Actor* thisx, PlayState* play);

void func_80B3C260(ObjGhaka* this);
void func_80B3C29C(ObjGhaka* this);
void func_80B3C2B0(ObjGhaka* this);
void func_80B3C2C4(ObjGhaka* this, PlayState* play);
void func_80B3C39C(ObjGhaka* this, PlayState* play);
void func_80B3C4E0(ObjGhaka* this, PlayState* play);
void func_80B3C624(ObjGhaka* this, PlayState* play);

ActorProfile Obj_Ghaka_Profile = {
    /**/ ACTOR_OBJ_GHAKA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GHAKA,
    /**/ sizeof(ObjGhaka),
    /**/ ObjGhaka_Init,
    /**/ ObjGhaka_Destroy,
    /**/ ObjGhaka_Update,
    /**/ ObjGhaka_Draw,
};

static Vec3f D_80B3C960 = { 0.0f, 0.0f, 0.0f };

static InitChainEntry D_80B3C96C[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_0, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 30, ICHAIN_STOP),
};

void func_80B3C260(ObjGhaka* this) {
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_20_20)) {
        this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z + 100.0f;
    }
    this->actionFunc = func_80B3C39C;
}

void func_80B3C29C(ObjGhaka* this) {
    this->actionFunc = func_80B3C4E0;
}

void func_80B3C2B0(ObjGhaka* this) {
    this->actionFunc = func_80B3C624;
}

void func_80B3C2C4(ObjGhaka* this, PlayState* play) {
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_20_20)) {
        Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, ACTOR_BG_GORON_OYU, 0.0f, 25.0f, 261.0f, 0, 0, 0, 0,
                                      this->dyna.actor.csId, this->dyna.actor.halfDaysBits, 0);
    } else {
        Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, ACTOR_BG_GORON_OYU, 0.0f, 25.0f, 261.0f, 0, 0, 0, 1,
                                      CS_ID_NONE, this->dyna.actor.halfDaysBits, 0);
    }
}

void func_80B3C39C(ObjGhaka* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yaw = this->dyna.actor.yawTowardsPlayer - this->dyna.actor.shape.rot.y;

    if (Actor_TalkOfferAccepted(&this->dyna.actor, &play->state)) {
        func_80B3C29C(this);
    } else if ((this->dyna.actor.xzDistToPlayer < 100.0f) || this->dyna.actor.isLockedOn) {
        if ((yaw <= -0x5556) || (yaw >= 0x5556)) {
            Actor_OfferTalkNearColChkInfoCylinder(&this->dyna.actor, play);
            if (player->transformation == PLAYER_FORM_GORON) {
                this->dyna.actor.textId = 0xCF3;
            } else {
                this->dyna.actor.textId = 0xCF2;
            }
        }
    }

    if ((this->dyna.pushForce < 0.0f) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_20_20) &&
        (player->transformation == PLAYER_FORM_GORON)) {
        func_80B3C2B0(this);
    } else {
        player->stateFlags2 &= ~PLAYER_STATE2_10;
        this->dyna.pushForce = 0.0f;
    }
}

void func_80B3C4E0(ObjGhaka* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (talkState == TEXT_STATE_EVENT) {
        if (Message_ShouldAdvance(play)) {
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            func_80B3C260(this);
        }
    } else if (talkState == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(play)) {
            switch (play->msgCtx.choiceIndex) {
                case 0:
                    Audio_PlaySfx_MessageDecide();
                    this->dyna.actor.textId = 0xCF5;
                    Message_StartTextbox(play, this->dyna.actor.textId, &this->dyna.actor);
                    break;

                case 1:
                    Audio_PlaySfx_MessageDecide();
                    this->dyna.actor.textId = 0xCF7;
                    Message_StartTextbox(play, this->dyna.actor.textId, &this->dyna.actor);
                    break;

                case 2:
                    Audio_PlaySfx_MessageCancel();
                    play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                    play->msgCtx.stateTimer = 4;
                    func_80B3C260(this);
                    break;
            }
        }
    }
}

void func_80B3C624(ObjGhaka* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u8 stepTemp = Math_StepToS(&this->unk_168, 0x64, 1);

    this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z + this->unk_168;

    if (stepTemp) {
        player->stateFlags2 &= ~PLAYER_STATE2_10;
        this->dyna.pushForce = 0.0f;
        func_80B3C2C4(this, play);
        SET_WEEKEVENTREG(WEEKEVENTREG_20_20);
        func_80B3C260(this);
        Audio_PlaySfx_AtPos(&D_80B3C960, NA_SE_EV_BLOCK_BOUND);
    } else {
        Audio_PlaySfx_AtPos(&D_80B3C960, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
    }
}

void ObjGhaka_Init(Actor* thisx, PlayState* play) {
    ObjGhaka* this = (ObjGhaka*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&this->dyna.actor, D_80B3C96C);
    Actor_SetScale(&this->dyna.actor, 0.1f);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    CollisionHeader_GetVirtual(&object_ghaka_Colheader_003CD0, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    if (this->dyna.actor.floorPoly == NULL) {
        Actor_Kill(&this->dyna.actor);
    }
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_20_20)) {
        func_80B3C2C4(this, play);
    }
    func_80B3C260(this);
}

void ObjGhaka_Destroy(Actor* thisx, PlayState* play) {
    ObjGhaka* this = (ObjGhaka*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjGhaka_Update(Actor* thisx, PlayState* play) {
    ObjGhaka* this = (ObjGhaka*)thisx;

    this->actionFunc(this, play);
    thisx->focus.pos.x = thisx->world.pos.x;
    thisx->focus.pos.y = thisx->world.pos.y + 60.0f;
    thisx->focus.pos.z = thisx->world.pos.z;
}

void ObjGhaka_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, object_ghaka_DL_001A20);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, object_ghaka_DL_001980);

    CLOSE_DISPS(play->state.gfxCtx);
}
