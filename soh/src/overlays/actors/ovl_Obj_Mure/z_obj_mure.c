/*
 * File: z_obj_mure.c
 * Overlay: ovl_Obj_Mure
 * Description: Spawns Fish, Bug, Butterfly
 */

#include "z_obj_mure.h"

#define FLAGS 0

void ObjMure_Init(Actor* thisx, PlayState* play);
void ObjMure_Destroy(Actor* thisx, PlayState* play);
void ObjMure_Update(Actor* thisx, PlayState* play);

void ObjMure_InitialAction(ObjMure* this, PlayState* play);
void ObjMure_CulledState(ObjMure* this, PlayState* play);
void ObjMure_ActiveState(ObjMure* this, PlayState* play);

s32 ObjMure_GetMaxChildSpawns(ObjMure* this);

const ActorInit Obj_Mure_InitVars = {
    ACTOR_OBJ_MURE,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ObjMure),
    (ActorFunc)ObjMure_Init,
    (ActorFunc)ObjMure_Destroy,
    (ActorFunc)ObjMure_Update,
    NULL,
    NULL,
};

typedef enum {
    /* 0 */ OBJMURE_TYPE_GRASS,
    /* 1 */ OBJMURE_TYPE_UNDEFINED,
    /* 2 */ OBJMURE_TYPE_FISH,
    /* 3 */ OBJMURE_TYPE_BUGS,
    /* 4 */ OBJMURE_TYPE_BUTTERFLY
} ObjMureType;

typedef enum {
    /* 0 */ OBJMURE_CHILD_STATE_0,
    /* 1 */ OBJMURE_CHILD_STATE_1, // Dead
    /* 2 */ OBJMURE_CHILD_STATE_2
} ObjMureChildState;

static f32 sZClip[] = { 1600.0f, 1600.0f, 1000.0f, 1000.0f, 1000.0f };

static s32 sMaxChildSpawns[] = { 12, 9, 8, 0 };

static s16 sSpawnActorIds[] = { ACTOR_EN_KUSA, 0, ACTOR_EN_FISH, ACTOR_EN_INSECT, ACTOR_EN_BUTTE };

static s16 sSpawnParams[] = { 0, 2, -1, 0, -1 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1200, ICHAIN_STOP),
};

