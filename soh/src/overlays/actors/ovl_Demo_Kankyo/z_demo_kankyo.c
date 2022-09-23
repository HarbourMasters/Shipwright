#include "z_demo_kankyo.h"
#include "z64cutscene_commands.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_efc_star_field/object_efc_star_field.h"
#include "objects/object_toki_objects/object_toki_objects.h"
#include "soh/frame_interpolation.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void DemoKankyo_Init(Actor* thisx, GlobalContext* globalCtx);
void DemoKankyo_Destroy(Actor* thisx, GlobalContext* globalCtx);
void DemoKankyo_Update(Actor* thisx, GlobalContext* globalCtx);
void DemoKankyo_Draw(Actor* thisx, GlobalContext* globalCtx);

void DemoKankyo_SetupType(DemoKankyo* this, GlobalContext* globalCtx);
void DemoKankyo_UpdateClouds(DemoKankyo* this, GlobalContext* globalCtx);
void DemoKankyo_UpdateRock(DemoKankyo* this, GlobalContext* globalCtx);
void DemoKankyo_DoNothing2(DemoKankyo* this, GlobalContext* globalCtx);
void DemoKankyo_UpdateDoorOfTime(DemoKankyo* this, GlobalContext* globalCtx);
void DemoKankyo_DoNothing(DemoKankyo* this, GlobalContext* globalCtx);
void DemoKankyo_KillDoorOfTimeCollision(DemoKankyo* this, GlobalContext* globalCtx);

void DemoKankyo_DrawRain(Actor* thisx, GlobalContext* globalCtx);
void DemoKankyo_DrawRock(Actor* thisx, GlobalContext* globalCtx);
void DemoKankyo_DrawClouds(Actor* thisx, GlobalContext* globalCtx);
void DemoKankyo_DrawDoorOfTime(Actor* thisx, GlobalContext* globalCtx);
void DemoKankyo_DrawLightPlane(Actor* thisx, GlobalContext* globalCtx);
void DemoKankyo_DrawWarpSparkles(Actor* thisx, GlobalContext* globalCtx);
void DemoKankyo_DrawSparkles(Actor* thisx, GlobalContext* globalCtx);

// adult warp songs cutscenes
extern CutsceneData gAdultWarpInCS[];
extern CutsceneData gAdultWarpOutCS[];
// adult warp songs cutscenes in temple of time
extern CutsceneData gAdultWarpInToTCS[];
extern CutsceneData gAdultWarpOutToTCS[];
// child warp songs cutscenes
extern CutsceneData gChildWarpInCS[];
extern CutsceneData gChildWarpOutCS[];
// child warp songs cutscenes in temple of time
extern CutsceneData gChildWarpInToTCS[];
extern CutsceneData gChildWarpOutToTCS[];

const ActorInit Demo_Kankyo_InitVars = {
    ACTOR_DEMO_KANKYO,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(DemoKankyo),
    (ActorFunc)DemoKankyo_Init,
    (ActorFunc)DemoKankyo_Destroy,
    (ActorFunc)DemoKankyo_Update,
    (ActorFunc)DemoKankyo_Draw,
    NULL,
};

static s16 sObjIds[] = {
    OBJECT_EFC_STAR_FIELD, OBJECT_EFC_STAR_FIELD, OBJECT_EFC_STAR_FIELD, OBJECT_EFC_STAR_FIELD, OBJECT_EFC_STAR_FIELD,
    OBJECT_EFC_STAR_FIELD, OBJECT_EFC_STAR_FIELD, OBJECT_GAMEPLAY_KEEP,  OBJECT_GI_MELODY,      OBJECT_GI_MELODY,
    OBJECT_GI_MELODY,      OBJECT_GI_MELODY,      OBJECT_GI_MELODY,      OBJECT_TOKI_OBJECTS,   OBJECT_TOKI_OBJECTS,
    OBJECT_GAMEPLAY_KEEP,  OBJECT_GAMEPLAY_KEEP,  OBJECT_GAMEPLAY_KEEP,
};

// unused, presumed to be floats
static f32 D_8098C314[] = {
    0.0f,
    150.0f,
};

static Color_RGB8 sWarpSparkleEnvColors[] = {
    { 0, 200, 0 },    // minuet
    { 255, 50, 0 },   // bolero
    { 0, 150, 255 },  // serenade
    { 255, 150, 0 },  // requiem
    { 200, 50, 255 }, // nocturne
    { 200, 255, 0 },  // prelude
};

static CutsceneCameraPoint sWarpOutCameraPoints[] = {
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0x0000, 0x0000, 0xFFE5 } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0x0000, 0x0000, 0xFFE5 } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0xFFE6, 0x0000, 0x0000 } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0x0000, 0x0017, 0x0024 } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0x001C, 0x0032, 0xFFFF } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0x0001, 0x0018, 0xFFD9 } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0xFFE6, 0xFFFA, 0x0003 } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0x0000, 0x0025, 0x0037 } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0x004F, 0x0066, 0x0029 } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0x00A6, 0x00AD, 0x0006 } },
    { CS_CMD_CONTINUE, 0, 5, 45.0f, { 0x010D, 0x015A, 0xFF4C } },
    { CS_CMD_CONTINUE, 0, 5, 45.0f, { 0x019F, 0x0245, 0xFE35 } },
    { CS_CMD_STOP, 0, 5, 45.0f, { 0x01CE, 0x036F, 0xFCC2 } },
    { CS_CMD_STOP, 0, 5, 45.0f, { 0x01CE, 0x036F, 0xFCC2 } },
};

