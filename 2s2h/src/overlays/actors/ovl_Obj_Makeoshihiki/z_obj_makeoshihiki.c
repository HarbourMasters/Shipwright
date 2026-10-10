/*
 * File: z_obj_makeoshihiki.c
 * Overlay: ovl_Obj_Makeoshihiki
 * Description: Pushable Block Switch Flags Handler
 */

#include "z_obj_makeoshihiki.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjMakeoshihiki_Init(Actor* thisx, PlayState* play);
void ObjMakeoshihiki_Update(Actor* thisx, PlayState* play);

ActorProfile Obj_Makeoshihiki_Profile = {
    /**/ ACTOR_OBJ_MAKEOSHIHIKI,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjMakeoshihiki),
    /**/ ObjMakeoshihiki_Init,
    /**/ Actor_Noop,
    /**/ ObjMakeoshihiki_Update,
    /**/ NULL,
};

s32 ObjMakeoshihiki_GetChildSpawnPointIndex(ObjMakeoshihiki* this, PlayState* play) {
    s32 pad;
    s32 pathIndexOffset1 = Flags_GetSwitch(play, OBJMAKEOSHIHIKI_GET_SWITCH_FLAG_1(&this->actor)) ? 1 : 0;
    s32 pathIndexOffset2 = Flags_GetSwitch(play, OBJMAKEOSHIHIKI_GET_SWITCH_FLAG_2(&this->actor)) ? 2 : 0;

    return pathIndexOffset1 + pathIndexOffset2;
}

void ObjMakeoshihiki_SetSwitchFlags(ObjMakeoshihiki* this, PlayState* play, s32 pathIndex) {
    s32 pad;
    s32 pad2;
    s32 switchFlag1;
    s32 switchFlag2;

    switchFlag2 = OBJMAKEOSHIHIKI_GET_SWITCH_FLAG_2(&this->actor);
    switchFlag1 = OBJMAKEOSHIHIKI_GET_SWITCH_FLAG_1(&this->actor);

    if (pathIndex & 2) {
        Flags_SetSwitch(play, switchFlag2);
    } else {
        Flags_UnsetSwitch(play, switchFlag2);
    }

    if (pathIndex & 1) {
        Flags_SetSwitch(play, switchFlag1);
    } else {
        Flags_UnsetSwitch(play, switchFlag1);
    }
}

void ObjMakeoshihiki_Init(Actor* thisx, PlayState* play) {
    ObjMakeoshihiki* this = (ObjMakeoshihiki*)thisx;
    Vec3s* childPoint;
    Path* path;
    s32 childPointIndex;

    path = &play->setupPathList[OBJMAKEOSHIHIKI_GET_PATH_INDEX(&this->actor)];
    this->pathPoints = Lib_SegmentedToVirtual(path->points);
    this->pathCount = path->count;
    childPointIndex = ObjMakeoshihiki_GetChildSpawnPointIndex(this, play);
    childPoint = &this->pathPoints[childPointIndex];
    if (Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_OBJ_OSHIHIKI, childPoint->x, childPoint->y,
                           childPoint->z, 0, 0, 0, 0xFFF1) == NULL) {
        Actor_Kill(&this->actor);
    }
}

void ObjMakeoshihiki_Update(Actor* thisx, PlayState* play) {
    ObjMakeoshihiki* this = (ObjMakeoshihiki*)thisx;
    Actor* child;
    s32 loopPathIndex;
    Vec3f pathPointF;

    child = this->actor.child;
    if (child != NULL) {
        if (child->update == NULL) {
            this->actor.child = NULL;
        } else {
            for (loopPathIndex = 0; loopPathIndex < this->pathCount; loopPathIndex++) {
                Math_Vec3s_ToVec3f(&pathPointF, &this->pathPoints[loopPathIndex]);
                if (Math3D_Vec3fDistSq(&this->actor.child->world.pos, &pathPointF) < SQ(0.5f)) {
                    ObjMakeoshihiki_SetSwitchFlags(this, play, loopPathIndex);
                    return;
                }
            }
        }
    }
}
