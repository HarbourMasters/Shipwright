/*
 * File: z_obj_tokei_step.c
 * Overlay: obj_Tokei_Step
 * Description: Door to Top of Clock Tower
 */

#include "z_obj_tokei_step.h"
#include "z64quake.h"
#include "z64rumble.h"
#include "objects/object_tokei_step/object_tokei_step.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_IGNORE_LEGACY_POINT_LIGHTS)

void ObjTokeiStep_Init(Actor* thisx, PlayState* play);
void ObjTokeiStep_Destroy(Actor* thisx, PlayState* play);
void ObjTokeiStep_Update(Actor* thisx, PlayState* play);
void ObjTokeiStep_Draw(Actor* thisx, PlayState* play);

void ObjTokeiStep_SetupBeginOpen(ObjTokeiStep* this);
void ObjTokeiStep_BeginOpen(ObjTokeiStep* this, PlayState* play);
void ObjTokeiStep_SetupDoNothing(ObjTokeiStep* this);
void ObjTokeiStep_DoNothing(ObjTokeiStep* this, PlayState* play);
void ObjTokeiStep_SetupOpen(ObjTokeiStep* this);
void ObjTokeiStep_Open(ObjTokeiStep* this, PlayState* play);
void ObjTokeiStep_SetupDoNothingOpen(ObjTokeiStep* this);
void ObjTokeiStep_DoNothingOpen(ObjTokeiStep* this, PlayState* play);
void ObjTokeiStep_DrawOpen(Actor* thisx, PlayState* play);

ActorProfile Obj_Tokei_Step_Profile = {
    /**/ ACTOR_OBJ_TOKEI_STEP,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_TOKEI_STEP,
    /**/ sizeof(ObjTokeiStep),
    /**/ ObjTokeiStep_Init,
    /**/ ObjTokeiStep_Destroy,
    /**/ ObjTokeiStep_Update,
    /**/ ObjTokeiStep_Draw,
};

static f32 sPanelXOffsets[] = { -105.0f, -90.0f, -75.0f, -60.0f, -45.0f, -30.0f, -15.0f };

static f32 sDustSpawnXOffsets[] = { -60.0f, -40.0f, -20.0f, 0.0f, 20.0f, 40.0f, 60.0f };

static Vec3f sDustEffectAccel = { 0.0f, 0.3f, 0.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 300, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 300, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void ObjTokeiStep_SetMatrixTranslation(ObjTokeiStepPanel* panel) {
    MtxF* mtx = Matrix_GetCurrent();

    mtx->xw = panel->pos.x;
    mtx->yw = panel->pos.y;
    mtx->zw = panel->pos.z;
}

void ObjTokeiStep_RequestQuakeAndRumble(ObjTokeiStep* this, PlayState* play) {
    s32 pad[2];
    s16 quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);

    Quake_SetSpeed(quakeIndex, 20000);
    Quake_SetPerturbations(quakeIndex, 1, 0, 0, 0);
    Quake_SetDuration(quakeIndex, 7);

    Rumble_Request(this->dyna.actor.xyzDistToPlayerSq, 120, 20, 10);
}

void ObjTokeiStep_SpawnDust(ObjTokeiStep* this, ObjTokeiStepPanel* panel, PlayState* play) {
    s32 i;
    s32 pad;
    Vec3f dustSpawnOffset;
    Vec3f dustSpawnPos;

    Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_NEW);

    dustSpawnOffset.y = 115.0f;
    dustSpawnOffset.z = -10.0f;
    for (i = 0; i < 7; i++) {
        dustSpawnOffset.x = sDustSpawnXOffsets[i];
        Matrix_MultVec3f(&dustSpawnOffset, &dustSpawnPos);
        dustSpawnPos.x += panel->pos.x;
        dustSpawnPos.y += panel->pos.y;
        dustSpawnPos.z += panel->pos.z;
        func_800B1210(play, &dustSpawnPos, &gZeroVec3f, &sDustEffectAccel, (s32)((Rand_ZeroOne() * 40.0f) + 80.0f),
                      (s32)((Rand_ZeroOne() * 20.0f) + 50.0f));
    }
}

void ObjTokeiStep_InitSteps(ObjTokeiStep* this) {
    s32 i;
    ObjTokeiStepPanel* panel;
    Vec3f panelOffset;
    s32 pad;

    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);

    panelOffset.x = 0.0f;
    panelOffset.y = 0.0f;
    for (i = 0; i < ARRAY_COUNT(this->panels); i++) {
        panel = &this->panels[i];
        panelOffset.z = i * -20.0f;
        Matrix_MultVec3f(&panelOffset, &panel->pos);
        panel->posChangeY = 0.0f;
        panel->numBounces = 0;
    }
}

void ObjTokeiStep_InitStepsOpen(ObjTokeiStep* this) {
    s32 i;
    ObjTokeiStepPanel* panel;
    Vec3f panelOffset;

    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);

    panelOffset.x = 0.0f;
    for (i = 0; i < ARRAY_COUNT(this->panels); i++) {
        panel = &this->panels[i];
        panelOffset.y = sPanelXOffsets[i];
        panelOffset.z = i * -20.0f;
        Matrix_MultVec3f(&panelOffset, &panel->pos);
    }
}

void ObjTokeiStep_InitTimers(ObjTokeiStep* this) {
    s32 i;

    this->panels[0].startFallingTimer = 0;
    for (i = 1; i < ARRAY_COUNT(this->panels); i++) {
        this->panels[i].startFallingTimer = 10;
    }
}

