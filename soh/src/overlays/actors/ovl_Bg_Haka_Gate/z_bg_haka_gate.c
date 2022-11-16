/*
 * File: z_bg_haka_gate.c
 * Overlay: ovl_Bg_Haka_Gate
 * Description: Truth Spinner Puzzle (Shadow Temple)
 */

#include "z_bg_haka_gate.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_haka_objects/object_haka_objects.h"

#define FLAGS 0

// general purpose timer
#define vTimer actionVar1

// variables for turning the statue. Deg10 rotations are in tenths of a degree
#define vTurnDirection actionVar1
#define vTurnRateDeg10 actionVar2
#define vTurnAngleDeg10 actionVar3
#define vRotYDeg10 actionVar4
#define vInitTurnAngle actionVar5

// opening angle for floor
#define vOpenAngle actionVar2

// variables for the skull flames
#define vFlameScale actionVar3
#define vIsSkullOfTruth actionVar4
#define vScrollTimer actionVar5

#define SKULL_OF_TRUTH_FOUND 100

void BgHakaGate_Init(Actor* thisx, PlayState* play);
void BgHakaGate_Destroy(Actor* thisx, PlayState* play);
void BgHakaGate_Update(Actor* thisx, PlayState* play);
void BgHakaGate_Draw(Actor* this, PlayState* play);
void BgHakaGate_Reset(void);

void BgHakaGate_DoNothing(BgHakaGate* this, PlayState* play);
void BgHakaGate_StatueInactive(BgHakaGate* this, PlayState* play);
void BgHakaGate_StatueIdle(BgHakaGate* this, PlayState* play);
void BgHakaGate_StatueTurn(BgHakaGate* this, PlayState* play);
void BgHakaGate_FloorClosed(BgHakaGate* this, PlayState* play);
void BgHakaGate_FloorOpen(BgHakaGate* this, PlayState* play);
void BgHakaGate_GateWait(BgHakaGate* this, PlayState* play);
void BgHakaGate_GateOpen(BgHakaGate* this, PlayState* play);
void BgHakaGate_SkullOfTruth(BgHakaGate* this, PlayState* play);
void BgHakaGate_FalseSkull(BgHakaGate* this, PlayState* play);

static s16 sSkullOfTruthRotY = 0x100;
static u8 sBgPoEventPuzzleState = 1;
static f32 sStatueDistToPlayer = 0;

static s16 sStatueRotY;

