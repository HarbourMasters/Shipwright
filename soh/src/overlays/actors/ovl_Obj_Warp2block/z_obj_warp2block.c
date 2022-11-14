/*
 * File: z_obj_warp2block.c
 * Overlay: ovl_Obj_Warp2Block
 * Description: Navi Infospot (Green, Time Block)
 */

#include "z_obj_warp2block.h"
#include "objects/object_timeblock/object_timeblock.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_4 | ACTOR_FLAG_25 | ACTOR_FLAG_27)

void ObjWarp2block_Init(Actor* thisx, PlayState* play);
void ObjWarp2block_Destroy(Actor* thisx, PlayState* play);
void ObjWarp2block_Update(Actor* thisx, PlayState* play);
void ObjWarp2block_Draw(Actor* thisx, PlayState* play);

void ObjWarp2block_Spawn(ObjWarp2block* this, PlayState* play);
s32 func_80BA1ECC(ObjWarp2block* this, PlayState* play);
void ObjWarp2block_SwapWithChild(ObjWarp2block* this, PlayState* play);
s32 func_80BA2218(ObjWarp2block* this, PlayState* play);
s32 func_80BA228C(ObjWarp2block* this, PlayState* play);
s32 func_80BA2304(ObjWarp2block* this, PlayState* play);
void ObjWarp2block_SetInactive(ObjWarp2block* this);
void ObjWarp2block_DoNothing(ObjWarp2block* this, PlayState* play);
void func_80BA24E8(ObjWarp2block* this);
void func_80BA24F8(ObjWarp2block* this, PlayState* play);
void func_80BA2600(ObjWarp2block* this);
void func_80BA2610(ObjWarp2block* this, PlayState* play);

const ActorInit Obj_Warp2block_InitVars = {
    ACTOR_OBJ_WARP2BLOCK,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_TIMEBLOCK,
    sizeof(ObjWarp2block),
    (ActorFunc)ObjWarp2block_Init,
    (ActorFunc)ObjWarp2block_Destroy,
    (ActorFunc)ObjWarp2block_Update,
    (ActorFunc)ObjWarp2block_Draw,
    NULL,
};

typedef struct {
    /* 0x00 */ f32 scale;
    /* 0x04 */ f32 focus;
    /* 0x08 */ s16 params;
} Warp2BlockSpawnData; // size = 0x0C

static Warp2BlockSpawnData sSpawnData[] = {
    { 1.0f, 60.0f, 0x0018 },
    { 0.6f, 40.0f, 0x0019 },
};

static f32 sDistances[] = { 60.0f, 100.0f, 140.0f, 180.0f, 220.0f, 260.0f, 300.0f, 300.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 1800, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 300, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1500, ICHAIN_STOP),
};

static Color_RGB8 sColors[] = {
    { 100, 120, 140 }, { 80, 140, 200 }, { 100, 150, 200 }, { 100, 200, 240 },
    { 80, 110, 140 },  { 70, 160, 225 }, { 80, 100, 130 },  { 100, 110, 190 },
};

void ObjWarp2block_Spawn(ObjWarp2block* this, PlayState* play) {
    Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_EFFECT, this->dyna.actor.world.pos.x,
                this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z, 0, 0, 0,
                sSpawnData[(this->dyna.actor.params >> 8) & 1].params);

    Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_EFFECT, this->dyna.actor.child->world.pos.x,
                this->dyna.actor.child->world.pos.y, this->dyna.actor.child->world.pos.z, 0, 0, 0,
                sSpawnData[(this->dyna.actor.child->params >> 8) & 1].params);
}

s32 func_80BA1ECC(ObjWarp2block* this, PlayState* play) {
    s32 pad;
    Actor* temp_a3;
    Player* player;
    Vec3f sp20;
    f32 temp_f2;

    if (func_80043590(&this->dyna)) {
        return 0;
    }

    temp_a3 = this->dyna.actor.child;
    player = GET_PLAYER(play);
    if ((this->dyna.actor.xzDistToPlayer <= sDistances[(((this->dyna.actor.params >> 0xB) & 7))]) ||
        (temp_a3->xzDistToPlayer <= sDistances[(((temp_a3->params >> 0xB) & 7))])) {

        func_8002DBD0(&this->dyna.actor, &sp20, &player->actor.world.pos);
        temp_f2 = (this->dyna.actor.scale.x * 50.0f) + 6.0f;

        if (!(temp_f2 < fabsf(sp20.x)) && !(temp_f2 < fabsf(sp20.z))) {
            return 0;
        }

        func_8002DBD0(temp_a3, &sp20, &player->actor.world.pos);
        temp_f2 = (temp_a3->scale.x * 50.0f) + 6.0f;

        if (!(temp_f2 < fabsf(sp20.x)) && !(temp_f2 < fabsf(sp20.z))) {
            return 0;
        }
    } else {
        return 0;
    }

    return 1;
}

