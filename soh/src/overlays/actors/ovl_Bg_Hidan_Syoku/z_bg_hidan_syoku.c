/*
 * File: z_bg_hidan_syoku.c
 * Overlay: ovl_Bg_Hidan_Syoku
 * Description: Stone Elevator in the Fire Temple
 */

#include "z_bg_hidan_syoku.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgHidanSyoku_Init(Actor* thisx, PlayState* play);
void BgHidanSyoku_Destroy(Actor* thisx, PlayState* play);
void BgHidanSyoku_Update(Actor* thisx, PlayState* play);
void BgHidanSyoku_Draw(Actor* thisx, PlayState* play);

void func_8088F4B8(BgHidanSyoku* this, PlayState* play);
void func_8088F514(BgHidanSyoku* this, PlayState* play);
void func_8088F62C(BgHidanSyoku* this, PlayState* play);

const ActorInit Bg_Hidan_Syoku_InitVars = {
    ACTOR_BG_HIDAN_SYOKU,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HIDAN_OBJECTS,
    sizeof(BgHidanSyoku),
    (ActorFunc)BgHidanSyoku_Init,
    (ActorFunc)BgHidanSyoku_Destroy,
    (ActorFunc)BgHidanSyoku_Update,
    (ActorFunc)BgHidanSyoku_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgHidanSyoku_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHidanSyoku* this = (BgHidanSyoku*)thisx;
    CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_PLAYER);
    CollisionHeader_GetVirtual(&gFireTempleFlareDancerPlatformCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->actionFunc = func_8088F4B8;
    this->dyna.actor.home.pos.y += 540.0f;
}

void BgHidanSyoku_Destroy(Actor* thisx, PlayState* play) {
    BgHidanSyoku* this = (BgHidanSyoku*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_8088F47C(BgHidanSyoku* this) {
    this->timer = 60;
    Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
    this->actionFunc = func_8088F62C;
}

void func_8088F4B8(BgHidanSyoku* this, PlayState* play) {
    if (Flags_GetClear(play, this->dyna.actor.room) && func_8004356C(&this->dyna)) {
        this->timer = 140;
        this->actionFunc = func_8088F514;
    }
}

void func_8088F514(BgHidanSyoku* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    this->dyna.actor.world.pos.y = (cosf(this->timer * (M_PI / 140)) * 540.0f) + this->dyna.actor.home.pos.y;
    if (this->timer == 0) {
        func_8088F47C(this);
    } else {
        func_8002F974(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE3 - SFX_FLAG);
    }
}

void func_8088F5A0(BgHidanSyoku* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - (cosf(this->timer * (M_PI / 140)) * 540.0f);
    if (this->timer == 0) {
        func_8088F47C(this);
    } else {
        func_8002F974(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE3 - SFX_FLAG);
    }
}

void func_8088F62C(BgHidanSyoku* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        this->timer = 140;
        if (this->dyna.actor.world.pos.y < this->dyna.actor.home.pos.y) {
            this->actionFunc = func_8088F514;
        } else {
            this->actionFunc = func_8088F5A0;
        }
    }
}

void BgHidanSyoku_Update(Actor* thisx, PlayState* play) {
    BgHidanSyoku* this = (BgHidanSyoku*)thisx;

    this->actionFunc(this, play);
    if (func_8004356C(&this->dyna)) {
        if (this->unk_168 == 0) {
            this->unk_168 = 3;
        }
        Camera_ChangeSetting(play->cameraPtrs[MAIN_CAM], CAM_SET_FIRE_PLATFORM);
    } else if (!func_8004356C(&this->dyna)) {
        if (this->unk_168 != 0) {
            Camera_ChangeSetting(play->cameraPtrs[MAIN_CAM], CAM_SET_DUNGEON0);
        }
        this->unk_168 = 0;
    }
}

void BgHidanSyoku_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, gFireTempleFlareDancerPlatformDL);
}