const ActorInit Bg_Haka_Gate_InitVars = {
    ACTOR_BG_HAKA_GATE,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_HAKA_OBJECTS,
    sizeof(BgHakaGate),
    (ActorFunc)BgHakaGate_Init,
    (ActorFunc)BgHakaGate_Destroy,
    (ActorFunc)BgHakaGate_Update,
    (ActorFunc)BgHakaGate_Draw,
    (ActorResetFunc)BgHakaGate_Reset,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgHakaGate_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHakaGate* this = (BgHakaGate*)thisx;
    CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(thisx, sInitChain);
    this->switchFlag = (thisx->params >> 8) & 0xFF;
    thisx->params &= 0xFF;
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    if (thisx->params == BGHAKAGATE_SKULL) {
        if (sSkullOfTruthRotY != 0x100) {
            this->actionFunc = BgHakaGate_FalseSkull;
        } else if (ABS(thisx->shape.rot.y) < 0x4000) {
            if ((Rand_ZeroOne() * 3.0f) < sBgPoEventPuzzleState) {
                this->vIsSkullOfTruth = true;
                sSkullOfTruthRotY = thisx->shape.rot.y + 0x8000;
                if (Flags_GetSwitch(play, this->switchFlag)) {
                    this->actionFunc = BgHakaGate_DoNothing;
                } else {
                    this->actionFunc = BgHakaGate_SkullOfTruth;
                }
            } else {
                sBgPoEventPuzzleState++;
                this->actionFunc = BgHakaGate_FalseSkull;
            }
        } else {
            this->actionFunc = BgHakaGate_FalseSkull;
        }
        this->vScrollTimer = Rand_ZeroOne() * 20.0f;
        thisx->flags |= ACTOR_FLAG_4;
        if (Flags_GetSwitch(play, this->switchFlag)) {
            this->vFlameScale = 350;
        }
    } else {
        if (thisx->params == BGHAKAGATE_STATUE) {
            CollisionHeader_GetVirtual(&object_haka_objects_Col_0131C4, &colHeader);
            this->vTimer = 0;
            sStatueDistToPlayer = 0.0f;
            if (Flags_GetSwitch(play, this->switchFlag)) {
                this->actionFunc = BgHakaGate_StatueInactive;
            } else {
                this->actionFunc = BgHakaGate_StatueIdle;
            }
        } else if (thisx->params == BGHAKAGATE_FLOOR) {
            CollisionHeader_GetVirtual(&object_haka_objects_Col_010E10, &colHeader);
            if (Flags_GetSwitch(play, this->switchFlag)) {
                this->actionFunc = BgHakaGate_DoNothing;
            } else {
                this->actionFunc = BgHakaGate_FloorClosed;
            }
        } else { // BGHAKAGATE_GATE
            CollisionHeader_GetVirtual(&object_haka_objects_Col_00A938, &colHeader);
            if (Flags_GetSwitch(play, this->switchFlag)) {
                this->actionFunc = BgHakaGate_DoNothing;
                thisx->world.pos.y += 80.0f;
            } else {
                thisx->flags |= ACTOR_FLAG_4;
                Actor_SetFocus(thisx, 30.0f);
                this->actionFunc = BgHakaGate_GateWait;
            }
        }
        this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, thisx, colHeader);
    }
}

void BgHakaGate_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHakaGate* this = (BgHakaGate*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    if (this->dyna.actor.params == BGHAKAGATE_STATUE) {
        sSkullOfTruthRotY = 0x100;
        sBgPoEventPuzzleState = 1;
    }
}

void BgHakaGate_DoNothing(BgHakaGate* this, PlayState* play) {
}

void BgHakaGate_StatueInactive(BgHakaGate* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->dyna.unk_150 != 0.0f) {
        player->stateFlags2 &= ~0x10;
        this->dyna.unk_150 = 0.0f;
    }
}

void BgHakaGate_StatueIdle(BgHakaGate* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 linkDirection;
    f32 forceDirection;

    if (this->dyna.unk_150 != 0.0f) {
        if (this->vTimer == 0) {
            this->vInitTurnAngle = this->dyna.actor.shape.rot.y - this->dyna.actor.yawTowardsPlayer;
            sStatueDistToPlayer = this->dyna.actor.xzDistToPlayer;
            forceDirection = (this->dyna.unk_150 >= 0.0f) ? 1.0f : -1.0f;
            linkDirection = ((s16)(this->dyna.actor.yawTowardsPlayer - player->actor.shape.rot.y) > 0) ? -1 : 1;
            this->vTurnDirection = linkDirection * forceDirection;
            this->actionFunc = BgHakaGate_StatueTurn;
        } else {
            player->stateFlags2 &= ~0x10;
            this->dyna.unk_150 = 0.0f;
            if (this->vTimer != 0) {
                this->vTimer--;
            }
        }
    } else {
        if (sBgPoEventPuzzleState == SKULL_OF_TRUTH_FOUND) {
            this->actionFunc = BgHakaGate_StatueInactive;
        } else {
            this->vTimer = 0;
        }
    }
}

