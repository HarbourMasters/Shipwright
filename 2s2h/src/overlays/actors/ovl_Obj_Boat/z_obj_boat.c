/*
 * File: z_obj_boat.c
 * Overlay: ovl_Obj_Boat
 * Description: Pirate Boat
 */

#include "z_obj_boat.h"
#include "objects/object_kaizoku_obj/object_kaizoku_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjBoat_Init(Actor* thisx, PlayState* play);
void ObjBoat_Destroy(Actor* thisx, PlayState* play);
void ObjBoat_Update(Actor* thisx, PlayState* play);
void ObjBoat_Draw(Actor* thisx, PlayState* play);

void ObjBoat_UpdateCutscene(Actor* thisx, PlayState* play2);

ActorProfile Obj_Boat_Profile = {
    /**/ ACTOR_OBJ_BOAT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_KAIZOKU_OBJ,
    /**/ sizeof(ObjBoat),
    /**/ ObjBoat_Init,
    /**/ ObjBoat_Destroy,
    /**/ ObjBoat_Update,
    /**/ ObjBoat_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

/**
 * Get the next point on the path and the yaw to use to get there.
 *
 * @param this
 * @param nextPoint Position of next point on path
 * @return yaw to use (or opposite of yaw if reversing)
 */
s16 ObjBoat_GetNextPoint(ObjBoat* this, Vec3f* nextPoint) {
    s16 yaw;
    Vec3s* curPoint = &this->pathPoints[(s32)this->curPointIndex];

    Math_Vec3s_ToVec3f(nextPoint, &curPoint[this->direction]);
    yaw = Math_Vec3f_Yaw(&this->dyna.actor.world.pos, nextPoint);

    return ((this->direction > 0) ? yaw : yaw + 0x8000);
}

void ObjBoat_Init(Actor* thisx, PlayState* play) {
    s32 pad[2];
    Path* path;
    ObjBoat* this = (ObjBoat*)thisx;
    Vec3f sp24;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
    DynaPolyActor_LoadMesh(play, &this->dyna, &object_kaizoku_obj_Colheader_009A88);
    if (thisx->params < 0) {
        this->dyna.actor.update = ObjBoat_UpdateCutscene;
    } else {
        path = &play->setupPathList[OBJBOAT_GET_PATH_INDEX(thisx)];
        this->maxPointIndex = path->count - 1;
        this->pathPoints = Lib_SegmentedToVirtual(path->points);
        this->direction = 1;
        this->dyna.actor.world.pos.x = this->pathPoints[this->curPointIndex].x;
        this->dyna.actor.world.pos.z = this->pathPoints[this->curPointIndex].z;
        this->dyna.actor.shape.rot.y = ObjBoat_GetNextPoint(this, &sp24);
        this->dyna.actor.world.rot.y = this->dyna.actor.shape.rot.y;
        this->direction = -this->direction;
    }
}

void ObjBoat_Destroy(Actor* thisx, PlayState* play) {
    ObjBoat* this = (ObjBoat*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjBoat_SetRotations(ObjBoat* this) {
    this->angle += 0x3E8;
    this->dyna.actor.world.pos.y = Math_SinS(this->angle) + this->dyna.actor.home.pos.y;
    this->dyna.actor.shape.rot.x = Math_SinS(this->angle) * 100.0f;
    this->dyna.actor.shape.rot.z = Math_SinS(this->angle * 2) * 50.0f;
}

void ObjBoat_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjBoat* this = (ObjBoat*)thisx;
    Player* player = GET_PLAYER(play);
    s32 isPlayerOnTop = DynaPolyActor_IsPlayerOnTop(&this->dyna);
    f32 speedTarget = 0.0f;
    s16 yawTarget = this->dyna.actor.shape.rot.y;
    Vec3f nextPoint;

    if (isPlayerOnTop || DynaPolyActor_IsActorOnTop(&this->dyna)) {
        if ((this->timer == 0) &&
            (OBJBOAT_GET_4000(thisx) || (isPlayerOnTop && (this->curPointIndex == this->lastPointIndex)))) {
            this->direction = -this->direction;
            if (this->direction > 0) {
                this->lastPointIndex = this->maxPointIndex;
            } else {
                this->lastPointIndex = 0;
            }
            this->timer = 60;
        }
    } else if (this->dyna.actor.speed == 0.0f) {
        if (this->timer != 0) {
            this->timer--;
        }
    }

    if (this->curPointIndex != this->lastPointIndex) {
        yawTarget = ObjBoat_GetNextPoint(this, &nextPoint);
        if (Math_Vec3f_DistXZ(&this->dyna.actor.world.pos, &nextPoint) < 200.0f) {
            this->curPointIndex += this->direction;
            if (this->curPointIndex == this->lastPointIndex) {
                if (OBJBOAT_GET_4000(thisx)) {
                    this->curPointIndex = 0;
                } else if (this->dyna.actor.speed == 0.0f) {
                    this->curPointIndex = 0;
                    this->direction = -1;
                }
            }
        } else {
            speedTarget = this->direction * (OBJBOAT_GET_4000(thisx) ? 5.0f : 3.0f);
        }
    }

    if (player->csAction != PLAYER_CSACTION_26) {
        Math_ScaledStepToS(&this->dyna.actor.shape.rot.y, yawTarget, (s32)(fabsf(this->dyna.actor.speed) * 40.0f));
        this->dyna.actor.world.rot.y = this->dyna.actor.shape.rot.y;
        Math_StepToF(&this->dyna.actor.speed, speedTarget, 0.05f);
        Actor_MoveWithGravity(&this->dyna.actor);
        if (this->dyna.actor.speed != 0.0f) {
            Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_PIRATE_SHIP - SFX_FLAG);
        }
    }
    ObjBoat_SetRotations(this);
}

// Update used in cutscenes
void ObjBoat_UpdateCutscene(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjBoat* this = (ObjBoat*)thisx;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_511)) {
        CsCmdActorCue* cue = play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_511)];

        if (this->cueId != cue->id) {
            this->dyna.actor.shape.rot.x = cue->rot.x;
            if (cue->id != 1) {
                Path* path = &play->setupPathList[OBJBOAT_GET_PATH_INDEX(&this->dyna.actor)];

                if (cue->id == 3) {
                    path = &play->setupPathList[path->additionalPathIndex];
                }
                //! @bug This is missing a -1 for last valid index
                // 2S2H [Port] Opting to fix this to keep it in bounds
                this->maxPointIndex = path->count - 1;
                this->pathPoints = Lib_SegmentedToVirtual(path->points);
                Math_Vec3s_ToVec3f(&this->dyna.actor.world.pos, this->pathPoints);
                this->dyna.actor.speed = cue->rot.z * (45.0f / 0x2000);
                this->pathPoints++;
                this->curPointIndex = 1;
            }

            this->cueId = cue->id;
        } else {
            if (cue->id != 1) {
                Vec3f posTarget;
                f32 distRemaining;

                Math_Vec3s_ToVec3f(&posTarget, this->pathPoints);
                distRemaining = Math_Vec3f_StepTo(&this->dyna.actor.world.pos, &posTarget, this->dyna.actor.speed);
                if ((this->curPointIndex < this->maxPointIndex) && (distRemaining < this->dyna.actor.speed)) {
                    this->pathPoints++;
                    this->curPointIndex++;
                }
            }

            if (cue->id != 3) {
                ObjBoat_SetRotations(this);
                if (cue->id == 2) {
                    Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_PIRATE_SHIP - SFX_FLAG);
                }
            } else {
                // Tumble in the air
                this->dyna.actor.shape.rot.y += 0x7D0;
                this->dyna.actor.shape.rot.x += 0x3E8;
                this->dyna.actor.shape.rot.z += 0x1F4;
            }
        }
    }
}

void ObjBoat_Draw(Actor* thisx, PlayState* play) {
    ObjBoat* this = (ObjBoat*)thisx;

    Gfx_DrawDListOpa(play, object_kaizoku_obj_DL_007630);
}
