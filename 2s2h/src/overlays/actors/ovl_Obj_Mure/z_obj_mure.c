/*
 * File: z_obj_mure.c
 * Overlay: ovl_Obj_Mure
 * Description: Bugs, Insects, Butterfly spawner and behavior
 */

#include "z_obj_mure.h"
#include "overlays/actors/ovl_En_Butte/z_en_butte.h"
#include "overlays/actors/ovl_En_Fish/z_en_fish.h"
#include "overlays/actors/ovl_En_Insect/z_en_insect.h"
#include "overlays/actors/ovl_En_Kusa/z_en_kusa.h"

#include "2s2h/ShipUtils.h"

#define FLAGS 0x00000000

void ObjMure_Init(Actor* thisx, PlayState* play);
void ObjMure_Destroy(Actor* thisx, PlayState* play);
void ObjMure_Update(Actor* thisx, PlayState* play);

void ObjMure_InitialAction(ObjMure* this, PlayState* play);
void ObjMure_CulledState(ObjMure* this, PlayState* play);
void ObjMure_ActiveState(ObjMure* this, PlayState* play);
void ObjMure_KillActors(ObjMure* this, PlayState* play);
void ObjMure_CheckChildren(ObjMure* this, PlayState* play);

ActorProfile Obj_Mure_Profile = {
    /**/ ACTOR_OBJ_MURE,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjMure),
    /**/ ObjMure_Init,
    /**/ ObjMure_Destroy,
    /**/ ObjMure_Update,
    /**/ NULL,
};

static f32 sZClip[OBJMURE_TYPE_MAX] = {
    1600.0f, // OBJMURE_TYPE_GRASS
    1600.0f, // OBJMURE_TYPE_UNDEFINED
    1000.0f, // OBJMURE_TYPE_FISH
    1000.0f, // OBJMURE_TYPE_BUGS
    1000.0f, // OBJMURE_TYPE_BUTTERFLY
};

static s32 sMaxChildSpawns[] = {
    12,
    9,
    8,
    0,
};

static s16 sSpawnActorIds[OBJMURE_TYPE_MAX] = {
    ACTOR_EN_KUSA,   // OBJMURE_TYPE_GRASS
    ACTOR_PLAYER,    // OBJMURE_TYPE_UNDEFINED
    ACTOR_EN_FISH,   // OBJMURE_TYPE_FISH
    ACTOR_EN_INSECT, // OBJMURE_TYPE_BUGS
    ACTOR_EN_BUTTE,  // OBJMURE_TYPE_BUTTERFLY
};

static s16 sSpawnParams[OBJMURE_TYPE_MAX] = {
    KUSA_BUSH_PARAMS(false, 0, false),           // OBJMURE_TYPE_GRASS
    PLAYER_PARAMS(2, PLAYER_START_MODE_NOTHING), // OBJMURE_TYPE_UNDEFINED
    FISH_PARAMS(ENFISH_MINUS1),                  // OBJMURE_TYPE_FISH
    ENINSECT_PARAMS(false),                      // OBJMURE_TYPE_BUGS
    BUTTERFLY_PARAMS(BUTTERFLY_MINUS1),          // OBJMURE_TYPE_BUTTERFLY
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1200, ICHAIN_STOP),
};

typedef enum {
    /* 0 */ OBJMURE_CHILD_STATE_0,
    /* 1 */ OBJMURE_CHILD_STATE_DEAD,
    /* 2 */ OBJMURE_CHILD_STATE_2
} ObjMureChildState;

s32 func_808D78D0(ObjMure* this, PlayState* play) {
    if ((this->type == OBJMURE_TYPE_FISH) || (this->type == OBJMURE_TYPE_BUGS) ||
        (this->type == OBJMURE_TYPE_BUTTERFLY)) {
        Actor_ProcessInitChain(&this->actor, sInitChain);
    } else {
        return false;
    }
    return true;
}

s32 func_808D7928(ObjMure* this, PlayState* play) {
    if (!func_808D78D0(this, play)) {
        return false;
    }
    return true;
}

void ObjMure_Init(Actor* thisx, PlayState* play) {
    ObjMure* this = (ObjMure*)thisx;

    this->chNum = OBJ_MURE_GET_CHNUM(&this->actor);
    this->ptn = OBJ_MURE_GET_PTN(&this->actor);
    this->svNum = OBJ_MURE_GET_SVNUM(&this->actor);
    this->type = OBJ_MURE_GET_TYPE(&this->actor);
    if (this->ptn >= 4) {
        Actor_Kill(&this->actor);
        return;
    }
    if (this->type >= OBJMURE_TYPE_MAX) {
        Actor_Kill(&this->actor);
        return;
    }
    if (!func_808D7928(this, play)) {
        Actor_Kill(&this->actor);
        return;
    }
    this->actionFunc = ObjMure_InitialAction;
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
    *outPos = *inPos;
}

