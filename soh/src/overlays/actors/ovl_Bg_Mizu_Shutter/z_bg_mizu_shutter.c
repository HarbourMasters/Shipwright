#include "z_bg_mizu_shutter.h"
#include "objects/object_mizu_objects/object_mizu_objects.h"

#define FLAGS ACTOR_FLAG_4

#define SIZE_PARAM (((u16)this->dyna.actor.params >> 0xC) & 0xF)
#define TIMER_PARAM (((u16)this->dyna.actor.params >> 6) & 0x3F)

void BgMizuShutter_Init(BgMizuShutter* this, PlayState* play);
void BgMizuShutter_Destroy(BgMizuShutter* this, PlayState* play);
void BgMizuShutter_Update(BgMizuShutter* this, PlayState* play);
void BgMizuShutter_Draw(BgMizuShutter* this, PlayState* play);

void BgMizuShutter_WaitForTimer(BgMizuShutter* this, PlayState* play);
void BgMizuShutter_WaitForSwitch(BgMizuShutter* this, PlayState* play);
void BgMizuShutter_Move(BgMizuShutter* this, PlayState* play);
void BgMizuShutter_WaitForCutscene(BgMizuShutter* this, PlayState* play);

const ActorInit Bg_Mizu_Shutter_InitVars = {
    ACTOR_BG_MIZU_SHUTTER,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_MIZU_OBJECTS,
    sizeof(BgMizuShutter),
    (ActorFunc)BgMizuShutter_Init,
    (ActorFunc)BgMizuShutter_Destroy,
    (ActorFunc)BgMizuShutter_Update,
    (ActorFunc)BgMizuShutter_Draw,
    NULL,
};

static Gfx* sDisplayLists[] = { gObjectMizuObjectsShutterDL_007130, gObjectMizuObjectsShutterDL_0072D0 };

static CollisionHeader* sCollisionHeaders[] = {
    &gObjectMizuObjectsShutterCol_007250,
    &gObjectMizuObjectsShutterCol_0073F0,
};

static Vec3f sDisplacements[] = {
    { 0.0f, 100.0f, 0.0f },
    { 0.0f, 140.0f, 0.0f },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgMizuShutter_Init(BgMizuShutter* thisx, PlayState* play) {
    s32 pad;
    BgMizuShutter* this = (BgMizuShutter*)thisx;
    s32 pad2;
    CollisionHeader* sp30 = NULL;
    s32 pad3;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    this->displayList = sDisplayLists[SIZE_PARAM];
    DynaPolyActor_Init(&this->dyna, DPM_PLAYER);
    CollisionHeader_GetVirtual(sCollisionHeaders[SIZE_PARAM], &sp30);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, sp30);
    if ((SIZE_PARAM == BGMIZUSHUTTER_SMALL) || (SIZE_PARAM == BGMIZUSHUTTER_LARGE)) {
        this->closedPos = this->dyna.actor.world.pos;
        this->timer = 0;
        this->timerMax = TIMER_PARAM * 20;
        Matrix_RotateY(this->dyna.actor.world.rot.y * (M_PI / 0x8000), MTXMODE_NEW);
        Matrix_RotateX(this->dyna.actor.world.rot.x * (M_PI / 0x8000), MTXMODE_APPLY);
        Matrix_RotateZ(this->dyna.actor.world.rot.z * (M_PI / 0x8000), MTXMODE_APPLY);
        Matrix_MultVec3f(&sDisplacements[SIZE_PARAM], &this->openPos);
        this->openPos.x += this->dyna.actor.world.pos.x;
        this->openPos.y += this->dyna.actor.world.pos.y;
        this->openPos.z += this->dyna.actor.world.pos.z;
        if (this->timerMax != 0x3F * 20) {
            Flags_UnsetSwitch(play, (u16)this->dyna.actor.params & 0x3F);
            this->dyna.actor.world.pos = this->closedPos;
        }
        if (Flags_GetSwitch(play, (u16)this->dyna.actor.params & 0x3F)) {
            this->dyna.actor.world.pos = this->openPos;
            this->actionFunc = BgMizuShutter_WaitForTimer;
        } else {
            this->actionFunc = BgMizuShutter_WaitForSwitch;
        }
    }
}

