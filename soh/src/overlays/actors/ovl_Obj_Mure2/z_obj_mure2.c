/*
 * File: z_obj_mure2.c
 * Overlay: ovl_Obj_Mure2
 * Description: Rock/Bush groups
 */

#include "z_obj_mure2.h"

#define FLAGS 0

typedef void (*ObjMure2SetPosFunc)(Vec3f* vec, ObjMure2* this);

typedef struct {
    s16 radius;
    s16 angle;
} Mure2sScatteredShrubInfo;

void ObjMure2_Init(Actor* thisx, PlayState* play);
void ObjMure2_Update(Actor* thisx, PlayState* play);

void ObjMure2_SetPosShrubCircle(Vec3f* vec, ObjMure2* this);
void ObjMure2_SetPosShrubScattered(Vec3f* vec, ObjMure2* this);
void ObjMure2_SetPosRockCircle(Vec3f* vec, ObjMure2* this);
void ObjMure2_Wait(ObjMure2* this, PlayState* play);
void func_80B9A668(ObjMure2* this, PlayState* play);
void func_80B9A6F8(ObjMure2* this, PlayState* play);
void ObjMure2_SetupWait(ObjMure2* this);
void func_80B9A658(ObjMure2* this);
void func_80B9A6E8(ObjMure2* this);

const ActorInit Obj_Mure2_InitVars = {
    ACTOR_OBJ_MURE2,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ObjMure2),
    (ActorFunc)ObjMure2_Init,
    (ActorFunc)Actor_Noop,
    (ActorFunc)ObjMure2_Update,
    NULL,
    NULL,
};

static f32 sDistSquared1[] = { SQ(1600.0f), SQ(1600.0f), SQ(1600.0f) };

static f32 sDistSquared2[] = { SQ(1705.0f), SQ(1705.0f), SQ(1705.0f) };

static s16 D_80B9A818[] = { 9, 12, 8 };

static s16 sActorSpawnIDs[] = { ACTOR_EN_KUSA, ACTOR_EN_KUSA, ACTOR_EN_ISHI };

void ObjMure2_SetPosShrubCircle(Vec3f* vec, ObjMure2* this) {
    s32 i;

    Math_Vec3f_Copy(vec, &this->actor.world.pos);
    for (i = 1; i < D_80B9A818[this->actor.params & 3]; i++) {
        Math_Vec3f_Copy(vec + i, &this->actor.world.pos);
        (vec + i)->x += (80.0f * Math_SinS((i - 1) * 0x2000));
        (vec + i)->z += (80.0f * Math_CosS((i - 1) * 0x2000));
    }
}

static Mure2sScatteredShrubInfo sScatteredShrubInfo[] = {
    { 40, 0x0666 }, { 40, 0x2CCC }, { 40, 0x5999 }, { 40, 0x8666 }, { 20, 0xC000 }, { 80, 0x1333 },
    { 80, 0x4000 }, { 80, 0x6CCC }, { 80, 0x9333 }, { 80, 0xACCC }, { 80, 0xC666 }, { 60, 0xE000 },
};

void ObjMure2_SetPosShrubScattered(Vec3f* vec, ObjMure2* this) {
    s32 i;

    for (i = 0; i < D_80B9A818[this->actor.params & 3]; i++) {
        Math_Vec3f_Copy(vec + i, &this->actor.world.pos);
        (vec + i)->x += (sScatteredShrubInfo[i].radius * Math_CosS(sScatteredShrubInfo[i].angle));
        (vec + i)->z -= (sScatteredShrubInfo[i].radius * Math_SinS(sScatteredShrubInfo[i].angle));
    }
}

void ObjMure2_SetPosRockCircle(Vec3f* vec, ObjMure2* this) {
    s32 i;

    for (i = 0; i < D_80B9A818[this->actor.params & 3]; i++) {
        Math_Vec3f_Copy(vec + i, &this->actor.world.pos);
        (vec + i)->x += (80.0f * Math_SinS(i * 0x2000));
        (vec + i)->z += (80.0f * Math_CosS(i * 0x2000));
    }
}

void ObjMure2_SetActorSpawnParams(s16* params, ObjMure2* this) {
    static s16 actorSpawnParams[] = { 0, 0, 0 };
    s16 dropTable = (this->actor.params >> 8) & 0xF;

    if (dropTable >= 13) {
        dropTable = 0;
    }
    *params = actorSpawnParams[this->actor.params & 3] & 0xF0FF;
    *params |= (dropTable << 8);
}