void ObjMure_SpawnActors0(Actor* thisx, PlayState* play) {
    ObjMure* this = (ObjMure*)thisx;
    s32 i;
    Vec3f pos;
    s32 pad;
    s32 maxChildren = ObjMure_GetMaxChildSpawns(this);

    for (i = 0; i < maxChildren; i++) {
        switch (this->childrenStates[i]) {
            case OBJMURE_CHILD_STATE_DEAD:
                break;

            case OBJMURE_CHILD_STATE_2:
                ObjMure_GetSpawnPos(&pos, &this->actor.world.pos, this->ptn, i);
                this->children[i] = Actor_SpawnAsChildAndCutscene(
                    &play->actorCtx, play, sSpawnActorIds[this->type], pos.x, pos.y, pos.z, this->actor.world.rot.x,
                    this->actor.world.rot.y, this->actor.world.rot.z, sSpawnParams[this->type], this->actor.csId,
                    this->actor.halfDaysBits, NULL);
                if (this->children[i] != NULL) {
                    if (this->type == 0x90) {
                        ((ObjMureChild*)this->children[i])->unk_197 = 1;
                    }
                    this->children[i]->room = this->actor.room;
                }
                break;

            default:
                ObjMure_GetSpawnPos(&pos, &this->actor.world.pos, this->ptn, i);
                this->children[i] = Actor_SpawnAsChildAndCutscene(
                    &play->actorCtx, play, sSpawnActorIds[this->type], pos.x, pos.y, pos.z, this->actor.world.rot.x,
                    this->actor.world.rot.y, this->actor.world.rot.z, sSpawnParams[this->type], this->actor.csId,
                    this->actor.halfDaysBits, NULL);
                if (this->children[i] != NULL) {
                    this->children[i]->room = this->actor.room;
                }
                break;
        }
    }
}

void ObjMure_SpawnActors1(ObjMure* this, PlayState* play2) {
    PlayState* play = play2;
    Actor* actor = &this->actor;
    Vec3f spawnPos;
    s32 maxChildren = ObjMure_GetMaxChildSpawns(this);
    s32 i;

    for (i = 0; i < maxChildren; i++) {
        ObjMure_GetSpawnPos(&spawnPos, &actor->world.pos, this->ptn, i);
        this->children[i] = Actor_SpawnAsChildAndCutscene(
            &play2->actorCtx, play, sSpawnActorIds[this->type], spawnPos.x, spawnPos.y, spawnPos.z, actor->world.rot.x,
            actor->world.rot.y, actor->world.rot.z,
            (this->type == OBJMURE_TYPE_BUTTERFLY && i == 0) ? 1 : sSpawnParams[this->type], this->actor.csId,
            this->actor.halfDaysBits, NULL);
        if (this->children[i] != NULL) {
            this->childrenStates[i] = OBJMURE_CHILD_STATE_0;
            this->children[i]->room = actor->room;
        } else {
            this->childrenStates[i] = OBJMURE_CHILD_STATE_DEAD;
        }
    }
}

void ObjMure_SpawnActors(ObjMure* this, PlayState* play) {
    switch (this->svNum) {
        case 0:
            ObjMure_SpawnActors0(&this->actor, play);
            break;

        case 1:
            ObjMure_SpawnActors1(this, play);
            break;

        default:
            break;
    }
}

void ObjMure_KillActorsImpl(ObjMure* this, PlayState* play) {
    s32 maxChildren = ObjMure_GetMaxChildSpawns(this);
    s32 i;

    for (i = 0; i < maxChildren; i++) {
        switch (this->childrenStates[i]) {
            case OBJMURE_CHILD_STATE_DEAD:
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
                    if ((this->type == 0x90) && (((ObjMureChild*)this->children[i])->unk_197 != 0)) {
                        this->childrenStates[i] = OBJMURE_CHILD_STATE_2;
                    }
                } else {
                    this->childrenStates[i] = OBJMURE_CHILD_STATE_DEAD;
                    this->children[i] = NULL;
                }
            } else if ((this->childrenStates[i] == OBJMURE_CHILD_STATE_2) && (this->children[i]->update == NULL)) {
                this->childrenStates[i] = OBJMURE_CHILD_STATE_DEAD;
                this->children[i] = NULL;
            }
        }
    }
}

