/*
 * File: z_bg_hidan_fwbig.c
 * Overlay: ovl_Bg_Hidan_Fwbig
 * Description: Large fire walls at Fire Temple (flame wall before bombable door and the one that chases the player in
 * the lava room)
 */

#include "z_bg_hidan_fwbig.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS ACTOR_FLAG_4

typedef enum {
    /* 0 */ FWBIG_MOVE,
    /* 1 */ FWBIG_RESET,
    /* 2 */ FWBIG_KILL
} HidanFwbigMoveState;

void BgHidanFwbig_Init(Actor* thisx, PlayState* play);
void BgHidanFwbig_Destroy(Actor* thisx, PlayState* play);
void BgHidanFwbig_Update(Actor* thisx, PlayState* play);
void BgHidanFwbig_Draw(Actor* thisx, PlayState* play);

void BgHidanFwbig_UpdatePosition(BgHidanFwbig* this);

void BgHidanFwbig_WaitForSwitch(BgHidanFwbig* this, PlayState* play);
void BgHidanFwbig_WaitForCs(BgHidanFwbig* this, PlayState* play);
void BgHidanFwbig_Lower(BgHidanFwbig* this, PlayState* play);
void BgHidanFwbig_WaitForTimer(BgHidanFwbig* this, PlayState* play);
void BgHidanFwbig_WaitForPlayer(BgHidanFwbig* this, PlayState* play);
void BgHidanFwbig_Move(BgHidanFwbig* this, PlayState* play);

const ActorInit Bg_Hidan_Fwbig_InitVars = {
    ACTOR_BG_HIDAN_FWBIG,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_HIDAN_OBJECTS,
    sizeof(BgHidanFwbig),
    (ActorFunc)BgHidanFwbig_Init,
    (ActorFunc)BgHidanFwbig_Destroy,
    (ActorFunc)BgHidanFwbig_Update,
    (ActorFunc)BgHidanFwbig_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x20000000, 0x01, 0x04 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 30, 130, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 1000, ICHAIN_STOP),
};

void BgHidanFwbig_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgHidanFwbig* this = (BgHidanFwbig*)thisx;
    Player* player = GET_PLAYER(play);

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->direction = (u16)(thisx->params >> 8);
    thisx->params &= 0xFF;
    if (this->direction != 0) {
        this->actor.home.pos.x = 1560.0f;
        this->actor.home.pos.z = 0.0f;
        if (player->actor.world.pos.z > 300.0f) {
            this->direction = -1;
            this->actor.home.rot.y = this->actor.shape.rot.y = -0x4E38;
        } else if (player->actor.world.pos.z < -300.0f) {
            this->direction = 1;
            this->actor.home.rot.y = this->actor.shape.rot.y = -0x31C8;
        } else {
            Actor_Kill(&this->actor);
            return;
        }
        BgHidanFwbig_UpdatePosition(this);
        Actor_SetScale(&this->actor, 0.15f);
        this->collider.dim.height = 230;
        this->actor.flags |= ACTOR_FLAG_4;
        this->moveState = FWBIG_MOVE;
        this->actionFunc = BgHidanFwbig_WaitForPlayer;
        this->actor.world.pos.y = this->actor.home.pos.y - (2400.0f * this->actor.scale.y);
    } else {
        Actor_SetScale(&this->actor, 0.1f);
        this->actionFunc = BgHidanFwbig_WaitForSwitch;
    }
}

void BgHidanFwbig_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHidanFwbig* this = (BgHidanFwbig*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void BgHidanFwbig_UpdatePosition(BgHidanFwbig* this) {
    s16 startAngle = this->actor.shape.rot.y + this->direction * -0x4000;

    this->actor.world.pos.x = (Math_SinS(startAngle) * 885.4f) + this->actor.home.pos.x;
    this->actor.world.pos.z = (Math_CosS(startAngle) * 885.4f) + this->actor.home.pos.z;
}

void BgHidanFwbig_WaitForSwitch(BgHidanFwbig* this, PlayState* play) {
    if (Flags_GetSwitch(play, this->actor.params)) {
        this->actionFunc = BgHidanFwbig_WaitForCs;
        OnePointCutscene_Init(play, 3340, -99, &this->actor, MAIN_CAM);
        this->timer = 35;
    }
}

void BgHidanFwbig_WaitForCs(BgHidanFwbig* this, PlayState* play) {
    if (this->timer-- == 0) {
        this->actionFunc = BgHidanFwbig_Lower;
    }
}

void BgHidanFwbig_Rise(BgHidanFwbig* this, PlayState* play) {
    if (Math_StepToF(&this->actor.world.pos.y, this->actor.home.pos.y, 10.0f)) {
        if (this->direction == 0) {
            Flags_UnsetSwitch(play, this->actor.params);
            this->actionFunc = BgHidanFwbig_WaitForSwitch;
        } else {
            this->actionFunc = BgHidanFwbig_Move;
        }
    }
}

void BgHidanFwbig_Lower(BgHidanFwbig* this, PlayState* play) {
    if (Math_StepToF(&this->actor.world.pos.y, this->actor.home.pos.y - (2400.0f * this->actor.scale.y), 10.0f)) {
        if (this->direction == 0) {
            this->actionFunc = BgHidanFwbig_WaitForTimer;
            this->timer = 150;
        } else if (this->moveState == FWBIG_KILL) {
            Actor_Kill(&this->actor);
        } else {
            if (this->moveState == FWBIG_MOVE) {
                this->actor.shape.rot.y -= (this->direction * 0x1800);
            } else {
                this->moveState = FWBIG_MOVE;
                this->actor.shape.rot.y = this->actor.home.rot.y;
            }
            BgHidanFwbig_UpdatePosition(this);
            this->actionFunc = BgHidanFwbig_Rise;
        }
    }
}

void BgHidanFwbig_WaitForTimer(BgHidanFwbig* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        this->actionFunc = BgHidanFwbig_Rise;
    }
    func_8002F994(&this->actor, this->timer);
}

