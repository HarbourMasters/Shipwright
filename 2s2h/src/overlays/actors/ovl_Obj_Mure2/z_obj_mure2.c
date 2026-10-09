/*
 * File: z_obj_mure2.c
 * Overlay: ovl_Obj_Mure2
 * Description: Circle of rocks spawner
 */

#include "z_obj_mure2.h"

#include "2s2h/ShipUtils.h"
#include "GameInteractor/GameInteractor.h"

#define FLAGS 0x00000000

#define OBJ_MURE2_CHILD_COUNT_BUSH_RING 9
#define OBJ_MURE2_CHILD_COUNT_BUSH_SCATTERED 12
#define OBJ_MURE2_CHILD_COUNT_ROCK_RING 8

typedef enum Mure2ChildType {
    /* 0 */ OBJMURE2_CHILDTYPE_BUSH_RING,
    /* 1 */ OBJMURE2_CHILDTYPE_BUSH_SCATTERED,
    /* 2 */ OBJMURE2_CHILDTYPE_ROCK_RING,
    /* 3 */ OBJMURE2_CHILDTYPE_MAX
} Mure2ChildType;

void ObjMure2_Init(Actor* thisx, PlayState* play);
void ObjMure2_Update(Actor* thisx, PlayState* play);

void ObjMure2_SpawnChildren(ObjMure2* this, PlayState* play);
void func_809613C4(ObjMure2* this, PlayState* play);
void ObjMure2_WaitForPlayerInRange(ObjMure2* this, PlayState* play);
void ObjMure2_WaitForPlayerOutOfRange(ObjMure2* this, PlayState* play);
void ObjMure2_KillChildren(ObjMure2* this, PlayState* play);
void func_809613B0(ObjMure2* this);
void ObjMure2_SetupWaitForPlayerInRange(ObjMure2* this);
void ObjMure2_SetupWaitForPlayerOutOfRange(ObjMure2* this);
void ObjMure2_GetChildParams(s16*, ObjMure2*);

ActorProfile Obj_Mure2_Profile = {
    ACTOR_OBJ_MURE2,
    ACTORCAT_PROP,
    FLAGS,
    GAMEPLAY_KEEP,
    sizeof(ObjMure2),
    (ActorFunc)ObjMure2_Init,
    (ActorFunc)Actor_Noop,
    (ActorFunc)ObjMure2_Update,
    (ActorFunc)NULL,
};

static f32 sActivationRangesSq[OBJMURE2_CHILDTYPE_MAX] = {
    SQ(1600.0f), // OBJMURE2_CHILDTYPE_BUSH_RING
    SQ(1600.0f), // OBJMURE2_CHILDTYPE_BUSH_SCATTERED
    SQ(1600.0f), // OBJMURE2_CHILDTYPE_ROCK_RING
};

static f32 sDeactivationRangesSq[OBJMURE2_CHILDTYPE_MAX] = {
    SQ(1705.0f), // OBJMURE2_CHILDTYPE_BUSH_RING
    SQ(1705.0f), // OBJMURE2_CHILDTYPE_BUSH_SCATTERED
    SQ(1705.0f), // OBJMURE2_CHILDTYPE_ROCK_RING
};

static s16 sChildCounts[OBJMURE2_CHILDTYPE_MAX] = {
    OBJ_MURE2_CHILD_COUNT_BUSH_RING,      // OBJMURE2_CHILDTYPE_BUSH_RING
    OBJ_MURE2_CHILD_COUNT_BUSH_SCATTERED, // OBJMURE2_CHILDTYPE_BUSH_SCATTERED
    OBJ_MURE2_CHILD_COUNT_ROCK_RING,      // OBJMURE2_CHILDTYPE_ROCK_RING
};

static s16 sActorIds[OBJMURE2_CHILDTYPE_MAX] = {
    ACTOR_EN_KUSA, // OBJMURE2_CHILDTYPE_BUSH_RING
    ACTOR_EN_KUSA, // OBJMURE2_CHILDTYPE_BUSH_SCATTERED
    ACTOR_EN_ISHI, // OBJMURE2_CHILDTYPE_ROCK_RING
};

