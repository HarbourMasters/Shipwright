/*
 * File: z_bg_haka_water.c
 * Overlay: ovl_Bg_Haka_Water
 * Description: Bottom of the Well water level changer
 */

#include "z_bg_haka_water.h"
#include "objects/object_hakach_objects/object_hakach_objects.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgHakaWater_Init(Actor* thisx, GlobalContext* globalCtx);
void BgHakaWater_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgHakaWater_Update(Actor* thisx, GlobalContext* globalCtx);
void BgHakaWater_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgHakaWater_LowerWater(BgHakaWater* this, GlobalContext* globalCtx);
void BgHakaWater_Wait(BgHakaWater* this, GlobalContext* globalCtx);
void BgHakaWater_ChangeWaterLevel(BgHakaWater* this, GlobalContext* globalCtx);

const ActorInit Bg_Haka_Water_InitVars = {
    ACTOR_BG_HAKA_WATER,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_HAKACH_OBJECTS,
    sizeof(BgHakaWater),
    (ActorFunc)BgHakaWater_Init,
    (ActorFunc)BgHakaWater_Destroy,
    (ActorFunc)BgHakaWater_Update,
    (ActorFunc)BgHakaWater_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgHakaWater_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgHakaWater* this = (BgHakaWater*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    if (Flags_GetSwitch(globalCtx, this->actor.params)) {
        this->isLowered = true;
        this->actor.home.pos.y -= 200.0f;
        this->actor.world.pos.y = this->actor.home.pos.y;
    } else {
        this->isLowered = false;
    }
    BgHakaWater_LowerWater(this, globalCtx);
    this->actionFunc = BgHakaWater_Wait;
}

void BgHakaWater_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void BgHakaWater_LowerWater(BgHakaWater* this, GlobalContext* globalCtx) {
    s32 i;

    for (i = 0; i < 9; i++) {
        globalCtx->colCtx.colHeader->waterBoxes[i].ySurface = (s16)this->actor.world.pos.y - 8;
    }
}

void BgHakaWater_Wait(BgHakaWater* this, GlobalContext* globalCtx) {
    if ((!this->isLowered && Flags_GetSwitch(globalCtx, this->actor.params)) ||
        (this->isLowered && !Flags_GetSwitch(globalCtx, this->actor.params))) {
        if (this->isLowered) {
            this->isLowered = false;
            this->actor.draw = BgHakaWater_Draw;
            this->actor.home.pos.y += 200.0f;
        } else {
            this->isLowered = true;
            this->actor.home.pos.y -= 200.0f;
        }
        this->actionFunc = BgHakaWater_ChangeWaterLevel;
    }
}

void BgHakaWater_ChangeWaterLevel(BgHakaWater* this, GlobalContext* globalCtx) {
    if (!this->isLowered && Flags_GetSwitch(globalCtx, this->actor.params)) {
        this->isLowered = true;
        this->actor.home.pos.y -= 200.0f;
    } else if (this->isLowered && !Flags_GetSwitch(globalCtx, this->actor.params)) {
        this->isLowered = false;
        this->actor.home.pos.y += 200.0f;
    }

    if (this->actor.home.pos.y < this->actor.world.pos.y) {
        func_8002F948(&this->actor, NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
    } else {
        func_8002F948(&this->actor, NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
    }

    if (Math_StepToF(&this->actor.world.pos.y, this->actor.home.pos.y, 0.5f) != 0) {
        this->actionFunc = BgHakaWater_Wait;
        if (this->isLowered) {
            this->actor.draw = NULL;
        }
    }
    BgHakaWater_LowerWater(this, globalCtx);
}

void BgHakaWater_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgHakaWater* this = (BgHakaWater*)thisx;

    this->actionFunc(this, globalCtx);
}

void BgHakaWater_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgHakaWater* this = (BgHakaWater*)thisx;
    s32 pad;
    f32 temp;
    s32 pad2;

    OPEN_DISPS(globalCtx->state.gfxCtx);
    func_80093D84(globalCtx->state.gfxCtx);

    if (this->isLowered) {
        temp = this->actor.world.pos.y - this->actor.home.pos.y;
    } else {
        temp = this->actor.world.pos.y - (this->actor.home.pos.y - 200.0f);
    }

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (u8)(0.765f * temp));
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, globalCtx->gameplayFrames % 128,
                                globalCtx->gameplayFrames % 128, 32, 32, 1, 0, (0 - globalCtx->gameplayFrames) % 128,
                                32, 32));

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gBotwWaterRingDL);

    Matrix_Translate(0.0f, 92.0f, -1680.0f, MTXMODE_NEW);
    Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    temp -= 170.0f;
    if (temp < 0.0f) {
        temp = 0.0f;
    }

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (u8)(5.1f * temp));
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gBotwWaterFallDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
