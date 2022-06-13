/*
 * File: z_bg_mori_idomizu.c
 * Overlay: ovl_Bg_Mori_Idomizu
 * Description: Square of water in Forest Temple well
 */

#include "z_bg_mori_idomizu.h"
#include "objects/object_mori_objects/object_mori_objects.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgMoriIdomizu_Init(Actor* thisx, GlobalContext* globalCtx);
void BgMoriIdomizu_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgMoriIdomizu_Update(Actor* thisx, GlobalContext* globalCtx);
void BgMoriIdomizu_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgMoriIdomizu_SetupWaitForMoriTex(BgMoriIdomizu* this);
void BgMoriIdomizu_WaitForMoriTex(BgMoriIdomizu* this, GlobalContext* globalCtx);
void BgMoriIdomizu_SetupMain(BgMoriIdomizu* this);
void BgMoriIdomizu_Main(BgMoriIdomizu* this, GlobalContext* globalCtx);

static s16 sKankyoIsSpawned = false;

const ActorInit Bg_Mori_Idomizu_InitVars = {
    ACTOR_BG_MORI_IDOMIZU,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MORI_OBJECTS,
    sizeof(BgMoriIdomizu),
    (ActorFunc)BgMoriIdomizu_Init,
    (ActorFunc)BgMoriIdomizu_Destroy,
    (ActorFunc)BgMoriIdomizu_Update,
    NULL,
    NULL,
};

void BgMoriIdomizu_SetupAction(BgMoriIdomizu* this, BgMoriIdomizuActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void BgMoriIdomizu_SetWaterLevel(GlobalContext* globalCtx, s16 waterLevel) {
    WaterBox* waterBox = globalCtx->colCtx.colHeader->waterBoxes;

    waterBox[2].ySurface = waterLevel;
    waterBox[3].ySurface = waterLevel;
    waterBox[4].ySurface = waterLevel;
}

void BgMoriIdomizu_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriIdomizu* this = (BgMoriIdomizu*)thisx;

    if (sKankyoIsSpawned) {
        Actor_Kill(&this->actor);
        return;
    }
    this->actor.scale.x = 1.1f;
    this->actor.scale.y = 1.0f;
    this->actor.scale.z = 1.0f;
    this->actor.world.pos.x = 119.0f;
    this->actor.world.pos.z = -1820.0f;
    this->prevSwitchFlagSet = Flags_GetSwitch(globalCtx, this->actor.params & 0x3F);
    if (this->prevSwitchFlagSet != 0) {
        this->actor.world.pos.y = -282.0f;
        BgMoriIdomizu_SetWaterLevel(globalCtx, -282);
    } else {
        this->actor.world.pos.y = 184.0f;
        BgMoriIdomizu_SetWaterLevel(globalCtx, 184);
    }
    this->moriTexObjIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_MORI_TEX);
    if (this->moriTexObjIndex < 0) {
        Actor_Kill(&this->actor);
        // "Bank danger!"
        osSyncPrintf("Error : バンク危険！(arg_data 0x%04x)(%s %d)\n", this->actor.params, "../z_bg_mori_idomizu.c",
                     202);
        return;
    }
    BgMoriIdomizu_SetupWaitForMoriTex(this);
    sKankyoIsSpawned = true;
    this->isLoaded = true;
    this->actor.room = -1;
    // "Forest Temple well water"
    osSyncPrintf("(森の神殿 井戸水)(arg_data 0x%04x)\n", this->actor.params);
}

void BgMoriIdomizu_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriIdomizu* this = (BgMoriIdomizu*)thisx;

    if (this->isLoaded) {
        sKankyoIsSpawned = false;
    }
}

void BgMoriIdomizu_SetupWaitForMoriTex(BgMoriIdomizu* this) {
    BgMoriIdomizu_SetupAction(this, BgMoriIdomizu_WaitForMoriTex);
}

void BgMoriIdomizu_WaitForMoriTex(BgMoriIdomizu* this, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, this->moriTexObjIndex)) {
        BgMoriIdomizu_SetupMain(this);
        this->actor.draw = BgMoriIdomizu_Draw;
    }
}

void BgMoriIdomizu_SetupMain(BgMoriIdomizu* this) {
    BgMoriIdomizu_SetupAction(this, BgMoriIdomizu_Main);
}

void BgMoriIdomizu_Main(BgMoriIdomizu* this, GlobalContext* globalCtx) {
    s8 roomNum;
    Actor* thisx = &this->actor;
    s32 switchFlagSet;

    roomNum = globalCtx->roomCtx.curRoom.num;
    switchFlagSet = Flags_GetSwitch(globalCtx, thisx->params & 0x3F);
    if (switchFlagSet) {
        this->targetWaterLevel = -282.0f;
    } else {
        this->targetWaterLevel = 184.0f;
    }
    if (switchFlagSet && !this->prevSwitchFlagSet) {
        OnePointCutscene_Init(globalCtx, 3240, 70, thisx, MAIN_CAM);
        this->drainTimer = 90;
    } else if (!switchFlagSet && this->prevSwitchFlagSet) {
        OnePointCutscene_Init(globalCtx, 3240, 70, thisx, MAIN_CAM);
        this->drainTimer = 90;
        thisx->world.pos.y = 0.0f;
    }
    this->drainTimer--;
    if ((roomNum == 7) || (roomNum == 8) || (roomNum == 9)) {
        if (this->drainTimer < 70) {
            Math_StepToF(&thisx->world.pos.y, this->targetWaterLevel, 3.5f);
            BgMoriIdomizu_SetWaterLevel(globalCtx, thisx->world.pos.y);
            if (this->drainTimer > 0) {
                if (switchFlagSet) {
                    func_800788CC(NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
                } else {
                    func_800788CC(NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
                }
            }
        }
    } else {
        thisx->world.pos.y = this->targetWaterLevel;
        BgMoriIdomizu_SetWaterLevel(globalCtx, thisx->world.pos.y);
        Actor_Kill(thisx);
        return;
    }
    this->prevSwitchFlagSet = switchFlagSet;
}

void BgMoriIdomizu_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriIdomizu* this = (BgMoriIdomizu*)thisx;

    if (this->actionFunc != NULL) {
        this->actionFunc(this, globalCtx);
    }
}

void BgMoriIdomizu_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriIdomizu* this = (BgMoriIdomizu*)thisx;
    u32 gameplayFrames = globalCtx->gameplayFrames;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_mori_idomizu.c", 356);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_mori_idomizu.c", 360),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPSegment(POLY_XLU_DISP++, 0x08, globalCtx->objectCtx.status[this->moriTexObjIndex].segment);

    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 128);

    gSPSegment(POLY_XLU_DISP++, 0x09,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0x7F - (gameplayFrames & 0x7F), gameplayFrames % 0x80, 0x20,
                                0x20, 1, gameplayFrames & 0x7F, gameplayFrames % 0x80, 0x20, 0x20));

    gSPDisplayList(POLY_XLU_DISP++, gMoriIdomizuWaterDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_mori_idomizu.c", 382);
}
