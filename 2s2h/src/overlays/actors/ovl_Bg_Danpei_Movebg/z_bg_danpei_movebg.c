/*
 * File: z_bg_danpei_movebg.c
 * Overlay: ovl_Bg_Danpei_Movebg
 * Description: Moving platforms beneath Dampe's house
 */

#include "z_bg_danpei_movebg.h"
#include "objects/object_d_lift/object_d_lift.h"

#include "objects/object_d_lift/object_d_lift.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

#define DANPEI_MOVEBG_FLAG_4 (1 << 2)
#define DANPEI_MOVEBG_FLAG_8 (1 << 3)
#define DANPEI_MOVEBG_FLAG_10 (1 << 4)

void BgDanpeiMovebg_Init(Actor* thisx, PlayState* play);
void BgDanpeiMovebg_Destroy(Actor* thisx, PlayState* play);
void BgDanpeiMovebg_Update(Actor* thisx, PlayState* play);

void func_80AF6EA8(BgDanpeiMovebg* this, PlayState* play);
void func_80AF70FC(BgDanpeiMovebg* this, PlayState* play);
void func_80AF74CC(Actor* thisx, PlayState* play);
u16 func_80AF705C(BgDanpeiMovebg* this, u16 flags);
void func_80AF71FC(BgDanpeiMovebg* this, PlayState* play);
void func_80AF72F8(BgDanpeiMovebg* this, PlayState* play);
void func_80AF7354(BgDanpeiMovebg* this, PlayState* play);
void func_80AF746C(BgDanpeiMovebg* this, PlayState* play);

ActorProfile Bg_Danpei_Movebg_Profile = {
    /**/ ACTOR_BG_DANPEI_MOVEBG,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(BgDanpeiMovebg),
    /**/ BgDanpeiMovebg_Init,
    /**/ BgDanpeiMovebg_Destroy,
    /**/ BgDanpeiMovebg_Update,
    /**/ NULL,
};

static u16 D_80AF7530[] = { OBJECT_D_LIFT, OBJECT_DANPEI_OBJECT };

static Gfx* D_80AF7534[] = { gDampeGraveBrownElevatorDL };

static CollisionHeader* D_80AF7538[] = { &gDampeGraveBrownElevatorCol };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeScale, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

s32 func_80AF6DE0(PlayState* this, ActorPathing* actorPathing) {
    Actor* actor = actorPathing->actor;
    s32 ret = false;

    if (actorPathing->distSqToCurPoint < SQ(actor->speed)) {
        ret = true;
    } else {
        actor->world.rot.x = actorPathing->rotToCurPoint.x;
        actor->world.rot.y = actorPathing->rotToCurPoint.y;
    }
    return ret;
}

void BgDanpeiMovebg_Init(Actor* thisx, PlayState* play) {
    BgDanpeiMovebg* this = (BgDanpeiMovebg*)thisx;

    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);

    this->objectSlot = SubS_GetObjectSlot(D_80AF7530[BGDANPEIMOVEBG_GET_TYPE(thisx)], play);
    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->dyna.actor);
    }

    this->actionFunc = func_80AF6EA8;
}

void func_80AF6EA8(BgDanpeiMovebg* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    if (SubS_IsObjectLoaded(this->objectSlot, play)) {
        gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->objectSlot].segment);
        this->dyna.actor.objectSlot = this->objectSlot;
        this->dyna.actor.draw = func_80AF74CC;
        Actor_ProcessInitChain(thisx, sInitChain);
        DynaPolyActor_LoadMesh(play, &this->dyna, D_80AF7538[BGDANPEIMOVEBG_GET_TYPE(thisx)]);
        this->dList = D_80AF7534[BGDANPEIMOVEBG_GET_TYPE(thisx)];

        /* If D_LIFT */
        if (BGDANPEIMOVEBG_GET_TYPE(thisx) == 0) {
            this->dyna.actor.gravity = 0.0f;
            this->dyna.actor.speed = 1.0f;
            SubS_ActorPathing_Init(play, &thisx->world.pos, thisx, &this->actorPath, play->setupPathList,
                                   BGDANPEIMOVEBG_GET_PATH_INDEX(thisx), 0, 0, 0, 0);
            this->actionFunc = func_80AF70FC;
        }
    }
}

void BgDanpeiMovebg_Destroy(Actor* thisx, PlayState* play) {
    BgDanpeiMovebg* this = (BgDanpeiMovebg*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgDanpeiMovebg_Update(Actor* thisx, PlayState* play) {
    BgDanpeiMovebg* this = (BgDanpeiMovebg*)thisx;

    this->actionFunc(this, play);
    this->prevFlags = this->flags;
    this->flags &= ~DANPEI_MOVEBG_FLAG_1;
}

u16 func_80AF705C(BgDanpeiMovebg* this, u16 flags) {
    u16 newFlags = flags & ~(DANPEI_MOVEBG_FLAG_4 | DANPEI_MOVEBG_FLAG_8 | DANPEI_MOVEBG_FLAG_10);

    if (DynaPolyActor_IsActorOnTop(&this->dyna) && (this->flags & DANPEI_MOVEBG_FLAG_1)) {
        newFlags |= DANPEI_MOVEBG_FLAG_4;
    }
    if (DynaPolyActor_IsPlayerOnTop(&this->dyna)) {
        if (gSaveContext.save.playerForm == PLAYER_FORM_DEKU) {
            newFlags |= DANPEI_MOVEBG_FLAG_8;
        } else {
            newFlags |= DANPEI_MOVEBG_FLAG_10;
        }
    }
    return newFlags;
}

void func_80AF70FC(BgDanpeiMovebg* this, PlayState* play) {
    this->flags = func_80AF705C(this, this->flags);
    if (this->flags & (DANPEI_MOVEBG_FLAG_4 | DANPEI_MOVEBG_FLAG_8)) {
        if (this->flags & DANPEI_MOVEBG_FLAG_4) {
            this->flags |= DANPEI_MOVEBG_FLAG_2;
        }
        this->actionFunc = func_80AF71FC;
    } else if (this->flags & DANPEI_MOVEBG_FLAG_10) {
        if (!(this->prevFlags & DANPEI_MOVEBG_FLAG_10)) {
            Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_SINK_WOOD_FLOOR);
        }
        Math_SmoothStepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y - 8.0f, 1.0f, 2.0f, 0.01f);
    } else {
        if (this->prevFlags & DANPEI_MOVEBG_FLAG_10) {
            Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_REBOUND_WOOD_FLOOR);
        }
        Math_SmoothStepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 1.0f, 2.0f, 0.01f);
    }
}