void ObjWarp2block_SwapWithChild(ObjWarp2block* this, PlayState* play) {
    Vec3f tempVec;
    Vec3s tempRot;
    s32 temp;

    Math_Vec3f_Copy(&tempVec, &this->dyna.actor.world.pos);
    Math_Vec3f_Copy(&this->dyna.actor.world.pos, &this->dyna.actor.child->world.pos);
    Math_Vec3f_Copy(&this->dyna.actor.child->world.pos, &tempVec);

    temp = this->dyna.actor.world.rot.y;
    this->dyna.actor.world.rot.y = this->dyna.actor.child->world.rot.y;
    this->dyna.actor.child->world.rot.y = temp;

    temp = this->dyna.actor.shape.rot.y;
    this->dyna.actor.shape.rot.y = this->dyna.actor.child->shape.rot.y;
    this->dyna.actor.child->shape.rot.y = temp;

    temp = this->dyna.actor.home.rot.z;
    this->dyna.actor.home.rot.z = this->dyna.actor.child->home.rot.z;
    this->dyna.actor.child->home.rot.z = temp;

    Math_Vec3f_Copy(&tempVec, &this->dyna.actor.scale);
    Math_Vec3f_Copy(&this->dyna.actor.scale, &this->dyna.actor.child->scale);
    Math_Vec3f_Copy(&this->dyna.actor.child->scale, &tempVec);

    Math_Vec3f_Copy(&tempVec, &this->dyna.actor.focus.pos);
    Math_Vec3f_Copy(&this->dyna.actor.focus.pos, &this->dyna.actor.child->focus.pos);
    Math_Vec3f_Copy(&this->dyna.actor.child->focus.pos, &tempVec);

    tempRot = this->dyna.actor.focus.rot;
    this->dyna.actor.focus.rot = this->dyna.actor.child->focus.rot;
    this->dyna.actor.child->focus.rot = tempRot;

    temp = this->dyna.actor.params & 0x7FFF;
    this->dyna.actor.params = (this->dyna.actor.params & 0x8000) | (this->dyna.actor.child->params & 0x7FFF);
    this->dyna.actor.child->params = (this->dyna.actor.child->params & 0x8000) | (temp & 0x7FFF);

    if (Math3D_Vec3fDistSq(&this->dyna.actor.world.pos, &this->dyna.actor.home.pos) < 0.01f) {
        Flags_UnsetSwitch(play, this->dyna.actor.params & 0x3F);
    } else {
        Flags_SetSwitch(play, this->dyna.actor.params & 0x3F);
    }
}

s32 func_80BA2218(ObjWarp2block* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (func_80BA1ECC(this, play)) {
        if (player->stateFlags2 & 0x1000000) {
            func_8010BD58(play, OCARINA_ACTION_FREE_PLAY);
            this->func_168 = func_80BA228C;
        } else {
            player->stateFlags2 |= 0x800000;
        }
    }

    return 0;
}

s32 func_80BA228C(ObjWarp2block* this, PlayState* play) {
    if (play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        this->func_168 = func_80BA2218;
    }

    if (play->msgCtx.lastPlayedSong == OCARINA_SONG_TIME) {
        if (this->unk_172 == 0xFE) {
            this->unk_16E = 0x6E;
        } else {
            this->unk_16E--;
            if (this->unk_16E == 0) {
                return 1;
            }
        }
    }
    return 0;
}

s32 func_80BA2304(ObjWarp2block* this, PlayState* play) {
    s32 ret = this->func_168(this, play);

    this->unk_172 = play->msgCtx.lastPlayedSong;

    return ret;
}

