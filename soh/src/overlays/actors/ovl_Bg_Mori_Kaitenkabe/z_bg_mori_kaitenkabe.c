/*
 * File: z_bg_mori_kaitenkabe.c
 * Overlay: ovl_Bg_Mori_Kaitenkabe
 * Description: Rotating wall in Forest Temple basement
 */

#include "z_bg_mori_kaitenkabe.h"
#include "objects/object_mori_objects/object_mori_objects.h"

#define FLAGS 0

void BgMoriKaitenkabe_Init(Actor* thisx, PlayState* play);
void BgMoriKaitenkabe_Destroy(Actor* thisx, PlayState* play);
void BgMoriKaitenkabe_Update(Actor* thisx, PlayState* play);
void BgMoriKaitenkabe_Draw(Actor* thisx, PlayState* play);

void BgMoriKaitenkabe_WaitForMoriTex(BgMoriKaitenkabe* this, PlayState* play);
void BgMoriKaitenkabe_SetupWait(BgMoriKaitenkabe* this);
void BgMoriKaitenkabe_Wait(BgMoriKaitenkabe* this, PlayState* play);
void BgMoriKaitenkabe_SetupRotate(BgMoriKaitenkabe* this);
void BgMoriKaitenkabe_Rotate(BgMoriKaitenkabe* this, PlayState* play);

const ActorInit Bg_Mori_Kaitenkabe_InitVars = {
    ACTOR_BG_MORI_KAITENKABE,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MORI_OBJECTS,
    sizeof(BgMoriKaitenkabe),
    (ActorFunc)BgMoriKaitenkabe_Init,
    (ActorFunc)BgMoriKaitenkabe_Destroy,
    (ActorFunc)BgMoriKaitenkabe_Update,
    NULL,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void BgMoriKaitenkabe_CrossProduct(Vec3f* dest, Vec3f* v1, Vec3f* v2) {
    dest->x = (v1->y * v2->z) - (v1->z * v2->y);
    dest->y = (v1->z * v2->x) - (v1->x * v2->z);
    dest->z = (v1->x * v2->y) - (v1->y * v2->x);
}

void BgMoriKaitenkabe_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriKaitenkabe* this = (BgMoriKaitenkabe*)thisx;
    CollisionHeader* colHeader = NULL;

    // "Forest Temple object 【Rotating Wall (arg_data: 0x% 04x)】 appears"
    osSyncPrintf("◯◯◯森の神殿オブジェクト【回転壁(arg_data : 0x%04x)】出現 \n", this->dyna.actor.params);
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gMoriKaitenkabeCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->moriTexObjIndex = Object_GetIndex(&play->objectCtx, OBJECT_MORI_TEX);
    if (this->moriTexObjIndex < 0) {
        Actor_Kill(&this->dyna.actor);
        // "【Rotating wall】 Bank danger!"
        osSyncPrintf("【回転壁】 バンク危険！(%s %d)\n", __FILE__, __LINE__);
    } else {
        this->actionFunc = BgMoriKaitenkabe_WaitForMoriTex;
    }
}

void BgMoriKaitenkabe_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriKaitenkabe* this = (BgMoriKaitenkabe*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgMoriKaitenkabe_WaitForMoriTex(BgMoriKaitenkabe* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->moriTexObjIndex)) {
        BgMoriKaitenkabe_SetupWait(this);
        this->dyna.actor.draw = BgMoriKaitenkabe_Draw;
    }
}

void BgMoriKaitenkabe_SetupWait(BgMoriKaitenkabe* this) {
    this->actionFunc = BgMoriKaitenkabe_Wait;
    this->timer = 0;
}

void BgMoriKaitenkabe_Wait(BgMoriKaitenkabe* this, PlayState* play) {
    Vec3f push;
    Vec3f leverArm;
    Vec3f torque;
    Player* player = GET_PLAYER(play);

    if (this->dyna.unk_150 > 0.001f) {
        this->timer++;
        if ((this->timer > (28 - CVar_GetS32("gFasterBlockPush", 0) * 4)) && !Player_InCsMode(play)) {
            BgMoriKaitenkabe_SetupRotate(this);
            func_8002DF54(play, &this->dyna.actor, 8);
            Math_Vec3f_Copy(&this->lockedPlayerPos, &player->actor.world.pos);
            push.x = Math_SinS(this->dyna.unk_158);
            push.y = 0.0f;
            push.z = Math_CosS(this->dyna.unk_158);
            leverArm.x = this->dyna.actor.world.pos.x - player->actor.world.pos.x;
            leverArm.y = 0.0f;
            leverArm.z = this->dyna.actor.world.pos.z - player->actor.world.pos.z;
            BgMoriKaitenkabe_CrossProduct(&torque, &push, &leverArm);
            this->rotDirection = (torque.y > 0.0f) ? 1.0f : -1.0f;
        }
    } else {
        this->timer = 0;
    }
    if (fabsf(this->dyna.unk_150) > 0.001f) {
        this->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~0x10;
    }
}

void BgMoriKaitenkabe_SetupRotate(BgMoriKaitenkabe* this) {
    this->actionFunc = BgMoriKaitenkabe_Rotate;
    this->rotSpeed = CVar_GetS32("gFasterBlockPush", 0) * 0.1f;
    this->rotYdeg = 0.0f;
}

void BgMoriKaitenkabe_Rotate(BgMoriKaitenkabe* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* thisx = &this->dyna.actor;
    s16 rotY;

    Math_StepToF(&this->rotSpeed, 0.6f, 0.02f);
    if (Math_StepToF(&this->rotYdeg, this->rotDirection * 45.0f, this->rotSpeed)) {
        BgMoriKaitenkabe_SetupWait(this);
        func_8002DF54(play, thisx, 7);
        if (this->rotDirection > 0.0f) {
            thisx->home.rot.y += 0x2000;
        } else {
            thisx->home.rot.y -= 0x2000;
        }
        thisx->world.rot.y = thisx->shape.rot.y = thisx->home.rot.y;
        func_800788CC(NA_SE_EV_STONEDOOR_STOP);
    } else {
        rotY = this->rotYdeg * (0x10000 / 360.0f);
        thisx->world.rot.y = thisx->shape.rot.y = thisx->home.rot.y + rotY;
        func_800788CC(NA_SE_EV_WALL_SLIDE - SFX_FLAG);
    }
    if (fabsf(this->dyna.unk_150) > 0.001f) {
        this->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~0x10;
    }
    Math_Vec3f_Copy(&player->actor.world.pos, &this->lockedPlayerPos);
}

void BgMoriKaitenkabe_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriKaitenkabe* this = (BgMoriKaitenkabe*)thisx;

    this->actionFunc(this, play);
}

void BgMoriKaitenkabe_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriKaitenkabe* this = (BgMoriKaitenkabe*)thisx;

    OPEN_DISPS(play->state.gfxCtx);
    func_80093D18(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, play->objectCtx.status[this->moriTexObjIndex].segment);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_OPA_DISP++, gMoriKaitenkabeDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