void func_80AF71FC(BgDanpeiMovebg* this, PlayState* play) {
    SubS_ActorPathing_Update(play, &this->actorPath, SubS_ActorPathing_ComputePointInfo, func_80AF6DE0,
                             SubS_ActorPathing_MoveWithoutGravityReverse, SubS_ActorPathing_SetNextPoint);
    Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_PLATE_LIFT_LEVEL - SFX_FLAG);

    if (this->actorPath.flags & ACTOR_PATHING_REACHED_END_TEMPORARY) {
        if (this->flags & DANPEI_MOVEBG_FLAG_2) {
            this->flags &= ~DANPEI_MOVEBG_FLAG_2;
        }
        this->actionFunc = func_80AF72F8;
    } else {
        this->flags = func_80AF705C(this, this->flags);
        if ((this->flags & DANPEI_MOVEBG_FLAG_10) ||
            CHECK_FLAG_ALL(this->flags, DANPEI_MOVEBG_FLAG_4 | DANPEI_MOVEBG_FLAG_8)) {
            this->actionFunc = func_80AF746C;
        } else if (!(this->flags & (DANPEI_MOVEBG_FLAG_4 | DANPEI_MOVEBG_FLAG_8 | DANPEI_MOVEBG_FLAG_10))) {
            this->actorPath.flags |= ACTOR_PATHING_MOVE_BACKWARDS;
            this->dyna.actor.speed = 2.0f;
            this->actionFunc = func_80AF7354;
        }
    }
}

void func_80AF72F8(BgDanpeiMovebg* this, PlayState* play) {
    this->flags = func_80AF705C(this, this->flags);
    if (!(this->flags & (DANPEI_MOVEBG_FLAG_4 | DANPEI_MOVEBG_FLAG_8 | DANPEI_MOVEBG_FLAG_10))) {
        this->actorPath.flags |= ACTOR_PATHING_MOVE_BACKWARDS;
        this->dyna.actor.speed = 2.0f;
        this->actionFunc = func_80AF7354;
    }
}

void func_80AF7354(BgDanpeiMovebg* this, PlayState* play) {
    Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_PLATE_LIFT_LEVEL - SFX_FLAG);
    this->flags = func_80AF705C(this, this->flags);
    SubS_ActorPathing_Update(play, &this->actorPath, SubS_ActorPathing_ComputePointInfo, func_80AF6DE0,
                             SubS_ActorPathing_MoveWithoutGravityReverse, SubS_ActorPathing_SetNextPoint);

    if ((this->flags & DANPEI_MOVEBG_FLAG_10) ||
        CHECK_FLAG_ALL(this->flags, DANPEI_MOVEBG_FLAG_4 | DANPEI_MOVEBG_FLAG_8)) {
        this->actionFunc = func_80AF746C;
    } else if ((this->flags & (DANPEI_MOVEBG_FLAG_4 | DANPEI_MOVEBG_FLAG_8)) &&
               !CHECK_FLAG_ALL(this->flags, DANPEI_MOVEBG_FLAG_4 | DANPEI_MOVEBG_FLAG_8)) {
        this->actorPath.flags &= ~ACTOR_PATHING_MOVE_BACKWARDS;
        this->dyna.actor.speed = 1.0f;
        if (this->flags & DANPEI_MOVEBG_FLAG_4) {
            this->flags |= DANPEI_MOVEBG_FLAG_2;
        }
        this->actionFunc = func_80AF71FC;
    }

    if (this->actorPath.flags & ACTOR_PATHING_REACHED_END_TEMPORARY) {
        this->actorPath.flags &= ~ACTOR_PATHING_MOVE_BACKWARDS;
        this->flags &= ~DANPEI_MOVEBG_FLAG_2;
        this->dyna.actor.speed = 1.0f;
        this->actionFunc = func_80AF70FC;
    }
}

void func_80AF746C(BgDanpeiMovebg* this, PlayState* play) {
    this->flags = func_80AF705C(this, this->flags);
    if (!(this->flags & (DANPEI_MOVEBG_FLAG_8 | DANPEI_MOVEBG_FLAG_10))) {
        if (this->actorPath.flags & ACTOR_PATHING_MOVE_BACKWARDS) {
            this->actionFunc = func_80AF7354;
        } else {
            this->actionFunc = func_80AF71FC;
        }
    }
}

void func_80AF74CC(Actor* thisx, PlayState* play) {
    BgDanpeiMovebg* this = (BgDanpeiMovebg*)thisx;

    if (this->dList != NULL) {
        Gfx_DrawDListOpa(play, this->dList);
    }
}
