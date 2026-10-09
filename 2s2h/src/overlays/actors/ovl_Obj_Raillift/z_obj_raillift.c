/*
 * File: z_obj_raillift.c
 * Overlay: ovl_Obj_Raillift
 * Description: Moving Deku Flower Platform and OoT Water Temple Waterfall Platform
 */

#include "z_obj_raillift.h"
#include "objects/object_raillift/object_raillift.h"
#include "overlays/actors/ovl_Obj_Etcetera/z_obj_etcetera.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjRaillift_Init(Actor* thisx, PlayState* play);
void ObjRaillift_Destroy(Actor* thisx, PlayState* play);
void ObjRaillift_Update(Actor* thisx, PlayState* play);
void ObjRaillift_Draw(Actor* thisx, PlayState* play);

void ObjRaillift_DrawDekuFlowerPlatformColorful(Actor* thisx, PlayState* play);
void ObjRaillift_DrawDekuFlowerPlatform(Actor* thisx, PlayState* play);

void ObjRaillift_DoNothing(ObjRaillift* this, PlayState* play);
void ObjRaillift_Idle(ObjRaillift* this, PlayState* play);
void ObjRaillift_UpdatePosition(ObjRaillift* this, s32 index);
void ObjRaillift_StartCutscene(ObjRaillift* this, PlayState* play);
void ObjRaillift_Teleport(ObjRaillift* this, PlayState* play);
void ObjRaillift_Wait(ObjRaillift* this, PlayState* play);
void ObjRaillift_Move(ObjRaillift* this, PlayState* play);

ActorProfile Obj_Raillift_Profile = {
    /**/ ACTOR_OBJ_RAILLIFT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_RAILLIFT,
    /**/ sizeof(ObjRaillift),
    /**/ ObjRaillift_Init,
    /**/ ObjRaillift_Destroy,
    /**/ ObjRaillift_Update,
    /**/ ObjRaillift_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 400, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static CollisionHeader* sColHeaders[] = { &object_raillift_Colheader_004FF8, &object_raillift_Colheader_0048D0 };

void ObjRaillift_UpdatePosition(ObjRaillift* this, s32 index) {
    Math_Vec3s_ToVec3f(&this->dyna.actor.world.pos, &this->pathPoints[index]);
}

void ObjRaillift_Init(Actor* thisx, PlayState* play) {
    ObjRaillift* this = (ObjRaillift*)thisx;
    s32 pad;
    Path* path;
    s32 type = OBJRAILLIFT_GET_TYPE(thisx);
    s32 isColorful = false;

    Actor_ProcessInitChain(thisx, sInitChain);

    thisx->shape.rot.x = 0;
    thisx->world.rot.x = 0;
    thisx->shape.rot.z = 0;
    thisx->world.rot.z = 0;
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, sColHeaders[type]);
    this->speed = OBJRAILLIFT_GET_SPEED(thisx);
    if (this->speed < 0.0f) {
        this->speed = -this->speed;
        isColorful = true;
    }
    if (type == DEKU_FLOWER_PLATFORM) {
        Actor_SpawnAsChild(&play->actorCtx, thisx, play, ACTOR_OBJ_ETCETERA, thisx->world.pos.x, thisx->world.pos.y,
                           thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y, thisx->shape.rot.z,
                           DEKU_FLOWER_PARAMS(DEKU_FLOWER_TYPE_PINK));
        if (isColorful) {
            thisx->draw = ObjRaillift_DrawDekuFlowerPlatformColorful;
        } else {
            thisx->draw = ObjRaillift_DrawDekuFlowerPlatform;
        }
    }
    if (this->speed < 0.01f) {
        this->actionFunc = ObjRaillift_DoNothing;
    } else {
        path = &play->setupPathList[OBJRAILLIFT_GET_PATH_INDEX(thisx)];
        this->curPoint = OBJRAILLIFT_GET_STARTING_POINT(thisx);
        this->endPoint = path->count - 1;
        this->direction = 1;
        this->pathPoints = Lib_SegmentedToVirtual(path->points);
        ObjRaillift_UpdatePosition(this, this->curPoint);
        if (OBJRAILLIFT_HAS_FLAG(thisx) && !Flags_GetSwitch(play, OBJRAILLIFT_GET_SWITCH_FLAG(thisx))) {
            this->actionFunc = ObjRaillift_Idle;
        } else {
            this->actionFunc = ObjRaillift_Move;
        }
    }
}