void BgHakaGate_StatueTurn(BgHakaGate* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 turnFinished;
    s16 turnAngle;

    this->vTurnRateDeg10++;
    this->vTurnRateDeg10 = CLAMP_MAX(this->vTurnRateDeg10, 10);
    turnFinished = Math_StepToS(&this->vTurnAngleDeg10, 600, this->vTurnRateDeg10);
    turnAngle = this->vTurnAngleDeg10 * this->vTurnDirection;
    this->dyna.actor.shape.rot.y = (this->vRotYDeg10 + turnAngle) * 0.1f * (0x10000 / 360.0f);
    if ((player->stateFlags2 & 0x10) && (sStatueDistToPlayer > 0.0f)) {
        player->actor.world.pos.x =
            this->dyna.actor.home.pos.x +
            (Math_SinS(this->dyna.actor.shape.rot.y - this->vInitTurnAngle) * sStatueDistToPlayer);
        player->actor.world.pos.z =
            this->dyna.actor.home.pos.z +
            (Math_CosS(this->dyna.actor.shape.rot.y - this->vInitTurnAngle) * sStatueDistToPlayer);
    } else {
        sStatueDistToPlayer = 0.0f;
    }
    sStatueRotY = this->dyna.actor.shape.rot.y;
    if (turnFinished) {
        player->stateFlags2 &= ~0x10;
        this->vRotYDeg10 = (this->vRotYDeg10 + turnAngle) % 3600;
        this->vTurnRateDeg10 = CVar_GetS32("gFasterBlockPush", 0) * 2;
        this->vTurnAngleDeg10 = 0;
        this->vTimer = 5 - ((CVar_GetS32("gFasterBlockPush", 0) * 3) / 5);
        this->actionFunc = BgHakaGate_StatueIdle;
        this->dyna.unk_150 = 0.0f;
    }
    func_8002F974(&this->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
}

void BgHakaGate_FloorClosed(BgHakaGate* this, PlayState* play) {
    if ((sStatueDistToPlayer > 1.0f) && (sStatueRotY != 0)) {
        Player* player = GET_PLAYER(play);
        f32 radialDist;
        f32 angDist;
        f32 cos = Math_CosS(sStatueRotY);
        f32 sin = Math_SinS(sStatueRotY);
        f32 dx = player->actor.world.pos.x - this->dyna.actor.world.pos.x;
        f32 dz = player->actor.world.pos.z - this->dyna.actor.world.pos.z;

        radialDist = dx * cos - dz * sin;
        angDist = dx * sin + dz * cos;

        if ((radialDist > 110.0f) || (fabsf(angDist) > 40.0f)) {
            s16 yawDiff = sSkullOfTruthRotY - sStatueRotY;

            sStatueDistToPlayer = 0.0f;
            if (ABS(yawDiff) < 0x80) {
                Flags_SetSwitch(play, this->switchFlag);
                sBgPoEventPuzzleState = SKULL_OF_TRUTH_FOUND;
                this->actionFunc = BgHakaGate_DoNothing;
            } else {
                func_80078884(NA_SE_SY_ERROR);
                Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_GROUND_GATE_OPEN);
                func_8003EBF8(play, &play->colCtx.dyna, this->dyna.bgId);
                this->vTimer = 60;
                this->actionFunc = BgHakaGate_FloorOpen;
            }
        }
    }
}

void BgHakaGate_FloorOpen(BgHakaGate* this, PlayState* play) {
    if (this->vTimer != 0) {
        this->vTimer--;
    }
    if (this->vTimer == 0) {
        if (Math_ScaledStepToS(&this->vOpenAngle, 0, 0x800)) {
            func_8003EC50(play, &play->colCtx.dyna, this->dyna.bgId);
            this->actionFunc = BgHakaGate_FloorClosed;
        }
    } else {
        Math_ScaledStepToS(&this->vOpenAngle, 0x3000, 0x800);
    }
}

void BgHakaGate_GateWait(BgHakaGate* this, PlayState* play) {
    if (Flags_GetSwitch(play, this->switchFlag)) {
        OnePointCutscene_Attention(play, &this->dyna.actor);
        this->actionFunc = BgHakaGate_GateOpen;
    }
}

