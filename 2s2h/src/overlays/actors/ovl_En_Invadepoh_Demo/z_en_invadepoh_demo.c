/*
 * File: z_en_invadepoh_demo.c
 * Overlay: ovl_En_Invadepoh_Demo
 * Description: Aliens cutscene actors
 *
 * This actor is responsible for handling the characters that appear in two cutscenes:
 * 1. The cutscene where Romani introduces the aliens to the player.
 * 2. The cutscene where the aliens abduct the cows and Romani from the barn.
 *
 * To be more specific, this actor has five variations:
 * 1. An alien
 * 2. Romani
 * 3. A cow (minus the tail)
 * 4. The UFO (which appears as a spinning ball of light)
 * 5. A cow tail
 */
#include "prevent_bss_reordering.h"
#include "z_en_invadepoh_demo.h"
#include "sys_cfb.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnInvadepohDemo_Init(Actor* thisx, PlayState* play);
void EnInvadepohDemo_Destroy(Actor* thisx, PlayState* play);
void EnInvadepohDemo_Update(Actor* thisx, PlayState* play);
void EnInvadepohDemo_Draw(Actor* thisx, PlayState* play);

void EnInvadepohDemo_DoNothing(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Alien_Init(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Romani_Init(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Cow_Init(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Ufo_Init(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_CowTail_Init(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Cow_Destroy(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_CowTail_Destroy(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Alien_Idle(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Alien_FollowPath(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Romani_Idle(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Romani_FollowPath(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Cow_Idle(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Cow_FollowPath(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Ufo_Idle(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Ufo_FollowPath(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_SelectCueAction(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_CowTail_Idle(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Alien_WaitForObject(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Romani_WaitForObject(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Cow_WaitForObject(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_CowTail_WaitForObject(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Alien_Draw(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Romani_Draw(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Cow_Draw(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_Ufo_Draw(EnInvadepohDemo* this, PlayState* play);
void EnInvadepohDemo_CowTail_Draw(EnInvadepohDemo* this, PlayState* play);

#define DRAW_FLAG_SHOULD_DRAW 1
#define EN_INVADEPOH_DEMO_CUEID_NONE -1

typedef enum {
    /* 0 */ EN_INVADEPOH_DEMO_ALIEN_CUEID_DO_NOTHING,
    /* 1 */ EN_INVADEPOH_DEMO_ALIEN_CUEID_IDLE,
    /* 2 */ EN_INVADEPOH_DEMO_ALIEN_CUEID_FOLLOW_PATH_1,
    /* 3 */ EN_INVADEPOH_DEMO_ALIEN_CUEID_FOLLOW_PATH_2,
    /* 4 */ EN_INVADEPOH_DEMO_ALIEN_CUEID_FOLLOW_PATH_3,
    /* 5 */ EN_INVADEPOH_DEMO_ALIEN_CUEID_FOLLOW_PATH_4,
    /* 6 */ EN_INVADEPOH_DEMO_ALIEN_CUEID_FOLLOW_PATH_5, // Doesn't seem to be actually used in the final game
    /* 7 */ EN_INVADEPOH_DEMO_ALIEN_CUEID_MAX
} EnInvadepohDemoAlienCueId;

typedef enum {
    /* 0 */ EN_INVADEPOH_DEMO_ROMANI_CUEID_DO_NOTHING,
    /* 1 */ EN_INVADEPOH_DEMO_ROMANI_CUEID_IDLE,
    /* 2 */ EN_INVADEPOH_DEMO_ROMANI_CUEID_FOLLOW_PATH,
    /* 3 */ EN_INVADEPOH_DEMO_ROMANI_CUEID_MAX
} EnInvadepohDemoRomaniCueId;

typedef enum {
    /* 0 */ EN_INVADEPOH_DEMO_COW_CUEID_DO_NOTHING,
    /* 1 */ EN_INVADEPOH_DEMO_COW_CUEID_IDLE,
    /* 2 */ EN_INVADEPOH_DEMO_COW_CUEID_FOLLOW_PATH_1,
    /* 3 */ EN_INVADEPOH_DEMO_COW_CUEID_FOLLOW_PATH_2,
    /* 4 */ EN_INVADEPOH_DEMO_COW_CUEID_FOLLOW_PATH_3,
    /* 5 */ EN_INVADEPOH_DEMO_COW_CUEID_MAX
} EnInvadepohDemoCowCueId;

typedef enum {
    /* 0 */ EN_INVADEPOH_DEMO_UFO_CUEID_DO_NOTHING,
    /* 1 */ EN_INVADEPOH_DEMO_UFO_CUEID_IDLE_1,
    /* 2 */ EN_INVADEPOH_DEMO_UFO_CUEID_FOLLOW_PATH,
    /* 3 */ EN_INVADEPOH_DEMO_UFO_CUEID_IDLE_2, // Doesn't seem to be actually used in the final game
    /* 4 */ EN_INVADEPOH_DEMO_UFO_CUEID_MAX
} EnInvadepohDemoUfoCueId;

ActorProfile En_Invadepoh_Demo_Profile = {
    /**/ ACTOR_EN_INVADEPOH_DEMO,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnInvadepohDemo),
    /**/ EnInvadepohDemo_Init,
    /**/ EnInvadepohDemo_Destroy,
    /**/ EnInvadepohDemo_Update,
    /**/ EnInvadepohDemo_Draw,
};

static s32 sCueTypes[EN_INVADEPOH_DEMO_TYPE_MAX] = {
    CS_CMD_ACTOR_CUE_553, // EN_INVADEPOH_DEMO_TYPE_ALIEN
    CS_CMD_ACTOR_CUE_554, // EN_INVADEPOH_DEMO_TYPE_ROMANI
    CS_CMD_ACTOR_CUE_563, // EN_INVADEPOH_DEMO_TYPE_COW
    CS_CMD_ACTOR_CUE_555, // EN_INVADEPOH_DEMO_TYPE_UFO
    CS_CAM_STOP,          // EN_INVADEPOH_DEMO_TYPE_COW_TAIL
};

static InitChainEntry sAlienInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 20000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 600, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 6000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_STOP),
};

static InitChainEntry sRomaniInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 20000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 1500, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_6, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_STOP),
};

static InitChainEntry sCowInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 20000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 300, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_STOP),
};

static InitChainEntry sUfoInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 20000, ICHAIN_CONTINUE), ICHAIN_F32(cullingVolumeScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_CONTINUE),  ICHAIN_VEC3S(shape.rot, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(terminalVelocity, -100, ICHAIN_CONTINUE),       ICHAIN_VEC3F_DIV1000(scale, 800, ICHAIN_STOP),
};

static InitChainEntry sCowTailInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 20000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 100, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_STOP),
};

static EnInvadepohDemoFunc sInitFuncs[EN_INVADEPOH_DEMO_TYPE_MAX] = {
    EnInvadepohDemo_Alien_Init,   // EN_INVADEPOH_DEMO_TYPE_ALIEN
    EnInvadepohDemo_Romani_Init,  // EN_INVADEPOH_DEMO_TYPE_ROMANI
    EnInvadepohDemo_Cow_Init,     // EN_INVADEPOH_DEMO_TYPE_COW
    EnInvadepohDemo_Ufo_Init,     // EN_INVADEPOH_DEMO_TYPE_UFO
    EnInvadepohDemo_CowTail_Init, // EN_INVADEPOH_DEMO_TYPE_COW_TAIL
};

static EnInvadepohDemoFunc sDestroyFuncs[EN_INVADEPOH_DEMO_TYPE_MAX] = {
    EnInvadepohDemo_DoNothing,       // EN_INVADEPOH_DEMO_TYPE_ALIEN
    EnInvadepohDemo_DoNothing,       // EN_INVADEPOH_DEMO_TYPE_ROMANI
    EnInvadepohDemo_Cow_Destroy,     // EN_INVADEPOH_DEMO_TYPE_COW
    EnInvadepohDemo_DoNothing,       // EN_INVADEPOH_DEMO_TYPE_UFO
    EnInvadepohDemo_CowTail_Destroy, // EN_INVADEPOH_DEMO_TYPE_COW_TAIL
};

static EnInvadepohDemoFunc sAlienCueActionCsFuncs[EN_INVADEPOH_DEMO_ALIEN_CUEID_MAX] = {
    EnInvadepohDemo_DoNothing,        // EN_INVADEPOH_DEMO_ALIEN_CUEID_DO_NOTHING
    EnInvadepohDemo_Alien_Idle,       // EN_INVADEPOH_DEMO_ALIEN_CUEID_IDLE
    EnInvadepohDemo_Alien_FollowPath, // EN_INVADEPOH_DEMO_ALIEN_CUEID_FOLLOW_PATH_1
    EnInvadepohDemo_Alien_FollowPath, // EN_INVADEPOH_DEMO_ALIEN_CUEID_FOLLOW_PATH_2
    EnInvadepohDemo_Alien_FollowPath, // EN_INVADEPOH_DEMO_ALIEN_CUEID_FOLLOW_PATH_3
    EnInvadepohDemo_Alien_FollowPath, // EN_INVADEPOH_DEMO_ALIEN_CUEID_FOLLOW_PATH_4
    EnInvadepohDemo_Alien_FollowPath, // EN_INVADEPOH_DEMO_ALIEN_CUEID_FOLLOW_PATH_5
};

static EnInvadepohDemoFunc sRomaniCueActionCsFuncs[EN_INVADEPOH_DEMO_ROMANI_CUEID_MAX] = {
    EnInvadepohDemo_DoNothing,         // EN_INVADEPOH_DEMO_ROMANI_CUEID_DO_NOTHING
    EnInvadepohDemo_Romani_Idle,       // EN_INVADEPOH_DEMO_ROMANI_CUEID_IDLE
    EnInvadepohDemo_Romani_FollowPath, // EN_INVADEPOH_DEMO_ROMANI_CUEID_FOLLOW_PATH
};

static EnInvadepohDemoFunc sCowCueActionCsFuncs[EN_INVADEPOH_DEMO_COW_CUEID_MAX] = {
    EnInvadepohDemo_DoNothing,      // EN_INVADEPOH_DEMO_COW_CUEID_DO_NOTHING
    EnInvadepohDemo_Cow_Idle,       // EN_INVADEPOH_DEMO_COW_CUEID_IDLE
    EnInvadepohDemo_Cow_FollowPath, // EN_INVADEPOH_DEMO_COW_CUEID_FOLLOW_PATH_1
    EnInvadepohDemo_Cow_FollowPath, // EN_INVADEPOH_DEMO_COW_CUEID_FOLLOW_PATH_2
    EnInvadepohDemo_Cow_FollowPath, // EN_INVADEPOH_DEMO_COW_CUEID_FOLLOW_PATH_3
};

static EnInvadepohDemoFunc sUfoCueActionCsFuncs[EN_INVADEPOH_DEMO_UFO_CUEID_MAX] = {
    EnInvadepohDemo_DoNothing,      // EN_INVADEPOH_DEMO_UFO_CUEID_DO_NOTHING
    EnInvadepohDemo_Ufo_Idle,       // EN_INVADEPOH_DEMO_UFO_CUEID_IDLE_1
    EnInvadepohDemo_Ufo_FollowPath, // EN_INVADEPOH_DEMO_UFO_CUEID_FOLLOW_PATH
    EnInvadepohDemo_Ufo_Idle,       // EN_INVADEPOH_DEMO_UFO_CUEID_IDLE_2
};

static EnInvadepohDemoFunc sDrawFuncs[EN_INVADEPOH_DEMO_TYPE_MAX] = {
    EnInvadepohDemo_Alien_Draw,   // EN_INVADEPOH_DEMO_TYPE_ALIEN
    EnInvadepohDemo_Romani_Draw,  // EN_INVADEPOH_DEMO_TYPE_ROMANI
    EnInvadepohDemo_Cow_Draw,     // EN_INVADEPOH_DEMO_TYPE_COW
    EnInvadepohDemo_Ufo_Draw,     // EN_INVADEPOH_DEMO_TYPE_UFO
    EnInvadepohDemo_CowTail_Draw, // EN_INVADEPOH_DEMO_TYPE_COW_TAIL
};

MtxF sInvadepohDemoAlienLeftEyeBeamMtxF;
MtxF sInvadepohDemoAlienRightEyeBeamMtxF;

void EnInvadepohDemo_DoNothing(EnInvadepohDemo* this, PlayState* play) {
}

void EnInvadepohDemo_Alien_Init(EnInvadepohDemo* this, PlayState* play) {
    Actor_ProcessInitChain(&this->actor, sAlienInitChain);
    this->actor.flags = ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_IGNORE_QUAKE | ACTOR_FLAG_MINIMAP_ICON_ENABLED;
    this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_UCH);
    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actionFunc = EnInvadepohDemo_Alien_WaitForObject;
}

void EnInvadepohDemo_Romani_Init(EnInvadepohDemo* this, PlayState* play) {
    Actor_ProcessInitChain(&this->actor, sRomaniInitChain);
    this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_MA1);
    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actionFunc = EnInvadepohDemo_Romani_WaitForObject;
}

void EnInvadepohDemo_Cow_Init(EnInvadepohDemo* this, PlayState* play) {
    Actor_ProcessInitChain(&this->actor, sCowInitChain);
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_INVADEPOH_DEMO, 0.0f, 0.0f, 0.0f, 0, 0, 0,
                       EN_INVADEPOH_DEMO_TYPE_COW_TAIL);
    this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_COW);
    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actionFunc = EnInvadepohDemo_Cow_WaitForObject;
}

void EnInvadepohDemo_Ufo_Init(EnInvadepohDemo* this, PlayState* play) {
    Actor_ProcessInitChain(&this->actor, sUfoInitChain);
    this->actionFunc = EnInvadepohDemo_SelectCueAction;
    this->drawFlags |= DRAW_FLAG_SHOULD_DRAW;
}

void EnInvadepohDemo_CowTail_Init(EnInvadepohDemo* this, PlayState* play) {
    Actor_ProcessInitChain(&this->actor, sCowTailInitChain);
    this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_COW);
    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actionFunc = EnInvadepohDemo_CowTail_WaitForObject;
}

void EnInvadepohDemo_Cow_Destroy(EnInvadepohDemo* this, PlayState* play) {
    Actor* thisx = &this->actor;

    if (thisx->child != NULL) {
        thisx->child->parent = NULL;
    }

    Actor_Kill(thisx->child);
}

void EnInvadepohDemo_CowTail_Destroy(EnInvadepohDemo* this, PlayState* play) {
    if (this->actor.parent != NULL) {
        this->actor.parent->child = NULL;
    }
}

void EnInvadepohDemo_Alien_Idle(EnInvadepohDemo* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
}

void EnInvadepohDemo_Alien_FollowPath(EnInvadepohDemo* this, PlayState* play) {
    Path* path = &play->setupPathList[this->pathIndex];
    s32 pathCount = path->count;
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    Vec3f point;

    if (pathCount != 0) {
        Math_Vec3s_ToVec3f(&point, &points[this->pointIndex]);
        if (Math_Vec3f_StepTo(&this->actor.world.pos, &point, this->speed) < this->speed) {
            this->pointIndex++;
            if (this->pointIndex >= pathCount) {
                Actor_Kill(&this->actor);
            }
        } else {
            SkelAnime_Update(&this->skelAnime);
        }
    }
}

void EnInvadepohDemo_Romani_Idle(EnInvadepohDemo* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
}

void EnInvadepohDemo_Romani_FollowPath(EnInvadepohDemo* this, PlayState* play) {
    Path* path = &play->setupPathList[this->pathIndex];
    s32 pathCount = path->count;
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    Vec3f point;

    if (pathCount != 0) {
        Math_Vec3s_ToVec3f(&point, &points[this->pointIndex]);
        if (Math_Vec3f_StepTo(&this->actor.world.pos, &point, this->speed) < this->speed) {
            this->pointIndex++;
            if (this->pointIndex >= pathCount) {
                Actor_Kill(&this->actor);
            }
        } else {
            SkelAnime_Update(&this->skelAnime);
        }
    }
}

/**
 * Positions the cow tail actor at the appropriate spot on the cow's body and rotates it to
 * match the cow's rotation. This function is also responsible for playing the cow's animation.
 */
void EnInvadepohDemo_Cow_UpdateCommon(EnInvadepohDemo* this, PlayState* play) {
    s32 pad;
    MtxF mtx;

    if (this->actor.child != NULL) {
        Matrix_Push();
        Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                     &this->actor.shape.rot);
        Matrix_Translate(0.0f, 57.0f, -36.0f, MTXMODE_APPLY);
        Matrix_RotateXS(this->actor.shape.rot.x * -0.7f, MTXMODE_APPLY);
        Matrix_RotateZS(this->actor.shape.rot.z * -0.7f, MTXMODE_APPLY);
        Matrix_MultZero(&this->actor.child->world.pos);
        Matrix_Get(&mtx);
        Matrix_MtxFToYXZRot(&mtx, &this->actor.child->shape.rot, false);
        Matrix_Pop();
    }

    SkelAnime_Update(&this->skelAnime);
}

void EnInvadepohDemo_Cow_Idle(EnInvadepohDemo* this, PlayState* play) {
    EnInvadepohDemo_Cow_UpdateCommon(this, play);
}

void EnInvadepohDemo_Cow_FollowPath(EnInvadepohDemo* this, PlayState* play) {
    Path* path = &play->setupPathList[this->pathIndex];
    s32 pathCount = path->count;
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    Vec3f point;

    if (pathCount != 0) {
        if (((this->cueId == EN_INVADEPOH_DEMO_COW_CUEID_FOLLOW_PATH_1) && (play->csCtx.curFrame == 343)) ||
            ((this->cueId == EN_INVADEPOH_DEMO_COW_CUEID_FOLLOW_PATH_2) && (play->csCtx.curFrame == 421)) ||
            ((this->cueId == EN_INVADEPOH_DEMO_COW_CUEID_FOLLOW_PATH_3) && (play->csCtx.curFrame == 521))) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_COW_CRY);
        }

        Math_Vec3s_ToVec3f(&point, &points[this->pointIndex]);
        if (Math_Vec3f_StepTo(&this->actor.world.pos, &point, this->speed) < this->speed) {
            this->pointIndex++;
            if (this->pointIndex >= pathCount) {
                Actor_Kill(&this->actor);
            }
        } else {
            EnInvadepohDemo_Cow_UpdateCommon(this, play);
        }
    }
}

void EnInvadepohDemo_Ufo_Idle(EnInvadepohDemo* this, PlayState* play) {
    this->ufoRotZ += 0x258;
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_UFO_FLY - SFX_FLAG);
}

void EnInvadepohDemo_Ufo_FollowPath(EnInvadepohDemo* this, PlayState* play) {
    Path* path = &play->setupPathList[this->pathIndex];
    s32 pathCount = path->count;
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    Vec3f point;

    if (pathCount != 0) {
        this->ufoRotZ += 0x258;
        Math_Vec3s_ToVec3f(&point, &points[this->pointIndex]);
        if (Math_Vec3f_StepTo(&this->actor.world.pos, &point, this->speed) < this->speed) {
            this->pointIndex++;
            if (this->pointIndex >= pathCount) {
                Actor_Kill(&this->actor);
            }
        }
    }
}

/**
 * Updates the actor's cue ID if certain conditions are met and calls a function based
 * on their current cue ID. Specifically, cows and aliens offset the cue ID from their
 * cue channel (which applies to *all* aliens/cows, not just this specific actor), then
 * they compare this offset value to a value read from their params. If these two values
 * match, the actor's cue ID is updated.
 *
 * The distinction between a "global" cue ID from the cue channel and a "local" cue ID
 * specific to an instance of this actor is used to prevent all the cows or aliens from
 * moving along the path at once. The cow or alien waits in a stationary "idle" state
 * until the "global" cue ID changes to a specific value; if the cue ID offset matches
 * what this actor specifies in its params, then that acts like a signal to tell the
 * actor to update the "local" cue ID and start moving. The other cows or aliens in
 * the scene should have different params, meaning the cue ID offset will not match;
 * they will thus ignore this "global" cue ID change and remain idle.
 */
void EnInvadepohDemo_SelectCueAction(EnInvadepohDemo* this, PlayState* play) {
    CsCmdActorCue* cue;
    s32 cueIdOffset;

    if (Cutscene_IsCueInChannel(play, sCueTypes[this->type])) {
        this->cueChannel = Cutscene_GetCueChannel(play, sCueTypes[this->type]);
        cue = play->csCtx.actorCues[this->cueChannel];

        switch (this->type) {
            case EN_INVADEPOH_DEMO_TYPE_UFO:
                if (this->cueId != cue->id) {
                    if (cue->id == EN_INVADEPOH_DEMO_UFO_CUEID_FOLLOW_PATH) {
                        Actor_PlaySfx(&this->actor, NA_SE_EV_UFO_DASH);
                    }

                    this->cueId = cue->id;
                }
                break;

            case EN_INVADEPOH_DEMO_TYPE_ROMANI:
                if (this->cueId != cue->id) {
                    this->cueId = cue->id;
                }
                break;

            case EN_INVADEPOH_DEMO_TYPE_ALIEN:
                if (cue->id > EN_INVADEPOH_DEMO_ALIEN_CUEID_IDLE) {
                    cueIdOffset = cue->id - EN_INVADEPOH_DEMO_ALIEN_CUEID_FOLLOW_PATH_1;
                    if (this->cueIdOffset != cueIdOffset) {
                        break;
                    }
                }

                if (this->cueId != cue->id) {
                    this->cueId = cue->id;
                }
                break;

            case EN_INVADEPOH_DEMO_TYPE_COW:
                if (cue->id > EN_INVADEPOH_DEMO_COW_CUEID_IDLE) {
                    cueIdOffset = cue->id - EN_INVADEPOH_DEMO_COW_CUEID_IDLE;
                    if (this->cueIdOffset != cueIdOffset) {
                        break;
                    }
                }

                if (this->cueId != cue->id) {
                    this->cueId = cue->id;
                }
                break;

            default:
                break;
        }

        switch (this->type) {
            case EN_INVADEPOH_DEMO_TYPE_UFO:
                sUfoCueActionCsFuncs[this->cueId](this, play);
                break;

            case EN_INVADEPOH_DEMO_TYPE_ALIEN:
                sAlienCueActionCsFuncs[this->cueId](this, play);
                break;

            case EN_INVADEPOH_DEMO_TYPE_ROMANI:
                sRomaniCueActionCsFuncs[this->cueId](this, play);
                break;

            case EN_INVADEPOH_DEMO_TYPE_COW:
                sCowCueActionCsFuncs[this->cueId](this, play);
                break;

            default:
                break;
        }
    }
}

void EnInvadepohDemo_CowTail_Idle(EnInvadepohDemo* this, PlayState* play) {
    if (this->actor.parent == NULL) {
        Actor_Kill(&this->actor);
        return;
    }

    SkelAnime_Update(&this->skelAnime);
}

void EnInvadepohDemo_Alien_WaitForObject(EnInvadepohDemo* this, PlayState* play) {
    s32 pad[2];

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        this->drawFlags |= DRAW_FLAG_SHOULD_DRAW;
        this->actionFunc = EnInvadepohDemo_SelectCueAction;
        SkelAnime_InitFlex(play, &this->skelAnime, &gAlienSkel, &gAlienFloatAnim, this->jointTable, this->morphTable,
                           ALIEN_LIMB_MAX);
        Animation_PlayLoop(&this->skelAnime, &gAlienFloatAnim);
    }
}

void EnInvadepohDemo_Romani_WaitForObject(EnInvadepohDemo* this, PlayState* play) {
    s32 pad[2];

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        this->drawFlags |= DRAW_FLAG_SHOULD_DRAW;
        this->actionFunc = EnInvadepohDemo_SelectCueAction;
        SkelAnime_InitFlex(play, &this->skelAnime, &gRomaniSkel, &gRomaniAbductedAnim, this->jointTable,
                           this->morphTable, ROMANI_LIMB_MAX);
        Animation_PlayLoop(&this->skelAnime, &gRomaniAbductedAnim);
    }
}

void EnInvadepohDemo_Cow_WaitForObject(EnInvadepohDemo* this, PlayState* play) {
    s32 pad[2];

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        this->drawFlags |= DRAW_FLAG_SHOULD_DRAW;
        this->actionFunc = EnInvadepohDemo_SelectCueAction;
        SkelAnime_InitFlex(play, &this->skelAnime, &gCowSkel, &gCowMooAnim, this->jointTable, this->morphTable,
                           COW_LIMB_MAX);
        Animation_PlayLoop(&this->skelAnime, &gCowMooAnim);
    }
}

void EnInvadepohDemo_CowTail_WaitForObject(EnInvadepohDemo* this, PlayState* play) {
    s32 pad[2];

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        this->drawFlags |= DRAW_FLAG_SHOULD_DRAW;
        this->actionFunc = EnInvadepohDemo_CowTail_Idle;
        SkelAnime_InitFlex(play, &this->skelAnime, &gCowTailSkel, &gCowTailSwishAnim, this->jointTable,
                           this->morphTable, COW_TAIL_LIMB_MAX);
        Animation_PlayLoop(&this->skelAnime, &gCowTailSwishAnim);
    }
}

void EnInvadepohDemo_Ufo_SetMatrixTranslation(Vec3f* translation) {
    MtxF* currentMatrix = Matrix_GetCurrent();

    currentMatrix->xw = translation->x;
    currentMatrix->yw = translation->y;
    currentMatrix->zw = translation->z;
}

s32 EnInvadepohDemo_Ufo_LensFlareCheck(PlayState* play, Vec3f* pos) {
    Vec3f projectedPos;
    f32 invW;

    Actor_GetProjectedPos(play, pos, &projectedPos, &invW);

    if ((projectedPos.z > 1.0f) && (fabsf(projectedPos.x * invW) < 1.0f) && (fabsf(projectedPos.y * invW) < 1.0f)) {
        f32 screenPosX = PROJECTED_TO_SCREEN_X(projectedPos, invW);
        f32 screenPosY = PROJECTED_TO_SCREEN_Y(projectedPos, invW);
        s32 wZ = (s32)(projectedPos.z * invW * ((G_MAXZ / 2) * 32)) + ((G_MAXZ / 2) * 32);

        if (wZ < SysCfb_GetZBufferInt(screenPosX, screenPosY)) {
            return true;
        }
    }

    return false;
}

void EnInvadepohDemo_Alien_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx,
                                        Gfx** gfx) {
    if (limbIndex == ALIEN_LIMB_LEFT_EYE) {
        Matrix_Push();
        Matrix_RotateZS(-0x53ED, MTXMODE_APPLY);
        Matrix_RotateYS(-0x3830, MTXMODE_APPLY);
        Matrix_Scale(1.0f, 1.0f, 1.5f, MTXMODE_APPLY);
        Matrix_Get(&sInvadepohDemoAlienLeftEyeBeamMtxF);
        Matrix_Pop();
    } else if (limbIndex == ALIEN_LIMB_RIGHT_EYE) {
        Matrix_Push();
        Matrix_RotateZS(-0x53ED, MTXMODE_APPLY);
        Matrix_RotateYS(-0x47D0, MTXMODE_APPLY);
        Matrix_Scale(1.0f, 1.0f, 1.5f, MTXMODE_APPLY);
        Matrix_Get(&sInvadepohDemoAlienRightEyeBeamMtxF);
        Matrix_Pop();
    }
}

void EnInvadepohDemo_Alien_Draw(EnInvadepohDemo* this, PlayState* play) {
    s32 pad[2];
    Mtx* mtx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Matrix_Push();
    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(&gAlienEmptyTexAnim));
    Scene_SetRenderModeXlu(play, 0, 1);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    POLY_OPA_DISP =
        SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                           EnInvadepohDemo_Alien_PostLimbDraw, &this->actor, POLY_OPA_DISP);

    // In EnInvadepoh, the eye beam alpha is controlled by an instance variable, and there is an if-block that checks
    // to see if this variable is not zero. In this actor, the eye beam alpha is hardcoded, but this block is still
    // necessary to prevent reordering; this is likely a result of copy-pasting from EnInvadepoh.
    if (true) {
        Gfx* gfx;

        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(&gAlienEyeBeamTexAnim));

        OPEN_DISPS(play->state.gfxCtx);

        gfx = POLY_XLU_DISP;
        gDPPipeSync(gfx++);
        gDPSetPrimColor(gfx++, 0, 0xFF, 240, 180, 100, 60);
        gDPSetEnvColor(gfx++, 255, 255, 255, 150);
        Matrix_Mult(&sInvadepohDemoAlienLeftEyeBeamMtxF, MTXMODE_NEW);

        mtx = Matrix_Finalize(play->state.gfxCtx);

        if (mtx != NULL) {
            gSPMatrix(gfx++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(gfx++, gAlienEyeBeamDL);
            Matrix_Mult(&sInvadepohDemoAlienRightEyeBeamMtxF, MTXMODE_NEW);

            mtx = Matrix_Finalize(play->state.gfxCtx);

            if (mtx != NULL) {
                gSPMatrix(gfx++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(gfx++, gAlienEyeBeamDL);
                POLY_XLU_DISP = gfx;
            }
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }

    CLOSE_DISPS(play->state.gfxCtx);

    // In EnInvadepoh, the code to draw the light flash checks an instance variable which is set to true if the actor
    // should draw. In this actor, an equivalent variable is checked in EnInvadepohDemo_Draw, so it isn't checked here.
    // However, this block is still necessary to prevent reordering; this is likely a result of copy-pasting from
    // EnInvadepoh. Similarly, this function requires a lot of extra stack space for unused variables; the names and
    // types of these variables are inferred from EnInvadepoh's version of this code.
    if (true) {
        Gfx* gfx;
        Vec3f glowOffset;
        Vec3f glowPos; // unused, inferred from EnInvadepoh
        s32 glowAlpha; // unused, inferred from EnInvadepoh

        OPEN_DISPS(play->state.gfxCtx);

        gfx = POLY_XLU_DISP;
        gfx = Gfx_SetupDL20_NoCD(gfx);

        gDPSetDither(gfx++, G_CD_NOISE);
        gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                          0);
        Matrix_Mult(&play->billboardMtxF, MTXMODE_NEW);
        Matrix_MultVecZ(90.0f, &glowOffset);
        Matrix_Translate(this->actor.world.pos.x + glowOffset.x, this->actor.world.pos.y + glowOffset.y + 25.0f,
                         this->actor.world.pos.z + glowOffset.z, MTXMODE_NEW);
        Matrix_Scale(0.15f, 0.15f, 0.15f, MTXMODE_APPLY);
        gSPDisplayList(gfx++, gameplay_keep_DL_029CB0);
        gDPSetPrimColor(gfx++, 0, 0, 240, 180, 100, 100);

        mtx = Matrix_Finalize(play->state.gfxCtx);

        if (mtx != NULL) {
            gSPMatrix(gfx++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(gfx++, gameplay_keep_DL_029CF0);
            POLY_XLU_DISP = gfx;
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }

    Matrix_Pop();
}

void EnInvadepohDemo_Romani_Draw(EnInvadepohDemo* this, PlayState* play) {
    s32 pad[2];

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(&gRomaniEyeSadTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(&gRomaniMouthHangingOpenTex));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnInvadepohDemo_Cow_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                         Actor* thisx) {
    if (limbIndex == COW_LIMB_NOSE_RING) {
        *dList = NULL;
    }

    return false;
}

void EnInvadepohDemo_Cow_Draw(EnInvadepohDemo* this, PlayState* play) {
    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnInvadepohDemo_Cow_OverrideLimbDraw, NULL, &this->actor);
}

void EnInvadepohDemo_Ufo_Draw(EnInvadepohDemo* this, PlayState* play) {
    s32 pad[2];
    Vec3f flashPos = gZeroVec3f;
    Mtx* mtx;

    flashPos.x = this->actor.world.pos.x;
    flashPos.y = this->actor.world.pos.y;
    flashPos.z = this->actor.world.pos.z;

    EnInvadepohDemo_Ufo_SetMatrixTranslation(&flashPos);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    Matrix_RotateZS(this->ufoRotZ, MTXMODE_APPLY);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    mtx = Matrix_Finalize(play->state.gfxCtx);
    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetPrimColor(POLY_XLU_DISP++, 0xFF, 0x80, 255, 255, 0, 180);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 50, 0, 0);
        gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);
    }

    CLOSE_DISPS(play->state.gfxCtx);

    if (EnInvadepohDemo_Ufo_LensFlareCheck(play, &flashPos)) {
        Environment_DrawLensFlare(play, &play->envCtx, &play->view, play->state.gfxCtx, flashPos, 20.0f, 9.0f, 0,
                                  false);
    }
}