void ObjMure_InitialAction(ObjMure* this, PlayState* play) {
    this->actionFunc = ObjMure_CulledState;
}

void ObjMure_CulledState(ObjMure* this, PlayState* play) {
    Ship_ExtendedCullingActorAdjustProjectedZ(&this->actor);

    if (fabsf(this->actor.projectedPos.z) < sZClip[this->type]) {
        this->actionFunc = ObjMure_ActiveState;
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        ObjMure_SpawnActors(this, play);
    }

    Ship_ExtendedCullingActorRestoreProjectedPos(play, &this->actor);
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
                    if ((i != j) && (this->children[j]->child == this->children[i])) {
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
    if (this->unk_19C <= 0) {
        if (this->unk_19E) {
            this->unk_19E = false;
            ObjMure_SetFollowTargets(this, (Rand_ZeroOne() * 0.5f) + 0.1f);
            if (this->actor.xzDistToPlayer < 60.0f) {
                this->unk_19C = (s32)(Rand_ZeroOne() * 5.5f) + 4;
            } else {
                this->unk_19C = (s32)(Rand_ZeroOne() * 40.5f) + 4;
            }
        } else {
            this->unk_19E = true;
            if (this->actor.xzDistToPlayer < 60.0f) {
                this->unk_19C = (s32)(Rand_ZeroOne() * 10.5f) + 4;
                ObjMure_SetFollowTargets(this, (Rand_ZeroOne() * 0.2f) + 0.8f);
            } else {
                this->unk_19C = (s32)(Rand_ZeroOne() * 10.5f) + 4;
                ObjMure_SetFollowTargets(this, (Rand_ZeroOne() * 0.2f) + 0.6f);
            }
        }
    }
    if (this->actor.xzDistToPlayer < 120.0f) {
        this->unk_1A0++;
    } else {
        this->unk_1A0 = 0;
    }
    if (this->unk_1A0 >= 80) {
        ObjMure_SetChildToFollowPlayer(this, 1);
    } else {
        ObjMure_SetChildToFollowPlayer(this, 0);
    }
}

// Butterflies
void ObjMure_GroupBehavior1(ObjMure* this, PlayState* play) {
    s32 maxChildren;
    s32 i;

    if (this->unk_19C <= 0) {
        if (this->unk_19E) {
            this->unk_19E = false;
            ObjMure_SetFollowTargets(this, Rand_ZeroOne() * 0.2f);
            if (this->actor.xzDistToPlayer < 60.0f) {
                this->unk_19C = (s32)(Rand_ZeroOne() * 5.5f) + 4;
            } else {
                this->unk_19C = (s32)(Rand_ZeroOne() * 40.5f) + 4;
            }
        } else {
            this->unk_19E = true;
            ObjMure_SetFollowTargets(this, Rand_ZeroOne() * 0.7f);
            this->unk_19C = (s32)(Rand_ZeroOne() * 10.5f) + 4;
        }
    }

    maxChildren = ObjMure_GetMaxChildSpawns(this);
    for (i = 0; i < maxChildren; i++) {
        if (this->children[i] != NULL) {
            if ((this->children[i]->child != NULL) && (this->children[i]->child->update == NULL)) {
                this->children[i]->child = NULL;
            }
        }
    }
}

static ObjMureActionFunc sTypeGroupBehaviorFunc[] = {
    NULL, NULL, ObjMure_GroupBehavior0, ObjMure_GroupBehavior0, ObjMure_GroupBehavior1,
};

void ObjMure_ActiveState(ObjMure* this, PlayState* play) {
    Ship_ExtendedCullingActorAdjustProjectedZ(&this->actor);

    ObjMure_CheckChildren(this, play);
    if ((sZClip[this->type] + 40.0f) <= fabsf(this->actor.projectedPos.z)) {
        this->actionFunc = ObjMure_CulledState;
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        ObjMure_KillActors(this, play);
    } else if (sTypeGroupBehaviorFunc[this->type] != NULL) {
        sTypeGroupBehaviorFunc[this->type](this, play);
    }

    Ship_ExtendedCullingActorRestoreProjectedPos(play, &this->actor);
}

void ObjMure_Update(Actor* thisx, PlayState* play) {
    ObjMure* this = (ObjMure*)thisx;

    if (this->unk_19C > 0) {
        this->unk_19C--;
    }
    this->actionFunc(this, play);
}