void ObjWarp2block_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjWarp2block* this = (ObjWarp2block*)thisx;
    CollisionHeader* collisionHeader;

    collisionHeader = NULL;
    this->dyna.actor.world.rot.z = this->dyna.actor.shape.rot.z = 0;
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);

    Actor_SetScale(&this->dyna.actor, sSpawnData[(this->dyna.actor.params >> 8) & 1].scale);
    this->func_168 = func_80BA2218;
    Actor_SetFocus(&this->dyna.actor, sSpawnData[(this->dyna.actor.params >> 8) & 1].focus);

    if ((this->dyna.actor.params >> 0xF) & 1) {
        func_80BA24E8(this);
        if (Flags_GetSwitch(play, this->dyna.actor.params & 0x3F)) {
            this->dyna.actor.draw = NULL;
        }
        DynaPolyActor_Init(&this->dyna, 0);
        CollisionHeader_GetVirtual(&gSongOfTimeBlockCol, &collisionHeader);
        this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, collisionHeader);
    } else {
        ObjWarp2block_SetInactive(this);
    }

    osSyncPrintf("時のブロック(ワープ２) (<arg> %04xH <type> color:%d range:%d)\n", this->dyna.actor.params & 0xFFFF,
                 this->dyna.actor.home.rot.z & 7, (this->dyna.actor.params >> 0xB) & 7);
}

void ObjWarp2block_Destroy(Actor* thisx, PlayState* play) {
    ObjWarp2block* this = (ObjWarp2block*)thisx;
    if ((this->dyna.actor.params >> 0xF) & 1) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void ObjWarp2block_SetInactive(ObjWarp2block* this) {
    this->actionFunc = ObjWarp2block_DoNothing;
    this->dyna.actor.draw = NULL;
}

void ObjWarp2block_DoNothing(ObjWarp2block* this, PlayState* play) {
}

void func_80BA24E8(ObjWarp2block* this) {
    this->actionFunc = func_80BA24F8;
}

void func_80BA24F8(ObjWarp2block* this, PlayState* play) {
    Actor* current = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;

    while (current != NULL) {
        if (current->id == ACTOR_OBJ_WARP2BLOCK && !((current->params >> 0xF) & 1) &&
            ((this->dyna.actor.params & 0x3F) == (current->params & 0x3F))) {
            this->dyna.actor.child = current;
            if (Flags_GetSwitch(play, this->dyna.actor.params & 0x3F)) {
                ObjWarp2block_SwapWithChild(this, play);
                this->dyna.actor.draw = ObjWarp2block_Draw;
            }
            func_80BA2600(this);
            return;
        }

        current = current->next;
    }

    this->unk_174++;
    if (this->unk_174 > 60) {
        osSyncPrintf(VT_COL(RED, WHITE));
        osSyncPrintf("Error : 時のブロック(ワープ２)が対でセットされていません(%s %d)\n", __FILE__, __LINE__);
        osSyncPrintf(VT_RST);
        Actor_Kill(&this->dyna.actor);
    }
}

void func_80BA2600(ObjWarp2block* this) {
    this->actionFunc = func_80BA2610;
}

void func_80BA2610(ObjWarp2block* this, PlayState* play) {
    if ((func_80BA2304(this, play) != 0) && (this->unk_16C <= 0)) {
        ObjWarp2block_Spawn(this, play);
        this->unk_16C = 0xA0;
        OnePointCutscene_Attention(play, &this->dyna.actor);
        this->unk_170 = 0xC;
    }

    if (this->unk_170 > 0) {
        this->unk_170--;
        if (this->unk_170 == 0) {
            ObjWarp2block_SwapWithChild(this, play);
        }
    }
    if (this->unk_16C == 0x32) {
        func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
    }
}

void ObjWarp2block_Update(Actor* thisx, PlayState* play) {
    ObjWarp2block* this = (ObjWarp2block*)thisx;

    this->actionFunc(this, play);
    if (this->unk_16C > 0) {
        this->unk_16C--;
    }
}

void ObjWarp2block_Draw(Actor* thisx, PlayState* play) {
    Color_RGB8* sp44;

    sp44 = &sColors[thisx->home.rot.z & 7];

    OPEN_DISPS(play->state.gfxCtx);
    func_80093D18(play->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sp44->r, sp44->g, sp44->b, 255);
    gSPDisplayList(POLY_OPA_DISP++, gSongOfTimeBlockDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