void BgMizuShutter_Destroy(BgMizuShutter* thisx, PlayState* play) {
    s32 pad;
    BgMizuShutter* this = (BgMizuShutter*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgMizuShutter_WaitForSwitch(BgMizuShutter* this, PlayState* play) {
    if (Flags_GetSwitch(play, (u16)this->dyna.actor.params & 0x3F)) {
        if (ABS(this->dyna.actor.world.rot.x) > 0x2C60) {
            OnePointCutscene_Init(play, 4510, -99, &this->dyna.actor, MAIN_CAM);
        } else {
            OnePointCutscene_Attention(play, &this->dyna.actor);
        }
        this->actionFunc = BgMizuShutter_WaitForCutscene;
        this->timer = 30;
    }
}

void BgMizuShutter_WaitForCutscene(BgMizuShutter* this, PlayState* play) {
    if (this->timer-- == 0) {
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_METALDOOR_OPEN);
        this->actionFunc = BgMizuShutter_Move;
    }
}

void BgMizuShutter_Move(BgMizuShutter* this, PlayState* play) {
    if (Flags_GetSwitch(play, (u16)this->dyna.actor.params & 0x3F)) {
        Math_SmoothStepToF(&this->dyna.actor.world.pos.x, this->openPos.x, 1.0f, 4.0f, 0.1f);
        Math_SmoothStepToF(&this->dyna.actor.world.pos.y, this->openPos.y, 1.0f, 4.0f, 0.1f);
        Math_SmoothStepToF(&this->dyna.actor.world.pos.z, this->openPos.z, 1.0f, 4.0f, 0.1f);
        if ((this->dyna.actor.world.pos.x == this->openPos.x) && (this->dyna.actor.world.pos.y == this->openPos.y) &&
            (this->dyna.actor.world.pos.z == this->openPos.z)) {
            this->timer = this->timerMax;
            this->actionFunc = BgMizuShutter_WaitForTimer;
        }
    } else {
        Math_SmoothStepToF(&this->maxSpeed, 20.0f, 1.0f, 3.0f, 0.1f);
        Math_SmoothStepToF(&this->dyna.actor.world.pos.x, this->closedPos.x, 1.0f, this->maxSpeed, 0.1f);
        Math_SmoothStepToF(&this->dyna.actor.world.pos.y, this->closedPos.y, 1.0f, this->maxSpeed, 0.1f);
        Math_SmoothStepToF(&this->dyna.actor.world.pos.z, this->closedPos.z, 1.0f, this->maxSpeed, 0.1f);
        if ((this->dyna.actor.world.pos.x == this->closedPos.x) &&
            (this->dyna.actor.world.pos.y == this->closedPos.y) &&
            (this->dyna.actor.world.pos.z == this->closedPos.z)) {
            func_800AA000(this->dyna.actor.xyzDistToPlayerSq, 0x78, 0x14, 0xA);
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_STONE_BOUND);
            this->actionFunc = BgMizuShutter_WaitForSwitch;
        }
    }
}

void BgMizuShutter_WaitForTimer(BgMizuShutter* this, PlayState* play) {
    if (this->timerMax != 0x3F * 20) {
        this->timer--;
        func_8002F994(&this->dyna.actor, this->timer);
        if (this->timer == 0) {
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_METALDOOR_CLOSE);
            Flags_UnsetSwitch(play, (u16)this->dyna.actor.params & 0x3F);
            this->actionFunc = BgMizuShutter_Move;
        }
    }
}

void BgMizuShutter_Update(BgMizuShutter* thisx, PlayState* play) {
    s32 pad;
    BgMizuShutter* this = (BgMizuShutter*)thisx;

    this->actionFunc(this, play);
}

void BgMizuShutter_Draw(BgMizuShutter* thisx, PlayState* play) {
    s32 pad;
    BgMizuShutter* this = (BgMizuShutter*)thisx;

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (this->displayList != NULL) {
        gSPDisplayList(POLY_OPA_DISP++, this->displayList);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}
