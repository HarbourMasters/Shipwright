/*
 * File: z_bg_mizu_water.c
 * Overlay: ovl_Bg_Mizu_Water
 * Description: Water plane in Water Temple. Changes height based on switches 0x1C, 0x1D, 0x1E.
 */

#include "z_bg_mizu_water.h"
#include "objects/object_mizu_objects/object_mizu_objects.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgMizuWater_Init(Actor* thisx, PlayState* play);
void BgMizuWater_Destroy(Actor* thisx, PlayState* play);
void BgMizuWater_Update(Actor* thisx, PlayState* play);
void BgMizuWater_Draw(Actor* thisx, PlayState* play);

void BgMizuWater_WaitForAction(BgMizuWater* this, PlayState* play);
void BgMizuWater_ChangeWaterLevel(BgMizuWater* this, PlayState* play);

typedef struct {
    s32 switchFlag;
    s32 yDiff;
} WaterLevel;

static WaterLevel sWaterLevels[] = {
    { 0x00, 0 },
    { WATER_TEMPLE_WATER_F3_FLAG, 0 },
    { WATER_TEMPLE_WATER_F2_FLAG, WATER_TEMPLE_WATER_F2_Y - WATER_TEMPLE_WATER_F3_Y },
    { WATER_TEMPLE_WATER_F1_FLAG, WATER_TEMPLE_WATER_F1_Y - WATER_TEMPLE_WATER_F3_Y },
};

const ActorInit Bg_Mizu_Water_InitVars = {
    ACTOR_BG_MIZU_WATER,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MIZU_OBJECTS,
    sizeof(BgMizuWater),
    (ActorFunc)BgMizuWater_Init,
    (ActorFunc)BgMizuWater_Destroy,
    (ActorFunc)BgMizuWater_Update,
    (ActorFunc)BgMizuWater_Draw,
    NULL,
};

static f32 sUnused1 = 0;
static f32 sUnused2 = 110.0f;

static u32 sWaterBoxIndexes[] = { 2, 3, 5, 7, 12, 20, 21, 22 };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F(scale, 1, ICHAIN_STOP),
};

u32 BgMizuWater_GetWaterLevelActionIndex(s16 switchFlag, PlayState* play) {
    u32 ret;

    if (bREG(0) != 0) {
        switch (bREG(1)) {
            case 0:
                Flags_SetSwitch(play, WATER_TEMPLE_WATER_F1_FLAG);
                break;
            case 1:
                Flags_SetSwitch(play, WATER_TEMPLE_WATER_F2_FLAG);
                break;
            case 2:
                Flags_SetSwitch(play, WATER_TEMPLE_WATER_F3_FLAG);
                break;
        }
        bREG(0) = 0;
    }
    if (Flags_GetSwitch(play, WATER_TEMPLE_WATER_F1_FLAG) && (switchFlag != WATER_TEMPLE_WATER_F1_FLAG)) {
        ret = 3;
    } else if (Flags_GetSwitch(play, WATER_TEMPLE_WATER_F2_FLAG) && (switchFlag != WATER_TEMPLE_WATER_F2_FLAG)) {
        ret = 2;
    } else if (Flags_GetSwitch(play, WATER_TEMPLE_WATER_F3_FLAG) && (switchFlag != WATER_TEMPLE_WATER_F3_FLAG)) {
        ret = 1;
    } else {
        ret = 0;
    }

    return ret;
}

void BgMizuWater_SetWaterBoxesHeight(WaterBox* waterBoxes, s16 height) {
    u32 i;

    for (i = 0; i < 8; i++) {
        waterBoxes[sWaterBoxIndexes[i]].ySurface = height;
    }
}