void ObjMure2_SpawnActors(ObjMure2* this, PlayState* play) {
    static ObjMure2SetPosFunc setPosFunc[] = {
        ObjMure2_SetPosShrubCircle,
        ObjMure2_SetPosShrubScattered,
        ObjMure2_SetPosRockCircle,
    };
    s32 actorNum = this->actor.params & 3;
    s32 i;
    Vec3f spawnPos[12];
    s16 params;

    setPosFunc[actorNum](spawnPos, this);
    ObjMure2_SetActorSpawnParams(&params, this);

    for (i = 0; i < D_80B9A818[actorNum]; i++) {
        if (this->actorSpawnPtrList[i] != NULL) {
            // "Warning : I already have a child (%s %d)(arg_data 0x%04x)"
            osSyncPrintf("Warning : 既に子供がいる(%s %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                         this->actor.params);
            continue;
        }

        if (((this->currentActorNum >> i) & 1) == 0) {
            this->actorSpawnPtrList[i] =
                Actor_Spawn(&play->actorCtx, play, sActorSpawnIDs[actorNum], spawnPos[i].x, spawnPos[i].y,
                            spawnPos[i].z, this->actor.world.rot.x, 0, this->actor.world.rot.z, params, true);
            if (this->actorSpawnPtrList[i] != NULL) {
                this->actorSpawnPtrList[i]->room = this->actor.room;
            }
        }
    }
}

void ObjMure2_CleanupAndDie(ObjMure2* this, PlayState* play) {
    s32 i;

    for (i = 0; i < D_80B9A818[this->actor.params & 3]; i++) {
        if (((this->currentActorNum >> i) & 1) == 0) {
            if (this->actorSpawnPtrList[i] != NULL) {
                if (Actor_HasParent(this->actorSpawnPtrList[i], play)) {
                    this->currentActorNum |= (1 << i);
                } else {
                    Actor_Kill(this->actorSpawnPtrList[i]);
                }
                this->actorSpawnPtrList[i] = NULL;
            }
        } else {
            this->actorSpawnPtrList[i] = NULL;
        }
    }
}

void func_80B9A534(ObjMure2* this) {
    s32 i;

    for (i = 0; i < D_80B9A818[this->actor.params & 3]; i++) {
        if (this->actorSpawnPtrList[i] != NULL && (((this->currentActorNum >> i) & 1) == 0) &&
            (this->actorSpawnPtrList[i]->update == NULL)) {
            this->currentActorNum |= (1 << i);
            this->actorSpawnPtrList[i] = NULL;
        }
    }
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 2100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 100, ICHAIN_STOP),
};

void ObjMure2_Init(Actor* thisx, PlayState* play) {
    ObjMure2* this = (ObjMure2*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    if (play->csCtx.state != CS_STATE_IDLE) {
        this->actor.uncullZoneForward += 1200.0f;
    }
    ObjMure2_SetupWait(this);
}

void ObjMure2_SetupWait(ObjMure2* this) {
    this->actionFunc = ObjMure2_Wait;
}

void ObjMure2_Wait(ObjMure2* this, PlayState* play) {
    func_80B9A658(this);
}

void func_80B9A658(ObjMure2* this) {
    this->actionFunc = func_80B9A668;
}

void func_80B9A668(ObjMure2* this, PlayState* play) {
    if (Math3D_Dist1DSq(this->actor.projectedPos.x, this->actor.projectedPos.z) <
            (sDistSquared1[this->actor.params & 3] * this->unk_184) ||
        CVar_GetS32("gDisableDrawDistance", 0) != 0) {
        this->actor.flags |= ACTOR_FLAG_4;
        ObjMure2_SpawnActors(this, play);
        func_80B9A6E8(this);
    }
}

void func_80B9A6E8(ObjMure2* this) {
    this->actionFunc = func_80B9A6F8;
}

void func_80B9A6F8(ObjMure2* this, PlayState* play) {
    func_80B9A534(this);

    if (CVar_GetS32("gDisableDrawDistance", 0) != 0) {
        return;
    }

    if ((sDistSquared2[this->actor.params & 3] * this->unk_184) <=
            Math3D_Dist1DSq(this->actor.projectedPos.x, this->actor.projectedPos.z)) {
        this->actor.flags &= ~ACTOR_FLAG_4;
        ObjMure2_CleanupAndDie(this, play);
        func_80B9A658(this);
    }
}

void ObjMure2_Update(Actor* thisx, PlayState* play) {
    ObjMure2* this = (ObjMure2*)thisx;

    if (play->csCtx.state == CS_STATE_IDLE) {
        this->unk_184 = 1.0f;
    } else {
        this->unk_184 = 4.0f;
    }
    this->actionFunc(this, play);
}
