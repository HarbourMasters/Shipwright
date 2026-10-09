/*
 * File: z_obj_moon_stone.c
 * Overlay: ovl_Obj_Moon_Stone
 * Description: Moon's Tear
 */

#include "z_obj_moon_stone.h"
#include "objects/object_gi_reserve00/object_gi_reserve00.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_FREEZE_EXCEPTION)

void ObjMoonStone_Init(Actor* thisx, PlayState* play);
void ObjMoonStone_Destroy(Actor* thisx, PlayState* play);
void ObjMoonStone_Update(Actor* thisx, PlayState* play);
void ObjMoonStone_Draw(Actor* thisx, PlayState* play);

void func_80C0662C(ObjMoonStone* this);
void func_80C06640(ObjMoonStone* this, PlayState* play);
void func_80C066F8(ObjMoonStone* this);
void func_80C0670C(ObjMoonStone* this, PlayState* play);
void func_80C0673C(ObjMoonStone* this);
void func_80C06768(ObjMoonStone* this, PlayState* play);
void func_80C0685C(ObjMoonStone* this);
void func_80C06870(ObjMoonStone* this, PlayState* play);

ActorProfile Obj_Moon_Stone_Profile = {
    /**/ ACTOR_OBJ_MOON_STONE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GI_RESERVE00,
    /**/ sizeof(ObjMoonStone),
    /**/ ObjMoonStone_Init,
    /**/ ObjMoonStone_Destroy,
    /**/ ObjMoonStone_Update,
    /**/ ObjMoonStone_Draw,
};

void ObjMoonStone_Init(Actor* thisx, PlayState* play) {
    ObjMoonStone* this = (ObjMoonStone*)thisx;

    Actor_SetScale(&this->actor, 0.3f);
    this->unk194 = (this->actor.params & 0xF000) >> 0xC;
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    this->actor.shape.yOffset = 25.0f;
    this->actor.focus.pos.y += 10.0f;
    if (this->unk194 == 0) {
        this->actor.colChkInfo.health = 0;
        this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        func_80C0662C(this);
    } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_74_40)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_74_80)) {
            Actor_Spawn(&play->actorCtx, play, 1, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, 0, 0, -1);
        }
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        func_80C0673C(this);
    } else {
        Actor_Kill(&this->actor);
    }
}

void ObjMoonStone_Destroy(Actor* thisx, PlayState* play) {
}

void func_80C0662C(ObjMoonStone* this) {
    this->actionFunc = func_80C06640;
}

void func_80C06640(ObjMoonStone* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 sp1A = this->actor.yawTowardsPlayer - 0x8000;

    sp1A -= player->actor.shape.rot.y;
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.colChkInfo.health = 1;
        Message_StartTextbox(play, 0x5E3, &this->actor);
        func_80C066F8(this);
    } else {
        s32 phi_v0 = ABS_ALT(sp1A);
        if (phi_v0 < 0x1555) {
            Actor_OfferTalk(&this->actor, play, 80.0f);
        }
    }
}

void func_80C066F8(ObjMoonStone* this) {
    this->actionFunc = func_80C0670C;
}

void func_80C0670C(ObjMoonStone* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->actor.colChkInfo.health = 0;
        func_80C0662C(this);
    }
}

void func_80C0673C(ObjMoonStone* this) {
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_74_80)) {
        this->actor.draw = NULL;
    }
    this->actionFunc = func_80C06768;
}

void func_80C06768(ObjMoonStone* this, PlayState* play) {
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_74_80)) {
        if (this->actor.draw == NULL) {
            if (GameInteractor_Should(VB_REVEAL_MOON_STONE_IN_CRATER, true, this)) {
                this->actor.draw = ObjMoonStone_Draw;
            }
            Actor_Spawn(&play->actorCtx, play, 1, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, 0, 0, -1);
        }
    }
    if (this->actor.draw != NULL) {
        if (Actor_HasParent(&this->actor, play) || !GameInteractor_Should(VB_GIVE_ITEM_FROM_MOONS_TEAR, true, this)) {
            this->actor.parent = NULL;
            this->actor.draw = NULL;
            func_80C0685C(this);
        } else if (this->actor.xzDistToPlayer < 25.0f) {
            Actor_OfferGetItem(&this->actor, play, GI_MOONS_TEAR, 100.0f, 30.0f);
        }
    }
}

void func_80C0685C(ObjMoonStone* this) {
    this->actionFunc = func_80C06870;
}

void func_80C06870(ObjMoonStone* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        SET_WEEKEVENTREG(WEEKEVENTREG_74_40);
        Actor_Kill(&this->actor);
    }
}

void ObjMoonStone_Update(Actor* thisx, PlayState* play) {
    ObjMoonStone* this = (ObjMoonStone*)thisx;
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 & (PLAYER_STATE1_2 | PLAYER_STATE1_DEAD | PLAYER_STATE1_200 | PLAYER_STATE1_10000000))) {
        this->actionFunc(this, play);
    }
}

void ObjMoonStone_Draw(Actor* thisx, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gGiMoonsTearTexAnim));
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gGiMoonsTearItemDL);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gGiMoonsTearGlowDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