void ObjRaillift_Destroy(Actor* thisx, PlayState* play) {
    ObjRaillift* this = (ObjRaillift*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjRaillift_DoNothing(ObjRaillift* this, PlayState* play) {
}

void ObjRaillift_Move(ObjRaillift* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;
    Vec3f nextPoint;
    f32 speed;
    f32 target;
    f32 step;
    s32 isTeleporting;
    s32 isPosUpdated;
    Vec3s* endPoint;

    if (OBJRAILLIFT_HAS_FLAG(thisx)) {
        if (!Flags_GetSwitch(play, OBJRAILLIFT_GET_SWITCH_FLAG(thisx))) {
            this->actionFunc = ObjRaillift_Idle;
            return;
        }

        if (OBJRAILLIFT_GET_TYPE(thisx) == DEKU_FLOWER_PLATFORM) {
            Actor_PlaySfx_Flagged(thisx, NA_SE_EV_PLATE_LIFT_LEVEL - SFX_FLAG);
        }
    }

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
        isTeleporting = OBJRAILLIFT_SHOULD_TELEPORT(thisx);
        isPosUpdated = true;
        if (((this->curPoint >= this->endPoint) && (this->direction > 0)) ||
            ((this->curPoint <= 0) && (this->direction < 0))) {
            if (!isTeleporting) {
                this->direction = -this->direction;
                this->waitTimer = 10;
                this->actionFunc = ObjRaillift_Wait;
            } else {
                endPoint = &this->pathPoints[this->endPoint];
                this->curPoint = this->direction > 0 ? 0 : this->endPoint;
                if ((this->pathPoints[0].x != endPoint->x) || (this->pathPoints[0].y != endPoint->y) ||
                    (this->pathPoints[0].z != endPoint->z)) {
                    this->actionFunc = ObjRaillift_Teleport;
                    DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
                    isPosUpdated = false;
                }
            }
        }

        if (isPosUpdated) {
            ObjRaillift_UpdatePosition(this, this->curPoint);
        }
    }
}

/*
Will teleport to what ever curpoint is set to
*/
void ObjRaillift_Teleport(ObjRaillift* this, PlayState* play) {
    if (!DynaPolyActor_IsPlayerOnTop(&this->dyna)) {
        ObjRaillift_UpdatePosition(this, this->curPoint);
        DynaPoly_EnableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        this->actionFunc = ObjRaillift_Move;
    }
}

void ObjRaillift_Wait(ObjRaillift* this, PlayState* play) {
    this->waitTimer--;
    if (this->waitTimer <= 0) {
        this->actionFunc = ObjRaillift_Move;
        this->dyna.actor.speed = 0.0f;
    }
}

void ObjRaillift_Idle(ObjRaillift* this, PlayState* play) {
    if (Flags_GetSwitch(play, OBJRAILLIFT_GET_SWITCH_FLAG(&this->dyna.actor))) {
        this->dyna.actor.speed = 0.0f;
        CutsceneManager_Queue(this->dyna.actor.csId);
        this->actionFunc = ObjRaillift_StartCutscene;
    }
}

void ObjRaillift_StartCutscene(ObjRaillift* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        this->cutsceneTimer = 50;
        this->actionFunc = ObjRaillift_Move;
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void ObjRaillift_Update(Actor* thisx, PlayState* play) {
    ObjRaillift* this = (ObjRaillift*)thisx;

    this->actionFunc(this, play);
    Actor_SetFocus(thisx, 10.0f);
    if (this->cutsceneTimer > 0) {
        this->cutsceneTimer--;
        if (this->cutsceneTimer == 0) {
            CutsceneManager_Stop(this->dyna.actor.csId);
        }
    }
    if (OBJRAILLIFT_REACT_TO_PLAYER_ON_TOP(thisx)) {
        f32 target;
        f32 step;

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
    if (OBJRAILLIFT_GET_TYPE(thisx) == DEKU_FLOWER_PLATFORM && this->dyna.actor.child != NULL) {
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

void ObjRaillift_Draw(Actor* thisx, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08,
               Gfx_TwoTexScrollEnvColorEx(play->state.gfxCtx, 0, play->gameplayFrames, 0, 32, 32, 1, 0, 0, 32, 32, 0, 0,
                                          0, 160, 1, 0, 0, 0));
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, object_raillift_DL_004BF0);

    CLOSE_DISPS(play->state.gfxCtx);
}

/*
The non-colorful platforms are the ones found in Woodfall Temple
*/
void ObjRaillift_DrawDekuFlowerPlatform(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, object_raillift_DL_000208);
}

/*
The colorful platforms are the ones found in Deku Palace
*/
void ObjRaillift_DrawDekuFlowerPlatformColorful(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, object_raillift_DL_0071B8);
}