void BgMizuWater_Init(Actor* thisx, PlayState* play) {
    BgMizuWater* this = (BgMizuWater*)thisx;
    f32 initialActorY;
    WaterBox* waterBoxes;
    s32 waterLevelActionIndex;

    waterBoxes = play->colCtx.colHeader->waterBoxes;
    this->type = this->actor.params & 0xFF;
    this->switchFlag = (this->actor.params >> 8) & 0xFF;
    Actor_ProcessInitChain(&this->actor, sInitChain);
    initialActorY = this->actor.world.pos.y;
    this->baseY = initialActorY;
    this->targetY = initialActorY;

    switch (this->type) {
        case 0:
            if (bREG(15) == 0) {
                osSyncPrintf("<コンストラクト>%x %x %x\n", Flags_GetSwitch(play, WATER_TEMPLE_WATER_F1_FLAG),
                             Flags_GetSwitch(play, WATER_TEMPLE_WATER_F2_FLAG),
                             Flags_GetSwitch(play, WATER_TEMPLE_WATER_F3_FLAG));
            }
            waterLevelActionIndex = BgMizuWater_GetWaterLevelActionIndex(-1, play);
            this->actor.world.pos.y = sWaterLevels[waterLevelActionIndex].yDiff + this->baseY;
            BgMizuWater_SetWaterBoxesHeight(waterBoxes, this->actor.world.pos.y);
            this->actor.params = sWaterLevels[waterLevelActionIndex].switchFlag;
            Flags_UnsetSwitch(play, WATER_TEMPLE_WATER_F1_FLAG);
            Flags_UnsetSwitch(play, WATER_TEMPLE_WATER_F2_FLAG);
            Flags_UnsetSwitch(play, WATER_TEMPLE_WATER_F3_FLAG);

            switch (this->actor.params) {
                case 0x1E:
                    Flags_SetSwitch(play, WATER_TEMPLE_WATER_F3_FLAG);
                    break;
                case 0x1D:
                    Flags_SetSwitch(play, WATER_TEMPLE_WATER_F2_FLAG);
                    break;
                case 0x1C:
                default:
                    Flags_SetSwitch(play, WATER_TEMPLE_WATER_F1_FLAG);
                    break;
            }
            this->targetY = this->actor.world.pos.y;
            break;
        case 1:
            break;
        case 2:
            if (Flags_GetSwitch(play, this->switchFlag)) {
                this->actor.world.pos.y = this->baseY + 85.0f;
            }
            waterBoxes[6].ySurface = this->actor.world.pos.y;
            break;
        case 3:
            if (Flags_GetSwitch(play, this->switchFlag)) {
                this->actor.world.pos.y = this->baseY + 110.0f;
                this->targetY = this->actor.world.pos.y;
            }
            waterBoxes[8].ySurface = this->actor.world.pos.y;
            break;
        case 4:
            if (Flags_GetSwitch(play, this->switchFlag)) {
                this->actor.world.pos.y = this->baseY + 160.0f;
                this->targetY = this->actor.world.pos.y;
            }
            waterBoxes[16].ySurface = this->actor.world.pos.y;
            break;
    }

    this->actionFunc = BgMizuWater_WaitForAction;
}

void BgMizuWater_Destroy(Actor* thisx, PlayState* play) {
}

void BgMizuWater_WaitForAction(BgMizuWater* this, PlayState* play) {
    s32 pad;
    s32 waterLevelActionIndex;
    s16 prevSwitchFlag;

    switch (this->type) {
        case 0:
            prevSwitchFlag = this->actor.params;
            waterLevelActionIndex = BgMizuWater_GetWaterLevelActionIndex(this->actor.params, play);
            if (waterLevelActionIndex != 0) {
                if (prevSwitchFlag != sWaterLevels[waterLevelActionIndex].switchFlag) {
                    OnePointCutscene_Init(play, 3120, -100 - waterLevelActionIndex, NULL, MAIN_CAM);
                    this->actor.params = sWaterLevels[waterLevelActionIndex].switchFlag;
                    this->targetY = sWaterLevels[waterLevelActionIndex].yDiff + this->baseY;
                }
            }
            if ((prevSwitchFlag != this->actor.params) && (prevSwitchFlag != 0)) {
                Flags_UnsetSwitch(play, prevSwitchFlag);
            }
            break;
        case 1:
            break;
        case 2:
            if (Flags_GetSwitch(play, this->switchFlag)) {
                this->targetY = this->baseY + 85.0f;
            } else {
                this->targetY = this->baseY;
            }
            break;
        case 3:
            if (Flags_GetSwitch(play, this->switchFlag)) {
                this->targetY = this->baseY + 110.0f;
            } else {
                this->targetY = this->baseY;
            }
            break;
        case 4:
            if (Flags_GetSwitch(play, this->switchFlag)) {
                this->targetY = this->baseY + 160.0f;
            } else {
                this->targetY = this->baseY;
            }
    }

    if (this->targetY != this->actor.world.pos.y) {
        this->actionFunc = BgMizuWater_ChangeWaterLevel;
    }
}