s32 ObjTokeiStep_OpenProcess(ObjTokeiStep* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;
    s32 i;
    ObjTokeiStepPanel* panel;
    f32 finalPosY;
    s32 isOpen = true;
    s32 hasPrevBounced = true;

    for (i = 0; i < ARRAY_COUNT(this->panels); i++) {
        panel = &this->panels[i];
        if (hasPrevBounced && (panel->startFallingTimer > 0)) {
            panel->startFallingTimer--;
            isOpen = false;
        }
        if (hasPrevBounced && (panel->numBounces < 3) && (panel->startFallingTimer <= 0)) {
            finalPosY = sPanelXOffsets[i] + this->dyna.actor.world.pos.y;
            if (!panel->hasSoundPlayed) {
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_CLOCK_TOWER_STAIR_MOVE);
                panel->hasSoundPlayed = true;
            }
            panel->posChangeY += -2.5f;
            panel->posChangeY *= 0.83f;
            panel->pos.y += panel->posChangeY;
            isOpen = false;
            if (panel->pos.y < finalPosY) {
                panel->numBounces++;
                if (panel->numBounces >= 3) {
                    panel->pos.y = finalPosY;
                } else {
                    panel->posChangeY *= -0.4f;
                    if (panel->posChangeY > 4.0f) {
                        panel->posChangeY = 4.0f;
                    }
                    panel->pos.y = (panel->pos.y - finalPosY) * -0.4f;
                    if (panel->posChangeY < panel->pos.y) {
                        panel->pos.y = panel->posChangeY + finalPosY;
                    } else {
                        panel->pos.y += finalPosY;
                    }
                    if (panel->numBounces == 1) {
                        ObjTokeiStep_SpawnDust(this, panel, play);
                        ObjTokeiStep_RequestQuakeAndRumble(this, play);
                    }
                }
            }
        }
        hasPrevBounced = panel->numBounces > 0;
    }
    return isOpen;
}

void ObjTokeiStep_Init(Actor* thisx, PlayState* play) {
    ObjTokeiStep* this = (ObjTokeiStep*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);
    if ((play->sceneId == SCENE_CLOCKTOWER) && (gSaveContext.sceneLayer == 2) && (play->csCtx.scriptIndex == 0)) {
        DynaPolyActor_LoadMesh(play, &this->dyna, &gClocktowerPanelCol);
        ObjTokeiStep_InitSteps(this);
        ObjTokeiStep_SetupBeginOpen(this);
    } else if (((CURRENT_DAY == 3) && (CURRENT_TIME < CLOCK_TIME(6, 0))) || (gSaveContext.save.day >= 4)) {
        this->dyna.actor.draw = ObjTokeiStep_DrawOpen;
        ObjTokeiStep_InitStepsOpen(this);
        ObjTokeiStep_SetupDoNothingOpen(this);
    } else {
        DynaPolyActor_LoadMesh(play, &this->dyna, &gClocktowerPanelCol);
        ObjTokeiStep_InitSteps(this);
        ObjTokeiStep_SetupDoNothing(this);
    }
}

void ObjTokeiStep_Destroy(Actor* thisx, PlayState* play) {
    ObjTokeiStep* this = (ObjTokeiStep*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjTokeiStep_SetupBeginOpen(ObjTokeiStep* this) {
    this->actionFunc = ObjTokeiStep_BeginOpen;
}

void ObjTokeiStep_BeginOpen(ObjTokeiStep* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_134)) {
        cue = play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_134)];

        if ((cue->startFrame == play->csCtx.curFrame) && (cue->id != 0)) {
            this->dyna.actor.draw = ObjTokeiStep_DrawOpen;
            ObjTokeiStep_SetupOpen(this);
        }
    }
}

void ObjTokeiStep_SetupDoNothing(ObjTokeiStep* this) {
    this->actionFunc = ObjTokeiStep_DoNothing;
}

void ObjTokeiStep_DoNothing(ObjTokeiStep* this, PlayState* play) {
}

void ObjTokeiStep_SetupOpen(ObjTokeiStep* this) {
    ObjTokeiStep_InitTimers(this);
    this->actionFunc = ObjTokeiStep_Open;
}

void ObjTokeiStep_Open(ObjTokeiStep* this, PlayState* play) {
    if (ObjTokeiStep_OpenProcess(this, play)) {
        DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        ObjTokeiStep_SetupDoNothingOpen(this);
    }
}

void ObjTokeiStep_SetupDoNothingOpen(ObjTokeiStep* this) {
    this->dyna.actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actionFunc = ObjTokeiStep_DoNothingOpen;
}

void ObjTokeiStep_DoNothingOpen(ObjTokeiStep* this, PlayState* play) {
}

void ObjTokeiStep_Update(Actor* thisx, PlayState* play) {
    ObjTokeiStep* this = (ObjTokeiStep*)thisx;

    this->actionFunc(this, play);
}

void ObjTokeiStep_Draw(Actor* thisx, PlayState* play) {
    ObjTokeiStep* this = (ObjTokeiStep*)thisx;

    Gfx_DrawDListOpa(play, gClocktowerPanelDL);
}

void ObjTokeiStep_DrawOpen(Actor* thisx, PlayState* play) {
    ObjTokeiStep* this = (ObjTokeiStep*)thisx;
    s32 i;
    ObjTokeiStepPanel* panel;
    Gfx* gfx;

    OPEN_DISPS(play->state.gfxCtx);

    gfx = POLY_OPA_DISP;
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_25]);

    for (i = 0; i < ARRAY_COUNT(this->panels); i++) {
        panel = &this->panels[i];
        ObjTokeiStep_SetMatrixTranslation(panel);
        MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
        gSPDisplayList(gfx++, gClocktowerPanelDL);
    }
    POLY_OPA_DISP = gfx;

    CLOSE_DISPS(play->state.gfxCtx);
}