s32 ObjMure_SetCullingImpl(Actor* thisx, PlayState* play) {
    ObjMure* this = (ObjMure*)thisx;
    s32 result;

    switch (this->type) {
        case OBJMURE_TYPE_FISH:
        case OBJMURE_TYPE_BUGS:
        case OBJMURE_TYPE_BUTTERFLY:
            Actor_ProcessInitChain(&this->actor, sInitChain);
            result = true;
            break;
        default:
            // "Error : Culling is not set.(%s %d)(arg_data 0x%04x)"
            osSyncPrintf("Error : カリングの設定がされていません。(%s %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                         this->actor.params);
            return false;
    }
    return result;
}

s32 ObjMure_SetCulling(Actor* thisx, PlayState* play) {
    if (!ObjMure_SetCullingImpl(thisx, play)) {
        return false;
    }
    return true;
}

void ObjMure_Init(Actor* thisx, PlayState* play) {
    ObjMure* this = (ObjMure*)thisx;

    this->chNum = (thisx->params >> 0xC) & 0x0F;
    this->ptn = (thisx->params >> 8) & 0x07;
    this->svNum = (thisx->params >> 5) & 0x03;
    this->type = thisx->params & 0x1F;

    if (this->ptn >= 4) {
        osSyncPrintf("Error 群れな敵 (%s %d)(arg_data 0x%04x)\n", __FILE__, __LINE__, thisx->params);
        Actor_Kill(&this->actor);
        return;
    } else if (this->type >= 5) {
        osSyncPrintf("Error 群れな敵 (%s %d)(arg_data 0x%04x)\n", __FILE__, __LINE__, thisx->params);
        Actor_Kill(&this->actor);
        return;
    } else if (!ObjMure_SetCulling(thisx, play)) {
        Actor_Kill(&this->actor);
        return;
    }
    this->actionFunc = ObjMure_InitialAction;
    osSyncPrintf("群れな敵 (arg_data 0x%04x)(chNum(%d) ptn(%d) svNum(%d) type(%d))\n", thisx->params, this->chNum,
                 this->ptn, this->svNum, this->type);
    if (ObjMure_GetMaxChildSpawns(this) <= 0) {
        osSyncPrintf("Warning : 個体数が設定されていません(%s %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                     thisx->params);
    }
}

void ObjMure_Destroy(Actor* thisx, PlayState* play) {
}

s32 ObjMure_GetMaxChildSpawns(ObjMure* this) {
    if (this->chNum == 0) {
        return sMaxChildSpawns[this->ptn];
    }
    return this->chNum;
}

void ObjMure_GetSpawnPos(Vec3f* outPos, Vec3f* inPos, s32 ptn, s32 idx) {
    if (ptn >= 4) {
        osSyncPrintf("おかしなの (%s %d)\n", __FILE__, __LINE__);
    }
    *outPos = *inPos;
}

void ObjMure_SpawnActors0(ObjMure* this, PlayState* play) {
    ActorContext* ac;
    s32 i;
    Vec3f pos;
    s32 pad;
    s32 maxChildren = ObjMure_GetMaxChildSpawns(this);

    for (i = 0; i < maxChildren; i++) {
        if (this->children[i] != NULL) {
            // "Error: I already have a child(%s %d)(arg_data 0x%04x)"
            osSyncPrintf("Error : 既に子供がいる(%s %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                         this->actor.params);
        }
        switch (this->childrenStates[i]) {
            case OBJMURE_CHILD_STATE_1:
                break;
            case OBJMURE_CHILD_STATE_2:
                ac = &play->actorCtx;
                ObjMure_GetSpawnPos(&pos, &this->actor.world.pos, this->ptn, i);
                this->children[i] =
                    Actor_Spawn(ac, play, sSpawnActorIds[this->type], pos.x, pos.y, pos.z, this->actor.world.rot.x,
                                this->actor.world.rot.y, this->actor.world.rot.z, sSpawnParams[this->type]);
                if (this->children[i] != NULL) {
                    this->children[i]->flags |= ACTOR_FLAG_ENKUSA_CUT;
                    this->children[i]->room = this->actor.room;
                } else {
                    osSyncPrintf("warning 発生失敗 (%s %d)\n", __FILE__, __LINE__);
                }
                break;
            default:
                ac = &play->actorCtx;
                ObjMure_GetSpawnPos(&pos, &this->actor.world.pos, this->ptn, i);
                this->children[i] =
                    Actor_Spawn(ac, play, sSpawnActorIds[this->type], pos.x, pos.y, pos.z, this->actor.world.rot.x,
                                this->actor.world.rot.y, this->actor.world.rot.z, sSpawnParams[this->type]);
                if (this->children[i] != NULL) {
                    this->children[i]->room = this->actor.room;
                } else {
                    osSyncPrintf("warning 発生失敗 (%s %d)\n", __FILE__, __LINE__);
                }
                break;
        }
    }
}

void ObjMure_SpawnActors1(ObjMure* this, PlayState* play) {
    ActorContext* ac = (ActorContext*)play; // fake match
    Actor* actor = &this->actor;
    Vec3f spawnPos;
    s32 maxChildren = ObjMure_GetMaxChildSpawns(this);
    s32 i;

    for (i = 0; i < maxChildren; i++) {
        if (this->children[i] != NULL) {
            osSyncPrintf("Error : 既に子供がいる(%s %d)(arg_data 0x%04x)\n", __FILE__, __LINE__, actor->params);
        }
        ac = &play->actorCtx;
        ObjMure_GetSpawnPos(&spawnPos, &actor->world.pos, this->ptn, i);
        this->children[i] = Actor_Spawn(ac, play, sSpawnActorIds[this->type], spawnPos.x, spawnPos.y, spawnPos.z,
                                        actor->world.rot.x, actor->world.rot.y, actor->world.rot.z,
                                        (this->type == 4 && i == 0) ? 1 : sSpawnParams[this->type]);
        if (this->children[i] != NULL) {
            this->childrenStates[i] = OBJMURE_CHILD_STATE_0;
            this->children[i]->room = actor->room;
        } else {
            this->childrenStates[i] = OBJMURE_CHILD_STATE_1;
            osSyncPrintf("warning 発生失敗 (%s %d)\n", __FILE__, __LINE__);
        }
    }
}

void ObjMure_SpawnActors(ObjMure* this, PlayState* play) {
    switch (this->svNum) {
        case 0:
            ObjMure_SpawnActors0(this, play);
            break;
        case 1:
            ObjMure_SpawnActors1(this, play);
            break;
    }
}

void ObjMure_KillActorsImpl(ObjMure* this, PlayState* play) {
    s32 maxChildren = ObjMure_GetMaxChildSpawns(this);
    s32 i;

    for (i = 0; i < maxChildren; i++) {
        switch (this->childrenStates[i]) {
            case OBJMURE_CHILD_STATE_1:
                this->children[i] = NULL;
                break;
            case OBJMURE_CHILD_STATE_2:
                if (this->children[i] != NULL) {
                    Actor_Kill(this->children[i]);
                    this->children[i] = NULL;
                }
                break;
            default:
                if (this->children[i] != NULL) {
                    if (Actor_HasParent(this->children[i], play)) {
                        this->children[i] = NULL;
                    } else {
                        Actor_Kill(this->children[i]);
                        this->children[i] = NULL;
                    }
                }
                break;
        }
    }
}

void ObjMure_KillActors(ObjMure* this, PlayState* play) {
    ObjMure_KillActorsImpl(this, play);
}

void ObjMure_CheckChildren(ObjMure* this, PlayState* play) {
    s32 maxChildren = ObjMure_GetMaxChildSpawns(this);
    s32 i;

    for (i = 0; i < maxChildren; i++) {
        if (this->children[i] != NULL) {
            if (this->childrenStates[i] == OBJMURE_CHILD_STATE_0) {
                if (this->children[i]->update != NULL) {
                    if (this->children[i]->flags & ACTOR_FLAG_ENKUSA_CUT) {
                        this->childrenStates[i] = OBJMURE_CHILD_STATE_2;
                    }
                } else {
                    this->childrenStates[i] = OBJMURE_CHILD_STATE_1;
                    this->children[i] = NULL;
                }
            } else if (this->childrenStates[i] == OBJMURE_CHILD_STATE_2 && this->children[i]->update == NULL) {
                this->childrenStates[i] = OBJMURE_CHILD_STATE_1;
                this->children[i] = NULL;
            }
        }
    }
}

void ObjMure_InitialAction(ObjMure* this, PlayState* play) {
    this->actionFunc = ObjMure_CulledState;
}

void ObjMure_CulledState(ObjMure* this, PlayState* play) {
    if (fabsf(this->actor.projectedPos.z) < sZClip[this->type] || CVar_GetS32("gDisableDrawDistance", 0) != 0) {
        this->actionFunc = ObjMure_ActiveState;
        this->actor.flags |= ACTOR_FLAG_4;
        ObjMure_SpawnActors(this, play);
    }
}

void ObjMure_SetFollowTargets(ObjMure* this, f32 randMax) {
    s32 index;
    s32 maxChildren = ObjMure_GetMaxChildSpawns(this);
    s32 i;

    for (i = 0; i < maxChildren; i++) {
        if (this->children[i] != NULL) {
            this->children[i]->child = NULL;
            if (Rand_ZeroOne() <= randMax) {
                index = Rand_ZeroOne() * (maxChildren - 0.5f);
                if (i != index) {
                    this->children[i]->child = this->children[index];
                }
            }
        }
    }
}

/**
 * Selects a child that will follow after the player
 * `idx1` is the index + 1 of the child that will follow the player. If `idx1` is zero, no actor will follow the player
 */
void ObjMure_SetChildToFollowPlayer(ObjMure* this, s32 idx1) {
    s32 maxChildren = ObjMure_GetMaxChildSpawns(this);
    s32 i;
    s32 i2;
    s32 j;

    for (i = 0, i2 = 0; i < maxChildren; i++) {
        if (this->children[i] != NULL) {
            if (i2 < idx1) {
                i2++;
                this->children[i]->child = this->children[i];
                for (j = 0; j < maxChildren; j++) {
                    if (i != j && this->children[j]->child == this->children[i]) {
                        this->children[j]->child = NULL;
                    }
                }
            } else if (this->children[i]->child == this->children[i]) {
                this->children[i]->child = NULL;
            }
        }
    }
}

// Fish, Bugs
void ObjMure_GroupBehavior0(ObjMure* this, PlayState* play) {
    if (this->unk_1A4 <= 0) {
        if (this->unk_1A6) {
            this->unk_1A6 = false;
            ObjMure_SetFollowTargets(this, (Rand_ZeroOne() * 0.5f) + 0.1f);
            if (this->actor.xzDistToPlayer < 60.0f) {
                this->unk_1A4 = (s16)(Rand_ZeroOne() * 5.5f) + 4;
            } else {
                this->unk_1A4 = (s16)(Rand_ZeroOne() * 40.5f) + 4;
            }
        } else {
            this->unk_1A6 = true;
            if (this->actor.xzDistToPlayer < 60.0f) {
                this->unk_1A4 = (s16)(Rand_ZeroOne() * 10.5f) + 4;
                ObjMure_SetFollowTargets(this, (Rand_ZeroOne() * 0.2f) + 0.8f);
            } else {
                this->unk_1A4 = (s16)(Rand_ZeroOne() * 10.5f) + 4;
                ObjMure_SetFollowTargets(this, (Rand_ZeroOne() * 0.2f) + 0.6f);
            }
        }
    }
    if (this->actor.xzDistToPlayer < 120.0f) {
        this->unk_1A8++;
    } else {
        this->unk_1A8 = 0;
    }
    if (this->unk_1A8 >= 80) {
        ObjMure_SetChildToFollowPlayer(this, 1);
    } else {
        ObjMure_SetChildToFollowPlayer(this, 0);
    }
}

// Butterflies
void ObjMure_GroupBehavior1(ObjMure* this, PlayState* play) {
    s32 maxChildren;
    s32 i;

    if (this->unk_1A4 <= 0) {
        if (this->unk_1A6) {
            this->unk_1A6 = false;
            ObjMure_SetFollowTargets(this, Rand_ZeroOne() * 0.2f);
            if (this->actor.xzDistToPlayer < 60.0f) {
                this->unk_1A4 = (s16)(Rand_ZeroOne() * 5.5f) + 4;
            } else {
                this->unk_1A4 = (s16)(Rand_ZeroOne() * 40.5f) + 4;
            }
        } else {
            this->unk_1A6 = true;
            ObjMure_SetFollowTargets(this, Rand_ZeroOne() * 0.7f);
            this->unk_1A4 = (s16)(Rand_ZeroOne() * 10.5f) + 4;
        }
    }

    maxChildren = ObjMure_GetMaxChildSpawns(this);
    for (i = 0; i < maxChildren; i++) {
        if (this->children[i] != NULL) {
            if (this->children[i]->child != NULL && this->children[i]->child->update == NULL) {
                this->children[i]->child = NULL;
            }
        }
    }
}

static ObjMureActionFunc sTypeGroupBehaviorFunc[] = {
    NULL, NULL, ObjMure_GroupBehavior0, ObjMure_GroupBehavior0, ObjMure_GroupBehavior1,
};

void ObjMure_ActiveState(ObjMure* this, PlayState* play) {
    ObjMure_CheckChildren(this, play);
    if (sZClip[this->type] + 40.0f <= fabsf(this->actor.projectedPos.z) &&
        CVar_GetS32("gDisableDrawDistance", 1) != 0) {
        this->actionFunc = ObjMure_CulledState;
        this->actor.flags &= ~ACTOR_FLAG_4;
        ObjMure_KillActors(this, play);
    } else if (sTypeGroupBehaviorFunc[this->type] != NULL) {
        sTypeGroupBehaviorFunc[this->type](this, play);
    }
}

void ObjMure_Update(Actor* thisx, PlayState* play) {
    ObjMure* this = (ObjMure*)thisx;

    if (this->unk_1A4 > 0) {
        this->unk_1A4--;
    }
    this->actionFunc(this, play);
}