static CutsceneCameraPoint sWarpInCameraPoints[] = {
    { CS_CMD_CONTINUE, 0, 5, 45.0f, { 0x019F, 0x0245, 0xFE35 } },
    { CS_CMD_CONTINUE, 0, 5, 45.0f, { 0x010D, 0x015A, 0xFF4C } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0x00A6, 0x00AD, 0x0006 } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0x004F, 0x0066, 0x0029 } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0x0000, 0x0025, 0x0037 } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0xFFE6, 0xFFFA, 0x0003 } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0x0001, 0x0018, 0xFFD9 } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0x001C, 0x0032, 0xFFFF } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0x0000, 0x0017, 0x0024 } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0xFFE6, 0x0000, 0x0000 } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0x0000, 0x0000, 0xFFE5 } },
    { CS_CMD_CONTINUE, 0, 8, 45.0f, { 0x0000, 0x0000, 0xFFE5 } },
    { CS_CMD_STOP, 0, 5, 45.0f, { 0x01CE, 0x036F, 0xFCC2 } },
    { CS_CMD_STOP, 0, 5, 45.0f, { 0x01CE, 0x036F, 0xFCC2 } },
};

static Color_RGB8 sSparkleEnvColors[] = {
    { 0, 200, 0 },   { 255, 50, 0 },  { 0, 150, 255 }, { 255, 150, 0 }, // only this one is used
    { 0, 255, 255 }, { 200, 255, 0 },
};

static CutsceneCameraPoint sSparklesCameraPoints[] = {
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFF7, 0x0000, 0xFFD0 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFF7, 0x0000, 0xFFD0 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFF7, 0x0000, 0xFFD0 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFF7, 0x0000, 0xFFD0 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFD7, 0x0000, 0xFFE9 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFD3, 0x0000, 0x000A } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFE8, 0x0001, 0x0027 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x0015, 0x0000, 0x002B } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x002F, 0x0005, 0x000E } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x0031, 0x0005, 0xFFF5 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x0020, 0x0005, 0xFFDA } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFF5, 0x0005, 0xFFD1 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFD7, 0x0006, 0xFFEA } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFD5, 0x0009, 0x000D } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFE9, 0x0009, 0x0027 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x0014, 0x000B, 0x0029 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x002D, 0x000B, 0x000F } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x002E, 0x000B, 0xFFF0 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x001E, 0x000B, 0xFFDA } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFFA, 0x000E, 0xFFD3 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFDA, 0x000E, 0xFFEB } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFD7, 0x0010, 0x0008 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFE9, 0x0010, 0x0024 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x0011, 0x0010, 0x0028 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x002C, 0x0010, 0x000D } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x002C, 0x0012, 0xFFF5 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x001F, 0x0011, 0xFFDE } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFFB, 0x0014, 0xFFD5 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFDD, 0x0014, 0xFFEC } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFDA, 0x0017, 0x0008 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFE8, 0x0014, 0x001F } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x000C, 0x0018, 0x0026 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x0027, 0x0018, 0x000D } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x0027, 0x001B, 0xFFF6 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x001C, 0x001A, 0xFFE2 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFFA, 0x000E, 0xFFD4 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFD9, 0x001B, 0xFFEF } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFD7, 0x001B, 0x000A } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFE6, 0x001B, 0x0022 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x000F, 0x001F, 0x002C } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x0032, 0x0020, 0x0009 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x0030, 0x0021, 0xFFF0 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x001C, 0x0025, 0xFFD9 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFFA, 0x0028, 0xFFD4 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFD8, 0x002B, 0xFFF5 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFD7, 0x002B, 0x0006 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFDF, 0x002B, 0x0019 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x000E, 0x002E, 0x002C } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x0032, 0x002E, 0x0003 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0x002A, 0x0030, 0xFFE7 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFF6, 0x002B, 0xFFD4 } },
    { CS_CMD_CONTINUE, 0, 2, 45.0f, { 0xFFF6, 0x002B, 0xFFD4 } },
    { CS_CMD_STOP, 0, 2, 45.0f, { 0xFFF6, 0x002B, 0xFFD4 } },
    { CS_CMD_STOP, 0, 2, 45.0f, { 0xFFF6, 0x002B, 0xFFD4 } },
};

static s16 D_8098CF80;
static s16 sRainScale;
static s16 D_8098CF84;