void EnInvadepohDemo_CowTail_Draw(EnInvadepohDemo* this, PlayState* play) {
    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, &this->actor);
}

void EnInvadepohDemo_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnInvadepohDemo* this = (EnInvadepohDemo*)thisx;

    this->cueIdOffset = EN_INVADEPOH_DEMO_GET_CUEID_OFFSET(&this->actor);
    this->type = EN_INVADEPOH_DEMO_GET_TYPE(&this->actor);
    if ((this->type < EN_INVADEPOH_DEMO_TYPE_ALIEN) || (this->type > EN_INVADEPOH_DEMO_TYPE_COW_TAIL)) {
        Actor_Kill(&this->actor);
        return;
    }

    if (this->type == EN_INVADEPOH_DEMO_TYPE_UFO) {
        this->actor.world.rot.z = 0;
        this->speed = EN_INVADEPOH_DEMO_GET_SPEED(&this->actor);
        this->actor.shape.rot.z = this->actor.world.rot.z;
    } else {
        this->actor.world.rot.z = 0;
        this->speed = EN_INVADEPOH_DEMO_GET_SPEED(&this->actor) / 10.0f;
        this->actor.shape.rot.z = this->actor.world.rot.z;
    }

    this->drawFlags = 0;
    this->cueId = EN_INVADEPOH_DEMO_CUEID_NONE;
    this->ufoRotZ = 0;
    this->pathIndex = EN_INVADEPOH_DEMO_GET_PATH_INDEX(&this->actor);
    this->pointIndex = 0;
    this->objectSlot = OBJECT_SLOT_NONE;
    sInitFuncs[this->type](this, play);
}

void EnInvadepohDemo_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnInvadepohDemo* this = (EnInvadepohDemo*)thisx;

    sDestroyFuncs[this->type](this, play);
}

void EnInvadepohDemo_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnInvadepohDemo* this = (EnInvadepohDemo*)thisx;

    this->actionFunc(this, play);
}

void EnInvadepohDemo_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnInvadepohDemo* this = (EnInvadepohDemo*)thisx;

    if ((this->cueId != EN_INVADEPOH_DEMO_CUEID_NONE) && (this->drawFlags & DRAW_FLAG_SHOULD_DRAW)) {
        sDrawFuncs[this->type](this, play);
    }
}