void BgHakaGate_GateOpen(BgHakaGate* this, PlayState* play) {
    if (Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 80.0f, 1.0f)) {
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_METALDOOR_STOP);
        this->dyna.actor.flags &= ~ACTOR_FLAG_4;
        this->actionFunc = BgHakaGate_DoNothing;
    } else {
        func_8002F974(&this->dyna.actor, NA_SE_EV_METALDOOR_SLIDE - SFX_FLAG);
    }
}

void BgHakaGate_SkullOfTruth(BgHakaGate* this, PlayState* play) {
    if (Flags_GetSwitch(play, this->switchFlag) && Math_StepToS(&this->vFlameScale, 350, 20)) {
        this->actionFunc = BgHakaGate_DoNothing;
    }
}

void BgHakaGate_FalseSkull(BgHakaGate* this, PlayState* play) {
    if (Flags_GetSwitch(play, this->switchFlag)) {
        Math_StepToS(&this->vFlameScale, 350, 20);
    }
    if (play->actorCtx.lensActive) {
        this->dyna.actor.flags |= ACTOR_FLAG_7;
    } else {
        this->dyna.actor.flags &= ~ACTOR_FLAG_7;
    }
}

void BgHakaGate_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHakaGate* this = (BgHakaGate*)thisx;

    this->actionFunc(this, play);
    if (this->dyna.actor.params == BGHAKAGATE_SKULL) {
        this->vScrollTimer++;
    }
}

void BgHakaGate_DrawFlame(BgHakaGate* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;
    f32 scale;

    if (this->vFlameScale > 0) {
        OPEN_DISPS(play->state.gfxCtx);

        func_80093D84(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                    (this->vScrollTimer * -20) & 0x1FF, 0x20, 0x80));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 0, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);

        Matrix_Translate(thisx->world.pos.x, thisx->world.pos.y + 15.0f, thisx->world.pos.z, MTXMODE_NEW);
        Matrix_RotateY(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) * (M_PI / 0x8000), MTXMODE_APPLY);
        scale = this->vFlameScale * 0.00001f;
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void BgHakaGate_Draw(Actor* thisx, PlayState* play) {
    static Gfx* displayLists[] = {
        object_haka_objects_DL_012270,
        object_haka_objects_DL_010A10,
        object_haka_objects_DL_00A860,
        object_haka_objects_DL_00F1B0,
    };
    BgHakaGate* this = (BgHakaGate*)thisx;
    MtxF currentMtxF;

    if (CHECK_FLAG_ALL(thisx->flags, ACTOR_FLAG_7)) {
        Gfx_DrawDListXlu(play, object_haka_objects_DL_00F1B0);
    } else {
        func_80093D18(play->state.gfxCtx);
        if (thisx->params == BGHAKAGATE_FLOOR) {
            OPEN_DISPS(play->state.gfxCtx);
            Matrix_Get(&currentMtxF);
            Matrix_Translate(0.0f, 0.0f, -2000.0f, MTXMODE_APPLY);
            Matrix_RotateX(this->vOpenAngle * (M_PI / 0x8000), MTXMODE_APPLY);
            Matrix_Translate(0.0f, 0.0f, 2000.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, object_haka_objects_DL_010A10);
            Matrix_Put(&currentMtxF);
            Matrix_Translate(0.0f, 0.0f, 2000.0f, MTXMODE_APPLY);
            Matrix_RotateX(-this->vOpenAngle * (M_PI / 0x8000), MTXMODE_APPLY);
            Matrix_Translate(0.0f, 0.0f, -2000.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, object_haka_objects_DL_010C10);
            CLOSE_DISPS(play->state.gfxCtx);
        } else {
            Gfx_DrawDListOpa(play, displayLists[thisx->params]);
        }
    }
    if (thisx->params == BGHAKAGATE_SKULL) {
        BgHakaGate_DrawFlame(this, play);
    }
}

void BgHakaGate_Reset(void) {
    sStatueRotY = 0;
}