void DemoKankyo_SetupAction(DemoKankyo* this, DemoKankyoActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void DemoKankyo_Init(Actor* thisx, GlobalContext* globalCtx) {
    DemoKankyo* this = (DemoKankyo*)thisx;
    s16 i;
    s32 objBankIndex = Object_GetIndex(&globalCtx->objectCtx, sObjIds[this->actor.params]);

    osSyncPrintf("bank_ID = %d\n", objBankIndex);
    if (objBankIndex < 0) {
        ASSERT(objBankIndex < 0);
    } else {
        this->objBankIndex = objBankIndex;
    }

    switch (this->actor.params) {
        case DEMOKANKYO_BLUE_RAIN:
        case DEMOKANKYO_BLUE_RAIN_2:
            switch (globalCtx->sceneNum) {
                case SCENE_HIRAL_DEMO:
                    globalCtx->roomCtx.curRoom.segment = NULL;
                    D_8098CF80 = 10;
                    sRainScale = 8;
                    break;
                case SCENE_TOKINOMA:
                    D_8098CF80 = 14;
                    sRainScale = 8;
                    break;
                case SCENE_SPOT00:
                    D_8098CF80 = 1;
                    sRainScale = 5;
                    break;
                default:
                    Actor_Kill(&this->actor);
                    break;
            }
            break;
        case DEMOKANKYO_ROCK_1:
        case DEMOKANKYO_ROCK_2:
        case DEMOKANKYO_ROCK_3:
        case DEMOKANKYO_ROCK_4:
        case DEMOKANKYO_ROCK_5:
            globalCtx->roomCtx.curRoom.segment = NULL;
            this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = Rand_ZeroOne() * 0.5f + 0.5f;
            this->unk_150[0].unk_0.x = Rand_ZeroOne() * 3.0f + 1.0f;
            this->unk_150[0].unk_0.y = Rand_ZeroOne() * 3.0f + 1.0f;
            this->unk_150[0].unk_0.z = Rand_ZeroOne() * 3.0f + 1.0f;
            break;
        case DEMOKANKYO_CLOUDS:
            for (i = 0; i < 30; i++) {
                this->unk_150[i].unk_20 = Rand_ZeroOne() * 65535.0f;
                this->unk_150[i].unk_18 = Rand_ZeroOne() * 100.0f + 60.0f;
            }
            break;
        case DEMOKANKYO_DOOR_OF_TIME:
            this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = 1.0f;
            this->unk_150[0].unk_18 = 0.0f;
            if (!(gSaveContext.eventChkInf[4] & 0x800)) {
                Actor_SpawnAsChild(&globalCtx->actorCtx, &this->actor, globalCtx, ACTOR_DOOR_TOKI,
                                   this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0,
                                   0x0000);
            } else {
                globalCtx->roomCtx.unk_74[1] = 0xFF;
                Actor_Kill(&this->actor);
            }
            break;
        case DEMOKANKYO_LIGHT_PLANE:
            this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = 1.0f;
            this->unk_150[0].unk_18 = 0.0f;
            break;
        case DEMOKANKYO_WARP_OUT:
        case DEMOKANKYO_WARP_IN:
            Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &this->actor, ACTORCAT_ITEMACTION);
            this->actor.flags |= ACTOR_FLAG_25;
            this->actor.room = -1;
            this->warpTimer = 35;
            this->sparkleCounter = 0;
            this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = 1.0f;
            if (this->actor.params == DEMOKANKYO_WARP_OUT) {
                Audio_PlaySoundGeneral(NA_SE_EV_SARIA_MELODY, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
            break;
        case DEMOKANKYO_SPARKLES:
            this->warpTimer = 35;
            this->sparkleCounter = 0;
            this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = 1.0f;
            break;
        default:
            break;
    }
    for (i = 0; i < 30; i++) {
        this->unk_150[i].unk_22 = 0;
    }
    DemoKankyo_SetupAction(this, DemoKankyo_SetupType);
}

void DemoKankyo_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    if (thisx) {}
}

void DemoKankyo_SetupType(DemoKankyo* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 temp;

    if (this->actor.objBankIndex == this->objBankIndex) {
        switch (this->actor.params) {
            case DEMOKANKYO_ROCK_1:
            case DEMOKANKYO_ROCK_2:
            case DEMOKANKYO_ROCK_3:
            case DEMOKANKYO_ROCK_4:
            case DEMOKANKYO_ROCK_5:
                DemoKankyo_SetupAction(this, DemoKankyo_UpdateRock);
                break;
            case DEMOKANKYO_CLOUDS:
                DemoKankyo_SetupAction(this, DemoKankyo_UpdateClouds);
                break;
            case DEMOKANKYO_DOOR_OF_TIME:
                if (Flags_GetEnv(globalCtx, 2)) {
                    DemoKankyo_SetupAction(this, DemoKankyo_UpdateDoorOfTime);
                }
                break;
            case DEMOKANKYO_WARP_OUT:
                globalCtx->envCtx.screenFillColor[0] = 0xFF;
                globalCtx->envCtx.screenFillColor[1] = 0xFF;
                globalCtx->envCtx.screenFillColor[2] = 0xFF;
                globalCtx->envCtx.fillScreen = false;
                if (this->warpTimer < 21 && this->warpTimer >= 15) {
                    temp = (this->warpTimer - 15.0f) / 5.0f;
                    globalCtx->envCtx.fillScreen = true;
                    globalCtx->envCtx.screenFillColor[3] = 255 - 255 * temp;
                }
                if (this->warpTimer < 15 && this->warpTimer >= 4) {
                    temp = (this->warpTimer - 4.0f) / 10.0f;
                    globalCtx->envCtx.fillScreen = true;
                    globalCtx->envCtx.screenFillColor[3] = 255 * temp;
                }
                if (this->warpTimer == 15) {
                    player->actor.draw = NULL;
                }
                if ((u32)this->warpTimer != 0) {
                    this->warpTimer--;
                }
                if (this->warpTimer == 1) {
                    if (globalCtx->sceneNum == SCENE_TOKINOMA) {
                        D_8098CF84 = 25;
                        if (!LINK_IS_ADULT) {
                            globalCtx->csCtx.segment = gChildWarpInToTCS;
                        } else {
                            globalCtx->csCtx.segment = gAdultWarpInToTCS;
                        }
                    } else {
                        D_8098CF84 = 32;
                        if (!LINK_IS_ADULT) {
                            globalCtx->csCtx.segment = gChildWarpInCS;
                        } else {
                            globalCtx->csCtx.segment = gAdultWarpInCS;
                        }
                    }
                    if (func_800C0CB8(globalCtx) != 0) {
                        gSaveContext.cutsceneTrigger = 1;
                    }
                    DemoKankyo_SetupAction(this, DemoKankyo_DoNothing);
                }
                break;
            case DEMOKANKYO_WARP_IN:
                if (globalCtx->sceneNum == SCENE_TOKINOMA) {
                    if (!LINK_IS_ADULT) {
                        globalCtx->csCtx.segment = gChildWarpOutToTCS;
                    } else {
                        globalCtx->csCtx.segment = gAdultWarpOutToTCS;
                    }
                } else {
                    if (!LINK_IS_ADULT) {
                        globalCtx->csCtx.segment = gChildWarpOutCS;
                    } else {
                        globalCtx->csCtx.segment = gAdultWarpOutCS;
                    }
                }
                gSaveContext.cutsceneTrigger = 1;
                DemoKankyo_SetupAction(this, DemoKankyo_DoNothing2);
                break;
            case DEMOKANKYO_BLUE_RAIN:
            case DEMOKANKYO_SPARKLES:
                break;
        }
    }
}

void DemoKankyo_DoNothing(DemoKankyo* this, GlobalContext* globalCtx) {
}

void DemoKankyo_DoNothing2(DemoKankyo* this, GlobalContext* globalCtx) {
    DemoKankyo_SetupAction(this, DemoKankyo_DoNothing);
}

void DemoKankyo_SetRockPos(DemoKankyo* this, GlobalContext* globalCtx, s32 params) {
    Vec3f startPos;
    Vec3f endPos;
    CsCmdActorAction* csAction = globalCtx->csCtx.npcActions[params];
    f32 temp_f0;

    startPos.x = csAction->startPos.x;
    startPos.y = csAction->startPos.y;
    startPos.z = csAction->startPos.z;
    endPos.x = csAction->endPos.x;
    endPos.y = csAction->endPos.y;
    endPos.z = csAction->endPos.z;
    temp_f0 = Environment_LerpWeight(csAction->endFrame, csAction->startFrame, globalCtx->csCtx.frames);
    this->actor.world.pos.x = ((endPos.x - startPos.x) * temp_f0) + startPos.x;
    this->actor.world.pos.y = ((endPos.y - startPos.y) * temp_f0) + startPos.y;
    this->actor.world.pos.z = ((endPos.z - startPos.z) * temp_f0) + startPos.z;
}

void DemoKankyo_UpdateRock(DemoKankyo* this, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state != CS_STATE_IDLE && globalCtx->csCtx.npcActions[this->actor.params - 2] != NULL) {
        DemoKankyo_SetRockPos(this, globalCtx, this->actor.params - 2);
    }
    this->unk_150[0].unk_C.x += this->unk_150[0].unk_0.x;
    this->unk_150[0].unk_C.y += this->unk_150[0].unk_0.y;
    this->unk_150[0].unk_C.z += this->unk_150[0].unk_0.z;
}

