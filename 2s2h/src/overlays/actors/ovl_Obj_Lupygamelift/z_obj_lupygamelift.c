/*
 * File: z_obj_lupygamelift.c
 * Overlay: ovl_Obj_Lupygamelift
 * Description: Deku Scrub Playground - Rupee Elevator
 */

#include "z_obj_lupygamelift.h"
#include "objects/object_raillift/object_raillift.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void ObjLupygamelift_Init(Actor* thisx, PlayState* play);
void ObjLupygamelift_Destroy(Actor* thisx, PlayState* play);
void ObjLupygamelift_Update(Actor* thisx, PlayState* play);
void ObjLupygamelift_Draw(Actor* thisx, PlayState* play);

void func_80AF04BC(ObjLupygamelift* this);
void func_80AF04D8(ObjLupygamelift* this, PlayState* play);
void func_80AF0514(ObjLupygamelift* this);
void func_80AF0530(ObjLupygamelift* this, PlayState* play);

ActorProfile Obj_Lupygamelift_Profile = {
    /**/ ACTOR_OBJ_LUPYGAMELIFT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_RAILLIFT,
    /**/ sizeof(ObjLupygamelift),
    /**/ ObjLupygamelift_Init,
    /**/ ObjLupygamelift_Destroy,
    /**/ ObjLupygamelift_Update,
    /**/ ObjLupygamelift_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 400, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void ObjLupygamelift_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjLupygamelift* this = (ObjLupygamelift*)thisx;
    Path* path;
    s32 params;

    Actor_ProcessInitChain(thisx, sInitChain);
    this->dyna.actor.scale.y = 0.15f;
    this->dyna.actor.shape.rot.x = 0;
    this->dyna.actor.world.rot.x = 0;
    this->dyna.actor.shape.rot.z = 0;
    this->dyna.actor.world.rot.z = 0;
    this->timer = 0;
    Actor_UpdateBgCheckInfo(play, thisx, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawSquare, 0.0f);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, &object_raillift_Colheader_0048D0);
    this->targetSpeedXZ = thisx->home.rot.z * 0.1f;
    if (this->targetSpeedXZ < 0.0f) {
        this->targetSpeedXZ = -this->targetSpeedXZ;
    }
    this->dyna.actor.home.rot.x = 0;
    this->dyna.actor.home.rot.y = 0;
    this->dyna.actor.home.rot.z = 0;

    path = &play->setupPathList[OBJLUPYGAMELIFT_GET_PATH_INDEX(thisx)];
    this->pointIndex = OBJLUPYGAMELIFT_GET_7(thisx);
    this->count = path->count;
    if (this->pointIndex >= this->count) {
        this->pointIndex = 0;
    }
    this->pathPoints = Lib_SegmentedToVirtual(path->points);
    Actor_SpawnAsChild(&play->actorCtx, &this->dyna.actor, play, ACTOR_OBJ_ETCETERA, this->dyna.actor.world.pos.x,
                       this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z, this->dyna.actor.shape.rot.x,
                       this->dyna.actor.shape.rot.y, this->dyna.actor.shape.rot.z, 0);
    if (OBJLUPYGAMELIFT_GET_C(thisx) != 0) {
        params = 1;
    } else {
        params = 0;
    }
    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_GAMELUPY, this->dyna.actor.home.pos.x, this->dyna.actor.home.pos.y,
                this->dyna.actor.home.pos.z, 0, 0, 0, params);
    func_80AF04BC(this);
}

void ObjLupygamelift_Destroy(Actor* thisx, PlayState* play) {
    ObjLupygamelift* this = (ObjLupygamelift*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80AF0394(ObjLupygamelift* this) {
    static f32 D_80AF0750 = 100.0f;
    static f32 D_80AF0754 = 255.0f;
    static f32 D_80AF0758 = 5.0f;
    static f32 D_80AF075C = 10.0f;
    static f32 D_80AF0760 = -240.0f;
    static f32 D_80AF0764 = 30.0f;
    f32 new_var = D_80AF0754 - D_80AF0750;
    f32 new_var2 = D_80AF0764 - D_80AF0760;
    f32 new_var3 = D_80AF075C - D_80AF0758;
    f32 temp_fa0;
    f32 phi_fa1;

    temp_fa0 = this->dyna.actor.world.pos.y - D_80AF0760;
    if (temp_fa0 < 0.0f) {
        phi_fa1 = 0.0f;
    } else if (new_var2 < temp_fa0) {
        phi_fa1 = 1.0f;
    } else {
        phi_fa1 = temp_fa0 / new_var2;
    }
    this->dyna.actor.shape.shadowAlpha = D_80AF0754 - (phi_fa1 * new_var);
    this->dyna.actor.shape.shadowScale = D_80AF075C - (phi_fa1 * new_var3);
}

void func_80AF04BC(ObjLupygamelift* this) {
    this->timer = 5;
    this->actionFunc = func_80AF04D8;
}

void func_80AF04D8(ObjLupygamelift* this, PlayState* play) {
    if (this->timer == 0) {
        func_80AF0514(this);
    } else {
        this->timer--;
    }
}

void func_80AF0514(ObjLupygamelift* this) {
    this->actionFunc = func_80AF0530;
    this->dyna.actor.speed = this->targetSpeedXZ;
}

void func_80AF0530(ObjLupygamelift* this, PlayState* play) {
    f32 distRemaining;
    Vec3f target;

    target.x = this->pathPoints[this->pointIndex].x;
    target.y = this->pathPoints[this->pointIndex].y;
    target.z = this->pathPoints[this->pointIndex].z;
    distRemaining = Math_Vec3f_StepTo(&this->dyna.actor.world.pos, &target, this->dyna.actor.speed);
    if (distRemaining > 30.0f) {
        Math_SmoothStepToF(&this->dyna.actor.speed, this->targetSpeedXZ, 0.5f, 5.0f, 0.1f);
    } else if (distRemaining > 0.0f) {
        Math_SmoothStepToF(&this->dyna.actor.speed, 5.0f, 0.5f, 5.0f, 1.0f);
    } else {
        if (this->pointIndex < (this->count - 1)) {
            this->pointIndex++;
        } else {
            this->pointIndex = 0;
        }
    }
    if (this->dyna.actor.child->update == NULL) {
        this->dyna.actor.child = NULL;
    } else {
        this->dyna.actor.child->world.pos.x = this->dyna.actor.world.pos.x;
        this->dyna.actor.child->world.pos.y = this->dyna.actor.world.pos.y;
        this->dyna.actor.child->world.pos.z = this->dyna.actor.world.pos.z;
    }
    func_80AF0394(this);
}

void ObjLupygamelift_Update(Actor* thisx, PlayState* play) {
    ObjLupygamelift* this = (ObjLupygamelift*)thisx;

    this->actionFunc(this, play);
}

void ObjLupygamelift_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, object_raillift_DL_0071B8);
}