static VecPolarS sScatteredBushSpawnInfo[OBJ_MURE2_CHILD_MAX] = {
    { 40, 0x0666 }, { 40, 0x2CCC }, { 40, 0x5999 },  { 40, -0x799A }, { 20, -0x4000 }, { 80, 0x1333 },
    { 80, 0x4000 }, { 80, 0x6CCC }, { 80, -0x6CCD }, { 80, -0x5334 }, { 80, -0x399A }, { 60, -0x2000 },
};

void ObjMure2_GetBushCircleSpawnPos(Vec3f pos[OBJ_MURE2_CHILD_MAX], ObjMure2* this) {
    s32 i;

    Math_Vec3f_Copy(&pos[0], &this->actor.world.pos);
    for (i = 1; i < sChildCounts[OBJ_MURE2_GET_CHILD_TYPE(&this->actor)]; i++) {
        Math_Vec3f_Copy(&pos[i], &this->actor.world.pos);
        (pos + i)->x += 80.0f * Math_SinS((i - 1) * 0x2000);
        (pos + i)->z += 80.0f * Math_CosS((i - 1) * 0x2000);
    }
}

void ObjMure2_GetBushScatteredPos(Vec3f pos[OBJ_MURE2_CHILD_MAX], ObjMure2* this) {
    s32 i;

    for (i = 0; i < sChildCounts[OBJ_MURE2_GET_CHILD_TYPE(&this->actor)]; i++) {
        Math_Vec3f_Copy(pos + i, &this->actor.world.pos);
        (pos + i)->x += sScatteredBushSpawnInfo[i].distance * Math_CosS(sScatteredBushSpawnInfo[i].angle);
        (pos + i)->z -= sScatteredBushSpawnInfo[i].distance * Math_SinS(sScatteredBushSpawnInfo[i].angle);
    }
}

void ObjMure2_GetRocksSpawnPos(Vec3f pos[OBJ_MURE2_CHILD_MAX], ObjMure2* this) {
    s32 i;

    for (i = 0; i < sChildCounts[OBJ_MURE2_GET_CHILD_TYPE(&this->actor)]; i++) {
        Math_Vec3f_Copy(&pos[i], &this->actor.world.pos);
        (pos + i)->x += 80.0f * Math_SinS(i * 0x2000);
        (pos + i)->z += 80.0f * Math_CosS(i * 0x2000);
    }
}

void ObjMure2_GetChildParams(s16* childParams, ObjMure2* this) {
    Mure2ChildType childType = OBJ_MURE2_GET_CHILD_TYPE(&this->actor);
    s32 temp_a2 = OBJ_MURE2_GET_CHILD_UPPER_PARAMS(&this->actor);

    if (childType == OBJMURE2_CHILDTYPE_ROCK_RING) {
        *childParams = temp_a2 << 4;
    } else {
        *childParams = temp_a2 << 8;
    }
}

typedef void (*ObjMure2SpawnPosFunc)(Vec3f[OBJ_MURE2_CHILD_MAX], ObjMure2*);
static ObjMure2SpawnPosFunc sSpawnPosFuncs[OBJMURE2_CHILDTYPE_MAX] = {
    ObjMure2_GetBushCircleSpawnPos, // OBJMURE2_CHILDTYPE_BUSH_RING
    ObjMure2_GetBushScatteredPos,   // OBJMURE2_CHILDTYPE_BUSH_SCATTERED
    ObjMure2_GetRocksSpawnPos,      // OBJMURE2_CHILDTYPE_ROCK_RING
};