void DemoKankyo_UpdateClouds(DemoKankyo* this, GlobalContext* globalCtx) {
    u8 i;

    for (i = 0; i < 30; i++) {
        this->unk_150[i].unk_20 += (s16)this->unk_150[i].unk_18;
    }
}

void DemoKankyo_UpdateDoorOfTime(DemoKankyo* this, GlobalContext* globalCtx) {
    Audio_PlayActorSound2(&this->actor, NA_SE_EV_STONE_STATUE_OPEN - SFX_FLAG);
    this->unk_150[0].unk_18 += 1.0f;
    if (this->unk_150[0].unk_18 >= 102.0f) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EV_STONEDOOR_STOP);
        gSaveContext.eventChkInf[4] |= 0x800;
        Actor_Kill(this->actor.child);
        DemoKankyo_SetupAction(this, DemoKankyo_KillDoorOfTimeCollision);
    }
}

void DemoKankyo_KillDoorOfTimeCollision(DemoKankyo* this, GlobalContext* globalCtx) {
    Actor_Kill(this->actor.child);
}

void DemoKankyo_Update(Actor* thisx, GlobalContext* globalCtx) {
    DemoKankyo* this = (DemoKankyo*)thisx;
    this->actionFunc(this, globalCtx);
}

void DemoKankyo_Draw(Actor* thisx, GlobalContext* globalCtx) {
    DemoKankyo* this = (DemoKankyo*)thisx;

    if (this->actor.objBankIndex == this->objBankIndex) {
        switch (this->actor.params) {
            case DEMOKANKYO_BLUE_RAIN:
            case DEMOKANKYO_BLUE_RAIN_2:
                if (globalCtx->sceneNum == SCENE_TOKINOMA) {
                    if (!Flags_GetEnv(globalCtx, 1)) {
                        break;
                    } else if (!Actor_IsFacingAndNearPlayer(&this->actor, 300.0f, 0x7530)) {
                        break;
                    } else {
                        if (!LINK_IS_ADULT) {
                            if (globalCtx->csCtx.frames < 170 || globalCtx->csCtx.state == CS_STATE_IDLE) {
                                break;
                            }
                        } else {
                            if (globalCtx->csCtx.frames < 120 || globalCtx->csCtx.state == CS_STATE_IDLE) {
                                break;
                            }
                        }
                    }
                }
                DemoKankyo_DrawRain(thisx, globalCtx);
                break;
            case DEMOKANKYO_ROCK_1:
            case DEMOKANKYO_ROCK_2:
            case DEMOKANKYO_ROCK_3:
            case DEMOKANKYO_ROCK_4:
            case DEMOKANKYO_ROCK_5:
                DemoKankyo_DrawRock(thisx, globalCtx);
                break;
            case DEMOKANKYO_CLOUDS:
                DemoKankyo_DrawClouds(thisx, globalCtx);
                break;
            case DEMOKANKYO_DOOR_OF_TIME:
                DemoKankyo_DrawDoorOfTime(thisx, globalCtx);
                break;
            case DEMOKANKYO_LIGHT_PLANE:
                DemoKankyo_DrawLightPlane(thisx, globalCtx);
                break;
            case DEMOKANKYO_WARP_OUT:
            case DEMOKANKYO_WARP_IN:
                DemoKankyo_DrawWarpSparkles(thisx, globalCtx);
                break;
            case DEMOKANKYO_SPARKLES:
                DemoKankyo_DrawSparkles(thisx, globalCtx);
                break;
        }
    }
    if (Object_IsLoaded(&globalCtx->objectCtx, this->objBankIndex)) {
        this->actor.objBankIndex = this->objBankIndex;
    }
}

