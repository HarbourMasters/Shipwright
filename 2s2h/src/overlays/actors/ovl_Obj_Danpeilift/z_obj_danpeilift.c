/*
 * File: z_obj_danpeilift.c
 * Overlay: ovl_Obj_Danpeilift
 * Description: Deku Shrine & Snowhead Temple floating blocks
 */

#include "z_obj_danpeilift.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjDanpeilift_Init(Actor* thisx, PlayState* play);
void ObjDanpeilift_Destroy(Actor* thisx, PlayState* play);
void ObjDanpeilift_Update(Actor* thisx, PlayState* play);
void ObjDanpeilift_Draw(Actor* thisx, PlayState* play);

void ObjDanpeilift_UpdatePosition(ObjDanpeilift* this, s32 index);
void ObjDanpeilift_DoNothing(ObjDanpeilift* this, PlayState* play);
void ObjDanpeilift_Move(ObjDanpeilift* this, PlayState* play);
void ObjDanpeilift_Teleport(ObjDanpeilift* this, PlayState* play);
void ObjDanpeilift_Wait(ObjDanpeilift* this, PlayState* play);

ActorProfile Obj_Danpeilift_Profile = {
    /**/ ACTOR_OBJ_DANPEILIFT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_OBJ_DANPEILIFT,
    /**/ sizeof(ObjDanpeilift),
    /**/ ObjDanpeilift_Init,
    /**/ ObjDanpeilift_Destroy,
    /**/ ObjDanpeilift_Update,
    /**/ ObjDanpeilift_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 400, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void ObjDanpeilift_UpdatePosition(ObjDanpeilift* this, s32 index) {
    Math_Vec3s_ToVec3f(&this->dyna.actor.world.pos, &this->pathPoints[index]);
}

void ObjDanpeilift_Init(Actor* thisx, PlayState* play) {
    Path* path;
    ObjDanpeiliftActionFunc tempActionFunc;
    ObjDanpeilift* this = (ObjDanpeilift*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    this->dyna.actor.shape.rot.x = 0;
    this->dyna.actor.world.rot.x = 0;
    this->dyna.actor.shape.rot.z = 0;
    this->dyna.actor.world.rot.z = 0;
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, &object_obj_danpeilift_Colheader_000BA0);
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    this->speed = OBJDANPEILIFT_GET_SPEED(thisx);
    if (this->speed < 0.0f) {
        this->speed = -this->speed;
    }
    if (this->speed < 0.01f) {
        this->actionFunc = ObjDanpeilift_DoNothing;
    } else {
        path = &play->setupPathList[OBJDANPEILIFT_GET_PATH_INDEX(thisx)];
        this->curPoint = OBJDANPEILIFT_GET_STARTING_POINT(thisx);
        this->endPoint = path->count - 1;
        this->direction = 1;
        this->pathPoints = Lib_SegmentedToVirtual(path->points);
        ObjDanpeilift_UpdatePosition(this, this->curPoint);
        this->actionFunc = ObjDanpeilift_Move;
    }
}

void ObjDanpeilift_Destroy(Actor* thisx, PlayState* play) {
    ObjDanpeilift* this = (ObjDanpeilift*)thisx;
    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjDanpeilift_DoNothing(ObjDanpeilift* this, PlayState* play) {
}

void ObjDanpeilift_Move(ObjDanpeilift* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;
    Vec3f nextPoint;
    f32 speed;
    f32 target;
    f32 step;
    s32 isTeleporting;
    s32 isPosUpdated;
    Vec3s* endPoint;

    Math_Vec3s_ToVec3f(&nextPoint, this->pathPoints + this->curPoint + this->direction);
    Math_Vec3f_Diff(&nextPoint, &thisx->world.pos, &thisx->velocity);
    speed = Math3D_Vec3fMagnitude(&thisx->velocity);
    if ((speed < (this->speed * 8.0f)) && (this->speed > 2.0f)) {
        target = ((this->speed - 2.0f) * 0.1f) + 2.0f;
        step = this->speed * 0.03f;
    } else {
        target = this->speed;
        step = this->speed * 0.16f;
    }

    Math_StepToF(&thisx->speed, target, step);
    if ((thisx->speed + 0.05f) < speed) {
        Math_Vec3f_Scale(&thisx->velocity, thisx->speed / speed);
        thisx->world.pos.x += thisx->velocity.x;
        thisx->world.pos.y += thisx->velocity.y;
        thisx->world.pos.z += thisx->velocity.z;
    } else {
        this->curPoint += this->direction;
        thisx->speed *= 0.4f;
        isTeleporting = OBJDANPEILIFT_SHOULD_TELEPORT(thisx);
        isPosUpdated = true;
        if (((this->curPoint >= this->endPoint) && (this->direction > 0)) ||
            ((this->curPoint <= 0) && (this->direction < 0))) {
            if (!isTeleporting) {
                this->direction = -this->direction;
                this->waitTimer = 10;
                this->actionFunc = ObjDanpeilift_Wait;
            } else {
                endPoint = &this->pathPoints[this->endPoint];
                this->curPoint = this->direction > 0 ? 0 : this->endPoint;
                if ((this->pathPoints[0].x != endPoint->x) || (this->pathPoints[0].y != endPoint->y) ||
                    (this->pathPoints[0].z != endPoint->z)) {
                    this->actionFunc = ObjDanpeilift_Teleport;
                    DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
                    isPosUpdated = false;
                }
            }
        }

        if (isPosUpdated) {
            ObjDanpeilift_UpdatePosition(this, this->curPoint);
        }
    }
}

void ObjDanpeilift_Teleport(ObjDanpeilift* this, PlayState* play) {
    if (!DynaPolyActor_IsPlayerOnTop(&this->dyna)) {
        ObjDanpeilift_UpdatePosition(this, this->curPoint);
        DynaPoly_EnableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        this->actionFunc = ObjDanpeilift_Move;
    }
}

void ObjDanpeilift_Wait(ObjDanpeilift* this, PlayState* play) {
    this->waitTimer--;
    if (this->waitTimer <= 0) {
        this->actionFunc = ObjDanpeilift_Move;
        this->dyna.actor.speed = 0.0f;
    }
}

void ObjDanpeilift_Update(Actor* thisx, PlayState* play) {
    f32 step;
    ObjDanpeilift* this = (ObjDanpeilift*)thisx;

    this->actionFunc(this, play);
    Actor_SetFocus(&this->dyna.actor, 10.0f);
    if (this->cutsceneTimer > 0) {
        this->cutsceneTimer--;
        if (this->cutsceneTimer == 0) {
            CutsceneManager_Stop(this->dyna.actor.csId);
        }
    }
    if (OBJDANPEILIFT_REACT_TO_PLAYER_ON_TOP(thisx)) {
        f32 target;

        this->isPlayerOnTopPrev = this->isPlayerOnTop;
        this->isPlayerOnTop = DynaPolyActor_IsPlayerOnTop(&this->dyna) ? true : false;
        if ((this->isPlayerOnTop != this->isPlayerOnTopPrev) && (this->maxHeight < 1.0f)) {
            this->cycle = -0x8000;
            this->maxHeight = 6.0f;
        }
        this->cycle += 0xCE4;
        Math_StepToF(&this->maxHeight, 0.0f, 0.12f);
        step = this->isPlayerOnTop ? Math_CosS(fabsf(this->cycleSpeed) * 2048.0f) + 0.02f
                                   : Math_SinS(fabsf(this->cycleSpeed) * 2048.0f) + 0.02f;
        target = this->isPlayerOnTop ? -8.0f : 0.0f;
        Math_StepToF(&this->cycleSpeed, target, step);
        this->dyna.actor.shape.yOffset = ((Math_SinS(this->cycle) * this->maxHeight) + this->cycleSpeed) * 10.0f;
    }
    if ((OBJDANPEILIFT_GET_TYPE(thisx) == 1) && (this->dyna.actor.child != NULL)) {
        if (this->dyna.actor.child->update == NULL) {
            this->dyna.actor.child = NULL;
        } else {
            this->dyna.actor.child->world.pos.x = this->dyna.actor.world.pos.x;
            this->dyna.actor.child->world.pos.y =
                this->dyna.actor.world.pos.y + (this->dyna.actor.shape.yOffset * this->dyna.actor.scale.y);
            this->dyna.actor.child->world.pos.z = this->dyna.actor.world.pos.z;
        }
    }
}

void ObjDanpeilift_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, object_obj_danpeilift_DL_000180);
}
