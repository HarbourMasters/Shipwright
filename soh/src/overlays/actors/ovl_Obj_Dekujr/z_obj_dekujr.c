/*
 * File: z_obj_dekujr.c
 * Overlay: ovl_Obj_Dekujr
 * Description: Deku Tree Sprout
 */

#include "z_obj_dekujr.h"
#include "objects/object_dekujr/object_dekujr.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void ObjDekujr_Init(Actor* thisx, PlayState* play);
void ObjDekujr_Destroy(Actor* thisx, PlayState* play);
void ObjDekujr_Update(Actor* thisx, PlayState* play);
void ObjDekujr_Draw(Actor* thisx, PlayState* play);

void ObjDekujr_ComeUp(ObjDekujr* this, PlayState* play);

const ActorInit Obj_Dekujr_InitVars = {
    ACTOR_OBJ_DEKUJR,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_DEKUJR,
    sizeof(ObjDekujr),
    (ActorFunc)ObjDekujr_Init,
    (ActorFunc)ObjDekujr_Destroy,
    (ActorFunc)ObjDekujr_Update,
    (ActorFunc)ObjDekujr_Draw,
    NULL,
};

static ColliderCylinderInitToActor sCylinderInit = {
    {
        NULL,
        0x00,
        0x00,
        0x39,
        COLSHAPE_CYLINDER,
    },
    { 0x02, { 0x00000000, 0x00, 0x00 }, { 0xFFCFFFFF, 0x00, 0x00 }, 0x00, 0x00, 0x01 },
    { 60, 80, 0, { 0, 0, 0 } },
};

void ObjDekujr_Init(Actor* thisx, PlayState* play) {
    ObjDekujr* this = (ObjDekujr*)thisx;
    s32 pad;

    if (gSaveContext.cutsceneIndex < 0xFFF0) {
        if (!LINK_IS_ADULT) {
            Actor_Kill(thisx);
            return;
        }
        this->unk_19C = 2;
        this->unk_19B = 0;
    } else {
        this->unk_19C = 0;
        this->unk_19B = 1;
    }
    if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
        Actor_Kill(thisx);
    } else {
        ActorShape_Init(&thisx->shape, 0.0f, NULL, 0.0f);
        Collider_InitCylinder(play, &this->collider);
        sCylinderInit.base.actor = thisx;
        Collider_SetCylinderToActor(play, &this->collider, &sCylinderInit);
        thisx->colChkInfo.mass = MASS_IMMOVABLE;
        thisx->textId = func_80037C30(play, 0xF);
        Actor_SetScale(thisx, 0.4f);
    }
}

void ObjDekujr_Destroy(Actor* thisx, PlayState* play) {
}

void ObjDekujr_SetInitialPos(CsCmdActorAction* npcAction, Vec3f* initPos) {
    initPos->x = npcAction->startPos.x;
    initPos->y = npcAction->startPos.y;
    initPos->z = npcAction->startPos.z;
}

void ObjDekujr_SetFinalPos(CsCmdActorAction* npcAction, Vec3f* finalPos) {
    finalPos->x = npcAction->endPos.x;
    finalPos->y = npcAction->endPos.y;
    finalPos->z = npcAction->endPos.z;
}

void ObjDekujr_ComeUp(ObjDekujr* this, PlayState* play) {
    CsCmdActorAction* csCmdNPCAction;
    Vec3f initPos;
    Vec3f finalPos;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    f32 actionLength;
    f32 gravity;

    if (play->csCtx.state == CS_STATE_IDLE) {
        this->unk_19C = 2;
        this->unk_19B = 0;
    } else {
        if (play->csCtx.frames == 351) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_COME_UP_DEKU_JR);
        }
        csCmdNPCAction = play->csCtx.npcActions[1];
        if (csCmdNPCAction != NULL) {
            ObjDekujr_SetInitialPos(csCmdNPCAction, &initPos);
            ObjDekujr_SetFinalPos(csCmdNPCAction, &finalPos);
            if (this->unk_19C == 0) {
                this->actor.world.pos = initPos;
                this->unk_19C = 1;
            }
            this->actor.shape.rot.x = csCmdNPCAction->urot.x;
            this->actor.shape.rot.y = csCmdNPCAction->urot.y;
            this->actor.shape.rot.z = csCmdNPCAction->urot.z;
            this->actor.velocity = velocity;
            if (csCmdNPCAction->endFrame >= play->csCtx.frames) {
                actionLength = csCmdNPCAction->endFrame - csCmdNPCAction->startFrame;
                this->actor.velocity.x = (finalPos.x - initPos.x) / actionLength;
                gravity = this->actor.gravity;
                this->actor.velocity.y = (finalPos.y - initPos.y) / actionLength;
                this->actor.velocity.y += gravity;
                if (this->actor.velocity.y < this->actor.minVelocityY) {
                    this->actor.velocity.y = this->actor.minVelocityY;
                }
                this->actor.velocity.z = (finalPos.z - initPos.z) / actionLength;
            }
        }
    }
}

void ObjDekujr_Update(Actor* thisx, PlayState* play) {
    ObjDekujr* this = (ObjDekujr*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    if ((gSaveContext.cutsceneIndex >= 0xFFF0) && (this->unk_19B == 0)) {
        this->unk_19C = 0;
        this->unk_19B = 1;
    }
    if (this->unk_19B == 1) {
        ObjDekujr_ComeUp(this, play);
        this->actor.world.pos.x += this->actor.velocity.x;
        this->actor.world.pos.y += this->actor.velocity.y;
        this->actor.world.pos.z += this->actor.velocity.z;
    } else {
        func_80037D98(play, &this->actor, 0xF, &this->unk_1A0);
        Actor_SetFocus(&this->actor, 40.0f);
    }
}

void ObjDekujr_Draw(Actor* thisx, PlayState* play) {
    u32 frameCount;

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D18(play->state.gfxCtx);
    func_80093D84(play->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, object_dekujr_DL_0030D0);

    frameCount = play->state.frames;
    gSPSegment(
        POLY_XLU_DISP++, 0x08,
        Gfx_TwoTexScroll(play->state.gfxCtx, 0, frameCount % 128, 0, 32, 32, 1, frameCount % 128, 0, 32, 32));
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, object_dekujr_DL_0032D8);

    CLOSE_DISPS(play->state.gfxCtx);
}