// transform relating to blue rain
void func_80989B54(Actor* thisx, GlobalContext* globalCtx, s16 i) {
    DemoKankyo* this = (DemoKankyo*)thisx;

    switch (globalCtx->sceneNum) {
        case SCENE_HIRAL_DEMO:
            this->unk_150[i].unk_0.x = (Rand_ZeroOne() - 0.5f) * 500.0f;
            this->unk_150[i].unk_0.y = 500.0f;
            this->unk_150[i].unk_0.z = (Rand_ZeroOne() - 0.5f) * 500.0f;
            break;
        case SCENE_TOKINOMA:
            this->unk_150[i].unk_C.x = 0.0f;
            this->unk_150[i].unk_C.y = 0.0f;
            this->unk_150[i].unk_C.z = 0.0f;
            this->unk_150[i].unk_0.x = (Rand_ZeroOne() - 0.5f) * 180.0f;
            this->unk_150[i].unk_0.y = 10.0f;
            this->unk_150[i].unk_0.z = (Rand_ZeroOne() - 0.5f) * 180.0f;
            break;
        case SCENE_SPOT00:
            this->unk_150[i].unk_0.x = (Rand_ZeroOne() - 0.5f) * 600.0f;
            this->unk_150[i].unk_0.y = -500.0f;
            this->unk_150[i].unk_0.z = (Rand_ZeroOne() - 0.5f) * 600.0f;
            break;
    }
    this->unk_150[i].unk_18 = Rand_ZeroOne() * (D_8098CF80 * 4.0f) + D_8098CF80;
}