void BgMizuWater_ChangeWaterLevel(BgMizuWater* this, PlayState* play) {
    s32 pad;
    s16 prevSwitchFlag;
    s32 waterLevelActionIndex;
    WaterBox* waterBoxes;

    waterBoxes = play->colCtx.colHeader->waterBoxes;
    switch (this->type) {
        case 0:
            prevSwitchFlag = this->actor.params;
            waterLevelActionIndex = BgMizuWater_GetWaterLevelActionIndex(this->actor.params, play);
            if (waterLevelActionIndex != 0) {
                if (prevSwitchFlag != sWaterLevels[waterLevelActionIndex].switchFlag) {
                    this->actor.params = sWaterLevels[waterLevelActionIndex].switchFlag;
                    this->targetY = sWaterLevels[waterLevelActionIndex].yDiff + this->baseY;
                }
            }

            if ((prevSwitchFlag != this->actor.params) && (prevSwitchFlag != 0)) {
                Flags_UnsetSwitch(play, prevSwitchFlag);
            }

            if (Math_StepToF(&this->actor.world.pos.y, this->targetY, 5.0f)) {
                play->roomCtx.unk_74[0] = 0;
                this->actionFunc = BgMizuWater_WaitForAction;
                Message_CloseTextbox(play);
            }
            BgMizuWater_SetWaterBoxesHeight(play->colCtx.colHeader->waterBoxes, this->actor.world.pos.y);
            break;
        case 1:
            break;
        case 2:
            if (Flags_GetSwitch(play, this->switchFlag)) {
                this->targetY = this->baseY + 85.0f;
            } else {
                this->targetY = this->baseY;
            }
            if (Math_StepToF(&this->actor.world.pos.y, this->targetY, 1.0f)) {
                play->roomCtx.unk_74[0] = 0;
                this->actionFunc = BgMizuWater_WaitForAction;
            }
            waterBoxes[6].ySurface = this->actor.world.pos.y;
            break;
        case 3:
            if (Flags_GetSwitch(play, this->switchFlag)) {
                this->targetY = this->baseY + 110.0f;
            } else {
                this->targetY = this->baseY;
            }
            if (Math_StepToF(&this->actor.world.pos.y, this->targetY, 1.0f)) {
                play->roomCtx.unk_74[0] = 0;
                this->actionFunc = BgMizuWater_WaitForAction;
            }
            waterBoxes[8].ySurface = this->actor.world.pos.y;
            break;
        case 4:
            if (Flags_GetSwitch(play, this->switchFlag)) {
                this->targetY = this->baseY + 160.0f;
            } else {
                this->targetY = this->baseY;
            }
            if (Math_StepToF(&this->actor.world.pos.y, this->targetY, 1.0f)) {
                play->roomCtx.unk_74[0] = 0;
                this->actionFunc = BgMizuWater_WaitForAction;
            }
            waterBoxes[16].ySurface = this->actor.world.pos.y;
            break;
    }

    if (this->targetY < this->actor.world.pos.y) {
        func_800AA000(0.0f, 0x78, 0x14, 0xA);
        func_8002F948(&this->actor, NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
    } else if (this->targetY > this->actor.world.pos.y) {
        func_800AA000(0.0f, 0x78, 0x14, 0xA);
        func_8002F948(&this->actor, NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
    }
}

void BgMizuWater_Update(Actor* thisx, PlayState* play) {
    BgMizuWater* this = (BgMizuWater*)thisx;
    s32 posY;
    s32 unk0;
    s32 unk1;
    s32 pad;

    if (bREG(15) == 0) {
        osSyncPrintf("%x %x %x\n", Flags_GetSwitch(play, WATER_TEMPLE_WATER_F1_FLAG),
                     Flags_GetSwitch(play, WATER_TEMPLE_WATER_F2_FLAG),
                     Flags_GetSwitch(play, WATER_TEMPLE_WATER_F3_FLAG));
    }
    if (this->type == 0) {
        posY = this->actor.world.pos.y;
        unk0 = 0;
        unk1 = 0;
        if (posY < WATER_TEMPLE_WATER_F1_Y) {
            unk0 = 0;
            unk1 = (posY - WATER_TEMPLE_WATER_B1_Y) / (WATER_TEMPLE_WATER_F1_Y - WATER_TEMPLE_WATER_B1_Y) * 200;
        } else if (posY < WATER_TEMPLE_WATER_F2_Y) {
            unk0 = 1;
            unk1 = 255 - (s32)((posY - WATER_TEMPLE_WATER_F1_Y) / (WATER_TEMPLE_WATER_F2_Y - WATER_TEMPLE_WATER_F1_Y) *
                               (255 - 160));
        } else if (posY <= WATER_TEMPLE_WATER_F3_Y) {
            unk0 = 2;
            unk1 = 255 - (s32)((posY - WATER_TEMPLE_WATER_F2_Y) / (WATER_TEMPLE_WATER_F3_Y - WATER_TEMPLE_WATER_F2_Y) *
                               (255 - 160));
        }
        play->roomCtx.unk_74[1] = ((u8)unk0 << 8) | (unk1 & 0xFF);
    }

    this->actionFunc(this, play);
}

void BgMizuWater_Draw(Actor* thisx, PlayState* play) {
    BgMizuWater* this = (BgMizuWater*)thisx;
    s32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx);
    gameplayFrames = play->gameplayFrames;
    func_80093D84(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x0C,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, -gameplayFrames * 1, gameplayFrames * 1, 32, 32, 1, 0,
                                -gameplayFrames * 1, 32, 32));

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 128);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 102);

    gSPDisplayList(POLY_XLU_DISP++, gObjectMizuObjectsWaterDL_004B20);

    CLOSE_DISPS(play->state.gfxCtx);
}