void BgHidanFwbig_WaitForPlayer(BgHidanFwbig* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->actor.world.pos.x < 1150.0f) {
        this->actionFunc = BgHidanFwbig_Rise;
        OnePointCutscene_Init(play, 3290, -99, &this->actor, MAIN_CAM);
    }
}

void BgHidanFwbig_Move(BgHidanFwbig* this, PlayState* play) {
    if (!Player_InCsMode(play)) {
        if (Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y + (this->direction * 0x6390), 0x20)) {
            this->moveState = FWBIG_RESET;
            this->actionFunc = BgHidanFwbig_Lower;
        } else {
            BgHidanFwbig_UpdatePosition(this);
        }
    }
}

void BgHidanFwbig_MoveCollider(BgHidanFwbig* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f projPos;
    f32 cs;
    f32 sn;

    func_8002DBD0(&this->actor, &projPos, &player->actor.world.pos);
    projPos.z = ((projPos.z >= 0.0f) ? 1.0f : -1.0f) * 25.0f * -1.0f;
    if (this->direction == 0) {
        projPos.x = CLAMP(projPos.x, -360.0f, 360.0f);
    } else {
        projPos.x = CLAMP(projPos.x, -500.0f, 500.0f);
    }

    sn = Math_SinS(this->actor.shape.rot.y);
    cs = Math_CosS(this->actor.shape.rot.y);
    this->collider.dim.pos.x = this->actor.world.pos.x + (projPos.x * cs) + (projPos.z * sn);
    this->collider.dim.pos.z = this->actor.world.pos.z - (projPos.x * sn) + (projPos.z * cs);
    this->collider.dim.pos.y = this->actor.world.pos.y;

    this->actor.world.rot.y = (projPos.z < 0.0f) ? this->actor.shape.rot.y : this->actor.shape.rot.y + 0x8000;
}

void BgHidanFwbig_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHidanFwbig* this = (BgHidanFwbig*)thisx;

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        func_8002F71C(play, &this->actor, 5.0f, this->actor.world.rot.y, 1.0f);
        if (this->direction != 0) {
            this->actionFunc = BgHidanFwbig_Lower;
        }
    }
    if ((this->direction != 0) && (play->roomCtx.prevRoom.num == this->actor.room)) {
        this->moveState = FWBIG_KILL;
        this->actionFunc = BgHidanFwbig_Lower;
    }

    this->actionFunc(this, play);

    if ((this->actor.home.pos.y - 200.0f) < this->actor.world.pos.y) {
        if (gSaveContext.sceneSetupIndex < 4) {
            func_8002F974(&this->actor, NA_SE_EV_BURNING - SFX_FLAG);
        } else if ((s16)this->actor.world.pos.x == -513) {
            func_8002F974(&this->actor, NA_SE_EV_FLAME_OF_FIRE - SFX_FLAG);
        }
        BgHidanFwbig_MoveCollider(this, play);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void BgHidanFwbig_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    f32 height;

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D84(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gEffUnknown4Tex));

    gSPSegment(POLY_XLU_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gEffUnknown5Tex));

    height = thisx->scale.y * 2400.0f;
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 220, 0,
                    ((height - (thisx->home.pos.y - thisx->world.pos.y)) * 255.0f) / height);

    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, play->gameplayFrames % 0x80, 0, 0x20, 0x40, 1, 0,
                                (u8)(play->gameplayFrames * -15), 0x20, 0x40));

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_XLU_DISP++, gFireTempleBigFireWallDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