void DemoKankyo_DrawRain(Actor* thisx, GlobalContext* globalCtx) {
    DemoKankyo* this = (DemoKankyo*)thisx;
    f32 temp_f12_2;
    s16 i;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 norm;
    f32 translateX;
    f32 translateY;
    f32 translateZ;
    s16 j;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    for (i = 0; i < 30; i++) {
        s32 pad[2];

        dx = globalCtx->view.lookAt.x - globalCtx->view.eye.x;
        dy = globalCtx->view.lookAt.y - globalCtx->view.eye.y;
        dz = globalCtx->view.lookAt.z - globalCtx->view.eye.z;
        norm = sqrtf(SQ(dx) + SQ(dy) + SQ(dz));

        if (globalCtx->sceneNum != SCENE_TOKINOMA) {
            this->unk_150[i].unk_C.x = globalCtx->view.eye.x + (dx / norm) * 350.0f;
            this->unk_150[i].unk_C.y = globalCtx->view.eye.y + (dy / norm) * 80.0f;
            this->unk_150[i].unk_C.z = globalCtx->view.eye.z + (dz / norm) * 350.0f;
        }

        switch (this->unk_150[i].unk_22) {
            case 0:
                func_80989B54(thisx, globalCtx, i);
                if (gSaveContext.entranceIndex == 0x00A0) { // Cutscene Map
                    this->unk_150[i].unk_0.y = Rand_ZeroOne() * 500.0f;
                } else {
                    this->unk_150[i].unk_0.y = Rand_ZeroOne() * -500.0f;
                }
                this->unk_150[i].unk_22++;
                break;
            case 1:
                temp_f12_2 = globalCtx->view.eye.y + (dy / norm) * 150.0f;
                if (gSaveContext.entranceIndex == 0x00A0) { // Cutscene Map
                    this->unk_150[i].unk_0.y -= this->unk_150[i].unk_18;
                } else {
                    this->unk_150[i].unk_0.y += this->unk_150[i].unk_18;
                }
                if (gSaveContext.entranceIndex == 0x00A0) { // Cutscene Map
                    if (this->unk_150[i].unk_C.y + this->unk_150[i].unk_0.y < temp_f12_2 - 300.0f) {
                        this->unk_150[i].unk_22++;
                    }
                } else if (gSaveContext.entranceIndex == 0x00CD) { // Hyrule Field
                    if (temp_f12_2 + 300.0f < this->unk_150[i].unk_C.y + this->unk_150[i].unk_0.y) {
                        this->unk_150[i].unk_22++;
                    }
                } else {
                    if (1000.0f < this->unk_150[i].unk_C.y + this->unk_150[i].unk_0.y) {
                        this->unk_150[i].unk_22++;
                    }
                }
                break;
            case 2:
                func_80989B54(thisx, globalCtx, i);
                this->unk_150[i].unk_22--;
                break;
        }

        Matrix_Translate(this->unk_150[i].unk_C.x + this->unk_150[i].unk_0.x,
                         this->unk_150[i].unk_C.y + this->unk_150[i].unk_0.y,
                         this->unk_150[i].unk_C.z + this->unk_150[i].unk_0.z, MTXMODE_NEW);
        if (gSaveContext.entranceIndex != 0x00A0) { // Cutscene Map
            Matrix_RotateX(M_PI, MTXMODE_APPLY);
        }

        gDPPipeSync(POLY_XLU_DISP++);
        if (gSaveContext.entranceIndex == 0x00CD) { // Hyrule Field
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 255);
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 150, 255, 255);
        }

        Matrix_Scale(sRainScale * 0.001f, sRainScale * 0.001f, sRainScale * 0.001f, MTXMODE_APPLY);

        for (j = 0; j < 5; j++) {
            FrameInterpolation_RecordOpenChild("Kankyo Rain", i * j);

            s32 pad1;

            if (globalCtx->sceneNum != SCENE_TOKINOMA) {
                if (this->unk_150[i].unk_0.x >= 0.0f) {
                    translateX = -j * 1500.0f;
                } else {
                    translateX = j * 1500.0f;
                }
                if (this->unk_150[i].unk_0.z >= 0.0f) {
                    translateZ = -j * 1500.0f;
                } else {
                    translateZ = j * 1500.0f;
                }
                if (j % 2 != 0) {
                    translateY = j * 4000.0f;
                } else {
                    translateY = -j * 4000.0f;
                }
            } else {
                translateX = 0.0f;
                translateY = j * 10.0f;
                translateZ = 0.0f;
            }

            Matrix_Translate(translateX, translateY, translateZ, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0x14);
            gSPDisplayList(POLY_XLU_DISP++, object_efc_star_field_DL_000080);

            FrameInterpolation_RecordCloseChild();
        }
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void DemoKankyo_DrawRock(Actor* thisx, GlobalContext* globalCtx) {
    DemoKankyo* this = (DemoKankyo*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D18(globalCtx->state.gfxCtx);
    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateX(DEG_TO_RAD(this->unk_150[0].unk_C.x), MTXMODE_APPLY);
    Matrix_RotateY(DEG_TO_RAD(this->unk_150[0].unk_C.y), MTXMODE_APPLY);
    Matrix_RotateZ(DEG_TO_RAD(this->unk_150[0].unk_C.z), MTXMODE_APPLY);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 155, 55, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 155, 255, 55, 255);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, object_efc_star_field_DL_000DE0);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void DemoKankyo_DrawClouds(Actor* thisx, GlobalContext* globalCtx) {
    DemoKankyo* this = (DemoKankyo*)thisx;
    s16 i;
    s32 pad;
    f32 dx;
    f32 dy;
    f32 dz;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    for (i = 0; i < 30; i++) {
        FrameInterpolation_RecordOpenChild("Kankyo Clouds", i);

        dx = -(Math_SinS(this->unk_150[i].unk_20 - 0x8000) * 120.0f) * (30.0f + (i / 30.0f) * 10.0f);
        dy = Math_CosS(this->unk_150[i].unk_20 - 0x8000) * 5.0f + 1200.0f;
        dz = (Math_CosS(this->unk_150[i].unk_20 - 0x8000) * 120.0f) * (30.0f + (i / 30.0f) * 10.0f);

        Matrix_Translate(globalCtx->view.eye.x + dx, globalCtx->view.eye.y + dy + ((i - 12.0f) * 300.0f),
                         globalCtx->view.eye.z + dz, MTXMODE_NEW);
        Matrix_Scale(125.0f, 60.0f, 125.0f, MTXMODE_APPLY);

        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 210, 210, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 255);
        gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);
        gDPSetColorDither(POLY_XLU_DISP++, G_AD_NOTPATTERN | G_CD_MAGICSQ);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gDust5Tex));

        func_80094C50(globalCtx->state.gfxCtx);

        gSPMatrix(POLY_XLU_DISP++, SEG_ADDR(1, 0), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gEffDustDL);

        FrameInterpolation_RecordCloseChild();
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void DemoKankyo_DrawDoorOfTime(Actor* thisx, GlobalContext* globalCtx) {
    DemoKankyo* this = (DemoKankyo*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D18(globalCtx->state.gfxCtx);
    Matrix_Translate(-this->unk_150[0].unk_18, 0.0f, 0.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, object_toki_objects_DL_007440);
    Matrix_Translate(this->unk_150[0].unk_18 + this->unk_150[0].unk_18, 0.0f, 0.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, object_toki_objects_DL_007578);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void DemoKankyo_DrawLightPlane(Actor* thisx, GlobalContext* globalCtx) {
    DemoKankyo* this = (DemoKankyo*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    if (globalCtx->csCtx.state == CS_STATE_IDLE || gSaveContext.sceneSetupIndex >= 4) {
        func_80093D84(globalCtx->state.gfxCtx);

        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TexScroll(globalCtx->state.gfxCtx, 0, globalCtx->state.frames & 0x7F, 64, 32));
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, object_toki_objects_DL_008390);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void DemoKankyo_Vec3fCopy(Vec3f* src, Vec3f* dst) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}

Vec3f* DemoKankyo_Vec3fAddVecSph(Vec3f* dst, Vec3f* vec, VecSph* sph) {
    Vec3f result;
    Vec3f sphVec;

    OLib_VecSphGeoToVec3f(&sphVec, sph);
    result.x = vec->x + sphVec.x;
    result.y = vec->y + sphVec.y;
    result.z = vec->z + sphVec.z;
    *dst = result;
    return dst;
}

void DemoKankyo_Vec3fAddPosRot(PosRot* posRot, Vec3f* vec, Vec3f* dst) {
    VecSph sph;
    Vec3f vecCopy;

    DemoKankyo_Vec3fCopy(vec, &vecCopy);
    OLib_Vec3fToVecSphGeo(&sph, &vecCopy);
    sph.yaw += posRot->rot.y;
    DemoKankyo_Vec3fAddVecSph(dst, &posRot->pos, &sph);
}

void DemoKankyo_DrawWarpSparkles(Actor* thisx, GlobalContext* globalCtx) {
    static f32 sWarpRoll;
    static f32 sWarpFoV;
    // the following 2 vars are unused
    static u32 D_8098CF90;
    static u32 D_8098CF94;
    static Vec3f D_8098CF98;

    s16 i;
    f32 temp_f22;
    DemoKankyo* this = (DemoKankyo*)thisx;
    Gfx* disp;
    Player* player = GET_PLAYER(globalCtx);
    Vec3f camPos;
    f32 translateX;
    f32 translateY;
    f32 translateZ;
    PosRot posRot;
    u8 linkAge = gSaveContext.linkAge;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    if (this->sparkleCounter < 30) {
        this->sparkleCounter += 2;
    }
    for (i = this->sparkleCounter - 1; i >= 0; i--) {
        FrameInterpolation_RecordOpenChild("Kankyo Warp Sparkles", i);

        temp_f22 = 1.0f - (i / (f32)this->sparkleCounter);

        switch (this->unk_150[i].unk_22) {
            case 0:
                this->unk_150[i].unk_20 = 0;
                this->unk_150[i].unk_1C = 0;
                this->unk_150[i].unk_0.x = (s16)((Rand_ZeroOne() - 0.5f) * 16.0f * temp_f22);
                this->unk_150[i].unk_0.y = (s16)((Rand_ZeroOne() - 0.5f) * 16.0f * temp_f22);
                this->unk_150[i].unk_0.z = (s16)((Rand_ZeroOne() - 0.5f) * 16.0f * temp_f22);
                this->unk_150[i].unk_23 = 0;

                // Skip the first part of warp song cutscenes in rando
                if (gSaveContext.n64ddFlag && this->actor.params == DEMOKANKYO_WARP_OUT) {
                    this->unk_150[i].unk_22 = 2;
                } else {
                    this->unk_150[i].unk_22++;
                }

            case 1:
                if (this->actor.params == DEMOKANKYO_WARP_OUT) {
                    if (func_800BB2B4(&camPos, &sWarpRoll, &sWarpFoV, sWarpOutCameraPoints, &this->unk_150[i].unk_20,
                                      &this->unk_150[i].unk_1C) != 0) {
                        this->unk_150[i].unk_22++;
                    }
                    if (globalCtx->sceneNum == SCENE_TOKINOMA && globalCtx->csCtx.frames == 25) {
                        this->unk_150[i].unk_22++;
                    }
                } else {
                    Audio_PlaySoundGeneral(NA_SE_EV_LINK_WARP_OUT - SFX_FLAG, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    if (func_800BB2B4(&camPos, &sWarpRoll, &sWarpFoV, sWarpInCameraPoints, &this->unk_150[i].unk_20,
                                      &this->unk_150[i].unk_1C) != 0) {
                        this->unk_150[i].unk_22++;
                    }
                    if (D_8098CF84 < globalCtx->csCtx.frames && this->actor.params == DEMOKANKYO_WARP_OUT) {
                        this->unk_150[i].unk_22++;
                    }
                }
                Actor_GetWorld(&posRot, &player->actor);
                DemoKankyo_Vec3fAddPosRot(&posRot, &camPos, &D_8098CF98);
                break;
            case 2:
                if (this->actor.params == DEMOKANKYO_WARP_OUT) {
                    if (i == 0) {
                        Environment_WarpSongLeave(globalCtx);
                        this->unk_150[i].unk_22++;
                    }
                } else if (i + 1 == this->sparkleCounter && globalCtx->csCtx.state == CS_STATE_IDLE) {
                    func_80088AF0(globalCtx);
                    Actor_Kill(&this->actor);
                }
                break;
        }

        this->unk_150[i].unk_C.x = D_8098CF98.x;
        this->unk_150[i].unk_C.y = D_8098CF98.y;
        this->unk_150[i].unk_C.z = D_8098CF98.z;

        switch (this->unk_150[i].unk_23) {
            case 0:
                this->unk_150[i].unk_18 = Rand_ZeroOne();
                this->unk_150[i].unk_23++;
            case 1:
                Math_SmoothStepToF(&this->unk_150[i].unk_18, 1.0f, 0.5f, 0.4f, 0.2f);
                if (this->unk_150[i].unk_18 >= 1.0f) {
                    this->unk_150[i].unk_23 = 2;
                }
                break;
            case 2:
                Math_SmoothStepToF(&this->unk_150[i].unk_18, 0.0f, 0.5f, 0.3f, 0.2f);
                if (this->unk_150[i].unk_18 <= 0.0f) {
                    this->unk_150[i].unk_0.x = (s16)((Rand_ZeroOne() - 0.5f) * 16.0f * temp_f22);
                    this->unk_150[i].unk_0.y = (s16)((Rand_ZeroOne() - 0.5f) * 16.0f * temp_f22);
                    this->unk_150[i].unk_0.z = (s16)((Rand_ZeroOne() - 0.5f) * 16.0f * temp_f22);
                    this->unk_150[i].unk_18 = 0.0f;
                    this->unk_150[i].unk_23 = 1;
                }
                break;
        }

        translateX = this->unk_150[i].unk_C.x + this->unk_150[i].unk_0.x;
        translateY = this->unk_150[i].unk_C.y + this->unk_150[i].unk_0.y;
        translateZ = this->unk_150[i].unk_C.z + this->unk_150[i].unk_0.z;

        if (this->unk_150[i].unk_22 < 2) {
            disp = (uintptr_t)gEffFlash1DL; //This is probably fake
            if (linkAge != 0) {
                Matrix_Translate(translateX, translateY, translateZ, MTXMODE_NEW);
            } else {
                if (translateY) {}
                Matrix_Translate(translateX, translateY + 15.0f, translateZ, MTXMODE_NEW);
            }
            Matrix_Scale(this->unk_150[i].unk_18 * (0.018f * temp_f22), this->unk_150[i].unk_18 * (0.018f * temp_f22),
                         this->unk_150[i].unk_18 * (0.018f * temp_f22), MTXMODE_APPLY);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, 255);
            if (this->actor.params == DEMOKANKYO_WARP_OUT) {
                gDPSetEnvColor(POLY_XLU_DISP++, sWarpSparkleEnvColors[globalCtx->msgCtx.lastPlayedSong].r,
                               sWarpSparkleEnvColors[globalCtx->msgCtx.lastPlayedSong].g,
                               sWarpSparkleEnvColors[globalCtx->msgCtx.lastPlayedSong].b, 255);
            } else {
                s8 respawnData = gSaveContext.respawn[1].data;

                gDPSetEnvColor(POLY_XLU_DISP++, sWarpSparkleEnvColors[respawnData].r,
                               sWarpSparkleEnvColors[respawnData].g, sWarpSparkleEnvColors[respawnData].b, 255);
            }
            func_80093D84(globalCtx->state.gfxCtx);
            Matrix_Mult(&globalCtx->billboardMtxF, MTXMODE_APPLY);
            Matrix_RotateZ(DEG_TO_RAD(this->unk_150[i].unk_24), MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, disp);
            this->unk_150[i].unk_24 += 0x190;
        }

        FrameInterpolation_RecordCloseChild();
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void DemoKankyo_DrawSparkles(Actor* thisx, GlobalContext* globalCtx) {
    static f32 sSparklesRoll;
    static f32 sSparklesFoV;
    // the following 3 vars are unused
    static u32 D_8098CFAC;
    static u32 D_8098CFB0;
    static u32 D_8098CFB4;
    static Vec3f D_8098CFB8;

    DemoKankyo* this = (DemoKankyo*)thisx;
    f32 translateX;
    f32 translateY;
    f32 translateZ;
    Vec3f camPos;
    f32 temp_f20;
    f32 scale;
    s16 i;
    PosRot posRot;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    if (this->sparkleCounter < 20) {
        this->sparkleCounter++;
    }

    for (i = this->sparkleCounter - 1; i >= 0; i--) {
        FrameInterpolation_RecordOpenChild("Kankyo Sparkles", i);

        temp_f20 = 1.0f - (i / (f32)this->sparkleCounter);

        switch (this->unk_150[i].unk_22) {
            case 0:
                this->unk_150[i].unk_20 = 0;
                this->unk_150[i].unk_1C = 0;
                this->unk_150[i].unk_0.x = (s16)((Rand_ZeroOne() - 0.5f) * 16.0f * temp_f20);
                this->unk_150[i].unk_0.y = (s16)((Rand_ZeroOne() - 0.5f) * 16.0f * temp_f20);
                this->unk_150[i].unk_0.z = (s16)((Rand_ZeroOne() - 0.5f) * 16.0f * temp_f20);
                this->unk_150[i].unk_23 = 0;
                this->unk_150[i].unk_22++;
            case 1:
                if (func_800BB2B4(&camPos, &sSparklesRoll, &sSparklesFoV, sSparklesCameraPoints,
                                  &this->unk_150[i].unk_20, &this->unk_150[i].unk_1C) != 0) {
                    this->unk_150[i].unk_22++;
                }
                Actor_GetWorld(&posRot, &this->actor);
                DemoKankyo_Vec3fAddPosRot(&posRot, &camPos, &D_8098CFB8);
                break;
            case 2:
                if (i + 1 == this->sparkleCounter && globalCtx->csCtx.state == CS_STATE_IDLE) {
                    Actor_Kill(&this->actor);
                }
                break;
        }

        this->unk_150[i].unk_C.x = D_8098CFB8.x;
        this->unk_150[i].unk_C.y = D_8098CFB8.y;
        this->unk_150[i].unk_C.z = D_8098CFB8.z;

        switch (this->unk_150[i].unk_23) {
            case 0:
                this->unk_150[i].unk_18 = Rand_ZeroOne();
                this->unk_150[i].unk_23++;
            case 1:
                Math_SmoothStepToF(&this->unk_150[i].unk_18, 1.0f, 0.5f, 0.4f, 0.2f);
                if (1.0f <= this->unk_150[i].unk_18) {
                    this->unk_150[i].unk_23 = 2;
                }
                break;
            case 2:
                Math_SmoothStepToF(&this->unk_150[i].unk_18, 0.0f, 0.5f, 0.3f, 0.2f);
                if (this->unk_150[i].unk_18 <= 0.0f) {
                    this->unk_150[i].unk_0.x = (s16)((Rand_ZeroOne() - 0.5f) * 16.0f * temp_f20);
                    this->unk_150[i].unk_0.y = (s16)((Rand_ZeroOne() - 0.5f) * 16.0f * temp_f20);
                    this->unk_150[i].unk_0.z = (s16)((Rand_ZeroOne() - 0.5f) * 16.0f * temp_f20);
                    this->unk_150[i].unk_18 = 0.0f;
                    this->unk_150[i].unk_23 = 1;
                }
                break;
        }

        translateX = this->unk_150[i].unk_C.x + this->unk_150[i].unk_0.x;
        translateY = this->unk_150[i].unk_C.y + this->unk_150[i].unk_0.y;
        translateZ = this->unk_150[i].unk_C.z + this->unk_150[i].unk_0.z;

        if (this->unk_150[i].unk_22 < 2) {
            Matrix_Translate(translateX, translateY, translateZ, MTXMODE_NEW);
            scale = this->unk_150[i].unk_18 * (0.02f * temp_f20);
            Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, sSparkleEnvColors[3].r, sSparkleEnvColors[3].g, sSparkleEnvColors[3].b,
                           255);
            func_80093D84(globalCtx->state.gfxCtx);
            Matrix_Mult(&globalCtx->billboardMtxF, MTXMODE_APPLY);
            Matrix_RotateZ(DEG_TO_RAD(this->unk_150[i].unk_24), MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);
            this->unk_150[i].unk_24 += 0x190;
        }

        FrameInterpolation_RecordCloseChild();
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