void ObjMure2_SpawnChildren(ObjMure2* this, PlayState* play) {
    Vec3f* pos;
    Mure2ChildType childType = OBJ_MURE2_GET_CHILD_TYPE(&this->actor);
    Vec3f spawnPos[OBJ_MURE2_CHILD_MAX];
    s16 childParams;
    s32 i;

    sSpawnPosFuncs[childType](spawnPos, this);
    ObjMure2_GetChildParams(&childParams, this);
    for (i = 0; i < sChildCounts[childType]; i++) {
        if (this->actors[i] != NULL) {
            continue;
        }
        if (((this->spawnFlags >> i) & 1) == 0) {
            pos = &spawnPos[i];
            this->actors[i] = Actor_SpawnAsChildAndCutscene(
                &play->actorCtx, play, sActorIds[childType], pos->x, pos->y, pos->z, this->actor.world.rot.x, 0,
                this->actor.world.rot.z, childParams, this->actor.csId, this->actor.halfDaysBits, NULL);
            if (GameInteractor_Should(VB_OBJ_MURE2_SET_CHILD_ROOM, this->actors[i] != NULL, this, i)) {
                this->actors[i]->room = this->actor.room;
            }
        }
    }
}

void ObjMure2_KillChildren(ObjMure2* this, PlayState* play) {
    s32 i;

    for (i = 0; i < sChildCounts[OBJ_MURE2_GET_CHILD_TYPE(&this->actor)]; i++) {
        if (((this->spawnFlags >> i) & 1) == 0) {
            if (this->actors[i] != NULL) {
                if (Actor_HasParent(this->actors[i], play)) {
                    this->spawnFlags |= 1 << i;
                } else {
                    Actor_Kill(this->actors[i]);
                }
                this->actors[i] = NULL;
            }
        } else {
            this->actors[i] = NULL;
        }
    }
}

void ObjMure2_ClearChildrenList(ObjMure2* this) {
    s32 i;

    for (i = 0; i < sChildCounts[OBJ_MURE2_GET_CHILD_TYPE(&this->actor)]; i++) {
        if (this->actors[i] == NULL) {
            continue;
        }
        if ((((this->spawnFlags >> i) & 1) == 0) && (this->actors[i]->update == NULL)) {
            this->spawnFlags |= (1 << i);
            this->actors[i] = NULL;
        }
    }
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 2100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 100, ICHAIN_STOP),
};

void ObjMure2_Init(Actor* thisx, PlayState* play) {
    ObjMure2* this = (ObjMure2*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    if (play->csCtx.state != CS_STATE_IDLE) {
        this->actor.cullingVolumeDistance += 1200.0f;
    }
    func_809613B0(this);
}

void func_809613B0(ObjMure2* this) {
    this->actionFunc = func_809613C4;
}

void func_809613C4(ObjMure2* this, PlayState* play) {
    ObjMure2_SetupWaitForPlayerInRange(this);
}

void ObjMure2_SetupWaitForPlayerInRange(ObjMure2* this) {
    this->actionFunc = ObjMure2_WaitForPlayerInRange;
}

void ObjMure2_WaitForPlayerInRange(ObjMure2* this, PlayState* play) {
    if (Math3D_Dist1DSq(this->actor.projectedPos.x, this->actor.projectedPos.z) <
        sActivationRangesSq[OBJ_MURE2_GET_CHILD_TYPE(&this->actor)] * this->rangeMultiplier) {
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        ObjMure2_SpawnChildren(this, play);
        ObjMure2_SetupWaitForPlayerOutOfRange(this);
    }
}

void ObjMure2_SetupWaitForPlayerOutOfRange(ObjMure2* this) {
    this->actionFunc = ObjMure2_WaitForPlayerOutOfRange;
}

void ObjMure2_WaitForPlayerOutOfRange(ObjMure2* this, PlayState* play) {
    ObjMure2_ClearChildrenList(this);

    if ((sDeactivationRangesSq[OBJ_MURE2_GET_CHILD_TYPE(&this->actor)] * this->rangeMultiplier) <=
        Math3D_Dist1DSq(this->actor.projectedPos.x, this->actor.projectedPos.z)) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        ObjMure2_KillChildren(this, play);
        ObjMure2_SetupWaitForPlayerInRange(this);
    }
}

void ObjMure2_Update(Actor* thisx, PlayState* play) {
    ObjMure2* this = (ObjMure2*)thisx;

    if (play->csCtx.state == CS_STATE_IDLE) {
        this->rangeMultiplier = 1.0f;
    } else {
        this->rangeMultiplier = 4.0f;
    }
    this->actionFunc(this, play);
}
